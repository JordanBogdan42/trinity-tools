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
bool CompareStructTime(const DtStruct &a, const DtStruct &b);

#endif