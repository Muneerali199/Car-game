// RacingVehicle.h
// Advanced racing vehicle class with realistic physics simulation
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "RacingVehicle.generated.h"

// Forward declarations
class UChaosWheeledVehicleMovementComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;

/**
 * Tire Model Type
 */
UENUM(BlueprintType)
enum class ETireModel : uint8
{
    Simple      UMETA(DisplayName = "Simple"),
    Pacejka     UMETA(DisplayName = "Pacejka (MF6.1)"),
    Brush       UMETA(DisplayName = "Brush Model")
};

/**
 * Drivetrain Configuration
 */
UENUM(BlueprintType)
enum class EDrivetrainType : uint8
{
    FWD         UMETA(DisplayName = "Front-Wheel Drive"),
    RWD         UMETA(DisplayName = "Rear-Wheel Drive"),
    AWD         UMETA(DisplayName = "All-Wheel Drive"),
    Custom      UMETA(DisplayName = "Custom Split")
};

/**
 * Vehicle Telemetry Data Structure
 */
USTRUCT(BlueprintType)
struct FVehicleTelemetry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float EngineRPM;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float Throttle;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float Brake;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float Steering;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    int32 CurrentGear;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float SuspensionCompressionFL;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float SuspensionCompressionFR;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float SuspensionCompressionRL;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float SuspensionCompressionRR;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    FVector Velocity;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    FVector AngularVelocity;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float LateralG;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float LongitudinalG;

    FVehicleTelemetry()
        : Speed(0.f), EngineRPM(0.f), Throttle(0.f), Brake(0.f), 
          Steering(0.f), CurrentGear(1), 
          SuspensionCompressionFL(0.f), SuspensionCompressionFR(0.f),
          SuspensionCompressionRL(0.f), SuspensionCompressionRR(0.f),
          Velocity(FVector::ZeroVector), AngularVelocity(FVector::ZeroVector),
          LateralG(0.f), LongitudinalG(0.f)
    {
    }
};

/**
 * Advanced Racing Vehicle with high-fidelity physics
 */
UCLASS()
class CARGAME_API ARacingVehicle : public AWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    ARacingVehicle();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ============================================================
    // COMPONENTS
    // ============================================================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UChaosWheeledVehicleMovementComponent* VehicleMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* Camera;

    // ============================================================
    // VEHICLE CONFIGURATION
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Configuration")
    ETireModel TireModelType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Configuration")
    EDrivetrainType DrivetrainType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Engine")
    float MaxEngineTorque;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Engine")
    float MaxEngineRPM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Engine")
    float IdleRPM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Engine")
    UCurveFloat* EngineTorqueCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Mass")
    float VehicleMass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Mass")
    FVector CenterOfMassOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Aerodynamics")
    float DragCoefficient;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Aerodynamics")
    float DownforceCoefficient;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Aerodynamics")
    float FrontalArea;

    // ============================================================
    // ASSISTS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Assists")
    bool bABSEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Assists")
    bool bTractionControlEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Assists")
    bool bStabilityControlEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Assists")
    float ABSThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Assists")
    float TractionControlThreshold;

    // ============================================================
    // TELEMETRY
    // ============================================================

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle|Telemetry")
    FVehicleTelemetry CurrentTelemetry;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Telemetry")
    bool bEnableTelemetryLogging;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Telemetry")
    float TelemetryUpdateRate;

    // ============================================================
    // INPUT
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
    void SetThrottle(float Value);

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
    void SetBrake(float Value);

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
    void SetSteering(float Value);

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
    void ShiftUp();

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
    void ShiftDown();

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
    void ToggleHandbrake(bool bEnabled);

    // ============================================================
    // TELEMETRY ACCESS
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Telemetry")
    FVehicleTelemetry GetTelemetry() const;

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Telemetry")
    void ExportTelemetryToFile(const FString& FilePath);

    // ============================================================
    // PHYSICS
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
    void ApplyAerodynamicForces(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
    void UpdateTelemetry(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
    void ApplyDrivingAssists(float DeltaTime);

private:
    // Internal state
    float CurrentThrottle;
    float CurrentBrake;
    float CurrentSteering;
    bool bHandbrakeEngaged;

    float TelemetryTimer;
    FVector PreviousVelocity;

    // Helper functions
    void CalculateGForces(float DeltaTime);
    void UpdateSuspensionTelemetry();
    void LogTelemetry();
};
