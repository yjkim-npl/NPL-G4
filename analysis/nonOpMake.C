int PDGtoIndex(int pID)
{
	enum {pro,neu,ele,ion};
	int idx=0;
	if(pID == 2212)       idx = pro;
	else if (pID == 2112) idx = neu;
	else if (pID == 11 || pID == -11) idx = ele;
	else if (pID > 1000000000) idx = ion;
	else idx = -1;
	return idx;
}

void nonOpMake(
		const char* particle = "proton",
		const char* energy = "100MeV",
		const char* suffix = "")
{
	/* HIST lists
		 0. 1-D[SCs][p,n,e,A] Edep distribution with pID on SCs
		 1. 1-D[SCs][] Time distribution with pID on SCs
		 2. 2-D[SCs] Generation process & pID on SCs
		 3. 2-D[SCs][] Step position on SC
	*/

	const int n_Hist = 1;
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

	cout << "#################" << endl;
	cout << "infile: " << infile << endl;
	cout << "#################" << endl;
	const char* data_prefix = Form("./data");

	// load SiPM efficiency csv file
	ifstream in("SiPMEff.csv");
	string line;
	getline(in,line); // dummy line (title)
	vector<double> vec_wav,vec_eff,vec_E;
	while(getline(in,line))
	{
		stringstream ss(line);
		double wavlen, E, eff;
		ss >> wavlen >> E >> eff;
		vec_wav.push_back(wavlen);
		vec_E.push_back(eff);
		vec_eff.push_back(E);
	}
	in.close();

	// load the data root file
	TFile* F = new TFile(Form("%s/%s",data_prefix,infile),"read");
	TList* L_para = (TList*) F -> Get("InputParameters");
	TTree* T = (TTree*) F -> Get("Opsim");
	map<string,string> map_para;
	for(int a=0; a<L_para->GetSize(); a++)
	{
		TNamed* name = (TNamed*) L_para->At(a);
		map_para.insert(make_pair(name->GetName(),name->GetTitle()));
//		cout << name -> GetName() << " " << name -> GetTitle() << endl;
	}
	TBranch* branch_t[13];
	TBranch* branch_p[11];
	TBranch* branch_s[12];

	// Define container for data
	int nTrack;
	vector<int> *t_ID, *t_ProcID, *t_ParentID, *t_PDG, *t_DetID;
	vector<double> *t_px, *t_py, *t_pz, *t_vx, *t_vy, *t_vz, *t_time, *t_KE;

	int nPostTrack;
	vector<int> *p_ID, *p_ProcID, *p_DetID;
	vector<double> *p_px, *p_py, *p_pz, *p_vx, *p_vy, *p_vz, *p_time, *p_KE;

	int nStep;
	vector<int> *s_ID, *s_FromHit, *s_ProcID, *s_PDG, *s_DetID;
	vector<double> *s_vx, *s_vy, *s_vz, *s_time, *s_Edep, *s_Length, *s_prevKE;


	if(map_para["MCTrack"] == "true")
	{
		cout << "MCTrack was called" << endl;
		T -> SetBranchAddress("nTrack",&nTrack);
		T -> SetBranchAddress("TrackID",&t_ID,&branch_t[0]);
//		T -> SetBranchAddress("TrackProcID",&t_ProcID,&branch_t[1]);
//		T -> SetBranchAddress("ParentID",&t_ParentID,&branch_t[2]);
//		T -> SetBranchAddress("TrackPDG",&t_PDG,&branch_t[3]);
//		T -> SetBranchAddress("TrackDetID",&t_DetID,&branch_t[4]);
//		T -> SetBranchAddress("TrackPX",&t_px,&branch_t[5]);
//		T -> SetBranchAddress("TrackPY",&t_py,&branch_t[6]);
//		T -> SetBranchAddress("TrackPZ",&t_pz,&branch_t[7]);
//		T -> SetBranchAddress("TrackVX",&t_vx,&branch_t[8]);
//		T -> SetBranchAddress("TrackVY",&t_vy,&branch_t[9]);
//		T -> SetBranchAddress("TrackVZ",&t_vz,&branch_t[10]);
//		T -> SetBranchAddress("TrackTime",&t_time,&branch_t[11]);
//		T -> SetBranchAddress("TrackKEnergy",&t_KE,&branch_t[12]);
	}
	if(map_para["MCPostTrack"] == "true")
	{
		cout << "MCPostTrack was called" << endl;
//		T -> SetBranchAddress("nPostTrack",&nPostTrack);
//		T -> SetBranchAddress("PostTrackID",&p_ID,&branch_p[0]);
//		T -> SetBranchAddress("PostTrackProcID",&p_ProcID,&branch_p[1]);
//		T -> SetBranchAddress("PostTrackDetID",&p_DetID,&branch_p[2]);
//		T -> SetBranchAddress("PostTrackPX",&p_px,&branch_p[3]);
//		T -> SetBranchAddress("PostTrackPY",&p_py,&branch_p[4]);
//		T -> SetBranchAddress("PostTrackPZ",&p_pz,&branch_p[5]);
//		T -> SetBranchAddress("PostTrackVX",&p_vx,&branch_p[6]);
//		T -> SetBranchAddress("PostTrackVY",&p_vy,&branch_p[7]);
//		T -> SetBranchAddress("PostTrackVZ",&p_vz,&branch_p[8]);
//		T -> SetBranchAddress("PostTrackTime",&p_time,&branch_p[9]);
//		T -> SetBranchAddress("PostTrackKEnergy",&p_KE,&branch_p[10]);
	}
	if(map_para["MCStep"] == "true")
	{
		cout << "MCStep was called" << endl;
//		T -> SetBranchAddress("nStep",&nStep);
//		T -> SetBranchAddress("StepTrackID",&s_ID,&branch_s[0]);
//		T -> SetBranchAddress("StepFromHit",&s_FromHit,&branch_s[1]);
//		T -> SetBranchAddress("StepProcID",&s_ProcID,&branch_s[2]);
//		T -> SetBranchAddress("StepTrackPDG",&s_PDG,&branch_s[3]);
//		T -> SetBranchAddress("StepPostDetID",&s_DetID,&branch_s[4]);
//		T -> SetBranchAddress("StepVX",&s_vx,&branch_s[5]);
//		T -> SetBranchAddress("StepVY",&s_vy,&branch_s[6]);
//		T -> SetBranchAddress("StepVZ",&s_vz,&branch_s[7]);
//		T -> SetBranchAddress("StepTime",&s_time,&branch_s[8]);
//		T -> SetBranchAddress("StepEdep",&s_Edep,&branch_s[9]);
//		T -> SetBranchAddress("StepLength",&s_Length,&branch_s[10]);
//		T -> SetBranchAddress("StepPrevKE",&s_prevKE,&branch_s[11]);
	}

	// output root file
	char* output_prefix = "./out_root";
	TFile* G = new TFile(Form("%s/H_%s",output_prefix,infile),"recreate");

	enum {SC1,SC2,SC3,SC4};
	char* str_SC[] = {"SC1","SC2","SC3","SC4"};
	const int nSC = sizeof(str_SC)/sizeof(str_SC[0]);
	enum {pro,neu,ele,ion};
	char* str_pID[] = {"pro","neu","ele","ion"};
	const int npID = sizeof(str_pID)/sizeof(str_pID[0]);
	// HIST 0
	TH1F* H1_Edep[nSC][npID];
	// HIST 1
	TH1F* H1_Time[nSC][npID];
	// HIST 2
	TH2F* H2_proc_pID[nSC];
	// HIST 3
	TH2F* H2_XYpos[nSC][npID];

	for(int a=0; a<nSC; a++)
	{
		H2_proc_pID[a] = 
			new TH2F(Form("H2_proc_pID_%s",str_SC[a]),"",100,0,100,4,0,4);
		for(int b=0; b<npID; b++)
		{
			H1_Edep[a][b] = 
				new TH1F(Form("H1_Edep_%s_%s",str_SC[a],str_pID[b]),"",100,0,100);
			H1_Time[a][b] = 
				new TH1F(Form("H1_Time_%s_%s",str_SC[a],str_pID[b]),"",100,0,100);
			H2_XYpos[a][b] = 
				new TH2F(Form("H2_XYpos_%s_%s",str_SC[a],str_pID[b]),"",
						300,-150,150,300,-150,150);
		}
	}

	// event loop
	for(Long64_t a=0; a<T->GetEntries(); ++a)
	{
		if(a%100 == 0)
			cout << "Processing " << a << " th event" << endl;

		T -> GetEntry(a);
//		Long64_t tentry = T->LoadTree(a);
//		branch_s -> GetEntry(tentry);
//		T -> GetEntry(a);
//		for(int b=0; b<nTrack; b++)
//		{
//		}
//		for(int b=0; b<nPostTrack; b++)
//		{
//		}
		for(int b=0; b<nStep; b++)
		{
			int pdg = s_PDG -> at(b);
			int idx = PDGtoIndex(pdg);
			int proc = s_ProcID -> at(b);
//			int detID = s_DetID -> at(b) - stoi(map_para["SCID"]);
			double edep = s_Edep -> at(b);
//			H1_Edep[detID][idx] -> Fill(edep);
		}
	}

	// Write
//	for(int a=0; a<nSC; a++)
//	{
//		H2_proc_pID[a] -> Write();
//		for(int b=0; b<npID; b++)
//		{
//			H1_Edep[a][b] -> Write();
//			H1_Time[a][b] -> Write();
//			H2_XYpos[a][b] -> Write();
//		}
//	}
	F -> Close();
	G -> Close();
}
