#include "OpDetectorConstruction.hh"
#include "OpSD.hh"
#include "OpSiPMSD.hh"
#include "OpParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SystemOfUnits.hh"

OpDetectorConstruction::OpDetectorConstruction()
: G4VUserDetectorConstruction()
{
	PC = OpParameterContainer::GetInstance();
	fMaterials = new OpMaterials();
	G4double max_step_length = 1*mm;
	fStepLimit = new G4UserLimits(max_step_length);

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
	G4double trans = PC -> GetParDouble("Translation");

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
		logicSC1 = 
			new G4LogicalVolume(solidSC1,mat,"SC1");
		logicSC1 -> SetUserLimits(fStepLimit);
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-trans),logicSC1,"SC1",logicWorld,false,ID+1,checkOverlaps);

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
		logicSC2 = 
			new G4LogicalVolume(solidSC2,mat,"SC2");
		logicSC2 -> SetUserLimits(fStepLimit);

//		G4VPhysicalVolume* phySC2 =  
			new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-trans),logicSC2,"SC2",logicWorld,false,ID+1,checkOverlaps);

//		G4cout << "########" << G4endl;
//		G4cout << "SC2 x: " << -0.5*sizeX << " ~ " << 0.5*sizeX << G4endl;
//		G4cout << "SC2 y: " << -0.5*sizeY << " ~ " << 0.5*sizeY << G4endl;
//		G4cout << "SC2 z: " << ZOffset-trans-0.5*sizeZ << " ~ " << ZOffset-trans+0.5*sizeZ << G4endl;

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
		logicSC3 = 
			new G4LogicalVolume(solidSC3,mat,"SC3");
		logicSC3 -> SetUserLimits(fStepLimit);
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-trans),logicSC3,"SC3",logicWorld,false,ID+1,checkOverlaps);

		G4VisAttributes* attSC = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attSC -> SetVisibility(true);
		attSC -> SetForceWireframe(true);
		logicSC3 -> SetVisAttributes(attSC);
	}


	if(PC -> GetParBool("SiPMIn"))
	{
		G4int SiPMID = PC -> GetParInt("SiPMID");
		G4double sizeX = PC -> GetParDouble("SiPM_sizeX");
		G4double sizeY = PC -> GetParDouble("SiPM_sizeY");
		G4double sizeZ = PC -> GetParDouble("SiPM_sizeZ");
		G4double totY  = PC -> GetParDouble("SiPM_totY");
		G4double gap   = PC -> GetParDouble("SiPM_gap");
		G4double SC1_sizeX = PC -> GetParDouble("SC1_sizeX");
		G4double SC2_sizeX = PC -> GetParDouble("SC2_sizeX");
		G4double SC3_sizeX = PC -> GetParDouble("SC3_sizeX");
		G4double SC1ZOffset = PC -> GetParDouble("SC1_ZOffset");
		G4double SC2ZOffset = PC -> GetParDouble("SC2_ZOffset");
		G4double SC3ZOffset = PC -> GetParDouble("SC3_ZOffset");
		G4int nReplica = 21;	// 96.85 = 3.85*21+0.8*20
		G4double width_Replica = 0.8 * mm;
		G4Material* mat_SiPM = fMaterials -> GetMaterial("Silicon");

		if(PC -> GetParBool("SiPM_gapIn"))
		{
			G4Box* solid_area = 
				new G4Box("solid_SiPM_area",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
			G4LogicalVolume* logic_area = 
				new G4LogicalVolume(solid_area,fMaterials->GetMaterial("G4_AIR"),"logic_SiPM_area");
			if(PC -> GetParBool("SC1In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-trans),logic_area,"Area1L",logicWorld,false,SiPMID+1,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-trans),logic_area,"Area1R",logicWorld,false,SiPMID+2,checkOverlaps);
			}
			if(PC -> GetParBool("SC2In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC2_sizeX+sizeX),0,SC2ZOffset-trans),logic_area,"Area2L",logicWorld,false,SiPMID+3,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC2_sizeX+sizeX),0,SC2ZOffset-trans),logic_area,"Area2R",logicWorld,false,SiPMID+4,checkOverlaps);
			}
			if(PC -> GetParBool("SC3In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC3_sizeX+sizeX),0,SC3ZOffset-trans),logic_area,"Area2L",logicWorld,false,SiPMID+5,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC3_sizeX+sizeX),0,SC3ZOffset-trans),logic_area,"Area2R",logicWorld,false,SiPMID+6,checkOverlaps);
			}

			G4Box* solid_sensor = 
				new G4Box("solid_SiPM",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
			logicSiPM = 
				new G4LogicalVolume(solid_sensor,mat_SiPM,"logic_SiPM");
			new G4PVReplica("SiPM",logicSiPM,logic_area,kYAxis,nReplica,width_Replica+sizeY,0);
		}
		else // unified one silicon column
		{
			G4Box*solid_sensor = 
				new G4Box("solid_SiPM",0.5*sizeX,0.5*totY,0.5*sizeZ);
			logicSiPM = 
				new G4LogicalVolume(solid_sensor,mat_SiPM,"logicSiPM");
			if(PC->GetParBool("OpticalPhysics"))
			{
				G4OpticalSurface* surf_SiPM = fMaterials -> GetOpticalSurface("SiPMSurf");
				new G4LogicalSkinSurface("SiPM_surf",logicSiPM,surf_SiPM);
			}
			if(PC->GetParBool("SC1In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-trans),logicSiPM,"SiPML",logicWorld,false,SiPMID+1,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC1_sizeX+sizeX),0,SC1ZOffset-trans),logicSiPM,"SiPMR",logicWorld,false,SiPMID+2,checkOverlaps);
			}
			if(PC->GetParBool("SC2In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*(SC2_sizeX+sizeX),0,SC2ZOffset-trans),logicSiPM,"SiPML",logicWorld,false,SiPMID+3,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC2_sizeX+sizeX),0,SC2ZOffset-trans),logicSiPM,"SiPMR",logicWorld,false,SiPMID+4,checkOverlaps);
			}
			if(PC->GetParBool("SC3In"))
			{
				new G4PVPlacement(0,G4ThreeVector(0.5*SC3_sizeX+sizeX,0,SC3ZOffset-trans),logicSiPM,"SiPML",logicWorld,false,SiPMID+5,checkOverlaps);
				new G4PVPlacement(0,G4ThreeVector(-0.5*(SC3_sizeX+sizeX),0,SC3ZOffset-trans),logicSiPM,"SiPMR",logicWorld,false,SiPMID+6,checkOverlaps);
			}
		}
		G4VisAttributes* attSiPM = new G4VisAttributes(G4Colour(G4Colour::Grey()));
		attSiPM -> SetVisibility(true);
		attSiPM -> SetForceWireframe(true);
		logicSiPM -> SetVisAttributes(attSiPM);
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
		logicBDC = 
			new G4LogicalVolume(solidBDC,mat,"logicBDC");
		logicBDC -> SetUserLimits(fStepLimit);
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-trans),logicBDC,"BDC",logicWorld,false,ID+1,checkOverlaps);

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
			logicBTOF = 
				new G4LogicalVolume(solidBTOF,mat,"logicBTOF");
			logicBTOF -> SetUserLimits(fStepLimit);
			new G4PVPlacement(0,G4ThreeVector(0,0,(ZOffset1-trans)),logicBTOF,"BTOF1",logicWorld,false,ID+1,checkOverlaps);
			new G4PVPlacement(0,G4ThreeVector(0,0,(ZOffset2-trans)),logicBTOF,"BTOF2",logicWorld,false,ID+2,checkOverlaps);

			G4VisAttributes* attBTOF = new G4VisAttributes(G4Colour(G4Colour::Grey()));
			attBTOF -> SetVisibility(true);
			attBTOF -> SetForceWireframe(true);
			logicBTOF -> SetVisAttributes(attBTOF);
		}
		else if(PC->GetParInt("BTOFopt") == 1)
		{
			G4Box* solidBTOF = 
				new G4Box("solidBTOF",0.5*sizeY,0.5*sizeZ,0.5*sizeX);
			logicBTOF = 
				new G4LogicalVolume(solidBTOF,mat,"logicBTOF");
			new G4PVPlacement(0,G4ThreeVector(0,0.5*sizeY+30*mm,(ZOffset1-trans)),logicBTOF,"BTOFL",logicWorld,false,ID+1,checkOverlaps);
			new G4PVPlacement(0,G4ThreeVector(0,-0.5*sizeY-30*mm,(ZOffset2-trans)),logicBTOF,"BTOFR",logicWorld,false,ID+2,checkOverlaps);
		}
		G4VisAttributes* attBTOF = new G4VisAttributes(G4Colour(G4Colour::Grey()));
		attBTOF -> SetVisibility(true);
		attBTOF -> SetForceWireframe(true);
		logicBTOF -> SetVisAttributes(attBTOF);
	}

	if(PC -> GetParBool("TargetIn"))
	{
		G4int ID = PC -> GetParInt("TargetID");
		G4int opt = PC -> GetParInt("Targetopt");
		G4double sizeX = PC -> GetParDouble("Target_sizeX");
		G4double sizeY = PC -> GetParDouble("Target_sizeY");
		G4double sizeZ1 = PC -> GetParDouble("Target_sizeZ1");
		G4double sizeZ2 = PC -> GetParDouble("Target_sizeZ2");
		G4double sizeZ;
		if(opt == 0 || opt == 1)
			sizeZ = sizeZ1;
		else
			sizeZ = sizeZ2;
		G4double ZOffset = PC -> GetParDouble("Target_ZOffset");
		G4Material* mat = nullptr;
		if(opt == 0)	// empty box
			mat = fMaterials -> GetMaterial("Air");
		else if (opt == 1)
			mat = fMaterials -> GetMaterial("G4_POLYETHYLENE");
		else if (opt == 2)
			mat = fMaterials -> GetMaterial("Graphite");
		G4cout << "done" << G4endl;
//		else if (opt == 1)
//		{
//		}
		G4VSolid* solidTG = 
			new G4Box("solidTG",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		G4LogicalVolume* logicTG = 
			new G4LogicalVolume(solidTG,mat,"logicTG");
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-trans-0.5*sizeZ),logicTG,"TG",logicWorld,false,ID+1,checkOverlaps);

		G4VisAttributes* attTG = new G4VisAttributes(G4Colour(G4Colour::Red()));
		attTG -> SetVisibility(true);
		attTG -> SetForceWireframe(true);
		logicTG -> SetVisAttributes(attTG);
	}
	return physWorld;
}

void OpDetectorConstruction::ConstructSDandField()
{
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if(PC -> GetParBool("SC1In"))
	{
		OpSD* SC1SD = new OpSD("SC1","SC1C");
		SDman -> AddNewDetector(SC1SD);
		logicSC1 -> SetSensitiveDetector(SC1SD);
	}
	if(PC -> GetParBool("SC2In"))
	{
		OpSD* SC2SD = new OpSD("SC2","SC2C");
		SDman -> AddNewDetector(SC2SD);
		logicSC2 -> SetSensitiveDetector(SC2SD);
		if(PC->GetParBool("SiPMIn"))
		{
			OpSiPMSD* SC2SiPMSD = new OpSiPMSD("SC2SiPM","SC2SiPMC");
			SDman -> AddNewDetector(SC2SiPMSD);
			logicSiPM -> SetSensitiveDetector(SC2SiPMSD);
		}
	}
	if(PC -> GetParBool("SC3In"))
	{
		OpSD* SC3SD = new OpSD("SC3","SC3C");
		SDman -> AddNewDetector(SC3SD);
		logicSC3 -> SetSensitiveDetector(SC3SD);
	}
	if(PC -> GetParBool("BDCIn"))
	{
		OpSD* BDCSD = new OpSD("BDC","BDCC");
		SDman -> AddNewDetector(BDCSD);
		logicBDC -> SetSensitiveDetector(BDCSD);
	}
	if(PC -> GetParBool("BTOFIn")) 
	{
		OpSD* BTOFSD = new OpSD("BTOF","BTOFC");
		SDman -> AddNewDetector(BTOFSD);
		logicBTOF -> SetSensitiveDetector(BTOFSD);
	}
}
