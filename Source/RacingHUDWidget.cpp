// RacingHUDWidget.cpp
// HUD widget implementation
// Copyright 2025. All Rights Reserved.

#include "RacingHUDWidget.h"
#include "RacingVehicle.h"
#include "RacingGameMode.h"
#include "Kismet/GameplayStatics.h"

void URacingHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize settings
    bShowSpeedometer = true;
    bShowTachometer = true;
    bShowMiniMap = true;
    bShowLapTimes = true;
    bShowPosition = true;
    bShowInputs = false;
    bShowTelemetry = false;
    bUseMetricUnits = true;

    // Initialize data
    Speed = 0.0f;
    SpeedMPH = 0.0f;
    SpeedKMH = 0.0f;
    RPM = 0.0f;
    CurrentGear = 1;
    ThrottleInput = 0.0f;
    BrakeInput = 0.0f;
    SteeringInput = 0.0f;

    CurrentLap = 0;
    TotalLaps = 3;
    CurrentPosition = 1;
    TotalRacers = 1;
    CurrentLapTime = 0.0f;
    BestLapTime = 0.0f;
    LastLapTime = 0.0f;

    LateralG = 0.0f;
    LongitudinalG = 0.0f;

    CurrentLapTimeString = TEXT("0:00.000");
    BestLapTimeString = TEXT("--:--.---");
    LastLapTimeString = TEXT("--:--.---");

    // Cache game mode
    CachedGameMode = Cast<ARacingGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    UE_LOG(LogTemp, Log, TEXT("Racing HUD Widget initialized"));
}

void URacingHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Get player's vehicle
    if (!CachedVehicle)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        CachedVehicle = Cast<ARacingVehicle>(PlayerPawn);
    }

    if (CachedVehicle)
    {
        UpdateVehicleData(CachedVehicle);
    }

    UpdateRaceData();
}

// ============================================================
// FUNCTIONS
// ============================================================

void URacingHUDWidget::UpdateVehicleData(ARacingVehicle* Vehicle)
{
    if (!Vehicle)
        return;

    // Get telemetry from vehicle
    FVehicleTelemetry Telemetry = Vehicle->GetTelemetry();

    // Update speed
    Speed = Telemetry.Speed;
    SpeedKMH = Speed;
    SpeedMPH = Speed * 0.621371f;

    // Update engine data
    RPM = Telemetry.EngineRPM;
    CurrentGear = Telemetry.CurrentGear;

    // Update inputs
    ThrottleInput = Telemetry.Throttle;
    BrakeInput = Telemetry.Brake;
    SteeringInput = Telemetry.Steering;

    // Update G-forces
    LateralG = Telemetry.LateralG;
    LongitudinalG = Telemetry.LongitudinalG;
}

void URacingHUDWidget::UpdateRaceData()
{
    if (!CachedGameMode)
        return;

    if (!CachedVehicle)
        return;

    // Get racer data from game mode
    FRacerData RacerData = CachedGameMode->GetRacerData(CachedVehicle);

    // Update lap data
    CurrentLap = RacerData.CurrentLap;
    TotalLaps = CachedGameMode->TotalLaps;
    CurrentPosition = RacerData.Position;
    TotalRacers = CachedGameMode->RacerDataList.Num();

    // Update lap times
    CurrentLapTime = RacerData.CurrentLapTime;
    CurrentLapTimeString = FormatTime(CurrentLapTime);

    if (RacerData.BestLapTime < FLT_MAX && RacerData.BestLapTime > 0.0f)
    {
        BestLapTime = RacerData.BestLapTime;
        BestLapTimeString = FormatTime(BestLapTime);
    }

    // Check for lap completion
    if (RacerData.CurrentLap > 0 && CurrentLapTime < 1.0f && LastLapTime > 0.0f)
    {
        LastLapTimeString = FormatTime(LastLapTime);
        OnLapCompleted(LastLapTime);

        // Check for new best lap
        if (LastLapTime < BestLapTime || BestLapTime == 0.0f)
        {
            OnNewBestLap(LastLapTime);
        }
    }

    LastLapTime = CurrentLapTime;
}

FString URacingHUDWidget::FormatTime(float TimeInSeconds)
{
    if (TimeInSeconds <= 0.0f)
        return TEXT("0:00.000");

    int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(TimeInSeconds, 60.0f));
    int32 Milliseconds = FMath::FloorToInt(FMath::Fmod(TimeInSeconds * 1000.0f, 1000.0f));

    return FString::Printf(TEXT("%d:%02d.%03d"), Minutes, Seconds, Milliseconds);
}
