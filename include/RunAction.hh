#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"

#include <vector>
#include <map>
using namespace std;

class ParameterContainer;

class G4Run;

enum {MCTrack,MCPostTrack};	// Opt for FillTrack
enum {StepAll, StepTot};	// Opt for FillStep

class RunAction : public G4UserRunAction
{
	public:
	    RunAction(ParameterContainer* par);
    	virtual ~RunAction();

		void init();

	    // virtual G4Run* GenerateRun();
    	virtual void BeginOfRunAction(const G4Run*);
	    virtual void   EndOfRunAction(const G4Run*);

		void clear();

		void FillTrack
			(G4int opt, G4int trkID, G4int parentID, G4int pdg, G4int detID,
			 G4ThreeVector p, G4ThreeVector v, G4double energy);

		void FillStep
			(G4int opt, G4int trkID, G4int detID,
			 G4ThreeVector v, G4double edep);

		void update()
			{T -> Fill();}

	private:
		ParameterContainer* PC;

		int find_index(int* a);

		TFile* F;
		TTree* T;

		// Track data
		G4int nTrack;
		G4int TrackID[500];
		G4int ParentID[500];
		G4int TrackPDG[500];
		G4int TrackDetID[500];
		G4double TrackPX[500];
		G4double TrackPY[500];
		G4double TrackPZ[500];
		G4double TrackVX[500];
		G4double TrackVY[500];
		G4double TrackVZ[500];
		G4double TrackEnergy[500];

		// PostTrack data
		G4int nPostTrack;
		G4int PostTrackID[500];
		G4int PostTrackPDG[500];
		G4int PostTrackDetID[500];
		G4double PostTrackPX[500];
		G4double PostTrackPY[500];
		G4double PostTrackPZ[500];
		G4double PostTrackVX[500];
		G4double PostTrackVY[500];
		G4double PostTrackVZ[500];
		G4double PostTrackEnergy[500];

		// Step data
		G4int nStep;
		G4int StepTrackID[500];
		G4int StepDetID[500];
		G4double StepVX[500];
		G4double StepVY[500];
		G4double StepVZ[500];
		G4double StepEdep[500];
//		G4double StepEdepTot;
};
#endif
