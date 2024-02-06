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
		vector<G4int> TrackID;
		vector<G4int> TrackProcID;
		vector<G4int> ParentID;
		vector<G4int> TrackPDG;
		vector<G4int> TrackDetID;
		vector<G4double> TrackPX;
		vector<G4double> TrackPY;
		vector<G4double> TrackPZ;
		vector<G4double> TrackVX;
		vector<G4double> TrackVY;
		vector<G4double> TrackVZ;
		vector<G4double> TrackEnergy;
		vector<G4double> TrackKEnergy;
		vector<G4double> TrackEdepSum;

		// PostTrack data
		G4int nPostTrack;
		vector<G4int> PostTrackID;
		vector<G4int> PostTrackProcID;
		vector<G4int> PostTrackPDG;
		vector<G4int> PostTrackDetID;
		vector<G4double> PostTrackPX;
		vector<G4double> PostTrackPY;
		vector<G4double> PostTrackPZ;
		vector<G4double> PostTrackVX;
		vector<G4double> PostTrackVY;
		vector<G4double> PostTrackVZ;
		vector<G4double> PostTrackEnergy;
		vector<G4double> PostTrackKEnergy;

		// Step data
		G4int nStep;
		vector<G4int> StepFromHit;
		vector<G4int> StepTrackID;
		vector<G4int> StepProcID;
		vector<G4int> StepTrackPDG;
		vector<G4int> StepPrevDetID;
		vector<G4int> StepPostDetID;
		vector<G4int> IsBoundary;		// for SteppingAction
		vector<G4double> StepPrevKE;	// for SteppingAction
		vector<G4double> StepVX;
		vector<G4double> StepVY;
		vector<G4double> StepVZ;
		vector<G4double> StepEdep;
		vector<G4double> StepLength;
		vector<G4int> StepNSecondaryOP;

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

