
// Random/Pulse Variables
#define MAX_NUM_RND_PULSES    150
#define MAX_NUM_PULSES        150
#define MIN_NUM_PULSES        30

#define MAX_BRIGHTNESS        128

#define RANDOM_MODE_REFRESH_RATE_HZ  3
#define RANDOM_MODE_WAIT_MS (1/RANDOM_MODE_REFRESH_RATE_HZ) * 1000
uint32_t mnLastRandomModeTic = 0;

#define PULSE_MODE_REFRESH_RATE_HZ   20
#define PULSE_MODE_WAIT_MS (1/PULSE_MODE_REFRESH_RATE_HZ) * 1000
uint32_t mnLastPulseModeTic = 0;

// Structure to hold the pulse information as well as its fade
typedef struct
{
  short mnR;
  short mnG;
  short mnB;
  boolean mbFadeOutR;
  boolean mbFadeOutG;
  boolean mbFadeOutB;
  short mnLocation;
} tePulseType;
tePulseType manPulses[MAX_NUM_PULSES];

//***********************************************************
//  Function: SleepModeInit
//
//  Description:
//    Seed the random number generator and clear the manPulses array.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void PulseModeInit()
{
  randomSeed(12345);
  
  for (byte i = 0; i < MAX_NUM_PULSES; i++)
  {
    manPulses[i].mnR = 0;
    manPulses[i].mnG = 0;
    manPulses[i].mnB = 0;
    manPulses[i].mbFadeOutR = true;
    manPulses[i].mbFadeOutG = true;
    manPulses[i].mbFadeOutB = true;
    manPulses[i].mnLocation = -1;
  }
}

//***********************************************************
//  Function: RandomMode
//
//  Description:
//    Generates random colors/locations for output to the LED Strip.  The
//    rate of this is controlled by RANDOM_MODE_REFRESH_RATE_HZ.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void RandomMode()
{
  if (millis() - mnLastRandomModeTic > RANDOM_MODE_WAIT_MS)
  {
    mnLastRandomModeTic = millis();
    
    int lcNumPulses = random(MIN_NUM_PULSES, MAX_NUM_RND_PULSES);
    
    // Clear strips
    LEDStripClear();
    
    for (int i = 0; i < lcNumPulses; i++)
    {
      // Set to a random color at a random location on the strip
      strip.setPixelColor(random(strip.numPixels()+1),
        strip.Color(random(MAX_BRIGHTNESS), random(MAX_BRIGHTNESS), random(MAX_BRIGHTNESS)));
    } 
    
    LEDStripUpdate();
  }
}

//***********************************************************
//  Function: PulseMode
//
//  Description:
//    Generates random pulses on the LED Strip that slowly fade w/time
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void PulseMode()
{
  if (millis() - mnLastPulseModeTic > PULSE_MODE_WAIT_MS)
  {
    // Update the current tic for refresh rate timing
    mnLastPulseModeTic = millis();
   
    // Clear strips
    LEDStripClear();
    
    // For each pulse (pixel)
    //  If valid (location != -1)
    //    Fade each color
    //    Test to see if valid
    //  Else
    //    Generate new pulse
    //  End If
    // End For
    for (byte i = 0; i < MAX_NUM_PULSES; i++)
    {
      // If the pulse location is -1 then the pulse is considered inactive
      if (-1 != manPulses[i].mnLocation)
      {
        FadePixel(i);
        boolean lbPixelStillValid = ValidPixel(manPulses[i].mnR, manPulses[i].mnG, manPulses[i].mnB);
        manPulses[i].mnLocation = (lbPixelStillValid ? manPulses[i].mnLocation : -1);
      }
      else
      {
        // Set to a random color at a random location on the strip
        manPulses[i].mnR = random(MAX_BRIGHTNESS);
        manPulses[i].mnG = random(MAX_BRIGHTNESS);
        manPulses[i].mnB = random(MAX_BRIGHTNESS);
        manPulses[i].mbFadeOutR = random(0,2);
        manPulses[i].mbFadeOutG = random(0,2);
        manPulses[i].mbFadeOutB = random(0,2);
        manPulses[i].mnLocation = random(strip.numPixels()+1);
      }
      
      // Update strip data
      strip.setPixelColor(manPulses[i].mnLocation,
        strip.Color(manPulses[i].mnR, manPulses[i].mnG, manPulses[i].mnB));
    }

    LEDStripUpdate();
  }
}

//***********************************************************
//  Function: FadePixel
//
//  Description:
//    Randomly adjusts the color pixel depending on the
//    corresponding fade out boolean up or down
//  Inputs:
//    anPixel - Array index of pixel into manPulses to adjust
//  Returns:
//    NONE
//***********************************************************
void FadePixel(byte anPixel)
{
  short lnNewColorValue;
  lnNewColorValue = random(2, 10) * (manPulses[anPixel].mbFadeOutR ? -1 : 1);
  manPulses[anPixel].mnR += lnNewColorValue;
  lnNewColorValue = random(2, 10) * (manPulses[anPixel].mbFadeOutG ? -1 : 1);
  manPulses[anPixel].mnG += lnNewColorValue;
  lnNewColorValue = random(2, 10) * (manPulses[anPixel].mbFadeOutB ? -1 : 1);
  manPulses[anPixel].mnB += lnNewColorValue;
}

//***********************************************************
//  Function: ValidPixel
//
//  Description:
//    Tests to see if a pixels color values are still above 0
//  Inputs:
//    anR, anG, anB - color values
//  Returns:
//    boolean - true if the colors should be displayed
//***********************************************************
boolean ValidPixel(byte anR, byte anG, byte anB)
{
  return ((0 < anR && 127 > anR) &&
    (0 < anG && 127 > anG) && 
    (0 < anB && 127 > anB));
}

