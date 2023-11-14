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
#include "G4MaterialPropertiesTable.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"

#include "G4SDManager.hh"
#include "SiPMSD.hh"

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
   fMaterials(NULL),
   fNoSiPMs(-1)
{
	fNofLayers = 4;
  fNoSiPMs = 4; //Num of SiPMs per scintillator (for now must be even number)
	
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

  // G4VSolid* shieldS = new G4Box("Shield", shieldSizeXY/2., shieldSizeXY/2., shieldZ/2.);
  // G4LogicalVolume* shieldLV = new G4LogicalVolume(shieldS, shieldMaterial, "ShieldLV");
  // G4VPhysicalVolume* shieldPV = new G4PVPlacement(0, G4ThreeVector(), shieldLV, "ShieldPV", worldLV, false, 0, fCheckOverlaps);
  
  // Vacuum between shield and scintillators
  // G4VSolid* vacuumS = new G4Box("Vacuum", shieldSizeXY/2.-shieldThickness, shieldSizeXY/2.-shieldThickness, shieldZ/2.-shieldThickness);
  // G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumS, defaultMaterial, "VacuumLV");
  // G4VPhysicalVolume* vacuumPV = new G4PVPlacement(0, G4ThreeVector(), vacuumLV,"VacuumPV", shieldLV, false, 0, fCheckOverlaps);

  // Calo that joins all detectors made of default material
  G4VSolid* caloS = new G4Box("Calo", caloSizeXY/2., caloSizeXY/2., totalDetZ/2.);
  G4LogicalVolume* caloLV = new G4LogicalVolume(caloS, defaultMaterial, "CaloLV");
  G4VPhysicalVolume* caloPV = new G4PVPlacement(0, G4ThreeVector(), caloLV,"CaloPV", worldLV, false, 0, fCheckOverlaps);
  

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
  
  
  // SiPM placement:

  G4double SiPMsize = 1.3*mm;
  G4double SiPMThickness = 1*mm;
  G4double couplingThickness = 0.2*mm;

  G4double detectorsize = SiPMsize;
  G4double detectorThickness = SiPMThickness + couplingThickness;

    // Rotation Matrix (DON'T modify if used) 
  G4RotationMatrix* RotationA = new G4RotationMatrix();
  RotationA->rotateX(0*deg);
  RotationA->rotateY(90*deg);
  RotationA->rotateZ(0*deg);

  G4RotationMatrix* RotationB = new G4RotationMatrix();
  RotationB->rotateX(0*deg);
  RotationB->rotateY(-90*deg);
  RotationB->rotateZ(0*deg);


      // Detector volume that contains SiPM + optical couping
  G4VSolid* detectorS = new G4Box("Detector", detectorsize/2., detectorsize/2., detectorThickness/2.);
  G4LogicalVolume* detectorLV = new G4LogicalVolume(detectorS, defaultMaterial, "DetectorLV");
  // G4VPhysicalVolume* detectorPV = new G4PVPlacement(Rotation,  G4ThreeVector(caloSizeXY/2.+detectorThickness/2., 0., 0.), detectorLV, "DetectorPV", scintLV, false, 0, fCheckOverlaps);
  // G4ThreeVector(caloSizeXY/2.+detectorThickness/2., 0., 0.)
  // G4ThreeVector(0.,0.,-(scintThickness+detectorThickness)/2.)

  G4VPhysicalVolume* detectorPV;
  for (int i=0; i<fNoSiPMs/2; i++){
    auto pos = G4ThreeVector(caloSizeXY/2.+detectorThickness/2., -caloSizeXY/2.+(i+1)*caloSizeXY/(fNoSiPMs/2.+1),0.);
	  detectorPV = new G4PVPlacement( RotationA, pos, detectorLV, "DetectorPV", scintLV, false, i, fCheckOverlaps);
  }
  for (int i=0; i<fNoSiPMs/2; i++){
    auto pos = G4ThreeVector(-1.*(caloSizeXY/2.+detectorThickness/2.), -caloSizeXY/2.+(i+1)*caloSizeXY/(fNoSiPMs/2.+1),0.);
	  detectorPV = new G4PVPlacement( RotationB, pos, detectorLV, "DetectorPV", scintLV, false, i+fNoSiPMs/2, fCheckOverlaps);
  }

      // OpCoupling
	G4Box* couplingS = new G4Box("OpCoupling", detectorsize/2., detectorsize/2., couplingThickness/2.);
	G4LogicalVolume* couplingLV = new G4LogicalVolume(couplingS, couplingMaterial,"OpCouplingLV");
	G4VPhysicalVolume* couplingPV = new G4PVPlacement(0, G4ThreeVector(0., 0., SiPMThickness/2.), couplingLV, "OpCouplingPV", detectorLV, false, 0, fCheckOverlaps);

	    // SiPM
  G4VSolid* sipmS = new G4Box("SiPM", SiPMsize/2., SiPMsize/2., SiPMThickness/2.);
  G4LogicalVolume* sipmLV = new G4LogicalVolume(sipmS, sipmMaterial, "SiPMLV");
  G4VPhysicalVolume* sipmPV = new G4PVPlacement(0, G4ThreeVector(0., 0., -couplingThickness/2.), sipmLV, "SiPMPV", detectorLV, false, 0, fCheckOverlaps);

  
  
	//------------------------------------------------------
  // Surfaces and boundary processes
	//------------------------------------------------------

  // For no surface absorption, set REFLECTIVITY = 1 :
  //      P(surface absorption) = 1 - REFLECTIVITY
  // To tune reclectivity-transmittance (the physical parameters not the surface property)
  // set TRANSMITTANCE parameter in [0., 1.]
  //      R = 1 - TRANSMITTANCE
  //      T = TRANSMITTANCE
  // see Peculiarities in the Simulation of Optical Physics: https://arxiv.org/pdf/1612.05162.pdf 

  std::vector<G4double> energy = { 1.2915 * eV, 5.166 * eV };
  std::vector<G4double> scintreflectivity = {1., 1.};
  std::vector<G4double> scinttransmittance = {0.3, 0.3};
  // For couplings:
  std::vector<G4double> reflectivity = {1., 1.};
  std::vector<G4double> transmittance = {1., 1.};

  G4MaterialPropertiesTable* mpt_skin = new G4MaterialPropertiesTable();
  mpt_skin->AddProperty("REFLECTIVITY", energy, scintreflectivity);
  mpt_skin->AddProperty("TRANSMITTANCE", energy, scinttransmittance);

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
	mpt->AddProperty("REFLECTIVITY", energy, transmittance);
  mpt->AddProperty("TRANSMITTANCE", energy, reflectivity);


  // Scintillator - exterior
  G4OpticalSurface* ScintSkinSurface = new G4OpticalSurface("ScintSkinSurface");
  ScintSkinSurface->SetType(dielectric_dielectric);
	ScintSkinSurface->SetModel(glisur);
	ScintSkinSurface->SetFinish(polished);
  G4LogicalBorderSurface* ScintSurface = new G4LogicalBorderSurface("ScintSkinSurface", scintPV, worldPV,ScintSkinSurface);
  ScintSkinSurface-> SetMaterialPropertiesTable(mpt_skin);


  // Scintillator - OpCoupling
	G4OpticalSurface* ScintOpCouplSurface = new G4OpticalSurface("ScintCouplSurface");
  ScintOpCouplSurface->SetType(dielectric_dielectric);
	ScintOpCouplSurface->SetModel(glisur);
	ScintOpCouplSurface->SetFinish(polished);
	G4LogicalBorderSurface* ScintOpCouplBorder = new G4LogicalBorderSurface("ScintCouplSurface",scintPV,couplingPV,ScintOpCouplSurface);
  ScintOpCouplSurface->SetMaterialPropertiesTable(mpt);

	// Opticalcoupling - SiPM
	G4OpticalSurface* OpCouplSiPMSurface = new G4OpticalSurface("CouplSiPMSurface");
  OpCouplSiPMSurface->SetType(dielectric_dielectric);
	OpCouplSiPMSurface->SetModel(glisur);
	OpCouplSiPMSurface->SetFinish(polished);
	G4LogicalBorderSurface* CouplingSiPMBorder = new G4LogicalBorderSurface("CouplSiPMSurface",couplingPV,sipmPV,OpCouplSiPMSurface);
  OpCouplSiPMSurface->SetMaterialPropertiesTable(mpt);

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
  // G4Sphere* Hemisphere = new G4Sphere("Hemisphere", 70.0*mm, 70.1*mm, fStartPhiAngle, fAperturePhiAngle, fStartThetaAngle, fApertureThetaAngle);
    
  // G4LogicalVolume* HemisphereLV = new G4LogicalVolume(Hemisphere, defaultMaterial, "HemisphereLV");
    
  // G4VPhysicalVolume* HemispherePV = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), HemisphereLV, "HemispherePV", worldLV, false, 0, fCheckOverlaps);
  
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

  // scintLV->SetVisAttributes(Att_Green);
  // couplingLV->SetVisAttributes(Att_Yellow);
  // sipmLV->SetVisAttributes(Att_Blue);
  // for (int i=0; i<fNofLayers-1; i++) absoLV[i]->SetVisAttributes(Att_Red);

  // caloLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  // detectorLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  
    
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
  
  // Sensitive detector manager
  G4SDManager* sdman = G4SDManager::GetSDMpointer();

    
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
  auto ScinSD = new B4cCalorimeterSD("ScinSD", "ScintillatorHitsCollection", fNofLayers);
  sdman->AddNewDetector(ScinSD);
  SetSensitiveDetector("ScintillatorLV",ScinSD);



  //Define SIPMs sensitive detectorS
	auto sensSiPM = new SiPMSD("SiPMSD","SiPMHitsCollection");
	sdman->AddNewDetector(sensSiPM);
	//Set a sensitive detector to all logical volumes with name "SiPM"
	SetSensitiveDetector("SiPMLV",sensSiPM, true);




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