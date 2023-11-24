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
// $Id: B4RunAction.hh 74265 2013-10-02 14:41:20Z gcosmo $
// 
/// \file B4RunAction.hh
/// \brief Definition of the B4RunAction class

#ifndef B4RunAction_h
#define B4RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "B4cDetectorConstruction.hh"

class G4Run;

/// Run action class
///
/// It accumulates statistic and computes dispersion of the energy deposit 
/// and track lengths of charged particles with use of analysis tools:
/// H1D histograms are created in BeginOfRunAction() for the following 
/// physics quantities:
/// - Edep in absorber
/// - Edep in gap
/// - Track length in absorber
/// - Track length in gap
/// The same values are also saved in the ntuple.
/// The histograms and ntuple are saved in the output file in a format
/// accoring to a selected technology in B4Analysis.hh.
///
/// In EndOfRunAction(), the accumulated statistic and computed 
/// dispersion is printed.
///

class B4RunAction : public G4UserRunAction
{
  public:
    B4RunAction(B4cDetectorConstruction* );
    virtual ~B4RunAction();
    
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);


    G4int fTotalNSiPMs; //larger than the maximum bumber of SiPMs accepted
    G4int GetTotalNSiPMs() {return fTotalNSiPMs;}
    
    G4int fRate12;
    void UpdateRate12(G4int count) { fRate12 = fRate12 + count; }
    G4int GetRate12() { return fRate12; }
    
    G4int fRate34;
    void UpdateRate34(G4int count) { fRate34 = fRate34 + count; }
    G4int GetRate34() { return fRate34; }
    
    G4int fRate123;
    void UpdateRate123(G4int count) { fRate123 = fRate123 + count; }
    G4int GetRate123() { return fRate123; }
    
    G4int fRate234;
    void UpdateRate234(G4int count) { fRate234 = fRate234 + count; }
    G4int GetRate234() { return fRate234; }
    
    G4int fRate1234;
    void UpdateRate1234(G4int count) { fRate1234 = fRate1234 + count; }
    G4int GetRate1234() { return fRate1234; }
    
    G4double fKinEnergy;
    void SetKinEnergy(G4double energy)        { fKinEnergy = energy; }
    G4double GetKinEnergy()        { return fKinEnergy; }
    
    G4int FirstInt;
    void SetFirstIntFlag(G4int flag) { FirstInt = flag; }
    G4int GetFirstIntFlag() { return FirstInt; }
    
  private:
    G4int TotalBins;

    B4cDetectorConstruction*   fDetector;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

