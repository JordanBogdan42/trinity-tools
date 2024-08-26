#ifndef DataSummary_H
#define DataSummary_H

#include "DtStruct.h"

#include <TROOT.h>
#include <TH2.h>
#include <TCanvas.h>

#include <vector>
#include <cmath>

using namespace std;

class DataSummary {
	private:
		vector<DtStruct> testEv;
		vector<DtStruct> hledEv;
		vector<vector<Double_t>> pixMeans;
		void ReadEv(string readStr);
		void PlotCamera(int dp);
		void PlotDt(int dp);
		void PlotAverages(int dp);
	public:
		TH2F *camera;
		TH2F *ddt;
		TCanvas *t_disp;
		DataSummary(char* dateStr);
		void PlotHLED();
		void PlotHLEDNorm();
		void PlotPedestal();
		void PlotPedestalRMS();
		void PlotAmplitude();
		void PlotCharge();
		void PlotTimePeak();
};

#endif