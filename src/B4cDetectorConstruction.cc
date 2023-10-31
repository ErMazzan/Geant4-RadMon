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
#include "DetectorMessenger.hh"

#include "B4cCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "MyMaterials.hh"
#include "G4Material.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"

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
   fNofLayers(-1),
   fMaterials(NULL)
{
	fNofLayers = 4;
	
    // DAVID -> Changed geometry of the absorbers: [0] 2 mm -> 10 mm ; [2] 50 mm -> 40 mm
	absoThickness[0] = 10.*mm;
	absoThickness[1] = 25.*mm;
	absoThickness[2] = 35.*mm;
	
	scintThickness= 3.*mm;
	caloSizeXY = 20.*mm;
	
	shieldThickness= 6.*mm;
	shieldSizeXY = 40. *mm;
	shieldZ = 100.* mm;
	worldSize = 50.*cm;
    
  fDetectorMessenger = new DetectorMessenger(this);
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::~B4cDetectorConstruction()
{
  if (fDetectorMessenger) delete fDetectorMessenger;
	if (fMaterials)         delete fMaterials;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::Construct()
{
  // Define materials 
  // DefineMaterials();
  fMaterials = MyMaterials::GetInstance();
  
  // Define volumes
  return DefineVolumes();
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
  auto defaultMaterial = FindMaterial("fVacuum");
  // auto shieldMaterial = FindMaterial("fCu");
  auto shieldMaterial = FindMaterial("fVacuum");
  auto absoMaterial = FindMaterial("fW");
  // auto absoMaterial = FindMaterial("fTa");
  auto scintMaterial = FindMaterial("fPolystyrene");
  auto sipmMaterial = FindMaterial("fEpoxy");
  auto couplingMaterial = FindMaterial("fSilicone");
  
  
  
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
  /*
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
  */

  // Scintillators
  G4VSolid* scintS = new G4Box("Scintillator", caloSizeXY/2., caloSizeXY/2., scintThickness/2.);
  G4LogicalVolume* scintLV = new G4LogicalVolume(scintS, scintMaterial, "ScintillatorLV");
  G4VPhysicalVolume* scintPV = new G4PVPlacement( 0, G4ThreeVector(0., 0., 0.), scintLV, "ScintillatorPV", worldLV, false, 0, fCheckOverlaps);



// OpCoupling
	// G4Box* solidOpCoupling = new G4Box("OpCoupling", (sqrt(NSiPMs)*(SiPMsize+SiPMgap)-SiPMgap)/2., (sqrt(NSiPMs)*(SiPMsize+SiPMgap)-SiPMgap)/2., OpCouplThickness/2.);
	// G4LogicalVolume* logicOpCoupling = new G4LogicalVolume(solidOpCoupling,FindMaterial(tempMaterial),"OpCoupling");
	// G4VPhysicalVolume* physiOpCoupling = new G4PVPlacement(0,positionOpCoupling,"OpCoupling",logicOpCoupling,physiWorld,false,0);

	// SiPM
  G4double SiPMsize = 1.3*mm;
  G4double SiPMThickness = 0.3*mm;

    //Create a Rotation Matrix
  G4RotationMatrix* Rotation = new G4RotationMatrix();
  Rotation->rotateX(0*deg);
  Rotation->rotateY(90*deg);
  Rotation->rotateZ(0*deg);

  G4VSolid* sipmS = new G4Box("SiPM_SV", SiPMsize/2., SiPMsize/2., SiPMThickness/2.);
  G4LogicalVolume* sipmLV = new G4LogicalVolume(sipmS, scintMaterial, "SiPM_LV");
  G4VPhysicalVolume* sipmPV = new G4PVPlacement(Rotation, G4ThreeVector(caloSizeXY/2.+SiPMThickness/2., 0., 0.), sipmLV, "SiPM_PV", scintLV, false, 0, fCheckOverlaps);

  
  
	//------------------------------------------------------
  // Surfaces and boundary processes
	//------------------------------------------------------

  // Scintillator - OpCoupling
	
	// G4OpticalSurface* OpCouplSurface = new G4OpticalSurface("WinCouplSurface");
	// OpCouplSurface->SetType(dielectric_dielectric);
	// OpCouplSurface->SetModel(glisur);
	// OpCouplSurface->SetFinish(polished);
	
	// G4LogicalBorderSurface* WinCouplSurface = new G4LogicalBorderSurface("WinCouplSurface",sipmPV,couplingPV,OpCouplSurface);

	
	// Opticalcoupling - SiPM
	
	G4OpticalSurface* OpCouplSiPMSurface = new G4OpticalSurface("CouplSiPMSurface");
	OpCouplSiPMSurface->SetType(dielectric_dielectric);
	OpCouplSiPMSurface->SetModel(glisur);
	OpCouplSiPMSurface->SetFinish(polished);
	
	G4LogicalBorderSurface* CouplSiPMSurface = new G4LogicalBorderSurface("CouplSiPMSurface",scintPV,sipmPV,OpCouplSiPMSurface);
	

  /*
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
  */
  G4Sphere* Hemisphere = new G4Sphere("Hemisphere", 70.0*mm, 70.1*mm, fStartPhiAngle, fAperturePhiAngle, fStartThetaAngle, fApertureThetaAngle);
    
  G4LogicalVolume* HemisphereLV = new G4LogicalVolume(Hemisphere, defaultMaterial, "HemisphereLV");
    
  G4VPhysicalVolume* HemispherePV = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), HemisphereLV, "HemispherePV", worldLV, false, 0, fCheckOverlaps);
  
  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  
  /*
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
  */
    
  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::UpdateGeometry()
{
    if (!worldPV) return;
    
    
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    
    G4RunManager::GetRunManager()->DefineWorldVolume(DefineVolumes());
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    
}

void B4cDetectorConstruction::SetStartPhiAngle(G4double StartPhiAngle)
{
    fStartPhiAngle = StartPhiAngle;
}

void B4cDetectorConstruction::SetAperturePhiAngle(G4double AperturePhiAngle)
{
    fAperturePhiAngle = AperturePhiAngle;
}

void B4cDetectorConstruction::SetStartThetaAngle(G4double StartThetaAngle)
{
    fStartThetaAngle = StartThetaAngle;
}

void B4cDetectorConstruction::SetApertureThetaAngle(G4double ApertureThetaAngle)
{
    fApertureThetaAngle = ApertureThetaAngle;
}

void B4cDetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

    
  // DAVID -> In order to follow how to know how Sensitive Detector and HitsCollection work, check this lecture:
  // https://agenda.infn.it/event/5981/sessions/10089/attachments/43814/52014/SensitiveDetector_alghero2013.pdf
    
  // 
  // Sensitive detectors
  //
  /*
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
  */
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* B4cDetectorConstruction::FindMaterial(G4String name) {
  G4Material* material = fMaterials->GetMaterial(name);
  return material;
}