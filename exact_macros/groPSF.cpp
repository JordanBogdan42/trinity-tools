#include <TH2.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>

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

void groPSF(const char* filename)
{
        TCanvas *canvas = new TCanvas("Display","Event Display",4000,2000);
        canvas->Divide(2,1);
        int nResMult = 100;//3*2*2;

        TH2F* hPhotonDist = new TH2F("hist", "Camera View", 96, -48, 48, 96, -48, 48);
        hPhotonDist->GetXaxis()->SetTitle("X (mm)");
        hPhotonDist->GetYaxis()->SetTitle("Y (mm)");
        hPhotonDist->SetStats(0);

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
                hPhotonDist->Fill((*vPhotonX)[j], (*vPhotonY)[j]);
                vPhotonR[j] = std::sqrt(std::pow((*vPhotonX)[j] - avgPhotonX,2)+std::pow((*vPhotonY)[j] - avgPhotonY,2));
        }

        int numSt = 1000;
        std::vector<int> vPhoton(numSt);
        TGraph *gPhotonPSF = new TGraph(numSt);
        gPhotonPSF->SetTitle("PSF at 2f point;radial distance from center [mm];fraction of photons in radius");
        gPhotonPSF->SetLineWidth(3);
        gPhotonPSF->SetLineColor(4);
        float psfval = 0;
        
        for(int i = 0; i < numSt; i++){
                vPhoton[i] = std::count_if(vPhotonR.begin(),vPhotonR.end(),[i](float f){return f <= (float)i/100;});
                gPhotonPSF->SetPoint(i,(Double_t)i/100,(Double_t)vPhoton[i]/vPhotonX->size());
                if(psfval == 0 && (Double_t)vPhoton[i]/vPhotonX->size() >= 0.8){psfval = (float)i/100;}
        }
        TLine psfline(psfval,0,psfval,1.05);
        psfline.SetLineWidth(5);
        psfline.SetLineColor(2);
        psfline.SetLineStyle(9);

        canvas->cd(1);
        DrawMUSICBoundaries();
        hPhotonDist->SetMarkerSize(200.0);
        hPhotonDist->Draw("colz");
        canvas->cd(2);
        gPhotonPSF->Draw();
        psfline.Draw();
        TLegend *legend = new TLegend(0.6,0.1,0.9,0.15);
        legend->AddEntry(&psfline,Form("80%% brightness at radius %f mm",psfval),"lp");
        legend->Draw();
        file->Close();
        canvas->Print("groPSF.png");
}
