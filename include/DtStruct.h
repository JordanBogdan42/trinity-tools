#ifndef DtStruct_H
#define DtStruct_H

#include <vector>

#include <TROOT.h>

using namespace std;

struct DtStruct {
    // Double_t pedestal;
    // Double_t pedestalRMS;
    // Double_t amplitude;
    // Double_t charge;
    // Double_t timePeak;
    vector<Double_t> data;
    ULong64_t time;
    DtStruct(bool isHLED);
    void Avg();
    private:
        bool truHLED;
};

#endif