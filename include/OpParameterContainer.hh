#ifndef OpParameterContainer_h
#define OpParameterContainer_h

#include "globals.hh"
#include "TList.h"

#include <iostream>
#include <fstream>
#include <map>
#include <string>
using namespace std;

class OpParameterContainer
{
	public:
		static OpParameterContainer* GetInstance(G4String fName = "OpParameters.conf");
		virtual ~OpParameterContainer();
		const OpParameterContainer& operator=(const OpParameterContainer &right);
		void ReadParameters();
		void PrintParameter(G4String par);

		G4bool GetParBool(G4String name)
			{return par_bool[name];}
		G4int GetParInt(G4String name)
			{return par_int[name];}
		G4double GetParDouble(G4String name)
			{return par_double[name];}
		G4String GetParString(G4String name)
			{return par_string[name];}
		map<G4String,G4String> GetInputParameters(G4int nevnts);

	private:
		static OpParameterContainer* fInstance;
		OpParameterContainer(G4String fName);

		string par_Name;
		map<G4String,G4bool> par_bool;
		map<G4String,G4int> par_int;
		map<G4String,G4double> par_double;
		map<G4String,G4String> par_string;
};
#endif

