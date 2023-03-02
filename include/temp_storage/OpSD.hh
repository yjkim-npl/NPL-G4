#ifndef OpSD_h
#define OpSD_h

#include "OpHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

class OpSD : public G4VSensitiveDetector
{
	public:
		OpSD(const G4String &name, const G4String &HCname);
		virtual ~OpSD();

		virtual void Initialize(G4HCofThisEvent* HCE);
		virtual G4Bool ProcessHits(G4Step* aStep, G4TouchableHistory*);

	private:
		OpHitsCollection* fHitCollection;
};
#endif
