void SY_PT_Draw()
{
	ifstream in("SY_PT.dat");
	string line;
	getline(in,line); // dummy line
	vector<string> vec_pID;
	vector<int> vec_E;
	vector<int> vec_LY;
	while(getline(in,line))
	{
		stringstream ss(line);
		string name;
		int E,LY;
		int DUM1, DUM2;
		double DUM3;
		ss >> name >> E >> DUM1 >> DUM2 >> DUM3 >> LY;
		vec_pID.push_back(name);
		vec_E.push_back(E);
		vec_LY.push_back(LY);
	}
	in.close();
	const int n_size = vec_pID.size();
	double x_ele[3], y_ele[3];
	double x_pro[3], y_pro[3];
	double x_alp[3], y_alp[3];
	for(int a=0; a<n_size; a++)
	{
		int idx = 0;
		TString name = vec_pID[a];
		if(name.Contains("e-")){
			idx = a;
			x_ele[a] = vec_E[a];
			y_ele[a] = vec_LY[a];
		}else if(name.Contains("proton")){
			idx = a-3;
			x_pro[idx] = vec_E[a];
			y_pro[idx] = vec_LY[a];
		}else if(name.Contains("alpha")){
			idx = a-6;
			x_alp[idx] = vec_E[a];
			y_alp[idx] = vec_LY[a];
		}else if (a>9){
			continue;
		}
			cout << idx << " " <<  vec_pID[a] << " " << vec_E[a] << " " <<  vec_LY[a] << endl;
	}
	double x_ele_ref[3], y_ele_ref[3];
	double x_pro_ref[3], y_pro_ref[3];
	double x_alp_ref[3], y_alp_ref[3];
	{
		x_ele_ref[0] = 1;
		y_ele_ref[0] = 9788;
		x_ele_ref[1] = 5;
		y_ele_ref[1] = 48911;
		x_ele_ref[2] = 10;
		y_ele_ref[2] = 96630;

		x_pro_ref[0] = 1;
		y_pro_ref[0] = 1505;
		x_pro_ref[1] = 5;
		y_pro_ref[1] = 17350;
		x_pro_ref[2] = 10;
		y_pro_ref[2] = 43394;

		x_alp_ref[0] = 1;
		y_alp_ref[0] = 202;
		x_alp_ref[1] = 5;
		y_alp_ref[1] = 2666;
		x_alp_ref[2] = 10;
		y_alp_ref[2] = 12436;
	}
	TGraph* g_ele = new TGraph(3,x_ele,y_ele);
	g_ele -> SetMarkerSize(1);
	g_ele -> SetMarkerStyle(24);
	g_ele -> SetMarkerColor(kRed);
	TGraph* g_pro = new TGraph(3,x_pro,y_pro);
	g_pro -> SetMarkerSize(1);
	g_pro -> SetMarkerStyle(24);
	g_pro -> SetMarkerColor(kGreen);
	TGraph* g_alp = new TGraph(3,x_alp,y_alp);
	g_alp -> SetMarkerSize(1);
	g_alp -> SetMarkerStyle(24);
	g_alp -> SetMarkerColor(kBlue);
	TGraph* g_ele_ref = new TGraph(3,x_ele_ref,y_ele_ref);
	g_ele_ref -> SetMarkerSize(2);
	g_ele_ref -> SetMarkerStyle(20);
	g_ele_ref -> SetMarkerColor(kRed-6);
	TGraph* g_pro_ref = new TGraph(3,x_pro_ref,y_pro_ref);
	g_pro_ref -> SetMarkerSize(2);
	g_pro_ref -> SetMarkerStyle(20);
	g_pro_ref -> SetMarkerColor(kGreen-6);
	TGraph* g_alp_ref = new TGraph(3,x_alp_ref,y_alp_ref);
	g_alp_ref -> SetMarkerSize(2);
	g_alp_ref -> SetMarkerStyle(20);
	g_alp_ref -> SetMarkerColor(kBlue-6);

	TLegend* leg = new TLegend(.2,.9-0.05*5,.5,.9);
	leg -> SetBorderSize(0);
	leg -> SetFillStyle(0);
	leg -> SetTextSize(0.04);
	leg -> AddEntry((TObject*)0,"Response check of EJ-230 Scintillator","h");
	leg -> AddEntry(g_ele_ref,"reference","p");
	leg -> AddEntry(g_ele,"electron","p");
	leg -> AddEntry(g_pro,"proton","p");
	leg -> AddEntry(g_alp,"alpha","p");

	TCanvas* c1 = new TCanvas("c1","c1",1.2*600,600);
	gPad -> SetMargin(.13,.05,.12,.05);
	TH1D* htemp = (TH1D*)gPad -> DrawFrame(0.1,1e1,1e2,1e6);
	htemp -> GetXaxis() -> SetTitle("Energy [MeV]");
	htemp -> GetXaxis() -> SetTitleSize(0.05);
	htemp -> GetYaxis() -> SetTitle("Number of Scintillation photons");
	htemp -> GetYaxis() -> SetTitleSize(0.05);
	htemp -> GetYaxis() -> SetTitleOffset(1);
	gPad -> SetLogx();
	gPad -> SetLogy();
	g_ele_ref -> Draw("p same");
	g_pro_ref -> Draw("p same");
	g_alp_ref -> Draw("p same");
	g_ele -> Draw("p same");
	g_pro -> Draw("p same");
	g_alp -> Draw("p same");
	leg -> Draw();

	c1 -> SaveAs("SY_PT_confirmation.pdf");
}
