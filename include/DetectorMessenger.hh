//
//  DetectorMessenger.h
//  
//
//  Created by David Mazzanti on 8/5/23.
//

#ifndef DetectorMessenger_h
#define DetectorMessenger_h

#include "G4UImessenger.hh"
#include "globals.hh"

class B4cDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger: public G4UImessenger
{
  public:
  
    DetectorMessenger(B4cDetectorConstruction* );
   ~DetectorMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
  
    B4cDetectorConstruction*   fDetector;
    
    G4UIdirectory*             fRadMon;
    G4UIdirectory*             fDetDir;
    G4UIcmdWithADoubleAndUnit* fStartPhiAngleCmd;
    G4UIcmdWithADoubleAndUnit* fAperturePhiAngleCmd;
    G4UIcmdWithADoubleAndUnit* fStartThetaAngleCmd;
    G4UIcmdWithADoubleAndUnit* fApertureThetaAngleCmd;
    
    G4UIcmdWithoutParameter* UpdateCmd;
    
};

#endif /* DetectorMessenger_h */
