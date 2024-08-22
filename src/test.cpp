#include "DataSummary.h"

#include "rootils.h"

#include <TCanvas.h>
#include <TH2.h>

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv){
    TCanvas *c_disp = new TCanvas("Display","CameraPlot",2500,1000);
    c_disp->Divide(2,1);
    DataSummary test(argv[1]);
    vector<TH2F> testH = test.PlotPedestal();
    c_disp->cd(1);
    testH[0].Draw("colz");
    DrawMUSICBoundaries();
    c_disp->cd(1)->SetRightMargin(0.15);
    c_disp->cd(2);
    testH[1].Draw("P");
    c_disp->Print("test.png");
}