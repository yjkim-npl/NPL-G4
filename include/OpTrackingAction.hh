#ifndef OpTrackingAction_h
#define OpTrackingAction_h 1

#include "OpRunAction.hh"

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"

class OpTrackingAction : public G4UserTrackingAction
{
	public:
		OpTrackingAction(OpRunAction*);
		virtual ~OpTrackingAction();

		virtual void PreUserTrackingAction(const G4Track* track);
		virtual void PostUserTrackingAction(const G4Track* track);

	private:
		OpRunAction* fRunAction;
};
#endif
