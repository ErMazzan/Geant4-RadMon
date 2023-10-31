/////////////////////////////////////////////////////////
//
// Apr/2015  E. Nacher -> PhysicsList.cc
//
// Physics List is a simplification of the LXePhysicsList 
// ($G4INSTALL/examples/extended/optical/LXe). EM physics 
// just registering G4EmStandardPhysics and no Decay Physics.
//
////////////////////////////////////////////////////////

#include "MyPhysicsList.hh"

#include "FTFP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"


// #include "G4OpticalProcessIndex.hh"
#include "G4Scintillation.hh"

#include "G4HadronPhysicsQGSP_BIC.hh"

#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"
#include "G4Scintillation.hh"

#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"


MyPhysicsList::MyPhysicsList() : G4VModularPhysicsList()
{	
	// Construct all physics to use:

	// hadronic physics
	auto hadronicList = new G4HadronPhysicsFTFP_BERT();
	RegisterPhysics(hadronicList);

	// em physics
	// G4VPhysicsConstructor* emList = new G4EmStandardPhysics_option3(); // for medical and space
	auto emList = new G4EmStandardPhysics();
	RegisterPhysics(emList);
	
	// optical physics
	G4OpticalPhysics* opticalList = new G4OpticalPhysics();

	// Optical processes parameters
	// Default processes enabled:
		// processActivation["OpRayleigh"]    = true;
		// processActivation["OpBoundary"]    = true;
		// processActivation["OpMieHG"]       = true;
		// processActivation["OpAbsorption"]  = true;
		// processActivation["OpWLS"]         = true;
		// processActivation["OpWLS2"]        = true;
		// processActivation["Cerenkov"]      = true;
		// processActivation["Scintillation"] = true;
	auto opticalParams = G4OpticalParameters::Instance();
	opticalParams->SetProcessActivation("Cerenkov",false); //disable cherenkov effect
	opticalParams->SetProcessActivation("Scintillation",true);
	opticalParams->SetScintTrackSecondariesFirst(true);  // track secondaries for scintillating processes

	RegisterPhysics(opticalList);

	G4cout << "All physical processes set :)" << G4endl;

}


MyPhysicsList::~MyPhysicsList() {}

void MyPhysicsList::SetCuts(){
	
	SetCutsWithDefault();
}
