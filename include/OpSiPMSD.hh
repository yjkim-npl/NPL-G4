#ifndef OpSiPMSD_h
#define OpSiPMSD_h

#include "OpSiPMHit.hh"

#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"
#include "G4VSensitiveDetector.hh"
using namespace std;

class G4OpBoundaryProcess;

class OpSiPMSD : public G4VSensitiveDetector
{
	public:
		OpSiPMSD(const G4String &name, const G4String &HCname);
		virtual ~OpSiPMSD();

		virtual void Initialize(G4HCofThisEvent* HCE);
		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);

	private:
		G4OpBoundaryProcess* fOpProcess;
		OpSiPMHitsCollection* fHitsCollection;
		G4int fHCID;
};
#endif
