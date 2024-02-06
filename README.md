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
#### 06.Feb
 - `OpDetectorConstruction.cc` & `OpMaterials.cc`: modified the option for non-optical simulation 
