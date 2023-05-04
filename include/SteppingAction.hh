//
//  SteppingAction.hh
//  
//
//  Created by David Mazzanti on 2/5/23.
//

#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"
#include "B4RunAction.hh"
#include "B4cEventAction.hh"

class B4cDetectorConstruction;
class G4Track;
class G4Step;

class SteppingAction : public G4UserSteppingAction
{
    public:
      /*
      SteppingAction(B4cDetectorConstruction* detConstruction);
      ~SteppingAction() override;
      */
    
      SteppingAction(B4RunAction* );
      ~SteppingAction();
      
      void UserSteppingAction(const G4Step* step) override;

    private:
      B4cDetectorConstruction* fDetConstruction;
      B4cEventAction* fEventAct;
      B4RunAction* fRunAct;
    
      G4int FirstIntStep;
};

#endif
