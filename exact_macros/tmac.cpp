R__LOAD_LIBRARY(libExACT.so)

#include <iostream>
#include <vector>
#include <cmath>
#include <dirent.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>

using namespace std;

const int maxCh = 256;
const string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
const vector<int> MUSICmap = {2,3,10,11,18,19,26,27,6,7,14,15,22,23,30,31,0,1,8,9,16,17,24,25,4,5,12,13,20,21,28,29};
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();

void tmac(string fileStr){
    TH2F *hist = new TH2F("musicHeat","Triggered Music [Counts]",8,-0.5,15.5,4,-0.5,15.5);
    TTree *tree;
    Event *ev;
    cout << "Loading file: " << fileStr << endl;
    TFile *f0 = TFile::Open(fileStr.c_str());
    tree = (TTree*)f0->Get("Test");
    ev = new Event();
    tree->SetBranchAddress("Events", &ev);
    int nEntries = tree->GetEntries();
    cout << "\"Test\" Events: " << nEntries << endl;
    for(int evCount = 0; evCount < nEntries; evCount++){
        tree->GetEntry(evCount);
        int MUSICpos = MUSICmap[(ev->GetROIMusicID())[0] - 1];
        cout << "MUSICpos: " << MUSICpos << endl;
        int pTrig = MUSICpos*8;
        int nx, ny;
        FindBin(pTrig,&nx,&ny);
        hist->Fill(nx,ny);
    }
    delete ev;
    delete tree;
    TCanvas *c_disp = new TCanvas("Display","DataSummary",1250,1000);
    hist->Draw("colz");
    gStyle->SetOptStat(10);
    DrawMUSICBoundaries();
    c_disp->Print("tmac.png");
}

void FindBin(int pixelID, int *nx, int *ny)
{
	int SIAB_Number = pixelID / 16;
	int SIAB_Pixel_Number = pixelID % 16;
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
}

void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,1.5,3.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=1; i < maxCh/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetX1((i%8)*2-0.5);
		bn->SetX2((i%8)*2+1.5);
		bn->SetY1((i/8)*4-0.5);
		bn->SetY2((i/8)*4+3.5);
		bn->Draw();
	}
}