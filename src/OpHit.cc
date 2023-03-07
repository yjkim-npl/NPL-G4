#include "OpHit.hh"

G4ThreadLocal G4Allocator<OpHit>* OpHitAllocator;

OpHit::OpHit()
	:G4VHit(),
	trkID(0),
	detID(0),
	size_vec(0)
{
}

OpHit::OpHit(const OpHit &right)
	:G4VHit()
{
	trkID = right.trkID;
	detID = right.detID;
	size_vec = right.size_vec;
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
	trkID = right.trkID;
	detID = right.detID;
	size_vec = right.size_vec;
	vec_procID = right.vec_procID;
	vec_procName = right.vec_procName;
	vec_p = right.vec_p;
	vec_v = right.vec_v;
	vec_t = right.vec_t;
//	pair_Eif = right.pair_Eif;
	return *this;
}

void OpHit::AddHit(G4int trackID, G4int detectorID)
{
	trkID = trackID;
	detID = detectorID;
}

void OpHit::AddStep(G4int procID, G4String procName, G4ThreeVector p, G4ThreeVector v, G4double t)
{
//	G4cout << t << G4endl;
	vec_procID.push_back(procID);
	vec_procName.push_back(procName);
	vec_p.push_back(p);
	vec_v.push_back(v);
	vec_t.push_back(t);
	size_vec++;
}
