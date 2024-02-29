#include "OpSiPMHit.hh"
#include "OpSiPMSD.hh"
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
#include "G4ThreeVector.hh"

OpSiPMSD::OpSiPMSD(const G4String &name, const G4String &HCname)
	:G4VSensitiveDetector(name),
	fHitsCollection(0),
	fHCID(-1)
{
	collectionName.insert(HCname);
}

OpSiPMSD::~OpSiPMSD()
{}

void OpSiPMSD::Initialize(G4HCofThisEvent* HCE)
{
	fHitsCollection = new OpSiPMHitsCollection(SensitiveDetectorName, collectionName[0]);
	if(fHCID<0)
	{
		fHCID = G4SDManager::GetSDMpointer() -> GetCollectionID(fHitsCollection);
	}
	HCE -> AddHitsCollection(fHCID,fHitsCollection);
}

G4bool OpSiPMSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
	if(step->GetTrack()->GetDefinition()!=G4OpticalPhoton::OpticalPhotonDefinition())
	{
		return false;
	}
	G4ThreeVector pos = step -> GetPostStepPoint() -> GetPosition();
	G4int SCNo = OpParameterContainer::GetInstance()->GetParInt("SC2ID");
	G4double widthX;
	G4double widthY;

	G4int prevNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
	G4double prevKE = step -> GetPreStepPoint() -> GetKineticEnergy();
	G4ThreeVector mom = step -> GetPreStepPoint() -> GetMomentum();
	G4double time = step -> GetTrack() -> GetGlobalTime();
	G4int procID = -10;
	G4String procName = "";

	OpSiPMHit* hit = NULL;
	G4int NofHits = fHitsCollection -> entries();
	G4bool SaveTrackSum = OpParameterContainer::GetInstance() -> GetParBool("SaveTrackSum");
	for(G4int a=0; a<NofHits; a++)
	{
		// for optical photon hit
		if((*fHitsCollection)[a] -> GetDetectorID() == prevNo)
		{
			hit = (*fHitsCollection)[a];
			break;
		}
	}
	if(hit == NULL)
	{
		hit = new OpSiPMHit();
		hit -> AddHit(prevNo);
		fHitsCollection -> insert(hit);
	}
	// new hit of optical photon
	if(step->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
		OpParameterContainer::GetInstance() -> GetParBool("OpticalPhysics") &&
		OpParameterContainer::GetInstance() -> GetParBool("OpSiPM") &&
		OpParameterContainer::GetInstance() -> GetParBool("SiPMIn"))
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
			hit -> photonCount();
			hit -> SetPosition(pos);
			hit -> SetMomentum(mom);
			hit -> SetTime(time);
			hit -> SetEnergy(prevKE);
			hit -> SetProcess(procID,procName);
		}
	}
	return true;
}
