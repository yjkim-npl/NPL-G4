#include "OpTrackingAction.hh"
#include "OpParameterContainer.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Event.hh"
#include "G4VProcess.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

OpTrackingAction::OpTrackingAction(OpRunAction* runAction)
:G4UserTrackingAction(),
 fRunAction(runAction)
{
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpTrackingAction" << G4endl;

}

OpTrackingAction::~OpTrackingAction()
{
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpTrackingAction" << G4endl;
}

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
		G4int processID = track -> GetCreatorProcess() -> GetProcessSubType();
		// Scintillation 22
		// Cerenkov 21
//		G4String processName = track -> GetCreatorProcess() -> GetProcessName();
//		G4cout << "Creator Process: "  <<  processName << " " << processID << G4endl;
		fRunAction -> FillOpticalPhoton(MCTrack, trkID, processID, parentID, detID, p, v, time);
	}
	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();

	G4double totenergy = track -> GetTotalEnergy();
	G4double kinenergy = track -> GetKineticEnergy();

//	fRunAction -> FillTrack(MCTrack, trkID, parentID, pdg, detID, p, v, totenergy, kinenergy);
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
		const G4VProcess* process = track -> GetStep() -> GetPostStepPoint() -> GetProcessDefinedStep();
		G4int processID = process -> GetProcessSubType();
		// Transportation 91
		// OpAbsorption 31
//		G4String processName = process -> GetProcessName();
		fRunAction -> FillOpticalPhoton(MCPostTrack, trkID, processID, parentID, detID, p, v, time);
	}

	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();

	G4double totenergy = track -> GetTotalEnergy();
	G4double kinenergy = track -> GetKineticEnergy();

//	fRunAction -> FillTrack(MCPostTrack, trkID, parentID, pdg, detID, p, v, totenergy,kinenergy);
}
