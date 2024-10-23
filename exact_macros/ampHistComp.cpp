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
void ReadDir(TH1F *hist, string dirStr);

void ampHistComp(){

    string dirStr = Form("%s20240904/RawDataMerged/",dataDir.c_str());

    TH1F *dist1 = new TH1F("dist1","20240904",200,-0.5,599.5);
    TH1F *dist2 = new TH1F("dist2","20240901,20240902,20240903",200,-0.5,599.5);
    dist1->GetXaxis()->SetTitle("Amplitude of highest amplitude pixel in the triggering MUSIC");
    dist2->GetXaxis()->SetTitle("Amplitude of highest amplitude pixel in the triggering MUSIC");

    ReadDir(dist1,Form("%s20240904/RawDataMerged/",dataDir.c_str()));
    ReadDir(dist2,Form("%s20240903/RawDataMerged/",dataDir.c_str()));
    ReadDir(dist2,Form("%s20240902/RawDataMerged/",dataDir.c_str()));
    ReadDir(dist2,Form("%s20240901/RawDataMerged/",dataDir.c_str()));

    TCanvas *c_disp = new TCanvas("tmac","tmac",2500,1000);
    c_disp->Divide(2,1);
    c_disp->cd(1);
    dist1->Draw("hist");
    c_disp->cd(2);
    dist2->Draw("hist");
    c_disp->Print("ampHistComp.png");
}

void ReadDir(TH1F *hist, string dirStr){
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
                cout << "\"Test\" Events: " << nEntries << endl;
                for(int evCount = 0; evCount < nEntries; evCount++){
                    tree->GetEntry(evCount);
                    int MUSICpos = MUSICmap[(ev->GetROIMusicID())[0] - 1];
                    int pTrig = MUSICpos*8;
                    Double_t ampMax = 0;
                    Pulse *pulse;
                    for(int i = 0; i < 8; i++){
                        pulse = new Pulse(ev->GetSignalValue(MUSICpos*8 + i));
                        if(pulse->GetAmplitude() > ampMax){
                            pTrig = MUSICpos*8 + i;
                            ampMax = pulse->GetAmplitude();
                        }
                        delete pulse;
                    }
                    hist->Fill(ampMax);
                }
                delete ev;
                delete tree;
            }
        }
    }
}