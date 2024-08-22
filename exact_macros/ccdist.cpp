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

Double_t Median(vector<Double_t> v);
Double_t RMS(vector<Double_t> v);

void ccdist(string dateStr){

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
                for(int evCount = 0; evCount < nEntries; evCount++){
                    tree->GetEntry(evCount);
                    Pulse *pulse;
                    vector<Double_t> evAvg(maxCh);
                    bool qplt = true;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        evAvg[i] = pulse->GetAmplitude();
                        if((ev->GetSignalValue(i))[pulse->GetTimePeak()] == 0){
                            qplt = true;
                            cout << "Test ev " << evCount << " saturated pixel " << i << endl;
                        }
                        delete pulse;
                    }
                    if(qplt){
                        dist->Fill(Median(evAvg));
                        plot->Fill(Median(evAvg),RMS(evAvg)/Median(evAvg));
                    }
                    if(Median(evAvg) > 220 &&  Median(evAvg) < 340){
                        cout << "file: " << fileStr << " ev: " << evCount << endl;
                        cout << "rms: " << RMS(evAvg) << endl;
                    }
                }
                delete ev;
                delete tree;

                tree = (TTree*)f0->Get("HLED");
                ev = new Event();
                tree->SetBranchAddress("Events", &ev);
                nEntries = tree->GetEntries();
                if(nEntries == 0){
                    cout << "File has no data in the \"HLED\" branch..." << endl;
                }
                cout << "\"HLED\" Events: " << nEntries << endl;
                for(int evCount = 0; evCount <nEntries; evCount++){
                    tree->GetEntry(evCount);
                    Pulse *pulse;
                    vector<Double_t> evAvg(maxCh);
                    bool qplt = true;
                    for(int i = 0; i < maxCh; i++){
                        pulse = new Pulse(ev->GetSignalValue(i));
                        evAvg[i] = pulse->GetAmplitude();
                        if((ev->GetSignalValue(i))[pulse->GetTimePeak()] == 0){
                            qplt = true;
                            cout << "HLED ev " << evCount << " saturated pixel " << i << endl;
                        }
                        delete pulse;
                    }
                    if(qplt){
                        dist->Fill(Median(evAvg));
                        plot->Fill(Median(evAvg),RMS(evAvg)/Median(evAvg));
                    }
                    if(Median(evAvg) > 220 && Median(evAvg) < 340){
                        cout << "file: " << fileStr << " ev: " << evCount << endl;
                        cout << "rms: " << RMS(evAvg) << endl;
                    }
                }
                delete ev;
                delete tree;
            }
        }
    }
    TCanvas *c_disp = new TCanvas("Display","Dist",2500,1000);
    c_disp->Divide(2);
    c_disp->cd(1);
    dist->Draw("hist");
    c_disp->cd(1)->SetLogy();
    dist->GetXaxis()->SetTitle("Median amplitude of camera per event [ADC]");
    c_disp->cd(2);
    plot->Draw("colz");
    c_disp->cd(2)->SetRightMargin(0.15);
    plot->GetXaxis()->SetTitle("Median amplitude of camera per event [ADC]");
    plot->GetYaxis()->SetTitle("Amplitude RMS/Median amplitude");
    plot->SetStats(0);
    //c_disp->Print(Form("%s.png",dateStr.c_str()));
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