#include "DataSummary.h"

#include "constants.h"
#include "rootils.h"

#include <TTree.h>
#include <TFile.h>

#include <Event.h>
#include <Pulse.h>

#include <dirent.h>
#include <iostream>
#include <vector>

using namespace std;

DataSummary::DataSummary(string dateStr){
    evTimes = vector<vector<ULong64_t>>(2,vector<ULong64_t>());
    evMeans = vector<vector<Double_t>>(7,vector<Double_t>());
    pixMeans = vector<vector<Double_T>>(7,vector<Double_t>(maxCh,0.0));

    string evStr = Form("%s%s/RawDataMerged/",dataDir.c_str(),dateStr.c_str());
    ReadEv(evStr);
}

DataSummary::ReadEv(string readStr){
    TTree *tree;
    Event *ev;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(readStr)) != NULL){
        while((ent = readdir(dir)) != NULL){
            string fileStr = Form("%s%s",readStr.c_str(),ent->d_name);
            if(fileStr.substr(fileStr.find_last_of(".")+1) == "root"){
                cout << "Loading file: " << fileStr << endl;
                TFile *f0 = TFile::Open(fileStr.c_str());
                tree = (TTree*)f0->Get("Test");
                ev = new Event();
                SetBranches(&ev);
                int nEntries = tree->GetEntries();
                if(nEntries = 0){
                    cout << "File has no data in the \"Test\" branch...skipping"
                    break;
                }
                cout << "\"Test\" Events: " << nEntries << endl;
                for(int evCount = 0; evCount < nEntries; evCount++){
                    tree->GetEntry(evCount);
                    evTimes[1].push_back(ev->GetTBTime());
                    for(int i = 2; i < 7; i++){
                        evMeans[i].push_back(0.0);
                    }
                    Pulse *pulse;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        
                        pixMeans[2][i] += pulse->GetPedestal();
                        pixMeans[3][i] += pulse->GetPedestalRMS();
                        pixMeans[4][i] += pulse->GetAmplitude();
                        pixMeans[5][i] += pulse->GetCharge();
                        pixMeans[6][i] += pulse->GetTimePeak();

                        evMeans[2][evMeans[2].size()-1] += pulse->GetPedestal();
                        evMeans[3][evMeans[2].size()-1] += pulse->GetPedestalRMS();
                        evMeans[4][evMeans[2].size()-1] += pulse->GetAmplitude();
                        evMeans[5][evMeans[2].size()-1] += pulse->GetCharge();
                        evMeans[6][evMeans[2].size()-1] += pulse->GetTimePeak();
                        
                        delete pulse;
                    }
                    for(int i = 2; i < 7; i++){
                        evMeans[i][evMeans[i].size()-1] /= maxCh;
                    }
                }
                delete ev;
                delete tree;
                delete nEntries;

                tree = (TTree*)f0->Get("HLED");
                ev = new Event();
                SetBranches(&ev);
                nEntries = tree->GetEntries();
                if(nEntries = 0){
                    cout << "File has no data in the \"HLED\" branch...skipping"
                    break;
                }
                cout << "\"HLED\" Events: " << nEntries << endl;
                for(int evCount = 0; evCount < nEntries; evCount++){
                    tree->GetEntry(evCount);
                    evTimes[0].push_back(ev->GetTBTime());
                    for(int i = 0; i < 2; i++){
                        evMeans[i].push_back(0.0);
                    }
                    Pulse *pulse;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        
                        pixMeans[0][i] += pulse->GetAmplitude();
                        pixMeans[1][i] += pulse->GetAmplitude();

                        evMeans[0][evMeans[2].size()-1] += pulse->GetAmplitude();
                        evMeans[1][evMeans[2].size()-1] += pulse->GetAmplitude();
                        
                        delete pulse;
                    }
                }
            }
        }
    }
}