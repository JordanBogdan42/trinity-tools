#ifndef DataSummary_H
#define DataSummary_H

#include "TestDtStruct.h"
#include "HLEDDTStruct.h"

#include <TROOT.h>

#include <vector>
#include <cmath>

using namespace std;

class DataSummary {
	private:
		vector<TestDtStruct> testEv;
		vector<HLEDDtStruct> hledEv;
		vector<vector<Double_t>> pixMeans;
		void ReadEv(string readStr);
	public:
		DataSummary(char* dateStr);
};

#endif