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
// $Id: WLSMaterials.hh 69561 2013-05-08 12:25:56Z gcosmo $
//
/// \file oinclude/WLSMaterials.hh
/// \brief Definition of the WLSMaterials class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef MyMaterials_h
#define MyMaterials_h 1

#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

class MyMaterials
{
  public:

    virtual ~MyMaterials();
    static MyMaterials* GetInstance();
    G4Material* GetMaterial(const G4String);
 
  private:
 
    MyMaterials();
    void CreateMaterials();

    static MyMaterials* fInstance;
    G4NistManager* fNistMan;

    G4Material* fVacuum;
    G4Material* fPb;
    G4Material* fW;
    G4Material* fTa;
    G4Material* fCu;
    G4Material* fPolystyrene;
    G4Material* fSi;
    
};

#endif /*Materials_h*/
