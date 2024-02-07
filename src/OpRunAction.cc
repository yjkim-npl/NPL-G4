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
	if(PC -> GetParBool("VectorOutput"))
	{
		if(PC -> GetParBool("MCTrack"))
		{
			T -> Branch("nTrack",&nTrack);
			T -> Branch("TrackID",&v_TrackID);
			T -> Branch("TrackProcID",&v_TrackProcID);
			T -> Branch("ParentID",&v_ParentID);
			T -> Branch("TrackPDG",&v_TrackPDG);
			T -> Branch("TrackDetID",&v_TrackDetID);
			T -> Branch("TrackPX",&v_TrackPX);
			T -> Branch("TrackPY",&v_TrackPY);
			T -> Branch("TrackPZ",&v_TrackPZ);
			T -> Branch("TrackVX",&v_TrackVX);
			T -> Branch("TrackVY",&v_TrackVY);
			T -> Branch("TrackVZ",&v_TrackVZ);
			T -> Branch("TrackTime",&v_TrackTime);
			T -> Branch("TrackEnergy",&v_TrackEnergy);
			T -> Branch("TrackKEnergy",&v_TrackKEnergy);
		}
		if(PC -> GetParBool("MCPostTrack"))
		{
			T -> Branch("nPostTrack",&nPostTrack);
			T -> Branch("PostTrackID",&v_PostTrackID);
			T -> Branch("PostTrackProcID",&v_PostTrackProcID);
			T -> Branch("PostTrackDetID",&v_PostTrackDetID);
			T -> Branch("PostTrackPX",&v_PostTrackPX);
			T -> Branch("PostTrackPY",&v_PostTrackPY);
			T -> Branch("PostTrackPZ",&v_PostTrackPZ);
			T -> Branch("PostTrackVX",&v_PostTrackVX);
			T -> Branch("PostTrackVY",&v_PostTrackVY);
			T -> Branch("PostTrackVZ",&v_PostTrackVZ);
			T -> Branch("PostTrackTime",&v_PostTrackTime);
			T -> Branch("PostTrackEnergy",&v_PostTrackEnergy);
			T -> Branch("PostTrackKEnergy",&v_PostTrackKEnergy);

		}
		if(PC -> GetParBool("MCStep"))
		{
			T -> Branch("nStep",&nStep);
			T -> Branch("StepTrackID"  ,&v_StepTrackID);
			T -> Branch("StepFromHit"  ,&v_StepFromHit);
			T -> Branch("StepProcID"   ,&v_StepProcID);
			T -> Branch("StepTrackPDG" ,&v_StepTrackPDG);
			T -> Branch("StepPrevDetID",&v_StepPrevDetID);
			T -> Branch("StepPostDetID",&v_StepPostDetID);
			T -> Branch("IsBoundary"   ,&v_IsBoundary);

			T -> Branch("StepVX"       ,&v_StepVX); 
			T -> Branch("StepVY"       ,&v_StepVY);
			T -> Branch("StepVZ"       ,&v_StepVZ);
			T -> Branch("StepTime"     ,&v_StepTime);
			T -> Branch("StepEdep"     ,&v_StepEdep);
			T -> Branch("StepLength"   ,&v_StepLength);
			T -> Branch("StepPrevKE"   ,&v_StepPrevKE);
			if(PC->GetParBool("OpticalPhysics"))
			{
				T -> Branch("StepNSecondaryOP",&v_StepNSecondaryOP);
			}
		}
	}else{
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
			T -> Branch("TrackTime",TrackTime,"TrackTime[nTrack]/D");
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
			T -> Branch("PostTrackTime",PostTrackTime,"PostTrackTime[nPostTrack]/D");
			T -> Branch("PostTrackEnergy",PostTrackEnergy,"PostTrackEnergy[nPostTrack]/D");
			T -> Branch("PostTrackKEnergy",PostTrackKEnergy,"PostTrackKEnergy[nPostTrack]/D");

		}
		if(PC -> GetParBool("MCStep"))
		{
			T -> Branch("nStep",&nStep);
			T -> Branch("StepTrackID"  ,StepTrackID,  "StepTrackID[nStep]/I");
			T -> Branch("StepFromHit"  ,StepFromHit,  "StepFromHit[nStep]/I");
			T -> Branch("StepProcID"   ,StepProcID,   "StepProcID[nStep]/I");
			T -> Branch("StepTrackPDG" ,StepTrackPDG, "StepTrackPDG[nStep]/I");
			T -> Branch("StepPrevDetID",StepPrevDetID,"StepPrevDetID[nStep]/I");
			T -> Branch("StepPostDetID",StepPostDetID,"StepPostDetID[nStep]/I");
			T -> Branch("IsBoundary"   ,IsBoundary,   "IsBoundary[nStep]/I");

			T -> Branch("StepVX"       ,StepVX,    "SetpVX[nStep]/D"); 
			T -> Branch("StepVY"       ,StepVY,    "SetpVY[nStep]/D");
			T -> Branch("StepVZ"       ,StepVZ,    "SetpVZ[nStep]/D");
			T -> Branch("StepTime"     ,StepTime,  "StepTime[nStep]/D");
			T -> Branch("StepEdep"     ,StepEdep,  "StepEdep[nStep]/D");
			T -> Branch("StepLength"   ,StepLength,"StepLength[nStep]/D");
			T -> Branch("StepPrevKE"   ,StepPrevKE,"StepPrevKE[nStep]/D");
			if(PC->GetParBool("OpticalPhysics"))
			{
				T -> Branch("StepNSecondaryOP",StepNSecondaryOP,"StepNSecondaryOp[nStep]/I");
			}
		}
	}
	if(PC -> GetParBool("OpTrack"))
	{
		T -> Branch("NOp",&NOp);
//		T -> Branch("OpPX",OpPX,"OpPX[NOp]/D");
//		T -> Branch("OpPY",OpPY,"OpPY[NOp]/D");
//		T -> Branch("OpPZ",OpPZ,"OpPZ[NOp]/D");
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
//		T -> Branch("PostProcID",PostProcID,"PostProcID[PostNOp]/I");
//		T -> Branch("PostOpPX",PostOpPX,"PostOpPX[PostNOp]/D");
//		T -> Branch("PostOpPY",PostOpPY,"PostOpPY[PostNOp]/D");
//		T -> Branch("PostOpPZ",PostOpPZ,"PostOpPZ[PostNOp]/D");
		T -> Branch("PostOpTrackID",&PostOpTrackID);
		T -> Branch("PostOpDetID",&PostOpDetID);
		T -> Branch("PostProcID",&PostProcID);
		T -> Branch("OpTrackLength",&OpTrackLength);
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
//		T -> Branch("OpTrackIDBoundary",OpTrackIDBoundary,"OpTrackIDBoundary[NOpBoundary]/I");
//		T -> Branch("OpProcIDBoundary",OpProcIDBoundary,"OpProcIDBoundary[NOpBoundary]/I");
//		T -> Branch("OpPXBoundary",OpPXBoundary,"OpPXBoundary[NOpBoundary]/D");
//		T -> Branch("OpPYBoundary",OpPYBoundary,"OpPYBoundary[NOpBoundary]/D");
//		T -> Branch("OpPZBoundary",OpPZBoundary,"OpPZBoundary[NOpBoundary]/D");
//		T -> Branch("OpVXBoundary",OpVXBoundary,"OpVXBoundary[NOpBoundary]/D");
//		T -> Branch("OpVYBoundary",OpVYBoundary,"OpVYBoundary[NOpBoundary]/D");
//		T -> Branch("OpVZBoundary",OpVZBoundary,"OpVZBoundary[NOpBoundary]/D");
//		T -> Branch("OpTBoundary",OpTBoundary,"OpTBoundary[NOpBoundary]/D");

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
	stopwatch.Start();
	// verbosity
	if(PC -> GetParInt("UserVerbosity") > 0) 
	{
		G4cout << "Begin of OpRunAction" << G4endl;
	}
}

void OpRunAction::EndOfRunAction(const G4Run* run)
{
	nevnts = run -> GetNumberOfEvent();
	stopwatch.Print("m");

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
		v_TrackID.clear();
		v_TrackProcID.clear();
		v_ParentID.clear();
		v_TrackPDG.clear();
		v_TrackDetID.clear();
		v_TrackPX.clear();
		v_TrackPY.clear();
		v_TrackPZ.clear();
		v_TrackVX.clear();
		v_TrackVY.clear();
		v_TrackVZ.clear();
		v_TrackTime.clear();
		v_TrackEnergy.clear();
		v_TrackKEnergy.clear();

		fill_n(TrackID,max_t,0);
		fill_n(TrackProcID,max_t,0);
		fill_n(ParentID,max_t,0);
		fill_n(TrackPDG,max_t,0);
		fill_n(TrackDetID,max_t,0);
		fill_n(TrackPX,max_t,0);
		fill_n(TrackPY,max_t,0);
		fill_n(TrackPZ,max_t,0);
		fill_n(TrackVX,max_t,0);
		fill_n(TrackVY,max_t,0);
		fill_n(TrackVZ,max_t,0);
		fill_n(TrackTime,max_t,0);
		fill_n(TrackEnergy,max_t,0);
		fill_n(TrackKEnergy,max_t,0);
	}
	if(PC->GetParBool("MCPostTrack"))
	{
		nPostTrack = 0;
		v_PostTrackID.clear();
		v_PostTrackProcID.clear();
		v_PostTrackPDG.clear();
		v_PostTrackDetID.clear();
		v_PostTrackPX.clear();
		v_PostTrackPY.clear();
		v_PostTrackPZ.clear();
		v_PostTrackVX.clear();
		v_PostTrackVY.clear();
		v_PostTrackVZ.clear();
		v_PostTrackTime.clear();
		v_PostTrackEnergy.clear();
		v_PostTrackKEnergy.clear();

		fill_n(PostTrackID,max_t,0);
		fill_n(PostTrackProcID,max_t,0);
		fill_n(PostTrackPDG,max_t,0);
		fill_n(PostTrackDetID,max_t,0);
		fill_n(PostTrackPX,max_t,0);
		fill_n(PostTrackPY,max_t,0);
		fill_n(PostTrackPZ,max_t,0);
		fill_n(PostTrackVX,max_t,0);
		fill_n(PostTrackVY,max_t,0);
		fill_n(PostTrackVZ,max_t,0);
		fill_n(PostTrackTime,max_t,0);
		fill_n(PostTrackEnergy,max_t,0);
		fill_n(PostTrackKEnergy,max_t,0);
	}
	if(PC->GetParBool("MCStep"))
	{
		nStep = 0;
		v_StepTrackID.clear();
		v_StepFromHit.clear();
		v_StepProcID.clear();
		v_StepTrackPDG.clear();
		v_StepPrevDetID.clear();
		v_StepPostDetID.clear();
		v_IsBoundary.clear();
		v_StepPrevKE.clear();
		v_StepVX.clear();
		v_StepVY.clear();
		v_StepVZ.clear();
		v_StepTime.clear();
		v_StepEdep.clear();
		v_StepLength.clear();
		v_StepNSecondaryOP.clear();
		fill_n(StepTrackID,max_s,0);
		fill_n(StepFromHit,max_s,0);
		fill_n(StepProcID,max_s,0);
		fill_n(StepTrackPDG,max_s,0);
		fill_n(StepPrevDetID,max_s,0);
		fill_n(StepPostDetID,max_s,0);
		fill_n(IsBoundary,max_s,0);
		fill_n(StepPrevKE,max_s,0);
		fill_n(StepVX,max_s,0);
		fill_n(StepVY,max_s,0);
		fill_n(StepVZ,max_s,0);
		fill_n(StepTime,max_s,0);
		fill_n(StepEdep,max_s,0);
		fill_n(StepLength,max_s,0);
		fill_n(StepNSecondaryOP,max_s,0);
	}
	if(PC->GetParBool("OpTrack"))
	{
		NOp = 0;
//		fill_n(OpPX,1000,0);
//		fill_n(OpPY,1000,0);
//		fill_n(OpPZ,1000,0);
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
//		fill_n(PostProcID,1000,0);
//		fill_n(PostOpPX,1000,0);
//		fill_n(PostOpPY,1000,0);
//		fill_n(PostOpPZ,1000,0);
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
//		fill_n(OpTrackIDBoundary,1000,0);
//		fill_n(OpProcIDBoundary,1000,0);
//		fill_n(OpPXBoundary,1000,0);
//		fill_n(OpPYBoundary,1000,0);
//		fill_n(OpPZBoundary,1000,0);
//		fill_n(OpVXBoundary,1000,0);
//		fill_n(OpVYBoundary,1000,0);
//		fill_n(OpVZBoundary,1000,0);
//		fill_n(OpTBoundary,1000,0);

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
	}
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double time, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
	if(PC->GetParBool("VectorOutput"))
	{
		if(opt == MCTrack)	// data at the time of creation
		{
			v_TrackID.push_back(trkID);
			v_TrackProcID.push_back(procID);
			v_ParentID.push_back(parentID);
			v_TrackPDG.push_back(pdg);
			v_TrackDetID.push_back(detID);
			v_TrackPX.push_back(p.x());
			v_TrackPY.push_back(p.y());
			v_TrackPZ.push_back(p.z());
			v_TrackVX.push_back(v.x());
			v_TrackVY.push_back(v.y());
			v_TrackVZ.push_back(v.z());
			v_TrackTime.push_back(time);
			v_TrackEnergy.push_back(totenergy);
			v_TrackKEnergy.push_back(kinenergy);
			nTrack++;
		}
		else if(opt == MCPostTrack)	// data at the time of died
		{
			v_PostTrackID.push_back(trkID);
			v_PostTrackProcID.push_back(procID);
			v_PostTrackDetID.push_back(detID);
			v_PostTrackPX.push_back(p.x());
			v_PostTrackPY.push_back(p.y());
			v_PostTrackPZ.push_back(p.z());
			v_PostTrackVX.push_back(v.x());
			v_PostTrackVY.push_back(v.y());
			v_PostTrackVZ.push_back(v.z());
			v_PostTrackTime.push_back(time);
			v_PostTrackEnergy.push_back(totenergy);
			v_PostTrackKEnergy.push_back(kinenergy);
			nPostTrack++;
		}
	}else{
		if(opt == MCTrack)	// data at the time of creation
		{
			TrackID[nTrack]=trkID;
			TrackProcID[nTrack]=procID;
			ParentID[nTrack]=parentID;
			TrackPDG[nTrack]=pdg;
			TrackDetID[nTrack]=detID;
			TrackPX[nTrack]=p.x();
			TrackPY[nTrack]=p.y();
			TrackPZ[nTrack]=p.z();
			TrackVX[nTrack]=v.x();
			TrackVY[nTrack]=v.y();
			TrackVZ[nTrack]=v.z();
			TrackTime[nTrack]=time;
			TrackEnergy[nTrack]=totenergy;
			TrackKEnergy[nTrack]=kinenergy;
			nTrack++;
		}
		else if(opt == MCPostTrack)	// data at the time of died
		{
			PostTrackID[nPostTrack]=trkID;
			PostTrackProcID[nPostTrack]=procID;
			PostTrackDetID[nPostTrack]=detID;
			PostTrackPX[nPostTrack]=p.x();
			PostTrackPY[nPostTrack]=p.y();
			PostTrackPZ[nPostTrack]=p.z();
			PostTrackVX[nPostTrack]=v.x();
			PostTrackVY[nPostTrack]=v.y();
			PostTrackVZ[nPostTrack]=v.z();
			PostTrackTime[nPostTrack]=time;
			PostTrackEnergy[nPostTrack]=totenergy;
			PostTrackKEnergy[nPostTrack]=kinenergy;
			nPostTrack++;
		}
	}
}

void OpRunAction::FillOpticalPhoton
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int detID, G4ThreeVector p,
 G4ThreeVector v, G4double time, G4double energy, G4double kenergy, G4double length)
{
	if(opt == MCTrack)
	{
//		OpPX[NOp] = p.x();
//		OpPY[NOp] = p.y();
//		OpPZ[NOp] = p.z();
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
//		PostProcID[PostNOp] = procID;
//		PostOpPX[PostNOp] = p.x();
//		PostOpPY[PostNOp] = p.y();
//		PostOpPZ[PostNOp] = p.z();
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
(G4int trkID, G4int procID, G4ThreeVector v, G4ThreeVector p, G4double t)
{
	if(procID == -4) // procID 2 is `total internal reflection`
		return;
//	OpTrackIDBoundary[NOpBoundary] = trkID;
//	OpProcIDBoundary[NOpBoundary] = procID;
//	OpPXBoundary[NOpBoundary] = p.x();
//	OpPYBoundary[NOpBoundary] = p.y();
//	OpPZBoundary[NOpBoundary] = p.z();
//	OpVXBoundary[NOpBoundary] = v.x();
//	OpVYBoundary[NOpBoundary] = v.y();
//	OpVZBoundary[NOpBoundary] = v.z();
//	OpTBoundary[NOpBoundary] = t;
	OpTrackIDBoundary.push_back(trkID);
	OpProcIDBoundary.push_back(procID);
	OpPXBoundary.push_back(p.x());
	OpPYBoundary.push_back(p.y());
	OpPZBoundary.push_back(p.z());
	OpVXBoundary.push_back(v.x());
	OpVYBoundary.push_back(v.y());
	OpVZBoundary.push_back(v.z());
	OpTBoundary.push_back(t);
	NOpBoundary++;
}


void OpRunAction::FillStep
(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double time, G4double edep, G4double length, G4int nSecondaries, G4double prevKE)
{
	if(PC->GetParBool("VectorOutput"))
	{
		v_StepFromHit.push_back(fromHit);
		v_StepTrackID.push_back(trkID);
		v_StepProcID.push_back(procID);
		v_StepTrackPDG.push_back(pdg);
		v_StepPrevDetID.push_back(prev_detID);
		v_StepPostDetID.push_back(post_detID);
		v_IsBoundary.push_back(boundary);
		v_StepPrevKE.push_back(prevKE);
		v_StepVX.push_back(v.x());
		v_StepVY.push_back(v.y());
		v_StepVZ.push_back(v.z());
		v_StepTime.push_back(time);
		v_StepEdep.push_back(edep);
		v_StepLength.push_back(length);
		v_StepNSecondaryOP.push_back(nSecondaries);
		nStep++;
	}else{
		StepFromHit[nStep]=fromHit;
		StepTrackID[nStep]=trkID;
		StepProcID[nStep]=procID;
		StepTrackPDG[nStep]=pdg;
		StepPrevDetID[nStep]=prev_detID;
		StepPostDetID[nStep]=post_detID;
		IsBoundary[nStep]=boundary;
		StepPrevKE[nStep]=prevKE;
		StepVX[nStep]=v.x();
		StepVY[nStep]=v.y();
		StepVZ[nStep]=v.z();
		StepTime[nStep]=time;
		StepEdep[nStep]=edep;
		StepLength[nStep]=length;
		StepNSecondaryOP[nStep]=nSecondaries;
		nStep++;
	}
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

