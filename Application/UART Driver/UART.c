#include "UART.h"
#include "UART_Cnfg.h"
#include "M4MemMap.h"
#include <stdint.h>
typedef volatile uint32_t* const UART_RegAddType;
#define UART_MODULE_NUMBER 8u
#define PORTS_NUMBER 6u
/*Register memory map*/
#define UART0_BASE_ADDRESS 0x4000C000
#define UART1_BASE_ADDRESS 0x4000D000
#define UART2_BASE_ADDRESS 0x4000E000
#define UART3_BASE_ADDRESS 0x4000F000
#define UART4_BASE_ADDRESS 0x40010000
#define UART5_BASE_ADDRESS 0x40011000
#define UART6_BASE_ADDRESS 0x40012000
#define UART7_BASE_ADDRESS 0x40013000
static const uint32_t UARTsBaseAddressLut[UART_MODULE_NUMBER] =
{   UART0_BASE_ADDRESS,
    UART1_BASE_ADDRESS,
    UART2_BASE_ADDRESS,
    UART3_BASE_ADDRESS,
    UART4_BASE_ADDRESS,
    UART5_BASE_ADDRESS,
    UART6_BASE_ADDRESS,
    UART7_BASE_ADDRESS
};
/*internal symbpls*/
#define WORD_LENGTH_POS     0x9F
#define FIFO_STATUS_POS     0xEF
#define STOP_BITS_POS       0xF7
#define PARITY_MODE_POS     0xFB
#define PARITY_ENABLE_POS   0xFD


#define UART_REG_ADDRESS(CHANNEL_ID,REG_OFFSET) (UARTsBaseAddressLut[CHANNEL_ID] + REG_OFFSET)

/*UART data Register */
#define UART_DATA_REG(Module_ID)           (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x000))))


/*UART Baud Rate Register control*/
#define UART_INTEGER_BR(Module_ID)      *((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x024)))
#define UART_FRACTION_BR(Module_ID)     *((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x028)))

/*UART Data Frame Parameters (line ctrl reg)*/
#define UART_WORD_LENGTH(DATA,Module_ID)          (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))=\
                                               ((*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))&((WORD_LENGTH_POS)))+(DATA<<5)

#define UART_FIFO_STATUS(DATA,Module_ID)          (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))=\
                                               ((*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))&((FIFO_STATUS_POS)))+(DATA<<4)

#define UART_STOP_BITS(DATA,Module_ID)            (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))=\
                                               ((*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))&((STOP_BITS_POS)))+(DATA<<3)

#define UART_PARITY_MODE(DATA,Module_ID)          (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))=\
                                               ((*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))&((PARITY_MODE_POS)))+(DATA<<2)

#define UART_PARITY_ENABLE(DATA,Module_ID)        (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))=\
                                               ((*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x02C))))&((PARITY_ENABLE_POS)))+(DATA<<1)

/*UART Module Control (control reg)*/
#define UART_ENABLE(Module_ID)               (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x030))))

/*UART flag Register*/
#define UART_FLAG_REG(Module_ID)           (*((UART_RegAddType)(UART_REG_ADDRESS(Module_ID,0x018))))
#define UART_FR_TXFF(Module_ID)            ((UART_FLAG_REG(Module_ID)) & (0x00000020))   /* UART Transmit FIFO Full */
#define UART_FR_RXFE(Module_ID)            ((UART_FLAG_REG(Module_ID)) & (0x00000010))   /* UART Receive FIFO Empty */

static uint8_t UART_GroupState[UART_GROUPS_NUMBER] = {0};

/*A function to initialize all the module Groups in the configurations*/
UART_CheckType UART_Init(void)
{
    uint8_t LoopIndex;
    uint8_t ErrorFlag = 0;
    UART_CheckType RetVar;
    const UART_CfgType * CfgPtr;


    for(LoopIndex = 0; (LoopIndex < UART_GROUPS_NUMBER) && (ErrorFlag == 0); LoopIndex ++)
    {
        if((UART_ConfigParam[LoopIndex].UART_Module_Id < UART_MODULE_NUMBER)&&\
           (UART_ConfigParam[LoopIndex].UART_Port_Id < PORTS_NUMBER))
                {
                    CfgPtr = & UART_ConfigParam[LoopIndex];
                    /*enable clock for UART module */
                    RCGCUART_REG |= 1 << CfgPtr->UART_Module_Id;
                    /*enable clock for Port */
                    RCGCGPIO_REG |= 1 << CfgPtr->UART_Port_Id;
                    /*disable UART module */
                    UART_ENABLE(CfgPtr->UART_Module_Id) &=~ 0;
                    /*Baud rate integer & fraction part*/
                    UART_INTEGER_BR(CfgPtr->UART_Module_Id)= CfgPtr->BaudRate_integer ;
                    UART_FRACTION_BR(CfgPtr->UART_Module_Id)= CfgPtr->BaudRate_fraction;
                    /*UART Data Frame Parameters (line control reg)*/
                    UART_WORD_LENGTH(CfgPtr->Word_length,CfgPtr->UART_Module_Id) ;
                    UART_FIFO_STATUS(CfgPtr->FIFO_Status,CfgPtr->UART_Module_Id);
                    UART_STOP_BITS(CfgPtr->Stop_bits,CfgPtr->UART_Module_Id);
                    UART_PARITY_ENABLE(CfgPtr->Parity_enable,CfgPtr->UART_Module_Id);
                    UART_PARITY_MODE(CfgPtr->Parity_mode,CfgPtr->UART_Module_Id) ;
                    /*enable UART module */
                    UART_ENABLE(CfgPtr->UART_Module_Id) |= 1;
                    /***************************************************/
                    UART_GroupState[LoopIndex] = 1;  // indication that this group has been initialized
                    RetVar = UART_OK;
                }
        else
                {
                    /*Invalid GroupId OR PortId*/
                    RetVar = UART_NOK;
                    ErrorFlag = 1;
                }
    }
        return RetVar;
    }

/*A function to send data through a specific group*/
UART_CheckType UART_Send(uint8_t GroupId,uint8_t GroupData){

    const UART_CfgType * CfgPtr;
    UART_CheckType RetVar;

    if (( GroupId <UART_GROUPS_NUMBER))
    {

        CfgPtr =  & UART_ConfigParam[GroupId];

        if (UART_GroupState[GroupId]==1 )
        {
            while(UART_FR_TXFF(CfgPtr->UART_Module_Id) != 0){}
            UART_DATA_REG(CfgPtr->UART_Module_Id) = GroupData ;
            RetVar = UART_OK;
        }
        else {RetVar = UART_NOK ; }
            }
    else {RetVar = UART_NOK ; }

return RetVar;}

/*A function to receive data from a specific group*/
UART_CheckType UART_Receive(uint8_t GroupId,uint8_t* GroupDataPtr)
{
        const UART_CfgType * CfgPtr;
        UART_CheckType RetVar;

        if (( GroupId <UART_GROUPS_NUMBER))
        {

            CfgPtr =  & UART_ConfigParam[GroupId];

            if (UART_GroupState[GroupId]==1)
            {
                while(UART_FR_RXFE(CfgPtr->UART_Module_Id) != 0){}
                *GroupDataPtr = UART_DATA_REG(CfgPtr->UART_Module_Id);
                RetVar = UART_OK;
            }
            else {RetVar = UART_NOK ; }
                }
        else {RetVar = UART_NOK ; }

    return RetVar;
}


void UART_Send_String(uint8_t GroupId,uint8_t *GroupDataPtr)
{
    uint8_t * data=GroupDataPtr;
    while(*data!='\0')
    {
        UART_Send(GroupId, *data);
        data++;
    }
}

void UART_Send_Integer(uint8_t GroupId,uint32_t GroupData)
{
    int8_t c[10] = {0} ;
    uint8_t m;
    int32_t i = 0 ;

    if(GroupData == 0)
    {
        c[0] = 48;
        i = 0 ;
    }
    while(GroupData)
    {
        m = (char)(GroupData%10);
        c[i++] = m + 48;
        GroupData = GroupData/10;
    }
    //c[i] = 48 ;
    while(i != -1 )
    {
        UART_Send(GroupId,c[i--]);
    }

}
void UART_Print_New_Line(uint8_t GroupId)
{
    UART_Send_String(GroupId, "\r\n");
}


