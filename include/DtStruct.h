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
    Int_t pTrig;
    ULong64_t time;
    DtStruct(bool isHLED);
    void Avg();
    bool operator < (const DtStruct &other);
    private:
        bool truHLED;
};

#endif