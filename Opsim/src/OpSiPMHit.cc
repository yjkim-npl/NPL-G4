#include "OpSiPMHit.hh"

G4ThreadLocal G4Allocator<OpSiPMHit>* OpSiPMHitAllocator;

OpSiPMHit::OpSiPMHit()
	:G4VHit(),
	fPhotons(0),
	fDetID(0)
{
}

OpSiPMHit::OpSiPMHit(const OpSiPMHit &right)
	:G4VHit()
{
	fPhotons = right.fPhotons;
	fDetID = right.fDetID;
	vec_vx = right.vec_vx;
	vec_vy = right.vec_vy;
	vec_vz = right.vec_vz;
	vec_px = right.vec_px;
	vec_py = right.vec_py;
	vec_pz = right.vec_pz;
	vec_t = right.vec_t;
	vec_E = right.vec_E;
}

OpSiPMHit::~OpSiPMHit()
{}

const OpSiPMHit& OpSiPMHit::operator=(const OpSiPMHit &right)
{
	fPhotons = right.fPhotons;
	fDetID = right.fDetID;
	vec_vx = right.vec_vx;
	vec_vy = right.vec_vy;
	vec_vz = right.vec_vz;
	vec_px = right.vec_px;
	vec_py = right.vec_py;
	vec_pz = right.vec_pz;
	vec_t = right.vec_t;
	vec_E = right.vec_E;
	return *this;
}
