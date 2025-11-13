// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AchievementSystem.generated.h"

/**
 * Achievement Category
 */
UENUM(BlueprintType)
enum class EAchievementCategory : uint8
{
    Racing          UMETA(DisplayName = "Racing - Race performance"),
    Career          UMETA(DisplayName = "Career - Progression"),
    Skill           UMETA(DisplayName = "Skill - Driving mastery"),
    Collection      UMETA(DisplayName = "Collection - Vehicles & items"),
    Social          UMETA(DisplayName = "Social - Multiplayer"),
    Special         UMETA(DisplayName = "Special - Unique challenges")
};

/**
 * Achievement Rarity
 */
UENUM(BlueprintType)
enum class EAchievementRarity : uint8
{
    Common          UMETA(DisplayName = "Common - Easy to unlock"),
    Uncommon        UMETA(DisplayName = "Uncommon - Moderate challenge"),
    Rare            UMETA(DisplayName = "Rare - Difficult"),
    Epic            UMETA(DisplayName = "Epic - Very difficult"),
    Legendary       UMETA(DisplayName = "Legendary - Extremely rare")
};

/**
 * Achievement Data
 */
USTRUCT(BlueprintType)
struct FAchievement
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString AchievementID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAchievementCategory Category = EAchievementCategory::Racing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAchievementRarity Rarity = EAchievementRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RewardXP = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RewardMoney = 1000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsSecret = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasProgress = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredProgress = 1;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentProgress = 0;

    UPROPERTY(BlueprintReadOnly)
    bool bUnlocked = false;

    UPROPERTY(BlueprintReadOnly)
    FDateTime UnlockedDate;
};

/**
 * Achievement Save Data
 */
UCLASS()
class CARGAME_API UAchievementSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FAchievement> Achievements;

    UPROPERTY()
    int32 TotalUnlocked = 0;

    UPROPERTY()
    int32 AchievementScore = 0;
};

/**
 * Achievement System
 * 
 * Tracks player accomplishments and awards
 * Features:
 * - 50+ unique achievements
 * - Progress tracking
 * - Rarity system
 * - Rewards (XP, money)
 * - Secret achievements
 * - Statistics integration
 */
UCLASS()
class CARGAME_API UAchievementSystem : public UObject
{
    GENERATED_BODY()

public:
    UAchievementSystem();

    // ============================================================
    // Achievement Management
    // ============================================================

    /** Initialize achievement system */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void Initialize();

    /** Load achievements from save */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void LoadAchievements();

    /** Save achievements */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void SaveAchievements();

    /** Get all achievements */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    TArray<FAchievement> GetAllAchievements() const { return AllAchievements; }

    /** Get unlocked achievements */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    TArray<FAchievement> GetUnlockedAchievements() const;

    /** Get locked achievements */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    TArray<FAchievement> GetLockedAchievements() const;

    /** Get achievements by category */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    TArray<FAchievement> GetAchievementsByCategory(EAchievementCategory Category) const;

    // ============================================================
    // Unlocking
    // ============================================================

    /** Unlock achievement */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    bool UnlockAchievement(const FString& AchievementID);

    /** Update achievement progress */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void UpdateAchievementProgress(const FString& AchievementID, int32 Progress);

    /** Increment achievement progress */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void IncrementAchievementProgress(const FString& AchievementID, int32 Amount = 1);

    /** Check if achievement unlocked */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    bool IsAchievementUnlocked(const FString& AchievementID) const;

    /** Get achievement progress */
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    float GetAchievementProgressPercent(const FString& AchievementID) const;

    // ============================================================
    // Statistics
    // ============================================================

    /** Get total achievements */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Stats")
    int32 GetTotalAchievements() const { return AllAchievements.Num(); }

    /** Get unlocked count */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Stats")
    int32 GetUnlockedCount() const;

    /** Get completion percentage */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Stats")
    float GetCompletionPercentage() const;

    /** Get achievement score */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Stats")
    int32 GetAchievementScore() const;

    /** Get rarest unlocked achievement */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Stats")
    FAchievement GetRarestUnlockedAchievement() const;

    // ============================================================
    // Event Tracking (Auto-unlock based on events)
    // ============================================================

    /** Track race finish */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackRaceFinish(int32 Position, int32 TotalRacers, float RaceTime);

    /** Track lap completed */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackLapCompleted(float LapTime, bool bCleanLap);

    /** Track overtake */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackOvertake();

    /** Track drift */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackDrift(float DriftTime, float DriftAngle);

    /** Track collision */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackCollision(float ImpactForce);

    /** Track distance driven */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackDistanceDriven(float Distance);

    /** Track air time */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackAirTime(float Time);

    /** Track top speed */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackTopSpeed(float Speed);

    /** Track vehicle purchased */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackVehiclePurchased(int32 VehicleID);

    /** Track upgrade installed */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackUpgradeInstalled();

    /** Track multiplayer race */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackMultiplayerRace(int32 Position, int32 Players);

    /** Track career tier promoted */
    UFUNCTION(BlueprintCallable, Category = "Achievements|Tracking")
    void TrackCareerTierPromoted(int32 TierLevel);

    // ============================================================
    // Events
    // ============================================================

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAchievementUnlocked, FAchievement, Achievement, int32, RewardXP);
    UPROPERTY(BlueprintAssignable, Category = "Achievements|Events")
    FOnAchievementUnlocked OnAchievementUnlocked;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAchievementProgress, FString, AchievementID, int32, Current, int32, Required);
    UPROPERTY(BlueprintAssignable, Category = "Achievements|Events")
    FOnAchievementProgress OnAchievementProgress;

private:
    UPROPERTY()
    TArray<FAchievement> AllAchievements;

    UPROPERTY()
    UAchievementSaveGame* SaveData;

    void CreateDefaultAchievements();
    FAchievement* FindAchievement(const FString& AchievementID);
    void CheckAutoUnlocks();

    // Achievement IDs (constants)
    static const FString ACH_FIRST_RACE;
    static const FString ACH_FIRST_WIN;
    static const FString ACH_FIRST_PODIUM;
    static const FString ACH_WIN_10_RACES;
    static const FString ACH_WIN_50_RACES;
    static const FString ACH_WIN_100_RACES;
    static const FString ACH_PERFECT_RACE;
    static const FString ACH_COMEBACK_KING;
    static const FString ACH_SPEED_DEMON;
    static const FString ACH_DRIFT_KING;
    static const FString ACH_AIR_TIME;
    static const FString ACH_CLEAN_DRIVER;
    static const FString ACH_CRASHER;
    static const FString ACH_COLLECTOR;
    static const FString ACH_UPGRADE_MASTER;
    static const FString ACH_CAREER_LEGEND;
    static const FString ACH_MULTIPLAYER_ROOKIE;
    static const FString ACH_MULTIPLAYER_PRO;
    static const FString ACH_100_OVERTAKES;
    static const FString ACH_1000_KM;
};
