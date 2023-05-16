void CalcWavToEnergy()
{
	// calc optical photon wavlen to energy
	const double min_wav = 0.31;
	const double max_wav = 0.9;
	vector<double> vec_wav;
	vector<double> vec_E;
	for(int a=0; ; a++)
	{
		double wav = min_wav + 0.01*a;
		if(wav > max_wav)
			break;
		double E = 1.2398/wav;
		vec_wav.push_back(wav);
		vec_E.push_back(E);
//		cout << Form("wav: %.2f, E: %1.4f",wav,E) << endl;
	}

	// read and store the detection efficiency from csv table
	string line;
	size_t pos = -1;

	ifstream Eff_SiPM("Eff_SiPM.csv");
	getline(Eff_SiPM,line);
	cout << line << endl;
	vector<double> vec_eff;
	while(getline(Eff_SiPM,line))
	{
		while((pos=line.rfind(',')) != string::npos)
			line.erase(pos,1);
		stringstream ss(line);
		double wav, eff;
		ss >> wav >> eff;
//		cout << wav << " " << eff/100 << endl;;
		vec_eff.push_back(eff/100);
	}
	Eff_SiPM.close();
	//read FC of PVT
	ifstream FC_PVT("FC_PVT.csv");
	getline(FC_PVT,line);	// reject dummy line (x,y)
	vector<double> vec_fc;
	vec_fc.assign(vec_E.size(),0);
	while(getline(FC_PVT,line))
	{
		while((pos=line.rfind(',')) != string::npos)
			line.erase(pos,1);
		stringstream ss(line);
		double wav, fc;
		ss >> wav >> fc;
//		cout << wav << endl;
//		cout << vec_wav[0]*1e3 << endl;
		int index = 0;
		for(index = 0; ; index++)
		{
//				cout << vec_wav[index]*1000 << " " << wav << endl;
			if(vec_wav[index]*1000 == wav)
			{
				cout << vec_wav[index]*1000 << " " << wav << endl;
				vec_fc[index] = fc;
				break;
			}
			if(index > 100)
				break;
		}
	}

	ofstream out("out_MP.txt");

	// write wavlengh
	out << "wavlen: [nm]" << endl;
	for(int a=vec_wav.size()-1; a>=0; a--)
	{
		out << Form("%3.0f, ",vec_wav[a]*1000);
		if(a%8==4)
			out << endl;
	}

	// write energy
	out << endl << "eney: [eV]" << endl;
	for(int a=vec_E.size()-1; a>=0; a--)
	{
		out << Form("%1.4f*eV, ",vec_E[a]);
		if(a%8==4)
			out << endl;
	}

	// write SiPM detection efficiency
	out << endl << "efficiency: " << endl;
	for(int a=vec_eff.size()-1; a>=0; a--)
	{
		out << Form("%.2f,",vec_eff[a]);
		if(a%8==4)
			out << endl;
	}

	// write FC(emission spectra) of PVT
	out << endl << "FC of PVT: " << endl;
	for(int a=vec_fc.size()-1; a>=0; a--)
	{
		out << Form("%.5f, ",vec_fc[a]);
		if(a%8==4)
			out << endl;
	}
}
