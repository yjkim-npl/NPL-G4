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
	// Load input parameters
	PC = OpParameterContainer::GetInstance();

	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Constructor of OpRunAction" << G4endl;

	// make output root file & tree 
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
	if(PC -> GetParInt("UserVerbosity") > 0)
	{
		G4cout << "#######################" << G4endl;
		G4cout << "outName: " << outName << G4endl;
		G4cout << "#######################" << G4endl << G4endl;
	}

	if(PC -> GetParBool("UserOutputName"))
		F = new TFile(PC -> GetParString("OutName").c_str(),"recreate");
	else
		F = new TFile(outName,"recreate");
	T = new TTree("Opsim","Opsim");

	init_Tree();
}

OpRunAction::~OpRunAction()
{
	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0) 
		G4cout << "Destructor of OpRunAction" << G4endl;

	// write all processes & parameters into output TList
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
	F -> cd();
//	delete F;
//	delete T;
}

// Link all variables into branch
void OpRunAction::init_Tree()
{
	if(PC -> GetParBool("MCTrack"))
	{
		T -> Branch("nTrack",&nTrack);
		T -> Branch("TrackID",&TrackID);
		T -> Branch("TrackProcID",&TrackProcID);
		T -> Branch("ParentID",&ParentID);
		T -> Branch("TrackPDG",&TrackPDG);
		T -> Branch("TrackDetID",&TrackDetID);
		T -> Branch("TrackPX",&TrackPX);
		T -> Branch("TrackPY",&TrackPY);
		T -> Branch("TrackPZ",&TrackPZ);
		T -> Branch("TrackVX",&TrackVX);
		T -> Branch("TrackVY",&TrackVY);
		T -> Branch("TrackVZ",&TrackVZ);
		T -> Branch("TrackEnergy",&TrackEnergy);
		T -> Branch("TrackKEnergy",&TrackKEnergy);
	}
	if(PC -> GetParBool("MCPostTrack"))
	{
		T -> Branch("nPostTrack",&nPostTrack);
		T -> Branch("PostTrackID",&PostTrackID);
		T -> Branch("PostTrackProcID",&PostTrackProcID);
		T -> Branch("PostTrackDetID",&PostTrackDetID);
		T -> Branch("PostTrackPX",&PostTrackPX);
		T -> Branch("PostTrackPY",&PostTrackPY);
		T -> Branch("PostTrackPZ",&PostTrackPZ);
		T -> Branch("PostTrackVX",&PostTrackVX);
		T -> Branch("PostTrackVY",&PostTrackVY);
		T -> Branch("PostTrackVZ",&PostTrackVZ);
		T -> Branch("PostTrackEnergy",&PostTrackEnergy);
		T -> Branch("PostTrackKEnergy",&PostTrackKEnergy);

	}
	if(PC -> GetParBool("MCStep"))
	{
		T -> Branch("nStep",&nStep);
		T -> Branch("StepTrackID"  ,&StepTrackID);
		T -> Branch("StepFromHit"  ,&StepFromHit);
		T -> Branch("StepProcID"   ,&StepProcID);
		T -> Branch("StepTrackPDG" ,&StepTrackPDG);
		T -> Branch("StepPrevDetID",&StepPrevDetID);
		T -> Branch("StepPostDetID",&StepPostDetID);
		T -> Branch("IsBoundary"   ,&IsBoundary);

		T -> Branch("StepVX"       ,&StepVX); 
		T -> Branch("StepVY"       ,&StepVY);
		T -> Branch("StepVZ"       ,&StepVZ);
		T -> Branch("StepEdep"     ,&StepEdep);
		T -> Branch("StepLength"   ,&StepLength);
		T -> Branch("StepPrevKE"   ,&StepPrevKE);
		if(PC->GetParBool("OpticalPhysics"))
		{
			T -> Branch("StepNSecondaryOP",&StepNSecondaryOP);
		}
	}
	if(PC -> GetParBool("OpTrack"))
	{
		T -> Branch("NOp",&NOp);
		T -> Branch("OpPX",OpPX,"OpPX[NOp]/D");
		T -> Branch("OpPY",OpPY,"OpPY[NOp]/D");
		T -> Branch("OpPZ",OpPZ,"OpPZ[NOp]/D");
//		T -> Branch("OpTrackID",&OpTrackID);
//		T -> Branch("OpProcessID",&OpProcessID);
//		T -> Branch("OpParentID",&OpParentID);
//		T -> Branch("OpDetID",&OpDetID);
//		T -> Branch("OpPX",&OpPX);
//		T -> Branch("OpPY",&OpPY);
//		T -> Branch("OpPZ",&OpPZ);
//		T -> Branch("OpVX",&OpVX);
//		T -> Branch("OpVY",&OpVY);
//		T -> Branch("OpVZ",&OpVZ);
//		T -> Branch("OpEnergy",&OpEnergy);
//		T -> Branch("OpKEnergy",&OpKEnergy);
//		T -> Branch("OpTime",&OpTime);

	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		T -> Branch("PostNOp",&PostNOp);
		T -> Branch("PostProcID",PostProcID,"PostProcID[PostNOp]/I");
		T -> Branch("PostOpPX",PostOpPX,"PostOpPX[PostNOp]/D");
		T -> Branch("PostOpPY",PostOpPY,"PostOpPY[PostNOp]/D");
		T -> Branch("PostOpPZ",PostOpPZ,"PostOpPZ[PostNOp]/D");
//		T -> Branch("PostOpTrackID",&PostOpTrackID);
//		T -> Branch("PostOpDetID",&PostOpDetID);
//		T -> Branch("PostProcID",&PostProcID);
//		T -> Branch("OpTrackLength",&OpTrackLength);
//		T -> Branch("PostOpPX",&PostOpPX);
//		T -> Branch("PostOpPY",&PostOpPY);
//		T -> Branch("PostOpPZ",&PostOpPZ);
//		T -> Branch("PostOpVX",&PostOpVX);
//		T -> Branch("PostOpVY",&PostOpVY);
//		T -> Branch("PostOpVZ",&PostOpVZ);
//		T -> Branch("PostOpEnergy",&PostOpEnergy);
//		T -> Branch("PostOpKEnergy",&PostOpKEnergy);
//		T -> Branch("PostOpTime",&PostOpTime);
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

//		T -> Branch("OpTrackIDBoundary",&OpTrackIDBoundary);
//		T -> Branch("OpProcIDBoundary",&OpProcIDBoundary);
//		T -> Branch("OpPXBoundary",&OpPXBoundary);
//		T -> Branch("OpPYBoundary",&OpPYBoundary);
//		T -> Branch("OpPZBoundary",&OpPZBoundary);
//		T -> Branch("OpVXBoundary",&OpVXBoundary);
//		T -> Branch("OpVYBoundary",&OpVYBoundary);
//		T -> Branch("OpVZBoundary",&OpVZBoundary);
//		T -> Branch("OpTBoundary",&OpTBoundary);
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
	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0) 
	{
		G4cout << "Begin of OpRunAction" << G4endl;
	}
}

void OpRunAction::EndOfRunAction(const G4Run* run)
{
	nevnts = run -> GetNumberOfEvent();

	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0) 
	{
		G4cout << "End of OpRunAction" << G4endl;
	}
}

void OpRunAction::clear_data()
{
	if(PC->GetParBool("MCTrack"))
	{
		nTrack = 0;
		TrackID.clear();
		TrackProcID.clear();
		ParentID.clear();
		TrackPDG.clear();
		TrackDetID.clear();
		TrackPX.clear();
		TrackPY.clear();
		TrackPZ.clear();
		TrackVX.clear();
		TrackVY.clear();
		TrackVZ.clear();
		TrackEnergy.clear();
		TrackKEnergy.clear();
	}
	if(PC->GetParBool("MCPostTrack"))
	{
		nPostTrack = 0;
		PostTrackID.clear();
		PostTrackProcID.clear();
		PostTrackPDG.clear();
		PostTrackDetID.clear();
		PostTrackPX.clear();
		PostTrackPY.clear();
		PostTrackPZ.clear();
		PostTrackVX.clear();
		PostTrackVY.clear();
		PostTrackVZ.clear();
		PostTrackEnergy.clear();
		PostTrackKEnergy.clear();
	}
	if(PC->GetParBool("MCStep"))
	{
		nStep = 0;
		StepTrackID.clear();
		StepFromHit.clear();
		StepProcID.clear();
		StepTrackPDG.clear();
		StepPrevDetID.clear();
		StepPostDetID.clear();
		IsBoundary.clear();
		StepPrevKE.clear();
		StepVX.clear();
		StepVY.clear();
		StepVZ.clear();
		StepEdep.clear();
		StepLength.clear();
		StepNSecondaryOP.clear();
	}
	if(PC->GetParBool("OpTrack"))
	{
		NOp = 0;
		fill_n(OpPX,1000,0);
		fill_n(OpPY,1000,0);
		fill_n(OpPZ,1000,0);
//		OpTrackID.clear();
//		OpProcessID.clear();
//		OpParentID.clear();
//		OpDetID.clear();
//		OpPX.clear();
//		OpPY.clear();
//		OpPZ.clear();
//		OpVX.clear();
//		OpVY.clear();
//		OpVZ.clear();
//		OpEnergy.clear();
//		OpKEnergy.clear();
//		OpTime.clear();

	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		PostNOp = 0;
		fill_n(PostProcID,1000,0);
		fill_n(PostOpPX,1000,0);
		fill_n(PostOpPY,1000,0);
		fill_n(PostOpPZ,1000,0);
//		PostOpDetID.clear();
//		PostProcID.clear();
//		OpTrackLength.clear();
//		PostOpPX.clear();
//		PostOpPY.clear();
//		PostOpPZ.clear();
//		PostOpVX.clear();
//		PostOpVY.clear();
//		PostOpVZ.clear();
//		PostOpEnergy.clear();
//		PostOpKEnergy.clear();
//		PostOpTime.clear();
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		NOpBoundary = 0;
		fill_n(OpTrackIDBoundary,1000,0);
		fill_n(OpProcIDBoundary,1000,0);
		fill_n(OpPXBoundary,1000,0);
		fill_n(OpPYBoundary,1000,0);
		fill_n(OpPZBoundary,1000,0);
		fill_n(OpVXBoundary,1000,0);
		fill_n(OpVYBoundary,1000,0);
		fill_n(OpVZBoundary,1000,0);
		fill_n(OpTBoundary,1000,0);

//		OpTrackIDBoundary.clear();
//		OpProcIDBoundary.clear();
//		OpPXBoundary.clear();
//		OpPYBoundary.clear();
//		OpPZBoundary.clear();
//		OpVXBoundary.clear();
//		OpVYBoundary.clear();
//		OpVZBoundary.clear();
//		OpTBoundary.clear();
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
	}
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
	if(opt == MCTrack)	// data at the time of creation
	{
		TrackID.push_back(trkID);
		TrackProcID.push_back(procID);
		ParentID.push_back(parentID);
		TrackPDG.push_back(pdg);
		TrackDetID.push_back(detID);
		TrackPX.push_back(p.x());
		TrackPY.push_back(p.y());
		TrackPZ.push_back(p.z());
		TrackVX.push_back(v.x());
		TrackVY.push_back(v.y());
		TrackVZ.push_back(v.z());
		TrackEnergy.push_back(totenergy);
		TrackKEnergy.push_back(kinenergy);
		nTrack++;
	}
	else if(opt == MCPostTrack)	// data at the time of died
	{
		PostTrackID.push_back(trkID);
		PostTrackProcID.push_back(procID);
		PostTrackDetID.push_back(detID);
		PostTrackPX.push_back(p.x());
		PostTrackPY.push_back(p.y());
		PostTrackPZ.push_back(p.z());
		PostTrackVX.push_back(v.x());
		PostTrackVY.push_back(v.y());
		PostTrackVZ.push_back(v.z());
		PostTrackEnergy.push_back(totenergy);
		PostTrackKEnergy.push_back(kinenergy);
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
		OpPX[NOp] = p.x();
		OpPY[NOp] = p.y();
		OpPZ[NOp] = p.z();
//		OpTrackID.push_back(trkID);
//		OpProcessID.push_back(procID);
//		OpParentID.push_back(parentID);
//		OpDetID.push_back(detID);
//		OpPX.push_back(p.x());
//		OpPY.push_back(p.y());
//		OpPZ.push_back(p.z());
//		OpVX.push_back(v.x());
//		OpVY.push_back(v.y());
//		OpVZ.push_back(v.z());
//		OpTime.push_back(time);
//		OpEnergy.push_back(energy);
//		OpKEnergy.push_back(kenergy);

		NOp++;
	}
	else if (opt == MCPostTrack)
	{
		PostProcID[PostNOp] = procID;
		PostOpPX[PostNOp] = p.x();
		PostOpPY[PostNOp] = p.y();
		PostOpPZ[PostNOp] = p.z();
//		PostOpTrackID.push_back(trkID);
//		PostOpDetID.push_back(detID);
//		OpTrackLength.push_back(length);
//		PostProcID.push_back(procID);
//		PostOpPX.push_back(p.x());
//		PostOpPY.push_back(p.y());
//		PostOpPZ.push_back(p.z());
//		PostOpVX.push_back(v.x());
//		PostOpVY.push_back(v.y());
//		PostOpVZ.push_back(v.z());
//		PostOpEnergy.push_back(energy);
//		PostOpKEnergy.push_back(kenergy);
//		PostOpTime.push_back(time);
		PostNOp++;
	}
}

void OpRunAction::FillOpticalPhotonBoundary
(G4int trkID, G4int procID, G4ThreeVector v, G4ThreeVector p, G4double t)
{
	if(procID == -4) // procID 2 is `total internal reflection`
		return;
	OpTrackIDBoundary[NOpBoundary] = trkID;
	OpProcIDBoundary[NOpBoundary] = procID;
	OpPXBoundary[NOpBoundary] = p.x();
	OpPYBoundary[NOpBoundary] = p.y();
	OpPZBoundary[NOpBoundary] = p.z();
	OpVXBoundary[NOpBoundary] = v.x();
	OpVYBoundary[NOpBoundary] = v.y();
	OpVZBoundary[NOpBoundary] = v.z();
	OpTBoundary[NOpBoundary] = t;
//	OpTrackIDBoundary.push_back(trkID);
//	OpProcIDBoundary.push_back(procID);
//	OpPXBoundary.push_back(p.x());
//	OpPYBoundary.push_back(p.y());
//	OpPZBoundary.push_back(p.z());
//	OpVXBoundary.push_back(v.x());
//	OpVYBoundary.push_back(v.y());
//	OpVZBoundary.push_back(v.z());
//	OpTBoundary.push_back(t);
	NOpBoundary++;
}


void OpRunAction::FillStep
(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep, G4double length, G4int nSecondaries, G4double prevKE)
{
	StepFromHit.push_back(fromHit);
	StepTrackID.push_back(trkID);
	StepProcID.push_back(procID);
	StepTrackPDG.push_back(pdg);
	StepPrevDetID.push_back(prev_detID);
	StepPostDetID.push_back(post_detID);
	IsBoundary.push_back(boundary);
	StepPrevKE.push_back(prevKE);
	StepVX.push_back(v.x());
	StepVY.push_back(v.y());
	StepVZ.push_back(v.z());
	StepEdep.push_back(edep);
	StepLength.push_back(length);
	StepNSecondaryOP.push_back(nSecondaries);
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

void OpRunAction::update_Tree()
{
	T -> Fill();
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
//			G4cout << a << " OptrckID: " << OpTrackID[a] <<
//				" ProcID: " << OpProcessID[a] <<
//				" ParID: " << OpParentID[a] <<
//				" DetID: " << OpDetID[a] <<
//				" PZ: " << OpPZ[a] << 
//				" VX: " << (G4float)OpVX[a] << 
//				" VY: " << (G4float)OpVY[a] << 
//				" Time: " << (G4float)OpTime[a] << 
//				" PostDetID: " << PostOpDetID[a] << 
//				" PostProcID: " << PostProcID[a] << 
//				" pPZ: " << PostOpPZ[a] << 
//				" pVX: " << PostOpVX[a] << 
//				" pVY: " << PostOpVY[a] << 
//				" pTime: " << PostOpTime[a] << 
//				G4endl;
		}
	}
	else if (opt == Process)
	{
		auto procNames = G4ProcessTable::GetProcessTable() -> GetNameList();
		G4cout << "###############" << G4endl;
		G4cout << "Process Table" << G4endl;
		G4int idx = 0;
		for(auto name : *procNames)
		{
			G4cout << idx << " " << name << G4endl;
			idx++;
		}
		G4cout << "###############" << G4endl;
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

