// RacingPlayerController.cpp
// Player controller implementation
// Copyright 2025. All Rights Reserved.

#include "RacingPlayerController.h"
#include "RacingVehicle.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ARacingPlayerController::ARacingPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentCameraIndex = 0;
    SteeringInputSmoothness = 0.15f;
    ThrottleInputSmoothness = 0.1f;

    RawThrottleInput = 0.0f;
    RawBrakeInput = 0.0f;
    RawSteeringInput = 0.0f;
    SmoothedThrottleInput = 0.0f;
    SmoothedBrakeInput = 0.0f;
    SmoothedSteeringInput = 0.0f;

    ControlledVehicle = nullptr;
    HUDWidget = nullptr;
}

void ARacingPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Get controlled vehicle
    ControlledVehicle = Cast<ARacingVehicle>(GetPawn());

    // Create HUD
    CreateHUD();

    UE_LOG(LogTemp, Log, TEXT("Racing Player Controller initialized"));
}

void ARacingPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Smooth inputs for better vehicle feel
    SmoothInputs(DeltaTime);

    // Apply smoothed inputs to vehicle
    if (ControlledVehicle)
    {
        ControlledVehicle->SetThrottle(SmoothedThrottleInput);
        ControlledVehicle->SetBrake(SmoothedBrakeInput);
        ControlledVehicle->SetSteering(SmoothedSteeringInput);
    }
}

void ARacingPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Axis bindings
    InputComponent->BindAxis("Throttle", this, &ARacingPlayerController::HandleThrottle);
    InputComponent->BindAxis("Brake", this, &ARacingPlayerController::HandleBrake);
    InputComponent->BindAxis("Steering", this, &ARacingPlayerController::HandleSteering);

    // Action bindings
    InputComponent->BindAction("Handbrake", IE_Pressed, this, &ARacingPlayerController::HandleHandbrakePressed);
    InputComponent->BindAction("Handbrake", IE_Released, this, &ARacingPlayerController::HandleHandbrakeReleased);
    InputComponent->BindAction("ShiftUp", IE_Pressed, this, &ARacingPlayerController::HandleShiftUp);
    InputComponent->BindAction("ShiftDown", IE_Pressed, this, &ARacingPlayerController::HandleShiftDown);
    InputComponent->BindAction("ResetVehicle", IE_Pressed, this, &ARacingPlayerController::HandleResetVehicle);
    InputComponent->BindAction("ChangeCamera", IE_Pressed, this, &ARacingPlayerController::HandleCameraChange);
}

// ============================================================
// CAMERA
// ============================================================

void ARacingPlayerController::CycleCamera()
{
    CurrentCameraIndex = (CurrentCameraIndex + 1) % 3; // 3 camera views
    SetCameraView(CurrentCameraIndex);
}

void ARacingPlayerController::SetCameraView(int32 CameraIndex)
{
    CurrentCameraIndex = FMath::Clamp(CameraIndex, 0, 2);
    
    // Camera switching will be implemented in camera system
    UE_LOG(LogTemp, Log, TEXT("Camera switched to view %d"), CurrentCameraIndex);
}

// ============================================================
// HUD
// ============================================================

void ARacingPlayerController::ShowHUD()
{
    if (HUDWidget)
    {
        HUDWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void ARacingPlayerController::HideHUD()
{
    if (HUDWidget)
    {
        HUDWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ARacingPlayerController::TogglePauseMenu()
{
    bool bIsPaused = UGameplayStatics::IsGamePaused(GetWorld());
    UGameplayStatics::SetGamePaused(GetWorld(), !bIsPaused);
    
    // Show/hide pause menu (to be implemented with UI system)
    UE_LOG(LogTemp, Log, TEXT("Game %s"), bIsPaused ? TEXT("Resumed") : TEXT("Paused"));
}

// ============================================================
// VEHICLE ACCESS
// ============================================================

ARacingVehicle* ARacingPlayerController::GetControlledVehicle() const
{
    return ControlledVehicle;
}

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

void ARacingPlayerController::HandleThrottle(float Value)
{
    RawThrottleInput = FMath::Clamp(Value, 0.0f, 1.0f);
}

void ARacingPlayerController::HandleBrake(float Value)
{
    RawBrakeInput = FMath::Clamp(Value, 0.0f, 1.0f);
}

void ARacingPlayerController::HandleSteering(float Value)
{
    RawSteeringInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ARacingPlayerController::HandleHandbrakePressed()
{
    if (ControlledVehicle)
    {
        ControlledVehicle->ToggleHandbrake(true);
    }
}

void ARacingPlayerController::HandleHandbrakeReleased()
{
    if (ControlledVehicle)
    {
        ControlledVehicle->ToggleHandbrake(false);
    }
}

void ARacingPlayerController::HandleShiftUp()
{
    if (ControlledVehicle)
    {
        ControlledVehicle->ShiftUp();
    }
}

void ARacingPlayerController::HandleShiftDown()
{
    if (ControlledVehicle)
    {
        ControlledVehicle->ShiftDown();
    }
}

void ARacingPlayerController::HandleResetVehicle()
{
    if (ControlledVehicle)
    {
        // Reset vehicle position and rotation
        FVector Location = ControlledVehicle->GetActorLocation() + FVector(0, 0, 200);
        FRotator Rotation = FRotator(0, ControlledVehicle->GetActorRotation().Yaw, 0);
        
        ControlledVehicle->SetActorLocationAndRotation(Location, Rotation);
        ControlledVehicle->GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
        ControlledVehicle->GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        
        UE_LOG(LogTemp, Log, TEXT("Vehicle reset"));
    }
}

void ARacingPlayerController::HandleCameraChange()
{
    CycleCamera();
}

void ARacingPlayerController::HandlePause()
{
    TogglePauseMenu();
}

void ARacingPlayerController::SmoothInputs(float DeltaTime)
{
    // Interpolate steering
    SmoothedSteeringInput = FMath::FInterpTo(
        SmoothedSteeringInput, 
        RawSteeringInput, 
        DeltaTime, 
        1.0f / SteeringInputSmoothness
    );

    // Interpolate throttle
    SmoothedThrottleInput = FMath::FInterpTo(
        SmoothedThrottleInput, 
        RawThrottleInput, 
        DeltaTime, 
        1.0f / ThrottleInputSmoothness
    );

    // Interpolate brake
    SmoothedBrakeInput = FMath::FInterpTo(
        SmoothedBrakeInput, 
        RawBrakeInput, 
        DeltaTime, 
        1.0f / ThrottleInputSmoothness
    );
}

void ARacingPlayerController::CreateHUD()
{
    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<URacingHUDWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("HUD created"));
        }
    }
}
