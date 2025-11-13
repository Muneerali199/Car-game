// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleCustomizationComponent.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/**
 * Customization Categories
 */
UENUM(BlueprintType)
enum class ECustomizationCategory : uint8
{
    Paint           UMETA(DisplayName = "Paint & Livery"),
    Wheels          UMETA(DisplayName = "Wheels & Tires"),
    Aerodynamics    UMETA(DisplayName = "Aero (spoilers, splitters)"),
    Body            UMETA(DisplayName = "Body Kits"),
    Performance     UMETA(DisplayName = "Performance Upgrades"),
    Interior        UMETA(DisplayName = "Interior"),
    Decals          UMETA(DisplayName = "Decals & Stickers"),
    Lights          UMETA(DisplayName = "Lights & LEDs")
};

/**
 * Performance Upgrade Types
 */
UENUM(BlueprintType)
enum class EPerformanceUpgrade : uint8
{
    Engine          UMETA(DisplayName = "Engine - More power"),
    Turbo           UMETA(DisplayName = "Turbocharger - Boost"),
    Exhaust         UMETA(DisplayName = "Exhaust - Better flow"),
    Intake          UMETA(DisplayName = "Air Intake - More air"),
    ECU             UMETA(DisplayName = "ECU Tune - Optimized mapping"),
    Transmission    UMETA(DisplayName = "Transmission - Faster shifts"),
    Suspension      UMETA(DisplayName = "Suspension - Better handling"),
    Brakes          UMETA(DisplayName = "Brakes - Better stopping"),
    Tires           UMETA(DisplayName = "Tires - More grip"),
    Weight          UMETA(DisplayName = "Weight Reduction - Lighter"),
    Nitrous         UMETA(DisplayName = "Nitrous Oxide - Boost"),
    Differential    UMETA(DisplayName = "Differential - Power delivery")
};

/**
 * Paint Finish Types
 */
UENUM(BlueprintType)
enum class EPaintFinish : uint8
{
    Gloss           UMETA(DisplayName = "Gloss - Shiny"),
    Matte           UMETA(DisplayName = "Matte - Flat"),
    Metallic        UMETA(DisplayName = "Metallic - Sparkle"),
    Pearlescent     UMETA(DisplayName = "Pearlescent - Color shift"),
    Chrome          UMETA(DisplayName = "Chrome - Mirror"),
    Carbon          UMETA(DisplayName = "Carbon Fiber - Weave"),
    Chameleon       UMETA(DisplayName = "Chameleon - Multi-color shift"),
    Candy           UMETA(DisplayName = "Candy - Deep translucent")
};

/**
 * Wheel Style
 */
UENUM(BlueprintType)
enum class EWheelStyle : uint8
{
    Stock           UMETA(DisplayName = "Stock - Original"),
    Sport           UMETA(DisplayName = "Sport - Lightweight alloy"),
    Racing          UMETA(DisplayName = "Racing - Track focused"),
    Deep            UMETA(DisplayName = "Deep Dish - Wide and aggressive"),
    MultiSpoke      UMETA(DisplayName = "Multi-Spoke - Classic"),
    Mesh            UMETA(DisplayName = "Mesh - Retro"),
    Split           UMETA(DisplayName = "Split-Spoke - Modern"),
    Forged          UMETA(DisplayName = "Forged - Ultra light")
};

/**
 * Paint Configuration
 */
USTRUCT(BlueprintType)
struct FPaintConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    FLinearColor PrimaryColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    FLinearColor SecondaryColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    FLinearColor AccentColor = FLinearColor::Black;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    EPaintFinish Finish = EPaintFinish::Gloss;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    float Metallic = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    float Roughness = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    bool bTwoTone = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    bool bRacingStripes = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
    float StripeWidth = 0.3f;
};

/**
 * Performance Stats
 */
USTRUCT(BlueprintType)
struct FPerformanceStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float Horsepower = 300.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float Torque = 400.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float TopSpeed = 250.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float Acceleration = 5.0f; // 0-100 km/h time

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float BrakeForce = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float GripLevel = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float Weight = 1500.0f; // kg

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    float PowerToWeight = 0.2f; // HP per kg
};

/**
 * Customization Data
 */
USTRUCT(BlueprintType)
struct FVehicleCustomization
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FPaintConfig PaintConfiguration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    EWheelStyle WheelStyle = EWheelStyle::Stock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    int32 WheelSize = 18; // inches

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FLinearColor WheelColor = FLinearColor::Black;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    TArray<EPerformanceUpgrade> PerformanceUpgrades;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    bool bHasSpoiler = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    bool bHasBodyKit = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    bool bCustomExhaust = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    bool bTintedWindows = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    float WindowTintLevel = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    TArray<FString> DecalNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FString LicensePlate = "RACING";
};

/**
 * Advanced Vehicle Customization System
 * 
 * Features:
 * - Visual customization (paint, wheels, body kits)
 * - Performance upgrades (engine, suspension, tires)
 * - Real-time preview
 * - Save/load custom configurations
 * - Preset liveries
 * - Decal system
 * - Performance statistics
 * - Cost system for upgrades
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGAME_API UVehicleCustomizationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleCustomizationComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ============================================================
    // Current Customization
    // ============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FVehicleCustomization CurrentCustomization;

    UPROPERTY(BlueprintReadOnly, Category = "Performance")
    FPerformanceStats CurrentPerformance;

    // ============================================================
    // Paint Customization
    // ============================================================

    /** Set primary paint color */
    UFUNCTION(BlueprintCallable, Category = "Customization|Paint")
    void SetPrimaryColor(FLinearColor Color);

    /** Set secondary paint color */
    UFUNCTION(BlueprintCallable, Category = "Customization|Paint")
    void SetSecondaryColor(FLinearColor Color);

    /** Set accent color */
    UFUNCTION(BlueprintCallable, Category = "Customization|Paint")
    void SetAccentColor(FLinearColor Color);

    /** Set paint finish type */
    UFUNCTION(BlueprintCallable, Category = "Customization|Paint")
    void SetPaintFinish(EPaintFinish Finish);

    /** Enable/disable racing stripes */
    UFUNCTION(BlueprintCallable, Category = "Customization|Paint")
    void SetRacingStripes(bool bEnabled, float Width = 0.3f);

    /** Apply preset livery */
    UFUNCTION(BlueprintCallable, Category = "Customization|Paint")
    void ApplyLiveryPreset(const FString& PresetName);

    /** Get available livery presets */
    UFUNCTION(BlueprintCallable, Category = "Customization|Paint")
    TArray<FString> GetLiveryPresets() const;

    // ============================================================
    // Wheel Customization
    // ============================================================

    /** Change wheel style */
    UFUNCTION(BlueprintCallable, Category = "Customization|Wheels")
    void SetWheelStyle(EWheelStyle Style);

    /** Change wheel size (inches) */
    UFUNCTION(BlueprintCallable, Category = "Customization|Wheels")
    void SetWheelSize(int32 Size);

    /** Set wheel color */
    UFUNCTION(BlueprintCallable, Category = "Customization|Wheels")
    void SetWheelColor(FLinearColor Color);

    /** Set tire brand/compound */
    UFUNCTION(BlueprintCallable, Category = "Customization|Wheels")
    void SetTireCompound(const FString& TireType);

    // ============================================================
    // Aerodynamics
    // ============================================================

    /** Add/remove rear spoiler */
    UFUNCTION(BlueprintCallable, Category = "Customization|Aero")
    void SetSpoiler(bool bEnabled, int32 SpoilerType = 0);

    /** Add/remove front splitter */
    UFUNCTION(BlueprintCallable, Category = "Customization|Aero")
    void SetFrontSplitter(bool bEnabled);

    /** Add/remove side skirts */
    UFUNCTION(BlueprintCallable, Category = "Customization|Aero")
    void SetSideSkirts(bool bEnabled);

    /** Add/remove diffuser */
    UFUNCTION(BlueprintCallable, Category = "Customization|Aero")
    void SetRearDiffuser(bool bEnabled);

    // ============================================================
    // Body Customization
    // ============================================================

    /** Apply body kit */
    UFUNCTION(BlueprintCallable, Category = "Customization|Body")
    void SetBodyKit(bool bEnabled, int32 KitType = 0);

    /** Set hood style */
    UFUNCTION(BlueprintCallable, Category = "Customization|Body")
    void SetHoodStyle(int32 HoodType);

    /** Set bumper style */
    UFUNCTION(BlueprintCallable, Category = "Customization|Body")
    void SetBumperStyle(bool bFront, int32 BumperType);

    /** Window tint */
    UFUNCTION(BlueprintCallable, Category = "Customization|Body")
    void SetWindowTint(float TintLevel);

    // ============================================================
    // Performance Upgrades
    // ============================================================

    /** Install performance upgrade */
    UFUNCTION(BlueprintCallable, Category = "Customization|Performance")
    bool InstallUpgrade(EPerformanceUpgrade Upgrade);

    /** Remove performance upgrade */
    UFUNCTION(BlueprintCallable, Category = "Customization|Performance")
    void RemoveUpgrade(EPerformanceUpgrade Upgrade);

    /** Check if upgrade is installed */
    UFUNCTION(BlueprintCallable, Category = "Customization|Performance")
    bool HasUpgrade(EPerformanceUpgrade Upgrade) const;

    /** Get upgrade cost */
    UFUNCTION(BlueprintCallable, Category = "Customization|Performance")
    int32 GetUpgradeCost(EPerformanceUpgrade Upgrade) const;

    /** Get upgrade performance bonus */
    UFUNCTION(BlueprintCallable, Category = "Customization|Performance")
    float GetUpgradeBonus(EPerformanceUpgrade Upgrade) const;

    /** Reset all upgrades */
    UFUNCTION(BlueprintCallable, Category = "Customization|Performance")
    void ResetAllUpgrades();

    // ============================================================
    // Decals & Stickers
    // ============================================================

    /** Add decal to vehicle */
    UFUNCTION(BlueprintCallable, Category = "Customization|Decals")
    void AddDecal(const FString& DecalName, FVector Location, FRotator Rotation, FVector Scale);

    /** Remove decal */
    UFUNCTION(BlueprintCallable, Category = "Customization|Decals")
    void RemoveDecal(int32 DecalIndex);

    /** Clear all decals */
    UFUNCTION(BlueprintCallable, Category = "Customization|Decals")
    void ClearAllDecals();

    /** Add racing number */
    UFUNCTION(BlueprintCallable, Category = "Customization|Decals")
    void SetRacingNumber(int32 Number, FLinearColor NumberColor);

    /** Set license plate text */
    UFUNCTION(BlueprintCallable, Category = "Customization|Decals")
    void SetLicensePlate(const FString& PlateText);

    // ============================================================
    // Statistics
    // ============================================================

    /** Calculate current performance stats */
    UFUNCTION(BlueprintCallable, Category = "Customization|Stats")
    FPerformanceStats CalculatePerformanceStats();

    /** Get performance rating (0-100) */
    UFUNCTION(BlueprintCallable, Category = "Customization|Stats")
    float GetPerformanceRating() const;

    /** Get total customization cost */
    UFUNCTION(BlueprintCallable, Category = "Customization|Stats")
    int32 GetTotalCustomizationCost() const;

    /** Compare with stock performance */
    UFUNCTION(BlueprintCallable, Category = "Customization|Stats")
    void CompareWithStock(float& OutPowerIncrease, float& OutSpeedIncrease, float& OutGripIncrease);

    // ============================================================
    // Save/Load
    // ============================================================

    /** Save current customization */
    UFUNCTION(BlueprintCallable, Category = "Customization|Storage")
    bool SaveCustomization(const FString& SaveName);

    /** Load customization */
    UFUNCTION(BlueprintCallable, Category = "Customization|Storage")
    bool LoadCustomization(const FString& SaveName);

    /** Get saved customizations */
    UFUNCTION(BlueprintCallable, Category = "Customization|Storage")
    TArray<FString> GetSavedCustomizations();

    /** Delete saved customization */
    UFUNCTION(BlueprintCallable, Category = "Customization|Storage")
    void DeleteCustomization(const FString& SaveName);

    /** Export customization as JSON */
    UFUNCTION(BlueprintCallable, Category = "Customization|Storage")
    FString ExportToJSON();

    /** Import customization from JSON */
    UFUNCTION(BlueprintCallable, Category = "Customization|Storage")
    bool ImportFromJSON(const FString& JSONString);

    // ============================================================
    // Presets
    // ============================================================

    /** Apply full customization preset */
    UFUNCTION(BlueprintCallable, Category = "Customization|Presets")
    void ApplyPreset(const FString& PresetName);

    /** Reset to stock configuration */
    UFUNCTION(BlueprintCallable, Category = "Customization|Presets")
    void ResetToStock();

    /** Available presets (Street, Sport, Race, Drift, Drag) */
    UFUNCTION(BlueprintCallable, Category = "Customization|Presets")
    TArray<FString> GetAvailablePresets() const;

    // ============================================================
    // Events
    // ============================================================

    /** Called when customization changes */
    UFUNCTION(BlueprintImplementableEvent, Category = "Customization|Events")
    void OnCustomizationChanged(ECustomizationCategory Category);

    /** Called when upgrade installed */
    UFUNCTION(BlueprintImplementableEvent, Category = "Customization|Events")
    void OnUpgradeInstalled(EPerformanceUpgrade Upgrade, float NewPerformanceRating);

    /** Called when preset applied */
    UFUNCTION(BlueprintImplementableEvent, Category = "Customization|Events")
    void OnPresetApplied(const FString& PresetName);

    // ============================================================
    // Material Management
    // ============================================================

private:
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> DynamicMaterials;

    UPROPERTY()
    FPerformanceStats StockPerformance;

    void ApplyPaintToMaterials();
    void UpdateWheelMeshes();
    void UpdatePerformanceStats();
    void ToggleAeroPart(const FString& PartName, bool bEnabled);
    float CalculateUpgradeMultiplier() const;

    // Material parameter names
    static const FName PARAM_PRIMARY_COLOR;
    static const FName PARAM_SECONDARY_COLOR;
    static const FName PARAM_ACCENT_COLOR;
    static const FName PARAM_METALLIC;
    static const FName PARAM_ROUGHNESS;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization|Debug")
    bool bShowDebugInfo = false;
};
