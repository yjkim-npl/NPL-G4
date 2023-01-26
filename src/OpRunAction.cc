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

OpRunAction::OpRunAction(OpParameterContainer* par)
: G4UserRunAction()
{ 
	PC = par;

	F = new TFile(PC -> GetParString("outName").c_str(),"recreate");
	T = new TTree("G4sim","G4sim");

	init_Tree();
}

OpRunAction::~OpRunAction()
{
	F -> Write();
	F -> Close();
}

void OpRunAction::init_Tree()
{
	bool bMCTrack = PC -> GetParBool("MCTrack");
	bool bMCPostTrack = PC -> GetParBool("MCPostTrack");
	bool bStep = PC -> GetParBool("Step");

	if(bMCTrack)
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
	if(bMCPostTrack)
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
	if(bStep)
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
}

void OpRunAction::BeginOfRunAction(const G4Run*)
{ 
}

void OpRunAction::EndOfRunAction(const G4Run* run)
{
}

void OpRunAction::clear_data()
{
	nTrack = 0;
	fill_n(TrackID,1000,0);
	fill_n(ParentID,1000,0);
	fill_n(TrackPDG,1000,0);
	fill_n(TrackDetID,1000,0);
	fill_n(TrackPX,1000,0);
	fill_n(TrackPY,1000,0);
	fill_n(TrackPZ,1000,0);
	fill_n(TrackVX,1000,0);
	fill_n(TrackVY,1000,0);
	fill_n(TrackVZ,1000,0);
	fill_n(TrackEnergy,1000,0);
	fill_n(TrackKEnergy,1000,0);

	nPostTrack = 0;
	fill_n(PostTrackID,1000,0);
	fill_n(PostTrackPDG,1000,0);
	fill_n(PostTrackDetID,1000,0);
	fill_n(PostTrackPX,1000,0);
	fill_n(PostTrackPY,1000,0);
	fill_n(PostTrackPZ,1000,0);
	fill_n(PostTrackVX,1000,0);
	fill_n(PostTrackVY,1000,0);
	fill_n(PostTrackVZ,1000,0);
	fill_n(PostTrackEnergy,1000,0);
	fill_n(PostTrackKEnergy,1000,0);

	nStep = 0;
	fill_n(StepTrackID,1000,0);
	fill_n(StepDetID,1000,0);
	fill_n(StepVX,1000,0);
	fill_n(StepVY,1000,0);
	fill_n(StepVZ,1000,0);
	fill_n(StepEdep,1000,0);
	EdepSumBox = 0;
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
	if(opt == MCTrack)	// starting point, prev track
	{
		nTrack++;
		int idx = find_index(TrackID);
		TrackID[idx] = trkID;
		ParentID[idx] = parentID;
		TrackPDG[idx] = pdg;
		TrackDetID[idx] = detID;
		TrackPX[idx] = p.x();
		TrackPY[idx] = p.y();
		TrackPZ[idx] = p.z();
		TrackVX[idx] = v.x();
		TrackVY[idx] = v.y();
		TrackVZ[idx] = v.z();
		TrackEnergy[idx] = totenergy;
		TrackKEnergy[idx] = kinenergy;
	}
	else if(opt == MCPostTrack)	// end point, post Track
	{
		int idx = find_index(PostTrackID);
		nPostTrack++;
		PostTrackID[idx] = trkID;
		PostTrackDetID[idx] = detID;
		PostTrackPX[idx] = p.x();
		PostTrackPY[idx] = p.y();
		PostTrackPZ[idx] = p.z();
		PostTrackVX[idx] = v.x();
		PostTrackVY[idx] = v.y();
		PostTrackVZ[idx] = v.z();
		PostTrackEnergy[idx] = totenergy;
		PostTrackKEnergy[idx] = kinenergy;
	}
	else
	{
		G4cout << "Error in OpRunAction::FillTrack()" << G4endl;
	}
}

void OpRunAction::FillStep
(G4int trkID, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep)
{
	int idx = find_index(StepTrackID);
	if(prev_detID != post_detID)	// at the boundary
		StepDetID[idx] = post_detID;	// mainly this case means particle track killed in a volume
	else
		StepDetID[idx] = prev_detID;
	nStep++;
	StepTrackID[idx] = trkID;
	StepVX[idx] = v.x();
	StepVY[idx] = v.y();
	StepVZ[idx] = v.z();
	StepEdep[idx] = edep;

	if(StepDetID[idx] == PC -> GetParInt("BoxID"))
		EdepSumBox += edep;
}


int OpRunAction::find_index(int*a) 
{
	for(int b=0; b<1000; b++)
	{
		if(a[b]!=0) continue;
		else return b;
	}
}

