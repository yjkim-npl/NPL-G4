#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "RunAction.hh"

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"

class TrackingAction : public G4UserTrackingAction
{
	public:
		TrackingAction(RunAction*);
		virtual ~TrackingAction();

		virtual void PreUserTrackingAction(const G4Track* track);
		virtual void PostUserTrackingAction(const G4Track* track);

	private:
		RunAction* fRunAction;
};
#endif
