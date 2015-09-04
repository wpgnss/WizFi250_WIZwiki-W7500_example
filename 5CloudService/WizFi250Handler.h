#ifndef WIZFI250HANDLER_H_
#define WIZFI250HANDLER_H_

#include "W7500x_uart.h"

#define RET_FAIL	1
#define RET_OK		0
#define RET_NOK		-1

typedef enum{
	PF_UART0=0,
	PF_UART1
}platform_uart;


void delay_ms(__IO uint32_t nCount);
int8_t UartGetc_nonblk(UART_TypeDef* UARTxt);
uint8_t send_and_check_command(char* send_command, uint8_t cr_lf, uint32_t check_delay, uint32_t check_count, char* str_find1, char* str_find2, uint8_t debug_print);
void WizFi250_Restart(void);
void WizFi250_FactoryReset (void);


#endif /* WIZFI250HANDLER_H_ */
