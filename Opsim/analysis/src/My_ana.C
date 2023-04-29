#include "My_ana.h"

My_ana::My_ana(const int MakeOpt, char* prefix, char* p, char* e, char*s)
{
	// Link Input ROOT file
	TString str_p = p;
	TString str_s = s;
	int NameOpt = 0;
	if(str_p.Contains("temp")){
		if(str_s=="") NameOpt = 1;
		else          NameOpt = 2;
	}else{
		if(str_s=="") NameOpt = 3;
		else          NameOpt = 4;
	}
	string infile = OutName(NameOpt, prefix,p,e,s,"root");
	Init(infile);
	ReadLists();
	LinkTree(MakeOpt);
}

My_ana::~My_ana()
{
}

void My_ana::Init(string infile)
{
	cout << "Infile: " << infile << endl;
	F = new TFile(infile,"read");
}

void My_ana::ReadLists()
{
	L_proc = (TList*) F -> Get("ProcessTable");
	for(int a=0; a<L_proc->GetSize(); a++)
	{
		TNamed* name = (TNamed*)L_proc -> At(a);
		map_proc.insert(make_pair(name->GetTitle(),stoi(name->GetName())));
	}

	L_para = (TList*) F -> Get("InputParameters");
	for(int a=0; a<L_para->GetSize(); a++)
	{
		TNamed* name = (TNamed*)L_para->At(a);
		map_para.insert(make_pair(name->GetName(),name->GetTitle()));
	}
}

void My_ana::LinkTree(const int MakeOpt)
{
	if(MakeOpt == OPT::Make)
	{
		T = (TTree*) F -> Get("Opsim");
		TObjArray* branches = (TObjArray*) T -> GetListOfBranches();
		for(int a=0; a<branches -> GetEntries(); a++)
		{
			TObjString* branch = (TObjString*) branches -> At(a);
			TString b_name = branch -> GetName();
			if(b_name.BeginsWith("n") || b_name.BeginsWith("N"))
			{
				int a;
				T -> 
			}
		}
	}
	else if(MakeOpt == OPT::Draw)
	{
	}
}

string OutName(int opt, char* prefix, char* p, char* e, char* s, char* ext)
{
	string out;
	if(opt == 1)
		out = Form("%s_%s.%s",prefix,p,ext);
	else if(opt == 2)
		out = Form("%s_%s_%s.%s",prefix,p,s,ext);
	else if(opt == 3)
		out = Form("%s_%s_%s.%s",prefix,p,e,ext);
	else if(opt == 4)
		out = Form("%s_%s_%s_%s.%s",prefix,p,e,s,ext);
	else
		out = "blank";
	return out;
}
