 #ifndef MyPhysicsList_h
 #define MyPhysicsList_h 1
 
 #include "G4VModularPhysicsList.hh"
 #include "globals.hh"
 
 class MyPhysicsList: public G4VModularPhysicsList
 {
	   public:
	 
		MyPhysicsList();
		virtual ~MyPhysicsList();
	 
	   public:
	 
		// SetCuts()
		virtual void SetCuts();
	 
	 };
 
 #endif


