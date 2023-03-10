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
	if(step->GetTrack()->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition() ||
		 step -> GetPreStepPoint() -> GetStepStatus() == fWorldBoundary ||
		 step -> GetPreStepPoint() -> GetStepStatus() != fGeomBoundary ||
		 OpParameterContainer::GetInstance()->GetParBool("MCStep")==false)
		return;
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4int trackPDG = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();
	G4int prevNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
	G4double prevKE = step -> GetPreStepPoint() -> GetKineticEnergy();
	G4int postNo = -1;
	G4double postKE = -1;
	postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
//	G4cout << step -> GetPostStepPoint() -> GetTouchable()->GetVolume() -> GetName() << G4endl;

//	postNo = step -> GetPostStepPoint() -> GetTouchable()->GetVolume() -> GetCopyNo();
//	G4double postKE = step -> GetPostStepPoint() -> GetKineticEnergy();
	const G4VProcess *process = step -> GetPostStepPoint() -> GetProcessDefinedStep();
//	G4int procID = -999;
//	G4String procName = "Blank";
	G4int procID = process -> GetProcessSubType();
	G4String procName = process -> GetProcessName();
	fRunAction -> SetProcess(procID,procName);
	G4String procTypeName = process -> GetProcessTypeName(process->GetProcessType());
	G4ThreeVector mom = step -> GetPreStepPoint() -> GetMomentum();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
	G4double fedep = step -> GetTotalEnergyDeposit();

	if(prevNo == 201 || postNo == 201)
	{
		if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 1)
		{
			G4cout << "##########" <<G4endl;
			G4cout << "OpSteppingAction::UserSteppingAction" << G4endl;
			G4cout << "TrackID(PDG): " << trackID << "(" << trackPDG << ")" << G4endl;
			G4cout << "ProcID: " << procID << G4endl;
			G4cout << "ProcName(Type): " << procName << "(" << procTypeName << ")" << G4endl;
			G4cout << "DetID: " <<prevNo << " -> " << postNo << G4endl;
			G4cout << "KE: " << prevKE << " -> " << postKE << G4endl;
			const G4TrackVector* secondary = step -> GetSecondary();
			auto NSecondaryInStep1 = secondary -> size();
			auto NSecondaryInStep2 = step -> GetNumberOfSecondariesInCurrentStep();
			G4cout << "NSecondaryInStep1: " << NSecondaryInStep1 << G4endl;
			G4cout << "NSecondaryInStep2: " << NSecondaryInStep2 << G4endl << G4endl;
		}
		fRunAction -> FillStep(true,trackID,procID,trackPDG,prevNo,prevNo,pos,fedep,prevKE);
	}
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
