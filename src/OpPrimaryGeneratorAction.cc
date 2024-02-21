#include "OpPrimaryGeneratorAction.hh"
#include "OpParameterContainer.hh"

#include "G4IonTable.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4OpticalPhoton.hh"


OpPrimaryGeneratorAction::OpPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0) 
{
	PC = OpParameterContainer::GetInstance();
	fParticleGun  = new G4ParticleGun();
	if(PC->GetParInt("Beam_InputMode") == 1){
		fInputName = PC->GetParString("InputFile");
		ReadInputFile();
	}
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Constructor of OpPrimaryGeneratorAction" << G4endl;
}

OpPrimaryGeneratorAction::~OpPrimaryGeneratorAction()
{
  delete fParticleGun;
	if(PC -> GetParInt("UserVerbosity") > 0)
		G4cout << "Destructor of OpPrimaryGeneratorAction" << G4endl;
}

void OpPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	if(PC->GetParInt("Beam_InputMode")==0)
		GeneratePrimariesMode0(anEvent);
	else if (PC->GetParInt("Beam_InputMode")==1){
		GeneratePrimariesMode1(anEvent);
	} else {
		G4ExceptionDescription out;
		out << "OpParameterContainer::Beam_InputMode should be 0 or 1";
		G4Exception("OpPrimaryGeneratorAction::GeneratePrimaries","",FatalException,out);
	}
}

void OpPrimaryGeneratorAction::GeneratePrimariesMode0(G4Event* anEvent)
{
	fPolarized = true;
	fPolarization = 30.;

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName = PC -> GetParString("Beam_particle");
	if(particleName=="ion")
	{
		G4ParticleDefinition* particle = 
			G4IonTable::GetIonTable()->GetIon(PC->GetParInt("InputIonID"));
		fParticleGun->SetParticleDefinition(particle);
		fParticleGun->SetParticleCharge(PC->GetParInt("InputCharge"));
		fParticleGun -> SetParticleEnergy(PC->GetParDouble("Beam_energy")*MeV);	// and its energy
	}
	else if (particleName=="opticalphoton")
	{
		G4ParticleDefinition* particle
			= G4OpticalPhoton::OpticalPhotonDefinition();
		fParticleGun -> SetParticleDefinition(particle);	// particle
		G4double OpE = 1.2398e-3/PC->GetParDouble("OpWavlen") * MeV;
		fParticleGun -> SetParticleEnergy(OpE);	// and its energy
		// for optical photon beam, the polarization must be defined
		if(fPolarized)
		{
			SetOptPhotonPolar(fPolarization);
		}else{
			SetOptPhotonPolar();
		}
	}
	else
	{
		G4ParticleDefinition* particle
			= particleTable->FindParticle(particleName);
		fParticleGun -> SetParticleDefinition(particle);	// particle
		fParticleGun -> SetParticleEnergy(PC->GetParDouble("Beam_energy")*MeV);	// and its energy
	}
	G4double px = PC -> GetParDouble("Beam_px");
	G4double py = PC -> GetParDouble("Beam_py");
	G4double pz = PC -> GetParDouble("Beam_pz");
	G4double time = PC -> GetParDouble("TimeWindow");
	fParticleGun -> SetParticleTime(time * ns);	// generated time
	fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(px,py,pz));	// with momentum dir

	G4int n_particle = PC -> GetParInt("NperEvent");

	if(PC -> GetParInt("Beam_shape") == 0)
	{
		fParticleGun -> SetParticlePosition(G4ThreeVector(PC->GetParDouble("Beam_x0"),PC->GetParDouble("Beam_y0"),PC->GetParDouble("Beam_z0")-PC->GetParDouble("Translation")));
		fParticleGun -> GeneratePrimaryVertex(anEvent);
	}
	else if(PC->GetParInt("Beam_shape") > 0)
	{
		for(G4int n=0; n< n_particle; n++)
		{
			G4double x0 = 
				(PC->GetParDouble("Beam_x0")+PC->GetParDouble("Beam_dx")*(G4UniformRand()-0.5)) * mm;
			G4double y0 = 
				(PC->GetParDouble("Beam_y0")+PC->GetParDouble("Beam_dy")*(G4UniformRand()-0.5)) * mm;
			G4double z0 =
				(PC->GetParDouble("Beam_z0")+PC->GetParDouble("Beam_dz")*(G4UniformRand()-0.5)) * mm;
			G4double trans = PC -> GetParDouble("Translation");

			G4double r0 = PC -> GetParDouble("Beam_r0")*2*(G4UniformRand()-0.5)*mm;
			G4double phi = 360*G4UniformRand()*deg;
			G4double rx = r0*std::cos(phi);
			G4double ry = r0*std::sin(phi);

			if(PC->GetParInt("Beam_shape") == 1)
				fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0-trans));
			if(PC->GetParInt("Beam_shape") == 2)
				fParticleGun->SetParticlePosition(G4ThreeVector(rx,ry,z0-trans));
			fParticleGun->GeneratePrimaryVertex(anEvent);
		}
	}
}

void OpPrimaryGeneratorAction::GeneratePrimariesMode1(G4Event* anEvent)
{
	// find event number(ID)
	for(G4int a=0; a<vec_eventID.size(); a++)
	{
		if(a == anEvent -> GetEventID())
		{
			G4ParticleDefinition* particle;
			if(vec_PDG[a] > 1e9)
				particle = G4IonTable::GetIonTable()->GetIon(vec_PDG[a]);
			else
				particle = G4ParticleTable::GetParticleTable()->FindParticle(vec_PDG[a]);
			fParticleGun -> SetParticleDefinition(particle);
			G4ThreeVector mom(vec_px[a],vec_py[a],vec_pz[a]);
			G4ThreeVector pos(vec_vx[a],vec_vy[a],vec_vz[a]);
			fParticleGun -> SetParticleTime(vec_vt[a]*ns);
			fParticleGun -> SetParticleMomentumDirection(mom.unit());
			fParticleGun -> SetParticleMomentum(mom.mag()*MeV);
			fParticleGun -> SetParticlePosition(pos);
			fParticleGun -> GeneratePrimaryVertex(anEvent);
			break;
		}
	}
}

void OpPrimaryGeneratorAction::ReadInputFile()
{
	fInputFile.open(fInputName.data());
	G4String line;
	getline(fInputFile,line); // dummy line(p)
	getline(fInputFile,line); // event
	G4int nEvents = 0;
	stringstream ss_E(line);
	ss_E >> nEvents;
	G4cout << "Input: " << fInputName << " contains " << nEvents << " events" << G4endl;
	getline(fInputFile,line); // dummy line(idx)
	G4int eventID, nTracks, pdg;
	G4double vx, vy, vz, vt, px, py, pz;
	while(getline(fInputFile,line))
	{
		stringstream ss(line);
		ss >> eventID >> pdg >> vx >> vy >> vz >> vt >> px >> py >> pz;
		vec_eventID.push_back(eventID);
		vec_PDG.push_back(pdg);
		vec_vx.push_back(vx);
		vec_vy.push_back(vy);
		vec_vz.push_back(vz);
		vec_vt.push_back(vt);
		vec_px.push_back(px);
		vec_py.push_back(py);
		vec_pz.push_back(pz);
	}

	ofstream runmac("run1.mac");
	runmac << "/run/initialize" << G4endl;
	runmac << "/run/beamOn " << nEvents << G4endl;
}
void OpPrimaryGeneratorAction::SetOptPhotonPolar()
{
	G4double angle = G4UniformRand() * 360.0 * deg;
	SetOptPhotonPolar(angle);
}

void OpPrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)	// Zero polarization error
{
	if(fParticleGun -> GetParticleDefinition() !=
			G4OpticalPhoton::OpticalPhotonDefinition())
	{
		G4cout << "gun particle is not optical photon!" << G4endl;
		return;
	}

	fPolarized = true;
	fPolarization = angle;

	G4ThreeVector normal(0.,0.,1.);
	G4ThreeVector kphoton = fParticleGun -> GetParticleMomentumDirection();
	G4ThreeVector product = normal.cross(kphoton);
	G4double modul2 = product * product;

	G4ThreeVector e_perpend(1.,0.,0.);
	if(modul2 >0.)
		e_perpend = (1./std::sqrt(modul2))*product;
	G4ThreeVector e_paralle = e_perpend.cross(kphoton);

	G4ThreeVector polar = 
		std::cos(angle) * e_paralle + std::sin(angle) * e_perpend;
	fParticleGun -> SetParticlePolarization(polar);
}
