void OpBoundaryDraw()
{
	const char* infile = "out_root/H_Op_out_alpha_5.48MeV_Boundary_v3.root";
	TFile* F = new TFile(infile,"read");
	char* str_proc[] = {"Gen","Total_reflection","Transmission","Fresnel_reflection"};
	char* str_axis[] = {"px-py","py-pz","px-pz"};
	TH2F* H2_p[3][4];
	for(int b=0; b<4; b++)
	{
		H2_p[0][b] = (TH2F*) F -> Get(Form("H2_%s_%s",str_proc[b],str_axis[0]));
		H2_p[1][b] = (TH2F*) F -> Get(Form("H2_%s_%s",str_proc[b],str_axis[1]));
		H2_p[2][b] = (TH2F*) F -> Get(Form("H2_%s_%s",str_proc[b],str_axis[2]));
	}


	TCanvas* c1 = new TCanvas("c1","c1",600*4,600*3);
	c1 -> Divide(4,3);
	for(int a=0; a<4; a++)
	for(int b=0; b<3; b++)
	{
		c1 -> cd(a + 4*b + 1);
		gPad -> SetMargin(.13,.05,.12,.05);
		H2_p[b][a] -> SetStats(false);
		H2_p[b][a] -> GetYaxis() -> SetRangeUser(-5,7);
		TLegend* leg = new TLegend(.5,.93-0.07*(b==0?2:1),.9,.93-0.07*(b==0?0:1));
		leg -> SetBorderSize(0);
		leg -> SetFillStyle(0);
		leg -> SetTextSize(0.06);
		if(b==0)
		{
			if(a==0)
				leg -> AddEntry("","Generated photon","h");
			else if (a==1)
				leg -> AddEntry("","Absorbed photon","h");
			else if (a==2)
				leg -> AddEntry("","Transmitted photon","h");
			else if (a==3)
				leg -> AddEntry("","Fresnel reflection","h");
		}
		leg -> AddEntry("",Form("Entry: %d",(int)H2_p[b][a] -> GetEntries()),"h");
		if(b==0){
			H2_p[b][a] -> GetXaxis() -> SetTitle("px[eV]");
			H2_p[b][a] -> GetYaxis() -> SetTitle("py[eV]");
			H2_p[b][a] -> GetXaxis() -> SetTitleSize(0.05);
			H2_p[b][a] -> GetYaxis() -> SetTitleSize(0.05);
		}else if (b==1){
			H2_p[b][a] -> GetXaxis() -> SetTitle("py[eV]");
			H2_p[b][a] -> GetYaxis() -> SetTitle("pz[eV]");
			H2_p[b][a] -> GetXaxis() -> SetTitleSize(0.05);
			H2_p[b][a] -> GetYaxis() -> SetTitleSize(0.05);
		}	else {
			H2_p[b][a] -> GetXaxis() -> SetTitle("px[eV]");
			H2_p[b][a] -> GetYaxis() -> SetTitle("pz[eV]");
			H2_p[b][a] -> GetXaxis() -> SetTitleSize(0.05);
			H2_p[b][a] -> GetYaxis() -> SetTitleSize(0.05);
		}
		H2_p[b][a] -> Draw("colz");
		leg -> Draw();
	}
	c1 -> SaveAs("fig_Boundary.pdf");
}
