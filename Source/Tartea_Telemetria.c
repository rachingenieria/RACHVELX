

#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f0xx_hal.h"
#include <stdlib.h>

#include "VELOCISTA.h"


extern UART_HandleTypeDef huart2;
extern velocista_st vel;
extern velocista_setup_st vel_setup;

uint8_t uart_buffer_rx[32];
uint8_t uart_buffer_tx[32];
uint8_t uart_rx[10];
int count;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	 if(huart->Instance == USART2)
    {

			if(count < 3 )
			{

					if( (count == 0) && (uart_rx[0] == 'V' || uart_rx[0] == 'D' || uart_rx[0] == 'P' || uart_rx[0] == 'I'|| uart_rx[0] == 'T' || uart_rx[0] == 'S'))
					{
						 uart_buffer_rx[0] = uart_rx[0];
						 count ++;
					}

					else if ((count == 1) && (uart_rx[0] >= '0'  && uart_rx[0] <= '9'))
					{

						uart_buffer_rx[1] = uart_rx[0];
						count ++;
					}

					else if ((count == 2) && (uart_rx[0] >= '0'  && uart_rx[0] <= '9'))
						{
							uart_buffer_rx[2] = uart_rx[0];

									if(uart_buffer_rx[0] == 'V')
									{
										 vel_setup.vavg = (uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2]- '0' );
									}
									else if(uart_buffer_rx[0] == 'P')
									{
										 vel_setup.kpg = (float)((uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2]- '0' ))/10;
									}
									else if(uart_buffer_rx[0] == 'D')
									{
										 vel_setup.kdg = (float)((uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2]- '0' ))/10;
									}
									else if(uart_buffer_rx[0] == 'I')
									{
										 vel_setup.kig = (float)((uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2]- '0' ))/1000;
									}
									else if(uart_buffer_rx[0] == 'T')
									{
										 vel_setup.pmw_t = ( uart_buffer_rx[1] - '0' );
									}
									else if(uart_buffer_rx[0] == 'S')
									{
										 if((uart_buffer_rx[1] == '1'  && uart_buffer_rx[2] == '5'))
										 {
												 MC_Write_Setup();
												 LED_On_Off(1,1);
												 osDelay(200);
												 LED_On_Off(0,0);
												 osDelay(200);
												LED_On_Off(1,1);
												osDelay(200);
												LED_On_Off(0,0);
										 }

										 if((uart_buffer_rx[1] == '2'  && uart_buffer_rx[2] == '0'))
										 {
												 vel.start = 0XFF;
										 }

										 if((uart_buffer_rx[1] == '2'  && uart_buffer_rx[2] == '5'))
										 {
												 vel.start = 0;
										 }

										 if((uart_buffer_rx[1] == '3'  && uart_buffer_rx[2] == '0'))
										 {
												 vel_setup.remoto_enable = 1;
										 }

										 if((uart_buffer_rx[1] == '3'  && uart_buffer_rx[2] == '5'))
										 {
												 vel_setup.remoto_enable = 0;
										 }

										 if((uart_buffer_rx[1] == '4'  && uart_buffer_rx[2] == '0'))
										 {
												 vel_setup.sw_enable = 1;
										 }

										 if((uart_buffer_rx[1] == '4'  && uart_buffer_rx[2] == '5'))
										 {
												 vel_setup.sw_enable = 0;
										 }

										 //PISO
										  if((uart_buffer_rx[1] == '5'  && uart_buffer_rx[2] == '0'))
										 {
												 vel_setup.tipopista = 2;
										 }

										 if((uart_buffer_rx[1] == '5'  && uart_buffer_rx[2] == '5'))
										 {
												 vel_setup.tipopista = 1;
										 }
									}

									count = 0;
									vel.flag_tx=1;


					}
				}
				else
				{
				  count = 0;
				  uart_buffer_rx[0] = 0;
			      uart_buffer_rx[1] = 0;
			      uart_buffer_rx[2] = 0;
				}



				//MX_USART1_UART_Init();
				HAL_UART_Receive_IT(&huart2, &uart_rx[0] , 1);
	}

}

