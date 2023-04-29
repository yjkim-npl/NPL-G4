#ifndef OpSteppingAction_h
#define OpSteppingAction_h 1

#include "OpParameterContainer.hh"

#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"
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
		OpParameterContainer* PC;

		G4int CalculatePostNo(G4ThreeVector pos);
		G4OpBoundaryProcess* fOpProcess;
};

#endif
