#ifndef SiPMHit_h
#define SiPMHit_h 1

#include"G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

class SiPMHit : public G4VHit
{
  public:
  
    SiPMHit();
    SiPMHit(const SiPMHit&);
    virtual ~SiPMHit();

    // operators: Do I need them?
    const SiPMHit& operator=(const SiPMHit&);
    G4int operator==(const SiPMHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);
  
    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTrackID		(G4int track)      	{ fTrackID = track; };
    void SetSiPMID		(G4int SiPMnr)		{ fSiPMID = SiPMnr; };
    void SetEdep		(G4double de)      	{ fEdep = de; };
    void SetPos			(G4ThreeVector xyz)	{ fPos = xyz; };
    void SetArrTime		(G4double t)       	{ fArrTime = t; };

    // Get methods
    G4int GetTrackID() const     	{ return fTrackID; };
    G4double GetSiPMID() const		{ return fSiPMID; };
    G4double GetEdep() const     	{ return fEdep; };
    G4ThreeVector GetPos() const 	{ return fPos; };
    G4double GetArrTime() const  	{ return fArrTime; };

      //G4int			    fNbounces;
      G4int         fTrackID;
      G4int			    fSiPMID;
      G4double      fEdep;
      G4ThreeVector fPos;
      G4double      fArrTime;
      
  private:
};

typedef G4THitsCollection<SiPMHit> SiPMHitsCollection;

extern G4ThreadLocal G4Allocator<SiPMHit>* SiPMHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SiPMHit::operator new(size_t)
{
  if(!SiPMHitAllocator)
      SiPMHitAllocator = new G4Allocator<SiPMHit>;
  return (void *) SiPMHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void SiPMHit::operator delete(void *hit)
{
  SiPMHitAllocator->FreeSingle((SiPMHit*) hit);
}


#endif
