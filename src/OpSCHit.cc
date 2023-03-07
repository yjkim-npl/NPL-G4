#include "OpSCHit.hh"

G4ThreadLocal G4Allocator<OpSCHit>* OpSCHitAllocator;

OpSCHit::OpSCHit()
	:G4VHit(),
	trkID(0),
	trkPDG(0),
	detID(0),
	edep(0)
{}

OpSCHit::OpSCHit(const OpSCHit &right)
	:G4VHit()
{
	trkID = right.trkID;
	trkPDG = right.trkPDG;
	detID = right.detID;
	edep = right.edep;
}

OpSCHit::~OpSCHit()
{}

const OpSCHit& OpSCHit::operator=(const OpSCHit &right)
{
	trkID = right.trkID;
	trkPDG = right.trkPDG;
	detID = right.detID;
	edep = right.edep;
	return *this;
}

void OpSCHit::AddHit(G4int trackID, G4int trackPDG, G4int detectorID)
{
	trkID = trackID;
	trkPDG = trackPDG;
	detID = detectorID;
}

void OpSCHit::AddEdep(G4double fedep)
{
	edep += fedep;
}
