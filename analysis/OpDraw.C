inline char* OutName(int opt, char* prefix, char* p, char* e, char* s, char* format)
{
	char* out;
	if      (opt == 1)
		out = Form("%s_%s.%s",prefix,p,format);
	else if (opt == 2)
		out = Form("%s_%s_%s.%s",prefix,p,s,format);
	else if (opt == 3)
		out = Form("%s_%s_%s.%s",prefix,p,e,format);
	else if (opt == 4)
		out = Form("%s_%s_%s_%s.%s",prefix,p,e,s,format);
	else
		out = "blank";
	return out;
}

void OpDraw
(
 char* particle = "proton",
 char* energy   = "100MeV",
 char* suffix   = "gap_LR"
)
{
	/*
		 HIST LISTs
		 0. 1-D Number of optical photon distribution (OpTrack)
		 1. 1-D creator process of optical photon     (OpTrack)
		 2. 1-D wavelength(energy) distribution       (OpTrack)
		 3. 1-D time of generation / died             (OpTrack || OpPostTrack)
		 4. 2-D momentum profile at boundary          (OpBoundary)
		 5. 2-D LY - Edep                             (MCStep)
		 6. 1-D total track length of optical photon  (OpPostTrack)
		 7. 1-D Number of optical photon at SiPM      (OpSiPM)
		 8. 1-D Time distribution at SiPM             (OpSiPM)
		 9. 1-D Time difference at SiPM LR            (OpSiPM);
		 10. 1-D ratio of photons                     (OpTrack && OpPostTrack && OpSiPM)
		     total, absorbed, transmitted to SiPM and detected(efficiency)
		 11. 2-D # of detected photon vs. Timing resolution (OpSiPM)
	 */
	const int n_Hist = 12;
	bool DrawOpt[n_Hist];
	DrawOpt[0] = 0;
	DrawOpt[1] = 0;
	DrawOpt[2] = 0;
	DrawOpt[3] = 0;
	DrawOpt[4] = 0;
	DrawOpt[5] = 0;
	DrawOpt[6] = 0;
	DrawOpt[7] = 1;
	DrawOpt[8] = 0;
	DrawOpt[9] = 0;
	DrawOpt[10] = 1;
	DrawOpt[11] = 0;

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

	TFile* F = new TFile(infile,"read");
	TList* L_MatProp = (TList*)F ->Get("ConstMaterialProperties");
	map<string,string> map_MatProp;
	for(int a=0; a<L_MatProp->GetSize(); a++)
	{
		TNamed* name = (TNamed*) L_MatProp -> At(a);
		map_MatProp.insert(make_pair(name->GetName(),name->GetTitle()));
	}
	
	double SY = stod(map_MatProp["PVT_ScintYield"]);
	if(DrawOpt[0])	// Number of o.p distribution
	{
		TCanvas* c0 = new TCanvas("c0","c0",1.2*600,600);
		gPad -> SetMargin(.13,.05,.12,.05);
		TH1F* H1_NOp = (TH1F*) F -> Get("H1_NOp");
		H1_NOp -> SetStats(false);
		H1_NOp -> Rebin(10);
		H1_NOp -> GetXaxis() -> SetTitle("# of photons");
		H1_NOp -> GetXaxis() -> SetTitleSize(0.05);
		H1_NOp -> GetXaxis() -> SetRangeUser(1000,5000);
		H1_NOp -> Draw("hist");

		TLegend* leg = new TLegend(0.5,0.6,0.9,0.6+0.05*4);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.045);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("SY: %.1f / MeV",SY),"h");
		leg -> AddEntry((TObject*)0,Form("Mean: %.1f",H1_NOp->GetMean()),"h");
		leg -> AddEntry((TObject*)0,Form("StdDev: %.1f",H1_NOp->GetStdDev()),"h");
		leg -> Draw();

		char* pref = "fig/fig_NOp";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c0 -> SaveAs(outname);
	}
	if(DrawOpt[1])	// Creation process distribution
	{
		TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
		TH1F* H1_OpProcID = (TH1F*) F -> Get("H1_OpProcID");
		H1_OpProcID -> SetStats(false);
		gPad -> SetLogy();
		gPad -> SetMargin(.13,.05,.12,.05);
		H1_OpProcID -> GetYaxis() -> SetRangeUser(0.9,1000*H1_OpProcID->GetBinContent(1));
		H1_OpProcID -> GetXaxis() -> SetLabelSize(0.05);
		H1_OpProcID -> GetXaxis() -> SetBinLabel(H1_OpProcID->FindBin(20),"Total");
		H1_OpProcID -> GetXaxis() -> SetBinLabel(H1_OpProcID->FindBin(21),"Cherenkov");
		H1_OpProcID -> GetXaxis() -> SetBinLabel(H1_OpProcID->FindBin(22),"Scintillation");
		H1_OpProcID -> GetXaxis() -> SetBinLabel(H1_OpProcID->FindBin(23),"Others");
		H1_OpProcID -> GetXaxis() -> SetLabelSize(0.06);
		H1_OpProcID -> Draw("hist text0");

		TLegend* leg = new TLegend(.5,.7,.9,.7+.05*4);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.045);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("SY: %.0f / MeV",SY),"h");
		const int ceren = H1_OpProcID -> GetBinContent(H1_OpProcID->FindBin(21));
		const int scint = H1_OpProcID -> GetBinContent(H1_OpProcID->FindBin(22));
		leg -> AddEntry((TObject*)0,Form("\tCherenkov: %d",ceren),"h");
		leg -> AddEntry((TObject*)0,Form("\tScintillation: %d",scint),"h");
		leg -> Draw();

		char* pref = "fig/fig_OpProcID";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c1 -> SaveAs(outname);
	}
	if(DrawOpt[2])	// Emission spectra per generation process
	{
		enum {Scint,Cheren};
		const char* cre_opt[] = {"Scintillation","Cherenkov"};
		const int n_cre = sizeof(cre_opt)/sizeof(cre_opt[0]);
		TH1F* H1_OpWav[n_cre];
		TH1F* H1_OpE[n_cre];
		int max_E = 0;
		int max_wav = 0;
		for(int a=0; a<n_cre; a++)
		{
			H1_OpWav[a] = (TH1F*) F -> Get(Form("H1_OpWav_%s",cre_opt[a]));
			H1_OpWav[a] -> SetStats(false);
			H1_OpWav[a] -> GetXaxis() -> SetTitle("#lambda [nm]");
			H1_OpWav[a] -> GetXaxis() -> SetTitleSize(0.05);
			H1_OpWav[a] -> SetLineColor(a==0?2:4);
			if(max_wav < H1_OpWav[a] -> GetMaximum())
				max_wav = H1_OpWav[a] -> GetMaximum();
			H1_OpE[a] = (TH1F*) F -> Get(Form("H1_OpE_%s",cre_opt[a]));
			H1_OpE[a] -> SetStats(false);
			H1_OpE[a] -> GetXaxis() -> SetTitle("Energy[eV]");
			H1_OpE[a] -> GetXaxis() -> SetTitleSize(0.05);
			H1_OpE[a] -> SetLineColor(a==0?2:4);
			if(max_E < H1_OpE[a] -> GetMaximum())
				max_E = H1_OpWav[a] -> GetMaximum();
		}
		TCanvas* c2 = new TCanvas("c2","c2",1.2*600*2,600);
		c2 -> Divide(2);
		c2 -> cd(1);
		gPad -> SetMargin(.13,.05,.12,.05);
		const int SC2mat = stoi(map_MatProp["SC2matOpt"]);
		TLegend* leg1 = new TLegend(.45,.6,.9,.6+.05*6);
		leg1 -> SetBorderSize(0);
		leg1 -> SetFillStyle(0);
		leg1 -> SetTextSize(0.045);
		TLegend* leg2 = new TLegend(.2,.6,.6,.6+.05*6);
		leg2 -> SetBorderSize(0);
		leg2 -> SetFillStyle(0);
		leg2 -> SetTextSize(0.045);
		leg1 -> AddEntry((TObject*)0,"Emission spectra(wavelength)","h");
		leg1 -> AddEntry((TObject*)0,Form("%s %s simulation",particle, energy),"h");
		leg1 -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
		leg1 -> AddEntry((TObject*)0,Form("SY: %.0f / MeV",SY),"h");
		leg2 -> AddEntry((TObject*)0,"Emission spectra(Energy)","h");
		leg2 -> AddEntry((TObject*)0,Form("%s %s simulation",particle, energy),"h");
		leg2 -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
		leg2 -> AddEntry((TObject*)0,Form("SY: %.0f / MeV",SY),"h");
		for(int a=0; a<n_cre; a++)
		{
			leg1 -> AddEntry(H1_OpWav[a],Form("%s",cre_opt[a]),"l");
			H1_OpWav[a] -> Draw("hist same");
		}
		leg1 -> Draw();
		c2 -> cd(2);
		gPad -> SetMargin(.13,.05,.12,.05);
		for(int a=0; a<n_cre; a++)
		{
			leg2 -> AddEntry(H1_OpE[a],Form("%s",cre_opt[a]),"l");
			H1_OpE[a] -> Draw("hist same");
		}
		leg2 -> Draw();

		char* pref = "fig/fig_OpEmission";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c2 -> SaveAs(outname);
	}
	if(DrawOpt[3])
	{
		TH1F* H1_OpTime = (TH1F*) F -> Get("H1_OpTime");
		H1_OpTime -> SetStats(false);
		H1_OpTime -> GetXaxis() -> SetTitle("Time[ns]");
		H1_OpTime -> GetXaxis() -> SetTitleSize(0.05);
		H1_OpTime -> SetLineColor(2);
		TH1F* H1_PostOpTime = (TH1F*) F -> Get("H1_PostOpTime");
		H1_PostOpTime -> SetStats(false);
		H1_PostOpTime -> GetXaxis() -> SetTitle("Time[ns]");
		H1_PostOpTime -> GetXaxis() -> SetTitleSize(0.05);
		H1_PostOpTime -> SetLineColor(4);
		TCanvas* c3 = new TCanvas("c3","c3",1.2*600,600);
		gPad -> SetMargin(.12,.05,.13,.05);
		TLegend* leg = new TLegend(.5,.6,.9,.6+.052*5);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.045);
		const int SC2mat = stoi(map_MatProp["SC2matOpt"]);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
		leg -> AddEntry((TObject*)0,Form("Decay Time: %.1f ns",1.5),"h");
		leg -> AddEntry(H1_OpTime,"Generation Time","l");
		leg -> AddEntry(H1_PostOpTime,"Died Time","l");

		H1_OpTime -> Draw("hist same");
		H1_PostOpTime -> Draw("hist same");
		leg -> Draw();

		char* pref = "fig/fig_Time";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c3 -> SaveAs(outname);
	}
	if(DrawOpt[4])
	{
	}
	if(DrawOpt[5])
	{
		TH2F* H2_Edep_LY = (TH2F*) F -> Get("H2_Edep_LY");
		H2_Edep_LY -> SetStats(false);
		H2_Edep_LY -> GetXaxis() -> SetTitle("Edep[MeV]");
		H2_Edep_LY -> GetYaxis() -> SetTitle("LightYield");
		H2_Edep_LY -> GetXaxis() -> SetRangeUser(0,10);
		H2_Edep_LY -> GetYaxis() -> SetRangeUser(0,50);
		TH2F* H2_dEdx_dLdx = (TH2F*) F -> Get("H2_dEdx_dLdx");
		H2_dEdx_dLdx -> SetStats(false);
		H2_dEdx_dLdx -> GetXaxis() -> SetTitle("dEdx[MeV/mm]");
		H2_dEdx_dLdx -> GetYaxis() -> SetTitle("dLdx[mm^{-1}]");
		H2_dEdx_dLdx -> GetXaxis() -> SetRangeUser(0,5);
		H2_dEdx_dLdx -> GetYaxis() -> SetRangeUser(0,250);
		TCanvas* c5 = new TCanvas("c5","c5",1.2*600*2,600);
		c5 -> Divide(2);
		c5 -> cd(1);
		gPad -> SetMargin(.12,.05,.13,.05);
		H2_Edep_LY -> Draw("colz");
		TLegend* leg1 = new TLegend(.15,.75,.5,.75+.052*2);
		leg1 -> SetBorderSize(0);
		leg1 -> SetFillStyle(0);
		leg1 -> SetTextSize(0.045);
		leg1 -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg1 -> AddEntry((TObject*)0,"Total Edep - Total LY","h");
		leg1 -> Draw();
		c5 -> cd(2);
		H2_dEdx_dLdx -> Draw("colz");
		gPad -> SetMargin(.12,.05,.13,.05);
		TLegend* leg2 = new TLegend(.15,.75,.5,.75+.052*2);
		leg2 -> SetBorderSize(0);
		leg2 -> SetFillStyle(0);
		leg2 -> SetTextSize(0.045);
		leg2 -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg2 -> AddEntry((TObject*)0,"dEdx - dLdx","h");
		leg2 -> Draw();

		char* pref = "fig/fig_E_L";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c5 -> SaveAs(outname);
	}
	if(DrawOpt[6])
	{
		TH1F* H1_OpTrackLength = (TH1F*) F -> Get("H1_OpTrackLength");
		H1_OpTrackLength -> SetStats(false);
		H1_OpTrackLength -> GetXaxis() -> SetTitle("TrackLength[mm]");
		H1_OpTrackLength -> GetXaxis() -> SetTitleSize(0.05);
		H1_OpTrackLength -> GetXaxis() -> SetRangeUser(0,1000);
		H1_OpTrackLength -> GetYaxis() -> SetRangeUser(0,1e3);
		TCanvas* c6 = new TCanvas("c6","c6",1.2*600,600);
		gPad -> SetMargin(.13,.05,.12,.05);

		TLegend* leg = new TLegend(.4,.6,.9,.6+0.05*4);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.045);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("photon track length distribution"),"h");
		leg -> AddEntry((TObject*)0,Form("Attenuation Length: %.1f m",1.2),"h");
		leg -> AddEntry(H1_OpTrackLength,Form("Mean: %.1f",H1_OpTrackLength->GetMean()),"l");
		H1_OpTrackLength -> Draw("hist");
		leg -> Draw();

		char* pref = "fig/fig_OpTrackLength";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c6 -> SaveAs(outname);
	}
	if(DrawOpt[7])
	{
		const int x_edge = 700; // for SY1000
//		const int x_edge = 100; // for SY100
//		const int x_edge = 30; // for SY10
		const int rebin = 10; // for SY1000
//		const int rebin = 5; // for SY100
//		const int rebin = 1; // for SY100
		const int y_max = 500;
			//2*H1_NOpSiPM_Le -> GetMaximum(); // for SY100

		// SY1000
		const int fit_x_min = 400;
		const int fit_x_max = 600;
		const int fit_xe_min = 0;
		const int fit_xe_max = 200;
		
		//SY100
//		const int fit_x_min = 30;
//		const int fit_x_max = 70;
//		const int fit_xe_min = 0;
//		const int fit_xe_max = 30;
		//SY10
//		const int fit_x_min = 0;
//		const int fit_x_max = 10;
//		const int fit_xe_min = 0;
//		const int fit_xe_max = 5;

		bool LR = map_MatProp["SiPMLR"]=="true"?true:false;
		bool UD = map_MatProp["SiPMUD"]=="true"?true:false;
		
		TH1F* H1_NOpSiPM_Le;
		TH1F* H1_NOpSiPM_Re;
		TH1F* H1_NOpSiPM_Ue;
		TH1F* H1_NOpSiPM_De;
		TF1* fLe;
		TF1* fRe;
		TF1* fUe;
		TF1* fDe;
		if(LR)
		{
			H1_NOpSiPM_Le = (TH1F*) F -> Get("H1_NOpSiPM_Le");
			H1_NOpSiPM_Re = (TH1F*) F -> Get("H1_NOpSiPM_Re");
			fLe = new TF1("fL","gaus",fit_xe_min,fit_xe_max);
			fRe = new TF1("fR","gaus",fit_xe_min,fit_xe_max);
			H1_NOpSiPM_Le -> SetStats(false);
			H1_NOpSiPM_Re -> SetStats(false);
			H1_NOpSiPM_Le -> Rebin(rebin);
			H1_NOpSiPM_Re -> Rebin(rebin);
			H1_NOpSiPM_Le -> GetXaxis() -> SetTitle("# of photons");
			H1_NOpSiPM_Le -> SetLineStyle(2);
			H1_NOpSiPM_Le -> SetLineColor(2);
			H1_NOpSiPM_Le -> GetXaxis() -> SetTitleSize(0.05);
			H1_NOpSiPM_Le -> GetXaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_Le -> GetYaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_Le -> GetXaxis() -> SetRangeUser(0,x_edge);
			H1_NOpSiPM_Le -> GetYaxis() -> SetRangeUser(0,y_max);
			H1_NOpSiPM_Re -> GetXaxis() -> SetTitle("# of photons");
			H1_NOpSiPM_Re -> SetLineStyle(2);
			H1_NOpSiPM_Re -> SetLineColor(4);
			H1_NOpSiPM_Re -> GetXaxis() -> SetTitleSize(0.05);
			H1_NOpSiPM_Re -> GetXaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_Re -> GetYaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_Re -> GetXaxis() -> SetRangeUser(0,x_edge);
			H1_NOpSiPM_Re -> GetYaxis() -> SetRangeUser(0,y_max);
			fLe -> SetLineColor(2);
			fLe -> SetLineStyle(2);
			fLe -> SetLineWidth(3);
			H1_NOpSiPM_Le -> Fit(fLe,"R0Q");
			fRe -> SetLineColor(4);
			fRe -> SetLineStyle(2);
			fRe -> SetLineWidth(3);
			H1_NOpSiPM_Re -> Fit(fRe,"R0Q");
		}
		if(UD)
		{
			H1_NOpSiPM_Ue = (TH1F*) F -> Get("H1_NOpSiPM_Ue");
			H1_NOpSiPM_De = (TH1F*) F -> Get("H1_NOpSiPM_De");
			fUe = new TF1("fU","gaus",fit_xe_min,fit_xe_max);
			fDe = new TF1("fD","gaus",fit_xe_min,fit_xe_max);
			H1_NOpSiPM_Ue -> SetStats(false);
			H1_NOpSiPM_De -> SetStats(false);
			H1_NOpSiPM_Ue -> Rebin(rebin);
			H1_NOpSiPM_De -> Rebin(rebin);
			H1_NOpSiPM_Ue -> SetLineStyle(1);
			H1_NOpSiPM_Ue -> GetXaxis() -> SetTitle("# of photons");
			H1_NOpSiPM_Ue -> SetLineColor(2);
			H1_NOpSiPM_Ue -> GetXaxis() -> SetTitleSize(0.05);
			H1_NOpSiPM_Ue -> GetXaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_Ue -> GetYaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_Ue -> GetXaxis() -> SetRangeUser(0,x_edge);
			H1_NOpSiPM_Ue -> GetYaxis() -> SetRangeUser(0,y_max);
			H1_NOpSiPM_De -> SetLineStyle(1);
			H1_NOpSiPM_De -> GetXaxis() -> SetTitle("# of photons");
			H1_NOpSiPM_De -> SetLineColor(4);
			H1_NOpSiPM_De -> GetXaxis() -> SetTitleSize(0.05);
			H1_NOpSiPM_De -> GetXaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_De -> GetYaxis() -> SetLabelSize(0.045);
			H1_NOpSiPM_De -> GetXaxis() -> SetRangeUser(0,x_edge);
			H1_NOpSiPM_De -> GetYaxis() -> SetRangeUser(0,y_max);
			TF1* fUe = new TF1("fLe","gaus",fit_xe_min,fit_xe_max);
			fUe -> SetLineColor(2);
			fUe -> SetLineStyle(1);
			fUe -> SetLineWidth(3);
			H1_NOpSiPM_Ue -> Fit(fUe,"R0Q");
			TF1* fDe = new TF1("fDe","gaus",fit_xe_min,fit_xe_max);
			fDe -> SetLineColor(4);
			fDe -> SetLineStyle(1);
			fDe -> SetLineWidth(3);
			H1_NOpSiPM_De -> Fit(fDe,"R0Q");
		}

		TLegend* leg = new TLegend(.45,.9-.05*(3+2*LR+2*UD),.9,.9);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.04);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("# of photon at SiPM"),"h");
		leg -> AddEntry((TObject*)0,Form("SY: %.1f / MeV",SY),"h");
		if(LR)
		{
			leg -> AddEntry(fLe,
					Form("Mean_{L}: %.1f, #sigma_{L}: %.1f",
						fLe->GetParameter(1),fLe->GetParameter(2)),"l");
			leg -> AddEntry(fRe,
					Form("Mean_{R}: %.1f, #sigma_{R}: %.1f",
						fRe->GetParameter(1),fRe->GetParameter(2)),"l");
		}
		if(UD)
		{
			leg -> AddEntry(fUe,
					Form("Mean_{U}: %.1f, #sigma_{U}: %.1f",
						fUe->GetParameter(1),fUe->GetParameter(2)),"l");
			leg -> AddEntry(fDe,
					Form("Mean_{D}: %.1f, #sigma_{D}: %.1f",
						fDe->GetParameter(1),fDe->GetParameter(2)),"l");
		}
		TCanvas* c7 = new TCanvas("c7","c7",1.2*600,600);
		gPad -> SetMargin(.13,.05,.12,.05);
		if(LR)
		{
			H1_NOpSiPM_Le -> Draw("hist same");
			H1_NOpSiPM_Re -> Draw("hist same");
			fLe -> Draw("same");
			fRe -> Draw("same");
		}
		if(UD)
		{
			H1_NOpSiPM_Ue -> Draw("hist same");
			H1_NOpSiPM_De -> Draw("hist same");
			fUe -> Draw("same");
			fDe -> Draw("same");
		}

		leg -> Draw();

		char* pref = "fig/fig_NOpSiPM";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c7 -> SaveAs(outname);
	}

	if(DrawOpt[8])
	{
		TH1F* H1_OpSiPMTime_Le = (TH1F*) F -> Get("H1_OpSiPMTime_Le");
		const int maxY = H1_OpSiPMTime_Le -> GetMaximum();
		// SY1000
		const double fit_x_min = 1.7;
		const double fit_x_max = 2.1;
		const double fit_xe_min = 1.5;
		const double fit_xe_max = 2.5;
		const double edge_x_min = 1;
		const double edge_x_max = 5;
		// SY100
//		const double fit_x_min = 1.5;
//		const double fit_x_max = 2.5;
//		const double fit_xe_min = 1.0;
//		const double fit_xe_max = 3.0;
		// SY10
//		const double fit_x_min = 0;
//		const double fit_x_max = 4;
//		const double fit_xe_min = 0;
//		const double fit_xe_max = 4.0;
		H1_OpSiPMTime_Le -> SetStats(false);
		H1_OpSiPMTime_Le -> GetXaxis() -> SetTitle("<Time> [ns]");
		H1_OpSiPMTime_Le -> GetXaxis() -> SetRangeUser(edge_x_min,edge_x_max);
		H1_OpSiPMTime_Le -> GetYaxis() -> SetRangeUser(0,1.2*maxY);
		H1_OpSiPMTime_Le -> GetXaxis() -> SetTitleSize(0.05);
		H1_OpSiPMTime_Le -> SetLineColor(2);
		TF1* fLe = new TF1("fLe","gaus",fit_x_min,fit_x_max);
		fLe -> SetLineColor(2);
		fLe -> SetLineStyle(2);
		fLe -> SetLineWidth(2);
		H1_OpSiPMTime_Le -> Fit(fLe,"R0Q");
		TH1F* H1_OpSiPMTime_Re = (TH1F*) F -> Get("H1_OpSiPMTime_Re");
		H1_OpSiPMTime_Re -> SetStats(false);
		H1_OpSiPMTime_Re -> GetXaxis() -> SetTitle("<Time> [ns]");
		H1_OpSiPMTime_Re -> GetXaxis() -> SetRangeUser(edge_x_min,edge_x_max);
		H1_OpSiPMTime_Re -> GetXaxis() -> SetTitleSize(0.05);
		H1_OpSiPMTime_Re -> SetLineColor(4);
		TF1* fRe = new TF1("fRe","gaus",fit_x_min,fit_x_max);
		fRe -> SetLineColor(4);
		fRe -> SetLineStyle(2);
		fRe -> SetLineWidth(2);
		H1_OpSiPMTime_Re -> Fit(fRe,"R0Q");
		TH1F* H1_OpSiPMTime_Ue = (TH1F*) F -> Get("H1_OpSiPMTime_Ue");
		H1_OpSiPMTime_Ue -> SetStats(false);
		H1_OpSiPMTime_Ue -> GetXaxis() -> SetTitle("<Time> [ns]");
		H1_OpSiPMTime_Ue -> GetXaxis() -> SetRangeUser(edge_x_min,edge_x_max);
		H1_OpSiPMTime_Ue -> GetYaxis() -> SetRangeUser(0,1.2*maxY);
		H1_OpSiPMTime_Ue -> GetXaxis() -> SetTitleSize(0.05);
		H1_OpSiPMTime_Ue -> SetLineColor(2);
		TF1* fUe = new TF1("fUe","gaus",fit_xe_min,fit_xe_max);
		fUe -> SetLineColor(2);
		fUe -> SetLineStyle(1);
		fUe -> SetLineWidth(2);
		H1_OpSiPMTime_Ue -> Fit(fUe,"R0Q");
		TH1F* H1_OpSiPMTime_De = (TH1F*) F -> Get("H1_OpSiPMTime_De");
		H1_OpSiPMTime_De -> SetStats(false);
		H1_OpSiPMTime_De -> GetXaxis() -> SetTitle("<Time> [ns]");
		H1_OpSiPMTime_De -> GetXaxis() -> SetRangeUser(edge_x_min,edge_x_max);
		H1_OpSiPMTime_De -> GetXaxis() -> SetTitleSize(0.05);
		H1_OpSiPMTime_De -> SetLineColor(4);
		TF1* fDe = new TF1("fDe","gaus",fit_xe_min,fit_xe_max);
		fDe -> SetLineColor(4);
		fDe -> SetLineStyle(1);
		fDe -> SetLineWidth(2);
		H1_OpSiPMTime_De -> Fit(fDe,"R0Q");

		TLegend* leg = new TLegend(.43,.5,.9,.5+.055*7);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.045);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,"Time distribution at SiPM","h");
		leg -> AddEntry((TObject*)0,Form("SY: %.1f / MeV",SY),"h");
		leg -> AddEntry(fLe,
				Form("Mean_{L}: %.3f, #sigma_{L} : %.3f",
					fLe->GetParameter(1),fLe->GetParameter(2)),"l");
		leg -> AddEntry(fRe,
				Form("Mean_{R}: %.3f, #sigma_{R} : %.3f",
					fRe->GetParameter(1),fRe->GetParameter(2)),"l");
		leg -> AddEntry(fUe,
				Form("Mean_{U}: %.3f, #sigma_{L} : %.3f",
					fUe->GetParameter(1),fUe->GetParameter(2)),"l");
		leg -> AddEntry(fDe,
				Form("Mean_{D}: %.3f, #sigma_{R} : %.3f",
					fDe->GetParameter(1),fDe->GetParameter(2)),"l");

		TCanvas* c8 = new TCanvas("c8","c8",1.2*600,600);
		gPad -> SetMargin(.13,.05,.12,.05);
		H1_OpSiPMTime_Le -> Draw("hist same");
		H1_OpSiPMTime_Re -> Draw("hist same");
		H1_OpSiPMTime_Ue -> Draw("hist same");
		H1_OpSiPMTime_De -> Draw("hist same");
		fLe -> Draw("same");
		fRe -> Draw("same");
		fUe -> Draw("same");
		fDe -> Draw("same");
		leg -> Draw("same");

		char* pref = "fig/fig_OpSiPMTime";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c8 -> SaveAs(outname);
	}
	if(DrawOpt[9])
	{
		// for SY1000
		const double fit_x_min = -0.5;
		const double fit_x_max = 0.5;
		const double edge_x_min = -1;
		const double edge_x_max = 3;
		const double y_max = 400;
		// for SY100
//		const double fit_x_min = -1;
//		const double fit_x_max = 1;
		// for SY10
//		const double fit_x_min = -3;
//		const double fit_x_max = 3;
//		const double edge_x_min = -5;
//		const double edge_x_max = 10;
		const int rebin = 10;
		TH1F* H1_SiPMTimeDiff = (TH1F*) F -> Get("H1_SiPMTimeDiff");
		H1_SiPMTimeDiff -> Rebin(rebin);
		H1_SiPMTimeDiff -> SetStats(false);
		H1_SiPMTimeDiff -> SetLineColor(4);
		H1_SiPMTimeDiff -> GetXaxis() -> SetTitle("#Delta t [ns]");
		H1_SiPMTimeDiff -> GetXaxis() -> SetTitleSize(0.05);
		H1_SiPMTimeDiff -> GetXaxis() -> SetRangeUser(edge_x_min,edge_x_max);
		H1_SiPMTimeDiff -> GetYaxis() -> SetRangeUser(0,y_max);
		H1_SiPMTimeDiff -> GetYaxis() -> SetTitle("Entries");
		H1_SiPMTimeDiff -> GetYaxis() -> SetTitleSize(0.05);
//		TH1F* H1_SiPMTimeDiff_eg = (TH1F*) F -> Get("H1_SiPMTimeDiff_eg");
//		H1_SiPMTimeDiff_eg -> Rebin(rebin);
//		H1_SiPMTimeDiff_eg -> SetStats(false);
//		H1_SiPMTimeDiff_eg -> SetLineColor(2);
//		H1_SiPMTimeDiff_eg -> GetXaxis() -> SetTitle("#Delta t [ns]");
//		H1_SiPMTimeDiff_eg -> GetXaxis() -> SetTitleSize(0.05);
//		H1_SiPMTimeDiff_eg -> GetXaxis() -> SetRangeUser(edge_x_min,edge_x_max);
//		H1_SiPMTimeDiff -> GetYaxis() -> SetRangeUser(0,y_max);
//		H1_SiPMTimeDiff_eg -> GetYaxis() -> SetTitle("Entries");
//		H1_SiPMTimeDiff_eg -> GetYaxis() -> SetTitleSize(0.05);
		TF1* f1 = new TF1("f1","gaus",fit_x_min,fit_x_max);
		f1 -> SetLineColor(4);
		f1 -> SetLineStyle(1);
		f1 -> SetLineWidth(4);
		H1_SiPMTimeDiff -> Fit(f1,"R0Q");
//		TF1* f1e = new TF1("f1e","gaus",fit_x_min,fit_x_max);
//		f1e -> SetLineColor(2);
//		f1e -> SetLineStyle(1);
//		f1e -> SetLineWidth(4);
//		H1_SiPMTimeDiff_eg -> Fit(f1e,"R0Q");
		const double mean = f1 -> GetParameter(1);
		const double sigma = f1 -> GetParameter(2);
//		const double mean_e = f1e -> GetParameter(1);
//		const double sigma_e = f1e -> GetParameter(2);
//		const double sigma_ee = f1e -> GetParError(2);
		
		
		TLegend* leg = new TLegend(0.45,0.6,0.9,0.6+0.055*5);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.045);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle,energy),"h");
		leg -> AddEntry((TObject*)0,Form("T_{L}-T_{R} distribution"),"h");
		leg -> AddEntry((TObject*)0,Form("Scintillation Yield: %.1f / MeV",SY),"h");
		leg -> AddEntry(f1,Form("Mean: %.3f, #sigma: %.3f",mean,sigma),"l");
//		leg -> AddEntry(f1e,Form("Mean_{e}: %.3f, #sigma_{e}: %.3f",mean_e,sigma_e),"l");
//		leg -> AddEntry(f1e,Form("sigma_{e}: %.3f #pm %.3f",sigma_e, sigma_ee),"l");

		TCanvas* c9 = new TCanvas("c9","c9",1.2*600,600);
		gPad -> SetMargin(.13,.05,.12,.05);
		H1_SiPMTimeDiff -> Draw("hist same");
//		H1_SiPMTimeDiff_eg -> Draw("hist same");
		f1 -> Draw("same");
//		f1e -> Draw("same");
		leg -> Draw();

		char* pref = "fig/fig_SiPMTimeDiff";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c9 -> SaveAs(outname);
	}
	if(DrawOpt[10])
	{
		TH1D* H1_ratio = (TH1D*) F -> Get("H1_ratio");
		H1_ratio -> SetStats(false);
		const double tot = H1_ratio -> GetBinContent(1);
		cout << tot << endl;
		H1_ratio -> Scale(1/tot);
		H1_ratio -> GetYaxis() -> SetRangeUser(0,1.2);
		H1_ratio -> GetYaxis() -> SetTitle("ratio(x/Gen)");
		H1_ratio -> GetXaxis() -> SetRangeUser(0,6);
		const char* str_x[]= {"Gen","Abs","Trans","SiPM In","Geom","Geom+Eff"};
		H1_ratio -> GetXaxis() -> SetLabelSize(0.05);
		for(int x=0; x<H1_ratio->GetNbinsX(); x++)
		{
			H1_ratio -> GetXaxis() -> SetBinLabel(x+1,str_x[x]);
		}

		TLegend* leg = new TLegend(.5,.78,.9,.78+0.05*3);
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.045);
		leg -> AddEntry((TObject*)0,Form("%s %s simulation",particle, energy),"h");
		leg -> AddEntry((TObject*)0,Form("# of photon ratio"),"h");
		leg -> AddEntry((TObject*)0,Form("SY = %.1f / MeV",SY),"h");

		TCanvas* c10 = new TCanvas("c10","c10",1.2*600,600);
		gPad -> SetMargin(.13,.05,.12,.05);
		H1_ratio -> Draw("hist text0");
		leg -> Draw();

		char* pref = "fig/fig_ratio";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c10 -> SaveAs(outname);
	}
	if(DrawOpt[11])
	{
		TH2F* H2_dLY_TimeRes = (TH2F*) F -> Get("H2_dLY_TimeRes");
		H2_dLY_TimeRes -> SetStats(false);
		double x[] = {(double) H2_dLY_TimeRes -> ProjectionX() -> GetMean()};
		double xe[] = {(double) H2_dLY_TimeRes -> ProjectionX() -> GetStdDev()};
		double y[] = {(double) 0.5*H2_dLY_TimeRes -> ProjectionY() -> GetStdDev()};
		double ye[] = {(double) H2_dLY_TimeRes -> ProjectionY() -> GetStdDevError()};
		TGraphErrors* g = new TGraphErrors(1,x,y,xe,ye);
//		g -> SetName(Form("g_dLY_TimeRes_%s_%s_%s",particle,energy,suffix));
		g -> SetMarkerStyle(20);
		g -> SetMarkerSize(2);
		g -> SetMarkerColor(1);

		TCanvas* c11 = new TCanvas("c11","c11",1.2*600,600);
		gPad -> SetMargin(.13,.05,.12,.05);
		TH1D* htemp = (TH1D*) gPad -> DrawFrame(0,0.001,200,1);
		htemp -> GetXaxis() -> SetTitle("#LTLY_{SiPM}#GT");
		htemp -> GetXaxis() -> SetTitleSize(0.05);
		htemp -> GetYaxis() -> SetTitle("#sigma(t_{L}-t_{R})/2");
		htemp -> GetYaxis() -> SetTitleSize(0.05);
		g -> Draw("p");

		char* pref = "fig/fig_dLY_TimeRes";
		char* outname = OutName(name_opt,pref,particle,energy,suffix,"pdf");
		c11 -> SaveAs(outname);
	}
}
