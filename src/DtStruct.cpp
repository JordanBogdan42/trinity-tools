#include "DtStruct.h"

#include "constants.h"

#include <vector>

#include <TROOT.h>

using namespace std;

DtStruct::DtStruct(bool isHLED) {
    if(isHLED){
        truHLED = true;
        data = vector<Double_t>(2,0.0);
    }
    else{
        truHLED = false;
        data = vector<Double_t>(5,0.0);
    }
    time = 0;
}

void DtStruct::Avg() {
    if(truHLED){
        for(size_t i = 0; i < data.size()-1; i++){
            data[i] /= maxCh;
        }
    }
    else{
        for(size_t i = 0; i < data.size(); i++){
            data[i] /= maxCh;
        }
    }
}