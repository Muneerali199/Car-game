# Vehicle Physics System Documentation

## Overview
The Racing Vehicle system implements high-fidelity vehicle physics suitable for AAA racing games. It uses Unreal Engine 5's Chaos physics with custom extensions for realistic handling.

## Architecture

### Core Classes
- **ARacingVehicle**: Main vehicle pawn class
- **FVehicleTelemetry**: Telemetry data structure
- Vehicle movement handled by `UChaosWheeledVehicleMovementComponent`

### Key Systems

#### 1. Tire Models
Three tire friction models are supported:

**Simple Model**
- Basic friction calculation
- Best for arcade-style gameplay
- Low computational cost

**Pacejka Model (MF6.1)**
- Industry-standard tire model
- Accurate slip angle and slip ratio curves
- Parameters: B (stiffness), C (shape), D (peak), E (curvature)
- Formula: `F = D * sin(C * atan(B * slip - E * (B * slip - atan(B * slip))))`

**Brush Model**
- Physical bristle deflection simulation
- Good balance of accuracy and performance
- Suitable for most racing scenarios

#### 2. Suspension System
Multi-link suspension with configurable parameters:
- Spring rate (N/m)
- Damper coefficient (N·s/m)
- Anti-roll bar stiffness
- Suspension travel limits
- Ride height adjustment

#### 3. Drivetrain
Configurable power distribution:
- **FWD**: Front wheels powered (e.g., 100/0)
- **RWD**: Rear wheels powered (e.g., 0/100)
- **AWD**: All wheels powered (e.g., 50/50)
- **Custom**: User-defined split

#### 4. Engine Simulation
- Torque curve (from UCurveFloat asset)
- RPM range: Idle to Max
- Gear ratios
- Final drive ratio
- Engine braking

#### 5. Aerodynamics
**Drag Force**
```
F_drag = 0.5 * ρ * Cd * A * v²
```
Where:
- ρ = air density (1.225 kg/m³)
- Cd = drag coefficient (0.25-0.35 for race cars)
- A = frontal area (m²)
- v = velocity (m/s)

**Downforce**
```
F_downforce = 0.5 * ρ * Cl * A * v²
```
- Cl = lift coefficient (negative for downforce)
- Applied at center of pressure

#### 6. Driving Assists

**ABS (Anti-lock Braking System)**
- Monitors wheel slip
- Modulates brake pressure
- Prevents wheel lock-up
- Maintains steering control

**Traction Control**
- Monitors drive wheel slip
- Reduces engine power when slip exceeds threshold
- Prevents wheel spin under acceleration

**Stability Control**
- Monitors yaw rate vs steering input
- Applies individual wheel braking
- Prevents oversteer/understeer

## Telemetry System

### Real-time Data
The `FVehicleTelemetry` structure provides:
- Speed (km/h)
- Engine RPM
- Current gear
- Throttle/brake/steering inputs
- Suspension compression per wheel
- G-forces (lateral and longitudinal)
- Velocity vectors

### Data Export
```cpp
ExportTelemetryToFile("C:/Telemetry/session_001.csv");
```

Output format (CSV):
```
Time,Speed,RPM,Gear,Throttle,Brake,Steering,LatG,LongG
0.00,0.0,1000,1,0.00,0.00,0.00,0.00,0.00
0.10,5.2,2500,1,0.50,0.00,0.20,0.15,0.30
```

## Configuration

### Blueprint Setup
1. Create Blueprint based on `ARacingVehicle`
2. Assign skeletal mesh for vehicle body
3. Configure wheel colliders
4. Set tire model and drivetrain type
5. Tune engine curve asset
6. Adjust mass and center of mass
7. Configure aerodynamic parameters

### C++ Usage
```cpp
// Create vehicle instance
ARacingVehicle* Vehicle = GetWorld()->SpawnActor<ARacingVehicle>();

// Configure
Vehicle->TireModelType = ETireModel::Pacejka;
Vehicle->DrivetrainType = EDrivetrainType::RWD;
Vehicle->MaxEngineTorque = 650.0f;
Vehicle->VehicleMass = 1300.0f;

// Enable assists
Vehicle->bABSEnabled = true;
Vehicle->bTractionControlEnabled = true;

// Enable telemetry
Vehicle->bEnableTelemetryLogging = true;
Vehicle->TelemetryUpdateRate = 0.1f;
```

## Performance Optimization

### Tick Rate
- Full physics: Every frame
- Telemetry update: Configurable (default 0.1s)
- Aerodynamics: Every frame (can be reduced for distant vehicles)

### LOD Strategy
- **LOD0** (< 10m): Full physics, full visual detail
- **LOD1** (10-50m): Simplified physics, reduced mesh
- **LOD2** (50-200m): Basic physics, low-poly mesh
- **LOD3** (> 200m): Kinematic motion, impostor/billboard

### Multithreading
- Physics calculated on physics thread
- Telemetry aggregation on game thread
- Input processing on main thread

## Tuning Guide

### Understeer vs Oversteer
**Increase understeer (safer):**
- Increase front tire grip
- Decrease rear tire grip
- Move CoM forward
- Soften rear suspension

**Increase oversteer (faster in skilled hands):**
- Decrease front tire grip
- Increase rear tire grip
- Move CoM rearward
- Stiffen rear suspension

### Responsiveness
- Lower spring rates = more body roll, slower response
- Higher damping = less oscillation, more planted
- Lower mass = quicker acceleration/deceleration
- Lower CoM = less weight transfer

### Top Speed vs Acceleration
- Lower gear ratios = better acceleration, lower top speed
- Higher final drive ratio = opposite
- Balance engine torque curve for desired characteristic

## Network Replication

The vehicle movement component is replicated for multiplayer:
```cpp
VehicleMovement->SetIsReplicated(true);
```

### Client-Server Model
- Server: Authoritative physics simulation
- Client: Prediction with server reconciliation
- Interpolation for smooth remote vehicle display

### Bandwidth Optimization
- Send compressed state snapshots
- Delta compression for unchanged values
- Priority-based update frequency

## Future Enhancements
- [ ] Tire temperature and wear simulation
- [ ] Fuel consumption and weight reduction
- [ ] Damage model with performance impact
- [ ] Adaptive AI using telemetry learning
- [ ] Weather effects (rain, ice, wind)
- [ ] Advanced diff models (open, limited-slip, locked)

## References
- Pacejka, H. B. (2012). *Tire and Vehicle Dynamics*
- Milliken, W. F., & Milliken, D. L. (1995). *Race Car Vehicle Dynamics*
- Unreal Engine 5 Documentation: Chaos Vehicle System
