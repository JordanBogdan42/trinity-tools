#include "DataSummary.h"

#include <TCanvas.h>
#include <TH2.h>

#include <iostream>

using namespace std;

int main(int argc, char **argv){
    TCanvas *c_disp = new TCanvas("Display","CameraPlot",1250,1000);
    DataSummary test(argv[1]);
    TH2F testH = test.PlotPedestal();
    testH.SetMinimum(3500);
    testH.SetMaximum(4000);
    testH.Draw("colz");
    c_disp->Print("test.png");
}