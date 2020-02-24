
#include "cmsis_os.h"                                           // CMSIS RTOS header file

#include "stm32f0xx_hal.h"
#include <stdlib.h>

#include "VELOCISTA.h"

#define TIME_OUT_BLANCO  500
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Sensores':
 *---------------------------------------------------------------------------*/

 
void Thread_Sensores (void const *argument);                             // thread function
osThreadId tid_Thread;                                          // thread id
osThreadDef (Thread_Sensores, osPriorityNormal, 1, 0);                   // thread object

//VAriables GLobales
extern TIM_HandleTypeDef htim14;

extern velocista_st vel;
extern velocista_setup_st vel_setup;

//VARIABLES LOCALES
uint32_t sensor_timing[NUM_SENSORES];

int Sensores_peso[6] = {-35,-20,-8,8,20,35};

volatile int32_t sensores_actual[NUM_SENSORES];
volatile int32_t sensores_estado[NUM_SENSORES];

volatile int ultimo_error_valido;

int sensores_leidos[NUM_SENSORES];
volatile signed int Sensores_Error;
volatile	int	suma = 0;
volatile	int	num_sensors  = 0;

//CALIBRACION
uint32_t max_sens[NUM_SENSORES];
uint32_t min_sens[NUM_SENSORES];

int Init_Thread_Sensores (void) {

  tid_Thread = osThreadCreate (osThread(Thread_Sensores), NULL);
  if (!tid_Thread) return(-1);
  
  return(0);
}

void Thread_Sensores (void const *argument) {


GPIO_InitTypeDef GPIO_InitStruct;

HAL_TIM_Base_Start(&htim14);
vel.sensores = 0;
	
	for(int is=0; is < NUM_SENSORES ; is ++)
		{
			vel.tr_sens[is]  = 1000; //Default Value
		}
		

  while (1) {

	//OUTPUT


	GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_10; //GPIO_PIN_9,GPIO_PIN_4
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);

		//INPUT
	GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_10; //GPIO_PIN_9,GPIO_PIN_4
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



		//LEER SENSORES
		for(int i=0; i<NUM_SENSORES ; i++)
		{
		     sensores_leidos[i] = 1;
			   sensor_timing[i] = TIME_OUT_BLANCO; //frecuencia 1M -- 500 es 0.5ms
																//PERO SE DESCARGA EN LINEA A 0.25ms
			   sensores_estado[i] = 1; //LINEA NEGRA
			 
		}

		TIM14->CNT = 0;
		while(( sensores_leidos[0] ||
						sensores_leidos[1] ||
						sensores_leidos[2] ||
						sensores_leidos[3] ||
						sensores_leidos[4] ||
						sensores_leidos[5]	) && TIM14->CNT < TIME_OUT_BLANCO)
		{
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) && sensores_leidos[0])
					{
						 sensor_timing[0] = TIM14->CNT;
						 sensores_leidos[0] = 0;
             sensores_estado[0] = 0; 
					}

					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) && sensores_leidos[1])
					{
						 sensor_timing[1] = TIM14->CNT;
						sensores_leidos[1] = 0;
						sensores_estado[1] = 0; 

					}

						if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) && sensores_leidos[2])
					{
						 sensor_timing[2] = TIM14->CNT;
						 sensores_leidos[2] = 0;
						 sensores_estado[2] = 0; 

					}

					if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) && sensores_leidos[3])
					{
						 sensor_timing[3] = TIM14->CNT;
						 sensores_leidos[3] = 0;
						sensores_estado[3] = 0; 
					}

					if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) && sensores_leidos[4])
					{
						 sensor_timing[4] = TIM14->CNT;
						 sensores_leidos[4] = 0;
						sensores_estado[4] = 0; 
					}

					if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) && sensores_leidos[5])
					{
						 sensor_timing[5] = TIM14->CNT;
						 sensores_leidos[5] = 0;
						 sensores_estado[5] = 0; 

					}
				}
		   
			if((sensores_estado[0] == 1 && sensores_estado[5] == 1) &&
				 (sensores_estado[2] == 0 || sensores_estado[3] == 0))
			  {
			     vel_setup.tipopista = 1; //PISTA NEGRA
			  }
				
		 if((sensores_estado[0] == 0 && sensores_estado[5] == 0) &&
				 (sensores_estado[2] == 1 || sensores_estado[3] == 1))
			  {
			     vel_setup.tipopista = 2; //PISTA BLANCA
			  }
				
				
			if(vel_setup.tipopista == 1) //LINEA 
			{
					for(int i=0; i<NUM_SENSORES ; i++)
								{
									if(sensores_estado[i] == 1)
									   sensores_estado[i] = 0;
									else
										 sensores_estado[i] = 1;
								}
		 }
				
			
       suma = 0;
       num_sensors = 0;
				for(int i=0; i<NUM_SENSORES ; i++)
					{
		     if(sensores_estado[i] == 1)
					 {
						 suma += Sensores_peso[i];
					   num_sensors++;
					 }
				}
		
					if(num_sensors == 0) //Si no lee ningun mantiene el anterior
								{
									if(vel.sensores > 10)
									{
										vel.sensores = 45; //MAXIMO ERROR
									}
									else if(vel.sensores < -10)
									{
										vel.sensores = -45;
									}
									else //PERDIDA DE LINEA DE FRENTE
									{
										vel.sensores = 0;
									}

						  }
						else
						{
							     vel.sensores = suma/num_sensors;
						}


		osDelay(1);  // suspend thread
  }
}






void Sensores_Calibracion_Backgound(void)
{

for(int ind=0; ind < 100 ; ind ++)
			{
				for(int is=0; is < NUM_SENSORES ; is ++) //Leer los sensores y guardar los valores iniciales
					{
						 vel.fondo_pista[is] += sensor_timing[is];
					}
					osDelay(5);
			}

	for(int is=0; is < NUM_SENSORES ; is ++) //Leer los sensores y guardar los valores promedio de fondo de pista
		{
			 vel.fondo_pista[is] = vel.fondo_pista[is]/100;
	  }

}


void Sensores_Calibracion_Line(void)
{

	for(int is=0; is < NUM_SENSORES ; is ++)
		{
			vel.tr_sens[is]  = 0;
			max_sens[is]     = 0;
			min_sens[is]     = 10000;
		}

	 for(int ind=0; ind < 1000 ; ind ++)
				 {
					  for(int is=0; is < NUM_SENSORES ; is ++) //Leer los sensores y guardar el maximo, el minimo y promedio
				      {
								  if(sensor_timing[is] > max_sens[is])
										max_sens[is] = sensor_timing[is];

								  if(sensor_timing[is] < min_sens[is])
										 min_sens[is] = sensor_timing[is];

					              //vel.tr_sens[is] = (min_sens[is] + max_sens[is])/2;
									      vel.tr_sens[is] = TIME_OUT_BLANCO - 10;
				      }

						 osDelay(2);
				}

		//definir color de PISTA
				if ((vel.fondo_pista[0] > vel.tr_sens[0]) && (vel.fondo_pista[NUM_SENSORES-1] > vel.tr_sens[NUM_SENSORES-1]))
				{
				     vel_setup.tipopista = 1; // pista < negro
				}

				else if ((vel.fondo_pista[0] < vel.tr_sens[0]) && (vel.fondo_pista[NUM_SENSORES-1] < vel.tr_sens[NUM_SENSORES-1]))
				{
				     vel_setup.tipopista = 2; // pista > Blanco
				}
	      else
				{
					vel_setup.tipopista = 2; //SET MANUAL
				}

}
