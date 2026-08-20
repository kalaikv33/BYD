/*---------------------------------------BBK---------------------------------------------------------
  1.Bluetooth
  2.FM
  3.Whistle Counter
  4.Light and fan control
  5.Auto shut-off
  6.Auto clean
  7.Gesture Control
  Version-------1.29----------(//1.26/28(Bugs Fixed)
                               //1.23(UI Fix, features and touch key logic changed)
                               //1.22(GESTURE UI)
                               //1.21(HYD/Gesture)
                               //1.20(eshwar)
                               //1.19(OTA Final)
                               //)
  ---------------------------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <RDA5807M.h>
#include <EEPROM.h>
#include <radio.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
//---------------------------------------------------------------------------------------------------//
#define UART_RX 16
#define UART_TX 17
#define EEPROM_SIZE 512  //((40 * sizeof(float))+12+4)
#define SETTING_ADDR ((40 * sizeof(float)) + 1)
#define BUFFER_SIZE 64
#define SAMPLE_INTERVAL 35  // Sampling interval (ms)
// Reserve address 0 for EEPROM init check
#define INIT_FLAG_ADDR 0  // 2 bytes reserved for init flag
#define MAGIC_KEY 0xBEEF  // marker for EEPROM init (choose any 16-bit value)
//---------------------------------------------------------------------------------------------------//
typedef enum {
  idle,
  home_screen_init,
  home_screen,
  bluetooth_screen_init,
  bluetooth_connection_init,
  bluetooth_playback,
  bluetooth_screen,
  bluetooth_idle,
  bluetooth_screen_idle,
  fm_checking_page,
  fm_screen_init,
  fm_begin,
  fm_screen,
  fm_screen_autoscan_init,
  fm_screen_autoscan,
  timer_screen_init,
  timer_screen,
  settings_screen_init,
  settings_screen,
  auto_turn_off_delay_init,
  auto_turn_off_delay,
  auto_turn_off_delay_countdown,
  auto_turn_off_delay_countdown_init,
  Whistle_Counter,
  Whistle_Counter_loop,
  Whistle_Counter_SET_VAL,
  Whistle_Counter_counting,
  settings_poll,
  settings_screen_info,
  fm_no_station_tuned,
  settings_screen_wifi,
  auto_clean_init,
  Set_clean_time_val,
  Set_clean_time_countdown,
  Whistle_Counter_completed,
  Whistle_Counter_setting_init,
  Whistle_Counter_setting_poll,
  settings_screen_wifi_OTA,
  settings_screen_OTA,
  settings_screen_wifi_OTA_poll,
  settings_screen_wifi_OTA_no_update,
  settings_screen_wifi_OTA_error,
  settings_screen_wifi_poll,
  settings_screen_wifi_wait,
  settings_screen_wifi_disconnect,
  settings_screen_wifi_OTA_fail,
  customercare,
  customercare_idle,
  bluetooth_screen_init_switch,
} __machine_display_states;

__machine_display_states machine_state;
//---------------------------------------------------------------------------------------------------//
unsigned long lastSampleTime = 0;
uint8_t uart_tx_buffer[64];
uint8_t bluetooth_a2dp_flag, a2dp_used, a2dp_deinit_completed;
uint8_t rx_buffer[BUFFER_SIZE];
uint8_t buf_index = 0;
uint8_t firmware_version_venus[3];
uint8_t eerprom_init_flag = 0;
//---------------------------------------------------------------------------------------------------//
float venus_version;
//---------------------------------------------------------------------------------------------------//
extern uint8_t light_brightness;
extern uint8_t fan_speed;
extern float firmware_verison;
extern float firmware_verison_saved;
extern uint16_t settings_buff[5];  // = {370, 430, 15, 30, 10};
//-----------------------------------------Declaration-Function--------------------------------------//
void Display_Init_9341();
void display_text_center(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_text(int16_t x, int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value_2_digit(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_frequency(int16_t x, int16_t y, float frequency, uint16_t text_color, uint16_t bg, uint8_t text_size);
void turnOffDisplay();
void turnOnDisplay();
void reinitializeDisplay();
void display_value_float(int16_t x, int16_t y, float value, uint16_t text_color, uint16_t bg, uint8_t text_size, uint8_t decimal_places);
void display_value_center(int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void wipeScreenFromCenter(uint16_t color);
void display_text_center_2(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value_2(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_text_2(int16_t x, int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value_2_digit_2(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, const GFXfont *font);
void display_frequency_2(int16_t x, int16_t y, float frequency, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_text_center_MAC(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
//---------------------------------------------------------------------------------------------------//
void IRAM_ATTR onTimer();
void IRAM_ATTR onTimer2();
void startCountdown(int seconds);
void startCountdownautoclean(int seconds);
void printRemainingTime();
void printRemainingTimeautoclean();
void stopTimer();
void stopTimer2();
void display_process();
//---------------------------------------------------------------------------------------------------//
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
//---------------------------------------------------------------------------------------------------//
void send_fan_speed_to_chimney(uint8_t fan_speed);
void send_light_to_chimney(uint8_t light);
void volumeUp();
void volumeDown();
void nextStation();
void previousStation();
void keypad_process();
//---------------------------------------------------------------------------------------------------//
void displayStatus();
void processCommand(String command);
void a2dp_play();
void a2dp_pause();
void a2dp_previous();
void a2dp_forward();
void a2dp_rewind();
void a2dp_stop();
void read_data_stream(const uint8_t *data, uint32_t length);
void avrc_metadata_callback(uint8_t attribute_id, const uint8_t *value);
void connection_state_callback(esp_a2d_connection_state_t state, void *param);
void Ble_sound_init();
void Ble_sound_deinit();
void deinitBluetoothA2DP();
void resetBluetoothStack();
void cleanUp();
void disconnectAndPrepareForPairing();
void clearBondedDevices();
void enterPairingMode();
void beep(int frequency, int duration_ms);
void disableBluetooth();
void volumeUp_BLE();
void volumeDown_BLE();
void initI2S();
void deinitI2S();
//---------------------------------------------------------------------------------------------------//
void init_all_touch_buttons();
void processButtonStates();
void startBeep(int frequency, int duration_ms);
void updateBeep();
//---------------------------------------------------------------------------------------------------//
void Whistle_Setup();
void Process_Whistle_Count();
void beep(int frequency, int duration_ms);
//---------------------------------------------------------------------------------------------------//
void OTASetup();
void OTALoop();
void ConnectToWiFi();
void GetDeviceId();
void Get_OTA_Filename();
void ConnectToAWS();
void MQTTCallback(char *topic, byte *payload, unsigned int length);
void OTAUpdate(const char *filename);
void OTAUpdateRoutine();
bool ValidateFirmwareSize(const char *url);
float read_firmware_version();
void write_firmware_version(float firmware_version);
//---------------------------------------------------------------------------------------------------//
void startAPMode();
void handleRootPage();
void handleWifiList();
void handleConnect();
void WifiSetup();
void WifiLoop();
void reset_wifi();
void wifi_off();
//------------------------------ Function Defnition-------------------------------------------------//
//void transmit64BytesUART2(uint8_t *data)
//{
//  if (data == nullptr)
//  {
//    Serial.println("Error: Null data pointer.");
//    return;
//  }
//  for (int i = 0; i < 64; i++)
//  {
//    UART2.write(data[i]);
//  }
//  UART2.flush();
//}
//---------------------------------------------------------------------------------------------------//
void transmit64BytesUART2(uint8_t *data) {
  if (data == nullptr) {
    //    Serial.println("Error: Null data pointer.");
    return;
  }
  Serial2.write(data, 64);
  //UART2.flush();
  delay(100);
}
//---------------------------------------------------------------------------------------------------//
void receive_from_stm() {  //stm32 data
  static bool in_packet = false;
  static uint8_t rx_index = 0;

  while (Serial2.available()) {
    uint8_t byte_in = Serial2.read();

    if (!in_packet) {
      if (byte_in == 0x13)  // Start of packet
      {
        in_packet = true;
        rx_index = 0;
        rx_buffer[rx_index++] = byte_in;
      }
    } else {
      rx_buffer[rx_index++] = byte_in;

      if (rx_index == 64) {
        in_packet = false;

        if (rx_buffer[63] == 0x12) {
          //          Serial.println("Valid Packet Received:");
          //          for (int i = 0; i < 64; i++)
          //          {
          //            Serial.print(rx_buffer[i], HEX);
          //            Serial.print(" ");
          //          }
          light_brightness = rx_buffer[2];
          fan_speed = rx_buffer[4];
          firmware_version_venus[0] = rx_buffer[5];  // major
          firmware_version_venus[1] = rx_buffer[6];  // minor
          venus_version = firmware_version_venus[0] + (firmware_version_venus[1] / 10.0f);
        } else {
          //          Serial.println("Invalid Packet Format");
        }

        memset(rx_buffer, 0, 64);
        rx_index = 0;
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------//
//void receive_from_stm()
//{
//  static bool in_packet = false;
//  static uint8_t rx_index = 0;
//
//  while (Serial2.available())
//  {
//    uint8_t byte_in = Serial2.read();
//
//    if (!in_packet)
//    {
//      // Look for start of packet
//      if (byte_in == 0x13)
//      {
//        in_packet = true;
//        rx_index = 0;
//        rx_buffer[rx_index++] = byte_in;
//      }
//    }
//    else
//    {
//      // Already inside a packet
//      rx_buffer[rx_index++] = byte_in;
//
//      // If start byte is seen again mid-packet, resync
//      if (byte_in == 0x13 && rx_index < 64)
//      {
//        rx_index = 1;                // restart from beginning
//        rx_buffer[0] = 0x13;
//        continue;
//      }
//
//      // Check if we reached full packet
//      if (rx_index >= 64)
//      {
//        in_packet = false;  // reset for next packet
//
//        // Validate packet: first & last bytes
//        if (rx_buffer[0] == 0x13 && rx_buffer[63] == 0x12)
//        {
//          // Extract values
//          light_brightness = rx_buffer[2];
//          fan_speed = rx_buffer[4];
//
//          firmware_version_venus[0] = rx_buffer[5]; // major
//          firmware_version_venus[1] = rx_buffer[6]; // minor
//          venus_version = firmware_version_venus[0] + (firmware_version_venus[1] / 10.0f);
//
//          // Debug (optional)
//          /*
//          Serial.println("Valid Packet:");
//          for (int i = 0; i < 64; i++) {
//            Serial.print(rx_buffer[i], HEX);
//            Serial.print(" ");
//          }
//          Serial.println();
//          */
//        }
//        else
//        {
//          // Debug (optional)
//          // Serial.println("Invalid Packet");
//        }
//
//        // Reset index for next packet
//        rx_index = 0;
//      }
//    }
//  }
//}
//---------------------------------------------------------------------------------------------------//
void everything_off() {
  //Clean and reset radio& bluetooth
  reinitializeRadio();
  cleanUp();

  // Turn OFF FAN
  uart_tx_buffer[0] = 0x13;
  uart_tx_buffer[1] = 0x03;
  uart_tx_buffer[2] = 0x00;
  uart_tx_buffer[63] = 0x12;
  transmit64BytesUART2(uart_tx_buffer);

  // Turn OFF LIGHT
  uart_tx_buffer[0] = 0x13;
  uart_tx_buffer[1] = 0x02;
  uart_tx_buffer[2] = 0x00;
  uart_tx_buffer[63] = 0x12;
  transmit64BytesUART2(uart_tx_buffer);

  // Turn OFF HEATER
  uart_tx_buffer[0] = 0x13;
  uart_tx_buffer[1] = 0x04;
  uart_tx_buffer[2] = 0x00;
  uart_tx_buffer[63] = 0x12;
  transmit64BytesUART2(uart_tx_buffer);
}
//---------------------------------------------------------------------------------------------------//

void eeprom_setup_init() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    //    Serial.println("Failed to initialize EEPROM.");
    ESP.restart();
  }
  // settings_buff[0] = 350;  // Minimum frequency (Hz)
  // settings_buff[1] = 470;  // Maximum frequency (Hz)
  // settings_buff[2] = 5;    // Minimum duration (×100ms = 1500ms)
  // settings_buff[3] = 25;   // Maximum duration (×100ms = 3000ms)
  // settings_buff[4] = 12;   // Cooldown (×1000ms = 12000ms)

  // settings_buff[0] = 50;  //350;
  // settings_buff[3] = 3;
  // settings_buff[1] = 500;
  // settings_buff[2] = 43000;

  // for (int i = 0; i < 5; i++) {
  //   EEPROM.write((i * 2) + SETTING_ADDR, (settings_buff[i] >> 8));
  //   EEPROM.write((i * 2) + (SETTING_ADDR + 1), (settings_buff[i] & 0xFF));
  //   Serial.print((i * 2) + SETTING_ADDR);
  //   Serial.print("-");
  //   Serial.println(settings_buff[i]);
  // }
  // EEPROM.commit();

  firmware_verison = 1.35;
  write_firmware_version(firmware_verison);
  venus_version = firmware_version_venus[0] + (firmware_version_venus[1] / 10.0f);
  //  Serial.println("Venus version check");
  //  Serial.println(venus_version);

  firmware_verison = read_firmware_version();
  //  Serial.println("Login version check");
  //  Serial.println(firmware_verison);

  firmware_verison_saved = read_firmware_version();
  //  Serial.println(firmware_verison_saved);

  for (int i = 0; i < 5; i++) {
    uint8_t byteLow = EEPROM.read((i * 2) + SETTING_ADDR);
    uint8_t byteHigh = EEPROM.read((i * 2) + (SETTING_ADDR + 1));
    if ((byteLow == 0 || byteLow == 255) && (byteHigh == 0 || byteHigh == 255)) {
      EEPROM.write((i * 2) + SETTING_ADDR, 0);
      EEPROM.write((i * 2) + (SETTING_ADDR + 1), 0);
      EEPROM.commit();
    }
  }
}

//---------------------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(115200);
  //  Serial.println("//--------------------------BBK SIGNED IN--------------------------------------//");
  //  Serial.println("//--------------------------Begin Setup----------------------------------------//");
  Serial2.begin(115200, SERIAL_8N1, UART_RX, UART_TX);
  esp_task_wdt_init(10, true);  // 10 seconds timeout
  if (eerprom_init_flag != 1) {
    eeprom_setup_init();
  }
  GetDeviceId();
  init_all_touch_buttons();
  Display_Init_9341();
  machine_state = home_screen_init;
  everything_off();
  // whistleAlert();
  //  Serial.println("//----------------------------End Setup----------------------------------------//");
}
//---------------------------------------------------------------------------------------------------//
void loop() {
  receive_from_stm();
  processButtonStates();
  keypad_process();
  display_process();
  vTaskDelay(15 / portTICK_PERIOD_MS);
}
//---------------------------------------------------------------------------------------------------//
