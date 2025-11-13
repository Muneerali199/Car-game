// RacingGameMode.cpp
// Core game mode implementation
// Copyright 2025. All Rights Reserved.

#include "RacingGameMode.h"
#include "RacingVehicle.h"
#include "RaceTrackManager.h"
#include "Kismet/GameplayStatics.h"

ARacingGameMode::ARacingGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    // Default settings
    TotalLaps = 3;
    CountdownTime = 3.0f;
    bEnableAI = false;
    NumberOfAIRacers = 7;

    CurrentRaceState = ERaceState::Waiting;
    RaceTimer = 0.0f;
    CountdownTimer = 0.0f;
    TrackManager = nullptr;
}

void ARacingGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Find track manager
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ARaceTrackManager::StaticClass());
    TrackManager = Cast<ARaceTrackManager>(FoundActor);

    if (!TrackManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("No RaceTrackManager found in level!"));
    }

    // Find and register all racing vehicles
    TArray<AActor*> FoundVehicles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARacingVehicle::StaticClass(), FoundVehicles);
    
    for (AActor* Actor : FoundVehicles)
    {
        ARacingVehicle* Vehicle = Cast<ARacingVehicle>(Actor);
        if (Vehicle)
        {
            RegisterRacer(Vehicle);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Racing Game Mode initialized with %d racers"), RacerDataList.Num());
}

void ARacingGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    switch (CurrentRaceState)
    {
        case ERaceState::Countdown:
            UpdateCountdown(DeltaTime);
            break;

        case ERaceState::Racing:
            UpdateRaceTimer(DeltaTime);
            UpdateRacerPositions();
            CheckRaceCompletion();
            break;

        default:
            break;
    }
}

// ============================================================
// RACE CONTROL
// ============================================================

void ARacingGameMode::StartRace()
{
    if (CurrentRaceState != ERaceState::Waiting)
        return;

    CurrentRaceState = ERaceState::Countdown;
    CountdownTimer = CountdownTime;
    OnRaceStateChanged.Broadcast(CurrentRaceState);

    UE_LOG(LogTemp, Log, TEXT("Race countdown started"));
}

void ARacingGameMode::EndRace()
{
    CurrentRaceState = ERaceState::Finished;
    OnRaceStateChanged.Broadcast(CurrentRaceState);

    // Determine winner
    if (RacerDataList.Num() > 0)
    {
        UpdateRacerPositions();
        FRacerData WinnerData = RacerDataList[0];
        OnRaceFinished.Broadcast(WinnerData.Vehicle);
        
        UE_LOG(LogTemp, Log, TEXT("Race finished! Winner: %s"), 
            WinnerData.Vehicle ? *WinnerData.Vehicle->GetName() : TEXT("Unknown"));
    }
}

void ARacingGameMode::PauseRace()
{
    if (CurrentRaceState == ERaceState::Racing)
    {
        CurrentRaceState = ERaceState::Paused;
        OnRaceStateChanged.Broadcast(CurrentRaceState);
        UGameplayStatics::SetGamePaused(GetWorld(), true);
    }
}

void ARacingGameMode::ResumeRace()
{
    if (CurrentRaceState == ERaceState::Paused)
    {
        CurrentRaceState = ERaceState::Racing;
        OnRaceStateChanged.Broadcast(CurrentRaceState);
        UGameplayStatics::SetGamePaused(GetWorld(), false);
    }
}

void ARacingGameMode::RestartRace()
{
    // Reset all racer data
    for (FRacerData& Data : RacerDataList)
    {
        Data.CurrentLap = 0;
        Data.CurrentLapTime = 0.0f;
        Data.TotalRaceTime = 0.0f;
        Data.CheckpointsHit = 0;
        Data.Position = 0;
    }

    RaceTimer = 0.0f;
    CurrentRaceState = ERaceState::Waiting;
    OnRaceStateChanged.Broadcast(CurrentRaceState);

    UE_LOG(LogTemp, Log, TEXT("Race restarted"));
}

// ============================================================
// RACER TRACKING
// ============================================================

void ARacingGameMode::RegisterRacer(ARacingVehicle* Racer)
{
    if (!Racer)
        return;

    FRacerData NewData;
    NewData.Vehicle = Racer;
    NewData.Position = RacerDataList.Num() + 1;
    NewData.BestLapTime = FLT_MAX;

    RacerDataList.Add(NewData);

    UE_LOG(LogTemp, Log, TEXT("Registered racer: %s"), *Racer->GetName());
}

void ARacingGameMode::UpdateRacerCheckpoint(ARacingVehicle* Racer, int32 CheckpointIndex)
{
    for (FRacerData& Data : RacerDataList)
    {
        if (Data.Vehicle == Racer)
        {
            Data.CheckpointsHit = CheckpointIndex;
            break;
        }
    }
}

void ARacingGameMode::OnRacerCompleteLap(ARacingVehicle* Racer, float LapTime)
{
    for (FRacerData& Data : RacerDataList)
    {
        if (Data.Vehicle == Racer)
        {
            Data.CurrentLap++;
            Data.CurrentLapTime = 0.0f;
            Data.CheckpointsHit = 0;

            // Update best lap time
            if (LapTime < Data.BestLapTime)
            {
                Data.BestLapTime = LapTime;
            }

            OnLapCompleted.Broadcast(Racer, LapTime);

            UE_LOG(LogTemp, Log, TEXT("%s completed lap %d in %.2f seconds"), 
                *Racer->GetName(), Data.CurrentLap, LapTime);

            break;
        }
    }
}

void ARacingGameMode::UpdateRacerPositions()
{
    // Sort racers by lap and checkpoint progress
    RacerDataList.Sort([](const FRacerData& A, const FRacerData& B)
    {
        if (A.CurrentLap != B.CurrentLap)
            return A.CurrentLap > B.CurrentLap;
        
        return A.CheckpointsHit > B.CheckpointsHit;
    });

    // Update positions
    for (int32 i = 0; i < RacerDataList.Num(); i++)
    {
        RacerDataList[i].Position = i + 1;
    }
}

FRacerData ARacingGameMode::GetRacerData(ARacingVehicle* Racer)
{
    for (const FRacerData& Data : RacerDataList)
    {
        if (Data.Vehicle == Racer)
        {
            return Data;
        }
    }
    return FRacerData();
}

TArray<FRacerData> ARacingGameMode::GetLeaderboard()
{
    return RacerDataList;
}

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

void ARacingGameMode::UpdateCountdown(float DeltaTime)
{
    CountdownTimer -= DeltaTime;

    if (CountdownTimer <= 0.0f)
    {
        CurrentRaceState = ERaceState::Racing;
        OnRaceStateChanged.Broadcast(CurrentRaceState);
        UE_LOG(LogTemp, Log, TEXT("Race started!"));
    }
}

void ARacingGameMode::UpdateRaceTimer(float DeltaTime)
{
    RaceTimer += DeltaTime;

    // Update individual racer times
    for (FRacerData& Data : RacerDataList)
    {
        Data.TotalRaceTime += DeltaTime;
        Data.CurrentLapTime += DeltaTime;
    }
}

void ARacingGameMode::CheckRaceCompletion()
{
    for (const FRacerData& Data : RacerDataList)
    {
        if (Data.CurrentLap >= TotalLaps)
        {
            EndRace();
            break;
        }
    }
}
