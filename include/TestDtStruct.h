#ifndef TestDtStruct_H
#define TestDtStruct_H

#include <TROOT.h>

struct TestDtStruct {
    Double_t pedestal;
    Double_t pedestalRMS;
    Double_t amplitude;
    Double_t charge;
    Double_t timePeak;
    ULong64_t time;
    TestDtStruct();
    void Avg();
};

#endif