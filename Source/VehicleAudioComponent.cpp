// VehicleAudioComponent.cpp
// Vehicle audio implementation
// Copyright 2025. All Rights Reserved.

#include "VehicleAudioComponent.h"
#include "RacingVehicle.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

UVehicleAudioComponent::UVehicleAudioComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    EngineVolumeMultiplier = 1.0f;
    EnginePitchMultiplier = 1.5f;
    
    TireSquealThreshold = 0.3f;
    TireSquealVolumeMultiplier = 1.0f;
    
    WindVolumeMultiplier = 0.5f;
    WindSpeedThreshold = 50.0f;
    
    MinImpactVelocity = 500.0f;

    EngineAudioComponent = nullptr;
    TireAudioComponent = nullptr;
    WindAudioComponent = nullptr;
    OwnerVehicle = nullptr;
}

void UVehicleAudioComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerVehicle = Cast<ARacingVehicle>(GetOwner());

    // Create audio components
    if (EngineMidRPMSound)
    {
        EngineAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), EngineMidRPMSound);
        if (EngineAudioComponent)
        {
            EngineAudioComponent->bAutoDestroy = false;
            EngineAudioComponent->Play();
        }
    }

    if (TireSquealSound)
    {
        TireAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), TireSquealSound);
        if (TireAudioComponent)
        {
            TireAudioComponent->bAutoDestroy = false;
            TireAudioComponent->SetVolumeMultiplier(0.0f);
            TireAudioComponent->Play();
        }
    }

    if (WindSound)
    {
        WindAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), WindSound);
        if (WindAudioComponent)
        {
            WindAudioComponent->bAutoDestroy = false;
            WindAudioComponent->SetVolumeMultiplier(0.0f);
            WindAudioComponent->Play();
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Vehicle Audio Component initialized"));
}

void UVehicleAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateEngineSound(DeltaTime);
    UpdateTireSound(DeltaTime);
    UpdateWindSound(DeltaTime);
}

// ============================================================
// IMPACT & TRANSMISSION SOUNDS
// ============================================================

void UVehicleAudioComponent::PlayImpactSound(float ImpactStrength, FVector Location)
{
    if (ImpactStrength < MinImpactVelocity)
        return;

    if (ImpactSounds.Num() == 0)
        return;

    // Select random impact sound
    int32 RandomIndex = FMath::RandRange(0, ImpactSounds.Num() - 1);
    USoundBase* ImpactSound = ImpactSounds[RandomIndex];

    if (ImpactSound)
    {
        float Volume = FMath::Clamp(ImpactStrength / 1000.0f, 0.3f, 1.0f);
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location, Volume);
    }
}

void UVehicleAudioComponent::PlayGearShiftSound()
{
    if (GearShiftSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), GearShiftSound);
    }
}

void UVehicleAudioComponent::PlayBackfireSound()
{
    if (BackfireSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), BackfireSound);
    }
}

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

void UVehicleAudioComponent::UpdateEngineSound(float DeltaTime)
{
    if (!EngineAudioComponent || !OwnerVehicle)
        return;

    float RPMNormalized = GetEngineRPMNormalized();

    // Adjust pitch based on RPM
    float TargetPitch = 0.8f + (RPMNormalized * EnginePitchMultiplier);
    EngineAudioComponent->SetPitchMultiplier(TargetPitch);

    // Adjust volume
    float TargetVolume = FMath::Lerp(0.3f, 1.0f, RPMNormalized) * EngineVolumeMultiplier;
    EngineAudioComponent->SetVolumeMultiplier(TargetVolume);
}

void UVehicleAudioComponent::UpdateTireSound(float DeltaTime)
{
    if (!TireAudioComponent || !OwnerVehicle)
        return;

    float TireSlip = GetTireSlipAmount();

    if (TireSlip > TireSquealThreshold)
    {
        float SlipAmount = (TireSlip - TireSquealThreshold) / (1.0f - TireSquealThreshold);
        float TargetVolume = SlipAmount * TireSquealVolumeMultiplier;
        TireAudioComponent->SetVolumeMultiplier(TargetVolume);

        float TargetPitch = 0.8f + (SlipAmount * 0.4f);
        TireAudioComponent->SetPitchMultiplier(TargetPitch);
    }
    else
    {
        TireAudioComponent->SetVolumeMultiplier(0.0f);
    }
}

void UVehicleAudioComponent::UpdateWindSound(float DeltaTime)
{
    if (!WindAudioComponent || !OwnerVehicle)
        return;

    float Speed = GetVehicleSpeed();

    if (Speed > WindSpeedThreshold)
    {
        float SpeedFactor = (Speed - WindSpeedThreshold) / 200.0f;
        float TargetVolume = FMath::Clamp(SpeedFactor, 0.0f, 1.0f) * WindVolumeMultiplier;
        WindAudioComponent->SetVolumeMultiplier(TargetVolume);

        float TargetPitch = 0.9f + (SpeedFactor * 0.3f);
        WindAudioComponent->SetPitchMultiplier(TargetPitch);
    }
    else
    {
        WindAudioComponent->SetVolumeMultiplier(0.0f);
    }
}

float UVehicleAudioComponent::GetEngineRPMNormalized()
{
    if (!OwnerVehicle)
        return 0.0f;

    FVehicleTelemetry Telemetry = OwnerVehicle->GetTelemetry();
    float RPM = Telemetry.EngineRPM;
    float MaxRPM = OwnerVehicle->MaxEngineRPM;

    return FMath::Clamp(RPM / MaxRPM, 0.0f, 1.0f);
}

float UVehicleAudioComponent::GetTireSlipAmount()
{
    if (!OwnerVehicle)
        return 0.0f;

    FVehicleTelemetry Telemetry = OwnerVehicle->GetTelemetry();
    
    // Simplified slip calculation based on lateral G-force
    float LateralSlip = FMath::Abs(Telemetry.LateralG) / 2.0f;
    float LongitudinalSlip = FMath::Abs(Telemetry.LongitudinalG) / 2.0f;
    
    return FMath::Max(LateralSlip, LongitudinalSlip);
}

float UVehicleAudioComponent::GetVehicleSpeed()
{
    if (!OwnerVehicle)
        return 0.0f;

    return OwnerVehicle->GetTelemetry().Speed;
}
