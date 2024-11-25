#include "DataSummary.h"

#include "rootils.h"
#include "constants.h"

#include <TCanvas.h>
#include <TH2.h>

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char **argv){
    if(argc != 1){
        cout << "Usage: ./FolderDataSum YYYYMMDD" << endl;
    }

    std::ofstream csvOutput;
    csvOutput.open(Form("%s%s.csv",outDir.c_str(),argv[1]));

    string outStr = Form("%s%s",outDir.c_str(),argv[1]);
    DataSummary ds(argv[1]);
    ds.t_disp->Print((outStr+".pdf[").c_str());

    ds.PlotTrig();
    ds.t_disp->Print((outStr+".pdf").c_str());

    ds.PlotROIMusic();
    ds.t_disp->Print((outStr+".pdf").c_str());

    ds.PlotFF();
    ds.t_disp->Print((outStr+".pdf").c_str());
    ds.t_disp->ls();
    TH1F *avgTH1 = (TH1F*)gROOT->FindObject("misc1");
    double stats[4];
    avgTH1->GetStats(stats); // Fill the stats array
    double sumWeights = stats[0];        // Sum of weights (sum of bin contents)
    double sumWeightsX = stats[2];      // Sum of weights * x-coordinates
    double sumWeightsX2 = stats[3];     // Sum of weights * x^2-coordinates
    // Calculate mean
    double mean = (sumWeights > 0) ? (sumWeightsX / sumWeights) : 0.0;
    // Calculate variance and standard deviation
    double variance = (sumWeights > 0) ? (sumWeightsX2 / sumWeights - mean * mean) : 0.0;
    double std = (variance > 0) ? std::sqrt(variance) : 0.0;
    // Output to CSV
    csvOutput << std/mean << ","; // Output the mean and standard deviation

    ds.PlotHLED();
    ds.t_disp->Print((outStr+".pdf").c_str());
    TH2F *avgTH2 = (TH2F*)gROOT->FindObject("ddt");
    csvOutput << avgTH2->GetMean(2) << ",";    

    ds.PlotHLEDNorm();
    ds.t_disp->Print((outStr+".pdf").c_str());
    avgTH2 = (TH2F*)gROOT->FindObject("ddt");
    csvOutput << avgTH2->GetMean(2) << ",";    
   

    ds.PlotPedestal();
    ds.t_disp->Print((outStr+".pdf").c_str());
    avgTH2 = (TH2F*)gROOT->FindObject("ddt");
    csvOutput << avgTH2->GetMean(2) << ",";     

    ds.PlotPedestalRMS();
    ds.t_disp->Print((outStr+".pdf").c_str());
    avgTH2 = (TH2F*)gROOT->FindObject("ddt");
    csvOutput << avgTH2->GetMean(2) << ",";    

    ds.PlotAmplitude();
    ds.t_disp->Print((outStr+".pdf").c_str());
    avgTH2 = (TH2F*)gROOT->FindObject("ddt");
    csvOutput << avgTH2->GetMean(2) << ",";    

    ds.PlotCharge();
    ds.t_disp->Print((outStr+".pdf").c_str());
    avgTH2 = (TH2F*)gROOT->FindObject("ddt");
    csvOutput << avgTH2->GetMean(2) << ",";  

    ds.PlotTimePeak();
    ds.t_disp->Print((outStr+".pdf").c_str());
    avgTH2 = (TH2F*)gROOT->FindObject("ddt");
    csvOutput << avgTH2->GetMean(2);    

    
    ds.PlotPSF();
    ds.t_disp->Print((outStr+".pdf").c_str());
    // ds.t_disp->ls();
    // avgTH1 =(TH1F*)gROOT->FindObject("misc1");
    // csvOutput << avgTH1->GetMean(2);   

    ds.t_disp->Print((outStr+".pdf]").c_str());
    csvOutput.close();
    
    system(Form("chmod 660 %s",(outStr+".pdf").c_str()));
    system(Form("chmod 660 %s",(outStr+".csv").c_str()));
    cout << "csv file " << Form("%s%s.csv",outDir.c_str(),argv[1]) << endl;

    
}