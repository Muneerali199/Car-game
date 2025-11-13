// ParticleEffectsManager.h - Advanced particle effects system for racing game
// Manages all particle effects with pooling, LOD, and performance optimization

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "ParticleEffectsManager.generated.h"

// Effect types for categorization
UENUM(BlueprintType)
enum class EEffectType : uint8
{
	TireSmoke,
	Sparks,
	Dust,
	WaterSplash,
	Explosion,
	Nitrous,
	Impact,
	SkidMark,
	EngineFlame,
	BoostTrail,
	RainDrops,
	SnowFlakes,
	DamageSmoke,
	GlassShatter
};

// Effect quality levels for performance scaling
UENUM(BlueprintType)
enum class EEffectQuality : uint8
{
	Low,      // Minimal particles, simple effects
	Medium,   // Balanced particle count
	High,     // Full detail
	Ultra     // Maximum particles, all features
};

// Surface type for effect variation
UENUM(BlueprintType)
enum class ESurfaceType : uint8
{
	Asphalt,
	Concrete,
	Dirt,
	Grass,
	Gravel,
	Sand,
	Snow,
	Ice,
	Water,
	Metal,
	Wood
};

// Particle effect instance data
USTRUCT(BlueprintType)
struct FParticleEffectData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EEffectType EffectType;

	UPROPERTY(BlueprintReadWrite)
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite)
	float SpawnTime;

	UPROPERTY(BlueprintReadWrite)
	float Lifetime;

	UPROPERTY(BlueprintReadWrite)
	bool bIsActive;

	UPROPERTY(BlueprintReadWrite)
	int32 Priority; // Higher = more important

	FParticleEffectData()
		: EffectType(EEffectType::TireSmoke)
		, NiagaraComponent(nullptr)
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
		, SpawnTime(0.0f)
		, Lifetime(2.0f)
		, bIsActive(false)
		, Priority(0)
	{}
};

// Effect pool for performance
USTRUCT(BlueprintType)
struct FEffectPool
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FParticleEffectData> AvailableEffects;

	UPROPERTY(BlueprintReadWrite)
	TArray<FParticleEffectData> ActiveEffects;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPoolSize;

	FEffectPool()
		: MaxPoolSize(50)
	{}
};

// Effect template configuration
USTRUCT(BlueprintType)
struct FEffectTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultLifetime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpawnDistance; // Don't spawn beyond this distance

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAttachToVehicle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinTimeBetweenSpawns; // Throttle spawning

	FEffectTemplate()
		: EffectType(EEffectType::TireSmoke)
		, NiagaraSystem(nullptr)
		, DefaultLifetime(2.0f)
		, Priority(1)
		, MaxSpawnDistance(5000.0f)
		, bAttachToVehicle(false)
		, AttachSocketName(NAME_None)
		, MinTimeBetweenSpawns(0.1f)
	{}
};

/**
 * AParticleEffectsManager - Centralized particle effects management system
 * Features:
 * - Object pooling for performance
 * - LOD based on distance from camera
 * - Quality scaling for different hardware
 * - Surface-specific effect variations
 * - Automatic cleanup and recycling
 * - Priority-based spawning when at limit
 */
UCLASS()
class CARGAME_API AParticleEffectsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AParticleEffectsManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Effect spawning
	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	UNiagaraComponent* SpawnEffect(EEffectType EffectType, FVector Location, FRotator Rotation, AActor* AttachTo = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	UNiagaraComponent* SpawnTireSmoke(FVector Location, FRotator Rotation, float SlipRatio, ESurfaceType SurfaceType);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	UNiagaraComponent* SpawnSparks(FVector Location, FVector Normal, float Intensity);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	UNiagaraComponent* SpawnDust(FVector Location, float VehicleSpeed, ESurfaceType SurfaceType);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	UNiagaraComponent* SpawnWaterSplash(FVector Location, float VehicleSpeed, float WaterDepth);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	UNiagaraComponent* SpawnExplosion(FVector Location, float Magnitude);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	UNiagaraComponent* SpawnNitrousEffect(FVector Location, FRotator Rotation, AActor* Vehicle);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	void SpawnSkidMark(FVector StartLocation, FVector EndLocation, float Width, float Darkness);

	// Effect management
	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	void StopEffect(UNiagaraComponent* EffectComponent);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	void StopAllEffects();

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	void StopEffectsByType(EEffectType EffectType);

	// Configuration
	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	void SetEffectQuality(EEffectQuality Quality);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	void SetMaxActiveEffects(int32 MaxEffects);

	UFUNCTION(BlueprintCallable, Category = "Particle Effects")
	void EnableEffectType(EEffectType EffectType, bool bEnable);

	// Query
	UFUNCTION(BlueprintPure, Category = "Particle Effects")
	int32 GetActiveEffectCount() const;

	UFUNCTION(BlueprintPure, Category = "Particle Effects")
	int32 GetActiveEffectCountByType(EEffectType EffectType) const;

	UFUNCTION(BlueprintPure, Category = "Particle Effects")
	float GetEffectBudgetUsage() const; // Returns 0-1 ratio of effects used

	// Singleton access
	UFUNCTION(BlueprintPure, Category = "Particle Effects", meta = (WorldContext = "WorldContextObject"))
	static AParticleEffectsManager* GetInstance(UObject* WorldContextObject);

protected:
	// Effect templates (configured in Blueprint)
	UPROPERTY(EditDefaultsOnly, Category = "Effect Templates")
	TMap<EEffectType, FEffectTemplate> EffectTemplates;

	// Surface-specific parameters
	UPROPERTY(EditDefaultsOnly, Category = "Surface Effects")
	TMap<ESurfaceType, FLinearColor> SurfaceDustColors;

	UPROPERTY(EditDefaultsOnly, Category = "Surface Effects")
	TMap<ESurfaceType, float> SurfaceParticleSizes;

	// Performance settings
	UPROPERTY(EditAnywhere, Category = "Performance")
	EEffectQuality CurrentQuality;

	UPROPERTY(EditAnywhere, Category = "Performance")
	int32 MaxActiveEffects;

	UPROPERTY(EditAnywhere, Category = "Performance")
	float MaxEffectDistance; // Effects beyond this distance are not spawned

	UPROPERTY(EditAnywhere, Category = "Performance")
	float LODDistance1; // Distance for LOD level 1

	UPROPERTY(EditAnywhere, Category = "Performance")
	float LODDistance2; // Distance for LOD level 2

	UPROPERTY(EditAnywhere, Category = "Performance")
	bool bUseObjectPooling;

	// Effect pools per type
	UPROPERTY()
	TMap<EEffectType, FEffectPool> EffectPools;

	// Active effects tracking
	UPROPERTY()
	TArray<FParticleEffectData> ActiveEffects;

	// Disabled effect types
	UPROPERTY()
	TSet<EEffectType> DisabledEffectTypes;

	// Spawn throttling
	UPROPERTY()
	TMap<EEffectType, float> LastSpawnTimes;

	// Singleton instance
	static AParticleEffectsManager* Instance;

private:
	// Internal methods
	UNiagaraComponent* GetOrCreateEffect(EEffectType EffectType);
	void ReturnEffectToPool(FParticleEffectData& EffectData);
	void UpdateActiveEffects(float DeltaTime);
	void ApplyLOD(FParticleEffectData& EffectData, float DistanceToCamera);
	void CleanupExpiredEffects();
	bool CanSpawnEffect(EEffectType EffectType) const;
	FEffectTemplate* GetEffectTemplate(EEffectType EffectType);
	float GetDistanceToCamera(FVector Location) const;
	void InitializeEffectPools();
	void ApplyQualitySettings();
	int32 CalculateParticleCountForQuality(int32 BaseCount) const;
	FLinearColor GetSurfaceColor(ESurfaceType SurfaceType) const;
	float GetSurfaceParticleSize(ESurfaceType SurfaceType) const;
};
