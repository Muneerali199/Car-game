// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleDamageComponent.generated.h"

class ARacingVehicle;
class UNiagaraComponent;

/**
 * Damage Types
 */
UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Collision       UMETA(DisplayName = "Collision - Hit wall/vehicle"),
    BottomOut       UMETA(DisplayName = "Bottom Out - Suspension damage"),
    OverRevving     UMETA(DisplayName = "Over-Revving - Engine damage"),
    Overheating     UMETA(DisplayName = "Overheating - Engine heat damage"),
    TireWear        UMETA(DisplayName = "Tire Wear - Grip loss over time")
};

/**
 * Vehicle Part Types
 */
UENUM(BlueprintType)
enum class EVehiclePart : uint8
{
    Engine          UMETA(DisplayName = "Engine"),
    Transmission    UMETA(DisplayName = "Transmission"),
    Suspension      UMETA(DisplayName = "Suspension"),
    Brakes          UMETA(DisplayName = "Brakes"),
    Tires           UMETA(DisplayName = "Tires"),
    Aerodynamics    UMETA(DisplayName = "Aerodynamics (body damage)"),
    Fuel            UMETA(DisplayName = "Fuel System")
};

/**
 * Part Damage Data
 */
USTRUCT(BlueprintType)
struct FPartDamage
{
    GENERATED_BODY()

    /** Part identifier */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    EVehiclePart Part = EVehiclePart::Engine;

    /** Damage amount (0 = perfect, 1 = destroyed) */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    float DamageAmount = 0.0f;

    /** Performance multiplier due to damage (1.0 = normal) */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    float PerformanceMultiplier = 1.0f;

    /** Is this part completely broken? */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    bool bIsBroken = false;
};

/**
 * Collision Data
 */
USTRUCT(BlueprintType)
struct FCollisionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Collision")
    FVector ImpactLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Collision")
    FVector ImpactNormal = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Collision")
    float ImpactForce = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Collision")
    float Timestamp = 0.0f;
};

/**
 * Advanced Vehicle Damage System
 * 
 * Features:
 * - Part-specific damage (engine, suspension, tires, etc.)
 * - Visual damage (deformation, scratches, broken parts)
 * - Performance degradation
 * - Repair system (pits)
 * - Realistic damage physics
 * - Smoke/fire effects for severe damage
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGAME_API UVehicleDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleDamageComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ============================================================
    // Damage Configuration
    // ============================================================

    /** Enable realistic damage system */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
    bool bEnableDamage = true;

    /** Enable visual damage (deformation) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
    bool bEnableVisualDamage = true;

    /** Damage multiplier (higher = more fragile) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float DamageMultiplier = 1.0f;

    /** Minimum impact force to cause damage (Newtons) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
    float MinImpactForce = 50000.0f;

    /** Enable tire wear over time */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
    bool bEnableTireWear = true;

    /** Tire wear rate (per second at max speed) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config", meta = (ClampMin = "0.0", ClampMax = "0.01"))
    float TireWearRate = 0.001f;

    /** Enable engine damage from over-revving */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
    bool bEnableEngineDamage = true;

    /** Maximum RPM before damage occurs */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
    float MaxSafeRPM = 8500.0f;

    // ============================================================
    // Part Damage State
    // ============================================================

    /** Damage state for each vehicle part */
    UPROPERTY(BlueprintReadOnly, Category = "Damage|State")
    TMap<EVehiclePart, FPartDamage> PartDamages;

    /** Total vehicle health (0-100%) */
    UPROPERTY(BlueprintReadOnly, Category = "Damage|State")
    float OverallHealth = 100.0f;

    /** Is vehicle totaled (too damaged to continue)? */
    UPROPERTY(BlueprintReadOnly, Category = "Damage|State")
    bool bIsTotaled = false;

    /** Recent collision history (for replay/analysis) */
    UPROPERTY(BlueprintReadOnly, Category = "Damage|State")
    TArray<FCollisionData> CollisionHistory;

    // ============================================================
    // Damage Application
    // ============================================================

    /** Apply collision damage */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void ApplyCollisionDamage(const FVector& ImpactLocation, const FVector& ImpactNormal, float ImpactForce);

    /** Apply damage to specific part */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void ApplyPartDamage(EVehiclePart Part, float DamageAmount, EDamageType DamageType);

    /** Apply tire wear */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void ApplyTireWear(float DeltaTime, float Speed, bool bIsDrifting);

    /** Apply engine damage from over-revving */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void ApplyEngineDamage(float CurrentRPM);

    /** Apply suspension damage from bottoming out */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void ApplySuspensionDamage(float CompressionForce);

    // ============================================================
    // Repair System
    // ============================================================

    /** Repair specific part */
    UFUNCTION(BlueprintCallable, Category = "Damage|Repair")
    void RepairPart(EVehiclePart Part, float RepairAmount = 1.0f);

    /** Repair all parts completely */
    UFUNCTION(BlueprintCallable, Category = "Damage|Repair")
    void RepairAll();

    /** Replace tires (pit stop) */
    UFUNCTION(BlueprintCallable, Category = "Damage|Repair")
    void ReplaceTires();

    /** Get repair cost for part */
    UFUNCTION(BlueprintCallable, Category = "Damage|Repair")
    float GetRepairCost(EVehiclePart Part);

    /** Get repair time estimate (seconds) */
    UFUNCTION(BlueprintCallable, Category = "Damage|Repair")
    float GetRepairTime(EVehiclePart Part);

    // ============================================================
    // Performance Impact
    // ============================================================

    /** Get current engine power multiplier */
    UFUNCTION(BlueprintCallable, Category = "Damage|Performance")
    float GetEnginePowerMultiplier() const;

    /** Get current top speed multiplier */
    UFUNCTION(BlueprintCallable, Category = "Damage|Performance")
    float GetTopSpeedMultiplier() const;

    /** Get current grip multiplier */
    UFUNCTION(BlueprintCallable, Category = "Damage|Performance")
    float GetGripMultiplier() const;

    /** Get current brake force multiplier */
    UFUNCTION(BlueprintCallable, Category = "Damage|Performance")
    float GetBrakeForceMultiplier() const;

    /** Get current aerodynamic drag multiplier */
    UFUNCTION(BlueprintCallable, Category = "Damage|Performance")
    float GetDragMultiplier() const;

    /** Get part damage amount */
    UFUNCTION(BlueprintCallable, Category = "Damage|State")
    float GetPartDamage(EVehiclePart Part) const;

    /** Is part broken? */
    UFUNCTION(BlueprintCallable, Category = "Damage|State")
    bool IsPartBroken(EVehiclePart Part) const;

    // ============================================================
    // Visual Damage
    // ============================================================

    /** Apply visual deformation to mesh */
    UFUNCTION(BlueprintCallable, Category = "Damage|Visual")
    void ApplyVisualDamage(const FVector& ImpactLocation, float Severity);

    /** Add scratches/paint damage */
    UFUNCTION(BlueprintCallable, Category = "Damage|Visual")
    void ApplyPaintDamage(const FVector& Location, float Radius);

    /** Detach broken parts (bumpers, mirrors, etc.) */
    UFUNCTION(BlueprintCallable, Category = "Damage|Visual")
    void DetachBrokenPart(FName PartName);

    // ============================================================
    // Damage Effects
    // ============================================================

    /** Smoke from engine damage */
    UPROPERTY(BlueprintReadWrite, Category = "Damage|VFX")
    UNiagaraComponent* EngineSmokeEffect;

    /** Fire effect for severe damage */
    UPROPERTY(BlueprintReadWrite, Category = "Damage|VFX")
    UNiagaraComponent* FireEffect;

    /** Oil/fluid leaks */
    UPROPERTY(BlueprintReadWrite, Category = "Damage|VFX")
    UNiagaraComponent* FluidLeakEffect;

    /** Update damage visual effects */
    UFUNCTION(BlueprintCallable, Category = "Damage|VFX")
    void UpdateDamageEffects();

    // ============================================================
    // Events
    // ============================================================

    /** Called when vehicle takes damage */
    UFUNCTION(BlueprintImplementableEvent, Category = "Damage|Events")
    void OnVehicleDamaged(EVehiclePart Part, float DamageAmount, EDamageType DamageType);

    /** Called when part breaks */
    UFUNCTION(BlueprintImplementableEvent, Category = "Damage|Events")
    void OnPartBroken(EVehiclePart Part);

    /** Called when vehicle is totaled */
    UFUNCTION(BlueprintImplementableEvent, Category = "Damage|Events")
    void OnVehicleTotaled();

    /** Called when part is repaired */
    UFUNCTION(BlueprintImplementableEvent, Category = "Damage|Events")
    void OnPartRepaired(EVehiclePart Part);

    // ============================================================
    // Debug
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Debug")
    bool bShowDebugInfo = false;

    UFUNCTION(BlueprintCallable, Category = "Damage|Debug")
    void DrawDebugDamageInfo();

private:
    /** Reference to owning vehicle */
    UPROPERTY()
    ARacingVehicle* OwnerVehicle;

    /** Initialize part damages */
    void InitializePartDamages();

    /** Update overall health */
    void UpdateOverallHealth();

    /** Calculate impact severity */
    float CalculateImpactSeverity(float ImpactForce) const;

    /** Get affected parts from impact location */
    TArray<EVehiclePart> GetAffectedParts(const FVector& ImpactLocation) const;

    /** Check if vehicle should be totaled */
    void CheckVehicleTotaled();
};
