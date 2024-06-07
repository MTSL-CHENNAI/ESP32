#ifndef _UART_H
#define _UART_H


/********************Macros********************/
#define RXpin 10
#define TXpin 9
#define BAUDRATE9600 9600
#define BAUDRATE115200 115200
#define SER_BUFF  1024

/********************Functions Declaration********************/
void UART1_Init(void);
void UART1_Tx(String str);
void UART1_Rx_Buff(String str);
void UART1_Rx(void);
#endif