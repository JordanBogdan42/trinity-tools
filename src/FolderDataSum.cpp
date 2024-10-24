#include "DataSummary.h"

#include "rootils.h"
#include "constants.h"

#include <TCanvas.h>
#include <TH2.h>

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv){
    if(argc != 1){
        cout << "Usage: ./FolderDataSum YYYYMMDD" << endl;
    }
    string outStr = Form("%s%s",outDir.c_str(),argv[1]);
    DataSummary ds(argv[1]);
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
    system(Form("chmod 660 %s",(outStr+".pdf").c_str()));
}