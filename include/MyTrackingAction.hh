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
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class B4cDetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MyTrackingAction : public G4UserTrackingAction {

  public:  
    MyTrackingAction(B4cDetectorConstruction*);
    
   ~MyTrackingAction() {};
   
    void  PreUserTrackingAction(const G4Track*);   
    void PostUserTrackingAction(const G4Track*);


    inline void SetS1Ekin(G4double val)  { S1Ekin = val; }
    inline void SetS2Ekin(G4double val)  { S2Ekin = val; }
    inline void SetS3Ekin(G4double val)  { S3Ekin = val; }
    inline void SetS4Ekin(G4double val)  { S4Ekin = val; }

    inline G4double GetS1Ekin(){return S1Ekin;}
    inline G4double GetS2Ekin(){return S2Ekin;}
    inline G4double GetS3Ekin(){return S3Ekin;}
    inline G4double GetS4Ekin(){return S4Ekin;}
    
  private:
  
    G4double S1Ekin;
    G4double S2Ekin;
    G4double S3Ekin;
    G4double S4Ekin;

    B4cDetectorConstruction*   detector;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
