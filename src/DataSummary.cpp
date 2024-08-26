#include "DataSummary.h"

#include "constants.h"
#include "rootils.h"
#include "DtStruct.h"

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>

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
    t_disp = new TCanvas("Display","DataSummary",2500,1000);

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
                    testEv[testEv.size()-1].time = ev->GetTBTime()*1e-8;
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
                    hledEv[hledEv.size()-1].time = ev->GetTBTime()*1e-8;
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

void DataSummary::PlotDt(int dp){
    vector<DtStruct> *thisVec;
    int dpt = dp - (dp >= 2)*2;
    if(dp<2){
        thisVec = &hledEv;
    }
    else{
        thisVec = &testEv;
    }
    if(ddt){delete ddt;}
    //below finds y axis range s.t. it includes 90% of points; purpose is to neglect outliers as opposed to just using min and max 
    vector<int> yRangeInd(2);
    vector<Double_t> valSort;
    for(auto i: (*thisVec)){
        valSort.push_back(i.data[dpt]);
    }
    sort(valSort.begin(),valSort.end());
    int valSize = valSort.size();
    double valInc;
    if(valSize%2 != 0){
        yRangeInd = {valSize/2 - 1,valSize/2 + 1};
        valInc = 3;
    }
    else{
        yRangeInd = {valSize/2 - 1, valSize/2};
        valInc = 2;
    }
    while(valInc < valSize*0.999){
        Double_t up = valSort[yRangeInd[1]+1] - valSort[yRangeInd[1]];
        Double_t down = valSort[yRangeInd[0]] - valSort[yRangeInd[0]-1];
        if(up < down){
            ++yRangeInd[1];
            if(yRangeInd[1] == valSize - 1){
                yRangeInd[0] -= floor(valSize*0.999 - valInc);
                break;
            }
        }
        else{
            --yRangeInd[0];
            if(yRangeInd[0] == 0){
                yRangeInd[1] += floor(valSize*0.999 - valInc);
                break;
            }
        }
        ++valInc;
    }
    vector<Double_t> yRange = {valSort[yRangeInd[0]],valSort[yRangeInd[1]]};
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
        ddt->Fill(i.time,i.data[dpt]);
    }
    ddt->GetXaxis()->SetTimeDisplay(1);
    ddt->GetXaxis()->SetTimeFormat("%H:%M");
    ddt->GetXaxis()->SetTimeOffset(0,"gmt");
    ddt->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
    ddt->SetStats(0);
    ddt->SetMarkerStyle(6);
    ddt->SetMarkerSize(6);
    ddt->SetMarkerColor(1);
}

void DataSummary::PlotAverages(int dp){
    PlotCamera(dp);
    PlotDt(dp);
    if(t_disp){delete t_disp;}
    t_disp = new TCanvas("Display","DataSummary",2500,1000);
    t_disp->Divide(2,1);
    t_disp->cd(1);
    camera->Draw("colz");
    DrawMUSICBoundaries();
    t_disp->cd(1)->SetRightMargin(0.15);
    t_disp->cd(2);
    ddt->Draw("P");
}

void DataSummary::PlotHLED(){
    PlotAverages(0);
}

void DataSummary::PlotHLEDNorm(){
    PlotAverages(1);
}

void DataSummary::PlotPedestal(){
    PlotAverages(2);
}

void DataSummary::PlotPedestalRMS(){
    PlotAverages(3);
}

void DataSummary::PlotAmplitude(){
    PlotAverages(4);
}

void DataSummary::PlotCharge(){
    PlotAverages(5);
}

void DataSummary::PlotTimePeak(){
    PlotAverages(6);
}