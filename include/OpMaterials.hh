#ifndef OpMaterials_h
#define OpMaterials_h 1

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"

#include "globals.hh"

#include <map>
using namespace std;

class OpMaterials
{
	public:
		OpMaterials();
		virtual ~OpMaterials();
		G4Material* GetMaterial(G4String);
		G4OpticalSurface* GetOpticalSurface(const G4String);

	private:
		void CreateMaterials();
		void ApplyMaterialProperties();

		G4NistManager* fNistMan;

//		G4Material* fVac;
//		G4Material* fAir;
//		G4Material* fPMMA;
//		G4Material* fPS;
//		G4Material* fSi;

		map<G4String, G4Material*> map_mat;
};
#endif
