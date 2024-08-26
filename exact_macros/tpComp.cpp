R__LOAD_LIBRARY(libExACT.so)

#include <iostream>
#include <vector>
#include <cmath>
#include <dirent.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

const vector<int> pixels = {80,112,208,240};

Double_t Median(vector<Double_t> v);
Double_t RMS(vector<Double_t> v);

void tpComp(string fullDir, string fourDir){

    vector<vector<double>> tpAvg(2,vector<double>(4,0));
    vector<vector<double>> ampAvg(2,vector<double>(4,0));

    TTree *tree;
    Event *ev;

    cout << "Loading file: " << fullDir << endl;
    TFile *f0 = TFile::Open(fullDir.c_str());
    tree = (TTree*)f0->Get("HLED");
    ev = new Event();
    tree->SetBranchAddress("Events", &ev);
    int nEntries = tree->GetEntries();
    cout << "\"Test\" Events: " << nEntries << endl;
    for(int evCount = 0; evCount < nEntries; evCount++){
        tree->GetEntry(evCount);
        Pulse *pulse;
        for(auto i = 0; i < pixels.size(); i++){
            pulse = new Pulse(ev->GetSignalValue(pixels[i]));
            tpAvg[0][i] += pulse->GetTimePeak();
            ampAvg[0][i] += pulse->GetAmplitude();
            delete pulse;
        }
    }
    delete ev;
    delete tree;
    f0->Close();
    delete f0;
    for(auto i = 0; i < tpAvg[0].size(); i++){
        tpAvg[0][i] /= nEntries;
        ampAvg[0][i] /= nEntries;
    }

    cout << "Loading file: " << fourDir << endl;
    f0 = TFile::Open(fourDir.c_str());
    tree = (TTree*)f0->Get("HLED");
    ev = new Event();
    tree->SetBranchAddress("Events", &ev);
    nEntries = tree->GetEntries();
    cout << "\"Test\" Events: " << nEntries << endl;
    for(int evCount = 0; evCount < nEntries; evCount++){
        tree->GetEntry(evCount);
        Pulse *pulse;
        for(auto i = 0; i < pixels.size(); i++){
            pulse = new Pulse(ev->GetSignalValue(pixels[i]));
            tpAvg[1][i] += pulse->GetTimePeak();
            ampAvg[1][i] += pulse->GetAmplitude();
            delete pulse;
        }
    }
    delete ev;
    delete tree;
    f0->Close();
    delete f0;
    for(auto i = 0; i < tpAvg[1].size(); i++){
        tpAvg[1][i] /= nEntries;
        ampAvg[1][i] /= nEntries;
    }

    cout << endl << "Time Peaks" << endl;
    cout << "_______________________________________________" << endl << "Pixel ID:\t";
    for(auto i: pixels){
        cout << i << "\t";
    }
    cout << endl << "All:\t\t";
    for(auto i: tpAvg[0]){
        cout << i << "\t";
    }
    cout << endl << "Quads:\t\t";
    for(auto i: tpAvg[1]){
        cout << i << "\t";
    }
    cout << endl << "Difference:\t";
    cout << fixed << setprecision(4);

    for(auto i = 0; i < pixels.size(); i++){
        cout << tpAvg[0][i] - tpAvg[1][i] << "\t";
    }
    cout << endl;
    cout << endl << "Amplitude" << endl;
    cout << "_______________________________________________" << endl << "Pixel ID:\t";
    cout << setprecision(3);
    for(auto i: pixels){
        cout << i << "\t";
    }
    cout << endl << "All:\t\t";
    for(auto i: ampAvg[0]){
        cout << i << "\t";
    }
    cout << endl << "Quads:\t\t";
    for(auto i: ampAvg[1]){
        cout << i << "\t";
    }
    cout << endl << "Difference:\t";
    cout << setprecision(4);

    for(auto i = 0; i < pixels.size(); i++){
        cout << ampAvg[0][i] - ampAvg[1][i] << "\t";
    }
    cout << endl;
}

Double_t Median(vector<Double_t> v)
{
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

Double_t RMS(vector<Double_t> v)
{
	int n = v.size();
	Double_t rms_val = 0;
	for(auto i: v){
		rms_val += pow(i,2);
	}
	rms_val /= n;
	return(Double_t)sqrt(rms_val);
}