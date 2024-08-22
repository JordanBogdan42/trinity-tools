#ifndef DataSummary_H
#define DataSummary_H

#include "DtStruct.h"

#include <TROOT.h>
#include <TH2.h>

#include <vector>
#include <cmath>

using namespace std;

class DataSummary {
	private:
		vector<DtStruct> testEv;
		vector<DtStruct> hledEv;
		vector<vector<Double_t>> pixMeans;
		TH2F *camera;
		TH2F *ddt;
		void ReadEv(string readStr);
		void PlotCamera(int dp);
		void PlotDt(bool isHLED, int dp);
	public:
		DataSummary(char* dateStr);
		vector<TH2F> PlotPedestal();
};

#endif