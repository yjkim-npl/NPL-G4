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
	int idx = SC2;
	if      (detID == 101) idx = SC1;
	else if (detID == 201) idx = SC2;
	else if (detID == 301) idx = SC3;
	else if (detID == 401) idx = BDC;
	else if (detID == 501) idx = BTOF1;
	else if (detID == 502) idx = BTOF2;
	else                   idx = ELS;
	return idx;
}
void MakeHIMAC
(char* p = "ion",
 char* e = "2400MeV",
 char* s  =""
 )
{
	// opt 0: step from SteppingAction
	// opt 1: step from Hit obj
	cout << "done" << endl;
	const int opt = 0;
	// Link Input
	const char* path = "data_root";
	char* infile;
	TString str_p = p;
	TString str_s = s;
	if(str_p.Contains("temp")){
		if(str_s=="") infile = Form("out_%s.root",p);
		else          infile = Form("out_%s_%s.root",p,s);
	}else{
		if(str_s=="") infile = Form("out_%s_%s.root",p,e);
		else          infile = Form("out_%s_%s_%s.root",p,e);
	}
	cout<< Form("%s/%s",path,infile) << endl;
	TFile* F = new TFile(Form("%s/%s",path,infile),"read");
	TList* ProcessTable = (TList*) F -> Get("ProcessTable");
	TList* InputParameters = (TList*) F -> Get("InputParameters");
	TTree* T = (TTree*) F -> Get("Opsim");

	map<string,int> map_process;
	for(int a=0; a<ProcessTable->GetSize(); a++)
	{
		TNamed* name = (TNamed*) ProcessTable -> At(a);
		map_process.insert(make_pair(name->GetTitle(),stoi(name->GetName())));
		cout << name->GetTitle() << " " << name->GetName() << endl;
	}
	map<string,string> map_parameters;
	for(int a=0; a<InputParameters->GetSize();a++)
	{
		TNamed* name = (TNamed*) InputParameters -> At(a);
		map_parameters.insert(make_pair(name->GetName(),name->GetTitle()));
	}
	int nTracks, nSteps;
	int trackID[max], parentID[max], trackPDG[max], trackDetID[max];
	double px[max],py[max],pz[max],vx[max],vy[max],vz[max],E[max],kE[max];
	int stepTrackID[max], stepProcID[max], stepTrackPDG[max], stepPrevDetID[max], stepPostDetID[max];
	int IsBoundary[max], stepFromHit[max];
	double stepvx[max],stepvy[max],stepvz[max],Edep[max], stepPrevKE[max];

	if(map_parameters["MCTrack"] == "true")
	{
		T->SetBranchAddress("nTrack",&nTracks);
		T->SetBranchAddress("TrackID",trackID);
		T->SetBranchAddress("ParentID",parentID);
		T->SetBranchAddress("TrackPDG",trackPDG);
		T->SetBranchAddress("TrackDetID",trackDetID);
		T->SetBranchAddress("TrackPX",px);
		T->SetBranchAddress("TrackPY",py);
		T->SetBranchAddress("TrackPZ",pz);
		T->SetBranchAddress("TrackVX",vx);
		T->SetBranchAddress("TrackVY",vy);
		T->SetBranchAddress("TrackVZ",vz);
		T->SetBranchAddress("TrackEnergy",E);
		T->SetBranchAddress("TrackKEnergy",kE);
	}
	if(map_parameters["MCStep"] == "true")
	{
		T->SetBranchAddress("nStep",&nSteps);
		T->SetBranchAddress("StepTrackID",stepTrackID);
		T->SetBranchAddress("StepFromHit",stepFromHit);
		T->SetBranchAddress("StepProcID",stepProcID);
		T->SetBranchAddress("StepTrackPDG",stepTrackPDG);
		T->SetBranchAddress("StepPrevDetID",stepPrevDetID);
		T->SetBranchAddress("StepPostDetID",stepPostDetID);
		T->SetBranchAddress("IsBoundary",IsBoundary);
		T->SetBranchAddress("StepPrevKE",stepPrevKE);
		T->SetBranchAddress("StepVX",stepvx);
		T->SetBranchAddress("StepVY",stepvy);
		T->SetBranchAddress("StepVZ",stepvz);
		T->SetBranchAddress("StepEdep",Edep);
	}

	enum {pro,neu,ele,pos,gam,pELS};
	const char* particle[] = {"pro","neu","ele","pos","gam","pELS"};
	const int n_particle = sizeof(particle)/sizeof(particle[0]);

	enum {SC1,SC2,SC3,BDC,BTOF1,BTOF2,dELS};
	const char* det[] = {"SC1","SC2","SC3","BDC","BTOF1","BTOF2","dELS"};
	const int n_det = sizeof(det)/sizeof(det[0]);

	TFile* G = new TFile(Form("out_root/H_%s",infile),"recreate");
	// HIST 1: energy deposit of each step
	TH1F* H1_dE[n_particle][n_det];
	// HIST 2: energy of particle which goes to detector
	TH1F* H1_E[n_particle][n_det];
	// HIST 3: xy profile of particle at detector
	TH2F* H2_XY[n_particle][n_det];
//	double x[] = {60,60,90,800,800,10};
	double x[] = {60,60,60,90,150,150,10};
	double y[] = {60,60,60,90,50,50,10};
	for(int a=0; a<n_particle; a++)
	for(int b=0; b<n_det; b++)
	{
//		if(b==0)
//			continue;
		H1_E[a][b] = new TH1F(Form("H1_E_%s_%s",particle[a],det[b]),"",2200,0,110);
		float max_range = b==0?6:b==1?2:b==2?0.1:20;
		H1_dE[a][b] = new TH1F(Form("H1_dE_%s_%s",particle[a],det[b]),"",200,0,max_range);
		H2_XY[a][b] = new TH2F(Form("H2_XY_%s_%s",particle[a],det[b]),"",
				2*x[b],-x[b],x[b],2*y[b],-y[b],y[b]);
	}

	// event loop
	for(int a=0; a<T->GetEntriesFast(); a++)
//	for(int a=0; a<1; a++)
	{
		T->GetEntry(a);
		if(a%100 == 0)
			cout << "Processing " << a << "th event" << endl;
		// step loop
		vector<int> vec_stepTrackID;
		vector<int> vec_stepFromHit;
		vector<int> vec_prevDetIdx;
		vector<int> vec_postDetIdx;
		vector<double> vec_Edep;
		vector<int> vec_PDG;
		vector<int> vec_IsBoundary;
		vector<double> vec_PrevKE;
		vector<double> vec_stepX;
		vector<double> vec_stepY;
		map<int,double> map_TrackID_EdepSum[n_det];

		for(int b=0; b<nSteps; b++)
		{
			const int trkID = stepTrackID[b];
			const int boundary = IsBoundary[b];
			const int fromHit = stepFromHit[b];
			const int trkPDG = PDGtoIndex(stepTrackPDG[b]);
			const int prevDetIdx = DetIDtoIndex(stepPrevDetID[b]);
			const int postDetIdx = DetIDtoIndex(stepPostDetID[b]);
			const double edep = Edep[b];
			const double prevKE = stepPrevKE[b];
			vec_stepTrackID.push_back(trkID);
			vec_prevDetIdx.push_back(prevDetIdx);
			vec_postDetIdx.push_back(postDetIdx);
			vec_PDG.push_back(trkPDG);
			vec_Edep.push_back(edep);
			vec_IsBoundary.push_back(boundary);
			vec_PrevKE.push_back(prevKE);
			vec_stepFromHit.push_back(fromHit);
			vec_stepX.push_back(stepvx[b]);
			vec_stepY.push_back(stepvy[b]);
			if(fromHit == opt)
			{
				map_TrackID_EdepSum[prevDetIdx][trkID] += edep;
//				cout << "trkID: " << trkID << "(" << particle[trkPDG] << ")" << endl;
//				cout << "DetID: " << det[prevDetIdx] << " -> " << det[postDetIdx] << endl;
//				cout << "PrevKE: " << prevKE << endl;
//				cout << "Edep: " << edep << endl;
//				cout << endl;
//				cout << map_TrackID_EdepSum[prevDetIdx][trkID] << " " << edep << endl;
			}
		}

		// Fill historgram
		map<int,bool> map_trkID_Fill[n_det];
		for(int b=0; b<vec_stepTrackID.size(); b++)
		for(int c=0; c<n_det; c++)
		{
			{
				const int trkID = vec_stepTrackID[b];
				const int p = vec_PDG[b];
				const int prevNo = vec_prevDetIdx[b];
				const double edep = vec_Edep[b];
				const double EdepSum = map_TrackID_EdepSum[prevNo][trkID];
//				if(map_trkID_Fill[c][trkID] == 0 && vec_stepFromHit[b] == opt)
//					cout << "filled?: " << map_trkID_Fill[c][trkID] << endl;
				// for opt==0 debug code
				if(vec_stepFromHit[b]==opt && EdepSum != 0)// && vec_prevDetIdx[b]==SC2)
				{
					if(map_trkID_Fill[prevNo][trkID]==false && false)
					{
						cout << "EventNo: " << a << endl;
						cout << "Det: " << det[vec_prevDetIdx[b]] << " > " << det[vec_postDetIdx[b]] << endl;
						cout << "trkID(PDG): " << trkID << "(" << particle[vec_PDG[b]] << ")" << endl;
						cout << "filled?: " << map_trkID_Fill[c][trkID] << endl;
						cout << "EdepSum: " << map_TrackID_EdepSum[c][trkID] << endl;
						cout << "not filled" << endl;
						cout << endl;
						H1_dE[p][prevNo] -> Fill(map_TrackID_EdepSum[prevNo][trkID]);
						map_trkID_Fill[prevNo][trkID] = true;
					}
				}
				// Fill HIST 1
				if(vec_stepFromHit[b] == opt && map_trkID_Fill[c][trkID]==0)
				{
					H1_dE[p][c] -> Fill(map_TrackID_EdepSum[c][trkID]);
					map_trkID_Fill[c][trkID] = 1;
				}
				const int boundary = vec_IsBoundary[b];
//				const int prevNo = vec_prevDetIdx[b];
				const int postNo = vec_postDetIdx[b];
				const double prevKE = vec_PrevKE[b];
				const double vx = vec_stepX[b];
				const double vy = vec_stepY[b];
//				if(boundary == 1)
//				{
//					cout << p << " " << boundary << " " << prevKE << " " << endl;
//				}
				// Fill HIST 2
				if(boundary && prevKE != 0 && (prevNo == dELS && postNo == c))
				{
//					if(a < 100 && (postNo == 4 ||postNo ==5))
//					cout << prevNo << " -> " << postNo << endl;
//					cout << det[prevNo] << " -> " << det[postNo] << " " << particle[p] << endl << endl;
					H1_E[p][c] -> Fill(prevKE);
					H2_XY[p][c] -> Fill(vx,vy);
				}
			}
		}
	}
	G -> Write();
	G -> Close();
//	F -> Close();
	cout << "done" << endl;
}

