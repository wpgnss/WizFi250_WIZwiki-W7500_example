/**
  ******************************************************************************
  * @file    EX2_ATCommandBypass
  * @author  V6 Team
  * @version V1.0.0
  * @date    10-Jun-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WIZnet SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 WIZnet Co.,Ltd.</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "W7500x.h"
#include "WizFi250Handler.h"

/* Private typedef -----------------------------------------------------------*/
UART_InitTypeDef UART_InitStructure;
GPIO_InitTypeDef GPIO_InitDef;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int8_t WizFi250_BypassMode();


/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main()
{
    int8_t uart_menu;
		int8_t uart_recv;
	
    /*System clock configuration*/
		SystemInit();
		S_UART_Init(115200);
    /* UART0 and UART1 configuration*/
    UART_StructInit(&UART_InitStructure);
    /* Configure UART0, UART1 */
    UART_Init(UART0,&UART_InitStructure);
    UART_Init(UART1,&UART_InitStructure);
  
	/* Configure Interrupt Enable*/
    UART_ITConfig(UART0, UART_IT_FLAG_RXI,ENABLE);
		UART_ITConfig(UART1,UART_IT_FLAG_RXI,ENABLE);
    
	/* NVIC configuration */
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);
	
		NVIC_ClearPendingIRQ(UART1_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);

	/* GPIO Set  */
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_1; // Set to Pin_1 (D2)
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
    GPIO_Init(GPIOC, &GPIO_InitDef);
    PAD_AFConfig(PAD_PC,GPIO_Pin_1, PAD_AF1); // PAD Config - 2nd Function
	
	
	
	/* Restart WizFi250 */
		printf("Restarting WizFi250...\n");
		WizFi250_Restart();
		
			while(1)
			{
						if((uart_recv = UartGetc_nonblk(UART0)) == RET_NOK)
										break;
						UartPutc(UART1,uart_recv);
			}		

		while(1)
		{
	
						printf("\r\n\r\n//////////////////////////////////////////////////////\r\n");
						printf("// 1 : Bypass mode\r\n");
						printf("//////////////////////////////////////////////////////\r\n");

						while(1)
						{
									uart_menu = UartGetc_nonblk(UART1);
									if ( uart_menu != RET_NOK )	break;
						}
						
						
						if( uart_menu == '1')
						{
									WizFi250_BypassMode();
						}
					
			}
}


int8_t WizFi250_BypassMode()
{
	int8_t uart_recv;
	printf("\r\nDBG>> Starting Bypass mode.....\r\n");
	printf("\r\nDBG>> If you want to exit this mode, press ESC key\r\n\r\n");
	
			while(1)
			{
						uart_recv = UartGetc_nonblk(UART0);
						if(uart_recv != RET_NOK)
									UartPutc(UART1,uart_recv);
						
						
						uart_recv = UartGetc_nonblk(UART1);
						if(uart_recv != RET_NOK)
						{
									if(uart_recv == 0x1b)		break;
							
									UartPutc(UART0,uart_recv);
						}
			}

	return RET_OK;
}



