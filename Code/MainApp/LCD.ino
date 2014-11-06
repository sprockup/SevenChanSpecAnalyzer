
// LCD Object (Serial1, PIN18)
SeetronLcd mcLcd(1);

extern LPD8806 strip;

const char* manLcdScreens[eeNumScreens+2] = 
{
// ------ Line 1 -----|***** Line 2 ******|------ Line 3 -----|***** Line 4 ******| 
// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
  "Adjusting Peak Mkr    Select Color(s)     *                  127     0      0 ",
  "Adjusting Top Mkr     Select Color(s)     *                  127    127    127",
  "Adjusting Fill        Select Color(s)     *                   0      0     127",
  "Change Mode                                                                   ",                 
  "",
  "Seven Channel          Spectrum Analyzer                         ACTIVATE!!"
};

void LCDInit()
{
  // LCD Init
  Serial1.begin(9600);
  mcLcd.SetBacklight();
  mcLcd.Home();
  mcLcd.ClearScreen();

  // 'Screen' initializations
  meLcdState.mnBaseScreen = eeInitScreen;
  Serial1.println(manLcdScreens[meLcdState.mnBaseScreen]);
  
  // State tracking and init value setup for screens
  PeakMkrColor = strip.Color(127,0,0);
  maeColorOptions[eeAdjustPeakMkrScreen].mnR = 127;
  maeColorOptions[eeAdjustPeakMkrScreen].mnG = 0;
  maeColorOptions[eeAdjustPeakMkrScreen].mnB = 0;
  
  TopMkrColor = strip.Color(127,127,127);
  maeColorOptions[eeAdjustTopMkrScreen].mnR = 127;
  maeColorOptions[eeAdjustTopMkrScreen].mnG = 127;
  maeColorOptions[eeAdjustTopMkrScreen].mnB = 127;
  
  FillColor = strip.Color(0,0,127);
  maeColorOptions[eeAdjustFillScreen].mnR = 0;
  maeColorOptions[eeAdjustFillScreen].mnG = 0;
  maeColorOptions[eeAdjustFillScreen].mnB = 127;
  
  EditingOption = 0; 
}

void UpdateLcd()
{
  // Only perform an LCD write if the flag is set
  if (meLcdState.mbBaseScreenRefreshReq)
  {
    // Clear update required flag
    meLcdState.mbBaseScreenRefreshReq = 0;
    
    // Display new 'base screen'
    mcLcd.ClearScreen();
    Serial1.println(manLcdScreens[meLcdState.mnBaseScreen]);
    
    // Update 'sub screen' options
    if (meLcdState.mbSubScreenRefreshReq)
    {
      // Clear flag
      meLcdState.mbSubScreenRefreshReq = 0;
      
      // Perform updates
    } // if sub screen update req'd
    
    if (eeChangeModeScreen != meLcdState.mnBaseScreen)
    {
      // Reposition cursor
      mcLcd.StartCursorPos(40);
      Serial1.println("                  ");
      mcLcd.StartCursorPos(manColorOptionCursorPos[EditingOption]);
      mcLcd.Write("*");
      
      // Bound color variables
      if (maeColorOptions[eeAdjustPeakMkrScreen].mnR > 127)
        maeColorOptions[eeAdjustPeakMkrScreen].mnR = 0;
      
      if (maeColorOptions[eeAdjustPeakMkrScreen].mnG > 127)
        maeColorOptions[eeAdjustPeakMkrScreen].mnG = 0;
      
      if (maeColorOptions[eeAdjustPeakMkrScreen].mnB > 127)
        maeColorOptions[eeAdjustPeakMkrScreen].mnB = 0;
      
      if (maeColorOptions[eeAdjustTopMkrScreen].mnR > 127)
        maeColorOptions[eeAdjustTopMkrScreen].mnR = 0;
      
      if (maeColorOptions[eeAdjustTopMkrScreen].mnG > 127)
        maeColorOptions[eeAdjustTopMkrScreen].mnG = 0;
      
      if (maeColorOptions[eeAdjustTopMkrScreen].mnB > 127)
        maeColorOptions[eeAdjustTopMkrScreen].mnB = 0;
      
      if (maeColorOptions[eeAdjustFillScreen].mnR > 127)
        maeColorOptions[eeAdjustFillScreen].mnR = 0;
      
      if (maeColorOptions[eeAdjustFillScreen].mnG > 127)
        maeColorOptions[eeAdjustFillScreen].mnG = 0;
      
      if (maeColorOptions[eeAdjustFillScreen].mnB > 127)
        maeColorOptions[eeAdjustFillScreen].mnB = 0;
  
      // Pull colors from variables
      mcLcd.StartCursorPos(60);
      Serial1.println("                  ");
      mcLcd.StartCursorPos(61);
      if (maeColorOptions[meLcdState.mnBaseScreen].mnR > 99)
      {
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnR);
        Serial1.print("    ");
      }
      else if (maeColorOptions[meLcdState.mnBaseScreen].mnR > 9)
      {
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnR);
        Serial1.print("     ");
      }
      else
      { 
        Serial1.print(" ");
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnR);
        Serial1.print("     ");
      }
      
      if (maeColorOptions[meLcdState.mnBaseScreen].mnG > 99)
      {
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnG);
        Serial1.print("    ");
      }
      else if (maeColorOptions[meLcdState.mnBaseScreen].mnG > 9)
      {
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnG);
        Serial1.print("     ");
      }
      else
      { 
        Serial1.print(" ");
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnG);
        Serial1.print("     ");
      }
      
      if (maeColorOptions[meLcdState.mnBaseScreen].mnB > 9)
      {
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnB);
      }
      else
      { 
        Serial1.print(" ");
        Serial1.print(maeColorOptions[meLcdState.mnBaseScreen].mnB);
      }
      
      // Update all colors from variables
      PeakMkrColor = strip.Color(
        maeColorOptions[eeAdjustPeakMkrScreen].mnR,
        maeColorOptions[eeAdjustPeakMkrScreen].mnG,
        maeColorOptions[eeAdjustPeakMkrScreen].mnB);
      TopMkrColor = strip.Color(
        maeColorOptions[eeAdjustTopMkrScreen].mnR,
        maeColorOptions[eeAdjustTopMkrScreen].mnG,
        maeColorOptions[eeAdjustTopMkrScreen].mnB);
      FillColor = strip.Color(
        maeColorOptions[eeAdjustFillScreen].mnR,
        maeColorOptions[eeAdjustFillScreen].mnG,
        maeColorOptions[eeAdjustFillScreen].mnB);
    }
    else if (eeChangeModeScreen == meLcdState.mnBaseScreen)
    {
        mcLcd.StartCursorPos(45);
      
        switch (meSysMode)
        {
        case eeSysModeRandom:
          Serial1.println("Random");
          break;
        case eeSysModePulse:
          Serial1.println("Pulse");
          break;
        case eeSysModeAnalyzer2:
          Serial1.println("Analyzer2");
          break;
        case eeSysModeLedTest:
          Serial1.println("Test Pattern");
          break;
        case eeSysModeAnalyzer1:
        default:
          Serial1.println("Analyzer1");
          break;
        }
    }
  } // if base screen update req'd
}

