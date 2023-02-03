#include "OpTrackingAction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

OpTrackingAction::OpTrackingAction(OpRunAction* runAction)
:G4UserTrackingAction(),
 fRunAction(runAction)
{
}

OpTrackingAction::~OpTrackingAction()
{}

void OpTrackingAction::PreUserTrackingAction(const G4Track* track)
{
	G4int trkID = track -> GetTrackID();
	G4int parentID = track -> GetParentID();
	G4int detID = track -> GetVolume() -> GetCopyNo();
	G4double time = track -> GetGlobalTime();
	G4ThreeVector p = track -> GetMomentum();
	G4ThreeVector v = track -> GetPosition();

	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
	{
//		G4cout << "OpticalPhoton is created" << G4endl;
		fRunAction -> FillOpticalPhoton(MCTrack, trkID, parentID, detID, p, v, time);
	}
	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();

	G4double totenergy = track -> GetTotalEnergy();
	G4double kinenergy = track -> GetKineticEnergy();

	fRunAction -> FillTrack(MCTrack, trkID, parentID, pdg, detID, p, v, totenergy, kinenergy);
}

void OpTrackingAction::PostUserTrackingAction(const G4Track* track)
{
	G4int trkID = track -> GetTrackID();
	G4int parentID = track -> GetParentID();
	G4int detID = track -> GetVolume() -> GetCopyNo();
	G4ThreeVector p = track -> GetMomentum();
	G4ThreeVector v = track -> GetPosition();
	G4double time = track -> GetGlobalTime();

	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
	{
//		G4cout << "OpticalPhoton is dead" << G4endl;
		fRunAction -> FillOpticalPhoton(MCPostTrack, trkID, parentID, detID, p, v, time);
	}

	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();

	G4double totenergy = track -> GetTotalEnergy();
	G4double kinenergy = track -> GetKineticEnergy();

	fRunAction -> FillTrack(MCPostTrack, trkID, parentID, pdg, detID, p, v, totenergy,kinenergy);
}
