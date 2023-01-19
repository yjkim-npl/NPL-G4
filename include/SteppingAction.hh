#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class RunAction;

class G4LogicalVolume;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(RunAction* );
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:
	RunAction* fRunAction;

};

#endif
