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
		G4OpticalSurface* GetOpticalSurface(G4String);

	private:
		void CreateMaterials();
		void ApplyMaterialProperties();

		G4NistManager* fNistMan;

		map<G4String, G4Material*> map_mat;
		map<G4String, G4OpticalSurface*> map_surf;
};
#endif
