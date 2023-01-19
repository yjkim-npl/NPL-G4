#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "ParameterContainer.hh"

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

RunAction::RunAction(ParameterContainer* par)
: G4UserRunAction()
{ 
	PC = par;

	F = new TFile(PC -> GetParString("outName").c_str(),"recreate");
	T = new TTree("G4sim","G4sim");

	init();
}

RunAction::~RunAction()
{
	F -> Write();
	F -> Close();
}

void RunAction::init()
{
	bool bMCTrack = PC -> GetParBool("MCTrack");
	bool bMCPostTrack = PC -> GetParBool("MCPostTrack");
	bool bStepAll = PC -> GetParBool("StepAll");
	bool bStepTot = PC -> GetParBool("StepTot");

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
	}
	if(bMCPostTrack)
	{
		T -> Branch("nPostTrack",&nPostTrack);
		T -> Branch("PostTrackID",PostTrackID,"PostTrackID[nPostTrack]/I");
		T -> Branch("PostTrackPDG",PostTrackPDG,"PostTrackPDG[nPostTrack]/I");
		T -> Branch("PostTrackDetID",PostTrackDetID,"PostTrackDetID[nPostTrack]/I");

		T -> Branch("PostTrackPX",PostTrackPX,"PostTrackPX[nPostTrack]/D");
		T -> Branch("PostTrackPY",PostTrackPY,"PostTrackPY[nPostTrack]/D");
		T -> Branch("PostTrackPZ",PostTrackPZ,"PostTrackPZ[nPostTrack]/D");
		T -> Branch("PostTrackVX",PostTrackVX,"PostTrackVX[nPostTrack]/D");
		T -> Branch("PostTrackVY",PostTrackVY,"PostTrackVY[nPostTrack]/D");
		T -> Branch("PostTrackVZ",PostTrackVZ,"PostTrackVZ[nPostTrack]/D");
		T -> Branch("PostTrackEnergy",PostTrackEnergy,"PostTrackEnergy[nPostTrack]/D");
	}
	if(bStepAll || bStepTot)
	{
		T -> Branch("nStep",&nStep);
		T -> Branch("StepTrackID",StepTrackID,"StepTrackID[nStep]/I");
		T -> Branch("StepDetID",StepDetID,"StepDetID[nStep]/I");

		T -> Branch("StepVX",StepVX,"StepVX[nStep]/D");
		T -> Branch("StepVY",StepVY,"StepVY[nStep]/D");
		T -> Branch("StepVZ",StepVZ,"StepVZ[nStep]/D");
		T -> Branch("StepEdep",StepEdep,"StepEdep[nStep]/D");
	}
}

void RunAction::BeginOfRunAction(const G4Run*)
{ 
}

void RunAction::EndOfRunAction(const G4Run* run)
{
}

void RunAction::clear()
{
	nTrack = 0;
	fill_n(TrackID,500,0);
	fill_n(ParentID,500,0);
	fill_n(TrackPDG,500,0);
	fill_n(TrackDetID,500,0);
	fill_n(TrackPX,500,0);
	fill_n(TrackPY,500,0);
	fill_n(TrackPZ,500,0);
	fill_n(TrackVX,500,0);
	fill_n(TrackVY,500,0);
	fill_n(TrackVZ,500,0);
	fill_n(TrackEnergy,500,0);

	nPostTrack = 0;
	fill_n(PostTrackID,500,0);
	fill_n(PostTrackPDG,500,0);
	fill_n(PostTrackDetID,500,0);
	fill_n(PostTrackPX,500,0);
	fill_n(PostTrackPY,500,0);
	fill_n(PostTrackPZ,500,0);
	fill_n(PostTrackVX,500,0);
	fill_n(PostTrackVY,500,0);
	fill_n(PostTrackVZ,500,0);
	fill_n(PostTrackEnergy,500,0);

	nStep = 0;
	fill_n(StepTrackID,500,0);
	fill_n(StepDetID,500,0);
	fill_n(StepVX,500,0);
	fill_n(StepVY,500,0);
	fill_n(StepVZ,500,0);
	fill_n(StepEdep,500,0);
}

void RunAction::FillTrack
(G4int opt, G4int trkID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double energy)	// invoked in TrackingAction
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
		TrackEnergy[idx] = energy;
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
		PostTrackEnergy[idx] = energy;
	}
	else
	{
		G4cout << "Error in RunAction::FillTrack()" << G4endl;
	}
}

void RunAction::FillStep
(G4int opt, G4int trkID, G4int detID, G4ThreeVector v, G4double edep)
{
	if(opt == StepAll)
	{
		int idx = find_index(StepTrackID);
		nStep++;
		StepTrackID[idx] = trkID;
		StepDetID[idx] = detID;
		StepVX[idx] = v.x();
		StepVY[idx] = v.y();
		StepVZ[idx] = v.z();
		StepEdep[idx] = edep;
	}
	else
	{}
}


int RunAction::find_index(int*a) 
{
	for(int b=0; b<500; b++)
	{
		if(a[b]!=0) continue;
		else return b;
	}
}

