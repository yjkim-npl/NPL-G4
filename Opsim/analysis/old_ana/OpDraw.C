inline char* OutName(int opt, char* prefix, char* p, char* e, char* s, char* ext)
{
	char* out;
	if      (opt == 1)
		out = Form("%s_%s.%s",prefix,p,ext);
	else if (opt == 2)
		out = Form("%s_%s_%s.%s",prefix,p,s,ext);
	else if (opt == 3)
		out = Form("%s_%s_%s.%s",prefix,p,e,ext);
	else if (opt == 4)
		out = Form("%s_%s_%s_%s.%s",prefix,p,e,s,ext);
	else
		out = "blank";
	return out;
}

void OpDraw
(
 char* particle = "proton",
 char* energy = "200MeV",
 char* suffix = ""
 )
{
	/* HIST LISTs
		0: 1-D Number of optical photon distribution
			to check the property "SCINTILLATIONYIELD"
		1: 1-D creator process of generated optical photons 
			to check scint/cheren ratio of electron
		2: 1-D wavelength(energy) spectra
		3: 1-D time when o.p created and o.p died
			to check "ABSLENGTH" effect
		4: momentum profile with boundary process
		5: 2-D L/x - Edep/x 
		6: 2-D dL/dx - dE/dx
		7: 1-D parentID(particle) of optical photon
		8: 1-D histrogram of total track length of optical photon
		   to check attenuation(absorption)
		9: 1-D Time distribution at each side of SiPM
		10: 1-D histogram of Number of optical photons reached at each side of SiPM
	 */
	bool Opt[11];
	Opt[0] = 0;
	Opt[1] = 1;
	Opt[2] = 1;
	Opt[3] = 0;
	Opt[4] = 0; // not working
	Opt[5] = 0;
	Opt[6] = 0; // not working
	Opt[7] = 0; // wierd 
	Opt[8] = 0;
	Opt[9] = 0;
	Opt[10] = 0;

	char* in_pref = "out_root/H_Op_out";
	int name_opt = 0;
	TString str_p = particle;
	TString str_s = suffix;
	if(str_p.Contains("temp")){
		if(str_s=="") name_opt = 1;
		else          name_opt = 2;
	}else{
		if(str_s=="") name_opt = 3;
		else          name_opt = 4;
	}
	char* infile = OutName(name_opt,in_pref,particle,energy,suffix,"root");
	cout << "infile: " << infile << endl;
	// call the data root file
	TFile* F = new TFile(infile,"read");
	TList* L_MatProp = (TList*) F -> Get("ConstMaterialProperties");
	map<string,string> map_MatProp;
	for(int a=0; a<L_MatProp->GetSize(); a++)
	{
		TNamed* name = (TNamed*) L_MatProp -> At(a);
		map_MatProp.insert(make_pair(name->GetName(),name->GetTitle()));
	}
	TH1D* H1_NOp_dist;  // HIST 0
	TH1F* H1_OpProcID;  // HIST 1
	enum {Total,Scint,Cheren,Att,Forward,Backward,SiPM};
	const char* cre_opt[] = {"Total","Scint","Cheren","Attenuation","Forward","Backward","SiPM"};
	const int n_opt = sizeof(cre_opt)/sizeof(cre_opt[0]);
	TH1F* H1_OpWav[n_opt];     // HIST 2
	TH1F* H1_OpE[n_opt];       // HIST 2
	TH1F* H1_OpTime;    // HIST 3
	TH1F* H1_PostOpTime;// HIST 3
//	TH2F* H2_p;         // HIST 4
	TH2F* H2_Edep_L;    // HIST 5
	TH2F* H2_dEdx_dLdx; // HIST 6
	TH2F* H2_LY_OpParentID;// HIST 7
	TH1F* H1_TrackLength;	// HIST 8
	TH1F* H1_Time_Left;		// HIST 9
	TH1F* H1_Time_Right;	// HIST 9
	TH1F* H1_NOp_Left;		// HIST 10
	TH1F* H1_NOp_Right;		

	if(Opt[0])
	{
		TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
		H1_NOp_dist = (TH1D*) F -> Get("H1_NOp_dist");
		H1_NOp_dist -> SetStats(false);
		H1_NOp_dist -> GetXaxis() -> SetTitle("# of photons");
		H1_NOp_dist -> Draw("hist");

		double SY = stod(map_MatProp["PVT_ScintYield"]);
		TLegend* leg = new TLegend(0.5,0.6,0.9,0.6+0.045*4);
		leg -> SetBorderSize(0);
		leg -> SetFillColor(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.038);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("Scintillation Yield: %.1f /MeV",SY),"h");
		leg -> AddEntry((TObject*)0,Form("Mean: %.1f",H1_NOp_dist -> GetMean()),"h");
		leg -> AddEntry((TObject*)0,Form("StdDev: %.1f",H1_NOp_dist -> GetStdDev()),"h");
		leg -> Draw();

		char* pref = "fig/fig_NOp";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c1 -> SaveAs(outname);
	}
	if(Opt[1])
	{
		TCanvas* c2 = new TCanvas("c2","c2",1.2*600,600);
		H1_OpProcID = (TH1F*) F -> Get("H1_OpProcID");
		H1_OpProcID -> SetStats(false);
		const int max = H1_OpProcID -> GetMaximum();
		gPad -> SetLogy();
		H1_OpProcID -> GetYaxis() -> SetRangeUser(0.9,100*max);
		H1_OpProcID -> GetXaxis() -> SetRangeUser(21,23);
		H1_OpProcID -> GetXaxis() -> SetLabelSize(0.05);
		H1_OpProcID -> GetXaxis() -> SetBinLabel(H1_OpProcID->FindBin(21),"Cherenkov");
		H1_OpProcID -> GetXaxis() -> SetBinLabel(H1_OpProcID->FindBin(22),"Scintillation");
		H1_OpProcID -> GetXaxis() -> SetBinLabel(H1_OpProcID->FindBin(23),"others");
		H1_OpProcID -> Draw("hist");

		double SY = stod(map_MatProp["PVT_ScintYield"]);
		TLegend* leg = new TLegend(0.55,0.70,0.9,0.70+0.045*4);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.038);
		leg -> AddEntry((TObject*)0, Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0, Form("SY: %.0f / MeV",SY),"h");
		const int ceren = H1_OpProcID -> GetBinContent(H1_OpProcID->FindBin(21));
		const int scint = H1_OpProcID -> GetBinContent(H1_OpProcID->FindBin(22));
		leg -> AddEntry((TObject*)0, Form("\tCherenkov: %d",ceren),"h");
		leg -> AddEntry((TObject*)0, Form("\tScintillation: %d",scint),"h");
		leg -> Draw();
		
		char* pref = "fig/fig_OpProcID";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c2 -> SaveAs(outname);
	}
	if(Opt[2])
	{
		int max_E=0;
		int max_wav = 0;
		for(int a=0; a<n_opt; a++)
		{
			H1_OpWav[a] = (TH1F*)F -> Get(Form("H1_OpWav_%s",cre_opt[a]));
			H1_OpE[a] = (TH1F*) F -> Get(Form("H1_OpE_%s",cre_opt[a]));
			H1_OpWav[a] -> SetStats(false);
			H1_OpWav[a] -> GetXaxis() -> SetTitle("#lambda [nm]");
			H1_OpWav[a] -> SetLineColor(a==3?46:a>=2?a+2:a+1);
			H1_OpE[a] -> SetStats(false);
			if(max_E < H1_OpE[a]->GetMaximum())
				max_E = H1_OpE[a] ->GetMaximum();
			if(max_wav < H1_OpWav[a] -> GetMaximum())
				max_wav = H1_OpWav[a] -> GetMaximum();
			H1_OpE[a] -> GetYaxis() -> SetRangeUser(0,1.2*max_E);
			H1_OpWav[a] -> GetYaxis() -> SetRangeUser(0,1.2*max_wav);
			H1_OpE[a] -> GetXaxis() -> SetTitle("Energy[eV]");
			H1_OpE[a] -> SetLineColor(a==3?46:a>=2?a+2:a+1);
		}
		TCanvas* c3 = new TCanvas("c3","c3",1.2*600*2,600*2);
		c3 -> Divide(2,2);
		TH1F* H1_wav_ratio = new TH1F("H1_wav_ratio","",600,300,900);
		TH1F* H1_E_ratio = new TH1F("H1_E_ratio","",350,1,4.5);

		H1_wav_ratio -> Clone(H1_OpWav[SiPM]);
		H1_E_ratio -> Clone(H1_OpE[SiPM]);
		H1_wav_ratio -> Divide(H1_OpWav[Total]);
		H1_E_ratio -> Divide(H1_OpE[Total]);
		H1_wav_ratio -> GetYaxis() -> SetRangeUser(0,0.6);
		H1_wav_ratio -> SetLineColor(1);
		H1_E_ratio -> GetYaxis() -> SetRangeUser(0,0.6);
		H1_E_ratio -> SetLineColor(1);

		c3 -> cd(3);
		H1_wav_ratio -> Draw();
		c3 -> cd(4);
		H1_E_ratio -> Draw();

//		cout << "SC2matOpt: " << map_MatProp["SC2matOpt"] << endl;
		const int SC2mat = stoi(map_MatProp["SC2matOpt"]);
		TLegend* leg1 = new TLegend(0.45,0.5-0.05*3,0.9,0.5+0.05*6);
		leg1 -> SetBorderSize(0);
		leg1 -> SetFillStyle(0);
		leg1 -> SetTextSize(0.04);
		leg1 -> AddEntry((TObject*)0,"Emission spectra(wavelength)","h");
		leg1 -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg1 -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
		TLegend* leg2 = new TLegend(0.15,0.5-0.05*3,0.6,0.5+0.05*6);
		leg2 -> SetBorderSize(0);
		leg2 -> SetFillStyle(0);
		leg2 -> SetTextSize(0.04);
		leg2 -> AddEntry((TObject*)0,"Emission spectra(Energy)","h");
		leg2 -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg2 -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
		for(int a=0; a<n_opt-1; a++)
		{
			leg1 -> AddEntry(H1_OpWav[a],Form("%s",cre_opt[a]),"l");
			leg2 -> AddEntry(H1_OpE[a],Form("%s",cre_opt[a]),"l");
			c3 -> cd(1);
			H1_OpWav[a] -> Draw("hist same");
			leg1 -> Draw();
			c3 -> cd(2);
			H1_OpE[a] -> Draw("hist same");
		}
		leg2 -> Draw();
		c3 -> cd(3);

		char* pref = "fig/fig_OpWavE";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c3 -> SaveAs(outname);
	}
	if(Opt[3])
	{
		H1_OpTime = (TH1F*) F -> Get("H1_OpTime");
		H1_OpTime -> SetStats(false);
		H1_OpTime -> GetXaxis() -> SetTitle("Time[ns]");
		H1_PostOpTime = (TH1F*) F -> Get("H1_PostOpTime");
		H1_PostOpTime -> SetStats(false);
		H1_PostOpTime -> GetXaxis() -> SetTitle("Time[ns]");

		const int SC2mat = stoi(map_MatProp["SC2matOpt"]);
		TLegend* leg1 = new TLegend(0.5,0.6,0.9,0.6+0.05*4);
		leg1 -> SetBorderSize(0);
		leg1 -> SetFillStyle(0);
		leg1 -> SetTextSize(0.04);
		leg1 -> AddEntry((TObject*)0,"Time of generation","h");
		leg1 -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg1 -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
		leg1 -> AddEntry((TObject*)0,Form("\tMean: %.2f ns",H1_OpTime->GetMean()),"h");

		TLegend* leg2 = new TLegend(0.5,0.6,0.9,0.6+0.05*4);
		leg2 -> SetBorderSize(0);
		leg2 -> SetFillStyle(0);
		leg2 -> SetTextSize(0.04);
		leg2 -> AddEntry((TObject*)0,"Time of death","h");
		leg2 -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg2 -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
		leg2 -> AddEntry((TObject*)0,Form("\tMean: %.2f ns",H1_PostOpTime->GetMean()),"h");

		TCanvas* c4 = new TCanvas("c4","c4",1.2*600*2,600);
		c4 -> Divide(2);
		c4 -> cd(1);
		H1_OpTime -> Draw("hist");
		leg1 -> Draw();
		c4 -> cd(2);
		H1_PostOpTime -> Draw("hist");
		leg2 -> Draw();

		char* pref = "fig/fig_OpTime";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c4 -> SaveAs(outname);
	}
	if(Opt[4])
	{
		cout << "Opt 4 is null " << endl;
	}
	if(Opt[5])
	{
		H2_Edep_L = (TH2F*) F -> Get("H2_Edep_L");
		TCanvas* c6 = new TCanvas("c6","c6",1.2*600,600);
		H2_Edep_L -> GetXaxis() -> SetTitle("Edep[MeV]");
		H2_Edep_L -> GetYaxis() -> SetTitle("LightYield");
		H2_Edep_L -> Draw("colz");

		TLegend* leg = new TLegend(0.15,0.75,0.5,0.75+0.05*2);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.04);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,"Total Edep - Total LY","h");
		leg -> Draw();

		char* pref = "fig/fig_Edep_L";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c6 -> SaveAs(outname);
	}
	if(Opt[6])
	{
		H2_dEdx_dLdx = (TH2F*) F -> Get("H2_dEdx_dLdx");
		TCanvas* c7 = new TCanvas("c7","c7",1.2*600,600);
		H2_dEdx_dLdx -> GetXaxis() -> SetTitle("dEdx[MeV/mm]");
		H2_dEdx_dLdx -> GetYaxis() -> SetTitle("dLdx[mm^{-1}]");

		char* pref = "fig/fig_dEdx_dLdx";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c7 -> SaveAs(outname);
	}
	if(Opt[7])
	{
		H2_LY_OpParentID = (TH2F*) F -> Get("H2_LY_OpParentID");
		TH1D* H1_ParentPDG = (TH1D*) H2_LY_OpParentID -> ProjectionY();
		H1_ParentPDG -> GetYaxis() -> SetRangeUser(0.1,1e4);
		TCanvas* c8 = new TCanvas("c8","c8",1.2*600,600);
		gPad -> SetLogy();
		H1_ParentPDG -> Draw("hist");
//		H1_OpParentID -> GetXaxis() -> SetBinContent()
	}
	if(Opt[8])
	{
		H1_TrackLength = (TH1F*) F -> Get("H1_TrackLength");
		H1_TrackLength -> Rebin(5);
		H1_TrackLength -> GetXaxis() -> SetTitle("Total length [mm]");
		H1_TrackLength -> GetYaxis() -> SetTitle("Number of photons");
		TCanvas* c9 = new TCanvas("c9","c9",1.2*600,600);
		gPad -> SetLogy();
		H1_TrackLength -> Draw("hist");
	}
	if(Opt[9])
	{
		H1_Time_Left = (TH1F*) F -> Get("H1_Time_Left");
		H1_Time_Right = (TH1F*) F -> Get("H1_Time_Right");
		TCanvas* c10 = new TCanvas("c10","c10",1.2*600,600);
		gPad -> SetLogy();
		H1_Time_Left -> SetStats(false);
		H1_Time_Left -> GetXaxis() -> SetTitle("Time[ns]");
		H1_Time_Right -> SetStats(false);
		H1_Time_Left -> SetLineColor(2);
		H1_Time_Right -> SetLineColor(4);
		H1_Time_Left -> Draw("hist");
		H1_Time_Right -> Draw("hist same");
		TLegend* leg = new TLegend(0.45,0.65,0.9,0.65+0.045*4);
		double SY = stod(map_MatProp["PVT_ScintYield"]);
		leg -> SetBorderSize(0);
		leg -> SetFillColor(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.038);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("Scintillation Yield: %.0f /MeV",SY),"h");
		leg -> AddEntry(H1_Time_Left,"Left SiPM","l");
		leg -> AddEntry(H1_Time_Right,"Right SiPM","l");
		leg -> Draw();

		char* pref = "fig/fig_SiPMTime_LR";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c10 -> SaveAs(outname);
	}
	if(Opt[10])
	{
		H1_NOp_Left = (TH1F*) F -> Get("H1_NOp_Left");
		H1_NOp_Right = (TH1F*) F -> Get("H1_NOp_Right");
		H1_NOp_Left -> SetStats(false);
		H1_NOp_Left -> GetXaxis() -> SetTitle("# of photons");
		H1_NOp_Right -> SetStats(false);
		TCanvas* c11 = new TCanvas("c11","c11",1.2*600,600);
		H1_NOp_Left -> SetLineColor(2);
		H1_NOp_Right -> SetLineColor(4);

		H1_NOp_Left -> Draw("hist");
		H1_NOp_Right -> Draw("hist same");

		double SY = stod(map_MatProp["PVT_ScintYield"]);
		TLegend* leg = new TLegend(0.4,0.5,0.9,0.6+0.045*4);
		leg -> SetBorderSize(0);
		leg -> SetFillColor(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.038);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("Scintillation Yield: %.1f /MeV",SY),"h");
		leg -> AddEntry(H1_NOp_Left,"Left SiPM","l");
		leg -> AddEntry((TObject*)0,
				Form("Mean: %.1f, StdDeV: %.1f",H1_NOp_Left->GetMean(),H1_NOp_Left->GetStdDev()),"");
		leg -> AddEntry(H1_NOp_Right,"Right SiPM","l");
		leg -> AddEntry((TObject*)0,
				Form("Mean: %.1f, StdDeV: %.1f",H1_NOp_Right->GetMean(),H1_NOp_Right->GetStdDev()),"");
		leg -> Draw();

		char* pref = "fig/fig_NOp_LR";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c11 -> SaveAs(outname);
	}
}
