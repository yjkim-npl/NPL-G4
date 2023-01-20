#include "OpActionInitialization.hh"
#include "OpPrimaryGeneratorAction.hh"
#include "OpRunAction.hh"
#include "OpEventAction.hh"
#include "OpSteppingAction.hh"
#include "OpTrackingAction.hh"

#include "OpParameterContainer.hh"

OpActionInitialization::OpActionInitialization(OpParameterContainer* par)
 : G4VUserActionInitialization()
{
	PC = par;
}

OpActionInitialization::~OpActionInitialization()
{}

void OpActionInitialization::BuildForMaster() const
{
  OpRunAction* runAction = new OpRunAction(PC);
  SetUserAction(runAction);
  
}

void OpActionInitialization::Build() const
{
  SetUserAction(new OpPrimaryGeneratorAction(PC));

  OpRunAction* runAction = new OpRunAction(PC);
  SetUserAction(runAction);
  
  OpEventAction* eventAction = new OpEventAction(runAction);
  SetUserAction(eventAction);
  
  SetUserAction(new OpTrackingAction(runAction));
  SetUserAction(new OpSteppingAction(runAction));
}  
