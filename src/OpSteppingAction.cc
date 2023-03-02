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
	G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();

	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4ThreeVector posp = step -> GetPostStepPoint() -> GetPosition();
	G4ThreeVector mom = step -> GetPreStepPoint() -> GetMomentum();
	G4double edep = step -> GetTotalEnergyDeposit();
	G4int prevNo = step -> GetPreStepPoint()  -> GetPhysicalVolume() -> GetCopyNo();
	G4int postNo = 0;

	if(stat == fWorldBoundary)
		return;
	else
		postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

	if(OpParameterContainer::GetInstance() -> GetParBool("MCStep"))
		fRunAction -> FillStep(trackID, prevNo, postNo, pos, edep);

	// boundary process status
	if(step -> GetTrack() -> GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()
			&& stat != fWorldBoundary)
	{
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
			G4int procID = -10;
			G4String processName;
			if(theStatus == 2)
//			if(theStatus == Transmission)
			{
				procID = -2;
				processName = "Transmission";
//				if(stat != fWorldBoundary)
//				else
//					G4cout << "step is on world boundary" << G4endl;
//				G4cout << "OpBoundary: " << theStatus<< G4endl;
			}else if(theStatus == 3){
				procID = -3;
				processName = "FresnelReflection";
			}else if(theStatus == 4){
				procID = -4;
				processName = "TotalInternalReflection";
			}else if(theStatus == 13){
				procID = -13;
				processName = "StepTooSmall";
//				G4cout << trackID << " " << step->GetPreStepPoint()->GetTotalEnergy() << " " << pos.x() <<
//					" " << posp.x() << " " << pos.y() << " " << posp.y() << " " << pos.z() << " " << posp.z() << G4endl;
			}else if(theStatus == 14){
				procID = -14;
				processName = "NoRINDEX";
			}else{
				procID = 0;
				processName = "OtherProcesses";
			}
			if(stat != fWorldBoundary)// && procID != -4 && procID != -13)
				fRunAction -> FillOpticalPhotonBoundary(trackID,procID,mom,pos);
//			if(step->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
//					stat == fGeomBoundary)
//				G4cout << step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetName() << " " <<	
//					step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetName() << G4endl;
//			G4cout << procID << " " << processName << G4endl;
			fRunAction -> SetProcess(procID,processName);
			OpEventAction::nOPsteps++;
//	  	if(OpEventAction::nOPsteps%2 == 0)
//			if(theStatus !=0 &&
//					prevNo != step->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo())
//			{
//				G4cout << "OpBoundary: " << theStatus << G4endl;
//				G4int tempPostNo = step->GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
//				G4cout << "prevNo: " << prevNo << " postNo:" << tempPostNo << G4endl << G4endl;
//			}
		// 2 : Transmission
		// 3 : Fresnel Refraction
		// 4 : Total Internal Reflection
		// 13: StepTooSmall
		// 14: NoRINDEX (vacuum)
		}
	}
}
