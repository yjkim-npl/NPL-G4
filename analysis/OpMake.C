inline double FindEff(double wav, vector<double> vec_wav, vector<double> vec_eff)
{
	double eff;
	int idx;
	for(int a=0; a<vec_wav.size(); a++)
	{
		if(wav < vec_wav[a]-10)
			continue;
		if(wav > vec_wav[a])
		{
			idx = a;
			break;
		}
	}
	eff = vec_eff[idx];
	return eff;
}

void OpMake
(
 const char* particle = "proton",
 const char* energy   = "100MeV",
 const char* suffix   = "gap_LR"
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
		 9. 1-D Time difference at LR,UD              (OpSiPM)
		 10. 1-D ratio of photons                     (OpTrack && OpPostTrack && OpSiPM && OpBoundary)
		     total, absorbed, transmitted to SiPM and efficiency, geom
		 11. 2-D # of detected photon vs. Timing resolution (OpSiPM)
	 */
	const int n_Hist = 12;
	bool Opt[n_Hist]; fill_n(Opt,n_Hist,1);
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

	// load SiPM efficiency csv file
	ifstream in("SiPMEff.csv");
	string line;
	getline(in,line); // title " wavlen efficiecny"
	vector<double> vec_wav, vec_eff, vec_E;
	while(getline(in,line))
	{
		stringstream ss(line);
		double wavlen, E, eff;
		ss >> wavlen >> E >> eff;
		vec_wav.push_back(wavlen);
		vec_eff.push_back(eff);
		vec_E.push_back(E);
	}
	in.close();

	// use random number generator for using efficiency
	srand((unsigned int) time(NULL));

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
		if(((TString)name->GetName()).Contains("SiPM"))
			L_out.Add(name);
	}
	
	// Define the data container
	const int m_s = 2.5e4;
	const int m_o = 1e4;
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

	// OpBoundary
	int NOpBoundary;
	int ob_ID[m_s], ob_ProcID[m_s];
	double ob_px[m_s], ob_py[m_s], ob_pz[m_s], ob_vx[m_s], ob_vy[m_s], ob_vz[m_s], ob_Time[m_s];

	// Link container
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
	if(map_parameters["OpBoundary"] == "true")
	{
		cout << "OpBoundary was called" << endl;
		T -> SetBranchAddress("NOpBoundary",&NOpBoundary);
		T -> SetBranchAddress("OpTrackIDBoundary",ob_ID);
		T -> SetBranchAddress("OpProcIDBoundary",ob_ProcID);
		T -> SetBranchAddress("OpPXBoundary",ob_px);
		T -> SetBranchAddress("OpPYBoundary",ob_py);
		T -> SetBranchAddress("OpPZBoundary",ob_pz);
		T -> SetBranchAddress("OpVXBoundary",ob_vx);
		T -> SetBranchAddress("OpVYBoundary",ob_vy);
		T -> SetBranchAddress("OpVZBoundary",ob_vz);
		T -> SetBranchAddress("OpTBoundary",ob_Time);
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
	enum {Le,Re,Ue,De};
	const char* str_SiPM_opt[] = {"Le","Re","Ue","De"};
	const int n_SiPM_opt = sizeof(str_SiPM_opt)/sizeof(str_SiPM_opt[0]);
	TH1F* H1_NOpSiPM[n_SiPM_opt];
	for(int a=0; a<n_SiPM_opt; a++)
	{
		if((a > 1 && map_parameters["SiPMUD"] != "true") || (a < 2 && map_parameters["SiPMLR"] != "true"))
			continue;
		H1_NOpSiPM[a] = new TH1F(Form("H1_NOpSiPM_%s",str_SiPM_opt[a]),"",5000,0,5000);
	}
	// HIST 8
	TH1F* H1_OpSiPMTime[n_SiPM_opt];
	for(int a=0; a<n_SiPM_opt; a++)
	{
		if((a > 1 && map_parameters["SiPMUD"] != "true") || (a < 2 && map_parameters["SiPMLR"] != "true"))
			continue;
		H1_OpSiPMTime[a] = new TH1F(Form("H1_OpSiPMTime_%s",str_SiPM_opt[a]),"",1000,0,50);
	}
	// HIST 9
	enum {LR,UD};
	const char* str_SiPM_pos[] = {"LR","UD"};
	const int n_pos = sizeof(str_SiPM_pos)/sizeof(str_SiPM_pos[0]);
	TH1F* H1_SiPMTimeDiff[n_pos];
	for(int a=0; a<n_pos; a++)
	{
		if(map_parameters[Form("SiPM%s",str_SiPM_pos[a])] == "true")
			H1_SiPMTimeDiff[a] = new TH1F(Form("H1_SiPMTimeDiff_%s",str_SiPM_pos[a]),"",10000,-50,50);
	}
	// HIST 10
	TH1F* H1_ratio = new TH1F("H1_ratio","",6,0,6);
	// HIST 11
	TH2F* H2_dLY_TimeRes = new TH2F(Form("H2_dLY_TimeRes"),"",5000,0,5000,200,-2,2);

	// event loop
	int o_gen = 0;
	int o_die = 0;
	int o_abs1 = 0;
	int o_abs2 = 0;
	int o_trans = 0;
	int o_trans_to_world=0;
	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		if(a%100 == 0)
			cout << "Processing " << a << " th event" << endl;
		T -> GetEntry(a);
		if(Opt[0])
			H1_NOp -> Fill(NOp);
		if(Opt[10])
		{
			int gen = H1_ratio -> GetBinContent(1) + NOp;
			H1_ratio -> SetBinContent(1,gen);
			o_gen += NOp;
		}
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
			if(op_DetID[b] == 201)
			{
				if(Opt[3])
					H1_PostOpTime -> Fill(op_Time[b]);
				if(Opt[6])
					H1_OpTrackLength -> Fill(op_Length[b]);
				if(Opt[10]){
					H1_ratio -> Fill(1);
					o_abs1++;
				}
			}else{
				if(Opt[10]){
					H1_ratio -> Fill(2);
					o_trans_to_world++;
				}
			}
			if(op_ProcID[b] == 31 && Opt[10])
			{
				o_abs2++;
//				H1_ratio -> Fill(3);
			}
		}// each optical photon at PostOpTrack

		// container for calculation of energy weighted time 
		int NOp_Le = 0;	// for opt 7
		int NOp_Re = 0;
		int NOp_Ue = 0;
		int NOp_De = 0;
		double MeanTime_Le = 0;
		double MeanTime_Re = 0;
		double MeanTime_Ue = 0;
		double MeanTime_De = 0;
		double MeanEnergy_Le = 0;
		double MeanEnergy_Re = 0;
		double MeanEnergy_Ue = 0;
		double MeanEnergy_De = 0;

		if(Opt[10])
		{
			int trans = H1_ratio -> GetBinContent(3) + NOpSiPM;
//			H1_ratio -> SetBinContent(3, trans);
		}
		for(int b=0; b<NOpSiPM; b++)
		{
			double wav = 1.2398e-3/os_KE[b];
			int R_eff = rand()%100;
			int R_geom;
			if(map_parameters["SiPM_gapIn"] == "true")
			{
				R_geom = 0;
//				cout << "gap in inserted!" << endl;
			}
			else
				R_geom = rand()%100;
			if(Opt[10])
			{
				H1_ratio -> Fill(3);
				if(R_geom < 65)
				{
					H1_ratio -> Fill(4);
					if(R_eff < 100*FindEff(wav,vec_wav,vec_eff))
						H1_ratio -> Fill(5);
				}
			}

			if(Opt[7] || Opt[9] || Opt[11])
			{
				if(R_eff > 100* FindEff(wav,vec_wav,vec_eff) || R_geom > 65)
					continue;
				if(os_DetID[b]/1000 == 11){
					NOp_Le++;	// Left + eff + geom
				}else if (os_DetID[b]/1000 == 12){
					NOp_Re++; // Right + eff + geom
				}else if (os_DetID[b]/1000 == 13){
					NOp_Ue++; // Up + eff + geom
				}else if (os_DetID[b]/1000 == 14){
					NOp_De++; // Down + eff + geom
				}else{
					cout << a << " Opt[7] || Opt[9] :: DetID is not assigned in LRUD: " << os_DetID[b] <<  endl;
					continue;
				}
			}
			if(Opt[8] || Opt[9])
			{
				if(R_eff > 100*FindEff(wav,vec_wav,vec_eff) || R_geom > 65)
					continue;
				if(os_DetID[b]/1000 == 11){
					MeanTime_Le += os_KE[b]*os_Time[b];
					MeanEnergy_Le += os_KE[b];
				}else if(os_DetID[b]/1000 == 12){
					MeanTime_Re += os_KE[b]*os_Time[b];
					MeanEnergy_Re += os_KE[b];
				}else if(os_DetID[b]/1000 == 13){
					MeanTime_Ue += os_KE[b]*os_Time[b];
					MeanEnergy_Ue += os_KE[b];
				}else if(os_DetID[b]/1000 == 14){
					MeanTime_De += os_KE[b]*os_Time[b];
					MeanEnergy_De += os_KE[b];
				} else {
					continue;
				}
			}
		}// each optical photon at OpSiPM
		for(int b=0; b<NOpBoundary; b++)
		{
			if(ob_ProcID[b] == -2 && Opt[10]){
				H1_ratio -> Fill(5);
				o_trans++;
			}
		}
		if(Opt[7])
		{
			if(map_parameters["SiPMLR"] == "true")
			{
				H1_NOpSiPM[Le] -> Fill(NOp_Le);
				H1_NOpSiPM[Re] -> Fill(NOp_Re);
			}
			if(map_parameters["SiPMUD"] == "true")
			{
				H1_NOpSiPM[Ue] -> Fill(NOp_Ue);
				H1_NOpSiPM[De] -> Fill(NOp_De);
			}
		}
		if(Opt[8] || Opt[9] || Opt[11])
		{
			MeanTime_Le /= MeanEnergy_Le;
			MeanTime_Re /= MeanEnergy_Re;
			MeanTime_Ue /= MeanEnergy_Ue;
			MeanTime_De /= MeanEnergy_De;
			if(Opt[8])
			{
				if(map_parameters["SiPMLR"] == "true")
				{
					H1_OpSiPMTime[Le] -> Fill(MeanTime_Le);
					H1_OpSiPMTime[Re] -> Fill(MeanTime_Re);
				}
				if(map_parameters["SiPMUD"] == "true")
				{
					H1_OpSiPMTime[Ue] -> Fill(MeanTime_Ue);
					H1_OpSiPMTime[De] -> Fill(MeanTime_De);
				}
			}
			if(Opt[9])
			{
				double Time_diff_LR = MeanTime_Le - MeanTime_Re;
				double Time_diff_UD = MeanTime_Ue - MeanTime_De;
				if(map_parameters["SiPMLR"] == "true")
					H1_SiPMTimeDiff[LR] -> Fill(Time_diff_LR);
				if(map_parameters["SiPMUD"] == "true")
					H1_SiPMTimeDiff[UD] -> Fill(Time_diff_UD);
			}
			if(Opt[11])
			{
				double NOp_eg = 0.25*NOp_Le + 0.25*NOp_Re + 0.25*NOp_Ue + 0.25*NOp_De;
				double Time_diff_eg = MeanTime_Le - MeanTime_Re;
				H2_dLY_TimeRes -> Fill(NOp_eg,Time_diff_eg);
			}
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
		for(int a=0; a<n_SiPM_opt; a++)
		{
			if((a > 1 && map_parameters["SiPMUD"] != "true") || (a < 2 && map_parameters["SiPMLR"] != "true"))
				continue;
			H1_NOpSiPM[a] -> Write();
		}
	}
	if(Opt[8]){
		for(int a=0; a<n_SiPM_opt; a++)
		{
			if((a > 1 && map_parameters["SiPMUD"] != "true") || (a < 2 && map_parameters["SiPMLR"] != "true"))
				continue;
			H1_OpSiPMTime[a] -> Write();
		}
	}
	if(Opt[9]){
		if(map_parameters["SiPMLR"] == "true")
		{
			H1_SiPMTimeDiff[LR] -> Write();
			cout << "LR was created" << endl;
		}
		if(map_parameters["SiPMUD"] == "true")
		{
			H1_SiPMTimeDiff[UD] -> Write();
			cout << "UD was created" << endl;
		}
	}
	if(Opt[10]){
		H1_ratio -> Write();
	}
	if(Opt[11]){
		H2_dLY_TimeRes -> Write();
	}
	F -> Close();
	G -> Close();
//	cout << "gen: " << o_gen << endl;
//	cout << "die: " << o_die << endl;
//	cout << "abs1: " << o_abs1 << endl;
//	cout << "abs2: " << o_abs2 << endl;
//	cout << "trans: " << o_trans << endl;
//	cout << "trans to world: " << o_trans_to_world << endl;
}
