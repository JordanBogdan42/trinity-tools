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
		void ReadEv(string readStr);
		void PlotCamera(int data);
		void PlotDt(bool isHLED, int data);
	public:
		DataSummary(char* dateStr);
		TH2F PlotPedestal();
};

#endif