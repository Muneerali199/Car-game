// RaceTrackManager.h
// Race track with checkpoints and lap timing
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceTrackManager.generated.h"

class ARacingVehicle;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FCheckpointData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector BoxExtent;

    UPROPERTY(BlueprintReadOnly)
    int32 CheckpointIndex;

    FCheckpointData()
        : Location(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , BoxExtent(FVector(500.0f, 1000.0f, 300.0f))
        , CheckpointIndex(0)
    {
    }
};

/**
 * Manages race track checkpoints, lap counting, and timing
 */
UCLASS()
class CARGAME_API ARaceTrackManager : public AActor
{
    GENERATED_BODY()

public:
    ARaceTrackManager();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // TRACK CONFIGURATION
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Setup")
    TArray<FCheckpointData> Checkpoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Setup")
    bool bAutoGenerateCheckpoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Setup")
    float CheckpointSpacing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Info")
    FString TrackName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Info")
    float TrackLength;

    UPROPERTY(BlueprintReadOnly, Category = "Track Info")
    int32 TotalCheckpoints;

    // ============================================================
    // CHECKPOINT MANAGEMENT
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Track")
    void AddCheckpoint(FVector Location, FRotator Rotation, FVector BoxExtent);

    UFUNCTION(BlueprintCallable, Category = "Track")
    void RemoveCheckpoint(int32 Index);

    UFUNCTION(BlueprintCallable, Category = "Track")
    void ClearAllCheckpoints();

    UFUNCTION(BlueprintCallable, Category = "Track")
    void GenerateCheckpointsFromSpline();

    // ============================================================
    // VEHICLE TRACKING
    // ============================================================

    UFUNCTION()
    void OnCheckpointEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                           bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable, Category = "Track")
    int32 GetVehicleCheckpointIndex(ARacingVehicle* Vehicle);

    UFUNCTION(BlueprintCallable, Category = "Track")
    float GetVehicleDistanceToNextCheckpoint(ARacingVehicle* Vehicle);

    // ============================================================
    // LAP TIMING
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Timing")
    float GetCurrentLapTime(ARacingVehicle* Vehicle);

    UFUNCTION(BlueprintCallable, Category = "Timing")
    float GetBestLapTime(ARacingVehicle* Vehicle);

    // ============================================================
    // EVENTS
    // ============================================================

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCheckpointPassed, ARacingVehicle*, Vehicle, int32, CheckpointIndex);
    UPROPERTY(BlueprintAssignable, Category = "Track Events")
    FOnCheckpointPassed OnCheckpointPassed;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLapCompleted, ARacingVehicle*, Vehicle, float, LapTime);
    UPROPERTY(BlueprintAssignable, Category = "Track Events")
    FOnLapCompleted OnLapCompleted;

private:
    UPROPERTY()
    TArray<UBoxComponent*> CheckpointColliders;

    UPROPERTY()
    TMap<ARacingVehicle*, int32> VehicleCheckpoints;

    UPROPERTY()
    TMap<ARacingVehicle*, float> VehicleLapStartTimes;

    UPROPERTY()
    TMap<ARacingVehicle*, float> VehicleBestLaps;

    void CreateCheckpointColliders();
    void HandleVehicleCheckpoint(ARacingVehicle* Vehicle, int32 CheckpointIndex);
    bool IsLapComplete(ARacingVehicle* Vehicle, int32 CheckpointIndex);
};
