#include "OpRunAction.hh"
#include "OpPrimaryGeneratorAction.hh"
#include "OpDetectorConstruction.hh"
#include "OpParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <utility>
#include <algorithm>
#include <functional>
#include <map>

OpRunAction::OpRunAction()
: G4UserRunAction()
{ 
	PC = OpParameterContainer::GetInstance();

	F = new TFile(PC -> GetParString("OutName").c_str(),"recreate");
	T = new TTree("Opsim","Opsim");

	init_Tree();
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Constructor of OpRunAction" << G4endl;
}

OpRunAction::~OpRunAction()
{
	F -> Write();
	F -> Close();
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Destructor of OpRunAction" << G4endl;
}

void OpRunAction::init_Tree()
{
	if(PC -> GetParBool("MCTrack"))
	{
		T -> Branch("nTrack",&nTrack);
		T -> Branch("TrackID",TrackID,"TrackID[nTrack]/I");
		T -> Branch("ParentID",ParentID,"ParentID[nTrack]/I");
		T -> Branch("TrackPDG",TrackPDG,"TrackPDG[nTrack]/I");
		T -> Branch("TrackDetID",TrackDetID,"TrackDetID[nTrack]/I");

		T -> Branch("TrackPX",TrackPX,"TrackPX[nTrack]/D");
		T -> Branch("TrackPY",TrackPY,"TrackPY[nTrack]/D");
		T -> Branch("TrackPZ",TrackPZ,"TrackPZ[nTrack]/D");
		T -> Branch("TrackVX",TrackVX,"TrackVX[nTrack]/D");
		T -> Branch("TrackVY",TrackVY,"TrackVY[nTrack]/D");
		T -> Branch("TrackVZ",TrackVZ,"TrackVZ[nTrack]/D");
		T -> Branch("TrackEnergy",TrackEnergy,"TrackEnergy[nTrack]/D");
		T -> Branch("TrackKEnergy",TrackKEnergy,"TrackKEnergy[nTrack]/D");
	}
	if(PC -> GetParBool("MCPostTrack"))
	{
		T -> Branch("nPostTrack",&nPostTrack);
		T -> Branch("PostTrackID",PostTrackID,"PostTrackID[nPostTrack]/I");
		T -> Branch("PostTrackDetID",PostTrackDetID,"PostTrackDetID[nPostTrack]/I");

		T -> Branch("PostTrackPX",PostTrackPX,"PostTrackPX[nPostTrack]/D");
		T -> Branch("PostTrackPY",PostTrackPY,"PostTrackPY[nPostTrack]/D");
		T -> Branch("PostTrackPZ",PostTrackPZ,"PostTrackPZ[nPostTrack]/D");
		T -> Branch("PostTrackVX",PostTrackVX,"PostTrackVX[nPostTrack]/D");
		T -> Branch("PostTrackVY",PostTrackVY,"PostTrackVY[nPostTrack]/D");
		T -> Branch("PostTrackVZ",PostTrackVZ,"PostTrackVZ[nPostTrack]/D");
		T -> Branch("PostTrackEnergy",PostTrackEnergy,"PostTrackEnergy[nPostTrack]/D");
		T -> Branch("PostTrackKEnergy",PostTrackKEnergy,"PostTrackKEnergy[nPostTrack]/D");
	}
	if(PC -> GetParBool("MCStep"))
	{
		T -> Branch("nStep",&nStep);
		T -> Branch("StepTrackID",StepTrackID,"StepTrackID[nStep]/I");
		T -> Branch("StepDetID",StepDetID,"StepDetID[nStep]/I");

		T -> Branch("StepVX",StepVX,"StepVX[nStep]/D");
		T -> Branch("StepVY",StepVY,"StepVY[nStep]/D");
		T -> Branch("StepVZ",StepVZ,"StepVZ[nStep]/D");
		T -> Branch("StepEdep",StepEdep,"StepEdep[nStep]/D");

		// for energy sum of box
		T -> Branch("EdepSumBox",&EdepSumBox);
	}
	if(PC -> GetParBool("OpTrack"))
	{
		T -> Branch("NOpticalPhotons",&NOpticalPhotons);
		T -> Branch("OpTrackID",OpTrackID,"OpTrackID[NOpticalPhotons]/I");
		T -> Branch("OpProcessID",OpProcessID,"OpProcessID[NOpticalPhotons]/I");
		T -> Branch("OpParentID",OpParentID,"OpParentID[NOpticalPhotons]/I");
		T -> Branch("OpDetID",OpDetID,"OpDetID[NOpticalPhotons]/I");
		T -> Branch("OpPX",OpPX,"OpPX[NOpticalPhotons]/D");
		T -> Branch("OpPY",OpPY,"OpPY[NOpticalPhotons]/D");
		T -> Branch("OpPZ",OpPZ,"OpPZ[NOpticalPhotons]/D");
		T -> Branch("OpVX",OpVX,"OpVX[NOpticalPhotons]/D");
		T -> Branch("OpVY",OpVY,"OpVY[NOpticalPhotons]/D");
		T -> Branch("OpVZ",OpVZ,"OpVZ[NOpticalPhotons]/D");
		T -> Branch("OpTime",OpTime,"OpTime[NOpticalPhotons]/D");
		T -> Branch("PostOpDetID",PostOpDetID,"PostOpDetID[NOpticalPhotons]/I");
		T -> Branch("PostProcID",PostProcID,"PostProcID[NOpticalPhotons]/I");
		T -> Branch("PostOpPX",PostOpPX,"PostOpPX[NOpticalPhotons]/D");
		T -> Branch("PostOpPY",PostOpPY,"PostOpPY[NOpticalPhotons]/D");
		T -> Branch("PostOpPZ",PostOpPZ,"PostOpPZ[NOpticalPhotons]/D");
		T -> Branch("PostOpVX",PostOpVX,"PostOpVX[NOpticalPhotons]/D");
		T -> Branch("PostOpVY",PostOpVY,"PostOpVY[NOpticalPhotons]/D");
		T -> Branch("PostOpVZ",PostOpVZ,"PostOpVZ[NOpticalPhotons]/D");
		T -> Branch("PostOpTime",PostOpTime,"PostOpTime[NOpticalPhotons]/D");
	}
}

void OpRunAction::BeginOfRunAction(const G4Run*)
{
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Begin of OpRunAction" << G4endl;
}

void OpRunAction::EndOfRunAction(const G4Run* run)
{
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "End of OpRunAction" << G4endl;
}

void OpRunAction::clear_data()
{
	if(PC->GetParBool("MCTrack"))
	{
		nTrack = 0;
		fill_n(TrackID,max_tracks,0);
		fill_n(ParentID,max_tracks,0);
		fill_n(TrackPDG,max_tracks,0);
		fill_n(TrackDetID,max_tracks,0);
		fill_n(TrackPX,max_tracks,0);
		fill_n(TrackPY,max_tracks,0);
		fill_n(TrackPZ,max_tracks,0);
		fill_n(TrackVX,max_tracks,0);
		fill_n(TrackVY,max_tracks,0);
		fill_n(TrackVZ,max_tracks,0);
		fill_n(TrackEnergy,max_tracks,0);
		fill_n(TrackKEnergy,max_tracks,0);
	}
	if(PC->GetParBool("MCPostTrack"))
	{
		nPostTrack = 0;
		fill_n(PostTrackID,max_tracks,0);
		fill_n(PostTrackPDG,max_tracks,0);
		fill_n(PostTrackDetID,max_tracks,0);
		fill_n(PostTrackPX,max_tracks,0);
		fill_n(PostTrackPY,max_tracks,0);
		fill_n(PostTrackPZ,max_tracks,0);
		fill_n(PostTrackVX,max_tracks,0);
		fill_n(PostTrackVY,max_tracks,0);
		fill_n(PostTrackVZ,max_tracks,0);
		fill_n(PostTrackEnergy,max_tracks,0);
		fill_n(PostTrackKEnergy,max_tracks,0);
	}
	if(PC->GetParBool("MCStep"))
	{
		nStep = 0;
		fill_n(StepTrackID,max_steps,0);
		fill_n(StepDetID,max_steps,0);
		fill_n(StepVX,max_steps,0);
		fill_n(StepVY,max_steps,0);
		fill_n(StepVZ,max_steps,0);
		fill_n(StepEdep,max_steps,0);
		EdepSumBox = 0;
	}
	if(PC->GetParBool("OpTrack"))
	{
		NOpticalPhotons = 0;
		fill_n(OpTrackID,max_opticalphotons,0);
		fill_n(OpProcessID,max_opticalphotons,0);
		fill_n(OpParentID,max_opticalphotons,0);
		fill_n(OpDetID,max_opticalphotons,0);
		fill_n(OpPX,max_opticalphotons,0);
		fill_n(OpPY,max_opticalphotons,0);
		fill_n(OpPZ,max_opticalphotons,0);
		fill_n(OpVX,max_opticalphotons,0);
		fill_n(OpVY,max_opticalphotons,0);
		fill_n(OpVZ,max_opticalphotons,0);
		fill_n(OpTime,max_opticalphotons,0);

		fill_n(PostOpDetID,max_opticalphotons,0);
		fill_n(PostProcID,max_opticalphotons,0);
		fill_n(PostOpPX,max_opticalphotons,0);
		fill_n(PostOpPY,max_opticalphotons,0);
		fill_n(PostOpPZ,max_opticalphotons,0);
		fill_n(PostOpVX,max_opticalphotons,0);
		fill_n(PostOpVY,max_opticalphotons,0);
		fill_n(PostOpVZ,max_opticalphotons,0);
		fill_n(PostOpTime,max_opticalphotons,0);
	}
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
	if(opt == MCTrack)	// starting point, prev track
	{
		TrackID[nTrack] = trkID;
		ParentID[nTrack] = parentID;
		TrackPDG[nTrack] = pdg;
		TrackDetID[nTrack] = detID;
		TrackPX[nTrack] = p.x();
		TrackPY[nTrack] = p.y();
		TrackPZ[nTrack] = p.z();
		TrackVX[nTrack] = v.x();
		TrackVY[nTrack] = v.y();
		TrackVZ[nTrack] = v.z();
		TrackEnergy[nTrack] = totenergy;
		TrackKEnergy[nTrack] = kinenergy;
		nTrack++;
	}
	else if(opt == MCPostTrack)	// end point, post Track
	{
		PostTrackID[nPostTrack] = trkID;
		PostTrackDetID[nPostTrack] = detID;
		PostTrackPX[nPostTrack] = p.x();
		PostTrackPY[nPostTrack] = p.y();
		PostTrackPZ[nPostTrack] = p.z();
		PostTrackVX[nPostTrack] = v.x();
		PostTrackVY[nPostTrack] = v.y();
		PostTrackVZ[nPostTrack] = v.z();
		PostTrackEnergy[nPostTrack] = totenergy;
		PostTrackKEnergy[nPostTrack] = kinenergy;
		nPostTrack++;
	}
	else
	{
		G4cout << "Error in OpRunAction::FillTrack()" << G4endl;
	}
}

void OpRunAction::FillOpticalPhoton
(G4int opt, G4int trkID, G4int processID, G4int parentID, G4int detID, 
 G4ThreeVector p, G4ThreeVector v, G4double time)
{
	if(opt == MCTrack)
	{
//		G4cout << "Stacked o.p: " << NOpticalPhotons << G4endl;
//		G4int idx = find_OpIndex(OpTrackID);
//		G4cout << "Stacked o.p: " << sizeof(OpTrackID)/sizeof(OpTrackID[0]) << G4endl;
//		G4cout << creProcID << G4endl;
//		G4cout << OpProcessID[idx] << G4endl;
		OpTrackID[NOpticalPhotons] = trkID;
		OpProcessID[NOpticalPhotons] = processID;
		OpParentID[NOpticalPhotons] = parentID;
		OpDetID[NOpticalPhotons] = detID;
		OpPX[NOpticalPhotons] = p.x();
		OpPY[NOpticalPhotons] = p.y();
		OpPZ[NOpticalPhotons] = p.z();
		OpVX[NOpticalPhotons] = v.x();
		OpVY[NOpticalPhotons] = v.y();
		OpVZ[NOpticalPhotons] = v.z();
		OpTime[NOpticalPhotons] = time;
		NOpticalPhotons++;
	}
	else if (opt == MCPostTrack)
	{
		G4int idx = find_OpIndex(trkID);
		PostOpDetID[idx] = detID;
		PostProcID[idx] = processID;
		PostOpPX[idx] = p.x();
		PostOpPY[idx] = p.y();
		PostOpPZ[idx] = p.z();
		PostOpVX[idx] = v.x();
		PostOpVY[idx] = v.y();
		PostOpVZ[idx] = v.z();
		PostOpTime[idx] = time;
	}
}

void OpRunAction::FillStep
(G4int trkID, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep)
{
	if(prev_detID != post_detID)	// at the boundary
		StepDetID[nStep] = post_detID;	// mainly this case means particle track killed in a volume
	else
		StepDetID[nStep] = prev_detID;
	StepTrackID[nStep] = trkID;
	StepVX[nStep] = v.x();
	StepVY[nStep] = v.y();
	StepVZ[nStep] = v.z();
	StepEdep[nStep] = edep;

	if(StepDetID[nStep] == PC -> GetParInt("BoxID"))
		EdepSumBox += edep;
	nStep++;
}


G4int OpRunAction::find_OpIndex(G4int trkID)
{
	for(G4int idx=0; idx<NOpticalPhotons; idx++)
	{
		if(OpTrackID[idx] == trkID)
			return idx;
		else if (idx == max_opticalphotons-1)
		{
			G4ExceptionDescription msg;
			msg << "OpRunAction::find_OpIndex exceed maxOpticalPhotons" << G4endl;
			G4Exception("OpRunAction::find_OpIndex()", "OpR01",FatalException,msg);
		}
//		else
//		{
//			G4ExceptionDescription msg;
//			msg << "OpRunAction::find_OpIndex can not find trkID" << G4endl;
//			G4Exception("OpRunAction::find_OpIndex()", "OpR02",FatalException,msg);
//		}
	}
//	G4cout << "RunAction::find_OpIndex " << trkID <<
//		" " << OpTrackID[22] 
//		<<G4endl;
//	return 0;
}

void OpRunAction::update_Tree()
{
	if(PC -> GetParBool("MCTrack"))
		nTrack = sizeof(TrackID)/sizeof(TrackID[0]);
	if(PC -> GetParBool("MCPostTrack"))
		nPostTrack = sizeof(PostTrackID)/sizeof(PostTrackID[0]);
	if(PC -> GetParBool("MCStep"))
		nStep = sizeof(StepTrackID)/sizeof(StepTrackID[0]);
//	if(PC -> GetParBool("OpTrack"))
//		return;
//		NOpticalPhotons = sizeof(OpTrackID)/sizeof(OpTrackID[0]);

	T -> Fill();

//	PrintData(OpticalPhoton);
}

void OpRunAction::PrintData(G4int opt)
{
	if(opt == MCTrack)
	{
		G4cout << nTrack << G4endl;
	}
	else if (opt == MCPostTrack)
	{
		G4cout << nPostTrack << G4endl;
	}
	else if (opt == OpticalPhoton)
	{
		for(G4int a=0; a<NOpticalPhotons; a++)
		{
			if(a%10 == 0) 
				G4cout << G4endl;
			G4cout << a << " OptrckID: " << OpTrackID[a] <<
//				" ProcID: " << OpProcessID[a] <<
				" ParID: " << OpParentID[a] <<
//				" DetID: " << OpDetID[a] <<
//				" PZ: " << OpPZ[a] << 
//				" VX: " << (G4float)OpVX[a] << 
//				" VY: " << (G4float)OpVY[a] << 
//				" Time: " << (G4float)OpTime[a] << 
				" PostDetID: " << PostOpDetID[a] << 
				" PostProcID: " << PostProcID[a] << 
//				" pPZ: " << PostOpPZ[a] << 
//				" pVX: " << PostOpVX[a] << 
//				" pVY: " << PostOpVY[a] << 
//				" pTime: " << PostOpTime[a] << 
				G4endl;
		}
		G4cout << "NOpticalPhotns: " << NOpticalPhotons << G4endl;
	}
}

