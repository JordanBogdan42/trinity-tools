#include "DataSummary.h"

#include "constants.h"
#include "rootils.h"
#include "DtStruct.h"

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TPaveText.h>
#include <TLegend.h>

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
    meanPedRMS = vector<Double_t>(16,0.0);
    fConvolutedFit = new TF1();
    camera = new TH2F();
    ddt = new TH2F();
    addt = new TH2F();
    trig = new TH1F();
    misc1 = new TH1F();
    misc2 = new TGraph();
    lin = new TLine();
    leg = new TLegend();
    pt = new TPaveText();
    t_disp = new TCanvas("Display","DataSummary",2500,1000);

    string evStr = Form("%s%s/RawDataMerged/",dataDir.c_str(),dateStr);
    ReadEv(evStr);
    FillTrig();
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
                    if(isHLED(ev)){AddTestEv(ev);}
                    else{AddHLEDEv(ev);}
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
                for(int evCount = 0; evCount < nEntries; evCount++){
                    tree->GetEntry(evCount);
                    if(isHLED(ev)){AddTestEv(ev);}
                    else{AddHLEDEv(ev);}
                }
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
    for(int i = 0; i < 16; i++){
        meanPedRMS[i] /= testEnt;
    }
    Double_t medianLED = Median(pixMeans[1]);
    for(int i = 0; i < maxCh; i++){
        pixMeans[1][i] /= medianLED;
    }
    sort(testEv.begin(),testEv.end());
    sort(hledEv.begin(),hledEv.end());
}

bool DataSummary::isHLED(Event *&ev){
    Pulse *pulse;
    double ampVal = 0;
    for(int i = 0; i < maxCh; i++){
        pulse = new Pulse(ev->GetSignalValue(i));
        ampVal += pulse->GetAmplitude();
        delete pulse;
    }
    if((ampVal/maxCh) < 350){
        return true;
    }
    return false;
}

void DataSummary::AddTestEv(Event *&ev){
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

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            for(int k = 8; k < 9; k++){
                pulse = new Pulse(ev->GetSignalValue(i*64+j+k*4));
                meanPedRMS[4*i+j] += pulse->GetPedestalRMS();
                delete pulse;
            }
        }
    }

    int MUSICpos = MUSICmap[(ev->GetROIMusicID())[0]];
    testEv[testEv.size()-1].pTrig = MUSICpos*8;
    Double_t ampMax = 0;
    for(int i = 0; i < 8; i++){
        pulse = new Pulse(ev->GetSignalValue(MUSICpos*8 + i));
        if(pulse->GetAmplitude() > ampMax){
            testEv[testEv.size()-1].pTrig = MUSICpos*8 + i;
            ampMax = pulse->GetAmplitude();
        }
        delete pulse;
    }
}

void DataSummary::AddHLEDEv(Event *&ev){
    vector<Double_t> amps(maxCh);
    TH1 *ledDist = new TH1F("hledDist","Amplitudes normalized to camera median",100,0,2);

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
    delete ledDist;
}

void DataSummary::FillCamera(int dp){
    if(camera){delete camera;}
    camera = new TH2F("camera",hTitles[dp].c_str(),16,-0.5,15.5,16,-0.5,15.5);
    for(int i = 0; i < maxCh; i++){
        int nx, ny;
        FindBin(i,&nx,&ny);
        camera->SetBinContent(nx+1,ny+1,pixMeans[dp][i]);
    }
    camera->SetStats(0);
    //below finds histogram scale s.t. it includes 95% of pixels; purpose is to neglect outliers as opposed to just using min and max 
    vector<int> hRangeInd(2);
    vector<Double_t> valSort;
    for(auto i: pixMeans[dp]){
        valSort.push_back(i);
    }
    sort(valSort.begin(),valSort.end());
    int valSize = valSort.size();
    double valInc;
    if(valSize%2 != 0){
        hRangeInd = {valSize/2 - 1,valSize/2 + 1};
        valInc = 3;
    }
    else{
        hRangeInd = {valSize/2 - 1, valSize/2};
        valInc = 2;
    }
    while(valInc < valSize*0.95){
        Double_t up = valSort[hRangeInd[1]+1] - valSort[hRangeInd[1]];
        Double_t down = valSort[hRangeInd[0]] - valSort[hRangeInd[0]-1];
        if(up < down){
            ++hRangeInd[1];
            if(hRangeInd[1] == valSize - 1){
                hRangeInd[0] -= floor(valSize*0.95 - valInc);
                break;
            }
        }
        else{
            --hRangeInd[0];
            if(hRangeInd[0] == 0){
                hRangeInd[1] += floor(valSize*0.95 - valInc);
                break;
            }
        }
        ++valInc;
    }
    vector<Double_t> hRange = {valSort[hRangeInd[0]],valSort[hRangeInd[1]]};
    Double_t cushion = (hRange[1] - hRange[0]) * 0.05;
    camera->SetMinimum(hRange[0] - cushion);
    camera->SetMaximum(hRange[1] + cushion);
}

void DataSummary::FillDt(int dp){
    if(ddt){delete ddt;}
    if(addt){delete addt;}
    if(lin){delete lin;}
    vector<DtStruct> *thisVec;
    int dpt = dp - (dp >= 2)*2;
    if(dp<2){
        thisVec = &hledEv;
    }
    else{
        thisVec = &testEv;
    }
    //below finds y axis range s.t. it includes 99.9% of points; purpose is to neglect outliers as opposed to just using min and max 
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
    if(avgVals[dp] < yRange[0]){yRange[0] = avgVals[dp];}
    else if(avgVals[dp] > yRange[1]){yRange[1] = avgVals[dp];}
    Double_t yCushion = (yRange[1] - yRange[0]) * 0.05;
    ddt = new TH2F("ddt", //Name
        dTitles[dp].c_str(), //Title
        ((*thisVec).back().time - (*(*thisVec).begin()).time)/binLen, //number of bins on x axis
        (*(*thisVec).begin()).time, //x axis minimum
        (*thisVec).back().time, //x axis maximum
        1000, //number of bins on y axis
        yRange[0] - yCushion, //y axis minimum
        yRange[1] + yCushion //y axis maximum
    );
    addt = new TH2F("addt", //Name
        dTitles[dp].c_str(), //Title
        ((*thisVec).back().time - (*(*thisVec).begin()).time)/binLen, //number of bins on x axis
        (*(*thisVec).begin()).time, //x axis minimum
        (*thisVec).back().time, //x axis maximum
        1000, //number of bins on y axis
        yRange[0] - yCushion, //y axis minimum
        yRange[1] + yCushion //y axis maximum
    );
    int count = 0;
    Double_t runAvg = 0.0;
    for(auto i: (*thisVec)){
        runAvg += i.data[dpt];
        ++count;
        ddt->Fill(i.time,i.data[dpt]);
        addt->Fill(i.time,runAvg/count);
    }

    lin=new TLine((*(*thisVec).begin()).time, //x1
        avgVals[dp], //y1
        (*thisVec).back().time, //x2
        avgVals[dp] //y2
    );
}

void DataSummary::PlotAverages(int dp){
    if(leg){delete leg;}
    FillCamera(dp);
    FillDt(dp);
    if(t_disp){delete t_disp;}
    t_disp = new TCanvas("Display","DataSummary",2500,1000);
    t_disp->Divide(2,1);

    t_disp->cd(1);
    camera->Draw("colz");
    DrawMUSICBoundaries();
    t_disp->cd(1)->SetRightMargin(0.15);

    t_disp->cd(2);

    ddt->GetXaxis()->SetTimeDisplay(1);
    ddt->GetXaxis()->SetTimeFormat("%H:%M");
    ddt->GetXaxis()->SetTimeOffset(0,"gmt");
    ddt->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
    ddt->SetStats(0);
    ddt->SetMarkerStyle(6);
    ddt->SetMarkerSize(6);
    ddt->SetMarkerColor(1);
    
    addt->SetStats(0);
    addt->SetMarkerStyle(6);
    addt->SetMarkerSize(6);
    addt->SetMarkerColor(6);

    lin->SetLineColor(kGreen);
    lin->SetLineColorAlpha(kGreen,0.6);
    lin->SetLineWidth(4);

    ddt->Draw("P");
    addt->Draw("P,SAME");
    lin->Draw("SAME");

    leg = new TLegend(0.1,0.90,0.9,0.94);
    leg->SetNColumns(4);
    leg->AddEntry(ddt,"Data Points","p");
    leg->AddEntry(addt,"Running Avg","p");
    leg->AddEntry(lin,"Expected Avg","l");
    leg->Draw("SAME");
}

void DataSummary::FillTrig(){
    if(trig){delete trig;}
    trig = new TH1F("trig", //Name
        "Number of Events", //Title
        (testEv.back().time - (*testEv.begin()).time)/binLen, //number of bins on x axis
        (*testEv.begin()).time, //x axis minimum
        testEv.back().time //x axis maximum
    );
    for(auto i: testEv){
        trig->Fill(i.time);
    }
    trig->GetXaxis()->SetTimeDisplay(1);
    trig->GetXaxis()->SetTimeFormat("%H:%M");
    trig->GetXaxis()->SetTimeOffset(0,"gmt");
    trig->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
    trig->GetYaxis()->SetTitle("Events");
    trig->SetStats(0);
    trig->SetMarkerStyle(6);
    trig->SetMarkerSize(6);
    trig->SetMarkerColor(1);
}

void DataSummary::PlotTrig(){
    if(t_disp){delete t_disp;}
    if(misc1){delete misc1;}
    t_disp = new TCanvas("Display","DataSummary",2500,1000);
    t_disp->Divide(2,1);
    t_disp->cd(1);
    t_disp->cd(1)->SetLogy();
    trig->SetAxisRange(1,4000,"Y");
    trig->Draw("P");
    t_disp->cd(2);
    misc1 = (TH1F*)trig->Clone("misc1");
    misc1->SetTitle("Trigger Rate [Events/s]");
    misc1->Scale((Double_t)1/binLen);
    misc1->SetAxisRange(0,35,"Y");
    misc1->GetYaxis()->SetTitle("Trigger rate [events/s]");
    misc1->Draw("P");
}

void DataSummary::PlotROIMusic(){
    if(t_disp){delete t_disp;}
    if(camera){delete camera;}
    if(ddt){delete ddt;}

    camera = new TH2F("pixHeat","Highest Amplitude Pixels in Triggered Music [Counts]",16,-0.5,15.5,16,-0.5,15.5);
    ddt = new TH2F("musicHeat","Triggered Music [Counts]",8,-0.5,15.5,4,-0.5,15.5);
    for(auto i: testEv){
        int nx, ny;
		FindBin(i.pTrig,&nx,&ny);
		camera->Fill(nx,ny);
		ddt->Fill(nx,ny);
    }

    t_disp = new TCanvas("Display","DataSummary",2500,1000);
    t_disp->Divide(2,1);
    t_disp->cd(1);
	camera->Draw("colz");
	DrawMUSICBoundaries();
	camera->SetStats(0);
	t_disp->cd(1)->SetRightMargin(0.15);
	t_disp->cd(2);
	ddt->Draw("colz");
	DrawMUSICBoundaries();
	ddt->SetStats(0);
	t_disp->cd(2)->SetRightMargin(0.15);
}

void DataSummary::PlotFF(){
    if(t_disp){delete t_disp;}
    if(misc1){delete misc1;}
    t_disp = new TCanvas("Display","DataSummary",1250,1000);
    misc1 = new TH1F("misc1", //Name
        "Distribution of Daily Average HLED Amplitude normalized to median", //Title
        150, //number of bins on x axis
        0, //x axis minimum
        1499 //x axis maximum
    );
    for(auto i: pixMeans[0]){
        misc1->Fill(i);
    }
    misc1->GetXaxis()->SetTitle("HLED signal amplitude normalized to median");
    misc1->GetYaxis()->SetTitle("Number of pixels");
    gStyle->SetOptStat(1100);
    t_disp->cd();
    misc1->Draw("hist");
    /*TPaveStats *st = (TPaveStats*)t_disp->GetPrimitive("stats");
    st->AddText("aaa = 33");
    cout << "3" << endl;
    st->DrawClone();
    cout << "4" << endl;*/
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

void DataSummary::PlotPSF(){
    if(t_disp){delete t_disp;}
    if(misc1){delete misc1;}
    if(misc2){delete misc2;}
    if(fConvolutedFit){delete fConvolutedFit;}
    if(pt){delete pt;}
    t_disp = new TCanvas("Display","DataSummary",1250,1000);
    misc1 = new TH1F("misc1", //Name
        "Average Pedestal RMS per Row for pixel column 8", //Title
        16, //number of bins on x axis
        0, //x axis minimum
        16 //x axis maximum
    );
    misc2 = new TGraph(16);

    for(int i = 0; i < 16; i ++){
        misc1->SetBinContent(i+1,meanPedRMS[i]);
        misc2->SetPoint(i,(i+0.5),meanPedRMS[i]);
    }

    fConvolutedFit = new TF1("fConvolutedFit", //Name
        ConvolutedRMSFunction, //Formula
        0, //x axis minimum
        16, //x axis maximum
        4 //number of free parameters (?)
    );
    // Initial parameters: [0]=offset, [1]=swing, [2]=mean (mu), [3]=sigma
    fConvolutedFit->SetParameters(20, 10, 8, 2); // Adjust initial parameters accordingly
    // Set limits for the parameters
    fConvolutedFit->SetParLimits(0, 0, 100); // Limits for offset
    fConvolutedFit->SetParLimits(1, 0, 100); // Limits for swing
    fConvolutedFit->SetParLimits(2, 0, 16); // Limits for mu
    fConvolutedFit->SetParLimits(3, 0, 5); // Limits for sigma
    fConvolutedFit->SetParNames("Offset", "Swing", "Mu", "Sigma");
    // Restrict the fit range using SetRange
    fConvolutedFit->SetRange(0, 9);// * 0.3); // Set range from 0 to 8 * 0.3
    // Fit the function to the data in the graph
    misc2->Fit(fConvolutedFit, "R"); // "R" for restricted fit

    misc1->GetXaxis()->SetTitle("Camera Row");
    misc1->GetXaxis()->SetNdivisions(16);
    misc1->GetYaxis()->SetTitle("Average Pedestal RMS [ADC counts]");
    misc1->SetLineWidth(1);
    misc1->SetLineColor(kBlack);
    misc1->SetStats(0);
    
    misc2->SetMarkerStyle(20);
    misc2->SetMarkerColor(kBlack);
    misc2->SetLineColor(kBlack);
    misc2->SetLineWidth(1);

    // Optionally draw the fitted function on the same canvas
    fConvolutedFit->SetLineColor(kRed);
    fConvolutedFit->Draw("same");

    Double_t offset = fConvolutedFit->GetParameter(0);
    Double_t offsetError = fConvolutedFit->GetParError(0);
    Double_t swing = fConvolutedFit->GetParameter(1);
    Double_t swingError = fConvolutedFit->GetParError(1);
    Double_t mu = fConvolutedFit->GetParameter(2);
    Double_t muError = fConvolutedFit->GetParError(2);
    Double_t sigma = fConvolutedFit->GetParameter(3);
    Double_t sigmaError = fConvolutedFit->GetParError(3);
    Double_t chi2 = fConvolutedFit->GetChisquare();
    Int_t ndf = fConvolutedFit->GetNDF(); // Number of degrees of freedom
    pt = new TPaveText(0.6, 0.6, 0.9, 0.9, "NDC"); // NDC: Normalized Device Coordinates
    pt->SetFillColor(0); // Transparent background
    pt->SetTextAlign(12); // Align left
	pt->AddText(Form("Offset: %.3f +/- %.3f [ADC counts]", offset, offsetError));
    pt->AddText(Form("Swing: %.3f +/- %.3f [ADC counts]", swing, swingError));
    pt->AddText(Form("Mu: %.3f +/- %.3f", mu, muError));
    pt->AddText(Form("Sigma: %.3f +/- %.3f", sigma, sigmaError));
    pt->AddText(Form("#chi^{2}/ndf: %.2f / %d", chi2, ndf));

    t_disp->cd();
    misc1->Draw("hist");
    misc2->Draw("P same");
    pt->Draw();
}