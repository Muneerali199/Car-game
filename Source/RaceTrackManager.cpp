// RaceTrackManager.cpp
// Race track manager implementation
// Copyright 2025. All Rights Reserved.

#include "RaceTrackManager.h"
#include "RacingVehicle.h"
#include "RacingGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ARaceTrackManager::ARaceTrackManager()
{
    PrimaryActorTick.bCanEverTick = true;

    bAutoGenerateCheckpoints = false;
    CheckpointSpacing = 1000.0f;
    TrackName = TEXT("Unnamed Track");
    TrackLength = 0.0f;
    TotalCheckpoints = 0;

    // Create root component
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
}

void ARaceTrackManager::BeginPlay()
{
    Super::BeginPlay();

    TotalCheckpoints = Checkpoints.Num();
    CreateCheckpointColliders();

    UE_LOG(LogTemp, Log, TEXT("Race Track Manager initialized: %s with %d checkpoints"), 
        *TrackName, TotalCheckpoints);
}

void ARaceTrackManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Draw debug visualization for checkpoints
    if (GetWorld()->WorldType == EWorldType::Editor || GetWorld()->WorldType == EWorldType::PIE)
    {
        for (const FCheckpointData& Checkpoint : Checkpoints)
        {
            FVector WorldLocation = GetActorLocation() + Checkpoint.Location;
            DrawDebugBox(GetWorld(), WorldLocation, Checkpoint.BoxExtent, 
                Checkpoint.Rotation.Quaternion(), FColor::Green, false, -1.0f, 0, 10.0f);
        }
    }
}

// ============================================================
// CHECKPOINT MANAGEMENT
// ============================================================

void ARaceTrackManager::AddCheckpoint(FVector Location, FRotator Rotation, FVector BoxExtent)
{
    FCheckpointData NewCheckpoint;
    NewCheckpoint.Location = Location;
    NewCheckpoint.Rotation = Rotation;
    NewCheckpoint.BoxExtent = BoxExtent;
    NewCheckpoint.CheckpointIndex = Checkpoints.Num();

    Checkpoints.Add(NewCheckpoint);
    TotalCheckpoints = Checkpoints.Num();

    // Create collider for new checkpoint
    CreateCheckpointColliders();

    UE_LOG(LogTemp, Log, TEXT("Checkpoint added at index %d"), NewCheckpoint.CheckpointIndex);
}

void ARaceTrackManager::RemoveCheckpoint(int32 Index)
{
    if (Checkpoints.IsValidIndex(Index))
    {
        Checkpoints.RemoveAt(Index);
        TotalCheckpoints = Checkpoints.Num();

        // Recreate colliders
        CreateCheckpointColliders();

        UE_LOG(LogTemp, Log, TEXT("Checkpoint %d removed"), Index);
    }
}

void ARaceTrackManager::ClearAllCheckpoints()
{
    Checkpoints.Empty();
    TotalCheckpoints = 0;

    // Clear colliders
    for (UBoxComponent* Collider : CheckpointColliders)
    {
        if (Collider)
        {
            Collider->DestroyComponent();
        }
    }
    CheckpointColliders.Empty();

    UE_LOG(LogTemp, Log, TEXT("All checkpoints cleared"));
}

void ARaceTrackManager::GenerateCheckpointsFromSpline()
{
    // TODO: Implement spline-based checkpoint generation
    UE_LOG(LogTemp, Warning, TEXT("Spline-based generation not yet implemented"));
}

// ============================================================
// VEHICLE TRACKING
// ============================================================

void ARaceTrackManager::OnCheckpointEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                          bool bFromSweep, const FHitResult& SweepResult)
{
    ARacingVehicle* Vehicle = Cast<ARacingVehicle>(OtherActor);
    if (!Vehicle)
        return;

    // Find which checkpoint was triggered
    int32 CheckpointIndex = CheckpointColliders.Find(Cast<UBoxComponent>(OverlappedComponent));
    if (CheckpointIndex != INDEX_NONE)
    {
        HandleVehicleCheckpoint(Vehicle, CheckpointIndex);
    }
}

int32 ARaceTrackManager::GetVehicleCheckpointIndex(ARacingVehicle* Vehicle)
{
    if (VehicleCheckpoints.Contains(Vehicle))
    {
        return VehicleCheckpoints[Vehicle];
    }
    return 0;
}

float ARaceTrackManager::GetVehicleDistanceToNextCheckpoint(ARacingVehicle* Vehicle)
{
    if (!Vehicle)
        return 0.0f;

    int32 CurrentCheckpoint = GetVehicleCheckpointIndex(Vehicle);
    int32 NextCheckpoint = (CurrentCheckpoint + 1) % TotalCheckpoints;

    if (Checkpoints.IsValidIndex(NextCheckpoint))
    {
        FVector VehicleLocation = Vehicle->GetActorLocation();
        FVector CheckpointLocation = GetActorLocation() + Checkpoints[NextCheckpoint].Location;
        return FVector::Dist(VehicleLocation, CheckpointLocation);
    }

    return 0.0f;
}

// ============================================================
// LAP TIMING
// ============================================================

float ARaceTrackManager::GetCurrentLapTime(ARacingVehicle* Vehicle)
{
    if (VehicleLapStartTimes.Contains(Vehicle))
    {
        return GetWorld()->GetTimeSeconds() - VehicleLapStartTimes[Vehicle];
    }
    return 0.0f;
}

float ARaceTrackManager::GetBestLapTime(ARacingVehicle* Vehicle)
{
    if (VehicleBestLaps.Contains(Vehicle))
    {
        return VehicleBestLaps[Vehicle];
    }
    return 0.0f;
}

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

void ARaceTrackManager::CreateCheckpointColliders()
{
    // Clear existing colliders
    for (UBoxComponent* Collider : CheckpointColliders)
    {
        if (Collider)
        {
            Collider->DestroyComponent();
        }
    }
    CheckpointColliders.Empty();

    // Create new colliders
    for (int32 i = 0; i < Checkpoints.Num(); i++)
    {
        const FCheckpointData& Checkpoint = Checkpoints[i];

        FString ComponentName = FString::Printf(TEXT("Checkpoint_%d"), i);
        UBoxComponent* BoxCollider = NewObject<UBoxComponent>(this, *ComponentName);
        
        if (BoxCollider)
        {
            BoxCollider->RegisterComponent();
            BoxCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            BoxCollider->SetRelativeLocation(Checkpoint.Location);
            BoxCollider->SetRelativeRotation(Checkpoint.Rotation);
            BoxCollider->SetBoxExtent(Checkpoint.BoxExtent);
            BoxCollider->SetCollisionProfileName(TEXT("OverlapAll"));
            BoxCollider->SetGenerateOverlapEvents(true);
            
            BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ARaceTrackManager::OnCheckpointEnter);

            CheckpointColliders.Add(BoxCollider);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Created %d checkpoint colliders"), CheckpointColliders.Num());
}

void ARaceTrackManager::HandleVehicleCheckpoint(ARacingVehicle* Vehicle, int32 CheckpointIndex)
{
    // Initialize vehicle tracking if needed
    if (!VehicleCheckpoints.Contains(Vehicle))
    {
        VehicleCheckpoints.Add(Vehicle, 0);
        VehicleLapStartTimes.Add(Vehicle, GetWorld()->GetTimeSeconds());
        VehicleBestLaps.Add(Vehicle, FLT_MAX);
    }

    int32 ExpectedCheckpoint = VehicleCheckpoints[Vehicle];

    // Check if vehicle hit the correct next checkpoint
    if (CheckpointIndex == ExpectedCheckpoint)
    {
        VehicleCheckpoints[Vehicle] = (CheckpointIndex + 1) % TotalCheckpoints;
        
        OnCheckpointPassed.Broadcast(Vehicle, CheckpointIndex);

        // Notify game mode
        ARacingGameMode* GameMode = Cast<ARacingGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        if (GameMode)
        {
            GameMode->UpdateRacerCheckpoint(Vehicle, CheckpointIndex);
        }

        // Check for lap completion
        if (IsLapComplete(Vehicle, CheckpointIndex))
        {
            float LapTime = GetCurrentLapTime(Vehicle);
            
            // Update best lap
            if (LapTime < VehicleBestLaps[Vehicle])
            {
                VehicleBestLaps[Vehicle] = LapTime;
            }

            // Reset lap timer
            VehicleLapStartTimes[Vehicle] = GetWorld()->GetTimeSeconds();

            OnLapCompleted.Broadcast(Vehicle, LapTime);

            // Notify game mode
            if (GameMode)
            {
                GameMode->OnRacerCompleteLap(Vehicle, LapTime);
            }

            UE_LOG(LogTemp, Log, TEXT("%s completed lap in %.2f seconds"), 
                *Vehicle->GetName(), LapTime);
        }
    }
}

bool ARaceTrackManager::IsLapComplete(ARacingVehicle* Vehicle, int32 CheckpointIndex)
{
    // Lap is complete when vehicle passes the last checkpoint (finishes a full loop)
    return CheckpointIndex == (TotalCheckpoints - 1);
}
