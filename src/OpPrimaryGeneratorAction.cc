#include "OpPrimaryGeneratorAction.hh"
#include "OpParameterContainer.hh"

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


OpPrimaryGeneratorAction::OpPrimaryGeneratorAction(OpParameterContainer* par)
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0) 
{
	PC = par;
	G4int n_particle = PC -> GetParInt("NperEvent");
	fParticleGun  = new G4ParticleGun(n_particle);
	fPolarized = true;
	fPolarization = 30.;

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName = PC -> GetParString("particle");
	G4ParticleDefinition* particle
		= particleTable->FindParticle(particleName);
	fParticleGun -> SetParticleDefinition(particle);	// particle
	fParticleGun -> SetParticleTime(0.0 * ns);	// generated time
	fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));	// with momentum dir
	fParticleGun -> SetParticleEnergy(PC->GetParDouble("energy")*MeV);	// and its energy
}

OpPrimaryGeneratorAction::~OpPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void OpPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	G4double x0 = PC -> GetParDouble("x0") *(G4UniformRand()-0.5);
	G4double y0 = PC -> GetParDouble("y0") *(G4UniformRand()-0.5);
	G4double z0 = PC -> GetParDouble("z0");

	fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

	fParticleGun->GeneratePrimaryVertex(anEvent);

//	G4cout << fParticleGun -> GetParticleDefinition() -> GetParticleName() << G4endl;
	// for optical photon beam, the polarization must be defined
	if(fParticleGun -> GetParticleDefinition() ==
			G4OpticalPhoton::OpticalPhotonDefinition())
	{
		if(fPolarized)
		{
//			G4cout << "yjkim" << G4endl;
			SetOptPhotonPolar(fPolarization);
		}else{
			SetOptPhotonPolar();
		}
	}
//	G4cout << "yjkim: " << fParticleGun -> GetParticleDefinition() -> GetParticleName() << G4endl;
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
