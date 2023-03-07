#include "OpSCHit.hh"
#include "OpSCSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

OpSCSD::OpSCSD(const G4String &name, const G4String &HCname)
	:G4VSensitiveDetector(name),
	fHitsCollection(0),
	fHCID(-1)
{
	collectionName.insert(HCname);
}

OpSCSD::~OpSCSD()
{}

void OpSCSD::Initialize(G4HCofThisEvent* HCE)
{
	fHitsCollection = new OpSCHitsCollection(SensitiveDetectorName,collectionName[0]);
	if(fHCID<0)
	{
		fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
	}
	HCE -> AddHitsCollection(fHCID,fHitsCollection);
}

G4bool OpSCSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4int trackPDG = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();
	G4int prevNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
	G4double fedep = step -> GetTotalEnergyDeposit();
	if(fedep==0.) return true;

//	OpSCHit* hit = (*fHitsCollection)[prevNo];
	OpSCHit* hit = NULL;
	G4int NofHits = fHitsCollection -> entries();
	for(G4int a=0; a<NofHits; a++)
	{
		if((*fHitsCollection)[a] -> GetTrackID()==trackID &&
			(*fHitsCollection)[a] -> GetDetID() == prevNo)
		{
			hit = (*fHitsCollection)[a];
			break;
		}
	}
	if(hit == NULL)
	{
		hit = new OpSCHit();
		hit -> AddHit(trackID,trackPDG,prevNo);
		fHitsCollection->insert(hit);
	}
	hit -> AddEdep(fedep);
	return true;
}
