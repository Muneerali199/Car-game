// RacingGameMode.h
// Core game mode for racing gameplay
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RacingGameMode.generated.h"

class ARaceTrackManager;
class ARacingVehicle;

UENUM(BlueprintType)
enum class ERaceState : uint8
{
    Waiting         UMETA(DisplayName = "Waiting to Start"),
    Countdown       UMETA(DisplayName = "Countdown"),
    Racing          UMETA(DisplayName = "Race Active"),
    Finished        UMETA(DisplayName = "Race Finished"),
    Paused          UMETA(DisplayName = "Paused")
};

USTRUCT(BlueprintType)
struct FRacerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    ARacingVehicle* Vehicle;

    UPROPERTY(BlueprintReadOnly)
    int32 Position;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentLap;

    UPROPERTY(BlueprintReadOnly)
    float CurrentLapTime;

    UPROPERTY(BlueprintReadOnly)
    float BestLapTime;

    UPROPERTY(BlueprintReadOnly)
    float TotalRaceTime;

    UPROPERTY(BlueprintReadOnly)
    int32 CheckpointsHit;

    FRacerData()
        : Vehicle(nullptr), Position(0), CurrentLap(0), 
          CurrentLapTime(0.f), BestLapTime(0.f), TotalRaceTime(0.f), CheckpointsHit(0)
    {
    }
};

/**
 * Main game mode for racing events
 */
UCLASS()
class CARGAME_API ARacingGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ARacingGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // RACE MANAGEMENT
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Settings")
    int32 TotalLaps;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Settings")
    float CountdownTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Settings")
    bool bEnableAI;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Settings")
    int32 NumberOfAIRacers;

    UPROPERTY(BlueprintReadOnly, Category = "Race State")
    ERaceState CurrentRaceState;

    UPROPERTY(BlueprintReadOnly, Category = "Race State")
    float RaceTimer;

    UPROPERTY(BlueprintReadOnly, Category = "Race State")
    TArray<FRacerData> RacerDataList;

    // ============================================================
    // RACE CONTROL
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void StartRace();

    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void EndRace();

    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void PauseRace();

    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void ResumeRace();

    UFUNCTION(BlueprintCallable, Category = "Race Control")
    void RestartRace();

    // ============================================================
    // RACER TRACKING
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "Race Tracking")
    void RegisterRacer(ARacingVehicle* Racer);

    UFUNCTION(BlueprintCallable, Category = "Race Tracking")
    void UpdateRacerCheckpoint(ARacingVehicle* Racer, int32 CheckpointIndex);

    UFUNCTION(BlueprintCallable, Category = "Race Tracking")
    void OnRacerCompleteLap(ARacingVehicle* Racer, float LapTime);

    UFUNCTION(BlueprintCallable, Category = "Race Tracking")
    void UpdateRacerPositions();

    UFUNCTION(BlueprintCallable, Category = "Race Tracking")
    FRacerData GetRacerData(ARacingVehicle* Racer);

    UFUNCTION(BlueprintCallable, Category = "Race Tracking")
    TArray<FRacerData> GetLeaderboard();

    // ============================================================
    // EVENTS
    // ============================================================

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaceStateChanged, ERaceState, NewState);
    UPROPERTY(BlueprintAssignable, Category = "Race Events")
    FOnRaceStateChanged OnRaceStateChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLapCompleted, ARacingVehicle*, Racer, float, LapTime);
    UPROPERTY(BlueprintAssignable, Category = "Race Events")
    FOnLapCompleted OnLapCompleted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaceFinished, ARacingVehicle*, Winner);
    UPROPERTY(BlueprintAssignable, Category = "Race Events")
    FOnRaceFinished OnRaceFinished;

private:
    float CountdownTimer;
    ARaceTrackManager* TrackManager;

    void UpdateCountdown(float DeltaTime);
    void UpdateRaceTimer(float DeltaTime);
    void CheckRaceCompletion();
};
