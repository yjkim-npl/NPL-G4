void BirksDraw(const char* infile = "data_birks.txt")
{
  ifstream in(infile);
  string line;
  getline(in,line); // for dummy line
  TGraph* g_tot = new TGraph();
  TGraph* g_alp = new TGraph();
  TGraph* g_pro = new TGraph();
  g_pro -> SetMarkerSize(1.5);
  g_pro -> SetMarkerStyle(71);
  g_pro -> SetMarkerColor(kBlack);

  g_alp -> SetMarkerSize(1.5);
  g_alp -> SetMarkerStyle(71);
  g_alp -> SetMarkerColor(kRed);
  int n = 0;
  int n_alp = 0;
  int n_pro = 0;
  while(getline(in,line))
  {
    stringstream ss(line);
    string p, E, s;
    double dEdx, dEdx_e, dLdx, dLdx_e;
    ss >> p >> E >> s >> dEdx >> dEdx_e >> dLdx >> dLdx_e;
    TString str_E = E;
    TString str_p = p;
    if(str_E.Contains("50")) continue;
    if(str_E.Contains("200")) continue;
    if(str_E.Contains("300")) continue;
    g_tot -> SetPoint(n,dEdx,dLdx);
    if(str_p.Contains("alpha"))
    {
      g_alp -> SetPoint(n_alp,dEdx,dLdx);
      n_alp++;
    }
    if(str_p.Contains("proton"))
    {
      g_pro -> SetPoint(n_pro,dEdx,dLdx);
      n_pro++;
    }
    n++;
  } 

  TF1* fit = new TF1("fit","[0]*x/(1+[1]*x)",0.5,200);
  fit -> SetLineColor(kRed);
  fit -> SetLineStyle(2);
  fit -> SetLineWidth(2);
  g_tot -> Fit(fit,"R0Q");

  TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
  gPad -> SetMargin(.13,.05,.12,.05);
  gPad -> SetLogx();
  gPad -> SetLogy();
  gPad -> SetTicks();
  TH1D* htemp = (TH1D*) gPad -> DrawFrame(.1,1e3,1e3,5e6);
  htemp -> GetXaxis() -> SetTitle("Energy deposit/path length [MeV/mm]");
  htemp -> GetXaxis() -> SetTitleSize(0.05);
  htemp -> GetYaxis() -> SetTitle("Light Yield/path length [mm^{-1}]");
  htemp -> GetYaxis() -> SetTitleSize(0.05);
  fit -> Draw("same");
  g_alp -> Draw("p same");
  g_pro -> Draw("p same");

  TLegend* leg = new TLegend(.2,.92-0.062*5,.5,.92);
  leg -> SetTextSize(0.05);
  leg -> SetBorderSize(0);
  leg -> SetFillStyle(0);
  leg -> AddEntry("","proton & alpha simulation","h");
  leg -> AddEntry(g_pro,"proton","p");
  leg -> AddEntry(g_alp,"alpha","p");
  leg -> AddEntry(fit,Form("fit = #LTScintillationYield#GT #frac{x}{1+%.3fx}",fit->GetParameter(1)),"l");
	leg -> AddEntry("",Form("Birks' constant = %.3f #pm %.3f",fit -> GetParameter(1), fit -> GetParError(1)),"l");
  leg -> Draw();

  c1 -> SaveAs("fig/fig_Birks.pdf");

}
