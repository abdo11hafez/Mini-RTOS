#ifndef UART_H_
#define UART_H_
#include <stdint.h>
#include "UART_Cnfg.h"

typedef struct
{
uint8_t UART_Module_Id;  //UART module 0:7
uint8_t UART_Port_Id ;   //port associated with this module 0:5

/*Baud Rate integer & fraction*/
uint8_t BaudRate_integer;
uint8_t BaudRate_fraction;
/*Data frame parameters*/
uint8_t Word_length;    // 0 -> 5 bits       1 -> 6 bits    2-> 7 bits     3-> 8 bits
uint8_t FIFO_Status ;   // 0 -> disabled     1 -> enabled
uint8_t Stop_bits ;     // 0 -> 1 stop bit   1 -> 2 stop bits
uint8_t Parity_mode ;   // 0 -> odd parity   1 -> even parity
uint8_t Parity_enable ; // 0 -> disabled     1 -> enabled

}UART_CfgType;

typedef enum {UART_OK =0, UART_NOK} UART_CheckType;

extern const UART_CfgType UART_ConfigParam [UART_GROUPS_NUMBER];

/*A function to initialize all the UART Groups in the configurations*/
UART_CheckType UART_Init(void);
/*A function to send data through a specific group*/
UART_CheckType UART_Send(uint8_t GroupId,uint8_t GroupData);
/*A function to receive data from a specific group*/
UART_CheckType UART_Receive(uint8_t GroupId,uint8_t* GroupDataPtr);
void UART_Send_String(uint8_t GroupId,uint8_t *GroupDataPtr);

#endif /* UART_H_ */
