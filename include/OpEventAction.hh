#ifndef OpEventAction_h
#define OpEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <vector>

using namespace std;

class OpRunAction;

class OpEventAction : public G4UserEventAction
{
	public:
		OpEventAction(OpRunAction* runAction);
		virtual ~OpEventAction();

		static G4int nOPsteps;

		virtual void BeginOfEventAction(const G4Event* event);
		virtual void EndOfEventAction(const G4Event* event);

	private:
		OpRunAction* fRunAction;
};
#endif
