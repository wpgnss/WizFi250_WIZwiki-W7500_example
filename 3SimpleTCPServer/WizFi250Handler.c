#include "WizFi250Handler.h"
#include "W7500x_gpio.h"


#define BUFF_LEN 1024
uint8_t  g_wizfi250_resp_buff[BUFF_LEN]={0,};
uint32_t g_resp_data_len=0;

// interrupt 
#define U0RX_BUF_SIZE		1024
#define U1RX_BUF_SIZE		1024
extern uint8_t  u0rx_buf[U0RX_BUF_SIZE];
extern uint16_t u0rx_wr, u0rx_rd;

extern uint8_t  u1rx_buf[U0RX_BUF_SIZE];
extern uint16_t u1rx_wr, u1rx_rd;


void delay_ms(__IO uint32_t nCount)
{
    volatile uint32_t delay = nCount * 2500; // approximate loops per ms at 24 MHz, Debug config
    for(; delay != 0; delay--)
        __NOP();
}

int8_t UartGetc_nonblk(UART_TypeDef* UARTx)
{
			int uart;
	
	
				if(UARTx == UART0){
							uart = PF_UART0;
				}
				else if(UARTx == UART1){
							uart = PF_UART1;
				}
				
			switch(uart)
			{
							case PF_UART0:
							{
										if(u0rx_rd == u0rx_wr) return RET_NOK;
										if(u0rx_rd < U0RX_BUF_SIZE-1)
										{

														u0rx_rd++;
														return u0rx_buf[u0rx_rd-1];
										}
										else
										{

														u0rx_rd = 0;
														return u0rx_buf[U0RX_BUF_SIZE-1];
										}
										break;
							}
						

							case PF_UART1:
							{	
											if(u1rx_rd == u1rx_wr) return RET_NOK;
											if(u1rx_rd < U1RX_BUF_SIZE-1)
											{

														u1rx_rd++;
														return u1rx_buf[u1rx_rd-1];
											}
											else
											{

														u1rx_rd = 0;
											return u1rx_buf[U1RX_BUF_SIZE-1];
											}				
											break;
							}
							

							default:
							{
												return RET_NOK;
							}
			}

			return RET_NOK;
}



uint8_t send_and_check_command(char* send_command, uint8_t cr_lf, uint32_t check_delay, uint32_t check_count, char* str_find1, char* str_find2, uint8_t debug_print)
{
				uint32_t i;
				uint8_t is_found1=0, is_found2=0;
				uint32_t nResult = 0;
				int8_t ch;
				char crlf_string[3];

				g_resp_data_len = 0;

				// Send Command
				UartPuts(UART0,(uint8_t*)send_command);

				
				// Send CR, LF
				if ( cr_lf == 1 )		strcpy(crlf_string, "\r");
				else if ( cr_lf == 2 )	strcpy(crlf_string, "\n");
				else if ( cr_lf==3 )	strcpy(crlf_string, "\r\n");
				if ( cr_lf==1 || cr_lf==2 || cr_lf==3 )
				{
						UartPuts(UART0,(uint8_t*)crlf_string);		
				}
		
				if ( debug_print==1 )	printf("\r\nDBG>>> Send : %s\r\n", send_command);
				// Recv Response
				if ( str_find1==0 ) is_found1 = 1;
				if ( str_find2==0 ) is_found2 = 1;
				printf("\r\nDBG>>> Resp Recv \r\n");
				

				
				for(i=0;i<check_count;i++)
				{
								do{
							
											ch = UartGetc_nonblk(UART0);
											if(ch != RET_NOK)
															g_wizfi250_resp_buff[g_resp_data_len++] = (uint8_t)ch;
											if(ch == '\n')
															break;

									}while(ch != RET_NOK);

									g_wizfi250_resp_buff[g_resp_data_len]='\0';
							
									if ( is_found1==0 )
									{
													if ( strstr((char*)g_wizfi250_resp_buff, str_find1) ) is_found1 = 1;
									}
									if ( is_found2==0 )
									{
													if ( strstr((char*)g_wizfi250_resp_buff, str_find2) ) is_found2 = 1;
									}
									
									if ( debug_print==1 )	printf("%s", g_wizfi250_resp_buff);
									
									
									g_resp_data_len = 0;

									if( (is_found1 == 1) && (is_found2 == 1) ) break;

									delay_ms(check_delay);
				}


				// Error : Timeout or Not Found Success String
				if ( !(is_found1 && is_found2) )
				{
								printf("DBG>>> Error : Timeout or Not Found Success String\r\n");
								nResult = 2;
								return nResult;
				}


				return nResult;
}


void WizFi250_Restart(void)
{

					GPIO_SetBits(GPIOC, GPIO_Pin_1);		// HIGH
					delay_ms(10);
					GPIO_ResetBits(GPIOC, GPIO_Pin_1);		// LOW
					delay_ms(10);
					GPIO_SetBits(GPIOC, GPIO_Pin_1);		// HIGH

					delay_ms(1000);
}

void WizFi250_FactoryReset (void)
{
//					int i;
//					GPIO_SetBits(GPIOC, GPIO_Pin_10);	// HIGH
//					delay_ms(200);

//	for(i=0;i<3;i++)
//	{
//					GPIO_ResetBits(GPIOC, GPIO_Pin_10);// LOW
//					delay_ms(200);
//					GPIO_SetBits(GPIOC, GPIO_Pin_10);// HIGH
//					delay_ms(200);
//	}
}

