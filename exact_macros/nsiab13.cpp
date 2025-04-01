R__LOAD_LIBRARY(libExACT.so)

#include <TCanvas.h>
#include <TH2.h>
#include <TTree.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>

using namespace std;

int MaxNofChannels = 256;

vector<vector<int>> dirData(string readStr){
    vector<vector<int>> out;
    vector<int> trigs(32,0);
	DIR *dir;
    struct dirent *ent;
    if((dir = opendir(readStr.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            string fileStr = Form("%s%s",readStr.c_str(),ent->d_name);
            if(fileStr.substr(fileStr.find_last_of(".")+1) == "root"){
                cout << "Loading file: " << fileStr << endl;
                TFile *f0 = TFile::Open(fileStr.c_str());
                if(!f0){
                    cout << "File is a zombie...skipping" << endl;
                    continue;
                }
                TTree *tree = (TTree*)f0->Get("Test");
                Event *ev = new Event();
                tree->SetBranchAddress("Events", &ev);
                int nEntries = tree->GetEntries();
                if(nEntries == 0){
                    cout << "File has no data in the \"Test\" branch...skipping" << endl;
                    delete ev;
                    delete tree;
                    f0->TFile::Close();
                    continue;
                }
                cout << "\"Test\" Events: " << nEntries << endl;
                for(int evCount = 0; evCount < nEntries; evCount++){
                    vector<int> evec(MaxNofChannels+1,0);
                    tree->GetEntry(evCount);
                    Pulse *pulse;
                    int mTrig = ev->GetROIMusicID()[0];
                    if(mTrig == 18 || mTrig == 19){evec.back() = 1;}
                    trigs[mTrig] += 1;
                    for(int i = 0; i < MaxNofChannels; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        evec[i] = pulse->GetAmplitude();
                        delete pulse;
                    }
                    out.push_back(evec);
                }
                delete ev;
                delete tree;
                f0->TFile::Close();
            }
        }
    }
    cout << "Total events: " << accumulate(trigs.begin(), trigs.end(), 0) << endl;
    for(int i = 0; i < trigs.size(); i++){
        cout << "MUSIC" << i << " triggers: " << trigs[i] << endl; 
    }
	return out;
}

void nsiab13(string dString){
    vector<vector<int>> data = dirData(dString);

    TH1D *trig13 = new TH1D("t13","SIAB 13 Triggered",1000,-0.5,3999.5);
    TH1D *trigOt = new TH1D("tOt","Other SIAB Triggered",1000,-0.5,3999.5);
    
    for(int i = 0; i < data.size(); i++){
        if(data[i].back() == 1){
            for(int j = 0; j < MaxNofChannels; j++){
                trig13->Fill(data[i][j]);
            }
        }
        else{
            for(int j = 0; j < MaxNofChannels; j++){
                trigOt->Fill(data[i][j]);
            }
        }
    }


    TCanvas *can1 = new TCanvas("can1","can1",2500,1000);
    can1->Divide(2,1);
    can1->cd(1);
    trig13->Draw();
    can1->cd(2);
    trigOt->Draw();
    can1->Print("test.png");
}