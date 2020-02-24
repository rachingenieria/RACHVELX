

#include "stm32f0xx_hal.h"
#include <stdlib.h>

#include "motores.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
/*----------------------------------------------------------------------------
 *      FUNCION DE CONTROL MOTORES
 *---------------------------------------------------------------------------*/

void Motores_Init (void)
{
	HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);

	TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;

	TIM3->CCR1 = 0;
	TIM3->CCR2 = 0;
}


void Motores_Control (int mi, int md)
{
	int pmw_d = md;
	int pmw_i = mi;

		//derecha
		if( pmw_d >= 100)
		{
		   pmw_d = 99;
		}
		else if(pmw_d <= -100)
		{
		   pmw_d = -99;
		}

		if(pmw_d > 0)
		{
		  TIM3->CCR2 = MAXPWM - abs(pmw_d)*10;
		  TIM3->CCR1 = MAXPWM;
		}
		else if ( pmw_d < 0   )
		{
		  TIM3->CCR2 = MAXPWM;
		  TIM3->CCR1 = MAXPWM - abs(pmw_d)*10;

		}
		else // vel.pmw_d == 0
        {
		  TIM3->CCR2 = MAXPWM;
		  TIM3->CCR1 = MAXPWM;
		}


		//IZQUIERDA

	if( pmw_i >= 100)
		{
		   pmw_i = 99;
		}
		else if(pmw_i <= -100)
		{
		   pmw_i = -99;
		}

		if(pmw_i > 0)
		{
		  TIM2->CCR2 = MAXPWM - abs(pmw_i)*10;
		  TIM2->CCR1 = MAXPWM;
		}
		else if ( pmw_i < 0   )
		{
		  TIM2->CCR2 = MAXPWM;
		  TIM2->CCR1 = MAXPWM - abs(pmw_i)*10;

		}
		else // vel.pmw_i == 0
         {
		  TIM2->CCR2 = MAXPWM;
		  TIM2->CCR1 = MAXPWM;
		}
}

