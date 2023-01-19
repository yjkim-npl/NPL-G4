#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"

#include "ParameterContainer.hh"

ActionInitialization::ActionInitialization(ParameterContainer* par)
 : G4VUserActionInitialization()
{
	PC = par;
}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
  RunAction* runAction = new RunAction(PC);
  SetUserAction(runAction);
  
}

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction(PC));

  RunAction* runAction = new RunAction(PC);
  SetUserAction(runAction);
  
  EventAction* eventAction = new EventAction(runAction);
  SetUserAction(eventAction);
  
  SetUserAction(new TrackingAction(runAction));
  SetUserAction(new SteppingAction(runAction));
}  
