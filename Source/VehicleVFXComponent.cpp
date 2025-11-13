// VehicleVFXComponent.cpp
// Vehicle VFX implementation
// Copyright 2025. All Rights Reserved.

#include "VehicleVFXComponent.h"
#include "RacingVehicle.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UVehicleVFXComponent::UVehicleVFXComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Tire smoke settings
    SmokeEmissionThreshold = 0.4f;
    SmokeIntensityMultiplier = 100.0f;
    bEnableTireSmoke = true;

    // Sparks settings
    SparkImpactThreshold = 800.0f;
    bEnableSparks = true;

    // Dust settings
    DustSpeedThreshold = 30.0f;
    bEnableDust = true;

    // Skid marks
    SkidMarkWidth = 20.0f;
    SkidMarkLifetime = 30.0f;
    bEnableSkidMarks = true;

    // Speed lines
    SpeedLinesThreshold = 150.0f;
    bEnableSpeedLines = true;

    // Exhaust
    bEnableExhaust = true;
    ExhaustLocations = {
        FVector(-200.0f, -50.0f, 20.0f),
        FVector(-200.0f, 50.0f, 20.0f)
    };

    OwnerVehicle = nullptr;
}

void UVehicleVFXComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerVehicle = Cast<ARacingVehicle>(GetOwner());

    if (!OwnerVehicle)
    {
        UE_LOG(LogTemp, Warning, TEXT("VehicleVFXComponent: Owner is not a RacingVehicle"));
        return;
    }

    // Create tire smoke components (one for each wheel)
    if (bEnableTireSmoke && TireSmokeEffect)
    {
        TArray<FVector> WheelLocations = GetWheelLocations();
        for (const FVector& Location : WheelLocations)
        {
            UNiagaraComponent* SmokeComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                TireSmokeEffect,
                OwnerVehicle->GetMesh(),
                NAME_None,
                Location,
                FRotator::ZeroRotator,
                EAttachLocation::KeepRelativeOffset,
                false
            );

            if (SmokeComp)
            {
                SmokeComp->SetFloatParameter(TEXT("SpawnRate"), 0.0f);
                TireSmokeComponents.Add(SmokeComp);
            }
        }
    }

    // Create speed lines
    if (bEnableSpeedLines && SpeedLinesEffect)
    {
        SpeedLinesComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            SpeedLinesEffect,
            OwnerVehicle->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            false
        );

        if (SpeedLinesComponent)
        {
            SpeedLinesComponent->SetFloatParameter(TEXT("Intensity"), 0.0f);
        }
    }

    // Create exhaust effects
    if (bEnableExhaust && ExhaustEffect)
    {
        for (const FVector& ExhaustLoc : ExhaustLocations)
        {
            UNiagaraComponent* ExhaustComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                ExhaustEffect,
                OwnerVehicle->GetMesh(),
                NAME_None,
                ExhaustLoc,
                FRotator::ZeroRotator,
                EAttachLocation::KeepRelativeOffset,
                true
            );

            if (ExhaustComp)
            {
                ExhaustComponents.Add(ExhaustComp);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Vehicle VFX Component initialized"));
}

void UVehicleVFXComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bEnableTireSmoke)
        UpdateTireSmoke(DeltaTime);

    if (bEnableDust)
        UpdateDust(DeltaTime);

    if (bEnableSpeedLines)
        UpdateSpeedLines(DeltaTime);

    if (bEnableExhaust)
        UpdateExhaust(DeltaTime);
}

// ============================================================
// SPARK EFFECTS
// ============================================================

void UVehicleVFXComponent::SpawnSparksAtLocation(FVector Location, FVector Normal, float Intensity)
{
    if (!bEnableSparks || !SparksEffect)
        return;

    if (Intensity < SparkImpactThreshold)
        return;

    FRotator Rotation = Normal.Rotation();
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        SparksEffect,
        Location,
        Rotation,
        FVector(1.0f),
        true,
        true,
        ENCPoolMethod::None
    );
}

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

void UVehicleVFXComponent::UpdateTireSmoke(float DeltaTime)
{
    if (!OwnerVehicle)
        return;

    float TireSlip = GetTireSlipAmount();

    for (UNiagaraComponent* SmokeComp : TireSmokeComponents)
    {
        if (!SmokeComp)
            continue;

        if (TireSlip > SmokeEmissionThreshold)
        {
            float SlipAmount = (TireSlip - SmokeEmissionThreshold) / (1.0f - SmokeEmissionThreshold);
            float SpawnRate = SlipAmount * SmokeIntensityMultiplier;
            
            SmokeComp->SetFloatParameter(TEXT("SpawnRate"), SpawnRate);
            SmokeComp->SetFloatParameter(TEXT("Lifetime"), 2.0f);
        }
        else
        {
            SmokeComp->SetFloatParameter(TEXT("SpawnRate"), 0.0f);
        }
    }
}

void UVehicleVFXComponent::UpdateDust(float DeltaTime)
{
    // TODO: Implement dust based on surface type and speed
}

void UVehicleVFXComponent::UpdateSpeedLines(float DeltaTime)
{
    if (!SpeedLinesComponent || !OwnerVehicle)
        return;

    float Speed = GetVehicleSpeed();

    if (Speed > SpeedLinesThreshold)
    {
        float SpeedFactor = (Speed - SpeedLinesThreshold) / 100.0f;
        float Intensity = FMath::Clamp(SpeedFactor, 0.0f, 1.0f);
        
        SpeedLinesComponent->SetFloatParameter(TEXT("Intensity"), Intensity);
    }
    else
    {
        SpeedLinesComponent->SetFloatParameter(TEXT("Intensity"), 0.0f);
    }
}

void UVehicleVFXComponent::UpdateExhaust(float DeltaTime)
{
    if (!OwnerVehicle)
        return;

    FVehicleTelemetry Telemetry = OwnerVehicle->GetTelemetry();
    float RPMNormalized = Telemetry.EngineRPM / OwnerVehicle->MaxEngineRPM;

    for (UNiagaraComponent* ExhaustComp : ExhaustComponents)
    {
        if (ExhaustComp)
        {
            ExhaustComp->SetFloatParameter(TEXT("Intensity"), RPMNormalized);
            ExhaustComp->SetFloatParameter(TEXT("Temperature"), RPMNormalized * 1000.0f);
        }
    }
}

void UVehicleVFXComponent::CreateSkidMark(FVector Location, FRotator Rotation, int32 WheelIndex)
{
    // TODO: Implement decal-based skid marks
}

float UVehicleVFXComponent::GetTireSlipAmount()
{
    if (!OwnerVehicle)
        return 0.0f;

    FVehicleTelemetry Telemetry = OwnerVehicle->GetTelemetry();
    
    float LateralSlip = FMath::Abs(Telemetry.LateralG) / 2.0f;
    float LongitudinalSlip = FMath::Abs(Telemetry.LongitudinalG) / 2.0f;
    
    return FMath::Max(LateralSlip, LongitudinalSlip);
}

float UVehicleVFXComponent::GetVehicleSpeed()
{
    if (!OwnerVehicle)
        return 0.0f;

    return OwnerVehicle->GetTelemetry().Speed;
}

TArray<FVector> UVehicleVFXComponent::GetWheelLocations()
{
    // Default wheel positions (relative to vehicle)
    TArray<FVector> Locations;
    Locations.Add(FVector(150.0f, -80.0f, 0.0f));  // Front Left
    Locations.Add(FVector(150.0f, 80.0f, 0.0f));   // Front Right
    Locations.Add(FVector(-150.0f, -80.0f, 0.0f)); // Rear Left
    Locations.Add(FVector(-150.0f, 80.0f, 0.0f));  // Rear Right
    
    return Locations;
}
