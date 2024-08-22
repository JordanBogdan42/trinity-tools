#include "DataSummary.h"

#include "constants.h"
#include "rootils.h"
#include "DtStruct.h"

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

#include <Event.h>
#include <Pulse.h>

#include <dirent.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

DataSummary::DataSummary(char* dateStr){
    hledEv = vector<DtStruct>();
    testEv = vector<DtStruct>();
    pixMeans = vector<vector<Double_t>>(7,vector<Double_t>(maxCh,0.0));
    camera = new TH2F();
    ddt = new TH2F();

    string evStr = Form("%s%s/RawDataMerged/",dataDir.c_str(),dateStr);
    ReadEv(evStr);
}

void DataSummary::ReadEv(string readStr){
    TTree *tree;
    Event *ev;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(readStr.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            string fileStr = Form("%s%s",readStr.c_str(),ent->d_name);
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
                for(int evCount = 0; evCount < nEntries; evCount++){
                    tree->GetEntry(evCount);
                    testEv.push_back(DtStruct(false));
                    testEv[testEv.size()-1].time = ev->GetTBTime();
                    Pulse *pulse;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        
                        pixMeans[2][i] += pulse->GetPedestal();
                        pixMeans[3][i] += pulse->GetPedestalRMS();
                        pixMeans[4][i] += pulse->GetAmplitude();
                        pixMeans[5][i] += pulse->GetCharge();
                        pixMeans[6][i] += pulse->GetTimePeak();

                        testEv[testEv.size()-1].data[0] += pulse->GetPedestal();
                        testEv[testEv.size()-1].data[1] += pulse->GetPedestalRMS();
                        testEv[testEv.size()-1].data[2] += pulse->GetAmplitude();
                        testEv[testEv.size()-1].data[3] += pulse->GetCharge();
                        testEv[testEv.size()-1].data[4] += pulse->GetTimePeak();
                        
                        delete pulse;
                    }
                    testEv[testEv.size()-1].Avg();
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
                TH1 *ledDist = new TH1F("hledDist","Amplitudes normalized to camera median",100,0,2);
                for(int evCount = 0; evCount < nEntries; evCount++){
                    vector<Double_t> amps(maxCh);
                    ledDist->Reset();

                    tree->GetEntry(evCount);
                    hledEv.push_back(DtStruct(true));
                    hledEv[hledEv.size()-1].time = ev->GetTBTime();
                    Pulse *pulse;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        
                        pixMeans[0][i] += pulse->GetAmplitude();
                        pixMeans[1][i] += pulse->GetAmplitude();

                        amps[i] = pulse->GetAmplitude();

                        hledEv[hledEv.size()-1].data[0] += pulse->GetAmplitude();
                        
                        delete pulse;
                    }
                    Double_t medianLED = Median(amps);
                    for(int i = 0; i < maxCh; i++){
                        ledDist->Fill(amps[i]/medianLED);
                    }
                    hledEv[hledEv.size()-1].Avg();
                    hledEv[hledEv.size()-1].data[1] =  ledDist->GetStdDev();
                }
                delete ledDist;
                delete ev;
                delete tree;
            }
        }
    }
    int hledEnt = hledEv.size();
    int testEnt = testEv.size();
    for(int i = 0; i < maxCh; i++){
        for(int j = 0; j < 2; j++){
            pixMeans[j][i] /= hledEnt;
        }
        for(int j = 2; j < 7; j++){
            pixMeans[j][i] /= testEnt;
        }
    }
    Double_t medianLED = Median(pixMeans[1]);
    for(int i = 0; i < maxCh; i++){
        pixMeans[1][i] /= medianLED;
    }
    cout << CompareStructTime(*testEv.begin(),*testEv.end()) << endl;
    sort(testEv.begin(),testEv.end(),CompareStructTime);
    sort(hledEv.begin(),hledEv.end(),CompareStructTime);
}

void DataSummary::PlotCamera(int dp){
    if(camera){delete camera;}
    camera = new TH2F("Trinity Camera",hTitles[dp].c_str(),16,-0.5,15.5,16,-0.5,15.5);
    for(int i = 0; i < maxCh; i++){
        int nx, ny;
        FindBin(i,&nx,&ny);
        camera->SetBinContent(nx+1,ny+1,pixMeans[dp][i]);
    }
    camera->SetStats(0);
    vector<Double_t> hRange = {floor(*(min_element(pixMeans[dp].begin(),pixMeans[dp].end()))),floor(*(max_element(pixMeans[dp].begin(),pixMeans[dp].end())))};
    Double_t cushion = (hRange[1] - hRange[0]) * 0.05;
    camera->SetMinimum(hRange[0] - cushion);
    camera->SetMaximum(hRange[1] + cushion);
}

void DataSummary::PlotDt(bool isHLED, int dp){
    vector<DtStruct> *thisVec;
    if(isHLED){
        thisVec = &hledEv;
    }
    else{
        thisVec = &testEv;
    }
    if(ddt){delete ddt;}
    vector<Double_t> yRange = {10000,0};
    for(auto i: (*thisVec)){
        if(i.data[dp] < yRange[0]){
            yRange[0] = i.data[dp];
        }
        else if(i.data[dp] > yRange[1]){
            yRange[1] = i.data[dp];
        }
    }
    Double_t yCushion = (yRange[1] - yRange[0]) * 0.05;
    ddt = new TH2F("Trinity Data over Time", //Name
        dTitles[dp].c_str(), //Title
        thisVec->size()/120, //number of bins on x axis
        (*(*thisVec).begin()).time, //x axis minimum
        (*thisVec).back().time, //x axis maximum
        (yRange[1] - yRange[0] + (2*yCushion)) * 10, //number of bins on y axis
        yRange[0] - yCushion, //y axis minimum
        yRange[1] + yCushion //y axis maximum
    );
    for(auto i: (*thisVec)){
        cout << "t: " << i.time << " val: " << i.data[dp] << endl;
        ddt->Fill(i.time,i.data[dp]);
    }
    cout << (*(*thisVec).begin()).time << endl;
    cout << (*thisVec).back().time << endl;
    ddt->GetXaxis()->SetTimeDisplay(1);
    ddt->GetXaxis()->SetTimeFormat("%H:%M");
    ddt->GetXaxis()->SetTimeOffset(0,"gmt");
    ddt->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
    ddt->SetStats(0);
    ddt->SetMarkerStyle(6);
    ddt->SetMarkerSize(6);
    ddt->SetMarkerColor(1);
}

vector<TH2F> DataSummary::PlotPedestal(){
    PlotCamera(2);
    PlotDt(false,0);
    return {*camera,*ddt};
}