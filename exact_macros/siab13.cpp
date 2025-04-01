R__LOAD_LIBRARY(libExACT.so)

#include <TCanvas.h>
#include <TH2.h>
#include <TTree.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>

using namespace std;

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
                    vector<int> evec(17,0);
                    tree->GetEntry(evCount);
                    Pulse *pulse;
                    pulse = new Pulse(ev->GetSignalValue(74));
                    if(pulse->GetAmplitude() > 400){
                        delete pulse;
                        continue;
                    }
                    delete pulse;
                    int mTrig = ev->GetROIMusicID()[0];
                    if(mTrig == 18 || mTrig == 19){evec.back() = 1;}
                    trigs[mTrig] += 1;
                    for(int i = 64; i < 80; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        evec[i-64] = pulse->GetAmplitude();
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

void siab13(string dString){
    vector<vector<int>> data = dirData(dString);

    vector<TH1D*> trig13;
    vector<TH1D*> trigOt;
    for(int i = 64; i<80; i++){
        TH1D *h1 = new TH1D(Form("t13,pixel %d",i),"SIAB 13 Triggered",1000,-0.5,3999.5);
        TH1D *h2 = new TH1D(Form("tOt,pixel %d",i),"Other SIAB Triggered",1000,-0.5,3999.5);
        trig13.push_back(h1);
        trigOt.push_back(h2);
    }
    for(int i = 0; i < data.size(); i++){
        if(data[i].back() == 1){
            for(int j = 0; j < 16; j++){
                trig13[j]->Fill(data[i][j]);
            }
        }
        else{
            for(int j = 0; j < 16; j++){
                trigOt[j]->Fill(data[i][j]);
            }
        }
    }


    TCanvas *can1 = new TCanvas("can1","can1",2500,1000);
    can1->Print("test.pdf[");
    delete can1;
    for(int i = 0; i < 16; i++){
        can1 = new TCanvas("can1","can1",2500,1000);
        can1->Divide(2,1);
        can1->cd(1);
        trig13[i]->Draw();
        can1->cd(2);
        trigOt[i]->Draw();
        can1->Print("test.pdf");
        delete can1;
    }
    can1 = new TCanvas();
    can1->Print("test.pdf]");
}