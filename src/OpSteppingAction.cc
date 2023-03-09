#include "OpDetectorConstruction.hh"
#include "OpEventAction.hh"
#include "OpParameterContainer.hh"
#include "OpRunAction.hh"
#include "OpSteppingAction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4SystemOfUnits.hh"

OpSteppingAction::OpSteppingAction(OpRunAction* runAction)
: G4UserSteppingAction(),
  fRunAction(runAction)
{
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpSteppingAction" << G4endl;
}

OpSteppingAction::~OpSteppingAction()
{
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpSteppingAction" << G4endl;
}

void OpSteppingAction::UserSteppingAction(const G4Step* step)
{
	// information of each steps
	if(step->GetTrack()->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition())
		return;
	if(step -> GetPreStepPoint() -> GetStepStatus() == fWorldBoundary)
		return;
//	G4int trkID = step -> GetTrack() -> GetTrackID();
//	G4int trkPDG = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();
//	G4double fedep = step -> GetTotalEnergyDeposit();
//	G4int prevNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
//	G4int postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
//	if(trkID == 1 &&  (prevNo == 201 || postNo == 201))
//	{
//		G4cout << "SteppingAction::edep " << trkPDG << " " << fedep << 
//			" " << prevNo <<
//			" " << postNo << G4endl;
//	}
}
