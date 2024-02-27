inline char* OutName(int opt, char* prefix, char* p, char* e, char* s, char* format)
{
	char* out;
	if(opt == 1)
		out = Form("%s_%s.%s",prefix,p,format);
	else if(opt == 2)
		out = Form("%s_%s_%s.%s",prefix,p,s,format);
	else if(opt == 3)
		out = Form("%s_%s_%s.%s",prefix,p,e,format);
	else if(opt == 4)
		out = Form("%s_%s_%s_%s.%s",prefix,p,e,s,format);
	else
		out = "blank";
	return out;
}

void nonOpDraw(
		char* particle = "proton",
		char* energy = "100MeV",
		char* suffix = "SC")
{
	/* HIST lists
		 0. 1-D Trig cond (1&2, 2&3, 3&4)
		 1. 2-D[SCs] incident particle - nStep at each SC
		 2. 1-D[SCs][p,n,e,I,else] Edep sum with track tagging
		 3. 2-D[SCs-1][] delta energy weighted T - avg edep
	*/
	const int n_Hist = 4;
	bool Opt[n_Hist]; fill_n(Opt,n_Hist,0);
	Opt[0] = 1;
	Opt[1] = 1;
	Opt[2] = 1; 
	Opt[3] = 1;

	TString str_p = particle;
	TString str_s = suffix;
	int name_opt = 0;
	if(str_p.Contains("temp")){
		if(str_s=="") name_opt = 1;
		else          name_opt = 2;
	}else{
		if(str_s=="") name_opt = 3;
		else          name_opt = 4;
	}
	char* in_pref = Form("./out_root/H_out");
	char* infile = OutName(name_opt,in_pref,particle,energy,suffix,"root");
	
	char* str_SC[] = {"SC1","SC2","SC3","SC4"};
	char* str_pID[] = {"pro","neu","ele","ion","ELSE"};
	const int nSC = sizeof(str_SC)/sizeof(str_SC[0]);
	const int npID = sizeof(str_pID)/sizeof(str_pID[0]);

	// load root file
	TFile* F = new TFile(infile,"read");

	// HIST 0 Trig cond (1&2 2&3 3&4)
	if(Opt[0])
	{
		TCanvas* c0 = new TCanvas("c0","c0",1.2*600,600);
		TH1D* H1 = (TH1D*) F -> Get("H1_Trig");
		H1 -> SetStats(false);
		H1 -> GetXaxis() -> SetLabelSize(0.06);
		H1 -> GetXaxis() -> SetBinLabel(1,"SC1 & SC2");
		H1 -> GetXaxis() -> SetBinLabel(2,"SC2 & SC3");
		H1 -> GetXaxis() -> SetBinLabel(3,"SC3 & SC4");
		H1 -> Draw();
		char* pref = "fig/fig_Trig";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c0 -> SaveAs(outname);
	}

	// HIST 1. 2-D[SCs] incident particle - nStep at each SC
	if(Opt[1])
	{
		TCanvas* c1 = new TCanvas("c1","c1",1.2*400*2,400*2);
		c1 -> Divide(2,2);
		for(int a=0; a<nSC; a++)
		{
			TH2F* H2 = (TH2F*) F -> Get(Form("H2_Beam_nSteps_%s",str_SC[a]));
			H2 -> GetYaxis() -> SetRangeUser(0,20);
			for(int b=0; b<npID; b++)
				H2 -> GetXaxis() -> SetBinLabel(b+1,str_pID[b]);
			H2 -> Draw("colz");
//			TLegend* leg = new TLegend(.15,.93-0.06*5,.9,.93);
//			leg -> SetBorderSize(0);
//			leg -> SetFillStyle(0);
//			leg -> SetTextSize(0.05);
//			leg -> AddEntry("",Form("%s %s simulation", particle, energy),"h");
//			leg -> Draw();
		}
		char* pref = "fig/fig_nStep";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c1 -> SaveAs(outname);
	}
	// HIST 2. 1-D[SCs][p,n,e,I,else] Edep sum with track tagging
	if(Opt[2])
	{
		TCanvas* c2 = new TCanvas("c2","c2",1.2*400*2,400*2);
		c2 -> Divide(2,2);
		for(int a=0; a<nSC; a++)
		{
			TLegend* leg = new TLegend(.15,.93-0.06*5,.9,.93);
			leg -> SetBorderSize(0);
			leg -> SetFillStyle(0);
			leg -> SetTextSize(0.05);
			leg -> AddEntry("",Form("%s %s simulation", particle, energy),"h");
			for(int b=0; b<npID; b++)
			{
				c2 -> cd(a+1);
				gPad -> SetMargin(.13,.05,.12,.05);
				gPad -> SetLogy();
				TH1F* H1 = 
					(TH1F*) F -> Get(Form("H1_Edep_%s_%s",str_SC[a],str_pID[b]));
				H1 -> SetStats(false);
				H1 -> Rebin(10);
				if(b==0)
					H1 -> GetYaxis() -> SetRangeUser(0.1,1e3* H1->GetMaximum());
//				H1 -> GetXaxis() -> SetRangeUser(15,30);
				H1 -> GetXaxis() -> SetTitle("Edep [MeV]");
				H1 -> GetXaxis() -> SetTitleSize(0.06);
				H1 -> GetXaxis() -> SetLabelSize(0.045);
//				H1 -> GetYaxis() -> SetTitle("Entries");
				H1 -> GetYaxis() -> SetTitleSize(0.06);
				H1 -> GetYaxis() -> SetLabelSize(0.045);
				H1 -> SetLineColor(b==2?b+4:b+1);
//				H1 -> SetLineWidth(2);
				leg -> AddEntry(H1,Form("%s, Mean: %.2f, StdDev: %.2f",str_pID[b],H1->GetMean(),H1->GetStdDev()),"l");

				H1 -> Draw("same");
			}
			leg -> Draw();
		}
		char* pref = "fig/fig_EdepSum";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c2 -> SaveAs(outname);
	}

	// HIST 3. 2-D[SCs-1][] delta energy weighted T - avg edep
	if(Opt[3])
	{
		TCanvas* c3 = new TCanvas("c3","c3",1.2*400*4,400*4);
		c3 -> Divide(5,3);
		for(int a=0; a<nSC-1; a++)
		{
			for(int b=0; b<npID; b++)
			{
				c3 -> cd(b+5*a+1);
				gPad -> SetMargin(.13,.1,.12,.05);
				TH2F* H2 = 
					(TH2F*) F -> Get(Form("H2_Time_Edep_%s_%s_%s",str_SC[a],str_SC[a+1],str_pID[b]));
				H2 -> Rebin2D(10,10);
				H2 -> SetStats(false);
//				H2 -> Rebin(10);
//				if(b==0)
//					H2 -> GetYaxis() -> SetRangeUser(0,1.7* H2->GetMaximum());
				H2 -> GetXaxis() -> SetRangeUser(-10,30);
				H2 -> GetYaxis() -> SetRangeUser(-100,100);
				H2 -> GetXaxis() -> SetTitle("Time of flight [ns]");
				H2 -> GetXaxis() -> SetTitleSize(0.06);
				H2 -> GetXaxis() -> SetLabelSize(0.045);
				H2 -> GetYaxis() -> SetTitle("Average Edep [MeV]");
				H2 -> GetYaxis() -> SetTitleSize(0.06);
				H2 -> GetYaxis() -> SetLabelSize(0.045);
				H2 -> SetLineColor(b==2?b+4:b+1);
//				H2 -> SetLineWidth(2);

				H2 -> Draw("same colz");
				int row = 0;
				TLegend* leg = 
					new TLegend(.15,.93-0.06*3,.9,.93);
				leg -> SetBorderSize(0);
				leg -> SetFillStyle(0);
				leg -> SetTextSize(0.06);
				if(a==0 && b==0)
					leg -> AddEntry("",Form("%s %s simulation", particle, energy),"h");
				if(b==0)
					leg -> AddEntry("",Form("%s-%s",str_SC[a],str_SC[a+1]),"h");
				if(a==0)
					leg -> AddEntry("",Form("%s",str_pID[b]),"h");
				leg -> Draw();
			}
		}
		char* pref = "fig/fig_Timediff_Edep";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c3 -> SaveAs(outname);
	}
	
}
