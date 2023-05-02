//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4cActionInitialization.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file B4cActionInitialization.cc
/// \brief Implementation of the B4cActionInitialization class

#include "B4cActionInitialization.hh"
#include "MyPrimaryGeneratorAction.hh"
#include "B4RunAction.hh"
#include "B4cEventAction.hh"
#include "SteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*
B4cActionInitialization::B4cActionInitialization()
 : G4VUserActionInitialization()
{}
*/

B4cActionInitialization::B4cActionInitialization
                            (B4cDetectorConstruction* detConstruction)
 : fDetConstruction(detConstruction)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cActionInitialization::~B4cActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cActionInitialization::BuildForMaster() const
{
    B4RunAction* runAction = new B4RunAction;
    SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cActionInitialization::Build() const
{
  SetUserAction(new MyPrimaryGeneratorAction);
    
  B4RunAction* runAction = new B4RunAction;
  SetUserAction(runAction);
    
  B4cEventAction* evtAction = new B4cEventAction(runAction);
  SetUserAction(evtAction);
    
  SetUserAction(new SteppingAction(runAction));

  // B4RunAction* runAction = new B4RunAction;
  // SetUserAction(runAction);

  // B4cEventAction* eventAction = new B4cEventAction(runAction);
  // SetUserAction(eventAction);
  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
