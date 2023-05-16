void DrawHIMAC
(char* p = "temp",
 char* e = "",
 char* s = ""
 )
{
	bool DrawOpt1 = 1;
	bool DrawOpt2 = 1;
	bool DrawOpt3 = 0;
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

	TFile* F = new TFile(infile,"read");
	enum {pro,neu,ele,pos,gam,pELS};
	const char* particle[] = {"pro","neu","ele","pos","gam","pELS"};
	const int n_particle = sizeof(particle)/sizeof(particle[0]);

	enum {SC1,SC2,SC3,BDC,BTOF1,BTOF2,dELS};
	const char* det[] = {"SC1","SC2","SC3","BDC","BTOF1","BTOF2","dELS"};
	const int n_det = sizeof(det)/sizeof(det[0]);

	// HIST opt 1
	TH1F* H1_dE[n_particle][n_det];
	// HIST opt 2
	TH1F* H1_E[n_particle][n_det];
	// HIST opt 3
	TH2F* H2_XY[n_particle][n_det];
	int max_ent[n_det]; fill_n(max_ent,n_det,0);
	int max_ent_KE[n_det]; fill_n(max_ent_KE,n_det,0);
	for(int a=0; a<n_particle; a++)
	for(int b=0; b<n_det; b++)
	{
		H1_dE[a][b] = (TH1F*) F -> Get(Form("H1_dE_%s_%s",particle[a],det[b]));
		H1_E[a][b] = (TH1F*) F -> Get(Form("H1_E_%s_%s",particle[a],det[b]));
		H2_XY[a][b] = (TH2F*) F -> Get(Form("H2_XY_%s_%s",particle[a],det[b]));
		cout << "Maximum of " << particle[a] << " " << det[b]<< " " <<H1_dE[a][b]->GetMaximum() << endl;
		if(max_ent[b] < H1_dE[a][b] -> GetMaximum() && a != ele)
		{
			max_ent[b] = H1_dE[a][b] -> GetMaximum();
//			cout << "max_ent[" << det[b] << "]: " << max_ent[b] << endl;
//			cout << "max_ent_KE[" << det[b] << "]: " << max_ent_KE[b] << endl;
		}
		if(max_ent_KE[b] < H1_E[a][b] -> GetMaximum())
			 max_ent_KE[b] = H1_E[a][b] -> GetMaximum();
	}
	for(int b=0; b<n_det; b++)
	{
	}

	if(DrawOpt1)
	{
		TCanvas* c1 = new TCanvas("c1","c1",1.2*600*3,600*2);
		c1 -> Divide(3,2);
		for(int b=0; b<n_det-1; b++)
		{
			c1 -> cd(b+1);
			gPad -> SetMargin(0.13,0.05,0.12,0.05);
			gPad -> SetTicks();

			TLegend* leg = new TLegend(0.55,0.5,0.9,0.5+(n_particle+2)*0.05);
			leg -> SetFillColor(0);
			leg -> SetBorderSize(0);
			leg -> SetFillStyle(0);
			leg -> SetTextSize(0.04);
			leg -> AddEntry((TObject*)0,"proton 100 MeV simulation","h");
			leg -> AddEntry((TObject*)0,Form("Edep at %s",det[b]),"h");
			for(int a=0; a<n_particle; a++)
			{
//				if(a!=0)
//					continue;
				H1_dE[a][b] -> SetStats(false);
				H1_dE[a][b] -> GetXaxis() -> SetTitle("E[MeV]");
				H1_dE[a][b] -> GetYaxis() -> SetTitle("Entries");
				H1_dE[a][b] -> GetYaxis() -> SetRangeUser(0,1.5*max_ent[b]);
				H1_dE[a][b] -> SetLineColor(a+1);
				H1_dE[a][b] -> Draw("hist same");
//				cout << H1_dE[a][b] -> GetEntries() << endl;
				leg -> AddEntry(H1_dE[a][b],Form("%s,Mean:%.4f",particle[a],H1_dE[a][b] -> GetMean()),"lf");
			}
			leg -> Draw();
		}
		c1 -> SaveAs("fig/fig_Edep_det.pdf");
	}
	if(DrawOpt2)
	{
		TCanvas* c2 = new TCanvas("c2","c2",1.2*600*3,600*2);
		c2 -> Divide(3,2);
		for(int b=0; b<n_det-1; b++)
		{
			c2 -> cd(b+1);
			gPad -> SetMargin(0.13,0.05,0.12,0.05);
			gPad -> SetTicks();

			TLegend* leg = new TLegend(0.2,0.5,0.5,0.5+(n_particle+2)*0.05);
			leg -> SetFillColor(0);
			leg -> SetBorderSize(0);
			leg -> SetFillStyle(0);
			leg -> SetTextSize(0.04);
			leg -> AddEntry((TObject*)0,"proton 100 MeV simulation","h");
			leg -> AddEntry((TObject*)0,Form("Energy of particle to %s",det[b]),"h");
			for(int a=0; a<n_particle; a++)
			{
				H1_E[a][b] -> SetStats(false);
				H1_E[a][b] -> GetXaxis() -> SetTitle("E[MeV]");
				H1_E[a][b] -> GetYaxis() -> SetRangeUser(0,1.5*max_ent_KE[b]);
				H1_E[a][b] -> SetLineColor(a+1);
				leg -> AddEntry(H1_E[a][b],Form("%s,Mean:%.2f",particle[a],H1_E[a][b]->GetMean()),"lf");
				H1_E[a][b] -> Draw("hist same");
			}
			leg -> Draw();
		}
		c2 -> SaveAs("fig/fig_prevE_det.pdf");
	}
	if(DrawOpt3)
	{
		TCanvas* c3 = new TCanvas("c3","c3",1.2*600*3,600*2);
		c3 -> Divide(3,2);
		for(int b=0; b<n_det-1; b++)
		{
			c3 -> cd(b+1);
			gPad -> SetMargin(0.13,0.05,0.12,0.05);
			gPad -> SetTicks();

			TLegend* leg = new TLegend(0.2,0.8,0.5,0.8+(2)*0.05);
			leg -> SetFillColor(0);
			leg -> SetBorderSize(0);
			leg -> SetFillStyle(0);
			leg -> SetTextSize(0.04);
			leg -> AddEntry((TObject*)0,"proton 100 MeV simulation","h");
			leg -> AddEntry((TObject*)0,Form("XY %s",det[b]),"h");
			for(int a=0; a<n_particle; a++)
			{
				if(a!=0)
					continue;
				H2_XY[a][b] -> SetStats(false);
				H2_XY[a][b] -> GetXaxis() -> SetTitle("X[mm]");
				H2_XY[a][b] -> GetYaxis() -> SetTitle("Y[mm]");
//				H2_XY[a][b] -> GetYaxis() -> SetRangeUser(0,1.5*max_ent_KE[b]);
				H2_XY[a][b] -> SetLineColor(a+1);
//				leg -> AddEntry(H2_XY[a][b],Form("%s,Mean:%.2f",particle[a],H2_XY[a][b]->GetMean()),"lf");
				H2_XY[a][b] -> Draw("hist same colz");
			}
			leg -> Draw();
		}
		c3 -> SaveAs("fig/fig_XY_det.pdf");
	}
}
