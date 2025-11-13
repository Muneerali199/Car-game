// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RacingVehicle.h"
#include "NetworkedRacingVehicle.generated.h"

/**
 * Networked Racing Vehicle
 * 
 * Extends RacingVehicle with multiplayer replication
 * Handles client-side prediction and server reconciliation
 * Optimized network bandwidth usage
 */
UCLASS()
class CARGAME_API ANetworkedRacingVehicle : public ARacingVehicle
{
    GENERATED_BODY()

public:
    ANetworkedRacingVehicle();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // Network Replication
    // ============================================================

    /** Replicated vehicle state for smooth interpolation */
    UPROPERTY(Replicated)
    FVector ReplicatedLocation;

    UPROPERTY(Replicated)
    FRotator ReplicatedRotation;

    UPROPERTY(Replicated)
    FVector ReplicatedVelocity;

    UPROPERTY(Replicated)
    float ReplicatedSteeringInput;

    UPROPERTY(Replicated)
    float ReplicatedThrottleInput;

    UPROPERTY(Replicated)
    float ReplicatedBrakeInput;

    UPROPERTY(Replicated)
    float ReplicatedSpeed;

    UPROPERTY(Replicated)
    int32 ReplicatedGear;

    /** Last server timestamp */
    UPROPERTY(Replicated)
    float ServerTimestamp;

    // ============================================================
    // Client Prediction
    // ============================================================

    /** Enable client-side prediction */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Network")
    bool bEnableClientPrediction = true;

    /** Position correction threshold (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Network")
    float PositionCorrectionThreshold = 100.0f;

    /** Rotation correction threshold (degrees) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Network")
    float RotationCorrectionThreshold = 10.0f;

    /** Interpolation speed for corrections */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Network")
    float CorrectionInterpolationSpeed = 10.0f;

    // ============================================================
    // Server RPCs (Client -> Server)
    // ============================================================

    /** Send input to server */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSendInput(float Steering, float Throttle, float Brake, float Timestamp);

    /** Request vehicle reset */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRequestReset();

    /** Request respawn */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRequestRespawn();

    /** Send camera change */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerChangCamera(int32 CameraIndex);

    // ============================================================
    // Client RPCs (Server -> Client)
    // ============================================================

    /** Correct client position */
    UFUNCTION(Client, Unreliable)
    void ClientCorrectPosition(FVector NewLocation, FRotator NewRotation, FVector NewVelocity, float Timestamp);

    /** Notify collision */
    UFUNCTION(Client, Reliable)
    void ClientNotifyCollision(FVector ImpactLocation, FVector ImpactNormal, float ImpactForce);

    /** Notify lap completed */
    UFUNCTION(Client, Reliable)
    void ClientNotifyLapCompleted(int32 LapNumber, float LapTime, bool bBestLap);

    /** Notify position changed */
    UFUNCTION(Client, Reliable)
    void ClientNotifyPositionChanged(int32 NewPosition);

    // ============================================================
    // Multicast RPCs (Server -> All Clients)
    // ============================================================

    /** Broadcast horn sound */
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastPlayHorn();

    /** Broadcast impact effect */
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastPlayImpactEffect(FVector Location, float Severity);

    /** Broadcast nitrous effect */
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastActivateNitrous();

    // ============================================================
    // Network Optimization
    // ============================================================

    /** Update replication rate based on distance */
    UFUNCTION()
    void UpdateNetworkRelevancy();

    /** Compress vehicle state for network */
    UFUNCTION()
    void CompressVehicleState();

    /** Decompress vehicle state from network */
    UFUNCTION()
    void DecompressVehicleState();

    /** Get network priority (for bandwidth management) */
    virtual float GetNetPriority(const FVector& ViewPos, const FVector& ViewDir, AActor* Viewer, AActor* ViewTarget, UActorChannel* InChannel, float Time, bool bLowBandwidth) override;

    // ============================================================
    // Interpolation
    // ============================================================

    /** Enable smooth interpolation between updates */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Network")
    bool bEnableSmoothing = true;

    /** Interpolation time (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Network")
    float InterpolationTime = 0.1f;

    /** Smooth vehicle movement on clients */
    UFUNCTION()
    void SmoothNetworkMovement(float DeltaTime);

    // ============================================================
    // Anti-Cheat
    // ============================================================

    /** Maximum allowed speed (server validation) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|AntiCheat")
    float MaxAllowedSpeed = 400.0f; // km/h

    /** Check for suspicious behavior */
    UFUNCTION()
    bool ValidateClientInput(float Steering, float Throttle, float Brake);

    /** Check for speed hacks */
    UFUNCTION()
    bool ValidateSpeed();

    /** Check for teleporting */
    UFUNCTION()
    bool ValidatePosition(FVector NewPosition);

    // ============================================================
    // Lag Compensation
    // ============================================================

    /** Compensate for client latency */
    UFUNCTION()
    void ApplyLagCompensation(float ClientTimestamp);

    /** Estimate client ping */
    UFUNCTION()
    float GetEstimatedPing() const;

    /** Show network debug info */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Debug")
    bool bShowNetworkDebug = false;

    UFUNCTION()
    void DrawNetworkDebugInfo();

private:
    // Client prediction history
    struct FInputHistory
    {
        float Timestamp;
        float Steering;
        float Throttle;
        float Brake;
        FVector Location;
        FRotator Rotation;
    };

    TArray<FInputHistory> InputHistory;
    int32 MaxHistorySize = 60; // 1 second at 60fps

    // Interpolation state
    FVector InterpolationStartLocation;
    FRotator InterpolationStartRotation;
    FVector InterpolationTargetLocation;
    FRotator InterpolationTargetRotation;
    float InterpolationProgress = 0.0f;

    // Network stats
    float LastUpdateTime = 0.0f;
    float PacketLoss = 0.0f;
    int32 PacketsSent = 0;
    int32 PacketsReceived = 0;
};
