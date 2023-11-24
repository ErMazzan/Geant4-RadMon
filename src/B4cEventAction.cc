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

#include "SiPMHit.hh"
#include "SiPMSD.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::B4cEventAction(B4RunAction* runAction)
: fRunAct(runAction), 
  fAbsHCID(-1), fGapHCID(-1), fScintHCID(-1), fSiPMHCID(-1),
  fTotalNSiPMs(0)
{
  fTotalNSiPMs=runAction->GetTotalNSiPMs();
}


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
  // auto hitsCollection = static_cast<B4cCalorHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
  auto hitsCollection = (B4cCalorHitsCollection*) (event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4cEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         
  return hitsCollection;
}    


SiPMHitsCollection* 
B4cEventAction::GetSiPMHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  // auto hitsCollection = static_cast<B4cCalorHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
  auto hitsCollection = (SiPMHitsCollection*) (event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4cEventAction::GetSiPMHitsCollection()",
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

void B4cEventAction::BeginOfEventAction(const G4Event* event)
{
  if (event->GetEventID()%1000000 == 0) G4cout << "<<< Begin of Event:  " << event->GetEventID() << G4endl;
  
  Scint1Flag = FALSE;
  Scint2Flag = FALSE;
  Scint3Flag = FALSE;
  Scint4Flag = FALSE;
  
  auto analysisManager = G4AnalysisManager::Instance();
  
  FirstInt = 0;
  fRunAct->SetFirstIntFlag(FirstInt);
  
  // G4double ekin = fRunAct->GetKinEnergy();
  // G4cerr << "Setting kinetic energy " << ekin << G4endl;
  //analysisManager->FillH1(9, ekin);    

  nDetectedPhotons = 0;
  nScintPhotons = 0; 
  for(int i=0; i<fTotalNSiPMs; i++) Ndet[i] = 0;
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{  

  // G4cout << "####  Scintillation photons this event: # Generated: " << nScintPhotons <<
  //           " # Detected: " << nDetectedPhotons << G4endl;

  // Get hits collections IDs (only once)
  // David -> Added fScintHCID
    
  // G4cout << "hola:)" << G4endl;
  if ( fScintHCID == -1 ) {

    // fAbsHCID 
    //   = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
    // fGapHCID 
    //   = G4SDManager::GetSDMpointer()->GetCollectionID("GapHitsCollection");
    fScintHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("ScintillatorHitsCollection");
  }

  // Get hits collections
  // auto absoHC = GetHitsCollection(fAbsHCID, event);
  // auto gapHC = GetHitsCollection(fGapHCID, event);
  auto scintHC = GetHitsCollection(fScintHCID, event);

  // DAVID -> Added Hits detector for each layer
    
  // Get hit with total values
  // auto absoHit = (*absoHC)[absoHC->entries()-1];
  // auto gapHit = (*gapHC)[gapHC->entries()-1];
    
  auto Scint1Hit = (*scintHC)[0];
  auto Scint2Hit = (*scintHC)[1];
  auto Scint3Hit = (*scintHC)[2];
  auto Scint4Hit = (*scintHC)[3];
 
  // Print per event (modulo n)
  //
  
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();


  // if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    // G4cout << "---> End of event: " << eventID << G4endl;     

    // PrintEventStatistics(
    //   absoHit->GetEdep(), absoHit->GetTrackLength(),
    //   gapHit->GetEdep(), gapHit->GetTrackLength());
  // }
  
  
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
  
  G4double ekin = fRunAct->GetKinEnergy();
  analysisManager->FillH1(9, ekin);

    
  if (Scint1Hit->GetEdep() != 0) {
    analysisManager->FillH1(0, Scint1Hit->GetEdep());
    Scint1Flag = TRUE;
  }

  if (Scint2Hit->GetEdep() != 0) {
    analysisManager->FillH1(1, Scint2Hit->GetEdep());
    Scint2Flag = TRUE;
  }
    
  if (Scint3Hit->GetEdep() != 0) {
    analysisManager->FillH1(2, Scint3Hit->GetEdep());
    Scint3Flag = TRUE;
  }
    
  if (Scint4Hit->GetEdep() != 0) {
    analysisManager->FillH1(3, Scint4Hit->GetEdep());
    Scint4Flag = TRUE;
  }


  // Coincidences:
  
  // Coinc 1-2 and NOT 3, 4
  if ((Scint1Flag && Scint2Flag) == TRUE && (Scint3Flag || Scint4Flag) == FALSE) {
      Scint12Energy = Scint1Hit->GetEdep() + Scint2Hit->GetEdep();
      analysisManager->FillH1(4, Scint12Energy);
      analysisManager->FillH1(12, ekin);
      analysisManager->FillH1(17, Scint1Hit->GetEdep());
      analysisManager->FillH1(18, Scint2Hit->GetEdep());
      analysisManager->FillH2(0, Scint1Hit->GetEdep(), Scint2Hit->GetEdep());
      fRunAct->UpdateRate12(1);
  }
  
  // Coinc 3-4 and NOT 1, 2
  if ((Scint3Flag && Scint4Flag) == TRUE && (Scint1Flag || Scint2Flag) == FALSE) {
      Scint34Energy = Scint3Hit->GetEdep() + Scint4Hit->GetEdep();
      analysisManager->FillH1(5, Scint34Energy);
      analysisManager->FillH1(13, ekin);
      fRunAct->UpdateRate34(1);
  }
  
  // Coinc 1-2-3 and NOT 4
  if ((Scint1Flag && Scint2Flag && Scint3Flag) == TRUE && Scint4Flag == FALSE) {
      Scint123Energy = Scint1Hit->GetEdep() + Scint2Hit->GetEdep() + Scint3Hit->GetEdep();
      analysisManager->FillH1(6, Scint123Energy);
      analysisManager->FillH1(14, ekin);
      analysisManager->FillH1(19, Scint1Hit->GetEdep());
      analysisManager->FillH1(20, Scint2Hit->GetEdep());
      analysisManager->FillH1(21, Scint3Hit->GetEdep());
      analysisManager->FillH2(1, Scint1Hit->GetEdep(), Scint3Hit->GetEdep());
      analysisManager->FillH2(2, Scint2Hit->GetEdep(), Scint3Hit->GetEdep());
      fRunAct->UpdateRate123(1);
  }

  // Coinc 2-3-4 and NOT 1
  if ((Scint2Flag && Scint3Flag && Scint4Flag) == TRUE && Scint1Flag == FALSE) {
      // Scint234Energy = Scint2Hit->GetEdep() + Scint3Hit->GetEdep() + Scint4Hit->GetEdep();
      // analysisManager->FillH1(7, Scint234Energy);
      analysisManager->FillH1(15, ekin);
      fRunAct->UpdateRate234(1);
  }

  // Coinc 1-2-3-4
  if ((Scint1Flag && Scint2Flag && Scint3Flag && Scint4Flag) == TRUE) {
      Scint1234Energy = Scint1Hit->GetEdep() + Scint2Hit->GetEdep() + Scint3Hit->GetEdep() + Scint4Hit->GetEdep();
      // analysisManager->FillH1(8, Scint1234Energy);
      analysisManager->FillH1(7, Scint1234Energy);
      analysisManager->FillH1(16, ekin);
      analysisManager->FillH1(22, Scint1Hit->GetEdep());
      analysisManager->FillH1(23, Scint2Hit->GetEdep());
      analysisManager->FillH1(24, Scint3Hit->GetEdep());
      analysisManager->FillH1(25, Scint4Hit->GetEdep());
      analysisManager->FillH2(3, Scint1Hit->GetEdep(), Scint4Hit->GetEdep());
      analysisManager->FillH2(4, Scint2Hit->GetEdep(), Scint4Hit->GetEdep());
      analysisManager->FillH2(5, Scint3Hit->GetEdep(), Scint4Hit->GetEdep());
      fRunAct->UpdateRate1234(1);
  }

  /*
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
  */

    
  // fill ntuple
  // analysisManager->FillNtupleDColumn(0, absoHit->GetEdep());
  // analysisManager->FillNtupleDColumn(1, gapHit->GetEdep());
  // analysisManager->FillNtupleDColumn(2, absoHit->GetTrackLength());
  // analysisManager->FillNtupleDColumn(3, gapHit->GetTrackLength());
  // analysisManager->AddNtupleRow();  


  ////
  //  sipms SD hits info
  ////
  
  // Get Hits collection ID
  if ( fSiPMHCID == -1 ) {fSiPMHCID = G4SDManager::GetSDMpointer()->GetCollectionID("SiPMHitsCollection");}

  // Get Hits collection
  auto SiPMHC = GetSiPMHitsCollection(fSiPMHCID, event);
  G4int nhits = SiPMHC->entries();
  // G4cout << "SiPM hit collection entries " << nhits << G4endl;

  // Loop for each entry since one event generates many scintillation photons that can be detected
  // Count the detections and add them to total sim detections for that event
  for (int i=0; i<nhits; i++){
    SiPMHit *thisHit = (*SiPMHC)[i];
    G4int thisSiPM = thisHit->fSiPMID;
    Ndet[thisSiPM]++;
  }

  // Fill NTuple with total detections that event:
  for (int id=0; id<fTotalNSiPMs; id++){
    analysisManager->FillNtupleDColumn(0,id,Ndet[id]);
    // G4cout << "Filling NTuple. SiPM " << id << ". Detections: "<<  Ndet[id]  << G4endl;
  }

  analysisManager->AddNtupleRow();

  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
