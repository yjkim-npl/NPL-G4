void OpMake
(
 const char* particle = "proton",
 const char* energy = "100MeV",
 const char* suffix = "NoSiPM"
)
{
	/*
		 HIST LISTs
		 0. 1-D Number of optical photon distribution (OpTrack)
		 1. 1-D creator process of optical photon     (OpTrack)
		 2. 1-D wavelength(energy) distribution       (OpTrack)
		 3. 1-D time of generation / died             (OpTrack || OpPostTrack)
		 4. 2-D momentum profile at boundary          (OpBoundary)
		 5. 2-D LY - Edep                             (MCStep)
		 6. 1-D total track length of optical photon  (OpPostTrack)
		 7. 1-D Number of optical photon at SiPM      (OpSiPM)
		 8. 1-D Time distribution at SiPM             (OpSiPM)
	 */
	bool Opt[9]; fill_n(Opt,9,1);
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

	cout << "####################" << endl;
	cout << "infile: " << infile << endl;
	cout << "####################" << endl;
	const char* data_prefix = "data_root";

	// Call the data root file
	TFile* F = new TFile(Form("%s/%s",data_prefix,infile),"read");
	TList* L_proc = (TList*) F -> Get("ProcessTable");
	TList* L_para = (TList*) F -> Get("InputParameters");
	TList L_out;
	TTree* T = (TTree*) F -> Get("Opsim");

	// Link the process and parameters
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
		if(((TString)name->GetName()).Contains("SC2matOpt"))
			L_out.Add(name);
		if(((TString)name->GetName()).Contains("PVT") || ((TString)name->GetName()).Contains("PS"))
			L_out.Add(name);
	}
	
	// Define the data container
	const int m_t = 1e4;
	const int m_s = 2.5e4;
	const int m_o = 1e4;
	// MCTrack data
	int nTrack;
	int t_ID[m_t], t_ProcID[m_t], t_ParentID[m_t], t_PDG[m_t], t_DetID[m_t];
	double t_px[m_t], t_py[m_t], t_pz[m_t], t_vx[m_t], t_vy[m_t], t_vz[m_t], t_KE[m_t];

	// MCPostTrack data
	int nPostTrack;
	int p_ID[m_t], p_ProcID[m_t], p_DetID[m_t];
	double p_px[m_t], p_py[m_t], p_pz[m_t], p_vx[m_t], p_vy[m_t], p_vz[m_t], p_KE[m_t];

	// MCStep data
	int nStep;
	int s_ID[m_s], s_FromHit[m_s], s_ProcID[m_s], s_PDG[m_s], s_PrevDetID[m_s], s_PostDetID[m_s];
	int s_IsBoundary[m_s], s_NSecondOP[m_s];
	double s_PrevKE[m_s], s_Edep[m_s], s_Length[m_s], s_vx[m_s], s_vy[m_s], s_vz[m_s];

	// OpTrack data
	int NOp;
	int o_ID[m_o], o_ProcID[m_o], o_ParentID[m_o], o_DetID[m_o];
	double o_px[m_o], o_py[m_o], o_pz[m_o], o_vx[m_o], o_vy[m_o], o_vz[m_o], o_KE[m_o], o_Time[m_o];

	// OpPostTrack data
	int PostNOp;
	int op_ID[m_o], op_ProcID[m_o], op_DetID[m_o];
	double op_px[m_o], op_py[m_o], op_pz[m_o], op_vx[m_o], op_vy[m_o], op_vz[m_o];
	double op_KE[m_o], op_Time[m_o], op_Length[m_o];

	// OpSiPM data
	int NOpSiPM;
	int os_ProcID[m_o], os_DetID[m_o];
	double os_px[m_o], os_py[m_o], os_pz[m_o], os_vx[m_o], os_vy[m_o], os_vz[m_o];
	double os_KE[m_o], os_Time[m_o];

	// Link container
	if(map_parameters["MCTrack"] == "true")
	{
		cout << "MCTrack was called" << endl;
		T -> SetBranchAddress("nTrack",&nTrack);
		T -> SetBranchAddress("TrackID",t_ID);
		T -> SetBranchAddress("TrackProcID",t_ProcID);
		T -> SetBranchAddress("ParentID",t_ParentID);
		T -> SetBranchAddress("TrackPDG",t_PDG);
		T -> SetBranchAddress("TrackDetID",t_DetID);
		T -> SetBranchAddress("TrackPX",t_px);
		T -> SetBranchAddress("TrackPY",t_py);
		T -> SetBranchAddress("TrackPZ",t_pz);
		T -> SetBranchAddress("TrackVX",t_vx);
		T -> SetBranchAddress("TrackVY",t_vy);
		T -> SetBranchAddress("TrackVZ",t_vz);
		T -> SetBranchAddress("TrackKEnergy",t_KE);
	}
	if(map_parameters["MCPostTrack"] == "true")
	{
		cout << "MCPostTrack was called" << endl;
		T -> SetBranchAddress("nPostTrack",&nPostTrack);
		T -> SetBranchAddress("PostTrackID",p_ID);
		T -> SetBranchAddress("PostTrackProcID",p_ProcID);
		T -> SetBranchAddress("PostTrackDetID",p_DetID);
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
		T -> SetBranchAddress("StepTrackID",s_ID);
		T -> SetBranchAddress("StepFromHit",s_FromHit);
		T -> SetBranchAddress("StepProcID",s_ProcID);
		T -> SetBranchAddress("StepTrackPDG",s_PDG);
		T -> SetBranchAddress("StepPrevDetID",s_PrevDetID);
		T -> SetBranchAddress("StepPostDetID",s_PostDetID);
		T -> SetBranchAddress("IsBoundary",s_IsBoundary);
		T -> SetBranchAddress("StepNSecondaryOP",s_NSecondOP);
		T -> SetBranchAddress("StepPrevKE",s_PrevKE);
		T -> SetBranchAddress("StepEdep",s_Edep);
		T -> SetBranchAddress("StepLength",s_Length);
	}
	if(map_parameters["OpTrack"] == "true")
	{
		cout << "OpTrack was called" << endl;
		T -> SetBranchAddress("NOp",&NOp);
		T -> SetBranchAddress("OpTrackID",o_ID);
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
	if(map_parameters["OpPostTrack"] == "true")
	{
		cout << "OpPostTrack was called" << endl;
		T -> SetBranchAddress("PostNOp",&PostNOp);
		T -> SetBranchAddress("PostOpTrackID",op_ID);
		T -> SetBranchAddress("PostProcID",op_ProcID);
		T -> SetBranchAddress("PostOpDetID",op_DetID);
		T -> SetBranchAddress("PostOpPX",op_px);
		T -> SetBranchAddress("PostOpPY",op_py);
		T -> SetBranchAddress("PostOpPZ",op_pz);
		T -> SetBranchAddress("PostOpVX",op_vx);
		T -> SetBranchAddress("PostOpVY",op_vy);
		T -> SetBranchAddress("PostOpVZ",op_vz);
		T -> SetBranchAddress("PostOpKEnergy",op_KE);
		T -> SetBranchAddress("PostOpTime",op_Time);
		T -> SetBranchAddress("OpTrackLength",op_Length);
	}
	if(map_parameters["OpSiPM"] == "true")
	{
		cout << "OpSiPM was called" << endl;
		T -> SetBranchAddress("NOpSiPM",&NOpSiPM);
		T -> SetBranchAddress("OpSiPMProcID",os_ProcID);
		T -> SetBranchAddress("OpSiPMDetID",os_DetID);
		T -> SetBranchAddress("OpSiPMPX",os_px);
		T -> SetBranchAddress("OpSiPMPY",os_py);
		T -> SetBranchAddress("OpSiPMPZ",os_pz);
		T -> SetBranchAddress("OpSiPMVX",os_vx);
		T -> SetBranchAddress("OpSiPMVY",os_vy);
		T -> SetBranchAddress("OpSiPMVZ",os_vz);
		T -> SetBranchAddress("OpSiPMEnergy",os_KE);
		T -> SetBranchAddress("OpSiPMTime",os_Time);
	}

	// define output root file
	const char* output_prefix = "out_root";
	TFile* G = new TFile(Form("%s/H_Op_%s",output_prefix,infile),"recreate");

	// HIST 0
	TH1F* H1_NOp = new TH1F("H1_NOp","",5000,0,5000);
	// HIST 1
	TH1F* H1_OpProcID = new TH1F("H1_OpProcID","",4,19.5,23.5);
	// HIST 2
	enum {Total,Scint,Cheren,ELSE};
	const char* cre_opt[] = {"Total","Scintillation","Cherenkov","ELSE"};
	const int n_opt = sizeof(cre_opt)/sizeof(cre_opt[0]);
	TH1F* H1_OpWav[n_opt];
	TH1F* H1_OpE[n_opt];
	for(int a=0; a<n_opt; a++)
	{
		H1_OpWav[a] = new TH1F(Form("H1_OpWav_%s",cre_opt[a]),"",600,300,900);
		H1_OpE[a] = new TH1F(Form("H1_OpE_%s",cre_opt[a]),"",350,1,4.5);
	}
	// HIST 3
	TH1F* H1_OpTime = new TH1F("H1_OpTime","",200,0,20);
	TH1F* H1_PostOpTime = new TH1F("H1_PostOpTime","",500,0,50);
	// HIST 4
	// HIST 5
	TH2F* H2_Edep_LY = new TH2F("H2_Edep_LY","",100,0,100,5000,0,5000);
	TH2F* H2_dEdx_dLdx = new TH2F("H2_dEdx_dLdx","",100,0,100,5000,0,5000);
	// HIST 6
	TH1F* H1_OpTrackLength = new TH1F("H1_OpTrackLength","",1000,0,1000);
	// HIST 7
	enum {L,R};
	const char* str_SiPM_opt[] = {"Left","Right"};
	const int n_SiPM_opt = sizeof(str_SiPM_opt)/sizeof(str_SiPM_opt[0]);
	TH1F* H1_NOpSiPM[n_SiPM_opt];
	for(int a=0; a<n_SiPM_opt; a++)
		H1_NOpSiPM[a] = new TH1F(Form("H1_NOpSiPM_%s",str_SiPM_opt[a]),"",5000,0,5000);
	// HIST 8
	TH1F* H1_OpSiPMTime[n_SiPM_opt];
	for(int a=0; a<n_SiPM_opt; a++)
		H1_OpSiPMTime[a] = new TH1F(Form("H1_OpSiPMTime_%s",str_SiPM_opt[a]),"",500,0,50);

	// event loop
	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		if(a%100 == 0)
			cout << "Processing " << a << " th event" << endl;
		T -> GetEntry(a);
		if(Opt[0])
			H1_NOp -> Fill(NOp);
		for(int b=0; b<NOp; b++)
		{
			if(Opt[1])
			{
				H1_OpProcID -> Fill(20); // Fill total entries
				if(o_ProcID[b] == 21 || o_ProcID[b] == 22)
					H1_OpProcID -> Fill(o_ProcID[b]);
				else
					H1_OpProcID -> Fill(23);
			}// Opt1
			if(Opt[2])
			{
				double wav = 1.2398e-3/o_KE[b];
				double E = 1e6*o_KE[b];
				H1_OpE[Total] -> Fill(E);
				H1_OpWav[Total] -> Fill(wav);
				if(o_ProcID[b] == 21){ // cherenkov
					H1_OpE[Cheren] -> Fill(E);
					H1_OpWav[Cheren] -> Fill(wav);
				}else if (o_ProcID[b] == 22){ // scintillation
					H1_OpE[Scint] -> Fill(E);
					H1_OpWav[Scint] -> Fill(wav);
				}else{
					H1_OpE[ELSE] -> Fill(E);
					H1_OpWav[ELSE] -> Fill(wav);
				}
			}// Opt2
			if(Opt[3])
				H1_OpTime -> Fill(o_Time[b]);
		}// each optical photon at OpTrack
		for(int b=0; b<PostNOp; b++)
		{
			if(Opt[3])
			{
				if(op_DetID[b] == 201)
					H1_PostOpTime -> Fill(op_Time[b]);
			}
			if(Opt[6])
			{
				if(op_DetID[b] == 201)
					H1_OpTrackLength -> Fill(op_Length[b]);
			}
		}// each optical photon at PostOpTrack

		// container for calculation of energy weighted time 
		int NOp_L = 0;	// for opt 7
		int NOp_R = 0;
		double MeanTime_L = 0;
		double MeanTime_R = 0;
		double MeanEnergy_L = 0;
		double MeanEnergy_R = 0;
		for(int b=0; b<NOpSiPM; b++)
		{
			if(Opt[7])
			{
				if(os_vx[b] > 0) NOp_R++;
				else if (os_vx[b] <0) NOp_L++;
				else cout << "other position was detected" << endl;
			}
			if(Opt[8])
			{
				if(os_vx[b] >0)
				{
					MeanTime_R += os_KE[b]*os_Time[b];
					MeanEnergy_R += os_KE[b];
				} else if (os_vx[b] <0) {
					MeanTime_L += os_KE[b]*os_Time[b];
					MeanEnergy_L += os_KE[b];
				} else {
					cout << "other position was detected" << endl;
				}
			}
		}// each optical photon at OpSiPM
		if(Opt[7])
		{
			H1_NOpSiPM[L] -> Fill(NOp_L);
			H1_NOpSiPM[R] -> Fill(NOp_R);
		}
		if(Opt[8])
		{
			MeanTime_L /= MeanEnergy_L;
			MeanTime_R /= MeanEnergy_R;
			H1_OpSiPMTime[L] -> Fill(MeanTime_L);
			H1_OpSiPMTime[R] -> Fill(MeanTime_R);
		}
		map<int,double> map_TrkID_Edep;
		map<int,int> map_TrkID_LY;
		map<int,double> map_TrkID_Length;
		for(int b=0; b<nStep; b++)
		{
			if(s_PrevDetID[b] == 0 && s_PrevDetID[b] == 0) continue;
			if(Opt[5] && s_FromHit[b] == StepFromHit && s_PDG[b] > 100){
				if(map_TrkID_Edep[s_ID[b]] == 0 && map_TrkID_LY[s_ID[b]] == 0){
					map_TrkID_Edep[s_ID[b]] = s_Edep[b];
					map_TrkID_LY[s_ID[b]] = s_NSecondOP[b];
					map_TrkID_Length[s_ID[b]] = s_Length[b];
				} else {
					map_TrkID_Edep[s_ID[b]] += s_Edep[b];
					map_TrkID_LY[s_ID[b]] += s_NSecondOP[b];
					map_TrkID_Length[s_ID[b]] += s_Length[b];
				}
			}// Opt 5
		}// each step in MCStep
		if(Opt[5]){
			H2_Edep_LY -> Fill(map_TrkID_Edep[1],map_TrkID_LY[1]);
			H2_dEdx_dLdx -> Fill(map_TrkID_Edep[1]/map_TrkID_Length[1],map_TrkID_LY[1]/map_TrkID_Length[1]);
		}
	}// event
	L_out.Write("ConstMaterialProperties",TObject::kSingleKey);
	if(Opt[0])
		H1_NOp -> Write();
	if(Opt[1])
		H1_OpProcID -> Write();
	if(Opt[2]){
		for(int a=0; a<n_opt; a++){
			H1_OpWav[a] -> Write();
			H1_OpE[a] -> Write();
		}
	}
	if(Opt[3]){
		H1_OpTime -> Write();
		H1_PostOpTime -> Write();
	}
	if(Opt[4]) {}
	if(Opt[5]){
		H2_Edep_LY -> Write();
		H2_dEdx_dLdx -> Write();
	}
	if(Opt[6])
		H1_OpTrackLength -> Write();
	if(Opt[7])	{
		H1_NOpSiPM[L] -> Write();
		H1_NOpSiPM[R] -> Write();
	}
	if(Opt[8]){
		H1_OpSiPMTime[L] -> Write();
		H1_OpSiPMTime[R] -> Write();
	}
	F -> Close();
	G -> Close();
}
