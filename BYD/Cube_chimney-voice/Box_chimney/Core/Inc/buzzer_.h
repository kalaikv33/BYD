/*
 * buzzer_.h
 *
 *  Created on: Nov 21, 2020
 *      Author: Santhosh
 */

#ifndef INC_BUZZER__H_
#define INC_BUZZER__H_

//#include "stm32f0xx.h"
//#include "stm32f0xx_hal_rcc.h"
//#include "stm32f0xx_hal_gpio.h"
//#include "stm32f0xx_hal_exti.h"
//#include "stm32f030x8.h"
#include "main.h"

typedef struct {
    uint8_t is_active;        // 1 = Buzzer in use
    uint8_t total_beeps;      // Number of ON-OFF beeps
    uint8_t current_beep;     // How many beeps completed
    uint8_t buzzer_on_alter;        // State flag
    uint32_t beep_on_time;    // ON duration (ms)
    uint32_t beep_off_time;   // OFF duration (ms)
    uint32_t last_tick;       // Last time buzzer toggled
} Buzzer_t;
Buzzer_t buzzer_alter;




typedef struct BUZZ__
{
	char buzz_start_flg,on_time_ld_flg;
	char no_of_beeps,temp_no_beeps, alternative_beeps;
	int beep_length,temp_beep_length,hal_wait_delay;

}BUZER;
extern char one_time_preheat_bfr_sleep_flg,after_preheat_flag,process_complete_buzzer_play;
extern void buzzer(char no_of_beeps,char alternative_beeps,int beep_length);
extern void buzz_call_50_ms();
extern void buzeer_beep(int beep_length);
void start_buzzer(uint8_t beeps, uint32_t on_time, uint32_t off_time);
void buzzer_task(void);


//#define NOTE_B0 31
//
//// Octave 1
////#define NOTE_C1 33
////#define NOTE_CS1 35
////#define NOTE_D1 37
////#define NOTE_DS1 39
////#define NOTE_E1 41
////#define NOTE_F1 44
////#define NOTE_FS1 46
////#define NOTE_G1 49
////#define NOTE_GS1 52
////#define NOTE_A1 55
////#define NOTE_AS1 58
////
////// Octave 2
////#define NOTE_B1 62
////#define NOTE_C2 65
////#define NOTE_CS2 69
////#define NOTE_D2 73
////#define NOTE_DS2 78
////#define NOTE_E2 82
////#define NOTE_F2 87
////#define NOTE_FS2 93
////#define NOTE_G2 98
////#define NOTE_GS2 104
////#define NOTE_A2 110
////#define NOTE_AS2 117
////
////// Octave 3
////#define NOTE_B2 123
////#define NOTE_C3 131
////#define NOTE_CS3 139
////#define NOTE_D3 147
////#define NOTE_DS3 156
////#define NOTE_E3 165
////#define NOTE_F3 175
////#define NOTE_FS3 185
////#define NOTE_G3 196
////#define NOTE_GS3 208
////#define NOTE_A3 220
////#define NOTE_AS3 233
////
////// Octave 4 (standard piano middle)
////#define NOTE_B3 247
////#define NOTE_C4 262
////#define NOTE_CS4 277
////#define NOTE_D4 294
////#define NOTE_DS4 311
////#define NOTE_E4 330
////#define NOTE_F4 349
////#define NOTE_FS4 370
////#define NOTE_G4 392
////#define NOTE_GS4 415
////#define NOTE_A4 440
////#define NOTE_AS4 466
////
////// Octave 5
////#define NOTE_B4 494
////#define NOTE_C5 523
////#define NOTE_CS5 554
////#define NOTE_D5 587
////#define NOTE_DS5 622
////#define NOTE_E5 659
////#define NOTE_F5 698
////#define NOTE_FS5 740
////#define NOTE_G5 784
////#define NOTE_GS5 831
////#define NOTE_A5 880
////#define NOTE_AS5 932
//
//// Octave 6
//#define NOTE_B5 988
//#define NOTE_C6 1047
//#define NOTE_CS6 1109
//#define NOTE_D6 1175
//#define NOTE_DS6 1245
//#define NOTE_E6 1319
//#define NOTE_F6 1397
//#define NOTE_FS6 1480
//#define NOTE_G6 1568
//#define NOTE_GS6 1661
//#define NOTE_A6 1760
//#define NOTE_AS6 1865
//
//// Octave 7
//#define NOTE_B6 1976
//#define NOTE_C7 2093
//#define NOTE_CS7 2217
//#define NOTE_D7 2349
//#define NOTE_DS7 2489
//#define NOTE_E7 2637
//#define NOTE_F7 2794
//#define NOTE_FS7 2960
//#define NOTE_G7 3136
//#define NOTE_GS7 3322
//#define NOTE_A7 3520
//#define NOTE_AS7 3729
//
//// Octave 8
//#define NOTE_B7 3951
//#define NOTE_C8 4186
//#define NOTE_CS8 4435
//#define NOTE_D8 4699
//#define NOTE_DS8 4978
//
//// Silence
//#define REST 0


BUZER buzz;
int _50_us_cnt,_50_ms_cnt,_1_sec_cnt;
char one_time_preheat_bfr_sleep_flg,after_preheat_flag,process_complete_buzzer_play;
//
//extern TIM_HandleTypeDef htim3;
//extern uint8_t tone_active=0;
//extern uint32_t tone_duration_ms=0;

//const uint16_t tone_array[] = {
//		NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_FS7, NOTE_G7, NOTE_GS7
//};
//
//const uint16_t duration_array[] = {
//		300, 300, 300, 300, 400, 200, 400, 500
//};


#endif /* INC_BUZZER__H_ */
