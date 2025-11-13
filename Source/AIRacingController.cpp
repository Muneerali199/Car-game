// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIRacingController.h"
#include "RacingVehicle.h"
#include "RaceTrackManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AAIRacingController::AAIRacingController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    bWantsPlayerState = true;
}

void AAIRacingController::BeginPlay()
{
    Super::BeginPlay();

    // Find track manager
    TrackManager = Cast<ARaceTrackManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ARaceTrackManager::StaticClass()));

    // Apply difficulty settings
    switch (Difficulty)
    {
        case EAIDifficulty::Easy:
            MaxSpeedMultiplier = 0.7f;
            MistakeChance = 0.2f;
            break;
        case EAIDifficulty::Medium:
            MaxSpeedMultiplier = 0.85f;
            MistakeChance = 0.1f;
            break;
        case EAIDifficulty::Hard:
            MaxSpeedMultiplier = 0.95f;
            MistakeChance = 0.05f;
            break;
        case EAIDifficulty::Expert:
            MaxSpeedMultiplier = 1.0f;
            MistakeChance = 0.02f;
            break;
        case EAIDifficulty::Impossible:
            MaxSpeedMultiplier = 1.1f;
            MistakeChance = 0.0f;
            break;
    }
}

void AAIRacingController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledVehicle = Cast<ARacingVehicle>(InPawn);
    
    if (ControlledVehicle)
    {
        UE_LOG(LogTemp, Log, TEXT("AI Controller possessed vehicle: %s"), *ControlledVehicle->GetName());
    }
}

void AAIRacingController::OnUnPossess()
{
    ControlledVehicle = nullptr;
    Super::OnUnPossess();
}

void AAIRacingController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!ControlledVehicle || RacelineWaypoints.Num() == 0)
    {
        return;
    }

    // Update AI racing logic
    UpdateRacingInputs(DeltaTime);

    // Apply inputs to vehicle
    ApplyInputsToVehicle();

    // Debug visualization
    if (bShowDebugInfo)
    {
        DrawDebugRaceline();
    }
}

void AAIRacingController::InitializeRacingAI(const TArray<FVector>& Waypoints)
{
    RacelineWaypoints = Waypoints;
    
    // Generate optimal speeds for each waypoint
    WaypointSpeeds.SetNum(Waypoints.Num());
    
    for (int32 i = 0; i < Waypoints.Num(); i++)
    {
        int32 NextIndex = (i + 1) % Waypoints.Num();
        int32 PrevIndex = (i - 1 + Waypoints.Num()) % Waypoints.Num();
        
        FVector ToNext = Waypoints[NextIndex] - Waypoints[i];
        FVector ToPrev = Waypoints[i] - Waypoints[PrevIndex];
        
        ToNext.Normalize();
        ToPrev.Normalize();
        
        // Calculate corner sharpness (lower dot = sharper corner)
        float CornerSharpness = FVector::DotProduct(ToNext, ToPrev);
        
        // Speed based on corner sharpness (sharper = slower)
        float BaseSpeed = 200.0f; // km/h
        float MaxSpeed = 300.0f;
        float CornerSpeed = FMath::Lerp(BaseSpeed, MaxSpeed, (CornerSharpness + 1.0f) * 0.5f);
        
        WaypointSpeeds[i] = CornerSpeed * MaxSpeedMultiplier;
    }

    UE_LOG(LogTemp, Log, TEXT("AI Racing initialized with %d waypoints"), Waypoints.Num());
}

void AAIRacingController::UpdateRacingInputs(float DeltaTime)
{
    if (!ControlledVehicle)
    {
        return;
    }

    // Update current waypoint
    FVector VehicleLocation = ControlledVehicle->GetActorLocation();
    float ClosestDistance = MAX_FLT;
    
    for (int32 i = 0; i < RacelineWaypoints.Num(); i++)
    {
        float Distance = FVector::Dist(VehicleLocation, RacelineWaypoints[i]);
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            RacingData.CurrentWaypointIndex = i;
        }
    }

    RacingData.DistanceToNextWaypoint = ClosestDistance;

    // Check for mistakes
    TimeSinceMistake += DeltaTime;
    if (!bCurrentlyMakingMistake && FMath::FRand() < MistakeChance * DeltaTime)
    {
        SimulateMistake();
    }

    // Update mistake recovery
    if (bCurrentlyMakingMistake)
    {
        MistakeDuration -= DeltaTime;
        if (MistakeDuration <= 0.0f)
        {
            bCurrentlyMakingMistake = false;
            MistakeOffset = FVector::ZeroVector;
            RacingData.CurrentState = EAIRacingState::FollowingRaceline;
        }
    }

    // Calculate steering
    RacingData.SteeringInput = CalculateSteeringInput();

    // Calculate throttle/brake
    CalculateThrottleBrake(RacingData.ThrottleInput, RacingData.BrakeInput);

    // Apply rubber-banding
    if (bEnableRubberBanding)
    {
        ApplyRubberBanding();
    }

    // Check for overtaking opportunities
    if (ShouldAttemptOvertake())
    {
        RacingData.CurrentState = EAIRacingState::Overtaking;
        RacingData.bIsOvertaking = true;
        OvertakeTimer = 3.0f; // Overtake attempt duration
    }

    // Update overtaking
    if (RacingData.bIsOvertaking)
    {
        OvertakeTimer -= DeltaTime;
        if (OvertakeTimer <= 0.0f)
        {
            RacingData.bIsOvertaking = false;
            RacingData.CurrentState = EAIRacingState::FollowingRaceline;
        }
    }
}

float AAIRacingController::CalculateSteeringInput()
{
    if (!ControlledVehicle || RacelineWaypoints.Num() == 0)
    {
        return 0.0f;
    }

    // Get target waypoint (look ahead)
    int32 LookAheadWaypoints = FMath::CeilToInt(ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() / 100.0f);
    LookAheadWaypoints = FMath::Clamp(LookAheadWaypoints, 1, 10);
    
    int32 TargetIndex = GetNextWaypointIndex(RacingData.CurrentWaypointIndex, LookAheadWaypoints);
    FVector TargetWaypoint = RacelineWaypoints[TargetIndex];

    // Apply overtaking offset
    if (RacingData.bIsOvertaking)
    {
        float Offset = GetOvertakingOffset();
        FVector Right = ControlledVehicle->GetActorRightVector();
        TargetWaypoint += Right * Offset;
    }

    // Apply mistake offset
    if (bCurrentlyMakingMistake)
    {
        TargetWaypoint += MistakeOffset;
    }

    // Calculate steering angle
    FVector VehicleLocation = ControlledVehicle->GetActorLocation();
    FVector VehicleForward = ControlledVehicle->GetActorForwardVector();
    FVector ToTarget = TargetWaypoint - VehicleLocation;
    ToTarget.Z = 0.0f;
    ToTarget.Normalize();

    // Cross product for steering direction
    float CrossProduct = FVector::CrossProduct(VehicleForward, ToTarget).Z;
    
    // Dot product for steering magnitude
    float DotProduct = FVector::DotProduct(VehicleForward, ToTarget);
    
    float SteeringAngle = FMath::Asin(FMath::Clamp(CrossProduct, -1.0f, 1.0f));
    float SteeringInput = SteeringAngle / (PI * 0.5f); // Normalize to -1 to 1

    // Apply behavior-specific steering sharpness
    SteeringInput *= SteeringSharpness * GetBehaviorSpeedMultiplier();

    return FMath::Clamp(SteeringInput, -1.0f, 1.0f);
}

void AAIRacingController::CalculateThrottleBrake(float& OutThrottle, float& OutBrake)
{
    if (!ControlledVehicle || WaypointSpeeds.Num() == 0)
    {
        OutThrottle = 0.0f;
        OutBrake = 0.0f;
        return;
    }

    // Get current and target speed
    float CurrentSpeed = ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() * 0.036f; // cm/s to km/h
    
    // Look ahead for corner speeds
    int32 LookAheadWaypoints = 3;
    float TargetSpeed = WaypointSpeeds[RacingData.CurrentWaypointIndex];
    
    for (int32 i = 1; i <= LookAheadWaypoints; i++)
    {
        int32 CheckIndex = GetNextWaypointIndex(RacingData.CurrentWaypointIndex, i);
        TargetSpeed = FMath::Min(TargetSpeed, WaypointSpeeds[CheckIndex]);
    }

    // Apply behavior multiplier
    TargetSpeed *= GetBehaviorSpeedMultiplier();

    // Calculate braking distance needed
    float SpeedDifference = CurrentSpeed - TargetSpeed;
    float BrakingDistance = (SpeedDifference > 0.0f) ? (SpeedDifference * SpeedDifference) / (2.0f * 9.8f * 1.2f) : 0.0f;
    BrakingDistance *= BrakingAggressiveness * GetBehaviorBrakingMultiplier();

    // Distance to next waypoint
    float DistanceToCorner = RacingData.DistanceToNextWaypoint * 0.01f; // cm to m

    // Decide throttle/brake
    if (CurrentSpeed > TargetSpeed && DistanceToCorner < BrakingDistance)
    {
        // Need to brake
        OutThrottle = 0.0f;
        OutBrake = FMath::Clamp((CurrentSpeed - TargetSpeed) / TargetSpeed, 0.0f, 1.0f);
        RacingData.CurrentState = EAIRacingState::Braking;
    }
    else if (CurrentSpeed < TargetSpeed * 0.95f)
    {
        // Can accelerate
        OutThrottle = FMath::Clamp((TargetSpeed - CurrentSpeed) / TargetSpeed, 0.3f, 1.0f);
        OutBrake = 0.0f;
        RacingData.CurrentState = EAIRacingState::FollowingRaceline;
    }
    else
    {
        // Maintain speed
        OutThrottle = 0.5f;
        OutBrake = 0.0f;
    }

    // Check for obstacles
    float ObstacleDistance;
    if (DetectObstaclesAhead(ObstacleDistance))
    {
        if (ObstacleDistance < 10.0f)
        {
            OutBrake = 1.0f;
            OutThrottle = 0.0f;
        }
        else if (ObstacleDistance < 30.0f)
        {
            OutThrottle *= 0.5f;
        }
    }
}

bool AAIRacingController::ShouldAttemptOvertake()
{
    if (RacingData.bIsOvertaking || bCurrentlyMakingMistake)
    {
        return false;
    }

    // Check for vehicles ahead
    float ObstacleDistance;
    if (DetectObstaclesAhead(ObstacleDistance))
    {
        float Aggression = GetOvertakingAggression();
        return (ObstacleDistance < 20.0f * Aggression);
    }

    return false;
}

float AAIRacingController::GetOvertakingOffset()
{
    float MaxOffset = 400.0f; // cm (4 meters)
    
    // Alternate sides
    bOvertakeLeft = !bOvertakeLeft;
    float Direction = bOvertakeLeft ? -1.0f : 1.0f;
    
    return Direction * MaxOffset * GetOvertakingAggression();
}

void AAIRacingController::SimulateMistake()
{
    bCurrentlyMakingMistake = true;
    MistakeDuration = FMath::RandRange(0.5f, 2.0f);
    TimeSinceMistake = 0.0f;

    // Random offset from racing line
    MistakeOffset = FVector(
        FMath::RandRange(-300.0f, 300.0f),
        FMath::RandRange(-300.0f, 300.0f),
        0.0f
    );

    RacingData.CurrentState = EAIRacingState::Recovering;
    
    UE_LOG(LogTemp, Warning, TEXT("AI made a mistake! Duration: %.2fs"), MistakeDuration);
}

void AAIRacingController::ApplyRubberBanding()
{
    // Find player vehicle
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        return;
    }

    ARacingVehicle* PlayerVehicle = Cast<ARacingVehicle>(PlayerController->GetPawn());
    if (!PlayerVehicle || !ControlledVehicle)
    {
        return;
    }

    // Calculate distance to player
    float Distance = FVector::Dist(PlayerVehicle->GetActorLocation(), ControlledVehicle->GetActorLocation());
    
    // If AI is far ahead, slow down
    // If AI is far behind, speed up
    float IdealDistance = 5000.0f; // 50 meters
    float DistanceDifference = Distance - IdealDistance;
    
    float SpeedAdjustment = (DistanceDifference / IdealDistance) * RubberBandingStrength;
    MaxSpeedMultiplier = FMath::Clamp(MaxSpeedMultiplier + SpeedAdjustment * 0.01f, 0.5f, 1.2f);
}

int32 AAIRacingController::GetNextWaypointIndex(int32 CurrentIndex, int32 LookAhead)
{
    if (RacelineWaypoints.Num() == 0)
    {
        return 0;
    }

    return (CurrentIndex + LookAhead) % RacelineWaypoints.Num();
}

float AAIRacingController::GetDistanceToWaypoint(int32 WaypointIndex)
{
    if (!ControlledVehicle || WaypointIndex >= RacelineWaypoints.Num())
    {
        return 0.0f;
    }

    return FVector::Dist(ControlledVehicle->GetActorLocation(), RacelineWaypoints[WaypointIndex]);
}

bool AAIRacingController::DetectObstaclesAhead(float& OutDistance)
{
    if (!ControlledVehicle)
    {
        return false;
    }

    FVector Start = ControlledVehicle->GetActorLocation();
    FVector Forward = ControlledVehicle->GetActorForwardVector();
    FVector End = Start + Forward * 3000.0f; // 30 meters ahead

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(ControlledVehicle);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Vehicle, QueryParams);

    if (bHit)
    {
        OutDistance = HitResult.Distance * 0.01f; // cm to m
        return true;
    }

    return false;
}

void AAIRacingController::ApplyInputsToVehicle()
{
    if (!ControlledVehicle)
    {
        return;
    }

    // Apply steering
    ControlledVehicle->SetSteeringInput(RacingData.SteeringInput);

    // Apply throttle
    ControlledVehicle->SetThrottleInput(RacingData.ThrottleInput);

    // Apply brake
    ControlledVehicle->SetBrakeInput(RacingData.BrakeInput);
}

float AAIRacingController::GetBehaviorSpeedMultiplier() const
{
    switch (RacingBehavior)
    {
        case EAIRacingBehavior::Aggressive:
            return 1.1f;
        case EAIRacingBehavior::Balanced:
            return 1.0f;
        case EAIRacingBehavior::Defensive:
            return 0.9f;
        case EAIRacingBehavior::Chaser:
            return 1.05f;
        case EAIRacingBehavior::Blocker:
            return 0.95f;
        default:
            return 1.0f;
    }
}

float AAIRacingController::GetBehaviorBrakingMultiplier() const
{
    switch (RacingBehavior)
    {
        case EAIRacingBehavior::Aggressive:
            return 0.8f; // Late braking
        case EAIRacingBehavior::Balanced:
            return 1.0f;
        case EAIRacingBehavior::Defensive:
            return 1.3f; // Early braking
        case EAIRacingBehavior::Chaser:
            return 0.9f;
        case EAIRacingBehavior::Blocker:
            return 1.1f;
        default:
            return 1.0f;
    }
}

float AAIRacingController::GetOvertakingAggression() const
{
    switch (RacingBehavior)
    {
        case EAIRacingBehavior::Aggressive:
            return 1.5f;
        case EAIRacingBehavior::Balanced:
            return 1.0f;
        case EAIRacingBehavior::Defensive:
            return 0.5f;
        case EAIRacingBehavior::Chaser:
            return 1.3f;
        case EAIRacingBehavior::Blocker:
            return 0.3f;
        default:
            return 1.0f;
    }
}

void AAIRacingController::DrawDebugRaceline()
{
    if (!GetWorld() || RacelineWaypoints.Num() < 2)
    {
        return;
    }

    // Draw waypoints
    for (int32 i = 0; i < RacelineWaypoints.Num(); i++)
    {
        int32 NextIndex = (i + 1) % RacelineWaypoints.Num();
        
        FColor LineColor = FColor::Green;
        if (i == RacingData.CurrentWaypointIndex)
        {
            LineColor = FColor::Yellow;
        }

        DrawDebugLine(GetWorld(), RacelineWaypoints[i], RacelineWaypoints[NextIndex], LineColor, false, -1.0f, 0, 5.0f);
        DrawDebugSphere(GetWorld(), RacelineWaypoints[i], 50.0f, 8, LineColor, false, -1.0f);
    }

    // Draw AI target
    if (ControlledVehicle)
    {
        int32 TargetIndex = GetNextWaypointIndex(RacingData.CurrentWaypointIndex, 3);
        DrawDebugLine(GetWorld(), ControlledVehicle->GetActorLocation(), RacelineWaypoints[TargetIndex], FColor::Red, false, -1.0f, 0, 3.0f);
    }
}
