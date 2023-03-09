#ifndef OpSteppingAction_h
#define OpSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class OpRunAction;

class G4LogicalVolume;
class G4OpBoundaryProcess;

class OpSteppingAction : public G4UserSteppingAction
{
  public:
    OpSteppingAction(OpRunAction* );
    virtual ~OpSteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:
		OpRunAction* fRunAction;
		G4OpBoundaryProcess* fOpProcess;
};

#endif
