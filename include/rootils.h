#ifndef rootils_H
#define rootils_H

#include "DtStruct.h"

#include <TROOT.h>

#include <Event.h>

#include <vector>
#include <cmath>

using namespace std;

Double_t Median(vector<Double_t> v);
Double_t RMS(vector<Double_t> v);
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();
Double_t ErfcIntegrand(Double_t x, Double_t *par);
Double_t ConvolutedRMSFunction(Double_t *x, Double_t *par);

#endif