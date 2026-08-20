////-------------------------------BBK--------------------------------------//
//#include <Arduino.h>
////------------------------------------------------------------------------------------------------------------------//
//// pin numbers def
//#define TOUCH1_PIN            36
//#define TOUCH2_PIN            39
//#define TOUCH3_PIN            34
//#define TOUCH4_PIN            35
//#define TOUCH5_PIN            25//33
//#define TOUCH6_PIN            26//25
//#define TOUCH7_PIN            27//26
//#define TOUCH8_PIN            33//27
//#define LONG_PRESS_THRESHOLD  1000 //(50 MS)
//
////------------------------------------------------------------------------------------------------------------------//
//volatile unsigned long pressStartTime[8] = {0};
//volatile bool isPressing[8] = {false};
//volatile bool shortPressState[8] = {false};
//volatile bool longPressState[8] = {false};
//volatile unsigned long lastInterruptTime[8] = {0};
//uint8_t L1_PRESSED = 0, L2_PRESSED = 0, L3_PRESSED = 0;
//uint8_t R1_PRESSED = 0, R2_PRESSED = 0, R3_PRESSED = 0;
//uint8_t POWER_PRESSED = 0, FAN_PRESSED = 0;
//uint8_t L1_PRESSED_LONG = 0, L2_PRESSED_LONG = 0, L3_PRESSED_LONG = 0;
//uint8_t R1_PRESSED_LONG = 0, R2_PRESSED_LONG = 0, R3_PRESSED_LONG = 0;
//uint8_t FAN_PRESSED_LONG = 0, POWER_PRESSED_LONG = 0;
//const int touchPins[8] = {TOUCH1_PIN, TOUCH2_PIN, TOUCH3_PIN, TOUCH4_PIN, TOUCH5_PIN, TOUCH6_PIN, TOUCH7_PIN, TOUCH8_PIN};
////extern bool fan_speed_sent,fan_speed_sent_SHORT;
//////------------------------------------------------------------------------------------------------------------------//
//void init_all_touch_buttons();
//void IRAM_ATTR handleInterrupt(void* arg);
//void processButtonStates();
//
//////------------------------------------------------------------------------------------------------------------------//
//
//void init_all_touch_buttons()
//{
//  for (int i = 0; i < 8; i++)
//  {
//    pinMode(touchPins[i], INPUT_PULLUP);
//    attachInterruptArg(digitalPinToInterrupt(touchPins[i]), handleInterrupt, (void*)i, CHANGE);
//  }
//}
////------------------------------------------------------------------------------------------------------------------//
//
//void IRAM_ATTR handleInterrupt(void* arg)
//{
//  int buttonIndex = (int)arg;
//  bool isPressed = digitalRead(touchPins[buttonIndex]) == HIGH; //LOW
//
//  unsigned long currentTime = millis();
//  if (currentTime - lastInterruptTime[buttonIndex] < 50)
//  {
//    return;
//  }
//  lastInterruptTime[buttonIndex] = currentTime;
//
//  if (isPressed)
//  {
//    if (!isPressing[buttonIndex])
//    {
//      isPressing[buttonIndex] = true;
//      pressStartTime[buttonIndex] = millis();
//    }
//  }
//  else
//  {
//    if (isPressing[buttonIndex])
//    {
//      isPressing[buttonIndex] = false;
//      unsigned long pressDuration = millis() - pressStartTime[buttonIndex];
//      if (pressDuration < LONG_PRESS_THRESHOLD)
//      {
//        shortPressState[buttonIndex] = true;
//      }
//      else
//      {
//        longPressState[buttonIndex] = true;
//      }
//
//      pressStartTime[buttonIndex] = 0;
//    }
//  }
//}
//
////------------------------------------------------------------------------------------------------------------------//
//
//void processButtonStates()
//{
//  for (int i = 0; i < 8; i++)
//  {
//    unsigned long pressDuration = millis() - pressStartTime[i];
//    if (isPressing[i] && pressDuration >= LONG_PRESS_THRESHOLD && !longPressState[i])
//    {
//      longPressState[i] = true;
//      Serial.print("Button ");
//      Serial.print(i + 1);
//      Serial.print(": Long Press detected. Pin State: ");
//    }
//    if (shortPressState[i])
//    {
//      Serial.print("Button ");
//      Serial.print(i + 1);
//      Serial.print(": Short Press detected. Pin State: ");
//      switch (i)
//      {
//        case 0: POWER_PRESSED = 1; break;
//        case 1: L1_PRESSED = 1; break;
//        case 2: L2_PRESSED = 1; break;
//        case 3: L3_PRESSED = 1; break;
//        case 4: R1_PRESSED = 1; break;
//        case 5: R2_PRESSED = 1; break;
//        case 6: R3_PRESSED = 1; break;
//        case 7: FAN_PRESSED = 1; break;
//      }
//      shortPressState[i] = false;
//    }
//
//    if (longPressState[i])
//    {
//      Serial.print("Button ");
//      Serial.print(i + 1);
//      Serial.print(": Long Press detected (Action). Pin State: ");
//      switch (i)
//      {
//        case 0: POWER_PRESSED_LONG = 1; break;
//        case 1: L1_PRESSED_LONG = 1; break;
//        case 2: L2_PRESSED_LONG = 1; break;
//        case 3: L3_PRESSED_LONG = 1; break;
//        case 4: R1_PRESSED_LONG = 1; break;
//        case 5: R2_PRESSED_LONG = 1; break;
//        case 6: R3_PRESSED_LONG = 1; break;
//        case 7: FAN_PRESSED_LONG = 1; break;
//      }
//
//      longPressState[i] = false;
//
//    }
//  }
//}
//------------------------------------------------------------------------------------------------------------------//

////-------------------------------BBK--------------------------------------//
//#include <Arduino.h>
//
////------------------------------------------------------------------------------------------------------------------//
//// Pin numbers definition
//#define TOUCH1_PIN 36
//#define TOUCH2_PIN 39
//#define TOUCH3_PIN 34
//#define TOUCH4_PIN 35
//#define TOUCH5_PIN 25
//#define TOUCH6_PIN 26
//#define TOUCH7_PIN 27
//#define TOUCH8_PIN 33
//
//#define FILTER_LEN 2                // Debounce filter length
//#define LONG_PRESS_MS 1000          // Long press detection time (ms)
////#define SHORT_PRESS_MS 500
//#define SAMPLE_INTERVAL 50          // Sampling interval (ms)
////------------------------------------------------------------------------------------------------------------------//
//const int touchPins[8] = {TOUCH1_PIN, TOUCH2_PIN, TOUCH3_PIN, TOUCH4_PIN, TOUCH5_PIN, TOUCH6_PIN, TOUCH7_PIN, TOUCH8_PIN};
//
//// Filter registers and states
//unsigned int filterReg[8] = {0};
//bool stableState[8] = {LOW};
//unsigned long pressStartTime[8] = {0};
//bool longPressReported[8] = {false};
//
//// Flags for short/long press
//uint8_t L1_PRESSED = 0, L2_PRESSED = 0, L3_PRESSED = 0;
//uint8_t R1_PRESSED = 0, R2_PRESSED = 0, R3_PRESSED = 0;
//uint8_t POWER_PRESSED = 0, FAN_PRESSED = 0;
//uint8_t L1_PRESSED_LONG = 0, L2_PRESSED_LONG = 0, L3_PRESSED_LONG = 0;
//uint8_t R1_PRESSED_LONG = 0, R2_PRESSED_LONG = 0, R3_PRESSED_LONG = 0;
//uint8_t FAN_PRESSED_LONG = 0, POWER_PRESSED_LONG = 0;
//
////------------------------------------------------------------------------------------------------------------------//
//void init_all_touch_buttons();
//void processButtonStates();
//
////------------------------------------------------------------------------------------------------------------------//
//
//
////------------------------------------------------------------------------------------------------------------------//
//void init_all_touch_buttons()
//{
//  Serial.println("BBK Touch Button with polling logic with filter ");
//  for (int i = 0; i < 8; i++)
//  {
//    pinMode(touchPins[i], INPUT);
//  }
//}
//
////------------------------------------------------------------------------------------------------------------------//
//void processButtonStates()
//{
//  for (int i = 0; i < 8; i++)
//  {
//    // Read raw input
//    int rawState = digitalRead(touchPins[i]);
//
//    // Shift register filter
//    filterReg[i] = ((filterReg[i] << 1) | rawState) & ((1 << FILTER_LEN) - 1);
//
//    // Stable LOW
//    if (filterReg[i] == 0)
//    {
//      if (stableState[i] != LOW)
//      {
//        stableState[i] = LOW;
//        unsigned long pressDuration = millis() - pressStartTime[i];
//
//        if (!longPressReported[i] && pressDuration < LONG_PRESS_MS)
//        {
//          // Short press detected
//          switch (i)
//          {
//            case 0: POWER_PRESSED = 1; break;
//            case 1: L1_PRESSED = 1; break;
//            case 2: L2_PRESSED = 1; break;
//            case 3: L3_PRESSED = 1; break;
//            case 4: R1_PRESSED = 1; break;
//            case 5: R2_PRESSED = 1; break;
//            case 6: R3_PRESSED = 1; break;
//            case 7: FAN_PRESSED = 1; break;
//          }
//          Serial.print("Button ");
//          Serial.print(i + 1);
//          Serial.println(": Short Press detected");
//        }
//      }
//    }
//    // Stable HIGH
//    else if (filterReg[i] == ((1 << FILTER_LEN) - 1))
//    {
//      if (stableState[i] != HIGH)
//      {
//        stableState[i] = HIGH;
//        pressStartTime[i] = millis();
//        longPressReported[i] = false;
//        Serial.print("Button ");
//        Serial.print(i + 1);
//        Serial.println(": Pressed (Stable HIGH)");
//      }
//    }
//
//    // Long press detection
//    if (stableState[i] == HIGH && !longPressReported[i])
//    {
//      if (millis() - pressStartTime[i] >= LONG_PRESS_MS)
//      {
//        longPressReported[i] = true;
//        switch (i)
//        {
//          case 0: POWER_PRESSED_LONG = 1; break;
//          case 1: L1_PRESSED_LONG = 1; break;
//          case 2: L2_PRESSED_LONG = 1; break;
//          case 3: L3_PRESSED_LONG = 1; break;
//          case 4: R1_PRESSED_LONG = 1; break;
//          case 5: R2_PRESSED_LONG = 1; break;
//          case 6: R3_PRESSED_LONG = 1; break;
//          case 7: FAN_PRESSED_LONG = 1; break;
//        }
//        Serial.print("Button ");
//        Serial.print(i + 1);
//        Serial.println(": Long Press detected");
//      }
//    }
//  }
//}
//------------------------------------------------------------------------------------------------------------------//


//-----------------------------------------------BBK--Digital Filter------------------------------------------------//
#include <Arduino.h>

//------------------------------------------------------------------------------------------------------------------//
// Pin numbers definition ttp224
#define TOUCH1_PIN 36
#define TOUCH2_PIN 39
#define TOUCH3_PIN 34
#define TOUCH4_PIN 35
#define TOUCH5_PIN 25
#define TOUCH6_PIN 26
#define TOUCH7_PIN 27
#define TOUCH8_PIN 33

#define FILTER_LEN 3        //2
#define LONG_PRESS_MS 800   // Long press threshold (ms)
#define SHORT_PRESS_MS 50   // Min duration for valid short press
#define SAMPLE_INTERVAL 10  // Faster sampling (10ms) \

// Pin numbers definition holtek
// #define TOUCH1_PIN 36
// #define TOUCH2_PIN 39
// #define TOUCH3_PIN 34
// #define TOUCH4_PIN 35
// #define TOUCH5_PIN 26
// #define TOUCH6_PIN 25
// #define TOUCH7_PIN 33
// #define TOUCH8_PIN 27
//------------------------------------------------------------------------------------------------------------------//
const int touchPins[8] = { TOUCH1_PIN, TOUCH2_PIN, TOUCH3_PIN, TOUCH4_PIN, TOUCH5_PIN, TOUCH6_PIN, TOUCH7_PIN, TOUCH8_PIN };

// Filter registers and states
unsigned int filterReg[8] = { 0 };
bool stableState[8] = { LOW };
unsigned long pressStartTime[8] = { 0 };
bool longPressReported[8] = { false };

// Flags for short/long press
uint8_t L1_PRESSED = 0, L2_PRESSED = 0, L3_PRESSED = 0;
uint8_t R1_PRESSED = 0, R2_PRESSED = 0, R3_PRESSED = 0;
uint8_t POWER_PRESSED = 0, FAN_PRESSED = 0;
uint8_t L1_PRESSED_LONG = 0, L2_PRESSED_LONG = 0, L3_PRESSED_LONG = 0;
uint8_t R1_PRESSED_LONG = 0, R2_PRESSED_LONG = 0, R3_PRESSED_LONG = 0;
uint8_t FAN_PRESSED_LONG = 0, POWER_PRESSED_LONG = 0;

//------------------------------------------------------------------------------------------------------------------//
void init_all_touch_buttons() {
  //  Serial.println("Touch Button Initialized - Digital Filter based");
  for (int i = 0; i < 8; i++) {
    pinMode(touchPins[i], INPUT);
  }
}

//------------------------------------------------------------------------------------------------------------------//
void processButtonStates() {
  for (int i = 0; i < 8; i++) {
    // int rawState = !digitalRead(touchPins[i]);  //holtek
    int rawState = digitalRead(touchPins[i]);  //ttp224

    filterReg[i] = ((filterReg[i] << 1) | rawState) & ((1 << FILTER_LEN) - 1);
    // Stable LOW = button released
    if (filterReg[i] == 0) {
      if (stableState[i] != LOW) {
        stableState[i] = LOW;
        unsigned long pressDuration = millis() - pressStartTime[i];

        if (!longPressReported[i] && pressDuration >= SHORT_PRESS_MS && pressDuration < LONG_PRESS_MS) {
          switch (i) {
            case 0: POWER_PRESSED = 1; break;
            case 1: L1_PRESSED = 1; break;
            case 2: L2_PRESSED = 1; break;
            case 3: L3_PRESSED = 1; break;
            case 4: R1_PRESSED = 1; break;
            case 5: R2_PRESSED = 1; break;
            case 6: R3_PRESSED = 1; break;
            case 7: FAN_PRESSED = 1; break;
          }
          // Serial.printf("Button %d: Short Press (%lu ms)\n", i + 1, pressDuration);
        }
      }
    } else if (filterReg[i] == ((1 << FILTER_LEN) - 1)) {
      if (stableState[i] != HIGH) {
        stableState[i] = HIGH;
        pressStartTime[i] = millis();
        longPressReported[i] = false;
      }
    }

    // Long press detection
    if (stableState[i] == HIGH && !longPressReported[i]) {
      if (millis() - pressStartTime[i] >= LONG_PRESS_MS) {
        longPressReported[i] = true;
        switch (i) {
          case 0: POWER_PRESSED_LONG = 1; break;
          case 1: L1_PRESSED_LONG = 1; break;
          case 2: L2_PRESSED_LONG = 1; break;
          case 3: L3_PRESSED_LONG = 1; break;
          case 4: R1_PRESSED_LONG = 1; break;
          case 5: R2_PRESSED_LONG = 1; break;
          case 6: R3_PRESSED_LONG = 1; break;
          case 7: FAN_PRESSED_LONG = 1; break;
        }
        // Serial.printf("Button %d: Long Press\n", i + 1);
      }
    }
  }
}
//------------------------------------------------------------------------------------------------------------------//
