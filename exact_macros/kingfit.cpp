#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <sstream>
#include <iostream>
#include <fstream>

void kingfit()
{
        TGraph *psfMeas2f = new TGraph(100);
        psfMeas2f->SetLineWidth(4);
        psfMeas2f->SetLineColor(4);
        psfMeas2f->SetMarkerStyle(22);
        psfMeas2f->SetMarkerSize(4);
        std::ifstream CSVFile("/storage/home/hhive1/jbogdan3/data/Sims/Output/20241012PSF_abs.csv");
        std::vector<float> bri(100);
        std::vector<float> rad(100);
        for(int i = 0; i < 100; i++){
                std::string txt1,txt2;
                getline(CSVFile,txt1,',');
                getline(CSVFile,txt2);
                if(i == 0 || i == 2){
                        continue;
                }
                float pden = std::stof(txt2);
                pden /= std::stof(txt1)*std::stof(txt1)*M_PI;
                //psfMeas2f->SetPoint(psfMeas2f->GetN(),std::stof(txt1),pden);
                bri[i] = pden;
                rad[i] = std::stof(txt1);
        }
        CSVFile.close();
        float bris = std::accumulate(bri.begin(),bri.end(),0);
        for(int i = 0; i < 100; i++){
                if(i == 0 || i == 2){
                        continue;
                }
                psfMeas2f->SetPoint(psfMeas2f->GetN(),rad[i],bri[i]/bris);
        }

        TF1 *kFit = new TF1("kFit","(1/(2*3.14159*([0]^2)))*(1-(1/[1]))*(1+(x^2/(2*([0]^2)*[1])))^(-[1])",0.1,20);
        //TF1 *kFit = new TF1("kFit","[4]*((1/(2*3.14159*([0]^2)))*(1-(1/[1]))*(1+(x^2/(2*([0]^2)*[1])))^(-[1]))+(1-[4])*((1/(2*3.14159*([2]^2)))*(1-(1/[3]))*(1+(x^2/(2*([2]^2)*[3])))^(-[3]))",0.2,10);
        kFit->SetParameter(0,1);
        kFit->SetParLimits(0,0,2);
        kFit->SetParameter(1,5);
        //kFit->SetParLimits(1,0.01,10000);
        /*kFit->SetParameter(2,2);
        kFit->SetParLimits(2,1.9,2.1);
        kFit->SetParameter(3,1000);
        kFit->SetParameter(4,0.5);
        kFit->SetParLimits(4,0.01,0.99);*/
        psfMeas2f->Fit(kFit);
        kFit->SetLineWidth(4);
        kFit->SetLineColor(2);

        TCanvas *can = new TCanvas("canvas","can",2300,2000);
        can->cd();
        psfMeas2f->SetTitle("Measured PSF of Trinity Demonstrator");
        psfMeas2f->GetXaxis()->SetTitle("Radial distance from brightest point in image [mm]");
        psfMeas2f->GetYaxis()->SetTitle("Photons in radius/total/area [mm^{-2}]");
        psfMeas2f->Draw("ap");
        kFit->Draw("SAME");
        TLegend *legend = new TLegend(0.1,0.1,0.3,0.2);
        legend->AddEntry(psfMeas2f,"Measured PSF","l");
        legend->AddEntry(kFit,"King's Function fit","l");
        legend->SetTextSize(0.02);
        legend->Draw();
        gPad->SetLogx();
        gPad->SetLogy();
        can->Print("psfKingFit.png");
}