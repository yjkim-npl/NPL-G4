#ifndef OpMaterials_h
#define OpMaterials_h

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"

#include <map>
using namespace std;

class OpMaterials
{
	public:
		virtual ~OpMaterials();
		static OpMaterials* GetInstance();
		G4Material* GetMaterial(const G4String);
		G4OpticalSurface* GetOpticalSurface(const G4String);

	private:
		OpMaterials();
		void CreateMaterials();
		void ApplyMaterialProperties();

		static OpMaterials* fInstance();
		G4NistManager* fNistMan;

		map<G4String,G4Material*> map_mat;
		map<G4String,G4OpticalSurface*> map_surf;
};
#endif
