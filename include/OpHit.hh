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

		void AddHit(G4int trackID, G4int detectorID);
		void AddStep(G4int procID, G4String procName, G4ThreeVector p, G4ThreeVector v, G4double t);
//		void AddE(G4int opt, G4double e);

		G4int GetTrackID()
			{return trkID;}
		G4int GetDetID()
			{return detID;}
		G4int GetSizeOfVector()
			{return size_vec;}
		G4int GetProcID(G4int idx)
			{return vec_procID[idx];}
		G4String GetProcName(G4int idx) 
			{return vec_procName[idx];}
		G4ThreeVector GetMomentum(G4int idx) 
			{return vec_p[idx];}
		G4ThreeVector GetPosition(G4int idx) 
			{return vec_v[idx];}
		G4double GetTime(G4int idx)
			{return vec_t[idx];}
//		G4Double GetEnergy(G4int opt) 
//			{return opt==0?pair_Eif.first:opt==1?pair_Eif.second:-10;}

	private:
		G4int trkID;
		G4int detID;
		G4int size_vec;
		vector<G4int> vec_procID;
		vector<G4String> vec_procName;
		vector<G4ThreeVector> vec_p;
		vector<G4ThreeVector> vec_v;
		vector<G4double> vec_t;
//		pair<G4double,G4double> pair_Eif;
};
typedef G4THitsCollection<OpHit> OpHitsCollection;
#endif
