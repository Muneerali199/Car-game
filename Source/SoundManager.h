// SoundManager.h - Advanced audio management system for racing game
// Handles dynamic music, 3D positional audio, and context-aware sound mixing

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "SoundManager.generated.h"

// Audio categories for mixing
UENUM(BlueprintType)
enum class EAudioCategory : uint8
{
	Music,
	SFX,
	VehicleEngine,
	VehicleTires,
	VehicleImpact,
	Ambient,
	UI,
	Voice
};

// Music intensity levels
UENUM(BlueprintType)
enum class EMusicIntensity : uint8
{
	Menu,       // Calm menu music
	Explore,    // Free roam ambient
	Racing,     // Active racing
	Intense,    // Close competition
	Victory,    // Winner celebration
	Defeat      // Loss theme
};

// Audio mix preset for different scenarios
UENUM(BlueprintType)
enum class EAudioMixPreset : uint8
{
	Default,
	CockpitView,      // Enhanced engine, muffled external
	ExternalView,     // Balanced all sounds
	ReplayMode,       // Cinematic mixing
	PhotoMode,        // Minimal sounds
	Multiplayer       // Voice chat priority
};

// Sound instance data
USTRUCT(BlueprintType)
struct FSoundInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UAudioComponent* AudioComponent;

	UPROPERTY(BlueprintReadWrite)
	EAudioCategory Category;

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadWrite)
	AActor* AttachedActor;

	UPROPERTY(BlueprintReadWrite)
	float StartTime;

	UPROPERTY(BlueprintReadWrite)
	float FadeInDuration;

	UPROPERTY(BlueprintReadWrite)
	float FadeOutDuration;

	UPROPERTY(BlueprintReadWrite)
	bool bIsLooping;

	UPROPERTY(BlueprintReadWrite)
	float Priority; // Higher = more important

	FSoundInstance()
		: AudioComponent(nullptr)
		, Category(EAudioCategory::SFX)
		, Location(FVector::ZeroVector)
		, AttachedActor(nullptr)
		, StartTime(0.0f)
		, FadeInDuration(0.0f)
		, FadeOutDuration(0.5f)
		, bIsLooping(false)
		, Priority(1.0f)
	{}
};

// Music track data
USTRUCT(BlueprintType)
struct FMusicTrack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TrackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* MusicSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMusicIntensity IntensityLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BPM; // Beats per minute for sync

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> BeatTimestamps; // For synchronized effects

	FMusicTrack()
		: TrackName(TEXT(""))
		, MusicSound(nullptr)
		, IntensityLevel(EMusicIntensity::Menu)
		, BPM(120.0f)
		, Duration(180.0f)
	{}
};

// Audio occlusion data
USTRUCT(BlueprintType)
struct FAudioOcclusion
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float OcclusionAmount; // 0-1, 0 = not occluded, 1 = fully blocked

	UPROPERTY(BlueprintReadWrite)
	float LowPassFilterFrequency; // Hz, lower = more muffled

	UPROPERTY(BlueprintReadWrite)
	float VolumeAttenuation; // Additional volume reduction

	FAudioOcclusion()
		: OcclusionAmount(0.0f)
		, LowPassFilterFrequency(20000.0f)
		, VolumeAttenuation(1.0f)
	{}
};

/**
 * ASoundManager - Centralized audio management system
 * Features:
 * - Dynamic music system with intensity transitions
 * - 3D positional audio with distance attenuation
 * - Audio occlusion simulation
 * - Context-aware mixing (cockpit vs external camera)
 * - Sound pooling for performance
 * - Beat-synchronized effects
 * - Voice chat integration for multiplayer
 * - Doppler effect for passing vehicles
 * - Environmental reverb zones
 */
UCLASS()
class CARGAME_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASoundManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Music control
	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Music")
	void PlayMusic(EMusicIntensity IntensityLevel, float FadeInTime = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Music")
	void StopMusic(float FadeOutTime = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Music")
	void TransitionMusicIntensity(EMusicIntensity NewIntensity, float TransitionTime = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Music")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Music")
	void PauseMusic();

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Music")
	void ResumeMusic();

	// Sound effects
	UFUNCTION(BlueprintCallable, Category = "Sound Manager|SFX")
	UAudioComponent* PlaySound2D(USoundBase* Sound, EAudioCategory Category, float Volume = 1.0f, float Pitch = 1.0f, bool bLooping = false);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|SFX")
	UAudioComponent* PlaySound3D(USoundBase* Sound, EAudioCategory Category, FVector Location, FRotator Rotation = FRotator::ZeroRotator, float Volume = 1.0f, float Pitch = 1.0f, float AttenuationDistance = 3000.0f, bool bLooping = false);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|SFX")
	UAudioComponent* PlaySoundAttached(USoundBase* Sound, EAudioCategory Category, AActor* AttachToActor, FName SocketName = NAME_None, float Volume = 1.0f, float Pitch = 1.0f, bool bLooping = false);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|SFX")
	void StopSound(UAudioComponent* AudioComponent, float FadeOutTime = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|SFX")
	void StopAllSounds(EAudioCategory Category, float FadeOutTime = 0.5f);

	// Vehicle-specific audio
	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Vehicle")
	void PlayEngineSound(AActor* Vehicle, float RPM, float Throttle, float Load);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Vehicle")
	void PlayTireSound(AActor* Vehicle, float SlipRatio, float Speed);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Vehicle")
	void PlayImpactSound(FVector Location, float ImpactForce, AActor* HitActor);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Vehicle")
	void PlayGearShiftSound(AActor* Vehicle, int32 NewGear);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Vehicle")
	void PlayTurboSound(AActor* Vehicle, float TurboSpoolUp);

	// Mix control
	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Mix")
	void SetAudioMixPreset(EAudioMixPreset Preset, float TransitionTime = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Mix")
	void SetCategoryVolume(EAudioCategory Category, float Volume);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Mix")
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Mix")
	void MuteCategory(EAudioCategory Category, bool bMute);

	// Advanced features
	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Advanced")
	void EnableDopplerEffect(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Advanced")
	void EnableAudioOcclusion(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Advanced")
	void SetListenerLocation(FVector Location, FVector Forward, FVector Up);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Advanced")
	void CreateReverbZone(FVector Center, float Radius, float ReverbAmount);

	// Beat synchronization
	UFUNCTION(BlueprintPure, Category = "Sound Manager|Music")
	float GetCurrentMusicBeat() const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager|Music")
	bool IsOnBeat(float Tolerance = 0.1f) const;

	UFUNCTION(BlueprintCallable, Category = "Sound Manager|Music")
	void RegisterBeatCallback(float BeatNumber, FName CallbackFunctionName);

	// Query
	UFUNCTION(BlueprintPure, Category = "Sound Manager|Query")
	int32 GetActiveSoundCount() const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager|Query")
	int32 GetActiveSoundCountByCategory(EAudioCategory Category) const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager|Query")
	EMusicIntensity GetCurrentMusicIntensity() const { return CurrentMusicIntensity; }

	UFUNCTION(BlueprintPure, Category = "Sound Manager|Query")
	bool IsMusicPlaying() const;

	// Singleton access
	UFUNCTION(BlueprintPure, Category = "Sound Manager", meta = (WorldContext = "WorldContextObject"))
	static ASoundManager* GetInstance(UObject* WorldContextObject);

protected:
	// Music system
	UPROPERTY(EditDefaultsOnly, Category = "Music")
	TMap<EMusicIntensity, FMusicTrack> MusicTracks;

	UPROPERTY()
	UAudioComponent* CurrentMusicComponent;

	UPROPERTY()
	UAudioComponent* CrossfadeMusicComponent; // For smooth transitions

	UPROPERTY()
	EMusicIntensity CurrentMusicIntensity;

	UPROPERTY()
	float MusicVolume;

	UPROPERTY()
	float CurrentBeatTime;

	UPROPERTY()
	int32 CurrentBeatNumber;

	// Sound pools
	UPROPERTY()
	TArray<FSoundInstance> ActiveSounds;

	UPROPERTY()
	TArray<UAudioComponent*> AudioComponentPool;

	// Mix settings
	UPROPERTY()
	EAudioMixPreset CurrentMixPreset;

	UPROPERTY()
	TMap<EAudioCategory, float> CategoryVolumes;

	UPROPERTY()
	TSet<EAudioCategory> MutedCategories;

	UPROPERTY(EditAnywhere, Category = "Mix")
	float MasterVolume;

	// Performance settings
	UPROPERTY(EditAnywhere, Category = "Performance")
	int32 MaxConcurrentSounds;

	UPROPERTY(EditAnywhere, Category = "Performance")
	int32 AudioComponentPoolSize;

	UPROPERTY(EditAnywhere, Category = "Performance")
	float MaxAudioDistance; // Sounds beyond this are not played

	// Advanced features
	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bEnableDopplerEffect;

	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bEnableAudioOcclusion;

	UPROPERTY(EditAnywhere, Category = "Advanced")
	float OcclusionUpdateRate; // How often to check occlusion (seconds)

	UPROPERTY(EditAnywhere, Category = "Advanced")
	float DopplerScale; // Intensity of Doppler effect

	// Listener data
	UPROPERTY()
	FVector ListenerLocation;

	UPROPERTY()
	FVector ListenerForward;

	UPROPERTY()
	FVector ListenerVelocity;

	// Singleton instance
	static ASoundManager* Instance;

private:
	// Internal methods
	UAudioComponent* GetOrCreateAudioComponent();
	void ReturnAudioComponentToPool(UAudioComponent* Component);
	void UpdateActiveSounds(float DeltaTime);
	void UpdateMusicBeat(float DeltaTime);
	void UpdateDopplerEffect(FSoundInstance& SoundInstance);
	FAudioOcclusion CalculateOcclusion(FVector SoundLocation) const;
	void ApplyOcclusion(UAudioComponent* AudioComponent, const FAudioOcclusion& Occlusion);
	void ApplyMixPreset();
	float GetCategoryVolume(EAudioCategory Category) const;
	bool CanPlaySound() const;
	void CleanupStoppedSounds();
	void InitializeAudioComponentPool();
	FMusicTrack* GetMusicTrackForIntensity(EMusicIntensity Intensity);
};
