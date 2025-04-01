R__LOAD_LIBRARY(libExACT.so)
#include<TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>

using namespace std;

vector<int> mapMUSIC = {17,16,1,0,25,24,9,8,19,18,3,2,27,26,11,10,21,20,5,4,29,28,13,12,23,22,7,6,31,30,15,14};
vector<int> mapSIAB = {12,12,8,8,4,4,0,0,13,13,9,9,5,5,1,1,14,14,10,10,6,6,2,2,15,15,11,11,7,7,3,3};
int iLastPix = -1;
int eventCounter = 0;
int eventFail = 0;
TLatex *text = 0;
TTree *tree = 0;
Event *ev;

TCanvas *c_disp = 0;
TH1I *hPixelTrace = 0;

int MaxNofChannels = 256;
int LengthofPedestal = 200;
int SignalStart = 230;
int SignalWidth = 20;

Bool_t HandleInput()
{
	TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
	while (1)
	{
        	timer.TurnOn();
        	TString input = Getline("Type 'q' to exit, <return> to go on:");
        	timer.TurnOff();
        	if (input=="q\n") return kFALSE;
        	if (input=="\n") return kTRUE;
	}
	return kFALSE;
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

//Draws red boxes to make obvious which pixels are associated with the same MUSIC chip
void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,1.5,3.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=1; i < MaxNofChannels/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetX1((i%8)*2-0.5);
		bn->SetX2((i%8)*2+1.5);
		bn->SetY1((i/8)*4-0.5);
		bn->SetY2((i/8)*4+3.5);
		bn->Draw();
	}
}

void PlotTrace(int iPix)
{
    if(hPixelTrace == 0)
    {
        hPixelTrace = new TH1I("hPixelTrace","Pixel Trace",500,-0.5,499.5);
        hPixelTrace->SetStats(0);
        hPixelTrace->GetXaxis()->SetTitle("ADC sample");
        hPixelTrace->GetYaxis()->SetTitle("ADC counts");
    }

    hPixelTrace->Reset();
    TString title;
    title.Form("Trace of Pixel %i", iPix);
    hPixelTrace->SetTitle(title);
    vector<Int_t> trace = vector<Int_t>(MaxNofChannels);

    trace  = ev->GetSignalValue(iPix);
    for(int k = 0; k<512; k++){
        hPixelTrace->SetBinContent(k+1,trace[k]);
    }

    c_disp->cd(3);
    hPixelTrace->Draw();

    gPad->Modified();
    gPad->Update();
}

void ShowInfoAtCursor(int x, int y)
{
    int MUSIC_column = x/2;
    int MUSIC_row = y/4;
    int MUSIC_LOC =  MUSIC_column+MUSIC_row*8;
    int MUSIC_Channel = y%4+4*(x%2);
    int PIX_ID = MUSIC_Channel + MUSIC_column*8 +MUSIC_row*64;
    int MUSIC_ID = mapMUSIC[MUSIC_LOC];
    int SIAB_ID = mapSIAB[MUSIC_LOC];

    TString statusline;
    statusline.Form("Pixel: %i, MUSIC: %i, SIAB: %i", PIX_ID, MUSIC_ID, SIAB_ID);
    if(text!=0)
        text->Delete();
    TLatex T1;
    text = T1.DrawLatexNDC(0.25,0.95,statusline.Data());
    gPad->Modified();
    gPad->Update();
}

int FindPixel(int nx, int ny)
{
    // Calculate the SIAB number (0 to 15)
    int SIAB_Row = nx / 4;
    int SIAB_Col = ny / 4;
    int SIAB_Number = SIAB_Row + 4*SIAB_Col;

    // Calculate the pixel number within the SIAB (0 to 15)
    int SIAB_Pixel_Row = nx % 4;
    int SIAB_Pixel_Col = ny % 4;
    int SIAB_Pixel_Number = SIAB_Pixel_Row * 4 + SIAB_Pixel_Col;

    // Calculate the pixel number (0 to 255)
    int Pixel_Number = SIAB_Number * 16 + SIAB_Pixel_Number;
    return Pixel_Number;
}

void PixelClicked()
{
    int event = gPad->GetEvent();
    TObject *o = gPad->GetSelected();
    if (!o){return;}
    if (!(o->InheritsFrom("TH2"))){return;}
    TH2F *h = (TH2F*)o;
    int px = gPad->GetEventX();
    int py = gPad->GetEventY();
    Float_t xx = gPad->AbsPixeltoX(px);
    Float_t yy = gPad->AbsPixeltoY(py);
    Float_t x = 0.5+gPad->PadtoX(xx);
    Float_t y = 0.5+gPad->PadtoY(yy);
    int pix = FindPixel((int)x,(int)y);
    if(pix!=iLastPix)
    {
        iLastPix = pix;
        ShowInfoAtCursor((int) x, (int) y);
    }
    if (event == 11)
    {
        PlotTrace(pix);
    }
}


void PlotEvent()
{
    c_disp->cd(1);
    gPad->AddExec("ev","PixelClicked()");
    TH2F *hCam = new TH2F("hCam","",16,-0.5,15.5,16,-0.5,15.5);
    hCam->SetStats(0);
    hCam->Draw("colz");
    DrawMUSICBoundaries();
    int nEntries = tree->GetEntries();
    cout << "Total Number of Events: " << nEntries << endl;

    TH1F *hAmp = new TH1F("hAmp","Amplitude Distribution",410, 0, 4096);
    hAmp->SetStats(0);
    hAmp->GetXaxis()->SetTitle("Signal Max Amplitude [ADC]");
    hAmp->GetYaxis()->SetTitle("No. of Pixels");
    
    while(1){
        tree->GetEntry(eventCounter);
        cout << "Event# " << eventCounter << " is displayed." << endl;
        int MUSICpos = (ev->GetROIMusicID())[0];
        cout << "The MUSIC which triggered the DAQ for event # " << eventCounter << " is " << MUSICpos << endl; 
        hCam->Reset();
        hAmp->Reset();

        Pulse *pulse;
        pulse = new Pulse(ev->GetSignalValue(74));
        if(pulse->GetAmplitude() <= 1500){
            eventCounter++;
            if(eventCounter>=nEntries){eventCounter=0;}
            delete pulse;
            if(eventFail == nEntries){
                cout << "All events in this file fail to meet criteria...quitting..." << endl;
                break;
            }
            continue;
        }
        delete pulse;
        
        for(int i = 0; i < MaxNofChannels; i++){
                pulse = new Pulse(ev->GetSignalValue(i));
                int nx, ny;
                FindBin(i,&nx,&ny);
                hCam->SetBinContent(nx+1,ny+1,pulse->GetAmplitude());
                hAmp->Fill(pulse->GetAmplitude());
                delete pulse;
        }

        uint64_t convtime = ((uint64_t)ev->GetTBTime() / 100000000ULL);
        //Assuming timeTrig is in the format "seconds since epoch with 10^-7 second resolution"
        time_t epochTime = static_cast<time_t>(convtime); // Convert to seconds
        int microsec = convtime % 1000000;

        // Convert to struct tm in UTC
        tm* utcTime = gmtime(&epochTime);

        // Format and display the UTC time
        cout << "Trigger Time (UTC): ";
        cout << put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << setfill('0') << setw(6) << microsec << endl;
        // cout << "time " << ev->GetTBTime() <<" is displayed." << endl;


        c_disp->cd(1);
        c_disp->SetTheta(90);
        c_disp->SetPhi(90);
        hCam->SetMinimum();
        hCam->SetMaximum();
        c_disp->cd(1)->Modified();
        c_disp->cd(1)->Update();

        c_disp->cd(2);
        hAmp->Draw();
        c_disp->cd(2)->Modified();
        c_disp->cd(2)->Update();


        eventCounter++;
        if(eventCounter>=nEntries){eventCounter = 0;}
        if(!HandleInput()){break;}
    }
}

void SetBranches(Event *evD)
{
    tree->SetBranchAddress("Events", &evD);
}

void LoadEvents(string fileName, string treeString)
{
    cout << "Loading file: " << fileName << endl;
    TFile *fO = new TFile(fileName.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}

void TCameraPlotEvents(string fileName,string treeString)
{   
	c_disp = new TCanvas("Display","CameraPlot",750,750);
	c_disp->Divide(2,2);
	LoadEvents(fileName, treeString);
	ev = new Event();
    tree->SetBranchAddress("Events", &ev);
	//SetBranches(ev);
	PlotEvent();
}