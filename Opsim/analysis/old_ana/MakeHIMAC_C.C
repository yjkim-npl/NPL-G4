#define max 20000
inline int PDGtoIndex(const int pID)
{
	enum {pro,neu,ele,pos,gam,ELS};
	int idx = ELS;
	if      (pID == 2212) idx = pro;
	else if (pID == 2112) idx = neu;
	else if (pID ==   11) idx = ele;
	else if (pID ==  -11) idx = pos;
	else if (pID ==   22) idx = gam;
	else                  idx = ELS;
	return idx;
}
inline int DetIDtoIndex(const int detID)
{
	enum {SC1,SC2,SC3,BDC,BTOF1,BTOF2,ELS};
	int idx = ELS;
	if      (detID == 101) idx = SC1;
	else if (detID == 201) idx = SC2;
	else if (detID == 301) idx = SC3;
	else if (detID == 401) idx = BDC;
	else if (detID == 501) idx = BTOF1;
	else if (detID == 502) idx = BTOF2;
	else                   idx = ELS;
	return idx;
}

void MakeHIMAC_C
(char* p = "temp",
 char* e = "",
 char* s = ""
)
{
	const int opt = 0;
	const char* path = "data_root";
	char* infile;
	TString str_p = p;
	TString str_s = s;
	if(str_p.Contains("temp")){
		if(str_s=="") infile = Form("out_%s.root",p);
		else          infile = Form("out_%s_%s.root",p,s);
	}else{
		if(str_s=="") infile = Form("out_%s_%s.root",p,e);
		else          infile = Form("out_%s_%s_%s.root",p,e,s);
	}
	cout << "infile: " << Form("%s/%s",path,infile) << endl;
	TFile* F = new TFile(Form("%s/%s",path,infile),"read");
	TList* InputParameters = (TList*) F -> Get("InputParameters");
	TTree* T = (TTree*) F -> Get("Opsim");

	map<string,string> map_parameters;
	for(int a=0; a<InputParameters->GetSize(); a++)
	{
		TNamed* name = (TNamed*) InputParameters->At(a);
		map_parameters.insert(make_pair(name->GetName(),name->GetTitle()));
	}

	// data container
	int nTracks, nSteps;
	vector<int> *trackID, *parentID, *trackPDG, *trackDetID;
	vector<double> *px, *py, *pz, *vx, *vy, *vz, *kE;
	vector<int> *stepTrackID, *stepProcID, *stepTrackPDG, *stepPrevDetID, *stepPostDetID;
	vector<int> *IsBoundary, *stepFromHit;
	vector<double> *stepvx,*stepvy,*stepvz,*Edep,*stepPrevKE;

	if(map_parameters["MCTrack"] == "true")
	{
		T->SetBranchAddress("nTrack",&nTracks);
		T->SetBranchAddress("TrackID",&trackID);
		T->SetBranchAddress("ParentID",&parentID);
		T->SetBranchAddress("TrackPDG",&trackPDG);
		T->SetBranchAddress("TrackPX",&px);
		T->SetBranchAddress("TrackPY",&py);
		T->SetBranchAddress("TrackPZ",&pz);
		T->SetBranchAddress("TrackVX",&vx);
		T->SetBranchAddress("TrackVY",&vy);
		T->SetBranchAddress("TrackVZ",&vz);
		T->SetBranchAddress("TrackKEnergy",&kE);
	}
	if(map_parameters["MCStep"] == "true")
	{
		T->SetBranchAddress("nStep",&nSteps);
		T->SetBranchAddress("StepTrackID",&stepTrackID);
		T->SetBranchAddress("StepFromHit",&stepFromHit);
		T->SetBranchAddress("StepProcID",&stepProcID);
		T->SetBranchAddress("StepTrackPDG",&stepTrackPDG);
		T->SetBranchAddress("StepPrevDetID",&stepPrevDetID);
		T->SetBranchAddress("StepPostDetID",&stepPostDetID);
		T->SetBranchAddress("IsBoundary",&IsBoundary);
		T->SetBranchAddress("StepPrevKE",&stepPrevKE);
		T->SetBranchAddress("StepVX",&stepvx);
		T->SetBranchAddress("StepVY",&stepvy);
		T->SetBranchAddress("StepVZ",&stepvz);
		T->SetBranchAddress("StepEdep",&Edep);
	}

	enum {pro,neu,ele,pos,gam,pELS};
	enum {SC1,SC2,SC3,BDC,BTOF1,BTOF2,dELS};
	const char* particle[] = {"pro","neu","ele","pos","gam","pELS"};
	const char* det[] = {"SC1","SC2","SC3","BDC","BTOF1","BTOF2","dELS"};
	const int n_particle = sizeof(particle)/sizeof(particle[0]);
	const int n_det = sizeof(det)/sizeof(det[0]);

	TFile* G = new TFile(Form("out_root/H_%s",infile),"recreate");
	// HIST 1 energy deposition
	TH1F* H1_Edep[n_particle][n_det];
	// HIST 2 energy profile
	TH1F* H1_E[n_particle][n_det];
	for(int a=0; a<n_particle; a++)
	for(int b=0; b<n_det; b++)
	{
		H1_Edep[a][b] = new TH1F(Form("H1_Edep_%s_%s",particle[a],det[b]),"",1000,0,100);
		H1_E[a][b] = new TH1F(Form("H1_E_%s_%s",particle[a],det[b]),"",2500,0,2500);
	}

	// event loop
//	for(int a=0; a<T->GetEntriesFast();a++)
	for(int a=0; a<1;a++)
	{
//		T->GetEntry(a);
		if(a%10==0)
			cout << "Processing " << a << "th event" << endl;
		map<int,double> map_TrackID_EdepSum[n_det];
		vector<int> vec_TrkID;
		vector<int> vec_PDG;
		vector<int> vec_prevTrkID;
		vector<int> vec_prevPDG;
		vector<double> vec_prevKE;
		vector<int> vec_postDetIdx;
		map<int,double> map_TrkID_EdepSum[n_det];
		for(int b=0; b<nSteps; b++)
		{
			const int trkID = stepTrackID->at(b);
			const int boundary = IsBoundary->at(b);
			const int fromHit = stepFromHit->at(b);
			const int trkPDG = PDGtoIndex(stepTrackPDG->at(b));
			const int prevDetIdx = DetIDtoIndex(stepPrevDetID->at(b));
			const int postDetIdx = DetIDtoIndex(stepPostDetID->at(b));
			const double edep = Edep->at(b);
			const double prevKE = stepPrevKE->at(b);
			if(fromHit == 0 && prevDetIdx != dELS) 
			{
				map_TrackID_EdepSum[prevDetIdx][trkID] += edep;
				vec_TrkID.push_back(trkID);
				vec_PDG.push_back(trkPDG);
			}
			if(fromHit == 0 && prevDetIdx == dELS && postDetIdx != dELS)
			{
				vec_prevTrkID.push_back(trkID);
				vec_prevPDG.push_back(trkPDG);
				vec_prevKE.push_back(prevKE);
				vec_postDetIdx.push_back(postDetIdx);
			}
			if(fromHit == 0 && prevDetIdx == SC2 && true)
			{
				cout << "EventNo: " << a << endl;
				cout << "TrkID: " << trkID << "(" << particle[trkPDG] << ")" << endl;
				cout << "PrevNo: " << det[prevDetIdx] << " -> " << det[postDetIdx] << endl;
				cout << "PrevKE: " << prevKE << endl;
				cout << "Edep: " << edep << endl << endl;
			}
		}
		// Filling HIST 1
		for(int b=0; b<vec_TrkID.size(); b++)
		for(int c=0; c<n_det; c++)
		{
			const int trkID = vec_TrkID[b];
			const int pdg = vec_PDG[b];
			const double EdepSum = map_TrackID_EdepSum[c][trkID];
			if(EdepSum != 0)
			{
				H1_Edep[pdg][c] -> Fill(EdepSum);
			}
		}

		// Filling HIST 2
		for(int b=0; b<vec_prevTrkID.size(); b++)
		{
			const int trkID = vec_prevTrkID[b];
			const int pdg = vec_prevPDG[b];
			const double prevKE = vec_prevKE[b];
			const int postNo = vec_postDetIdx[b];
			H1_E[pdg][postNo] -> Fill(prevKE);
		}
	}
	for(int a=0; a<n_particle; a++)
	for(int b=0; b<n_det; b++)
	{
		H1_Edep[a][b] -> Write(); 
		H1_E[a][b] -> Write();
	}
	F -> Close();
	G -> Close();
}
