void Draw()
{
	const char* prefix = "H_Op_LY_TimeRes";
	const char* Hprefix = "H2_LY_TimeRes";
	const char* type[2] = {"MeV","SY"};

	TCanvas* c1 = new TCanvas("c1","c1",1.2*600*5,600*4);
	c1 -> Divide(5,4);
	for(int a=0; a<2; a++)
	for(int b=0; b<10; b++)
	{
		c1 -> cd(10*a+b+1);
		TFile* F;
		TH2F* H2;
		if(a==0)
		{
			F = new TFile(Form("%s_%d%s.root",prefix,100+100*b,type[a]),"read");
			H2 = (TH2F*) F -> Get(Form("%s_%d%s",Hprefix,100+100*b,type[a]));
		}
		else if (a==1)
		{
			F = new TFile(Form("%s_%s%d.root",prefix,type[a],100+100*b),"read");
			H2 = (TH2F*) F -> Get(Form("%s_%s%d",Hprefix,type[a],100+100*b));
		}
		H2 -> Draw("colz");
	}
}
