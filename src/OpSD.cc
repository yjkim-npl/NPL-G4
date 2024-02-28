#include "OpHit.hh"
#include "OpSD.hh"

#include "OpParameterContainer.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4VProcess.hh"
#include "G4ParticleTypes.hh"
#include "G4SDManager.hh"
#include "G4TrackVector.hh"

OpSD::OpSD(const G4String &name, const G4String &HCname)
	:G4VSensitiveDetector(name),
	fHitsCollection(0),
	fHCID(-1)
{
	collectionName.insert(HCname);
}

OpSD::~OpSD()
{}

void OpSD::Initialize(G4HCofThisEvent* HCE)
{
	fHitsCollection = new OpHitsCollection(SensitiveDetectorName, collectionName[0]);
	if(fHCID<0)
	{
		fHCID = G4SDManager::GetSDMpointer() -> GetCollectionID(fHitsCollection);
	}
	HCE -> AddHitsCollection(fHCID,fHitsCollection);
}

G4bool OpSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
	if(step->GetTrack()->GetDefinition()!=G4OpticalPhoton::OpticalPhotonDefinition() && (
		 step->GetPreStepPoint()->GetStepStatus() == fWorldBoundary))
	{
		return true;
	}
	G4int boundary = 0;
	if(step->GetPreStepPoint() -> GetStepStatus() == fGeomBoundary)
	{
		boundary = 1;
	}
	if(step->GetPostStepPoint()->GetStepStatus()==fGeomBoundary)
	{
		boundary = 2;
	}
	
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4int trackPDG = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();
	G4int prevNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
	G4double prevKE = step -> GetPreStepPoint() -> GetKineticEnergy();
	G4int postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
	G4double postKE = step -> GetPostStepPoint() -> GetKineticEnergy();
	G4ThreeVector mom = step -> GetPreStepPoint() -> GetMomentum();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4ThreeVector post_pos = step -> GetPostStepPoint() -> GetPosition();
	G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
	G4double fedep = step -> GetTotalEnergyDeposit();
	G4double length = step -> GetStepLength();
	G4int procID = -10;
	G4String procName = "";
	G4int NSecondaryInStep = 0;

	if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
	{
		const G4TrackVector* secondary = step -> GetSecondary();
		NSecondaryInStep = step -> GetSecondaryInCurrentStep() -> size();
		const G4VProcess* process = step -> GetPostStepPoint() -> GetProcessDefinedStep();
		procID = process -> GetProcessSubType();
		procName = process -> GetProcessName();
		G4String procTypeName = process -> GetProcessTypeName(process->GetProcessType());
	}
	OpHit* hit = NULL;
	G4int NofHits = fHitsCollection -> entries();
	for(G4int a=0; a<NofHits; a++)
	{
		// for boundary interaction of optical photon
		if((*fHitsCollection)[a] -> GetTrackID() == trackID &&
			 (*fHitsCollection)[a] -> GetTrackPDG() == -22 &&
			 (*fHitsCollection)[a] -> GetDetID() == prevNo)
		{
			hit = (*fHitsCollection)[a];
			break;
		}
		// for other particle hit
		// save hit with detector
		else if((*fHitsCollection)[a] -> GetTrackID() == trackID &&
			      (*fHitsCollection)[a] -> GetDetID() == prevNo)
		{
			hit = (*fHitsCollection)[a];
			break;
		}
	}
	if(hit == NULL)
	{
		hit = new OpHit();
		hit -> AddHit(trackID,trackPDG,prevNo);
		fHitsCollection -> insert(hit);
	}
	// add hit of other particles 
	if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() &&
		 OpParameterContainer::GetInstance() -> GetParBool("MCStepHit"))
	{
		hit -> CountStep();
		hit -> AddMomentum(mom);
		hit -> AddProcess(procID, procName);
		hit -> AddPosition(pos);
		hit -> AddTime(time);
		hit -> AddEdep(fedep);
		hit -> AddBoundary(boundary);
		hit -> AddKE(prevKE);
		hit -> AddStepLength(length);
		hit -> AddNSecondaryOP(NSecondaryInStep);
		if(boundary)
			hit -> AddPostDetID(postNo);
	}
	// add hit of optical photon
	else if(step->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
		OpParameterContainer::GetInstance() -> GetParBool("OpticalPhysics") &&
		OpParameterContainer::GetInstance() -> GetParBool("OpBoundary"))
	{
		// Boundary process & step information
		G4OpBoundaryProcessStatus status = Undefined;
		static G4ThreadLocal G4ProcessManager* OpManager = 
			G4OpticalPhoton::OpticalPhoton() -> GetProcessManager();
		if(OpManager)
		{
			G4int nproc = OpManager -> GetPostStepProcessVector() -> entries();
			G4ProcessVector* fPostStepDoItVector = 
				OpManager -> GetPostStepProcessVector(typeDoIt);
			for(G4int a=0; a<nproc; a++)
			{
				G4VProcess* fProcess = (*fPostStepDoItVector)[a];
				fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fProcess);
				if(fOpProcess)
				{
					status = fOpProcess -> GetStatus();
					break;
				}
			}
			if       (status == 2){
				procID = -2;
				procName = "Transmission";
			}else if (status == 3){
				procID = -3;
				procName = "FresnelReflection";
			}else if (status == 4){
				procID = -4;
				procName = "TotalInternalReflection";
			}else if (status == 13){
				procID = -13;
				procName = "StepTooSmall";
			}else if (status == 14){
				procID = -14;
				procName = "NoRINDEX";
			}else{
				procID = 0;
				procName = "OtherProcesses";
			}
			hit -> CountStep();
			hit -> AddProcess(procID,procName);
			hit -> AddMomentum(mom);
			hit -> AddPosition(pos);
			hit -> AddTime(time);
			hit -> AddEdep(fedep);
		}
	}
	return true;
}
