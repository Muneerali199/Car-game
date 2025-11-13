// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerGameState.generated.h"

/**
 * Multiplayer Race State
 */
UENUM(BlueprintType)
enum class EMultiplayerRaceState : uint8
{
    Lobby           UMETA(DisplayName = "Lobby - Waiting for players"),
    Countdown       UMETA(DisplayName = "Countdown - Race starting"),
    Racing          UMETA(DisplayName = "Racing - In progress"),
    Finished        UMETA(DisplayName = "Finished - Race complete"),
    Podium          UMETA(DisplayName = "Podium - Results screen")
};

/**
 * Player Race Data (Replicated)
 */
USTRUCT(BlueprintType)
struct FPlayerRaceData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly)
    int32 PlayerID = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentPosition = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentLap = 0;

    UPROPERTY(BlueprintReadOnly)
    float CurrentLapTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float BestLapTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float TotalRaceTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    bool bFinished = false;

    UPROPERTY(BlueprintReadOnly)
    int32 Ping = 0;

    UPROPERTY(BlueprintReadOnly)
    bool bIsReady = false;
};

/**
 * Multiplayer Server Settings
 */
USTRUCT(BlueprintType)
struct FMultiplayerServerSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ServerName = "Racing Server";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxPlayers = 16;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TrackName = "TestTrack";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NumberOfLaps = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAllowCollisions = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRespawnEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RespawnDelay = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bVoteToKick = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ServerPassword;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRankedMatch = false;
};

/**
 * Multiplayer Game State
 * 
 * Manages multiplayer racing game state
 * Handles player data, race progression, leaderboards
 * Network replicated for all clients
 */
UCLASS()
class CARGAME_API AMultiplayerGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AMultiplayerGameState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ============================================================
    // Race State
    // ============================================================

    /** Current multiplayer race state */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|State")
    EMultiplayerRaceState RaceState = EMultiplayerRaceState::Lobby;

    /** Time remaining in countdown */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|State")
    float CountdownTime = 0.0f;

    /** Total race time elapsed */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|State")
    float RaceTimeElapsed = 0.0f;

    /** Number of laps for this race */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|State")
    int32 TotalLaps = 3;

    /** Current track name */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|State")
    FString CurrentTrack;

    // ============================================================
    // Player Data
    // ============================================================

    /** All connected players race data */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Players")
    TArray<FPlayerRaceData> ConnectedPlayers;

    /** Number of players ready */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Players")
    int32 PlayersReady = 0;

    /** Number of players finished */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Players")
    int32 PlayersFinished = 0;

    /** Maximum players allowed */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Server")
    int32 MaxPlayers = 16;

    // ============================================================
    // Server Info
    // ============================================================

    /** Server settings */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Server")
    FMultiplayerServerSettings ServerSettings;

    /** Server name */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Server")
    FString ServerName;

    /** Is server password protected? */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Server")
    bool bPasswordProtected = false;

    // ============================================================
    // Functions
    // ============================================================

    /** Update player race data */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void UpdatePlayerData(int32 PlayerID, const FPlayerRaceData& NewData);

    /** Get player data by ID */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    FPlayerRaceData GetPlayerData(int32 PlayerID) const;

    /** Get sorted leaderboard */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    TArray<FPlayerRaceData> GetLeaderboard() const;

    /** Get player position by ID */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    int32 GetPlayerPosition(int32 PlayerID) const;

    /** Add new player */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void AddPlayer(const FPlayerRaceData& PlayerData);

    /** Remove player */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void RemovePlayer(int32 PlayerID);

    /** Set player ready state */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void SetPlayerReady(int32 PlayerID, bool bReady);

    /** Check if all players ready */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    bool AreAllPlayersReady() const;

    /** Start race countdown */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void StartCountdown();

    /** Start race */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void StartRace();

    /** End race */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void EndRace();

    /** Return to lobby */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void ReturnToLobby();
};

/**
 * Multiplayer Player State
 * 
 * Per-player state information
 * Replicated to all clients
 */
UCLASS()
class CARGAME_API AMultiplayerPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AMultiplayerPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ============================================================
    // Player Stats
    // ============================================================

    /** Player's current race data */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Stats")
    FPlayerRaceData RaceData;

    /** Total wins */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Career")
    int32 TotalWins = 0;

    /** Total races */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Career")
    int32 TotalRaces = 0;

    /** Career best lap time */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Career")
    float CareerBestLap = 999999.0f;

    /** Player level/rank */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Career")
    int32 PlayerLevel = 1;

    /** Experience points */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Career")
    int32 ExperiencePoints = 0;

    /** Player rating (ELO/MMR) */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Career")
    float PlayerRating = 1000.0f;

    // ============================================================
    // Customization
    // ============================================================

    /** Selected vehicle ID */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Vehicle")
    int32 SelectedVehicleID = 0;

    /** Vehicle customization data (as JSON string for replication) */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Multiplayer|Vehicle")
    FString VehicleCustomizationJSON;

    // ============================================================
    // Functions
    // ============================================================

    /** Update race data */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void UpdateRaceData(const FPlayerRaceData& NewData);

    /** Add win */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void AddWin();

    /** Add race completion */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void AddRaceCompletion();

    /** Update best lap */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void UpdateBestLap(float LapTime);

    /** Add experience points */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void AddExperience(int32 XP);

    /** Update player rating */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    void UpdateRating(float NewRating);

    /** Get win rate percentage */
    UFUNCTION(BlueprintCallable, Category = "Multiplayer")
    float GetWinRate() const;
};
