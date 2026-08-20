/*
 * spi_rgb.h
 *
 *  Created on: Feb 23, 2024
 *      Author: Yashwardhan
 */
//----------------------------------------------------------------------------------------------//
#ifndef INC_SPI_RGB_H_
#define INC_SPI_RGB_H_
//----------------------------------------------------------------------------------------------//
#include "main.h"
//--------------------------------------------------------------------------------------------//

#define WS2812_BRIGHTNESS 						128
#define WS2812_BRIGHTNESS_R 					128
#define WS2812_BRIGHTNESS_G						100
#define WS2812_BRIGHTNESS_B 					75
#define MAX_LED              					21
#define LED_BITS             					24
#define RESET_SLOTS          					50
#define PWM_RESOLUTION       					90     // Timer period = 89
#define WS2812_HIGH         					40     // 2/3 for logic '1'
#define WS2812_LOW           					20     // 1/3 for logic '0'

//----------------------------------------------------------------------------------------------//

extern uint32_t press_duration,button_press_time,duration,autoclean_cntr;
extern uint8_t leds_to_light,button_pressed;
extern uint16_t led_cntr;
extern TIM_HandleTypeDef htim1;
extern Key_State_t Key_State;

//----------------------------------------------------------------------------------------------//

uint8_t previous_leds_to_light =0;
uint8_t LED_Data[MAX_LED][4];    // [LED][0: index, 1: G, 2: R, 3: B]
uint16_t pwmData[MAX_LED * LED_BITS + RESET_SLOTS];
uint8_t datasentflag;


//----------------------------------------------------------------------------------------------//

void system_init(void);
void ws2812_animation_all_off_then_middle_out(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms);
void Reset_All(void);
void Check_Button_Hold_Status(void);
void keypad_process(void);
void Gesture_process(void);
void LED_process(void);
void delay_us(uint32_t us);
void WS2812_Init(void);
void WS2812_Send(void);
void Set_LED(uint8_t num, uint8_t r, uint8_t g, uint8_t b);
void Set_LED_All(uint8_t r, uint8_t g, uint8_t b);
void LED_Fade_All_Comet(uint8_t led_no,uint8_t r, uint8_t g, uint8_t b, uint8_t tail_length, uint8_t delay_ms);
void LED_Fade_All_InOut(uint8_t r, uint8_t g, uint8_t b, uint8_t steps, uint8_t delay_ms);
void Clear_All(void);
void WS2812_Test_Pattern(uint8_t led_no);
void Main_led_on(void);
void Main_led_off(void);


extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

//----------------------------------------------------------------------------------------------//

#endif /* INC_SPI_RGB_H_ */

//----------------------------------------------------------------------------------------------//
