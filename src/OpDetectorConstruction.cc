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
	G4bool checkOverlaps = false;

	// World
	G4int worldID = PC -> GetParInt("WorldID");
	G4double world_sizeX = PC -> GetParDouble("World_sizeX");
	G4double world_sizeY = PC -> GetParDouble("World_sizeY");
	G4double world_sizeZ = PC -> GetParDouble("World_sizeZ");
	G4Material* world_mat;
	if(PC -> GetParBool("OpticalPhysics")){
		world_mat = fMaterials -> GetMaterial("G4_AIR");
	}else{
		world_mat = G4Material::GetMaterial("G4_AIR");
	}

	G4Box* solidWorld =    
		new G4Box("World",0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);
	G4LogicalVolume* logicWorld =                         
		new G4LogicalVolume(solidWorld,world_mat,"World");
	G4VPhysicalVolume* physWorld = 
		new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,worldID,checkOverlaps);  

	G4double trans = PC -> GetParDouble("Translation");

	// upstream large collimator
	if(PC -> GetParBool("UCollIn"))
	{
		G4int ID = PC -> GetParInt("UCollID");
		G4double sizeX = PC -> GetParDouble("UColl_sizeX");
		G4double sizeY = PC -> GetParDouble("UColl_sizeY");
		G4double sizeZ = PC -> GetParDouble("UColl_sizeZ");
		G4double holeR = PC -> GetParDouble("UColl_holeR");
		G4double ZOffset = PC -> GetParDouble("UColl_ZOffset");
		G4Material* mat = fMaterials -> GetMaterial("PMMA");

		G4Box* solid_UColl = 
			new G4Box("UColl",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		G4Tubs* sub_hole = 
			new G4Tubs("Hole",0,holeR,0.53*sizeZ,0,2*M_PI);
		G4SubtractionSolid* solid_UColl_sub =
			new G4SubtractionSolid("UColl_sub",solid_UColl,sub_hole,0,G4ThreeVector(0,0,0));
		G4LogicalVolume* logic_UColl = 
			new G4LogicalVolume(solid_UColl_sub,mat,"logic_UColl");
		G4VisAttributes* attUColl = new G4VisAttributes(G4Colour(G4Colour::Gray()));
		attUColl -> SetLineWidth(5);
		attUColl->SetForceWireframe(true);
		logic_UColl -> SetVisAttributes(attUColl);

		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset-trans-sizeZ/2),logic_UColl,"UColl",logicWorld,ID,checkOverlaps);
	}
	if(PC -> GetParBool("SlitIn"))
	{
		G4int ID = PC -> GetParInt("SlitID");
		G4double sizeX = PC -> GetParDouble("Slit_sizeX");
		G4double sizeY = PC -> GetParDouble("Slit_sizeY");
		G4double sizeZ = PC -> GetParDouble("Slit_sizeZ");
		G4double slitW = PC -> GetParDouble("Slit_slitW");
		G4double ZOffset1 = PC -> GetParDouble("Slit_ZOffset1");
		G4double ZOffset2 = PC -> GetParDouble("Slit_ZOffset2");
		G4Material* mat = fMaterials -> GetMaterial("PMMA");

		G4Box* solid_Slit = 
			new G4Box("Slit",0.5*sizeX,0.5*sizeY,0.5*sizeZ);
		G4Box* sub_Slit = 
			new G4Box("sub_Slit",0.5*slitW,0.99*0.5*sizeY,0.5*sizeZ);
		G4SubtractionSolid* solid_Slit_sub = 
			new G4SubtractionSolid("Slit_sub",solid_Slit,sub_Slit,0,G4ThreeVector(0,0,0));
		G4LogicalVolume* logic_Slit = 
			new G4LogicalVolume(solid_Slit_sub,mat,"logic_Slit");
		G4VisAttributes* att_Slit = new G4VisAttributes(G4Colour(G4Colour::Gray()));
		att_Slit -> SetLineWidth(5);
		att_Slit -> SetForceWireframe(true);
		logic_Slit -> SetVisAttributes(att_Slit);

		G4RotationMatrix* Rot = new G4RotationMatrix;
		Rot->rotateZ(90*deg);
		
		new G4PVPlacement(0, G4ThreeVector(0,0,ZOffset1-trans-sizeZ/2),logic_Slit,"Slit1",logicWorld,ID,checkOverlaps);
		new G4PVPlacement(Rot, G4ThreeVector(0,0,ZOffset2-trans-sizeZ/2),logic_Slit,"Slit2",logicWorld,ID,checkOverlaps);
	}

	if(PC->GetParBool("BoronIn"))
	{
		G4int ID = PC -> GetParInt("BoronID");
		G4double sizeX = PC -> GetParDouble("Boron_sizeX");
		G4double sizeY = PC -> GetParDouble("Boron_sizeY");
		G4double sizeZ = PC -> GetParDouble("Boron_sizeZ");
		G4double holeX1 = PC -> GetParDouble("Boron_holeX1");
		G4double holeX2 = PC -> GetParDouble("Boron_holeX2");
		G4double holeY1 = PC -> GetParDouble("Boron_holeY1");
		G4double holeY2 = PC -> GetParDouble("Boron_holeY2");
		G4double ZOffset1 = PC -> GetParDouble("Boron_ZOffset1");
		G4double ZOffset2 = PC -> GetParDouble("Boron_ZOffset2");
		G4double YOffset1 = PC -> GetParDouble("Boron_YOffset1");
		G4double YOffset2 = PC -> GetParDouble("Boron_YOffset2");
		G4Material* mat = fMaterials -> GetMaterial("BoratedPolyethylene");

		G4Box* solid_B1 = new G4Box("solid_B1",sizeX/2,sizeY/2,sizeZ/2);
		G4Box* solid_B2 = new G4Box("solid_B2",sizeX/2,sizeZ/2,sizeY/2);
		G4LogicalVolume* logicB1 = new G4LogicalVolume(solid_B1,mat,"logicB1");
		G4LogicalVolume* logicB2 = new G4LogicalVolume(solid_B2,mat,"logicB1");
		G4VisAttributes* attB = new G4VisAttributes(G4Colour(G4Colour::Brown()));
		attB -> SetForceWireframe(true);
		attB -> SetLineWidth(5);
		logicB1->SetVisAttributes(attB);
		logicB2->SetVisAttributes(attB);

		if(PC->GetParBool("BoronWall1In"))
		{
			for(G4int row=0; row<4; row++)
			{
				if(row<2)
				{
					G4ThreeVector pos0(-sizeX,(2*row+1)*sizeY/2+YOffset2,ZOffset1+trans-sizeZ/2);
					G4ThreeVector pos1(0,(2*row+1)*sizeY/2+YOffset2,ZOffset1+trans-sizeZ/2);
					G4ThreeVector pos2(+sizeX,(2*row+1)*sizeY/2+YOffset2,ZOffset1+trans-sizeZ/2);
					new G4PVPlacement(0,pos0,logicB1,Form("B1_%d_0",row),logicWorld,false,ID,checkOverlaps);
					new G4PVPlacement(0,pos1,logicB1,Form("B1_%d_1",row),logicWorld,false,ID,checkOverlaps);
					new G4PVPlacement(0,pos2,logicB1,Form("B1_%d_2",row),logicWorld,false,ID,checkOverlaps);
				}
				else if (row==2)
				{
					G4ThreeVector pos0(-sizeX/2-holeX1/2,(2*row+1)*sizeY/2+YOffset2,ZOffset1+trans-sizeZ/2);
					G4ThreeVector pos1(+sizeX/2+holeX1/2,(2*row+1)*sizeY/2+YOffset2,ZOffset1+trans-sizeZ/2);
					new G4PVPlacement(0,pos0,logicB1,Form("B1_%d_0",row),logicWorld,false,ID,checkOverlaps);
					new G4PVPlacement(0,pos1,logicB1,Form("B1_%d_1",row),logicWorld,false,ID,checkOverlaps);
				}
				else if(row==3)
				{
					G4ThreeVector pos0(0,(2*row+1)*sizeY/2+YOffset2,ZOffset1+trans-sizeZ/2);
					new G4PVPlacement(0,pos0,logicB1,Form("B1_%d_0",row),logicWorld,false,ID,checkOverlaps);
				}
			}
		}

		//wall
		if(PC -> GetParBool("BoronWall2In"))
		{
			for(G4int row=0; row<7; row++)
			{
				if(row !=4)
				{
					G4ThreeVector pos0(-sizeX/2,(2*row+1)*sizeZ/2+YOffset1,ZOffset2+trans-sizeY/2);
					G4ThreeVector pos1(+sizeX/2,(2*row+1)*sizeZ/2+YOffset1,ZOffset2+trans-sizeY/2);
					new G4PVPlacement(0,pos0,logicB2,Form("B2_%d_0",row),logicWorld,false,ID,checkOverlaps);
					new G4PVPlacement(0,pos1,logicB2,Form("B2_%d_1",row),logicWorld,false,ID,checkOverlaps);
				}
				else
				{
					G4ThreeVector pos0(-sizeX/2-holeX2/2,(2*row+1)*sizeZ/2+YOffset1,ZOffset2+trans-sizeY/2);
					G4ThreeVector pos1(+sizeX/2+holeX2/2,(2*row+1)*sizeZ/2+YOffset1,ZOffset2+trans-sizeY/2);
					new G4PVPlacement(0,pos0,logicB2,Form("B2_%d_0",row),logicWorld,false,ID,checkOverlaps);
					new G4PVPlacement(0,pos1,logicB2,Form("B2_%d_1",row),logicWorld,false,ID,checkOverlaps);
				}
			}
		}
	}


	// SC
	if(PC -> GetParBool("SCIn"))
	{
		G4int ID = PC -> GetParInt("SCID");
		G4double Box_sizeX = PC -> GetParDouble("SCBox_sizeX");
		G4double Box_sizeY = PC -> GetParDouble("SCBox_sizeY");
		G4double Box_sizeZ = PC -> GetParDouble("SCBox_sizeZ");
		G4double sizeX1 = PC -> GetParDouble("SC_sizeX1");
		G4double sizeY1 = PC -> GetParDouble("SC_sizeY1");
		G4double sizeZ1 = PC -> GetParDouble("SC_sizeZ1");
		G4double sizeX2 = PC -> GetParDouble("SC_sizeX2");
		G4double sizeY2 = PC -> GetParDouble("SC_sizeY2");
		G4double sizeZ2 = PC -> GetParDouble("SC_sizeZ2");
		G4double ZOffset_Box = PC -> GetParDouble("SCBox_ZOffset");
		G4double ZOffset1 = PC -> GetParDouble("SC_ZOffset1");
		G4double ZOffset2 = PC -> GetParDouble("SC_ZOffset2");
		G4double ZOffset3 = PC -> GetParDouble("SC_ZOffset3");
		G4double ZOffset4 = PC -> GetParDouble("SC_ZOffset4");
		G4Material* mat_Box;
		if(PC->GetParBool("OpticalPhysics")){
			mat_Box = fMaterials->GetMaterial("G4_optical_Air");
		}else{
			mat_Box = G4Material::GetMaterial("G4_AIR");
		}
		G4Material* mat;
		if(PC -> GetParInt("SCmatOpt") == 0)
			mat = fMaterials -> GetMaterial("Polystyrene");
		else if(PC -> GetParInt("SCmatOpt") == 1)
			mat = fMaterials -> GetMaterial("Scintillator");
		else
			mat = fMaterials -> GetMaterial("Polystyrene");

		cout << mat -> GetName() << endl;
		G4Box* solid_Box =
			new G4Box("SCBox",0.5*Box_sizeX,0.5*Box_sizeY,0.5*Box_sizeZ);
		logicBox =
			new G4LogicalVolume(solid_Box,mat_Box,"SCBox");
		G4VisAttributes* attEnv = new G4VisAttributes(G4Colour(G4Colour::Brown()));
		attEnv -> SetVisibility(false);
		attEnv -> SetForceWireframe(true);
		attEnv -> SetLineWidth(2);
		logicBox->SetVisAttributes(attEnv);
		new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset_Box-trans+0.5*Box_sizeZ),logicBox,"opticalEnv",logicWorld,false,0,checkOverlaps);
		G4Box* solidSC1 =
			new G4Box("SC1",0.5*sizeX1,0.5*sizeY1,0.5*sizeZ1);
		G4Box* solidSC2 =
			new G4Box("SC2",0.5*sizeX2,0.5*sizeY2,0.5*sizeZ2);
		logicSC1 = 
			new G4LogicalVolume(solidSC1,mat,"SC1");
		logicSC2 = 
			new G4LogicalVolume(solidSC2,mat,"SC2");

		if(PC->GetParBool("SC110In"))
		{
			new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset1-trans-0.5*Box_sizeZ),logicSC1,"SC1",logicBox,false,ID+1,checkOverlaps);
			new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset2-trans-0.5*Box_sizeZ),logicSC1,"SC2",logicBox,false,ID+2,checkOverlaps);
		}
		if(PC->GetParBool("SC60In"))
		{
			new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset3-trans-0.5*Box_sizeZ),logicSC2,"SC3",logicBox,false,ID+3,checkOverlaps);
			new G4PVPlacement(0,G4ThreeVector(0,0,ZOffset4-trans-0.5*Box_sizeZ),logicSC2,"SC4",logicBox,false,ID+4,checkOverlaps);
		}


		G4VisAttributes* attSC = new G4VisAttributes(G4Colour(G4Colour::Magenta()));
		attSC -> SetLineWidth(2);
		attSC -> SetVisibility(true);
		attSC -> SetForceWireframe(true);
		logicSC1 -> SetVisAttributes(attSC);
		logicSC2 -> SetVisAttributes(attSC);
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
		G4double gap1  = PC -> GetParDouble("SiPM_gap1");
		G4double gap2  = PC -> GetParDouble("SiPM_gap2");
		G4double frame = PC -> GetParDouble("SiPM_frame");
		G4int SCID = PC -> GetParInt("SCID");
		G4double SC_sizeX1 = PC -> GetParDouble("SC_sizeX1");
		G4double SC_sizeY1 = PC -> GetParDouble("SC_sizeY1");
		G4double SC_sizeX2 = PC -> GetParDouble("SC_sizeX2");
		G4double SC_sizeY2 = PC -> GetParDouble("SC_sizeY2");
		G4double Box_sizeZ = PC -> GetParDouble("SCBox_sizeZ");
		G4double SCZOffset_Box = PC -> GetParDouble("SCBox_ZOffset");
		G4double SCZOffset1 = PC -> GetParDouble("SC_ZOffset1");
		G4double SCZOffset2 = PC -> GetParDouble("SC_ZOffset2");
		G4double SCZOffset3 = PC -> GetParDouble("SC_ZOffset3");
		G4double SCZOffset4 = PC -> GetParDouble("SC_ZOffset4");
		G4int nReplica1 = PC -> GetParInt("SiPM_n1");	// 96.85 = 3.85*21+0.8*20
		G4int nReplica2 = PC -> GetParInt("SiPM_n2");	// 96.85 = 3.85*21+0.8*20
		const G4double width_Replica1 = (gap1 + frame) * mm;
		const G4double width_Replica2 = (gap2 + frame) * mm;
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
			if(PC->GetParBool("OpticalPhysics"))
			{
//				G4OpticalSurface* surf_SiPM = fMaterials -> GetOpticalSurface("SiPMSurf");
//				new G4LogicalSkinSurface("SiPM_surf",logicSiPM,surf_SiPM);
			}
			if(PC -> GetParBool("SCIn"))
			{
				if(LR)
				{
					G4double baseline1 = -1*((sizeY*nReplica1) + (width_Replica1*(nReplica1-2)))/2;
					G4double baseline2 = -1*((sizeY*nReplica2) + (width_Replica2*(nReplica2-2)))/2;
					for(G4int col=0; col<nReplica1; col++) // for 110x110 module
					{
						G4String str_GlassL = "Glass"+to_string(col)+"L";
						G4String str_GlassR = "Glass"+to_string(col)+"R";
						G4String str_SiPML = "SiPM"+to_string(col)+"L";
						G4String str_SiPMR = "SiPM"+to_string(col)+"R";
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX1+sizeX)+G_sizeX,baseline1+col*(frame+sizeY),SCZOffset1-trans-0.5*Box_sizeZ),logicSiPM,str_SiPML,logicBox,false,SiPMID+1000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX1+sizeX)-G_sizeX,baseline1+col*(width_Replica1+sizeY),SCZOffset1-trans-0.5*Box_sizeZ),logicSiPM,str_SiPMR,logicBox,false,SiPMID+2000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX1+G_sizeX),baseline1+col*(width_Replica1+sizeY),SCZOffset1-trans-0.5*Box_sizeZ),logic_Glass,str_GlassL,logicBox,false,0,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX1+G_sizeX),baseline1+col*(width_Replica1+sizeY),SCZOffset1-trans-0.5*Box_sizeZ),logic_Glass,str_GlassR,logicBox,false,0,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX1+sizeX)+G_sizeX,baseline1+col*(width_Replica1+sizeY),SCZOffset2-trans-0.5*Box_sizeZ),logicSiPM,str_SiPML,logicBox,false,SiPMID+1000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX1+sizeX)-G_sizeX,baseline1+col*(width_Replica1+sizeY),SCZOffset2-trans-0.5*Box_sizeZ),logicSiPM,str_SiPMR,logicBox,false,SiPMID+2000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX1+G_sizeX),baseline1+col*(width_Replica1+sizeY),SCZOffset2-trans-0.5*Box_sizeZ),logic_Glass,str_GlassL,logicBox,false,0,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX1+G_sizeX),baseline1+col*(width_Replica1+sizeY),SCZOffset2-trans-0.5*Box_sizeZ),logic_Glass,str_GlassR,logicBox,false,0,checkOverlaps);
					}
					for(G4int col=0; col<nReplica2; col++)
					{
						G4String str_GlassL = "Glass"+to_string(col)+"L";
						G4String str_GlassR = "Glass"+to_string(col)+"R";
						G4String str_SiPML = "SiPM"+to_string(col)+"L";
						G4String str_SiPMR = "SiPM"+to_string(col)+"R";
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX2+sizeX)+G_sizeX,baseline2+col*(width_Replica2+sizeY),SCZOffset3-trans-0.5*Box_sizeZ),logicSiPM,str_SiPML,logicBox,false,SiPMID+1000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX2+sizeX)-G_sizeX,baseline2+col*(width_Replica2+sizeY),SCZOffset3-trans-0.5*Box_sizeZ),logicSiPM,str_SiPMR,logicBox,false,SiPMID+2000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX2+G_sizeX),baseline2+col*(width_Replica2+sizeY),SCZOffset3-trans-0.5*Box_sizeZ),logic_Glass,str_GlassL,logicBox,false,0,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX2+G_sizeX),baseline2+col*(width_Replica2+sizeY),SCZOffset3-trans-0.5*Box_sizeZ),logic_Glass,str_GlassR,logicBox,false,0,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX2+sizeX)+G_sizeX,baseline2+col*(width_Replica2+sizeY),SCZOffset4-trans-0.5*Box_sizeZ),logicSiPM,str_SiPML,logicBox,false,SiPMID+1000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX2+sizeX)-G_sizeX,baseline2+col*(width_Replica2+sizeY),SCZOffset4-trans-0.5*Box_sizeZ),logicSiPM,str_SiPMR,logicBox,false,SiPMID+2000+SCID+col,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX2+G_sizeX),baseline2+col*(width_Replica2+sizeY),SCZOffset4-trans-0.5*Box_sizeZ),logic_Glass,str_GlassL,logicBox,false,0,checkOverlaps);
						new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX2+G_sizeX),baseline2+col*(width_Replica2+sizeY),SCZOffset4-trans-0.5*Box_sizeZ),logic_Glass,str_GlassR,logicBox,false,0,checkOverlaps);
					}
				}
				if(UD)
				{
//					for(G4int row=0; row<nReplica2; row++)
//					{
//						G4String str_GlassU = "Glass"+to_string(row)+"U";
//						G4String str_GlassD = "Glass"+to_string(row)+"D";
//						G4String str_SiPMU = "SiPM"+to_string(row)+"U";
//						G4String str_SiPMD = "SiPM"+to_string(row)+"D";
//						G4int cent = (G4int)nReplica2/2;
//						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),0.5*(SC_sizeX1+sizeX)+G_sizeX,SCZOffset1-trans),logicSiPM,str_SiPMU,logicWorld,false,SiPMID+3000+SCID+row,checkOverlaps);
//						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),-0.5*(SC_sizeX1+sizeX)-G_sizeX,SCZOffset1-trans),logicSiPM,str_SiPMD,logicWorld,false,SiPMID+4000+SCID+row,checkOverlaps);
//						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),0.5*(SC_sizeX1+G_sizeX),SCZOffset1-trans),logic_Glass,str_GlassU,logicWorld,false,0,checkOverlaps);
//						new G4PVPlacement(rot,G4ThreeVector((cent-row)*(gap+frame+sizeY),-0.5*(SC_sizeX1+G_sizeX),SCZOffset1-trans),logic_Glass,str_GlassD,logicWorld,false,0,checkOverlaps);
//					}
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
			if(PC->GetParBool("OpticalPhysics"))
			{
				G4OpticalSurface* surf_SiPM = fMaterials -> GetOpticalSurface("SiPMSurf");
				new G4LogicalSkinSurface("SiPM_surf",logicSiPM,surf_SiPM);
			}
			if(PC->GetParBool("SCIn"))
			{
				G4RotationMatrix* rot = new G4RotationMatrix(); rot->rotateX(90*deg);
				if(LR)
				{
					new G4PVPlacement(0,G4ThreeVector(0.5*(SC_sizeX1+sizeX)+G_sizeX,0,SCZOffset1-trans),logicSiPM,"SiPML2",logicWorld,false,SiPMID+3,checkOverlaps);
					new G4PVPlacement(0,G4ThreeVector(-0.5*(SC_sizeX1+sizeX)-G_sizeX,0,SCZOffset1-trans),logicSiPM,"SiPMR2",logicWorld,false,SiPMID+4,checkOverlaps);
					new G4PVPlacement(0,G4ThreeVector(0.5*(G_sizeX+SC_sizeX1),0,SCZOffset2-trans),logic_Glass,"GlassL2",logicWorld,false,SiPMID+21,checkOverlaps);
					new G4PVPlacement(0,G4ThreeVector(-0.5*(G_sizeX+SC_sizeX1),0,SCZOffset2-trans),logic_Glass,"GlassR2",logicWorld,false,SiPMID+22,checkOverlaps);
				}
				if(UD)
				{
//					new G4PVPlacement(rot,G4ThreeVector(0.5*(SC_sizeY+sizeX)+G_sizeX,SCZOffset-trans),logicSiPM,"SiPMU2",logicWorld,false,SiPMID+5,checkOverlaps);
//					new G4PVPlacement(rot,G4ThreeVector(-0.5*(SC_sizeY+sizeX)-G_sizeX,SCZOffset-trans),logicSiPM,"SiPMD2",logicWorld,false,SiPMID+6,checkOverlaps);
//					new G4PVPlacement(rot,G4ThreeVector(0.5*(G_sizeX+SC_sizeY),SCZOffset-trans),logic_Glass,"GlassU2",logicWorld,false,SiPMID+23,checkOverlaps);
//					new G4PVPlacement(rot,G4ThreeVector(-0.5*(G_sizeX+SC_sizeY),SCZOffset-trans),logic_Glass,"GlassD2",logicWorld,false,SiPMID+24,checkOverlaps);
				}
			}
			G4VisAttributes* attGlass = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attGlass -> SetVisibility(true);
			attGlass -> SetForceWireframe(true);
			logic_Glass -> SetVisAttributes(attGlass);
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
		logicSC1 -> SetSensitiveDetector(SCSD);
		logicSC2 -> SetSensitiveDetector(SCSD);
		if(PC->GetParBool("SiPMIn"))
		{
			OpSiPMSD* SCSiPMSD = new OpSiPMSD("SCSiPM","SCSiPMC");
			SDman -> AddNewDetector(SCSiPMSD);
			logicSiPM -> SetSensitiveDetector(SCSiPMSD);
		}
	}
}
