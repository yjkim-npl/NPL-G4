#include "OpMaterials.hh"
#include "OpParameterContainer.hh"

#include "G4SystemOfUnits.hh"

#include <utility>

OpMaterials::OpMaterials()
{
	fNistMan = G4NistManager::Instance();
	CreateMaterials();
	if(OpParameterContainer::GetInstance()->GetParBool("OpticalPhysics"))
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

G4OpticalSurface* OpMaterials::GetOpticalSurface(G4String surfName)
{
	G4OpticalSurface* surf = map_surf[surfName];
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
	fNistMan -> FindOrBuildMaterial("G4_POLYETHYLENE");
	fNistMan -> FindOrBuildMaterial("G4_B");

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
	G4Material* B = G4Material::GetMaterial("G4_B");
		map_mat.insert(make_pair("B",B));
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

	G4double fSTPTemp = 273.15 * kelvin;
	G4double fLabTemp = fSTPTemp + 20 * kelvin;
	G4double ArGasD = 1.7836* mg/cm3 * fSTPTemp/fLabTemp;
	G4Material* fAr = new G4Material("ArGas",  z=18, a=39.948*g/mole, ArGasD,kStateGas,fLabTemp);
		map_mat.insert(make_pair("Argon",fAr));

	// Create materials with complex component

	G4Material* fGlass = new G4Material("Glass",density=1.032*g/cm3,2);
	fGlass -> AddElement(C,91.533*perCent);
	fGlass -> AddElement(H,8.467*perCent);
		map_mat.insert(make_pair("Glass",fGlass));

	G4Material* fPVT = new G4Material("PVT", density=1.023*g/cm3, ncompo=2, kStateSolid);
	G4double nH_per_cm3 = 5.15;
	G4double nC_per_cm3 = 4.69;
	fPVT -> AddElement(H,natoms=9);
	fPVT -> AddElement(C,natoms=9);
		map_mat.insert(make_pair("Scintillator",fPVT));

	G4Material* fC2H4 = G4Material::GetMaterial("G4_POLYETHYLENE");
		map_mat.insert(make_pair("G4_POLYTEHTYLENE",fC2H4));
	
	G4double BPD = 0.3*B->GetDensity() + 0.7*fC2H4->GetDensity();
	G4Material* fBP = new G4Material("BoratedPolyethylene",BPD,2);
	fBP -> AddMaterial(B,0.3);
	fBP -> AddMaterial(fC2H4,0.7);
		map_mat.insert(make_pair("BoratedPolyethylene",fBP));
	
	G4Material* fGraphite = new G4Material("Graphite",density=1.95*g/cm3,ncompo=1,kStateSolid);
	fGraphite -> AddElement(C,natoms = 6);
		map_mat.insert(make_pair("Graphite",fGraphite));

	G4double CH4GasD = 0.717 * mg/cm3 * fSTPTemp/fLabTemp;
	G4Material* fCH4 = new G4Material("CH4Gas", CH4GasD, ncompo=2,kStateGas, fLabTemp);
	fCH4 -> AddElement(H,natoms=1);
	fCH4 -> AddElement(H,natoms=4);
		map_mat.insert(make_pair("MetanGas",fCH4));

	G4double d_p10 = 0.9*ArGasD + 0.1*CH4GasD;
	G4Material* fP10 = new G4Material("P10Gas", density=d_p10, ncompo=2,kStateGas, fLabTemp);
	fP10 -> AddMaterial(fAr,0.9*ArGasD/d_p10);
	fP10 -> AddMaterial(fCH4,0.1*CH4GasD/d_p10);
		map_mat.insert(make_pair("P10Gas",fP10));
	G4Material* fPMMA = new G4Material("PMMA",	density=1.19*g/cm3, ncompo=3);
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
	vector<G4double> opEn_PVT = {
		// from 900mm to 310mm with 10 nm step
		1.3776*eV, 1.3930*eV, 1.4089*eV, 1.4251*eV, 1.4416*eV, 1.4586*eV, 1.4760*eV, 1.4937*eV, 
		1.5120*eV, 1.5306*eV, 1.5497*eV, 1.5694*eV, 1.5895*eV, 1.6101*eV, 1.6313*eV, 1.6531*eV, 
		1.6754*eV, 1.6984*eV, 1.7219*eV, 1.7462*eV, 1.7711*eV, 1.7968*eV, 1.8232*eV, 1.8504*eV, 
		1.8785*eV, 1.9074*eV, 1.9372*eV, 1.9679*eV, 1.9997*eV, 2.0325*eV, 2.0663*eV, 2.1014*eV, 
		2.1376*eV, 2.1751*eV, 2.2139*eV, 2.2542*eV, 2.2959*eV, 2.3392*eV, 2.3842*eV, 2.4310*eV, 
		2.4796*eV, 2.5302*eV, 2.5829*eV, 2.6379*eV, 2.6952*eV, 2.7551*eV, 2.8177*eV, 2.8833*eV, 
		2.9519*eV, 3.0239*eV, 3.0995*eV, 3.1790*eV, 3.2626*eV, 3.3508*eV, 3.4439*eV, 3.5423*eV, 
		3.6465*eV, 3.7570*eV, 3.8744*eV, 3.9994*eV};
	G4double opEn_PVT_arr[] = {
		// from 900mm to 310mm with 10 nm step
		1.3776*eV, 1.3930*eV, 1.4089*eV, 1.4251*eV, 1.4416*eV, 1.4586*eV, 1.4760*eV, 1.4937*eV, 
		1.5120*eV, 1.5306*eV, 1.5497*eV, 1.5694*eV, 1.5895*eV, 1.6101*eV, 1.6313*eV, 1.6531*eV, 
		1.6754*eV, 1.6984*eV, 1.7219*eV, 1.7462*eV, 1.7711*eV, 1.7968*eV, 1.8232*eV, 1.8504*eV, 
		1.8785*eV, 1.9074*eV, 1.9372*eV, 1.9679*eV, 1.9997*eV, 2.0325*eV, 2.0663*eV, 2.1014*eV, 
		2.1376*eV, 2.1751*eV, 2.2139*eV, 2.2542*eV, 2.2959*eV, 2.3392*eV, 2.3842*eV, 2.4310*eV, 
		2.4796*eV, 2.5302*eV, 2.5829*eV, 2.6379*eV, 2.6952*eV, 2.7551*eV, 2.8177*eV, 2.8833*eV, 
		2.9519*eV, 3.0239*eV, 3.0995*eV, 3.1790*eV, 3.2626*eV, 3.3508*eV, 3.4439*eV, 3.5423*eV, 
		3.6465*eV, 3.7570*eV, 3.8744*eV, 3.9994*eV};
		
	vector<G4double> opEn_PS = {
		// from 900nm to 300 nm with 24 nm step
		1.37760*eV, 1.41696*eV, 1.45864*eV, 1.50284*eV, 1.54980*eV, 1.59980*eV, 1.65312*eV, 1.71013*eV,
		1.77120*eV, 1.83680*eV, 1.90745*eV, 1.98375*eV, 2.06640*eV, 2.15625*eV, 2.25426*eV, 2.36160*eV,
		2.47968*eV, 2.61019*eV, 2.75520*eV, 2.91728*eV, 3.09960*eV, 3.30625*eV, 3.54241*eV, 3.81490*eV,     4.13281*eV};

	// Air
	G4Material* fAir = map_mat["Air"];
	vector<G4double> RI_Air;
	G4MaterialPropertiesTable* mp_air = new G4MaterialPropertiesTable();
	if(OpParameterContainer::GetInstance() -> GetParInt("SCmatOpt") == 0)
	{
		RI_Air.assign(opEn_PS.size(),1.0);
		mp_air -> AddProperty("RINDEX",opEn_PS,RI_Air);
	}
	else if (OpParameterContainer::GetInstance() -> GetParInt("SCmatOpt") == 1)
	{
		RI_Air.assign(opEn_PVT.size(),1.0);
		mp_air -> AddProperty("RINDEX",opEn_PVT,RI_Air);
	}
	fAir -> SetMaterialPropertiesTable(mp_air);
	fAir -> SetName("G4_optical_Air");
		map_mat.insert(make_pair("optical_Air",fAir));

	// Glass
	G4Material* fGlass = map_mat["Glass"];
	vector<G4double> RI_Glass;
	vector<G4double> Abs_Glass; 
	G4MaterialPropertiesTable* mp_Glass = new G4MaterialPropertiesTable();
	if(OpParameterContainer::GetInstance()->GetParInt("SCmatOpt")==0){
		RI_Glass.assign(opEn_PS.size(),1.55);
		Abs_Glass.assign(opEn_PS.size(),420*cm);
		mp_Glass -> AddProperty("RINDEX",opEn_PS,RI_Glass);
		mp_Glass -> AddProperty("ABSLENGTH",opEn_PS,Abs_Glass);
	}else if (OpParameterContainer::GetInstance()->GetParInt("SCmatOpt") == 1){
		RI_Glass.assign(opEn_PVT.size(),1.55);
		Abs_Glass.assign(opEn_PVT.size(),420*cm);
		mp_Glass -> AddProperty("RINDEX",opEn_PVT,RI_Glass);
		mp_Glass -> AddProperty("ABSLENGTH",opEn_PVT,Abs_Glass);
	}else{
		G4cout << "SCmatOpt: " << OpParameterContainer::GetInstance()->GetParInt("SCmatOpt") << G4endl;
		G4cout << "SC mat Opt should be the value 0 or 1" << G4endl;
	}
	fGlass -> SetMaterialPropertiesTable(mp_Glass);
	map_mat["Glass"] = fGlass;

	// PolyVinylToluene
	G4Material* fScint = map_mat["Scintillator"];
	vector<G4double> ABS_PVT, RI_PVT;
	ABS_PVT.assign(opEn_PVT.size(),1.2*m);
	RI_PVT.assign(opEn_PVT.size(),1.58);
	vector<G4double> ScintFast_PVT = {
		0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 
		0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 
		0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 
		0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 
		0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 
		0.00336, 0.01466, 0.02871, 0.04328, 0.06822, 0.11101, 0.18534, 0.28374, 
		0.42875, 0.64257, 0.90365, 0.99851, 0.81198, 0.24271, 0.04360, 0.00132, 
		0.00000, 0.00000, 0.00000, 0.00000
	};
	G4double PVT_SY = OpParameterContainer::GetInstance() -> GetParDouble("PVT_ScintYield");
	G4double PVT_RS = OpParameterContainer::GetInstance() -> GetParDouble("PVT_ResScale");
	G4double PVT_FC = OpParameterContainer::GetInstance() -> GetParDouble("PVT_FastConst");
	G4double PVT_BC = OpParameterContainer::GetInstance() -> GetParDouble("PVT_BirksConst");
	G4MaterialPropertiesTable* mp_PVT = new G4MaterialPropertiesTable();
	mp_PVT -> AddProperty("RINDEX",opEn_PVT,RI_PVT);
	mp_PVT -> AddProperty("ABSLENGTH",opEn_PVT,ABS_PVT);
	mp_PVT -> AddProperty("FASTCOMPONENT",opEn_PVT,ScintFast_PVT);
	mp_PVT -> AddConstProperty("SCINTILLATIONYIELD",PVT_SY/MeV); // need to optimize at 1MeV e-
	mp_PVT -> AddConstProperty("RESOLUTIONSCALE",PVT_RS);
	mp_PVT -> AddConstProperty("FASTTIMECONSTANT",PVT_FC*ns);
	mp_PVT -> AddConstProperty("FASTCINTILLATIONRISETIME",0.5*ns);
	fScint -> SetMaterialPropertiesTable(mp_PVT);
	fScint -> GetIonisation() -> SetBirksConstant(PVT_BC*mm/MeV);
	map_mat["Scintillator"] = fScint;

	// PS
	G4Material* fPS = map_mat["Polystyrene"];
	vector<G4double> RI_PS = {
		1.57483, 1.57568, 1.57644, 1.57726, 1.57817, 1.57916, 1.58026, 1.58148,
		1.58284, 1.58435, 1.58605, 1.58796, 1.59013, 1.59328, 1.59621, 1.59960,
		1.60251, 1.60824, 1.61229, 1.62032, 1.62858, 1.63886, 1.65191, 1.66888, 1.69165
	};
	vector<G4double> ABS_PS = {
		100*m,100*m,100*m,100*m,100*m,100*m,100*m,100*m,
		100*m,100*m,100*m,100*m,100*m,100*m,100*m,100*m,
		100*m,100*m,100*m,100*m,100*m,100*m,100*m,100*m,100*m
//		2.714*m, 3.102*m, 3.619*m, 4.343*m, 5.791*m, 7.896*m, 4.343*m, 7.896*m,
//		5.429*m, 36.19*m, 17.37*m, 36.19*m, 5.429*m, 28.95*m, 21.71*m, 14.48*m,
//		12.41*m, 8.686*m, 7.238*m, 1.200*m, 0.200*m, 0.500*m, 0.200*m, 0.100*m, 0.100*m
	};
	vector<G4double> ScintFast_PS = {
		0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
		0.00, 0.00, 0.00, 0.00, 0.00, 0.03, 0.07, 0.13,
		0.33, 0.63, 1.00, 0.50, 0.00, 0.00, 0.00, 0.00, 0.00
	};
	G4double PS_SY = OpParameterContainer::GetInstance() -> GetParDouble("PS_ScintYield");
	G4double PS_RS = OpParameterContainer::GetInstance() -> GetParDouble("PS_ResScale");
	G4double PS_FC = OpParameterContainer::GetInstance() -> GetParDouble("PS_FastConst");
	G4double PS_BC = OpParameterContainer::GetInstance() -> GetParDouble("PS_BirksConst");
	G4MaterialPropertiesTable* mp_PS = new G4MaterialPropertiesTable();
	mp_PS -> AddProperty("RINDEX",opEn_PS,RI_PS);
	mp_PS -> AddProperty("ABSLENGTH",opEn_PS,ABS_PS);
	mp_PS -> AddProperty("FASTCOMPONENT",opEn_PS,ScintFast_PS);
	mp_PS -> AddConstProperty("SCINTILLATIONYIELD",PS_SY/MeV);
	mp_PS -> AddConstProperty("RESOLUTIONSCALE",PS_RS);
	mp_PS -> AddConstProperty("FASTTIMECONSTANT",PS_FC*ns);
	fPS -> SetMaterialPropertiesTable(mp_PS);
	fPS -> GetIonisation() -> SetBirksConstant(PS_BC*mm/MeV);
	map_mat["Polystyrene"] = fPS;

	// applying surface properties PVT
	vector<G4double> refl_SiPM; refl_SiPM.assign(opEn_PVT.size(),0);
	vector<G4double> eff_SiPM = {
		0.03,0.04,0.04,0.05,0.05,0.06,0.06,0.07,
		0.07,0.08,0.08,0.09,0.09,0.10,0.11,0.11,
		0.12,0.13,0.14,0.14,0.15,0.16,0.17,0.18,
		0.19,0.20,0.21,0.22,0.24,0.25,0.26,0.28,
		0.29,0.30,0.32,0.33,0.34,0.36,0.37,0.38,
		0.39,0.39,0.40,0.40,0.40,0.40,0.39,0.39,	// 450 nm modified to 1 default:0.4
		0.38,0.37,0.35,0.33,0.30,0.27,0.24,0.21,
		0.14,0.06,0.02,0.00
	};
	eff_SiPM.clear();
	eff_SiPM.assign(opEn_PVT.size(),1.0);

	G4MaterialPropertiesTable* mp_SiPM = new G4MaterialPropertiesTable();
	mp_SiPM -> AddProperty("REFLECTIVITY",opEn_PVT,refl_SiPM);
	mp_SiPM -> AddProperty("EFFICIENCY",opEn_PVT,eff_SiPM);
	G4OpticalSurface* fSiPMSurf = new G4OpticalSurface("SiPMSurf",glisur,polished,dielectric_metal);	
	fSiPMSurf -> SetMaterialPropertiesTable(mp_SiPM);
	map_surf["SiPMSurf"] = fSiPMSurf;
}
