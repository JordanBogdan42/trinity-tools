R__LOAD_LIBRARY(libExACT.so)

#include <iostream>
#include <vector>
#include <cmath>
#include <dirent.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

using namespace std;

const int maxCh = 256;
const string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";

void flasherEBanal(string dateStr){

    string dirStr = Form("%s%s/RawDataMerged/",dataDir.c_str(),dateStr.c_str());

    vector<vector<int>> fRat;
    vector<int> dRat = {0,0};

    TTree *tree;
    Event *ev;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(dirStr.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            string fileStr = Form("%s%s",dirStr.c_str(),ent->d_name);
            if(fileStr.substr(fileStr.find_last_of(".")+1) == "root"){
                cout << "Loading file: " << fileStr << endl;
                TFile *f0 = TFile::Open(fileStr.c_str());
                tree = (TTree*)f0->Get("Test");
                ev = new Event();
                tree->SetBranchAddress("Events", &ev);
                int nEntries = tree->GetEntries();
                if(nEntries == 0){
                    cout << "File has no data in the \"Test\" branch...skipping" << endl;
                    continue;
                }
                cout << "\"Test\" Events: " << nEntries << endl;
                fRat.push_back({0,0});
                for(int evCount = 0; evCount < nEntries; evCount++){
                    tree->GetEntry(evCount);
                    Pulse *pulse;
                    Double_t evAvg = 0;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        evAvg += pulse->GetAmplitude();
                        delete pulse;
                    }
                    evAvg /= maxCh;
                    if(evAvg > 350){
                        fRat[fRat.size()-1][0] += 1;
                        dRat[0] += 1;
                    }
                }
                delete ev;
                delete tree;

                tree = (TTree*)f0->Get("HLED");
                ev = new Event();
                tree->SetBranchAddress("Events", &ev);
                nEntries = tree->GetEntries();
                if(nEntries == 0){
                    cout << "File has no data in the \"HLED\" branch...skipping" << endl;
                    continue;
                }
                cout << "\"HLED\" Events: " << nEntries << endl;
                for(int evCount = 0; evCount <nEntries; evCount++){
                    tree->GetEntry(evCount);
                    Pulse *pulse;
                    Double_t evAvg = 0;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        evAvg += pulse->GetAmplitude();
                        delete pulse;
                    }
                    evAvg /= maxCh;
                    if(evAvg < 250){
                        fRat[fRat.size()-1][1] += 1;
                        dRat[1] += 1;
                    }
                }
                cout << "fRat[Test:HLED]: " << fRat[fRat.size()-1][0] << ":" << fRat[fRat.size()-1][1] << endl;
                cout << "dRat[Test:HLED]: " << dRat[0] << ":" << dRat[1] << endl;
            }
        }
    }
    cout << "End!" << endl;
}