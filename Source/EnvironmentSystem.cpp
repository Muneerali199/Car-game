// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnvironmentSystem.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/PostProcessVolume.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEnvironmentSystem::AEnvironmentSystem()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    // Create root component
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Create particle systems
    RainParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RainParticles"));
    RainParticles->SetupAttachment(Root);
    RainParticles->SetAutoActivate(false);

    SnowParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnowParticles"));
    SnowParticles->SetupAttachment(Root);
    SnowParticles->SetAutoActivate(false);

    LightningEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LightningEffect"));
    LightningEffect->SetupAttachment(Root);
    LightningEffect->SetAutoActivate(false);
}

void AEnvironmentSystem::BeginPlay()
{
    Super::BeginPlay();

    // Find lighting actors if not set
    if (!DirectionalLight)
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundActors);
        if (FoundActors.Num() > 0)
        {
            DirectionalLight = Cast<ADirectionalLight>(FoundActors[0]);
        }
    }

    if (!SkyLight)
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkyLight::StaticClass(), FoundActors);
        if (FoundActors.Num() > 0)
        {
            SkyLight = Cast<ASkyLight>(FoundActors[0]);
        }
    }

    if (!HeightFog)
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExponentialHeightFog::StaticClass(), FoundActors);
        if (FoundActors.Num() > 0)
        {
            HeightFog = Cast<AExponentialHeightFog>(FoundActors[0]);
        }
    }

    if (!PostProcessVolume)
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);
        if (FoundActors.Num() > 0)
        {
            PostProcessVolume = Cast<APostProcessVolume>(FoundActors[0]);
        }
    }

    // Initialize weather
    CurrentWeatherSettings = GetWeatherPreset(CurrentWeather);
    TargetWeatherSettings = CurrentWeatherSettings;

    UpdateLighting();
    UpdateWeatherEffects(0.0f);
    UpdateFog();
    UpdatePostProcessing();

    UE_LOG(LogTemp, Log, TEXT("Environment System initialized"));
}

void AEnvironmentSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update day/night cycle
    if (bEnableDayNightCycle)
    {
        CurrentTime += (DeltaTime / 3600.0f) * TimeSpeed;
        if (CurrentTime >= 24.0f)
        {
            CurrentTime -= 24.0f;
        }
        UpdateLighting();
    }

    // Update dynamic weather
    if (bEnableDynamicWeather)
    {
        TimeSinceWeatherChange += DeltaTime;
        if (TimeSinceWeatherChange >= WeatherChangeInterval)
        {
            TriggerRandomWeatherChange();
            TimeSinceWeatherChange = 0.0f;
        }
    }

    // Update weather transitions
    if (bIsTransitioning)
    {
        WeatherTransitionProgress += DeltaTime / WeatherTransitionDuration;
        
        if (WeatherTransitionProgress >= 1.0f)
        {
            WeatherTransitionProgress = 1.0f;
            bIsTransitioning = false;
            CurrentWeatherSettings = TargetWeatherSettings;
        }
        else
        {
            // Lerp weather settings
            CurrentWeatherSettings.RainIntensity = FMath::Lerp(CurrentWeatherSettings.RainIntensity, TargetWeatherSettings.RainIntensity, WeatherTransitionProgress);
            CurrentWeatherSettings.FogDensity = FMath::Lerp(CurrentWeatherSettings.FogDensity, TargetWeatherSettings.FogDensity, WeatherTransitionProgress);
            CurrentWeatherSettings.WindSpeed = FMath::Lerp(CurrentWeatherSettings.WindSpeed, TargetWeatherSettings.WindSpeed, WeatherTransitionProgress);
            CurrentWeatherSettings.CloudCoverage = FMath::Lerp(CurrentWeatherSettings.CloudCoverage, TargetWeatherSettings.CloudCoverage, WeatherTransitionProgress);
            CurrentWeatherSettings.TrackWetness = FMath::Lerp(CurrentWeatherSettings.TrackWetness, TargetWeatherSettings.TrackWetness, WeatherTransitionProgress);
            CurrentWeatherSettings.TrackGripMultiplier = FMath::Lerp(CurrentWeatherSettings.TrackGripMultiplier, TargetWeatherSettings.TrackGripMultiplier, WeatherTransitionProgress);
            CurrentWeatherSettings.Visibility = FMath::Lerp(CurrentWeatherSettings.Visibility, TargetWeatherSettings.Visibility, WeatherTransitionProgress);
        }
    }

    // Update effects
    UpdateWeatherEffects(DeltaTime);
    UpdateFog();
    UpdatePostProcessing();
}

void AEnvironmentSystem::SetWeather(EWeatherType NewWeather, bool bInstant)
{
    if (NewWeather == CurrentWeather && !bInstant)
    {
        return;
    }

    PreviousWeather = CurrentWeather;
    CurrentWeather = NewWeather;
    TargetWeatherSettings = GetWeatherPreset(NewWeather);

    if (bInstant)
    {
        CurrentWeatherSettings = TargetWeatherSettings;
        bIsTransitioning = false;
    }
    else
    {
        bIsTransitioning = true;
        WeatherTransitionProgress = 0.0f;
    }

    OnWeatherChanged(NewWeather);
    UE_LOG(LogTemp, Log, TEXT("Weather changed to: %d"), (int32)NewWeather);
}

void AEnvironmentSystem::SetTimeOfDay(ETimeOfDay NewTime, bool bInstant)
{
    PreviousTime = CurrentTimeOfDay;
    CurrentTimeOfDay = NewTime;

    switch (NewTime)
    {
        case ETimeOfDay::Dawn:
            CurrentTime = 6.0f;
            break;
        case ETimeOfDay::Morning:
            CurrentTime = 9.0f;
            break;
        case ETimeOfDay::Noon:
            CurrentTime = 12.0f;
            break;
        case ETimeOfDay::Afternoon:
            CurrentTime = 15.0f;
            break;
        case ETimeOfDay::Dusk:
            CurrentTime = 18.0f;
            break;
        case ETimeOfDay::Night:
            CurrentTime = 21.0f;
            break;
        case ETimeOfDay::Midnight:
            CurrentTime = 0.0f;
            break;
    }

    UpdateLighting();
    OnTimeOfDayChanged(NewTime);
}

void AEnvironmentSystem::SetExactTime(float Hours)
{
    CurrentTime = FMath::Clamp(Hours, 0.0f, 24.0f);
    UpdateLighting();
}

void AEnvironmentSystem::UpdateLighting()
{
    if (!DirectionalLight)
    {
        return;
    }

    UDirectionalLightComponent* LightComp = DirectionalLight->GetComponent();
    if (!LightComp)
    {
        return;
    }

    // Calculate sun angle
    float SunAngle = GetSunAngle();
    FRotator SunRotation = FRotator(-SunAngle, 0.0f, 0.0f);
    DirectionalLight->SetActorRotation(SunRotation);

    // Set sun intensity
    float Intensity = GetSunIntensity();
    LightComp->SetIntensity(Intensity);

    // Set sun color
    FLinearColor SunColor = GetSunColor();
    LightComp->SetLightColor(SunColor);

    // Update sky light
    if (SkyLight)
    {
        USkyLightComponent* SkyComp = SkyLight->GetLightComponent();
        if (SkyComp)
        {
            float SkyIntensity = FMath::Lerp(0.5f, 1.5f, Intensity / 10.0f);
            SkyComp->SetIntensity(SkyIntensity * (1.0f - CurrentWeatherSettings.CloudCoverage * 0.5f));
            SkyComp->RecaptureSky();
        }
    }
}

void AEnvironmentSystem::UpdateWeatherEffects(float DeltaTime)
{
    // Rain particles
    if (RainParticles)
    {
        if (CurrentWeatherSettings.RainIntensity > 0.1f)
        {
            if (!RainParticles->IsActive())
            {
                RainParticles->Activate(true);
            }
            // Set rain intensity in Niagara system (requires Niagara parameter)
            RainParticles->SetFloatParameter(FName("Intensity"), CurrentWeatherSettings.RainIntensity);
        }
        else
        {
            if (RainParticles->IsActive())
            {
                RainParticles->Deactivate();
            }
        }
    }

    // Snow particles
    if (SnowParticles && CurrentWeather == EWeatherType::Snow)
    {
        if (!SnowParticles->IsActive())
        {
            SnowParticles->Activate(true);
        }
        SnowParticles->SetFloatParameter(FName("Intensity"), 1.0f);
    }
    else if (SnowParticles && SnowParticles->IsActive())
    {
        SnowParticles->Deactivate();
    }

    // Lightning (random during storm)
    if (LightningEffect && CurrentWeather == EWeatherType::Storm)
    {
        if (FMath::FRand() < 0.01f * DeltaTime) // ~1% chance per second
        {
            LightningEffect->Activate(true);
        }
    }
}

void AEnvironmentSystem::UpdateFog()
{
    if (!HeightFog)
    {
        return;
    }

    UExponentialHeightFogComponent* FogComp = HeightFog->GetComponent();
    if (!FogComp)
    {
        return;
    }

    // Base fog density
    float BaseDensity = 0.02f;
    float FogDensity = BaseDensity + (CurrentWeatherSettings.FogDensity * 0.5f);
    FogComp->SetFogDensity(FogDensity);

    // Fog color based on time
    FLinearColor FogColor;
    if (CurrentTime >= 6.0f && CurrentTime < 18.0f)
    {
        FogColor = FLinearColor(0.8f, 0.85f, 0.9f); // Day - light blue
    }
    else
    {
        FogColor = FLinearColor(0.05f, 0.05f, 0.1f); // Night - dark blue
    }
    
    // Adjust for weather
    if (CurrentWeather == EWeatherType::Storm || CurrentWeather == EWeatherType::HeavyRain)
    {
        FogColor = FogColor * 0.5f; // Darker in storms
    }

    FogComp->SetFogInscatteringColor(FogColor);
}

void AEnvironmentSystem::UpdatePostProcessing()
{
    if (!PostProcessVolume)
    {
        return;
    }

    // Adjust bloom based on time (more at sunset/sunrise)
    float BloomIntensity = 0.5f;
    if ((CurrentTime >= 5.0f && CurrentTime <= 7.0f) || (CurrentTime >= 17.0f && CurrentTime <= 19.0f))
    {
        BloomIntensity = 1.5f; // Golden hour
    }

    // Reduce bloom in rain/fog
    BloomIntensity *= (1.0f - CurrentWeatherSettings.FogDensity * 0.5f);

    // Color grading for different weather
    if (CurrentWeather == EWeatherType::Overcast || CurrentWeather == EWeatherType::HeavyRain)
    {
        // Desaturate in bad weather
        PostProcessVolume->Settings.ColorSaturation = FVector4(0.8f, 0.8f, 0.8f, 1.0f);
    }
    else
    {
        PostProcessVolume->Settings.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    PostProcessVolume->Settings.bOverride_BloomIntensity = true;
    PostProcessVolume->Settings.BloomIntensity = BloomIntensity;
}

void AEnvironmentSystem::TriggerRandomWeatherChange()
{
    TArray<EWeatherType> WeatherTypes = {
        EWeatherType::Clear,
        EWeatherType::PartlyCloudy,
        EWeatherType::Overcast,
        EWeatherType::LightRain,
        EWeatherType::Fog
    };

    int32 RandomIndex = FMath::RandRange(0, WeatherTypes.Num() - 1);
    EWeatherType NewWeather = WeatherTypes[RandomIndex];
    
    if (NewWeather != CurrentWeather)
    {
        SetWeather(NewWeather, false);
    }
}

FWeatherSettings AEnvironmentSystem::GetWeatherPreset(EWeatherType Weather)
{
    FWeatherSettings Settings;

    switch (Weather)
    {
        case EWeatherType::Clear:
            Settings.RainIntensity = 0.0f;
            Settings.FogDensity = 0.0f;
            Settings.WindSpeed = 5.0f;
            Settings.CloudCoverage = 0.1f;
            Settings.TrackWetness = 0.0f;
            Settings.TrackGripMultiplier = 1.0f;
            Settings.Visibility = 1.0f;
            break;

        case EWeatherType::PartlyCloudy:
            Settings.RainIntensity = 0.0f;
            Settings.FogDensity = 0.1f;
            Settings.WindSpeed = 10.0f;
            Settings.CloudCoverage = 0.4f;
            Settings.TrackWetness = 0.0f;
            Settings.TrackGripMultiplier = 1.0f;
            Settings.Visibility = 0.95f;
            break;

        case EWeatherType::Overcast:
            Settings.RainIntensity = 0.0f;
            Settings.FogDensity = 0.2f;
            Settings.WindSpeed = 15.0f;
            Settings.CloudCoverage = 0.8f;
            Settings.TrackWetness = 0.1f;
            Settings.TrackGripMultiplier = 0.95f;
            Settings.Visibility = 0.9f;
            break;

        case EWeatherType::LightRain:
            Settings.RainIntensity = 0.4f;
            Settings.FogDensity = 0.3f;
            Settings.WindSpeed = 20.0f;
            Settings.CloudCoverage = 0.9f;
            Settings.TrackWetness = 0.5f;
            Settings.TrackGripMultiplier = 0.8f;
            Settings.Visibility = 0.8f;
            break;

        case EWeatherType::HeavyRain:
            Settings.RainIntensity = 1.0f;
            Settings.FogDensity = 0.5f;
            Settings.WindSpeed = 30.0f;
            Settings.CloudCoverage = 1.0f;
            Settings.TrackWetness = 1.0f;
            Settings.TrackGripMultiplier = 0.6f;
            Settings.Visibility = 0.6f;
            break;

        case EWeatherType::Storm:
            Settings.RainIntensity = 1.0f;
            Settings.FogDensity = 0.6f;
            Settings.WindSpeed = 50.0f;
            Settings.CloudCoverage = 1.0f;
            Settings.TrackWetness = 1.0f;
            Settings.TrackGripMultiplier = 0.5f;
            Settings.Visibility = 0.5f;
            break;

        case EWeatherType::Fog:
            Settings.RainIntensity = 0.0f;
            Settings.FogDensity = 0.8f;
            Settings.WindSpeed = 5.0f;
            Settings.CloudCoverage = 0.9f;
            Settings.TrackWetness = 0.2f;
            Settings.TrackGripMultiplier = 0.9f;
            Settings.Visibility = 0.4f;
            break;

        case EWeatherType::Snow:
            Settings.RainIntensity = 0.0f;
            Settings.FogDensity = 0.4f;
            Settings.WindSpeed = 15.0f;
            Settings.CloudCoverage = 0.9f;
            Settings.TrackWetness = 0.0f;
            Settings.TrackGripMultiplier = 0.7f;
            Settings.Visibility = 0.7f;
            break;
    }

    return Settings;
}

float AEnvironmentSystem::GetSunAngle() const
{
    // Sun angle from horizon (-90 to 90 degrees)
    // Noon (12:00) = 90 degrees (directly overhead)
    // Midnight (0:00/24:00) = -90 degrees (below horizon)
    
    float NormalizedTime = (CurrentTime - 6.0f) / 12.0f; // Normalize to sunrise-sunset
    float Angle = FMath::Lerp(-90.0f, 90.0f, NormalizedTime);
    
    return Angle;
}

float AEnvironmentSystem::GetSunIntensity() const
{
    float SunAngle = GetSunAngle();
    
    // Intensity based on sun angle
    float BaseIntensity = FMath::GetMappedRangeValueClamped(FVector2D(-20.0f, 60.0f), FVector2D(0.0f, 10.0f), SunAngle);
    
    // Reduce intensity in bad weather
    float WeatherMultiplier = 1.0f - (CurrentWeatherSettings.CloudCoverage * 0.7f);
    
    return BaseIntensity * WeatherMultiplier;
}

FLinearColor AEnvironmentSystem::GetSunColor() const
{
    float SunAngle = GetSunAngle();
    
    FLinearColor Color;
    
    if (SunAngle < 0.0f)
    {
        // Night - moonlight (blue-ish)
        Color = FLinearColor(0.7f, 0.8f, 1.0f);
    }
    else if (SunAngle < 20.0f)
    {
        // Sunrise/sunset - golden/orange
        float T = SunAngle / 20.0f;
        Color = FMath::Lerp(FLinearColor(1.0f, 0.5f, 0.2f), FLinearColor(1.0f, 0.95f, 0.9f), T);
    }
    else
    {
        // Day - white/yellow
        Color = FLinearColor(1.0f, 0.95f, 0.9f);
    }
    
    return Color;
}
