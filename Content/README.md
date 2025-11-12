# Content Directory

This directory contains all Unreal Engine assets:

## Structure

```
Content/
├── Blueprints/        # Blueprint classes and logic
│   ├── Vehicles/      # Vehicle blueprints
│   ├── GameModes/     # Game mode blueprints
│   └── UI/            # UI blueprints
│
├── Maps/              # Level files
│   ├── TestTrack      # Initial test track
│   └── Menus/         # Menu levels
│
├── Vehicles/          # Vehicle assets
│   ├── Meshes/        # 3D models (.fbx, .uasset)
│   ├── Materials/     # Material instances
│   ├── Textures/      # Texture files
│   ├── Physics/       # Physics assets
│   └── Audio/         # Vehicle sounds
│
├── Tracks/            # Race track assets
│   ├── Meshes/
│   ├── Materials/
│   ├── Textures/
│   └── Foliage/
│
├── Environment/       # Environment assets
│   ├── Landscape/
│   ├── Props/
│   ├── Sky/
│   └── Lighting/
│
├── UI/                # User interface
│   ├── Textures/
│   ├── Widgets/
│   └── Fonts/
│
├── VFX/               # Visual effects
│   ├── Niagara/       # Niagara systems
│   ├── Particles/     # Legacy particles
│   └── Materials/     # VFX materials
│
├── Audio/             # Sound effects and music
│   ├── Engine/
│   ├── Ambient/
│   ├── UI/
│   └── Music/
│
└── Data/              # Data tables and configs
    ├── Vehicles/      # Vehicle configuration data
    ├── Tracks/        # Track data
    └── GameData/      # Game settings
```

## Asset Naming Conventions

### Prefixes
- **BP_** - Blueprint
- **M_** - Material
- **MI_** - Material Instance
- **T_** - Texture
- **SM_** - Static Mesh
- **SK_** - Skeletal Mesh
- **A_** - Animation
- **PA_** - Physics Asset
- **NS_** - Niagara System
- **S_** - Sound
- **DT_** - Data Table
- **E_** - Enum
- **F_** - Function Library

### Examples
- `BP_RaceCar_SportsCar01`
- `SM_Track_Barrier_Concrete`
- `M_Vehicle_CarPaint_Master`
- `MI_Vehicle_CarPaint_Red`
- `T_Vehicle_Body_BaseColor`
- `NS_TireSmoke`
- `S_Engine_V8_Idle`

## Guidelines

1. **Organization**: Keep assets organized in appropriate folders
2. **Naming**: Follow naming conventions strictly
3. **Reusability**: Create master materials and data tables for shared properties
4. **Performance**: Use LODs for meshes, compress textures appropriately
5. **Source Control**: Commit .uasset files, use Git LFS for large binaries

## Content Creation Pipeline

1. **3D Models**: Maya/Blender → FBX → Import to UE5
2. **Textures**: Substance Painter → PNG/TGA → Import to UE5
3. **Materials**: Create in UE5 Material Editor
4. **Blueprints**: Create in UE5 Blueprint Editor
5. **VFX**: Create in UE5 Niagara Editor

## Getting Started

1. Open `CarGame.uproject` in Unreal Editor
2. Create a new map: File → New Level → Empty Level
3. Save as: `Content/Maps/TestTrack`
4. Add basic geometry and lighting
5. Place `BP_RaceCar_01` (to be created)
6. Press Play to test

## Notes

- This directory is empty initially
- Assets will be created in Unreal Editor
- Large asset packs should be documented in ASSET_SOURCES.md
- Free assets: Quixel Megascans, UE Marketplace
