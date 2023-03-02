#include "OpSD.hh"
#include "OpHit.hh"
#include "OpDetectorConstruction.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDMananger.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

OpSD::OpSD(const G4String &name, const G4String &HCname)
: G4VSensitiveDetector(name)
{
	collectionName.insert(HCname);
}

OpSD::~OpSD()
{
}

void OpSD::Initialize(G4HCofThisEvent* HCE)
{
	fHitCollection = new OpHitsCollection(SensitiveDetectorName,collectionName[0]);
	if(fHCID
