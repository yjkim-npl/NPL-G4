#include "OpMaterials.hh"
#include "OpParameterContainer.hh"

#include "G4SystemOfUnits.hh"

#include <utility>

OpMaterials::OpMaterials()
{
	fNistMan = G4NistManager::Instance();
	CreateMaterials();
	ApplyMaterialProperties();
	if(OpParameterContainer::GetInstance()->GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpMaterials" << G4endl;
}

OpMaterials::~OpMaterials()
{
	if(OpParameterContainer::GetInstance()->GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpMaterials" << G4endl;
}

G4Material* OpMaterials::GetMaterial(G4String matName)
{
	G4Material* mat = map_mat[matName];
	if(!mat)
	{
		mat = fNistMan -> FindOrBuildMaterial(matName);
	}
	if(!mat)
	{
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
	fNistMan -> FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

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
	G4Material* fVac = G4Material::GetMaterial("G4_Galactic");
		map_mat.insert(make_pair("Vacuum",fVac));
	G4Material* fAir = G4Material::GetMaterial("G4_AIR");
		map_mat.insert(make_pair("Air",fAir));
	G4Material* fScint = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
		map_mat.insert(make_pair("Scintillator",fScint));
	G4Material* fSi = new G4Material("Silicon",	 z=14., a=28.09*g/mole,  density=2.33*g/cm3);
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

	G4Material* fAr = new G4Material("ArGas",  z=18, a=39.948*g/mole, density=1.7836*g/cm3,kStateGas);
		map_mat.insert(make_pair("Argon",fAr));

	// Create materials with complex component
	G4Material* fCH4 = new G4Material("CH4Gas", density=0.717e-3*g/cm3, ncompo=2,kStateGas);
	fCH4 -> AddElement(H,natoms=1);
	fCH4 -> AddElement(H,natoms=4);
		map_mat.insert(make_pair("MetanGas",fCH4));

	G4double d_p10 = 0.9*fAr->GetDensity() + 0.1*fCH4->GetDensity();
	G4Material* fP10 = new G4Material("P10Gas", density=d_p10, ncompo=2,kStateGas);
	fP10 -> AddMaterial(fAr,0.9*fAr->GetDensity()/d_p10);
	fP10 -> AddMaterial(fCH4,0.1*fCH4->GetDensity()/d_p10);
		map_mat.insert(make_pair("P10Gas",fP10));
	G4Material* fPMMA = new G4Material("PMMA",	density=1.19*g/cm3, ncompo=3);
		// Acryl, core material of cherenkov fiber
	fPMMA -> AddElement(C,natoms=5);
	fPMMA -> AddElement(H,natoms=8);
	fPMMA -> AddElement(O,natoms=2);
		map_mat.insert(make_pair("PMMA",fPMMA));
	
	G4Material* fPS = new G4Material("Polystyrene",	density=1.05*g/cm3, ncompo=2);
	fPS -> AddElement(C,natoms=8);
	fPS -> AddElement(H,natoms=8);
		map_mat.insert(make_pair("Polystyrene",fPS));
}

void OpMaterials::ApplyMaterialProperties()
{
	// photon energy spectrum
	vector<G4double> opEn_pvt = {2.479694*eV, 3.54242*eV};
	vector<G4double> opEn = {	// from 900nm to 300 nm with 50 nm step
		1.37760*eV, 1.41696*eV, 1.45864*eV, 1.50284*eV, 1.54980*eV, 1.59980*eV, 1.65312*eV, 1.71013*eV,
		1.77120*eV, 1.83680*eV, 1.90745*eV, 1.98375*eV, 2.06640*eV, 2.15625*eV, 2.25426*eV, 2.36160*eV,
		2.47968*eV, 2.61019*eV, 2.75520*eV, 2.91728*eV, 3.09960*eV, 3.30625*eV, 3.54241*eV, 3.81490*eV,     4.13281*eV};

	// Air
	G4Material* fAir = map_mat["Air"];
	vector<G4double> RI_Air;	RI_Air.assign(opEn.size(),1.0);
	G4MaterialPropertiesTable* mp_air = new G4MaterialPropertiesTable();
	mp_air -> AddProperty("RINDEX",opEn,RI_Air);
	fAir -> SetMaterialPropertiesTable(mp_air);
	map_mat["Air"] = fAir;

	// PolyVinylToluene

	G4Material* fScint = map_mat["Scintillator"];
//	vector<G4double> RI_PS = {};
	// PS
	G4Material* fPS = map_mat["Polystyrene"];
	vector<G4double> RI_PS = {
		1.57483, 1.57568, 1.57644, 1.57726, 1.57817, 1.57916, 1.58026, 1.58148,
		1.58284, 1.58435, 1.58605, 1.58796, 1.59013, 1.59328, 1.59621, 1.59960,
		1.60251, 1.60824, 1.61229, 1.62032, 1.62858, 1.63886, 1.65191, 1.66888, 1.69165
	};
	vector<G4double> ABS_PS = {
		2.714*m, 3.102*m, 3.619*m, 4.343*m, 5.791*m, 7.896*m, 4.343*m, 7.896*m,
		5.429*m, 36.19*m, 17.37*m, 36.19*m, 5.429*m, 28.95*m, 21.71*m, 14.48*m,
		12.41*m, 8.686*m, 7.238*m, 1.200*m, 0.200*m, 0.500*m, 0.200*m, 0.100*m, 0.100*m
	};
	vector<G4double> ScintFast_PS = {
		0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
		0.00, 0.00, 0.00, 0.00, 0.00, 0.03, 0.07, 0.13,
		0.33, 0.63, 1.00, 0.50, 0.00, 0.00, 0.00, 0.00, 0.00
	};
	G4MaterialPropertiesTable* mp_PS = new G4MaterialPropertiesTable();
	mp_PS -> AddProperty("RINDEX",opEn,RI_PS);
	mp_PS -> AddProperty("ABSLENGTH",opEn,ABS_PS);
	mp_PS -> AddProperty("FASTCOMPONENT",opEn,ScintFast_PS);
	mp_PS -> AddConstProperty("SCINTILLATIONYIELD",10./MeV);
	mp_PS -> AddConstProperty("RESOLUTIONSCALE",1.0);
	mp_PS -> AddConstProperty("FASTTIMECONSTANT",2.8*ns);
	fPS -> SetMaterialPropertiesTable(mp_PS);
	fPS -> GetIonisation() -> SetBirksConstant(0.126*mm/MeV);
	map_mat["Polystyrene"] = fPS;
}
