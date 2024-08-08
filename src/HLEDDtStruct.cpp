#include "HLEDDtStruct.h"

#include "constants.h"

#include <TROOT.h>

HLEDDtStruct::HLEDDtStruct() {
    amplitude = 0.0;
    amplitudeSTD = 0.0;
    time = 0;
}

void HLEDDtStruct::Avg() {
    amplitude /= maxCh;
}