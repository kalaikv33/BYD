/*
 * gesture.h
 *
 *  Created on: Jul 9, 2025
 *      Author: Mukesh
 */

#ifndef INC_GESTURE_H_
#define INC_GESTURE_H_

#include "main.h"
#include <stdint.h>

extern UART_HandleTypeDef huart2;
extern Key_State_t Key_State;
uint8_t BM32_GetIRStatus(void);
uint8_t ReadVC02Data(void);
#endif /* INC_GESTURE_H_ */
