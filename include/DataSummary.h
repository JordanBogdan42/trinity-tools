#ifndef DataSummary_H
#define DataSummary_H

#include <vector>
#include <cmath>

using namespace std;

class DataSummary {
	private:
		vector<vector<ULong64_t>> evTimes;
		vector<vector<Double_t>> evMeans;
		vector<vector<Double_t>> pixMeans;
		void ReadEv(string readStr);
	public:
		DataSummary(char* dateStr);
};

#endif