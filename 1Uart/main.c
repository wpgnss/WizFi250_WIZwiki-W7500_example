/**
  ******************************************************************************
  * @file    EX1_UART Test
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

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main()
{
		int8_t ch;
	
    /* System clock configuration */
		SystemInit();

    /* UART1 configuration */
    UART_StructInit(&UART_InitStructure);
    /* Configure UART0, UART1 */
    UART_Init(UART1,&UART_InitStructure);
  
	/* Configure Interrupt Enable */
		UART_ITConfig(UART1,UART_IT_FLAG_RXI,ENABLE);
    
	/* NVIC configuration */
		NVIC_ClearPendingIRQ(UART1_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);

		/* Uart Test */
		UartPuts(UART1,(uint8_t*)"\r\nUART Test Example\r\n\r\n");
		
		while(1)
		{
						ch = UartGetc_nonblk(UART1);
							if(ch != RET_NOK) 
												UartPutc(UART1,ch);
		}
		
}



