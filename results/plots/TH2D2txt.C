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

#include <TFile.h>
#include <TH2D.h>

#include <iostream>
#include <fstream>

using namespace std;

void TH2D2txt(char* filename, char* histname, bool isInt=false){
    
    char* filenameout = "outHist.txt";

    TFile* file = new TFile(filename, "READ");
	TH2D* h1 = (TH2D*) file->Get(histname);

    //h1->Rebin2D(2,2);
    
    Int_t nBinsX = h1->GetNbinsX();
    Int_t nBinsY = h1->GetNbinsY();

    Int_t nBins = nBinsX*nBinsY;

    ofstream fout;
    fout.open(filenameout);

    Int_t thisX,thisY,thisZ;

    //nBins = 10;

    

    for (Int_t i = 0; i < nBins; i ++){
        if (isInt==true) {
            //cout << Form("X, Y = %.2f, %i\n", h1->GetBinCenter(i), int(h1->GetBinContent(i)));
            fout<<h1->GetXaxis()->GetBinCenter(thisX)<<" "<<h1->GetYaxis()->GetBinCenter(thisY)<<" "<<int(h1->GetBinContent(i))<<endl;
            //cout<<"here!\n";
        }
        else {
            
            //fout<<h1->GetBinCenter(i)<<"\t"<<h1->GetBinContent(i)<<endl;
            h1->GetBinXYZ(i,thisX,thisY,thisZ);
            fout<<h1->GetXaxis()->GetBinCenter(thisX)<<" "<<h1->GetYaxis()->GetBinCenter(thisY)<<" "<<h1->GetBinContent(i)<<endl;
            //cout << Form("X, Y = %.2f, %.2f\n", h1->GetBinCenter(i), h1->GetBinContent(i));
        }
    }
    fout.close();

    //cout<<nBinsX<<" "<<h1->GetXaxis()->GetBinCenter(0)<<" "<<h1->GetXaxis()->GetBinCenter(nBinsX)<<endl;
    //cout<<nBinsX<<" "<<h1->GetYaxis()->GetBinCenter(0)<<" "<<h1->GetYaxis()->GetBinCenter(nBinsY)<<endl;
}
    
