#include "SteppingAction.hh"
#include "RunAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

SteppingAction::SteppingAction(RunAction* runAction)
: G4UserSteppingAction(),
  fRunAction(runAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
	// information of each steps
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4double edep = step -> GetTotalEnergyDeposit();
	G4int prevNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

	fRunAction -> FillStep(StepAll, trackID, prevNo, pos, edep);
}
