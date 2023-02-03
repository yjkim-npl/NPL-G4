#ifndef OpDetectorConstruction_h
#define OpDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "OpMaterials.hh"
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

	private:
		OpParameterContainer* PC;
		OpMaterials* fMaterials;

};

#endif
