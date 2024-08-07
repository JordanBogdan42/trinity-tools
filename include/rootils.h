#ifndef rootils_H
#define rootils_H

#include <Event.h>

#include <vector>
#include <cmath>

using namespace std;

void SetBranches(Event **evP);
Double_t Median(vector<Double_t> v);
void FindBin(int pixelID, int);
void DrawMUSICBoundaries();

#endif