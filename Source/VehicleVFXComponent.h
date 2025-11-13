// VehicleVFXComponent.h
// Tire smoke, sparks, dust particles and visual effects
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h"
#include "VehicleVFXComponent.generated.h"

class UNiagaraComponent;
class ARacingVehicle;

/**
 * Handles all vehicle visual effects
 */
UCLASS(ClassGroup=(VFX), meta=(BlueprintSpawnableComponent))
class CARGAME_API UVehicleVFXComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleVFXComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ============================================================
    // TIRE SMOKE
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Effects")
    UNiagaraSystem* TireSmokeEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Effects")
    float SmokeEmissionThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Effects")
    float SmokeIntensityMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Effects")
    bool bEnableTireSmoke;

    // ============================================================
    // SPARKS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sparks")
    UNiagaraSystem* SparksEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sparks")
    float SparkImpactThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sparks")
    bool bEnableSparks;

    UFUNCTION(BlueprintCallable, Category = "VFX")
    void SpawnSparksAtLocation(FVector Location, FVector Normal, float Intensity);

    // ============================================================
    // DUST/DIRT
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dust Effects")
    UNiagaraSystem* DustEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dust Effects")
    float DustSpeedThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dust Effects")
    bool bEnableDust;

    // ============================================================
    // SKID MARKS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skid Marks")
    UMaterialInterface* SkidMarkMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skid Marks")
    float SkidMarkWidth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skid Marks")
    float SkidMarkLifetime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skid Marks")
    bool bEnableSkidMarks;

    // ============================================================
    // SPEED LINES
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed Effects")
    UNiagaraSystem* SpeedLinesEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed Effects")
    float SpeedLinesThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed Effects")
    bool bEnableSpeedLines;

    // ============================================================
    // EXHAUST
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exhaust")
    UNiagaraSystem* ExhaustEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exhaust")
    TArray<FVector> ExhaustLocations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exhaust")
    bool bEnableExhaust;

private:
    UPROPERTY()
    TArray<UNiagaraComponent*> TireSmokeComponents;

    UPROPERTY()
    TArray<UNiagaraComponent*> DustComponents;

    UPROPERTY()
    UNiagaraComponent* SpeedLinesComponent;

    UPROPERTY()
    TArray<UNiagaraComponent*> ExhaustComponents;

    ARacingVehicle* OwnerVehicle;

    void UpdateTireSmoke(float DeltaTime);
    void UpdateDust(float DeltaTime);
    void UpdateSpeedLines(float DeltaTime);
    void UpdateExhaust(float DeltaTime);
    void CreateSkidMark(FVector Location, FRotator Rotation, int32 WheelIndex);
    
    float GetTireSlipAmount();
    float GetVehicleSpeed();
    TArray<FVector> GetWheelLocations();
};
