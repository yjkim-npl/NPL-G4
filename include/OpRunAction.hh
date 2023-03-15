#ifndef OpRunAction_h
#define OpRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TList.h"
//#include "TNamed.h"

#include <vector>
#include <map>
#include <utility>
#include <set>

#define max_tracks 10000
#define max_steps 10000
#define max_opticalphotons 1000000
using namespace std;

class OpParameterContainer;

class G4Run;

enum {MCTrack,MCPostTrack, OpticalPhoton, Process};	// Opt for FillTrack

class OpRunAction : public G4UserRunAction
{
	public:
	  OpRunAction();
    virtual ~OpRunAction();

		void init_Tree();

	    // virtual G4Run* GenerateRun();
    	virtual void BeginOfRunAction(const G4Run*);
	    virtual void   EndOfRunAction(const G4Run*);

		void clear_data();

		void FillTrack
			(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int pdg, G4int detID,
			 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy);

		void FillOpticalPhoton
			(G4int opt, G4int trkID, G4int creProcID, G4int parentID, G4int detID, G4ThreeVector p, G4ThreeVector v, G4double time, G4double energy, G4double kenergy);

		void FillOpticalPhotonBoundary
			(G4int trkID, G4int procID, G4ThreeVector p, G4ThreeVector v, G4double t);

		void FillStep
			(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, G4int prev_detID, G4int post_detID,
			 G4ThreeVector v, G4double edep, G4double prevKE=0);

		void update_Tree();

		void PrintData(G4int opt);

		void SetProcess(G4int procID, G4String procTypeName);

		void SetInputParameters(G4int nevnts);

	private:
		OpParameterContainer* PC;
		TList fInputParameters;
		map<G4String, G4String> map_input_para;
		TList fProcessTable;
		map<G4String,G4int> map_process;

		G4int find_OpIndex(G4int trkID);
		G4int find_StepIndex(G4int trkID);

		TFile* F;
		TTree* T;

		set<G4int> set_procID;
		// Track data
		G4int nTrack;
		G4int TrackID[max_tracks];
		G4int TrackProcID[max_tracks];
		G4int ParentID[max_tracks];
		G4int TrackPDG[max_tracks];
		G4int TrackDetID[max_tracks];
		G4double TrackPX[max_tracks];
		G4double TrackPY[max_tracks];
		G4double TrackPZ[max_tracks];
		G4double TrackVX[max_tracks];
		G4double TrackVY[max_tracks];
		G4double TrackVZ[max_tracks];
		G4double TrackEnergy[max_tracks];
		G4double TrackKEnergy[max_tracks];
		G4double TrackEdepSum[max_steps];

		// PostTrack data
		G4int nPostTrack;
		G4int PostTrackID[max_tracks];
		G4int PostTrackProcID[max_tracks];
		G4int PostTrackPDG[max_tracks];
		G4int PostTrackDetID[max_tracks];
		G4double PostTrackPX[max_tracks];
		G4double PostTrackPY[max_tracks];
		G4double PostTrackPZ[max_tracks];
		G4double PostTrackVX[max_tracks];
		G4double PostTrackVY[max_tracks];
		G4double PostTrackVZ[max_tracks];
		G4double PostTrackEnergy[max_tracks];
		G4double PostTrackKEnergy[max_tracks];

		// Step data
		G4int nStep;
		G4int StepFromHit[max_steps];
		G4int StepTrackID[max_steps];
		G4int StepProcID[max_steps];
		G4int StepTrackPDG[max_steps];
		G4int StepPrevDetID[max_steps];
		G4int StepPostDetID[max_steps];
		G4int IsBoundary[max_steps];		// for SteppingAction
		G4double StepPrevKE[max_steps];	// for SteppingAction
		G4double StepVX[max_steps];
		G4double StepVY[max_steps];
		G4double StepVZ[max_steps];
		G4double StepEdep[max_steps];

		G4double EdepSumBox;

		// Optical photon
		G4int NOpticalPhotons;
		G4int OpTrackID[max_opticalphotons];
		G4int OpProcessID[max_opticalphotons];
		G4int OpParentID[max_opticalphotons];
		G4int OpDetID[max_opticalphotons];
		G4double OpPX[max_opticalphotons];
		G4double OpPY[max_opticalphotons];
		G4double OpPZ[max_opticalphotons];
		G4double OpVX[max_opticalphotons];
		G4double OpVY[max_opticalphotons];
		G4double OpVZ[max_opticalphotons];
		G4double OpEnergy[max_opticalphotons];
		G4double OpKEnergy[max_opticalphotons];
		G4double OpTime[max_opticalphotons];

		G4int PostOpDetID[max_opticalphotons];
		G4int PostProcID[max_opticalphotons];
		G4double PostOpPX[max_opticalphotons];
		G4double PostOpPY[max_opticalphotons];
		G4double PostOpPZ[max_opticalphotons];
		G4double PostOpVX[max_opticalphotons];
		G4double PostOpVY[max_opticalphotons];
		G4double PostOpVZ[max_opticalphotons];
		G4double PostOpEnergy[max_opticalphotons];
		G4double PostOpKEnergy[max_opticalphotons];
		G4double PostOpTime[max_opticalphotons];

		G4int NOpBoundary;
		G4int OpTrackIDBoundary[max_opticalphotons];
		G4int OpProcIDBoundary[max_opticalphotons];
		G4double OpPXBoundary[max_opticalphotons];
		G4double OpPYBoundary[max_opticalphotons];
		G4double OpPZBoundary[max_opticalphotons];
		G4double OpVXBoundary[max_opticalphotons];
		G4double OpVYBoundary[max_opticalphotons];
		G4double OpVZBoundary[max_opticalphotons];
		G4double OpTBoundary[max_opticalphotons];

		// temp
		G4int Ntemp;
		G4int* temp_trackID;
		G4double temp_Edepsum;
		G4double* temp_Edep;
};
#endif

