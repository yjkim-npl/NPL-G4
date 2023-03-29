#define max 20000
void OpMake
(
 const char* particle = "proton",
 const char* energy = "300MeV",
 const char* suffix = ""
)
{
	/* HIST LISTs
		0: 1-D Number of optical photon distribution
			to check the property "SCINTILLATIONYIELD"
		1: 1-D creator process of generated optical photons 
			to check scint/cheren ratio of electron
		2: 2-D wavelength(energy) - # of photons
		3: 1-D time when o.p created and o.p died
			to check "ABSLENGTH" effect
		4: momentum profile with boundary process
		5: 2-D L/x - Edep/x 
		6: 2-D dL/dx - dE/dx
		7: 1-D parentID(particle) of optical photon
	*/
	bool Opt[8];
	Opt[0] = 1;
	Opt[1] = 1;
	Opt[2] = 1;
	Opt[3] = 1;
	Opt[4] = 0; // not working
	Opt[5] = 1;
	Opt[6] = 0; // not working
	Opt[7] = 1;
	const int StepFromHit = 1;

	char* infile;
	TString str_p = particle;
	TString str_s = suffix;
	if(str_p.Contains("temp")){
		if(str_s=="") infile = Form("out_%s.root",particle);
		else          infile = Form("out_%s_%s.root",particle,suffix);
	}else{
		if(str_s=="") infile = Form("out_%s_%s.root",particle,energy);
		else          infile = Form("out_%s_%s_%s.root",particle,energy,suffix);
	}

	cout << "#####################" << endl;
	cout << "infile: " << infile << endl;
	cout << "#####################" << endl << endl;
	// call the data root file
	TFile* F = new TFile(Form("data_root/%s",infile),"read");

	// define data containers
	TList* L_proc = (TList*) F -> Get("ProcessTable");
	TList* L_para = (TList*) F -> Get("InputParameters");
	TList L_out;
	TTree* T = (TTree*) F -> Get("Opsim");

	map<string,int> map_process;
	for(int a=0; a<L_proc->GetSize(); a++)
	{
		TNamed* name = (TNamed*) L_proc->At(a);
		map_process.insert(make_pair(name->GetTitle(),stoi(name->GetName())));
	}
	map<string,string> map_parameters;
	for(int a=0; a<L_para->GetSize(); a++)
	{
		TNamed* name = (TNamed*) L_para->At(a);
		map_parameters.insert(make_pair(name->GetName(),name->GetTitle()));
//		cout << name->GetName() << " " << name->GetTitle() << endl;
		if(((TString)name->GetName()).Contains("SC2matOpt"))
			L_out.Add(name);
		if(((TString)name->GetName()).Contains("PVT") || ((TString)name->GetName()).Contains("PS"))
			L_out.Add(name);
	}

	// MCTrack data
	int nTrack;
	int TrackID[max], TrackProcID[max], ParentID[max], TrackPDG[max], TrackDetID[max];
	double px[max], py[max], pz[max], vx[max], vy[max], vz[max];
	double KE[max];

	// MCPostTrack data
	int nPostTrack;
	int p_TrackID[max], p_TrackProcID[max], p_TrackDetID[max];
	double p_px[max],p_py[max],p_pz[max],p_vx[max],p_vy[max],p_vz[max];
	double p_KE[max];

	// MCStep data
	int nStep; 
	int s_TrackID[max], s_FromHit[max], s_ProcID[max], s_PDG[max], s_PrevDetID[max], s_PostDetID[max];
	int s_IsBoundary[max], s_NSecondOP[max];
	double s_prevKE[max], s_Edep[max], s_Length[max];
	double s_vx[max], s_vy[max], s_vz[max];

	// OpTrack data
	int NOp;
	int o_TrackID[max], o_ProcID[max], o_ParentID[max], o_DetID[max];
	double o_px[max], o_py[max], o_pz[max], o_vx[max], o_vy[max], o_vz[max];
	double o_KE[max], o_Time[max];

	// OpPostTrackdata
	int op_DetID[max], op_ProcID[max];
	double op_px[max], op_py[max], op_pz[max], op_vx[max], op_vy[max], op_vz[max];
	double op_KE[max], op_Time[max];

	// Link container
	if(map_parameters["MCTrack"] == "true")
	{
		cout << "MCTrack was called" << endl;
		T -> SetBranchAddress("nTrack",&nTrack);
		T -> SetBranchAddress("TrackID",TrackID);
		T -> SetBranchAddress("TrackProcID",TrackProcID);
		T -> SetBranchAddress("ParentID",ParentID);
		T -> SetBranchAddress("TrackPDG",TrackPDG);
		T -> SetBranchAddress("TrackDetID",TrackDetID);
		T -> SetBranchAddress("TrackPX",px);
		T -> SetBranchAddress("TrackPY",py);
		T -> SetBranchAddress("TrackPZ",pz);
		T -> SetBranchAddress("TrackVX",vx);
		T -> SetBranchAddress("TrackVY",vy);
		T -> SetBranchAddress("TrackVZ",vz);
		T -> SetBranchAddress("TrackKEnergy",KE);
	}
	if(map_parameters["MCPostTrack"] == "true")
	{
		cout << "MCPostTrack was called" << endl;
		T -> SetBranchAddress("nPostTrack",&nPostTrack);
		T -> SetBranchAddress("PostTrackID",p_TrackID);
		T -> SetBranchAddress("PostTrackProcID",p_TrackProcID);
		T -> SetBranchAddress("PostTrackDetID",p_TrackDetID);
		T -> SetBranchAddress("PostTrackPX",p_px);
		T -> SetBranchAddress("PostTrackPY",p_py);
		T -> SetBranchAddress("PostTrackPZ",p_pz);
		T -> SetBranchAddress("PostTrackVX",p_vx);
		T -> SetBranchAddress("PostTrackVY",p_vy);
		T -> SetBranchAddress("PostTrackVZ",p_vz);
		T -> SetBranchAddress("PostTrackKEnergy",p_KE);
	}
	if(map_parameters["MCStep"] == "true")
	{
		cout << "MCStep was called" << endl;
		T -> SetBranchAddress("nStep",&nStep);
		T -> SetBranchAddress("StepTrackID",s_TrackID);
		T -> SetBranchAddress("StepFromHit",s_FromHit);
		T -> SetBranchAddress("StepProcID",s_ProcID);
		T -> SetBranchAddress("StepTrackPDG",s_PDG);
		T -> SetBranchAddress("StepPrevDetID",s_PrevDetID);
		T -> SetBranchAddress("StepPostDetID",s_PostDetID);
		T -> SetBranchAddress("IsBoundary",s_IsBoundary);
		T -> SetBranchAddress("StepNSecondaryOP",s_NSecondOP);
		T -> SetBranchAddress("StepPrevKE",s_prevKE);
		T -> SetBranchAddress("StepEdep",s_Edep);
		T -> SetBranchAddress("StepLength",s_Length);
		T -> SetBranchAddress("StepVX",s_vx);
		T -> SetBranchAddress("StepVY",s_vy);
		T -> SetBranchAddress("StepVZ",s_vz);
	}
	if(map_parameters["OpTrack"] == "true")
	{
		cout << "OpTrack was called" << endl;
		T -> SetBranchAddress("NOpticalPhotons",&NOp);
		T -> SetBranchAddress("OpTrackID",o_TrackID);
		T -> SetBranchAddress("OpProcessID",o_ProcID);
		T -> SetBranchAddress("OpParentID",o_ParentID);
		T -> SetBranchAddress("OpDetID",o_DetID);
		T -> SetBranchAddress("OpPX",o_px);
		T -> SetBranchAddress("OpPY",o_py);
		T -> SetBranchAddress("OpPZ",o_pz);
		T -> SetBranchAddress("OpVX",o_vx);
		T -> SetBranchAddress("OpVY",o_vy);
		T -> SetBranchAddress("OpVZ",o_vz);
		T -> SetBranchAddress("OpKEnergy",o_KE);
		T -> SetBranchAddress("OpTime",o_Time);
	}
	if(map_parameters["OpTrack"] == "true" && map_parameters["OpPostTrack"] == "true");
	{
		T -> SetBranchAddress("PostOpDetID",op_DetID);
		T -> SetBranchAddress("PostProcID",op_ProcID);
		T -> SetBranchAddress("PostOpPX",op_px);
		T -> SetBranchAddress("PostOpPY",op_py);
		T -> SetBranchAddress("PostOpPZ",op_pz);
		T -> SetBranchAddress("PostOpVX",op_vx);
		T -> SetBranchAddress("PostOpVY",op_vy);
		T -> SetBranchAddress("PostOpVZ",op_vz);
		T -> SetBranchAddress("PostOpKEnergy",op_KE);
		T -> SetBranchAddress("PostOpTime",op_Time);
	}
	
	
	// define output histrogram container
	TFile* G = new TFile(Form("out_root/H_Op_%s",infile),"recreate");
	// HIST 0
	TH1D* H1_NOp_dist = new TH1D("H1_NOp_dist","",150,0,150);
	// HIST 1
	TH1F* H1_OpProcID = new TH1F("H1_OpProcID","",4,20.5,24.5);
	// HIST 2
	TH1F* H1_OpWav = new TH1F("H1_OpWav","",600,300,900);
	TH1F* H1_OpE = new TH1F("H1_OpE","",350,1,4.5);
//	TH2F* H2_Wav_OP = new TH2F("H2_Wav_OP",600,300,900,100,0,100);
	// HIST 3
	TH1F* H1_OpTime = new TH1F("H1_OpTime","",200,0,20);
	TH1F* H1_PostOpTime = new TH1F("H1_PostOpTime","",500,0,50);
	// HIST 4 (currently not work)
//	enum {Prev,Post,trans,fre_refl,tot_refl,too_small};
//	TH2F* H2_p;
	// HIST 5 w/ step
	TH2F* H2_Edep_L = new TH2F("H2_Edep_L","",30,0,15,100,0,100);
	// HIST 6 w/ step(curretnly not work)
	TH2F* H2_dEdx_dLdx = new TH2F("H2_dEdx_dLdx","",50,0,50,50,0,50);
	// HIST 7 w/ step
	TH2F* H2_LY_OpParentID = new TH2F("H2_LY_OpParentID","",100,0,100,1e5,0,1e5);

	// event loop
	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		if(a%100 == 0) 
			cout << "Processing " << a << " th event" << endl;
		T -> GetEntry(a);
		if(Opt[0])
			H1_NOp_dist -> Fill(NOp);
		map<int,int> map_TrkID_PDG;	// for HIST 7
		map<int,double> map_TrkID_Edep; // for HIST 5
		map<int,int> map_TrkID_LY;      // for HIST 5
		for(int b=0; b<NOp; b++)
		{
			if(Opt[1])
			{
				if(o_ProcID[b] == 21 || o_ProcID[b] == 22)
					H1_OpProcID -> Fill(o_ProcID[b]);
				else
					H1_OpProcID -> Fill(23);
			}
			if(Opt[2]){
//				cout << o_KE[b] << endl;
				H1_OpE -> Fill(o_KE[b]*1e6);
				double wav = 1.2398e-3/o_KE[b];
				H1_OpWav -> Fill(wav);
			}
			if(Opt[3]){
				H1_OpTime -> Fill(o_Time[b]);
				H1_PostOpTime -> Fill(op_Time[b]);
			}
			int pdg = (int)s_PDG[b] % 100000;
			map_TrkID_PDG.insert(make_pair(s_TrackID[b],pdg));
		}// each optical photon
		for(int b=0; b<nStep; b++)
		{
			if(Opt[5] && s_FromHit[b]==StepFromHit && s_PDG[b] > 100)
			{
				if(map_TrkID_Edep[s_TrackID[b]] == 0 && map_TrkID_LY[s_TrackID[b]] == 0){
					map_TrkID_Edep[s_TrackID[b]] = s_Edep[b];
					map_TrkID_LY[s_TrackID[b]] = s_NSecondOP[b];
				} else{
					map_TrkID_Edep[s_TrackID[b]] += s_Edep[b];
					map_TrkID_LY[s_TrackID[b]] += s_NSecondOP[b];
				}
			}
			if(Opt[6])
				H2_dEdx_dLdx -> Fill(s_Edep[b]/s_Length[b],s_NSecondOP[b]/s_Length[b]);
			if(Opt[7] && 
				 s_FromHit[b]==StepFromHit &&
				 s_PDG[b] > 100 && // reject e+,e-,gam
				 s_PrevDetID[b] == 201)
			{
//				cout << "PDG: " << s_PDG[b] << endl;
//				cout << "Edep: " << s_Edep << endl;
				H2_LY_OpParentID -> Fill(s_NSecondOP[b],map_TrkID_PDG[o_ParentID[b]]);
			}
		}// each step
		if(Opt[5])
			H2_Edep_L -> Fill(map_TrkID_Edep[1],map_TrkID_LY[1]);	// primary particle
	}// each event

	// Write to output root file
	L_out.Write("ConstMaterialProperties",TObject::kSingleKey);
	if(Opt[0])
		H1_NOp_dist -> Write();
	if(Opt[1])
		H1_OpProcID -> Write();
	if(Opt[2])
	{
		H1_OpWav -> Write();
		H1_OpE -> Write();
	}
	if(Opt[3])
	{
		H1_OpTime -> Write();
		H1_PostOpTime -> Write();
	}
	if(Opt[4])
	{}
	if(Opt[5])
		H2_Edep_L -> Write();
	if(Opt[6])
	{}
	if(Opt[7])
		H2_LY_OpParentID -> Write();
}
