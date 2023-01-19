#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class ParameterContainer;

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(ParameterContainer* par);
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
  private:
	ParameterContainer* PC;
};

#endif
