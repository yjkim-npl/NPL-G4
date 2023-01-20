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

OpDetectorConstruction::OpDetectorConstruction(OpParameterContainer* par)
: G4VUserDetectorConstruction()
{
	PC = par;
}

OpDetectorConstruction::~OpDetectorConstruction()
{ }

G4VPhysicalVolume* OpDetectorConstruction::Construct()
{  
	// Get nist material manager
	G4NistManager* nist = G4NistManager::Instance();

	// Envelope parameters
	G4double env_sizeXY = 20*cm, env_sizeZ = 30*cm;
	G4Material* env_mat = nist->FindOrBuildMaterial("G4_WATER");
 
	// Option to switch on/off checking of volumes overlaps
	G4bool checkOverlaps = true;

	// World
	G4int worldID = PC -> GetParInt("worldID");
	G4double world_sizeX = PC -> GetParDouble("world_sizeX");
	G4double world_sizeY = PC -> GetParDouble("world_sizeY");
	G4double world_sizeZ = PC -> GetParDouble("world_sizeZ");
	G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

	G4Box* solidWorld =    
		new G4Box("World",0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);
	G4LogicalVolume* logicWorld =                         
		new G4LogicalVolume(solidWorld,world_mat,"World");
	G4VPhysicalVolume* physWorld = 
		new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,worldID,checkOverlaps);  

	// Box
	G4int boxID = PC -> GetParInt("BoxID");
	G4double box_sizeX = PC -> GetParDouble("Box_sizeX");
	G4double box_sizeY = PC -> GetParDouble("Box_sizeY");
	G4double box_sizeZ = PC -> GetParDouble("Box_sizeZ");
	G4Material* mat_box = nist -> FindOrBuildMaterial("G4_Pb");

	G4Box* solidBox =
		new G4Box("Box",0.5*box_sizeX,0.5*box_sizeY,0.5*box_sizeZ);
	G4LogicalVolume* logicBox = 
		new G4LogicalVolume(solidBox,mat_box,"Box");
	new G4PVPlacement(0,G4ThreeVector(),logicBox,"Box",logicWorld,false,boxID,checkOverlaps);

	G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Gray()));
	attBox -> SetVisibility(true);
	attBox -> SetForceWireframe(true);
	logicBox -> SetVisAttributes(attBox);

	return physWorld;
}
