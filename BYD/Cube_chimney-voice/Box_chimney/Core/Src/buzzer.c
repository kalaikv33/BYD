/*
 * buzzer.c
 *
 *  Created on: Nov 21, 2020
 *      Author: Santhosh
 */


#include "buzzer_.h"




void start_buzzer(uint8_t beeps, uint32_t on_time, uint32_t off_time)
{
	buzzer_alter.is_active = 1;
	buzzer_alter.total_beeps = beeps;
	buzzer_alter.current_beep = 0;
	buzzer_alter.buzzer_on_alter = 1;
	buzzer_alter.beep_on_time = on_time;
	buzzer_alter.beep_off_time = off_time;
	buzzer_alter.last_tick = HAL_GetTick();

    Buzzer_ON;
}
void buzzer_task(void)
{
    if (!buzzer_alter.is_active)
        return;

    uint32_t now = HAL_GetTick();

    if (buzzer_alter.buzzer_on_alter)
    {
        if ((now - buzzer_alter.last_tick) >= buzzer_alter.beep_on_time)
        {
        	Buzzer_OFF;
        	buzzer_alter.buzzer_on_alter = 0;
        	buzzer_alter.last_tick = now;
        }
    }
    else
    {
        if ((now - buzzer_alter.last_tick) >= buzzer_alter.beep_off_time)
        {
        	buzzer_alter.current_beep++;
            if (buzzer_alter.current_beep >= buzzer_alter.total_beeps)
            {
            	buzzer_alter.is_active = 0;
                return;
            }

            Buzzer_ON;
            buzzer_alter.buzzer_on_alter = 1;
            buzzer_alter.last_tick = now;
        }
    }
}

void buzzer(char no_of_beeps,char alternative_beeps,int beep_length)
{
	buzz.buzz_start_flg=SET;
	buzz.alternative_beeps=alternative_beeps;//alternative_beeps;
	buzz.beep_length=beep_length;
	buzz.no_of_beeps=no_of_beeps;
	buzz.temp_no_beeps=0;
	buzz.temp_beep_length=0;
	buzz.hal_wait_delay=0;

}
void buzeer_beep(int beep_length)
{
	Buzzer_ON;//HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
	HAL_Delay(beep_length);
//	delay_us(beep_length);
	Buzzer_OFF;//HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_RESET);
}

void buzz_call_50_ms()
{
	if(++_50_ms_cnt>(20))
	{
		_50_ms_cnt=0;
	}
	if(buzz.buzz_start_flg)
	{
		if (HAL_GetTick()-buzz.temp_beep_length >= buzz.beep_length)
		{
			buzz.temp_beep_length=HAL_GetTick();
			Buzzer_OFF;//HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_RESET);
			if (HAL_GetTick()-buzz.hal_wait_delay >= 150)
			{
				buzz.hal_wait_delay=HAL_GetTick();
				if(buzz.temp_no_beeps<buzz.no_of_beeps)
				{
					buzz.temp_no_beeps++;
					if(buzz.alternative_beeps)
					{
						if(buzz.beep_length)
						{
							Buzzer_ON;//HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
						}

					}
					else
					{

						if(buzz.beep_length)
						{
							Buzzer_ON;//HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
						}

					}
				}
				else
				{
					memset(&buzz,0,sizeof(buzz));
				}
			}
		}
	}
}



//void play_tone(uint32_t freq)
//{
//	if (freq == 0)
//	{
//	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET); // stop tone
//	// __HAL_TIM_DISABLE_IT(&htim3);
//	return;
//	}
//
//	uint32_t toggle_us = 1000000 / (2 * freq);
//	__HAL_TIM_SET_AUTORELOAD(&htim3, toggle_us - 1);
//	__HAL_TIM_SET_COUNTER(&htim3, 0);
//	// __HAL_TIM_ENABLE_IT(&htim3);
//}


//void play_tone(uint32_t freq, uint32_t duration_ms)
//{
//    if (freq == 0 || duration_ms == 0)
//    {
//        HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
////        __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
//        tone_active = 0;
//        return;
//    }
//
//    uint32_t toggle_us = 1000000 / (2 * freq);
//    __HAL_TIM_SET_AUTORELOAD(&htim3, toggle_us - 1);
//    __HAL_TIM_SET_COUNTER(&htim3, 0);
//
//    tone_duration_ms = duration_ms;
//    tone_active = 1;
//
//    HAL_TIM_Base_Start_IT(&htim3);
//}
