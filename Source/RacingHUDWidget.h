// RacingHUDWidget.h
// Racing HUD with speedometer, tachometer, and lap times
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RacingHUDWidget.generated.h"

class ARacingVehicle;
class ARacingGameMode;
struct FVehicleTelemetry;

/**
 * Main HUD widget for racing game
 * Shows speed, RPM, gear, lap times, position, etc.
 */
UCLASS()
class CARGAME_API URacingHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // ============================================================
    // VEHICLE DATA (Update these in Blueprint)
    // ============================================================

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    float SpeedMPH;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    float SpeedKMH;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    float RPM;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    int32 CurrentGear;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    float ThrottleInput;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    float BrakeInput;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Data")
    float SteeringInput;

    // ============================================================
    // RACE DATA
    // ============================================================

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    int32 CurrentLap;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    int32 TotalLaps;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    int32 CurrentPosition;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    int32 TotalRacers;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    FString CurrentLapTimeString;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    FString BestLapTimeString;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    FString LastLapTimeString;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    float CurrentLapTime;

    UPROPERTY(BlueprintReadOnly, Category = "Race Data")
    float BestLapTime;

    // ============================================================
    // G-FORCES
    // ============================================================

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float LateralG;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float LongitudinalG;

    // ============================================================
    // UI SETTINGS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bShowSpeedometer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bShowTachometer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bShowMiniMap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bShowLapTimes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bShowPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bShowInputs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bShowTelemetry;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Settings")
    bool bUseMetricUnits;

    // ============================================================
    // FUNCTIONS
    // ============================================================

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateVehicleData(ARacingVehicle* Vehicle);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateRaceData();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    FString FormatTime(float TimeInSeconds);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void OnLapCompleted(float LapTime);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void OnNewBestLap(float LapTime);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void OnPositionChanged(int32 NewPosition);

private:
    UPROPERTY()
    ARacingVehicle* CachedVehicle;

    UPROPERTY()
    ARacingGameMode* CachedGameMode;

    float LastLapTime;
};
