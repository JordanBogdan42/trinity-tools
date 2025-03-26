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
const vector<int> MUSICmap = {3,2,11,10,19,18,27,26,7,6,15,14,23,22,31,30,1,0,9,8,17,16,25,24,5,4,13,12,21,20,29,28};
const string trStr = "200000ffff250300"; //first 16 bits of 18 bit hex command for setting trigger threshold, last 2 bits are the threshold value