#ifndef OpSiPMHit_h
#define OpSiPMHit_h 

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>
#include <utility>
using namespace std;

class OpSiPMHit : public G4VHit
{
	public:
		OpSiPMHit();
		OpSiPMHit(const OpSiPMHit &right);
		virtual ~OpSiPMHit();

		const OpSiPMHit& operator=(const OpSiPMHit &right);
		G4bool operator==(const OpSiPMHit &right);

		void AddHit(G4int detID)
			{fDetID = detID;}
		void photonCount()
			{fPhotons++;}
		void SetPosition(G4ThreeVector pos)
			{vec_vx.push_back(pos.x()); vec_vy.push_back(pos.y()); vec_vz.push_back(pos.z());}
		void SetMomentum(G4ThreeVector mom)
			{vec_px.push_back(mom.x()); vec_py.push_back(mom.y()); vec_pz.push_back(mom.z());}
		void SetTime(G4double t)
			{vec_t.push_back(t);}
		void SetEnergy(G4double e)
			{vec_E.push_back(e);}
		void SetProcess(G4int procID, G4String procName)
			{vec_procID.push_back(procID); vec_procName.push_back(procName);}

		G4int GetDetectorID()
			{return fDetID;}
		G4int GetPhotonCount()
			{return fPhotons;}
		G4int GetProcID(G4int idx)
			{return vec_procID[idx];}
		G4ThreeVector GetPosition(G4int idx)
			{G4ThreeVector pos(vec_vx[idx],vec_vy[idx],vec_vz[idx]); return pos;}
		G4ThreeVector GetMomentum(G4int idx)
			{G4ThreeVector mom(vec_px[idx],vec_py[idx],vec_pz[idx]); return mom;}
		G4double GetTime(G4int idx)
			{return vec_t[idx];}
		G4double GetEnergy(G4int idx)
			{return vec_E[idx];}
		G4String GetProcName(G4int idx)
			{return vec_procName[idx];}


	private:
		G4int fDetID;
		G4int fPhotons;
		vector<G4double> vec_vx, vec_vy, vec_vz;
		vector<G4double> vec_px, vec_py, vec_pz, vec_t, vec_E;
		vector<G4int> vec_procID;
		vector<G4String> vec_procName;
};
typedef G4THitsCollection<OpSiPMHit> OpSiPMHitsCollection;
#endif
