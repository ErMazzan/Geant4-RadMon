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
// $Id: WLSMaterials.cc 82854 2014-07-14 09:08:25Z gcosmo $
//
/// \file src/WLSMaterials.cc
/// \brief Implementation of the Materials class
//
//
#include "MyMaterials.hh"
#include "G4SystemOfUnits.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4OpticalSurface.hh"

MyMaterials* MyMaterials::fInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyMaterials::MyMaterials()
{
  fNistMan = G4NistManager::Instance();
  fNistMan->SetVerbose(2);
  CreateMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyMaterials::~MyMaterials()
{
  delete fVacuum;
  delete fPb;
  delete fW;
  delete fTa;
  delete fCu;
  delete fPolystyrene;
  delete fSi;
  delete fEpoxy;
  delete fSilicone;
  delete fTeflon;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyMaterials* MyMaterials::GetInstance()
{
  if (fInstance == 0)
      fInstance = new MyMaterials();
      
  return fInstance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* MyMaterials::GetMaterial(const G4String material)
{
  G4Material* mat =  fNistMan->FindOrBuildMaterial(material);

  if (!mat) mat = G4Material::GetMaterial(material);
  if (!mat) {
     std::ostringstream o;
     o << "Material " << material << " not found!";
     G4Exception("Materials::GetMaterial","",
                 FatalException,o.str().c_str());
  }

  return mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyMaterials::CreateMaterials()
{

  std::vector<G4int> natoms;
  std::vector<G4String> elements;

  ///
  /// CREATE MATERIALS
  ///

  // Vacuum
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  new G4Material("Galactic", z=1., a=1.01*g/mole,density=CLHEP::universe_mean_density, 
                  // declared in geant4-v11.1.1_install/include/Geant4/CLHEP/Units/PhysicalConstants.h
                  // static constexpr double universe_mean_density = 1.e-25*g/cm3;
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);
  G4Material* fVacuum = fNistMan->FindOrBuildMaterial("Galactic");
  fVacuum->SetName("fVacuum");


  // Scintillator materials
  G4Material* fPolystyrene = fNistMan->FindOrBuildMaterial("G4_POLYSTYRENE");
  fPolystyrene->SetName("fPolystyrene");

  // Other materials
  G4Material* fPb = fNistMan->FindOrBuildMaterial("G4_Pb");
  fPb->SetName("fPb");

  G4Material* fW = fNistMan->FindOrBuildMaterial("G4_W");
  fW->SetName("fW");

  G4Material* fTa = fNistMan->FindOrBuildMaterial("G4_Ta");
  fTa->SetName("fTa");

  G4Material* fCu = fNistMan->FindOrBuildMaterial("G4_Cu");
  fCu->SetName("fCu");

  G4Material* fSi = fNistMan->FindOrBuildMaterial("G4_Si");
  fSi->SetName("fSi");


  // Reflector 
  G4Material* fTeflon = fNistMan->FindOrBuildMaterial("G4_TEFLON"); //G4_MAGNESIUM_OXIDE, G4_Al
  fTeflon->SetName("fTeflon");

 	// Epoxy
	elements.push_back("C");     natoms.push_back(2);
  elements.push_back("H");     natoms.push_back(2);
	density = 1.2*g/cm3;
  G4Material* fEpoxy = fNistMan->ConstructNewMaterial("fEpoxy", elements, natoms, density);
  elements.clear();
  natoms.clear();
    

  // Silicone
	elements.push_back("C");     natoms.push_back(2);
  elements.push_back("H");     natoms.push_back(6);
  density = 1.060*g/cm3;
  G4Material* fSilicone = fNistMan->ConstructNewMaterial("fSilicone", elements, natoms, density);
	elements.clear();
  natoms.clear();
  

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;


  ///
  /// ADD OPTICAL PROPERTIES TO CREATED MATERIALS
  ///

  // Scintillator

  G4MaterialPropertiesTable* MyScintMPT = new G4MaterialPropertiesTable();
  G4double lightyield = 10000./MeV;
  G4double pde = 0.3;

  std::vector<G4double> energy     = {2.64*eV, 3.06*eV, 3.22*eV};
  std::vector<G4double> rindex     = {1.58, 1.58, 1.58};
  std::vector<G4double> absorption = {140.*cm, 140.*cm, 140.*cm};
  std::vector<G4double> scintemission = {0.1, 1.0, 0.1};

  MyScintMPT->AddProperty("RINDEX", energy, rindex);
  MyScintMPT->AddProperty("ABSLENGTH", energy, absorption);
  MyScintMPT->AddConstProperty("SCINTILLATIONYIELD", 20); //lightyield scaled to the SiPM pde: lightyield*pde
  MyScintMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
  // only use one component 
  MyScintMPT->AddProperty("SCINTILLATIONCOMPONENT1", energy, scintemission);
  MyScintMPT->AddProperty("SCINTILLATIONCOMPONENT2", energy, scintemission);
  MyScintMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.2*ns);
  MyScintMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 1.*ns);
  MyScintMPT->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
  MyScintMPT->AddConstProperty("SCINTILLATIONYIELD2", 0.0);


  fPolystyrene->SetMaterialPropertiesTable(MyScintMPT);

  // Reflector

  std::vector<G4double> RRefractionIndex = {1.36,1.36,1.36}; //1.76 for MgO at 420 nm; maybe set to 1.36 for Teflon?
	std::vector<G4double> RAbsorptionLength = {1.0E-9*m,1.0E-9*m,1.0E-9*m};

  G4MaterialPropertiesTable* RMPT = new G4MaterialPropertiesTable();
	RMPT->AddProperty("RINDEX",energy,RRefractionIndex);
  RMPT->AddProperty("ABSLENGTH",energy,RAbsorptionLength);
  fTeflon->SetMaterialPropertiesTable(RMPT);


  // Epoxy (SiPM) 
  std::vector<G4double> EpoxyRefractionIndex = {1.51,1.51,1.51};
	std::vector<G4double> EpoxyAbsorptionLength = {1.0E-9*m,1.0E-9*m,1.0E-9*m};
  // std::vector<G4double> PhotonEnergy = {1.0*eV,4.0*eV,7.0*eV};
	G4MaterialPropertiesTable* EpoxyMPT = new G4MaterialPropertiesTable();
	EpoxyMPT->AddProperty("RINDEX",energy,EpoxyRefractionIndex);
	EpoxyMPT->AddProperty("ABSLENGTH",energy,EpoxyAbsorptionLength);
	fEpoxy->SetMaterialPropertiesTable(EpoxyMPT);


  // Silicone 
  std::vector<G4double> SiliconeRefractionIndex = {1.47,1.47,1.47};
	std::vector<G4double> SiliconeAbsorptionLength = {1.0E9*m,1.0E9*m,1.0E9*m};
	G4MaterialPropertiesTable* SiliconeMPT = new G4MaterialPropertiesTable();
	SiliconeMPT->AddProperty("RINDEX",energy,SiliconeRefractionIndex);
	SiliconeMPT->AddProperty("ABSLENGTH",energy,SiliconeAbsorptionLength);
	fSilicone->SetMaterialPropertiesTable(SiliconeMPT);


}
