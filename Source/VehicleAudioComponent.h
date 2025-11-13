// VehicleAudioComponent.h
// Engine sounds, tire squeal, and impact audio
// Copyright 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleAudioComponent.generated.h"

class USoundBase;
class UAudioComponent;
class ARacingVehicle;

/**
 * Handles all vehicle audio including engine, tires, wind, and impacts
 */
UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class CARGAME_API UVehicleAudioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleAudioComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ============================================================
    // ENGINE SOUNDS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Audio")
    USoundBase* EngineIdleSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Audio")
    USoundBase* EngineLowRPMSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Audio")
    USoundBase* EngineMidRPMSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Audio")
    USoundBase* EngineHighRPMSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Audio")
    float EngineVolumeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Audio")
    float EnginePitchMultiplier;

    // ============================================================
    // TIRE SOUNDS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Audio")
    USoundBase* TireSquealSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Audio")
    USoundBase* TireSkidSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Audio")
    float TireSquealThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tire Audio")
    float TireSquealVolumeMultiplier;

    // ============================================================
    // WIND/AIR SOUNDS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Audio")
    USoundBase* WindSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Audio")
    float WindVolumeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Audio")
    float WindSpeedThreshold;

    // ============================================================
    // IMPACT SOUNDS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact Audio")
    TArray<USoundBase*> ImpactSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact Audio")
    float MinImpactVelocity;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayImpactSound(float ImpactStrength, FVector Location);

    // ============================================================
    // TRANSMISSION SOUNDS
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission Audio")
    USoundBase* GearShiftSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission Audio")
    USoundBase* BackfireSound;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayGearShiftSound();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayBackfireSound();

private:
    UPROPERTY()
    UAudioComponent* EngineAudioComponent;

    UPROPERTY()
    UAudioComponent* TireAudioComponent;

    UPROPERTY()
    UAudioComponent* WindAudioComponent;

    ARacingVehicle* OwnerVehicle;

    void UpdateEngineSound(float DeltaTime);
    void UpdateTireSound(float DeltaTime);
    void UpdateWindSound(float DeltaTime);
    
    float GetEngineRPMNormalized();
    float GetTireSlipAmount();
    float GetVehicleSpeed();
};
