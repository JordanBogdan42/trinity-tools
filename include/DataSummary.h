#ifndef DataSummary_H
#define DataSummary_H

using namespace std;

class DataSummary {
	private:
		vector<vector<ULong64_t>> evTimes;
		vector<vector<Double_t>> evMeans;
		vector<vector<Double_t>> pixMeans;
		void ReadEv(string readStr);
	public:
		DataSummary(string dateStr);
}
