void MPTDraw()
{
//	double wavlen[] = {
//		900, 890, 880, 870, 860, 850, 840, 830, 
//		820, 810, 800, 790, 780, 770, 760, 750, 
//		740, 730, 720, 710, 700, 690, 680, 670, 
//		660, 650, 640, 630, 620, 610, 600, 590, 
//		580, 570, 560, 550, 540, 530, 520, 510, 
//		500, 490, 480, 470, 460, 450, 440, 430, 
//		420, 410, 400, 390, 380, 370, 360, 350, 
//		340, 330, 320, 310
//	};
//	const int n = sizeof(wavlen)/sizeof(wavlen[0]);
	const int n = 60;
	double wavlen[n];
	double eff[n] ;
	ifstream Eff_SiPM("Eff_SiPM.csv");
	string line;
	size_t pos = -1;
	getline(Eff_SiPM,line); // dummy line
	for(int a=0; a<n; a++)
	{
		getline(Eff_SiPM,line);
		while((pos=line.rfind(',')) != string::npos)
			line.erase(pos,1);
		stringstream ss(line);
		double fwav, feff;
		ss >> fwav >> feff;
		wavlen[a] = (int)fwav/10 * 10;
		eff[a] = feff;
//		cout << wavlen[a] << " " << eff[a] << endl;
//		cout << wavlen[a-1] << " " << feff << endl << endl;
	}

	TGraph* g = new TGraph(n,wavlen,eff);
	g -> SetMarkerColor(1);
	g -> SetMarkerSize(1.2);
	g -> SetMarkerStyle(20);

	TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
	gPad -> SetMargin(.13,.05,.12,.05);
	gPad -> SetTicks();
	TH1D* htemp = (TH1D*) gPad -> DrawFrame(200,0,1000,60);
	htemp -> GetXaxis() -> SetTitle("#lambda [nm]");
	htemp -> GetYaxis() -> SetTitle("Detection efficiency");
	g -> Draw("p");

	c1 -> SaveAs("fig/fig_SiPM_Efficiency.pdf");
}
