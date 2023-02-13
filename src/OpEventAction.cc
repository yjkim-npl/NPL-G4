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
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpEventAction" << G4endl;
} 

OpEventAction::~OpEventAction()
{
	OpEventAction::nOPsteps = 0;
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpEventAction" << G4endl;
}

void OpEventAction::BeginOfEventAction(const G4Event* event)
{
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		if(event -> GetEventID() == 0)
			G4cout << "Begin of OpEventAction" << G4endl;
	fRunAction -> clear_data();
}

void OpEventAction::EndOfEventAction(const G4Event* event)
{
	fRunAction -> update_Tree();
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 0)
		if(event -> GetEventID() == 0)
			G4cout << "End of OpEventAction" << G4endl;
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 1)
		if(event -> GetEventID() == 0)
			fRunAction -> PrintData(OpticalPhoton);
	if(OpParameterContainer::GetInstance() -> GetParInt("UserVerbosity") > 2)
		fRunAction -> PrintData(OpticalPhoton);
}

