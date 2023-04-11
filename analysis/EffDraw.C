void EffDraw()
{
	TFile* F = new TFile("out_root/out_Eff.root","read");
	TH1F* H1_Eff = (TH1F*) F -> Get("H1_Eff");

	TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
	H1_Eff -> Scale(1e-3);
	H1_Eff -> SetStats(false);
	H1_Eff -> GetYaxis() -> SetRangeUser(0,0.65);
	H1_Eff -> GetXaxis() -> SetTitle("wavlen[nm]");
	H1_Eff -> GetYaxis() -> SetTitle("Detection efficiency[%]");

	TLegend* leg = new TLegend(0.4,0.70,0.9,0.70+0.045*2);
	leg -> SetFillStyle(0);
	leg -> SetBorderSize(0);
	leg -> SetTextSize(0.04);
	leg -> AddEntry((TObject*)0,"Optical photon simulation","h");
	leg -> AddEntry((TObject*)0,"Beam: 300 ~ 900 nm with 1nm step","h");
//	leg -> AddEntry((TObject*)0,)

	H1_Eff -> Draw("hist");
	leg -> Draw();
	c1 -> SaveAs("fig/fig_Efficiency.pdf");
}
