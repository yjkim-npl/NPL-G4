#ifndef OpEventAction_h
#define OpEventAction_h 1

#include "OpParameterContainer.hh"

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
		OpParameterContainer* PC;

		vector<G4int> vec_fID;
		vector<G4int> vec_fOpID;
		vector<G4String> vec_HCname;
		vector<G4String> vec_SDname;
		G4int fSC1ID;
		G4int fSC2ID;
		G4int fOpSC2ID;
		G4int fSC3ID;
		G4int fBTOFID;
		G4int fSiPMID;
};
#endif
