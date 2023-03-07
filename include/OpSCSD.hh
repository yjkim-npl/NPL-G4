#ifndef OpSCSD_h
#define OpSCSD_h

#include "OpSCHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
using namespace std;

class OpSCSD : public G4VSensitiveDetector
{
	public:
		OpSCSD(const G4String &name, const G4String &HCname);
		virtual ~OpSCSD();

		virtual void Initialize(G4HCofThisEvent* HCE);
		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);

	private:
		OpSCHitsCollection* fHitsCollection;
		G4int fHCID;
};
#endif
