#include "OpMaterials.hh"

#include "G4SystemOfUnits.hh"

#include <utility>

OpMaterials::OpMaterials()
{
	fNistMan = G4NistManager::Instance();
	CreateMaterials();
//	ApplyMaterialProperties();
}

OpMaterials::~OpMaterials()
{}


G4Material* OpMaterials::GetMaterial(G4String matName)
{
	G4Material* mat = map_mat[matName];
	if(!mat)
	{
		G4cout << "yjkim in Nist" << G4endl;
		mat = fNistMan -> FindOrBuildMaterial(matName);
	}
	if(!mat)
	{
		G4cout << "yjkim in Nist" << G4endl;
		mat = G4Material::GetMaterial(matName);
	}
	if(!mat)
	{
		G4ExceptionDescription out;
		out << "Material " << mat << " not found!";
		G4Exception("OpMaterials::GetMaterial","",FatalException, out);
	}
	return mat;
}

G4OpticalSurface* OpMaterials::GetOpticalSurface(const G4String surfName)
{
//	G4OpticalSurface* surf = map_surf[surfName];
	G4OpticalSurface* surf = NULL;
	if(!surf)
	{
		G4ExceptionDescription out;
		out << "Surface " << surf << " not found";
		G4Exception("OpMaterials::GetOpticalSurface","",FatalException,out);
	}
	return surf;
}

void OpMaterials::CreateMaterials()
{
	// Define default material: Vac, Air
	fNistMan -> FindOrBuildMaterial("G4_Galactic");
	fNistMan -> FindOrBuildMaterial("G4_AIR");

	G4String symbol;
	G4double a, z, density;
	G4int ncompo, natoms;

	// Define elements
	G4Element* H = new G4Element("Hydrogen"	,symbol="H", z=1., a=1.01*g/mole);
	G4Element* C = new G4Element("Carbon"	,symbol="C", z=6., a=12.01*g/mole);
	G4Element* N = new G4Element("Nitrogen"	,symbol="N", z=7., a=14.01*g/mole);
	G4Element* O = new G4Element("Oxygen"	,symbol="O", z=8., a=16.00*g/mole);
	G4Element* F = new G4Element("Fluorine"	,symbol="F", z=9., a=18.9984*g/mole);

	// Create materials with single component and default material
	fVac = G4Material::GetMaterial("G4_Galactic");
		map_mat.insert(make_pair("Vacuum",fVac));
	fAir = G4Material::GetMaterial("G4_AIR");
		map_mat.insert(make_pair("Air",fAir));
	fSi = new G4Material("Silicon",	 z=14., a=28.09*g/mole,  density=2.33*g/cm3);
		map_mat.insert(make_pair("Silicon",fSi));
	G4Material* fAl = new G4Material("Aluminium",z=13., a=26.98*g/mole,  density=2.699*g/cm3);
		map_mat.insert(make_pair("Aluminium",fAl));
	G4Material* fFe = new G4Material("Iron",	 z=26., a=55.845*g/mole, density=7.874*g/cm3);
		map_mat.insert(make_pair("Iron",fFe));
	G4Material* fCu = new G4Material("Copper",	 z=29., a=63.546*g/mole, density=8.96*g/cm3);
		map_mat.insert(make_pair("Copper",fCu));
	G4Material* fW  = new G4Material("Tungsten", z=74., a=183.84*g/mole, density=19.30*g/cm3);
		map_mat.insert(make_pair("Tungsten",fW));
	G4Material* fPb = new G4Material("Lead",	 z=82., a=207.2*g/mole,  density=11.35*g/cm3);
		map_mat.insert(make_pair("Lead",fPb));

	// Create materials with complex component
	fPMMA = new G4Material("PMMA",	density=1.19*g/cm3, ncompo=3);
		// Acryl, core material of cherenkov fiber
	fPMMA -> AddElement(C,natoms=5);
	fPMMA -> AddElement(H,natoms=8);
	fPMMA -> AddElement(O,natoms=2);
		map_mat.insert(make_pair("PMMA",fPMMA));
	
	fPS = new G4Material("Polystyrene",	density=1.05*g/cm3, ncompo=2);
	fPS -> AddElement(C,natoms=8);
	fPS -> AddElement(H,natoms=8);
		map_mat.insert(make_pair("Polystyrene",fPS));
}

void OpMaterials::ApplyMaterialProperties()
{
	// photon energy spectrum
	G4double opEn_small[] = {1.37760*eV, 4.13281*eV};

	// Air
	G4double RI_Air[] = {1.0, 1.0};
	G4MaterialPropertiesTable* mp_air = new G4MaterialPropertiesTable();
	mp_air -> AddProperty("RINDEX",opEn_small,RI_Air,sizeof(opEn_small));
	fAir -> SetMaterialPropertiesTable(mp_air);
}
