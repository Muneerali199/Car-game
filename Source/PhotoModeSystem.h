// PhotoModeSystem.h - Professional photo mode for racing game
// Cinematic camera controls, filters, and screenshot capabilities

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PhotoModeSystem.generated.h"

// Camera movement modes
UENUM(BlueprintType)
enum class ECameraMovementMode : uint8
{
	Free,           // Free-flying camera
	Orbit,          // Orbit around subject
	Pan,            // Fixed orbit with pan
	Dolly,          // Track along path
	Locked,         // Fixed position and rotation
	FirstPerson,    // Inside vehicle
	FollowPath      // Animated camera path
};

// Photo filters
UENUM(BlueprintType)
enum class EPhotoFilter : uint8
{
	None,
	BlackAndWhite,
	Sepia,
	Vintage,
	CoolBlue,
	WarmOrange,
	HighContrast,
	Desaturated,
	Dramatic,
	Cinematic,
	HDR,
	CrossProcess,
	Vignette,
	FilmGrain,
	Technicolor
};

// Time of day presets for photo mode
UENUM(BlueprintType)
enum class EPhotoTimeOfDay : uint8
{
	Current,      // Keep current time
	GoldenHour,   // Sunrise/sunset
	MidDay,       // Bright sunlight
	BluHour,      // Twilight
	Night,        // Dark with lights
	Overcast,     // Cloudy day
	Custom        // User-defined
};

// Depth of field settings
USTRUCT(BlueprintType)
struct FDepthOfFieldSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "1"))
	float Enabled; // 0 = off, 1 = full effect

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0.1", UIMax = "32.0"))
	float Aperture; // f-stop, lower = more blur

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "1", UIMax = "10000"))
	float FocusDistance; // cm

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "2"))
	float BokehScale; // Size of bokeh shapes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "1"))
	float BokehIntensity;

	FDepthOfFieldSettings()
		: Enabled(0.0f)
		, Aperture(8.0f)
		, FocusDistance(500.0f)
		, BokehScale(1.0f)
		, BokehIntensity(0.5f)
	{}
};

// Motion blur settings
USTRUCT(BlueprintType)
struct FMotionBlurSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "1"))
	float Amount; // 0 = off, 1 = maximum

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "100"))
	float MaxVelocity; // Pixels per frame

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCameraMotionBlur; // Camera movement blur

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bObjectMotionBlur; // Object movement blur

	FMotionBlurSettings()
		: Amount(0.5f)
		, MaxVelocity(50.0f)
		, bCameraMotionBlur(true)
		, bObjectMotionBlur(true)
	{}
};

// Color grading settings
USTRUCT(BlueprintType)
struct FColorGradingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "2"))
	float Saturation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "2"))
	float Contrast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "4"))
	float Gamma;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "2"))
	float Gain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor ColorTint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-1", UIMax = "1"))
	float TemperatureAdjustment; // Cool to warm

	FColorGradingSettings()
		: Saturation(1.0f)
		, Contrast(1.0f)
		, Gamma(1.0f)
		, Gain(1.0f)
		, ColorTint(FLinearColor::White)
		, TemperatureAdjustment(0.0f)
	{}
};

// Lighting overrides
USTRUCT(BlueprintType)
struct FPhotoLightingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "20"))
	float SunIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-180", UIMax = "180"))
	float SunAngle; // Degrees, horizontal rotation

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-90", UIMax = "90"))
	float SunElevation; // Degrees, vertical angle

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor SunColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "10"))
	float AmbientIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor AmbientColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "1"))
	float SkyIntensity;

	FPhotoLightingSettings()
		: SunIntensity(10.0f)
		, SunAngle(0.0f)
		, SunElevation(45.0f)
		, SunColor(FLinearColor::White)
		, AmbientIntensity(1.0f)
		, AmbientColor(FLinearColor(0.3f, 0.3f, 0.4f))
		, SkyIntensity(1.0f)
	{}
};

// Pose/expression for vehicle (wheels turned, suspension compressed, etc.)
USTRUCT(BlueprintType)
struct FVehiclePoseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-45", UIMax = "45"))
	float SteeringAngle; // Degrees

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-10", UIMax = "10"))
	float SuspensionCompression; // cm per wheel

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-30", UIMax = "30"))
	float RollAngle; // Vehicle roll

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-30", UIMax = "30"))
	float PitchAngle; // Vehicle pitch

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOpenDoors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOpenHood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOpenTrunk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHeadlightsOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBrakeLightsOn;

	FVehiclePoseSettings()
		: SteeringAngle(0.0f)
		, SuspensionCompression(0.0f)
		, RollAngle(0.0f)
		, PitchAngle(0.0f)
		, bOpenDoors(false)
		, bOpenHood(false)
		, bOpenTrunk(false)
		, bHeadlightsOn(false)
		, bBrakeLightsOn(false)
	{}
};

// Complete photo mode settings
USTRUCT(BlueprintType)
struct FPhotoModeSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraMovementMode CameraMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CameraLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CameraRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "10", UIMax = "170"))
	float FieldOfView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDepthOfFieldSettings DepthOfField;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMotionBlurSettings MotionBlur;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColorGradingSettings ColorGrading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPhotoFilter Filter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPhotoLightingSettings Lighting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPhotoTimeOfDay TimeOfDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVehiclePoseSettings VehiclePose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFreezeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideDriver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "1"))
	float VignetteIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = "0", UIMax = "1"))
	float FilmGrainIntensity;

	FPhotoModeSettings()
		: CameraMode(ECameraMovementMode::Orbit)
		, CameraLocation(FVector::ZeroVector)
		, CameraRotation(FRotator::ZeroRotator)
		, FieldOfView(90.0f)
		, Filter(EPhotoFilter::None)
		, TimeOfDay(EPhotoTimeOfDay::Current)
		, bShowUI(false)
		, bFreezeTime(true)
		, bHideDriver(false)
		, VignetteIntensity(0.0f)
		, FilmGrainIntensity(0.0f)
	{}
};

// Screenshot metadata
USTRUCT(BlueprintType)
struct FScreenshotMetadata
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString FileName;

	UPROPERTY(BlueprintReadWrite)
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadWrite)
	FString TrackName;

	UPROPERTY(BlueprintReadWrite)
	FString VehicleName;

	UPROPERTY(BlueprintReadWrite)
	FPhotoModeSettings Settings;

	UPROPERTY(BlueprintReadWrite)
	int32 ResolutionX;

	UPROPERTY(BlueprintReadWrite)
	int32 ResolutionY;

	FScreenshotMetadata()
		: FileName(TEXT(""))
		, Timestamp(FDateTime::Now())
		, TrackName(TEXT(""))
		, VehicleName(TEXT(""))
		, ResolutionX(1920)
		, ResolutionY(1080)
	{}
};

/**
 * APhotoModeSystem - Professional photo mode system
 * Features:
 * - Free camera with smooth controls
 * - Orbit camera around vehicle
 * - Animated camera paths
 * - Depth of field with bokeh
 * - Motion blur controls
 * - Color grading and filters
 * - Lighting overrides (time of day, sun angle)
 * - Vehicle posing (steering, doors, lights)
 * - High-resolution screenshots (up to 8K)
 * - Watermark and logo overlay
 * - Photo gallery with metadata
 * - Social media export (aspect ratios)
 * - Rule of thirds grid
 * - Focus stacking
 * - Panoramic photos
 */
UCLASS()
class CARGAME_API APhotoModeSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	APhotoModeSystem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Photo mode control
	UFUNCTION(BlueprintCallable, Category = "Photo Mode")
	void EnterPhotoMode(AActor* SubjectVehicle);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode")
	void ExitPhotoMode();

	UFUNCTION(BlueprintPure, Category = "Photo Mode")
	bool IsInPhotoMode() const { return bIsActive; }

	// Camera control
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void SetCameraMode(ECameraMovementMode Mode);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void SetCameraLocation(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void SetCameraRotation(FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void SetFieldOfView(float FOV);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void MoveCamera(FVector DeltaLocation);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void RotateCamera(FRotator DeltaRotation);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void SetOrbitDistance(float Distance);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Camera")
	void ResetCamera();

	// Camera effects
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Effects")
	void SetDepthOfField(const FDepthOfFieldSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Effects")
	void SetMotionBlur(const FMotionBlurSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Effects")
	void SetColorGrading(const FColorGradingSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Effects")
	void ApplyFilter(EPhotoFilter Filter);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Effects")
	void SetVignetteIntensity(float Intensity);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Effects")
	void SetFilmGrainIntensity(float Intensity);

	// Lighting
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Lighting")
	void SetTimeOfDay(EPhotoTimeOfDay TimeOfDay);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Lighting")
	void SetLightingSettings(const FPhotoLightingSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Lighting")
	void SetSunAngle(float Angle, float Elevation);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Lighting")
	void SetSunColor(FLinearColor Color);

	// Vehicle posing
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Vehicle")
	void SetVehiclePose(const FVehiclePoseSettings& Pose);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Vehicle")
	void SetSteeringAngle(float Angle);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Vehicle")
	void SetVehicleRollPitch(float Roll, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Vehicle")
	void ToggleDoors(bool bOpen);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Vehicle")
	void ToggleLights(bool bHeadlights, bool bBrakeLights);

	// Screenshot
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Screenshot")
	void TakeScreenshot(int32 ResolutionX = 1920, int32 ResolutionY = 1080, bool bIncludeUI = false);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Screenshot")
	void TakeHighResScreenshot(int32 ResolutionMultiplier = 4); // 4 = 4K, 8 = 8K

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Screenshot")
	void TakePanorama(int32 Segments = 6, float FOVOverlap = 10.0f);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Screenshot")
	void TakeScreenshotWithWatermark(const FString& WatermarkText, FVector2D Position);

	// Presets
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Presets")
	void ApplyPreset(const FString& PresetName);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Presets")
	void SavePreset(const FString& PresetName, const FPhotoModeSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Presets")
	TArray<FString> GetAvailablePresets() const;

	// Settings management
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Settings")
	void ApplySettings(const FPhotoModeSettings& Settings);

	UFUNCTION(BlueprintPure, Category = "Photo Mode|Settings")
	FPhotoModeSettings GetCurrentSettings() const { return CurrentSettings; }

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Settings")
	void ResetSettings();

	// Gallery
	UFUNCTION(BlueprintPure, Category = "Photo Mode|Gallery")
	TArray<FScreenshotMetadata> GetScreenshotGallery() const;

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Gallery")
	void DeleteScreenshot(const FString& FileName);

	// Utilities
	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Utilities")
	void FreezeTime(bool bFreeze);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Utilities")
	void ShowRuleOfThirdsGrid(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Utilities")
	void ShowFocusPoint(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "Photo Mode|Utilities")
	void SetCameraSpeed(float Speed);

protected:
	// Photo mode state
	UPROPERTY(BlueprintReadOnly, Category = "Photo Mode")
	bool bIsActive;

	UPROPERTY(BlueprintReadOnly, Category = "Photo Mode")
	FPhotoModeSettings CurrentSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Photo Mode")
	AActor* SubjectVehicle;

	// Camera
	UPROPERTY(VisibleAnywhere, Category = "Components")
	ACameraActor* PhotoCamera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY()
	APlayerCameraManager* OriginalCameraManager;

	UPROPERTY()
	FVector OriginalCameraLocation;

	UPROPERTY()
	FRotator OriginalCameraRotation;

	// Orbit settings
	UPROPERTY(EditAnywhere, Category = "Photo Mode|Orbit")
	float OrbitDistance;

	UPROPERTY(EditAnywhere, Category = "Photo Mode|Orbit")
	float OrbitSpeed;

	// Camera movement
	UPROPERTY(EditAnywhere, Category = "Photo Mode|Movement")
	float CameraMovementSpeed;

	UPROPERTY(EditAnywhere, Category = "Photo Mode|Movement")
	float CameraRotationSpeed;

	UPROPERTY(EditAnywhere, Category = "Photo Mode|Movement")
	bool bSmoothCamera;

	UPROPERTY(EditAnywhere, Category = "Photo Mode|Movement")
	float CameraSmoothingFactor;

	// Post process
	UPROPERTY()
	class APostProcessVolume* PhotoPostProcessVolume;

	// Presets
	UPROPERTY(EditDefaultsOnly, Category = "Presets")
	TMap<FString, FPhotoModeSettings> PhotoPresets;

	// Screenshot settings
	UPROPERTY(EditAnywhere, Category = "Screenshot")
	FString ScreenshotFolder;

	UPROPERTY(EditAnywhere, Category = "Screenshot")
	bool bAutoSaveMetadata;

	UPROPERTY(EditAnywhere, Category = "Screenshot")
	UTexture2D* WatermarkLogo;

	// Gallery
	UPROPERTY()
	TArray<FScreenshotMetadata> ScreenshotGallery;

	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PhotoModeUIClass;

	UPROPERTY()
	class UUserWidget* PhotoModeUI;

	UPROPERTY(EditAnywhere, Category = "UI")
	bool bShowRuleOfThirdsGrid;

	UPROPERTY(EditAnywhere, Category = "UI")
	bool bShowFocusPoint;

private:
	// Internal methods
	void InitializePhotoCamera();
	void UpdateCameraPosition(float DeltaTime);
	void UpdateOrbitCamera(float DeltaTime);
	void ApplyPostProcessEffects();
	void ApplyFilterEffect(EPhotoFilter Filter);
	void OverrideLighting();
	void ApplyVehiclePose();
	void CaptureScreenshot(int32 Width, int32 Height, const FString& Filename);
	void SaveScreenshotMetadata(const FScreenshotMetadata& Metadata);
	void LoadScreenshotGallery();
	FString GenerateScreenshotFilename() const;
	void RestoreOriginalSettings();
	void ShowPhotoModeUI(bool bShow);
	void DrawRuleOfThirdsGrid();
	void DrawFocusPointIndicator();
};
