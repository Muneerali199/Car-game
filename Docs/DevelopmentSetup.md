# Development Setup Guide

## Prerequisites

### Required Software
1. **Visual Studio 2022** (v17.4 or later)
   - Workload: "Game development with C++"
   - Individual components:
     - MSVC v143 C++ build tools
     - Windows 10 SDK (10.0.19041.0 or later)
     - C++ profiling tools
     - C++ AddressSanitizer

2. **Unreal Engine 5.3+**
   - Download from Epic Games Launcher
   - Or build from source: https://github.com/EpicGames/UnrealEngine

3. **Git 2.40+**
   - Download from: https://git-scm.com/

4. **Git LFS**
   - Installed with: `git lfs install`

### Recommended Tools
- **RenderDoc**: Graphics debugging
- **PIX for Windows**: Performance profiling
- **Visual Studio Code**: Lightweight editing
- **Beyond Compare**: Visual diff tool

## Initial Setup

### 1. Clone Repository
```powershell
cd C:\Dev
git clone <repository-url> cargame
cd cargame
```

### 2. Configure Git LFS
```powershell
git lfs install
git lfs pull
```

### 3. Generate Project Files

**If you have UE5 source:**
```powershell
# Navigate to UE5 installation
cd "C:\Program Files\Epic Games\UE_5.3"

# Generate project files
.\Engine\Build\BatchFiles\GenerateProjectFiles.bat "C:\Users\Muneer Ali Subzwari\Desktop\cargame\CarGame.uproject"
```

**If using Epic Launcher UE5:**
- Right-click `CarGame.uproject`
- Select "Generate Visual Studio project files"

### 4. Open Solution
```powershell
# Open in Visual Studio
start CarGame.sln
```

## Building the Project

### Development Build
1. Set configuration to **Development Editor**
2. Set platform to **Win64**
3. Build Solution (Ctrl+Shift+B)

### Shipping Build
1. Set configuration to **Shipping**
2. Build Solution

### Expected Build Times
- Clean build: 15-30 minutes (depends on CPU)
- Incremental build: 30 seconds - 3 minutes

## Running the Game

### From Visual Studio
1. Set `CarGame` as startup project
2. Press F5 to debug or Ctrl+F5 to run without debugging

### From Unreal Editor
1. Open `CarGame.uproject` in UE5 Editor
2. Press Play (Alt+P) for PIE (Play In Editor)
3. Press Standalone (Alt+S) for standalone window

## Project Structure

```
cargame/
├── Source/
│   ├── CarGame/              # Game module
│   │   ├── Private/
│   │   ├── Public/
│   │   └── CarGame.Build.cs
│   ├── CarGameEditor/        # Editor module (optional)
│   └── RacingVehicle.h/cpp   # Vehicle implementation
│
├── Content/
│   ├── Blueprints/           # Blueprint assets
│   ├── Maps/                 # Level files
│   ├── Vehicles/             # Vehicle assets
│   │   ├── Meshes/
│   │   ├── Materials/
│   │   ├── Textures/
│   │   └── Physics/
│   ├── Tracks/               # Race track assets
│   ├── UI/                   # User interface
│   └── VFX/                  # Visual effects
│
├── Config/
│   ├── DefaultEngine.ini     # Engine configuration
│   ├── DefaultGame.ini       # Game settings
│   ├── DefaultInput.ini      # Input bindings
│   └── DefaultEditor.ini     # Editor settings
│
├── Plugins/                  # Third-party plugins
│
├── Tools/
│   ├── Scripts/              # Build/deployment scripts
│   └── Pipeline/             # Asset pipeline tools
│
├── Docs/
│   ├── API/                  # Code documentation
│   ├── Design/               # Design documents
│   └── Tuning/               # Vehicle tuning data
│
├── Builds/                   # Compiled builds (gitignored)
│
├── CI/
│   └── azure-pipelines.yml   # CI/CD configuration
│
├── .gitignore
├── .gitattributes
├── GAME_PLAN.md
├── README.md
└── CarGame.uproject
```

## Creating Your First Vehicle

### Step 1: Create Skeletal Mesh
1. Import FBX vehicle model into `Content/Vehicles/Meshes/`
2. Create Physics Asset:
   - Right-click mesh → Create → Physics Asset
   - Configure collision bodies
   - Set wheel bone names: `Wheel_FL`, `Wheel_FR`, `Wheel_RL`, `Wheel_RR`

### Step 2: Create Vehicle Blueprint
1. Content Browser → Add → Blueprint Class
2. Parent Class: `RacingVehicle`
3. Name: `BP_RaceCar_01`

### Step 3: Configure Components
**Vehicle Mesh:**
- Assign skeletal mesh
- Set location/rotation to center
- Enable simulation: False (handled by vehicle component)

**Wheels:**
- Add 4 wheel setups in Vehicle Movement component
- Assign bone names
- Set radius, width, mass
- Configure suspension (spring, damper, travel)

**Engine:**
- Create torque curve asset
- Assign to `EngineTorqueCurve`
- Set Max RPM (e.g., 7500)
- Set Max Torque (e.g., 500 Nm)

**Transmission:**
- Set gear ratios: [2.9, 2.1, 1.6, 1.3, 1.0, 0.8]
- Final drive: 3.5

**Aerodynamics:**
- Drag Coefficient: 0.3
- Downforce Coefficient: 2.5
- Frontal Area: 2.2 m²

### Step 4: Setup Input
Open `Config/DefaultInput.ini`:
```ini
[/Script/Engine.InputSettings]

+AxisMappings=(AxisName="Throttle",Key=W,Scale=1.0)
+AxisMappings=(AxisName="Throttle",Key=Gamepad_RightTrigger,Scale=1.0)
+AxisMappings=(AxisName="Brake",Key=S,Scale=1.0)
+AxisMappings=(AxisName="Brake",Key=Gamepad_LeftTrigger,Scale=1.0)
+AxisMappings=(AxisName="Steering",Key=A,Scale=-1.0)
+AxisMappings=(AxisName="Steering",Key=D,Scale=1.0)
+AxisMappings=(AxisName="Steering",Key=Gamepad_LeftX,Scale=1.0)

+ActionMappings=(ActionName="ShiftUp",Key=LeftShift)
+ActionMappings=(ActionName="ShiftUp",Key=Gamepad_FaceButton_Top)
+ActionMappings=(ActionName="ShiftDown",Key=LeftControl)
+ActionMappings=(ActionName="ShiftDown",Key=Gamepad_FaceButton_Left)
+ActionMappings=(ActionName="Handbrake",Key=SpaceBar)
+ActionMappings=(ActionName="Handbrake",Key=Gamepad_RightShoulder)
```

### Step 5: Create Test Track
1. File → New Level → Empty Level
2. Add Landscape or BSP for ground
3. Apply physics material: `PM_Track` (friction: 0.9)
4. Place vehicle spawn point
5. Add lighting:
   - Directional Light (sun)
   - Sky Light
   - Post Process Volume (enable infinite extent)

### Step 6: Test Drive
1. Place `BP_RaceCar_01` in level
2. Set as Default Pawn in World Settings
3. Press Play
4. Use WASD or gamepad to drive

## Performance Profiling

### CPU Profiling
**In-Engine:**
```
stat fps
stat unit
stat game
stat scenerendering
```

**External Tools:**
- Visual Studio Profiler: Debug → Performance Profiler
- Unreal Insights: UnrealEditor.exe -trace=cpu,frame,log

### GPU Profiling
```
stat gpu
profilegpu
```

**External:**
- PIX: Capture frame analysis
- RenderDoc: Graphics debugging

### Memory Profiling
```
stat memory
memreport -full
```

## Common Issues & Solutions

### Issue: Project won't open
**Solution:**
- Delete `Binaries/`, `Intermediate/`, `Saved/` folders
- Regenerate project files
- Rebuild in Visual Studio

### Issue: Missing DLLs
**Solution:**
- Ensure Visual Studio C++ redistributables installed
- Check Windows SDK version matches project

### Issue: Compilation errors
**Solution:**
- Verify UE5 version matches (5.3+)
- Check all source files saved
- Clean solution and rebuild

### Issue: Vehicle falls through ground
**Solution:**
- Enable collision on landscape/mesh
- Set collision profile to "BlockAll"
- Verify Physics Asset has collision bodies

### Issue: Poor performance
**Solution:**
- Check `stat unit` for bottleneck (Game/Draw/GPU)
- Reduce shadow quality in Post Process Volume
- Enable Nanite for static meshes
- Use LODs for distant objects

## Next Steps

1. **Week 1-2:** Complete vehicle configuration and test track
2. **Week 3-4:** Implement telemetry UI and replay system
3. **Week 5-6:** Add particle effects and audio
4. **Week 7-8:** Multiplayer networking prototype
5. **Week 9-12:** Polish and optimization

## Resources

- [Unreal Engine 5 Documentation](https://docs.unrealengine.com/)
- [Chaos Vehicles Guide](https://docs.unrealengine.com/5.3/en-US/vehicles-in-unreal-engine/)
- [C++ API Reference](https://docs.unrealengine.com/5.3/en-US/API/)
- Project Discord: [Link TBD]
- Weekly Dev Meetings: Fridays 2 PM

## Getting Help

**Questions?**
- Check documentation in `/Docs`
- Search existing issues on GitHub
- Ask in project Discord #development channel
- Email: dev@cargame.com
