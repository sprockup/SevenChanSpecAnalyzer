
// Associated Pins
int spectrumAnalyzerInputPin = 0;
int specturmAnalyzerStrobePin = 4;
int spectrumAnalyzerResetPin = 5;

//***********************************************************
//  Function: SpectrumAnalyzerInit
//
//  Description:
//    Sets pin modes and initializes the spectrum analyzer sheild
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void SpectrumAnalyzerInit()
{
    // Setup pins to drive the spectrum analyzer. It needs RESET and STROBE pins.
  pinMode(spectrumAnalyzerResetPin, OUTPUT);
  pinMode(specturmAnalyzerStrobePin, OUTPUT);

  // Init spectrum analyzer
  digitalWrite(specturmAnalyzerStrobePin,LOW);  //pin 4 is strobe on shield
  digitalWrite(spectrumAnalyzerResetPin,HIGH); //pin 5 is RESET on the shield
  digitalWrite(specturmAnalyzerStrobePin,HIGH);
  digitalWrite(specturmAnalyzerStrobePin,LOW);
  digitalWrite(spectrumAnalyzerResetPin,LOW);
}

//***********************************************************
//  Function: readSpectrum
//
//  Description:
//    Reads the 7 channel specturm analyzer chip data from the shield.
//    The data is stored in the Spectrum[] array by band [0-7).  This function
//    also tracks the 'Total Power' seen on each channel which is used to 
//    determine if the audio input has stopped.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void readSpectrum()
{
  // Band 0 = Lowest Frequencies.
  byte Band;
  uint32_t TotalPower = 0;
  for(Band=0;Band <CHANNELS; Band++)
  {
    Spectrum[Band] = (analogRead(spectrumAnalyzerInputPin) + analogRead(spectrumAnalyzerInputPin) ) >> 1; //Read twice and take the average by dividing by 2
    digitalWrite(specturmAnalyzerStrobePin,HIGH);  //Strobe pin on the shield
    digitalWrite(specturmAnalyzerStrobePin,LOW);  
 
     TotalPower += Spectrum[Band];
     if (DEBUG)
     {
       Serial.print(Spectrum[Band]);
       Serial.print(" ");
     }
    
  }
  if (DEBUG)
  {
    Serial.println("---");
    Serial.print("   Total Power = ");
    Serial.println(TotalPower);
  }
}

//***********************************************************
//  Function: normalizeSpectrum
//
//  Description:
//    Calls readSpecturm then...
//    Normalizes the specturm analyzer data from the Spectrum[] array to 
//    keep the output capped at the number of LEDs per strip.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void normalizeSpectrum()
{
 
  static unsigned int  Divisor = 30, ChangeTimer=0, scaledLevel; //, ReminderDivisor,
  byte Band, BarSize, MaxLevel;
  
  readSpectrum();
  
  MaxLevel=0;
  for(Band=0;Band<CHANNELS;Band++)
  {
     scaledLevel = Spectrum[Band]/Divisor;	//Bands are read in as 10 bit values. Scale them down to be 0 - 5
     Spectrum[Band]=scaledLevel;
     if (scaledLevel > MaxLevel)  //Check if this value is the largest so far.
       MaxLevel = scaledLevel;    
  }
  
  // Is the level off the chart!?? If so, increase the divisor to make it small next read.
  if (MaxLevel >= 31)
  {
    Divisor=Divisor+1;
    ChangeTimer=0;
  }
  else 
  // If the level is too low, make divisor smaller, increase the levels on next 
  // read! - but only if divisor is not too small. If too small we sample too much noise!
  {
    if (MaxLevel < 25)
    {
      if (Divisor > 35)
        if (ChangeTimer++ > 20)
        {
          Divisor--;
          ChangeTimer=0;
        }
    }
    else
    {
      ChangeTimer=0; 
    }
  }

  // Cap the output at 32
  for (Band=0;Band <CHANNELS; Band++)
  {
    if (Spectrum[Band] > 31)
    {
      Spectrum[Band] = 31;
    }
  }
}
