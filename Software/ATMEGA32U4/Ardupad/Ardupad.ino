/*  
 * Ardupad for Atmega32u4
 * Default configuration - 2 Joysticks, 13 Buttons, 1 Dpad.
 * 
 * Joystick calibration:
 * 1: Press the buttons on pins D0 and D3 simultaneously to put the controller into calibration mode.
 * 2: Centre both joysticks and press the button on pin D1 to record mid position, RX LED will blink once to confirm step complete.
 * 3: Move both joysticks to full extents of travel several times to record maximum axis values.
 * 4: Press button on D1 again. RX LED will blink once more to confirm.
 * 5: Calibration is now complete and stored.
 * 
 * If you're using this controller with Windows, you will also need to run the windows joypad calibration once you've completed the internal calibration.
 * If you change joysticks or your centerpoint drifts (thanks, nintendo) re-running the internal calibration should solve the problem.
 * 
 * Note: Whilst controller is in calibration mode all USB control output will cease until calibration is complete or controller is restarted.
 * 
 * Created by - Daniel McKenzie
 * 10-03-2021 v1.3
 */

#include <Joystick.h> //https://github.com/MHeironimus/ArduinoJoystickLibrary
#include <EEPROM.h>

//Options
const bool cycleTimeTest = false; //Enable to test CPU Cycle time.
#define baudrate 115200 //Baudrate for serial. Used only for polling rate test

//Define button pins
byte buttonCount = 13; //Number of buttons in use. Change length of buttonPins, lastButtonState and currentButtonState to match.
byte buttonPins[13] = {0,1,2,3,4,5,6,7,12,13,14,15,16}; //Array to store digital pins used for buttons. Length must be the same as buttonCount.
byte dpadPins[4] = {8,11,9,10}; //Up, Right, Down, Left. Do not change order of directions.

//Button state arrays
byte dpadPinsState[4];  //Empty State array for dPad
byte lastButtonState[13]; //Empty State array for buttons last sent state. Must be same length as buttonPins
byte currentButtonState[13]; //Empty State array for buttons. MMust be same length as buttonPins

//Define Analog Pins for joysticks
const int leftJoyX = A3;
const int leftJoyY = A2;
const int rightJoyX = A1;
const int rightJoyY = A0;

//Joystick Settings
const bool invertLeftY = false;   //------------------------------------------
const bool invertLeftX = false;    //Change these settings for Inverted mounting 
const bool invertRightY = true;  //of joysticks.
const bool invertRightX = true;  //------------------------------------------
const int deadBandLeft = 10;     //
const int deadBandRight = 10;    //Joystick deadband settings. Deadband is the same for both axis on each joystick.
const bool useDeadband = true;    //
const int earlyLeftX = 30;   //--------------------------------------------------
const int earlyLeftY = 30;   //Distance from end of travel to achieve full axis movement. 
const int earlyRightY = 30;  //This helps square out each axis response to allow full movement speed with direction input.
const int earlyRightX = 30;  //--------------------------------------------------
boolean scaledJoystickOutput = true; //Enable joystick scaling. Needed for switch joysticks due to uneven axis travels. Disabling will save some compute time if your joystick works well without it.

//Main Joystick setup
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
buttonCount, 0,       // Button Count, Hat Switch Count
true, true, true,     // X Y Z
true, true, true,  // Rx, Ry, Rz
false, false,          // Rudder, Throttle
false, false, false); // Accelerator, Brake, Steering

//Default Joystick calibration settings and EEPROM storage Address
int minLeftX = 330; //EEPROM Adr = 1
int maxLeftX = 830; //EEPROM Adr = 3
int midLeftX = 512; //EEPROM Adr = 5

int minLeftY = 205; //EEPROM Adr = 7
int maxLeftY = 635; //EEPROM Adr = 9
int midLeftY = 477; //EEPROM Adr = 11

int minRightY = 215; //EEPROM Adr = 13
int maxRightY = 730; //EEPROM Adr = 15
int midRightY = 529; //EEPROM Adr = 17

int minRightX = 300; //EEPROM Adr = 19
int maxRightX = 780; //EEPROM Adr = 21
int midRightX = 525; //EEPROM Adr = 23

//Joystick LookUp Tables, compiled at startup or after joystick calibration.
byte leftXLUT[350];
byte leftYLUT[350];
byte rightXLUT[350];
byte rightYLUT[350];

//All variables below general use, not used for configuration.
boolean calibrationMode = false;
int calibrationStep = 1;
long pollingTest;
float lastPollingMS;
long lastPollingReport;

void setup() { 
  //Needed for Retropie to detect axis.
  Joystick.setXAxisRange(0, 254);
  Joystick.setYAxisRange(0, 254);
  Joystick.setZAxisRange(0, 254);
  Joystick.setRxAxisRange(0, 254);
  Joystick.setRyAxisRange(0, 2);
  Joystick.setRzAxisRange(0, 2);
  
  if(cycleTimeTest){
    Serial.begin(baudrate);
    while (!Serial); //Needed on 32u4 based arduinos for predictable serial output.
  }
  Joystick.begin(false); //Initialise joystick mode with auto sendState disabled as it has a huge processor time penalty for seemingly no benefit.
  
  for(int i = 0; i < buttonCount; i++){ //Set all button pins as input pullup. Change to INPUT if using external resistors.
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  for(int i = 0; i < 4; i++){ //Set all dpad button pins as input pullup.
    pinMode(dpadPins[i], INPUT_PULLUP);
  }
  
  eepromLoad(); //Check for stored joystick settings and load if applicable.
  rebuildLUTs(); //Build joystick LUT's from stored calibration values.
}

void loop() {
  buttonRead();
  if(!calibrationMode){
    joypadButtons();
    joystickInput();
    dPadInput();
    Joystick.sendState(); //Update input changes
  } else {
    joystickCalibration();
  }
  if(lastButtonState[0] == HIGH && lastButtonState[3] == HIGH){ //If this combination of buttons is pressed, begin joystick calibration. (Start and right joystick button)
    Serial.println("Calibration Mode Triggered");
    calibrationMode = true;
  }
  if(cycleTimeTest){
    pollingRateTest();
  }
}

void rebuildLUTs(){
  joystickBuildLUT(leftXLUT, minLeftX, midLeftX, maxLeftX, earlyLeftX, deadBandLeft);
  joystickBuildLUT(leftYLUT, minLeftY, midLeftY, maxLeftY, earlyLeftY, deadBandLeft);
  joystickBuildLUT(rightXLUT, minRightX, midRightX, maxRightX, earlyRightX, deadBandRight);
  joystickBuildLUT(rightYLUT, minRightY, midRightY, maxRightY, earlyRightY, deadBandRight);
}

void pollingRateTest(){ //Reports current polling rate in MS every second.
  float pollingMS = (micros() - lastPollingMS) / 1000;
  lastPollingMS = micros();
  if(millis() > lastPollingReport + 1000){ //If more than 1 second has passed:
    Serial.print(pollingMS);
    Serial.println("ms");
    lastPollingReport = millis();
  }
}

void buttonRead(){ //Read button inputs and set state arrays.
  for (int i = 0; i < buttonCount; i++){
    int input = !digitalRead(buttonPins[i]);
    if (input != lastButtonState[i]){
      lastButtonState[i] = input;
    }
  }
  for(int i = 0; i < 4; i++){
    int input = !digitalRead(dpadPins[i]);
    if (input != dpadPinsState[i]){
      dpadPinsState[i] = input;
    }
  }
}

void joypadButtons(){ //Set joystick buttons for USB output
  for(int i = 0; i < buttonCount; i++){
    if(lastButtonState[i] != currentButtonState[i]){
      Joystick.setButton(i, lastButtonState[i]);
      Serial.print("Button: ");
      Serial.println(i);
      currentButtonState[i] = lastButtonState[i];
    }
  }
}

void dPadInput(){ //D-Pad as RY and RZ Axis
  if(dpadPinsState[0] == 1){ //Up
    Joystick.setRyAxis(2);
  } else if(dpadPinsState[2] == 1){ //Down
    Joystick.setRyAxis(0);
  } else {
    Joystick.setRyAxis(1);
  }
  if(dpadPinsState[1] == 1){ //Right
    Joystick.setRzAxis(2);
  } else if(dpadPinsState[3] == 1){ //Left
    Joystick.setRzAxis(0);
  } else {
    Joystick.setRzAxis(1);
  }
}

void eepromLoad(){ //Loads stored settings from EEPROM
  if(readIntFromEEPROM(1) != -1){ //Check Joystick Calibration in EEPROM is not Empty
    readJoystickConfig(); //Load joystick calibration from EEPROM
  }
}

void writeIntIntoEEPROM(int address, int number){ //Splits Int into BYTES for EEPROM
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}

int readIntFromEEPROM(int address){ //Converts BYTES to INT from EEPROM
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

void readJoystickConfig(){ //Read joystick calibration from EEPROM
//Left X
  minLeftX = readIntFromEEPROM(1);
  maxLeftX = readIntFromEEPROM(3);
  midLeftX = readIntFromEEPROM(5);
//Left Y
  minLeftY = readIntFromEEPROM(7);
  maxLeftY = readIntFromEEPROM(9);
  midLeftY = readIntFromEEPROM(11);
//Right Y
  minRightY = readIntFromEEPROM(13);
  maxRightY = readIntFromEEPROM(15);
  midRightY = readIntFromEEPROM(17);
// Right X
  minRightX = readIntFromEEPROM(19);
  maxRightX = readIntFromEEPROM(21);
  midRightX = readIntFromEEPROM(23);
}

void writeJoystickConfig(){ //Store joystick calibration in EEPROM
//Left X
  writeIntIntoEEPROM(1, minLeftX);
  writeIntIntoEEPROM(3, maxLeftX);
  writeIntIntoEEPROM(5, midLeftX);
//Left Y
  writeIntIntoEEPROM(7, minLeftY);
  writeIntIntoEEPROM(9, maxLeftY);
  writeIntIntoEEPROM(11, midLeftY);
//Right Y
  writeIntIntoEEPROM(13, minRightY);
  writeIntIntoEEPROM(15, maxRightY);
  writeIntIntoEEPROM(17, midRightY);
// Right X
  writeIntIntoEEPROM(19, minRightX);
  writeIntIntoEEPROM(21, maxRightX);
  writeIntIntoEEPROM(23, midRightX);
}

int readJoystick(int joystickPin, boolean invertOutput){ //Reads raw joystick values and inverts if required
  int var = analogRead(joystickPin);
  if(invertOutput){
    var = 1023 - var;
    return var;
  } else {
    return var;
  }
}

void joystickCalibration(){ //Very rough at the moment but it works. Read usage instructions at top of page.
  buttonRead();
  if(calibrationStep == 1){
    if(lastButtonState[1] == 1){
      RXLED1;
      delay(100);
      RXLED0;
      midLeftX = readJoystick(leftJoyX, invertLeftX);
      midLeftY = readJoystick(leftJoyY, invertLeftY);
      midRightX = readJoystick(rightJoyX, invertRightX);
      midRightY = readJoystick(rightJoyY, invertRightY);
      calibrationStep = 2;
    }
  } else if(calibrationStep == 2){
      RXLED1;
      delay(100);
      RXLED0;
      minLeftX = midLeftX;
      minLeftY = midLeftY;
      maxLeftX = 0;
      maxLeftY = 0;
      minRightX = midRightX;
      minRightY = midRightY;
      maxRightX = 0;
      maxRightY = 0;
      calibrationStep = 3;
      delay(500);
  } else if(calibrationStep == 3){
    int var = readJoystick(leftJoyX, invertLeftX);
    if(var > maxLeftX) maxLeftX = var;
    if(var < minLeftX) minLeftX = var;
    var = readJoystick(leftJoyY, invertLeftY);
    if(var > maxLeftY) maxLeftY = var;
    if(var < minLeftY) minLeftY = var;
    var = readJoystick(rightJoyX, invertRightX);
    if(var > maxRightX) maxRightX = var;
    if(var < minRightX) minRightX = var;
    var = readJoystick(rightJoyY, invertRightY);
    if(var > maxRightY) maxRightY = var;
    if(var < minRightY) minRightY = var;
    buttonRead();
    if(lastButtonState[1] == 1){ //Complete Calibration
      RXLED1;
      delay(100);
      RXLED0;
      delay(200);
      writeJoystickConfig(); //Update EEPROM
      rebuildLUTs();
      calibrationStep = 1;
      calibrationMode = false;
    }
  }
}

void joystickBuildLUT(byte output[350], int minIn, int midIn, int maxIn, int earlyStop, int deadBand){ //This function builds a lookup table for the given axis.
  //Shift all joystick values to a base of zero. All values are halved due to ram limitations on the 32u4.
  int shiftedMin = 0;
  int shiftedMid = (midIn - minIn) /2;
  int shiftedMax = (maxIn - minIn) /2;
  int temp;
  
  for(int i = 0; i < 350; i++){
    if(i < shiftedMid){
      if(i > shiftedMin + earlyStop){
        temp = map(i, shiftedMin, shiftedMid - deadBand, 0, 127);
      } else {
        temp = 0;
      }
    } else {
      if(i < shiftedMax - earlyStop){
        temp = map(i, shiftedMid + deadBand, shiftedMax, 127, 254);
      } else {
        temp = 254;
      }
    }
    if(i < shiftedMid + deadBand && i > shiftedMid - deadBand){
      temp = 127;
    }
    output[i] = temp;
  }
}

//OLD
/*void joystickBuildLUT(byte output[350], int minIn, int midIn, int maxIn, int earlyStop, int deadBand){ //This function builds a lookup table for the given axis.
  //Shift all joystick values to a base of zero. All values are halved due to ram limitations on the 32u4.
  int shiftedMin = 0;
  int shiftedMid = (midIn - minIn) /2;
  int shiftedMax = (maxIn - minIn) /2;
  int temp;
  
  for(int i = 0; i < 350; i++){
    if(i < shiftedMid){
      temp = map(i, shiftedMin + earlyStop, shiftedMid - deadBand, 0, 127);
      if(temp < 0){
        temp = 0;
      } else if(temp > 254){
        temp = 254;
      }
    } else {
      temp = map(i, shiftedMid + deadBand, shiftedMax - earlyStop, 127, 254);
      if(temp < 0){
        temp = 0;
      } else if(temp > 254){
        temp = 254;
      }
    }
    if(i < shiftedMid + deadBand && i > shiftedMid - deadBand){
      temp = 127;
    }
    output[i] = temp;
  }
}*/

void joystickInput(){
  int var = 0;
  var = readJoystick(rightJoyY, invertRightY);
  var = (var - minRightY) / 2;
  Joystick.setRxAxis(rightYLUT[var]);

  var = readJoystick(rightJoyX, invertRightX);
  var = (var - minRightX) / 2;
  Joystick.setZAxis(rightXLUT[var]);

  var = readJoystick(leftJoyY, invertLeftY);
  var = (var - minLeftY) / 2;
  Joystick.setYAxis(leftYLUT[var]);

  var = readJoystick(leftJoyX, invertLeftX);
  var = (var - minLeftX) / 2;
  Joystick.setXAxis(leftXLUT[var]);
}
