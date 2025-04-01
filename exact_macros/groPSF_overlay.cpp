#include <TH2.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <sstream>
#include <iostream>
#include <fstream>


void DrawMUSICBoundaries()
{
        TBox *b = new TBox(-3,-3,9,21);
        b->SetFillStyle(0);
        b->SetLineColor(kRed);
        for(int i=0; i<64; i++)
        {
                TBox *bn = (TBox*)b->Clone();
                bn->SetX1((i%16)*12-96);
                bn->SetX2((i%16)*12-84);
                bn->SetY1((i/16)*24-48);
                bn->SetY2((i/16)*24-24);
                bn->Draw();
        }
}

TGraph *groPSF(const char* filename)
{
        int nResMult = 100;//3*2*2;

        std::vector<float>* vPhotonX = 0;
        std::vector<float>* vPhotonY = 0;

        TFile* file = TFile::Open(filename, "READ");
        TTree* tree = (TTree*)file->Get("T1");
        tree->SetBranchAddress("photonX", &vPhotonX);
        tree->SetBranchAddress("photonY", &vPhotonY);

        tree->GetEntry(0);

        float avgPhotonX = std::accumulate(vPhotonX->begin(),vPhotonX->end(),0.0)/vPhotonX->size();
        float avgPhotonY = std::accumulate(vPhotonY->begin(),vPhotonY->end(),0.0)/vPhotonY->size();
        std::vector<float> vPhotonR((int)vPhotonX->size());

        for (size_t j = 0; j < vPhotonX->size(); j++)
        {
                vPhotonR[j] = std::sqrt(std::pow((*vPhotonX)[j] - avgPhotonX,2)+std::pow((*vPhotonY)[j] - avgPhotonY,2));
        }

        int numSt = 1000;
        std::vector<int> vPhoton(numSt);
        TGraph *gPhotonPSF = new TGraph(numSt);
        
        for(int i = 0; i < numSt; i++){
                vPhoton[i] = std::count_if(vPhotonR.begin(),vPhotonR.end(),[i](float f){return f <= (float)i/100;});
                gPhotonPSF->SetPoint(i,(Double_t)i/100,(Double_t)vPhoton[i]/vPhotonX->size());
        }
        file->Close();
        return gPhotonPSF;
}

void groPSF_overlay()
{
        TGraph *psfSim2f = groPSF("/storage/home/hhive1/jbogdan3/data/Sims/Output/gropsf2f.root");
        psfSim2f->SetLineWidth(3);
        psfSim2f->SetLineColor(3);

        /*TGraph *psfSimHome = groPSF("/storage/home/hhive1/jbogdan3/data/Sims/Output/gropsfhome20241120.root");
        psfSimHome->SetLineWidth(3);
        psfSimHome->SetLineColor(4);*/

        TGraph *psfMeas2f = new TGraph(100);
        psfMeas2f->SetLineWidth(3);
        psfMeas2f->SetLineColor(2);
        std::ifstream CSVFile("/storage/home/hhive1/jbogdan3/data/Sims/Output/20241012PSF.csv");
        for(int i = 0; i < 100; i++){
                std::string txt1,txt2;
                getline(CSVFile,txt1,',');
                getline(CSVFile,txt2);
                std::cout << txt1 << " " << txt2 << std::endl;
                psfMeas2f->SetPoint(i,std::stof(txt1),std::stof(txt2));
        }
        CSVFile.close();

        TCanvas *can = new TCanvas("canvas","can",2000,2000);
        can->cd();
        psfSim2f->SetTitle("Simulated vs Measured PSF of Trinity Demonstrator");
        psfSim2f->GetXaxis()->SetTitle("Radial distance from brightest point in image [mm]");
        psfSim2f->GetYaxis()->SetTitle("Photons in radius/total");
        psfSim2f->Draw("ac");
        //psfSimHome->Draw("c");
        psfMeas2f->Draw("c");
        TLegend *legend = new TLegend(0.5,0.1,0.9,0.2);
        legend->AddEntry(psfSim2f,"Simulated PSF with source and target at 2f point","l");
        //legend->AddEntry(psfSimHome,"Simulated PSF with parallel point source","l");
        legend->AddEntry(psfMeas2f,"Measured PSF with source and target at 2f point","l");
        legend->Draw();
        can->Print("groPSF_overlay.png");

}