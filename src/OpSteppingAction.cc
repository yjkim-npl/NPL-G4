#include "OpSteppingAction.hh"
#include "OpEventAction.hh"
#include "OpRunAction.hh"
#include "OpDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"

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

	// boundary process status
	G4OpBoundaryProcessStatus theStatus = Undefined;
	static G4ThreadLocal G4ProcessManager* OpManager = 
		G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
	if(OpManager)
	{
		G4int nproc = OpManager->GetPostStepProcessVector()->entries();
		G4ProcessVector* fPostStepDoItVector = 
			OpManager -> GetPostStepProcessVector(typeDoIt);

		for(G4int a=0; a<nproc; a++)
		{
			G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[a];
			fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
			if(fOpProcess)
			{
				theStatus = fOpProcess->GetStatus();
				break;
			}
		}
		OpEventAction::nOPsteps++;
//  	if(OpEventAction::nOPsteps%2 == 0)
//		if(theStatus !=0 &&
//				prevNo != step->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo())
//		{
//			G4cout << "OpBoundary: " << theStatus << G4endl;
//			G4int tempPostNo = step->GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
//			G4cout << "prevNo: " << prevNo << " postNo:" << tempPostNo << G4endl << G4endl;
//		}
	// 2 : Transmission
	// 3 : Fresnel Refraction
	// 4 : Total Internal Reflection
	// 13: StepTooSmall
	// 14: NoRINDEX (vacuum)
	}


	if(step->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
			stat == fGeomBoundary)
	{
//		G4cout << "Optical boundary process was shown " << G4endl;
	}

	// 
	if(stat == fWorldBoundary)
		return;
	else
		postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

	fRunAction -> FillStep(trackID, prevNo, postNo, pos, edep);
}
