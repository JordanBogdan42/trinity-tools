#ifndef HLEDDtStruct_H
#define HLEDDtStruct_H

#include <TROOT.h>

struct DataDtStruct {
    Double_t amplitude;
    Double_t amplitudeSTD;
    ULong64_t time;
    DataDtStruct();
    void Avg();
}