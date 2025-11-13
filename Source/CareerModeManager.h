// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CareerModeManager.generated.h"

/**
 * Career Tier/League
 */
UENUM(BlueprintType)
enum class ECareerTier : uint8
{
    Rookie          UMETA(DisplayName = "Rookie - Beginner races"),
    Amateur         UMETA(DisplayName = "Amateur - Local competitions"),
    SemiPro         UMETA(DisplayName = "Semi-Pro - Regional series"),
    Professional    UMETA(DisplayName = "Professional - National championship"),
    Elite           UMETA(DisplayName = "Elite - International series"),
    Master          UMETA(DisplayName = "Master - World championship"),
    Legend          UMETA(DisplayName = "Legend - Ultimate challenge")
};

/**
 * Race Event Type
 */
UENUM(BlueprintType)
enum class ERaceEventType : uint8
{
    SingleRace      UMETA(DisplayName = "Single Race - One-off event"),
    Sprint          UMETA(DisplayName = "Sprint - Short 5-lap race"),
    Feature         UMETA(DisplayName = "Feature - Full race distance"),
    TimeTrial       UMETA(DisplayName = "Time Trial - Beat target time"),
    Elimination     UMETA(DisplayName = "Elimination - Last place eliminated"),
    Endurance       UMETA(DisplayName = "Endurance - 30+ minute race"),
    Championship    UMETA(DisplayName = "Championship - Points series"),
    Tournament      UMETA(DisplayName = "Tournament - Bracket elimination"),
    Boss            UMETA(DisplayName = "Boss Battle - Beat rival")
};

/**
 * Contract Type
 */
UENUM(BlueprintType)
enum class EContractType : uint8
{
    None            UMETA(DisplayName = "No Contract - Free agent"),
    Amateur         UMETA(DisplayName = "Amateur Team - Low pay, low pressure"),
    Factory         UMETA(DisplayName = "Factory Team - Good pay, expectations"),
    Works           UMETA(DisplayName = "Works Team - High pay, high pressure"),
    Independent     UMETA(DisplayName = "Independent - Own team")
};

/**
 * Race Event Data
 */
USTRUCT(BlueprintType)
struct FRaceEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EventName = "Race Event";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERaceEventType EventType = ERaceEventType::SingleRace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECareerTier RequiredTier = ECareerTier::Rookie;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TrackName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NumberOfLaps = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NumberOfOpponents = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PrizeMoneyFirst = 10000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PrizeMoneySecond = 5000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PrizeMoneyThird = 2500;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinReputation = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCompleted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BestTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BestPosition = 0;
};

/**
 * Career Statistics
 */
USTRUCT(BlueprintType)
struct FCareerStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 TotalRaces = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 Wins = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 Podiums = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 PolePositions = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 FastestLaps = 0;

    UPROPERTY(BlueprintReadWrite)
    float TotalDistance = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    float TotalRaceTime = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    int32 Championships = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 CareerEarnings = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 CurrentMoney = 50000;

    UPROPERTY(BlueprintReadWrite)
    int32 ReputationPoints = 0;
};

/**
 * Rival Driver
 */
USTRUCT(BlueprintType)
struct FRivalDriver
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DriverName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DriverSkill = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TeamName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ChampionshipPoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsRival = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RivalryReason;
};

/**
 * Career Save Data
 */
UCLASS()
class CARGAME_API UCareerSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString PlayerName = "Driver";

    UPROPERTY()
    ECareerTier CurrentTier = ECareerTier::Rookie;

    UPROPERTY()
    int32 CareerLevel = 1;

    UPROPERTY()
    int32 ExperiencePoints = 0;

    UPROPERTY()
    FCareerStats Stats;

    UPROPERTY()
    TArray<FRaceEvent> AvailableEvents;

    UPROPERTY()
    TArray<FRaceEvent> CompletedEvents;

    UPROPERTY()
    TArray<FRivalDriver> Rivals;

    UPROPERTY()
    TArray<int32> OwnedVehicles;

    UPROPERTY()
    int32 CurrentVehicleID = 0;

    UPROPERTY()
    EContractType CurrentContract = EContractType::None;

    UPROPERTY()
    FString TeamName;

    UPROPERTY()
    int32 TeamSalary = 0;

    UPROPERTY()
    FDateTime LastPlayed;
};

/**
 * Career Mode Manager
 * 
 * Manages single-player career progression
 * Features:
 * - Progressive tier system
 * - Race events and championships
 * - Money and reputation system
 * - Vehicle ownership and upgrades
 * - Rival drivers and storylines
 * - Contracts and team management
 * - Save/load career progress
 */
UCLASS()
class CARGAME_API ACareerModeManager : public AActor
{
    GENERATED_BODY()

public:
    ACareerModeManager();

protected:
    virtual void BeginPlay() override;

public:
    // ============================================================
    // Career State
    // ============================================================

    /** Current career save */
    UPROPERTY(BlueprintReadOnly, Category = "Career")
    UCareerSaveGame* CurrentCareer;

    /** Save slot name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Career|Config")
    FString SaveSlotName = "CareerSave";

    // ============================================================
    // Career Management
    // ============================================================

    /** Start new career */
    UFUNCTION(BlueprintCallable, Category = "Career")
    void StartNewCareer(const FString& PlayerName);

    /** Load career from save */
    UFUNCTION(BlueprintCallable, Category = "Career")
    bool LoadCareer(const FString& SlotName);

    /** Save current career */
    UFUNCTION(BlueprintCallable, Category = "Career")
    bool SaveCareer();

    /** Delete career save */
    UFUNCTION(BlueprintCallable, Category = "Career")
    void DeleteCareer(const FString& SlotName);

    /** Get available career saves */
    UFUNCTION(BlueprintCallable, Category = "Career")
    TArray<FString> GetCareerSaves();

    // ============================================================
    // Progression
    // ============================================================

    /** Add experience points */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    void AddExperience(int32 XP);

    /** Add reputation */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    void AddReputation(int32 Points);

    /** Add money */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    void AddMoney(int32 Amount);

    /** Spend money */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    bool SpendMoney(int32 Amount);

    /** Check if can afford */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    bool CanAfford(int32 Cost) const;

    /** Promote to next tier */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    void PromoteToNextTier();

    /** Get XP required for next level */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    int32 GetXPRequiredForNextLevel() const;

    /** Get current level progress (0-1) */
    UFUNCTION(BlueprintCallable, Category = "Career|Progression")
    float GetLevelProgress() const;

    // ============================================================
    // Events & Races
    // ============================================================

    /** Generate events for current tier */
    UFUNCTION(BlueprintCallable, Category = "Career|Events")
    void GenerateEvents();

    /** Get available race events */
    UFUNCTION(BlueprintCallable, Category = "Career|Events")
    TArray<FRaceEvent> GetAvailableEvents() const;

    /** Get completed events */
    UFUNCTION(BlueprintCallable, Category = "Career|Events")
    TArray<FRaceEvent> GetCompletedEvents() const;

    /** Start race event */
    UFUNCTION(BlueprintCallable, Category = "Career|Events")
    void StartRaceEvent(int32 EventIndex);

    /** Complete race event */
    UFUNCTION(BlueprintCallable, Category = "Career|Events")
    void CompleteRaceEvent(int32 EventIndex, int32 FinishPosition, float RaceTime);

    /** Check if event is unlocked */
    UFUNCTION(BlueprintCallable, Category = "Career|Events")
    bool IsEventUnlocked(const FRaceEvent& Event) const;

    // ============================================================
    // Vehicles
    // ============================================================

    /** Buy vehicle */
    UFUNCTION(BlueprintCallable, Category = "Career|Vehicles")
    bool BuyVehicle(int32 VehicleID, int32 Cost);

    /** Sell vehicle */
    UFUNCTION(BlueprintCallable, Category = "Career|Vehicles")
    void SellVehicle(int32 VehicleID);

    /** Get owned vehicles */
    UFUNCTION(BlueprintCallable, Category = "Career|Vehicles")
    TArray<int32> GetOwnedVehicles() const;

    /** Set current vehicle */
    UFUNCTION(BlueprintCallable, Category = "Career|Vehicles")
    void SetCurrentVehicle(int32 VehicleID);

    /** Check if owns vehicle */
    UFUNCTION(BlueprintCallable, Category = "Career|Vehicles")
    bool OwnsVehicle(int32 VehicleID) const;

    // ============================================================
    // Rivals & AI
    // ============================================================

    /** Generate rival drivers */
    UFUNCTION(BlueprintCallable, Category = "Career|Rivals")
    void GenerateRivals();

    /** Get current rivals */
    UFUNCTION(BlueprintCallable, Category = "Career|Rivals")
    TArray<FRivalDriver> GetRivals() const;

    /** Add new rival */
    UFUNCTION(BlueprintCallable, Category = "Career|Rivals")
    void AddRival(const FString& DriverName, const FString& Reason);

    /** Update rival standings */
    UFUNCTION(BlueprintCallable, Category = "Career|Rivals")
    void UpdateRivalStandings();

    // ============================================================
    // Contracts & Teams
    // ============================================================

    /** Offer contract */
    UFUNCTION(BlueprintCallable, Category = "Career|Contracts")
    void OfferContract(EContractType ContractType, const FString& TeamName, int32 Salary);

    /** Accept contract */
    UFUNCTION(BlueprintCallable, Category = "Career|Contracts")
    void AcceptContract();

    /** Decline contract */
    UFUNCTION(BlueprintCallable, Category = "Career|Contracts")
    void DeclineContract();

    /** End current contract */
    UFUNCTION(BlueprintCallable, Category = "Career|Contracts")
    void EndContract();

    /** Get available contracts */
    UFUNCTION(BlueprintCallable, Category = "Career|Contracts")
    TArray<FString> GetAvailableContracts();

    // ============================================================
    // Statistics
    // ============================================================

    /** Record race result */
    UFUNCTION(BlueprintCallable, Category = "Career|Stats")
    void RecordRaceResult(int32 Position, float RaceTime, float Distance, bool bFastestLap, bool bPolePosition);

    /** Get career statistics */
    UFUNCTION(BlueprintCallable, Category = "Career|Stats")
    FCareerStats GetCareerStats() const;

    /** Get win rate */
    UFUNCTION(BlueprintCallable, Category = "Career|Stats")
    float GetWinRate() const;

    /** Get podium rate */
    UFUNCTION(BlueprintCallable, Category = "Career|Stats")
    float GetPodiumRate() const;

    /** Get average finish position */
    UFUNCTION(BlueprintCallable, Category = "Career|Stats")
    float GetAverageFinishPosition() const;

    // ============================================================
    // Rewards
    // ============================================================

    /** Calculate race rewards */
    UFUNCTION(BlueprintCallable, Category = "Career|Rewards")
    void CalculateRaceRewards(int32 Position, int32 BasePrize, int32& OutMoney, int32& OutXP, int32& OutReputation);

    /** Apply race rewards */
    UFUNCTION(BlueprintCallable, Category = "Career|Rewards")
    void ApplyRaceRewards(int32 Money, int32 XP, int32 Reputation);

    /** Unlock achievement */
    UFUNCTION(BlueprintCallable, Category = "Career|Rewards")
    void UnlockAchievement(const FString& AchievementID);

    // ============================================================
    // Events
    // ============================================================

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnCareerStarted(const FString& PlayerName);

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnLevelUp(int32 NewLevel);

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnTierPromoted(ECareerTier NewTier);

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnEventCompleted(const FRaceEvent& Event, int32 Position);

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnMoneyChanged(int32 NewAmount);

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnVehiclePurchased(int32 VehicleID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnRivalAdded(const FRivalDriver& Rival);

    UFUNCTION(BlueprintImplementableEvent, Category = "Career|Events")
    void OnContractOffered(EContractType ContractType, const FString& TeamName, int32 Salary);

private:
    void CalculateLevel();
    void CheckTierPromotion();
    FRaceEvent GenerateRandomEvent(ECareerTier Tier);
};
