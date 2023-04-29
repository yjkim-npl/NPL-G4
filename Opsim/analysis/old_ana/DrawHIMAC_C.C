void DrawHIMAC_C
(
 char* p = "temp",
 char* e = "",
 char* s = ""
)
{
	bool DrawOpt1 = 1;
	bool DrawOpt2 = 1;

	char* infile;
	TString str_p = p;
	TString str_s = s;
	if(str_p.Contains("temp")){
		if(str_s=="") infile = Form("out_root/H_out_%s.root",p);
		else          infile = Form("out_root/H_out_%s_%s.root",p,s);
	}else{
		if(str_s=="") infile = Form("out_root/H_out_%s_%s.root",p,e);
		else          infile = Form("out_root/H_out_%s_%s_%s.root",p,e,s);
	}
	cout << "infile: " << infile << endl;
	TFile* F = new TFile(infile,"read");
	enum {pro,neu,ele,pos,gam,pELS};
	enum {SC1,SC2,SC3,BDC,BTOF1,BTOF2,dELS};
	const char* particle[] = {"pro","neu","ele","pos","gam","pELS"};
	const char* det[] = {"SC1","SC2","SC3","BDC","BTOF1","BTOF2","dELS"};
	const int n_particle = sizeof(particle)/sizeof(particle[0]);
	const int n_det = sizeof(det)/sizeof(det[0]);

	TH1F* H1_Edep[n_particle][n_det];
	TH1F* H1_E[n_particle][n_det];

	for(int a=0; a<n_particle; a++)
	for(int b=0; b<n_det; b++)
	{
		H1_Edep[a][b] = (TH1F*) F -> Get(Form("H1_Edep_%s_%s",particle[a],det[b]));
		cout << "read: " << Form("H1_Edep_%s_%s",particle[a],det[b]) << endl;
		cout << Form("%s,%s,Mean: %d",particle[a],det[b],H1_Edep[a][b] -> GetMean()) << endl;
		H1_E[a][b] = (TH1F*) F -> Get(Form("H1_E_%s_%s",particle[a],det[b]));
	}

	if(DrawOpt1)
	{
		TCanvas* c1 = new TCanvas("c1","c1",1.2*600*3,600*2);
		c1 -> Divide(3,2);
//		for(int a=0; a<n_det-1; a++)
		for(int a=0; a<1; a++)
		{
			c1 -> cd(a+1);
//			gPad -> SetMargin(.13,.05,.12,.05);
//			gPad -> SetTicks();

			TLegend* leg = new TLegend(.55,.5,.9,.5+(n_particle+2)*0.05);
			leg -> SetFillStyle(0);
			leg -> SetBorderSize(0);
			leg -> SetTextSize(0.04);
			leg -> AddEntry((TObject*)0,"12C 200MeV/u simulation","h");
			leg -> AddEntry((TObject*)0,Form("Edep at %s",det[a]),"h");
			for(int b=0; b<n_particle; b++)
			{
				H1_Edep[b][a] -> SetStats(false);
				H1_Edep[b][a] -> GetXaxis() -> SetTitle("Edep[MeV]");
				H1_Edep[b][a] -> GetYaxis() -> SetTitle("Entries");
				H1_Edep[b][a] -> SetLineColor(b+1);
				H1_Edep[b][a] -> Draw("hist same");
				leg -> AddEntry(H1_Edep[b][a],Form("%s,Mean:%.4f",particle[b],H1_Edep[b][a]->GetMean()),"lf");
			}
//			leg -> Draw();
		}
	}

	F -> Close();
}
