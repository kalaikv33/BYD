//-------------------------------BBK--------------------------------------//
#include <radio.h>
#include <RDA5807M.h>
#include <EEPROM.h>
//-----------------------------------------------------------------------//
extern bool wifi_flag;
extern int set_cooker_wistle_cnt;
extern RDA5807M radio;
extern int station_num;
extern uint8_t radio_volume, settings_index;
extern uint8_t light_brightness;
extern bool light_b_change;
extern uint8_t fan_speed;
extern uint8_t uart_tx_buffer[64];
extern int set_time;
extern int set_time, set_countdown;
extern volatile bool transmit_uart_flag, timer_running_flag;
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
extern uint16_t settings_buff[5];
//---------------------------------------------------Function Declaration---------------------------------------------//
void send_fan_speed_to_chimney(uint8_t fan_speed);
void send_light_to_chimney(uint8_t light);
void volumeUp();
void volumeDown();
void nextStation();
void previousStation();
void keypad_process();
//---------------------------------------------------Function Defnition-----------------------------------------------//



//-----------------------------------------------------------------------//
void send_fan_speed_to_chimney(uint8_t fan_speed) {
  //  Serial.println(fan_speed);
  uart_tx_buffer[0] = 0x13;
  uart_tx_buffer[1] = 0x03;
  uart_tx_buffer[2] = fan_speed;
  uart_tx_buffer[63] = 0x12;
  transmit64BytesUART2(uart_tx_buffer);
  //  Serial.print("UART Buffer: ");
  for (int i = 0; i < 64; i++) {
    //    Serial.print(uart_tx_buffer[i], HEX);
    //    Serial.print(" ");
  }
  //  Serial.println();
  //  Serial.println("Fan Packet Sent");
  //  delay(10);
}
//--------------------------------------------------------------------------------------------------------------------//
void send_light_to_chimney(uint8_t light) {
  uart_tx_buffer[0] = 0x13;
  uart_tx_buffer[1] = 0x02;
  uart_tx_buffer[2] = light;
  uart_tx_buffer[63] = 0x12;
  transmit64BytesUART2(uart_tx_buffer);
  //  Serial.print("UART Buffer: ");
  for (int i = 0; i < 64; i++) {
    //    Serial.print(uart_tx_buffer[i], HEX);
    //    Serial.print(" ");
  }
  //  Serial.println();
  //  Serial.println("Light packet sent");
  //  delay(50);
}
//--------------------------------------------------------------------------------------------------------------------//
void volumeUp() {
  if (radio_volume == MIN_VOLUME) {
    radio.setMute(false);
    //    Serial.println("Unmuted.");
  }
  if (radio_volume < MAX_VOLUME) {
    radio_volume++;
    radio.setVolume(radio_volume);
    //    Serial.print("Volume increased to: ");
    //    Serial.println(radio_volume);
  } else {
    //    Serial.println("Volume is already at maximum.");
  }
}
//--------------------------------------------------------------------------------------------------------------------//
void volumeDown() {
  if (radio_volume > MIN_VOLUME) {
    radio_volume--;
    radio.setVolume(radio_volume);
    //    Serial.print("Volume decreased to: ");
    //    Serial.println(radio_volume);
  }
  if (radio_volume == MIN_VOLUME) {
    radio.setMute(true);
    //    Serial.println("Muted: Absolute silence.");
  }
}
//--------------------------------------------------------------------------------------------------------------------//
void nextStation() {
  bool hasTunedStations = false;
  for (int i = 0; i < tunedCount; i++) {
    if (tunedStations[i] > 0) {
      hasTunedStations = true;
      break;
    }
  }
  if (!hasTunedStations) {
    //    Serial.println("No stations tuned");
    machine_state = fm_no_station_tuned;
    return;
  }
  do {
    station_num++;
    if (station_num >= tunedCount)
      station_num = 0;
  } while (tunedStations[station_num] <= 0);
  //  Serial.println("Next Station");
  Set_Selected_FM_Station(station_num);
  printCurrentFrequency();
}
//--------------------------------------------------------------------------------------------------------------------//
void previousStation() {
  bool hasTunedStations = false;
  for (int i = 0; i < tunedCount; i++) {
    if (tunedStations[i] > 0) {
      hasTunedStations = true;
      break;
    }
  }
  if (!hasTunedStations) {
    //    Serial.println("No stations tuned");
    machine_state = fm_no_station_tuned;
    return;
  }
  do {
    station_num--;
    if (station_num < 0)
      station_num = tunedCount - 1;
  } while (tunedStations[station_num] <= 0);
  //  Serial.println("Previous Station");
  Set_Selected_FM_Station(station_num);
  printCurrentFrequency();
}
//--------------------------------------------------------------------------------------------------------------------//
void keypad_process() {
  //---------------------------------------------------------------------//
  if (L1_PRESSED) {
    //    Serial.println("L1 Pressed");
    L1_PRESSED = 0;
    switch (machine_state) {
      case home_screen:
        if (timer_running_flag == true) {
          machine_state = auto_turn_off_delay_countdown_init;
        } else {
          machine_state = auto_turn_off_delay_init;
        }
        break;
      case bluetooth_screen:
        break;
      case fm_screen:
        machine_state = fm_screen_autoscan_init;
        break;

      case bluetooth_playback:
        disconnectAndPrepareForPairing();
        break;

      case Whistle_Counter_setting_poll:
        if (settings_index > 0)
          settings_index--;
        else
          settings_index = 4;  // round robin
        break;
      case auto_turn_off_delay:

        uart_tx_buffer[0] = 0x13;
        uart_tx_buffer[1] = 0x04;
        uart_tx_buffer[2] = 0x01;
        uart_tx_buffer[63] = 0x12;
        transmit64BytesUART2(uart_tx_buffer);
        delay(100);
        set_countdown_auto_clean_VAL = (15 * 60);
        startCountdownautoclean(set_countdown_auto_clean_VAL);
        machine_state = auto_clean_init;
        break;
      case settings_poll:
        machine_state = customercare;
        break;
      case settings_screen_wifi_OTA_poll:
        machine_state = settings_screen_wifi_disconnect;
        break;
      case auto_turn_off_delay_countdown:
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
        break;
      default:
        break;
    }
  }
  //   //---------------------------------------------------------------------//
  //   if (L2_PRESSED) {
  //     //    Serial.println("L2 Pressed");
  //     L2_PRESSED = 0;
  //     switch (machine_state) {
  //       case home_screen:
  //         light_brightness++;
  //         if (light_brightness > 3)
  //           light_brightness = 1;
  //         send_light_to_chimney(light_brightness);
  //         break;
  //       case bluetooth_playback:
  //         volumeDown_BLE();
  //         break;
  //       case fm_screen:
  //         previousStation();
  //         //        volumeDown();
  //         break;
  //       case auto_turn_off_delay:
  //         set_time--;
  //         if (set_time < 0)
  //           set_time = 60;
  //         break;
  //       case Whistle_Counter_SET_VAL:
  //         set_cooker_wistle_cnt--;
  //         if (set_cooker_wistle_cnt < 0)
  //           set_cooker_wistle_cnt = 60;
  //         //        Serial.println(set_cooker_wistle_cnt);
  //         break;
  //       case Set_clean_time_val:
  //         set_auto_clean_time--;
  //         if (set_auto_clean_time < 0)
  //           set_auto_clean_time = 60;
  //         //        Serial.println(set_auto_clean_time);
  //         break;
  //       case settings_poll:
  //         //        machine_state = auto_clean_init;
  //         break;
  //       case Whistle_Counter_setting_poll:
  //         // if (settings_index == 0 || settings_index == 1) {
  //         //   if (settings_buff[settings_index] > 10)
  //         //     settings_buff[settings_index] -= 10;
  //         // } else {
  //         //   if (settings_buff[settings_index] > 1)
  //         //     settings_buff[settings_index]--;
  //         // }

  //         switch (settings_index) {
  //           case 0:  // Threshold
  //             if (settings_buff[0] >= 10)
  //               settings_buff[0] -= 10;
  //             break;

  //           case 1:  // Min Time
  //             if (settings_buff[1] > 1)
  //               settings_buff[1]--;
  //             break;

  //           case 2:  // Max Time
  //             if (settings_buff[2] > 1)
  //               settings_buff[2]--;
  //             break;

  //           case 3:  // Sensitivity
  //             if (settings_buff[3] > 1)
  //               settings_buff[3]--;
  //             break;

  //           case 4:  // Cooldown
  //             if (settings_buff[4] > 1)
  //               settings_buff[4]--;
  //             break;
  //         }
  //     }
  //   }

  //   break;

  //   // default:
  //   //   break;
  // }
  // }

  //---------------------------------------------------------------------//
  if (L2_PRESSED) {
    L2_PRESSED = 0;

    switch (machine_state) {

      case home_screen:
        light_brightness++;
        if (light_brightness > 3)
          light_brightness = 1;
        send_light_to_chimney(light_brightness);
        break;

      case bluetooth_playback:
        volumeDown_BLE();
        break;

      case fm_screen:
        previousStation();
        break;

      case auto_turn_off_delay:
        set_time--;
        if (set_time < 0)
          set_time = 60;
        break;

      case Whistle_Counter_SET_VAL:
        set_cooker_wistle_cnt--;
        if (set_cooker_wistle_cnt < 0)
          set_cooker_wistle_cnt = 60;
        break;

      case Set_clean_time_val:
        set_auto_clean_time--;
        if (set_auto_clean_time < 0)
          set_auto_clean_time = 60;
        break;

      case settings_poll:
        break;

      case Whistle_Counter_setting_poll:

        switch (settings_index) {

          case 0:  // Threshold (10 to 1000)
            if (settings_buff[0] > 10)
              settings_buff[0] -= 10;
            else
              settings_buff[0] = 1000;  // change range
            break;

          case 1:  // Min Time (1–60 sec)
            if (settings_buff[1] > 1)
              settings_buff[1]--;
            else
              settings_buff[1] = 60;  //
            break;

          case 2:  // Max Time (1–60 sec)
            if (settings_buff[2] > 1)
              settings_buff[2]--;
            else
              settings_buff[2] = 60;  //
            break;

          case 3:  // Sensitivity (1–5)
            if (settings_buff[3] > 1)
              settings_buff[3]--;
            else
              settings_buff[3] = 5;  //
            break;

          case 4:  // Cooldown (1–60 sec)
            if (settings_buff[4] > 1)
              settings_buff[4]--;
            else
              settings_buff[4] = 60;  //
            break;
        }

        break;
    }
  }

  //---------------------------------------------------------------------//
  if (L3_PRESSED) {
    //    Serial.println("L3 Pressed");
    L3_PRESSED = 0;
    switch (machine_state) {
      case home_screen:
        // Turn OFF FAN
        // uart_tx_buffer[0] = 0x13;
        // uart_tx_buffer[1] = 0x03;
        // uart_tx_buffer[2] = 0x00;
        // uart_tx_buffer[13] = 0x05;
        // uart_tx_buffer[63] = 0x12;
        // transmit64BytesUART2(uart_tx_buffer);
        if (!isConnected) {
          machine_state = Whistle_Counter;
        } else {
          Ble_sound_deinit();
          delay(1000);
          machine_state = Whistle_Counter;
        }
        break;
      case bluetooth_playback:
        a2dp_pause();
        break;
      case fm_screen:
        volumeDown();

        //        previousStation();
        break;
      case settings_poll:
        machine_state = settings_screen_wifi_wait;
        break;
      case Whistle_Counter_setting_poll:
        if (settings_index < 4)
          settings_index++;
        else
          settings_index = 0;
        break;
      case settings_screen_wifi_OTA_poll:
        machine_state = home_screen_init;
        break;
      case Whistle_Counter_counting:
        settings_buff[3]--;
        if (settings_buff[3] < 1) {
          settings_buff[3] = 5;
        }
        for (int i = 0; i < 5; i++) {
          EEPROM.write((i * 2) + SETTING_ADDR, (settings_buff[i] >> 8));
          EEPROM.write((i * 2) + (SETTING_ADDR + 1), (settings_buff[i] & 0xFF));
        }
        EEPROM.commit();
        break;
      default:
        break;
    }
  }
  //---------------------------------------------------------------------//
  if (R1_PRESSED) {
    //    Serial.println("R1 Pressed");
    R1_PRESSED = 0;
    switch (machine_state) {
      case home_screen:
        machine_state = settings_screen_init;
        break;
      case fm_screen_init:
        machine_state = home_screen_init;
        break;
      case bluetooth_connection_init:
        machine_state = home_screen_init;
        break;
      case bluetooth_playback:
        machine_state = home_screen_init;
        break;
      //      case fm_checking_page:
      //        turn_off_radio();
      //        machine_state = home_screen_init;
      //        break;
      case fm_screen:
        turn_off_radio();
        machine_state = home_screen_init;
        break;
      case auto_turn_off_delay:
        set_time = 0;
        machine_state = home_screen_init;
        break;
      case auto_turn_off_delay_countdown:
        machine_state = home_screen_init;
        break;
      case Whistle_Counter_SET_VAL:
        //        uart_tx_buffer[0] = 0x13;
        //        uart_tx_buffer[1] = 0x03;
        //        uart_tx_buffer[2] = 0x00;
        //        uart_tx_buffer[13] = 0x00;
        //        uart_tx_buffer[63] = 0x12;
        //        transmit64BytesUART2(uart_tx_buffer);
        machine_state = home_screen_init;
        break;
      case Whistle_Counter_counting:
        fan_speed = 0;
        machine_state = Whistle_Counter_completed;
        break;
      case idle:
        machine_state = settings_screen_init;
        break;
      case fm_screen_autoscan:
        machine_state = home_screen_init;
        break;
      case Whistle_Counter_setting_poll:
        machine_state = Whistle_Counter;
        break;
      case settings_screen_wifi_poll:
        wifi_off();
        machine_state = settings_screen_init;
        break;
      case settings_poll:
        machine_state = home_screen_init;
        break;
      case settings_screen_wifi_OTA_poll:
        wifi_off();
        machine_state = settings_screen_init;
        break;
      case customercare_idle:
        machine_state = settings_screen_init;
        break;
      case Set_clean_time_countdown:
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
        break;

      default:
        break;
    }
  }
  //---------------------------------------------------------------------//
  if (R2_PRESSED) {
    //    Serial.println("R2 Pressed");
    R2_PRESSED = 0;
    switch (machine_state) {
      case home_screen:
        //        if (isConnected)
        //        {
        //          wifi_off();
        //          machine_state = bluetooth_screen_init;
        //        }
        //        else
        //        {
        //          wifi_off();
        //          delay(10);
        //          Ble_sound_init();
        machine_state = bluetooth_screen_init_switch;
        //        }
        break;
      case bluetooth_playback:
        volumeUp_BLE();
        break;
      case fm_screen:
        nextStation();
        //        volumeUp();
        break;
      case auto_turn_off_delay:
        set_time++;
        if (set_time > 60)
          set_time = 0;
        //        Serial.println(set_time);
        break;
      case Whistle_Counter_SET_VAL:
        set_cooker_wistle_cnt++;
        if (set_cooker_wistle_cnt > 60)
          set_cooker_wistle_cnt = 0;
        //        Serial.println(set_cooker_wistle_cnt);
        break;
      case Set_clean_time_val:
        set_auto_clean_time++;
        if (set_auto_clean_time > 60)
          set_auto_clean_time = 0;
        //        Serial.println(set_auto_clean_time);
        break;
      case Whistle_Counter_setting_poll:
        // if (settings_index == 0 || settings_index == 1) {
        //   if (settings_buff[settings_index] < 990)
        //     settings_buff[settings_index] += 10;
        // } else {
        //   if (settings_buff[settings_index] < 600)
        //     settings_buff[settings_index]++;
        // }
        switch (settings_index) {

          case 0:  // Threshold (10–1000)
            if (settings_buff[0] < 1000)
              settings_buff[0] += 10;
            else
              settings_buff[0] = 10;  //
            break;

          case 1:  // Min Time (1–60 sec)
            if (settings_buff[1] < 60)
              settings_buff[1]++;
            else
              settings_buff[1] = 1;  //
            break;

          case 2:  // Max Time (1–60 sec)
            if (settings_buff[2] < 60)
              settings_buff[2]++;
            else
              settings_buff[2] = 1;  //
            break;

          case 3:  // Sensitivity (1–5)
            if (settings_buff[3] < 5)
              settings_buff[3]++;
            else
              settings_buff[3] = 1;  //
            break;

          case 4:  // Cooldown (1–60 sec)
            if (settings_buff[4] < 60)
              settings_buff[4]++;
            else
              settings_buff[4] = 1;  //
            break;
        }


        break;
      case settings_poll:
        //        ConnectToAWS();
        //        machine_state = settings_screen_wifi_OTA;

        break;
      default:
        break;
    }
  }
  //---------------------------------------------------------------------//
  if (R3_PRESSED) {
    //    Serial.println("R3 Pressed");
    R3_PRESSED = 0;
    switch (machine_state) {
      case home_screen:
        machine_state = fm_checking_page;
        //        delay(2000);
        //        Ble_sound_deinit();
        break;
      case bluetooth_playback:
        a2dp_play();
        break;
      case fm_screen:
        volumeUp();
        //        nextStation();
        break;
      case auto_turn_off_delay:
        if (set_time <= 0) {
          machine_state = home_screen_init;
          break;
        } else {
          if (fan_speed == 0) {
            fan_speed = 1;
            send_fan_speed_to_chimney(fan_speed);
            set_countdown = set_time * 60;
            startCountdown(set_countdown);
            machine_state = auto_turn_off_delay_countdown_init;
          } else {
            set_countdown = set_time * 60;
            startCountdown(set_countdown);
            machine_state = auto_turn_off_delay_countdown_init;
          }
        }
        break;
      case Whistle_Counter_SET_VAL:
        //        Serial.println(set_cooker_wistle_cnt);
        machine_state = Whistle_Counter_loop;
        break;
      case Set_clean_time_val:
        //        Serial.println(set_auto_clean_time);
        //        start_auto_clean = 1;
        //        startAutoClean(set_auto_clean_time*60);
        //        machine_state = Set_clean_time_countdown;
        break;
      case settings_poll:

        machine_state = settings_screen_info;
        break;
      case settings_screen_wifi_OTA_poll:
        //        ConnectToAWS();
        OTASetup();
        machine_state = settings_screen_wifi_OTA;
        break;
      case Whistle_Counter_setting_poll:
        for (int i = 0; i < 5; i++) {
          EEPROM.write((i * 2) + SETTING_ADDR, (settings_buff[i] >> 8));
          EEPROM.write((i * 2) + (SETTING_ADDR + 1), (settings_buff[i] & 0xFF));
          //          Serial.print((i * 2) + SETTING_ADDR);
          //          Serial.print("-");
          //          Serial.println(settings_buff[i]);
        }
        EEPROM.commit();
        machine_state = home_screen_init;
        break;
      case Whistle_Counter_counting:
        settings_buff[3]++;
        if (settings_buff[3] > 5) {
          settings_buff[3] = 1;
        }
        for (int i = 0; i < 5; i++) {
          EEPROM.write((i * 2) + SETTING_ADDR, (settings_buff[i] >> 8));
          EEPROM.write((i * 2) + (SETTING_ADDR + 1), (settings_buff[i] & 0xFF));
        }
        EEPROM.commit();
        break;

      default:
        break;
    }
  }
  //---------------------------------------------------------------------//
  if (POWER_PRESSED) {
    //    Serial.println("Power Pressed");
    POWER_PRESSED = 0;
    digitalWrite(2, !digitalRead(2));
  }
  //---------------------------------------------------------------------//
  if (FAN_PRESSED) {
    // if ((machine_state != Whistle_Counter_counting) && (machine_state != Whistle_Counter_SET_VAL) && (machine_state != Set_clean_time_countdown)) {
    if ((machine_state != Set_clean_time_countdown)) {
      Serial.println("Fan Pressed");
      FAN_PRESSED = 0;
      fan_speed++;
      if (fan_speed > 3)
        fan_speed = 1;
      send_fan_speed_to_chimney(fan_speed);
    } else {
      //      Serial.println("Fan NOT Pressed");
      FAN_PRESSED = 0;
    }
  }

  //---------------------------------------------------------------------//
  if (L2_PRESSED_LONG) {
    //    Serial.println("L2 Long Pressed");
    L2_PRESSED_LONG = 0;

    switch (machine_state) {
      case home_screen:
        send_light_to_chimney(0);
        light_brightness = 0;
        break;
    }
  }
  //---------------------------------------------------------------------//
  if (L3_PRESSED_LONG) {
    //    Serial.println("L3 Long Pressed");
    L3_PRESSED_LONG = 0;
    switch (machine_state) {
      case home_screen:
        break;
      case fm_screen:
        //scanStations();
        break;
      case auto_turn_off_delay:
        break;
      case Whistle_Counter_SET_VAL:
        for (int i = 0; i < 5; i++) {
          settings_buff[i] = (EEPROM.read((i * 2) + SETTING_ADDR) << 8) | EEPROM.read((i * 2) + (SETTING_ADDR + 1));
          //          Serial.print((i * 2) + SETTING_ADDR);
          //          Serial.print("-");
          //          Serial.println(settings_buff[i]);
        }
        machine_state = Whistle_Counter_setting_init;
        break;
      default:
        break;
    }
  }
  //---------------------------------------------------------------------//
  if (R3_PRESSED_LONG) {
    //    Serial.println("R3 Long Pressed");
    R3_PRESSED_LONG = 0;
    switch (machine_state) {
      case home_screen:
        break;
      case fm_screen:
        break;
      default:
        break;
    }
  }
  //---------------------------------------------------------------------//
  if (FAN_PRESSED_LONG) {
    //    Serial.println("Fan Long Pressed");
    FAN_PRESSED_LONG = 0;
    fan_speed = 0;
    send_fan_speed_to_chimney(fan_speed);
  }
  //---------------------------------------------------------------------//
  if (POWER_PRESSED_LONG) {
    //    Serial.println("Power Long Pressed");
    POWER_PRESSED_LONG = 0;
  }
}
//------------------------------------------------------------------------------------------------------------------//
