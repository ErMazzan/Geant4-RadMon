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
  nScint1Photons = 0; 
  nScint2Photons = 0; 
  nScint3Photons = 0; 
  nScint4Photons = 0; 

  for(int i=0; i<fTotalNSiPMs; i++) Ndet[i] = 0;
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{  

  // G4cout << "####  Scintillation photons this event: # Generated: " << nScintPhotons <<
  //           "\n" << nScint1Photons << " in Scint 1" <<
  //           "\n" << nScint2Photons << " in Scint 2" <<
  //           "\n" << nScint3Photons << " in Scint 3" <<
  //           "\n" << nScint4Photons << " in Scint 4" << G4endl;

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
  analysisManager->FillH1(11, ekin);


  // Some parameters
  G4double EDep_Thresh = 0.4;
  G4double ERes = 0.1;

    
  if (Scint1Hit->GetEdep() != 0) {
    G4double EDep1 = Scint1Hit->GetEdep();

    analysisManager->FillH1(0, EDep1);
    analysisManager->FillH1(98,nScint1Photons);

    G4double val1 = G4RandGauss::shoot(EDep1,EDep1*ERes);
    analysisManager->FillH1(76, val1);

    analysisManager->FillH2(6,EDep1,ekin);

    Scint1Flag = TRUE;
  }

  if (Scint2Hit->GetEdep() != 0) {
    G4double EDep2 = Scint2Hit->GetEdep();

    analysisManager->FillH1(1, EDep2);
    analysisManager->FillH1(99,nScint2Photons);

    G4double val2 = G4RandGauss::shoot(EDep2,EDep2*ERes);
    analysisManager->FillH1(77, val2);

    Scint2Flag = TRUE;
  }
    
  if (Scint3Hit->GetEdep() != 0) {
    G4double EDep3 = Scint3Hit->GetEdep();

    analysisManager->FillH1(2, EDep3);
    analysisManager->FillH1(100,nScint3Photons);

    G4double val3 = G4RandGauss::shoot(EDep3,EDep3*ERes);
    analysisManager->FillH1(78, val3);
    
    Scint3Flag = TRUE;
  }
    
  if (Scint4Hit->GetEdep() != 0) {
    G4double EDep4 = Scint4Hit->GetEdep();

    analysisManager->FillH1(3, EDep4);
    analysisManager->FillH1(101,nScint4Photons);

    G4double val4 = G4RandGauss::shoot(EDep4,EDep4*ERes);
    analysisManager->FillH1(79, val4);

    Scint4Flag = TRUE;
    // if (Scint3Hit->GetEdep() == 0){ 
    //   G4RunManager::GetRunManager()->rndmSaveThisEvent();
    //   G4cout << event->GetEventID() << G4endl;
    //   }
  }

  // Coincidences:
  
  // Coinc 1-2 and NOT 3, 4
  if ((Scint1Flag && Scint2Flag) == TRUE && (Scint3Flag || Scint4Flag) == FALSE) {
      G4double EDep1 = Scint1Hit->GetEdep();
      G4double EDep2 = Scint2Hit->GetEdep();
      Scint12Energy = EDep1 + EDep2;
      analysisManager->FillH1(4, Scint12Energy);
      analysisManager->FillH1(12, ekin);
      analysisManager->FillH1(17, EDep1);
      analysisManager->FillH1(18, EDep2);
      analysisManager->FillH2(0, EDep1, EDep2);

      analysisManager->FillH1(102, nScint1Photons);
      analysisManager->FillH1(103, nScint2Photons);

      // Energy resolution
      G4double val1 = G4RandGauss::shoot(EDep1,EDep1*ERes);
      G4double val2 = G4RandGauss::shoot(EDep2,EDep2*ERes);
      analysisManager->FillH1(80, val1);
      analysisManager->FillH1(81, val2);

      // Particle ID
      if (EDep1>EDep_Thresh && EDep2>EDep_Thresh) {
        if (EDep1<2. || EDep2<2.) {analysisManager->FillH1(89, ekin);} // p
        else if (EDep1>2.5 && EDep1<5 && EDep2>2.5 && EDep2<5){analysisManager->FillH1(90, ekin);} // a
        else {analysisManager->FillH1(91, ekin);} // gray
      }

      // PROTON CHANNELS
      if (EDep1>EDep_Thresh && EDep2>EDep_Thresh) {
        analysisManager->FillH1(8, ekin);
        analysisManager->FillNtupleDColumn(0,0,EDep1);
        analysisManager->FillNtupleDColumn(0,1,EDep2);
        analysisManager->AddNtupleRow(0);
        }

      // ALPHA CHANNELS
      if (EDep1<10 && EDep1>EDep_Thresh && EDep2<10 && EDep2>EDep_Thresh){
        if (EDep1<4 && EDep1>2 && EDep2<4 ){analysisManager->FillH1(71, ekin);}
        else if (EDep2<4 && EDep2>2 && EDep1<4 ){analysisManager->FillH1(71, ekin);}
        
        else if (EDep1>7 || EDep2>7){analysisManager->FillH1(72, ekin);}
      }

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
      G4double EDep1 = Scint1Hit->GetEdep();
      G4double EDep2 = Scint2Hit->GetEdep();
      G4double EDep3 = Scint3Hit->GetEdep();

      Scint123Energy = EDep1 + EDep2 + EDep3;
      analysisManager->FillH1(6, Scint123Energy);
      analysisManager->FillH1(14, ekin);
      analysisManager->FillH1(19, EDep1);
      analysisManager->FillH1(20, EDep2);
      analysisManager->FillH1(21, EDep3);
      analysisManager->FillH2(1, EDep1, EDep3);
      analysisManager->FillH2(2, EDep2, EDep3);

      analysisManager->FillH1(104, nScint1Photons);
      analysisManager->FillH1(105, nScint2Photons);
      analysisManager->FillH1(106, nScint3Photons);

      // Energy resolution
      G4double val1 = G4RandGauss::shoot(EDep1,EDep1*ERes);
      G4double val2 = G4RandGauss::shoot(EDep2,EDep2*ERes);
      G4double val3 = G4RandGauss::shoot(EDep3,EDep3*ERes);
      analysisManager->FillH1(82, val1);
      analysisManager->FillH1(83, val2);
      analysisManager->FillH1(84, val3);

      // Particle ID
      if (EDep1>EDep_Thresh && EDep2>EDep_Thresh && EDep3>EDep_Thresh) {
        if (EDep1<2. || EDep2<2. || EDep3<2.) {analysisManager->FillH1(92, ekin);}
        else if (EDep1>2.5 && EDep2>2.5 && EDep3>2.5){analysisManager->FillH1(93, ekin);}
        else {analysisManager->FillH1(94, ekin);}
      }

      // Proton channel
      if (EDep1>EDep_Thresh && EDep2>EDep_Thresh && EDep3>EDep_Thresh) {
        analysisManager->FillH1(9, ekin);

        analysisManager->FillNtupleDColumn(1,0,EDep1);
        analysisManager->FillNtupleDColumn(1,1,EDep2);
        analysisManager->FillNtupleDColumn(1,2,EDep3);
        analysisManager->AddNtupleRow(1);

        if (EDep3<0.7){
          if (EDep2<0.7){ analysisManager->FillH1(39, ekin); }
          else if (EDep2<1.){ analysisManager->FillH1(40, ekin); }
          else if (EDep2<1.3){ analysisManager->FillH1(41, ekin); }
          else if (EDep2<1.6){ analysisManager->FillH1(42, ekin); }
        }
        else if (EDep3<1.){
          if (EDep2<0.7){ analysisManager->FillH1(43, ekin); }
          else if (EDep2<1.){ analysisManager->FillH1(44, ekin); }
          else if (EDep2<1.3){ analysisManager->FillH1(45, ekin); }
          else if (EDep2<1.6){ analysisManager->FillH1(46, ekin); }
        }
        else if (EDep3<1.3){
          if (EDep2<0.7){ analysisManager->FillH1(47, ekin); }
          else if (EDep2<1.){ analysisManager->FillH1(48, ekin); }
          else if (EDep2<1.3){ analysisManager->FillH1(49, ekin); }
          else if (EDep2<1.6){ analysisManager->FillH1(50, ekin); }
        }
        else if (EDep3<1.6){
          if (EDep2<0.7){ analysisManager->FillH1(51, ekin); }
          else if (EDep2<1.){ analysisManager->FillH1(52, ekin); }
          else if (EDep2<1.3){ analysisManager->FillH1(53, ekin); }
          else if (EDep2<1.6){ analysisManager->FillH1(54, ekin); }
        }

        // ALPHA CHANNELS
        if (EDep2<3 && EDep2>2 && EDep3<3 && EDep3>2){ analysisManager->FillH1(73, ekin); }
        if (EDep2<8 && EDep2>5 && EDep3<8 && EDep3>5){ analysisManager->FillH1(74, ekin); }

      // if (EDep1>EDep_Thresh && EDep2>EDep_Thresh && EDep3>EDep_Thresh) {
      //   if (EDep2<0.7 && EDep3<1.5){ analysisManager->FillH1(40, ekin); }
      // }
      fRunAct->UpdateRate123(1);
      }
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
      G4double EDep1 = Scint1Hit->GetEdep();
      G4double EDep2 = Scint2Hit->GetEdep();
      G4double EDep3 = Scint3Hit->GetEdep();
      G4double EDep4 = Scint4Hit->GetEdep();

      Scint1234Energy = EDep1 + EDep2 + EDep3 + EDep4;
      // analysisManager->FillH1(8, Scint1234Energy);
      analysisManager->FillH1(7, Scint1234Energy);
      analysisManager->FillH1(16, ekin);
      analysisManager->FillH1(22, EDep1);
      analysisManager->FillH1(23, EDep2);
      analysisManager->FillH1(24, EDep3);
      analysisManager->FillH1(25, EDep4);
      analysisManager->FillH2(3, EDep1, EDep4);
      analysisManager->FillH2(4, EDep2, EDep4);
      analysisManager->FillH2(5, EDep3, EDep4);

      analysisManager->FillH1(107, nScint1Photons);
      analysisManager->FillH1(108, nScint2Photons);
      analysisManager->FillH1(109, nScint3Photons);
      analysisManager->FillH1(110, nScint4Photons);

      // Energy resolution
      G4double val1 = G4RandGauss::shoot(EDep1,EDep1*ERes);
      G4double val2 = G4RandGauss::shoot(EDep2,EDep2*ERes);
      G4double val3 = G4RandGauss::shoot(EDep3,EDep3*ERes);
      G4double val4 = G4RandGauss::shoot(EDep4,EDep4*ERes);
      analysisManager->FillH1(85, val1);
      analysisManager->FillH1(86, val2);
      analysisManager->FillH1(87, val3);
      analysisManager->FillH1(88, val4);


      // Particle ID
      if (EDep1>EDep_Thresh && EDep2>EDep_Thresh && EDep3>EDep_Thresh&& EDep4>EDep_Thresh) {
        if (EDep1<2. || EDep4<2.) {analysisManager->FillH1(95, ekin);}
        else if (EDep2>2. && EDep4>2.){analysisManager->FillH1(96, ekin);}
        else {analysisManager->FillH1(97, ekin);}
      }

      // Energy channels
      if (EDep1>EDep_Thresh && EDep2>EDep_Thresh && EDep3>EDep_Thresh && EDep4>EDep_Thresh) {
        analysisManager->FillH1(10, ekin); 

        analysisManager->FillNtupleDColumn(2,0,EDep1);
        analysisManager->FillNtupleDColumn(2,1,EDep2);
        analysisManager->FillNtupleDColumn(2,2,EDep3);
        analysisManager->FillNtupleDColumn(2,3,EDep4);
        analysisManager->AddNtupleRow(2);

        // PROTON CHANNELS
        if (EDep4<0.7){
          if (EDep1<0.7){ analysisManager->FillH1(55, ekin); }
          else if (EDep1<1.){ analysisManager->FillH1(56, ekin); }
          else if (EDep1<1.3){ analysisManager->FillH1(57, ekin); }
          else if (EDep1<1.6){ analysisManager->FillH1(58, ekin); }
        }
        else if (EDep4<1.){
          if (EDep1<0.7){ analysisManager->FillH1(59, ekin); }
          else if (EDep1<1.){ analysisManager->FillH1(60, ekin); }
          else if (EDep1<1.3){ analysisManager->FillH1(61, ekin); }
          else if (EDep1<1.6){ analysisManager->FillH1(62, ekin); }
        }
        else if (EDep4<1.3){
          if (EDep1<0.7){ analysisManager->FillH1(63, ekin); }
          else if (EDep1<1.){ analysisManager->FillH1(64, ekin); }
          else if (EDep1<1.3){ analysisManager->FillH1(65, ekin); }
          else if (EDep1<1.6){ analysisManager->FillH1(66, ekin); }
        }
        else if (EDep4<1.6){
          if (EDep1<0.7){ analysisManager->FillH1(67, ekin); }
          else if (EDep1<1.){ analysisManager->FillH1(68, ekin); }
          else if (EDep1<1.3){ analysisManager->FillH1(69, ekin); }
          else if (EDep1<1.6){ analysisManager->FillH1(70, ekin); }
        }

        // ALPHA CHANNELS

        if (EDep1<3.5 && EDep1>2.2 && EDep4<3.5 && EDep4>2.2){ analysisManager->FillH1(75, ekin); }
        
        // if (Scint1234Energy>5.5 && Scint1234Energy<7.5 ){ analysisManager->FillH1(41, ekin); }
        
        // // if (EDep1<1. && EDep1>0.5 && EDep4<1. && EDep4>0.5)
        // if (EDep1<1. && EDep4<1. )
        //   // (EDep1>2.5 && EDep1<4. && EDep4>0.4 && EDep4<1.6) ||
        //   //    (EDep4>2.5 && EDep4<4. && EDep1>0.4 && EDep1<1.6)
        //     { analysisManager->FillH1(42, ekin); }
      }

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
  
  // // Get Hits collection ID
  // if ( fSiPMHCID == -1 ) {fSiPMHCID = G4SDManager::GetSDMpointer()->GetCollectionID("SiPMHitsCollection");}

  // // Get Hits collection
  // auto SiPMHC = GetSiPMHitsCollection(fSiPMHCID, event);
  // G4int nhits = SiPMHC->entries();
  // // G4cout << "SiPM hit collection entries " << nhits << G4endl;

  // // Loop for each entry since one event generates many scintillation photons that can be detected
  // // Count the detections and add them to total sim detections for that event
  // for (int i=0; i<nhits; i++){
  //   SiPMHit *thisHit = (*SiPMHC)[i];
  //   G4int thisSiPM = thisHit->fSiPMID;
  //   Ndet[thisSiPM]++;
  // }

  // // Fill NTuple with total detections that event:
  // for (int id=0; id<fTotalNSiPMs; id++){
  //   analysisManager->FillNtupleDColumn(0,id,Ndet[id]);
  //   // G4cout << "Filling NTuple. SiPM " << id << ". Detections: "<<  Ndet[id]  << G4endl;
  // }

  // analysisManager->AddNtupleRow();

  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
