#include "OpEventAction.hh"
#include "OpRunAction.hh"
#include "OpSCHit.hh"
#include "OpHit.hh"

#include "G4GenericAnalysisManager.hh"
#include "G4Event.hh"
#include "G4VHitsCollection.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4VHitsCollection.hh"

G4int OpEventAction::nOPsteps = 0;

OpEventAction::OpEventAction(OpRunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction)
{
	PC = OpParameterContainer::GetInstance();
	vec_fID = {-1,-1,-1,-1};
	vec_fOpID = {-1,-1,-1,-1};
	vec_HCname = {"SC1C","SC2C","SC3C","BTOFC"};
	vec_SDname = {"SC1","SC2","SC3","BTOF"};
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpEventAction" << G4endl;
} 

OpEventAction::~OpEventAction()
{
	OpEventAction::nOPsteps = 0;
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpEventAction" << G4endl;
}

void OpEventAction::BeginOfEventAction(const G4Event* event)
{
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if(PC -> GetParBool("MCStep"))
	{
		for(G4int a=0; a<vec_fID.size(); a++)
		{
			if(vec_fID[a] == -1 && PC ->GetParBool(vec_SDname[a]+"In"))
			{
				vec_fID[a] = SDman -> GetCollectionID(vec_HCname[a]);
//				G4cout << vec_fID[a] << G4endl;
			}
		}
//		if(fSC1ID == -1 && PC->GetParBool("SC1In"))
//			 fSC1ID = SDman -> GetCollectionID("SC1C");
//		if(fSC2ID == -1 && PC->GetParBool("SC2In"))
//			 fSC2ID = SDman -> GetCollectionID("SC2C");
//		if(fSC3ID == -1 && PC->GetParBool("SC3In"))
//			 fSC3ID = SDman -> GetCollectionID("SC3C");
//		if(fBTOFID == -1 && PC->GetParBool("BTOFIn"))
//			 fBTOFID = SDman -> GetCollectionID("BTOFC");
	}
	if(PC -> GetParBool("OpTrack"))
	{
		for(G4int a=0; a<vec_fOpID.size(); a++)
		{
			if(vec_fOpID[a] == -1 && PC -> GetParBool(vec_SDname[a]+"In"))
				vec_fOpID[a] = SDman -> GetCollectionID("Op"+vec_HCname[a]);
		}
//		if(fOpSC2ID == -1 && PC->GetParBool("SC2In"))
//			fOpSC2ID = SCman -> GetCollectionID("OpSC2C");
	}
	fRunAction -> clear_data();
	if(PC -> GetParInt("UserVerbosity") > 0)
		if(event -> GetEventID() == 0)
			G4cout << "Begin of OpEventAction" << G4endl;
}

void OpEventAction::EndOfEventAction(const G4Event* event)
{
//	G4cout << "done" << G4endl;
	G4HCofThisEvent* HCE = event -> GetHCofThisEvent();
	if(!HCE)
	{
		G4ExceptionDescription msg;
		msg << "No hits collection of this event found.\n";
		G4Exception("OpEventAction::EndOfEventAction()","EA001",JustWarning,msg);
		return;
	}

	if(PC -> GetParBool("MCStep"))
	{
		for(G4int a=0; a<vec_fID.size(); a++)
		{
//			G4cout << vec_SDname[a]+"In" << G4endl;
			if(PC -> GetParBool(vec_SDname[a]+"In"))
			{
				OpSCHitsCollection* HC_SCn = 
					static_cast<OpSCHitsCollection*>(HCE->GetHC(vec_fID[a]));
				if(HC_SCn == nullptr)
					continue;
				G4int n_hit = HC_SCn -> entries();
				for(G4int b=0; b<n_hit; b++)
				{
					OpSCHit* hit = (*HC_SCn)[b];
					G4int trackID = hit -> GetTrackID();
					G4int trackPDG = hit -> GetTrackPDG();
					G4int detID = hit -> GetDetID();
					G4double edep = hit -> GetEdep();
					fRunAction -> FillStep(trackID, trackPDG, detID, detID, G4ThreeVector(),edep);
				}
			}
		}
//	if(PC -> GetParBool("SC2In"))
//	{
//		OpSCHitsCollection* HC_SC2 = 
//			static_cast<OpSCHitsCollection*>(HCE->GetHC(fSC2ID));
//		G4int n_hit2 = HC_SC2 -> entries();
//		for(G4int a=0; a<n_hit2; a++)
//		{
//			OpSCHit* hit = (*HC_SC2)[a];
//			G4int trackID = hit -> GetTrackID();
//			G4int trackPDG = hit -> GetTrackPDG();
//			G4int detID = hit -> GetDetID();
//			G4double edep = hit -> GetEdep();
//			fRunAction -> FillStep(trackID, trackPDG, detID, detID, G4ThreeVector(),edep);
//		}
//	}
//	if(PC -> GetParBool("SC3In"))
//	{
//		OpSCHitsCollection* HC_SC3 = 
//			static_cast<OpSCHitsCollection*>(HCE->GetHC(fSC3ID));
//		G4int n_hit2 = HC_SC3 -> entries();
//		for(G4int a=0; a<n_hit2; a++)
//		{
//			OpSCHit* hit = (*HC_SC3)[a];
//			G4int trackID = hit -> GetTrackID();
//			G4int trackPDG = hit -> GetTrackPDG();
//			G4int detID = hit -> GetDetID();
//			G4double edep = hit -> GetEdep();
//			fRunAction -> FillStep(trackID, trackPDG, detID, detID, G4ThreeVector(),edep);
//		}
//	}
//	if(PC -> GetParBool("BTOFIn"))
//	{
//		OpSCHitsCollection* HC_BTOF = 
//			static_cast<OpSCHitsCollection*>(HCE->GetHC(fBTOFID));
//		G4int n_hit = HC_BTOF -> entries();
//		for(G4int a=0; a<n_hit; a++)
//		{
//			OpSCHit* hit = (*HC_BTOF)[a];
//			G4int trackID = hit -> GetTrackID();
//			G4int trackPDG = hit -> GetTrackPDG();
//			G4int detID = hit -> GetDetID();
//			G4double edep = hit -> GetEdep();
//			fRunAction -> FillStep(trackID, trackPDG, detID, detID, G4ThreeVector(),edep);
//		}
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		for(G4int a=0; a<vec_fOpID.size(); a++)
		{
			if(PC -> GetParBool(vec_SDname[a]+"In"))
			{
				OpHitsCollection* OpHC_SCn = 
					static_cast<OpHitsCollection*>(HCE -> GetHC(vec_fOpID[a]));
				if(OpHC_SCn == nullptr)
					continue;
				G4int n_hit = OpHC_SCn -> entries();
				for(G4int b=0; b<n_hit; b++)	// N of optical photons
				{
					OpHit* hit = (*OpHC_SCn)[b];
					G4int trackID = hit -> GetTrackID();
					G4int detID = hit -> GetDetID();
					G4int size_vec = hit -> GetSizeOfVector();
					for(G4int c=0; c<size_vec; c++)	// N steps of each photon
					{
						G4int procID = hit -> GetProcID(c);
						G4ThreeVector mom = hit -> GetMomentum(c);
						G4ThreeVector pos = hit -> GetPosition(c);
						G4double time = hit -> GetTime(c);
						fRunAction -> FillOpticalPhotonBoundary(trackID, procID, mom, pos, time);
					}
				}
			}
		}
	}
	fRunAction -> update_Tree();
	if(PC -> GetParInt("UserVerbosity") > 0)
	{
		if(event -> GetEventID() == 0)
			G4cout << "End of OpEventAction" << G4endl;
		if(event -> GetEventID() % 100 == 0)
			G4cout << event -> GetEventID() << " event was done" << G4endl;
	}
	if(PC -> GetParInt("UserVerbosity") > 1)
	{
		if(event -> GetEventID() == 0)
			fRunAction -> PrintData(OpticalPhoton);
	}
	if(PC -> GetParInt("UserVerbosity") > 2)
		fRunAction -> PrintData(OpticalPhoton);
}

