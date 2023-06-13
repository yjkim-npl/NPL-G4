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
#define max_step 25000
#define max_photons 100000

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

//		G4int find_OpIndex(G4int trkID);
//		G4int find_StepIndex(G4int trkID);

		TFile* F;
		TTree* T;

		G4int nevnts;

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
		G4double TrackEdepSum[max_tracks];

//		vector<G4int> TrackID;
//		vector<G4int> TrackProcID;
//		vector<G4int> ParentID;
//		vector<G4int> TrackPDG;
//		vector<G4int> TrackDetID;
//		vector<G4double> TrackPX;
//		vector<G4double> TrackPY;
//		vector<G4double> TrackPZ;
//		vector<G4double> TrackVX;
//		vector<G4double> TrackVY;
//		vector<G4double> TrackVZ;
//		vector<G4double> TrackEnergy;
//		vector<G4double> TrackKEnergy;
//		vector<G4double> TrackEdepSum;

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

//		vector<G4int> PostTrackID;
//		vector<G4int> PostTrackProcID;
//		vector<G4int> PostTrackPDG;
//		vector<G4int> PostTrackDetID;
//		vector<G4double> PostTrackPX;
//		vector<G4double> PostTrackPY;
//		vector<G4double> PostTrackPZ;
//		vector<G4double> PostTrackVX;
//		vector<G4double> PostTrackVY;
//		vector<G4double> PostTrackVZ;
//		vector<G4double> PostTrackEnergy;
//		vector<G4double> PostTrackKEnergy;

		// Step data
		G4int nStep;
		G4int StepFromHit[max_step];
		G4int StepTrackID[max_step];
		G4int StepProcID[max_step];
		G4int StepTrackPDG[max_step];
		G4int StepPrevDetID[max_step];
		G4int StepPostDetID[max_step];
		G4int IsBoundary[max_step];		// for SteppingAction
		G4int StepNSecondaryOP[max_step];
		G4double StepPrevKE[max_step];	// for SteppingAction
		G4double StepVX[max_step];
		G4double StepVY[max_step];
		G4double StepVZ[max_step];
		G4double StepEdep[max_step];
		G4double StepLength[max_step];

//		vector<G4int> StepFromHit;
//		vector<G4int> StepTrackID;
//		vector<G4int> StepProcID;
//		vector<G4int> StepTrackPDG;
//		vector<G4int> StepPrevDetID;
//		vector<G4int> StepPostDetID;
//		vector<G4int> IsBoundary;		// for SteppingAction
//		vector<G4double> StepPrevKE;	// for SteppingAction
//		vector<G4double> StepVX;
//		vector<G4double> StepVY;
//		vector<G4double> StepVZ;
//		vector<G4double> StepEdep;
//		vector<G4double> StepLength;
//		vector<G4int> StepNSecondaryOP;

		// Optical photon
		G4int NOp;
//		G4int OpTrackID[max_photons];
//		G4int OpProcessID[max_photons];
//		G4int OpParentID[max_photons];
//		G4int OpDetID[max_photons];
//		G4double OpPX[max_photons];
//		G4double OpPY[max_photons];
//		G4double OpPZ[max_photons];
//		G4double OpVX[max_photons];
//		G4double OpVY[max_photons];
//		G4double OpVZ[max_photons];
//		G4double OpEnergy[max_photons];
//		G4double OpKEnergy[max_photons];
//		G4double OpTime[max_photons];

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
//		G4int PostOpTrackID[max_photons];
//		G4int PostOpDetID[max_photons];
//		G4int PostProcID[max_photons];
//		G4double OpTrackLength[max_photons];
//		G4double PostOpPX[max_photons];
//		G4double PostOpPY[max_photons];
//		G4double PostOpPZ[max_photons];
//		G4double PostOpVX[max_photons];
//		G4double PostOpVY[max_photons];
//		G4double PostOpVZ[max_photons];
//		G4double PostOpEnergy[max_photons];
//		G4double PostOpKEnergy[max_photons];
//		G4double PostOpTime[max_photons];

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
		G4int OpTrackIDBoundary[max_photons];
		G4int OpProcIDBoundary[max_photons];
		G4double OpPXBoundary[max_photons];
		G4double OpPYBoundary[max_photons];
		G4double OpPZBoundary[max_photons];
		G4double OpVXBoundary[max_photons];
		G4double OpVYBoundary[max_photons];
		G4double OpVZBoundary[max_photons];
		G4double OpTBoundary[max_photons];

//		vector<G4int> OpTrackIDBoundary;
//		vector<G4int> OpProcIDBoundary;
//		vector<G4double> OpPXBoundary;
//		vector<G4double> OpPYBoundary;
//		vector<G4double> OpPZBoundary;
//		vector<G4double> OpVXBoundary;
//		vector<G4double> OpVYBoundary;
//		vector<G4double> OpVZBoundary;
//		vector<G4double> OpTBoundary;

		// for SiPM readout
		G4int NOpSiPM;
		G4int OpSiPMProcID[max_photons];
		G4int OpSiPMDetID[max_photons];
		G4double OpSiPMVX[max_photons];
		G4double OpSiPMVY[max_photons];
		G4double OpSiPMVZ[max_photons];
		G4double OpSiPMPX[max_photons];
		G4double OpSiPMPY[max_photons];
		G4double OpSiPMPZ[max_photons];
		G4double OpSiPMTime[max_photons];
		G4double OpSiPMEnergy[max_photons];
		G4String OpSiPMProcName[max_photons];

//		vector<G4int> OpSiPMProcID;
//		vector<G4int> OpSiPMDetID;
//		vector<G4double> OpSiPMVX;
//		vector<G4double> OpSiPMVY;
//		vector<G4double> OpSiPMVZ;
//		vector<G4double> OpSiPMPX;
//		vector<G4double> OpSiPMPY;
//		vector<G4double> OpSiPMPZ;
//		vector<G4double> OpSiPMTime;
//		vector<G4double> OpSiPMEnergy;
//		vector<G4String> OpSiPMProcName;
};
#endif

