#include "OpHit.hh"

G4ThreadLocal G4Allocator<OpHit>* OpHitAllocator;

OpHit::OpHit()
	:G4VHit(),
	nSteps(0),
	trkID(0),
	trkPDG(0),
	detID(0),
	post_DetID(0),
	EdepSum(0)
{
}

OpHit::OpHit(const OpHit &right)
	:G4VHit()
{
	nSteps = right.nSteps;
	trkID = right.trkID;
	trkPDG = right.trkPDG;
	detID = right.detID;
	post_DetID = right.post_DetID;
	EdepSum = right.EdepSum;
	vec_IsBoundary = right.vec_IsBoundary;
	vec_prevKE = right.vec_prevKE;
	vec_Edep = right.vec_Edep;
	vec_procID = right.vec_procID;
	vec_procName = right.vec_procName;
	vec_p = right.vec_p;
	vec_v = right.vec_v;
	vec_t = right.vec_t;
//	pair_Eif = right.pair_Eif;
}

OpHit::~OpHit()
{}

const OpHit& OpHit::operator=(const OpHit &right)
{
	nSteps = right.nSteps;
	trkID = right.trkID;
	trkPDG = right.trkPDG;
	detID = right.detID;
	post_DetID = right.post_DetID;
	EdepSum = right.EdepSum;
	vec_IsBoundary = right.vec_IsBoundary;
	vec_prevKE = right.vec_prevKE;
	vec_Edep = right.vec_Edep;
	vec_procID = right.vec_procID;
	vec_procName = right.vec_procName;
	vec_p = right.vec_p;
	vec_v = right.vec_v;
	vec_t = right.vec_t;
//	pair_Eif = right.pair_Eif;
	return *this;
}

void OpHit::AddHit(G4int trackID, G4int trackPDG, G4int detectorID)
{
	trkID = trackID;
	trkPDG = trackPDG;
	detID = detectorID;
}
//
//void OpHit::AddStep(G4int procID, G4String procName, G4ThreeVector p, G4ThreeVector v, G4double t, G4double edep)
//{
//	vec_Edep.push_back(edep);
//	EdepSum += edep;
//	vec_procID.push_back(procID);
//	vec_procName.push_back(procName);
//	vec_p.push_back(p);
//	vec_v.push_back(v);
//	vec_t.push_back(t);
//	nSteps++;
//}
