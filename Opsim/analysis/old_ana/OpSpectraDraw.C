void OpSpectraDraw
(
 char* particle = "proton",
 char* energy = "100MeV",
 char* suffix = ""
 )
{
	char* in_pref = "out_root/H_Op_out";
	char* infile = Form("%s_%s_%s.root",in_pref,particle,energy);
	cout << "#############" << endl;
	cout << "infile: " << infile << endl;
	cout << "#############" << endl;

	TFile* F = new TFile(infile,"read");

	TList* L_MatProp = (TList*) F -> Get("ConstMaterialProperties");
	map<string,string> map_MatProp;
	for(int a=0; a< L_MatProp->GetSize(); a++)
	{
		TNamed* name = (TNamed*) L_MatProp -> At(a);
		map_MatProp.insert(make_pair(name->GetName(),name->GetTitle()));
	}
	enum {Total,Scint,Cheren};
	const char* cre_opt[] = {"Total","Scint","Cheren"};
	const int n_opt = sizeof(cre_opt)/sizeof(cre_opt[0]);

	TH1F* H1_OpWav[n_opt];
	TH1F* H1_OpE[n_opt];

	for(int a=0; a<n_opt; a++)
	{
		H1_OpWav[a] = (TH1F*)F -> Get(Form("H1_OpWav_%s",cre_opt[a]));
		H1_OpE[a] = (TH1F*) F -> Get(Form("H1_OpE_%s",cre_opt[a]));
//		H1_OpWav[a] -> SetStats(false);
		H1_OpWav[a] -> GetXaxis() -> SetTitle("#lambda [nm]");
		H1_OpWav[a] -> GetXaxis() -> SetTitleSize(0.04);
//		H1_OpWav[a] -> SetLineColor(a==3?46:a>=2?a+2:a+1);
//		H1_OpE[a] -> SetStats(false);
//		H1_OpE[a] -> GetXaxis() -> SetTitle("Energy[eV]");
//		H1_OpE[a] -> SetLineColor(a==3?46:a>=2?a+2:a+1);
//	}
	TCanvas* c3 = new TCanvas("c3","c3",1.2*600*2,600);
	c3 -> Divide(2);
//	const int SC2mat = stoi(map_MatProp["SC2matOpt"]);
	const int SC2mat = 1;
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
//	leg2 -> AddEntry((TObject*)0,Form("Scintillator: %s",SC2mat==0?"PS":"PVT"),"h");
//	for(int a=0; a<n_opt-1; a++)
//	{
//		leg1 -> AddEntry(H1_OpWav[a],Form("%s",cre_opt[a]),"l");
//		leg2 -> AddEntry(H1_OpE[a],Form("%s",cre_opt[a]),"l");
		c3 -> cd(1);
		H1_OpWav[Total] -> Draw("hist same");
//		H1_OpWav[Scint] -> Draw("hist same");
//		H1_OpWav[Cheren] -> Draw("hist same");
		leg1 -> Draw();
		c3 -> cd(2);
		H1_OpE[Total] -> Draw("hist same");
//		H1_OpE[Scint] -> Draw("hist same");
//		H1_OpE[Cheren] -> Draw("hist same");
		leg2 -> Draw();
	}

}
