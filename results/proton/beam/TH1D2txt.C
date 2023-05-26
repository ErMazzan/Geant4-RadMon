//////////////////////////////////////////////////
//
// Macro to save a TH1D histogram to a txt file
//
// usage: root -l TH1D2txt.C\(\"filename\",\"histogram_name\"\)
// e.g.: root -l TH1D2txt.C\(\"100-MeV.root\",\"EScint1\"\)
// optional for integer bins: root -l TH1D2txt.C\(\"100-MeV.root\",\"EScint1\",\"false\"\)
//
// D. Guberman, 27/03/2023
//
/////////////////////////////////////////////////

#include <iostream>
#include <fstream>

using namespace std;

void TH1D2txt(char* filename, char* histname, bool isInt=false){
    
    char* filenameout = "outHist.txt";

    TFile* file = new TFile(filename, "READ");
	TH1D* h1 = (TH1D*) file->Get(histname);
    
    Int_t nBins = h1->GetNbinsX();

    ofstream fout;
    fout.open(filenameout);

    for (Int_t i = 1; i < nBins; i ++){
        if (isInt) {
            //cout << Form("X, Y = %.2f, %i\n", h1->GetBinCenter(i), int(h1->GetBinContent(i)));
            fout<<h1->GetBinCenter(i)<<"; "<<int(h1->GetBinContent(i))<<endl;
        }
        else {
            //cout << Form("X, Y = %.2f, %.2f\n", h1->GetBinCenter(i), h1->GetBinContent(i));
            fout<<h1->GetBinCenter(i)<<"; "<<h1->GetBinContent(i)<<endl;
        }
    }
    fout.close();
}
    
