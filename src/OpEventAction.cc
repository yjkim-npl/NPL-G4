#include "OpEventAction.hh"
#include "OpRunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

G4int OpEventAction::nOPsteps = 0;

OpEventAction::OpEventAction(OpRunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction)
{
} 

OpEventAction::~OpEventAction()
{
	OpEventAction::nOPsteps = 0;
}

void OpEventAction::BeginOfEventAction(const G4Event*)
{
	fRunAction -> clear_data();
}

void OpEventAction::EndOfEventAction(const G4Event*)
{  
	fRunAction -> update_Tree();
	fRunAction -> Print_track();
}

