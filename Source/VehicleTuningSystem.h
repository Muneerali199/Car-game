// VehicleTuningSystem.h - Advanced vehicle tuning and setup system
// Comprehensive vehicle setup options like real racing simulators

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleTuningSystem.generated.h"

// Tuning categories
UENUM(BlueprintType)
enum class ETuningCategory : uint8
{
	Engine,
	Transmission,
	Suspension,
	Aerodynamics,
	Brakes,
	Tires,
	Differential,
	Weight,
	Electronics
};

// Suspension type
UENUM(BlueprintType)
enum class ESuspensionType : uint8
{
	MacPhersonStrut,
	DoubleWishbone,
	MultiLink,
	TorsionBeam,
	LeafSpring,
	AirSuspension,
	ActiveSuspension
};

// Differential type
UENUM(BlueprintType)
enum class EDifferentialType : uint8
{
	Open,              // No locking
	LimitedSlip,       // Mechanical LSD
	ElectronicLSD,     // E-LSD with adjustable preload
	TorsenLSD,         // Torque-sensing
	Locked,            // Fully locked (off-road)
	ActiveDifferential // Fully electronic control
};

// Tire compound
UENUM(BlueprintType)
enum class ETireCompound : uint8
{
	Street,      // Hard, long-lasting
	Sport,       // Medium grip
	SemiSlick,   // High grip, road legal
	Slick,       // Maximum grip, track only
	Rain,        // Wet weather
	Intermediate,// Mixed conditions
	Drag,        // Drag racing
	Rally,       // Off-road
	Snow,        // Winter conditions
	Drift        // Low grip for drifting
};

// Engine tuning parameters
USTRUCT(BlueprintType)
struct FEngineTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine", meta = (UIMin = "0.8", UIMax = "2.0"))
	float PowerMultiplier; // 1.0 = stock, 2.0 = double power

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine", meta = (UIMin = "0.8", UIMax = "1.5"))
	float TorqueMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine", meta = (UIMin = "1000", UIMax = "10000"))
	float RevLimit; // RPM

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine", meta = (UIMin = "-5", UIMax = "5"))
	float TorqueCurveAdjustment; // Shifts torque curve up/down RPM range

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float FuelMapRichness; // 0.8-1.2, affects power and consumption

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float BoostPressure; // Turbo/supercharger boost (bar)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	bool bEnableAntiLag; // Turbo anti-lag system

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	bool bEnableLaunchControl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine", meta = (UIMin = "0", UIMax = "10000"))
	float LaunchControlRPM;

	FEngineTuning()
		: PowerMultiplier(1.0f)
		, TorqueMultiplier(1.0f)
		, RevLimit(7500.0f)
		, TorqueCurveAdjustment(0.0f)
		, FuelMapRichness(1.0f)
		, BoostPressure(0.0f)
		, bEnableAntiLag(false)
		, bEnableLaunchControl(false)
		, LaunchControlRPM(0.0f)
	{}
};

// Transmission tuning
USTRUCT(BlueprintType)
struct FTransmissionTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	TArray<float> GearRatios; // Individual gear ratios

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission", meta = (UIMin = "2.0", UIMax = "6.0"))
	float FinalDriveRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission", meta = (UIMin = "0.05", UIMax = "0.5"))
	float ShiftTime; // Seconds to change gear

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission", meta = (UIMin = "4000", UIMax = "9000"))
	float AutoShiftUpRPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission", meta = (UIMin = "2000", UIMax = "7000"))
	float AutoShiftDownRPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	bool bAllowShiftCut; // Cut ignition during shifts for faster changes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	bool bEnableSequentialMode;

	FTransmissionTuning()
		: FinalDriveRatio(3.5f)
		, ShiftTime(0.2f)
		, AutoShiftUpRPM(6500.0f)
		, AutoShiftDownRPM(3500.0f)
		, bAllowShiftCut(false)
		, bEnableSequentialMode(false)
	{
		// Default 6-speed gear ratios
		GearRatios = { 3.82f, 2.20f, 1.52f, 1.15f, 0.92f, 0.75f };
	}
};

// Suspension tuning per wheel
USTRUCT(BlueprintType)
struct FSuspensionTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "20", UIMax = "200"))
	float SpringStiffness; // N/mm, higher = stiffer

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "1000", UIMax = "20000"))
	float DamperCompression; // N/(m/s), bump

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "1000", UIMax = "20000"))
	float DamperRebound; // N/(m/s), droop

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "0", UIMax = "50"))
	float AntiRollBarStiffness; // N*m/deg

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "0", UIMax = "100"))
	float RideHeight; // mm from default

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "-5", UIMax = "5"))
	float CamberAngle; // Degrees, negative = top tilted in

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "-2", UIMax = "2"))
	float ToeAngle; // Degrees, positive = toed in

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (UIMin = "0", UIMax = "15"))
	float CasterAngle; // Degrees, steering axis tilt

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	ESuspensionType SuspensionType;

	FSuspensionTuning()
		: SpringStiffness(60.0f)
		, DamperCompression(6000.0f)
		, DamperRebound(7000.0f)
		, AntiRollBarStiffness(10.0f)
		, RideHeight(0.0f)
		, CamberAngle(-1.5f)
		, ToeAngle(0.0f)
		, CasterAngle(6.0f)
		, SuspensionType(ESuspensionType::DoubleWishbone)
	{}
};

// Aerodynamic tuning
USTRUCT(BlueprintType)
struct FAerodynamicTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "0", UIMax = "30"))
	float FrontWingAngle; // Degrees, higher = more downforce and drag

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "0", UIMax = "30"))
	float RearWingAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "20", UIMax = "100"))
	float RideHeightFront; // mm, affects ground effect

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "20", UIMax = "100"))
	float RideHeightRear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "0.2", UIMax = "1.0"))
	float DragCoefficient; // Cd

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "0", UIMax = "5000"))
	float DownforceFront; // Newtons at 200 km/h

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "0", UIMax = "5000"))
	float DownforceRear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero", meta = (UIMin = "30", UIMax = "70"))
	float AeroBalancePercent; // % front downforce

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero")
	bool bEnableDRS; // Drag Reduction System

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aero")
	bool bEnableActiveAero; // Automatic aero adjustment

	FAerodynamicTuning()
		: FrontWingAngle(10.0f)
		, RearWingAngle(15.0f)
		, RideHeightFront(40.0f)
		, RideHeightRear(50.0f)
		, DragCoefficient(0.35f)
		, DownforceFront(800.0f)
		, DownforceRear(1200.0f)
		, AeroBalancePercent(40.0f)
		, bEnableDRS(false)
		, bEnableActiveAero(false)
	{}
};

// Brake tuning
USTRUCT(BlueprintType)
struct FBrakeTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brakes", meta = (UIMin = "30", UIMax = "70"))
	float BrakeBiasPercent; // % to front, 50 = even split

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brakes", meta = (UIMin = "1000", UIMax = "10000"))
	float MaxBrakeTorque; // N*m

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brakes", meta = (UIMin = "0", UIMax = "100"))
	float BrakePressure; // % of max

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brakes")
	bool bEnableABS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brakes", meta = (UIMin = "0", UIMax = "10"))
	float ABSIntervention; // 0 = off, 10 = max intervention

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brakes")
	bool bEnableBrakeBalance; // Allow dynamic brake balance

	FBrakeTuning()
		: BrakeBiasPercent(55.0f)
		, MaxBrakeTorque(3500.0f)
		, BrakePressure(100.0f)
		, bEnableABS(true)
		, ABSIntervention(5.0f)
		, bEnableBrakeBalance(false)
	{}
};

// Tire tuning
USTRUCT(BlueprintType)
struct FTireTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	ETireCompound Compound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires", meta = (UIMin = "1.5", UIMax = "4.0"))
	float TirePressureFront; // Bar

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires", meta = (UIMin = "1.5", UIMax = "4.0"))
	float TirePressureRear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires", meta = (UIMin = "175", UIMax = "335"))
	float TireWidthFront; // mm

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires", meta = (UIMin = "175", UIMax = "335"))
	float TireWidthRear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires", meta = (UIMin = "15", UIMax = "22"))
	float WheelDiameterFront; // Inches

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires", meta = (UIMin = "15", UIMax = "22"))
	float WheelDiameterRear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires", meta = (UIMin = "0.8", UIMax = "1.5"))
	float GripMultiplier; // Compound-specific grip adjustment

	FTireTuning()
		: Compound(ETireCompound::Sport)
		, TirePressureFront(2.2f)
		, TirePressureRear(2.4f)
		, TireWidthFront(245.0f)
		, TireWidthRear(275.0f)
		, WheelDiameterFront(18.0f)
		, WheelDiameterRear(18.0f)
		, GripMultiplier(1.0f)
	{}
};

// Differential tuning
USTRUCT(BlueprintType)
struct FDifferentialTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Differential")
	EDifferentialType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Differential", meta = (UIMin = "0", UIMax = "100"))
	float LockUnderPower; // % lock on acceleration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Differential", meta = (UIMin = "0", UIMax = "100"))
	float LockUnderBraking; // % lock on deceleration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Differential", meta = (UIMin = "0", UIMax = "200"))
	float Preload; // N*m, initial locking force

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Differential", meta = (UIMin = "0", UIMax = "100"))
	float CoastRampAngle; // Degrees, affects coast locking

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Differential", meta = (UIMin = "0", UIMax = "100"))
	float DriveRampAngle; // Degrees, affects drive locking

	FDifferentialTuning()
		: Type(EDifferentialType::LimitedSlip)
		, LockUnderPower(50.0f)
		, LockUnderBraking(30.0f)
		, Preload(50.0f)
		, CoastRampAngle(45.0f)
		, DriveRampAngle(60.0f)
	{}
};

// Weight distribution
USTRUCT(BlueprintType)
struct FWeightTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight", meta = (UIMin = "800", UIMax = "2500"))
	float TotalMass; // kg

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight", meta = (UIMin = "30", UIMax = "70"))
	float FrontWeightPercent; // % of weight on front axle

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight")
	FVector CenterOfMassOffset; // Offset from default (cm)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight", meta = (UIMin = "0", UIMax = "500"))
	float BallastMass; // Additional weight (kg)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight")
	FVector BallastPosition; // Where ballast is placed

	FWeightTuning()
		: TotalMass(1500.0f)
		, FrontWeightPercent(50.0f)
		, CenterOfMassOffset(FVector::ZeroVector)
		, BallastMass(0.0f)
		, BallastPosition(FVector::ZeroVector)
	{}
};

// Electronics tuning
USTRUCT(BlueprintType)
struct FElectronicsTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics")
	bool bEnableTractionControl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics", meta = (UIMin = "0", UIMax = "10"))
	int32 TractionControlLevel; // 0 = off, 10 = maximum

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics")
	bool bEnableStabilityControl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics", meta = (UIMin = "0", UIMax = "10"))
	int32 StabilityControlLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics")
	bool bEnableABSSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics")
	bool bEnablePowerSteering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics", meta = (UIMin = "0.5", UIMax = "2.0"))
	float SteeringAssistMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electronics")
	bool bEnableActiveDifferential;

	FElectronicsTuning()
		: bEnableTractionControl(true)
		, TractionControlLevel(5)
		, bEnableStabilityControl(true)
		, StabilityControlLevel(5)
		, bEnableABSSystem(true)
		, bEnablePowerSteering(true)
		, SteeringAssistMultiplier(1.0f)
		, bEnableActiveDifferential(false)
	{}
};

// Complete vehicle setup
USTRUCT(BlueprintType)
struct FVehicleSetup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FString SetupName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FString TrackName; // Track this setup is for

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FEngineTuning Engine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FTransmissionTuning Transmission;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FSuspensionTuning SuspensionFrontLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FSuspensionTuning SuspensionFrontRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FSuspensionTuning SuspensionRearLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FSuspensionTuning SuspensionRearRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FAerodynamicTuning Aerodynamics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FBrakeTuning Brakes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FTireTuning Tires;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FDifferentialTuning Differential;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FWeightTuning Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FElectronicsTuning Electronics;

	FVehicleSetup()
		: SetupName(TEXT("Default Setup"))
		, TrackName(TEXT(""))
	{}
};

/**
 * UVehicleTuningSystem - Comprehensive vehicle setup and tuning component
 * Features:
 * - Full mechanical setup (suspension, aero, brakes, diff, weight)
 * - Engine and transmission tuning
 * - Tire selection and pressure adjustment
 * - Electronic aids configuration
 * - Setup presets (street, sport, race, drift, drag)
 * - Track-specific setups
 * - Save/load custom setups
 * - Setup comparison and telemetry analysis
 * - Real-time setup changes
 * - Setup suggestions based on track/conditions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGAME_API UVehicleTuningSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVehicleTuningSystem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Setup management
	UFUNCTION(BlueprintCallable, Category = "Tuning")
	void ApplySetup(const FVehicleSetup& Setup);

	UFUNCTION(BlueprintCallable, Category = "Tuning")
	FVehicleSetup GetCurrentSetup() const;

	UFUNCTION(BlueprintCallable, Category = "Tuning")
	void ResetToDefault();

	// Individual system tuning
	UFUNCTION(BlueprintCallable, Category = "Tuning|Engine")
	void TuneEngine(const FEngineTuning& EngineTuning);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Transmission")
	void TuneTransmission(const FTransmissionTuning& TransmissionTuning);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Suspension")
	void TuneSuspension(const FSuspensionTuning& FrontLeft, const FSuspensionTuning& FrontRight, const FSuspensionTuning& RearLeft, const FSuspensionTuning& RearRight);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Aerodynamics")
	void TuneAerodynamics(const FAerodynamicTuning& AeroTuning);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Brakes")
	void TuneBrakes(const FBrakeTuning& BrakeTuning);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Tires")
	void TuneTires(const FTireTuning& TireTuning);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Differential")
	void TuneDifferential(const FDifferentialTuning& DiffTuning);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Weight")
	void TuneWeight(const FWeightTuning& WeightTuning);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Electronics")
	void TuneElectronics(const FElectronicsTuning& ElectronicsTuning);

	// Presets
	UFUNCTION(BlueprintCallable, Category = "Tuning|Presets")
	void ApplyPreset(const FString& PresetName);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Presets")
	FVehicleSetup GetPreset(const FString& PresetName) const;

	UFUNCTION(BlueprintCallable, Category = "Tuning|Presets")
	TArray<FString> GetAvailablePresets() const;

	// Save/Load
	UFUNCTION(BlueprintCallable, Category = "Tuning|IO")
	bool SaveSetup(const FVehicleSetup& Setup, const FString& SetupName);

	UFUNCTION(BlueprintCallable, Category = "Tuning|IO")
	FVehicleSetup LoadSetup(const FString& SetupName, bool& bSuccess);

	UFUNCTION(BlueprintCallable, Category = "Tuning|IO")
	TArray<FString> GetSavedSetups() const;

	UFUNCTION(BlueprintCallable, Category = "Tuning|IO")
	void DeleteSetup(const FString& SetupName);

	// Analysis
	UFUNCTION(BlueprintPure, Category = "Tuning|Analysis")
	float CalculateTotalPower() const; // HP

	UFUNCTION(BlueprintPure, Category = "Tuning|Analysis")
	float CalculatePowerToWeightRatio() const; // HP/ton

	UFUNCTION(BlueprintPure, Category = "Tuning|Analysis")
	float CalculateTopSpeed() const; // km/h

	UFUNCTION(BlueprintPure, Category = "Tuning|Analysis")
	float Calculate0To100Time() const; // Seconds

	UFUNCTION(BlueprintPure, Category = "Tuning|Analysis")
	float CalculateBrakingDistance() const; // Meters from 100 km/h

	UFUNCTION(BlueprintPure, Category = "Tuning|Analysis")
	float CalculateLateralGrip() const; // G-force

	UFUNCTION(BlueprintPure, Category = "Tuning|Analysis")
	FString GetSetupRating() const; // Overall setup quality rating

	// Suggestions
	UFUNCTION(BlueprintCallable, Category = "Tuning|Suggestions")
	FVehicleSetup GenerateSetupForTrack(const FString& TrackName, ETrackDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, Category = "Tuning|Suggestions")
	TArray<FString> GetSetupSuggestions() const; // List of improvements

protected:
	// Current setup
	UPROPERTY(BlueprintReadOnly, Category = "Tuning")
	FVehicleSetup CurrentSetup;

	// Default setup (for reset)
	UPROPERTY(EditDefaultsOnly, Category = "Tuning")
	FVehicleSetup DefaultSetup;

	// Preset setups
	UPROPERTY(EditDefaultsOnly, Category = "Tuning|Presets")
	TMap<FString, FVehicleSetup> PresetSetups;

	// Vehicle reference
	UPROPERTY()
	class ARacingVehicle* OwnerVehicle;

private:
	// Apply individual components
	void ApplyEngineTuning(const FEngineTuning& Tuning);
	void ApplyTransmissionTuning(const FTransmissionTuning& Tuning);
	void ApplySuspensionTuning(const FSuspensionTuning& Tuning, int32 WheelIndex);
	void ApplyAerodynamicTuning(const FAerodynamicTuning& Tuning);
	void ApplyBrakeTuning(const FBrakeTuning& Tuning);
	void ApplyTireTuning(const FTireTuning& Tuning);
	void ApplyDifferentialTuning(const FDifferentialTuning& Tuning);
	void ApplyWeightTuning(const FWeightTuning& Tuning);
	void ApplyElectronicsTuning(const FElectronicsTuning& Tuning);

	// Calculations
	float CalculateAeroDrag() const;
	float CalculateDownforce() const;
	float CalculateCorneringStiffness() const;
	float CalculateGearRatioOptimization() const;

	// Validation
	bool ValidateSetup(const FVehicleSetup& Setup, FString& OutError) const;
	void ClampSetupValues(FVehicleSetup& Setup) const;
};
