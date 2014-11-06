#include <SeetronLcd.h>
#include "LPD8806.h"
#include "SPI.h"
#include <IRremote.h>

int RECV_PIN = 30;
IRrecv irrecv(RECV_PIN);
decode_results results;

// LCD 'Screens'
typedef enum {
  eeAdjustPeakMkrScreen = 0,
  eeAdjustTopMkrScreen,
  eeAdjustFillScreen,
  eeChangeModeScreen,
  eeNumScreens,
  eeInitScreen
} teScreens;

  
// LCD state tracking
typedef struct {
  boolean  mbBaseScreenRefreshReq;
  int   mnBaseScreen;
  boolean mbSubScreenRefreshReq;
  int   mnSubScreen;
} teLcdState;
teLcdState meLcdState;  

// Color option tracking (tied to values used in teScreens)
typedef struct {
  int mnR;
  int mnG;
  int mnB;
} teColor;
// Each screen gets its own color options
teColor maeColorOptions[eeNumScreens];
int EditingOption;

typedef enum {
  eeRed = 0,
  eeGreen,
  eeBlue,
  eeNumColorOptions
} teColorOptions;
int manColorOptionCursorPos[eeNumColorOptions] = {42, 49, 56};

// Color options as written to LED strip
uint32_t PeakMkrColor;
uint32_t TopMkrColor;
uint32_t FillColor;

#define CHANNELS          7

// Specturm A/D data
int Spectrum[CHANNELS];

// History of the last high peak
int History[CHANNELS];
int HistoryUpdateTics[CHANNELS];
int HistoryTics[CHANNELS];

#define  DEBUG   0

typedef enum
{
  eeSysModeAnalyzer1,
  eeSysModeAnalyzer2,
  eeSysModeRandom,
  eeSysModePulse,
  eeSysModeLedTest,
  eeNumSysModes
} teSysMode;
teSysMode meSysMode;

boolean MusicPlaying;

//***********************************************************
//  Function: setup
//
//  Description:
//    Setup misc pinouts and direction registers before entering the main
//    'loop'.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void setup() 
{
  // Always start in spectrum analyzer mode
  meSysMode = eeSysModeAnalyzer1;
  
  // Start up the LED strip
  LEDStripInit();
  
  // Setup specturm analyzer pins and chip startup
  SpectrumAnalyzerInit();
  
  // LCD Init to clear screen and setup default values to
  // colors/screens
  LCDInit();
  
  // Setup switches/interrupts
  UserInputInit();
  
  // Serial debug init
  //if (DEBUG)
  {
    Serial.begin(115200);
  }
  
  MusicPlaying = true;
  
  irrecv.enableIRIn(); // Start the receiver

  PulseModeInit();  
  
  delay(2000);
}

#define LEDSTRIP_REFRESH_RATE_HZ  20
#define LEDSTRIP_WAIT_MS (1/LEDSTRIP_REFRESH_RATE_HZ) * 1000
uint32_t mnServiceSpecturm = 0;

//***********************************************************
//  Function: loop
//
//  Description:
//    Main 'forever' loop of the program.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void loop() 
{
  switch(meSysMode)
  {
  case eeSysModeRandom:
    RandomMode();
    break;
  case eeSysModePulse:
    PulseMode();
    break;
  case eeSysModeLedTest:
    //LedTest();
    break;
  case eeSysModeAnalyzer1:
  case eeSysModeAnalyzer2:
  default:
    if (millis() - mnServiceSpecturm > LEDSTRIP_WAIT_MS)
    { 
      mnServiceSpecturm = millis();
      
      // Grab the normalized 7 channel spectrum analyzer data
      normalizeSpectrum();
    
      // Clear strips
      LEDStripClear();
       
      // Process the history to mark the last high
      ComputeHistory();
          
      // Display data on LED strips
      if (MusicPlaying)
      {
        if (eeSysModeAnalyzer1 == meSysMode)
        {
          SpecturmToLeds();
        }
        else
        {
          MidSpecturmToLeds();
        }
      }
      
      // Update strip after setting in various methods above
      LEDStripUpdate();
    }
    break;
  }
  
  // Handle all LCD updates
  UpdateLcd();
  
  // Handle all IR input
  ProcessIR();
  
  if (DEBUG)
    delay(2000);

}



