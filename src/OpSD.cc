#include "OpHit.hh"
#include "OpSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4SDManager.hh"

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
	if(step -> GetTrack() -> GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
		return true;
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4int prevNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

	OpHit* hit = NULL;
	G4int NofHits = fHitsCollection -> entries();
	for(G4int a=0; a<NofHits; a++)
	{
		if((*fHitsCollection)[a] -> GetTrackID() == trackID &&
			 (*fHitsCollection)[a] -> GetDetID() == prevNo)
		{
			hit = (*fHitsCollection)[a];
			break;
		}
	}
	if(hit == NULL)
	{
		hit = new OpHit();
		hit -> AddHit(trackID,prevNo);
		fHitsCollection -> insert(hit);
	}

	// Boundary process & step information
	G4ThreeVector mom = step -> GetPreStepPoint() -> GetMomentum();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
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
		G4int procID = -10;
		G4String procName;
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
		hit -> AddStep(procID,procName, mom, pos, time);
	}
	return true;
}
