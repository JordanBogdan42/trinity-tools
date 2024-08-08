#include "TestDtStruct.h"

#include "constants.h"

#include <TROOT.h>

TestDtStruct::TestDtStruct() {
    pedestal = 0.0;
    pedestalRMS = 0.0;
    amplitude = 0.0;
    charge = 0.0;
    timePeak = 0.0;
    time = 0;
}

void TestDtStruct::Avg() {
    pedestal /= maxCh;
    pedestalRMS /= maxCh;
    amplitude /= maxCh;
    charge /= maxCh;
    timePeak /= maxCh;
}