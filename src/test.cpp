#include "DataSummary.h"

#include "rootils.h"

#include <TCanvas.h>
#include <TH2.h>

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv){
    DataSummary test(argv[1]);
    test.t_disp->Print("test.pdf[");
    test.PlotHLED();
    test.t_disp->Print("test.pdf");
    test.PlotHLEDNorm();
    test.t_disp->Print("test.pdf");
    test.PlotPedestal();
    test.t_disp->Print("test.pdf");
    test.PlotPedestalRMS();
    test.t_disp->Print("test.pdf");
    test.PlotAmplitude();
    test.t_disp->Print("test.pdf");
    test.PlotCharge();
    test.t_disp->Print("test.pdf");
    test.PlotTimePeak();
    test.t_disp->Print("test.pdf");
    test.t_disp->Print("test.pdf]");
}