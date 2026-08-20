//------------------------------------------------BBK---------------------------------------------------------------//
#include <radio.h>
#include <RDA5807M.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
//------------------------------------------------------------------------------------------------------------------//
#define FM_SDA 21
#define FM_SCL 32
#define FIX_BAND RADIO_BAND_FM
#define MAX_VOLUME 20
#define MIN_VOLUME 0
#define STD_VOLUME 3
#define FIX_STATION 9000
#define MAX_TUNED_STATIONS 32
#define FM_STATION_SIZE (40 * sizeof(float))
#define RDA5807M_I2C_ADDRESS 0x20  // Default I2C address for RDA5807M
//------------------------------------------------------------------------------------------------------------------//
RDA5807M radio;


float getfrequencies[] = {
  9040, 9080, 9110, 9120, 9190, 9270, 9350, 9430, 9440, 9450,
  9500, 9830, 9950, 10090, 10130, 10150, 10230, 10280, 10290, 10300,
  10400, 10450, 10480, 10500, 10560, 10620, 10640, 10660, 10680, 10720,
  10750, 10800
};
int baseRSSI;
int station_num = 0;
uint8_t radio_volume = 5;
bool isActive = false;
bool isStereo = false;
bool hasRDS = false;
int rssi = 0;
bool isTuned = false;
bool isMono = false;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000;
float tunedStations[MAX_TUNED_STATIONS];
int tunedCount = 0;
int currentFreq = 8900;
extern uint8_t L1_PRESSED,
  L2_PRESSED,
  L3_PRESSED,
  R1_PRESSED,
  R2_PRESSED,
  R3_PRESSED,
  POWER_PRESSED,
  FAN_PRESSED,
  L1_PRESSED_LONG,
  L2_PRESSED_LONG,
  L3_PRESSED_LONG,
  R1_PRESSED_LONG,
  R2_PRESSED_LONG,
  R3_PRESSED_LONG,
  FAN_PRESSED_LONG,
  POWER_PRESSED_LONG;
//--------------------------------------------------Function Definitions----------------------------------------------//
void fm_radio_init();
void reinitializeRadio();
void Set_Selected_FM_Station(int fm_channel);
void scanStations();
void saveTunedStations();
void loadTunedStations();
void showTunedStations();
void eraseEEPROM();
void printCurrentFrequency();
float getCurrentFrequency();
void turn_off_radio();
void set_volume(uint8_t volume);
//--------------------------------------------------Function Declarations-------------------------------------------//
void fm_radio_init() {
  // Enable debugging
  radio.debugEnable(true);
  radio._wireDebug(false);

  // Setup radio spacing and de-emphasis
  radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_100);
  radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);

  // Initialize I2C
  Wire.begin(FM_SDA, FM_SCL);
  Wire.setClock(100000);

  // Initialize radio chip
  if (!radio.initWire(Wire)) {
    Serial.println("No radio chip found. Restarting...");
    delay(2000);
    ESP.restart();
  }

  // Load stations from EEPROM
  loadTunedStations();

  // Set default band and frequency
  radio.setBandFrequency(FIX_BAND, 8900);

  delay(500);  // small delay for stability

  // Read base RSSI
  RADIO_INFO info;
  radio.getRadioInfo(&info);
  baseRSSI = info.rssi;

  // Set audio settings
  radio.setVolume(STD_VOLUME);
  radio.setMono(false);
  radio.setMute(false);
  radio.setBassBoost(true);

  Serial.println("FM Radio initialized successfully!");
}
//------------------------------------------------------------------------------------------------------------------//
void reinitializeRadio() {
  Wire.setClock(100000);  //naveen
  Wire.begin(FM_SDA, FM_SCL);
  delay(100);
  Wire.beginTransmission(RDA5807M_I2C_ADDRESS);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(500);
  if (!radio.initWire(Wire)) {
    delay(2000);
  }
}
//------------------------------------------------------------------------------------------------------------------//
void Set_Selected_FM_Station(int fm_channel) {
  if (fm_channel < 0 || fm_channel >= tunedCount) {
    //    Serial.println("Invalid FM channel index.");
    return;
  }
  //  Serial.print("Setting station to: ");
  //  Serial.print(tunedStations[fm_channel], 2);
  //  Serial.println(" MHz");
  radio.setBandFrequency(FIX_BAND, tunedStations[fm_channel] * 100);
  radio.setVolume(radio_volume);
  radio.setMono(false);  //naveen
  radio.setMute(false);
  radio.debugRadioInfo();
  radio.debugAudioInfo();
}
//------------------------------------------------------------------------------------------------------------------//
void scanStations() {
  int prevRssi = 0;
  eraseEEPROM();
  //  Serial.println("Starting FM station scan...");
  radio.setVolume(0);
  tunedCount = 0;
  currentFreq = 0;
  while (currentFreq <= MAX_TUNED_STATIONS /*10800*/ && tunedCount < MAX_TUNED_STATIONS) {
    processButtonStates();
    if (R1_PRESSED) {
      R1_PRESSED = 0;
      machine_state = fm_screen_init;
      //      Serial.println("Scanning stopped by user.");
      break;
    }
    if (FAN_PRESSED) {
      Serial.println("Fan button pressed.");
      FAN_PRESSED = false;
      fan_speed++;
      if (fan_speed > 3)
        fan_speed = 0;
      send_fan_speed_to_chimney(fan_speed);
    }
    delay(150);  //naveen
    radio.setBandFrequency(FIX_BAND, getfrequencies[currentFreq]);
    //    radio.setFrequency(getfrequencies[currentFreq]);
    delay(500);
    RADIO_INFO info;
    //    radio.getRadioInfo(&info);
    //    Serial.print("Frequency: ");
    //    Serial.print(getfrequencies[currentFreq] / 100.0, 2);
    int currRssi = info.rssi;
    //    Serial.print(" Tuned Station: ");
    //    Serial.println(tunedCount);
    radio.debugRadioInfo();
    //    Serial.println();
    //    if (info.tuned && info.rssi > 24)//24)
    //    if (info.tuned && info.rssi > baseRSSI && info.stereo )
    {
      //      if (abs(currentFreq - (tunedStations[tunedCount - 1] * 100)) > 10)// && currRssi>prevRssi)
      {
        tunedStations[tunedCount] = getfrequencies[tunedCount] / 100.0;  // currentFreq / 100.0;
        tunedCount++;

        //        Serial.print("Station saved: ");
        //        Serial.print(getfrequencies[tunedCount] / 100.0, 2);
        //        Serial.println(" MHz");
      }
    }
    currentFreq += 1;  //10
    prevRssi = currRssi;
    tft.fillRect(100, 120, 20 * 6, 25, ILI9341_BLACK);
    display_frequency(120, 140, getfrequencies[tunedCount] / 100.0, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
  }
  if (tunedCount == 0) {
    //    Serial.println("No stations tuned. Retry or check reception.");
    machine_state = fm_no_station_tuned;
  } else {
    //    Serial.println("Scan completed.");
    showTunedStations();
    saveTunedStations();
    machine_state = fm_screen_init;
  }
}
//------------------------------------------------------------------------------------------------------------------//
void saveTunedStations() {
  //  Serial.println("Saving tuned stations to EEPROM...");
  for (int i = 0; i < tunedCount; i++) {
    EEPROM.put(i * sizeof(float), tunedStations[i]);
  }
  EEPROM.commit();
  //  Serial.println("Stations saved.");
}
//------------------------------------------------------------------------------------------------------------------//
void loadTunedStations() {
  //  Serial.println("Loading tuned stations from EEPROM...");
  tunedCount = 0;

  for (int i = 0; i < MAX_TUNED_STATIONS; i++) {
    float station;
    EEPROM.get(i * sizeof(float), station);
    if (station >= 87.0 && station <= 108.0) {
      tunedStations[tunedCount] = station;
      tunedCount++;
    }
  }

  //  Serial.print("Loaded station count: ");
  //  Serial.println(tunedCount);

  if (tunedCount > 0) {
    //    Serial.println("Tuned stations loaded:");
    showTunedStations();
  } else {
    //    Serial.println("No stations found in EEPROM.");
  }
}
//------------------------------------------------------------------------------------------------------------------//
void showTunedStations() {
  //  Serial.println("Saved Stations:");
  for (int i = 0; i < tunedCount; i++) {
    //    Serial.print("Station ");
    //    Serial.print(i + 1);
    //    Serial.print(": ");
    //    Serial.print(tunedStations[i], 2);
    //    Serial.print (" MHz");
  }
}
//------------------------------------------------------------------------------------------------------------------//
void eraseEEPROM() {
  //  Serial.println("Erasing all EEPROM data...");
  for (int i = 0; i < (FM_STATION_SIZE); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  //  Serial.println("EEPROM erased.");
}
//------------------------------------------------------------------------------------------------------------------//
void printCurrentFrequency() {
  char s[12];
  radio.formatFrequency(s, sizeof(s));
  //  Serial.print("Current Station: ");
  //  Serial.println(s);
}
//------------------------------------------------------------------------------------------------------------------//
float getCurrentFrequency() {
  uint16_t freqInKHz = radio.getFrequency();
  return freqInKHz / 100.0;
}
//------------------------------------------------------------------------------------------------------------------//
void turn_off_radio() {
  radio.term();
}
//------------------------------------------------------------------------------------------------------------------//
void set_volume(uint8_t volume) {
  radio.setVolume(volume);
}
//------------------------------------------------------------------------------------------------------------------//
