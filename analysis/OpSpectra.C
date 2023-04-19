#define max 20000
void OpSpectra
(
 const char* particle = "proton",
 const char* energy = "300MeV",
 const char* suffix = ""
 )
{
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

	int NOp;
	int o_TrackID[max], o_ProcID[max], o_ParentID[max], o_DetID[max];
	double o_px[max], o_py[max], o_pz[max], o_vx[max], o_vy[max], o_vz[max];
	double o_KE[max], o_Time[max];

	if(map_parameters["OpTrack"] == "true")
	{
		cout << "OpTrack was called" << endl;
		T -> SetBranchAddress("NOp",&NOp);
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

	// define output histogram container
	TFile* G = new TFile(Form("out_root/H_Op_%s",infile),"recreate");
	enum {Total,Scint,Cheren};
	const char* cre_opt[] = {"Total","Scint","Ceren"};
	const int n_opt = sizeof(cre_opt)/sizeof(cre_opt[0]);
	TH1F* H1_OpWav[n_opt];
	TH1F* H1_OpE[n_opt];
	for(int a=0; a<n_opt; a++)
	{
		H1_OpWav[a] = new TH1F(Form("H1_OpWav_%s",cre_opt[a]),"",600,300,900);
		H1_OpE[a] = new TH1F(Form("H1_OpE_%s",cre_opt[a]),"",350,1,4.5);
	}

	// event loop
	for(int a=0; a<T -> GetEntriesFast(); a++)
	{
		if(a%100 == 0)
			cout << "Processing " << a << " th event" << endl;
		T -> GetEntry(a);
		for(int b=0; b<NOp; b++)
		{
			double wav = 1.2398e-3 / o_KE[b];
			double E = 1e6*o_KE[b];
			H1_OpE[Total] -> Fill(E);
			H1_OpWav[Total] -> Fill(wav);
			if(o_ProcID[b] == 21){ // cherenkov
				H1_OpE[Cheren] -> Fill(E);
				H1_OpWav[Cheren] -> Fill(wav);
			}else if (o_ProcID[b] == 22){ // scintillation
				H1_OpE[Scint] -> Fill(E);
				H1_OpWav[Scint]->Fill(wav);
			}	else {
				cout << "other process: " << o_ProcID[b] << endl;
			}
		}
	}// event
	L_out.Write("ConstMaterialProperties",TObject::kSingleKey);
	for(int a=0; a<n_opt; a++)
	{
		H1_OpE[a] -> Write();
		H1_OpWav[a] -> Write();
	}
	F -> Close();
	G -> Close();
}
