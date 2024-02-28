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
#include "G4ThreeVector.hh"
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
	G4bool checkOverlaps = false;

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

	G4double trans = PC -> GetParDouble("Translation");

	// SC
	if(PC -> GetParBool("SCIn"))
	{
		G4int ID = PC -> GetParInt("SCID");
		G4double sizeX = PC -> GetParDouble("SC_sizeX");
		G4double sizeY = PC -> GetParDouble("SC_sizeY");
		G4double sizeZ = PC -> GetParDouble("SC_sizeZ");
		G4double ZOffset = PC -> GetParDouble("SC_ZOffset");
		G4Material* mat;
		if(PC -> GetParInt("SCmatOpt") == 0)
			mat = fMaterials -> GetMaterial("Polystyrene");
		else if(PC -> GetParInt("SCmatOpt") == 1)
			mat = fMaterials -> GetMaterial("Scintillator");
		else
			mat = fMaterials -> GetMaterial("Polystyrene");

		G4Box* solidSC =
			new G4Box("SC",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		logicSC = 
			new G4LogicalVolume(solidSC,mat,"SC");

		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-trans),logicSC,"SC",logicWorld,false,ID+1,checkOverlaps);


		G4VisAttributes* attSC = new G4VisAttributes(G4Colour(G4Colour::Brown()));
		attSC -> SetVisibility(true);
		attSC -> SetForceWireframe(true);
		logicSC -> SetVisAttributes(attSC);
	}
	if(PC -> GetParBool("SiPMIn"))
	{
		G4int SiPMID = PC -> GetParInt("SiPMID");
		G4bool LR = PC -> GetParBool("SiPMLR");
		G4bool UD = PC -> GetParBool("SiPMUD");
		G4double sizeX = PC -> GetParDouble("SiPM_sizeX");
		G4double G_sizeX = PC -> GetParDouble("Glass_sizeX");
		G4double sizeY = PC -> GetParDouble("SiPM_sizeY");
		G4double sizeZ = PC -> GetParDouble("SiPM_sizeZ");
		G4double totY  = PC -> GetParDouble("SiPM_totY");
		G4double gap   = PC -> GetParDouble("SiPM_gap");
		G4double frame = PC -> GetParDouble("SiPM_frame");
		G4int SCID = PC -> GetParInt("SCID");
		G4double SC_sizeX = PC -> GetParDouble("SC_sizeX");
		G4double SC_sizeY = PC -> GetParDouble("SC_sizeY");
		G4double SCZOffset = PC -> GetParDouble("SC_ZOffset");
		G4int nReplica = PC -> GetParInt("SiPM_n");	// 96.85 = 3.85*21+0.8*20
		const G4double width_Replica = (gap + frame) * mm;
		G4Material* mat_SiPM = fMaterials -> GetMaterial("Silicon");
		G4Material* mat_Glass = fMaterials -> GetMaterial("Glass");

		if(PC -> GetParBool("SiPM_gapIn"))
		{
			G4RotationMatrix* rot = new G4RotationMatrix(); rot->rotateZ(90*deg);
			G4Box* solid_sensor = 
				new G4Box("solid_SiPM",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
			logicSiPM = 
				new G4LogicalVolume(solid_sensor,mat_SiPM,"logicSiPM");
			G4Box* solid_glass = 
				new G4Box("solid_Glass",0.5*G_sizeX,0.5*sizeY,0.5*sizeZ);
			G4LogicalVolume* logic_Glass =
				new G4LogicalVolume(solid_glass,mat_Glass,"logicGlass");
			if(PC -> GetParBool("SCIn"))
			{
				if(LR)
				{
					for(G4int col=0; col<nReplica; col++)
					{
						G4String str_GlassL = "Glass"+to_string(col)+"L";
						G4String str_GlassR = "Glass"+to_string(col)+"R";
						G4String str_SiPML = "SiPM"+to_string(col)+"L";
						G4String str_SiPMR = "SiPM"+to_string(col)+"R";
						G4int cent = (G4int)nReplica/2;
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX+sizeX)+G_sizeX,(cent-col)*(gap+frame+sizeY),SCZOffset-trans),logicSiPM,str_SiPML,logicWorld,false,SiPMID+1000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX+sizeX)-G_sizeX,(cent-col)*(gap+frame+sizeY),SCZOffset-trans),logicSiPM,str_SiPMR,logicWorld,false,SiPMID+2000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX+G_sizeX),(cent-col)*(gap+frame+sizeY),SCZOffset-trans),logic_Glass,str_GlassL,logicWorld,false,0,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX+G_sizeX),(cent-col)*(gap+frame+sizeY),SCZOffset-trans),logic_Glass,str_GlassR,logicWorld,false,0,checkOverlaps);
					}
				}
				if(UD)
				{
					for(G4int row=0; row<nReplica; row++)
					{
						G4String str_GlassU = "Glass"+to_string(row)+"U";
						G4String str_GlassD = "Glass"+to_string(row)+"D";
						G4String str_SiPMU = "SiPM"+to_string(row)+"U";
						G4String str_SiPMD = "SiPM"+to_string(row)+"D";
						G4int cent = (G4int)nReplica/2;
						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),0.5*(SC_sizeX+sizeX)+G_sizeX,SCZOffset-trans),logicSiPM,str_SiPMU,logicWorld,false,SiPMID+3000+SCID+row,checkOverlaps);
						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),-0.5*(SC_sizeX+sizeX)-G_sizeX,SCZOffset-trans),logicSiPM,str_SiPMD,logicWorld,false,SiPMID+4000+SCID+row,checkOverlaps);
						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),0.5*(SC_sizeX+G_sizeX),SCZOffset-trans),logic_Glass,str_GlassU,logicWorld,false,0,checkOverlaps);
						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),-0.5*(SC_sizeX+G_sizeX),SCZOffset-trans),logic_Glass,str_GlassD,logicWorld,false,0,checkOverlaps);
					}
				}
			}
			G4VisAttributes* attGlass = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attGlass -> SetVisibility(true);
			attGlass -> SetForceWireframe(true);
			logic_Glass -> SetVisAttributes(attGlass);
		}
		else // unified one silicon column
		{
			G4Box*solid_sensor = 
				new G4Box("solid_SiPM",0.5*sizeX,0.5*totY,0.5*sizeZ);
			G4Box* solid_glass = 
				new G4Box("solid_Glass",0.5*G_sizeX,0.5*totY,0.5*sizeZ);
			logicSiPM = 
				new G4LogicalVolume(solid_sensor,mat_SiPM,"logicSiPM");
			G4LogicalVolume* logic_Glass = 
				new G4LogicalVolume(solid_glass,mat_Glass,"logicGlass");
			if(PC->GetParBool("SCIn"))
			{
				G4RotationMatrix* rot = new G4RotationMatrix(); rot->rotateX(90*deg);
				if(LR)
				{
					new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX+sizeX)+G_sizeX,0,SCZOffset-trans),logicSiPM,"SiPML2",logicWorld,false,SiPMID+3,checkOverlaps);
					new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX+sizeX)-G_sizeX,0,SCZOffset-trans),logicSiPM,"SiPMR2",logicWorld,false,SiPMID+4,checkOverlaps);
					new G4PVPlacement(0,G4ThreeVector(0.5*(G_sizeX+SC_sizeX),0,SCZOffset-trans),logic_Glass,"GlassL2",logicWorld,false,SiPMID+21,checkOverlaps);
					new G4PVPlacement(0,G4ThreeVector(-0.5*(G_sizeX+SC_sizeX),0,SCZOffset-trans),logic_Glass,"GlassR2",logicWorld,false,SiPMID+22,checkOverlaps);
				}
				if(UD)
				{
					new G4PVPlacement(rot,G4ThreeVector(0.5*(SC_sizeY+sizeX)+G_sizeX,SCZOffset-trans),logicSiPM,"SiPMU2",logicWorld,false,SiPMID+5,checkOverlaps);
					new G4PVPlacement(rot,G4ThreeVector(-0.5*(SC_sizeY+sizeX)-G_sizeX,SCZOffset-trans),logicSiPM,"SiPMD2",logicWorld,false,SiPMID+6,checkOverlaps);
					new G4PVPlacement(rot,G4ThreeVector(0.5*(G_sizeX+SC_sizeY),SCZOffset-trans),logic_Glass,"GlassU2",logicWorld,false,SiPMID+23,checkOverlaps);
					new G4PVPlacement(rot,G4ThreeVector(-0.5*(G_sizeX+SC_sizeY),SCZOffset-trans),logic_Glass,"GlassD2",logicWorld,false,SiPMID+24,checkOverlaps);
				}
			}
			G4VisAttributes* attGlass = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attGlass -> SetVisibility(true);
			attGlass -> SetForceWireframe(true);
			logic_Glass -> SetVisAttributes(attGlass);
		}
		if(PC->GetParBool("OpticalPhysics"))
		{
			G4OpticalSurface* surf_SiPM = fMaterials -> GetOpticalSurface("SiPMSurf");
			new G4LogicalSkinSurface("SiPM_surf",logicSiPM,surf_SiPM);
		}
		G4VisAttributes* attSiPM = new G4VisAttributes(G4Colour(G4Colour::Grey()));
		attSiPM -> SetVisibility(true);
		attSiPM -> SetForceWireframe(true);
		logicSiPM -> SetVisAttributes(attSiPM);
	}

	return physWorld;
}

void OpDetectorConstruction::ConstructSDandField()
{
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if(PC -> GetParBool("SCIn"))
	{
		OpSD* SCSD = new OpSD("SC","SCC");
		SDman -> AddNewDetector(SCSD);
		logicSC -> SetSensitiveDetector(SCSD);
		if(PC->GetParBool("SiPMIn"))
		{
			OpSiPMSD* SCSiPMSD = new OpSiPMSD("SCSiPM","SCSiPMC");
			SDman -> AddNewDetector(SCSiPMSD);
			logicSiPM -> SetSensitiveDetector(SCSiPMSD);
		}
	}
}
