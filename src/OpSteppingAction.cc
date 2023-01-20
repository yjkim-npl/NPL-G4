#include "OpSteppingAction.hh"
#include "OpRunAction.hh"
#include "OpDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

OpSteppingAction::OpSteppingAction(OpRunAction* runAction)
: G4UserSteppingAction(),
  fRunAction(runAction)
{}

OpSteppingAction::~OpSteppingAction()
{}

void OpSteppingAction::UserSteppingAction(const G4Step* step)
{
	// information of each steps
	G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4double edep = step -> GetTotalEnergyDeposit();
	G4int prevNo = step -> GetPreStepPoint()  -> GetPhysicalVolume() -> GetCopyNo();
	G4int postNo = 0;
	if(stat == fWorldBoundary)
		return;
	else
		postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

	fRunAction -> FillStep(trackID, prevNo, postNo, pos, edep);
}
