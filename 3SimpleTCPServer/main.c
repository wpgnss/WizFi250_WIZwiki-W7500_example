/**
  ******************************************************************************
  * @file    EX3_Simple TCP Server
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
#define SSID 		"WizFiDemoAP"
#define PASSWORD	"12345678"
#define SERVER_PORT	"5000"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int8_t WizFi250_SimpleTCPServer();


/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main()
{
    int8_t uart_recv;
    int8_t uart_menu;

    /*System clock configuration*/
		SystemInit();
		
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
    PAD_AFConfig(PAD_PC,GPIO_Pin_1, PAD_AF1); // PAD Config - LED used 2nd Function
	
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
						printf("// 1 : Simple TCP Server\r\n");
						printf("//////////////////////////////////////////////////////\r\n\r\n");
						while(1)
						{
									uart_menu = UartGetc_nonblk(UART1);
									if ( uart_menu != RET_NOK )	break;
						}
						
						if(uart_menu == '1')
						{
									WizFi250_SimpleTCPServer();
						}			
		}
}

int8_t WizFi250_SimpleTCPServer()
{
			
			char cmd[256];
			char recv_data[256];
			int32_t idx=0,cr_lf_check=0;
			int8_t uart_recv;

			
			if ( send_and_check_command("AT+WLEAVE", 1, 500,  10, "[OK]", "", 1) !=0 )	{ printf("DBG>>> Error : AT Command\r\n");}
			if ( send_and_check_command("AT+WNET=1", 1, 500,  10, "[OK]", "", 1) !=0 )	{ printf("DBG>>> Error : AT Command\r\n");}


			sprintf(cmd,"AT+WSET=0,%s",SSID);
			if ( send_and_check_command(cmd, 1, 500,  10, "[OK]", "", 1) !=0 )			{ printf("DBG>>> Error : AT Command\r\n");}
			sprintf(cmd,"AT+WSEC=0,,%s",PASSWORD);
			if ( send_and_check_command(cmd, 1, 500,  10, "[OK]", "", 1) !=0 )			{ printf("DBG>>> Error : AT Command\r\n");}
			if ( send_and_check_command("AT+WJOIN", 1, 500,  100, "[OK]", "", 1) !=0 )	{ printf("DBG>>> Error : AT Command\r\n");}

			sprintf(cmd,"AT+SCON=O,TSN,,,%s,1",SERVER_PORT);
			if ( send_and_check_command(cmd, 1, 500,  100, "[OK]", "", 1) !=0 )			{ printf("DBG>>> Error : AT Command\r\n"); return RET_NOK;}


			printf("DBG>>> Recv Data\r\n");
			while(1)
			{
						uart_recv = UartGetc_nonblk(UART0);
						if(uart_recv != RET_NOK)
									UartPutc(UART1,uart_recv);
					
						uart_recv = UartGetc_nonblk(UART1);
						if(uart_recv != RET_NOK){
										if(uart_recv == 0x1b)
										{
																UartPuts(UART0,"+++");
																delay_ms(1000);
																while(1)
																{
																				uart_recv = UartGetc_nonblk(UART0);
																				if(uart_recv == RET_NOK)  break;
																				UartPutc(UART1,uart_recv);
																}
																UartPutc(UART0,'\r');
																return RET_OK;
										}
										UartPutc(UART0,uart_recv);	
						}
			}
	
}



