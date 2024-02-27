#include "OpDetectorConstruction.hh"
#include "OpActionInitialization.hh"
#include "OpParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4HadronicParameters.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4OpticalPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include <string>

int main(int argc,char** argv)
{
	G4UIExecutive* ui = 0;
	ui = new G4UIExecutive(argc, argv);
	G4String parName;
	if ( argc == 1 ) 
		parName = "OpParameters.conf";
	else
		parName = argv[1];

	G4RunManager* runManager = new G4RunManager;

	OpParameterContainer* PC = OpParameterContainer::GetInstance(parName);
//	PC -> PrintParameter("All");

	// initialize the physics list
	G4String physListStr = PC -> GetParString("PhysicsList");
	G4PhysListFactory* physListFac = new G4PhysListFactory();
	physListFac -> SetVerbose(0);
	G4VModularPhysicsList* physicsList = physListFac ->GetReferencePhysList(physListStr.c_str());

	// define optical physics
	if(PC -> GetParBool("OpticalPhysics") == true)
	{
		G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
		auto opticalParams = G4OpticalParameters::Instance();
		opticalParams -> SetProcessActivation("Cerenkov",true);
		// verbose level of optical params (default : 0)
		opticalParams -> SetVerboseLevel(0);
		opticalParams -> SetProcessActivation("Scintillation",true);
		opticalParams -> SetCerenkovTrackSecondariesFirst(true);
		opticalParams -> SetScintTrackSecondariesFirst(true);
		opticalParams -> SetBoundaryInvokeSD(true);
//		G4cout << "InvokeSD: " << opticalParams->GetBoundaryInvokeSD() << G4endl;
		physicsList -> RegisterPhysics(opticalPhysics);
	}
	if(PC -> GetParBool("StepLimiter") == true)
	{
		G4StepLimiterPhysics* stepLimitPhys = new G4StepLimiterPhysics();
		physicsList -> RegisterPhysics(stepLimitPhys);
	}
	G4HadronicParameters::Instance() -> SetVerboseLevel(0);
	physicsList -> SetVerboseLevel(0);
	runManager->SetUserInitialization(physicsList);
  
	// the random seed
	G4int seed = PC -> GetParInt("RandomSeed");
	if(seed == 0) 
		G4Random::setTheSeed(time(0));
	else
		G4Random::setTheSeed(seed);

	// User action initialization
	runManager->SetUserInitialization(new OpDetectorConstruction());
	runManager->SetUserInitialization(new OpActionInitialization());

	// Initialize visualization
	//
	G4VisManager* visManager = new G4VisExecutive;
	visManager -> SetVerboseLevel(0);
	visManager->Initialize();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Process macro or start UI session
	if ( PC -> GetParBool("Visualization") == false ) 
	{ 
		// batch mode
		G4String command = "/control/execute ";
		if(argc <= 2)
		{
			G4String fileName = PC -> GetParString("MacroFile");
			UImanager->ApplyCommand(command+fileName);
		}
		else if (argc >= 3)
		{
			G4String macroFile = argv[2];
			UImanager -> ApplyCommand(command+macroFile);
		}
	} else { 
		// interactive mode
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
	}
	delete ui;
	delete visManager;
	delete runManager;
	delete PC;
}
