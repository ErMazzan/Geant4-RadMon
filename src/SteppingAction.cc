//
//  SteppingAction.cpp
//  
//
//  Created by David Mazzanti on 2/5/23.
//

#include "SteppingAction.hh"
#include "B4cDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "B4cEventAction.hh"

#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"

/*
SteppingAction::SteppingAction(B4cDetectorConstruction* detConstruction)
  : fDetConstruction(detConstruction)
{
}
*/

SteppingAction::SteppingAction(B4RunAction* runAct, B4cEventAction* evtAct, MyTrackingAction* trackAct):  
    fRunAct(runAct), 
    fEventAct(evtAct),
    fTrackAct(trackAct)
{
}

SteppingAction::~SteppingAction()
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    
  G4Track* Track = step->GetTrack();
  G4int parent_id = Track->GetParentID();
  // const G4ParticleDefinition* part = Track->GetDefinition();
  // G4int pdg = part->GetPDGEncoding(); // The Particle Data Group integer identifier of this particle
  
  const G4StepPoint* prePoint = step->GetPreStepPoint();
  const G4StepPoint* endPoint = step->GetPostStepPoint();
  const G4String currentPhysicalName = prePoint->GetPhysicalVolume()->GetName();
  const G4String particleName = Track->GetDefinition()->GetParticleName();
  const G4String ProcessName = endPoint -> GetProcessDefinedStep() -> GetProcessName();

  FirstIntStep = fRunAct->GetFirstIntFlag();
  

  // Retrieve initial kinetic energy of primary particles
  if (currentPhysicalName == "WorldPV" && FirstIntStep == 0 && parent_id == 0) {
    G4double kinE = Track->GetKineticEnergy();
    fRunAct->SetKinEnergy(kinE);
    FirstIntStep = 1;
    fRunAct->SetFirstIntFlag(FirstIntStep);
  }


  // Get kinetic energy of protons before entering each scintillator
  if (particleName== "proton" && endPoint->GetStepStatus() == fGeomBoundary) {
    G4TouchableHandle touchable = endPoint->GetTouchableHandle();

    if (touchable->GetVolume()->GetName() == "ScintillatorPV"){
      G4int ScintCopyNo = touchable->GetCopyNumber(1); // Copy number of mother volume the refractor
      
      if (ScintCopyNo == 0){ fTrackAct->SetS1Ekin(Track->GetKineticEnergy()); }
      if (ScintCopyNo == 1){ fTrackAct->SetS2Ekin(Track->GetKineticEnergy()); }
      if (ScintCopyNo == 2){ fTrackAct->SetS3Ekin(Track->GetKineticEnergy()); }
      if (ScintCopyNo == 3){ fTrackAct->SetS4Ekin(Track->GetKineticEnergy()); }
      
    }
  }
 

  // Only for optical photons:
  if (particleName == "opticalphoton"){
    G4TouchableHandle touchable = prePoint->GetTouchableHandle();

    if (touchable->GetVolume()->GetName() == "ScintillatorPV"){
      fEventAct->nScintPhotons++;

      G4int ScintCopyNo = touchable->GetCopyNumber(1); // Copy number of mother volume the refractor
      // G4cout << " <<< SCINTILLATION PHOTON IN SCINT NUM  " << ScintCopyNo << 
      // "Current step number:" << Track->GetCurrentStepNumber() << G4endl; 
    
      if (ScintCopyNo == 0){ fEventAct->nScint1Photons++; }
      if (ScintCopyNo == 1){ fEventAct->nScint2Photons++; }
      if (ScintCopyNo == 2){ fEventAct->nScint3Photons++; }
      if (ScintCopyNo == 3){ fEventAct->nScint4Photons++; }

      step->GetTrack()->SetTrackStatus(fStopAndKill);
    }

    // Count num of scintillation photons
    // if (currentPhysicalName == "ScintillatorPV" && Track->GetCurrentStepNumber() == 1){
    //   fEventAct->nScintPhotons++;
    //   step->GetTrack()->SetTrackStatus(fStopAndKill); //use it to avoid the tracking of OpPhotons
    // }

    // Count photons detected by any sipm
    // if (currentPhysicalName == "SiPMPV"){
    //   if (ProcessName == "OpAbsorption"){ 
    //     fEventAct->nDetectedPhotons++;
    //     // G4cout << "Optical photon detected" << G4endl;
    //   } 
    // }

    // Kill photons that exit the detector
    if (endPoint->GetPhysicalVolume()){
      auto postPV = endPoint->GetPhysicalVolume()->GetName();

      // Check if it exits to world and kill
      if (postPV == "WorldPV"){
        Track->SetTrackStatus(fStopAndKill);
      }

      // If it doesn't exit to world check that from world doesn't enter into the detector again
      else {
        if (currentPhysicalName=="WorldPV") { 
          G4cout << "WARNING! Photon in World entering back to: " << postPV << 
          "Current step number:" << Track->GetCurrentStepNumber() << G4endl; 

          Track->SetTrackStatus(fStopAndKill);

          // G4cout << "Boundary process status: " << BoundStatus << G4endl;
        }
      }
      
    }
  
    
    // Boundary informations//////////////////////////////////////
    // G4OpBoundaryProcessStatus BoundStatus = Undefined;
    // G4ProcessManager* OpProcessManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

    // // if (OpManager) {
    // G4int n_processes = OpProcessManager->GetPostStepProcessVector()->entries();
    // G4ProcessVector* fPostStepDoItVector = OpProcessManager->GetPostStepProcessVector(typeDoIt);

    // for ( G4int i=0; i<n_processes; i++) {
    //   G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
    //   auto opProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
    //   // G4cerr << "Process vector: "<< fCurrentProcess->GetProcessName() << G4endl;
    
    //   if (opProcess) { /* If current optical process is OpBoundary get Status (type of boundary interaction) */
    //     BoundStatus = opProcess->GetStatus(); 
    //     // auto boundaryprocessname = opProcess->GetProcessName();
    //     // if (BoundStatus==Transmission){
    //     //   G4cerr << "OpticalPhoton transmitted from: "<< currentPhysicalName <<
    //     //   " to " << endPoint->GetPhysicalVolume()->GetName() << "; Process: " << ProcessName<< G4endl;
    //     //   }
    //     // G4cerr << "Process Status: "<< BoundStatus << G4endl;
    //     // G4cerr << "Process Name: "<< boundaryprocessname << G4endl;
    //     // G4cerr << "Get Process Defined Step: "<< ProcessName << G4endl;
    //     break;
    //   }
    // }


    // Check for photons processes in the reflector:
    // - OpAbsorption -> photons exit scint
    // - Transportation -> photons reflect back into scintillator
    // if (currentPhysicalName == "ReflectorPV"){
    //   G4cout << "Process: " << ProcessName << G4endl;
    // }

  }
    


/*
 / Boundary informations//////////////////////////////////////
 G4OpBoundaryProcessStatus BoundStatus = Undefined;
    G4ProcessManager* OpManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

    // if (OpManager) {
    G4int n_processes = OpManager->GetPostStepProcessVector()->entries();
    G4ProcessVector* fPostStepDoItVector = OpManager->GetPostStepProcessVector(typeDoIt);

    for ( G4int i=0; i<n_processes; i++) {
      G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
      auto opProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);

      if (opProcess) {
        BoundStatus = opProcess->GetStatus(); 
        auto boundaryprocessname = opProcess->GetProcessName();
        // if (BoundStatus==Transmission){
        //   G4cerr << "OpticalPhoton transmitted from: "<< currentPhysicalName <<
        //   " to " << endPoint->GetPhysicalVolume()->GetName() << "; Process: " << ProcessName<< G4endl;
        //   }
        G4cerr << "Process Status: "<< BoundStatus << G4endl;
        G4cerr << "Process Name: "<< boundaryprocessname << G4endl;
        // break;
        }
    }
    // }
*/

  // check if the photon is absorbed in the sensitive volume
  // if (particleName == "opticalphoton" && ProcessName == "OpAbsorption"){ 
	//   G4cout << "Optical photon absorbed in " << currentPhysicalName << currentPhysicalName  << G4endl;
	// }



  // Check for optical photons 
  // (more outputs than photons generated since we are checking every step)
  // 
  // G4ParticleDefinition* opticalphoton = G4OpticalPhoton::OpticalPhotonDefinition();
  // const G4ParticleDefinition* particleDef = step->GetTrack()->GetDynamicParticle()->GetParticleDefinition();
  // if(particleDef == opticalphoton){
  //   G4cerr << "I am an optical photon :)" << G4endl;
  // } 
      
  

  // Optical photon post step process (the interation in the step) is always another typer of interaction, never scintillation
  // Probably because sintillation is only a generator process (?)
  // if(step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "Scintillation") {
      // G4cerr << "Process: " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
  // }


  // Check creator process of secondary optical photons
  //
  // if (Track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {  // particle is optical photon
  //       if (Track->GetParentID() > 0) {  // particle is secondary
  //         G4cerr << "Optical photon generated by: " << Track->GetCreatorProcess()->GetProcessName() << G4endl;
  //       }
  //     }

}