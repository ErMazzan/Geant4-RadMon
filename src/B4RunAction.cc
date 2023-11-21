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

B4RunAction::B4RunAction(B4cDetectorConstruction *det)
 : G4UserRunAction(),
  fDetector(det)
{
    
  // set printing event number per each event
  // G4RunManager::GetRunManager()->SetPrintProgress(1);     

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
  /*
  analysisManager->CreateH1("Eabs","Edep in absorber", 100, 0., 800*MeV);
  analysisManager->CreateH1("Egap","Edep in gap", 100, 0., 20*MeV);
  analysisManager->CreateH1("Labs","trackL in absorber", 100, 0., 10*cm);
  analysisManager->CreateH1("Lgap","trackL in gap", 100, 0., 10*cm);
    
   */
  // DAVID -> Added Edep for Scintillators
    
  TotalBins = 1000;
    
  // 1D HISTOGRAMS
    
  // 0. Energy deposited in Scintillator 1
  analysisManager->CreateH1("EScint1","Edep in Scintillator 1", TotalBins, 0., 50*MeV);

  // 1. Energy deposited in Scintillator 2
  analysisManager->CreateH1("EScint2","Edep in Scintillator 2", TotalBins, 0., 50*MeV);
    
  // 2. Energy deposited in Scintillator 3
  analysisManager->CreateH1("EScint3","Edep in Scintillator 3", TotalBins, 0., 50*MeV);
    
  // 3. Energy deposited in Scintillator 4
  analysisManager->CreateH1("EScint4","Edep in Scintillator 4", TotalBins, 0., 50*MeV);
  
  // 4. Energy deposited in both Scintillator 1 & 2 for particles that reach both scintillators
  analysisManager->CreateH1("ESumCoinc1-2", "Edep in Scint 1 & 2", TotalBins, 0., 100*MeV);
    
  // 5. Energy deposited in both Scintillator 3 & 4 for particles that reach both scintillators
  analysisManager->CreateH1("ESumCoinc3-4", "Edep in Scint 3 & 4", TotalBins, 0., 100*MeV);
    
  // 6. Energy deposited in Scintillators 1 & 2 & 3 for particles that reach the three scintillators
  analysisManager->CreateH1("ESumCoinc1-2-3", "Edep in Scint 1 & 2 & 3", TotalBins, 0., 100*MeV);
    
  // 7. Energy deposited in Scintillators 2 & 3 & 4 for particles that reach the three scintillators
  analysisManager->CreateH1("ESumCoinc2-3-4", "Edep in Scint 2 & 3 & 4", TotalBins, 0., 100*MeV);
    
  // 8. Energy deposited in all Scintillators 1 & 2 & 3 & 4 for particles that reach all scintillators
  analysisManager->CreateH1("ESumCoinc1-2-3-4", "Edep in Scint 1 & 2 & 3 & 4", TotalBins, 0., 100*MeV);
    
  // 9. Initial kinetic energy of the protons
  analysisManager->CreateH1("InitialKin", "Kinetic energy of protons", TotalBins, 0., 20*GeV);

  // 10. Energy distribution of primary particles for 1-2 Coinc events
  analysisManager->CreateH1("EKinCoinc1-2", "KinEnergy for Coinc 1-2", TotalBins, 0., 20*GeV);
    
  // 11. Energy distribution of primary particles for 3-4 Coinc events
  analysisManager->CreateH1("EKinCoinc3-4", "KinEnergy for Coinc 3-4", TotalBins, 0., 20*GeV);
      
  // 12. Energy distribution of primary particles for 1-2-3 Coinc events
  analysisManager->CreateH1("EKinCoinc1-2-3", "KinEnergy for Coinc 1-2-3", TotalBins, 0., 20*GeV);

  // 13. Energy distribution of primary particles for 2-3-4 Coinc events
  analysisManager->CreateH1("EKinCoinc2-3-4", "KinEnergy for Coinc 2-3-4", TotalBins, 0.,20*GeV);
      
  // 14. Energy distribution of primary particles for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EKinCoinc1-2-3-4", "KinEnergy for Coinc 1-2-3-4", TotalBins, 0., 20*GeV);
    
  // 15. Energy deposited for Scintillator 1 for 1-2 Coinc events
  analysisManager->CreateH1("EDep1-Scint1-2", "EDep for Scint 1 for Coinc 1-2", TotalBins, 0., 50*MeV);
    
  // 16. Energy deposited for Scintillator 2 for 1-2 Coinc events
  analysisManager->CreateH1("EDep2-Scint1-2", "EDep for Scint 2 for Coinc 1-2", TotalBins, 0., 50*MeV);
    
  // 17. Energy deposited for Scintillator 2 for 1-2-3 Coinc events
  analysisManager->CreateH1("EDep2-Scint1-2-3", "EDep for Scint 2 for Coinc 1-2-3", TotalBins, 0., 50*MeV);
  
  // 18. Energy deposited for Scintillator 3 for 1-2-3 Coinc events
  analysisManager->CreateH1("EDep3-Scint1-2-3", "EDep for Scint 3 for Coinc 1-2-3", TotalBins, 0., 50*MeV);
    
  // 19. Energy deposited for Scintillator 2 for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EDep2-Scint1-2-3-4", "EDep for Scint 2 for Coinc 1-2-3-4", TotalBins, 0., 50*MeV);
    
  // 20. Energy deposited for Scintillator 3 for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EDep3-Scint1-2-3-4", "EDep for Scint 3 for Coinc 1-2-3-4", TotalBins, 0., 50*MeV);

  // 2D HISTOGRAMS
  /*
  // 0. Energy deposited in coincidence channel 1-2
  analysisManager->CreateH2("EDep1-vs-EDep2-Scint1-2", "EDep for Scint1 and Scint2 for Coinc 1-2", TotalBins, 0., 50*MeV, TotalBins, 0., 50*MeV);

  // 1. Energy deposited in coincidence channel 1-2-3
  analysisManager->CreateH2("EDep2-vs-EDep3-Scint1-2-3", "EDep for Scint2 and Scint3 for Coinc 1-2-3", TotalBins, 0., 50*MeV, TotalBins, 0., 50*MeV);
  
  // 2. Energy deposited in coincidence channel 1-2-3-4
  analysisManager->CreateH2("EDep2-vs-EDep3-Scint1-2-3-4", "EDep for Scint2 and Scint3 for Coinc 1-2-3-4", TotalBins, 0., 50*MeV, TotalBins, 0., 50*MeV);
  */



  // SiPM

  G4int SiPMID;
  G4int NSiPMs = fDetector->GetNofSiPMS();
  G4int NScints = fDetector->GetNofScint();

  analysisManager->CreateNtuple("RadMon", "SiPM Detections");
  
  //Add one branch per SiPM pixel
  for (int scint=0; scint<NScints; scint++){
	  for (int sipms=0; sipms<NSiPMs; sipms++){
      SiPMID = sipms + scint*10;
	    std::ostringstream os;
	    os << "SiPMid" <<SiPMID;
	    std::string name = os.str();
	    analysisManager->CreateNtupleDColumn(name);
	  }
  }

  // analysisManager->FinishNtuple();


  // Creating ntuple
  //
  // analysisManager->CreateNtuple("B4", "Edep and TrackL");
  // analysisManager->CreateNtupleDColumn("Eabs");
  // analysisManager->CreateNtupleDColumn("Egap");
  // analysisManager->CreateNtupleDColumn("Labs");
  // analysisManager->CreateNtupleDColumn("Lgap");
  // analysisManager->FinishNtuple();
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
  G4cout<<"Begin of Run Action\n";
  
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  
  // Open an output file
  //
  G4String fileName = "";
  analysisManager->OpenFile(fileName);

  G4cout<<"Here!\n";

  fRate12 = 0;
  fRate34 = 0;
  fRate123 = 0;
  fRate234 = 0;
  fRate1234 = 0;

  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  G4cout<<"End of Run Action\n";
  // print histogram statistics
  //
  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl; 
        G4cout << "Counts scintillators 1 & 2: " << GetRate12() << G4endl;
        G4cout << "Counts scintillators 3 & 4: " << GetRate34() << G4endl;
        G4cout << "Counts scintillators 1 & 2 & 3: " << GetRate123() << G4endl;
        G4cout << "Counts scintillators 2 & 3 & 4: " << GetRate234() << G4endl;
        G4cout << "Counts scintillators 1 & 2 & 3 & 4: " << GetRate1234() << G4endl;
    }
    else {
      // G4cout << "for the local thread " << G4endl << G4endl; 
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
    
  // G4cout << "Counts scintillators 1 & 2: " << GetRate12() << G4endl;
  // G4cout << "Counts scintillators 3 & 4: " << GetRate34() << G4endl;
  // G4cout << "Counts scintillators 1 & 2 & 3: " << GetRate123() << G4endl;
  // G4cout << "Counts scintillators 2 & 3 & 4: " << GetRate234() << G4endl;
  // G4cout << "Counts scintillators 1 & 2 & 3 & 4: " << GetRate1234() << G4endl;
  
  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
