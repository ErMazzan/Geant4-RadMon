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
// $Id: B4cEventAction.hh 75215 2013-10-29 16:07:06Z gcosmo $
// 
/// \file B4cEventAction.hh
/// \brief Definition of the B4cEventAction class

#ifndef B4cEventAction_h
#define B4cEventAction_h 1

#include "G4UserEventAction.hh"

#include "B4cCalorHit.hh"
#include "B4RunAction.hh"
#include "SiPMHit.hh"

#include "globals.hh"

/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy 
/// deposit and track lengths of charged particles in Absober and Gap layers 
/// stored in the hits collections.

class G4Track;
class G4Step;
class B4RunAction;

class B4cEventAction : public G4UserEventAction
{
    
public:
  /*
  B4cEventAction();
  virtual ~B4cEventAction();
  */
  
  B4cEventAction(B4RunAction* runAction);
  virtual ~B4cEventAction();
  
  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);


  G4int nScintPhotons;
  G4int nDetectedPhotons;

private:
  // methods
  B4cCalorHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;
  SiPMHitsCollection* GetSiPMHitsCollection(G4int hcID, const G4Event* event) const;

  void PrintEventStatistics(G4double absoEdep, G4double absoTrackLength,
                            G4double gapEdep, G4double gapTrackLength) const;
  
  // data members
  G4int  FirstInt;
  G4int  fAbsHCID;
  G4int  fGapHCID;
  G4int  fScintHCID;
  G4int  fSiPMHCID;

  G4bool Scint1Flag;
  G4bool Scint2Flag;
  G4bool Scint3Flag;
  G4bool Scint4Flag;
    
  G4double Scint12Energy;
  G4double Scint34Energy;
  G4double Scint123Energy;
  G4double Scint234Energy;
  G4double Scint1234Energy;
    
  B4RunAction* fRunAct;
  B4cDetectorConstruction* fDetector;

  G4int nSiPMs;
  G4int Ndet[50]; //larger than the maximum bumber of SiPMs accepted

};
                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
