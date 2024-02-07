void nonOpDraw(
		char* particle = "proton",
		char* energy = "100MeV",
		char* suffix = "")
{
	/* HIST lists
		 0. 1-D[SCs][p,n,e,A] Edep distribution with pID on SCs
		 1. 1-D[SCs][] Time distribution with pID on SCs
		 2. 2-D[SCs] Generation process & pID on SCs
		 3. 2-D[SCs][] Step position on SC
	*/
	const int n_Hist = 4;
	bool Opt[n_Hist]; fill_n(Opt,n_Hist,0);
	Opt[0] = 1;
	Opt[1] = 0;
	Opt[2] = 0;
	Opt[3] = 0;

	char* infile;
	TString str_p = particle;
	TString str_s = suffix;
	if(str_p.Contains("temp")){
		if(str_s=="") infile = Form("H_out_%s.root",particle);
		else          infile = Form("H_out_%s_%s.root",particle,suffix);
	}else{
		if(str_s=="") infile = Form("H_out_%s_%s.root",particle,energy);
		else          infile = Form("H_out_%s_%s_%s.root",particle,energy,suffix);
	}

	cout << "#################" << endl;
	cout << "infile: " << infile << endl;
	cout << "#################" << endl;
	const char* file_prefix = Form("./out_root");
	
	char* str_SC[] = {"SC1","SC2","SC3","SC4"};
	char* str_pID[] = {"pro","neu","ele","ion"};
	const int nSC = sizeof(str_SC)/sizeof(str_SC[0]);
	const int npID = sizeof(str_pID)/sizeof(str_pID[0]);

	// load root file
	TFile* F = new TFile(Form("%s/%s",file_prefix,infile),"read");
	if(Opt[0])
	{
		TCanvas* c0 = new TCanvas("c0","c0",1.2*600*nSC,600);
		c0 -> Divide(nSC);
		gPad -> SetMargin(.13,.05,.12,.05);
		for(int a=0; a<nSC; a++)
		{
//			TLegend* leg = new TLegend();
			for(int b=0; b<npID; b++)
			{
				c0 -> cd(a);
				TH1F* H1 = 
					(TH1F*) F -> Get(Form("H1_Edep_%s_%s",str_SC[a],str_pID[b]));
				H1 -> GetXaxis() -> SetTitle("Edep [MeV]");
				H1 -> GetXaxis() -> SetTitleSize(0.06);
				H1 -> GetXaxis() -> SetLabelSize(0.045);
				H1 -> GetYaxis() -> SetTitle("Entries");
				H1 -> GetYaxis() -> SetTitleSize(0.06);
				H1 -> GetYaxis() -> SetLabelSize(0.045);

				H1 -> Draw("same");
			}
		}
	}
	
}
