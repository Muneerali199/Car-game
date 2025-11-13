// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentSystem.generated.h"

class ADirectionalLight;
class ASkyLight;
class AExponentialHeightFog;
class APostProcessVolume;

/**
 * Weather Types
 */
UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Clear           UMETA(DisplayName = "Clear - Sunny, perfect visibility"),
    PartlyCloudy    UMETA(DisplayName = "Partly Cloudy - Some clouds"),
    Overcast        UMETA(DisplayName = "Overcast - Heavy clouds, dim"),
    LightRain       UMETA(DisplayName = "Light Rain - Wet track, reduced grip"),
    HeavyRain       UMETA(DisplayName = "Heavy Rain - Poor visibility, very slippery"),
    Storm           UMETA(DisplayName = "Storm - Lightning, extreme conditions"),
    Fog             UMETA(DisplayName = "Fog - Low visibility"),
    Snow            UMETA(DisplayName = "Snow - Cold, slippery")
};

/**
 * Time of Day
 */
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
    Dawn            UMETA(DisplayName = "Dawn - 06:00, golden hour"),
    Morning         UMETA(DisplayName = "Morning - 09:00, bright"),
    Noon            UMETA(DisplayName = "Noon - 12:00, harsh shadows"),
    Afternoon       UMETA(DisplayName = "Afternoon - 15:00, warm"),
    Dusk            UMETA(DisplayName = "Dusk - 18:00, golden hour"),
    Night           UMETA(DisplayName = "Night - 21:00, dark with lights"),
    Midnight        UMETA(DisplayName = "Midnight - 00:00, very dark")
};

/**
 * Weather Settings
 */
USTRUCT(BlueprintType)
struct FWeatherSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float RainIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float FogDensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WindSpeed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float CloudCoverage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float TrackWetness = 0.0f; // 0 = dry, 1 = soaked

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float TrackGripMultiplier = 1.0f; // 1 = normal, 0.5 = slippery

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float Visibility = 1.0f; // 0 = can't see, 1 = perfect
};

/**
 * Advanced Environment System
 * 
 * Features:
 * - Dynamic weather changes
 * - Day/night cycle
 * - Real-time lighting updates
 * - Weather effects on vehicle grip
 * - Particle systems for rain/snow
 * - Post-processing adjustments
 */
UCLASS()
class CARGAME_API AEnvironmentSystem : public AActor
{
    GENERATED_BODY()

public:
    AEnvironmentSystem();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // Weather Configuration
    // ============================================================

    /** Current weather type */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Weather")
    EWeatherType CurrentWeather = EWeatherType::Clear;

    /** Current time of day */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Time")
    ETimeOfDay CurrentTimeOfDay = ETimeOfDay::Noon;

    /** Enable dynamic weather changes */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Weather")
    bool bEnableDynamicWeather = false;

    /** Weather change interval (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Weather", meta = (ClampMin = "60.0"))
    float WeatherChangeInterval = 300.0f;

    /** Enable day/night cycle */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Time")
    bool bEnableDayNightCycle = false;

    /** Day/night cycle speed (1.0 = real-time, 60.0 = 1 minute = 1 hour) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Time", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
    float TimeSpeed = 60.0f;

    /** Current time (0-24 hours) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Time", meta = (ClampMin = "0.0", ClampMax = "24.0"))
    float CurrentTime = 12.0f;

    /** Weather transition duration (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Weather", meta = (ClampMin = "1.0"))
    float WeatherTransitionDuration = 10.0f;

    // ============================================================
    // Weather State
    // ============================================================

    /** Current weather settings */
    UPROPERTY(BlueprintReadOnly, Category = "Environment|State")
    FWeatherSettings CurrentWeatherSettings;

    /** Target weather settings (for transitions) */
    UPROPERTY(BlueprintReadOnly, Category = "Environment|State")
    FWeatherSettings TargetWeatherSettings;

    // ============================================================
    // Lighting References
    // ============================================================

    /** Directional light (sun/moon) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|References")
    ADirectionalLight* DirectionalLight;

    /** Sky light */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|References")
    ASkyLight* SkyLight;

    /** Exponential height fog */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|References")
    AExponentialHeightFog* HeightFog;

    /** Post process volume */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|References")
    APostProcessVolume* PostProcessVolume;

    // ============================================================
    // Weather Control Functions
    // ============================================================

    /** Change weather to specified type */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    void SetWeather(EWeatherType NewWeather, bool bInstant = false);

    /** Change time of day */
    UFUNCTION(BlueprintCallable, Category = "Environment|Time")
    void SetTimeOfDay(ETimeOfDay NewTime, bool bInstant = false);

    /** Set exact time (0-24) */
    UFUNCTION(BlueprintCallable, Category = "Environment|Time")
    void SetExactTime(float Hours);

    /** Get current weather settings */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    FWeatherSettings GetWeatherSettings() const { return CurrentWeatherSettings; }

    /** Get track grip multiplier for vehicle physics */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    float GetTrackGripMultiplier() const { return CurrentWeatherSettings.TrackGripMultiplier; }

    /** Get visibility factor */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    float GetVisibility() const { return CurrentWeatherSettings.Visibility; }

    // ============================================================
    // Update Functions
    // ============================================================

    /** Update lighting based on time */
    UFUNCTION(BlueprintCallable, Category = "Environment|Time")
    void UpdateLighting();

    /** Update weather effects */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    void UpdateWeatherEffects(float DeltaTime);

    /** Update fog based on weather */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    void UpdateFog();

    /** Update post-processing */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    void UpdatePostProcessing();

    /** Trigger random weather change */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    void TriggerRandomWeatherChange();

    // ============================================================
    // Weather Presets
    // ============================================================

    /** Get preset weather settings */
    UFUNCTION(BlueprintCallable, Category = "Environment|Weather")
    FWeatherSettings GetWeatherPreset(EWeatherType Weather);

    /** Get sun angle for time of day */
    UFUNCTION(BlueprintCallable, Category = "Environment|Time")
    float GetSunAngle() const;

    /** Get sun intensity for time */
    UFUNCTION(BlueprintCallable, Category = "Environment|Time")
    float GetSunIntensity() const;

    /** Get sun color for time */
    UFUNCTION(BlueprintCallable, Category = "Environment|Time")
    FLinearColor GetSunColor() const;

    // ============================================================
    // Events
    // ============================================================

    /** Called when weather changes */
    UFUNCTION(BlueprintImplementableEvent, Category = "Environment|Events")
    void OnWeatherChanged(EWeatherType NewWeather);

    /** Called when time of day changes */
    UFUNCTION(BlueprintImplementableEvent, Category = "Environment|Events")
    void OnTimeOfDayChanged(ETimeOfDay NewTime);

    // ============================================================
    // Particle Systems
    // ============================================================

    /** Rain particle system */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|VFX")
    class UNiagaraComponent* RainParticles;

    /** Snow particle system */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|VFX")
    class UNiagaraComponent* SnowParticles;

    /** Lightning effect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|VFX")
    class UNiagaraComponent* LightningEffect;

    // ============================================================
    // Debug
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Debug")
    bool bShowDebugInfo = false;

private:
    float WeatherTransitionProgress = 0.0f;
    float TimeSinceWeatherChange = 0.0f;
    bool bIsTransitioning = false;
    EWeatherType PreviousWeather;
    ETimeOfDay PreviousTime;
};
