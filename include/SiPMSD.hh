//header file: SiPMSD.hh
#ifndef SiPMSD_h
#define SiPMSD_h 1

#include "G4VSensitiveDetector.hh"
#include "SiPMHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
//class EventAction;
//class SteppingAction;

class SiPMSD : public G4VSensitiveDetector
{
 public:
  ///Constructor
  SiPMSD(G4String SDname,const G4String& hitsCollectionName, G4int TotalSiPMs);
  ///Destructor
  virtual ~SiPMSD();

  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *ROhist);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);

  void SetNofSiPMsperScint(G4int n){fSiPMperScint = n;}
  
 private:
  SiPMHitsCollection* hitsCollection;
  G4int collectionID;
  G4int fSiPMperScint;
  
};

#endif
