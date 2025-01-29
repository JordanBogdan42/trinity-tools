#include "rootils.h"

#include "constants.h"
#include "DtStruct.h"

#include <TROOT.h>
#include <TBox.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraphErrors.h>

#include <Event.h>

#include <vector>
#include <cmath>
#include "Math/Integrator.h"

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

Double_t RMS(vector<Double_t> v)
{
	int n = v.size();
	Double_t rms_val = 0;
	for(auto i: v){
		rms_val += pow(i,2);
	}
	rms_val /= n;
	return(Double_t)sqrt(rms_val);
}

void FindBin(int pixelID, int *nx, int *ny)
{
	int SIAB_Number = pixelID / 16;
	int SIAB_Pixel_Number = pixelID % 16;
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
}

void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,1.5,3.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=1; i < maxCh/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetX1((i%8)*2-0.5);
		bn->SetX2((i%8)*2+1.5);
		bn->SetY1((i/8)*4-0.5);
		bn->SetY2((i/8)*4+3.5);
		bn->Draw();
	}
}

Double_t ErfcIntegrand(Double_t x, Double_t *par) {
    // Define the Erfc function to be integrated
    return par[0] + par[1] * TMath::Erfc((x - par[2]) / (sqrt(2) * par[3]));
}

Double_t ConvolutedRMSFunction(Double_t *x, Double_t *par) {
    // Parameters:
    // par[0]: pedestal
    // par[1]: amplitude
    // par[2]: mean (mu)
    // par[3]: sigma

    // x[0] is the variable (angle of the camera row in this context)
    Double_t x_center = x[0];
    
    // Define the pixel size (0.3 degrees in both horizontal and vertical)
    //Double_t pixelSize = 0.3;
    
    // Compute the low and high edges of the pixel
    Double_t lowEdge = x_center - 0.5; //* pixelSize;
    Double_t highEdge = x_center + 0.5; //* pixelSize;
    
    // Lambda function for the integrand, capturing the parameters by reference
    auto integrand = [&](double xi) {
        return ErfcIntegrand(xi, par);
    };
    
    // Numerical integration of Erfc over the pixel range
    ROOT::Math::IntegratorOneDim integrator;
    Double_t integral = integrator.Integral(integrand, lowEdge, highEdge);
    return integral;
}

// Function to convert a time string to Unix timestamp
time_t convertToUnixTime(const string& timeString, int t_v) {
    tm tm = {};
    istringstream ss(timeString);
    if (t_v == 1) { 
    	ss >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    } else {
    	ss >> get_time(&tm, "%Y%m%dT%H%M%S");
    }
    time_t time_stamp = timegm(&tm);
    return time_stamp;
}