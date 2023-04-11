void OpMake
(
 const char* particle = "temp",
 const char* energy = "",
 const char* suffix = ""
)
{
	/* HIST LISTs
		0: 1-D Number of optical photon distribution
			to check the property "SCINTILLATIONYIELD"
		1: 1-D creator process of generated optical photons 
			to check scint/cheren ratio of electron
		2: 1-D wavelength(energy) - # of photons
		3: 1-D time when o.p created and o.p died
			to check "ABSLENGTH" effect
		4: momentum profile with boundary process
		5: 2-D L/x - Edep/x 
		6: 2-D dL/dx - dE/dx
		7: 1-D parentID(particle) of optical photon
		8: 1-D histogram of total track length of optical photon
		   to check attenuation(absorption)
		9: 1-D Time distribution at each side of SiPM
		10: 1-D histogram of Number of optical photons reached at each side of SiPM
		11: 2-D histogram of momentum at boundary (OpBoundary)
		12: 2-D histogram of position at boundary (OpBoundary)
	*/
	bool Opt[13];
	Opt[0] = 1;
	Opt[1] = 0;
	Opt[2] = 0;
	Opt[3] = 0; // segmenation error
	Opt[4] = 0; // not working
	Opt[5] = 0;
	Opt[6] = 0; // not working
	Opt[7] = 0;
	Opt[8] = 0;	// segmentation error
	Opt[9] = 0;
	Opt[10] = 0;
	Opt[11] = 0;
	Opt[12] = 0;
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
	vector<int> *TrackID, *TrackProcID, *ParentID, *TrackPDG, *TrackDetID;
	vector<double> *px, *py, *pz, *vx, *vy, *vz;
	vector<double> *KE;

	// MCPostTrack data
	int nPostTrack;
	vector<int> *p_TrackID, *p_TrackProcID, *p_TrackDetID;
	vector<double> *p_px,*p_py,*p_pz,*p_vx,*p_vy,*p_vz;
	vector<double> *p_KE;

	// MCStep data
	int nStep; 
	vector<int> *s_TrackID, *s_FromHit, *s_ProcID, *s_PDG, *s_PrevDetID, *s_PostDetID;
	vector<int> *s_IsBoundary, *s_NSecondOP;
	vector<double> *s_prevKE, *s_Edep, *s_Length;
	vector<double> *s_vx, *s_vy, *s_vz;

	// OpTrack data
	int NOp;
	vector<int> *o_TrackID, *o_ProcID, *o_ParentID, *o_DetID;
	vector<double> *o_px, *o_py, *o_pz, *o_vx, *o_vy, *o_vz;
	vector<double> *o_KE, *o_Time;

	// OpPostTrackdata
	int PostNOp;
	vector<int> *op_DetID, *op_ProcID;
	vector<double> *op_TrackLength;
	vector<double> *op_px, *op_py, *op_pz, *op_vx, *op_vy, *op_vz;
	vector<double> *op_KE, *op_Time;

	// OpBoundary data
	int NOpBoundary;
	vector<int> *ob_TrackID, *ob_ProcID;
	vector<double> *ob_px, *ob_py, *ob_pz, *ob_vx, *ob_vy, *ob_vz, *ob_Time;

	// OpSiPM data
	int NOpSiPM;
	vector<int> *os_ProcID, *os_DetID;
	vector<double> *os_vx, *os_vy, *os_vz, *os_px, *os_py, *os_pz, *os_Time, *os_E;

	// Link container
	if(map_parameters["MCTrack"] == "true")
	{
		cout << "MCTrack was called" << endl;
		T -> SetBranchAddress("nTrack",&nTrack);
		T -> SetBranchAddress("TrackID",&TrackID);
		T -> SetBranchAddress("TrackProcID",&TrackProcID);
		T -> SetBranchAddress("ParentID",&ParentID);
		T -> SetBranchAddress("TrackPDG",&TrackPDG);
		T -> SetBranchAddress("TrackDetID",&TrackDetID);
		T -> SetBranchAddress("TrackPX",&px);
		T -> SetBranchAddress("TrackPY",&py);
		T -> SetBranchAddress("TrackPZ",&pz);
		T -> SetBranchAddress("TrackVX",&vx);
		T -> SetBranchAddress("TrackVY",&vy);
		T -> SetBranchAddress("TrackVZ",&vz);
		T -> SetBranchAddress("TrackKEnergy",&KE);
	}
	if(map_parameters["MCPostTrack"] == "true")
	{
		cout << "MCPostTrack was called" << endl;
		T -> SetBranchAddress("nPostTrack",&nPostTrack);
		T -> SetBranchAddress("PostTrackID",&p_TrackID);
		T -> SetBranchAddress("PostTrackProcID",&p_TrackProcID);
		T -> SetBranchAddress("PostTrackDetID",&p_TrackDetID);
		T -> SetBranchAddress("PostTrackPX",&p_px);
		T -> SetBranchAddress("PostTrackPY",&p_py);
		T -> SetBranchAddress("PostTrackPZ",&p_pz);
		T -> SetBranchAddress("PostTrackVX",&p_vx);
		T -> SetBranchAddress("PostTrackVY",&p_vy);
		T -> SetBranchAddress("PostTrackVZ",&p_vz);
		T -> SetBranchAddress("PostTrackKEnergy",&p_KE);
	}
	if(map_parameters["MCStep"] == "true")
	{
		cout << "MCStep was called" << endl;
		T -> SetBranchAddress("nStep",&nStep);
		T -> SetBranchAddress("StepTrackID",&s_TrackID);
		T -> SetBranchAddress("StepFromHit",&s_FromHit);
		T -> SetBranchAddress("StepProcID",&s_ProcID);
		T -> SetBranchAddress("StepTrackPDG",&s_PDG);
		T -> SetBranchAddress("StepPrevDetID",&s_PrevDetID);
		T -> SetBranchAddress("StepPostDetID",&s_PostDetID);
		T -> SetBranchAddress("IsBoundary",&s_IsBoundary);
		T -> SetBranchAddress("StepNSecondaryOP",&s_NSecondOP);
		T -> SetBranchAddress("StepPrevKE",&s_prevKE);
		T -> SetBranchAddress("StepEdep",&s_Edep);
		T -> SetBranchAddress("StepLength",&s_Length);
		T -> SetBranchAddress("StepVX",&s_vx);
		T -> SetBranchAddress("StepVY",&s_vy);
		T -> SetBranchAddress("StepVZ",&s_vz);
	}
	if(map_parameters["OpTrack"] == "true")
	{
		cout << "OpTrack was called" << endl;
		T -> SetBranchAddress("NOp",&NOp);
		T -> SetBranchAddress("OpTrackID",&o_TrackID);
		T -> SetBranchAddress("OpProcessID",&o_ProcID);
		T -> SetBranchAddress("OpParentID",&o_ParentID);
		T -> SetBranchAddress("OpDetID",&o_DetID);
		T -> SetBranchAddress("OpPX",&o_px);
		T -> SetBranchAddress("OpPY",&o_py);
		T -> SetBranchAddress("OpPZ",&o_pz);
		T -> SetBranchAddress("OpVX",&o_vx);
		T -> SetBranchAddress("OpVY",&o_vy);
		T -> SetBranchAddress("OpVZ",&o_vz);
		T -> SetBranchAddress("OpKEnergy",&o_KE);
		T -> SetBranchAddress("OpTime",&o_Time);
	}
	if(map_parameters["OpTrack"] == "true" && map_parameters["OpPostTrack"] == "true")
	{
		cout << "OpPostTrack was called" << endl;
		T -> SetBranchAddress("PostNOp",&PostNOp);
		T -> SetBranchAddress("PostOpDetID",&op_DetID);
		T -> SetBranchAddress("OpTrackLength",&op_TrackLength);
		T -> SetBranchAddress("PostProcID",&op_ProcID);
		T -> SetBranchAddress("PostOpPX",&op_px);
		T -> SetBranchAddress("PostOpPY",&op_py);
		T -> SetBranchAddress("PostOpPZ",&op_pz);
		T -> SetBranchAddress("PostOpVX",&op_vx);
		T -> SetBranchAddress("PostOpVY",&op_vy);
		T -> SetBranchAddress("PostOpVZ",&op_vz);
		T -> SetBranchAddress("PostOpKEnergy",&op_KE);
		T -> SetBranchAddress("PostOpTime",&op_Time);
	}
	if(map_parameters["OpSiPM"]=="true")
	{
		cout << "OpSiPM was called" << endl;
		T -> SetBranchAddress("NOpSiPM",&NOpSiPM);
		T -> SetBranchAddress("OpSiPMProcID",&os_ProcID);
		T -> SetBranchAddress("OpSiPMDetID",&os_DetID);
		T -> SetBranchAddress("OpSiPMVX",&os_vx);
		T -> SetBranchAddress("OpSiPMVY",&os_vy);
		T -> SetBranchAddress("OpSiPMVZ",&os_vz);
		T -> SetBranchAddress("OpSiPMPX",&os_px);
		T -> SetBranchAddress("OpSiPMPY",&os_py);
		T -> SetBranchAddress("OpSiPMPZ",&os_pz);
		T -> SetBranchAddress("OpSiPMTime",&os_Time);
		T -> SetBranchAddress("OpSiPMEnergy",&os_E);
	}
	if(map_parameters["OpBoundary"] == "true" && false)
	{
		cout << "OpBoundary was called" << endl;
		T -> SetBranchAddress("NOpBoundary",&NOpBoundary);
		T -> SetBranchAddress("OpTrackIDBoundary",&ob_TrackID);
		T -> SetBranchAddress("OpProcIDBoundary",&ob_ProcID);
		T -> SetBranchAddress("OpPXBoundary",&ob_px);
		T -> SetBranchAddress("OpPYBoundary",&ob_py);
		T -> SetBranchAddress("OpPZBoundary",&ob_pz);
		T -> SetBranchAddress("OpVXBoundary",&ob_vx);
		T -> SetBranchAddress("OpVYBoundary",&ob_vy);
		T -> SetBranchAddress("OpVZBoundary",&ob_vz);
		T -> SetBranchAddress("OpTBoundary",&ob_Time);
	}
	
	
	// define output histrogram container
	TFile* G = new TFile(Form("out_root/H_Op_%s",infile),"recreate");
	// HIST 0
	TH1D* H1_NOp_dist = new TH1D("H1_NOp_dist","",300,0,300);
	// HIST 1
	TH1F* H1_OpProcID = new TH1F("H1_OpProcID","",4,20.5,24.5);
	// HIST 2
	enum {Total,Scint,Cheren,Att,Forward,Backward,SiPM};
	const char* cre_opt[] = {"Total","Scint","Cheren","Attenuation","Forward","Backward","SiPM"};
	const int n_opt = sizeof(cre_opt)/sizeof(cre_opt[0]);
	TH1F* H1_OpWav[n_opt];
	TH1F* H1_OpE[n_opt];
	for(int a=0; a<n_opt; a++)
	{
		H1_OpWav[a] = new TH1F(Form("H1_OpWav_%s",cre_opt[a]),"",600,300,900);
		H1_OpE[a] = new TH1F(Form("H1_OpE_%s",cre_opt[a]),"",350,1,4.5);
	}
//	TH2F* H2_Wav_OP = new TH2F("H2_Wav_OP",600,300,900,100,0,100);
	// HIST 3
	TH1F* H1_OpTime = new TH1F("H1_OpTime","",200,0,20);
	TH1F* H1_PostOpTime = new TH1F("H1_PostOpTime","",500,0,50);
	// HIST 4 (currently not work)
	enum {Prev,trans,fre_refl,tot_refl,too_small};
	char* str_proc[] = {"Prev","trans","fre_refl","tot_refl","too_small"};
	const int nproc = sizeof(str_proc)/sizeof(str_proc[0]);
	TH3F* H3_p[nproc];
	for(int a=0; a<nproc; a++)
	{
		H3_p[a] = new TH3F(Form("H3_p_%s",str_proc[a]),"",
//				800,-4e-6,4e6,800,-4e-6,4e6,800,-4e-6,4e6);
				8,-4e-6,4e6,8,-4e-6,4e6,8,-4e-6,4e6);
	}
	// HIST 5 w/ step
	TH2F* H2_Edep_L = new TH2F("H2_Edep_L","",30,0,15,100,0,100);
	// HIST 6 w/ step(curretnly not work)
	TH2F* H2_dEdx_dLdx = new TH2F("H2_dEdx_dLdx","",50,0,50,50,0,50);
	// HIST 7 w/ step
	TH2F* H2_LY_OpParentID = new TH2F("H2_LY_OpParentID","",100,0,100,1e5,0,1e5);
	// HIST 8 w/ OpPostTrack
	TH1F* H1_TrackLength = new TH1F("H1_TrackLength","",2500,0,2500);
	// HIST 9 w/ OpSiPM
	TH1F* H1_Time_Left = new TH1F("H1_Time_Left","",200,0,20);
	TH1F* H1_Time_Right = new TH1F("H1_Time_Right","",200,0,20);
	// HIST 10 w/ OpSiPM
	TH1F* H1_NOp_Left = new TH1F("H1_NOp_Left","",100,0,100);
	TH1F* H1_NOp_Right = new TH1F("H1_NOp_Right","",100,0,100);
	// HIST 11 w/ OpBoundary
//	TH2F* H2_OpB_mom[6]; 
	// HIST 12 w/ OpBoundary
	TH2F* H2_OpB_pos[6];// forward, backward, upside, downside, left, right

	// event loop
	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		if(a%100 == 0) 
			cout << "Processing " << a << " th event" << endl;
		cout << "done" << endl;
		T -> GetEntry(a);
		if(Opt[0])
			H1_NOp_dist -> Fill(NOp);
		map<int,int> map_TrkID_PDG;	// for HIST 7
		map<int,double> map_TrkID_Edep; // for HIST 5
		map<int,int> map_TrkID_LY;      // for HIST 5
		int NOp_Left = 0;
		int NOp_Right = 0;
		for(int b=0; b<NOpSiPM; b++)
		{
			if(Opt[9])
			{
				if(os_vx->at(b) < 0){
					H1_Time_Left -> Fill(os_Time->at(b));
					NOp_Left++;
				}else if (os_vx->at(b) > 0){
					H1_Time_Right-> Fill(os_Time->at(b));
					NOp_Right++;
				}else{
					continue;
				}
			}
			if(Opt[2])
			{
				double wav = 1.2398e-3/os_E->at(b);
				H1_OpE[SiPM] -> Fill(os_E->at(b)*1e6);
				H1_OpWav[SiPM] -> Fill(wav);
			}
		} // OpSiPM
		if(Opt[10])
		{
			H1_NOp_Left -> Fill(NOp_Left);
			H1_NOp_Right -> Fill(NOp_Right);
		}
		for(int b=0; b<NOp; b++)
		{
//			if(NOp > max)
//				break;
			if(Opt[1])
			{
				if(o_ProcID->at(b) == 21 || o_ProcID->at(b) == 22)
					H1_OpProcID -> Fill(o_ProcID->at(b));
				else
					H1_OpProcID -> Fill(23);
			}
			if(Opt[2]){
//				cout << o_KE[b] << endl;
				double wav = 1.2398e-3/o_KE->at(b);
				H1_OpE[Total] -> Fill(o_KE->at(b)*1e6);
				H1_OpWav[Total] -> Fill(wav);
				if(o_ProcID->at(b) == 21){ // Cherenkov
					H1_OpE[Cheren] -> Fill(o_KE->at(b)*1e6);
					H1_OpWav[Cheren] -> Fill(wav);
				}else if(o_ProcID->at(b) == 22){ // Scintillation
					H1_OpE[Scint] -> Fill(o_KE->at(b)*1e6);
					H1_OpWav[Scint] -> Fill(wav);
				}else{
					cout << "other process: " << o_ProcID->at(b) << endl;
				}
			}
			if(Opt[3]){
				if(map_parameters["OpTrack"]=="true")
					H1_OpTime -> Fill(o_Time->at(b));
				if(map_parameters["OpPostTrack"]=="true")
					H1_PostOpTime -> Fill(op_Time->at(b));
			}
			if(Opt[4]){
				H3_p[Prev] -> Fill(o_px->at(b), o_py->at(b), o_pz->at(b));
			}
			int pdg = (int)s_PDG->at(b) % 100000;
			map_TrkID_PDG.insert(make_pair(s_TrackID->at(b),pdg));
		}// each optical photon generation
		for(int b=0; b<PostNOp;b++)
		{
			if(Opt[8] && op_TrackLength->at(b) != 0 && map_parameters["OpPostTrack"]=="true"){
				H1_TrackLength -> Fill(op_TrackLength->at(b));
			}
			if(Opt[2]){
				double wav = 1.2398e-3/op_KE->at(b);
				if(op_DetID->at(b) == 201){
					H1_OpE[Att] -> Fill(op_KE->at(b)*1e6);
					H1_OpWav[Att] -> Fill(wav);
			}
				// not exactly goes through forward/backward plane
				else if (op_vz->at(b) > 0){
					H1_OpE[Forward] -> Fill(op_KE->at(b)*1e6);
					H1_OpWav[Forward] -> Fill(wav);
				}else if (op_vz->at(b) < 0){
					H1_OpE[Backward] -> Fill(op_KE->at(b)*1e6);
					H1_OpWav[Backward] -> Fill(wav);
				}
			}
		} // each dead photon
		for(int b=0; b<nStep; b++)
		{
			if(Opt[5] && s_FromHit->at(b)==StepFromHit && s_PDG->at(b) > 100)
			{
				if(map_TrkID_Edep[s_TrackID->at(b)] == 0 && map_TrkID_LY[s_TrackID->at(b)] == 0){
					map_TrkID_Edep[s_TrackID->at(b)] = s_Edep->at(b);
					map_TrkID_LY[s_TrackID->at(b)] = s_NSecondOP->at(b);
				} else{
					map_TrkID_Edep[s_TrackID->at(b)] += s_Edep->at(b);
					map_TrkID_LY[s_TrackID->at(b)] += s_NSecondOP->at(b);
				}
			}
			if(Opt[6])
				H2_dEdx_dLdx -> Fill(s_Edep->at(b)/s_Length->at(b),s_NSecondOP->at(b)/s_Length->at(b));
			if(Opt[7] && 
				 s_FromHit->at(b)==StepFromHit &&
				 s_PDG->at(b) > 100 && // reject e+,e-,gam
				 s_PrevDetID->at(b) == 201)
			{
//				cout << "PDG: " << s_PDG->at(b) << endl;
//				cout << "Edep: " << s_Edep << endl;
				H2_LY_OpParentID -> Fill(s_NSecondOP->at(b),map_TrkID_PDG[o_ParentID->at(b)]);
			}
		}// each step
		if(Opt[5])
			H2_Edep_L -> Fill(map_TrkID_Edep[1],map_TrkID_LY[1]);	// primary particle
		for(int b=0; b<NOpBoundary; b++)
		{
			if(b%100 ==0)
				cout << "Processing " << b << "th boundary step" << endl;
			int idx = -1;
			if(ob_ProcID->at(b) == map_process["Transmission"])                 idx = trans;
			else if(ob_ProcID->at(b) == map_process["FresnelReflection"])       idx = fre_refl;
			else if(ob_ProcID->at(b) == map_process["TotalInternalReflection"]) idx = tot_refl;
			else if(ob_ProcID->at(b) == map_process["StepTooSmall"])            idx = too_small;
			else{
				cout << "other process: " << ob_ProcID->at(b) << endl;
				continue;
			}
			if(Opt[4])
				H3_p[idx] -> Fill(ob_px->at(b),ob_py->at(b),ob_pz->at(b));
		}
	}// each event

	// Write to output root file
	L_out.Write("ConstMaterialProperties",TObject::kSingleKey);
	if(Opt[0])
		H1_NOp_dist -> Write();
	if(Opt[1])
		H1_OpProcID -> Write();
	if(Opt[2])
	{
		for(int a=0; a<n_opt; a++)
		{
			H1_OpWav[a] -> Write();
			H1_OpE[a] -> Write();
		}
	}
	if(Opt[3])
	{
		H1_OpTime -> Write();
		H1_PostOpTime -> Write();
	}
	if(Opt[4])
	{
		for(int a=0; a<nproc; a++)
		{
			H3_p[a] -> Write();
		}
	}
	if(Opt[5])
		H2_Edep_L -> Write();
	if(Opt[6])
	{}
	if(Opt[7])
		H2_LY_OpParentID -> Write();
	if(Opt[8])
		H1_TrackLength -> Write();
	if(Opt[9])
	{
		H1_Time_Left -> Write();
		H1_Time_Right -> Write();
	}
	if(Opt[10])
	{
		H1_NOp_Left -> Write();
		H1_NOp_Right -> Write();
	}
	F -> Close();
}
