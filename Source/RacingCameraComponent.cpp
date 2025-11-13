// RacingCameraComponent.cpp
// Camera system implementation
// Copyright 2025. All Rights Reserved.

#include "RacingCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"

URacingCameraComponent::URacingCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    CurrentView = ECameraView::Chase;
    bEnableSpeedShake = true;
    ShakeIntensity = 0.5f;
    ShakeFrequency = 10.0f;

    bEnableLookAhead = true;
    LookAheadDistance = 300.0f;
    LookAheadSpeed = 2.0f;

    FieldOfView = 90.0f;

    InitializeCameraViews();
}

void URacingCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateCameraTransform(DeltaTime);

    if (bEnableSpeedShake)
    {
        ApplyCameraShake(DeltaTime);
    }

    if (bEnableLookAhead && CurrentView == ECameraView::Chase)
    {
        ApplyLookAhead(DeltaTime);
    }
}

// ============================================================
// CAMERA VIEWS
// ============================================================

void URacingCameraComponent::SetCameraView(ECameraView NewView)
{
    if (CurrentView == NewView)
        return;

    CurrentView = NewView;

    // Get settings for new view
    if (CameraViews.Contains(NewView))
    {
        FCameraSettings Settings = CameraViews[NewView];
        TargetLocation = Settings.RelativeLocation;
        TargetRotation = Settings.RelativeRotation;
        TargetFOV = Settings.FOV;

        UE_LOG(LogTemp, Log, TEXT("Camera view changed to: %d"), (int32)NewView);
    }
}

void URacingCameraComponent::CycleCamera()
{
    int32 CurrentIndex = (int32)CurrentView;
    int32 NextIndex = (CurrentIndex + 1) % 5; // 5 camera views
    SetCameraView((ECameraView)NextIndex);
}

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

void URacingCameraComponent::InitializeCameraViews()
{
    // Chase Camera (default)
    FCameraSettings ChaseSettings;
    ChaseSettings.RelativeLocation = FVector(-600.0f, 0.0f, 200.0f);
    ChaseSettings.RelativeRotation = FRotator(-10.0f, 0.0f, 0.0f);
    ChaseSettings.FOV = 90.0f;
    ChaseSettings.LagSpeed = 10.0f;
    ChaseSettings.RotationLagSpeed = 8.0f;
    ChaseSettings.bEnableLag = true;
    CameraViews.Add(ECameraView::Chase, ChaseSettings);

    // Cockpit Camera
    FCameraSettings CockpitSettings;
    CockpitSettings.RelativeLocation = FVector(120.0f, 0.0f, 100.0f);
    CockpitSettings.RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);
    CockpitSettings.FOV = 95.0f;
    CockpitSettings.LagSpeed = 0.0f;
    CockpitSettings.RotationLagSpeed = 0.0f;
    CockpitSettings.bEnableLag = false;
    CameraViews.Add(ECameraView::Cockpit, CockpitSettings);

    // Hood Camera
    FCameraSettings HoodSettings;
    HoodSettings.RelativeLocation = FVector(150.0f, 0.0f, 80.0f);
    HoodSettings.RelativeRotation = FRotator(-3.0f, 0.0f, 0.0f);
    HoodSettings.FOV = 100.0f;
    HoodSettings.LagSpeed = 2.0f;
    HoodSettings.RotationLagSpeed = 3.0f;
    HoodSettings.bEnableLag = true;
    CameraViews.Add(ECameraView::Hood, HoodSettings);

    // Cinematic Camera
    FCameraSettings CinematicSettings;
    CinematicSettings.RelativeLocation = FVector(-800.0f, 300.0f, 300.0f);
    CinematicSettings.RelativeRotation = FRotator(-15.0f, -20.0f, 0.0f);
    CinematicSettings.FOV = 80.0f;
    CinematicSettings.LagSpeed = 5.0f;
    CinematicSettings.RotationLagSpeed = 4.0f;
    CinematicSettings.bEnableLag = true;
    CameraViews.Add(ECameraView::Cinematic, CinematicSettings);

    // Far Chase Camera
    FCameraSettings FarChaseSettings;
    FarChaseSettings.RelativeLocation = FVector(-1000.0f, 0.0f, 400.0f);
    FarChaseSettings.RelativeRotation = FRotator(-20.0f, 0.0f, 0.0f);
    FarChaseSettings.FOV = 85.0f;
    FarChaseSettings.LagSpeed = 8.0f;
    FarChaseSettings.RotationLagSpeed = 6.0f;
    FarChaseSettings.bEnableLag = true;
    CameraViews.Add(ECameraView::FarChase, FarChaseSettings);
}

void URacingCameraComponent::UpdateCameraTransform(float DeltaTime)
{
    if (!CameraViews.Contains(CurrentView))
        return;

    FCameraSettings Settings = CameraViews[CurrentView];

    // Smoothly interpolate to target settings
    FVector CurrentLocation = GetRelativeLocation();
    FRotator CurrentRotation = GetRelativeRotation();
    float CurrentFOV = FieldOfView;

    if (Settings.bEnableLag)
    {
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, Settings.LagSpeed);
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, Settings.RotationLagSpeed);
        float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 5.0f);

        SetRelativeLocation(NewLocation);
        SetRelativeRotation(NewRotation);
        FieldOfView = NewFOV;
    }
    else
    {
        SetRelativeLocation(TargetLocation);
        SetRelativeRotation(TargetRotation);
        FieldOfView = TargetFOV;
    }
}

void URacingCameraComponent::ApplyCameraShake(float DeltaTime)
{
    float Speed = GetVehicleSpeed();
    
    if (Speed < 10.0f)
        return;

    // Speed-based shake
    float ShakeAmount = (Speed / 100.0f) * ShakeIntensity;
    float Time = GetWorld()->GetTimeSeconds() * ShakeFrequency;

    FVector ShakeOffset;
    ShakeOffset.X = FMath::Sin(Time * 1.5f) * ShakeAmount * 2.0f;
    ShakeOffset.Y = FMath::Sin(Time * 2.3f) * ShakeAmount;
    ShakeOffset.Z = FMath::Sin(Time * 1.8f) * ShakeAmount;

    AddRelativeLocation(ShakeOffset * DeltaTime * 10.0f);
}

void URacingCameraComponent::ApplyLookAhead(float DeltaTime)
{
    AActor* Owner = GetOwner();
    if (!Owner)
        return;

    FVector Velocity = Owner->GetVelocity();
    float Speed = Velocity.Size();

    if (Speed > 100.0f)
    {
        FVector LookAheadOffset = Velocity.GetSafeNormal() * LookAheadDistance * (Speed / 1000.0f);
        
        // Smooth look ahead
        FVector CurrentLookTarget = TargetLocation + LookAheadOffset;
        FVector SmoothedLookTarget = FMath::VInterpTo(GetRelativeLocation(), CurrentLookTarget, DeltaTime, LookAheadSpeed);
        
        TargetLocation = SmoothedLookTarget;
    }
}

float URacingCameraComponent::GetVehicleSpeed() const
{
    AActor* Owner = GetOwner();
    if (Owner)
    {
        return Owner->GetVelocity().Size();
    }
    return 0.0f;
}
