/*
 * spi_rgb.c
 *
 *  Created on: Feb 23, 2024
 *      Author: Yashwardhan
 */
//----------------------------------------------------------------------------------------------//
//#include "machine_process.h"
#include "spi_rgb.h"
#include <string.h>
#include <math.h>
//----------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------//
void system_init(void)
{
	Key_State.Short_Press=0;
	Key_State.Long_Press=0;
	Key_State.Ultra_Long_Press=0;
	Key_State.machine_off = 1;
	Key_State.machine_idle = 1;
	LED_1_ON;
	LED_2_ON;
	Buzzer_OFF;
//	Light_OFF;
	Heater_OFF;
	Blower_Speed_1__OFF;
	Blower_Speed_2__OFF;
	Blower_Speed_3__OFF;
	Relay_Off;
	Key_State.autoclean=0;
	ws2812_animation_all_off_then_middle_out(WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B, 100);
	Clear_All();
	WS2812_Send();
	Set_LED(0, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(1, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(2, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
//	Set_LED(17, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(18, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(19, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(20, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	WS2812_Send();
	uint8_t cmd[5] = {0x55, 0x80, 0x02, 0x01, 0xD8};
	HAL_UART_Transmit(&huart2, cmd, 5, 100);
}

//----------------------------------------------------------------------------------------------//

void ws2812_animation_all_off_then_middle_out(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms)
{
    Clear_All();          // Step 1: Turn all LEDs off
    WS2812_Send();        // Send buffer via DMA
    HAL_Delay(delay_ms);  // Optional delay before starting
    uint16_t mid = MAX_LED / 2;
    for (uint16_t step = 0; step <= mid; step++)
    {
        if ((mid - step) < MAX_LED)
            Set_LED(mid - step, r, g, b);

        if ((mid + step) < MAX_LED && step != 0)
            Set_LED(mid + step, r, g, b);

        WS2812_Send();    // Send updated LED data via PWM DMA
        HAL_Delay(delay_ms);
    }
}

//----------------------------------------------------------------------------------------------//

void Reset_All(void)
{
	Main_led_off();
    Heater_OFF;
    Blower_Speed_1__OFF;
    Blower_Speed_2__OFF;
    Blower_Speed_3__OFF;
    Clear_All();
	WS2812_Send();
	Set_LED(0, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(1, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
    Set_LED(2, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(18, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(19, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	Set_LED(20, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
	WS2812_Send();
	WS2812_Send();
    Buzzer_OFF;
}

//----------------------------------------------------------------------------------------------//

void Check_Button_Hold_Status(void)
{
	if (Key_State.button_pressed && Key_State.Short_Press == 0)
	  {
		  duration = HAL_GetTick() - button_press_time;
		  if (duration >= 975 && !Key_State.long_press_indicated && Key_State.machine_idle== 0 && Key_State.Ultra_Long_Press == 0 )
		  {
			  Key_State.long_press_indicated = 1;
			  buzzer(1, 0, 500);  // Indicate long press is initiating
		  }
		  if (duration >= 2450 && !Key_State.ultra_long_press_indicated && Key_State.machine_idle == 1)
		  {
			  Key_State.Ultra_Long_Press = 1;
			  Key_State.ultra_long_press_indicated = 1;
			  buzzer(3, 0, 10); // Indicate ultra-long press is initiating
		  }
	  }
}

//----------------------------------------------------------------------------------------------//

void keypad_process(void)
{
	  if (Key_State.Short_Press == 1 && Key_State.autoclean == 0)
	  {
		  Key_State.Short_Press = 0;
		  Key_State.machine_off = 0;
		  Key_State.machine_idle=0;
		  press_duration = 0;
		  led_cntr =0;
	      switch (Key_State.Key_Count++)
	      {
	          case 0:
	        	  buzzer(1, 0, 5);
	              Main_led_on();
	              Key_State.led_flag=1;
	              break;
	          case 1:
	        	  Clear_All();
				  WS2812_Send();
	        	  Key_State.machine_off =0;
	              Blower_Speed_1_ON;
	              Blower_Speed_2__OFF;
	              Blower_Speed_3__OFF;
	              Key_State.led_flag =1;
	              buzzer(1, 0, 5);
	              break;
	          case 2:
	        	  Clear_All();
	        	  WS2812_Send();
	              Blower_Speed_1__OFF;
	              Blower_Speed_3__OFF;
	              Blower_Speed_2_ON;
	              Key_State.led_flag =1;
	              buzzer(1, 0, 5);
	              break;
	          case 3:
	        	  Clear_All();
	        	  WS2812_Send();
	              Blower_Speed_1__OFF;
	              Blower_Speed_2__OFF;
	              Blower_Speed_3_ON;
	              Key_State.led_flag =1;
	              buzzer(1, 0, 5);
	              break;
	          default:
	              Key_State.Key_Count = 1; // restart from speed 1 on next press
	              Key_State.Short_Press = 1; // reuse short press
	              break;
	      }
	  }
	  else if ((Key_State.Long_Press)&&(Key_State.machine_idle==0))
	  {
		  Key_State.machine_off=1;
		  Key_State.machine_idle=1;
	      Key_State.Long_Press = 0;
	      Key_State.Short_Press = 0;
	      Key_State.Ultra_Long_Press = 0;
	      Key_State.Key_Count = 0;
	      Key_State.autoclean = 0;
	      Reset_All();
	      autoclean_cntr=0;
	      leds_to_light=0;
	      press_duration = 0;
		  Key_State.long_press_indicated = 0;
		  Key_State.ultra_long_press_indicated = 0;
	  }
	  else if ((Key_State.Ultra_Long_Press)&&(Key_State.machine_idle == 1))
	  {
		  HAL_Delay(500);
	      Key_State.Ultra_Long_Press = 2;
	      Key_State.machine_idle=0;
	      Key_State.autoclean = 1;
	      Heater_ON;
	      Blower_Speed_1__OFF;
	      Blower_Speed_2__OFF;
	      Blower_Speed_3__OFF;
	      buzzer(1, 0, 800);
	      ws2812_animation_all_off_then_middle_out(16, 0, 128, 100);
	      Clear_All();
	      WS2812_Send();
	      Set_LED(0, 16, 0, 128);
	      press_duration = 0;
	      autoclean_cntr=0;
	      leds_to_light=0;
	      previous_leds_to_light=1;
	      Key_State.long_press_indicated = 0;
	      Key_State.ultra_long_press_indicated = 2;
	  }
}

void Gesture_process(void)
{
    if (Key_State.gesture_detected && Key_State.autoclean==0)
    {
        Key_State.gesture_detected = 0;
        if (Key_State.right_swipe)
        {
        	Key_State.machine_idle=0;
            Key_State.right_swipe = 0;
            Key_State.Key_Count++;

            if (Key_State.Key_Count > 4)
            {
                Key_State.Key_Count = 4;
                buzzer(2, 0, 100); // Double beep – max limit reached
            }
            else
            {
                buzzer(1, 0, 5);  // Single beep – valid swipe
            }
        }
        else if (Key_State.left_swipe)
        {
        	Key_State.machine_idle=0;
            Key_State.left_swipe = 0;
            if (Key_State.Key_Count > 2)
            {
                Key_State.Key_Count--;
                buzzer(1, 0, 5);  // Single beep – valid swipe
            }
            else
            {
                buzzer(2, 0, 100); // Double beep – already at minimum
            }
        }
        else if(Key_State.hover && Key_State.machine_idle == 0)
        {
        	buzzer(1, 0, 500);
			Key_State.machine_off=1;
			Key_State.hover =0;
			Key_State.machine_idle=1;
			Key_State.Long_Press = 0;
			Key_State.Short_Press = 0;
			Key_State.Ultra_Long_Press = 0;
			Key_State.Key_Count = 0;
			Key_State.autoclean = 0;
			Reset_All();
			autoclean_cntr=0;
			leds_to_light=0;
			press_duration = 0;
			Key_State.long_press_indicated = 0;
			Key_State.ultra_long_press_indicated = 0;
        }
        else
        {
          return; // No valid swipe
        }
        // Perform action based on current count
        Key_State.led_flag = 1;
        Key_State.machine_off = 0;
        if(Key_State.machine_idle == 0)
        {
			switch (Key_State.Key_Count)
			{
				case 1:
					Main_led_on();
					break;
				case 2:
					Clear_All();
					WS2812_Send();
					Blower_Speed_1_ON;
					Blower_Speed_2__OFF;
					Blower_Speed_3__OFF;
					break;
				case 3:
					Clear_All();
					WS2812_Send();
					Blower_Speed_1__OFF;
					Blower_Speed_2_ON;
					Blower_Speed_3__OFF;
					break;
				case 4:
					Clear_All();
					WS2812_Send();
					Blower_Speed_1__OFF;
					Blower_Speed_2__OFF;
					Blower_Speed_3_ON;
					break;
			}
        }
    }
}


//----------------------------------------------------------------------------------------------//

void LED_process(void)
{
	if(Key_State.autoclean == 1)
	{
		 if (leds_to_light != previous_leds_to_light)
		 {
			 previous_leds_to_light = leds_to_light;
			 if(leds_to_light < 20)
			 {
				 for (int i = 0; i < MAX_LED; i++)
				 {
					 if (i <= leds_to_light) { //no of leds to turn on
						 Set_LED(i, 16, 0, 128);// Set_LED(i, 0, 0, WS2812_BRIGHTNESS);  // blue LED ON
					 }
					 else
						 Set_LED(i, 0, 0, 0);
				 }
				 WS2812_Send();
			 }
		 }
		 else if(leds_to_light >=20)
		 {
			 LED_Fade_All_InOut(WS2812_BRIGHTNESS, 0, 0, 50, 10);
			 if(Key_State.autoclean_buzzer == 1)
				 	 start_buzzer(1, 500, 250);
		 }
		 Key_State.Short_Press=0;
	}
	else if((Key_State.led_flag == 1)&&(Key_State.autoclean == 0))	{
		switch (Key_State.Key_Count)
		  {
			case 0:
				if(Key_State.led_flag == 1 && Key_State.machine_off == 1)
				{
					Set_LED(0, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
					Set_LED(1, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
					Set_LED(2, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
					Set_LED(18, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
					Set_LED(19, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
					Set_LED(20, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B);
					WS2812_Send();
				}
			  break;
			case 1:
				if(/*Key_State.led_flag == 1 &&*/ Key_State.machine_off == 0)
				{
					Set_LED(0, 128, 50, 0);
					Set_LED(1, 128, 50, 0);
					Set_LED(2, 128, 50, 0);
					Set_LED(18, 128, 50, 0);
					Set_LED(19, 128, 50, 0);
					Set_LED(20, 128, 50, 0);
					WS2812_Send();
				}
			  break;
			case 2:
				if(Key_State.machine_off == 0)
				{
					WS2812_Test_Pattern(7);
//					LED_Fade_All_Comet(1, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B, 3, 100);
				}
				break;
			case 3:
				if(Key_State.machine_off == 0)
				{
					WS2812_Test_Pattern(14);
//					LED_Fade_All_Comet(2, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B, 6, 80);
				}
				break;
			case 4:
				if(Key_State.machine_off == 0)
				{
					WS2812_Test_Pattern(21);
//					LED_Fade_All_Comet(3, WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B, 9, 60);
				}
				break;
		  }
		 Key_State.led_flag = 0;
	}
}

//----------------------------------------------------------------------------------------------//

//void delay_us(uint32_t us)
//{
//	 __HAL_TIM_SET_COUNTER(&htim4, 0);  // Reset counter
//	    while (__HAL_TIM_GET_COUNTER(&htim4) < us);
//}

//----------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------//

//pwm dma

void WS2812_Init(void) {
    Clear_All();
    WS2812_Send();
}

//----------------------------------------------------------------------------------------------//

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_2);
        datasentflag = 1;
    }
}

//----------------------------------------------------------------------------------------------//

void WS2812_Send(void) {
    uint32_t indx = 0;
    uint32_t color;
    for (int i = 0; i < MAX_LED; i++) {
        color = ((LED_Data[i][1] << 16) | (LED_Data[i][2] << 8) | (LED_Data[i][3]));
        for (int bit = 23; bit >= 0; bit--) {
            pwmData[indx++] = (color & (1 << bit)) ? WS2812_HIGH : WS2812_LOW;
        }
    }
    // Reset pulse
    for (int i = 0; i < RESET_SLOTS; i++) {
        pwmData[indx++] = 0;
    }
    datasentflag = 0;
    HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_2,(uint32_t *)pwmData ,indx );
    while (!datasentflag);  // Wait for transmission done
}

//----------------------------------------------------------------------------------------------//

void Set_LED(uint8_t num, uint8_t r, uint8_t g, uint8_t b) {
    if (num < MAX_LED) {
        LED_Data[num][0] = num;
        LED_Data[num][1] = g;
        LED_Data[num][2] = r;
        LED_Data[num][3] = b;
    }
}

//----------------------------------------------------------------------------------------------//

void Set_LED_All(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < MAX_LED; i++)
    {
        Set_LED(i, r, g, b);
    }
}

//----------------------------------------------------------------------------------------------//

void LED_Fade_All_Comet(uint8_t led_no,uint8_t r, uint8_t g, uint8_t b, uint8_t tail_length, uint8_t delay_ms)
{
	static uint32_t last_update = 0;
	static int head = 0;
	static uint8_t active_led_count = 0;
	static uint8_t busy = 0;
	if (!busy) {
		if (led_no < 1 || led_no > 3)
			return;
		active_led_count = (led_no == 1) ? 7 : (led_no == 2) ? 14 : 21;
		head = 0;
		busy = 1;
	}
	if (HAL_GetTick() - last_update >= delay_ms) {
		last_update = HAL_GetTick();
		Clear_All();
		for (int i = 0; i < tail_length; i++) {
			int led_index = head - i;
			if (led_index >= 0 && led_index < active_led_count) {
				float fade = powf(0.7f, i);  // Exponential fade
				Set_LED(led_index,
						(uint8_t)(r * fade),
						(uint8_t)(g * fade),
						(uint8_t)(b * fade));
			}
		}
		WS2812_Send();
		head++;
		if (head > (active_led_count + tail_length)) {
			Clear_All();
			WS2812_Send();
			busy = 0;  // animation finished
		}
	}
}

//----------------------------------------------------------------------------------------------//

void LED_Fade_All_InOut(uint8_t r, uint8_t g, uint8_t b, uint8_t steps, uint8_t delay_ms)
{
    // Fade in
    for (int i = 0; i <= steps; i++)
    {
        float factor = (float)i / steps;
        Set_LED_All(r * factor, g * factor, b * factor);
        WS2812_Send();
        HAL_Delay(delay_ms);
    }
    // Fade out
    for (int i = steps; i >= 0; i--)
    {
        float factor = (float)i / steps;
        Set_LED_All(r * factor, g * factor, b * factor);
        WS2812_Send();
        HAL_Delay(delay_ms);
    }
}

//----------------------------------------------------------------------------------------------//

void Clear_All(void) {
    for (int i = 0; i < MAX_LED; i++) {
        Set_LED(i, 0, 0, 0);
    }
}

//----------------------------------------------------------------------------------------------//

void WS2812_Test_Pattern(uint8_t led_no) {
    for (int i = 0; i < led_no; i++) {
        Set_LED(i, 128, 100, 75);
        WS2812_Send();
        HAL_Delay(35);
    }
    WS2812_Send();
}

//----------------------------------------------------------------------------------------------//
//void Main_led_on(void)
//{
//	  for (int duty = 0; duty <= 999; duty += 9)
//	  {
//		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, duty);
//		  HAL_Delay(1); // controls speed of fade
//	  }
//}
//
//void Main_led_off(void)
//{
//	  for (int duty = 999; duty >= 0; duty-= 9)
//	  {
//		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, duty);
//		  HAL_Delay(1);
//	  }
//}
uint16_t current_duty = 0;   // keep track of current LED brightness

void Main_led_on(void)
{
    for (int duty = current_duty; duty <= 999; duty += 9)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, duty);
        HAL_Delay(1);
    }
    current_duty = 999;
}

void Main_led_off(void)
{
    for (int duty = current_duty; duty >= 0; duty -= 9)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, duty);
        HAL_Delay(1);
    }
    current_duty = 0;
}


