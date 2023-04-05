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

#define max_tracks 100000
#define max_steps 1000000
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
			(G4int opt, G4int trkID, G4int creProcID, G4int parentID, G4int detID, G4ThreeVector p, G4ThreeVector v, G4double time, G4double energy, G4double kenergy,G4double length=0);

		void FillOpticalPhotonBoundary
			(G4int trkID, G4int procID, G4ThreeVector p, G4ThreeVector v, G4double t);

		void FillStep
			(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, 
			 G4int prev_detID, G4int post_detID,
			 G4ThreeVector v, G4double edep, G4double length, G4int nSecondaries, G4double prevKE=0);

		void FillSiPM
			(G4int detID, G4int procID, G4String procName,
			 G4ThreeVector pos, G4ThreeVector mom, G4double time, G4double Energy);

		void update_Tree();

		void PrintData(G4int opt);

		void SetProcess(G4int procID, G4String procTypeName);

		void CountEvent()
			{nevnts++;}

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

		G4int nevnts;

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
		G4double StepLength[max_steps];
		G4int StepNSecondaryOP[max_steps];

		G4double EdepSumBox;

		// Optical photon
		G4int NOp;
		vector<G4int> OpTrackID;
		vector<G4int> OpProcessID;
		vector<G4int> OpParentID;
		vector<G4int> OpDetID;
		vector<G4double> OpPX;
		vector<G4double> OpPY;
		vector<G4double> OpPZ;
		vector<G4double> OpVX;
		vector<G4double> OpVY;
		vector<G4double> OpVZ;
		vector<G4double> OpEnergy;
		vector<G4double> OpKEnergy;
		vector<G4double> OpTime;

		G4int PostNOp;
		vector<G4int> PostOpTrackID;
		vector<G4int> PostOpDetID;
		vector<G4int> PostProcID;
		vector<G4double> OpTrackLength;
		vector<G4double> PostOpPX;
		vector<G4double> PostOpPY;
		vector<G4double> PostOpPZ;
		vector<G4double> PostOpVX;
		vector<G4double> PostOpVY;
		vector<G4double> PostOpVZ;
		vector<G4double> PostOpEnergy;
		vector<G4double> PostOpKEnergy;
		vector<G4double> PostOpTime;

		G4int NOpBoundary;
		vector<G4int> OpTrackIDBoundary;
		vector<G4int> OpProcIDBoundary;
		vector<G4double> OpPXBoundary;
		vector<G4double> OpPYBoundary;
		vector<G4double> OpPZBoundary;
		vector<G4double> OpVXBoundary;
		vector<G4double> OpVYBoundary;
		vector<G4double> OpVZBoundary;
		vector<G4double> OpTBoundary;

		// for SiPM readout
		G4int NOpSiPM;
		vector<G4int> OpSiPMProcID;
		vector<G4int> OpSiPMDetID;
		vector<G4double> OpSiPMVX;
		vector<G4double> OpSiPMVY;
		vector<G4double> OpSiPMVZ;
		vector<G4double> OpSiPMPX;
		vector<G4double> OpSiPMPY;
		vector<G4double> OpSiPMPZ;
		vector<G4double> OpSiPMTime;
		vector<G4double> OpSiPMEnergy;
		vector<G4String> OpSiPMProcName;

		// temp
		G4int Ntemp;
		G4int* temp_trackID;
		G4double temp_Edepsum;
		G4double* temp_Edep;
};
#endif

