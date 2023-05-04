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

/*
SteppingAction::SteppingAction(B4cDetectorConstruction* detConstruction)
  : fDetConstruction(detConstruction)
{
}
*/

SteppingAction::SteppingAction(B4RunAction* runAct):  fRunAct(runAct)
{
}

SteppingAction::~SteppingAction()
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    
    // G4cout << "Hola :)" << G4endl;
    G4Track* Track = step->GetTrack();
    G4int parent_id = Track->GetParentID();
   
    const G4StepPoint* prePoint = step->GetPreStepPoint();
    const G4StepPoint* endPoint = step->GetPostStepPoint();
    const G4String currentPhysicalName = prePoint->GetPhysicalVolume()->GetName();
   
    FirstIntStep = fRunAct->GetFirstIntFlag();
    
    if (currentPhysicalName == "WorldPV" && FirstIntStep == 0 && parent_id == 0) {
       G4double kinE = Track->GetKineticEnergy();
       fRunAct->SetKinEnergy(kinE);
       FirstIntStep = 1;
       fRunAct->SetFirstIntFlag(FirstIntStep);
    }
    
    
}
