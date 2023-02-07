#include "OpEventAction.hh"
#include "OpParameterContainer.hh"
#include "OpRunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

G4int OpEventAction::nOPsteps = 0;

OpEventAction::OpEventAction(OpRunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction)
{
	G4cout << "Constructor of OpEventAction" << G4endl;
} 

OpEventAction::~OpEventAction()
{
	OpEventAction::nOPsteps = 0;
	G4cout << "Destructor of OpEventAction" << G4endl;
}

void OpEventAction::BeginOfEventAction(const G4Event*)
{
	fRunAction -> clear_data();
}

void OpEventAction::EndOfEventAction(const G4Event*)
{
	fRunAction -> update_Tree();
}

