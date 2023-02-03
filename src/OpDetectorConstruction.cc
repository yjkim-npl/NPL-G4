#include "OpDetectorConstruction.hh"
#include "OpParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

OpDetectorConstruction::OpDetectorConstruction()
: G4VUserDetectorConstruction()
{
	PC = OpParameterContainer::GetInstance();
	fMaterials = new OpMaterials();
}

OpDetectorConstruction::~OpDetectorConstruction()
{ 
	delete fMaterials;
}

G4VPhysicalVolume* OpDetectorConstruction::Construct()
{  
	// Option to switch on/off checking of volumes overlaps
	G4bool checkOverlaps = true;

	// World
	G4int worldID = PC -> GetParInt("WorldID");
	G4double world_sizeX = PC -> GetParDouble("World_sizeX");
	G4double world_sizeY = PC -> GetParDouble("World_sizeY");
	G4double world_sizeZ = PC -> GetParDouble("World_sizeZ");
	G4Material* world_mat = fMaterials -> GetMaterial("G4_AIR");

	G4Box* solidWorld =    
		new G4Box("World",0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);
	G4LogicalVolume* logicWorld =                         
		new G4LogicalVolume(solidWorld,world_mat,"World");
	G4VPhysicalVolume* physWorld = 
		new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,worldID,checkOverlaps);  

	// SC
	if(PC -> GetParBool("SCIn"))
	{
		G4int SCID = PC -> GetParInt("SCID");
		G4double SC_sizeX = PC -> GetParDouble("SC_sizeX");
		G4double SC_sizeY = PC -> GetParDouble("SC_sizeY");
		G4double SC_sizeZ = PC -> GetParDouble("SC_sizeZ");
		G4Material* mat_SC;
		if(PC -> GetParInt("SCmatOpt") == 0)
			mat_SC = fMaterials -> GetMaterial("Polystyrene");
		else if(PC -> GetParInt("SCmatOpt") == 1)
			mat_SC = fMaterials -> GetMaterial("Scintillator");
		else
			mat_SC = fMaterials -> GetMaterial("Polystyrene");

		G4Box* solidSC =
			new G4Box("SC",0.5*SC_sizeX,0.5*SC_sizeY,0.5*SC_sizeZ);
		G4LogicalVolume* logicSC = 
			new G4LogicalVolume(solidSC,mat_SC,"SC");
		new G4PVPlacement(0,G4ThreeVector(),logicSC,"SC",logicWorld,false,SCID,checkOverlaps);

		G4VisAttributes* attSC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
		attSC -> SetVisibility(true);
		attSC -> SetForceWireframe(true);
		logicSC -> SetVisAttributes(attSC);
	}

	return physWorld;
}
