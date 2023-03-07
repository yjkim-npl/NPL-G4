#include "OpPrimaryGeneratorAction.hh"
#include "OpParameterContainer.hh"

#include "G4IonTable.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
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
	}
	else
	{
		G4ParticleDefinition* particle
			= particleTable->FindParticle(particleName);
		fParticleGun -> SetParticleDefinition(particle);	// particle
	}
	fParticleGun -> SetParticleTime(0.0 * ns);	// generated time
	fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));	// with momentum dir
	fParticleGun -> SetParticleEnergy(PC->GetParDouble("Beam_energy")*MeV);	// and its energy

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
//			G4double r0 = 
//				(PC->GetParDouble("Beam_r0")+PC->GetParDouble("Beam_dr")*(G4UniformRand()-0.5)) * mm;
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
			// for optical photon beam, the polarization must be defined
			if(fParticleGun -> GetParticleDefinition() ==
					G4OpticalPhoton::OpticalPhotonDefinition())
			{
				if(fPolarized)
				{
//			  	G4cout << "yjkim" << G4endl;
					SetOptPhotonPolar(fPolarization);
				}else{
					SetOptPhotonPolar();
				}
			}
		}
	}
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
	G4cout << "yjkim invoked from SetOptPhotonPolar" << G4endl;
}
