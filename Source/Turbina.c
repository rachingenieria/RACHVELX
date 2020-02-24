
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f0xx_hal.h"

#include "Turbina.h"
extern TIM_HandleTypeDef htim1;

/*----------------------------------------------------------------------------
 *      FUNCION DE CONTROL TURBINA
 *---------------------------------------------------------------------------*/

void Turbina_Control (int Vel)
{
	int turbina;
	turbina = (TURBINA_MAX - TURBINA_STOP) * Vel/10  + TURBINA_STOP;
	TIM1->CCR1 = turbina;
}


void Turbina_Init (void)
{
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_1);

	Turbina_Control(5);
	osDelay(3000);
	Turbina_Control(0);

}



