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
	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();
	G4int parentID = track -> GetParentID();
	G4int detID = track -> GetVolume() -> GetCopyNo();
	G4double time = track -> GetGlobalTime();
	G4ThreeVector p = track -> GetMomentum();
	G4ThreeVector v = track -> GetPosition();
//	if(parentID == 0)
//	{
//		G4cout << "OpTrackingAction::PreUserTrackingAction" << G4endl;
//		G4cout << "trackX: " << v.x() << G4endl;
//		G4cout << "trackY: " << v.y() << G4endl;
//		G4cout << "trackZ: " << v.z() << G4endl;
//	}
	G4double totenergy = track -> GetTotalEnergy();
	G4double kinenergy = track -> GetKineticEnergy();

	const G4VProcess* process = track -> GetCreatorProcess();
//	G4ProcessType processType = process -> GetProcessType();
	G4int processID = 0;
	if(parentID != 0)
	{
		processID = process -> GetProcessSubType();
		fRunAction -> SetProcess(processID, process -> GetProcessName());
	}

	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
		 OpParameterContainer::GetInstance() -> GetParBool("OpTrack"))
	{
		// Scintillation 22
		// Cerenkov 21
		fRunAction -> FillOpticalPhoton
			(MCTrack, trkID, processID, parentID, detID, p, v, time, totenergy, kinenergy);
	}

	if(track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() &&
			OpParameterContainer::GetInstance()->GetParBool("MCTrack"))
		fRunAction -> FillTrack(MCTrack, trkID, processID, parentID, pdg, detID, p, v, totenergy, kinenergy);
}

void OpTrackingAction::PostUserTrackingAction(const G4Track* track)
{
	G4int trkID = track -> GetTrackID();
	G4int parentID = track -> GetParentID();
	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();
	G4int detID = track -> GetVolume() -> GetCopyNo();
	G4ThreeVector p = track -> GetMomentum();
	G4ThreeVector v = track -> GetPosition();
//	G4ThreeVector p = track -> GetStep() -> GetPreStepPoint() -> GetMomentum();
//	G4ThreeVector v = track -> GetStep() -> GetPreStepPoint() -> GetPosition();
	G4double time = track -> GetGlobalTime();
//	G4double time = track -> GetStep() -> GetPreStepPoint() -> GetGlobalTime();
	G4int processID = -999;
	G4double totenergy = track -> GetStep() -> GetPreStepPoint() -> GetTotalEnergy();
	G4double kinenergy = track -> GetStep() -> GetPreStepPoint() -> GetKineticEnergy();

	const G4VProcess* process = track -> GetStep() -> GetPostStepPoint() -> GetProcessDefinedStep();
//	G4ProcessType processType = process -> GetProcessType();
	processID = process -> GetProcessSubType();
	fRunAction -> SetProcess(processID, process -> GetProcessName());
	// Transportation 91
	// OpAbsorption 31
	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
		 OpParameterContainer::GetInstance() -> GetParBool("OpPostTrack"))
	{
		fRunAction -> FillOpticalPhoton
			(MCPostTrack, trkID, processID, parentID, detID, p, v, time, totenergy, kinenergy);
	}


	if(track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() &&
		 OpParameterContainer::GetInstance()->GetParBool("MCPostTrack"))
		fRunAction -> FillTrack
			(MCPostTrack, trkID, processID,parentID, pdg, detID, p, v, totenergy,kinenergy);
}
