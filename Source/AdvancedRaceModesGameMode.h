// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AdvancedRaceModesGameMode.generated.h"

/**
 * Special Race Modes
 */
UENUM(BlueprintType)
enum class ESpecialRaceMode : uint8
{
    Standard            UMETA(DisplayName = "Standard - Classic racing"),
    TimeTrial           UMETA(DisplayName = "Time Trial - Solo against clock"),
    Elimination         UMETA(DisplayName = "Elimination - Last place eliminated each lap"),
    KnockoutRace        UMETA(DisplayName = "Knockout - Survive elimination rounds"),
    Drift               UMETA(DisplayName = "Drift - Score points drifting"),
    Drag                UMETA(DisplayName = "Drag - Straight line acceleration"),
    Rally               UMETA(DisplayName = "Rally - Mixed surfaces, checkpoints"),
    Pursuit             UMETA(DisplayName = "Pursuit - Chase or escape"),
    CircuitChallenge    UMETA(DisplayName = "Circuit Challenge - Beat target time"),
    Endurance           UMETA(DisplayName = "Endurance - Long distance race"),
    CopsAndRobbers      UMETA(DisplayName = "Cops & Robbers - Team pursuit"),
    KingOfTheHill       UMETA(DisplayName = "King of Hill - Control zones"),
    CheckpointRush      UMETA(DisplayName = "Checkpoint Rush - Time extension"),
    Infected            UMETA(DisplayName = "Infected - Tag to infect"),
    GhostRace           UMETA(DisplayName = "Ghost Race - Beat your best"),
    BattleRace          UMETA(DisplayName = "Battle Race - Power-ups enabled")
};

/**
 * Power-Up Types (for Battle Race mode)
 */
UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
    None                UMETA(DisplayName = "No Power-Up"),
    Nitrous             UMETA(DisplayName = "Nitrous - Speed boost"),
    Shield              UMETA(DisplayName = "Shield - Collision protection"),
    Missile             UMETA(DisplayName = "Missile - Target attack"),
    Mine                UMETA(DisplayName = "Mine - Drop behind"),
    EMP                 UMETA(DisplayName = "EMP - Disable nearby vehicles"),
    Teleport            UMETA(DisplayName = "Teleport - Skip ahead"),
    TimeFreeze          UMETA(DisplayName = "Time Freeze - Slow opponents"),
    Repair              UMETA(DisplayName = "Repair - Fix damage"),
    GhostMode           UMETA(DisplayName = "Ghost - Pass through vehicles"),
    Magnet              UMETA(DisplayName = "Magnet - Pull opponents back")
};

/**
 * Drift Scoring
 */
USTRUCT(BlueprintType)
struct FDriftScore
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float DriftTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float DriftAngle = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float DriftSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 DriftPoints = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 ChainMultiplier = 1;

    UPROPERTY(BlueprintReadOnly)
    bool bCleanDrift = true;
};

/**
 * Zone Control Data (King of Hill)
 */
USTRUCT(BlueprintType)
struct FControlZone
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 1000.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 ControllingPlayerID = -1;

    UPROPERTY(BlueprintReadOnly)
    float ControlTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PointsPerSecond = 10;
};

/**
 * Special Race Modes Game Mode
 * 
 * Implements various special racing modes
 * Each with unique rules and scoring
 */
UCLASS()
class CARGAME_API AAdvancedRaceModesGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AAdvancedRaceModesGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // Mode Configuration
    // ============================================================

    /** Current race mode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode")
    ESpecialRaceMode CurrentRaceMode = ESpecialRaceMode::Standard;

    /** Set race mode */
    UFUNCTION(BlueprintCallable, Category = "Race Mode")
    void SetRaceMode(ESpecialRaceMode NewMode);

    // ============================================================
    // ELIMINATION MODE
    // ============================================================

    /** Elimination interval (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Elimination")
    float EliminationInterval = 30.0f;

    /** Time until next elimination */
    UPROPERTY(BlueprintReadOnly, Category = "Race Mode|Elimination")
    float TimeToNextElimination = 0.0f;

    /** Eliminate last place vehicle */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Elimination")
    void EliminateLastPlace();

    // ============================================================
    // DRIFT MODE
    // ============================================================

    /** Drift scoring settings */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Drift")
    float MinDriftAngle = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Drift")
    float PointsPerSecond = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Drift")
    float ChainBonusMultiplier = 1.5f;

    /** Current drift scores by player */
    UPROPERTY(BlueprintReadOnly, Category = "Race Mode|Drift")
    TMap<int32, FDriftScore> DriftScores;

    /** Update drift scoring */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Drift")
    void UpdateDriftScore(int32 PlayerID, float DriftAngle, float Speed, float DeltaTime);

    /** End drift chain */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Drift")
    void EndDriftChain(int32 PlayerID);

    // ============================================================
    // DRAG RACE MODE
    // ============================================================

    /** Drag race distance (meters) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Drag")
    float DragRaceDistance = 402.0f; // 1/4 mile

    /** Perfect launch window (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Drag")
    float PerfectLaunchWindow = 0.2f;

    /** Check for perfect launch */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Drag")
    bool CheckPerfectLaunch(int32 PlayerID, float LaunchRPM);

    /** Calculate drag time */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Drag")
    float CalculateDragTime(int32 PlayerID);

    // ============================================================
    // PURSUIT MODE
    // ============================================================

    /** Pursuit type (chase or escape) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Pursuit")
    bool bIsEscapeMode = true;

    /** Pursuit distance threshold (meters) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Pursuit")
    float PursuitCatchDistance = 5.0f;

    /** Escape distance for win (meters) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Pursuit")
    float EscapeDistance = 500.0f;

    /** Time limit for pursuit (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Pursuit")
    float PursuitTimeLimit = 180.0f;

    /** Check pursuit status */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Pursuit")
    void CheckPursuitStatus();

    // ============================================================
    // KING OF THE HILL MODE
    // ============================================================

    /** Control zones */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|KingOfHill")
    TArray<FControlZone> ControlZones;

    /** Points to win */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|KingOfHill")
    int32 PointsToWin = 1000;

    /** Player scores */
    UPROPERTY(BlueprintReadOnly, Category = "Race Mode|KingOfHill")
    TMap<int32, int32> KingOfHillScores;

    /** Update zone control */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|KingOfHill")
    void UpdateZoneControl(float DeltaTime);

    /** Check which player is in zone */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|KingOfHill")
    int32 GetPlayerInZone(const FControlZone& Zone);

    // ============================================================
    // CHECKPOINT RUSH MODE
    // ============================================================

    /** Time added per checkpoint (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|CheckpointRush")
    float CheckpointTimeBonus = 10.0f;

    /** Starting time (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|CheckpointRush")
    float StartingTime = 30.0f;

    /** Current remaining time */
    UPROPERTY(BlueprintReadOnly, Category = "Race Mode|CheckpointRush")
    float RemainingTime = 30.0f;

    /** Add time for checkpoint */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|CheckpointRush")
    void AddCheckpointTime(int32 PlayerID);

    // ============================================================
    // INFECTED MODE
    // ============================================================

    /** Tag radius (meters) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|Infected")
    float InfectionRadius = 3.0f;

    /** Infected players */
    UPROPERTY(BlueprintReadOnly, Category = "Race Mode|Infected")
    TArray<int32> InfectedPlayers;

    /** Check for infection tags */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Infected")
    void CheckInfectionTags();

    /** Infect player */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Infected")
    void InfectPlayer(int32 PlayerID);

    // ============================================================
    // BATTLE RACE MODE (Power-Ups)
    // ============================================================

    /** Enable power-ups */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|BattleRace")
    bool bPowerUpsEnabled = false;

    /** Power-up spawn locations */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|BattleRace")
    TArray<FVector> PowerUpSpawnLocations;

    /** Power-up respawn time (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Mode|BattleRace")
    float PowerUpRespawnTime = 15.0f;

    /** Current player power-ups */
    UPROPERTY(BlueprintReadOnly, Category = "Race Mode|BattleRace")
    TMap<int32, EPowerUpType> PlayerPowerUps;

    /** Spawn power-up */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|BattleRace")
    void SpawnPowerUp(const FVector& Location);

    /** Collect power-up */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|BattleRace")
    void CollectPowerUp(int32 PlayerID, EPowerUpType PowerUp);

    /** Use power-up */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|BattleRace")
    void UsePowerUp(int32 PlayerID);

    /** Apply power-up effect */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|BattleRace")
    void ApplyPowerUpEffect(int32 PlayerID, EPowerUpType PowerUp);

    // ============================================================
    // GHOST RACE MODE
    // ============================================================

    /** Ghost replay data */
    UPROPERTY(BlueprintReadWrite, Category = "Race Mode|Ghost")
    class UReplaySystem* GhostReplaySystem;

    /** Load ghost data */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Ghost")
    void LoadGhostRace(const FString& ReplayName);

    /** Update ghost comparison */
    UFUNCTION(BlueprintCallable, Category = "Race Mode|Ghost")
    void UpdateGhostComparison(int32 PlayerID);

    /** Time difference from ghost */
    UPROPERTY(BlueprintReadOnly, Category = "Race Mode|Ghost")
    float GhostTimeDifference = 0.0f;

    // ============================================================
    // Mode Management
    // ============================================================

    /** Initialize mode-specific systems */
    UFUNCTION(BlueprintCallable, Category = "Race Mode")
    void InitializeModeSpecificSystems();

    /** Start race with current mode */
    UFUNCTION(BlueprintCallable, Category = "Race Mode")
    void StartRace();

    /** End race with results */
    UFUNCTION(BlueprintCallable, Category = "Race Mode")
    void EndRace();

    /** Get mode-specific scoring */
    UFUNCTION(BlueprintCallable, Category = "Race Mode")
    TMap<int32, int32> GetModeSpecificScores();

    // ============================================================
    // Events
    // ============================================================

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Mode|Events")
    void OnPlayerEliminated(int32 PlayerID, int32 RemainingPlayers);

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Mode|Events")
    void OnDriftChainStarted(int32 PlayerID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Mode|Events")
    void OnDriftChainEnded(int32 PlayerID, int32 TotalPoints);

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Mode|Events")
    void OnPerfectLaunch(int32 PlayerID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Mode|Events")
    void OnPowerUpCollected(int32 PlayerID, EPowerUpType PowerUp);

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Mode|Events")
    void OnPlayerInfected(int32 PlayerID, int32 InfectedByPlayerID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Race Mode|Events")
    void OnZoneCaptured(int32 ZoneIndex, int32 PlayerID);

private:
    void UpdateEliminationMode(float DeltaTime);
    void UpdateDriftMode(float DeltaTime);
    void UpdateCheckpointRushMode(float DeltaTime);
    void UpdateInfectedMode(float DeltaTime);
    void UpdateKingOfHillMode(float DeltaTime);
    void UpdatePursuitMode(float DeltaTime);
};
