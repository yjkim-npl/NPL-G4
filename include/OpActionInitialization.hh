#ifndef OpActionInitialization_h
#define OpActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class OpParameterContainer;

class OpActionInitialization : public G4VUserActionInitialization
{
	public:
		OpActionInitialization(OpParameterContainer* par);
		virtual ~OpActionInitialization();

		virtual void BuildForMaster() const;
		virtual void Build() const;
	private:
		OpParameterContainer* PC;
};

#endif
