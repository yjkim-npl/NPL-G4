#include "OpDetectorConstruction.hh"
#include "OpActionInitialization.hh"
#include "OpParameterContainer.hh"

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4OpticalPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include <string>

int main(int argc,char** argv)
{
	G4UIExecutive* ui = 0;
	if ( argc == 1 ) 
		ui = new G4UIExecutive(argc, argv);

	G4RunManager* runManager = new G4RunManager;

	string parameters = "OpParameters.conf";
	OpParameterContainer* PC = new OpParameterContainer(parameters);

	// initialize the physics list
	G4String physListStr = PC -> GetParString("physicslist");
	G4PhysListFactory* physListFac = new G4PhysListFactory();
	G4VModularPhysicsList* physicsList = physListFac ->GetReferencePhysList(physListStr.c_str());

	// define optical physics
	G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
	physicsList -> RegisterPhysics(opticalPhysics);

	runManager->SetUserInitialization(physicsList);
  
	// the random seed
	G4int seed = PC -> GetParInt("RandomSeed");
	G4Random::setTheSeed(seed);
	// User action initialization
	runManager->SetUserInitialization(new OpDetectorConstruction(PC));
	runManager->SetUserInitialization(new OpActionInitialization(PC));

	// Initialize visualization
	//
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Process macro or start UI session
	if ( PC -> GetParBool("visualisation") == false ) 
	{ 
		// batch mode
		G4String command = "/control/execute ";
		G4String fileName = PC -> GetParString("macroFile");
		UImanager->ApplyCommand(command+fileName);
	} else { 
		// interactive mode
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete ui;
	}

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// owned and deleted by the run manager, so they should not be deleted 
	// in the main() program !

	delete visManager;
	delete runManager;
}
