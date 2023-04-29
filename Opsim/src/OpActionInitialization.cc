#include "OpActionInitialization.hh"
#include "OpPrimaryGeneratorAction.hh"
#include "OpRunAction.hh"
#include "OpEventAction.hh"
#include "OpSteppingAction.hh"
#include "OpTrackingAction.hh"

#include "OpParameterContainer.hh"

OpActionInitialization::OpActionInitialization()
 : G4VUserActionInitialization()
{
	PC = OpParameterContainer::GetInstance();
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpActionInitialization" << G4endl;
}

OpActionInitialization::~OpActionInitialization()
{
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpActionInitialization" << G4endl;
}

void OpActionInitialization::BuildForMaster() const
{
  OpRunAction* runAction = new OpRunAction();
  SetUserAction(runAction);
  
}

void OpActionInitialization::Build() const
{
  SetUserAction(new OpPrimaryGeneratorAction());

  OpRunAction* runAction = new OpRunAction();
  SetUserAction(runAction);
  
  OpEventAction* eventAction = new OpEventAction(runAction);
  SetUserAction(eventAction);
  
  SetUserAction(new OpTrackingAction(runAction));
  SetUserAction(new OpSteppingAction(runAction));
}  
