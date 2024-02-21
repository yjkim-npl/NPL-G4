#ifndef OpPrimaryGeneratorAction_h
#define OpPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <fstream>
using namespace std;

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

		void GeneratePrimariesMode0(G4Event*); // for G4 particle generator
		void GeneratePrimariesMode1(G4Event*);	// for input data file

		void ReadInputFile();

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
		G4String fInputName;
		ifstream fInputFile;

		G4bool fPolarized;
		G4double fPolarization;

		vector<G4int> vec_eventID;
		vector<G4int> vec_PDG;
		vector<G4double> vec_vx, vec_vy, vec_vz, vec_vt,vec_px, vec_py, vec_pz;
};

#endif
