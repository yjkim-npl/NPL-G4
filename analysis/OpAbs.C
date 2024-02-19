void OpAbs(const char* infile = "ABS.root")
{
	TFile* F = new TFile(infile,"read");
	TH1F* H1 = (TH1F*) F -> Get("htemp");
	H1 -> SetTitle("");
	H1 -> SetStats(false);

	H1 -> GetXaxis() -> SetTitle("TrackLength[mm]");
	H1 -> GetXaxis() -> SetTitleSize(0.045);
	H1 -> GetYaxis() -> SetTitle("Entries");
	H1 -> GetYaxis() -> SetTitleSize(0.045);
	H1 -> GetXaxis() -> SetRangeUser(0,7000);
	H1 -> GetYaxis() -> SetRangeUser(1e3,1e7);

	TF1* fit = new TF1("fit","[0]*exp(-x/[1])",1000,3000);
	fit -> SetParameters(1e5,1500);
	fit -> SetLineColor(kRed);
	fit -> SetLineWidth(2);
	fit -> SetLineStyle(2);
	H1 -> Fit(fit,"R0");


	TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
	gPad -> SetMargin(.13,.05,.12,.05);
	gPad -> SetLogy();
	H1 -> Draw("hist");
	fit -> Draw("same");
	TLegend* leg = new TLegend(.3,.93-0.05*6,.9,.93);
	leg -> SetBorderSize(0);
	leg -> SetFillStyle(0);
	leg -> SetTextSize(0.045);
	leg -> AddEntry("","alpha 5.48MeV simulation","h");
	leg -> AddEntry("","Scintillator = PVT","h");
	leg -> AddEntry("","Absorption length = 1.2m","h");
	leg -> AddEntry(H1, "Path length of absorbed photon","l");
	leg -> AddEntry(fit,Form("L(x) = L(0) exp(-x/#varepsilon)"),"l");
	leg -> AddEntry("", Form("#varepsilon[m] = %.3f #pm %.3f",1/1e3*fit -> GetParameter(1), 1/1e3*fit -> GetParError(1)),"");

	leg -> Draw();
	c1 -> SaveAs("SC_att_length.pdf");
}
