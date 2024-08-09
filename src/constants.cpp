#include "constants.h"

#include <string>

using namespace std;

const string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
const string outDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataSummary/Output/";
const int maxCh = 256;
const vector<string> titles = {"Average Pedestal [ADC Counts]","Average Pedestal RMS [ADC Counts]","Average Amplitude [ADC Counts]","Average Charge [ADC Counts]","Average Peak Time [Time bins]","Average Amplitude of HLED Events [ADC Counts]","Standard Deviation of Amplitude Distribution"};