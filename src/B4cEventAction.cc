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
// $Id: B4cEventAction.cc 100946 2016-11-03 11:28:08Z gcosmo $
// 
/// \file B4cEventAction.cc
/// \brief Implementation of the B4cEventAction class

#include "B4cEventAction.hh"
#include "B4RunAction.hh"
#include "B4cCalorimeterSD.hh"
#include "B4cCalorHit.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
B4cEventAction::B4cEventAction(B4RunAction* runAction)
: fRunAct(runAction)
{}

/*
B4cEventAction::B4cEventAction()
 : G4UserEventAction(),
   fAbsHCID(-1),
   fGapHCID(-1)
{
}
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::~B4cEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHitsCollection* 
B4cEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<B4cCalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4cEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::PrintEventStatistics(
                              G4double absoEdep, G4double absoTrackLength,
                              G4double gapEdep, G4double gapTrackLength) const
{
  // print event statistics
  /*
  G4cout
     << "   Absorber: total energy: " 
     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
     << G4endl
     << "        Gap: total energy: " 
     << std::setw(7) << G4BestUnit(gapEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
     << G4endl;
 */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    // Reset23();
    Scint1Flag = FALSE;
    Scint2Flag = FALSE;
    Scint3Flag = FALSE;
    Scint4Flag = FALSE;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{  
  // Get hits collections IDs (only once)
  // David -> Added fScintHCID
    
  // G4cout << "hola:)" << G4endl;
  if ( fAbsHCID == -1 ) {
    fAbsHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
    fGapHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("GapHitsCollection");
    fScintHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("ScintillatorHitsCollection");
  }

  // Get hits collections
  auto absoHC = GetHitsCollection(fAbsHCID, event);
  auto gapHC = GetHitsCollection(fGapHCID, event);
  auto scintHC = GetHitsCollection(fScintHCID, event);

  // DAVID -> Added Hits detector for each layer
    
  // Get hit with total values
  auto absoHit = (*absoHC)[absoHC->entries()-1];
  auto gapHit = (*gapHC)[gapHC->entries()-1];
    
  auto Scint1Hit = (*scintHC)[0];
  auto Scint2Hit = (*scintHC)[1];
  auto Scint3Hit = (*scintHC)[2];
  auto Scint4Hit = (*scintHC)[3];
 
  // Print per event (modulo n)
  //
  
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    // G4cout << "---> End of event: " << eventID << G4endl;     

    PrintEventStatistics(
      absoHit->GetEdep(), absoHit->GetTrackLength(),
      gapHit->GetEdep(), gapHit->GetTrackLength());
  }
  
  
  // Fill histograms, ntuple
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
 
  // fill histograms
  // analysisManager->FillH1(0, absoHit->GetEdep());
  // analysisManager->FillH1(1, gapHit->GetEdep());
  // analysisManager->FillH1(2, absoHit->GetTrackLength());
  // analysisManager->FillH1(3, gapHit->GetTrackLength());

  // DAVID -> Added the filling for the histograms created in RunAction
  
  if (Scint1Hit->GetEdep() != 0) {
      G4cout << Scint1Hit->GetEdep() << G4endl;
    analysisManager->FillH1(4, Scint1Hit->GetEdep());
    Scint1Flag = TRUE;
  }
    
  if (Scint2Hit->GetEdep() != 0) {
      G4cout << Scint2Hit->GetEdep() << G4endl;
    analysisManager->FillH1(5, Scint2Hit->GetEdep());
    Scint2Flag = TRUE;
  }
    
  if (Scint3Hit->GetEdep() != 0) {
      G4cout << Scint3Hit->GetEdep() << G4endl;
    analysisManager->FillH1(6, Scint3Hit->GetEdep());
    Scint3Flag = TRUE;
  }
    
  if (Scint4Hit->GetEdep() != 0) {
      G4cout << Scint4Hit->GetEdep() << G4endl;
     analysisManager->FillH1(7, Scint4Hit->GetEdep());
     Scint4Flag = TRUE;
  }
    
  if ((Scint1Flag && Scint2Flag) == TRUE) {
      Scint12Energy = Scint1Hit->GetEdep() + Scint2Hit->GetEdep();
      analysisManager->FillH1(8, Scint12Energy);
      //fRunAct->UpdateRate12(1);
  }
    
  if ((Scint3Flag && Scint4Flag) == TRUE) {
      Scint34Energy = Scint3Hit->GetEdep() + Scint4Hit->GetEdep();
      analysisManager->FillH1(9, Scint34Energy);
      //fRunAct->UpdateRate34(1);
  }
    
  if ((Scint1Flag && Scint2Flag && Scint3Flag) == TRUE) {
      Scint123Energy = Scint1Hit->GetEdep() + Scint2Hit->GetEdep() + Scint3Hit->GetEdep();
      analysisManager->FillH1(10, Scint123Energy);
      //fRunAct->UpdateRate123(1);
  }

  if ((Scint2Flag && Scint3Flag && Scint4Flag) == TRUE) {
      Scint234Energy = Scint2Hit->GetEdep() + Scint3Hit->GetEdep() + Scint4Hit->GetEdep();
      analysisManager->FillH1(11, Scint234Energy);
      //fRunAct->UpdateRate234(1);
  }

  if ((Scint1Flag && Scint2Flag && Scint3Flag && Scint4Flag) == TRUE) {
      Scint1234Energy = Scint1Hit->GetEdep() + Scint2Hit->GetEdep() + Scint3Hit->GetEdep() + Scint4Hit->GetEdep();
      analysisManager->FillH1(12, Scint1234Energy);
      //fRunAct->UpdateRate1234(1);
  }

  if ((Scint1Flag && Scint2Flag) == TRUE || (Scint3Flag && Scint4Flag) == TRUE) {
      analysisManager->FillH1(13, Scint1Hit->GetEdep()+Scint2Hit->GetEdep()+Scint3Hit->GetEdep()+Scint4Hit->GetEdep());
  }

  if ((Scint1Flag && Scint2Flag) == TRUE && (Scint3Flag && Scint4Flag) == FALSE) {
      analysisManager->FillH1(14, Scint2Hit->GetEdep());
  }

  if ((Scint1Flag && Scint2Flag && Scint3Flag) == TRUE && Scint4Flag == FALSE) {
      analysisManager->FillH1(15, Scint3Hit->GetEdep());
  }

  if ((Scint1Flag && Scint2Flag) == FALSE && (Scint3Flag && Scint4Flag) == TRUE) {
      analysisManager->FillH1(16, Scint3Hit->GetEdep());
  }
    
  // fill ntuple
  analysisManager->FillNtupleDColumn(0, absoHit->GetEdep());
  analysisManager->FillNtupleDColumn(1, gapHit->GetEdep());
  analysisManager->FillNtupleDColumn(2, absoHit->GetTrackLength());
  analysisManager->FillNtupleDColumn(3, gapHit->GetTrackLength());
  analysisManager->AddNtupleRow();  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
