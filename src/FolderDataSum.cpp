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
    if(argc != 2){
        cout << "Usage: ./FolderDataSum YYYYMMDD" << endl;
    }

    string outStr = Form("%s%s",outDir.c_str(),argv[1]);
    DataSummary ds(argv[1]);
    if(ds.hasData()){
        ds.t_disp->Print((outStr+".pdf[").c_str());

        ds.PlotTrig();
        ds.t_disp->Print((outStr+".pdf").c_str());

        ds.PlotROIMusic();
        ds.t_disp->Print((outStr+".pdf").c_str());

        ds.PlotFF();
        ds.t_disp->Print((outStr+".pdf").c_str());
        
        ds.PlotHLED();
        ds.t_disp->Print((outStr+".pdf").c_str());  

        ds.PlotHLEDNorm();
        ds.t_disp->Print((outStr+".pdf").c_str()); 
    
        ds.PlotPedestal();
        ds.t_disp->Print((outStr+".pdf").c_str());    

        ds.PlotPedestalRMS();
        ds.t_disp->Print((outStr+".pdf").c_str()); 

        ds.PlotAmplitude();
        ds.t_disp->Print((outStr+".pdf").c_str());   

        ds.PlotCharge();
        ds.t_disp->Print((outStr+".pdf").c_str()); 

        ds.PlotTimePeak();
        ds.t_disp->Print((outStr+".pdf").c_str());   
        
        ds.PlotPSF();
        ds.t_disp->Print((outStr+".pdf").c_str()); 

        ds.t_disp->Print((outStr+".pdf]").c_str());

        ofstream csvOutput;
        csvOutput.open((outStr+".csv"));
        csvOutput << ds.GetAvgEv() << "," << ds.GetAmpDist() << "," << ds.GetHLEDMean() << "," << ds.GetHLEDNMean() << "," << ds.GetPedMean() << "," << ds.GetPedRMSMean() << "," << ds.GetqMean() << "," << ds.GetPTMean() << "," << ds.GetPSFSigma();
        for(vector<int>& vec : ds.GetTrTh()){
            csvOutput << ",(" << vec[0] << "," << vec[1] << ")";
        }
        csvOutput.close();
        
        system(Form("chmod 660 %s",(outStr+".pdf").c_str()));
        system(Form("chmod 660 %s",(outStr+".csv").c_str()));
    }
    else{
        cout << "No data saved to Test branch on date " << argv[1] << endl;
        ofstream csvOutput;
        csvOutput.open((outStr+".csv"));
        csvOutput << "-1,-1,-1,-1,-1,-1,-1,-1,-1";
        csvOutput.close();
        system(Form("chmod 660 %s",(outStr+".csv").c_str()));
    }
}