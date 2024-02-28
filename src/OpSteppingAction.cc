#include "OpDetectorConstruction.hh"
#include "OpEventAction.hh"
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
	PC = OpParameterContainer::GetInstance();
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpSteppingAction" << G4endl;
}

OpSteppingAction::~OpSteppingAction()
{
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpSteppingAction" << G4endl;
}

G4int OpSteppingAction::CalculatePostNo(G4ThreeVector pos)
{
	G4int postNo = 0;
	G4double trans = PC -> GetParDouble("Translation");
	const G4String det[] = {"SC"};
	const G4int ndet = sizeof(det)/sizeof(det[0]);
	for(G4int a=0; a<ndet; a++)
	{
		// case of SC
		if(fabs(pos.x()) <= 0.5*PC->GetParDouble(det[a]+"_sizeX") &&
			 fabs(pos.y()) <= 0.5*PC->GetParDouble(det[a]+"_sizeY") &&
			 pos.z() >= PC->GetParDouble(det[a]+"_ZOffset")-trans-0.5*PC->GetParDouble(det[a]+"_sizeZ") &&
			 pos.z() <=PC->GetParDouble(det[a]+"_ZOffset")-trans+0.5*PC->GetParDouble(det[a]+"_sizeZ"))
		{
			postNo = PC->GetParInt(det[a]+"ID")+1;
			if(PC->GetParBool(det[a]+"In") == false)
			{
				postNo = 0;
			}
			break;
		}
	}
	return postNo;
}

void OpSteppingAction::UserSteppingAction(const G4Step* step)
{
	// information of GeomBoundary(not o.p)
	if(step->GetTrack()->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition() ||
	   step -> GetPostStepPoint() -> GetStepStatus() == fWorldBoundary || 
		 PC->GetParBool("MCStep")==false)
		return;
	if(step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo() == PC->GetParInt("WorldID") &&
		 step->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() == PC->GetParInt("WorldID"))
		return;

	G4bool prev_boundary = 0;
	G4bool post_boundary = 0;
	if(step -> GetPreStepPoint() -> GetStepStatus() == fGeomBoundary)
		prev_boundary = 1;
	if(step -> GetPostStepPoint() -> GetStepStatus() == fGeomBoundary)
	{
		post_boundary = 1;
	}
	G4bool boundary = 0;
	if(prev_boundary!=0 || post_boundary!=0)
		boundary = 1;
	else
		boundary = 0;
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4int trackPDG = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();
	G4ThreeVector mom = step -> GetPreStepPoint() -> GetMomentum();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
	G4double fedep = step -> GetTotalEnergyDeposit();
	G4int prevNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
	G4double prevKE = step -> GetPreStepPoint() -> GetKineticEnergy();
	G4ThreeVector post_pos = step -> GetPostStepPoint() -> GetPosition();
	G4int postNo = CalculatePostNo(post_pos); // segmentation error while extract copyNo directly
	G4double postKE = step -> GetPostStepPoint() -> GetKineticEnergy();
	G4double length = step -> GetStepLength();

	// Secondary Tracking 
	G4int NSecondaryOP = 0;
	const vector<const G4Track*>* pvec_secondaries = step -> GetSecondaryInCurrentStep();
	for(G4int a=0; a<pvec_secondaries->size(); a++)
	{
		const G4Track* strack = pvec_secondaries->at(a);
		const G4int sTrkID = strack -> GetTrackID();
		const G4int sTrkPDG = strack -> GetDefinition() -> GetPDGEncoding();
		if(sTrkPDG != -22)
			continue;
		NSecondaryOP++;
	}

	const G4VProcess *process = step -> GetPostStepPoint() -> GetProcessDefinedStep();
	G4int procID = process -> GetProcessSubType();
	G4String procName = process -> GetProcessName();
	fRunAction -> SetProcess(procID,procName);
	G4String procTypeName = process -> GetProcessTypeName(process->GetProcessType());

	fRunAction -> FillStep
		(prevNo==postNo?0:1,0,trackID,procID,trackPDG,prevNo,postNo,pos,
		 time,fedep,length,NSecondaryOP,prevKE);
}
