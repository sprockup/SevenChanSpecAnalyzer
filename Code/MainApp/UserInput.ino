
// Input buttons (tied to corresponding interrupts)
int Switch0Pin = 2;  // Int 0
int Switch1Pin = 3;  // Int 1
int Switch2Pin = 20; // Int 2

//***********************************************************
//  Function: UserInputInit
//
//  Description:
//    Sets pin modes and interrupts used for 3 swtiches that allow
//    the user to control the system.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void UserInputInit()
{
  // Enable interrupts for switch inputs
  pinMode(Switch0Pin, INPUT);
  attachInterrupt(0, Switch1Interrupt, RISING);
  pinMode(Switch1Pin, INPUT);
  attachInterrupt(1, Switch2Interrupt, RISING);
  pinMode(Switch2Pin, INPUT);
  attachInterrupt(2, Switch3Interrupt, RISING);
}

//***********************************************************
//  Function: Switch1Interrupt
//
//  Description:
//    Switch 1 interrupt service routine.  Increments the 'screen' to the
//    next, handles rollover.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void Switch1Interrupt()
{
  // Set new screen number
  if (++meLcdState.mnBaseScreen >= eeNumScreens)
  {
    meLcdState.mnBaseScreen = 0;
  }
  
  // Reset editing cursor
  EditingOption = 0;
  
  // Flag refresh request
  meLcdState.mbBaseScreenRefreshReq = 1;
}

//***********************************************************
//  Function: Switch2Interrupt
//
//  Description:
//    Switch 2 interrupt service routine.  Increments the 'editing' pointer 
//    to the next option, handles rollover.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void Switch2Interrupt()
{
  // Track current option
  if (++EditingOption >= eeNumColorOptions)
  {
    EditingOption = 0;
  }
  
  // Trigger refresh
  meLcdState.mbBaseScreenRefreshReq = 1;
}

//***********************************************************
//  Function: Switch3Interrupt
//
//  Description:
//    Switch 3 interrupt service routine.  Increments the 'option' corresponding
//    to 'editing' pointer the next option(s), handles rollover.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void Switch3Interrupt()
{
  // Adjust current option
  if (eeChangeModeScreen == meLcdState.mnBaseScreen)
  {
    if (eeSysModeAnalyzer1 == meSysMode)
    {    
      meSysMode = eeSysModeAnalyzer2;
    }
    else if (eeSysModeAnalyzer2 == meSysMode)
    {    
      meSysMode = eeSysModeRandom;
    }
    else if (eeSysModeRandom == meSysMode)
    {
      meSysMode = eeSysModePulse;
    }
    else if (eeSysModePulse == meSysMode)
    {
      meSysMode = eeSysModeLedTest;
    }
    else
    {
      meSysMode = eeSysModeAnalyzer1;
    }
  }
  else
  {
    switch (EditingOption)
    {
      case 0:
        maeColorOptions[meLcdState.mnBaseScreen].mnR += 5;
        break;
      case 1:
        maeColorOptions[meLcdState.mnBaseScreen].mnG += 5;
        break;
      case 2:
        maeColorOptions[meLcdState.mnBaseScreen].mnB += 5;
        break;
      default:
      break;
    }
  }
  
  meLcdState.mbBaseScreenRefreshReq = 1;
}

//***********************************************************
//  Function: ProcessIR
//
//  Description:
//    Processes any IR input that may be awaiting.  Produces a tone
//    when an IR signal has been detected.
//  Inputs:
//    NONE
//  Returns:
//    NONE
//***********************************************************
void ProcessIR()
{
  // Handle IR Input
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    dump(&results);
    tone(45, 500, 250);
    irrecv.resume(); // Receive the next value
  }
}

// Dumps out the decode_results structure.
// Call this after IRrecv::decode()
// void * to work around compiler issue
//void dump(void *v) {
//  decode_results *results = (decode_results *)v
void dump(decode_results *results) 
{
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } 
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } 
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } 
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } 
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {	
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->panasonicAddress,HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == JVC) {
     Serial.print("Decoded JVC: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    } 
    else {
      Serial.print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println("");
}

