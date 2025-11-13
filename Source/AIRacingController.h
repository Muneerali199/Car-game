// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIRacingController.generated.h"

class ARacingVehicle;
class ARaceTrackManager;

/**
 * AI Racing Behavior Types
 */
UENUM(BlueprintType)
enum class EAIRacingBehavior : uint8
{
    Aggressive      UMETA(DisplayName = "Aggressive - Takes risks, late braking"),
    Balanced        UMETA(DisplayName = "Balanced - Smooth and consistent"),
    Defensive       UMETA(DisplayName = "Defensive - Safe driving, early braking"),
    Chaser          UMETA(DisplayName = "Chaser - Follows player closely"),
    Blocker         UMETA(DisplayName = "Blocker - Blocks overtaking attempts")
};

/**
 * AI Difficulty Settings
 */
UENUM(BlueprintType)
enum class EAIDifficulty : uint8
{
    Easy            UMETA(DisplayName = "Easy - 70% speed, makes mistakes"),
    Medium          UMETA(DisplayName = "Medium - 85% speed, occasional mistakes"),
    Hard            UMETA(DisplayName = "Hard - 95% speed, rare mistakes"),
    Expert          UMETA(DisplayName = "Expert - 100% speed, no mistakes"),
    Impossible      UMETA(DisplayName = "Impossible - 110% speed, perfect driving")
};

/**
 * AI Racing State
 */
UENUM(BlueprintType)
enum class EAIRacingState : uint8
{
    FollowingRaceline   UMETA(DisplayName = "Following Racing Line"),
    Overtaking          UMETA(DisplayName = "Attempting Overtake"),
    Defending           UMETA(DisplayName = "Defending Position"),
    Recovering          UMETA(DisplayName = "Recovering from Error"),
    Braking             UMETA(DisplayName = "Braking for Corner")
};

/**
 * AI Racing Data
 */
USTRUCT(BlueprintType)
struct FAIRacingData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    EAIRacingState CurrentState = EAIRacingState::FollowingRaceline;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    float TargetSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    float SteeringInput = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    float ThrottleInput = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    float BrakeInput = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    float DistanceToNextWaypoint = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    float LateralOffsetFromRaceline = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    int32 CurrentWaypointIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    bool bIsOvertaking = false;

    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    bool bIsDefending = false;
};

/**
 * Advanced AI Controller for Racing Opponents
 * 
 * Features:
 * - Multiple racing behaviors (aggressive, defensive, balanced)
 * - Dynamic difficulty adjustment
 * - Realistic racing line following
 * - Overtaking and defensive maneuvers
 * - Mistake simulation for realism
 * - Rubber-banding (optional)
 */
UCLASS()
class CARGAME_API AAIRacingController : public AAIController
{
    GENERATED_BODY()

public:
    AAIRacingController();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // AI Configuration
    // ============================================================

    /** AI racing behavior type */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config")
    EAIRacingBehavior RacingBehavior = EAIRacingBehavior::Balanced;

    /** AI difficulty level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config")
    EAIDifficulty Difficulty = EAIDifficulty::Medium;

    /** Maximum speed multiplier (1.0 = 100% of vehicle max speed) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config", meta = (ClampMin = "0.5", ClampMax = "1.5"))
    float MaxSpeedMultiplier = 0.85f;

    /** How aggressively AI brakes (higher = earlier braking) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config", meta = (ClampMin = "0.5", ClampMax = "2.0"))
    float BrakingAggressiveness = 1.0f;

    /** How sharply AI steers (higher = more responsive) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config", meta = (ClampMin = "0.5", ClampMax = "2.0"))
    float SteeringSharpness = 1.0f;

    /** Enable rubber-banding (AI adjusts speed based on player position) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config")
    bool bEnableRubberBanding = false;

    /** Rubber-banding strength (0.0 = none, 1.0 = maximum) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RubberBandingStrength = 0.3f;

    /** Chance of making a mistake per second (0.0 = never, 1.0 = constantly) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config", meta = (ClampMin = "0.0", ClampMax = "0.5"))
    float MistakeChance = 0.05f;

    /** Look-ahead distance for racing line (meters) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Config", meta = (ClampMin = "10.0", ClampMax = "200.0"))
    float LookAheadDistance = 50.0f;

    // ============================================================
    // Racing Line
    // ============================================================

    /** Racing line waypoints (set by RaceTrackManager) */
    UPROPERTY(BlueprintReadWrite, Category = "AI Racing|Waypoints")
    TArray<FVector> RacelineWaypoints;

    /** Corner speeds for each waypoint */
    UPROPERTY(BlueprintReadWrite, Category = "AI Racing|Waypoints")
    TArray<float> WaypointSpeeds;

    // ============================================================
    // AI State
    // ============================================================

    /** Current AI racing data */
    UPROPERTY(BlueprintReadOnly, Category = "AI Racing|State")
    FAIRacingData RacingData;

    /** Reference to controlled vehicle */
    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    ARacingVehicle* ControlledVehicle;

    /** Reference to track manager */
    UPROPERTY(BlueprintReadOnly, Category = "AI Racing")
    ARaceTrackManager* TrackManager;

    // ============================================================
    // AI Logic Functions
    // ============================================================

    /** Initialize AI with waypoints from track */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    void InitializeRacingAI(const TArray<FVector>& Waypoints);

    /** Update AI inputs based on racing logic */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    void UpdateRacingInputs(float DeltaTime);

    /** Calculate steering input to follow racing line */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    float CalculateSteeringInput();

    /** Calculate throttle/brake inputs based on speed and corners */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    void CalculateThrottleBrake(float& OutThrottle, float& OutBrake);

    /** Check if AI should attempt overtake */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    bool ShouldAttemptOvertake();

    /** Calculate overtaking path offset */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    float GetOvertakingOffset();

    /** Simulate AI mistake (off-line, late braking, etc.) */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    void SimulateMistake();

    /** Apply rubber-banding adjustment */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    void ApplyRubberBanding();

    /** Get next waypoint index */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    int32 GetNextWaypointIndex(int32 CurrentIndex, int32 LookAhead = 1);

    /** Get distance to waypoint */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    float GetDistanceToWaypoint(int32 WaypointIndex);

    /** Detect obstacles ahead */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    bool DetectObstaclesAhead(float& OutDistance);

    /** Apply AI inputs to vehicle */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    void ApplyInputsToVehicle();

    // ============================================================
    // Behavior-Specific Functions
    // ============================================================

    /** Get behavior-specific speed multiplier */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    float GetBehaviorSpeedMultiplier() const;

    /** Get behavior-specific braking distance multiplier */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    float GetBehaviorBrakingMultiplier() const;

    /** Get behavior-specific overtaking aggression */
    UFUNCTION(BlueprintCallable, Category = "AI Racing")
    float GetOvertakingAggression() const;

    // ============================================================
    // Debug
    // ============================================================

    /** Enable debug visualization */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Racing|Debug")
    bool bShowDebugInfo = false;

    /** Draw debug racing line */
    UFUNCTION(BlueprintCallable, Category = "AI Racing|Debug")
    void DrawDebugRaceline();

private:
    // Internal state
    float TimeSinceMistake = 0.0f;
    float MistakeDuration = 0.0f;
    FVector MistakeOffset = FVector::ZeroVector;
    bool bCurrentlyMakingMistake = false;

    // Overtaking state
    float OvertakeTimer = 0.0f;
    bool bOvertakeLeft = true;
};
