#ifndef OpDetectorConstruction_h
#define OpDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "OpMaterials.hh"
#include "G4UserLimits.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class OpParameterContainer;

class OpDetectorConstruction : public G4VUserDetectorConstruction
{
	public:
		OpDetectorConstruction();

		virtual ~OpDetectorConstruction();

		virtual G4VPhysicalVolume* Construct();
		virtual void ConstructSDandField();

	private:
		OpParameterContainer* PC;
		OpMaterials* fMaterials;

		G4LogicalVolume* logicSC;
		G4LogicalVolume* logicSiPM;
};

#endif
