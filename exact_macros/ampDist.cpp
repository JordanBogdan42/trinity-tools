//tells root where to find ExACT libraries. Only needed if using as a macro for root. To compile, you remove this line and include ExACT libraries directly
R__LOAD_LIBRARY(libExACT.so)
//includes libraries used in script
#include <TH1.h>
#include <TH2F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TPaveStats.h>
#include <typeinfo>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <cmath>

//set "base" directories
//dataDir is where the daily data directories are saved (i.e. "YYYYMMDD" directories)
//outDir is the directory where output will be saved
std::string outDir = "/storage/home/hhive1/jbogdan3/data/";
/*std::string dataDir = "/mnt/hgfs/vm/ff/";
std::string outDir = "/mnt/hgfs/vm/ff/";*/

//initialize or declare global variables, which are allocated static memory and are available in every scope
//TTree *tree = 0;
//Event *ev;
TCanvas *c_disp = 0;
int MaxNofChannels = 256;

//declare functions to be defined later
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();
std::vector<Double_t> fileData(std::string dirStr, std::string treeStr);
Double_t Median(vector<Double_t> v);

void ampDist(std::string dStr, std::string treeString)
{
	std::vector<Double_t> dvAvg = fileData(dStr,treeString);

	c_disp = new TCanvas("Display","CameraPlot",2500,1000);
	c_disp->Divide(2,1);
	TH2F *aCam = new TH2F("aCam","",16,-0.5,15.5,16,-0.5,15.5);
	TH1 *aDist = new TH1F("aDist","",100,0,2);

	Double_t dMedian = Median(dvAvg);
	
	for(int i = 0; i < MaxNofChannels; i++){
		int nx, ny;
		FindBin(i,&nx,&ny);
		aCam->SetBinContent(nx+1,ny+1,(dvAvg[i]/dMedian)-1);
		aDist->Fill(dvAvg[i]/dMedian);
	}
	aCam->SetStats(0);
	//aDist->SetStats(0);
	c_disp->cd(1);
	c_disp->cd(1)->SetRightMargin(0.15);
	aCam->GetXaxis()->SetTitle("Camera column");
	aCam->GetYaxis()->SetTitle("Camera row");
	aCam->GetZaxis()->SetTitle("Normalized deviation from camera median");
	aCam->GetZaxis()->SetTitleOffset(1.45);
	aCam->Draw("colz");
	DrawMUSICBoundaries();
	c_disp->cd(2);
	aDist->GetXaxis()->SetTitle("Signal amplitude normalized to camera median");
	aDist->GetYaxis()->SetTitle("# of pixels");
	aDist->SetLineWidth(3);
	gStyle->SetOptStat(1110);
	gStyle->SetStatFormat("6.2g");
	aDist->Draw("hist");
	gPad->Update();
	TPaveStats* st = (TPaveStats*)aDist->FindObject("stats");
	cout << st << endl;
	st->SetY1NDC(0.8);
	st->SetY2NDC(0.9);
	st->SetX1NDC(0.7);
	st->SetX2NDC(0.9);
	c_disp->Print("/storage/home/hhive1/jbogdan3/data/avgDist.png");
}

std::vector<Double_t> fileData(std::string fileName, std::string treeStr){
	std::vector<Double_t> out(MaxNofChannels,0.0);
	cout << "Loading file: " << fileName << endl;
	TFile *f0 = TFile::Open(fileName.c_str());
	TTree *tree = (TTree*)f0->Get(treeStr.c_str());
	Event *ev = new Event();
	tree->SetBranchAddress("Events", &ev);
	int nEntries = tree->GetEntries();
	cout << "Total Number of Events: " << nEntries << endl;
	for(int countEvent = 0; countEvent < nEntries; countEvent++){
		std::vector<Double_t> outRun(MaxNofChannels,0.0);
		tree->GetEntry(countEvent);
		Pulse *pulse;
		for(int i = 0; i < MaxNofChannels; i++){
			pulse = new Pulse(ev->GetSignalValue(i));
			int ampVal = pulse->GetAmplitude();
			/*if(ampVal == 0){
				nEntries -= 1;
				goto evskip;
			}
			outRun[i] = ampVal;*/
      out[i] += ampVal;
			delete pulse;
		}
		/*for(int i = 0; i < MaxNofChannels; i++){   
			out[i] += outRun[i];      
		}
		evskip:;*/
	}
	delete f0;
	//delete tree;
	delete ev;
	for(int i = 0; i < MaxNofChannels; i++){
		out[i] /= nEntries;
	}
	return out;
}

//Calculates the 2D bin coordinates associated with a 1D vector/number, i.e. pixel number, based on Trinity camera layout
void FindBin(int pixelID, int *nx, int *ny)
{
	int SIAB_Number = pixelID / 16;
	int SIAB_Pixel_Number = pixelID % 16;
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;	
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
}

//Draws red boxes to make obvious which pixels are associated with the same MUSIC chip
void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,1.5,3.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->SetLineWidth(2);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=1; i < MaxNofChannels/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetX1((i%8)*2-0.5);
		bn->SetX2((i%8)*2+1.5);
		bn->SetY1((i/8)*4-0.5);
		bn->SetY2((i/8)*4+3.5);
		bn->Draw();
	}
}

//Function for calculating median 
Double_t Median(vector<Double_t> v){
	//Size of vector
	int n = v.size();
	//Make temp copy of the vector to leave original in the same order
	std::vector<Double_t> tempV(v);
    //Sort the vector 
    sort(tempV.begin(), tempV.end()); 
    //Check if the number of elements is odd 
    if(n%2!=0){
        return(Double_t)tempV[n/2];
	}
    //If the number of elements is even, return the average of the two middle elements 
    return(Double_t)(tempV[(n-1)/2]+tempV[n/2])/2.0; 
}

//returns HVCh id -1 (because c++ indexes starting from 0)
int HVCh(int camPix){
	if((camPix <= 31) || (camPix >= 64 && camPix <= 95)){
		return(int)0;
	}
	if((camPix >= 32 && camPix <= 63) || (camPix >= 96 && camPix <= 127)){
		return(int)2;
	}
	if((camPix >= 128 && camPix <= 159) || (camPix >= 192 && camPix <= 224)){
		return(int)1;
	}
	if((camPix >= 160 && camPix <= 191) || (camPix >= 224)){
		return(int)3;
	}
	cout << "error: pixel ID did not match any HV channels" << endl;
	return(int)0;
}
