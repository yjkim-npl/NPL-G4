#include "OpDetectorConstruction.hh"
#include "OpParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SystemOfUnits.hh"

OpDetectorConstruction::OpDetectorConstruction()
: G4VUserDetectorConstruction()
{
	PC = OpParameterContainer::GetInstance();
	fMaterials = new OpMaterials();

	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpDetectorConstruction" << G4endl;
}

OpDetectorConstruction::~OpDetectorConstruction()
{ 
	delete fMaterials;

	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpDetectorConstruction" << G4endl;
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

	// to translate the center of SC1 at Z=0
	G4double translation = PC -> GetParDouble("Translation");

	// SC
	if(PC -> GetParBool("SC1In"))
	{
		G4int ID = PC -> GetParInt("SC1ID");
		G4double sizeX = PC -> GetParDouble("SC1_sizeX");
		G4double sizeY = PC -> GetParDouble("SC1_sizeY");
		G4double sizeZ = PC -> GetParDouble("SC1_sizeZ");
		G4double ZOffset = PC -> GetParDouble("SC1_ZOffset");
		G4Material* mat;
		if(PC -> GetParInt("SC1matOpt") == 0)
			mat = fMaterials -> GetMaterial("Polystyrene");
		else if(PC -> GetParInt("SC1matOpt") == 1)
			mat = fMaterials -> GetMaterial("Scintillator");
		else
			mat = fMaterials -> GetMaterial("Polystyrene");

		G4Box* solidSC1 =
			new G4Box("SC1",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		G4LogicalVolume* logicSC1 = 
			new G4LogicalVolume(solidSC1,mat,"SC1");
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-translation),logicSC1,"SC1",logicWorld,false,ID+1,checkOverlaps);

		G4VisAttributes* attSC = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attSC -> SetVisibility(true);
		attSC -> SetForceWireframe(true);
		logicSC1 -> SetVisAttributes(attSC);
	}
	if(PC -> GetParBool("SC2In"))
	{
		G4int ID = PC -> GetParInt("SC2ID");
		G4double sizeX = PC -> GetParDouble("SC2_sizeX");
		G4double sizeY = PC -> GetParDouble("SC2_sizeY");
		G4double sizeZ = PC -> GetParDouble("SC2_sizeZ");
		G4double ZOffset = PC -> GetParDouble("SC2_ZOffset");
		G4Material* mat;
		if(PC -> GetParInt("SC2matOpt") == 0)
			mat = fMaterials -> GetMaterial("Polystyrene");
		else if(PC -> GetParInt("SC2matOpt") == 1)
			mat = fMaterials -> GetMaterial("Scintillator");
		else
			mat = fMaterials -> GetMaterial("Polystyrene");

		G4Box* solidSC2 =
			new G4Box("SC2",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		G4LogicalVolume* logicSC2 = 
			new G4LogicalVolume(solidSC2,mat,"SC2");
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-translation),logicSC2,"SC2",logicWorld,false,ID+1,checkOverlaps);

		G4VisAttributes* attSC = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attSC -> SetVisibility(true);
		attSC -> SetForceWireframe(true);
		logicSC2 -> SetVisAttributes(attSC);
	}
	if(PC -> GetParBool("SC3In"))
	{
		G4int ID = PC -> GetParInt("SC3ID");
		G4double sizeX = PC -> GetParDouble("SC3_sizeX");
		G4double sizeY = PC -> GetParDouble("SC3_sizeY");
		G4double sizeZ = PC -> GetParDouble("SC3_sizeZ");
		G4double ZOffset = PC -> GetParDouble("SC3_ZOffset");
		G4Material* mat;
		if(PC -> GetParInt("SC3matOpt") == 0)
			mat = fMaterials -> GetMaterial("Polystyrene");
		else if(PC -> GetParInt("SC3matOpt") == 1)
			mat = fMaterials -> GetMaterial("Scintillator");
		else
			mat = fMaterials -> GetMaterial("Polystyrene");

		G4Box* solidSC3 =
			new G4Box("SC3",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		G4LogicalVolume* logicSC3 = 
			new G4LogicalVolume(solidSC3,mat,"SC3");
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-translation),logicSC3,"SC3",logicWorld,false,ID+1,checkOverlaps);

		G4VisAttributes* attSC = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attSC -> SetVisibility(true);
		attSC -> SetForceWireframe(true);
		logicSC3 -> SetVisAttributes(attSC);
	}


	if(PC -> GetParBool("SensorIn"))
	{
		G4int SensorID = PC -> GetParInt("SensorID");
		G4double sizeX = PC -> GetParDouble("Sensor_sizeX");
		G4double sizeY = PC -> GetParDouble("Sensor_sizeY");
		G4double sizeZ = PC -> GetParDouble("Sensor_sizeZ");
		G4double totY  = PC -> GetParDouble("Sensor_totY");
		G4double gap   = PC -> GetParDouble("Sensor_gap");
		G4double SC1_sizeX = PC -> GetParDouble("SC1_sizeX");
		G4double SC2_sizeX = PC -> GetParDouble("SC2_sizeX");
		G4double SC3_sizeX = PC -> GetParDouble("SC3_sizeX");
		G4double SC1ZOffset = PC -> GetParDouble("SC1_ZOffset");
		G4double SC2ZOffset = PC -> GetParDouble("SC2_ZOffset");
		G4double SC3ZOffset = PC -> GetParDouble("SC3_ZOffset");
		G4int nReplica = 21;	// 96.85 = 3.85*21+0.8*20
		G4double width_Replica = 0.8 * mm;
		G4Material* mat_Sensor = fMaterials -> GetMaterial("Silicon");

		if(PC -> GetParBool("Sensor_gapIn"))
		{
			G4Box* solid_area = 
				new G4Box("solid_Sensor_area",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
			G4LogicalVolume* logic_area = 
				new G4LogicalVolume(solid_area,fMaterials->GetMaterial("G4_AIR"),"logic_Sensor_area");
			if(PC -> GetParBool("SC1In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-translation),logic_area,"Area1L",logicWorld,false,SensorID+1,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-translation),logic_area,"Area1R",logicWorld,false,SensorID+2,checkOverlaps);
			}
			if(PC -> GetParBool("SC2In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC2_sizeX+sizeX),0,SC2ZOffset-translation),logic_area,"Area2L",logicWorld,false,SensorID+3,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC2_sizeX+sizeX),0,SC2ZOffset-translation),logic_area,"Area2R",logicWorld,false,SensorID+4,checkOverlaps);
			}
			if(PC -> GetParBool("SC3In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC3_sizeX+sizeX),0,SC3ZOffset-translation),logic_area,"Area2L",logicWorld,false,SensorID+5,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC3_sizeX+sizeX),0,SC3ZOffset-translation),logic_area,"Area2R",logicWorld,false,SensorID+6,checkOverlaps);
			}

			G4Box* solid_sensor = 
				new G4Box("solid_Sensor",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
			G4LogicalVolume* logic_sensor = 
				new G4LogicalVolume(solid_sensor,mat_Sensor,"logic_Sensor");
			new G4PVReplica("Sensor",logic_sensor,logic_area,kYAxis,nReplica,width_Replica+sizeY,0);
		}
		else
		{
			G4Box*solid_sensor = 
				new G4Box("solid_Sensor",0.5*sizeX,0.5*totY,0.5*sizeZ);
			G4LogicalVolume* logic_sensor = 
				new G4LogicalVolume(solid_sensor,mat_Sensor,"logic_Sensor");
			if(PC->GetParBool("SC1In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-translation),logic_sensor,"SensorL",logicWorld,false,SensorID+1,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-translation),logic_sensor,"SensorR",logicWorld,false,SensorID+2,checkOverlaps);
			}
			if(PC->GetParBool("SC2In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*SC2_sizeX+sizeX,0,SC2ZOffset-translation),logic_sensor,"SensorL",logicWorld,false,SensorID+3,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC2_sizeX+sizeX),0,SC2ZOffset-translation),logic_sensor,"SensorR",logicWorld,false,SensorID+4,checkOverlaps);
			}
			if(PC->GetParBool("SC3In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*SC3_sizeX+sizeX,0,SC3ZOffset-translation),logic_sensor,"SensorL",logicWorld,false,SensorID+5,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC3_sizeX+sizeX),0,SC3ZOffset-translation),logic_sensor,"SensorR",logicWorld,false,SensorID+6,checkOverlaps);
			}
		}
	}
	if(PC -> GetParBool("BDCIn"))
	{
		G4int ID = PC -> GetParInt("BDCID");
		G4double sizeX = PC -> GetParDouble("BDC_sizeX");
		G4double sizeY = PC -> GetParDouble("BDC_sizeY");
		G4double sizeZ = PC -> GetParDouble("BDC_sizeZ");
		G4double ZOffset = PC -> GetParDouble("BDC_ZOffset");
		G4Material* mat = fMaterials -> GetMaterial("P10Gas");

		G4Box* solidBDC = 
			new G4Box("solidBDC",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		G4LogicalVolume* logicBDC = 
			new G4LogicalVolume(solidBDC,mat,"logicBDC");
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-translation),logicBDC,"BDC",logicWorld,false,ID+1,checkOverlaps);

		G4VisAttributes* attBDC = new G4VisAttributes(G4Colour(G4Colour::Brown()));
		attBDC -> SetVisibility(true);
		attBDC -> SetForceWireframe(true);
		logicBDC -> SetVisAttributes(attBDC);
	}

	if(PC -> GetParBool("BTOFIn"))
	{
		G4int ID = PC -> GetParInt("BTOFID");
		G4double sizeX = PC -> GetParDouble("BTOF_sizeX");
		G4double sizeY = PC -> GetParDouble("BTOF_sizeY");
		G4double sizeZ = PC -> GetParDouble("BTOF_sizeZ");
		G4double fishX = PC -> GetParDouble("BTOF_fishX");
		G4double ZOffset1 = PC -> GetParDouble("BTOF_ZOffset1");
		G4double ZOffset2 = PC -> GetParDouble("BTOF_ZOffset2");
		G4Material* mat = fMaterials -> GetMaterial("Scintillator");

		if(PC->GetParInt("BTOFopt")==0)
		{
			G4Box* solidBTOF = 
				new G4Box("solidBTOF",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
			G4LogicalVolume* logicBTOF = 
				new G4LogicalVolume(solidBTOF,mat,"logicBTOF");
			new G4PVPlacement(0,G4ThreeVector(0,0,(ZOffset1-translation)),logicBTOF,"BTOF1",logicWorld,false,ID+1,checkOverlaps);
			new G4PVPlacement(0,G4ThreeVector(0,0,(ZOffset2-translation)),logicBTOF,"BTOF2",logicWorld,false,ID+2,checkOverlaps);

			G4VisAttributes* attBTOF = new G4VisAttributes(G4Colour(G4Colour::Grey()));
			attBTOF -> SetVisibility(true);
			attBTOF -> SetForceWireframe(true);
			logicBTOF -> SetVisAttributes(attBTOF);
		}
	}
	return physWorld;
}
