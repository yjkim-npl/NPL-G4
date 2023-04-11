#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TMath.h"
#include "TNamed.h"
#include "TLine.h"
#include "TList.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TStyle.h"
#include "TString.h"
#include "TTree.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <string>

#ifndef My_ana
#define My_ana
class My_ana
{
	public:
		My_ana(const int MakeOpt, char* prefix, char*p, char*e, char*s);
		virtual ~My_ana()

		virtual void MakeHist()
		virtual void DrawHist();

		string OutName(int opt, char* prefix, char* p, char* e, char* s, char* ext);

	protected:
		enum OPT{Make,Draw};

		void Init(string infile); //done
		void ReadLists(); // done
		virtual void LinkTree(const int opt);
		TFile* F;
		TTree* T;
		TList* L_proc;
		TList* L_para;
		map<string,int> map_proc;
		map<string,string> map_para;
		vector<TH1F*> vec_H1;
		vector<TH2F*> vec_H2;
		vector<TH3F*> vec_H3;
		vector<int> vec_int;
		vector<vector<int>*> vec_vecp_int;
		vector<vector<double>*> vec_vecp_double;
};
#endif
