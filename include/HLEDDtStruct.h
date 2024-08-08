#ifndef HLEDDtStruct_H
#define HLEDDtStruct_H

#include <TROOT.h>

struct HLEDDtStruct {
    Double_t amplitude;
    Double_t amplitudeSTD;
    ULong64_t time;
    HLEDDtStruct();
    void Avg();
};

#endif