// RacingPlayerController.h
// Player controller for racing vehicles
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RacingPlayerController.generated.h"

class ARacingVehicle;
class URacingHUDWidget;

/**
 * Player controller with racing-specific input and camera management
 */
UCLASS()
class CARGAME_API ARacingPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARacingPlayerController();

    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // CAMERA
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    int32 CurrentCameraIndex;

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void CycleCamera();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraView(int32 CameraIndex);

    // ============================================================
    // HUD
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<URacingHUDWidget> HUDWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    URacingHUDWidget* HUDWidget;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowHUD();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideHUD();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void TogglePauseMenu();

    // ============================================================
    // INPUT
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float SteeringInputSmoothness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float ThrottleInputSmoothness;

    // ============================================================
    // VEHICLE ACCESS
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    ARacingVehicle* GetControlledVehicle() const;

private:
    ARacingVehicle* ControlledVehicle;
    
    // Input values
    float RawThrottleInput;
    float RawBrakeInput;
    float RawSteeringInput;
    
    float SmoothedThrottleInput;
    float SmoothedBrakeInput;
    float SmoothedSteeringInput;

    // Input functions
    void HandleThrottle(float Value);
    void HandleBrake(float Value);
    void HandleSteering(float Value);
    void HandleHandbrakePressed();
    void HandleHandbrakeReleased();
    void HandleShiftUp();
    void HandleShiftDown();
    void HandleResetVehicle();
    void HandleCameraChange();
    void HandlePause();

    // Helper functions
    void SmoothInputs(float DeltaTime);
    void CreateHUD();
};
