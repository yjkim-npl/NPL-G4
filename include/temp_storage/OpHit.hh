#ifndef OpHit_h
#define OpHit_h

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
using namespace std;

enum {first,last};

class OpHit : public G4VHit
{
	public:
		OpHit();
		OpHit(const OpHit& right);
		virtual ~OpHit();

		const OpHit& operator=(const OpHit &right);
		G4bool operator==(const OpHit &right);

		void Print();

		void photonCount()
			{ fPhotons++; }
		G4int GetPhotonCount(G4int opt);
			{ return fPhotons; }
	private:
		G4int fPhotons;	// amount of photons 
};

typedef G4THitsCollection<OpHit> OpHitsCollection;

#endif
