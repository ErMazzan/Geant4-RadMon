//
//  DetectorMessenger.cc
//  
//
//  Created by David Mazzanti on 8/5/23.
//

#include "B4cDetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"

#include <stdio.h>

DetectorMessenger::DetectorMessenger(B4cDetectorConstruction* Det)
:G4UImessenger(), fDetector(Det),
 fRadMon(0),
 fDetDir(0),
 fStartPhiAngleCmd(0),
 fAperturePhiAngleCmd(0),
 fStartThetaAngleCmd(0),
 fApertureThetaAngleCmd(0),
 UpdateCmd(0),
 fNumOfSiPMsCmd(0),
 fSiPMSizeCmd(0)
{
    fRadMon = new G4UIdirectory("/RadMon/");
    fRadMon->SetGuidance("Commands specific for this example");
    
    fDetDir = new G4UIdirectory("/RadMon/det/");
    fDetDir->SetGuidance("Detector Construction commands");
    
    UpdateCmd = new G4UIcmdWithoutParameter("/RadMon/Update",this);
    UpdateCmd->SetGuidance("Update geometry");
    UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
    UpdateCmd->SetGuidance("if you changed geometrical value(s).");
    UpdateCmd->AvailableForStates(G4State_Idle);
    
    fStartPhiAngleCmd = new G4UIcmdWithADoubleAndUnit("/RadMon/det/setStartPhiAngle", this);
    fStartPhiAngleCmd->SetGuidance("Start of the phi angle for the Hemisphere");
    fStartPhiAngleCmd->SetParameterName("Angle",false);
    fStartPhiAngleCmd->SetUnitCategory("Degree");
    fStartPhiAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    fStartPhiAngleCmd->SetToBeBroadcasted(false);
    
    fAperturePhiAngleCmd = new G4UIcmdWithADoubleAndUnit("/RadMon/det/setAperturePhiAngle", this);
    fAperturePhiAngleCmd->SetGuidance("Aperture of the phi angle for the Hemisphere");
    fAperturePhiAngleCmd->SetParameterName("Angle",false);
    fAperturePhiAngleCmd->SetUnitCategory("Degree");
    fAperturePhiAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    fAperturePhiAngleCmd->SetToBeBroadcasted(false);
    
    fStartThetaAngleCmd = new G4UIcmdWithADoubleAndUnit("/RadMon/det/setStartThetaAngle", this);
    fStartThetaAngleCmd->SetGuidance("Start of the theta angle for the Hemisphere");
    fStartThetaAngleCmd->SetParameterName("Angle",false);
    fStartThetaAngleCmd->SetUnitCategory("Degree");
    fStartThetaAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    fStartThetaAngleCmd->SetToBeBroadcasted(false);
    
    fApertureThetaAngleCmd = new G4UIcmdWithADoubleAndUnit("/RadMon/det/setApertureThetaAngle", this);
    fApertureThetaAngleCmd->SetGuidance("Aperture of the theta angle for the Hemisphere");
    fApertureThetaAngleCmd->SetParameterName("Angle",false);
    fApertureThetaAngleCmd->SetUnitCategory("Degree");
    fApertureThetaAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    fApertureThetaAngleCmd->SetToBeBroadcasted(false);


  /// SiPMs commands

    fSiPMsDir = new G4UIdirectory("/RadMon/SiPMs/");
    fSiPMsDir->SetGuidance("Detector Construction commands for SiPMs");

    fNumOfSiPMsCmd = new G4UIcmdWithAnInteger("/RadMon/SiPMs/setNum", this);
    fNumOfSiPMsCmd->SetGuidance("Number of SiPMs to be placed in a Scintillator");
    fNumOfSiPMsCmd->SetParameterName("#",false);
    fNumOfSiPMsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    fNumOfSiPMsCmd->SetToBeBroadcasted(false);

    fSiPMSizeCmd = new G4UIcmdWithADoubleAndUnit("/RadMon/SiPMs/setSize", this);
    fSiPMSizeCmd->SetGuidance("Size of square SiPM in mm");
    fSiPMSizeCmd->SetParameterName("Length",false);
    fSiPMSizeCmd->SetUnitCategory("mm");
    fSiPMSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    fSiPMSizeCmd->SetToBeBroadcasted(false);


}

DetectorMessenger::~DetectorMessenger()
{
  delete fRadMon;
  delete fDetDir;
  delete fSiPMsDir;

  delete fStartPhiAngleCmd;
  delete fAperturePhiAngleCmd;
  delete fStartThetaAngleCmd;
  delete fApertureThetaAngleCmd;

  delete fNumOfSiPMsCmd;
  delete fSiPMSizeCmd;

  delete UpdateCmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if ( command == UpdateCmd ) fDetector->UpdateGeometry();
      
  if ( command == fStartPhiAngleCmd )
   { fDetector->SetStartPhiAngle(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue)); }
   
  if ( command == fAperturePhiAngleCmd )
   { fDetector->SetAperturePhiAngle(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue)); }
  
  if ( command == fStartThetaAngleCmd )
   { fDetector->SetStartThetaAngle(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue)); }
    
  if ( command == fApertureThetaAngleCmd )
  { fDetector->SetApertureThetaAngle(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue)); }
   

  if ( command == fNumOfSiPMsCmd )
  { fDetector->SetNofSiPMs(G4UIcmdWithAnInteger::GetNewIntValue(newValue)); }
  
  if ( command == fSiPMSizeCmd )
  { fDetector->SetSiPMSize(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue)); }


}

