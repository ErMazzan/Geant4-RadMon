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
  fDetector(det),
  fTotalNSiPMs(0)
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
    
  TotalBins = 200;

  G4double E1 = 10*MeV;
  G4double Ep = 10*GeV;

  G4double E1_ch = 10*GeV;
  G4double E0_ch = 10.;
  G4int ECHbins = 30;

  G4int maxScintPhotons = 10*10000;
  G4int binsScintPhotons = 200;
    
  // 1D HISTOGRAMS
    
  // ----> EDep single Scintillator for all possible cases:

  // 0. Energy deposited in Scintillator 1
  analysisManager->CreateH1("EScint1","Edep in Scintillator 1", TotalBins, 0., E1);

  // 1. Energy deposited in Scintillator 2
  analysisManager->CreateH1("EScint2","Edep in Scintillator 2", TotalBins, 0., E1);
    
  // 2. Energy deposited in Scintillator 3
  analysisManager->CreateH1("EScint3","Edep in Scintillator 3", TotalBins, 0., E1);
    
  // 3. Energy deposited in Scintillator 4
  analysisManager->CreateH1("EScint4","Edep in Scintillator 4", TotalBins, 0., E1);
  

  // ----> ESum for coincidence cases:

  // 4. Energy deposited in both Scintillator 1 & 2 for particles that reach both scintillators
  analysisManager->CreateH1("ESum1-2", "Edep in Scint 1 & 2", TotalBins, 0., 2*E1);
    
  // 5. Energy deposited in both Scintillator 3 & 4 for particles that reach both scintillators
  analysisManager->CreateH1("ESum3-4", "Edep in Scint 3 & 4", TotalBins, 0., 2*E1);
  

  // 6. Energy deposited in Scintillators 1 & 2 & 3 for particles that reach the three scintillators
  analysisManager->CreateH1("ESum1-2-3", "Edep in Scint 1+2+3 for Coinc 1-2-3", TotalBins, 0., 3*E1);

  // 7. Energy deposited in Scintillators 1 & 2 & 3 & 4 for particles that reach the three scintillators
  analysisManager->CreateH1("ESum1-2-3-4", "Edep in Scint 1+2+3+4 for Coinc 1-2-3-4", TotalBins, 0., 4*E1);


  // ----> Initial Ekin for conincidences for edeo over threshold

  // 8. 
  analysisManager->CreateH1("Ech_Coinc12", "Ekin for Coinc1-2 | EDep>0.4MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
    
  // 9. 
  analysisManager->CreateH1("Ech_Coinc123", "Ekin for Coinc1-2-3 | EDep>0.4MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
    
  // 10.
  analysisManager->CreateH1("Ech_Coinc1234", "Ekin for Coinc1-2-3-4 | EDep>0.4MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");


  // ----> Initial Ekin for conincidences:

  // 11. Initial kinetic energy of the protons
  analysisManager->CreateH1("InitialKin", "Kinetic energy of protons", ECHbins, E0_ch, E1_ch, "none", "none", "log");

  // 12. Energy distribution of primary particles for 1-2 Coinc events
  analysisManager->CreateH1("EKinCoinc1-2", "KinEnergy for Coinc 1-2", ECHbins, E0_ch, E1_ch, "none", "none", "log");
    
  // 13. Energy distribution of primary particles for 3-4 Coinc events
  analysisManager->CreateH1("EKinCoinc3-4", "KinEnergy for Coinc 3-4", ECHbins, E0_ch, E1_ch, "none", "none", "log");
      
  // 14. Energy distribution of primary particles for 1-2-3 Coinc events
  analysisManager->CreateH1("EKinCoinc1-2-3", "KinEnergy for Coinc 1-2-3", ECHbins, E0_ch, E1_ch, "none", "none", "log");

  // 15. Energy distribution of primary particles for 2-3-4 Coinc events
  analysisManager->CreateH1("EKinCoinc2-3-4", "KinEnergy for Coinc 2-3-4", ECHbins, E0_ch, E1_ch, "none", "none", "log");
      
  // 16. Energy distribution of primary particles for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EKinCoinc1-2-3-4", "KinEnergy for Coinc 1-2-3-4", ECHbins, E0_ch, E1_ch, "none", "none", "log");
    

  // ----> EDep single Scintillator for coincidence cases:

  // 17. Energy deposited for Scintillator 1 for 1-2 Coinc events
  analysisManager->CreateH1("EDep1-Coinc1-2", "EDep in Scint 1 for Coinc 1-2", TotalBins, 0., E1);
    
  // 18. Energy deposited for Scintillator 2 for 1-2 Coinc events
  analysisManager->CreateH1("EDep2-Coinc1-2", "EDep in Scint 2 for Coinc 1-2", TotalBins, 0., E1);


  // 19. Energy deposited for Scintillator 1 for 1-2 Coinc events
  analysisManager->CreateH1("EDep1-Coinc1-2-3", "EDep in Scint 1 for Coinc 1-2-3", TotalBins, 0., E1);
    
  // 20. Energy deposited for Scintillator 2 for 1-2-3 Coinc events
  analysisManager->CreateH1("EDep2-Coinc1-2-3", "EDep in Scint 2 for Coinc 1-2-3", TotalBins, 0., E1);
  
  // 21. Energy deposited for Scintillator 3 for 1-2-3 Coinc events
  analysisManager->CreateH1("EDep3-Coinc1-2-3", "EDep in Scint 3 for Coinc 1-2-3", TotalBins, 0., E1);


  // 22. Energy deposited for Scintillator 1 for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EDep1-Coinc1-2-3-4", "EDep in Scint 1 for Coinc 1-2-3-4", TotalBins, 0., E1);
    
  // 23. Energy deposited for Scintillator 2 for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EDep2-Coinc1-2-3-4", "EDep in Scint 2 for Coinc 1-2-3-4", TotalBins, 0., E1);
    
  // 24. Energy deposited for Scintillator 3 for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EDep3-Coinc1-2-3-4", "EDep in Scint 3 for Coinc 1-2-3-4", TotalBins, 0., E1);

  // 25. Energy deposited for Scintillator 4 for 1-2-3-4 Coinc events
  analysisManager->CreateH1("EDep4-Coinc1-2-3-4", "EDep in Scint 4 for Coinc 1-2-3-4", TotalBins, 0., E1);


  // ----> Ein into scintillator for coincidence cases:
  

  // 26. Energy incident into Scintillator 1
  analysisManager->CreateH1("p_EkintoScint1", "Einput in Scint 1", TotalBins, 10., Ep, "none", "none", "log");

  // 27. Energy incident into Scintillator 2
  analysisManager->CreateH1("p_EkintoScint2", "Einput in Scint 2", TotalBins, 10., Ep, "none", "none", "log");

  // 28. Energy incident into Scintillator 3
  analysisManager->CreateH1("p_EkintoScint3", "Einput in Scint 3", TotalBins, 10., Ep, "none", "none", "log");

  // 29. Energy incident into Scintillator 4
  analysisManager->CreateH1("p_EkintoScint4", "Einput in Scint 4", TotalBins, 10., Ep, "none", "none", "log");


  // 30. Energy into Scintillator 1 for Coinc 1-2
  analysisManager->CreateH1("p_EkintoS1-Coinc1-2", "Einput in Scint 1 for Coinc 1-2", TotalBins, 10., Ep, "none", "none", "log");

  // 31. Energy into Scintillator 2 for Coinc 1-2
  analysisManager->CreateH1("p_EkintoS2-Coinc1-2", "Einput in Scint 2 for Coinc 1-2", TotalBins, 10., Ep, "none", "none", "log");


  // 32. Energy into Scintillator 1 for Coinc 1-2-3
  analysisManager->CreateH1("p_EkintoS1-Coinc1-2-3", "Einput in Scint 1 for Coinc 1-2-3", TotalBins, 10., Ep, "none", "none", "log");

  // 33. Energy into Scintillator 1 for Coinc 1-2-3
  analysisManager->CreateH1("p_EkintoS2-Coinc1-2-3", "Einput in Scint 2 for Coinc 1-2-3", TotalBins, 10., Ep, "none", "none", "log");

  // 34. Energy into Scintillator 1 for Coinc 1-2-3
  analysisManager->CreateH1("p_EkintoS3-Coinc1-2-3", "Einput in Scint 3 for Coinc 1-2-3", TotalBins, 10., Ep, "none", "none", "log");


  // 35. Energy into Scintillator 1 for Coinc 1-2-3-4
  analysisManager->CreateH1("p_EkintoS1-Coinc1-2-3-4", "Einput in Scint 1 for Coinc 1-2-3-4", TotalBins, 10., Ep, "none", "none", "log");

  // 36. Energy into Scintillator 1 for Coinc 1-2-3-4
  analysisManager->CreateH1("p_EkintoS2-Coinc1-2-3-4", "Einput in Scint 2 for Coinc 1-2-3-4", TotalBins, 10., Ep, "none", "none", "log");

  // 37. Energy into Scintillator 1 for Coinc 1-2-3-4
  analysisManager->CreateH1("p_EkintoS3-Coinc1-2-3-4", "Einput in Scint 3 for Coinc 1-2-3-4", TotalBins, 10., Ep, "none", "none", "log");

  // 38. Energy into Scintillator 1 for Coinc 1-2-3-4
  analysisManager->CreateH1("p_EkintoS4-Coinc1-2-3-4", "Einput in Scint 4 for Coinc 1-2-3-4", TotalBins, 10., Ep, "none", "none", "log");


  //
  /////  Energy channels histograms:
  //
  
  
  // 39. Energy CH
  analysisManager->CreateH1("Ech_C123_00", "Ekin for Coinc1-2-3 | EDep(S2)=[0.4, 0.7)MeV and EDep(S3)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 40. Energy CH
  analysisManager->CreateH1("Ech_C123_01", "Ekin for Coinc1-2-3 | EDep(S2)=[0.7, 1.0)MeV and EDep(S3)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 41. Energy CH
  analysisManager->CreateH1("Ech_C123_02", "Ekin for Coinc1-2-3 | EDep(S2)=[1.0, 1.3)MeV and EDep(S3)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 42. Energy CH
  analysisManager->CreateH1("Ech_C123_03", "Ekin for Coinc1-2-3 | EDep(S2)=[1.3, 1.6)MeV and EDep(S3)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 43. Energy CH
  analysisManager->CreateH1("Ech_C123_10", "Ekin for Coinc1-2-3 | EDep(S2)=[0.4, 0.7)MeV and EDep(S3)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 44. Energy CH
  analysisManager->CreateH1("Ech_C123_11", "Ekin for Coinc1-2-3 | EDep(S2)=[0.7, 1.0)MeV and EDep(S3)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 45. Energy CH
  analysisManager->CreateH1("Ech_C123_12", "Ekin for Coinc1-2-3 | EDep(S2)=[1.0, 1.3)MeV and EDep(S3)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 46. Energy CH
  analysisManager->CreateH1("Ech_C123_13", "Ekin for Coinc1-2-3 | EDep(S2)=[1.3, 1.6)MeV and EDep(S3)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 47. Energy CH
  analysisManager->CreateH1("Ech_C123_20", "Ekin for Coinc1-2-3 | EDep(S2)=[0.4, 0.7)MeV and EDep(S3)=[1.0, 1.3)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 48. Energy CH
  analysisManager->CreateH1("Ech_C123_21", "Ekin for Coinc1-2-3 | EDep(S2)=[0.7, 1.0)MeV and EDep(S3)=[1.0, 1.3)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 49. Energy CH
  analysisManager->CreateH1("Ech_C123_22", "Ekin for Coinc1-2-3 | EDep(S2)=[1.0, 1.3)MeV and EDep(S3)=[1.0, 1.3)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 50. Energy CH
  analysisManager->CreateH1("Ech_C123_23", "Ekin for Coinc1-2-3 | EDep(S2)=[1.3, 1.6)MeV and EDep(S3)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 51. Energy CH
  analysisManager->CreateH1("Ech_C123_30", "Ekin for Coinc1-2-3 | EDep(S2)=[0.4, 0.7)MeV and EDep(S3)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 52. Energy CH
  analysisManager->CreateH1("Ech_C123_31", "Ekin for Coinc1-2-3 | EDep(S2)=[0.7, 1.0)MeV and EDep(S3)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 53. Energy CH
  analysisManager->CreateH1("Ech_C123_32", "Ekin for Coinc1-2-3 | EDep(S2)=[1.0, 1.3)MeV and EDep(S3)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 54. Energy CH
  analysisManager->CreateH1("Ech_C123_33", "Ekin for Coinc1-2-3 | EDep(S2)=[1.3, 1.6)MeV and EDep(S3)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
 

  // 55. Energy CH
  analysisManager->CreateH1("Ech_C1234_00", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.4, 0.7)MeV and EDep(S4)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 56. Energy CH
  analysisManager->CreateH1("Ech_C1234_01", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.7, 1.0)MeV and EDep(S4)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 57. Energy CH
  analysisManager->CreateH1("Ech_C1234_02", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.0, 1.3)MeV and EDep(S4)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 58. Energy CH
  analysisManager->CreateH1("Ech_C1234_03", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.3, 1.6)MeV and EDep(S4)=[0.4, 0.7)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 59. Energy CH
  analysisManager->CreateH1("Ech_C1234_10", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.4, 0.7)MeV and EDep(S4)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 60. Energy CH
  analysisManager->CreateH1("Ech_C1234_11", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.7, 1.0)MeV and EDep(S4)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 61. Energy CH
  analysisManager->CreateH1("Ech_C1234_12", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.0, 1.3)MeV and EDep(S4)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 62. Energy CH
  analysisManager->CreateH1("Ech_C1234_13", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.3, 1.6)MeV and EDep(S4)=[0.7, 1.0)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 63. Energy CH
  analysisManager->CreateH1("Ech_C1234_20", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.4, 0.7)MeV and EDep(S4)=[1.0, 1.3)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 64. Energy CH
  analysisManager->CreateH1("Ech_C1234_21", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.7, 1.0)MeV and EDep(S4)=[1.0, 1.3)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 65. Energy CH
  analysisManager->CreateH1("Ech_C1234_22", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.0, 1.3)MeV and EDep(S4)=[1.0, 1.3)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 66. Energy CH
  analysisManager->CreateH1("Ech_C1234_23", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.3, 1.6)MeV and EDep(S4)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 67. Energy CH
  analysisManager->CreateH1("Ech_C1234_30", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.4, 0.7)MeV and EDep(S4)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 68. Energy CH
  analysisManager->CreateH1("Ech_C1234_31", "Ekin for Coinc1-2-3-4 | EDep(S1)=[0.7, 1.0)MeV and EDep(S4)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 69. Energy CH
  analysisManager->CreateH1("Ech_C1234_32", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.0, 1.3)MeV and EDep(S4)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 70. Energy CH
  analysisManager->CreateH1("Ech_C1234_33", "Ekin for Coinc1-2-3-4 | EDep(S1)=[1.3, 1.6)MeV and EDep(S4)=[1.3, 1.6)MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");

  // Ech alphas

  // 71. Energy CH alpha
  analysisManager->CreateH1("Ech_a_C12_0", "Ekin for Coinc1-2 | EDep(S1)=[2, 4] MeV and EDep(S2)=[0.4, 4] MeV and viceversa", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 72. Energy CH alpha
  analysisManager->CreateH1("Ech_a_C12_1", "Ekin for Coinc1-2 | EDep(S1)=[7, 10] MeV and EDep(S2)=[0.4, 10] MeV and viceversa", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 73. Energy CH alpha
  analysisManager->CreateH1("Ech_a_C123_0", "Ekin for Coinc1-2-3 | EDep(S2)=[2, 3] MeV and EDep(S3)=[2, 3] MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 74. Energy CH alpha
  analysisManager->CreateH1("Ech_a_C123_1", "Ekin for Coinc1-2-3 | EDep(S2)=[5, 8] MeV and EDep(S3)=[5, 8] MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 75. Energy CH alpha
  analysisManager->CreateH1("Ech_a_C1234", "Ekin for Coinc1-2-3-4 | EDep(S1)=[2.2, 3.5] MeV and EDep(S4)=[2.2, 3.5] MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");


  // 76. E resolution
  analysisManager->CreateH1("ERes_EDep1", "EDep in Scint1 | E resolution 10%", TotalBins, 0., E1);
  // 77. E resolution
  analysisManager->CreateH1("ERes_EDep2", "EDep in Scint2 | E resolution 10%", TotalBins, 0., E1);
  // 78. E resolution
  analysisManager->CreateH1("ERes_EDep3", "EDep in Scint3 | E resolution 10%", TotalBins, 0., E1);
  // 79. E resolution
  analysisManager->CreateH1("ERes_EDep4", "EDep in Scint4 | E resolution 10%", TotalBins, 0., E1);
  // 80. E resolution
  analysisManager->CreateH1("ERes_EDep1-Coinc1-2", "EDep in Scint1  | E resolution 10%", TotalBins, 0., E1);
  // 81. E resolution
  analysisManager->CreateH1("ERes_EDep2-Coinc1-2", "EDep in Scint2 for Coinc1-2 | E resolution 10%", TotalBins, 0., E1);
  // 82. E resolution
  analysisManager->CreateH1("ERes_EDep1-Coinc1-2-3", "EDep in Scint1 for Coinc1-2-3 | E resolution 10%", TotalBins, 0., E1);
  // 83. E resolution
  analysisManager->CreateH1("ERes_EDep2-Coinc1-2-3", "EDep in Scint2 for Coinc1-2-3 | E resolution 10%", TotalBins, 0., E1);
  // 84. E resolution
  analysisManager->CreateH1("ERes_EDep3-Coinc1-2-3", "EDep in Scint3 for Coinc1-2-3 | E resolution 10%", TotalBins, 0., E1);
  // 85. E resolution
  analysisManager->CreateH1("ERes_EDep1-Coinc1-2-3-4", "EDep in Scint1 for Coinc1-2-3-4 | E resolution 10%", TotalBins, 0., E1);
  // 86. E resolution
  analysisManager->CreateH1("ERes_EDep2-Coinc1-2-3-4", "EDep in Scint2 for Coinc1-2-3-4 | E resolution 10%", TotalBins, 0., E1);
  // 87. E resolution
  analysisManager->CreateH1("ERes_EDep3-Coinc1-2-3-4", "EDep in Scint3 for Coinc1-2-3-4 | E resolution 10%", TotalBins, 0., E1);
  // 88. E resolution
  analysisManager->CreateH1("ERes_EDep4-Coinc1-2-3-4", "EDep in Scint4 for Coinc1-2-3-4 | E resolution 10%", TotalBins, 0., E1);
  

  // PARTICLE IDENTIFICATION

  // 89. Particle ID
  analysisManager->CreateH1("ID_p_C12", "Ekin for Coinc1-2 | EDep(S1) < 2 MeV or EDep(S2) < 2 MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 90. Particle ID
  analysisManager->CreateH1("ID_a_C12", "Ekin for Coinc1-2 | EDep(S1) = [2.5, 5] MeV and EDep(S2) = [2.5, 5] MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 91. Particle ID
  analysisManager->CreateH1("ID_gray_C12", "Ekin for Coinc1-2 gray area ", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 92. Particle ID
  analysisManager->CreateH1("ID_p_C123", "Ekin for Coinc1-2-3 | EDep(S1) < 2 MeV or EDep(S2) < 2 MeV or EDep(S3) < 2 MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 93. Particle ID
  analysisManager->CreateH1("ID_a_C123", "Ekin for Coinc1-2-3 | EDep(S1) > 2.5 MeV and EDep(S2) > 2.5 MeV and EDep(S3) > 2.5 MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 94. Particle ID
  analysisManager->CreateH1("ID_gray_C123", "Ekin for Coinc1-2-3 gray area ", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 95. Particle ID
  analysisManager->CreateH1("ID_p_C1234", "Ekin for Coinc1-2-3-4 | EDep(S1) < 2 MeV or EDep(S4) < 2 MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 96. Particle ID
  analysisManager->CreateH1("ID_a_C1234", "Ekin for Coinc1-2-3-4 | EDep(S1) > 2 MeV and EDep(S4) > 2 MeV", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  // 97. Particle ID
  analysisManager->CreateH1("ID_gray_C1234", "Ekin for Coinc1-2-3-4 gray area ", ECHbins, E0_ch, E1_ch, "none", "none", "log");
  
  // 98. Scintillation photons
  analysisManager->CreateH1("ScintPhotons_S1", "Num of scintillation photons generated in Scint1", binsScintPhotons, 0, maxScintPhotons);
  // 99. Scintillation photons
  analysisManager->CreateH1("ScintPhotons_S2", "Num of scintillation photons generated in Scint2", binsScintPhotons, 0, maxScintPhotons);
  // 100. Scintillation photons
  analysisManager->CreateH1("ScintPhotons_S3", "Num of scintillation photons generated in Scint3", binsScintPhotons, 0, maxScintPhotons);
  // 101. Scintillation photons
  analysisManager->CreateH1("ScintPhotons_S4", "Num of scintillation photons generated in Scint4", binsScintPhotons, 0, maxScintPhotons);
  // 102.
  analysisManager->CreateH1("ScintPhotons_S1_C12", "Num of scint. photons generated in Scint1 for Coinc 1-2", binsScintPhotons, 0, maxScintPhotons);
  // 103.
  analysisManager->CreateH1("ScintPhotons_S2_C12", "Num of scint. photons generated in Scint2 for Coinc 1-2", binsScintPhotons, 0, maxScintPhotons);
  // 104
  analysisManager->CreateH1("ScintPhotons_S1_C123", "Num of scint. photons generated in Scint1 for Coinc 1-2-3", binsScintPhotons, 0, maxScintPhotons);
  // 105
  analysisManager->CreateH1("ScintPhotons_S2_C123", "Num of scint. photons generated in Scint2 for Coinc 1-2-3", binsScintPhotons, 0, maxScintPhotons);
  // 106
  analysisManager->CreateH1("ScintPhotons_S3_C123", "Num of scint. photons generated in Scint3 for Coinc 1-2-3", binsScintPhotons, 0, maxScintPhotons);
  // 107
  analysisManager->CreateH1("ScintPhotons_S1_C1234", "Num of scint. photons generated in Scint1 for Coinc 1-2-3-4", binsScintPhotons, 0, maxScintPhotons);
  // 108
  analysisManager->CreateH1("ScintPhotons_S2_C1234", "Num of scint. photons generated in Scint2 for Coinc 1-2-3-4", binsScintPhotons, 0, maxScintPhotons);
  // 109
  analysisManager->CreateH1("ScintPhotons_S3_C1234", "Num of scint. photons generated in Scint3 for Coinc 1-2-3-4", binsScintPhotons, 0, maxScintPhotons);
  // 110
  analysisManager->CreateH1("ScintPhotons_S4_C1234", "Num of scint. photons generated in Scint4 for Coinc 1-2-3-4", binsScintPhotons, 0, maxScintPhotons);


  // 2D HISTOGRAMS
  
  // 0. Energy deposited in coincidence channel 1-2
  analysisManager->CreateH2("EDep1-vs-EDep2-Coinc1-2", "EDep in Scint1 and Scint2 for Coinc 1-2", TotalBins, 0., E1, TotalBins, 0., E1);

  // 1. Energy deposited in coincidence channel 1-2-3
  analysisManager->CreateH2("EDep1-vs-EDep3-Coinc1-2-3", "EDep in Scint1 and Scint3 for Coinc 1-2-3", TotalBins, 0., E1, TotalBins, 0., E1);
  
  // 2. Energy deposited in coincidence channel 1-2-3
  analysisManager->CreateH2("EDep2-vs-EDep3-Coinc1-2-3", "EDep in Scint2 and Scint3 for Coinc 1-2-3", TotalBins, 0., E1, TotalBins, 0., E1);
  
  // 3. Energy deposited in coincidence channel 1-2-3-4
  analysisManager->CreateH2("EDep1-vs-EDep4-Coinc1-2-3-4", "EDep in Scint1 and Scint4 for Coinc 1-2-3-4", TotalBins, 0., E1, TotalBins, 0., E1);
  
  // 4. Energy deposited in coincidence channel 1-2-3-4
  analysisManager->CreateH2("EDep2-vs-EDep4-Coinc1-2-3-4", "EDep in Scint2 and Scint3 for Coinc 1-2-3-4", TotalBins, 0., E1, TotalBins, 0., E1);
  
  // 5. Energy deposited in coincidence channel 1-2-3-4
  analysisManager->CreateH2("EDep3-vs-EDep4-Coinc1-2-3-4", "EDep in Scint3 and Scint4 for Coinc 1-2-3-4", TotalBins, 0., E1, TotalBins, 0., E1);
  
  
  // EDep vs. Ekin

  // 6.
  analysisManager->CreateH2("EDep1-vs-EKin", " ", TotalBins, 0., E1, ECHbins, E0_ch, E1_ch, "none", "none", "none", "none", "linear", "log");








  // SiPM

  // G4int NSiPMs = fDetector->GetNofSiPMS(); // # SiPMs per scintillator
  // G4int NScints = fDetector->GetNofScint();  // # Scintillators
  // fTotalNSiPMs = NSiPMs*NScints;

  // analysisManager->CreateNtuple("Detections", "SiPM Detections");
  
  // //Add one branch per SiPM pixel
  // for (int id=0; id<fTotalNSiPMs; id++){
      
  //   std::ostringstream os;
  //   os << "SiPMid" <<id;
  //   std::string name = os.str();
  //   analysisManager->CreateNtupleDColumn(name);
  // }

  // analysisManager->FinishNtuple();


  // Creating ntuple

  // 0.
  analysisManager->CreateNtuple("C-12", "Edep for detectable coincidences 1-2");
  analysisManager->CreateNtupleDColumn("Scint1");
  analysisManager->CreateNtupleDColumn("Scint2");
  analysisManager->FinishNtuple();

  // 1.
  analysisManager->CreateNtuple("C-123", "Edep for detectable coincidences 1-2-3");
  analysisManager->CreateNtupleDColumn("Scint1");
  analysisManager->CreateNtupleDColumn("Scint2");
  analysisManager->CreateNtupleDColumn("Scint3");
  analysisManager->FinishNtuple();

  // 2.
  analysisManager->CreateNtuple("C-1234", "Edep for detectable coincidences 1-2-3-4");
  analysisManager->CreateNtupleDColumn("Scint1");
  analysisManager->CreateNtupleDColumn("Scint2");
  analysisManager->CreateNtupleDColumn("Scint3");
  analysisManager->CreateNtupleDColumn("Scint4");
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
  // G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  G4cout<<"Begin of Run Action\n";
  
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  
  // Open an output file
  //
  G4String fileName = "";

  G4cout<<"Open File\n";
  analysisManager->OpenFile(fileName);

  // G4cout<<"Here!\n";

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
