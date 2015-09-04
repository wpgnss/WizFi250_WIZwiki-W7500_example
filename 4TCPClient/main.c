/**
  ******************************************************************************
  * @file    EX4_Get Weather Information
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
/* Private define ------------------------------------------------------------*/
		//Wi-Fi information
#define SSID 		"3PA-W"
#define PASSWORD	"wiznet218"

		//Weather Server information
#define SERVER_IP  "128.199.103.33"
#define SERVER_PORT	"80"
#define WEATHER_QUERY  "GET /data/2.5/weather?q=seoul,kor HTTP/1.1\r\nHost: api.openweathermap.org\r\n\r\n"

#define ENDCHAR1 0x30    //'0'
#define ENDCHAR2 0x0d   //'\r'
#define ENDCHAR3 0x0a	//'\n'

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int8_t is_associated = RET_NOK;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int8_t WizFi250_Associate();
int8_t WizFi250_GetWeather();

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
						uart_recv = UartGetc_nonblk(UART0);
						if(uart_recv == RET_NOK)  break;
						UartPutc(UART1,uart_recv);
		}

		while(1)
		{
	
						printf("\r\n\r\n//////////////////////////////////////////////////////\r\n");
						printf("// 1 : Association\r\n");
						printf("//////////////////////////////////////////////////////\r\n");
						printf("// 2 : Get weather\r\n");
						printf("//////////////////////////////////////////////////////\r\n");
						printf("// 3 : Dis-association\r\n");
						printf("//////////////////////////////////////////////////////\r\n");
						
						while(1)
						{
									uart_menu = UartGetc_nonblk(UART1);
									if ( uart_menu != RET_NOK )	break;
						}
												
						if(uart_menu == '1')
						{
									is_associated = WizFi250_Associate();
						}
						else if(uart_menu == '2')
						{
									if(is_associated == RET_OK)
													WizFi250_GetWeather();
									else
													printf("\r\nDBG>>> Error : Not associated\r\n");
						}
						else if(uart_menu == '3')
						{
									if ( send_and_check_command("AT+WLEAVE", 1, 500,  10, "[OK]", "", 1) !=0 )	{ printf("DBG>>> Error : AT Command\r\n"); }
									else is_associated = RET_NOK;
						}

			}
}

int8_t WizFi250_Associate()
{
			char cmd[256];
		
					if ( send_and_check_command("AT+WLEAVE", 1, 500,  10, "[OK]", "", 1) !=0 )	{ printf("DBG>>> Error : AT Command\r\n"); }
					if ( send_and_check_command("AT+WNET=1", 1, 500,  10, "[OK]", "", 1) !=0 )	{ printf("DBG>>> Error : AT Command\r\n"); }
					sprintf(cmd,"AT+WSET=0,%s",SSID);
					if ( send_and_check_command(cmd, 1, 500,  10, "[OK]1", "", 1) !=0 )		{ printf("DBG>>> Error : AT Command\r\n"); }
					sprintf(cmd,"AT+WSEC=0,,%s",PASSWORD);
					if ( send_and_check_command(cmd, 1, 500,  10, "[OK]", "", 1) !=0 )		{ printf("DBG>>> Error : AT Command\r\n"); }
					if ( send_and_check_command("AT+WJOIN", 1, 500,  100, "[OK]", "", 1) !=0 )	{ printf("DBG>>> Error : AT Command\r\n"); return RET_NOK; }
					
					return RET_OK;
	
}

int8_t WizFi250_GetWeather()
{
			char cmd[256];
			int8_t is_connected = 0;
			int8_t uart_recv;
	
			uint8_t end_step1 = 0;
			uint8_t end_step2 = 0;
			
					while(1)
					{
											if(is_connected == 0)
											{			
																sprintf(cmd,"AT+SCON=O,TCN,%s,%s,,0",SERVER_IP,SERVER_PORT);
																if ( send_and_check_command(cmd, 1, 500,  10, "[OK]", "[CONNECT", 1) !=0 )			{ printf("DBG>>> Error : AT Command\r\n"); break; }
																is_connected = 1;
																UartPuts(UART1,(uint8_t*)"Send Query\r\n");
															
																sprintf(cmd,"AT+SSEND=0,,,%d",strlen(WEATHER_QUERY));
																if ( send_and_check_command(cmd, 1, 500,  10, "", "", 1) !=0 )			{ printf("DBG>>> Error : AT Command\r\n"); break; }
																
																UartPuts(UART0, WEATHER_QUERY);
											}
											else
											{
																while(1)
																{
																				if( (uart_recv = UartGetc_nonblk(UART0)) == RET_NOK )   continue;

																				/* Check End String */
																				if(uart_recv == ENDCHAR1)	end_step1++;
																				
																				else if(uart_recv == ENDCHAR2 && end_step1 == 1) end_step2++;
																				else if(uart_recv == ENDCHAR2 && end_step1 != 1) end_step1 = 0;
																				else if(uart_recv != ENDCHAR2 && end_step1 == 1) end_step1 = 0;
																	
																				else if(uart_recv == ENDCHAR3 && end_step2 == 1)	break;
																				else if(uart_recv == ENDCHAR3 && end_step2 != 1) { end_step1 = 0; end_step2 = 0; }
																				else if(uart_recv != ENDCHAR3 && end_step2 == 1) { end_step1 = 0; end_step2 = 0; }
																				/* Check End String */

																				UartPutc(UART1,uart_recv);
																}
															
																if ( send_and_check_command("AT+SMGMT=ALL", 1, 500,  10, "[OK]", "", 1) !=0 )			{ printf("DBG>>> Error : AT Command\r\n"); }
															
																return RET_OK;
											}				
						}
						return RET_NOK;
	}	

