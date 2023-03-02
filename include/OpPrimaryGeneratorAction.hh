#ifndef OpPrimaryGeneratorAction_h
#define OpPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;

class OpParameterContainer;

class OpPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
	public:
		OpPrimaryGeneratorAction();    
		virtual ~OpPrimaryGeneratorAction();

		// method from the base class
		virtual void GeneratePrimaries(G4Event*);

		// method to access particle gun
		G4ParticleGun* GetParticleGun() { return fParticleGun; }

		// for Optical photon beam
		void SetOptPhotonPolar();
		void SetOptPhotonPolar(G4double);
		G4bool GetPolarized()	{return fPolarized;}
		G4double GetPolarization()	{return fPolarization;}

	private:
		G4ParticleGun*  fParticleGun; // pointer a to G4 gun class
		OpParameterContainer* PC;

		G4bool fPolarized;
		G4double fPolarization;
};

#endif
