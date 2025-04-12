/*
  Created by - LSteen
  Complete project details at https://github.com/thurstinn/RP5
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <RTCZero.h>

// Define pins
#define pwrBtn 1
#define enPin 0
#define piPwr 4
#define piOn 2
#define batV A3                

// Define variables
int adcRead;
float batVoltage;
bool pwrOn = true;                   
bool buttonHeld = false;             
bool awaitingShutdown = false;      
unsigned long buttonPressStart = 0; 
unsigned long lastDebounceTime = 0;
const float lowBat = 3.3;  
const unsigned long holdTime = 5000; 
const unsigned long debounceDelay = 50;
const unsigned long shutdownDelay = 2000;

RTCZero rtc;                         

void setup() {
  pinMode(pwrBtn, INPUT_PULLUP);     
  pinMode(enPin, OUTPUT);
  pinMode(piPwr, OUTPUT);
  pinMode(piOn, INPUT);
  pinMode(batV, INPUT);
  digitalWrite(enPin, LOW);         
  digitalWrite(piPwr, HIGH);

  Serial.begin(115200);
  while (!Serial);
  
  // Configure brown-out detection
  uint8_t resetCause = PM->RCAUSE.reg;

  if (resetCause & (1 << 2)) {
    Serial.println("Brown-out reset detected.");
  }

  while (!SYSCTRL->PCLKSR.bit.BOD33RDY);

  SYSCTRL->BOD33.reg =
      SYSCTRL_BOD33_LEVEL(48) |          // ~2.9V threshold (adjust as needed)
      SYSCTRL_BOD33_HYST |               // Enable hysteresis (optional)
      SYSCTRL_BOD33_RUNSTDBY |           // Keep BOD running in standby
      SYSCTRL_BOD33_ENABLE |             // Enable BOD
      SYSCTRL_BOD33_ACTION_RESET;        // Reset on brown-out
  
  while (!SYSCTRL->PCLKSR.bit.BOD33RDY);

  // Configure RTC
  attachInterrupt(digitalPinToInterrupt(pwrBtn), wakeFromSleep, FALLING); 
  
  rtc.begin();                       
  rtc.setAlarmSeconds(20);           
  rtc.enableAlarm(rtc.MATCH_SS);
  Serial.println(readBatVoltage());
  delay(1000);
  Serial.println(readBatVoltage());
}

// Setup adc 
float readBatVoltage() {
  analogReadResolution(12);
  const int numSamples = 20;
  int adcSum = 0;

  for (int i = 0; i < numSamples; i++) {
    adcSum += analogRead(batV);
    delay(5); // Small delay to allow ADC to settle
  }

  float adcAvg = adcSum / (float)numSamples;
  float batVoltage = adcAvg * (3.3 / 4096) * 2;

  return batVoltage;
}


void initiateShutdown() {
  digitalWrite(piPwr, LOW);           
  delay(100);                         
  digitalWrite(piPwr, HIGH);                   
  awaitingShutdown = true;
  Serial.println("Shutting down...");            
}

void completeShutdown() {
  delay(shutdownDelay);               
  digitalWrite(enPin, HIGH);                                
  awaitingShutdown = false;
  Serial.println("Powered off");
  pwrOn = false;           
}

void powerOnSystem() {
  digitalWrite(enPin, LOW);
  pwrOn = true;
  Serial.println("Powered on");
}

void handleButtonPress() {
  if (digitalRead(pwrBtn) == LOW) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastDebounceTime > debounceDelay) {
      lastDebounceTime = currentMillis; // Update debounce time

      if (buttonPressStart == 0) {
        buttonPressStart = millis();
      }

      if (millis() - buttonPressStart >= holdTime) {
        if (pwrOn) {
          initiateShutdown();
        } else {
          powerOnSystem();
        }
        buttonPressStart = 0;
      }
    }
  } else {
    // Reset button press state when released
    buttonPressStart = 0;
  }
}

void wakeFromSleep() {
  // Empty function just to handle wake interrupt
}

void enterSleepMode() {
  Serial.println("Entering sleep...");
  delay(100);
  PM->SLEEP.reg = PM_SLEEP_IDLE_Msk;
  __WFI();
                   
}

void loop() {
  handleButtonPress();
  if (awaitingShutdown && digitalRead(piOn) == HIGH) {
    completeShutdown();               
  } 
  else if (readBatVoltage() < lowBat && pwrOn) {
    Serial.println("Battery low, powering off");
    initiateShutdown();               
  }
  // Put microcontroller to sleep if system off
  if (!pwrOn) {
    enterSleepMode();                   
  }
}
