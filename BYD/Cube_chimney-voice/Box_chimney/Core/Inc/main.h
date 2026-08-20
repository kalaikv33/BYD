/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_OUT_1_Pin GPIO_PIN_13
#define LED_OUT_1_GPIO_Port GPIOC
#define LED_OUT_2_Pin GPIO_PIN_14
#define LED_OUT_2_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_4
#define Buzzer_GPIO_Port GPIOA
#define Relay_Pin GPIO_PIN_5
#define Relay_GPIO_Port GPIOA
#define M3_Pin GPIO_PIN_6
#define M3_GPIO_Port GPIOA
#define M2_Pin GPIO_PIN_7
#define M2_GPIO_Port GPIOA
#define M1_Pin GPIO_PIN_0
#define M1_GPIO_Port GPIOB
#define Heater_Pin GPIO_PIN_1
#define Heater_GPIO_Port GPIOB
#define HB_LED_Pin GPIO_PIN_2
#define HB_LED_GPIO_Port GPIOB
#define LED_Light_Pin GPIO_PIN_10
#define LED_Light_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_10
#define LED_GPIO_Port GPIOA
#define sensorled_Pin GPIO_PIN_3
#define sensorled_GPIO_Port GPIOB
#define Blower_Button_Pin GPIO_PIN_9
#define Blower_Button_GPIO_Port GPIOB
#define Blower_Button_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */
#define HB_LED_ON							HAL_GPIO_TogglePin(HB_LED_GPIO_Port,HB_LED_Pin)

#define Buzzer_ON							HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,SET)
#define Buzzer_OFF							HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,RESET)

#define Light_ON							HAL_GPIO_WritePin(LED_Light_GPIO_Port,LED_Light_Pin,SET)
#define Light_OFF							HAL_GPIO_WritePin(LED_Light_GPIO_Port,LED_Light_Pin,RESET)

#define Heater_ON							HAL_GPIO_WritePin(Heater_GPIO_Port,Heater_Pin,SET)
#define Heater_OFF							HAL_GPIO_WritePin(Heater_GPIO_Port,Heater_Pin,RESET)

#define Blower_Speed_1_ON					HAL_GPIO_WritePin(M1_GPIO_Port,M1_Pin,SET)
#define Blower_Speed_1__OFF					HAL_GPIO_WritePin(M1_GPIO_Port,M1_Pin,RESET)

#define Blower_Speed_2_ON					HAL_GPIO_WritePin(M2_GPIO_Port,M2_Pin,SET)
#define Blower_Speed_2__OFF					HAL_GPIO_WritePin(M2_GPIO_Port,M2_Pin,RESET)

#define Blower_Speed_3_ON					HAL_GPIO_WritePin(M3_GPIO_Port,M3_Pin,SET)
#define Blower_Speed_3__OFF					HAL_GPIO_WritePin(M3_GPIO_Port,M3_Pin,RESET)

#define Relay_On							HAL_GPIO_WritePin(Relay_GPIO_Port,Relay_Pin,SET)
#define Relay_Off							HAL_GPIO_WritePin(Relay_GPIO_Port,Relay_Pin,RESET)

#define Blower_button						HAL_GPIO_ReadPin(Blower_Button_GPIO_Port,Blower_Button_Pin)

#define LED_1_ON							HAL_GPIO_WritePin(LED_OUT_1_GPIO_Port,LED_OUT_1_Pin,SET)
#define LED_1_OFF							HAL_GPIO_WritePin(LED_OUT_1_GPIO_Port,LED_OUT_1_Pin,RESET)

#define LED_2_ON							HAL_GPIO_WritePin(LED_OUT_2_GPIO_Port,LED_OUT_2_Pin,SET)
#define LED_2_OFF							HAL_GPIO_WritePin(LED_OUT_2_GPIO_Port,LED_OUT_2_Pin,RESET)

#define ULTRA_LONG_PRESS_THRESHOLD			2500
#define LONG_PRESS_THRESHOLD				1000
#define SHORT_PRESS_THRESHOLD				15



typedef struct Key_State_tt
{
	uint8_t Short_Press;
	uint8_t Long_Press;
	uint8_t Ultra_Long_Press;
	uint8_t Key_Count;
	uint8_t long_press_indicated ;			//for buzzer
    uint8_t ultra_long_press_indicated ;	//for buzzer
    uint8_t autoclean;
    uint8_t button_pressed;
    uint8_t button_still_pressed;
    uint8_t led_flag;
    uint8_t machine_off;
    uint8_t machine_idle;
    uint8_t autoclean_buzzer;
    uint8_t right_swipe;
	uint8_t left_swipe;
	uint8_t hover;
	uint8_t approach;
	uint8_t gesture_detected;
}Key_State_t;

float Read_Dust(void);
//void delay_us(uint16_t us);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
