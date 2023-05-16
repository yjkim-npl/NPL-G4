void BirkDraw()
{
	TFile* F = new TFile("data_root/out_Birks.root","read");
	TH2F* H2_Edep_L[30];
	double arr_Edep[30];
	double arr_LY[30];
	for(int a=0; a<30; a++)
	{
		H2_Edep_L[a] = (TH2F*) F -> Get(Form("H2_Edep_L_%dMeV",10+10*a));
		arr_Edep[a] = H2_Edep_L[a] -> ProjectionX() -> GetMean();
		arr_LY[a] = H2_Edep_L[a] -> ProjectionY() -> GetMean();
	}

	TGraph* g_Birk = new TGraph(30,arr_Edep,arr_LY);
	g_Birk -> SetMarkerColor(1);
	g_Birk -> SetMarkerSize(1.2);
	g_Birk -> SetMarkerStyle(20);

	TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
	gPad -> SetMargin(.13,.05,.12,.05);
	gPad -> SetTicks();
	TH1D* htemp = (TH1D*) gPad -> DrawFrame(0,0,11,1);
	htemp -> GetXaxis() -> SetTitle("<Edep>[MeV]");
	htemp -> GetYaxis() -> SetTitle("<LY>");
	g_Birk -> Draw("p");
}
