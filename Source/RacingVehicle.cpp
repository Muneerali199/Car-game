// RacingVehicle.cpp
// Advanced racing vehicle implementation
// Copyright 2025. All Rights Reserved.

#include "RacingVehicle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

ARacingVehicle::ARacingVehicle()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create vehicle mesh component (inherited from AWheeledVehiclePawn)
    USkeletalMeshComponent* VehicleMesh = GetMesh();
    VehicleMesh->SetSimulatePhysics(true);
    VehicleMesh->SetCollisionProfileName(FName("Vehicle"));

    // Create Chaos Vehicle Movement Component
    VehicleMovement = CreateDefaultSubobject<UChaosWheeledVehicleMovementComponent>(TEXT("VehicleMovement"));
    VehicleMovement->SetIsReplicated(true);

    // Create Spring Arm for camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 600.0f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 15.0f;
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 10.0f;
    SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
    SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

    // Create Camera
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->FieldOfView = 90.0f;

    // Default vehicle configuration
    TireModelType = ETireModel::Pacejka;
    DrivetrainType = EDrivetrainType::RWD;
    
    MaxEngineTorque = 500.0f;
    MaxEngineRPM = 7500.0f;
    IdleRPM = 1000.0f;
    
    VehicleMass = 1500.0f;
    CenterOfMassOffset = FVector(0.0f, 0.0f, -50.0f);
    
    DragCoefficient = 0.3f;
    DownforceCoefficient = 2.0f;
    FrontalArea = 2.5f;

    // Assists
    bABSEnabled = true;
    bTractionControlEnabled = true;
    bStabilityControlEnabled = true;
    ABSThreshold = 0.95f;
    TractionControlThreshold = 0.9f;

    // Telemetry
    bEnableTelemetryLogging = false;
    TelemetryUpdateRate = 0.1f;
    TelemetryTimer = 0.0f;

    // Input state
    CurrentThrottle = 0.0f;
    CurrentBrake = 0.0f;
    CurrentSteering = 0.0f;
    bHandbrakeEngaged = false;

    PreviousVelocity = FVector::ZeroVector;
}

void ARacingVehicle::BeginPlay()
{
    Super::BeginPlay();

    // Apply mass and center of mass
    if (USkeletalMeshComponent* VehicleMesh = GetMesh())
    {
        VehicleMesh->SetMassOverrideInKg(NAME_None, VehicleMass, true);
        VehicleMesh->SetCenterOfMass(CenterOfMassOffset);
    }

    UE_LOG(LogTemp, Log, TEXT("Racing Vehicle Initialized: %s"), *GetName());
}

void ARacingVehicle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update physics
    ApplyAerodynamicForces(DeltaTime);
    ApplyDrivingAssists(DeltaTime);

    // Update telemetry
    UpdateTelemetry(DeltaTime);

    // Log telemetry if enabled
    if (bEnableTelemetryLogging)
    {
        TelemetryTimer += DeltaTime;
        if (TelemetryTimer >= TelemetryUpdateRate)
        {
            LogTelemetry();
            TelemetryTimer = 0.0f;
        }
    }
}

void ARacingVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind input axes
    PlayerInputComponent->BindAxis("Throttle", this, &ARacingVehicle::SetThrottle);
    PlayerInputComponent->BindAxis("Brake", this, &ARacingVehicle::SetBrake);
    PlayerInputComponent->BindAxis("Steering", this, &ARacingVehicle::SetSteering);

    // Bind input actions
    PlayerInputComponent->BindAction("ShiftUp", IE_Pressed, this, &ARacingVehicle::ShiftUp);
    PlayerInputComponent->BindAction("ShiftDown", IE_Pressed, this, &ARacingVehicle::ShiftDown);
    PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ARacingVehicle::ToggleHandbrake);
}

// ============================================================
// INPUT FUNCTIONS
// ============================================================

void ARacingVehicle::SetThrottle(float Value)
{
    CurrentThrottle = FMath::Clamp(Value, 0.0f, 1.0f);
    if (VehicleMovement)
    {
        VehicleMovement->SetThrottleInput(CurrentThrottle);
    }
}

void ARacingVehicle::SetBrake(float Value)
{
    CurrentBrake = FMath::Clamp(Value, 0.0f, 1.0f);
    if (VehicleMovement)
    {
        VehicleMovement->SetBrakeInput(CurrentBrake);
    }
}

void ARacingVehicle::SetSteering(float Value)
{
    CurrentSteering = FMath::Clamp(Value, -1.0f, 1.0f);
    if (VehicleMovement)
    {
        VehicleMovement->SetSteeringInput(CurrentSteering);
    }
}

void ARacingVehicle::ShiftUp()
{
    if (VehicleMovement)
    {
        VehicleMovement->IncreaseThrottleInput(1.0f);
        UE_LOG(LogTemp, Log, TEXT("Shift Up"));
    }
}

void ARacingVehicle::ShiftDown()
{
    if (VehicleMovement)
    {
        VehicleMovement->DecreaseThrottleInput(1.0f);
        UE_LOG(LogTemp, Log, TEXT("Shift Down"));
    }
}

void ARacingVehicle::ToggleHandbrake(bool bEnabled)
{
    bHandbrakeEngaged = bEnabled;
    if (VehicleMovement)
    {
        VehicleMovement->SetHandbrakeInput(bEnabled);
    }
}

// ============================================================
// PHYSICS FUNCTIONS
// ============================================================

void ARacingVehicle::ApplyAerodynamicForces(float DeltaTime)
{
    if (!VehicleMovement) return;

    USkeletalMeshComponent* VehicleMesh = GetMesh();
    if (!VehicleMesh) return;

    // Get current velocity
    FVector Velocity = VehicleMesh->GetPhysicsLinearVelocity();
    float Speed = Velocity.Size();

    if (Speed > 1.0f)
    {
        // Air density (kg/m^3)
        float AirDensity = 1.225f;

        // Calculate drag force: F = 0.5 * rho * Cd * A * v^2
        float DragMagnitude = 0.5f * AirDensity * DragCoefficient * FrontalArea * Speed * Speed;
        FVector DragForce = -Velocity.GetSafeNormal() * DragMagnitude;

        // Calculate downforce: F = 0.5 * rho * Cl * A * v^2
        float DownforceMagnitude = 0.5f * AirDensity * DownforceCoefficient * FrontalArea * Speed * Speed;
        FVector DownforceForce = FVector(0.0f, 0.0f, -DownforceMagnitude);

        // Apply forces
        VehicleMesh->AddForce(DragForce);
        VehicleMesh->AddForce(DownforceForce);
    }
}

void ARacingVehicle::ApplyDrivingAssists(float DeltaTime)
{
    // ABS implementation
    if (bABSEnabled && CurrentBrake > ABSThreshold)
    {
        // Modulate brake force to prevent wheel lock
        // This is a simplified implementation
        // TODO: Implement per-wheel slip monitoring
    }

    // Traction control implementation
    if (bTractionControlEnabled)
    {
        // Reduce throttle if wheel slip is detected
        // TODO: Implement wheel slip detection and power reduction
    }

    // Stability control
    if (bStabilityControlEnabled)
    {
        // Apply corrective steering/braking to prevent spin
        // TODO: Implement yaw rate monitoring and correction
    }
}

void ARacingVehicle::UpdateTelemetry(float DeltaTime)
{
    if (!VehicleMovement) return;

    USkeletalMeshComponent* VehicleMesh = GetMesh();
    if (!VehicleMesh) return;

    // Get velocity data
    CurrentTelemetry.Velocity = VehicleMesh->GetPhysicsLinearVelocity();
    CurrentTelemetry.Speed = CurrentTelemetry.Velocity.Size() * 0.036f; // Convert cm/s to km/h
    CurrentTelemetry.AngularVelocity = VehicleMesh->GetPhysicsAngularVelocityInDegrees();

    // Input states
    CurrentTelemetry.Throttle = CurrentThrottle;
    CurrentTelemetry.Brake = CurrentBrake;
    CurrentTelemetry.Steering = CurrentSteering;

    // Engine data (simplified - actual implementation would query engine state)
    CurrentTelemetry.EngineRPM = FMath::Lerp(IdleRPM, MaxEngineRPM, CurrentThrottle);
    CurrentTelemetry.CurrentGear = 1; // TODO: Get actual gear from transmission

    // Calculate G-forces
    CalculateGForces(DeltaTime);

    // Update suspension telemetry
    UpdateSuspensionTelemetry();

    PreviousVelocity = CurrentTelemetry.Velocity;
}

void ARacingVehicle::CalculateGForces(float DeltaTime)
{
    if (DeltaTime <= 0.0f) return;

    FVector Acceleration = (CurrentTelemetry.Velocity - PreviousVelocity) / DeltaTime;
    
    // Convert to G-force (1G = 980 cm/s^2)
    float GravityConstant = 980.0f;
    
    FVector LocalAcceleration = GetActorRotation().UnrotateVector(Acceleration);
    CurrentTelemetry.LongitudinalG = LocalAcceleration.X / GravityConstant;
    CurrentTelemetry.LateralG = LocalAcceleration.Y / GravityConstant;
}

void ARacingVehicle::UpdateSuspensionTelemetry()
{
    // TODO: Query actual suspension compression from vehicle movement component
    // For now, use placeholder values
    CurrentTelemetry.SuspensionCompressionFL = 0.5f;
    CurrentTelemetry.SuspensionCompressionFR = 0.5f;
    CurrentTelemetry.SuspensionCompressionRL = 0.5f;
    CurrentTelemetry.SuspensionCompressionRR = 0.5f;
}

void ARacingVehicle::LogTelemetry()
{
    UE_LOG(LogTemp, Log, TEXT("Telemetry - Speed: %.1f km/h | RPM: %.0f | Gear: %d | Throttle: %.2f | Brake: %.2f"),
        CurrentTelemetry.Speed,
        CurrentTelemetry.EngineRPM,
        CurrentTelemetry.CurrentGear,
        CurrentTelemetry.Throttle,
        CurrentTelemetry.Brake);
}

// ============================================================
// TELEMETRY ACCESS
// ============================================================

FVehicleTelemetry ARacingVehicle::GetTelemetry() const
{
    return CurrentTelemetry;
}

void ARacingVehicle::ExportTelemetryToFile(const FString& FilePath)
{
    FString TelemetryData = FString::Printf(
        TEXT("Time,Speed,RPM,Gear,Throttle,Brake,Steering,LatG,LongG\n%.2f,%.1f,%.0f,%d,%.2f,%.2f,%.2f,%.2f,%.2f\n"),
        GetWorld()->GetTimeSeconds(),
        CurrentTelemetry.Speed,
        CurrentTelemetry.EngineRPM,
        CurrentTelemetry.CurrentGear,
        CurrentTelemetry.Throttle,
        CurrentTelemetry.Brake,
        CurrentTelemetry.Steering,
        CurrentTelemetry.LateralG,
        CurrentTelemetry.LongitudinalG
    );

    FFileHelper::SaveStringToFile(TelemetryData, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
    UE_LOG(LogTemp, Log, TEXT("Telemetry exported to: %s"), *FilePath);
}
