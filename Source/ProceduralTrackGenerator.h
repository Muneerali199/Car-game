// ProceduralTrackGenerator.h - Procedural racing track generation system
// Creates dynamic race tracks with realistic layout, elevation, and track features

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "ProceduralTrackGenerator.generated.h"

// Track type categories
UENUM(BlueprintType)
enum class ETrackType : uint8
{
	Circuit,        // Closed loop racing
	Sprint,         // Point-to-point
	Oval,           // NASCAR-style oval
	Hillclimb,      // Uphill mountain course
	Rally,          // Off-road stages
	Drift,          // Drift-optimized layout
	Drag,           // Straight acceleration strip
	Technical,      // Tight, complex corners
	HighSpeed,      // Fast, sweeping turns
	Street          // City street circuit
};

// Track difficulty
UENUM(BlueprintType)
enum class ETrackDifficulty : uint8
{
	Beginner,       // Wide, simple layout
	Intermediate,   // Moderate complexity
	Advanced,       // Challenging corners
	Expert,         // Extreme technical skill required
	Professional    // Real-world circuit complexity
};

// Corner types
UENUM(BlueprintType)
enum class ECornerType : uint8
{
	Slow,           // < 60 km/h, tight hairpin
	Medium,         // 60-120 km/h, standard turn
	Fast,           // 120-180 km/h, sweeper
	Chicane,        // Quick left-right-left
	Hairpin,        // 180-degree turn
	Esses,          // S-curves
	Banked,         // High-speed banked turn
	Decreasing,     // Radius tightens mid-corner
	Increasing      // Radius opens mid-corner
};

// Track segment data
USTRUCT(BlueprintType)
struct FTrackSegment
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector StartLocation;

	UPROPERTY(BlueprintReadWrite)
	FVector EndLocation;

	UPROPERTY(BlueprintReadWrite)
	FVector StartTangent;

	UPROPERTY(BlueprintReadWrite)
	FVector EndTangent;

	UPROPERTY(BlueprintReadWrite)
	ECornerType CornerType;

	UPROPERTY(BlueprintReadWrite)
	float SegmentLength;

	UPROPERTY(BlueprintReadWrite)
	float TrackWidth;

	UPROPERTY(BlueprintReadWrite)
	float BankingAngle; // Degrees, 0 = flat, positive = banked

	UPROPERTY(BlueprintReadWrite)
	float ElevationChange; // Meters

	UPROPERTY(BlueprintReadWrite)
	float RecommendedSpeed; // km/h

	UPROPERTY(BlueprintReadWrite)
	bool bHasRumbleStrips;

	UPROPERTY(BlueprintReadWrite)
	bool bHasSafetyBarriers;

	FTrackSegment()
		: StartLocation(FVector::ZeroVector)
		, EndLocation(FVector::ZeroVector)
		, StartTangent(FVector::ForwardVector)
		, EndTangent(FVector::ForwardVector)
		, CornerType(ECornerType::Medium)
		, SegmentLength(100.0f)
		, TrackWidth(12.0f)
		, BankingAngle(0.0f)
		, ElevationChange(0.0f)
		, RecommendedSpeed(100.0f)
		, bHasRumbleStrips(false)
		, bHasSafetyBarriers(true)
	{}
};

// Complete track layout
USTRUCT(BlueprintType)
struct FTrackLayout
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString TrackName;

	UPROPERTY(BlueprintReadWrite)
	ETrackType TrackType;

	UPROPERTY(BlueprintReadWrite)
	ETrackDifficulty Difficulty;

	UPROPERTY(BlueprintReadWrite)
	TArray<FTrackSegment> Segments;

	UPROPERTY(BlueprintReadWrite)
	float TotalLength; // Meters

	UPROPERTY(BlueprintReadWrite)
	int32 NumberOfCorners;

	UPROPERTY(BlueprintReadWrite)
	float ElevationGain; // Total uphill meters

	UPROPERTY(BlueprintReadWrite)
	float ElevationLoss; // Total downhill meters

	UPROPERTY(BlueprintReadWrite)
	float LongestStraight; // Meters

	UPROPERTY(BlueprintReadWrite)
	float AverageWidth; // Meters

	UPROPERTY(BlueprintReadWrite)
	FVector StartLineLocation;

	UPROPERTY(BlueprintReadWrite)
	FRotator StartLineRotation;

	FTrackLayout()
		: TrackName(TEXT("Unnamed Track"))
		, TrackType(ETrackType::Circuit)
		, Difficulty(ETrackDifficulty::Intermediate)
		, TotalLength(0.0f)
		, NumberOfCorners(0)
		, ElevationGain(0.0f)
		, ElevationLoss(0.0f)
		, LongestStraight(0.0f)
		, AverageWidth(12.0f)
		, StartLineLocation(FVector::ZeroVector)
		, StartLineRotation(FRotator::ZeroRotator)
	{}
};

// Track feature placement
USTRUCT(BlueprintType)
struct FTrackFeature
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> FeatureClass; // Billboard, grandstand, pit building, etc.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceAlongTrack; // 0-1, position on track

	FTrackFeature()
		: Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
		, FeatureClass(nullptr)
		, DistanceAlongTrack(0.0f)
	{}
};

// Generation parameters
USTRUCT(BlueprintType)
struct FTrackGenerationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETrackType TrackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETrackDifficulty Difficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DesiredLength; // Meters (approximate)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TrackWidth; // Meters

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinCorners;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCorners;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxElevationChange; // Meters per segment

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StraightProbability; // 0-1, how often to add straights

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIncludeBanking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIncludeRumbleStrips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIncludePitLane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RandomSeed; // For reproducible generation

	FTrackGenerationParams()
		: TrackType(ETrackType::Circuit)
		, Difficulty(ETrackDifficulty::Intermediate)
		, DesiredLength(3000.0f)
		, TrackWidth(12.0f)
		, MinCorners(8)
		, MaxCorners(16)
		, MaxElevationChange(20.0f)
		, StraightProbability(0.3f)
		, bIncludeBanking(true)
		, bIncludeRumbleStrips(true)
		, bIncludePitLane(true)
		, RandomSeed(0)
	{}
};

/**
 * AProceduralTrackGenerator - Generates realistic racing tracks procedurally
 * Features:
 * - Multiple track types (circuit, sprint, oval, rally, etc.)
 * - Realistic corner types and combinations
 * - Elevation changes and banking
 * - Track width variation (braking zones, overtaking spots)
 * - Automatic checkpoint placement
 * - Track surface mesh generation
 * - Kerb and rumble strip placement
 * - Safety barrier generation
 * - Pit lane and paddock areas
 * - Environment decoration (trees, buildings, crowds)
 * - AI racing line calculation
 * - Track validation (no intersections, proper closure)
 */
UCLASS()
class CARGAME_API AProceduralTrackGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralTrackGenerator();

protected:
	virtual void BeginPlay() override;

public:	
	// Track generation
	UFUNCTION(BlueprintCallable, Category = "Track Generator")
	FTrackLayout GenerateTrack(const FTrackGenerationParams& Params);

	UFUNCTION(BlueprintCallable, Category = "Track Generator")
	void GenerateAndBuildTrack(const FTrackGenerationParams& Params);

	UFUNCTION(BlueprintCallable, Category = "Track Generator")
	void ClearGeneratedTrack();

	// Specific track types
	UFUNCTION(BlueprintCallable, Category = "Track Generator|Presets")
	FTrackLayout GenerateCircuitTrack(float Length, ETrackDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Presets")
	FTrackLayout GenerateOvalTrack(float Length, float BankingAngle);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Presets")
	FTrackLayout GenerateDragStrip(float Length);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Presets")
	FTrackLayout GenerateRallyStage(float Length, float Roughness);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Presets")
	FTrackLayout GenerateHillclimb(float Length, float ElevationGain);

	// Track building (mesh generation)
	UFUNCTION(BlueprintCallable, Category = "Track Generator|Building")
	void BuildTrackMesh(const FTrackLayout& Layout);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Building")
	void BuildTrackSurface(const FTrackLayout& Layout);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Building")
	void BuildKerbsAndRumbleStrips(const FTrackLayout& Layout);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Building")
	void BuildSafetyBarriers(const FTrackLayout& Layout);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Building")
	void BuildPitLane(const FTrackLayout& Layout);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Building")
	void PlaceCheckpoints(const FTrackLayout& Layout);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Building")
	void PlaceEnvironmentObjects(const FTrackLayout& Layout);

	// Track analysis
	UFUNCTION(BlueprintPure, Category = "Track Generator|Analysis")
	float CalculateTrackLength(const FTrackLayout& Layout) const;

	UFUNCTION(BlueprintPure, Category = "Track Generator|Analysis")
	TArray<FVector> CalculateRacingLine(const FTrackLayout& Layout, int32 PointsPerSegment = 10) const;

	UFUNCTION(BlueprintPure, Category = "Track Generator|Analysis")
	bool ValidateTrack(const FTrackLayout& Layout, FString& OutErrorMessage) const;

	UFUNCTION(BlueprintPure, Category = "Track Generator|Analysis")
	float EstimateLapTime(const FTrackLayout& Layout, float VehiclePerformance) const;

	// Track modification
	UFUNCTION(BlueprintCallable, Category = "Track Generator|Modification")
	void SmoothTrackLayout(FTrackLayout& Layout, int32 Iterations = 3);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Modification")
	void AddElevationVariation(FTrackLayout& Layout, float MaxChange);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|Modification")
	void AddWidthVariation(FTrackLayout& Layout, float WidthChangePercent);

	// Import/Export
	UFUNCTION(BlueprintCallable, Category = "Track Generator|IO")
	bool SaveTrackLayout(const FTrackLayout& Layout, const FString& Filename);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|IO")
	FTrackLayout LoadTrackLayout(const FString& Filename, bool& bSuccess);

	UFUNCTION(BlueprintCallable, Category = "Track Generator|IO")
	void ExportTrackToJSON(const FTrackLayout& Layout, const FString& Filename);

protected:
	// Spline component for track path
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USplineComponent* TrackSpline;

	// Generation parameters
	UPROPERTY(EditAnywhere, Category = "Generation")
	FTrackGenerationParams DefaultGenerationParams;

	// Mesh assets
	UPROPERTY(EditDefaultsOnly, Category = "Assets|Track")
	UStaticMesh* TrackSurfaceMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Track")
	UMaterialInterface* AsphaltMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Track")
	UMaterialInterface* ConcreteMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Track")
	UStaticMesh* KerbMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Track")
	UStaticMesh* RumbleStripMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Barriers")
	UStaticMesh* TireBarrierMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Barriers")
	UStaticMesh* ArmocoBarrierMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Barriers")
	UStaticMesh* CatchFenceMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Features")
	TArray<UStaticMesh*> BillboardMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Features")
	TArray<UStaticMesh*> GrandstandMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Features")
	TArray<UStaticMesh*> TreeMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Assets|Features")
	UStaticMesh* PitBuildingMesh;

	// Current layout
	UPROPERTY(BlueprintReadOnly, Category = "Track")
	FTrackLayout CurrentLayout;

	// Generated actors (for cleanup)
	UPROPERTY()
	TArray<AActor*> GeneratedActors;

private:
	// Generation algorithms
	FTrackLayout GenerateCircuitLayout(const FTrackGenerationParams& Params);
	FTrackLayout GenerateSprintLayout(const FTrackGenerationParams& Params);
	FTrackLayout GenerateOvalLayout(const FTrackGenerationParams& Params);
	
	// Segment generation
	FTrackSegment CreateStraightSegment(FVector Start, FVector Direction, float Length, float Width);
	FTrackSegment CreateCornerSegment(FVector Start, FVector Direction, ECornerType CornerType, float Angle, float Width);
	FTrackSegment CreateChicaneSegment(FVector Start, FVector Direction, float Width);
	
	// Layout helpers
	void ConnectSegments(FTrackSegment& Segment1, FTrackSegment& Segment2);
	bool CheckSegmentIntersection(const FTrackSegment& Segment1, const FTrackSegment& Segment2) const;
	void CloseTrackLoop(FTrackLayout& Layout);
	void OptimizeTrackFlow(FTrackLayout& Layout);
	
	// Mesh generation
	void GenerateProceduralMesh(const TArray<FVector>& Points, float Width, UStaticMesh* Mesh);
	TArray<FVector> SplineToPoints(USplineComponent* Spline, int32 NumPoints) const;
	void CreateMeshSection(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs);
	
	// Placement helpers
	FVector GetLocationAlongTrack(const FTrackLayout& Layout, float Distance) const;
	FRotator GetRotationAlongTrack(const FTrackLayout& Layout, float Distance) const;
	float GetTrackCurvature(const FTrackLayout& Layout, float Distance) const;
	
	// Validation
	bool CheckMinimumTrackWidth(const FTrackLayout& Layout) const;
	bool CheckProperClosure(const FTrackLayout& Layout) const;
	bool CheckNoSelfIntersections(const FTrackLayout& Layout) const;
	
	// Random generation utilities
	FRandomStream RandomStream;
	ECornerType PickRandomCornerType(ETrackDifficulty Difficulty);
	float GetCornerAngle(ECornerType CornerType) const;
	float GetCornerRadius(ECornerType CornerType, float Speed) const;
};
