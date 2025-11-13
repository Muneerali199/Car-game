// RacingCameraComponent.h
// Advanced camera system with multiple views
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "RacingCameraComponent.generated.h"

UENUM(BlueprintType)
enum class ECameraView : uint8
{
    Chase          UMETA(DisplayName = "Chase Camera"),
    Cockpit        UMETA(DisplayName = "Cockpit Camera"),
    Hood           UMETA(DisplayName = "Hood Camera"),
    Cinematic      UMETA(DisplayName = "Cinematic Camera"),
    FarChase       UMETA(DisplayName = "Far Chase Camera")
};

USTRUCT(BlueprintType)
struct FCameraSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector RelativeLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator RelativeRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LagSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RotationLagSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnableLag;

    FCameraSettings()
        : RelativeLocation(FVector(-500.0f, 0.0f, 200.0f))
        , RelativeRotation(FRotator(-10.0f, 0.0f, 0.0f))
        , FOV(90.0f)
        , LagSpeed(10.0f)
        , RotationLagSpeed(8.0f)
        , bEnableLag(true)
    {
    }
};

/**
 * Racing camera with multiple view modes and smooth transitions
 */
UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class CARGAME_API URacingCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    URacingCameraComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ============================================================
    // CAMERA VIEWS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Views")
    TMap<ECameraView, FCameraSettings> CameraViews;

    UPROPERTY(BlueprintReadOnly, Category = "Camera Views")
    ECameraView CurrentView;

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraView(ECameraView NewView);

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void CycleCamera();

    // ============================================================
    // CAMERA SHAKE
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Shake")
    bool bEnableSpeedShake;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Shake")
    float ShakeIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Shake")
    float ShakeFrequency;

    // ============================================================
    // LOOK AHEAD
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Behavior")
    bool bEnableLookAhead;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Behavior")
    float LookAheadDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Behavior")
    float LookAheadSpeed;

private:
    class USpringArmComponent* SpringArm;
    
    FVector TargetLocation;
    FRotator TargetRotation;
    float TargetFOV;

    void InitializeCameraViews();
    void UpdateCameraTransform(float DeltaTime);
    void ApplyCameraShake(float DeltaTime);
    void ApplyLookAhead(float DeltaTime);
    float GetVehicleSpeed() const;
};
