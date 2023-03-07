#ifndef OpSCHit_h
#define OpSCHit_h

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>

using namespace std;

class OpSCHit : public G4VHit
{
	public:
		OpSCHit();
		OpSCHit(const OpSCHit& right);
		virtual ~OpSCHit();

		const OpSCHit& operator=(const OpSCHit &right);
		G4bool operator==(const OpSCHit &right);

		void AddHit(G4int trackID, G4int trackPDG, G4int detectorID);
		void AddEdep(G4double fedep);

		G4int GetTrackID() {return trkID;}
		G4int GetTrackPDG() {return trkPDG;}
		G4int GetDetID() {return detID;}
		G4double GetEdep() {return edep;}
	private:
		G4int trkID;
		G4int trkPDG;
		G4int detID;
		G4double edep;
};
typedef G4THitsCollection<OpSCHit> OpSCHitsCollection;

#endif
