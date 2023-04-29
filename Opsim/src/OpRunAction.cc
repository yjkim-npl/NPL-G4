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

		T -> Branch("StepVX",StepVX,"StepVX[nStep]/D");
		T -> Branch("StepVY",StepVY,"StepVY[nStep]/D");
		T -> Branch("StepVZ",StepVZ,"StepVZ[nStep]/D");
		T -> Branch("StepEdep",StepEdep,"StepEdep[nStep]/D");
		T -> Branch("StepLength",StepLength,"StepLength[nStep]/D");
		T -> Branch("StepPrevKE",StepPrevKE,"StepPrevKE[nStep]/D");
		if(PC->GetParBool("OpticalPhysics"))
		{
			T -> Branch("StepNSecondaryOP",StepNSecondaryOP,"StepNSecondaryOP[nStep]/I");
		}
	}
	if(PC -> GetParBool("OpTrack"))
	{
		T -> Branch("NOp",&NOp);

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

		T -> Branch("OpTrackID",OpTrackID,"OpTrackID[NOp]/I");
		T -> Branch("OpProcessID",OpProcessID,"OpProcessID[NOp]/I");
		T -> Branch("OpParentID",OpParentID,"OpParentID[NOp]/I");
		T -> Branch("OpDetID",OpDetID,"OpDetID[NOp]/I");
		T -> Branch("OpPX",OpPX,"OpPX[NOp]/D");
		T -> Branch("OpPY",OpPY,"OpPY[NOp]/D");
		T -> Branch("OpPZ",OpPZ,"OpPZ[NOp]/D");
		T -> Branch("OpVX",OpVX,"OpVX[NOp]/D");
		T -> Branch("OpVY",OpVY,"OpVY[NOp]/D");
		T -> Branch("OpVZ",OpVZ,"OpVZ[NOp]/D");
		T -> Branch("OpEnergy",OpEnergy,"OpEnergy[NOp]/D");
		T -> Branch("OpKEnergy",OpKEnergy,"OpKEnergy[NOp]/D");
		T -> Branch("OpTime",OpTime,"OpTime[NOp]/D");
	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		T -> Branch("PostNOp",&PostNOp);
		T -> Branch("PostOpTrackID",PostOpTrackID,"PostOpTrackID[PostNOp]/I");
		T -> Branch("PostOpDetID",PostOpDetID,"PostOpDetID[PostNOp]/I");
		T -> Branch("PostProcID",PostProcID,"PostProcID[PostNOp]/I");
		T -> Branch("OpTrackLength",OpTrackLength,"OpTrackLength[PostNOp]/D");
		T -> Branch("PostOpPX",PostOpPX,"PostOpPX[PostNOp]/D");
		T -> Branch("PostOpPY",PostOpPY,"PostOpPY[PostNOp]/D");
		T -> Branch("PostOpPZ",PostOpPZ,"PostOpPZ[PostNOp]/D");
		T -> Branch("PostOpVX",PostOpVX,"PostOpVX[PostNOp]/D");
		T -> Branch("PostOpVY",PostOpVY,"PostOpVY[PostNOp]/D");
		T -> Branch("PostOpVZ",PostOpVZ,"PostOpVZ[PostNOp]/D");
		T -> Branch("PostOpEnergy",PostOpEnergy,"PostOpEnergy[PostNOp]/D");
		T -> Branch("PostOpKEnergy",PostOpKEnergy,"PostOpKEnergy[PostNOp]/D");
		T -> Branch("PostOpTime",PostOpTime,"PostOpTime[PostNOp]/D");
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

//		T -> Branch("OpTrackIDBoundary",OpTrackIDBoundary,"OpTrackIDBoundary[NOpBoundary]/I");
//		T -> Branch("OpProcIDBoundary",OpProcIDBoundary,"OpProcIDBoundary[NOpBoundary]/I");
//		T -> Branch("OpPXBoundary",OpPXBoundary,"OpPXBoundary[NOpBoundary]/D");
//		T -> Branch("OpPYBoundary",OpPYBoundary,"OpPYBoundary[NOpBoundary]/D");
//		T -> Branch("OpPZBoundary",OpPZBoundary,"OpPZBoundary[NOpBoundary]/D");
//		T -> Branch("OpVXBoundary",OpVXBoundary,"OpVXBoundary[NOpBoundary]/D");
//		T -> Branch("OpVYBoundary",OpVYBoundary,"OpVYBoundary[NOpBoundary]/D");
//		T -> Branch("OpVZBoundary",OpVZBoundary,"OpVZBoundary[NOpBoundary]/D");
//		T -> Branch("OpTBoundary",OpTBoundary,"OpTBoundary[NOpBoundary]/D");
	}
	if(PC->GetParBool("OpSiPM"))
	{
		T -> Branch("NOpSiPM",&NOpSiPM);
		T -> Branch("OpSiPMProcID",OpSiPMProcID,"OpSiPMProcID[NOpSiPM]/I");
		T -> Branch("OpSiPMDetID",OpSiPMDetID,"OpSiPMDetID[NOpSiPM]/I");
		T -> Branch("OpSiPMVX",OpSiPMVX,"OpSiPMVX[NOpSiPM]/D");
		T -> Branch("OpSiPMVY",OpSiPMVY,"OpSiPMVY[NOpSiPM]/D");
		T -> Branch("OpSiPMVZ",OpSiPMVZ,"OpSiPMVZ[NOpSiPM]/D");
		T -> Branch("OpSiPMPX",OpSiPMPX,"OpSiPMPX[NOpSiPM]/D");
		T -> Branch("OpSiPMPY",OpSiPMPY,"OpSiPMPY[NOpSiPM]/D");
		T -> Branch("OpSiPMPZ",OpSiPMPZ,"OpSiPMPZ[NOpSiPM]/D");
		T -> Branch("OpSiPMTime",OpSiPMTime,"OpSiPMTime[NOpSiPM]/D");
		T -> Branch("OpSiPMEnergy",OpSiPMEnergy,"OpSiPMEnergy[NOpSiPM]/D");
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
//		TrackID.clear();
//		TrackProcID.clear();
//		ParentID.clear();
//		TrackPDG.clear();
//		TrackDetID.clear();
//		TrackPX.clear();
//		TrackPY.clear();
//		TrackPZ.clear();
//		TrackVX.clear();
//		TrackVY.clear();
//		TrackVZ.clear();
//		TrackEnergy.clear();
//		TrackKEnergy.clear();
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
//		PostTrackID.clear();
//		PostTrackProcID.clear();
//		PostTrackPDG.clear();
//		PostTrackDetID.clear();
//		PostTrackPX.clear();
//		PostTrackPY.clear();
//		PostTrackPZ.clear();
//		PostTrackVX.clear();
//		PostTrackVY.clear();
//		PostTrackVZ.clear();
//		PostTrackEnergy.clear();
//		PostTrackKEnergy.clear();
	}
	if(PC->GetParBool("MCStep"))
	{
		nStep = 0;
		fill_n(StepTrackID,max_step,0);
		fill_n(StepFromHit,max_step,0);
		fill_n(StepProcID,max_step,0);
		fill_n(StepTrackPDG,max_step,0);
		fill_n(StepPrevDetID,max_step,0);
		fill_n(StepPostDetID,max_step,0);
		fill_n(IsBoundary,max_step,0);
		fill_n(StepPrevKE,max_step,0);
		fill_n(StepVX,max_step,0);
		fill_n(StepVY,max_step,0);
		fill_n(StepVZ,max_step,0);
		fill_n(StepEdep,max_step,0);
		fill_n(StepLength,max_step,0);
		fill_n(StepNSecondaryOP,max_step,0);
//		StepTrackID.clear();
//		StepFromHit.clear();
//		StepProcID.clear();
//		StepTrackPDG.clear();
//		StepPrevDetID.clear();
//		StepPostDetID.clear();
//		IsBoundary.clear();
//		StepPrevKE.clear();
//		StepVX.clear();
//		StepVY.clear();
//		StepVZ.clear();
//		StepEdep.clear();
//		StepLength.clear();
//		StepNSecondaryOP.clear();
	}
	if(PC->GetParBool("OpTrack"))
	{
		NOp = 0;
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

		fill_n(OpTrackID,max_photons,0);
		fill_n(OpProcessID,max_photons,0);
		fill_n(OpParentID,max_photons,0);
		fill_n(OpDetID,max_photons,0);
		fill_n(OpPX,max_photons,0);
		fill_n(OpPY,max_photons,0);
		fill_n(OpPZ,max_photons,0);
		fill_n(OpVX,max_photons,0);
		fill_n(OpVY,max_photons,0);
		fill_n(OpVZ,max_photons,0);
		fill_n(OpEnergy,max_photons,0);
		fill_n(OpKEnergy,max_photons,0);
		fill_n(OpTime,max_photons,0);
	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		PostNOp = 0;
		fill_n(PostOpDetID,max_photons,0);
		fill_n(PostProcID,max_photons,0);
		fill_n(OpTrackLength,max_photons,0);
		fill_n(PostOpPX,max_photons,0);
		fill_n(PostOpPY,max_photons,0);
		fill_n(PostOpPZ,max_photons,0);
		fill_n(PostOpVX,max_photons,0);
		fill_n(PostOpVY,max_photons,0);
		fill_n(PostOpVZ,max_photons,0);
		fill_n(PostOpEnergy,max_photons,0);
		fill_n(PostOpKEnergy,max_photons,0);
		fill_n(PostOpTime,max_photons,0);
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
		fill_n(OpTrackIDBoundary,max_photons,0);
		fill_n(OpProcIDBoundary,max_photons,0);
		fill_n(OpPXBoundary,max_photons,0);
		fill_n(OpPYBoundary,max_photons,0);
		fill_n(OpPZBoundary,max_photons,0);
		fill_n(OpVXBoundary,max_photons,0);
		fill_n(OpVYBoundary,max_photons,0);
		fill_n(OpVZBoundary,max_photons,0);
		fill_n(OpTBoundary,max_photons,0);
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
		fill_n(OpSiPMProcID,max_photons,0);
		fill_n(OpSiPMDetID,max_photons,0);
		fill_n(OpSiPMVX,max_photons,0);
		fill_n(OpSiPMVY,max_photons,0);
		fill_n(OpSiPMVZ,max_photons,0);
		fill_n(OpSiPMPX,max_photons,0);
		fill_n(OpSiPMPY,max_photons,0);
		fill_n(OpSiPMPZ,max_photons,0);
		fill_n(OpSiPMTime,max_photons,0);
		fill_n(OpSiPMEnergy,max_photons,0);
//		OpSiPMProcID.clear();
//		OpSiPMDetID.clear();
//		OpSiPMVX.clear();
//		OpSiPMVY.clear();
//		OpSiPMVZ.clear();
//		OpSiPMPX.clear();
//		OpSiPMPY.clear();
//		OpSiPMPZ.clear();
//		OpSiPMTime.clear();
//		OpSiPMEnergy.clear();
	}
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
	if(opt == MCTrack)	// starting point, prev track
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
		TrackEnergy[nTrack]=totenergy;
		TrackKEnergy[nTrack]=kinenergy;
//		TrackID.push_back(trkID);
//		TrackProcID.push_back(procID);
//		ParentID.push_back(parentID);
//		TrackPDG.push_back(pdg);
//		TrackDetID.push_back(detID);
//		TrackPX.push_back(p.x());
//		TrackPY.push_back(p.y());
//		TrackPZ.push_back(p.z());
//		TrackVX.push_back(v.x());
//		TrackVY.push_back(v.y());
//		TrackVZ.push_back(v.z());
//		TrackEnergy.push_back(totenergy);
//		TrackKEnergy.push_back(kinenergy);
		nTrack++;
	}
	else if(opt == MCPostTrack)	// end point, post Track
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
		PostTrackEnergy[nPostTrack]=totenergy;
		PostTrackKEnergy[nPostTrack]=kinenergy;
//		PostTrackID.push_back(trkID);
//		PostTrackProcID.push_back(procID);
//		PostTrackDetID.push_back(detID);
//		PostTrackPX.push_back(p.x());
//		PostTrackPY.push_back(p.y());
//		PostTrackPZ.push_back(p.z());
//		PostTrackVX.push_back(v.x());
//		PostTrackVY.push_back(v.y());
//		PostTrackVZ.push_back(v.z());
//		PostTrackEnergy.push_back(totenergy);
//		PostTrackKEnergy.push_back(kinenergy);
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

		OpTrackID[NOp] = trkID;
		OpProcessID[NOp] = procID;
		OpParentID[NOp] = parentID;
		OpDetID[NOp] = detID;
		OpPX[NOp] = p.x();
		OpPY[NOp] = p.y();
		OpPZ[NOp] = p.z();
		OpVX[NOp] = v.x();
		OpVY[NOp] = v.y();
		OpVZ[NOp] = v.z();
		OpTime[NOp] =  time;
		OpEnergy[NOp] =  energy;
		OpKEnergy[NOp] =  kenergy;
		NOp++;
	}
	else if (opt == MCPostTrack)
	{
//		G4int idx = find_OpIndex(trkID);
		PostOpTrackID[PostNOp]=trkID;
		PostOpDetID[PostNOp]=detID;
		OpTrackLength[PostNOp]=length;
		PostProcID[PostNOp]=procID;
		PostOpPX[PostNOp]=p.x();
		PostOpPY[PostNOp]=p.y();
		PostOpPZ[PostNOp]=p.z();
		PostOpVX[PostNOp]=v.x();
		PostOpVY[PostNOp]=v.y();
		PostOpVZ[PostNOp]=v.z();
		PostOpEnergy[PostNOp]=energy;
		PostOpKEnergy[PostNOp]=kenergy;
		PostOpTime[PostNOp]=time;
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
//	if(NOpBoundary == max_opticalphotons-1)
//	{
//		G4cout << "Number Of Optical Photon boundary steps exceed max_opticalphotons" << G4endl;
//		return;
//	}
//	if(find(OpTrackIDBoundary.begin(),OpTrackIDBoundary.end(),trkID) == OpTrackIDBoundary.end()){
	for(int a=0; a<max_photons; a++)
	{
		if(OpTrackIDBoundary[a] == trkID)
			return;
	}
//	OpTrackIDBoundary.push_back(trkID);
//	OpProcIDBoundary.push_back(procID);
//	OpPXBoundary.push_back(p.x());
//	OpPYBoundary.push_back(p.y());
//	OpPZBoundary.push_back(p.z());
//	OpVXBoundary.push_back(v.x());
//	OpVYBoundary.push_back(v.y());
//	OpVZBoundary.push_back(v.z());
//	OpTBoundary.push_back(t);
	OpTrackIDBoundary[NOpBoundary] = trkID;
	OpProcIDBoundary[NOpBoundary] = procID;
	OpPXBoundary[NOpBoundary] =  p.x();
	OpPYBoundary[NOpBoundary] =  p.y();
	OpPZBoundary[NOpBoundary] =  p.z();
	OpVXBoundary[NOpBoundary] =  v.x();
	OpVYBoundary[NOpBoundary] =  v.y();
	OpVZBoundary[NOpBoundary] =  v.z();
	OpTBoundary[NOpBoundary] = t;
	G4cout << t << G4endl;
	NOpBoundary++;
//	}
}


void OpRunAction::FillStep
(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep, G4double length, G4int nSecondaries, G4double prevKE)
{
//	G4int idx = find_StepIndex(trkID);
//	if(prev_detID != post_detID)	// at the boundary
//		StepDetID[nStep] = post_detID;	// mainly this case means particle track killed in a volume
//	else
//		StepDetID[nStep] = prev_detID;
//	if(nStep == max_steps-1)
//	{
//		G4cout << "Number of steps exceed the maximum step(" << max_steps << ")" << G4endl;
//		return;
//	}
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
	StepEdep[nStep]=edep;
	StepLength[nStep]=length;
	StepNSecondaryOP[nStep]=nSecondaries;
//	StepFromHit.push_back(fromHit);
//	StepTrackID.push_back(trkID);
//	StepProcID.push_back(procID);
//	StepTrackPDG.push_back(pdg);
//	StepPrevDetID.push_back(prev_detID);
//	StepPostDetID.push_back(post_detID);
//	IsBoundary.push_back(boundary);
//	StepPrevKE.push_back(prevKE);
//	StepVX.push_back(v.x());
//	StepVY.push_back(v.y());
//	StepVZ.push_back(v.z());
//	StepEdep.push_back(edep);
//	StepLength.push_back(length);
//	StepNSecondaryOP.push_back(nSecondaries);
	nStep++;
}

void OpRunAction::FillSiPM
(G4int detID, G4int procID, G4String procName,
 G4ThreeVector pos, G4ThreeVector mom, G4double time, G4double energy)
{
	OpSiPMVX[NOpSiPM]=pos.x();
	OpSiPMVY[NOpSiPM]=pos.y();
	OpSiPMVZ[NOpSiPM]=pos.z();
	OpSiPMPX[NOpSiPM]=mom.x();
	OpSiPMPY[NOpSiPM]=mom.y();
	OpSiPMPZ[NOpSiPM]=mom.z();
	OpSiPMTime[NOpSiPM]=time;
	OpSiPMEnergy[NOpSiPM]=energy;
//	OpSiPMVX.push_back(pos.x());
//	OpSiPMVY.push_back(pos.y());
//	OpSiPMVZ.push_back(pos.z());
//	OpSiPMPX.push_back(mom.x());
//	OpSiPMPY.push_back(mom.y());
//	OpSiPMPZ.push_back(mom.z());
//	OpSiPMTime.push_back(time);
//	OpSiPMEnergy.push_back(energy);
	NOpSiPM++;
	SetProcess(procID,procName);
}
//G4int OpRunAction::find_OpIndex(G4int trkID)
//{
//	for(G4int idx=0; idx<max_opticalphotons; idx++)
//	{
//		if(OpTrackID[idx] == trkID)
//			return idx;
//	}
//	G4ExceptionDescription msg;
//	msg << "OpRunAction::find_OpIndex can not find trkID" << G4endl;
//	G4Exception("OpRunAction::find_OpIndex()", "OpR01",FatalException,msg);
//}
//G4int OpRunAction::find_StepIndex(G4int trkID)
//{
//	for(G4int idx=0; idx<max_steps; idx++)
//	{
//		if(StepTrackID[idx] == trkID)
//			return idx;
//	}
////	G4ExceptionDescription msg;
////	msg << "OpRunAction::find_OpIndex can not find trkID" << G4endl;
////	G4Exception("OpRunAction::find_OpIndex()", "OpR01",FatalException,msg);
//}

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

