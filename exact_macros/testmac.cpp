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

void testmac(string dateStr){

    string dirStr = Form("%s%s/RawDataMerged/",dataDir.c_str(),dateStr.c_str());

    TH1F *dist = new TH1F("ampDist","Camera Median Amplitude [ADC]",600,-0.5,599.5);
    TH2F *plot = new TH2F("medVrms","Event Amplitude RMS v. Camera Median Amplitude",600,-0.5,599.5,1000,0,10);

    TTree *tree;
    Event *ev;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(dirStr.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            string fileStr = Form("%s%s",dirStr.c_str(),ent->d_name);
            if(fileStr.substr(fileStr.find_last_of(".")+1) == "root"){
                //cout << "Loading file: " << fileStr << endl;
                TFile *f0 = TFile::Open(fileStr.c_str());
                tree = (TTree*)f0->Get("Test");
                ev = new Event();
                tree->SetBranchAddress("Events", &ev);
                int nEntries = tree->GetEntries();
                if(nEntries == 0){
                    cout << "File has no data in the \"Test\" branch..." << endl;
                }
                cout << "\"Test\" Events: " << nEntries << endl;
                for(int evCount = 0; evCount < 1; evCount++){
                    tree->GetEntry(evCount);
                    Pulse *pulse;
                    vector<Double_t> evAvg(maxCh);
                    cout << (ev->GetROIMusicID()).size() << endl;
                    for(auto i:ev->GetROIMusicID()){
                        cout << i << ",";
                    }
                    cout << endl;
                }
                delete ev;
                delete tree;
                break;
            }
        }
    }
}