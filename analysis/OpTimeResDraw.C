void OpTimeResDraw()
{
	ifstream in("Resolution_data.txt");
	string line;

	int n_data = 0;
	// data container
	string p,E,sy,s;
	int LRUD;
	double L,Le,R,Re,U,Ue,D,De,resLR,resLRe,resUD,resUDe;
	vector<string> vec_p, vec_E, vec_SY, vec_suf;
	vector<int> vec_LRUD;
	vector<double> v_L,v_L_e,v_R,v_R_e,v_U,v_U_e,v_D,v_D_e;
	vector<double> res_LR, res_LR_e,res_UD,res_UD_e;

	// container for graph
	getline(in,line); // dummy line
	while(getline(in,line))
	{
		n_data++;
		stringstream ss(line);
		ss >> p >> E >> sy >> s >>  LRUD
			>> L >> Le >> R >> Re >> U >> Ue >> D >> De
			>> resLR >> resLRe >> resUD >> resUDe;
		vec_p.push_back(p);
		vec_E.push_back(E);
		vec_SY.push_back(sy);
		vec_suf.push_back(s);
		vec_LRUD.push_back(LRUD);
		v_L.push_back(L);
		v_R.push_back(R);
		v_U.push_back(U);
		v_D.push_back(D);
		v_L_e.push_back(Le);
		v_R_e.push_back(Re);
		v_U_e.push_back(Ue);
		v_D_e.push_back(De);
		res_LR.push_back(resLR);
		res_LR_e.push_back(resLRe);
		res_UD.push_back(resUD);
		res_UD_e.push_back(resUDe);
		cout << E << " " << sy << " " << s << " "  << endl;
	}
	in.close();

	double x_min = 1e4;
	double x_max = 1e1;
	double y_min = 1e4;
	double y_max = 1e-1;
//	vector<double> vec_LY, vec_LY_e, vec_res, vec_res_e;
	double vec_LY[n_data], vec_LY_e[n_data], vec_res[n_data], vec_res_e[n_data];
	for(int a=0; a<n_data; a++)
	{
		LRUD = vec_LRUD[a];
		double x,y,xe,ye;
		cout << vec_LRUD[a] << endl;
		if(LRUD == 10){ // LR
			x = 0.5*v_L[a] + 0.5*v_R[a];
			cout << v_L[a] <<" " << v_R[a] << endl;
			xe = sqrt(pow(v_L_e[a],2) + pow(v_R_e[a],2));
			y = 0.5*res_LR[a];
			ye = 0.5*res_LR_e[a];
		} else if (LRUD == 11){ // LRUD
			x = 0.25*v_L[a] + 0.25*v_R[a] + 0.25*v_U[a] + 0.25*v_D[a];
			cout << v_L[a] <<" " << v_R[a] << endl;
			xe = sqrt(pow(v_L_e[a],2)+pow(v_R_e[a],2)+pow(v_U_e[a],2)+pow(v_D_e[a],2));
			y = sqrt(pow(0.5*res_LR[a],2)+pow(0.5*res_UD[a],2));
			ye = sqrt(pow(0.5*res_LR_e[a],2)+pow(0.5*res_UD_e[a],2));
		}
		if(x_min > x)
			x_min = x;
		if(x_max < x)
			x_max = x;
		if(y_min > y)
			y_min = y;
		if(y_max < y)
			y_max = y;
//		vec_LY.push_back(x);
//		vec_LY_e.push_back(xe);
//		vec_res.push_back(y);
//		vec_res_e.push_back(ye);
		vec_LY[a] = x;
		vec_LY_e[a] = xe;
		vec_res[a] = y;
		vec_res_e[a] = ye;
		cout << "x: " << x << " y: " << y << endl;
	}

	TCanvas* c_TimeRes = new TCanvas("c_TimeRes","c_TimeRes",1.2*600,600);
	gPad -> SetMargin(.13,.05,.12,.05);
	gPad -> SetLogx();
	TH1D* htemp = (TH1D*) gPad -> DrawFrame(1e-1*x_min,1e-1*y_min,1e1*x_max,1e1*y_max);
	htemp -> GetXaxis() -> SetTitle("#LTLY_{SiPM}#GT");
	htemp -> GetXaxis() -> SetTitleSize(0.05);
	htemp -> GetYaxis() -> SetTitle("Timing Resolution");
	htemp -> GetYaxis() -> SetTitleSize(0.05);
	htemp -> GetYaxis() -> SetRangeUser(0,0.5);

//	double* x = *vec_LY[0];
//	double* y = *vec_LY_e[0];
//	double* xe = *vec_res[0];
//	double* ye = *vec_res_e[0];
	TGraphErrors * g_TR = new TGraphErrors(n_data, vec_LY,vec_res,vec_LY_e,vec_res_e);
	g_TR -> SetMarkerStyle(20);
	g_TR -> SetMarkerSize(1.5);
	g_TR -> SetMarkerColor(1);

	g_TR -> Draw("p");

	c_TimeRes -> SaveAs("fig/fig_TimeRes.pdf");
}
