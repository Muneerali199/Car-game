// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplaySystem.generated.h"

class ARacingVehicle;
class ARacingGameMode;

/**
 * Replay Camera Mode
 */
UENUM(BlueprintType)
enum class EReplayCameraMode : uint8
{
    FollowVehicle       UMETA(DisplayName = "Follow Vehicle - Chase camera"),
    Cockpit             UMETA(DisplayName = "Cockpit - Driver POV"),
    Orbit               UMETA(DisplayName = "Orbit - Rotate around vehicle"),
    TrackSide           UMETA(DisplayName = "Track Side - Static camera"),
    Cinematic           UMETA(DisplayName = "Cinematic - Smooth tracking"),
    Drone               UMETA(DisplayName = "Drone - Free flight"),
    TVBroadcast         UMETA(DisplayName = "TV Broadcast - Multiple angles")
};

/**
 * Vehicle Snapshot (single frame of data)
 */
USTRUCT(BlueprintType)
struct FVehicleSnapshot
{
    GENERATED_BODY()

    UPROPERTY()
    float Timestamp = 0.0f;

    UPROPERTY()
    FTransform Transform;

    UPROPERTY()
    FVector Velocity;

    UPROPERTY()
    FVector AngularVelocity;

    UPROPERTY()
    float SteeringInput = 0.0f;

    UPROPERTY()
    float ThrottleInput = 0.0f;

    UPROPERTY()
    float BrakeInput = 0.0f;

    UPROPERTY()
    float CurrentSpeed = 0.0f;

    UPROPERTY()
    float CurrentRPM = 0.0f;

    UPROPERTY()
    int32 CurrentGear = 0;

    UPROPERTY()
    TArray<FVector> WheelLocations;

    UPROPERTY()
    TArray<FRotator> WheelRotations;
};

/**
 * Complete Race Replay Data
 */
USTRUCT(BlueprintType)
struct FRaceReplayData
{
    GENERATED_BODY()

    UPROPERTY()
    FString ReplayName;

    UPROPERTY()
    FString TrackName;

    UPROPERTY()
    FDateTime RecordingDate;

    UPROPERTY()
    float TotalDuration = 0.0f;

    UPROPERTY()
    int32 TotalLaps = 0;

    UPROPERTY()
    float BestLapTime = 0.0f;

    UPROPERTY()
    TArray<float> LapTimes;

    UPROPERTY()
    TMap<int32, TArray<FVehicleSnapshot>> VehicleSnapshots; // VehicleID -> Snapshots

    UPROPERTY()
    int32 WinnerVehicleID = 0;

    UPROPERTY()
    TArray<int32> FinalPositions;
};

/**
 * Replay Playback State
 */
UENUM(BlueprintType)
enum class EReplayState : uint8
{
    Stopped         UMETA(DisplayName = "Stopped"),
    Playing         UMETA(DisplayName = "Playing"),
    Paused          UMETA(DisplayName = "Paused"),
    FastForward     UMETA(DisplayName = "Fast Forward"),
    Rewind          UMETA(DisplayName = "Rewind"),
    SlowMotion      UMETA(DisplayName = "Slow Motion")
};

/**
 * Advanced Replay System
 * 
 * Features:
 * - Record entire race sessions
 * - Multi-vehicle recording
 * - Smooth playback with interpolation
 * - Variable playback speed (slow-mo, fast-forward)
 * - Multiple camera angles
 * - Rewind capability
 * - Save/load replays to disk
 * - Highlight detection (overtakes, crashes)
 * - Export to video
 */
UCLASS()
class CARGAME_API AReplaySystem : public AActor
{
    GENERATED_BODY()

public:
    AReplaySystem();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ============================================================
    // Recording
    // ============================================================

    /** Start recording race */
    UFUNCTION(BlueprintCallable, Category = "Replay|Recording")
    void StartRecording(const FString& ReplayName);

    /** Stop recording */
    UFUNCTION(BlueprintCallable, Category = "Replay|Recording")
    void StopRecording();

    /** Is currently recording? */
    UFUNCTION(BlueprintCallable, Category = "Replay|Recording")
    bool IsRecording() const { return bIsRecording; }

    /** Record vehicle snapshot */
    UFUNCTION(BlueprintCallable, Category = "Replay|Recording")
    void RecordVehicleSnapshot(ARacingVehicle* Vehicle, int32 VehicleID);

    /** Recording sample rate (frames per second) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Config")
    float RecordingSampleRate = 60.0f;

    /** Maximum recording duration (seconds, 0 = unlimited) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Config")
    float MaxRecordingDuration = 3600.0f; // 1 hour

    // ============================================================
    // Playback
    // ============================================================

    /** Start replay playback */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void StartPlayback(const FRaceReplayData& ReplayData);

    /** Stop playback */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void StopPlayback();

    /** Pause playback */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void PausePlayback();

    /** Resume playback */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void ResumePlayback();

    /** Seek to specific time */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void SeekToTime(float TimeInSeconds);

    /** Seek to specific lap */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void SeekToLap(int32 LapNumber);

    /** Set playback speed (1.0 = normal, 0.5 = half speed, 2.0 = double speed) */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void SetPlaybackSpeed(float Speed);

    /** Skip forward/backward (seconds) */
    UFUNCTION(BlueprintCallable, Category = "Replay|Playback")
    void Skip(float Seconds);

    /** Current playback time */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|State")
    float CurrentPlaybackTime = 0.0f;

    /** Playback state */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|State")
    EReplayState PlaybackState = EReplayState::Stopped;

    /** Playback speed multiplier */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|State")
    float PlaybackSpeed = 1.0f;

    // ============================================================
    // Camera Control
    // ============================================================

    /** Set replay camera mode */
    UFUNCTION(BlueprintCallable, Category = "Replay|Camera")
    void SetCameraMode(EReplayCameraMode Mode);

    /** Cycle to next camera mode */
    UFUNCTION(BlueprintCallable, Category = "Replay|Camera")
    void CycleCamera();

    /** Focus on specific vehicle */
    UFUNCTION(BlueprintCallable, Category = "Replay|Camera")
    void FocusOnVehicle(int32 VehicleID);

    /** Enable auto camera switching (TV broadcast style) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Camera")
    bool bAutoSwitchCamera = false;

    /** Auto camera switch interval (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Camera")
    float AutoSwitchInterval = 10.0f;

    /** Current camera mode */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|Camera")
    EReplayCameraMode CurrentCameraMode = EReplayCameraMode::FollowVehicle;

    /** Currently focused vehicle */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|Camera")
    int32 FocusedVehicleID = 0;

    // ============================================================
    // Save/Load
    // ============================================================

    /** Save replay to disk */
    UFUNCTION(BlueprintCallable, Category = "Replay|Storage")
    bool SaveReplayToDisk(const FString& Filename);

    /** Load replay from disk */
    UFUNCTION(BlueprintCallable, Category = "Replay|Storage")
    FRaceReplayData LoadReplayFromDisk(const FString& Filename);

    /** Get list of saved replays */
    UFUNCTION(BlueprintCallable, Category = "Replay|Storage")
    TArray<FString> GetSavedReplays();

    /** Delete replay file */
    UFUNCTION(BlueprintCallable, Category = "Replay|Storage")
    bool DeleteReplay(const FString& Filename);

    /** Replay storage directory */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Config")
    FString ReplayDirectory = "Saved/Replays";

    // ============================================================
    // Highlights
    // ============================================================

    /** Auto-detect highlights (overtakes, crashes, close calls) */
    UFUNCTION(BlueprintCallable, Category = "Replay|Highlights")
    TArray<float> DetectHighlights();

    /** Jump to next highlight */
    UFUNCTION(BlueprintCallable, Category = "Replay|Highlights")
    void JumpToNextHighlight();

    /** Jump to previous highlight */
    UFUNCTION(BlueprintCallable, Category = "Replay|Highlights")
    void JumpToPreviousHighlight();

    /** Detected highlight timestamps */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|Highlights")
    TArray<float> HighlightTimestamps;

    /** Current highlight index */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|Highlights")
    int32 CurrentHighlightIndex = 0;

    // ============================================================
    // Ghost Racing
    // ============================================================

    /** Enable ghost vehicle (race against your replay) */
    UFUNCTION(BlueprintCallable, Category = "Replay|Ghost")
    void EnableGhostRacing(bool bEnable);

    /** Spawn ghost vehicle from replay */
    UFUNCTION(BlueprintCallable, Category = "Replay|Ghost")
    void SpawnGhostVehicle(int32 VehicleID);

    /** Is ghost racing enabled? */
    UPROPERTY(BlueprintReadOnly, Category = "Replay|Ghost")
    bool bGhostRacingEnabled = false;

    // ============================================================
    // Interpolation & Smoothing
    // ============================================================

    /** Get interpolated vehicle snapshot at specific time */
    UFUNCTION(BlueprintCallable, Category = "Replay|Interpolation")
    FVehicleSnapshot GetInterpolatedSnapshot(int32 VehicleID, float Time);

    /** Smooth camera transitions */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Config")
    bool bSmoothCameraTransitions = true;

    /** Camera transition time (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Config")
    float CameraTransitionTime = 1.0f;

    // ============================================================
    // Statistics & Analysis
    // ============================================================

    /** Get replay statistics */
    UFUNCTION(BlueprintCallable, Category = "Replay|Stats")
    void GetReplayStatistics(float& OutTotalDistance, float& OutAverageSpeed, float& OutTopSpeed);

    /** Get vehicle telemetry at time */
    UFUNCTION(BlueprintCallable, Category = "Replay|Stats")
    void GetTelemetryAtTime(int32 VehicleID, float Time, float& OutSpeed, float& OutRPM, int32& OutGear);

    /** Generate race heatmap (speed zones, braking points) */
    UFUNCTION(BlueprintCallable, Category = "Replay|Stats")
    void GenerateRaceHeatmap();

    // ============================================================
    // Events
    // ============================================================

    /** Called when recording starts */
    UFUNCTION(BlueprintImplementableEvent, Category = "Replay|Events")
    void OnRecordingStarted(const FString& ReplayName);

    /** Called when recording stops */
    UFUNCTION(BlueprintImplementableEvent, Category = "Replay|Events")
    void OnRecordingStopped();

    /** Called when playback starts */
    UFUNCTION(BlueprintImplementableEvent, Category = "Replay|Events")
    void OnPlaybackStarted();

    /** Called when playback ends */
    UFUNCTION(BlueprintImplementableEvent, Category = "Replay|Events")
    void OnPlaybackEnded();

    /** Called when highlight detected */
    UFUNCTION(BlueprintImplementableEvent, Category = "Replay|Events")
    void OnHighlightDetected(float Timestamp, const FString& HighlightType);

    // ============================================================
    // Debug
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replay|Debug")
    bool bShowDebugInfo = false;

    UFUNCTION(BlueprintCallable, Category = "Replay|Debug")
    void DrawDebugReplayInfo();

private:
    // Recording state
    bool bIsRecording = false;
    float RecordingStartTime = 0.0f;
    float TimeSinceLastSample = 0.0f;
    FRaceReplayData CurrentRecording;

    // Playback state
    bool bIsPlaying = false;
    FRaceReplayData CurrentReplay;
    TMap<int32, AActor*> SpawnedReplayVehicles;

    // Camera state
    float TimeSinceLastCameraSwitch = 0.0f;

    // Helper functions
    void UpdateRecording(float DeltaTime);
    void UpdatePlayback(float DeltaTime);
    void UpdateReplayVehicles();
    void UpdateReplayCamera();
    FVehicleSnapshot CreateVehicleSnapshot(ARacingVehicle* Vehicle);
    void SpawnReplayVehicles();
    void DestroyReplayVehicles();
    int32 FindNearestSnapshotIndex(int32 VehicleID, float Time);
    void DetectHighlightOvertake(int32 VehicleID, float Time);
    void DetectHighlightCrash(int32 VehicleID, float Time);
};
