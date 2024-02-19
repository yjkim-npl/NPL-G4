void temp()
{
	const char* infile = "data_root/out_alpha_5.48MeV_Boundary.root";
	TFile* F = new TFile(infile,"read");
	TTree* T = (TTree*) F -> Get("Opsim");

	int NOpBoundary;
	int ob_ProcID[1000];
//	vector<int> *ob_ProcID;

	T -> SetBranchAddress("NOpBoundary",&NOpBoundary);
	T -> SetBranchAddress("OpProcIDBoundary",ob_ProcID);

//	for(int a=0; a<T->GetEntriesFast(); a++)
	for(int a=0; a<10; a++)
	{
		cout << a << " th Event" << endl;
		T -> GetEntry(a);
//		cout << NOpBoundary << " " << ob_ProcID.size() <<  endl;
	}
}
