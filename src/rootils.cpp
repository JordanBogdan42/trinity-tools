#include "rootils.h"

#include "constants.h"

#include <TROOT.h>

#include <Event.h>

#include <vector>
#include <cmath>

void SetBranches(Event **evD)
{
	tree->SetBranchAddress("Events", evD);
}

Double_t Median(vector<Double_t> v)
{
	//Size of vector
	int n = v.size();
	//Make temp copy of the vector to leave original in the same order
	std::vector<Double_t> tempV(v);
    //Sort the vector 
    sort(tempV.begin(), tempV.end()); 
    //Check if the number of elements is odd 
    if(n%2!=0){
        return(Double_t)tempV[n/2];
	}
    //If the number of elements is even, return the average of the two middle elements 
    return(Double_t)(tempV[(n-1)/2]+tempV[n/2])/2.0; 
}