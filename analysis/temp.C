void temp()
{
	TFile* F = new TFile("data/out_proton_100MeV.root","read");
	TTree* T = (TTree*) F -> Get("Opsim");

	int nTrack;
	vector<int> *t_ID =0;
	vec
	TBranch* bt_ID = 0;
	int* a_ID;

	T -> SetBranchAddress("nTrack",&nTrack);
	T -> SetBranchAddress("TrackID",&t_ID,&bt_ID);

	cout << "done" << endl;
	T -> GetEntry(0);
	cout << t_ID ->size() << endl;
}
