# Optical photon simulation

## Required geant4 version
 - Geant4-10.7 stable version

## Install

### 1. Clone source
```bash
git clone https:://github.com/sh-lim/PNU-NPL.git
```

### 2. Compile
```bash
cd Geant4/Geant4_simulation/Opsim
mkdir build
cd build
cmake ..
make
```
### 3. Manage parameter.conf
To manage parameter.conf, link of parameter file is required.
and also if you want to make another parameter.conf or want to link , make and link it to the build directory

and linking other macros including vis.mac and run1.mac make easier to manage
```bash
cd build
ln -sf ../OpParameter.conf .
ln -sf ../run1.mac .
ln -sf ../vis.mac .
```

### 4. Run
You can run this example with simple command.
```bash
./Opsim
```
The default setting of parameter container is "OpParameter.conf" in build directory

### 5. development log

#### 26.Jan 
 - applied random number to X,Y and Z of beam starting point with its unit.
 - renamed function name 'read,'Print' to 'ReadParameters',"PrintParameter".
 - unified the style (int, Int_t, G4int) to G4int.

#### 30.Jan
 - checked Random Seed and output
 - applying seed at G4sim.cc is effective to PrimaryGeneratorAction
 - checked 10000 different outputs of flat distribution of G4UniformRand 
 - /random/setSeed command in run1.mac was removed because it is not effective

#### 31.Jan
 - `Opsim.cc`: batch mode condition was modified (argc>2 => argc>=2)
 - `Opsim.cc`: time based seed was implemented
