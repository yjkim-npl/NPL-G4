void OpBoundary
(
 const char* particle = "temp",
 const char* energy = "",
 const char* suffix = ""
)
{
	// load input data root file
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

	TFile* F = new TFile(Form("data_root/%s",infile),"read");
	TTree* T = (TTree*) F -> Get("Opsim");
	TList* L_proc = (TList*) F -> Get("ProcessTable");
	TList* L_para = (TList*) F -> Get("InputParameters");
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
	}

	// define container
	// OpTrack data
	int NOp;
	vector<int> *o_TrackID, *o_ProcID, *o_ParentID, *o_DetID;
	vector<double> *o_px, *o_py, *o_pz, *o_vx, *o_vy, *o_vz;
	vector<double> *o_KE, *o_Time;

	// OpBoundary data
	int NOpBoundary;
	vector<int> *ob_TrackID, *ob_ProcID;
	vector<double> *ob_px, *ob_py, *ob_pz, *ob_vx, *ob_vy, *ob_vz, *ob_Time;

	// link branch
	if(map_parameters["OpTrack"] == "true")
	{
		cout << "OpTrack was called" << endl;
		T -> SetBranchAddress("NOp",&NOp);
		T -> SetBranchAddress("OpTrackID",&o_TrackID);
//		T -> SetBranchAddress("OpProcessID",&o_ProcID);
//		T -> SetBranchAddress("OpParentID",&o_ParentID);
//		T -> SetBranchAddress("OpDetID",&o_DetID);
//		T -> SetBranchAddress("OpPX",&o_px);
//		T -> SetBranchAddress("OpPY",&o_py);
//		T -> SetBranchAddress("OpPZ",&o_pz);
//		T -> SetBranchAddress("OpVX",&o_vx);
//		T -> SetBranchAddress("OpVY",&o_vy);
//		T -> SetBranchAddress("OpVZ",&o_vz);
//		T -> SetBranchAddress("OpKEnergy",&o_KE);
//		T -> SetBranchAddress("OpTime",&o_Time);
	}
	if(map_parameters["OpBoundary"] == "true")
	{
		cout << "OpBoundary was called" << endl;
		T -> SetBranchAddress("NOpBoundary",&NOpBoundary);
//		T -> SetBranchAddress("OpTrackIDBoundary",&ob_TrackID);
//		T -> SetBranchAddress("OpProcIDBoundary",&ob_ProcID);
//		T -> SetBranchAddress("OpPXBoundary",&ob_px);
//		T -> SetBranchAddress("OpPYBoundary",&ob_py);
//		T -> SetBranchAddress("OpPZBoundary",&ob_pz);
//		T -> SetBranchAddress("OpVXBoundary",&ob_vx);
//		T -> SetBranchAddress("OpVYBoundary",&ob_vy);
//		T -> SetBranchAddress("OpVZBoundary",&ob_vz);
//		T -> SetBranchAddress("OpTBoundary",&ob_Time);
	}

	// define output histogram container
	TFile* G = new TFile(Form("out_root/H_Op_%s",infile),"recreate");
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

	// event loop
	for(int a=0; a<1; a++)
//	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		if(a%100==0)
			cout << "Processing " << a << "th event" << endl;
		T -> GetEntry(a);
	}
	F -> Close();
	G -> Close();
}
