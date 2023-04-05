void Birk(const char* particle = "proton")
{
	const int beam_E[] = {10,30,50,100,200,300};
	const int nE = sizeof(beam_E)/sizeof(beam_E[0]);

	// define container for the graph
	double Edep[nE];
	double LY[nE];
	// read histrogram from root file
	for(int a=0; a<nE; a++)
	{
		TFile* F = new TFile(Form("out_root/H_Op_out_%s_%dMeV.root",particle,beam_E[a]),"read");
		TH2F* H2_Edep_L = (TH2F*) F -> Get("H2_Edep_L");
		Edep[a] = H2_Edep_L -> ProjectionX() -> GetMean();
		LY[a] = H2_Edep_L -> ProjectionY() -> GetMean();
		cout << Form("%s, %dMeV",particle,beam_E[a]) << endl;
		cout << "X mean: " << Edep[a] << endl;
		cout << "Y mean: " << LY[a] << endl << endl;
	}

	// create graph (Edep - LY)
	TGraph* g_Birk = new TGraph(nE,Edep,LY);
	g_Birk -> SetMarkerColor(1);
	g_Birk -> SetMarkerSize(1.2);
	g_Birk -> SetMarkerStyle(20);

	// create canvas
	TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
	gPad -> SetMargin(.13,.05,.12,.05);
	gPad -> SetTicks();
	TH1D* htemp = (TH1D*) gPad -> DrawFrame(0,0,11,60);
	htemp -> GetXaxis() -> SetTitle("<Edep>[MeV]");
	htemp -> GetYaxis() -> SetTitle("<LY>");
	g_Birk -> Draw("p");

	c1 -> SaveAs("fig/fig_Birk.pdf");
}
