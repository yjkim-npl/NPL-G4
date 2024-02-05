#ifndef OpHit_h
#define OpHit_h 

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>
#include <utility>
using namespace std;

class OpHit : public G4VHit
{
	public:
		OpHit();
		OpHit(const OpHit &right);
		virtual ~OpHit();

		const OpHit& operator=(const OpHit &right);
		G4bool operator==(const OpHit &right);

		void AddHit(G4int trackID, G4int trackPDG, G4int detectorID);
		void AddPostDetID(G4int postDetID)
			{post_DetID = postDetID;}
		void CountStep()
			{nSteps++; vec_IsFirstStep.push_back(0);}
		void AddProcess(G4int procID, G4String procName)
			{vec_procID.push_back(procID); vec_procName.push_back(procName);}
		void AddBoundary(G4int boundary)
			{vec_IsBoundary.push_back(boundary);}
		void AddMomentum(G4ThreeVector p)
			{vec_p.push_back(p);}
		void AddPosition(G4ThreeVector v)
			{vec_v.push_back(v);}
		void AddTime(G4double t)
			{vec_t.push_back(t);}
		void AddEdep(G4double edep)
			{vec_Edep.push_back(edep); EdepSum+=edep;}
		void AddKE(G4double KE)
			{vec_prevKE.push_back(KE);}
		void AddStepLength(G4double length)
			{vec_length.push_back(length);}
		void AddNSecondaryOP(G4int NSecondaryOP)
			{vec_NSecondaryOP.push_back(NSecondaryOP);}


		G4int GetNSteps()
			{return nSteps;}
		G4int GetTrackID()
			{return trkID;}
		G4int GetTrackPDG()
			{return trkPDG;}
		G4int GetDetID()
			{return detID;}
		G4int GetPostDetID()
			{return post_DetID;}
		G4int GetIsBoundary(G4int idx)
			{return vec_IsBoundary[idx];}
		G4int GetIsFirstStep(G4int idx)
			{return vec_IsFirstStep[idx];}
		G4int GetProcID(G4int idx)
			{return vec_procID[idx];}
		G4int GetNSecondaryOP(G4int idx)
			{return vec_NSecondaryOP[idx];}
		G4String GetProcName(G4int idx) 
			{return vec_procName[idx];}
		G4ThreeVector GetMomentum(G4int idx) 
			{return vec_p[idx];}
		G4ThreeVector GetPosition(G4int idx) 
			{return vec_v[idx];}
		G4double GetTime(G4int idx)
			{return vec_t[idx];}
		G4double GetEdep(G4int idx)
			{return vec_Edep[idx];}
		G4double GetEdepSum() 
			{return EdepSum;}
		G4double GetKE(G4int idx)
			{return vec_prevKE[idx];}
		G4double GetStepLength(G4int idx)
			{return vec_length[idx];}

	private:
		G4int nSteps;
		G4int trkID;
		G4int trkPDG;
		G4int detID;	// prev det ID
		G4int post_DetID;
		G4double EdepSum;
		vector<G4int> vec_NSecondaryOP;
		vector<G4int> vec_IsBoundary;
		vector<G4int> vec_IsFirstStep;
		vector<G4double> vec_prevKE;
		vector<G4double> vec_Edep;
		vector<G4int> vec_procID;
		vector<G4String> vec_procName;
		vector<G4ThreeVector> vec_p;
		vector<G4ThreeVector> vec_v;
		vector<G4double> vec_t;
		vector<G4double> vec_length;
};
typedef G4THitsCollection<OpHit> OpHitsCollection;
#endif
