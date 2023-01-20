#ifndef OpParameterContainer_h
#define OpParameterContainer_h

#include "TString.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class OpParameterContainer
{
	public:
		OpParameterContainer();
		OpParameterContainer(string fileName);
		virtual ~OpParameterContainer();
		const OpParameterContainer& operator=(const OpParameterContainer &right);
		void read();
		void Print();

		Bool_t GetParBool(TString name)
			{return par_bool[name];}
		Int_t GetParInt(TString name)
			{return par_int[name];}
		Double_t GetParDouble(TString name)
			{return par_double[name];}
		string GetParString(TString name)
			{return par_string[name];}

	private:
		string par_Name;
		map<TString,bool> par_bool;
		map<TString,int> par_int;
		map<TString,double> par_double;
		map<TString, string> par_string;
};
#endif

