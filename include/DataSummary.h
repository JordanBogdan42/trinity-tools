#ifndef DataSummary_H
#define DataSummary_H

#include "DtStruct.h"

#include <TROOT.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TLegend.h>
#include <TPaveText.h>

#include <Event.h>

#include <vector>
#include <cmath>

using namespace std;

class DataSummary {
	private:
		vector<DtStruct> testEv;
		vector<DtStruct> hledEv;
		vector<vector<Double_t>> pixMeans;
		vector<Double_t> meanPedRMS;
		TF1 *fConvolutedFit;
		TH2F *camera;
		TH2F *ddt;
		TH2F *addt;
		TH1F *trig;
		TH1F *misc1;
		TGraph *misc2;
		TLine *lin;
		TLegend *leg;
		TPaveText *pt;
		void ReadEv(string readStr);
		bool isHLED(Event *&ev);
		void AddTestEv(Event *&ev);
		void AddHLEDEv(Event *&ev);
		void FillCamera(int dp);
		void FillDt(int dp);
		void PlotAverages(int dp);
		void FillTrig();
	public:
		TCanvas *t_disp;
		DataSummary(char* dateStr);
		void PlotTrig();
		void PlotROIMusic();
		void PlotFF();
		void PlotHLED();
		void PlotHLEDNorm();
		void PlotPedestal();
		void PlotPedestalRMS();
		void PlotAmplitude();
		void PlotCharge();
		void PlotTimePeak();
		void PlotPSF();
};

#endif