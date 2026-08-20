//-------------------------------------------------BBK--------------------------------------------------------------//
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Beyond_Icons.h"
#include "FreeSansBold10pt7b.h"
//------------------------------------------------------------------------------------------------------------------//

#define ICON_WIDTH 32
#define ICON_HEIGHT 32
#define ICON_1_X 20
#define ICON_1_Y 20
#define ICON_2_X 20
#define ICON_2_Y 105
#define ICON_3_X 20
#define ICON_3_Y 180
#define ICON_4_X 270
#define ICON_4_Y 20
#define ICON_5_X 270
#define ICON_5_Y 105
#define ICON_6_X 270
#define ICON_6_Y 180
#define TOP_SYMBOL_1_X 145
#define TOP_SYMBOL_1_Y 10
#define BOTTOM_SYMBOL_1_X 140
#define BOTTOM_SYMBOL_1_Y 200
#define BLOWER_X 95
#define BLOWER_Y 60
#define BLOWER_ICON_WIDTH 134
#define BLOWER_ICON_HEIGHT 105
//------------------------------------------------------------------------------------------------------------------//
bool transmit_uart_flag_2, timer_running_flag_2;
volatile bool transmit_uart_flag = false, timer_running_flag = false;
//------------------------------------------------------------------------------------------------------------------//
int fontWidth = 20 * 1;
int fontHeight = 25 * 1;
int textLength = 2;
int set_auto_clean_time;
int whistle_ka_count;
int set_time, set_countdown;
int set_cooker_wistle_cnt = 3;
uint8_t light_brightness = 0, settings_index = 0;
uint8_t fan_speed = 0;
int start_auto_clean = 0;
int auto_clean_cnt = 0;
int auto_clean_countdown;
int set_countdown_auto_clean_VAL;
int timer_one_time = 0;
volatile uint32_t counter = 0;
uint16_t settings_buff[5] = { 50, 2, 43, 3, 2 };
volatile int set_countdown_auto_clean = 0;

//------------------------------------------------------------------------------------------------------------------//

float firmware_verison;
//------------------------------------------------------------------------------------------------------------------//

hw_timer_t *timer = NULL;
hw_timer_t *timer2 = NULL;
//------------------------------------------------------------------------------------------------------------------//

extern __machine_display_states machine_state;
extern uint16_t radio_stations[3];
extern int station_num;
extern uint8_t radio_volume;
extern String display_devicename;
extern String display_currentTrack;
extern String display_currentArtist;
extern String display_currentAlbum;
extern bool isConnected;
extern RDA5807M radio;
extern float firmware_verison_saved;
extern int ota_retries, update_progrss_bar;
extern int whistleCount;
extern bool wifi_flag;
extern String chipId;
extern char btName[32];
//----------------------------------------------Function Defnition--------------------------------------------------//
void IRAM_ATTR onTimer();
void IRAM_ATTR onTimer2();
void startCountdown(int seconds);
void startCountdownautoclean(int seconds);
void printRemainingTime();
void printRemainingTimeautoclean();
void stopTimer();
void stopTimer2();
void display_process();
//-------------------------------------------- Function Declaration ------------------------------------------------//
// ISR for Timer auto-shut off
void IRAM_ATTR onTimer() {
  if (set_countdown > 0) {
    timer_running_flag = true;
    set_countdown--;
    if (set_countdown == 0) {
      transmit_uart_flag = true;
    }
  }
}
//------------------------------------------------------------------------------------------------------------------//
// ISR for Timer auto clean
void IRAM_ATTR onTimer2() {
  if (set_countdown_auto_clean > 0) {
    timer_running_flag_2 = true;
    set_countdown_auto_clean--;
    if (set_countdown_auto_clean == 0) {
      transmit_uart_flag_2 = true;
    }
  }
}
//------------------------------------------------------------------------------------------------------------------//
// Function to start Timer auto shut off
void startCountdown(int seconds) {
  set_countdown = seconds;

  timer = timerBegin(0, 80, true);
  //timer = timerBegin(0, 80);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
  //  Serial.println("Timer initialized");
}
//------------------------------------------------------------------------------------------------------------------//
// Function to start Timer auto clean
void startCountdownautoclean(int seconds) {
  set_countdown_auto_clean = seconds;
  timer2 = timerBegin(1, 80, true);
  timerAttachInterrupt(timer2, &onTimer2, true);
  timerAlarmWrite(timer2, 1000000, true);
  timerAlarmEnable(timer2);
  //  Serial.println("Timer 2 initialized");
}
//------------------------------------------------------------------------------------------------------------------//
// Function to print the remaining time for Timer auto  shut off
void printRemainingTime() {
  static int prev_minutes = -1;
  static int prev_seconds = -1;

  int minutes = set_countdown / 60;
  int seconds = set_countdown % 60;


  if (minutes != prev_minutes || seconds != prev_seconds) {
    tft.fillRect(100, 90, 25 * 2, 35, ILI9341_BLACK);
    display_value_2_digit(100, 120, minutes, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
    display_text(155, 120, ":", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
    display_text(170, 120, "   ", ILI9341_BLACK, ILI9341_BLACK, 2);
    tft.fillRect(180, 90, 25 * 2, 35, ILI9341_BLACK);
    display_value_2_digit(180, 120, seconds, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
    prev_minutes = minutes;
    prev_seconds = seconds;
  }

  if (set_countdown == 0 && transmit_uart_flag) {
    transmit_uart_flag = false;
    timer_running_flag = false;
    uart_tx_buffer[0] = 0x13;
    uart_tx_buffer[1] = 0x03;
    uart_tx_buffer[2] = 0x00;
    uart_tx_buffer[63] = 0x12;
    transmit64BytesUART2(uart_tx_buffer);
    stopTimer();
    fan_speed = 0;
    machine_state = home_screen_init;
  }
}
//------------------------------------------------------------------------------------------------------------------//
// Function to print the remaining time for Timer auto clean
void printRemainingTimeautoclean() {
  static int prev_minutes = -1;
  static int prev_seconds = -1;

  int minutes = set_countdown_auto_clean / 60;
  int seconds = set_countdown_auto_clean % 60;

  if (minutes != prev_minutes || seconds != prev_seconds) {
    tft.fillRect(80, 90, 25 * 2, 35, ILI9341_BLACK);
    display_value_2_digit(80, 120, minutes, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
    display_text(140, 120, ":", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
    tft.fillRect(170, 90, 25 * 2, 35, ILI9341_BLACK);
    display_value_2_digit(170, 120, seconds, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
    prev_minutes = minutes;
    prev_seconds = seconds;
  }

  if (set_countdown_auto_clean <= 300 && timer_one_time == 0) {
    transmit_uart_flag_2 = false;
    timer_running_flag_2 = false;
    uart_tx_buffer[0] = 0x13;
    uart_tx_buffer[1] = 0x03;
    uart_tx_buffer[2] = 0x01;
    uart_tx_buffer[63] = 0x12;
    transmit64BytesUART2(uart_tx_buffer);
    timer_one_time = 1;
  } else if (set_countdown_auto_clean == 0) {
    uart_tx_buffer[0] = 0x13;
    uart_tx_buffer[1] = 0x03;
    uart_tx_buffer[2] = 0x00;
    uart_tx_buffer[63] = 0x12;
    transmit64BytesUART2(uart_tx_buffer);

    // Turn OFF HEATER
    uart_tx_buffer[0] = 0x13;
    uart_tx_buffer[1] = 0x04;
    uart_tx_buffer[2] = 0x00;
    uart_tx_buffer[63] = 0x12;
    transmit64BytesUART2(uart_tx_buffer);
    stopTimer2();
    timer_one_time = 0;
    fan_speed = 0;
    machine_state = home_screen_init;
  }
}
//------------------------------------------------------------------------------------------------------------------//
// Function to stop Timer auto shut off
void stopTimer() {
  timerAlarmDisable(timer);
  timerDetachInterrupt(timer);
  timerEnd(timer);
}
//------------------------------------------------------------------------------------------------------------------//
// Function to stop Timer auto clean
void stopTimer2() {
  timerAlarmDisable(timer2);
  timerDetachInterrupt(timer2);
  timerEnd(timer2);
}
//------------------------------------------------------------------------------------------------------------------//
//Display ILI9341 states
void display_process() {
  if (set_countdown == 0 && transmit_uart_flag) {
    transmit_uart_flag = false;
    timer_running_flag = false;
    uart_tx_buffer[0] = 0x13;
    uart_tx_buffer[1] = 0x03;
    uart_tx_buffer[2] = 0x00;
    uart_tx_buffer[63] = 0x12;
    transmit64BytesUART2(uart_tx_buffer);
    set_time = 0;
    set_countdown = 0;
    fan_speed = 0;
    stopTimer();
  }
  //------------------------------------------------------------------------------------------------------------------//
  switch (machine_state) {
    case home_screen_init:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(ICON_1_X, ICON_1_Y, (uint16_t *)ui_2_Clock_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Light_1, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_3_X, ICON_3_Y, (uint16_t *)ui_2_Cokker_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Settings_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Bluetooth_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_radio_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(BLOWER_X, BLOWER_Y, (uint16_t *)ui_2_Blower_1, BLOWER_ICON_WIDTH, BLOWER_ICON_HEIGHT);
      uart_tx_buffer[0] = 0x13;
      uart_tx_buffer[1] = 0x00;
      uart_tx_buffer[2] = 0x00;
      uart_tx_buffer[13] = 0x00;
      uart_tx_buffer[63] = 0x12;
      transmit64BytesUART2(uart_tx_buffer);
      machine_state = home_screen;
      break;

    case home_screen:
      if (light_brightness == 0) {
        tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Light_1, ICON_WIDTH, ICON_HEIGHT);
      } else if (light_brightness == 1) {
        tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Light_2, ICON_WIDTH, ICON_HEIGHT);
      } else if (light_brightness == 2) {
        tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Light_3, ICON_WIDTH, ICON_HEIGHT);
      } else if (light_brightness == 3) {
        tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Light_4, ICON_WIDTH, ICON_HEIGHT);
      }
      if (fan_speed == 1) {
        tft.drawRGBBitmap(BLOWER_X, BLOWER_Y, (uint16_t *)ui_2_Blower_2, BLOWER_ICON_WIDTH, BLOWER_ICON_HEIGHT);
      } else if (fan_speed == 2) {
        tft.drawRGBBitmap(BLOWER_X, BLOWER_Y, (uint16_t *)ui_2_Blower_3, BLOWER_ICON_WIDTH, BLOWER_ICON_HEIGHT);
      } else if (fan_speed == 3) {
        tft.drawRGBBitmap(BLOWER_X, BLOWER_Y, (uint16_t *)ui_2_Blower_4, BLOWER_ICON_WIDTH, BLOWER_ICON_HEIGHT);
      } else {
        tft.drawRGBBitmap(BLOWER_X, BLOWER_Y, (uint16_t *)ui_2_Blower_1, BLOWER_ICON_WIDTH, BLOWER_ICON_HEIGHT);
      }
      break;

    case bluetooth_screen_init_switch:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Bluetooth_Disconnected, ICON_WIDTH, ICON_HEIGHT);
      display_text_center(100, "Please wait... ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      if (isConnected) {
        wifi_off();
        delay(5);
        machine_state = bluetooth_screen_init;
      } else {
        wifi_off();
        delay(5);
        Ble_sound_init();
        machine_state = bluetooth_screen_init;
      }
      break;

    case bluetooth_screen_init:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Bluetooth_Disconnected, ICON_WIDTH, ICON_HEIGHT);
      display_text_center(100, "Connect your phone ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(125, "To", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(155, (const char *)btName, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = bluetooth_connection_init;
      break;

    case bluetooth_connection_init:
      if (isConnected) {
        machine_state = bluetooth_screen;
      }
      break;

    case bluetooth_screen:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Bluetooth_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_1_X, ICON_1_Y, (uint16_t *)ui_2_Bluetooth_Disconnected, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_3_X, ICON_3_Y, (uint16_t *)ui_2_Pause, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_Start, ICON_WIDTH, ICON_HEIGHT);
      machine_state = bluetooth_playback;
      break;

    //    case bluetooth_playback:
    //      {
    //        static unsigned long lastUpdateTime = millis() - 500;
    //        static bool prevConnected = false;
    //
    //        if (isConnected)
    //        {
    //          if (!prevConnected || millis() - lastUpdateTime >= 2000)
    //          {
    //            lastUpdateTime = millis();
    //            prevConnected = true;
    //            displayStatus();
    //            tft.fillRect(10, 60, 25 * 10, 35, ILI9341_BLACK);
    //            tft.fillRect(0, 90, 25 * 13, 35, ILI9341_BLACK);
    //            tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Bluetooth_Icon, ICON_WIDTH, ICON_HEIGHT);
    //            tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
    //            tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
    //            display_text_center(80, "Connected To", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
    //            display_text_center(110, display_devicename.c_str(), ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
    //          }
    //        }
    //        else
    //        {
    //          if (prevConnected)
    //          {
    //            tft.fillRect(10, 60, 25 * 10, 35, ILI9341_BLACK);
    //            tft.fillRect(0, 90, 25 * 13, 35, ILI9341_BLACK);
    //            tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
    //            tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
    //            display_text_center(80, " READY TO PAIR ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
    //            display_text_center(110, "Pairing", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
    //          }
    //          prevConnected = false;
    //        }
    //      }
    //      break;
    case bluetooth_playback:
      {

        static unsigned long lastUpdateTime = 0;
        static bool prevConnected = false;
        if (isConnected) {
          displayStatus();
          if (!prevConnected || millis() - lastUpdateTime >= 1500) {
            lastUpdateTime = millis();
            prevConnected = true;
            //            displayStatus();
            //            tft.fillRect(0, 60, tft.width(), 35, ILI9341_WHITE);
            //            tft.fillRect(0, 90, tft.width(), 35, ILI9341_WHITE);
            tft.fillRect(55, 60, 19 * 11, 35, ILI9341_BLACK);
            tft.fillRect(55, 90, 19 * 11, 35, ILI9341_BLACK);
            tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Bluetooth_Icon, ICON_WIDTH, ICON_HEIGHT);
            //            tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
            //            tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
            display_text_center(80, "Connected To", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
            display_text_center(110, display_devicename.c_str(), ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
            display_devicename = "";
            display_devicename.clear();  // does the same thing
            Serial.println(display_devicename);
          }
        } else {
          if (prevConnected) {
            //            tft.fillRect(0, 60, tft.width(), 35, ILI9341_BLACK);
            //            tft.fillRect(0, 90, tft.width(), 35, ILI9341_BLACK);
            tft.fillRect(55, 60, 19 * 11, 35, ILI9341_BLACK);
            tft.fillRect(55, 90, 19 * 11, 35, ILI9341_BLACK);
            //            tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
            //            tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
            display_text_center(80, "Ready to Connect", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
            display_text_center(110, (const char *)btName, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);

            display_devicename = "";
            display_devicename.clear();  // does the same thing
          }
          prevConnected = false;
        }
      }
      break;

    case fm_checking_page:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(105, "Switching To Radio", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      Ble_sound_deinit();
      machine_state = fm_begin;
      break;

    case fm_no_station_tuned:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(120, "No stations tuned!", ILI9341_RED, ILI9341_BLACK, 1);
      delay(2000);
      machine_state = fm_screen_init;
      break;

    case fm_begin:
      fm_radio_init();
      machine_state = fm_screen_init;
      break;

    case fm_screen_init:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(ICON_1_X, ICON_1_Y, (uint16_t *)ui_2_Auto_tune, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_3_X, ICON_3_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Reverse, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_radio_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Next, ICON_WIDTH, ICON_HEIGHT);
      Set_Selected_FM_Station(station_num);
      machine_state = fm_screen;
      break;

    case fm_screen:
      {
        float __channelfreq = getCurrentFrequency();
        static unsigned long __displayrefresh_timeout = 0;
        if (millis() - __displayrefresh_timeout >= 300) {
          __displayrefresh_timeout = millis();
          //          display_text(80, 100, "Station: ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
          tft.fillRect(110, 105, 20 * 7, 35, ILI9341_BLACK);
          display_frequency_2(115, 130, __channelfreq, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
          //          display_text(80, 150, "Volume: ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
          tft.fillRect(145, 180, 20 * 2, 30, ILI9341_BLACK);
          display_value_2_digit_2(150, 200, radio_volume, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, &FreeSansBold12pt7b);

          //          display_text(80, 120, "Station", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
          //          tft.fillRect(170, 100, 20 * 5, 25, ILI9341_BLACK);
          //          display_frequency(170, 120, getCurrentFrequency(), ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
          //          display_text(100, 200, "Volume", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
          //          tft.fillRect(190, 180, 20 * 2, 25, ILI9341_BLACK);
          //          display_value_2_digit(198, 200, radio_volume, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        }
      }
      break;

    case fm_screen_autoscan_init:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_radio_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text_center(100, "Please wait While we tune FM", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = fm_screen_autoscan;
      break;

    case fm_screen_autoscan:
      scanStations();
      break;

    case auto_turn_off_delay_init:
      set_time = 0;
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Clock_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text(95, 80, "Auto Shut-off", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_Ok, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_1_X, ICON_1_Y, (uint16_t *)ui_2_Auto_clean, ICON_WIDTH, ICON_HEIGHT);
      display_text(135, 150, "Mins", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = auto_turn_off_delay;
      break;

    case auto_turn_off_delay:
      {
        static unsigned long __displayrefresh_timeout = 0;
        if (millis() - __displayrefresh_timeout >= 300) {
          __displayrefresh_timeout = millis();
          tft.fillRect(135, 100, 40, 25, ILI9341_BLACK);
          display_value_2_digit(150, 120, set_time, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        }
      }
      break;


    case auto_turn_off_delay_countdown_init:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Clock_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_1_X, ICON_1_Y, (uint16_t *)ui_2_Stop, ICON_WIDTH, ICON_HEIGHT);
      display_text(105, 80, "Auto Shut-off", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(110, 150, "Min", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(180, 150, "Sec", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = auto_turn_off_delay_countdown;
      break;
    case auto_turn_off_delay_countdown:
      printRemainingTime();
      if (set_countdown == 0) {
        fan_speed = 0;
        machine_state = home_screen_init;
      }
      break;

    case Whistle_Counter:
      fan_speed = 0;
      set_cooker_wistle_cnt = 0;
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Cokker_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_Ok, ICON_WIDTH, ICON_HEIGHT);
      display_text(85, 80, "Whistle Count", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(80, 150, "Count : ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value(180, 150, set_cooker_wistle_cnt, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = Whistle_Counter_SET_VAL;
      break;

    case Whistle_Counter_SET_VAL:
      {
        static unsigned long __displayrefresh_timeout = 0;
        if (millis() - __displayrefresh_timeout >= 250) {
          __displayrefresh_timeout = millis();
          tft.fillRect(180, 130, 20 * 2, 25, ILI9341_BLACK);
          display_value(180, 150, set_cooker_wistle_cnt, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        }
      }
      break;

    case Whistle_Counter_loop:
      {
        tft.fillScreen(ILI9341_BLACK);
        // Turn OFF FAN
        //      uart_tx_buffer[0] = 0x13;
        //      uart_tx_buffer[1] = 0x03;
        //      uart_tx_buffer[2] = 0x00;
        //      uart_tx_buffer[63] = 0x12;
        //      transmit64BytesUART2(uart_tx_buffer);
        tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Cokker_Icon, ICON_WIDTH, ICON_HEIGHT);
        tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
        tft.drawRGBBitmap(ICON_3_X, ICON_3_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
        tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
        display_text(90, 80, "Whistle Count", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_text(80, 140, "Count : ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_value(180, 140, whistle_ka_count, ILI9341_BLACK, ILI9341_BLACK, 1);
        display_value(180, 140, whistle_ka_count, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_text(210, 140, "/", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
        display_value(230, 140, set_cooker_wistle_cnt, ILI9341_BLACK, ILI9341_BLACK, 1);  //clear overlap
        display_value(230, 140, set_cooker_wistle_cnt, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_text(65, 195, "Senstivity", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        // display_value(200, 190, settings_buff[3], ILI9341_BLACK, ILI9341_BLACK, 1);  //clear overlap
        // display_value(200, 190, settings_buff[3], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        // Draw 5 small rectangles as a bar indicator (aligned below or beside text)
        int barStartX = 170;  // starting X position for the bar
        int barStartY = 185;  // aligned near text
        int barWidth = 10;    // width of each rectangle
        int barHeight = 10;   // height of each rectangle
        int barGap = 5;       // spacing between rectangles

        for (int i = 0; i < 5; i++) {
          uint16_t color = (i < settings_buff[3]) ? ILI9341_GREEN : ILI9341_DARKGREY;
          tft.fillRect(barStartX + (i * (barWidth + barGap)), barStartY, barWidth, barHeight, color);
        }
        machine_state = Whistle_Counter_counting;
        Whistle_Setup();
        initI2S();
        break;
      }
      // case Whistle_Counter_loop:
      //   tft.fillScreen(ILI9341_BLACK);
      //   // Turn OFF FAN
      //   uart_tx_buffer[0] = 0x13;
      //   uart_tx_buffer[1] = 0x03;
      //   uart_tx_buffer[2] = 0x00;
      //   uart_tx_buffer[13] = 0x05;
      //   uart_tx_buffer[63] = 0x12;
      //   transmit64BytesUART2(uart_tx_buffer);
      //   tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Cokker_Icon, ICON_WIDTH, ICON_HEIGHT);
      //   tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      //   display_text(85, 80, "Whistle Count", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_text(80, 150, "Count : ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_value(180, 150, whistle_ka_count, ILI9341_BLACK, ILI9341_BLACK, 1);
      //   display_value(180, 150, whistle_ka_count, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_text(210, 150, "/", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 2);
      //   display_value(230, 150, set_cooker_wistle_cnt, ILI9341_BLACK, ILI9341_BLACK, 1);  //clear overlap
      //   display_value(230, 150, set_cooker_wistle_cnt, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   machine_state = Whistle_Counter_counting;
      //   Whistle_Setup();
      //   initI2S();
      //   initI2S_alert();
      //   break;

    case Whistle_Counter_counting:
      Process_Whistle_Count();
      break;

    case Whistle_Counter_completed:
      whistle_ka_count = 0;
      set_cooker_wistle_cnt = 0;
      whistleCount = 0;
      machine_state = home_screen_init;
      deinitI2S();
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Clock_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text_center(105, "Whistle Counter", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(135, "Exiting", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      delay(2000);
      uart_tx_buffer[0] = 0x13;
      uart_tx_buffer[1] = 0x00;
      uart_tx_buffer[2] = 0x00;
      uart_tx_buffer[13] = 0x00;
      uart_tx_buffer[63] = 0x12;
      transmit64BytesUART2(uart_tx_buffer);
      machine_state = home_screen_init;
      break;

    case settings_screen_init:
      tft.fillScreen(ILI9341_BLACK);
      display_text(115, 80, "Settings", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Settings_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_1_X, ICON_1_Y, (uint16_t *)ui_2_Customer, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_3_X, ICON_3_Y, (uint16_t *)ui_2_Wifi, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2Info, ICON_WIDTH, ICON_HEIGHT);
      machine_state = settings_poll;
      break;

    case settings_poll:
      break;

    case auto_clean_init:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Clock_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text(95, 80, "Auto Clean", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Stop, ICON_WIDTH, ICON_HEIGHT);
      display_text(120, 150, "Min", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(170, 150, "Sec", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      uart_tx_buffer[0] = 0x13;
      uart_tx_buffer[1] = 0x03;
      uart_tx_buffer[2] = 0x00;
      uart_tx_buffer[13] = 0x05;
      uart_tx_buffer[63] = 0x12;
      transmit64BytesUART2(uart_tx_buffer);
      machine_state = Set_clean_time_countdown;
      break;

    case Set_clean_time_val:
      tft.fillRect(120, 100, 20 * 2, 25, ILI9341_BLACK);
      display_value(130, 120, set_auto_clean_time, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      break;

    case Set_clean_time_countdown:
      printRemainingTimeautoclean();
      if (set_countdown_auto_clean_VAL == 0) {
        set_countdown_auto_clean_VAL = 0;
        machine_state = home_screen_init;
      }
      break;

    case settings_screen_wifi_OTA_fail:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(120, "OTA FAILED....", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      delay(1000);
      ESP.restart();
      break;

    case settings_screen_wifi_disconnect:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(120, "Wi-Fi Disconnected", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      reset_wifi();
      delay(1500);
      machine_state = settings_screen_init;
      break;

    case settings_screen_wifi_wait:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(120, "Please Wait", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      Ble_sound_deinit();
      delay(100);
      WifiSetup();
      delay(50);
      GetDeviceId();
      delay(10);
      machine_state = settings_screen_wifi;
      break;

    case settings_screen_wifi:
      tft.fillScreen(ILI9341_BLACK);
      //      display_text_center(80, "Connect To 'Beyond-WiFi' For OTA", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(75, "Connect To 'Beyond-WiFi'", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Wifi, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      //      display_text_center(100, "Open Web Browser", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //      display_text_center(120, "And Enter", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //      display_text_center(150, "192.168.1.4", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      tft.drawRGBBitmap(110, 90, (uint16_t *)ui_2_URL_QR_Code, 100, 100);
      display_text_center(210, "Scan Me", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);

      machine_state = settings_screen_wifi_poll;
      break;

    case settings_screen_wifi_poll:
      WifiLoop();
      if (wifi_flag) {
        machine_state = settings_screen_OTA;
      }
      break;

    case settings_screen_OTA:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(80, "OTA", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Wifi, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_1_X, ICON_1_Y, (uint16_t *)ui_2_Wifi_Disconnect, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_3_X, ICON_3_Y, (uint16_t *)ui_2_left, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_right, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text_center(130, "Want to Upgrade?", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(220, 200, "Yes", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(70, 200, "No", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = settings_screen_wifi_OTA_poll;
      break;

    case settings_screen_wifi_OTA_poll:
      if (!wifi_flag) {
        tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Wifi_Disconnect, ICON_WIDTH, ICON_HEIGHT);
      }
      break;

    case settings_screen_wifi_OTA:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(80, "OTA", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      if (wifi_flag) {
        tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Wifi, ICON_WIDTH, ICON_HEIGHT);
      }
      display_text_center(150, "Updating... Stay tuned!", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      OTALoop();
      break;

    case settings_screen_wifi_OTA_no_update:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(80, "OTA", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(150, "No update", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      delay(1000);
      machine_state = home_screen_init;
      break;

    case settings_screen_wifi_OTA_error:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(80, "OTA", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(150, "Error", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      delay(1500);
      machine_state = home_screen_init;
      break;

    case settings_screen_info:
      tft.fillScreen(ILI9341_BLACK);
      display_text_center(80, "Info", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2Info, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text(100, 110, "Venus:", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value_float(200, 110, venus_version, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1, 2);
      display_text(100, 140, "Neptune:", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value_float(200, 140, firmware_verison_saved, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1, 2);
      display_text_center(180, "Serial No:", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //      display_text_center(210, chipId.c_str(), ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center_MAC(210, chipId.c_str(), ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = idle;
      break;

    case customercare:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Customer, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text_center(90, "Customer Care", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(110, "08043628277", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(160, "WhatsApp", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text_center(180, "+91-8105208433", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      machine_state = customercare_idle;
      break;

    case customercare_idle:
      break;

      // case Whistle_Counter_setting_init:
      //   tft.fillScreen(ILI9341_BLACK);
      //   tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Clock_Icon, ICON_WIDTH, ICON_HEIGHT);
      //   display_text(45, 80, "Wistle Count Settings", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
      //   tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      //   tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
      //   tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_Ok, ICON_WIDTH, ICON_HEIGHT);
      //   display_text(90, 115, "Min Freq- ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_value_2_digit(195, 115, settings_buff[0], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_text(90, 145, "Max Freq-", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_value_2_digit(195, 145, settings_buff[1], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_text(90, 175, "Min Time- ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_value_2_digit(195, 175, settings_buff[2], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_text(90, 205, "Max Time- ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_value_2_digit(195, 205, settings_buff[3], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_text(90, 235, "Gap Time- ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   display_value_2_digit(195, 235, settings_buff[4], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      //   settings_index = 0;
      //   machine_state = Whistle_Counter_setting_poll;
      //   break;
    case Whistle_Counter_setting_init:
      tft.fillScreen(ILI9341_BLACK);
      tft.drawRGBBitmap(TOP_SYMBOL_1_X, TOP_SYMBOL_1_Y, (uint16_t *)ui_2_Clock_Icon, ICON_WIDTH, ICON_HEIGHT);
      display_text(45, 60, "Wistle Count Settings", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      tft.drawRGBBitmap(ICON_2_X, ICON_2_Y, (uint16_t *)ui_2_Minus_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_4_X, ICON_4_Y, (uint16_t *)ui_2_Back_Icon, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_5_X, ICON_5_Y, (uint16_t *)ui_2_Plus, ICON_WIDTH, ICON_HEIGHT);
      tft.drawRGBBitmap(ICON_6_X, ICON_6_Y, (uint16_t *)ui_2_Ok, ICON_WIDTH, ICON_HEIGHT);
      display_text(90, 90, "ADC -", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(90, 115, "Thresh - ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value_2_digit(195, 115, settings_buff[0], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(90, 145, "Min Time-", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value_2_digit(195, 145, settings_buff[1], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(90, 175, "Max Time- ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value_2_digit(195, 175, settings_buff[2], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(90, 205, "Senstivity", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value_2_digit(195, 205, settings_buff[3], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_text(90, 235, "Cooldown- ", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      display_value_2_digit(195, 235, settings_buff[4], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
      settings_index = 0;
      Whistle_Setup();  //comment later
      machine_state = Whistle_Counter_setting_poll;
      break;

    case Whistle_Counter_setting_poll:
      {

        static int prev_adc_display = -1;

        int adc_display = Get_Filtered_ADC_Int();

        if (adc_display != prev_adc_display) {
          tft.fillRect(175, 75, 80, 30, ILI9341_BLACK);
          display_value_2_digit(195, 90, adc_display, ILI9341_RED, ILI9341_BLACK, 1);
          prev_adc_display = adc_display;
        }
        display_value_2_digit(195, 115, settings_buff[0], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_value_2_digit(195, 145, settings_buff[1], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_value_2_digit(195, 175, settings_buff[2], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_value_2_digit(195, 205, settings_buff[3], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        display_value_2_digit(195, 235, settings_buff[4], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
        tft.fillRect(195, 95 + (30 * settings_index), 20 * 2, 25, ILI9341_BLACK);
        display_value_2_digit(195, 115 + (30 * settings_index), settings_buff[settings_index], ILI9341_GREEN, ILI9341_BLACK, 1);
      }
      break;

    default:
      break;
  }
}

//------------------------------------------------------------------------------------------------------------------//
