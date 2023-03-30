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
// $Id: B4cDetectorConstruction.cc 101905 2016-12-07 11:34:39Z gunter $
// 
/// \file B4cDetectorConstruction.cc
/// \brief Implementation of the B4cDetectorConstruction class

#include "B4cDetectorConstruction.hh"
#include "B4cCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4cDetectorConstruction::fMagFieldMessenger = 0; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::B4cDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true),
   fNofLayers(-1)
{
	fNofLayers = 4;
	
    // DAVID -> Changed geometry of the absorbers: [0] 2 mm -> 10 mm ; [2] 50 mm -> 40 mm
	absoThickness[0] = 10.*mm;
	absoThickness[1] = 20.*mm;
	absoThickness[2] = 40.*mm;
	
	scintThickness= 3.*mm;
	caloSizeXY = 20.*mm;
	
	shieldThickness= 0.*mm;
	shieldSizeXY = 40. *mm;
	shieldZ = 100.* mm;
	worldSize = 50.*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::~B4cDetectorConstruction()
{ 
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::DefineMaterials()
{ 
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_Ta");
  nistManager->FindOrBuildMaterial("G4_Cu");
  // DAVID -> Added Tungsten material
  nistManager->FindOrBuildMaterial("G4_W");
  
  // Polystirene
  // G4_POLYSTYRENE       dty = 1.06     I (eV) = 68.7, C8H8
  nistManager->FindOrBuildMaterial("G4_POLYSTYRENE");
  
  // Vacuum
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::DefineVolumes()
{
  // Get total length of calorimeter and position of its components
  G4double totalAbsZ = 0;
  
  for (int i=0; i<fNofLayers-1; i++){
	  totalAbsZ += absoThickness[i];
  }
  
  G4double totalDetZ = fNofLayers*scintThickness + totalAbsZ;
  
  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto shieldMaterial = G4Material::GetMaterial("G4_Cu");
  // auto shieldMaterial = G4Material::GetMaterial("G4_W");
  auto absoMaterial = G4Material::GetMaterial("G4_Ta");
  auto scintMaterial = G4Material::GetMaterial("G4_POLYSTYRENE");
  
  
  if ( ! defaultMaterial || ! shieldMaterial || ! absoMaterial || ! scintMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }  
   
  //     
  // World
  //
  G4VSolid* worldS = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldS, defaultMaterial,"WorldLV");                            
  G4VPhysicalVolume* worldPV = new G4PVPlacement(0, // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume                         
                 "WorldPV",        // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
                    
  // Cuper shield
  G4VSolid* shieldS = new G4Box("Shield", shieldSizeXY/2., shieldSizeXY/2., shieldZ/2.);
  G4LogicalVolume* shieldLV = new G4LogicalVolume(shieldS, shieldMaterial, "ShieldLV");
  G4VPhysicalVolume* shieldPV = new G4PVPlacement(0, G4ThreeVector(), shieldLV, "ShieldPV", worldLV, false, 0, fCheckOverlaps);
  
  // Vacuum between shield and scintillators
  G4VSolid* vacuumS = new G4Box("Vacuum", shieldSizeXY/2.-shieldThickness, shieldSizeXY/2.-shieldThickness, shieldZ/2.-shieldThickness);
  G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumS, defaultMaterial, "VacuumLV");
  G4VPhysicalVolume* vacuumPV = new G4PVPlacement(0, G4ThreeVector(), vacuumLV,"VacuumPV", shieldLV, false, 0, fCheckOverlaps);
  
  // Calo that joins all detectors made of default material
  G4VSolid* caloS = new G4Box("Calo", caloSizeXY/2., caloSizeXY/2., totalDetZ/2.);
  G4LogicalVolume* caloLV = new G4LogicalVolume(caloS, defaultMaterial, "CaloLV");
  G4VPhysicalVolume* caloPV = new G4PVPlacement(0, G4ThreeVector(), caloLV,"CaloPV", vacuumLV, false, 0, fCheckOverlaps);
  
  // Scintillators
  G4VSolid* scintS = new G4Box("Scintillator", caloSizeXY/2., caloSizeXY/2., scintThickness/2.);
  G4LogicalVolume* scintLV = new G4LogicalVolume( scintS, scintMaterial, "ScintillatorLV");
  G4VPhysicalVolume* scintPV;
  G4double posDetEle;
  G4double accumulatedAbsTh=0.;
  
  for (int i=0; i<fNofLayers; i++){
	if (i>0) accumulatedAbsTh += absoThickness[i-1];
	posDetEle = totalDetZ/2 - (1+2*i)*scintThickness/2. - accumulatedAbsTh;
	scintPV = new G4PVPlacement( 0, G4ThreeVector(0., 0., posDetEle), scintLV, "ScintillatorPV", caloLV, false, i, fCheckOverlaps);
  }
  
  //Absorbers
  G4VSolid* absoS[fNofLayers-1];
  G4LogicalVolume* absoLV[fNofLayers-1];
  G4VPhysicalVolume* absoPV[fNofLayers-1];
  
  accumulatedAbsTh=0.;
  char str[1024];
  
  for (int i=0; i<fNofLayers-1; i++){
	  
	  
	  sprintf(str, "Absorber%d\n",i);
	  absoS[i] = new G4Box(str, caloSizeXY/2., caloSizeXY/2., absoThickness[i]/2.);
	  
	  sprintf(str, "AbsorberLV%d\n",i);
	  absoLV[i] = new G4LogicalVolume( absoS[i], absoMaterial, str);
	  
	  accumulatedAbsTh += absoThickness[i];
	  posDetEle = totalDetZ/2 - (1+i)*scintThickness - accumulatedAbsTh + absoThickness[i]/2.;
	  absoPV[i] = new G4PVPlacement( 0, G4ThreeVector(0., 0., posDetEle), absoLV[i], "AbsorberPV", caloLV, false, i, fCheckOverlaps);
  }
	   
  
 /*
  //
  // print parameters
  //
  G4cout
    << G4endl 
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << fNofLayers << " layers of: [ "
    << absoThickness/mm << "mm of " << absorberMaterial->GetName() 
    << " + "
    << scinThickness/mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;
  
  */
  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  
  G4VisAttributes* Att_Green = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  G4VisAttributes* Att_Gray= new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
  G4VisAttributes* Att_Blue= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  G4VisAttributes* Att_White= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  G4VisAttributes* Att_Red= new G4VisAttributes(G4Colour(1.0,0.,0.));
  G4VisAttributes* Att_Yellow= new G4VisAttributes(G4Colour(1.0,1.0,0.));
  G4VisAttributes* Att_Extra= new G4VisAttributes(G4Colour(0.,0.7,0.3));

  scintLV->SetVisAttributes(Att_Blue);
  shieldLV->SetVisAttributes(Att_Green);
  vacuumLV->SetVisAttributes(Att_Green);
  for (int i=0; i<fNofLayers-1; i++) absoLV[i]->SetVisAttributes(Att_Gray);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

    
  // DAVID -> In order to follow how to know how Sensitive Detector and HitsCollection work, check this lecture:
  // https://agenda.infn.it/event/5981/sessions/10089/attachments/43814/52014/SensitiveDetector_alghero2013.pdf
    
  // 
  // Sensitive detectors
  //
  char str[1024];
  auto absoSD 
    = new B4cCalorimeterSD("AbsorberSD", "AbsorberHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD);
    
  for (int i=0; i<fNofLayers-1; i++){
	  sprintf(str, "AbsorberLV%d\n",i);
	  SetSensitiveDetector(str,absoSD);
  }

  auto gapSD 
    = new B4cCalorimeterSD("GapSD", "GapHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(gapSD);
  SetSensitiveDetector("ScintillatorLV",gapSD);
    
  // DAVID -> Added a new SensitiveDetector object
    
  auto ScinSD
    = new B4cCalorimeterSD("ScinSD", "ScintillatorHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(ScinSD);
    SetSensitiveDetector("ScintillatorLV",ScinSD);

  // 
  // Magnetic field
  //
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
    
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
