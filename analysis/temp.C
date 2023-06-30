void temp()
{
	char* dir = "out_root";
	char* infiles[] = {"H_Op_out_proton_100MeV_SiPM.root",
										 "H_Op_out_proton_100MeV_SiPMLRUD.root",
										 "H_Op_out_proton_100MeV_gap_LR.root",
										 "H_Op_out_proton_100MeV_gap_LRUD.root"};
	char* label[] = {"bar_LR","bar_LRUD","cell_LR","cell_LRUD"};
	int n_files = sizeof(infiles)/sizeof(infiles[0]);

	double SiPMIn[n_files];// fill_n(n_files,SiPMIn,0);
	TH1F* H1_ratio = new TH1F("H1_ratio","",4,0,4);
	TH1F* H1_ratio_1 = new TH1F("H1_raio_1","",2,0,2);
	for(int a=0; a<n_files; a++)
	{
		TFile* F = new TFile(Form("%s/%s",dir,infiles[a]),"read");
		TH1F* H1 = (TH1F*) F -> Get("H1_ratio");
		SiPMIn[a] = H1 -> GetBinContent(4) / H1 -> GetBinContent(1);
		H1_ratio -> SetBinContent(a+1,SiPMIn[a]);
		H1_ratio_1 -> SetBinContent(a+1,SiPMIn[a]*0.65);
		H1_ratio -> GetXaxis() -> SetBinLabel(a+1,label[a]);
	}

	H1_ratio -> SetStats(0);
	H1_ratio_1 -> SetStats(0);
	H1_ratio_1 -> SetLineColor(2);
	H1_ratio -> GetYaxis() -> SetRangeUser(0,1.1);
	H1_ratio -> GetXaxis() -> SetLabelSize(0.06);

	H1_ratio -> Draw("hist text0");
	H1_ratio_1 -> Draw("hist text0 same");
}
