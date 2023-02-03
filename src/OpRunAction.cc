#include "OpRunAction.hh"
#include "OpPrimaryGeneratorAction.hh"
#include "OpDetectorConstruction.hh"

#include "OpParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <utility>
#include <algorithm>
#include <functional>
#include <map>

OpRunAction::OpRunAction()
: G4UserRunAction()
{ 
	PC = OpParameterContainer::GetInstance();

	F = new TFile(PC -> GetParString("OutName").c_str(),"recreate");
	T = new TTree("Opsim","Opsim");

	init_Tree();
}

OpRunAction::~OpRunAction()
{
	F -> Write();
	F -> Close();
}

void OpRunAction::init_Tree()
{
	if(PC -> GetParBool("MCTrack"))
	{
		T -> Branch("nTrack",&nTrack);
		T -> Branch("TrackID",TrackID,"TrackID[nTrack]/I");
		T -> Branch("ParentID",ParentID,"ParentID[nTrack]/I");
		T -> Branch("TrackPDG",TrackPDG,"TrackPDG[nTrack]/I");
		T -> Branch("TrackDetID",TrackDetID,"TrackDetID[nTrack]/I");

		T -> Branch("TrackPX",TrackPX,"TrackPX[nTrack]/D");
		T -> Branch("TrackPY",TrackPY,"TrackPY[nTrack]/D");
		T -> Branch("TrackPZ",TrackPZ,"TrackPZ[nTrack]/D");
		T -> Branch("TrackVX",TrackVX,"TrackVX[nTrack]/D");
		T -> Branch("TrackVY",TrackVY,"TrackVY[nTrack]/D");
		T -> Branch("TrackVZ",TrackVZ,"TrackVZ[nTrack]/D");
		T -> Branch("TrackEnergy",TrackEnergy,"TrackEnergy[nTrack]/D");
		T -> Branch("TrackKEnergy",TrackKEnergy,"TrackKEnergy[nTrack]/D");
	}
	if(PC -> GetParBool("MCPostTrack"))
	{
		T -> Branch("nPostTrack",&nPostTrack);
		T -> Branch("PostTrackID",PostTrackID,"PostTrackID[nPostTrack]/I");
		T -> Branch("PostTrackDetID",PostTrackDetID,"PostTrackDetID[nPostTrack]/I");

		T -> Branch("PostTrackPX",PostTrackPX,"PostTrackPX[nPostTrack]/D");
		T -> Branch("PostTrackPY",PostTrackPY,"PostTrackPY[nPostTrack]/D");
		T -> Branch("PostTrackPZ",PostTrackPZ,"PostTrackPZ[nPostTrack]/D");
		T -> Branch("PostTrackVX",PostTrackVX,"PostTrackVX[nPostTrack]/D");
		T -> Branch("PostTrackVY",PostTrackVY,"PostTrackVY[nPostTrack]/D");
		T -> Branch("PostTrackVZ",PostTrackVZ,"PostTrackVZ[nPostTrack]/D");
		T -> Branch("PostTrackEnergy",PostTrackEnergy,"PostTrackEnergy[nPostTrack]/D");
		T -> Branch("PostTrackKEnergy",PostTrackKEnergy,"PostTrackKEnergy[nPostTrack]/D");
	}
	if(PC -> GetParBool("MCStep"))
	{
		T -> Branch("nStep",&nStep);
		T -> Branch("StepTrackID",StepTrackID,"StepTrackID[nStep]/I");
		T -> Branch("StepDetID",StepDetID,"StepDetID[nStep]/I");

		T -> Branch("StepVX",StepVX,"StepVX[nStep]/D");
		T -> Branch("StepVY",StepVY,"StepVY[nStep]/D");
		T -> Branch("StepVZ",StepVZ,"StepVZ[nStep]/D");
		T -> Branch("StepEdep",StepEdep,"StepEdep[nStep]/D");

		// for energy sum of box
		T -> Branch("EdepSumBox",&EdepSumBox);
	}
	if(PC -> GetParBool("OpTrack"))
	{
		T -> Branch("NOpticalPhotons",&NOpticalPhotons);
		T -> Branch("OpTrackID",OpTrackID,"OpTrackID[NOpticalPhotons]/I");
		T -> Branch("OpParentID",OpParentID,"OpParentID[NOpticalPhotons]/I");
		T -> Branch("OpDetID",OpDetID,"OpDetID[NOpticalPhotons]/I");
		T -> Branch("OpPX",OpPX,"OpPX[NOpticalPhotons]/D");
		T -> Branch("OpPY",OpPY,"OpPY[NOpticalPhotons]/D");
		T -> Branch("OpPZ",OpPZ,"OpPZ[NOpticalPhotons]/D");
		T -> Branch("OpVX",OpVX,"OpVX[NOpticalPhotons]/D");
		T -> Branch("OpVY",OpVY,"OpVY[NOpticalPhotons]/D");
		T -> Branch("OpVZ",OpVZ,"OpVZ[NOpticalPhotons]/D");
	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		T -> Branch("PostNOpticalPhotons",&PostNOpticalPhotons);
		T -> Branch("PostOpTrackID",PostOpTrackID,"PostOpTrackID[PostNOpticalPhotons]/I");
		T -> Branch("PostOpDetID",PostOpDetID,"PostOpDetID[PostNOpticalPhotons]/I");
		T -> Branch("PostOpPX",PostOpPX,"PostOpPX[PostNOpticalPhotons]/D");
		T -> Branch("PostOpPY",PostOpPY,"PostOpPY[PostNOpticalPhotons]/D");
		T -> Branch("PostOpPZ",PostOpPZ,"PostOpPZ[PostNOpticalPhotons]/D");
		T -> Branch("PostOpVX",PostOpVX,"PostOpVX[PostNOpticalPhotons]/D");
		T -> Branch("PostOpVY",PostOpVY,"PostOpVY[PostNOpticalPhotons]/D");
		T -> Branch("PostOpVZ",PostOpVZ,"PostOpVZ[PostNOpticalPhotons]/D");
	}
}

void OpRunAction::BeginOfRunAction(const G4Run*)
{ 
}

void OpRunAction::EndOfRunAction(const G4Run* run)
{
}

void OpRunAction::clear_data()
{
	if(PC->GetParBool("MCTrack"))
	{
		nTrack = 0;
		fill_n(TrackID,max_tracks,0);
		fill_n(ParentID,max_tracks,0);
		fill_n(TrackPDG,max_tracks,0);
		fill_n(TrackDetID,max_tracks,0);
		fill_n(TrackPX,max_tracks,0);
		fill_n(TrackPY,max_tracks,0);
		fill_n(TrackPZ,max_tracks,0);
		fill_n(TrackVX,max_tracks,0);
		fill_n(TrackVY,max_tracks,0);
		fill_n(TrackVZ,max_tracks,0);
		fill_n(TrackEnergy,max_tracks,0);
		fill_n(TrackKEnergy,max_tracks,0);
	}
	if(PC->GetParBool("MCPostTrack"))
	{
		nPostTrack = 0;
		fill_n(PostTrackID,max_tracks,0);
		fill_n(PostTrackPDG,max_tracks,0);
		fill_n(PostTrackDetID,max_tracks,0);
		fill_n(PostTrackPX,max_tracks,0);
		fill_n(PostTrackPY,max_tracks,0);
		fill_n(PostTrackPZ,max_tracks,0);
		fill_n(PostTrackVX,max_tracks,0);
		fill_n(PostTrackVY,max_tracks,0);
		fill_n(PostTrackVZ,max_tracks,0);
		fill_n(PostTrackEnergy,max_tracks,0);
		fill_n(PostTrackKEnergy,max_tracks,0);
	}
	if(PC->GetParBool("MCStep"))
	{
		nStep = 0;
		fill_n(StepTrackID,max_steps,0);
		fill_n(StepDetID,max_steps,0);
		fill_n(StepVX,max_steps,0);
		fill_n(StepVY,max_steps,0);
		fill_n(StepVZ,max_steps,0);
		fill_n(StepEdep,max_steps,0);
		EdepSumBox = 0;
	}
	if(PC->GetParBool("OpTrack"))
	{
		NOpticalPhotons = 0;
		fill_n(OpTrackID,max_opticalphotons,0);
		fill_n(OpParentID,max_opticalphotons,0);
		fill_n(OpDetID,max_opticalphotons,0);
		fill_n(OpPX,max_opticalphotons,0);
		fill_n(OpPY,max_opticalphotons,0);
		fill_n(OpPZ,max_opticalphotons,0);
		fill_n(OpVX,max_opticalphotons,0);
		fill_n(OpVY,max_opticalphotons,0);
		fill_n(OpVZ,max_opticalphotons,0);
	}
	if(PC->GetParBool("OpPostTrack"))
	{
		PostNOpticalPhotons = 0;
		fill_n(PostOpTrackID,max_opticalphotons,0);
		fill_n(PostOpDetID,max_opticalphotons,0);
		fill_n(PostOpPX,max_opticalphotons,0);
		fill_n(PostOpPY,max_opticalphotons,0);
		fill_n(PostOpPZ,max_opticalphotons,0);
		fill_n(PostOpVX,max_opticalphotons,0);
		fill_n(PostOpVY,max_opticalphotons,0);
		fill_n(PostOpVZ,max_opticalphotons,0);
	}
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
	if(opt == MCTrack)	// starting point, prev track
	{
		TrackID[nTrack] = trkID;
		ParentID[nTrack] = parentID;
		TrackPDG[nTrack] = pdg;
		TrackDetID[nTrack] = detID;
		TrackPX[nTrack] = p.x();
		TrackPY[nTrack] = p.y();
		TrackPZ[nTrack] = p.z();
		TrackVX[nTrack] = v.x();
		TrackVY[nTrack] = v.y();
		TrackVZ[nTrack] = v.z();
		TrackEnergy[nTrack] = totenergy;
		TrackKEnergy[nTrack] = kinenergy;
		nTrack++;
	}
	else if(opt == MCPostTrack)	// end point, post Track
	{
		PostTrackID[nPostTrack] = trkID;
		PostTrackDetID[nPostTrack] = detID;
		PostTrackPX[nPostTrack] = p.x();
		PostTrackPY[nPostTrack] = p.y();
		PostTrackPZ[nPostTrack] = p.z();
		PostTrackVX[nPostTrack] = v.x();
		PostTrackVY[nPostTrack] = v.y();
		PostTrackVZ[nPostTrack] = v.z();
		PostTrackEnergy[nPostTrack] = totenergy;
		PostTrackKEnergy[nPostTrack] = kinenergy;
		nPostTrack++;
	}
	else
	{
		G4cout << "Error in OpRunAction::FillTrack()" << G4endl;
	}
}

void OpRunAction::FillOpticalPhoton
(G4int opt, G4int trkID, G4int parentID, G4int detID, G4ThreeVector p, G4ThreeVector v)
{
	if(opt == MCTrack)
	{
		OpTrackID[NOpticalPhotons] = trkID;
		OpParentID[NOpticalPhotons] = parentID;
		OpDetID[NOpticalPhotons] = detID;
		OpPX[NOpticalPhotons] = p.x();
		OpPY[NOpticalPhotons] = p.y();
		OpPZ[NOpticalPhotons] = p.z();
		OpVX[NOpticalPhotons] = v.x();
		OpVY[NOpticalPhotons] = v.y();
		OpVZ[NOpticalPhotons] = v.z();
		NOpticalPhotons++;
	}
	else if (opt == MCPostTrack)
	{
		PostOpTrackID[PostNOpticalPhotons] = trkID;
		PostOpDetID[PostNOpticalPhotons] = detID;
		PostOpPX[PostNOpticalPhotons] = p.x();
		PostOpPY[PostNOpticalPhotons] = p.y();
		PostOpPZ[PostNOpticalPhotons] = p.z();
		PostOpVX[PostNOpticalPhotons] = v.x();
		PostOpVY[PostNOpticalPhotons] = v.y();
		PostOpVZ[PostNOpticalPhotons] = v.z();
		PostNOpticalPhotons++;
	}
}

void OpRunAction::FillStep
(G4int trkID, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep)
{
	if(prev_detID != post_detID)	// at the boundary
		StepDetID[nStep] = post_detID;	// mainly this case means particle track killed in a volume
	else
		StepDetID[nStep] = prev_detID;
	StepTrackID[nStep] = trkID;
	StepVX[nStep] = v.x();
	StepVY[nStep] = v.y();
	StepVZ[nStep] = v.z();
	StepEdep[nStep] = edep;

	if(StepDetID[nStep] == PC -> GetParInt("BoxID"))
		EdepSumBox += edep;
	nStep++;
}
