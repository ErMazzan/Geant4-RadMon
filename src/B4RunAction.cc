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
// $Id: B4RunAction.cc 100946 2016-11-03 11:28:08Z gcosmo $
//
/// \file B4RunAction.cc
/// \brief Implementation of the B4RunAction class

#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::B4RunAction()
 : G4UserRunAction()
{
    
  fRate23 = 0;
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);     

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories 
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple
  //
  
  // Creating histograms
  analysisManager->CreateH1("Eabs","Edep in absorber", 100, 0., 800*MeV);
  analysisManager->CreateH1("Egap","Edep in gap", 100, 0., 20*MeV);
  analysisManager->CreateH1("Labs","trackL in absorber", 100, 0., 10*cm);
  analysisManager->CreateH1("Lgap","trackL in gap", 100, 0., 10*cm);
    
  // DAVID -> Added Edep for Scintillators
    
  // 4. Energy deposited in Scintillator 1
  analysisManager->CreateH1("EScint1","Edep in Scintillator 1", 1000, 0., 5000*keV);
    
  // 5. Energy deposited in Scintillator 2
  analysisManager->CreateH1("EScint2","Edep in Scintillator 2", 1000, 0., 5000*keV);
    
  // 6. Energy deposited in Scintillator 3
  analysisManager->CreateH1("EScint3","Edep in Scintillator 3", 1000, 0., 5000*keV);
    
  // 7. Energy deposited in Scintillator 4
  analysisManager->CreateH1("EScint4","Edep in Scintillator 4", 1000, 0., 5000*keV);
  
  // 8. Energy deposited in both Scintillator 1 & 2 for particles that reach both scintillators
  analysisManager->CreateH1("ES1&2", "Edep in Scint 1 & 2", 1000, 0., 5000*keV);
    
  // 9. Energy deposited in both Scintillator 3 & 3 for particles that reach both scintillators
  analysisManager->CreateH1("ES3&4", "Edep in Scint 3 & 4", 1000, 0., 5000*keV);
    
  // 10. Energy deposited in Scintillators 1 & 2 & 3 for particles that reach the three scintillators
  analysisManager->CreateH1("ES1&2&3", "Edep in Scint 1 & 2 & 3", 1000, 0., 5000*keV);
    
  // 11. Energy deposited in Scintillators 2 & 3 & 4 for particles that reach the three scintillators
  analysisManager->CreateH1("ES2&3&4", "Edep in Scint 2 & 3 & 4", 1000, 0., 5000*keV);
    
  // 12. Energy deposited in all Scintillators 1 & 2 & 3 & 4 for particles that reach all scintillators
  analysisManager->CreateH1("ES1&2&3&4", "Edep in Scint 3 & 4", 1000, 0., 5000*keV);

  // 13. Energy deposited in all Scintillators 1 & 2 & 3 & 4 for all coincidence events
  analysisManager->CreateH1("ESumCoinc", "Edep in All scintillators", 1000, 0., 5000*keV);

  // 14. Energy deposited in Scintillator 2 for 1-2 Coinc events
  analysisManager->CreateH1("E2Coinc1-2", "Edep in Scint2 for Coinc 1-2", 1000, 0., 5000*keV);

  // 15. Energy deposited in Scintillator 3 for 1-2-3 Coinc events
  analysisManager->CreateH1("E3Coinc1-2-3", "Edep in Scint3 for Coinc 1-2-3", 1000, 0., 5000*keV);

  // 16. Energy deposited in Scintillator 3 for 3-4 Coinc events
  analysisManager->CreateH1("E3Coinc3-4", "Edep in Scint3 for Coinc 3-4", 1000, 0., 5000*keV);



  // Creating ntuple
  //
  analysisManager->CreateNtuple("B4", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Egap");
  analysisManager->CreateNtupleDColumn("Labs");
  analysisManager->CreateNtupleDColumn("Lgap");
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::~B4RunAction()
{
  delete G4AnalysisManager::Instance();  
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "";
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  //
  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl; 
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl; 
    }
      
    /*
    
    G4cout << " EAbs : mean = " 
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;
    
    G4cout << " EGap : mean = " 
       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;
    
    G4cout << " LAbs : mean = " 
      << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;

    G4cout << " LGap : mean = " 
      << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") << G4endl;
     */
  }
    
  G4cout << "Counts scintillators 1 & 2: " << analysisManager->GetH1(8)->entries() << G4endl;
  G4cout << "Counts scintillators 3 & 4: " << analysisManager->GetH1(9)->entries() << G4endl;
  G4cout << "Counts scintillators 1 & 2 & 3: " << analysisManager->GetH1(10)->entries() << G4endl;
  G4cout << "Counts scintillators 2 & 3 & 4: " << analysisManager->GetH1(11)->entries() << G4endl;
  G4cout << "Counts scintillators 1 & 2 & 3 & 4: " << analysisManager->GetH1(12)->entries() << G4endl;
  
  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
