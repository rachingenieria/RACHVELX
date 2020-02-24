#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash_ex.h"
#include <stdlib.h>

#include "VELOCISTA.h"

extern velocista_setup_st vel_setup;


static uint32_t readFlash(int offset);
static void writeFlash(int mSize, uint32_t* Data);
static uint32_t readFlash(int offset);
static  void Erase_Page_Flash(uint32_t Address);

/************************************************************
//FUNCIONES DE USUARIO PARA GUARDAR EN LA FLASH
*****************************************************************/

void MC_Load_Setup(void)
{
    uint32_t* p_lm;
	p_lm = (uint32_t*)&vel_setup;
	for(int ind = 0; ind < sizeof(vel_setup)/4 ; ind ++)
	{
	    *p_lm = readFlash(ind);
		   p_lm++;
	}



	if((vel_setup.version & 0xFF00) == 0x0A00) // Valid Version
	 {
      	;//
	 }
	 else // DEFAULT_VALUES
   {
	    	VEL_Load_Default_Setup();
		 	  writeFlash(sizeof(vel_setup)/4, (uint32_t*) &vel_setup); // SAVE DATA
	 }
 }

 void MC_Save_Setup(void)
{
				uint32_t* p_lm;

	      p_lm = (uint32_t*)&vel_setup;
		 	  writeFlash(sizeof(vel_setup)/4, p_lm); // SAVE DATA

 }

 volatile int sise_stup;
void MC_Write_Setup(void)
{
	sise_stup = sizeof(vel_setup)/4;

				 writeFlash(sise_stup, (uint32_t*) &vel_setup); // SAVE DATA
 }




uint32_t startAddress = 0x8007000;//starting from 256KB
                                  //0x8020000 starting 128KB
/************************************************************
//FUNCIONES GENERALES DE GUARDAR EN LA FLASH
*****************************************************************/

extern void    FLASH_PageErase(uint32_t PageAddress);

void writeFlash(int mSize, uint32_t* Data)
{
    int i;
	  uint64_t* local_data;


		Erase_Page_Flash(startAddress);

	HAL_FLASH_Unlock();//unlock flash writing
__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );


    local_data = (uint64_t*)Data;
    for(i=0; i < mSize/2 ; i++)
	  {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, startAddress + (i*8), *local_data);
			      local_data++;

    }
    HAL_FLASH_Lock();//lock the flash for writing

}

uint32_t readFlash(int offset)
{
    uint32_t data;
            data = *(uint32_t *)(startAddress + (offset)*4);
	return data;
}

void Erase_Page_Flash(uint32_t Address)
{
  HAL_FLASH_Unlock();//unlock flash writing
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );
    //FLASH_ErasePage(startAddress);//erase the entire page before you can write as I //mentioned

				FLASH->CR |= FLASH_CR_PER; /* (1) */
				FLASH->AR = Address; /* (2) */
				FLASH->CR |= FLASH_CR_STRT; /* (3) */
				while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (4) */
				{
				 /* For robust implementation, add here time-out management */
				}
				if ((FLASH->SR & FLASH_SR_EOP) != 0) /* (5) */
				{
				 FLASH->SR = FLASH_SR_EOP; /* (6)*/
				}
				else
				{
				 /* Manage the error cases */
				}
				FLASH->CR &= ~FLASH_CR_PER; /* (7) */
				HAL_FLASH_Lock();//lock the flash for writing
		}
