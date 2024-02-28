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

#define max_t 1000     // maximum number of (post)track 
#define max_s 5000     // maximum number of step 
#define max_o 30000    // maximum number of optical photon (post)track
#define max_ob 100000  // maximum number of boundary interactions 
#define max_os 20000   // maximum number of optical photon hits at SiPM

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
		G4int OpTrackID[max_o];
		G4int OpProcessID[max_o];
		G4int OpParentID[max_o];
		G4int OpDetID[max_o];
		G4double OpPX[max_o];
		G4double OpPY[max_o];
		G4double OpPZ[max_o];
		G4double OpVX[max_o];
		G4double OpVY[max_o];
		G4double OpVZ[max_o];
		G4double OpEnergy[max_o];
		G4double OpKEnergy[max_o];
		G4double OpTime[max_o];

		G4int PostNOp;
		G4int PostOpTrackID[max_o];
		G4int PostOpDetID[max_o];
		G4int PostProcID[max_o];
		G4double OpTrackLength[max_o];
		G4double PostOpPX[max_o];
		G4double PostOpPY[max_o];
		G4double PostOpPZ[max_o];
		G4double PostOpVX[max_o];
		G4double PostOpVY[max_o];
		G4double PostOpVZ[max_o];
		G4double PostOpEnergy[max_o];
		G4double PostOpKEnergy[max_o];
		G4double PostOpTime[max_o];

		G4int NOpBoundary;
		G4int OpTrackIDBoundary[max_ob];
		G4int OpProcIDBoundary[max_ob];
		G4double OpPXBoundary[max_ob];
		G4double OpPYBoundary[max_ob];
		G4double OpPZBoundary[max_ob];
		G4double OpVXBoundary[max_ob];
		G4double OpVYBoundary[max_ob];
		G4double OpVZBoundary[max_ob];
		G4double OpTBoundary[max_ob];

		// for SiPM readout
		G4int NOpSiPM;
		G4int OpSiPMProcID[max_os];
		G4int OpSiPMDetID[max_os];
		G4double OpSiPMVX[max_os];
		G4double OpSiPMVY[max_os];
		G4double OpSiPMVZ[max_os];
		G4double OpSiPMPX[max_os];
		G4double OpSiPMPY[max_os];
		G4double OpSiPMPZ[max_os];
		G4double OpSiPMTime[max_os];
		G4double OpSiPMEnergy[max_os];
		G4String OpSiPMProcName[max_os];
};
#endif

