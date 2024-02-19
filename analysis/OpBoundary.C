void OpBoundary
(
 const char* particle = "alpha",
 const char* energy = "5.48MeV",
 const char* suffix = "Boundary_v3"
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
	const int max = 1000;
	// OpTrack data
	int NOp;
	double o_px[max], o_py[max], o_pz[max];

	int PostNOp;
	int op_ProcID[max];
	double op_px[max], op_py[max], op_pz[max];

	// OpBoundary data
	int NOpBoundary;
	int ob_ProcID[max];
	double ob_px[max], ob_py[max], ob_pz[max];
//	vector<int> *ob_ProcID;
//	vector<int> *ob_TrackID;
//	vector<int> *ob_TrackID, *ob_ProcID;
//	vector<double> *ob_px, *ob_py, *ob_pz, *ob_vx, *ob_vy, *ob_vz, *ob_Time;
//
	// link branch
//	if(map_parameters["OpTrack"] == "true")
//	{
//		cout << "OpTrack was called" << endl;
		T -> SetBranchAddress("NOp",&NOp);
		T -> SetBranchAddress("OpPX",o_px);
		T -> SetBranchAddress("OpPY",o_py);
		T -> SetBranchAddress("OpPZ",o_pz);
//	}
	T -> SetBranchAddress("PostNOp",&PostNOp);
	T -> SetBranchAddress("PostProcID",op_ProcID);
	T -> SetBranchAddress("PostOpPX",op_px);
	T -> SetBranchAddress("PostOpPY",op_py);
	T -> SetBranchAddress("PostOpPZ",op_pz);
//	if(map_parameters["OpBoundary"] == "true")
	{
		T -> SetBranchAddress("NOpBoundary",&NOpBoundary);
		T -> SetBranchAddress("OpProcIDBoundary",ob_ProcID);
		T -> SetBranchAddress("OpVXBoundary",ob_px);
		T -> SetBranchAddress("OpVYBoundary",ob_py);
		T -> SetBranchAddress("OpVZBoundary",ob_pz);
	}

	// define output histogram container
	TFile* G = new TFile(Form("out_root/H_Op_%s",infile),"recreate");
	enum {Gen,tot_refl,transmit,fre_refl};
	char* str_proc[] = {"Gen","Total_reflection","Transmission","Fresnel_reflection"};
	char* str_axis[] = {"px-py","py-pz","px-pz"};
	const int nproc = sizeof(str_proc)/sizeof(str_proc[0]);
	TH2F* H2_p[3][nproc];
	cout << nproc << endl;
	for(int a=0; a<nproc; a++)
	{
		H2_p[0][a] = new TH2F(Form("H2_%s_%s",str_proc[a],str_axis[0]),"",100,-7.,7.,100,-7.,7.);
		H2_p[1][a] = new TH2F(Form("H2_%s_%s",str_proc[a],str_axis[1]),"",100,-7.,7.,100,-7.,7.);
		H2_p[2][a] = new TH2F(Form("H2_%s_%s",str_proc[a],str_axis[2]),"",100,-7.,7.,100,-7.,7.);
	}

	// event loop
	cout << "nEntries: " << T -> GetEntriesFast() << endl;
//	for(int a=0; a<3; a++)
	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		if(a%100==0)
			cout << "Processing " << a << "th event" << endl;
		T -> GetEntry(a);
		for(int b=0; b<NOp; b++)
		{
//			cout << o_px[b] << " " << o_py[b] << " " << o_pz[b] << endl;
			H2_p[0][0] -> Fill(o_px[b]*1e6,o_py[b]*1e6);
			H2_p[1][0] -> Fill(o_py[b]*1e6,o_pz[b]*1e6);
			H2_p[2][0] -> Fill(o_px[b]*1e6,o_pz[b]*1e6);
		}
		for(int b=0; b<PostNOp; b++)
		{
			if(op_ProcID[b] == 31)
			{
				H2_p[0][1] -> Fill(op_px[b]*1e6,op_py[b]*1e6);
				H2_p[1][1] -> Fill(op_py[b]*1e6,op_pz[b]*1e6);
				H2_p[2][1] -> Fill(op_px[b]*1e6,op_pz[b]*1e6);
			}
		}
		for(int b=0; b<NOpBoundary; b++)
		{
			if(ob_ProcID[b] == -2) // transmission
			{
				cout << ob_px[b] << " " << ob_py[b] << " " << ob_pz[b] << endl;
				H2_p[0][2] -> Fill(ob_px[b]*1e6,ob_py[b]*1e6);
				H2_p[1][2] -> Fill(ob_py[b]*1e6,ob_pz[b]*1e6);
				H2_p[2][2] -> Fill(ob_px[b]*1e6,ob_pz[b]*1e6);
			}
			if(ob_ProcID[b] == -3) // fresnel
			{
				H2_p[0][3] -> Fill(ob_px[b]*1e6,ob_py[b]*1e6);
				H2_p[1][3] -> Fill(ob_py[b]*1e6,ob_pz[b]*1e6);
				H2_p[2][3] -> Fill(ob_px[b]*1e6,ob_pz[b]*1e6);
			}
		}
	}
	F -> Close();

	//--------------------------------
//	char* str_proc[] = {"Gen","Total_reflection","Transmission","Fresnel_reflection"};
//	char* str_axis[] = {"px-py","py-pz","px-pz"};
//	TCanvas* c1 = new TCanvas("c1","c1",1.2*600*nproc,600*3);
//	TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
//	H2_p[0][0] -> Draw("colz");
//	c1 -> Divide(nproc,3);
	for(int a=0; a<nproc; a++)
	for(int b=0; b<3; b++)
	{
		H2_p[b][a] -> Write();
//		cout << H2_p[b][a] -> GetEntries() << endl;
//		c1 -> cd(nproc*b + nproc + 1);
//		gPad -> SetMargin(.13,.05,.12,.05);
//		H2_p[0][0] -> Draw("colz");
	}
}
