#include "OpEventAction.hh"
#include "OpRunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

OpEventAction::OpEventAction(OpRunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction)
{
} 

OpEventAction::~OpEventAction()
{}

void OpEventAction::BeginOfEventAction(const G4Event*)
{
	fRunAction -> clear_data();
}

void OpEventAction::EndOfEventAction(const G4Event*)
{  
	fRunAction -> update_Tree();
}

