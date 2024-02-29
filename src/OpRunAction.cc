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
}

// Link all variables into branch
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
	if(PC -> GetParBool("MCStep") || PC -> GetParBool("MCStepHit"))
	{
		T -> Branch("nStep",&nStep);
		T -> Branch("StepTrackID"  ,StepTrackID    ,"StepTrackID[nStep]/I");
		T -> Branch("StepFromHit"  ,StepFromHit    ,"StepFromHit[nStep]/I");
		T -> Branch("StepProcID"   ,StepProcID     ,"StepProcID[nStep]/I");
		T -> Branch("StepTrackPDG" ,StepTrackPDG   ,"StepTrackPDG[nStep]/I");
		T -> Branch("StepPrevDetID",StepPrevDetID  ,"StepPrevDetID[nStep]/I");
		T -> Branch("StepPostDetID",StepPostDetID  ,"StepPostDetID[nStep]/I");
		T -> Branch("IsBoundary"   ,IsBoundary     ,"IsBoundary[nStep]/I");

		T -> Branch("StepVX"       ,StepVX         ,"StepVX[nStep]/D"); 
		T -> Branch("StepVY"       ,StepVY         ,"StepVY[nStep]/D");
		T -> Branch("StepVZ"       ,StepVZ         ,"StepVZ[nStep]/D");
		T -> Branch("StepTime"     ,StepTime       ,"StepTime[nStep]/D");
		T -> Branch("StepEdep"     ,StepEdep       ,"StepEdep[nStep]/D");
		T -> Branch("StepLength"   ,StepLength     ,"StepLength[nStep]/D");
		T -> Branch("StepPrevKE"   ,StepPrevKE     ,"StepPrevKE[nStep]/D");
		if(PC->GetParBool("OpticalPhysics"))
		{
			T -> Branch("StepNSecondaryOP",&StepNSecondaryOP);
		}
	}
	if(PC -> GetParBool("OpTrack"))
	{
		T -> Branch("NOp",&NOp);
		T -> Branch("OpTrackID"   ,OpTrackID   ,"OpTrackID[NOp]/I");
		T -> Branch("OpProcessID" ,OpProcessID ,"OpProcessID[NOp]/I");
		T -> Branch("OpParentID"  ,OpParentID  ,"OpParentID[NOp]/I");
		T -> Branch("OpDetID"     ,OpDetID     ,"OpDetID[NOp]/I");
		T -> Branch("OpPX"        ,OpPX        ,"OpPX[NOp]/D");
		T -> Branch("OpPY"        ,OpPY        ,"OpPY[NOp]/D");
		T -> Branch("OpPZ"        ,OpPZ        ,"OpPZ[NOp]/D");
		T -> Branch("OpVX"        ,OpVX        ,"OpVX[NOp]/D");
		T -> Branch("OpVY"        ,OpVY        ,"OpVY[NOp]/D");
		T -> Branch("OpVZ"        ,OpVZ        ,"OpVZ[NOp]/D");
		T -> Branch("OpEnergy"    ,OpEnergy    ,"OpEnergy[NOp]/D");
		T -> Branch("OpKEnergy"   ,OpKEnergy   ,"OpKEnergy[NOp]/D");
		T -> Branch("OpTime"      ,OpTime      ,"OpTime[NOp]/D");

	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		T -> Branch("PostNOp",&PostNOp);
		T -> Branch("PostOpTrackID"   ,PostOpTrackID   ,"PostOpTrackID[PostNOp]/I");
		T -> Branch("PostOpDetID"     ,PostOpDetID     ,"PostOpDetID[PostNOp]/I");
		T -> Branch("PostProcID"      ,PostProcID      ,"PostProcID[PostNOp]/I");
		T -> Branch("OpTrackLength"   ,OpTrackLength   ,"OpTrackLength[PostNOp]/D");
		T -> Branch("PostOpPX"        ,PostOpPX        ,"PostOpPX[PostNOp]/D");
		T -> Branch("PostOpPY"        ,PostOpPY        ,"PostOpPY[PostNOp]/D");
		T -> Branch("PostOpPZ"        ,PostOpPZ        ,"PostOpPZ[PostNOp]/D");
		T -> Branch("PostOpVX"        ,PostOpVX        ,"PostOpVX[PostNOp]/D");
		T -> Branch("PostOpVY"        ,PostOpVY        ,"PostOpVY[PostNOp]/D");
		T -> Branch("PostOpVZ"        ,PostOpVZ        ,"PostOpVZ[PostNOp]/D");
		T -> Branch("PostOpEnergy"    ,PostOpEnergy    ,"PostOpEnergy[PostNOp]/D");
		T -> Branch("PostOpKEnergy"   ,PostOpKEnergy   ,"PostOpKEnergy[PostNOp]/D");
		T -> Branch("PostOpTime"      ,PostOpTime      ,"PostOpTime[PostNOp]/D");
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		T -> Branch("NOpBoundary",&NOpBoundary);
		T -> Branch("OpTrackIDBoundary" ,OpTrackIDBoundary ,"OpTrackIDBoundary[NOpBoundary]/I");
		T -> Branch("OpProcIDBoundary"  ,OpProcIDBoundary  ,"OpProcIDBoundary[NOpBoundary]/I");
		T -> Branch("OpPXBoundary"      ,OpPXBoundary      ,"OpPXBoundary[NOpBoundary]/D");
		T -> Branch("OpPYBoundary"      ,OpPYBoundary      ,"OpPYBoundary[NOpBoundary]/D");
		T -> Branch("OpPZBoundary"      ,OpPZBoundary      ,"OpPZBoundary[NOpBoundary]/D");
		T -> Branch("OpVXBoundary"      ,OpVXBoundary      ,"OpVXBoundary[NOpBoundary]/D");
		T -> Branch("OpVYBoundary"      ,OpVYBoundary      ,"OpVYBoundary[NOpBoundary]/D");
		T -> Branch("OpVZBoundary"      ,OpVZBoundary      ,"OpVZBoundary[NOpBoundary]/D");
		T -> Branch("OpTBoundary"       ,OpTBoundary       ,"OpTBoundary[NOpBoundary]/D");

	}
	if(PC->GetParBool("OpSiPM"))
	{
		T -> Branch("NOpSiPM",&NOpSiPM);
		T -> Branch("OpSiPMProcID" ,OpSiPMProcID  ,"OpSiPMProcID[NOpSiPM]/I");
		T -> Branch("OpSiPMDetID"  ,OpSiPMDetID   ,"OpSiPMDetID[NOpSiPM]/I");
		T -> Branch("OpSiPMVX"     ,OpSiPMVX      ,"OpSiPMVX[NOpSiPM]/D");
		T -> Branch("OpSiPMVY"     ,OpSiPMVY      ,"OpSiPMVY[NOpSiPM]/D");
		T -> Branch("OpSiPMVZ"     ,OpSiPMVZ      ,"OpSiPMVZ[NOpSiPM]/D");
		T -> Branch("OpSiPMPX"     ,OpSiPMPX      ,"OpSiPMPX[NOpSiPM]/D");
		T -> Branch("OpSiPMPY"     ,OpSiPMPY      ,"OpSiPMPY[NOpSiPM]/D");
		T -> Branch("OpSiPMPZ"     ,OpSiPMPZ      ,"OpSiPMPZ[NOpSiPM]/D");
		T -> Branch("OpSiPMTime"   ,OpSiPMTime    ,"OpSiPMTime[NOpSiPM]/D");
		T -> Branch("OpSiPMEnergy" ,OpSiPMEnergy  ,"OpSiPMEnergy[NOpSiPM]/D");
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
	if(PC->GetParBool("MCStep") || PC->GetParBool("MCStepHit"))
	{
		nStep = 0;
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
		fill_n(OpTrackID,max_o,0);
		fill_n(OpProcessID,max_o,0);
		fill_n(OpParentID,max_o,0);
		fill_n(OpDetID,max_o,0);
		fill_n(OpPX,max_o,0);
		fill_n(OpPY,max_o,0);
		fill_n(OpPZ,max_o,0);
		fill_n(OpVX,max_o,0);
		fill_n(OpVY,max_o,0);
		fill_n(OpVZ,max_o,0);
		fill_n(OpEnergy,max_o,0);
		fill_n(OpKEnergy,max_o,0);
		fill_n(OpTime,max_o,0);

	}
	if(PC -> GetParBool("OpPostTrack"))
	{
		PostNOp = 0;
		fill_n(PostOpTrackID,max_o,0);
		fill_n(PostOpDetID,max_o,0);
		fill_n(PostProcID,max_o,0);
		fill_n(OpTrackLength,max_o,0);
		fill_n(PostOpPX,max_o,0);
		fill_n(PostOpPY,max_o,0);
		fill_n(PostOpPZ,max_o,0);
		fill_n(PostOpVX,max_o,0);
		fill_n(PostOpVY,max_o,0);
		fill_n(PostOpVZ,max_o,0);
		fill_n(PostOpEnergy,max_o,0);
		fill_n(PostOpKEnergy,max_o,0);
		fill_n(PostOpTime,max_o,0);
	}
	if(PC -> GetParBool("OpBoundary"))
	{
		NOpBoundary = 0;
		fill_n(OpTrackIDBoundary,max_ob,0);
		fill_n(OpProcIDBoundary,max_ob,0);
		fill_n(OpPXBoundary,max_ob,0);
		fill_n(OpPYBoundary,max_ob,0);
		fill_n(OpPZBoundary,max_ob,0);
		fill_n(OpVXBoundary,max_ob,0);
		fill_n(OpVYBoundary,max_ob,0);
		fill_n(OpVZBoundary,max_ob,0);
		fill_n(OpTBoundary,max_ob,0);

	}
	if(PC->GetParBool("OpSiPM"))
	{
		NOpSiPM = 0;
		fill_n(OpSiPMProcID,max_os,0);
		fill_n(OpSiPMDetID,max_os,0);
		fill_n(OpSiPMVX,max_os,0);
		fill_n(OpSiPMVY,max_os,0);
		fill_n(OpSiPMVZ,max_os,0);
		fill_n(OpSiPMPX,max_os,0);
		fill_n(OpSiPMPY,max_os,0);
		fill_n(OpSiPMPZ,max_os,0);
		fill_n(OpSiPMTime,max_os,0);
		fill_n(OpSiPMEnergy,max_os,0);
	}
}

void OpRunAction::FillTrack
(G4int opt, G4int trkID, G4int procID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double time, G4double totenergy, G4double kinenergy)
	// invoked in TrackingAction
{
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
		OpTrackID[NOp]=trkID;
		OpProcessID[NOp]=procID;
		OpParentID[NOp]=parentID;
		OpDetID[NOp]=detID;
		OpPX[NOp]=p.x();
		OpPY[NOp]=p.y();
		OpPZ[NOp]=p.z();
		OpVX[NOp]=v.x();
		OpVY[NOp]=v.y();
		OpVZ[NOp]=v.z();
		OpTime[NOp]=time;
		OpEnergy[NOp]=energy;
		OpKEnergy[NOp]=kenergy;

		NOp++;
	}
	else if (opt == MCPostTrack)
	{
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
		PostNOp++;
	}
}

void OpRunAction::FillOpticalPhotonBoundary
(G4int trkID, G4int procID, G4ThreeVector v, G4ThreeVector p, G4double t)
{
	OpTrackIDBoundary[NOpBoundary] = trkID;
	OpProcIDBoundary[NOpBoundary] = procID;
	OpPXBoundary[NOpBoundary] = p.x();
	OpPYBoundary[NOpBoundary] = p.y();
	OpPZBoundary[NOpBoundary] = p.z();
	OpVXBoundary[NOpBoundary] = v.x();
	OpVYBoundary[NOpBoundary] = v.y();
	OpVZBoundary[NOpBoundary] = v.z();
	OpTBoundary[NOpBoundary] = t;
	NOpBoundary++;
}


void OpRunAction::FillStep
(G4bool boundary, G4bool fromHit, G4int trkID, G4int procID, G4int pdg, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double time,G4double edep, G4double length, G4int nSecondaries, G4double prevKE)
{
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

void OpRunAction::FillSiPM
(G4int detID, G4int procID, G4String procName,
 G4ThreeVector pos, G4ThreeVector mom, G4double time, G4double energy)
{
	OpSiPMProcID[NOpSiPM]=procID;
	OpSiPMDetID[NOpSiPM]=detID;
	OpSiPMVX[NOpSiPM]=pos.x();
	OpSiPMVY[NOpSiPM]=pos.y();
	OpSiPMVZ[NOpSiPM]=pos.z();
	OpSiPMPX[NOpSiPM]=mom.x();
	OpSiPMPY[NOpSiPM]=mom.y();
	OpSiPMPZ[NOpSiPM]=mom.z();
	OpSiPMTime[NOpSiPM]=time;
	OpSiPMEnergy[NOpSiPM]=energy;
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

