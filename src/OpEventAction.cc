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

	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpEventAction" << G4endl;
} 

OpEventAction::~OpEventAction()
{
	OpEventAction::nOPsteps = 0;

	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpEventAction" << G4endl;
}

void OpEventAction::BeginOfEventAction(const G4Event* event)
{
	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0)
		if(event -> GetEventID() == 0)
			G4cout << "Begin of OpEventAction" << G4endl;

	// Initialization of the name of ID of hit collection & sensitive detector
	fSCID = -1;
	fSiPMID = -1;

	// load manager of Sensitive detector
	G4SDManager* SDman = G4SDManager::GetSDMpointer();

	if(PC->GetParBool("MCStep") ||
		(PC->GetParBool("OpticalPhysics") && PC -> GetParBool("SiPMIn")))
	{
		if(fSCID == -1 && PC ->GetParBool("SCIn"))
		{
			fSCID = SDman -> GetCollectionID("SCC");
		}
		if(fSiPMID == -1 && PC -> GetParBool("SiPMIn"))
		{
			fSiPMID = SDman -> GetCollectionID("SCSiPMC");
		}
	}

	// clear the data of previous event
	fRunAction -> clear_data();
}

void OpEventAction::EndOfEventAction(const G4Event* event)
{
	// call all Hit Collections in this event
	G4HCofThisEvent* HCE = event -> GetHCofThisEvent();
	if(!HCE)
	{
		G4ExceptionDescription msg;
		msg << "No hits collection of this event found.\n";
		G4Exception("OpEventAction::EndOfEventAction()","EA001",JustWarning,msg);
		return;
	}

	// MCStep & Optical boundary data from hit
	if(PC->GetParBool("MCStep") ||
		(PC->GetParBool("OpticalPhysics") &&
		 (PC->GetParBool("OpBoundary") || PC->GetParBool("OpSiPM") ) ) )
	{
		if(fSCID == -1)
			return;
		if(PC -> GetParBool("SCIn"))
		{
			OpHitsCollection* HC_SCn = 
				static_cast<OpHitsCollection*>(HCE->GetHC(fSCID));
			if(HC_SCn == nullptr)
				return;
			G4int n_hit = HC_SCn -> entries();
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
						(false,1,trackID,0,trackPDG,detID, detID, G4ThreeVector(),0,EdepSum,0,0);
				}
				// save all steps which contains optical photon data
				else
				{
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
								(boundary,1,trackID,procID,trackPDG,detID,boundary?postDetID:detID,pos, time,
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

		// OpSiPM data
		if(PC->GetParBool("OpticalPhysics") && PC->GetParBool("SiPMIn"))
		{
			OpSiPMHitsCollection* HC_SiPM = 
				static_cast<OpSiPMHitsCollection*>(HCE->GetHC(fSiPMID));
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
						}
					}
				}
			}
		}
	}
	fRunAction -> update_Tree();
	if(PC -> GetParInt("UserVerbosity") > 0)
	{
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

