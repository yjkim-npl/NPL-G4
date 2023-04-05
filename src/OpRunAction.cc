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
//	G4String beam_energy = to_string(PC->GetParDouble("Beam_energy"));
//	if(beam_energy.contains("."))
	G4String outName;
	if(PC->GetParBool("UserOutputName") == false)
	{
		if(PC->GetParString("Beam_particle") == "opticalphoton"){
			outName = "out_"+PC->GetParString("Beam_particle")+"_"+
				to_string((G4int)PC->GetParDouble("OpWavlen"))+"nm.root";
		}else{
			outName = "out_"+PC->GetParString("Beam_particle")+"_"+
				to_string((G4int)PC->GetParDouble("Beam_energy"))+"MeV.root";
		}
	}else{
		outName = PC->GetParString("OutName");
	}
	G4cout << "#######################" << G4endl;
	G4cout << "outName: " << outName << G4endl;
	G4cout << "#######################" << G4endl << G4endl;

	if(PC -> GetParBool("UserOutputName"))
		F = new TFile(PC -> GetParString("OutName").c_str(),"recreate");
	else
		F = new TFile(outName,"recreate");
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
	map_input_para = PC -> GetInputParameters(nevnts);
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
		T -> Branch("StepFromHit",StepFromHit,"StepFromHit[nStep]/I");
		T -> Branch("StepProcID",StepProcID,"StepProcID[nStep]/I");
		T -> Branch("StepTrackPDG",StepTrackPDG,"StepTrackPDG[nStep]/I");
		T -> Branch("StepPrevDetID",StepPrevDetID,"StepPrevDetID[nStep]/I");
		T -> Branch("StepPostDetID",StepPostDetID,"StepPostDetID[nStep]/I");
		T -> Branch("IsBoundary",IsBoundary,"IsBoundary[nStep]/I");
		T -> Branch("StepPrevKE",StepPrevKE,"StepPrevKE[nStep]/D");

		T -> Branch("StepVX",StepVX,"StepVX[nStep]/D");
		T -> Branch("StepVY",StepVY,"StepVY[nStep]/D");
		T -> Branch("StepVZ",StepVZ,"StepVZ[nStep]/D");
		T -> Branch("StepEdep",StepEdep,"StepEdep[nStep]/D");
		T -> Branch("StepLength",StepLength,"StepLength[nStep]/D");
		if(PC->GetParBool("OpticalPhysics"))
		{
			T -> Branch("StepNSecondaryOP",StepNSecondaryOP,"StepNSecondaryOP[nStep]/I");
		}
	}
	if(PC -> GetParBool("OpTrack"))
	{
		T -> Branch("NOp",&NOp);
		T -> Branch("OpTrackID",&OpTrackID);
		T -> Branch("OpProcessID",&OpProcessID);
		T -> Branch("OpParentID",&OpParentID);
		T -> Branch("OpDetID",&OpDetID);
		T -> Branch("OpPX",&OpPX);
		T -> Branch("OpPY",&OpPY);
		T -> Branch("OpPZ",&OpPZ);
		T -> Branch("OpVX",&OpVX);
		T -> Branch("OpVY",&OpVY);
		T -> Branch("OpVZ",&OpVZ);
		T -> Branch("OpEnergy",&OpEnergy);
		T -> Branch("OpKEnergy",&OpKEnergy);
		T -> Branch("OpTime",&OpTime);
	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		T -> Branch("PostNOp",&PostNOp);
		T -> Branch("PostOpTrackID",&PostOpTrackID);
		T -> Branch("PostOpDetID",&PostOpDetID);
		T -> Branch("OpTrackLength",&OpTrackLength);
		T -> Branch("PostProcID",&PostProcID);
		T -> Branch("PostOpPX",&PostOpPX);
		T -> Branch("PostOpPY",&PostOpPY);
		T -> Branch("PostOpPZ",&PostOpPZ);
		T -> Branch("PostOpVX",&PostOpVX);
		T -> Branch("PostOpVY",&PostOpVY);
		T -> Branch("PostOpVZ",&PostOpVZ);
		T -> Branch("PostOpEnergy",&PostOpEnergy);
		T -> Branch("PostOpKEnergy",&PostOpKEnergy);
		T -> Branch("PostOpTime",&PostOpTime);
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		T -> Branch("NOpBoundary",&NOpBoundary);
		T -> Branch("OpTrackIDBoundary",&OpTrackIDBoundary);
		T -> Branch("OpProcIDBoundary",&OpProcIDBoundary);
		T -> Branch("OpPXBoundary",&OpPXBoundary);
		T -> Branch("OpPYBoundary",&OpPYBoundary);
		T -> Branch("OpPZBoundary",&OpPZBoundary);
		T -> Branch("OpVXBoundary",&OpVXBoundary);
		T -> Branch("OpVYBoundary",&OpVYBoundary);
		T -> Branch("OpVZBoundary",&OpVZBoundary);
		T -> Branch("OpTBoundary",&OpTBoundary);
	}
	if(PC->GetParBool("OpSiPM"))
	{
		T -> Branch("NOpSiPM",&NOpSiPM);
		T -> Branch("OpSiPMProcID",&OpSiPMProcID);
		T -> Branch("OpSiPMDetID",&OpSiPMDetID);
		T -> Branch("OpSiPMVX",&OpSiPMVX);
		T -> Branch("OpSiPMVY",&OpSiPMVY);
		T -> Branch("OpSiPMVZ",&OpSiPMVZ);
		T -> Branch("OpSiPMPX",&OpSiPMPX);
		T -> Branch("OpSiPMPY",&OpSiPMPY);
		T -> Branch("OpSiPMPZ",&OpSiPMPZ);
		T -> Branch("OpSiPMTime",&OpSiPMTime);
		T -> Branch("OpSiPMEnergy",&OpSiPMEnergy);
	}
}

void OpRunAction::BeginOfRunAction(const G4Run* run)
{
	nevnts = 0;
	if(PC -> GetParInt("UserVerbosity") > 0) 
	{
		G4cout << "Begin of OpRunAction" << G4endl;
	}
}

void OpRunAction::EndOfRunAction(const G4Run* run)
{
	nevnts = run -> GetNumberOfEvent();
//	SetInputParameters(nevnts);
	if(PC -> GetParInt("UserVerbosity") > 0) 
	{
		G4cout << "End of OpRunAction" << G4endl;
	}
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
		fill_n(StepFromHit,max_steps,0);
		fill_n(StepProcID,max_steps,0);
		fill_n(StepTrackPDG,max_steps,0);
		fill_n(StepPrevDetID,max_steps,0);
		fill_n(StepPostDetID,max_steps,0);
		fill_n(IsBoundary,max_steps,0);
		fill_n(StepPrevKE,max_steps,0);
		fill_n(StepVX,max_steps,0);
		fill_n(StepVY,max_steps,0);
		fill_n(StepVZ,max_steps,0);
		fill_n(StepEdep,max_steps,0);
		fill_n(StepLength,max_steps,0);
		fill_n(StepNSecondaryOP,max_steps,0);
	}
	if(PC->GetParBool("OpTrack"))
	{
		NOp = 0;
		OpTrackID.clear();
		OpProcessID.clear();
		OpParentID.clear();
		OpDetID.clear();
		OpPX.clear();
		OpPY.clear();
		OpPZ.clear();
		OpVX.clear();
		OpVY.clear();
		OpVZ.clear();
		OpEnergy.clear();
		OpKEnergy.clear();
		OpTime.clear();
	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		PostNOp = 0;
		PostOpDetID.clear();
		PostProcID.clear();
		OpTrackLength.clear();
		PostOpPX.clear();
		PostOpPY.clear();
		PostOpPZ.clear();
		PostOpVX.clear();
		PostOpVY.clear();
		PostOpVZ.clear();
		PostOpEnergy.clear();
		PostOpKEnergy.clear();
		PostOpTime.clear();
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		NOpBoundary = 0;
		OpTrackIDBoundary.clear();
		OpProcIDBoundary.clear();
		OpPXBoundary.clear();
		OpPYBoundary.clear();
		OpPZBoundary.clear();
		OpVXBoundary.clear();
		OpVYBoundary.clear();
		OpVZBoundary.clear();
		OpTBoundary.clear();
	}
	if(PC->GetParBool("OpSiPM"))
	{
		NOpSiPM = 0;
		OpSiPMProcID.clear();
		OpSiPMDetID.clear();
		OpSiPMVX.clear();
		OpSiPMVY.clear();
		OpSiPMVZ.clear();
		OpSiPMPX.clear();
		OpSiPMPY.clear();
		OpSiPMPZ.clear();
		OpSiPMTime.clear();
		OpSiPMEnergy.clear();
//		fill_n(OpSiPMProcID,max_opticalphotons,0);
//		fill_n(OpSiPMDetID,max_opticalphotons,0);
//		fill_n(OpSiPMVX,max_opticalphotons,0);
//		fill_n(OpSiPMVY,max_opticalphotons,0);
//		fill_n(OpSiPMVZ,max_opticalphotons,0);
//		fill_n(OpSiPMPX,max_opticalphotons,0);
//		fill_n(OpSiPMPY,max_opticalphotons,0);
//		fill_n(OpSiPMPZ,max_opticalphotons,0);
//		fill_n(OpSiPMTime,max_opticalphotons,0);
//		fill_n(OpSiPMEnergy,max_opticalphotons,0);
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
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int detID, G4ThreeVector p,
 G4ThreeVector v, G4double time, G4double energy, G4double kenergy, G4double length)
{
	if(opt == MCTrack)
	{
		OpTrackID.push_back(trkID);
		OpProcessID.push_back(procID);
		OpParentID.push_back(parentID);
		OpDetID.push_back(detID);
		OpPX.push_back(p.x());
		OpPY.push_back(p.y());
		OpPZ.push_back(p.z());
		OpVX.push_back(v.x());
		OpVY.push_back(v.y());
		OpVZ.push_back(v.z());
		OpTime.push_back(time);
		OpEnergy.push_back(energy);
		OpKEnergy.push_back(kenergy);
		NOp++;
	}
	else if (opt == MCPostTrack)
	{
//		G4int idx = find_OpIndex(trkID);
		PostOpTrackID.push_back(trkID);
		PostOpDetID.push_back(detID);
		OpTrackLength.push_back(length);
		PostProcID.push_back(procID);
		PostOpPX.push_back(p.x());
		PostOpPY.push_back(p.y());
		PostOpPZ.push_back(p.z());
		PostOpVX.push_back(v.x());
		PostOpVY.push_back(v.y());
		PostOpVZ.push_back(v.z());
		PostOpEnergy.push_back(energy);
		PostOpKEnergy.push_back(kenergy);
		PostOpTime.push_back(time);
		PostNOp++;
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
	OpTrackIDBoundary.push_back(trkID);
	OpProcIDBoundary.push_back(procID);
	OpPXBoundary.push_back(p.x());
	OpPYBoundary.push_back(p.y());
	OpPZBoundary.push_back(p.z());
	OpVXBoundary.push_back(v.x());
	OpVYBoundary.push_back(v.y());
	OpVZBoundary.push_back(v.z());
	OpTBoundary.push_back(t);
//	G4cout << t << G4endl;
	NOpBoundary++;
}


void OpRunAction::FillStep
(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep, G4double length, G4int nSecondaries, G4double prevKE)
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
	StepFromHit[nStep] = fromHit;
	StepTrackID[nStep] = trkID;
	StepProcID[nStep] = procID;
	StepTrackPDG[nStep] = pdg;
	StepPrevDetID[nStep] = prev_detID;
	StepPostDetID[nStep] = post_detID;
	IsBoundary[nStep] = boundary;
	StepPrevKE[nStep] = prevKE;
	StepVX[nStep] = v.x();
	StepVY[nStep] = v.y();
	StepVZ[nStep] = v.z();
	StepEdep[nStep] = edep;
	StepLength[nStep] = length;
	StepNSecondaryOP[nStep] = nSecondaries;
	nStep++;
}

void OpRunAction::FillSiPM
(G4int detID, G4int procID, G4String procName,
 G4ThreeVector pos, G4ThreeVector mom, G4double time, G4double energy)
{
	OpSiPMVX.push_back(pos.x());
	OpSiPMVY.push_back(pos.y());
	OpSiPMVZ.push_back(pos.z());
	OpSiPMPX.push_back(mom.x());
	OpSiPMPY.push_back(mom.y());
	OpSiPMPZ.push_back(mom.z());
	OpSiPMTime.push_back(time);
	OpSiPMEnergy.push_back(energy);
	NOpSiPM++;
	SetProcess(procID,procName);
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
		for(G4int a=0; a<NOp; a++)
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
}

