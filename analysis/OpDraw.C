inline char *OutName(int opt, char *prefix, char *p, char *e, char *s, char *format)
{
	char *out;
	if (opt == 1)
		out = Form("%s_%s.%s", prefix, p, format);
	else if (opt == 2)
		out = Form("%s_%s_%s.%s", prefix, p, s, format);
	else if (opt == 3)
		out = Form("%s_%s_%s.%s", prefix, p, e, format);
	else if (opt == 4)
		out = Form("%s_%s_%s_%s.%s", prefix, p, e, s, format);
	else
		out = "blank";
	return out;
}

void OpDraw(
		char *particle = "Sn",
		char *energy = "250MeVu",
		char *suffix = "SY9700_1.0")
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
		 8. 1-D detected photon energy distribution   (OpSiPM)
		 		1-D detected photon time distribution
				1-D detected photon average time dist
		 9. 1-D Time difference at SiPM LRUD          (OpSiPM);
		 10. 1-D ratio of photons                     (OpTrack && OpPostTrack && OpSiPM)
				 total, absorbed, transmitted to SiPM and detected(efficiency)
		 11.(7+9) 2-D # of detected photon vs. Timing resolution (OpSiPM)
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
	DrawOpt[9] = 1;
	DrawOpt[10] = 0;
	DrawOpt[11] = 1;
	bool text_output_LY_res = 1;

	char *in_pref = "out_root/H_Op_out";
	int name_opt = 0;
	TString str_p = particle;
	TString str_s = suffix;
	if (str_p.Contains("temp"))
	{
		if (str_s == "")
			name_opt = 1;
		else
			name_opt = 2;
	}
	else
	{
		if (str_s == "")
			name_opt = 3;
		else
			name_opt = 4;
	}
	char *infile = OutName(name_opt, in_pref, particle, energy, suffix, "root");
	cout << "infile: " << infile << endl;

	TFile *F = new TFile(infile, "read");
	TList *L_MatProp = (TList *)F->Get("ConstMaterialProperties");
	map<string, string> map_MatProp;
	for (int a = 0; a < L_MatProp->GetSize(); a++)
	{
		TNamed *name = (TNamed *)L_MatProp->At(a);
		map_MatProp.insert(make_pair(name->GetName(), name->GetTitle()));
	}

	double SY = stod(map_MatProp["PVT_ScintYield"]);
	if (DrawOpt[0]) // Number of o.p distribution
	{
		TCanvas *c0 = new TCanvas("c0", "c0", 1.2 * 600, 600);
		gPad->SetMargin(.13, .05, .12, .05);
		TH1F *H1_NOp = (TH1F *)F->Get("H1_NOp");
		H1_NOp->SetStats(false);
		H1_NOp->Rebin(100);
		H1_NOp->GetXaxis()->SetTitle("# of photons");
		H1_NOp->GetXaxis()->SetTitleSize(0.05);
		H1_NOp->GetXaxis()->SetRangeUser(1000, 5000);
		H1_NOp->Draw("hist");

		TLegend *leg = new TLegend(0.5, 0.6, 0.9, 0.6 + 0.05 * 4);
		leg->SetBorderSize(0);
		leg->SetFillStyle(0);
		leg->SetTextSize(0.045);
		leg->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg->AddEntry((TObject *)0, Form("SY: %.1f / MeV", SY), "h");
		leg->AddEntry((TObject *)0, Form("Mean: %.1f", H1_NOp->GetMean()), "h");
		leg->AddEntry((TObject *)0, Form("StdDev: %.1f", H1_NOp->GetStdDev()), "h");
		leg->Draw();

		char *pref = "fig/fig_NOp";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c0->SaveAs(outname);
	}
	if (DrawOpt[1]) // Creation process distribution
	{
		TCanvas *c1 = new TCanvas("c1", "c1", 1.2 * 600, 600);
		TH1F *H1_OpProcID = (TH1F *)F->Get("H1_OpProcID");
		H1_OpProcID->SetStats(false);
		gPad->SetLogy();
		gPad->SetMargin(.13, .05, .12, .05);
		H1_OpProcID->GetYaxis()->SetRangeUser(1e4, 1000 * H1_OpProcID->GetBinContent(1));
		H1_OpProcID->GetXaxis()->SetLabelSize(0.05);
		H1_OpProcID->GetXaxis()->SetBinLabel(H1_OpProcID->FindBin(20), "Total");
		H1_OpProcID->GetXaxis()->SetBinLabel(H1_OpProcID->FindBin(21), "Cherenkov");
		H1_OpProcID->GetXaxis()->SetBinLabel(H1_OpProcID->FindBin(22), "Scintillation");
		H1_OpProcID->GetXaxis()->SetBinLabel(H1_OpProcID->FindBin(23), "Others");
		H1_OpProcID->GetXaxis()->SetLabelSize(0.06);
		H1_OpProcID->Draw("hist text0");

		TLegend *leg = new TLegend(.5, .7, .9, .7 + .05 * 4);
		leg->SetBorderSize(0);
		leg->SetFillStyle(0);
		leg->SetTextSize(0.045);
		leg->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg->AddEntry((TObject *)0, Form("SY: %.0f / MeV", SY), "h");
		const int ceren = H1_OpProcID->GetBinContent(H1_OpProcID->FindBin(21));
		const int scint = H1_OpProcID->GetBinContent(H1_OpProcID->FindBin(22));
		leg->AddEntry((TObject *)0, Form("\tCherenkov: %d", ceren), "h");
		leg->AddEntry((TObject *)0, Form("\tScintillation: %d", scint), "h");
		leg->Draw();

		char *pref = "fig/fig_OpProcID";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c1->SaveAs(outname);
	}
	if (DrawOpt[2]) // Emission spectra per generation process
	{
		enum
		{
			Scint,
			Cheren
		};
		const char *cre_opt[] = {"Scintillation"}; //,"Cherenkov"};
		const int n_cre = sizeof(cre_opt) / sizeof(cre_opt[0]);
		TH1F *H1_OpWav[n_cre];
		TH1F *H1_OpE[n_cre];
		int max_E = 0;
		int max_wav = 0;
		for (int a = 0; a < n_cre; a++)
		{
			H1_OpWav[a] = (TH1F *)F->Get(Form("H1_OpWav_%s", cre_opt[a]));
			H1_OpWav[a]->SetStats(false);
			H1_OpWav[a]->GetXaxis()->SetTitle("Wavelength [nm]");
			H1_OpWav[a]->GetXaxis()->SetTitleSize(0.05);
			H1_OpWav[a]->GetXaxis()->SetLabelSize(0.045);
			H1_OpWav[a]->GetYaxis()->SetLabelSize(0.045);
			H1_OpWav[a]->SetLineColor(a == 0 ? 2 : 4);
			if (max_wav < H1_OpWav[a]->GetMaximum())
				max_wav = H1_OpWav[a]->GetMaximum();
			H1_OpE[a] = (TH1F *)F->Get(Form("H1_OpE_%s", cre_opt[a]));
			H1_OpE[a]->SetStats(false);
			H1_OpE[a]->GetXaxis()->SetTitle("Energy [eV]");
			H1_OpE[a]->GetXaxis()->SetTitleSize(0.05);
			H1_OpE[a]->GetXaxis()->SetLabelSize(0.045);
			H1_OpE[a]->GetYaxis()->SetLabelSize(0.045);
			H1_OpE[a]->SetLineColor(a == 0 ? 2 : 4);
			if (max_E < H1_OpE[a]->GetMaximum())
				max_E = H1_OpE[a]->GetMaximum();
		}
		ifstream MP("MP.txt");
		string line;
		getline(MP, line);
		vector<double> vec_wav, vec_E, vec_FC_wav, vec_FC_E;
		while (getline(MP, line))
		{
			stringstream ss(line);
			double E, wavlen, FC, FC_wav, FC_E;
			ss >> E >> FC;
			FC_E = FC * max_E;
			FC_wav = FC * max_wav;
			wavlen = 1.2398e+3 / E;
			vec_wav.push_back(wavlen);
			vec_E.push_back(E);
			vec_FC_wav.push_back(FC_wav);
			vec_FC_E.push_back(FC_E);
		}
		const int n_data = vec_wav.size();
		double arr_wav[n_data], arr_E[n_data], arr_FC_wav[n_data], arr_FC_E[n_data];
		for (int a = 0; a < n_data; a++)
		{
			arr_wav[a] = vec_wav[a];
			arr_E[a] = vec_E[a];
			arr_FC_wav[a] = vec_FC_wav[a];
			arr_FC_E[a] = vec_FC_E[a];
		}
		TGraph *g_E = new TGraph(n_data, arr_E, arr_FC_E);
		g_E->SetLineColor(kBlack);
		g_E->SetLineWidth(2);
		g_E->SetLineStyle(2);
		TGraph *g_wav = new TGraph(n_data, arr_wav, arr_FC_wav);
		g_wav->SetLineColor(kBlack);
		g_wav->SetLineWidth(2);
		g_wav->SetLineStyle(2);
		TCanvas *c2 = new TCanvas("c2", "c2", 1.2 * 600 * 2, 600);
		c2->Divide(2);
		c2->cd(1);
		gPad->SetMargin(.13, .05, .12, .05);
		const int SC2mat = stoi(map_MatProp["SC2matOpt"]);
		TLegend *leg1 = new TLegend(.43, .6, .9, .6 + .05 * 6);
		leg1->SetBorderSize(0);
		leg1->SetFillStyle(0);
		leg1->SetTextSize(0.045);
		TLegend *leg2 = new TLegend(.17, .6, .6, .6 + .05 * 6);
		leg2->SetBorderSize(0);
		leg2->SetFillStyle(0);
		leg2->SetTextSize(0.045);
		leg1->AddEntry("","Geant4 simulation","h");
		leg1->AddEntry((TObject *)0, "Emission spectra (Wavelength)", "h");
		leg1->AddEntry((TObject *)0, Form("Beam: 5.48 MeV %s", particle, energy), "h");
		leg1->AddEntry((TObject *)0, Form("Scintillator: %s", SC2mat == 0 ? "PS" : "PVT"), "h");
//		leg1->AddEntry((TObject *)0, Form("SY: %.0f / MeV", SY), "h");
		leg1->AddEntry(g_wav, Form("Simulation Input"), "l");
		leg2->AddEntry("","Geant4 simulation","h");
		leg2->AddEntry((TObject *)0, "Emission spectra (Energy)", "h");
		leg2->AddEntry((TObject *)0, Form("Beam: 5.48 MeV %s", particle, energy), "h");
		leg2->AddEntry((TObject *)0, Form("Scintillator: %s", SC2mat == 0 ? "PS" : "PVT"), "h");
//		leg2->AddEntry((TObject *)0, Form("SY: %.0f / MeV", SY), "h");
		leg2->AddEntry(g_E, Form("Simulation Input"), "l");
		for (int a = 0; a < n_cre; a++)
		{
			leg1->AddEntry(H1_OpWav[a], Form("Simulation output", cre_opt[a]), "l");
			H1_OpWav[a]->Draw("hist same");
		}
		g_wav->Draw("l same");
		leg1->Draw();
		c2->cd(2);
		gPad->SetMargin(.13, .05, .12, .05);
		for (int a = 0; a < n_cre; a++)
		{
			leg2->AddEntry(H1_OpE[a], Form("Simulation output", cre_opt[a]), "l");
			H1_OpE[a]->Draw("hist same");
		}
		g_E->Draw("l same");
		leg2->Draw();

		char *pref = "fig/fig_OpEmission";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c2->SaveAs(outname);
	}
	if (DrawOpt[3])
	{
		TH1F *H1_OpTime = (TH1F *)F->Get("H1_OpTime");
		H1_OpTime->SetStats(false);
		H1_OpTime->GetXaxis()->SetTitle("Time [ns]");
		H1_OpTime->GetXaxis()->SetLabelSize(0.045);
		H1_OpTime->GetYaxis()->SetLabelSize(0.045);
		H1_OpTime->GetXaxis()->SetTitleSize(0.05);
		H1_OpTime->SetLineColor(2);
		H1_OpTime->GetXaxis()->SetRangeUser(0.1, 100);
		TH1F *H1_PostOpTime = (TH1F *)F->Get("H1_PostOpTime");
		H1_PostOpTime->SetStats(false);
		H1_PostOpTime->GetXaxis()->SetTitle("Time [ns]");
		H1_PostOpTime->GetXaxis()->SetLabelSize(0.045);
		H1_PostOpTime->GetYaxis()->SetLabelSize(0.045);
		H1_PostOpTime->GetXaxis()->SetTitleSize(0.05);
		H1_PostOpTime->SetLineColor(4);
		TF1 *fit_decay = new TF1("fit_decay", "[0]*exp(-1*x/[1])", 0.5, 15);
		fit_decay->SetParameters(1e6, 1.5);
		fit_decay->SetLineWidth(2);
		fit_decay->SetLineStyle(2);
		fit_decay->SetLineColor(kBlack);
		H1_OpTime->Fit(fit_decay, "R0Q");
		fit_decay->Draw();
		TCanvas *c3 = new TCanvas("c3", "c3", 1.2 * 600, 600);
		gPad->SetMargin(.12, .05, .13, .05);
		gPad->SetLogy();
//				gPad -> SetLogx();
		TLegend *leg = new TLegend(.5, .9 - 0.055 * 6, .9, .9);
		leg->SetBorderSize(0);
		leg->SetFillStyle(0);
		leg->SetTextSize(0.045);
		const int SC2mat = stoi(map_MatProp["SC2matOpt"]);
		leg->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg->AddEntry((TObject *)0, Form("Scintillator: %s", SC2mat == 0 ? "PS" : "PVT"), "h");
		leg->AddEntry((TObject *)0, Form("Decay Time: %.1f ns", 1.5), "h");
		leg->AddEntry(H1_OpTime, "Generation Time", "l");
		leg->AddEntry(fit_decay, Form("fit = N_{0} exp(-x/#tau)"), "l");
		leg->AddEntry(fit_decay, Form("#tau = %.2f (from fit)", fit_decay->GetParameter(1)), "l");
		//		leg -> AddEntry(H1_PostOpTime,"Died Time","l");

		H1_OpTime->Draw("hist same");
		fit_decay->Draw("same");
		//	H1_PostOpTime -> Draw("hist same");
		leg->Draw();

		char *pref = "fig/fig_Time";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c3->SaveAs(outname);
	}
	if (DrawOpt[4])
	{
	}
	if (DrawOpt[5])
	{
		ifstream i_data("data_birks.txt");
		string line;
		bool IsExist = 0;
		vector<string> vec_particle;
		vector<string> vec_energy;
		vector<string> vec_suffix;
		vector<string> vec_dEdx;
		vector<string> vec_dEdx_e;
		vector<string> vec_dLdx;
		vector<string> vec_dLdx_e;
		getline(i_data,line); // dummy line
		while(getline(i_data,line))
		{
			stringstream ss(line);
			string p, E, s, dEdx, dEdx_e, dLdx, dLdx_e;
			ss >> p >> E >> s >> dEdx >> dEdx_e >> dLdx >> dLdx_e;
			if((string)particle == p && (string) energy == E && (string)suffix == s)
				IsExist = 1;
			vec_particle.push_back(p);
			vec_energy.push_back(E);
			vec_suffix.push_back(s);
			vec_dEdx.push_back(dEdx);
			vec_dEdx_e.push_back(dEdx_e);
			vec_dLdx.push_back(dLdx);
			vec_dLdx_e.push_back(dLdx_e);
		}
		cout << "vec_particle.size(): " << vec_particle.size() << endl;
		i_data.close();

		TH2F *H2_Edep_LY = (TH2F *)F->Get("H2_Edep_LY");
		H2_Edep_LY->SetStats(false);
		H2_Edep_LY->GetXaxis()->SetTitle("Edep[MeV]");
		H2_Edep_LY->GetYaxis()->SetTitle("LightYield");
		H2_Edep_LY->GetXaxis()->SetRangeUser(0, 100);
		H2_Edep_LY->GetYaxis()->SetRangeUser(0, 5000);
		TH2F *H2_dEdx_dLdx = (TH2F *)F->Get("H2_dEdx_dLdx");
		H2_dEdx_dLdx->SetStats(false);
		H2_dEdx_dLdx->GetXaxis()->SetTitle("dEdx[MeV/mm]");
		H2_dEdx_dLdx->GetYaxis()->SetTitle("dLdx[mm^{-1}]");
		H2_dEdx_dLdx->GetXaxis()->SetRangeUser(0.1, 200);
		H2_dEdx_dLdx->GetYaxis()->SetRangeUser(0.1, 100000);
		double m_dEdx = H2_dEdx_dLdx -> ProjectionX() -> GetMean();
		double e_dEdx = H2_dEdx_dLdx -> ProjectionX() -> GetStdDev();
		double m_dLdx = H2_dEdx_dLdx -> ProjectionY() -> GetMean();
		double e_dLdx = H2_dEdx_dLdx -> ProjectionY() -> GetStdDev();

		TCanvas *c5 = new TCanvas("c5", "c5", 1.2 * 600 * 2, 600);
		c5->Divide(2);
		c5->cd(1);
		gPad->SetMargin(.12, .05, .13, .05);
		H2_Edep_LY->Draw("colz");
		TLegend *leg1 = new TLegend(.15, .75, .5, .75 + .052 * 2);
		leg1->SetBorderSize(0);
		leg1->SetFillStyle(0);
		leg1->SetTextSize(0.045);
		leg1->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg1->AddEntry((TObject *)0, "Total Edep - Total LY", "h");
		leg1->Draw();
		c5->cd(2);
		gPad->SetMargin(.12, .05, .13, .05);
		gPad -> SetLogx();
		gPad -> SetLogy();
		H2_dEdx_dLdx->Draw("colz");
		
		TLegend *leg2 = new TLegend(.15, .75, .5, .75 + .052 * 2);
		leg2->SetBorderSize(0);
		leg2->SetFillStyle(0);
		leg2->SetTextSize(0.045);
		leg2->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg2->AddEntry((TObject *)0, "dEdx - dLdx", "h");
		leg2->Draw();

		char *pref = "fig/fig_E_L";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c5->SaveAs(outname);

		ofstream o_data("data_birks.txt");
		o_data << "p E suffix dEdx dEdx_e dLdx dLdx_e" << endl;
		for(int a=0; a<vec_particle.size(); a++)
		{
			o_data << vec_particle[a] << " " << vec_energy[a] << " " << vec_suffix[a] 
			<< " " << vec_dEdx[a] << " " << vec_dEdx_e[a] << " " 
			<< vec_dLdx[a] << " " << vec_dLdx_e[a] << endl;
		}
		if(IsExist == 0)
		{
			o_data << particle << " " << energy << " " << suffix << " " << 
			m_dEdx << " " << e_dEdx << " " << m_dLdx << " " << e_dLdx << endl;
		}
		o_data.close();
	}
	if (DrawOpt[6])
	{
		TH1F *H1_OpTrackLength = (TH1F *)F->Get("H1_OpTrackLength");
		H1_OpTrackLength->SetStats(false);
		H1_OpTrackLength->GetXaxis()->SetTitle("TrackLength[mm]");
		H1_OpTrackLength->GetXaxis()->SetTitleSize(0.05);
		H1_OpTrackLength->GetXaxis()->SetRangeUser(0, 1000);
		H1_OpTrackLength->GetYaxis()->SetRangeUser(1, 5e5);
		TCanvas *c6 = new TCanvas("c6", "c6", 1.2 * 600, 600);
		gPad->SetMargin(.13, .05, .12, .05);
		gPad->SetLogy();

		TLegend *leg = new TLegend(.4, .6, .9, .6 + 0.05 * 4);
		leg->SetBorderSize(0);
		leg->SetFillStyle(0);
		leg->SetTextSize(0.045);
		leg->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg->AddEntry((TObject *)0, Form("photon track length distribution"), "h");
		leg->AddEntry((TObject *)0, Form("Attenuation Length: %.1f m", 1.2), "h");
		leg->AddEntry(H1_OpTrackLength, Form("Mean: %.1f", H1_OpTrackLength->GetMean()), "l");
		H1_OpTrackLength->Draw("hist");
		leg->Draw();

		char *pref = "fig/fig_OpTrackLength";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c6->SaveAs(outname);
	}
	double m_LY_L = 0, e_LY_L = 0, m_LY_R = 0, e_LY_R = 0, m_LY_U = 0, e_LY_U = 0, m_LY_D = 0, e_LY_D = 0;
	if (DrawOpt[7])
	{
		const int x_edge = 3000; // for SY1000
														 //		const int x_edge = 700; // for SY1000
														 //		const int x_edge = 100; // for SY100
														 //		const int x_edge = 30; // for SY10
		const int rebin = 5;		 // for SY1000
														 //		const int rebin = 10; // for SY1000
														 //		const int rebin = 5; // for SY100
														 //		const int rebin = 1; // for SY100
		int y_max;
		// 2*H1_NOpSiPM_Le -> GetMaximum(); // for SY100

		// SY10000
		const int fit_x_min = 4000;
		const int fit_x_max = 6000;
		const int fit_xe_min = 0;
		const int fit_xe_max = 2000;
		// SY1000
		//		const int fit_x_min = 400;
		//		const int fit_x_max = 600;
		//		const int fit_xe_min = 0;
		//		const int fit_xe_max = 200;

		// SY100
		//		const int fit_x_min = 30;
		//		const int fit_x_max = 70;
		//		const int fit_xe_min = 0;
		//		const int fit_xe_max = 30;
		// SY10
		//		const int fit_x_min = 0;
		//		const int fit_x_max = 10;
		//		const int fit_xe_min = 0;
		//		const int fit_xe_max = 5;

		bool LR = map_MatProp["SiPMLR"] == "true" ? true : false;
		bool UD = map_MatProp["SiPMUD"] == "true" ? true : false;

		TH1F *H1_NOpSiPM_Le;
		TH1F *H1_NOpSiPM_Re;
		TH2F *H2_NOpSiPMLR;
		TH1F *H1_NOpSiPM_Ue;
		TH1F *H1_NOpSiPM_De;
		TF1 *fLe;
		TF1 *fRe;
		TF1 *fUe;
		TF1 *fDe;
		TH1F* H1_temp;
		if (LR)
		{
			H1_NOpSiPM_Le = (TH1F *)F->Get("H1_NOpSiPM_Le");
			H1_NOpSiPM_Re = (TH1F *)F->Get("H1_NOpSiPM_Re");
			H1_temp = (TH1F*) H1_NOpSiPM_Le -> Clone();
		//	H1_temp -> Add(H1_NOpSiPM_Re);
			cout << H1_temp -> GetMean();
			H2_NOpSiPMLR = (TH2F*) F -> Get("H2_NOpSiPMLR");
			fLe = new TF1("fL", "gaus", fit_xe_min, fit_xe_max);
			fRe = new TF1("fR", "gaus", fit_xe_min, fit_xe_max);
			H1_NOpSiPM_Le->SetStats(false);
			H1_NOpSiPM_Re->SetStats(false);
			H2_NOpSiPMLR ->SetStats(false);
			H1_NOpSiPM_Le->Rebin(rebin);
			H1_NOpSiPM_Re->Rebin(rebin);
			H2_NOpSiPMLR ->Rebin2D(rebin,rebin);
//			y_max = H1_NOpSiPM_Le->GetMaximum();
			y_max = 100;
			H1_NOpSiPM_Le->GetXaxis()->SetTitle("Number of photons");
			H1_NOpSiPM_Le->SetLineStyle(1);
			H1_NOpSiPM_Le->SetLineColor(2);
			H1_NOpSiPM_Le->GetXaxis()->SetTitleSize(0.06);
			H1_NOpSiPM_Le->GetXaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_Le->GetYaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_Le->GetXaxis()->SetRangeUser(0, x_edge);
			H1_NOpSiPM_Le->GetYaxis()->SetRangeUser(0, y_max);
			H1_NOpSiPM_Re->GetXaxis()->SetTitle("Number of photons");
			H1_NOpSiPM_Re->SetLineStyle(1);
			H1_NOpSiPM_Re->SetLineColor(4);
			H1_NOpSiPM_Re->GetXaxis()->SetTitleSize(0.06);
			H1_NOpSiPM_Re->GetXaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_Re->GetYaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_Re->GetXaxis()->SetRangeUser(0, x_edge);
			H1_NOpSiPM_Re->GetYaxis()->SetRangeUser(0, y_max);
			fLe->SetLineColor(2);
			fLe->SetLineStyle(1);
			fLe->SetLineWidth(3);
			H1_NOpSiPM_Le->Fit(fLe, "R0Q");
			fRe->SetLineColor(4);
			fRe->SetLineStyle(1);
			fRe->SetLineWidth(3);
			H1_NOpSiPM_Re->Fit(fRe, "R0Q");

			H2_NOpSiPMLR -> GetXaxis() -> SetRangeUser(0,x_edge);
			H2_NOpSiPMLR -> GetYaxis() -> SetRangeUser(0,x_edge);
			H2_NOpSiPMLR -> GetXaxis() -> SetTitle("Number of photons (Left)");
			H2_NOpSiPMLR -> GetYaxis() -> SetTitle("Number of photons (Right)");
			H2_NOpSiPMLR -> GetXaxis() -> SetTitleSize(0.06);
			H2_NOpSiPMLR -> GetYaxis() -> SetTitleSize(0.06);
			H2_NOpSiPMLR -> GetYaxis() -> SetTitleOffset(1.1);
			H2_NOpSiPMLR -> GetXaxis() -> SetLabelSize(0.045);
			H2_NOpSiPMLR -> GetYaxis() -> SetLabelSize(0.045);
		}
		if (UD)
		{
			H1_NOpSiPM_Ue = (TH1F *)F->Get("H1_NOpSiPM_Ue");
			H1_NOpSiPM_De = (TH1F *)F->Get("H1_NOpSiPM_De");
			fUe = new TF1("fU", "gaus", fit_xe_min, fit_xe_max);
			fDe = new TF1("fD", "gaus", fit_xe_min, fit_xe_max);
			H1_NOpSiPM_Ue->SetStats(false);
			H1_NOpSiPM_De->SetStats(false);
			H1_NOpSiPM_Ue->Rebin(rebin);
			H1_NOpSiPM_De->Rebin(rebin);
			H1_NOpSiPM_Ue->SetLineStyle(1);
			H1_NOpSiPM_Ue->GetXaxis()->SetTitle("# of photons");
			H1_NOpSiPM_Ue->SetLineColor(2);
			H1_NOpSiPM_Ue->GetXaxis()->SetTitleSize(0.05);
			H1_NOpSiPM_Ue->GetXaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_Ue->GetYaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_Ue->GetXaxis()->SetRangeUser(0, x_edge);
			H1_NOpSiPM_Ue->GetYaxis()->SetRangeUser(0, y_max);
			H1_NOpSiPM_De->SetLineStyle(1);
			H1_NOpSiPM_De->GetXaxis()->SetTitle("# of photons");
			H1_NOpSiPM_De->SetLineColor(4);
			H1_NOpSiPM_De->GetXaxis()->SetTitleSize(0.05);
			H1_NOpSiPM_De->GetXaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_De->GetYaxis()->SetLabelSize(0.045);
			H1_NOpSiPM_De->GetXaxis()->SetRangeUser(0, x_edge);
			H1_NOpSiPM_De->GetYaxis()->SetRangeUser(0, y_max);
			TF1 *fUe = new TF1("fLe", "gaus", fit_xe_min, fit_xe_max);
			fUe->SetLineColor(2);
			fUe->SetLineStyle(1);
			fUe->SetLineWidth(3);
			H1_NOpSiPM_Ue->Fit(fUe, "R0Q");
			TF1 *fDe = new TF1("fDe", "gaus", fit_xe_min, fit_xe_max);
			fDe->SetLineColor(4);
			fDe->SetLineStyle(1);
			fDe->SetLineWidth(3);
			H1_NOpSiPM_De->Fit(fDe, "R0Q");
		}

		TLegend *leg = new TLegend(.2, .9 - .055 * (3 + 2 * LR + 2 * UD), .7, .9);
		leg->SetBorderSize(0);
		leg->SetFillStyle(0);
		leg->SetTextSize(0.045);
		leg->AddEntry((TObject *)0, "Geant4 simulation","h");
		leg->AddEntry((TObject *)0, Form("Beam: 5.48 MeV %s",particle), "h");
		leg->AddEntry((TObject *)0, Form("# of photons at MPPC"), "h");
//		leg->AddEntry((TObject *)0, Form("SY: %.1f / MeV", SY), "h");
		if (LR)
		{
			m_LY_L = H1_NOpSiPM_Le->GetMean();
			e_LY_L = H1_NOpSiPM_Le->GetMeanError();
			m_LY_R = H1_NOpSiPM_Re->GetMean();
			e_LY_R = H1_NOpSiPM_Re->GetMeanError();
			//			m_LY_L = fLe->GetParameter(1);
			//			e_LY_L = fLe->GetParameter(2);
			//			m_LY_R = fRe->GetParameter(1);
			//			e_LY_R = fRe->GetParameter(2);
			leg->AddEntry(fLe, Form("Left side (mean: %.1f, StdDev: %.1f)", m_LY_L, e_LY_R), "l");
			leg->AddEntry(fRe, Form("Right side (mean: %.1f, StdDev: %.1f)", m_LY_R, e_LY_R), "l");
		}
		if (UD)
		{
			m_LY_U = fUe->GetParameter(1);
			e_LY_U = fUe->GetParameter(2);
			m_LY_D = fDe->GetParameter(1);
			e_LY_D = fDe->GetParameter(2);
			leg->AddEntry(fUe, Form("Mean_{U}: %.1f, #sigma_{U}: %.1f", m_LY_U, e_LY_U), "l");
			leg->AddEntry(fDe, Form("Mean_{D}: %.1f, #sigma_{D}: %.1f", m_LY_D, e_LY_D), "l");
		}
		TCanvas *c7 = new TCanvas("c7", "c7", 1.2 * 600 * 2, 600);
		c7->Divide(2);
		gPad->SetMargin(.13, .05, .12, .05);
		if (LR)
		{
			c7 -> cd(1);
			gPad -> SetMargin(.13,.05,.12,.05);
//			H1_NOpSiPM_Le->GetXaxis()->SetRangeUser(0, 1500);
//			H1_NOpSiPM_Re->GetXaxis()->SetRangeUser(0, 1500);
			H1_NOpSiPM_Le->Draw("hist same");
			H1_NOpSiPM_Re->Draw("hist same");
			leg->Draw();
			//			fLe -> Draw("same");
			//			fRe -> Draw("same");
			c7 -> cd(2);
			gPad -> SetMargin(.13,.05,.12,.05);
			H2_NOpSiPMLR -> Draw("colz same");
		}
		if (UD)
		{
			H1_NOpSiPM_Ue->Draw("hist same");
			H1_NOpSiPM_De->Draw("hist same");
			fUe->Draw("same");
			fDe->Draw("same");
		}


		char *pref = "fig/fig_NOpSiPM";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c7->SaveAs(outname);
	}

	if (DrawOpt[8])
	{
		const bool LR = map_MatProp["SiPMLR"] == "true" ? true : false;
		const bool UD = map_MatProp["SiPMUD"] == "true" ? true : false;
		// SY1000
		const double fit_x_min = 1.7;
		const double fit_x_max = 2.1;
		const double fit_xe_min = 2;
		const double fit_xe_max = 3.5;
		const double edge_x_min = 1;
		const double edge_x_max = 5;
		const int maxY = 8000;
		TH1F* H1_OpSiPMEnergy_Le;
		TH1F* H1_OpSiPMTime_Le;
		TH1F *H1_OpSiPM_Mean_Time_Le;
		TF1 *fLe;
		TH1F* H1_OpSiPMEnergy_Re;
		TH1F* H1_OpSiPMTime_Re;
		TH1F *H1_OpSiPM_Mean_Time_Re;
		TF1 *fRe;
		TH1F *H1_OpSiPMTime_Ue;
		TF1 *fUe;
		TH1F *H1_OpSiPMTime_De;
		TF1 *fDe;
		if (LR)
		{
			H1_OpSiPMEnergy_Le = (TH1F*) F -> Get("H1_OpSiPMEnergy_Le");
			H1_OpSiPMEnergy_Le -> SetStats(false);
			H1_OpSiPMEnergy_Le -> SetLineColor(kRed);
			H1_OpSiPMEnergy_Le -> SetMarkerColor(kRed);
			H1_OpSiPMEnergy_Le -> SetMarkerSize(1.1);
			H1_OpSiPMEnergy_Le -> SetMarkerStyle(20);
			H1_OpSiPMEnergy_Le -> GetXaxis() -> SetTitle("Energy [eV]");
			H1_OpSiPMEnergy_Le -> GetXaxis() -> SetTitleSize(0.05);
			H1_OpSiPMEnergy_Le -> GetYaxis() -> SetRangeUser(0,5*55000);
			H1_OpSiPMTime_Le = (TH1F*) F -> Get("H1_OpSiPMTime_Le");
			H1_OpSiPMTime_Le -> SetStats(false);
			H1_OpSiPMTime_Le -> SetLineColor(kRed);
			H1_OpSiPMTime_Le -> SetMarkerColor(kRed);
			H1_OpSiPMTime_Le -> SetMarkerSize(0.9);
			H1_OpSiPMTime_Le -> SetMarkerStyle(20);
			H1_OpSiPMTime_Le -> GetXaxis() -> SetRangeUser(0,20);
			H1_OpSiPMTime_Le -> GetYaxis() -> SetRangeUser(0.1,1e5);
			H1_OpSiPMTime_Le -> GetXaxis() -> SetTitle("Time [ns]");
			H1_OpSiPMTime_Le -> GetXaxis() -> SetTitleSize(0.05);
			H1_OpSiPM_Mean_Time_Le = (TH1F*) F -> Get("H1_OpSiPM_Mean_Time_Le");
			H1_OpSiPM_Mean_Time_Le->SetStats(false);
			H1_OpSiPM_Mean_Time_Le->GetXaxis()->SetTitle("Energy weighted time [ns]");
			H1_OpSiPM_Mean_Time_Le->GetXaxis()->SetRangeUser(edge_x_min, edge_x_max);
			H1_OpSiPM_Mean_Time_Le->GetYaxis()->SetRangeUser(0, 2500);
			H1_OpSiPM_Mean_Time_Le->GetXaxis()->SetTitleSize(0.05);
			H1_OpSiPM_Mean_Time_Le->SetLineColor(2);
			fLe = new TF1("fLe", "gaus", fit_xe_min, fit_xe_max);
			fLe->SetLineColor(2);
			fLe->SetLineStyle(2);
			fLe->SetLineWidth(2);
			H1_OpSiPM_Mean_Time_Le->Fit(fLe, "R0Q");
			H1_OpSiPMEnergy_Re = (TH1F*) F -> Get("H1_OpSiPMEnergy_Re");
			H1_OpSiPMEnergy_Re -> SetStats(false);
			H1_OpSiPMEnergy_Re -> SetLineColor(kBlue);
			H1_OpSiPMEnergy_Re -> SetMarkerColor(kBlue);
			H1_OpSiPMEnergy_Re -> SetMarkerSize(1.5);
			H1_OpSiPMEnergy_Re -> SetMarkerStyle(24);
			H1_OpSiPMEnergy_Re -> GetXaxis() -> SetTitle("Energy [eV]");
			H1_OpSiPMEnergy_Re -> GetXaxis() -> SetTitleSize(0.05);
			H1_OpSiPMTime_Re = (TH1F*) F -> Get("H1_OpSiPMTime_Re");
			H1_OpSiPMTime_Re -> SetStats(false);
			H1_OpSiPMTime_Re -> SetLineColor(kBlue);
			H1_OpSiPMTime_Re -> SetMarkerColor(kBlue);
			H1_OpSiPMTime_Re -> SetMarkerSize(1.3);
			H1_OpSiPMTime_Re -> SetMarkerStyle(71);
			H1_OpSiPMTime_Re -> GetXaxis() -> SetRangeUser(0,20);
			H1_OpSiPMTime_Re -> GetXaxis() -> SetTitle("Time [ns]");
			H1_OpSiPMTime_Re -> GetXaxis() -> SetTitleSize(0.05);
			H1_OpSiPM_Mean_Time_Re = (TH1F *)F->Get("H1_OpSiPM_Mean_Time_Re");
			H1_OpSiPM_Mean_Time_Re->SetStats(false);
			H1_OpSiPM_Mean_Time_Re->GetXaxis()->SetTitle("Energy weighted time [ns]");
			H1_OpSiPM_Mean_Time_Re->GetXaxis()->SetRangeUser(edge_x_min, edge_x_max);
			H1_OpSiPM_Mean_Time_Re->GetXaxis()->SetTitleSize(0.05);
			H1_OpSiPM_Mean_Time_Re->SetLineColor(4);
			fRe = new TF1("fRe", "gaus", fit_xe_min, fit_xe_max);
			fRe->SetLineColor(4);
			fRe->SetLineStyle(2);
			fRe->SetLineWidth(2);
			H1_OpSiPM_Mean_Time_Re->Fit(fRe, "R0Q");
			H1_OpSiPMEnergy_Le -> SetFillStyle(3525);
			H1_OpSiPMEnergy_Le -> SetFillColor(kRed-9);
			H1_OpSiPMEnergy_Re -> SetFillStyle(3552);
			H1_OpSiPMEnergy_Re -> SetFillColor(kBlue-9);
		}
		if (UD)
		{
			H1_OpSiPMTime_Ue = (TH1F *)F->Get("H1_OpSiPMTime_Ue");
			H1_OpSiPMTime_Ue->SetStats(false);
			H1_OpSiPMTime_Ue->GetXaxis()->SetTitle("<Time> [ns]");
			H1_OpSiPMTime_Ue->GetXaxis()->SetRangeUser(edge_x_min, edge_x_max);
			H1_OpSiPMTime_Ue->GetYaxis()->SetRangeUser(0, 2000);
			H1_OpSiPMTime_Ue->GetXaxis()->SetTitleSize(0.05);
			H1_OpSiPMTime_Ue->SetLineColor(4);
			fUe = new TF1("fUe", "gaus", fit_xe_min, fit_xe_max);
			fUe->SetLineColor(4);
			fUe->SetLineStyle(2);
			fUe->SetLineWidth(2);
			H1_OpSiPMTime_Ue->Fit(fUe, "R0Q");
			H1_OpSiPMTime_De = (TH1F *)F->Get("H1_OpSiPMTime_De");
			H1_OpSiPMTime_De->SetStats(false);
			H1_OpSiPMTime_De->GetXaxis()->SetTitle("<Time> [ns]");
			H1_OpSiPMTime_De->GetXaxis()->SetRangeUser(edge_x_min, edge_x_max);
			H1_OpSiPMTime_De->GetXaxis()->SetTitleSize(0.05);
			H1_OpSiPMTime_De->SetLineColor(6);
			fDe = new TF1("fDe", "gaus", fit_xe_min, fit_xe_max);
			fDe->SetLineColor(3);
			fDe->SetLineStyle(2);
			fDe->SetLineWidth(2);
			H1_OpSiPMTime_De->Fit(fDe, "R0Q");
		}

		TCanvas *c8_1 = new TCanvas("c8_1", "c8_1", 1.2 * 600*2, 600);
		c8_1 -> Divide(2);
		TLegend* leg1 = new TLegend(.2,.92-0.06*5,.8,.92);
		leg1 -> SetBorderSize(0);
		leg1 -> SetFillStyle(0);
		leg1 -> SetTextSize(0.05);
		leg1 -> AddEntry("",Form("Geant4 simulation",particle, energy),"h");
		leg1 -> AddEntry("",Form("Beam: 5.48 MeV %s",particle),"h");
		leg1 -> AddEntry("","Energy of photon","h");
		leg1 -> AddEntry(H1_OpSiPMEnergy_Le,"Left","f");
		leg1 -> AddEntry(H1_OpSiPMEnergy_Re,"Right","f");
		TLegend* leg2 = new TLegend(.5,.92-0.06*5,.92,.92);
		leg2 -> SetBorderSize(0);
		leg2 -> SetFillStyle(0);
		leg2 -> SetTextSize(0.05);
		leg2 -> AddEntry("","Geant4 simulation","h");
		leg2 -> AddEntry("",Form("Beam: 5.48 MeV %s",particle),"h");
		leg2 -> AddEntry("","Arrival time","h");
		leg2 -> AddEntry(H1_OpSiPMTime_Le,"Left","l");
		leg2 -> AddEntry(H1_OpSiPMTime_Re,"Right","l");
		if (LR)
		{
			c8_1 -> cd(1);
			gPad->SetMargin(.13, .05, .12, .05);
			H1_OpSiPMEnergy_Le -> GetXaxis() -> SetLabelSize(0.045);
			H1_OpSiPMEnergy_Re -> GetXaxis() -> SetLabelSize(0.045);
			H1_OpSiPMEnergy_Le -> GetYaxis() -> SetLabelSize(0.045);
			H1_OpSiPMEnergy_Re -> GetYaxis() -> SetLabelSize(0.045);
			H1_OpSiPMEnergy_Le -> Rebin(5);
			H1_OpSiPMEnergy_Re -> Rebin(5);
//			H1_OpSiPMEnergy_Le -> Rebin(5);
//			H1_OpSiPMEnergy_Re -> Rebin(5);
			H1_OpSiPMEnergy_Le -> Draw(" hist f same");
			H1_OpSiPMEnergy_Re -> Draw(" hist f same");
			leg1 -> Draw("same");

			c8_1 -> cd(2);
			gPad->SetMargin(.13, .05, .12, .05);
			gPad -> SetLogy();
			H1_OpSiPMTime_Le -> GetXaxis() -> SetLabelSize(0.045);
			H1_OpSiPMTime_Re -> GetXaxis() -> SetLabelSize(0.045);
			H1_OpSiPMTime_Le -> GetYaxis() -> SetLabelSize(0.045);
			H1_OpSiPMTime_Re -> GetYaxis() -> SetLabelSize(0.045);
			H1_OpSiPMTime_Le->Draw("hist same");
			H1_OpSiPMTime_Re->Draw("hist same");
			leg2 -> Draw("same");
		}

			H1_OpSiPM_Mean_Time_Le -> SetMarkerColor(kRed);
			H1_OpSiPM_Mean_Time_Le -> SetMarkerStyle(24);
			H1_OpSiPM_Mean_Time_Le -> SetMarkerSize(1.1);
			H1_OpSiPM_Mean_Time_Re -> SetMarkerColor(kBlue);
			H1_OpSiPM_Mean_Time_Re -> SetMarkerStyle(24);
			H1_OpSiPM_Mean_Time_Re -> SetMarkerSize(1.5);
		TCanvas* c8_2 = new TCanvas("c8_2","c8_2",1.2*600,600);
		TLegend *leg3 = new TLegend(.15, .92 - 0.06 * (3 + 2 * LR + 2 * UD), .8, .92);
		leg3->SetBorderSize(0);
		leg3->SetFillStyle(0);
		leg3->SetTextSize(0.05);
		leg3 -> AddEntry("","Geant4 simulation","h");
		leg3->AddEntry((TObject *)0, Form("Beam: 5.48 MeV %s", particle), "h");
		leg3->AddEntry((TObject *)0, "Energy weighted time", "h");
//		leg3->AddEntry((TObject *)0, Form("SY: %.1f / MeV", SY), "h");
		if (LR)
		{
			leg3->AddEntry(H1_OpSiPM_Mean_Time_Le,
										Form("Left (mean:  %.3f, StdDev: %.3f)",
												 H1_OpSiPM_Mean_Time_Le -> GetMean(), H1_OpSiPM_Mean_Time_Le->GetStdDev()),
										"p");
			leg3->AddEntry(H1_OpSiPM_Mean_Time_Re,
										Form("Right (mean: %.3f, StdDev: %.3f)",
												 H1_OpSiPM_Mean_Time_Re->GetMean(), H1_OpSiPM_Mean_Time_Re->GetStdDev()),
										"p");
		}
		if (UD)
		{
			leg3->AddEntry(fUe,
										Form("Mean_{U}: %.3f, #sigma_{L} : %.3f",
												 fUe->GetParameter(1), fUe->GetParameter(2)),
										"l");
			leg3->AddEntry(fDe,
										Form("Mean_{D}: %.3f, #sigma_{R} : %.3f",
												 fDe->GetParameter(1), fDe->GetParameter(2)),
										"l");
		}

			//			fLe -> Draw("same");
			//			fRe -> Draw("same");

		{
			c8_2 -> cd();
			gPad->SetMargin(.13, .05, .12, .05);
			H1_OpSiPM_Mean_Time_Le -> Draw("p hist same");
			H1_OpSiPM_Mean_Time_Re -> Draw("p hist same");
			leg3->Draw("same");

		}
		if (UD)
		{
			H1_OpSiPMTime_Ue->Draw("hist same");
			H1_OpSiPMTime_De->Draw("hist same");
			fUe->Draw("same");
			fDe->Draw("same");
		}

		char *pref1 = "fig/fig_OpSiPMTime1";
		char *outname1 = OutName(name_opt, pref1, particle, energy, suffix, "pdf");
		char *pref2 = "fig/fig_OpSiPMTime2";
		char *outname2 = OutName(name_opt, pref2, particle, energy, suffix, "pdf");
		c8_1->SaveAs(outname1);
		c8_2->SaveAs(outname2);
	}
	double res_LR = 0, err_LR = 0, res_UD = 0, err_UD = 0;
	if (DrawOpt[9])
	{
		const bool LR = map_MatProp["SiPMLR"] == "true" ? true : false;
		const bool UD = map_MatProp["SiPMUD"] == "true" ? true : false;
		// for SY1000
		const double fit_x_min = -0.5;
		const double fit_x_max = 0.5;
		const double edge_x_min = -1;
		const double edge_x_max = 3;
		double y_max;
		// for SY100
		//		const double fit_x_min = -1;
		//		const double fit_x_max = 1;
		// for SY10
		//		const double fit_x_min = -3;
		//		const double fit_x_max = 3;
		//		const double edge_x_min = -5;
		//		const double edge_x_max = 10;
		const int rebin = 5;

		TH1F *H1_SiPMTimeDiff_LR;
		TF1 *fLR;
		double mean_LR;
		if (LR)
		{
			H1_SiPMTimeDiff_LR = (TH1F *)F->Get("H1_SiPMTimeDiff_LR");
			H1_SiPMTimeDiff_LR->Rebin(rebin);
			y_max = H1_SiPMTimeDiff_LR->GetMaximum();
			H1_SiPMTimeDiff_LR->SetStats(false);
			H1_SiPMTimeDiff_LR->SetLineColor(4);
			H1_SiPMTimeDiff_LR->SetLineWidth(2);
			H1_SiPMTimeDiff_LR->GetXaxis()->SetTitle("#Delta t [ns]");
			H1_SiPMTimeDiff_LR->GetXaxis()->SetTitleSize(0.05);
			H1_SiPMTimeDiff_LR->GetXaxis()->SetRangeUser(edge_x_min, edge_x_max);
			H1_SiPMTimeDiff_LR->GetYaxis()->SetRangeUser(0, 1000);
			H1_SiPMTimeDiff_LR->GetYaxis()->SetTitle("Entries");
			H1_SiPMTimeDiff_LR->GetYaxis()->SetTitleSize(0.05);
			fLR = new TF1("fLR", "gaus", fit_x_min, fit_x_max);
			fLR->SetLineColor(4);
			fLR->SetLineStyle(2);
			fLR->SetLineWidth(4);
			H1_SiPMTimeDiff_LR->Fit(fLR, "R0Q");
			mean_LR = H1_SiPMTimeDiff_LR -> GetMean();
			res_LR = 0.5*H1_SiPMTimeDiff_LR -> GetStdDev();
			err_LR = 0.5*H1_SiPMTimeDiff_LR -> GetStdDevError();
//			mean_LR = fLR->GetParameter(1);
//			res_LR = fLR->GetParameter(2);
//			err_LR = fLR->GetParError(2);
		}

		TH1F *H1_SiPMTimeDiff_UD;
		TF1 *fUD;
		double mean_UD;
		if (UD)
		{
			H1_SiPMTimeDiff_UD = (TH1F *)F->Get("H1_SiPMTimeDiff_UD");
			H1_SiPMTimeDiff_UD->Rebin(rebin);
			H1_SiPMTimeDiff_UD->SetStats(false);
			H1_SiPMTimeDiff_UD->SetLineColor(2);
			H1_SiPMTimeDiff_UD->SetLineWidth(2);
			H1_SiPMTimeDiff_UD->GetXaxis()->SetTitle("#Delta t [ns]");
			H1_SiPMTimeDiff_UD->GetXaxis()->SetTitleSize(0.05);
			H1_SiPMTimeDiff_UD->GetXaxis()->SetRangeUser(edge_x_min, edge_x_max);
			H1_SiPMTimeDiff_UD->GetYaxis()->SetRangeUser(0, y_max);
			H1_SiPMTimeDiff_UD->GetYaxis()->SetTitle("Entries");
			H1_SiPMTimeDiff_UD->GetYaxis()->SetTitleSize(0.05);
			fUD = new TF1("fUD", "gaus", fit_x_min, fit_x_max);
			fUD->SetLineColor(2);
			fUD->SetLineStyle(2);
			fUD->SetLineWidth(4);
			H1_SiPMTimeDiff_UD->Fit(fUD, "R0Q");
			mean_UD = fUD->GetParameter(1);
			res_UD = fUD->GetParameter(2);
			err_UD = fUD->GetParError(2);
		}

		TLegend *leg = new TLegend(0.4, 0.9 - 0.055 * (2 + 2 * LR + 2 * UD), 0.9, 0.9);
		leg->SetBorderSize(0);
		leg->SetFillStyle(0);
		leg->SetTextSize(0.045);
		leg -> AddEntry((TObject*)0,"Geant4 simulation","h");
		leg -> AddEntry((TObject*)0,Form("Beam: 5.48 MeV %s",particle),"h");
		if (LR)
			leg->AddEntry(H1_SiPMTimeDiff_LR, Form("#Delta t (Left-Right) distribution"), "l");
		if (UD)
			leg->AddEntry(H1_SiPMTimeDiff_UD, Form("T_{U}-T_{D} distribution"), "l");
		if (LR)
		{
			leg->AddEntry((TObject*)0, Form("Mean: %.3f, StdDev: %.3f", mean_LR, res_LR), "");
		}
		if (UD)
		{
			leg->AddEntry(fUD, Form("Mean_{UD}: %.3f, #sigma_{UD}: %.3f", mean_UD, res_UD), "l");
		}
		//		leg -> AddEntry(f1e,Form("Mean_{e}: %.3f, #sigma_{e}: %.3f",mean_e,sigma_e),"l");
		//		leg -> AddEntry(f1e,Form("sigma_{e}: %.3f #pm %.3f",sigma_e, sigma_ee),"l");

		TCanvas *c9 = new TCanvas("c9", "c9", 1.2 * 600, 600);
		gPad->SetMargin(.13, .05, .12, .05);
		if (LR)
			H1_SiPMTimeDiff_LR->Draw("hist same");
		if (UD)
			H1_SiPMTimeDiff_UD->Draw("hist same");
		//		if(LR) fLR -> Draw("same");
		if (UD)
			fUD->Draw("same");
		//		f1e -> Draw("same");
		leg->Draw();

		char *pref = "fig/fig_SiPMTimeDiff";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c9->SaveAs(outname);
	}
	if (DrawOpt[10])
	{
		TH1D *H1_ratio = (TH1D *)F->Get("H1_ratio");
		H1_ratio->SetStats(false);
		const double tot = H1_ratio->GetBinContent(1);
		cout << tot << endl;
		H1_ratio->Scale(1 / tot);
		H1_ratio->GetYaxis()->SetRangeUser(0, 1.2);
		H1_ratio->GetYaxis()->SetTitle("ratio(x/Gen)");
		H1_ratio->GetYaxis()->SetTitleSize(0.05);
		const char *str_x[] = {"case0", "case2", "case1", "case3", "DUM", "case4"};
		//		const char* str_x[]= {"Generated","Absorbed","Transmitted","SiPM In","Geom","Detected"};
		double ram_x[6];
		for (int x = 0; x < H1_ratio->GetNbinsX(); x++)
			ram_x[x] = H1_ratio->GetBinContent(x + 1);

		H1_ratio->SetBinContent(1, ram_x[0]); // 1, Gen
		H1_ratio->SetBinContent(2, ram_x[2]); // 2, Trans
		H1_ratio->SetBinContent(3, ram_x[3]); // 4, SiPM IN
		H1_ratio->SetBinContent(4, ram_x[5]); // 5, detected
		H1_ratio->GetXaxis()->SetBinLabel(1, str_x[0]);
		H1_ratio->GetXaxis()->SetBinLabel(2, str_x[2]);
		H1_ratio->GetXaxis()->SetBinLabel(3, str_x[3]);
		H1_ratio->GetXaxis()->SetBinLabel(4, str_x[5]);
		H1_ratio->GetXaxis()->SetRangeUser(0, 4);
		H1_ratio->GetXaxis()->SetLabelSize(0.05);

		//		for(int x=0; x<H1_ratio->GetNbinsX(); x++)
		//		{
		//			H1_ratio -> GetXaxis() -> SetBinLabel(x+1,str_x[x]);
		//			H1_ratio -> GetXaxis() -> SetLabelSize(0.05);
		//		}

		TLegend *leg = new TLegend(.53, .93 - 0.055 * 8, .9, .93);
		leg->SetBorderSize(0);
		leg->SetFillStyle(0);
		leg->SetTextSize(0.045);
		leg->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg->AddEntry((TObject *)0, Form("# of photon ratio"), "h");
		leg->AddEntry((TObject *)0, Form("SY = %.1f / MeV", SY), "h");
		leg->AddEntry((TObject *)0, "  case0: Generated", "h");
		leg->AddEntry((TObject *)0, "  case1: Transmitted", "h");
		leg->AddEntry((TObject *)0, "  case2: Absorbed", "h");
		leg->AddEntry((TObject *)0, "  case3: SiPM In", "h");
		leg->AddEntry((TObject *)0, "  case4: Detected", "h");

		TCanvas *c10 = new TCanvas("c10", "c10", 1.2 * 600, 600);
		gPad->SetMargin(.13, .05, .12, .05);
		H1_ratio->Draw("hist text0");
		leg->Draw();

		char *pref = "fig/fig_ratio";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c10->SaveAs(outname);
	}
	if (DrawOpt[11])
	{
		TH2F *H2_dLY_TimeRes = (TH2F *)F->Get("H2_dLY_TimeRes");
		H2_dLY_TimeRes -> Rebin2D(2,2);
		H2_dLY_TimeRes->SetStats(false);
		double x_mean = H2_dLY_TimeRes->ProjectionX()->GetMean();
		double x_dev = H2_dLY_TimeRes->ProjectionX()->GetStdDev();
		double x_min = x_mean - 2.5 * x_dev;
		double x_max = x_mean + 2.5 * x_dev;
		double gap = (x_max - x_min) / 9;
		double x[9], y[9];
		TLine *fLine[9];
		for (int a = 0; a < 9; a++)
		{
			x[a] = x_min + a * gap;
			H2_dLY_TimeRes->GetXaxis()->SetRangeUser(x_min + a * gap, x_min + (a + 1) * gap);
			y[a] = 0.5 * H2_dLY_TimeRes->ProjectionY()->GetStdDev();
			fLine[a] = new TLine(x_min + a * gap, -1, x_min + a * gap, 1);
			fLine[a]->SetLineStyle(2);
			fLine[a]->SetLineColor(kRed);
			fLine[a]->SetLineWidth(1.3);
		}
		TGraphErrors *g = new TGraphErrors(9, x, y, 0, 0);
		//		g -> SetName(Form("g_dLY_TimeRes_%s_%s_%s",particle,energy,suffix));
		g->SetMarkerStyle(20);
		g->SetMarkerSize(1.3);
		g->SetMarkerColor(1);

		TCanvas *c11 = new TCanvas("c11", "c11", 1.2 * 600 * 2, 600);
		c11->Divide(2);
		c11->cd(1);
		gPad->SetMargin(.13, .05, .12, .05);
		TLegend *leg1 = new TLegend(.5, .9 - 0.05 * 3, .9, .9);
		leg1->SetFillStyle(0);
		leg1->SetBorderSize(0);
		leg1->SetTextSize(0.045);
		leg1->AddEntry((TObject *)0, Form("%s %s simulation", particle, energy), "h");
		leg1->AddEntry((TObject *)0, Form("SY = %.1f / MeV", SY), "h");
		leg1->AddEntry(fLine[1], Form("LY split line"), "l");

		H2_dLY_TimeRes -> GetXaxis() -> SetRangeUser(0.7*x_min,1.2*x_max);
		H2_dLY_TimeRes -> GetYaxis() -> SetRangeUser(-1.5,1.8);
		H2_dLY_TimeRes->GetXaxis()->SetTitle("#LTNOp_{SiPM}#GT");
		H2_dLY_TimeRes->GetXaxis()->SetTitleSize(0.05);
		H2_dLY_TimeRes->GetYaxis()->SetTitle("Time difference [ns]");
		H2_dLY_TimeRes->GetYaxis()->SetTitleSize(0.05);
		H2_dLY_TimeRes->Draw("colz");
		leg1->Draw();
		for (int a = 0; a < 9; a++)
			fLine[a]->Draw("l same");

		c11->cd(2);
		gPad->SetMargin(.13, .05, .12, .05);
		TH1D *htemp = (TH1D *)gPad->DrawFrame(0.5*x_min, 0.001, 1.5*x_max,.2);
		htemp->GetXaxis()->SetTitle("#LTNOp_{SiPM}#GT");
		htemp->GetXaxis()->SetTitleSize(0.05);
		htemp->GetYaxis()->SetTitle("#sigma(t_{L}-t_{R})/2");
		htemp->GetYaxis()->SetTitleSize(0.05);
		TLegend *leg2 = new TLegend(.5, .9 - 0.05 * 3, .9, .9);
		leg2->SetFillStyle(0);
		leg2->SetBorderSize(0);
		leg2->SetTextSize(0.045);
		leg2 -> AddEntry("",Form("%s %s simulation",particle,energy),"h");
		leg2 -> AddEntry("",Form("SY = %.1f / MeV",SY),"h");
		leg2 -> AddEntry(g,Form("Splitted point"),"p");
		leg2 -> Draw();
		g->Draw("p");

		char *pref = "fig/fig_dLY_TimeRes";
		char *outname = OutName(name_opt, pref, particle, energy, suffix, "pdf");
		c11->SaveAs(outname);
	}
	if (text_output_LY_res && DrawOpt[7] && DrawOpt[9])
	{
		ifstream data("Resolution_data.txt");
		string line;
		bool IsExist = 0;
		vector<string> vec_particle;
		vec_particle.push_back("p");
		vector<string> vec_energy;
		vec_energy.push_back("E");
		vector<string> vec_SY;
		vec_SY.push_back("SY");
		vector<string> vec_suffix;
		vec_suffix.push_back("suffix");
		vector<string> vec_opt;
		vec_opt.push_back("LRUD(10*LR+UD)");														// LRUD
		vector<tuple<string, string, string, string>> vec_LY, vec_LY_e; // L,R,U,D
		vec_LY.push_back(make_tuple("LY_L", "LY_R", "LY_U", "LY_D"));
		vec_LY_e.push_back(make_tuple("LY_L_e", "LY_R_e", "LY_U_e", "LY_D_e"));
		vector<string> vec_resLR, vec_resLR_e;
		vec_resLR.push_back("resLR[ps]");
		vec_resLR_e.push_back("resLR_e");
		vector<string> vec_resUD, vec_resUD_e;
		vec_resUD.push_back("resUD[ps]");
		vec_resUD_e.push_back("resUD_e");
		getline(data, line); // dummy line
		while (getline(data, line))
		{
			stringstream ss(line);
			string p, e, sy, s, o;
			string LY_L, LY_R, LY_U, LY_D, LY_L_e, LY_R_e, LY_U_e, LY_D_e, rLR, rLR_e, rUD, rUD_e;
			ss >> p >> e >> sy >> s >> o >>
					LY_L >> LY_L_e >> LY_R >> LY_R_e >> LY_U >> LY_U_e >> LY_D >> LY_D_e >>
					rLR >> rLR_e >> rUD >> rUD_e;
			vec_particle.push_back(p);
			vec_energy.push_back(e);
			vec_SY.push_back(sy);
			vec_suffix.push_back(s);
			cout << s << endl;
			vec_opt.push_back(o);
			vec_LY.push_back(make_tuple(LY_L, LY_R, LY_U, LY_D));
			vec_LY_e.push_back(make_tuple(LY_L_e, LY_R_e, LY_U_e, LY_D_e));
			vec_resLR.push_back(rLR);
			vec_resLR_e.push_back(rLR_e);
			vec_resUD.push_back(rUD);
			vec_resUD_e.push_back(rUD_e);
			if ((string)particle == p && (string)energy == e &&
					(string)suffix == s && to_string((int)SY) == sy)
			{
				IsExist = 1;
			}
		}
		data.close();
		bool LR = map_MatProp["SiPMLR"] == "true" ? 1 : 0;
		bool UD = map_MatProp["SiPMUD"] == "true" ? 1 : 0;
		if (IsExist==0)
		{
			ofstream data("Resolution_data.txt");
			const int n_size = vec_particle.size();
			for (int a = 0; a < n_size; a++)
			{
				data << vec_particle[a] << " " << vec_energy[a] << " " << vec_SY[a] << " " << // beam info
						vec_suffix[a] << " " << vec_opt[a] << " " <<															// file info
						Form("%s %s %s %s ",
								 get<0>(vec_LY[a]).c_str(), get<0>(vec_LY_e[a]).c_str(),
								 get<1>(vec_LY[a]).c_str(), get<1>(vec_LY_e[a]).c_str())
						 << Form("%s %s %s %s ",
										 get<2>(vec_LY[a]).c_str(), get<2>(vec_LY_e[a]).c_str(),
										 get<3>(vec_LY[a]).c_str(), get<3>(vec_LY_e[a]).c_str())
						 << Form("%s %s %s %s",
										 vec_resLR[a].c_str(), vec_resLR_e[a].c_str(),
										 vec_resUD[a].c_str(), vec_resUD_e[a].c_str())
						 << endl;
			}
			data << particle << " " << energy << " " << SY << " " << suffix << " " << Form("%02d ", 10 * LR + UD) << Form("%.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f ", m_LY_L, e_LY_L, m_LY_R, e_LY_R, m_LY_U, e_LY_U, m_LY_D, e_LY_D) << Form("%.3f %.3f %.3f %.3f", 1e3*res_LR, 1e3*err_LR, res_UD, err_UD) << endl;
			data.close();
		}
	}
}
