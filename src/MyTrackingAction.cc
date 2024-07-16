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
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MyTrackingAction.hh"

#include "B4cDetectorConstruction.hh"
#include "G4AnalysisManager.hh"

#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4Track.hh"
#include "G4Positron.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyTrackingAction::MyTrackingAction(B4cDetectorConstruction* det)
:G4UserTrackingAction(),detector(det)
{ }
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyTrackingAction::PreUserTrackingAction(const G4Track*)
{
  SetS1Ekin(0.);
  SetS2Ekin(0.);
  SetS3Ekin(0.);
  SetS4Ekin(0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyTrackingAction::PostUserTrackingAction(const G4Track* track)
{ 
  
  G4bool S1Flag = FALSE;
  G4bool S2Flag = FALSE;
  G4bool S3Flag = FALSE;
  G4bool S4Flag = FALSE;


  auto analysisManager = G4AnalysisManager::Instance();

  if (GetS1Ekin()>0.) {
    S1Flag = TRUE;
    analysisManager->FillH1(26, GetS1Ekin());}

  if (GetS2Ekin()>0.) {
    S2Flag = TRUE;
    analysisManager->FillH1(27, GetS2Ekin());}

  if (GetS3Ekin()>0.) {
    S3Flag = TRUE;
    analysisManager->FillH1(28, GetS3Ekin());}

  if (GetS4Ekin()>0.) {
    S4Flag = TRUE;
    analysisManager->FillH1(29, GetS4Ekin());}


  // Coincidences:

  if (S1Flag && S2Flag && !(S3Flag || S4Flag)){
    analysisManager->FillH1(30, GetS1Ekin());
    analysisManager->FillH1(31, GetS2Ekin());
  }
  
  if (S1Flag && S2Flag && S3Flag && !S4Flag){
    analysisManager->FillH1(32, GetS1Ekin());
    analysisManager->FillH1(33, GetS2Ekin());
    analysisManager->FillH1(34, GetS3Ekin());
  }

  if (S1Flag && S2Flag && S3Flag && S4Flag){
    analysisManager->FillH1(35, GetS1Ekin());
    analysisManager->FillH1(36, GetS2Ekin());
    analysisManager->FillH1(37, GetS3Ekin());
    analysisManager->FillH1(38, GetS4Ekin());
  }
  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

