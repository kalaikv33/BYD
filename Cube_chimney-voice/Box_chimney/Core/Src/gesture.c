/*
 * gesture.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Mukesh
 */
#include "gesture.h"
#define WS2812_BRIGHTNESS_R 					128
#define WS2812_BRIGHTNESS_G						100
#define WS2812_BRIGHTNESS_B 					75
//uint8_t BM32_GetIRStatus(void)
//{
//	    uint8_t reply[6];
//	    uint8_t checksum = 0;
////	    uint8_t cmd[5] = {0x55, 0x80, 0x02, 0x01, 0xD8};
////	    	HAL_UART_Transmit(&huart2, cmd, 5, 100);
//
//	    if (HAL_UART_Receive(&huart2, reply, 2, 100) == HAL_OK)
//	    {
//	    	if(reply[0] == 0x55 && reply[1] == 0xC0 && reply[2] == 0x02 ) // Validate header
//	        // Validate checksum
//	        for (int i = 0; i < 5; i++)
//	            checksum += reply[i];
//	        if (checksum == reply[5])
//	        {
//	        	Key_State.gesture_detected = 1;
//	        	if ((reply[4] == 0x09) /*|| (reply[4] == 0x00)*/)  // Hover
//	        		Key_State.hover=1;
//
//	        	else if (reply[4] == 0x04)                     // Left swipe
//	        		Key_State.left_swipe=1;
//
//	        	else if (reply[4] == 0x02)                     // Right swipe
//	        		Key_State.right_swipe=1;
//
//	        	else if (reply[4] == 0x01)
//	        		Key_State.approach=1;
//
//	        }
////	        HAL_Delay(10);
////	        else
////	            return 0xFD; // checksum error
//	    }
////	    return 0xFF; // timeout or receive failure
//}

uint8_t ReadVC02Data(void)
{
    uint8_t reply[2];

    if (HAL_UART_Receive(&huart2, reply, 2, 400) == HAL_OK)
    {
        if (reply[0] == 0xA1)
        {
            switch (reply[1])
            {
			case 0x80://wake up
				buzzer(2, 0, 10);
				ws2812_animation_all_off_then_middle_out(WS2812_BRIGHTNESS_R, WS2812_BRIGHTNESS_G, WS2812_BRIGHTNESS_B, 100);
				Clear_All();
				WS2812_Send();

				break;

			case 0x90://light on
				buzzer(1, 0, 5);///yellow
				Clear_All();
				WS2812_Send();
				Key_State.Key_Count=1;
				Set_LED(0, 128, 50, 0);
				Set_LED(1, 128, 50, 0);
				Set_LED(2, 128, 50, 0);
				Set_LED(18, 128, 50, 0);
				Set_LED(19, 128, 50, 0);
				Set_LED(20, 128, 50, 0);
				WS2812_Send();
				Main_led_on();
				Key_State.led_flag = 1;

				break;

			case 0x91://white
				Clear_All();
				WS2812_Send();
				Set_LED(0, 128, 100, 75);
				Set_LED(1, 128, 100, 75);
				Set_LED(2, 128, 100, 75);
				Set_LED(18, 128, 100, 75);
				Set_LED(19, 128, 100, 75);
				Set_LED(20, 128, 100, 75);
				WS2812_Send();
				Main_led_off();
//				Key_State.Key_Count=0;
				buzzer(1, 0, 5);
				break;

			case 0x93:
				Clear_All();
				WS2812_Send();
//				WS2812_Test_Pattern(7);
				Key_State.Key_Count=2;
				Key_State.machine_off = 0;
				Blower_Speed_1_ON;
				Blower_Speed_2__OFF;
				Blower_Speed_3__OFF;
				Key_State.led_flag = 1;
				buzzer(1, 0, 5);
				break;

			case 0x94:
				Clear_All();
				WS2812_Send();
//				WS2812_Test_Pattern(14);
				Key_State.Key_Count=3;
				Blower_Speed_1__OFF;
				Blower_Speed_3__OFF;
				Blower_Speed_2_ON;
				Key_State.led_flag = 1;
				buzzer(1, 0, 5);
				break;

			case 0x95:
				Clear_All();
				WS2812_Send();
//				WS2812_Test_Pattern(21);
				Key_State.Key_Count=4;
				Blower_Speed_1__OFF;
				Blower_Speed_2__OFF;
				Blower_Speed_3_ON;
				Key_State.led_flag = 1;
				buzzer(1, 0, 5);
				break;

			case 0x96://off
				Clear_All();
				WS2812_Send();
				Blower_Speed_1__OFF;
				Blower_Speed_2__OFF;
				Blower_Speed_3__OFF;
				Set_LED(0, 128, 100, 75);
				Set_LED(1, 128, 100, 75);
				Set_LED(2, 128, 100, 75);
				Set_LED(18, 128, 100, 75);
				Set_LED(19, 128, 100, 75);
				Set_LED(20, 128, 100, 75);
				WS2812_Send();
			    Key_State.Key_Count=0;

				buzzer(1, 0, 5);
				break;

			default:
				break;
			}

            return reply[1];
        }
    }

    return 0;
}







