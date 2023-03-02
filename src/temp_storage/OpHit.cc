#include "OpHit.hh"

G4ThreadLocal G4Allocator<OpHit>* OpHitAllocator;

OpHit::OpHit()
:G4VHit(),
	fPhotons(0)
{
}

OpHit::OpHit(const OpHit &right)
:G4VHit()
{
	fPhotons = right.fPhotons;
}

OpHit::~OpHit()
{}

const OpHit& OpHit::operator=(const OpHit &right)
{
	fPhotons = right.fPhotons;
	return *this;
}

void OpHit::Print()
{
	G4cout << "OpHit::Print()" << G4endl;
}
