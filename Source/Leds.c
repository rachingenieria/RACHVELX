
#include "stm32f0xx_hal.h"

#include "Leds.h"


/*----------------------------------------------------------------------------
 *      FUNCION DE CONTROL LEDS
 *---------------------------------------------------------------------------*/

void LED_On_Off(int led_d, int led_i)
{
  if(led_d)
	{
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	}
	else
	{
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	}

	if(led_i)
	{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	}
	else
	{
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	}

}
