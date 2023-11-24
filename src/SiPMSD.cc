#include "SiPMSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "SteppingAction.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "B4Analysis.hh"
#include "G4UnitsTable.hh"

//constructor
SiPMSD::SiPMSD(G4String name, const G4String& hitsCollectionName, G4int NoSiPMs)
  : G4VSensitiveDetector(name),collectionID(-1),fSiPMperScint(0)
{
  collectionName.insert("SiPMHitsCollection");
  SetNofSiPMsperScint(NoSiPMs);
}

//destructor
SiPMSD::~SiPMSD()
{}

void SiPMSD::Initialize(G4HCofThisEvent* HCE)
{
  //Create hits collection
  hitsCollection = new SiPMHitsCollection(SensitiveDetectorName,collectionName[0]);

  if(collectionID < 0)
    collectionID = G4SDManager::GetSDMpointer()->GetCollectionID(hitsCollection);

  HCE->AddHitsCollection(collectionID, hitsCollection);
  
  //G4cout<<txtblue<<"Added a New Hit Collection: "<<collectionName[0]<<" (id "<<collectionID<<")\n"<<txtreset;
}

//ProcessHits
G4bool SiPMSD::ProcessHits(G4Step* step, G4TouchableHistory* ROhist)
{

  // G4String particleName = step->GetTrack()->GetDefinition()->GetParticleName();
  // if (particleName != "opticalphoton"){ G4cout << "\n\n"  <<particleName; return false; }



  //define something to store.
  G4double energydep = step->GetTotalEnergyDeposit();
  G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
  // G4int SiPMcopyNo = touchable->GetCopyNumber(0);  // <------------------------------------- 
  //G4int nbnc = step->GetTrack()->GetTrackID();


  // set unique SiPM ID using copy numbers from "parent" and "grandparent" volumes
  G4int DetectorID = touchable->GetCopyNumber(1);    // Detector copy 
  G4int ScintillatorID = touchable->GetCopyNumber(2);   // Scintillator copy
  auto SiPMID = DetectorID + ScintillatorID*fSiPMperScint;   

  // // Check that the copy ID really corresponds to desired Physical Volume
  // auto scintvol = touchable->GetVolume(2);
  // auto detvol = touchable->GetVolume(1);
  // G4cout << "Scintillator copy number " << scintvol->GetName() << " " << ScintillatorID << G4endl;
  // G4cout << "Detector copy number " << detvol->GetName() << " " << DetectorID << G4endl;

  // G4cout << "SiPM ID: " << SiPMID << G4endl;


  if (energydep==0.) return false;
  //std::cout<<nbnc<<" bounces in SD; ";
  SiPMHit* hit = new SiPMHit();

  hit->SetSiPMID(SiPMID);
  hit->SetTrackID(step->GetTrack()->GetTrackID());
  hit->SetEdep(energydep);
  hit->SetPos(step->GetPostStepPoint()->GetPosition());
  hit->SetArrTime(step->GetTrack()->GetGlobalTime());

  hitsCollection->insert(hit);
  
  //std::cout<< txtgreen << "A hit of "<< G4BestUnit(energydep,"Energy") <<"was stored in SiPM #" << SiPMcopyNo << ", in current volume: "<<step->GetPreStepPoint()->GetPhysicalVolume()->GetName()<< txtreset << std::endl;

  //hit->Print();

  return true;
}

//End of event
void SiPMSD::EndOfEvent(G4HCofThisEvent*){
  if ( verboseLevel>1 ) {
    G4int nofHits = hitsCollection->entries();
    G4cout << G4endl
	   << "-------->Hits Collection: in this event there were " << nofHits
	   << " hits in the SiPMs. " << G4endl;
	   for ( G4int i=0; i<nofHits; i++ ) (*hitsCollection)[i]->Print();
  }
}
