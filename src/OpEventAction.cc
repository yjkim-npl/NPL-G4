#include "OpEventAction.hh"
#include "OpRunAction.hh"
#include "OpHit.hh"
#include "OpSiPMHit.hh"

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
	vec_fID = {-1,-1,-1,-1,-1};
	fSiPMID = -1;
	vec_HCname = {"SC1C","SC2C","SC3C","BDCC","BTOFC"};
	vec_SDname = {"SC1","SC2","SC3","BDC","BTOF"};
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
	if(PC->GetParBool("MCStep") ||
		(PC->GetParBool("OpticalPhysics") && PC -> GetParBool("SiPMIn")))
	{
		for(G4int a=0; a<vec_fID.size(); a++)
		{
			if(vec_fID[a] == -1 && PC ->GetParBool(vec_SDname[a]+"In"))
			{
				vec_fID[a] = SDman -> GetCollectionID(vec_HCname[a]);
			}
		}
		if(fSiPMID == -1 && PC -> GetParBool("SiPMIn"))
		{
			fSiPMID = SDman -> GetCollectionID("SC2SiPMC");
		}
	}
	fRunAction -> clear_data();
	if(PC -> GetParInt("UserVerbosity") > 0)
		if(event -> GetEventID() == 0)
			G4cout << "Begin of OpEventAction" << G4endl;
}

void OpEventAction::EndOfEventAction(const G4Event* event)
{
	G4HCofThisEvent* HCE = event -> GetHCofThisEvent();
	if(!HCE)
	{
		G4ExceptionDescription msg;
		msg << "No hits collection of this event found.\n";
		G4Exception("OpEventAction::EndOfEventAction()","EA001",JustWarning,msg);
		return;
	}

	if(PC->GetParBool("MCStep") ||
		(PC->GetParBool("OpticalPhysics") && PC->GetParBool("OpBoundary")))
	{
		G4cout << "EndOfEventAction " << G4endl;
		for(G4int a=0; a<vec_fID.size(); a++)
		{
			if(vec_fID.at(a) == -1)
				continue;
//			G4cout << vec_SDname[a]+"In" << G4endl;
			if(PC -> GetParBool(vec_SDname[a]+"In"))
			{
//				G4cout << a << " : " << vec_fID.at(a) << G4endl;
//				G4cout << a << " : " << vec_SDname.at(a) << G4endl;
				OpHitsCollection* HC_SCn = 
					static_cast<OpHitsCollection*>(HCE->GetHC(vec_fID[a]));
				if(HC_SCn == nullptr)
					continue;
				G4int n_hit = HC_SCn -> entries();
//				G4cout << "n_hit : " << n_hit << G4endl;
				for(G4int b=0; b<n_hit; b++)
				{
					OpHit* hit = (*HC_SCn)[b];
					G4int nSteps = hit -> GetNSteps();
					G4int trackID = hit -> GetTrackID();
					G4int trackPDG = hit -> GetTrackPDG();
					G4int detID = hit -> GetDetID();
					G4int postDetID = hit -> GetPostDetID();
					G4double EdepSum = hit -> GetEdepSum();
					// save edepsum of particle species
					if(PC->GetParBool("SaveTrackSum"))
					{
						fRunAction -> FillStep
							(false,1,trackID,0,trackPDG,detID, detID, G4ThreeVector(),EdepSum,0,0);
//						G4cout << "#########" << G4endl;
//						G4cout << "TrkID(PDG): " << trackID << "(" << trackPDG << ")" << G4endl;
//						G4cout << "DetID: " << detID << G4endl;
//						G4cout << "EdepSum: " << EdepSum << G4endl;
					}
					// save all steps which contains optical photon data
					else
					{
//						if(trackPDG == -22)
//							G4cout << trackPDG << " " << nSteps << G4endl;
						for(G4int c=0; c<nSteps; c++)
						{
							G4int procID = hit -> GetProcID(c);
							G4int nSecondaryOP = 0;
							G4bool boundary = 0;
							G4double prevKE = 0;
							G4double length = 0;
							if(trackPDG != -22)
							{
								nSecondaryOP = hit -> GetNSecondaryOP(c);
								boundary = hit -> GetIsBoundary(c);
								prevKE = hit -> GetKE(c);
								length = hit -> GetStepLength(c);
							}
//							G4cout << "nSecondaryOP: " << nSecondaryOP << G4endl;
							G4String procName = hit -> GetProcName(c);
							G4ThreeVector mom = hit -> GetMomentum(c);
							G4ThreeVector pos = hit -> GetPosition(c);
							G4int firstStep = hit -> GetIsFirstStep(c);
							G4double time = hit -> GetTime(c);
							G4double edep = hit -> GetEdep(c);
							fRunAction -> SetProcess(procID,procName);
							if(PC->GetParBool("MCStep") && trackPDG != -22)
							{
								fRunAction -> FillStep
									(boundary,1,trackID,procID,trackPDG,detID,boundary?postDetID:detID,pos,
									 edep,length,nSecondaryOP,prevKE);
							}
							if(PC->GetParBool("OpBoundary") && trackPDG == -22)// && firstStep == 1)
							{
								fRunAction -> FillOpticalPhotonBoundary(trackID, procID, mom, pos, time);
							}
						}
					}
				}
			}
		}
		if(PC->GetParBool("OpticalPhysics") && PC->GetParBool("SiPMIn"))
		{
			OpSiPMHitsCollection* HC_SiPM = 
				static_cast<OpSiPMHitsCollection*>(HCE->GetHC(fSiPMID));
//			if(HC_SiPM == nullptr)
			if(HC_SiPM != nullptr)
			{
				for(G4int b=0; b<HC_SiPM->entries(); b++)
				{
					OpSiPMHit* hit = (*HC_SiPM)[b];
					G4int NOpSiPM = hit -> GetPhotonCount();
					G4int detID = hit -> GetDetectorID();
					for(G4int c=0; c<NOpSiPM; c++)
					{
						G4int procID = hit -> GetProcID(c);
						G4String procName = hit -> GetProcName(c);
						G4ThreeVector pos = hit -> GetPosition(c);
						G4ThreeVector mom = hit -> GetMomentum(c);
						G4double time = hit -> GetTime(c);
						G4double energy = hit -> GetEnergy(c);
						if(PC->GetParBool("OpSiPM"))
						{
							fRunAction -> FillSiPM(detID,procID,procName,pos,mom,time,energy);
//							G4cout << "Filling"
						}
					}
				}
			}
		}
	}
//	if(PC -> GetParBool("OpBoundary"))
//	{
//		for(G4int a=0; a<vec_fOpID.size(); a++)
//		{
//			if(PC -> GetParBool(vec_SDname[a]+"In"))
//			{
//				OpHitsCollection* OpHC_SCn = 
//					static_cast<OpHitsCollection*>(HCE -> GetHC(vec_fOpID[a]));
//				if(OpHC_SCn == nullptr)
//					continue;
//				G4int n_hit = OpHC_SCn -> entries();
//				for(G4int b=0; b<n_hit; b++)	// N of optical photons
//				{
//					OpHit* hit = (*OpHC_SCn)[b];
//					G4int trackID = hit -> GetTrackID();
//					G4int trackPDG = hit -> GetTrackPDG();
//					G4double edep = hit -> GetEdep();
//					G4int detID = hit -> GetDetID();
//					G4int size_vec = hit -> GetSizeOfVector();
//					if(trackID == 1)
////						G4cout << "NSteps: " << size_vec << G4endl;
//					if(PC->GetParBool("MCStep") && false)
//					{
////						if(trackPDG == 2212) G4cout << "proton" << G4endl;
//						fRunAction -> FillStep(trackID,trackPDG,detID,detID,G4ThreeVector(),edep);
//					}
//					for(G4int c=0; c<size_vec; c++)	// N steps of each photon
//					{
//						G4int IsBoundary = hit -> GetIsBoundary(c);
//						G4int procID = hit -> GetProcID(c);
//						G4ThreeVector mom = hit -> GetMomentum(c);
//						G4ThreeVector pos = hit -> GetPosition(c);
//						G4double time = hit -> GetTime(c);
//						fRunAction -> FillOpticalPhotonBoundary(trackID, procID, mom, pos, time);
//					}
//				}
//			}
//		}
//	}
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

