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
	const G4String det[] = {"SC1","SC2","SC3","BDC","BTOF"};
	const G4int ndet = sizeof(det)/sizeof(det[0]);
//	const G4int ndet = 2;
	for(G4int a=0; a<ndet-1; a++)
	{
		// case of SC2
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
	// case of BTOF1
	if(fabs(pos.x()) <= 0.5*PC->GetParDouble("BTOF_sizeX") &&
		 fabs(pos.y()) <= 0.5*PC->GetParDouble("BTOF_sizeY"))
	{
		if(pos.z()>=PC->GetParDouble("BTOF_ZOffset1")-trans-0.5*PC->GetParDouble("BTOF_sizeZ") &&
			 pos.z()<=PC->GetParDouble("BTOF_ZOffset1")-trans+0.5*PC->GetParDouble("BTOF_sizeZ"))
		{
			postNo = PC->GetParInt("BTOFID")+1;
		}
		if(pos.z()>=PC->GetParDouble("BTOF_ZOffset2")-trans-0.5*PC->GetParDouble("BTOF_sizeZ") &&
			 pos.z()<=PC->GetParDouble("BTOF_ZOffset2")-trans+0.5*PC->GetParDouble("BTOF_sizeZ"))
		{
			postNo = PC->GetParInt("BTOFID")+2;
		}
	}
//	G4cout <<
//		"x: " << pos.x() << " " <<
//		"y: " << pos.y() << " " <<
//		"z: " << pos.z() << " " <<
//		"postNo: " << postNo << G4endl;
	return postNo;
}

void OpSteppingAction::UserSteppingAction(const G4Step* step)
{
	// information of GeomBoundary(not o.p)
	if(step->GetTrack()->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition() ||
	   step -> GetPostStepPoint() -> GetStepStatus() == fWorldBoundary || 
		 PC->GetParBool("MCStep")==false)
		return;

	G4bool prev_boundary = 0;
	G4bool post_boundary = 0;
	if(step -> GetPreStepPoint() -> GetStepStatus() == fGeomBoundary)
		prev_boundary = 1;
	if(step -> GetPostStepPoint() -> GetStepStatus() == fGeomBoundary)
	{
		post_boundary = 1;
//		if(step -> GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo() ==
//			 CalculatePostNo(step->GetPostStepPoint()->GetPosition()))
//		{
//			return;
//		}
	}
//	if(step -> GetPreStepPoint() -> GetStepStatus() != fGeomBoundary)
//		return;
//	if(step -> GetPostStepPoint() -> GetStepStatus() != fGeomBoundary)
//		return;
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
//	G4double postKE = -1;
//	if(prevNo != postNo && boundary > 0)
	if(PC->GetParInt("UserVerbosity")>0 && false)
		//&& (postNo == 501 || prevNo == 501) && false)
	{
		G4cout << "prevB: " << prev_boundary << " postB: " << post_boundary << G4endl; 
		G4cout << "prevNo: "<<step -> GetPreStepPoint() ->GetPhysicalVolume() -> GetCopyNo() << G4endl;
		G4cout << "postNo: "<<postNo << G4endl << G4endl;
//		G4cout << "postNo: "<<step-> GetPostStepPoint() ->GetPhysicalVolume() -> GetCopyNo() << G4endl;
	}

	const G4VProcess *process = step -> GetPostStepPoint() -> GetProcessDefinedStep();
	G4int procID = process -> GetProcessSubType();
	G4String procName = process -> GetProcessName();
	fRunAction -> SetProcess(procID,procName);
	G4String procTypeName = process -> GetProcessTypeName(process->GetProcessType());

//	if(boundary)// &&prevNo == 201 || postNo == 201)
//	if(prevNo == 201 || postNo == 201)
	if(true)
	{
		if(PC -> GetParInt("UserVerbosity") > 1)
		{
			G4cout << "##########" <<G4endl;
			G4cout << "OpSteppingAction::UserSteppingAction" << G4endl;
			G4cout << "PrevBoundary : " << prev_boundary << " PostBoundary: " << post_boundary << G4endl;
			G4cout << "TrackID(PDG): " << trackID << "(" << trackPDG << ")" << G4endl;
			G4cout << "ProcID: " << procID << G4endl;
			G4cout << "ProcName(Type): " << procName << "(" << procTypeName << ")" << G4endl;
			G4cout << "DetID: " <<prevNo << " -> " << postNo << G4endl;
			G4cout << "KE: " << prevKE << " -> " << postKE << G4endl;
			G4cout << "Edep: " << fedep << G4endl;
			G4cout << "posZ: " << pos.z() << " -> " << post_pos.z() << G4endl;
			G4cout << "StepLength: " << step -> GetStepLength() << G4endl;
			const G4TrackVector* secondary = step -> GetSecondary();
			auto NSecondaryInStep1 = secondary -> size();
			auto NSecondaryInStep2 = step -> GetNumberOfSecondariesInCurrentStep();
			G4cout << "NSecondaryInStep1: " << NSecondaryInStep1 << G4endl;
			G4cout << "NSecondaryInStep2: " << NSecondaryInStep2 << G4endl << G4endl;
		}
		fRunAction -> FillStep
			(prevNo==postNo?0:1,0,trackID,procID,trackPDG,prevNo,postNo,pos,fedep,prevKE);
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
