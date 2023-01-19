#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "ParameterContainer.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(ParameterContainer* par)
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0) 
{
	PC = par;
	G4int n_particle = PC -> GetParInt("NperEvent");
	fParticleGun  = new G4ParticleGun(n_particle);

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName = PC -> GetParString("particle");
	G4ParticleDefinition* particle
		= particleTable->FindParticle(particleName);
	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
	fParticleGun->SetParticleEnergy(PC->GetParDouble("energy")*MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	G4double x0 = PC -> GetParDouble("x0") *(G4UniformRand()-0.5);
	G4double y0 = PC -> GetParDouble("y0") *(G4UniformRand()-0.5);
	G4double z0 = PC -> GetParDouble("z0");

	fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

	fParticleGun->GeneratePrimaryVertex(anEvent);
}
