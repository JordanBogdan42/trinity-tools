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
		double avgEv;
		double ampDist;
		double hledMean;
		double hledNMean;
		double pedMean;
		double pedRMSMean;
		double ampMean;
		double qMean;
		double ptMean;
		double psfSigma;
		vector<vector<int>> trTh;
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
		bool isData;
		void ReadEv(string readStr);
		bool isHLED(Event *&ev);
		void AddTestEv(Event *&ev);
		void AddHLEDEv(Event *&ev);
		void ReadTrThresholds(string readStr);
		void FillCamera(int dp);
		void FillDt(int dp);
		void PlotAverages(int dp);
		void FillTrig();
	public:
		TCanvas *t_disp;
		DataSummary(char* dateStr);
		bool hasData();
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
		vector<vector<int>> GetTrTh();
		double GetAvgEv();
		double GetAmpDist();
		double GetHLEDMean();
		double GetHLEDNMean();
		double GetPedMean();
		double GetPedRMSMean();
		double GetqMean();
		double GetPTMean();
		double GetPSFSigma();
};

#endif