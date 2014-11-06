
#define  NUMBER_OF_LEDS  224

// Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
// The LED strips are 32 LEDs per meter but you can extend/cut the strip.
// By leaving off the data and clock pin its using the hardware SPI.  This will
// 'fix' the pins to the following:
//    Arduino 168/328 thats data = 11, and clock = pin 13
//    Megas thats data = 51, and clock = 52
LPD8806 strip = LPD8806(NUMBER_OF_LEDS);

// Some constants to make indexing easier
#define LEDS_PER_CHANNEL  32
#define COL0_ROW0         0
#define COL1_ROW0         63
#define COL2_ROW0         64
#define COL3_ROW0         127
#define COL4_ROW0         128
#define COL5_ROW0         191
#define COL6_ROW0         192

int CHANNEL_START[CHANNELS] = {
    COL0_ROW0,
    COL1_ROW0,
    COL2_ROW0,
    COL3_ROW0,
    COL4_ROW0,
    COL5_ROW0,
    COL6_ROW0
  };

void LEDStripInit()
{
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  LEDStripUpdate();
}

void LEDStripClear()
{
  for (int i=0; i < strip.numPixels(); i++) 
  {
    strip.setPixelColor(i, 0);  // turn all pixels off
  }
}


void ComputeHistory()
{
  const static int WaitTics = 50;
  const static int DropTics = 2;
  static int UpdateTic = 0; 

  static int HighestLevelRunLength = 0;
  int HighestLevel = 0;
  
  // For each channel
  //  If the timer is higher than the limit
  //    If the history marker is higher than current reading
  //      Adjust the history marker down by one LED
  //      Adjust the timer limit lower to provide a faster fall out
  //    End If
  //    Clear history timer
  //  End If
  // End for
  for (byte i = 0; i < CHANNELS; i++)
  {
    if (++HistoryTics[i] > HistoryUpdateTics[i])
    {
      if (History[i] > Spectrum[i])
      {
        History[i]--;
        HistoryUpdateTics[i] = DropTics;
      }
      
      // Trigger update of LEDs and reset counter
      HistoryTics[i] = 0;

      if (DEBUG)
      {
        Serial.print(History[i]);
        Serial.print("  ");
      }
    }
  }

  // For each channel
  //  If history is less than current reading
  //    Set history to that reading (thus creating the peak for this channel)
  //    Adjust the timer limit higher to keep a max peak longer
  //    If the highest level is less than the current reading
  //      Set highest level to the current reading
  //      Set highest level updated flag
  //    End If
  //  End If
  // End For
  boolean HighestLevelUpdated = false;
  for (byte i = 0; i < CHANNELS; i++)
  {
    if (History[i] < Spectrum[i])
    {
      History[i] = Spectrum[i];
      HistoryUpdateTics[i] = WaitTics;
    }
    
    if (HighestLevel < Spectrum[i])
    {
      HighestLevel = Spectrum[i];
    }
  }
  
  // If the highest level is has not been updated
  //  If incremented run length is greater than timer
  //    This indicates that there is no music playing 
  //  End If
  // Else
  //  Clear run length
  // End If  
  if (HighestLevel < 8)
  {
    if (++HighestLevelRunLength > 100 && MusicPlaying)
    {
      MusicPlaying = false;
    }
  }
  else
  {
    HighestLevelRunLength = 0;
    MusicPlaying = true;
  }
  
  // Write peak markers to the LEDs if music is playing
  if (MusicPlaying && eeSysModeAnalyzer1 == meSysMode)
  {
    strip.setPixelColor(COL0_ROW0+History[0], PeakMkrColor);
    strip.setPixelColor(COL1_ROW0-History[1], PeakMkrColor);
    strip.setPixelColor(COL2_ROW0+History[2], PeakMkrColor);
    strip.setPixelColor(COL3_ROW0-History[3], PeakMkrColor);
    strip.setPixelColor(COL4_ROW0+History[4], PeakMkrColor);
    strip.setPixelColor(COL5_ROW0-History[5], PeakMkrColor);
    strip.setPixelColor(COL6_ROW0+History[6], PeakMkrColor);
    // Should be able to remove this
    // -->>  strip.show();
  }
}

void SpecturmToLeds()
{
  // Loop var
  byte i = 0;
  
  // Use the global variables as defined by settings/LCD options
  uint32_t LevelColor = TopMkrColor;
  uint32_t lnFillColor  = FillColor;
  
  // Push it to the LED strips
  strip.setPixelColor(COL0_ROW0+Spectrum[0], LevelColor);
  for (i = (Spectrum[0]-1); i > 0; --i)
  {
    strip.setPixelColor(COL0_ROW0+Spectrum[0]-i-1, lnFillColor);
  }
  strip.setPixelColor(COL1_ROW0-Spectrum[1], LevelColor);
  for (i = (Spectrum[1]-1); i > 0; --i)
  {
    strip.setPixelColor(COL1_ROW0-Spectrum[1]+i+1, lnFillColor);
  }
  strip.setPixelColor(COL2_ROW0+Spectrum[2], LevelColor);
  for (i = (Spectrum[2]-1); i > 0; i--)
  {
    strip.setPixelColor(COL2_ROW0+Spectrum[2]-i-1, lnFillColor);
  }
  strip.setPixelColor(COL3_ROW0-Spectrum[3], LevelColor);
  for (i = (Spectrum[3]-1); i > 0; i--)
  {
    strip.setPixelColor(COL3_ROW0-Spectrum[3]+i+1, lnFillColor);
  }
  strip.setPixelColor(COL4_ROW0+Spectrum[4], LevelColor);
  for (i = (Spectrum[4]-1); i > 0; i--)
  {
    strip.setPixelColor(COL4_ROW0+Spectrum[4]-i-1, lnFillColor);
  }
  strip.setPixelColor(COL5_ROW0-Spectrum[5], LevelColor);
  for (i = (Spectrum[5]-1); i > 0; i--)
  {
    strip.setPixelColor(COL5_ROW0-Spectrum[5]+i+1, lnFillColor);
  }
  strip.setPixelColor(COL6_ROW0+Spectrum[6], LevelColor);
  for (i = (Spectrum[6]-1); i > 0; i--)
  {
    strip.setPixelColor(COL6_ROW0+Spectrum[6]-i-1, lnFillColor);
  }
}

void MidSpecturmToLeds()
{
  // Loop var
  byte i = 0;
  
  // Use the global variables as defined by settings/LCD options
  uint32_t LevelColor = TopMkrColor;
  uint32_t lnFillColor  = FillColor;
  
  // Push it to the LED strips
  
  // Column 0 -----------
  uint32_t lnLowStart = COL0_ROW0+(LEDS_PER_CHANNEL-Spectrum[0])/2;
  for (i = 0; i < Spectrum[0]; i++)
  {
    strip.setPixelColor(lnLowStart+i, lnFillColor);
  }
  strip.setPixelColor((LEDS_PER_CHANNEL/2)-1, LevelColor);
  
  // Column 1 -----------
  lnLowStart = COL1_ROW0-((LEDS_PER_CHANNEL-Spectrum[1])/2);
  for (i = 0; i < Spectrum[1]; i++)
  {
    strip.setPixelColor(lnLowStart-i, lnFillColor);
  }
  strip.setPixelColor((COL1_ROW0-LEDS_PER_CHANNEL/2)+1, LevelColor);
  
  // Column 2 -----------
  lnLowStart = COL2_ROW0+(LEDS_PER_CHANNEL-Spectrum[2])/2;
  for (i = 0; i < Spectrum[2]; i++)
  {
    strip.setPixelColor(lnLowStart+i, lnFillColor);
  }
  strip.setPixelColor((COL2_ROW0+LEDS_PER_CHANNEL/2)-1, LevelColor);
  
  // Column 3 -----------
  lnLowStart = COL3_ROW0-((LEDS_PER_CHANNEL-Spectrum[3])/2);
  for (i = 0; i < Spectrum[3]; i++)
  {
    strip.setPixelColor(lnLowStart-i, lnFillColor);
  }
  strip.setPixelColor((COL3_ROW0-LEDS_PER_CHANNEL/2)+1, LevelColor);
  
  // Column 4 -----------
  lnLowStart = COL4_ROW0+(LEDS_PER_CHANNEL-Spectrum[4])/2;
  for (i = 0; i < Spectrum[4]; i++)
  {
    strip.setPixelColor(lnLowStart+i, lnFillColor);
  }
  strip.setPixelColor((COL4_ROW0+LEDS_PER_CHANNEL/2)-1, LevelColor);
  
  // Column 5 -----------
  lnLowStart = COL5_ROW0-((LEDS_PER_CHANNEL-Spectrum[5])/2);
  for (i = 0; i < Spectrum[5]; i++)
  {
    strip.setPixelColor(lnLowStart-i, lnFillColor);
  }
  strip.setPixelColor((COL5_ROW0-LEDS_PER_CHANNEL/2)+1, LevelColor);
  
  // Column 6 -----------
  lnLowStart = COL6_ROW0+(LEDS_PER_CHANNEL-Spectrum[6])/2;
  for (i = 0; i < Spectrum[6]; i++)
  {
    strip.setPixelColor(lnLowStart+i, lnFillColor);
  }
  strip.setPixelColor((COL6_ROW0+LEDS_PER_CHANNEL/2)-1, LevelColor);
 
}

void LEDStripUpdate()
{
  strip.show();
}

void LedTest()
{
  //  colorChase(strip.Color(127,127,127), 10);
//
//  // Send a simple pixel chase in...
//  colorChase(strip.Color(127,0,0), 10);  	// full brightness red
//  colorChase(strip.Color(127,127,0), 10);	// orange
//  colorChase(strip.Color(0,127,0), 10);		// green
//  colorChase(strip.Color(0,127,127), 10);	// teal
//  colorChase(strip.Color(0,0,127), 10);		// blue
//  colorChase(strip.Color(127,0,127), 10);	// violet
//
//  // fill the entire strip with...
//  colorWipe(strip.Color(127,0,0), 10);		// red
//  colorWipe(strip.Color(0, 127,0), 10);		// green
//  colorWipe(strip.Color(0,0,127), 10);		// blue
    colorWipe(strip.Color(127,127,127), 100);
//  rainbow(10);
//  rainbowCycle(0);  // make it go through the cycle fairly fast
  
  
    LEDStripClear(); 
  
    byte i = 0;
  
//  for (i=0; i < strip.numPixels()-3; i++) {
//      strip.setPixelColor(i, strip.Color(0,0,127));
//      strip.setPixelColor(i+1, strip.Color(127,0,0));
//      strip.setPixelColor(i+2, strip.Color(0,127,0));
//      strip.setPixelColor(i+3, strip.Color(10,60,10));
//      if (i == 0) { 
//        strip.setPixelColor(strip.numPixels()-1, 0);
//      } else {
//        strip.setPixelColor(i-1, 0);
//      }
//      strip.show();
//      delay(100);
//  }
  for (i = 0; i < LEDS_PER_CHANNEL; i++)
  {
    strip.setPixelColor(COL0_ROW0+i, strip.Color(127,127,i));
    strip.setPixelColor(COL1_ROW0-i, strip.Color(i,127,127));
    strip.setPixelColor(COL2_ROW0+i, strip.Color(127,i,127));
    strip.setPixelColor(COL3_ROW0-i, strip.Color(127,i,0));
    strip.setPixelColor(COL4_ROW0+i, strip.Color(127,0,i));
    strip.setPixelColor(COL5_ROW0-i, strip.Color(i,127,0));
    strip.setPixelColor(COL6_ROW0+i, strip.Color(0,75+i,0));
    strip.show();
    
    delay(15);
  }
  for (i = 0; i < LEDS_PER_CHANNEL; i++)
  {
    strip.setPixelColor(COL0_ROW0+i, 0);
    strip.setPixelColor(COL1_ROW0-i, 0);
    strip.setPixelColor(COL2_ROW0+i, 0);
    strip.setPixelColor(COL3_ROW0-i, 0);
    strip.setPixelColor(COL4_ROW0+i, 0);
    strip.setPixelColor(COL5_ROW0-i, 0);
    strip.setPixelColor(COL6_ROW0+i, 0);
    strip.show();
    
    delay(10);
  }
  int j = 0;
  int c = 0;
  for (c = 0; c < 125; c+=25)
  {
    for (j = 0; j < CHANNELS; j++)
    {
      for (i = 0; i < LEDS_PER_CHANNEL; i++)
      {
        int index = CHANNEL_START[j];
        if (j % 2 == 0)
        {
          index+=i;
        }
        else
        {
          index-=i;
        }
        strip.setPixelColor(index, strip.Color(127-c,c,0)); 
      }
      strip.show();
      delay(350);
    }
  }
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// Chase a dot down the strip
// good for testing purposes
void colorChase(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  } 
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      if (i == 0) { 
        strip.setPixelColor(strip.numPixels()-1, 0);
      } else {
        strip.setPixelColor(i-1, 0);
      }
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}
