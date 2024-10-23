#include "constants.h"

#include <string>

using namespace std;

const string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
const string outDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataSummary/Output/";
const int maxCh = 256;
const int binLen = 120;
const vector<string> hTitles = {"Average Amplitude of HLED Events [ADC Counts]","Average Amplitude of HLED Events normalized to median","Average Pedestal [ADC Counts]","Average Pedestal RMS [ADC Counts]","Average Amplitude [ADC Counts]","Average Charge [ADC Counts]","Average Peak Time [Time bins]"};
const vector<string> dTitles = {"Average Amplitude of HLED Events [ADC Counts]","Standard Deviation of Amplitude Distribution","Average Pedestal [ADC Counts]","Average Pedestal RMS [ADC Counts]","Average Amplitude [ADC Counts]","Average Charge [ADC Counts]","Average Peak Time [Time bins]"};
const vector<double> avgVals = {1100,0.15,3783,26,87,229,249};
const vector<int> MUSICmap = {2,3,10,11,18,19,26,27,6,7,14,15,22,23,30,31,0,1,8, 9,16,17,24,25,4,5,12,13,20,21,28,29};