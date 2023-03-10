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
#include "G4ParticleDefinition.hh"
#include "G4ProcessTable.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessType.hh"

#include "TObject.h"

#include <utility>
#include <algorithm>
#include <functional>
#include <map>

OpRunAction::OpRunAction()
: G4UserRunAction()
{ 
	PC = OpParameterContainer::GetInstance();

	if(PC -> GetParBool("UserOutputName"))
		F = new TFile(PC -> GetParString("OutName").c_str(),"recreate");
	else
		F = new TFile(("out_"+PC->GetParString("Beam_particle")+"_"+to_string(PC->GetParInt("Beam_energy"))+"MeV.root").c_str(),"recreate");
	T = new TTree("Opsim","Opsim");

	init_Tree();
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Constructor of OpRunAction" << G4endl;
}

OpRunAction::~OpRunAction()
{
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Destructor of OpRunAction" << G4endl;
	if(PC -> GetParInt("UserVerbosity") > 1)
		G4cout << "map_process size: " << map_process.size() << G4endl;
	for(auto iter : map_process)
	{
		if(PC -> GetParInt("UserVerbosity") > 1)
			G4cout << iter.first << " " << iter.second << G4endl;
		G4int procID = iter.second;
		G4String procName = iter.first;
		TNamed* process = new TNamed(to_string(procID),procName);
		fProcessTable.Add(process);
	}
	for(auto iter : map_input_para)
	{
		if(PC -> GetParInt("UserVerbosity") > 1)
			G4cout << iter.first << " " << iter.second << G4endl;
		G4String paraName = iter.first;
		G4String paraValue = iter.second;
		TNamed* input_para = new TNamed(paraName.data(), paraValue.data());
		fInputParameters.Add(input_para);
	}
	fProcessTable.Write("ProcessTable",TObject::kSingleKey);
	fInputParameters.Write("InputParameters",TObject::kSingleKey);
	F -> Write();
	F -> Close();
//	delete F;
//	delete T;
	F -> cd();
}

void OpRunAction::init_Tree()
{
	if(PC -> GetParBool("MCTrack"))
	{
		T -> Branch("nTrack",&nTrack);
		T -> Branch("TrackID",TrackID,"TrackID[nTrack]/I");
		T -> Branch("TrackProcID",TrackProcID,"TrackProcID[nTrack]/I");
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
		T -> Branch("PostTrackProcID",PostTrackProcID,"PostTrackProcID[nPostTrack]/I");
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
		T -> Branch("StepProcID",StepProcID,"StepProcID[nStep]/I");
		T -> Branch("StepTrackPDG",StepTrackPDG,"StepTrackPDG[nStep]/I");
		T -> Branch("StepDetID",StepDetID,"StepDetID[nStep]/I");
		T -> Branch("IsBoundary",IsBoundary,"IsBoundary[nStep]/B");
		T -> Branch("StepPrevKE",StepPrevKE,"StepPrevKE[nStep]/D");

		T -> Branch("StepVX",StepVX,"StepVX[nStep]/D");
		T -> Branch("StepVY",StepVY,"StepVY[nStep]/D");
		T -> Branch("StepVZ",StepVZ,"StepVZ[nStep]/D");
		T -> Branch("StepEdep",StepEdep,"StepEdep[nStep]/D");
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
		T -> Branch("OpEnergy",OpEnergy,"OpEnergy[NOpticalPhotons]/D");
		T -> Branch("OpKEnergy",OpKEnergy,"OpKEnergy[NOpticalPhotons]/D");
		T -> Branch("OpTime",OpTime,"OpTime[NOpticalPhotons]/D");
	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		T -> Branch("PostOpDetID",PostOpDetID,"PostOpDetID[NOpticalPhotons]/I");
		T -> Branch("PostProcID",PostProcID,"PostProcID[NOpticalPhotons]/I");
		T -> Branch("PostOpPX",PostOpPX,"PostOpPX[NOpticalPhotons]/D");
		T -> Branch("PostOpPY",PostOpPY,"PostOpPY[NOpticalPhotons]/D");
		T -> Branch("PostOpPZ",PostOpPZ,"PostOpPZ[NOpticalPhotons]/D");
		T -> Branch("PostOpVX",PostOpVX,"PostOpVX[NOpticalPhotons]/D");
		T -> Branch("PostOpVY",PostOpVY,"PostOpVY[NOpticalPhotons]/D");
		T -> Branch("PostOpVZ",PostOpVZ,"PostOpVZ[NOpticalPhotons]/D");
		T -> Branch("PostOpEnergy",PostOpEnergy,"PostOpEnergy[NOpticalPhotons]/D");
		T -> Branch("PostOpKEnergy",PostOpKEnergy,"PostOpKEnergy[NOpticalPhotons]/D");
		T -> Branch("PostOpTime",PostOpTime,"PostOpTime[NOpticalPhotons]/D");
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		T -> Branch("NOpBoundary",&NOpBoundary);
		T -> Branch("OpTrackIDBoundary",OpTrackIDBoundary,"OpTrackIDBoundary[NOpBoundary]/I");
		T -> Branch("OpProcIDBoundary",OpProcIDBoundary,"OpProcIDBoundary[NOpBoundary]/I");
		T -> Branch("OpPXBoundary",OpPXBoundary,"OpPXBoundary[NOpBoundary]/D");
		T -> Branch("OpPYBoundary",OpPYBoundary,"OpPYBoundary[NOpBoundary]/D");
		T -> Branch("OpPZBoundary",OpPZBoundary,"OpPZBoundary[NOpBoundary]/D");
		T -> Branch("OpVXBoundary",OpVXBoundary,"OpVXBoundary[NOpBoundary]/D");
		T -> Branch("OpVYBoundary",OpVYBoundary,"OpVYBoundary[NOpBoundary]/D");
		T -> Branch("OpVZBoundary",OpVZBoundary,"OpVZBoundary[NOpBoundary]/D");
		T -> Branch("OpTBoundary",OpTBoundary,"OpTBoundary[NOpBoundary]/D");
	}
}

void OpRunAction::BeginOfRunAction(const G4Run* run)
{
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Begin of OpRunAction" << G4endl;
}

void OpRunAction::EndOfRunAction(const G4Run* run)
{
	G4int Nevents = run -> GetNumberOfEvent();
	SetInputParameters(Nevents);
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "End of OpRunAction" << G4endl;
}

void OpRunAction::clear_data()
{
	set_procID.clear();
	if(PC->GetParBool("MCTrack"))
	{
		nTrack = 0;
		fill_n(TrackID,max_tracks,0);
		fill_n(TrackProcID,max_tracks,0);
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
		fill_n(PostTrackProcID,max_tracks,0);
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
		fill_n(StepProcID,max_steps,0);
		fill_n(StepTrackPDG,max_steps,0);
		fill_n(StepDetID,max_steps,0);
		fill_n(IsBoundary,max_steps,0);
		fill_n(StepPrevKE,max_steps,0);
		fill_n(StepVX,max_steps,0);
		fill_n(StepVY,max_steps,0);
		fill_n(StepVZ,max_steps,0);
		fill_n(StepEdep,max_steps,0);
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
		fill_n(OpEnergy,max_opticalphotons,0);
		fill_n(OpKEnergy,max_opticalphotons,0);
		fill_n(OpTime,max_opticalphotons,0);
	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		fill_n(PostOpDetID,max_opticalphotons,0);
		fill_n(PostProcID,max_opticalphotons,0);
		fill_n(PostOpPX,max_opticalphotons,0);
		fill_n(PostOpPY,max_opticalphotons,0);
		fill_n(PostOpPZ,max_opticalphotons,0);
		fill_n(PostOpVX,max_opticalphotons,0);
		fill_n(PostOpVY,max_opticalphotons,0);
		fill_n(PostOpVZ,max_opticalphotons,0);
		fill_n(PostOpEnergy,max_opticalphotons,0);
		fill_n(PostOpKEnergy,max_opticalphotons,0);
		fill_n(PostOpTime,max_opticalphotons,0);
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		NOpBoundary = 0;
		fill_n(OpTrackIDBoundary,max_opticalphotons,0);
		fill_n(OpProcIDBoundary,max_opticalphotons,0);
		fill_n(OpPXBoundary,max_opticalphotons,0);
		fill_n(OpPYBoundary,max_opticalphotons,0);
		fill_n(OpPZBoundary,max_opticalphotons,0);
		fill_n(OpVXBoundary,max_opticalphotons,0);
		fill_n(OpVYBoundary,max_opticalphotons,0);
		fill_n(OpVZBoundary,max_opticalphotons,0);
		fill_n(OpTBoundary,max_opticalphotons,0);
	}
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
	if(opt == MCTrack)	// starting point, prev track
	{
		TrackID[nTrack] = trkID;
		TrackProcID[nTrack] = procID;
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
		PostTrackProcID[nPostTrack] = procID;
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
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int detID, 
 G4ThreeVector p, G4ThreeVector v, G4double time, G4double energy, G4double kenergy)
{
	if(opt == MCTrack)
	{
//		G4cout << "Stacked o.p: " << NOpticalPhotons << G4endl;
//		G4int idx = find_OpIndex(OpTrackID);
//		G4cout << "Stacked o.p: " << sizeof(OpTrackID)/sizeof(OpTrackID[0]) << G4endl;
//		G4cout << creProcID << G4endl;
//		G4cout << OpProcessID[idx] << G4endl;
		OpTrackID[NOpticalPhotons] = trkID;
		OpProcessID[NOpticalPhotons] = procID;
		OpParentID[NOpticalPhotons] = parentID;
		OpDetID[NOpticalPhotons] = detID;
		OpPX[NOpticalPhotons] = p.x();
		OpPY[NOpticalPhotons] = p.y();
		OpPZ[NOpticalPhotons] = p.z();
		OpVX[NOpticalPhotons] = v.x();
		OpVY[NOpticalPhotons] = v.y();
		OpVZ[NOpticalPhotons] = v.z();
		OpTime[NOpticalPhotons] = time;
		OpEnergy[NOpticalPhotons] = energy;
		OpKEnergy[NOpticalPhotons] = kenergy;
		NOpticalPhotons++;
	}
	else if (opt == MCPostTrack)
	{
		G4int idx = find_OpIndex(trkID);
		PostOpDetID[idx] = detID;
		PostProcID[idx] = procID;
		PostOpPX[idx] = p.x();
		PostOpPY[idx] = p.y();
		PostOpPZ[idx] = p.z();
		PostOpVX[idx] = v.x();
		PostOpVY[idx] = v.y();
		PostOpVZ[idx] = v.z();
		PostOpEnergy[idx] = energy;
		PostOpKEnergy[idx] = kenergy;
		PostOpTime[idx] = time;
	}
}

void OpRunAction::FillOpticalPhotonBoundary
(G4int trkID, G4int procID, G4ThreeVector p, G4ThreeVector v, G4double t)
{
	set_procID.insert(procID);
	if(NOpBoundary == max_opticalphotons-1)
	{
		G4cout << "Number Of Optical Photon boundary steps exceed max_opticalphotons" << G4endl;
		return;
	}
	OpTrackIDBoundary[NOpBoundary] = trkID;
	OpProcIDBoundary[NOpBoundary] = procID;
	OpPXBoundary[NOpBoundary] = p.x();
	OpPYBoundary[NOpBoundary] = p.y();
	OpPZBoundary[NOpBoundary] = p.z();
	OpVXBoundary[NOpBoundary] = v.x();
	OpVYBoundary[NOpBoundary] = v.y();
	OpVZBoundary[NOpBoundary] = v.z();
	OpTBoundary[NOpBoundary] = t;
//	G4cout << t << G4endl;
	NOpBoundary++;
}


void OpRunAction::FillStep
(G4bool boundary, G4int trkID, G4int procID, G4int pdg, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep, G4double prevKE)
{
//	G4int idx = find_StepIndex(trkID);
//	if(prev_detID != post_detID)	// at the boundary
//		StepDetID[nStep] = post_detID;	// mainly this case means particle track killed in a volume
//	else
//		StepDetID[nStep] = prev_detID;
	if(nStep == max_steps-1)
	{
		G4cout << "Number of steps exceed the maximum step(" << max_steps << ")" << G4endl;
		return;
	}
	StepTrackID[nStep] = trkID;
	StepProcID[nStep] = procID;
	StepTrackPDG[nStep] = pdg;
	StepDetID[nStep] = prev_detID;
	IsBoundary[nStep] = boundary;
	StepPrevKE[nStep] = prevKE;
	StepVX[nStep] = v.x();
	StepVY[nStep] = v.y();
	StepVZ[nStep] = v.z();
	StepEdep[nStep] = edep;
	nStep++;
}


G4int OpRunAction::find_OpIndex(G4int trkID)
{
	for(G4int idx=0; idx<max_opticalphotons; idx++)
	{
		if(OpTrackID[idx] == trkID)
			return idx;
	}
	G4ExceptionDescription msg;
	msg << "OpRunAction::find_OpIndex can not find trkID" << G4endl;
	G4Exception("OpRunAction::find_OpIndex()", "OpR01",FatalException,msg);
}
G4int OpRunAction::find_StepIndex(G4int trkID)
{
	for(G4int idx=0; idx<max_steps; idx++)
	{
		if(StepTrackID[idx] == trkID)
			return idx;
	}
//	G4ExceptionDescription msg;
//	msg << "OpRunAction::find_OpIndex can not find trkID" << G4endl;
//	G4Exception("OpRunAction::find_OpIndex()", "OpR01",FatalException,msg);
}

void OpRunAction::update_Tree()
{
//	if(PC -> GetParBool("MCTrack"))
//		nTrack = sizeof(TrackID)/sizeof(TrackID[0]);
//	if(PC -> GetParBool("MCPostTrack"))
//		nPostTrack = sizeof(PostTrackID)/sizeof(PostTrackID[0]);
//	if(PC -> GetParBool("MCStep"))
//		nStep = sizeof(StepTrackID)/sizeof(StepTrackID[0]);
//	if(PC -> GetParBool("OpTrack"))
//		return;
//		NOpticalPhotons = sizeof(OpTrackID)/sizeof(OpTrackID[0]);

	T -> Fill();

//	PrintData(Process);
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
				" ProcID: " << OpProcessID[a] <<
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

	else if (opt == Process)
	{
//		auto procNames = G4ProcessTable::GetProcessTable() -> GetNameList();
//		G4cout << "###############" << G4endl;
//		G4cout << "Process Table" << G4endl;
//		G4int idx = 0;
//		for(auto name : *procNames)
//		{
//			G4cout << idx << " " << name << G4endl;
//			idx++;
//		}
//		G4cout << "###############" << G4endl;

//		for(iter = fProcessType.begin(); iter != fProcessType.end(); iter++)
	}
}

void OpRunAction::SetProcess(G4int procID, G4String processTypeName)
{
	if(map_process[processTypeName] == 0)
	{
		map_process[processTypeName] = procID;
	}else if(map_process[processTypeName] == procID){
		return;
	}else{
		G4cout << "process " << processTypeName << " is not matched to " << procID << G4endl;
	}

//	while(map_process[processTypeName] != procID)
//	{
//		G4cout << "Optimizing idx " << G4endl;//idx << " " << map_process[idx] << " " << processTypeName << G4endl;
//		if(map_process[processTypeName] == 0)
//			map_process[processTypeName] = procID;
//		if(map_process[processTypeName] == procID)
//		{
//			break;
//		}
////		idx++;
//	}
}

void OpRunAction::SetInputParameters(G4int nevnts)
{
	map_input_para.insert(make_pair("PhysicsList",PC->GetParString("PhysicsList")));
	map_input_para.insert(make_pair("RandomSeed",to_string(PC->GetParInt("RandomSeed"))));
	map_input_para.insert(make_pair("MacroFile",PC->GetParString("MacroFile")));
	map_input_para.insert(make_pair("Beam_particle",PC->GetParString("Beam_particle")));
	if(PC->GetParString("Beam_particle")=="ion")
	{
		map_input_para.insert(make_pair("InputIonID",to_string(PC->GetParInt("InputIonID"))));
		map_input_para.insert(make_pair("InputCharge",to_string(PC->GetParInt("InputCharge"))));
	}
	map_input_para.insert(make_pair("Beam_energy",to_string(PC->GetParInt("Beam_energy"))));
	map_input_para.insert(make_pair("NperEvent",to_string(PC->GetParInt("NperEvent"))));
	map_input_para.insert(make_pair("NEvents",to_string(nevnts)));
	map_input_para.insert(make_pair("Beam_shape",to_string(PC->GetParInt("Beam_shape"))));
	if(PC->GetParInt("Beam_shape") == 1)
	{
		map_input_para.insert(make_pair("Beam_x0",to_string(PC->GetParDouble("Beam_x0"))));
		map_input_para.insert(make_pair("Beam_dx",to_string(PC->GetParDouble("Beam_dx"))));
		map_input_para.insert(make_pair("Beam_y0",to_string(PC->GetParDouble("Beam_y0"))));
		map_input_para.insert(make_pair("Beam_dy",to_string(PC->GetParDouble("Beam_dy"))));
		map_input_para.insert(make_pair("Beam_z0",to_string(PC->GetParDouble("Beam_z0"))));
		map_input_para.insert(make_pair("Beam_dz",to_string(PC->GetParDouble("Beam_dz"))));
	}
	else if(PC->GetParInt("Beam_shape") == 2)
	{
		map_input_para.insert(make_pair("Beam_r0",to_string(PC->GetParDouble("Beam_r0"))));
	}
	map_input_para.insert(make_pair("MCTrack",PC->GetParBool("MCTrack")?"true":"false"));
	map_input_para.insert(make_pair("MCPostTrack",PC->GetParBool("MCPostTrack")?"true":"false"));
	map_input_para.insert(make_pair("MCStep",PC->GetParBool("MCStep")?"true":"false"));
	map_input_para.insert(make_pair("OpTrack",PC->GetParBool("OpTrack")?"true":"false"));
	map_input_para.insert(make_pair("OpPostTrack",PC->GetParBool("OpPostTrack")?"true":"false"));
	map_input_para.insert(make_pair("OpBoundary",PC->GetParBool("OpBoundary")?"true":"false"));
	map_input_para.insert(make_pair("SC1In",PC->GetParBool("SC1In")?"true":"false"));
	map_input_para.insert(make_pair("SC2In",PC->GetParBool("SC2In")?"true":"false"));
	map_input_para.insert(make_pair("SC3In",PC->GetParBool("SC2In")?"true":"false"));
	map_input_para.insert(make_pair("WorldID",to_string(PC->GetParInt("WorldID"))));
	map_input_para.insert(make_pair("World_sizeX",to_string(PC->GetParDouble("World_sizeX"))));
	map_input_para.insert(make_pair("World_sizeY",to_string(PC->GetParDouble("World_sizeY"))));
	map_input_para.insert(make_pair("World_sizeZ",to_string(PC->GetParDouble("World_sizeZ"))));
	if(PC->GetParBool("SC1In"))
	{
		map_input_para.insert(make_pair("SC1ID",to_string(PC->GetParInt("SC1ID"))));
		map_input_para.insert(make_pair("SC1_sizeX",to_string(PC->GetParDouble("SC1_sizeX"))));
		map_input_para.insert(make_pair("SC1_sizeY",to_string(PC->GetParDouble("SC1_sizeY"))));
		map_input_para.insert(make_pair("SC1_sizeZ",to_string(PC->GetParDouble("SC1_sizeZ"))));
		map_input_para.insert(make_pair("SC1_ZOffset",to_string(PC->GetParDouble("SC1_ZOffset"))));
		map_input_para.insert(make_pair("SC1matOpt",PC->GetParInt("SC1matOpt")==0?"PS":"PVT"));
	}
	if(PC->GetParBool("SC2In"))
	{
		map_input_para.insert(make_pair("SC2ID",to_string(PC->GetParInt("SC2ID"))));
		map_input_para.insert(make_pair("SC2_sizeX",to_string(PC->GetParDouble("SC2_sizeX"))));
		map_input_para.insert(make_pair("SC2_sizeY",to_string(PC->GetParDouble("SC2_sizeY"))));
		map_input_para.insert(make_pair("SC2_sizeZ",to_string(PC->GetParDouble("SC2_sizeZ"))));
		map_input_para.insert(make_pair("SC2_ZOffset",to_string(PC->GetParDouble("SC2_ZOffset"))));
		map_input_para.insert(make_pair("SC2matOpt",PC->GetParInt("SC2matOpt")==0?"PS":"PVT"));
	}
	if(PC->GetParBool("SC3In"))
	{
		map_input_para.insert(make_pair("SC3ID",to_string(PC->GetParInt("SC3ID"))));
		map_input_para.insert(make_pair("SC3_sizeX",to_string(PC->GetParDouble("SC3_sizeX"))));
		map_input_para.insert(make_pair("SC3_sizeY",to_string(PC->GetParDouble("SC3_sizeY"))));
		map_input_para.insert(make_pair("SC3_sizeZ",to_string(PC->GetParDouble("SC3_sizeZ"))));
		map_input_para.insert(make_pair("SC3_ZOffset",to_string(PC->GetParDouble("SC3_ZOffset"))));
		map_input_para.insert(make_pair("SC3matOpt",PC->GetParInt("SC3matOpt")==0?"PS":"PVT"));
	}
	if(PC->GetParBool("BDCIn"))
	{
		map_input_para.insert(make_pair("BDCID",to_string(PC->GetParInt("BDCID"))));
		map_input_para.insert(make_pair("BDC_sizeX",to_string(PC->GetParDouble("BDC_sizeX"))));
		map_input_para.insert(make_pair("BDC_sizeY",to_string(PC->GetParDouble("BDC_sizeY"))));
		map_input_para.insert(make_pair("BDC_sizeZ",to_string(PC->GetParDouble("BDC_sizeZ"))));
		map_input_para.insert(make_pair("BDC_ZOffset",to_string(PC->GetParDouble("BDC_ZOffset"))));
	}
	if(PC->GetParBool("BTOFIn"))
	{
		map_input_para.insert(make_pair("BTOFID",to_string(PC->GetParInt("BTOFID"))));
		map_input_para.insert(make_pair("BTOF_sizeX",to_string(PC->GetParDouble("BTOF_sizeX"))));
		map_input_para.insert(make_pair("BTOF_sizeY",to_string(PC->GetParDouble("BTOF_sizeY"))));
		map_input_para.insert(make_pair("BTOF_sizeZ",to_string(PC->GetParDouble("BTOF_sizeZ"))));
		map_input_para.insert(make_pair("BTOF_ZOffset",to_string(PC->GetParDouble("BTOF_ZOffset1"))));
		map_input_para.insert(make_pair("BTOF_ZOffset",to_string(PC->GetParDouble("BTOF_ZOffset2"))));
	}
	if(PC->GetParBool("SensorIn"))
	{
		map_input_para.insert(make_pair("SensorID",to_string(PC->GetParInt("SensorID"))));
		map_input_para.insert(make_pair("Sensor_sizeX",to_string(PC->GetParDouble("Sensor_sizeX"))));
		map_input_para.insert(make_pair("Sensor_sizeY",to_string(PC->GetParDouble("Sensor_sizeY"))));
		map_input_para.insert(make_pair("Sensor_totX",to_string(PC->GetParDouble("Sensor_totY"))));
		map_input_para.insert(make_pair("Sensor_sizeZ",to_string(PC->GetParDouble("Sensor_sizeZ"))));
		map_input_para.insert(make_pair("Sensor_ZOffset",to_string(PC->GetParDouble("Sensor_ZOffset"))));
		map_input_para.insert(make_pair("Sensor_gapIn",PC->GetParBool("Sensor_gapIn")?"true":"false"));
		if(PC->GetParBool("Sensor_gapIn"))
			map_input_para.insert(make_pair("Sensor_gap",to_string(PC->GetParDouble("Sensor_gap"))));
	}
//	map_input_para.insert(make_pair(
//	map_input_para.insert(make_pair(
//	map_input_para.insert(make_pair(
}
