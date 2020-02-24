#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f0xx_hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "VELOCISTA.h"

//ESTRUCTURA GLOBAL DEL PROYECTO
velocista_st vel;
velocista_setup_st vel_setup;

extern UART_HandleTypeDef huart2;
extern uint8_t uart_buffer_rx[32];
extern uint8_t uart_rx[10];
extern uint8_t uart_buffer_tx[32];

void VEL_Load_Default_Setup (void)
{
		vel_setup.version = 0x0A04;

		vel_setup.tipopista = 2; //SET MANUAL


		vel_setup.kpg     = 2.0;   //2.0  en test noche y vuelta
		vel_setup.kdg     = 7.2;   //7.2 EN PRIMERA
		vel_setup.kig     = 0.0005;

		vel_setup.vavg    =  75;    //75 ok //Control de velocidad - MINIMO 65

		vel_setup.pmw_t   =  6;      //0 A 9


		vel.sensores = 0;
		vel.pmw_d =  0;
		vel.pmw_i =  0;

		vel_setup.remoto_enable = 1;
		vel_setup.sw_enable = 0;
		vel.start = 0XFF;
}



static int Control_SW(void)
{
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6))
		return 1;
	else
		return 0;
}


static int Control_RM(void)
{
  if(vel_setup.remoto_enable)
	{
	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
		   return 1;
	   else
		   return 0;
	 }


  else if(vel_setup.sw_enable)
	{
	 if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6))
		return 0;
	else
		return 1;
	 }

	 else
   {
	    return 0;
	 }


}


void Thread_Control (void)
{
	 Motores_Init();
	 Motores_Control(0,0);

	 //Start Valores por defecto
	 VEL_Load_Default_Setup ();
	 MC_Load_Setup();

	 //ALISTA PARA RECIBIR COMANDOS
	 HAL_UART_Receive_IT(&huart2, &uart_rx[0] , 1);

//TEST TURNINA
	 Turbina_Init();
//	osDelay(2000);
//	Turbina_Control(vel_setup.pmw_t);
//	osDelay(1000);
//	Turbina_Control(0);


	//Calibracion
	LED_On_Off(1, 1);


	while(Control_SW())
	{
		LED_On_Off(1, 1);
		osDelay(1);
			if(vel.flag_tx)
				{
					sprintf((char*)uart_buffer_tx,"V%d -P%d -D%d -T%d\n\r",(int)vel_setup.vavg,(int)(10*vel_setup.kpg),(int)(10*vel_setup.kdg),vel_setup.pmw_t);
					HAL_UART_Transmit(&huart2, uart_buffer_tx, strlen((char*)uart_buffer_tx),1000);
					vel.flag_tx=0;
				}
	}


	/************************************************************
	// INICIO CALIBRACION
	*****************************************************************/

	LED_On_Off(0, 0);
	osDelay(500); //ESPERA QUE SE QUITE EL DEDO DEL SWITCH

	//Poner a un lado de la linea para calibrar fondo
	//fondo en todos los sensores
	Sensores_Calibracion_Backgound();
	osDelay(200);

	//ROBOT GIRA PARA CALIBRAR LA LUZ EN LOS SENSORES
	Motores_Control(-30,30);
	osDelay(100);
	Sensores_Calibracion_Line();

	osDelay(2);
	LED_On_Off(1, 1);

	//Buscar set point
	Motores_Control(30,-30);

	 while(abs(vel.sensores) <= 4)
	 {
		  // Carga de valores de los sensores
		  osDelay(2);
	 }

	 LED_On_Off(0, 0);
	 Motores_Control(0,0);

/************************************************************
// FIN CALIBRACION LISTA - TEST DE LINEA
*****************************************************************/

	 vel.suma_i  = 0;
	 vel.teta_b[9] = 0;
	 vel.teta_b[8] = 0;
	 vel.teta_b[7] = 0;
	 vel.teta_b[6] = 0;
	 vel.teta_b[5] = 0;
	 vel.teta_b[4] = 0;
	 vel.teta_b[3] = 0;
	 vel.teta_b[2] = 0;
	 vel.teta_b[1] = 0;

  	 while(Control_SW())
			{

					// Carga de valores de los sensores
  		vel.teta_b[9] = vel.teta_b[8];
  		vel.teta_b[8] = vel.teta_b[7];
  		vel.teta_b[7] = vel.teta_b[6];
  		vel.teta_b[6] = vel.teta_b[5];
  		vel.teta_b[5] = vel.teta_b[4];
  		vel.teta_b[4] = vel.teta_b[3];
  		vel.teta_b[3] = vel.teta_b[2];
  		vel.teta_b[2] = vel.teta_b[1];
  		vel.teta_b[1] = vel.teta_b[0];
  		vel.teta_b[0] = vel.sensores;

				//INTEGRAL
         if(vel.teta_b[0] == 0 )
            vel.suma_i  = 0;


			 if(vel.suma_i >= CONTROL_INT_MAX)
				 vel.suma_i = CONTROL_INT_MAX;
			 else if(vel.suma_i <= -CONTROL_INT_MAX)
				 vel.suma_i = -CONTROL_INT_MAX;
			 else
				 vel.suma_i += (vel.teta_b[0]);


			    int integral = (vel_setup.kig * vel.suma_i);
				vel.pwmd = (vel.teta_b[0]*vel_setup.kpg) +  (vel_setup.kdg*(vel.teta_b[0] - vel.teta_b[3])) + integral;

					if(vel.pwmd>20)
						vel.pwmd = 20;
					if(vel.pwmd<-20)
						vel.pwmd = -20;

					vel.pmw_d = (int) - vel.pwmd;
					vel.pmw_i = (int)  vel.pwmd;

					Motores_Control(vel.pmw_i,vel.pmw_d);

				 //Direccionales
				  if(vel.pwmd > 0)
					{
					    LED_On_Off(1, 0);
					}
					else
					{
					    LED_On_Off(0, 1);
					}


					osDelay(2);
			}


  	Motores_Control(0,0);
	LED_On_Off(0, 0);

/************************************************************
//INICIO COMPETENCIA
*****************************************************************/

//GIA PARA INDICAR QUE ESTA LIST0
	LED_On_Off(1, 1);
	    osDelay(200);
	LED_On_Off(0, 1);
		osDelay(200);
	LED_On_Off(1, 0);
		osDelay(200);
	LED_On_Off(0, 1);
		osDelay(200);
	LED_On_Off(1, 1);

	//ALISTA VALORES
	 vel.suma_i  = 0;
	 vel.teta_b[9] = 0;
	 vel.teta_b[8] = 0;
	 vel.teta_b[7] = 0;
	 vel.teta_b[6] = 0;
	 vel.teta_b[5] = 0;
	 vel.teta_b[4] = 0;
	 vel.teta_b[3] = 0;
	 vel.teta_b[2] = 0;
	 vel.teta_b[1] = 0;



	//Set_Turbina(vel.pmw_t);


	 //ARRACA!!!!!!!!!!!!!!!!!11
	while(!Control_RM())
	{
	   osDelay(1); //ESPERAR ARRANQUE REMOTO
	}


	  Turbina_Control(vel_setup.pmw_t);
	  osDelay(500);

	
	while(1){
    float pwml,pwmr;

		 //APAGADO DEL ROBOT
		 if(!Control_RM())
		 {
		  vel.pmw_d = (int) 0;
			vel.pmw_i = (int) 0;
			Motores_Control(vel.pmw_i,vel.pmw_d);
			Turbina_Control(0);

		    while(1)
				{
				     osDelay(500);
	           LED_On_Off(1, 1);
			       osDelay(500);
	           LED_On_Off(0, 0);
				}

		 }

		 vel.teta_b[19] = vel.teta_b[18];
		 vel.teta_b[18] = vel.teta_b[17];
		 vel.teta_b[17] = vel.teta_b[16];
		 vel.teta_b[16] = vel.teta_b[15];
		 vel.teta_b[15] = vel.teta_b[14];
		 vel.teta_b[14] = vel.teta_b[13];
		 vel.teta_b[13] = vel.teta_b[12];
		 vel.teta_b[12] = vel.teta_b[11];
		 vel.teta_b[11] = vel.teta_b[10];
		 vel.teta_b[10] = vel.teta_b[9];
		 vel.teta_b[9] = vel.teta_b[8];
		 vel.teta_b[8] = vel.teta_b[7];
		 vel.teta_b[7] = vel.teta_b[6];
		 vel.teta_b[6] = vel.teta_b[5];
		 vel.teta_b[5] = vel.teta_b[4];
		 vel.teta_b[4] = vel.teta_b[3];
		 vel.teta_b[3] = vel.teta_b[2];
		 vel.teta_b[2] = vel.teta_b[1];
		 vel.teta_b[1] = vel.teta_b[0];
		 vel.teta_b[0] = vel.sensores;


			vel.detect_recta_ant = vel.detect_recta;
			vel.detect_recta = 1;

			for(int i=0 ; i<20; i++)
			{
				if( vel.teta_b[i] > 8 || vel.teta_b[i] < -8)
			  {
				  vel.detect_recta = 0;
				}

			}

			if(vel.detect_recta)
			{
			   LED_On_Off(0, 0);
			}
			else //ES CURVA
			 {
				 if(vel.detect_recta_ant)
				 {
//					 vel.pmw_d = (int) -100;
//				   vel.pmw_i = (int) -100;
//					 osDelay(20);
				 }
			     LED_On_Off(1,1);
			}



		    vel.pwmd = (vel.teta_b[0]*vel_setup.kpg) +  (vel_setup.kdg*(vel.teta_b[0] - vel.teta_b[8]));

		    if(vel.pwmd >= 0)
				{//la diferencia es positiva
          pwml = vel_setup.vavg;//0 + (vel.pwmd*0.2);
			    pwmr = vel_setup.vavg - vel.pwmd;
				}
				else
				{//la diferencia es negativa
           pwml = vel_setup.vavg  + vel.pwmd;
			     pwmr = vel_setup.vavg;// - (vel.pwmd*0.2);
				}

//			    pwmr = vel_setup.vavg - vel.pwmd;
//			    pwml = vel_setup.vavg  + vel.pwmd;

				vel.pmw_d = (int) pwmr;
				vel.pmw_i = (int) pwml;

				Motores_Control(vel.pmw_i,vel.pmw_d);

	     // sprintf((char*)uart_buffer_tx,"%d",vel.sensores);
			 //	HAL_UART_Transmit_IT(&huart2, uart_buffer_tx, strlen((char*)uart_buffer_tx));


	      osDelay(1);
  }
}
