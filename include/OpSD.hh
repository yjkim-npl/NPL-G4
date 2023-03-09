#ifndef OpSD_h
#define OpSD_h

#include "OpHit.hh"

#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"
#include "G4VSensitiveDetector.hh"
using namespace std;

class G4OpBoundaryProcess;

class OpSD : public G4VSensitiveDetector
{
	public:
		OpSD(const G4String &name, const G4String &HCname);
		virtual ~OpSD();

		virtual void Initialize(G4HCofThisEvent* HCE);
		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);

	private:
		G4OpBoundaryProcess* fOpProcess;
		OpHitsCollection* fHitsCollection;
		G4int fHCID;
};
#endif
