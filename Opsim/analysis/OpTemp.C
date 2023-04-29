void OpTemp
(
 const char* particle = "temp",
 const char* energy = "",
 const char* suffix = ""
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
	const int max = 20000;
	// MCStep data
	int nStep;
	vector<int> *s_ID, *s_FromHit, *s_ProcID, *s_PDG, *s_PrevDetID, *s_PostDetID;
	vector<int> *s_IsBoundary, *s_NSecondOP;
	vector<double> *s_PrevKE, *s_Edep, *s_Length, *s_vx, *s_vy, *s_vz;

	// OpTrack data
	int NOp;
	int o_ID[max], o_ProcID[max], o_ParentID[max], o_DetID[max];
	double o_px[max], o_py[max], o_pz[max], o_vx[max], o_vy[max], o_vz[max], o_KE[max], o_Time[max];
//	vector<int> *o_ID, *o_ProcID, *o_ParentID, *o_DetID;
//	vector<double> *o_px, *o_py, *o_pz, *o_vx, *o_vy, *o_vz, *o_KE, *o_Time;

	// OpPostTrack data
	int PostNOp;
	vector<int> *op_ID, *op_ProcID, *op_DetID;
	vector<double> *op_px, *op_py, *op_pz, *op_vx, *op_vy, *op_vz, *op_KE, *op_Time, *op_Length;

	// OpSiPM data
	int NOpSiPM;
	vector<int> *os_ProcID, *os_DetID;
	vector<double> *os_px, *os_py, *os_pz, *os_vx, *os_vy, *os_vz, *os_KE, *os_Time;

	// Link container
	if(map_parameters["MCStep"] == "true" && false)
	{
		cout << "MCStep was called" << endl;
		T -> SetBranchAddress("nStep",&nStep);
		T -> SetBranchAddress("StepTrackID",&s_ID);
		T -> SetBranchAddress("StepFromHit",&s_FromHit);
		T -> SetBranchAddress("StepProcID",&s_ProcID);
		T -> SetBranchAddress("StepTrackPDG",&s_PDG);
		T -> SetBranchAddress("StepPrevDetID",&s_PrevDetID);
		T -> SetBranchAddress("StepPostDetID",&s_PostDetID);
		T -> SetBranchAddress("IsBoundary",&s_IsBoundary);
		T -> SetBranchAddress("StepNSecondaryOP",&s_NSecondOP);
		T -> SetBranchAddress("StepPrevKE",&s_PrevKE);
		T -> SetBranchAddress("StepEdep",&s_Edep);
		T -> SetBranchAddress("StepLength",&s_Length);
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
	if(map_parameters["OpPostTrack"] == "true" && false)
	{
		cout << "OpPostTrack was called" << endl;
		T -> SetBranchAddress("PostNOp",&PostNOp);
		T -> SetBranchAddress("PostOpTrackID",&op_ID);
		T -> SetBranchAddress("PostProcID",&op_ProcID);
		T -> SetBranchAddress("PostOpDetID",&op_DetID);
		T -> SetBranchAddress("PostOpPX",&op_px);
		T -> SetBranchAddress("PostOpPY",&op_py);
		T -> SetBranchAddress("PostOpPZ",&op_pz);
		T -> SetBranchAddress("PostOpVX",&op_vx);
		T -> SetBranchAddress("PostOpVY",&op_vy);
		T -> SetBranchAddress("PostOpVZ",&op_vz);
		T -> SetBranchAddress("PostOpKEnergy",&op_KE);
		T -> SetBranchAddress("PostOpTime",&op_Time);
		T -> SetBranchAddress("OpTrackLength",&op_Length);
	}
	if(map_parameters["OpSiPM"] == "true" && false)
	{
		cout << "OpSiPM was called" << endl;
		T -> SetBranchAddress("NOpSiPM",&NOpSiPM);
		T -> SetBranchAddress("OpSiPMProcID",&os_ProcID);
		T -> SetBranchAddress("OpSiPMDetID",&os_DetID);
		T -> SetBranchAddress("OpSiPMPX",&os_px);
		T -> SetBranchAddress("OpSiPMPY",&os_py);
		T -> SetBranchAddress("OpSiPMPZ",&os_pz);
		T -> SetBranchAddress("OpSiPMVX",&os_vx);
		T -> SetBranchAddress("OpSiPMVY",&os_vy);
		T -> SetBranchAddress("OpSiPMVZ",&os_vz);
		T -> SetBranchAddress("OpSiPMEnergy",&os_KE);
		T -> SetBranchAddress("OpSiPMTime",&os_Time);
	}

	cout << "Entries: " << T -> GetEntriesFast() << endl;
	// event loop
	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		if(a%100 == 0)
			cout << "Processing " << a << " th event" << endl;
		T -> GetEntry(a);
		cout << "done" << endl;
	}// event
	L_out.Write("ConstMaterialProperties",TObject::kSingleKey);
	F -> Close();
	// define output root file
	const char* output_prefix = "out_root";
	TFile* G = new TFile(Form("%s/H_Op_%s",output_prefix,infile),"recreate");

	G -> Close();
}
