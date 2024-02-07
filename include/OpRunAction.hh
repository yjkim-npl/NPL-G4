#ifndef OpRunAction_h
#define OpRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TStopwatch.h"
//#include "TNamed.h"

#include <vector>
#include <map>
#include <utility>
#include <set>

#define max_t 10000
#define max_s 10000

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

		virtual void BeginOfRunAction(const G4Run*);
		virtual void   EndOfRunAction(const G4Run*);

		void clear_data();

		void FillTrack
			(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int pdg, G4int detID,
			 G4ThreeVector p, G4ThreeVector v, G4double time, G4double totenergy, G4double kinenergy);

		void FillOpticalPhoton
			(G4int opt, G4int trkID, G4int creProcID, G4int parentID, G4int detID, G4ThreeVector p, G4ThreeVector v, G4double time, G4double energy, G4double kenergy,G4double length=0);

		void FillOpticalPhotonBoundary
			(G4int trkID, G4int procID, G4ThreeVector p, G4ThreeVector v, G4double t);

		void FillStep
			(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, 
			 G4int prev_detID, G4int post_detID,
			 G4ThreeVector v, G4double time, G4double edep, G4double length, G4int nSecondaries, G4double prevKE=0);

		void FillSiPM
			(G4int detID, G4int procID, G4String procName,
			 G4ThreeVector pos, G4ThreeVector mom, G4double time, G4double Energy);

		void update_Tree();

		void PrintData(G4int opt);

		void SetProcess(G4int procID, G4String procTypeName);

		void CountEvent()
			{nevnts++;}

	private:
		TStopwatch stopwatch;
		OpParameterContainer* PC;
		TList fInputParameters;
		map<G4String, G4String> map_input_para;
		TList fProcessTable;
		map<G4String,G4int> map_process;

		TFile* F;
		TTree* T;

		G4int nevnts;

		// Track data
		G4int nTrack;
		vector<G4int> v_TrackID;
		vector<G4int> v_TrackProcID;
		vector<G4int> v_ParentID;
		vector<G4int> v_TrackPDG;
		vector<G4int> v_TrackDetID;
		vector<G4double> v_TrackPX;
		vector<G4double> v_TrackPY;
		vector<G4double> v_TrackPZ;
		vector<G4double> v_TrackVX;
		vector<G4double> v_TrackVY;
		vector<G4double> v_TrackVZ;
		vector<G4double> v_TrackTime;
		vector<G4double> v_TrackEnergy;
		vector<G4double> v_TrackKEnergy;
		vector<G4double> v_TrackEdepSum;

		G4int TrackID[max_t];
		G4int TrackProcID[max_t];
		G4int ParentID[max_t];
		G4int TrackPDG[max_t];
		G4int TrackDetID[max_t];
		G4double TrackPX[max_t];
		G4double TrackPY[max_t];
		G4double TrackPZ[max_t];
		G4double TrackVX[max_t];
		G4double TrackVY[max_t];
		G4double TrackVZ[max_t];
		G4double TrackTime[max_t];
		G4double TrackEnergy[max_t];
		G4double TrackKEnergy[max_t];
		G4double TrackEdepSum[max_t];


		// PostTrack data
		G4int nPostTrack;
		vector<G4int> v_PostTrackID;
		vector<G4int> v_PostTrackProcID;
		vector<G4int> v_PostTrackPDG;
		vector<G4int> v_PostTrackDetID;
		vector<G4double> v_PostTrackPX;
		vector<G4double> v_PostTrackPY;
		vector<G4double> v_PostTrackPZ;
		vector<G4double> v_PostTrackVX;
		vector<G4double> v_PostTrackVY;
		vector<G4double> v_PostTrackVZ;
		vector<G4double> v_PostTrackTime;
		vector<G4double> v_PostTrackEnergy;
		vector<G4double> v_PostTrackKEnergy;

		G4int PostTrackID[max_t];
		G4int PostTrackProcID[max_t];
		G4int PostTrackPDG[max_t];
		G4int PostTrackDetID[max_t];
		G4double PostTrackPX[max_t];
		G4double PostTrackPY[max_t];
		G4double PostTrackPZ[max_t];
		G4double PostTrackVX[max_t];
		G4double PostTrackVY[max_t];
		G4double PostTrackVZ[max_t];
		G4double PostTrackTime[max_t];
		G4double PostTrackEnergy[max_t];
		G4double PostTrackKEnergy[max_t];
		// Step data
		G4int nStep;
		vector<G4int> v_StepFromHit;
		vector<G4int> v_StepTrackID;
		vector<G4int> v_StepProcID;
		vector<G4int> v_StepTrackPDG;
		vector<G4int> v_StepPrevDetID;
		vector<G4int> v_StepPostDetID;
		vector<G4int> v_IsBoundary;		// for SteppingAction
		vector<G4double> v_StepPrevKE;	// for SteppingAction
		vector<G4double> v_StepVX;
		vector<G4double> v_StepVY;
		vector<G4double> v_StepVZ;
		vector<G4double> v_StepTime;
		vector<G4double> v_StepEdep;
		vector<G4double> v_StepLength;
		vector<G4int> v_StepNSecondaryOP;

		G4int StepFromHit[max_s];
		G4int StepTrackID[max_s];
		G4int StepProcID[max_s];
		G4int StepTrackPDG[max_s];
		G4int StepPrevDetID[max_s];
		G4int StepPostDetID[max_s];
		G4int IsBoundary[max_s];		// for SteppingAction
		G4double StepPrevKE[max_s];	// for SteppingAction
		G4double StepVX[max_s];
		G4double StepVY[max_s];
		G4double StepVZ[max_s];
		G4double StepTime[max_s];
		G4double StepEdep[max_s];
		G4double StepLength[max_s];
		G4int StepNSecondaryOP[max_s];

		// Optical photon
		G4int NOp;

//		G4double OpPX[1000];
//		G4double OpPY[1000];
//		G4double OpPZ[1000];

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

//		G4int PostProcID[1000];
//		G4double PostOpPX[1000];
//		G4double PostOpPY[1000];
//		G4double PostOpPZ[1000];
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
//		G4int OpTrackIDBoundary[1000];
//		G4int OpProcIDBoundary[1000];
//		G4double OpPXBoundary[1000];
//		G4double OpPYBoundary[1000];
//		G4double OpPZBoundary[1000];
//		G4double OpVXBoundary[1000];
//		G4double OpVYBoundary[1000];
//		G4double OpVZBoundary[1000];
//		G4double OpTBoundary[1000];

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
};
#endif

