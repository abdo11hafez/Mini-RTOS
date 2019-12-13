#include "GPIO.h"
#include "GPIO_Cfg.h"
#include "M4MemMap.h"
#include <stdint.h>
typedef volatile uint32_t* const GPIO_RegAddType;
#define PORTS_NUMBER 6u
/*Register memory map*/
#define PORTA_BASE_ADDRESS 0x40004000
#define PORTB_BASE_ADDRESS 0x40005000
#define PORTC_BASE_ADDRESS 0x40006000
#define PORTD_BASE_ADDRESS 0x40007000
#define PORTE_BASE_ADDRESS 0x40024000
#define PORTF_BASE_ADDRESS 0x40025000
static const uint32_t PortsBaseAddressLut[PORTS_NUMBER] =
{       PORTA_BASE_ADDRESS,
	PORTB_BASE_ADDRESS,
	PORTC_BASE_ADDRESS,
	PORTD_BASE_ADDRESS,
	PORTE_BASE_ADDRESS,
	PORTF_BASE_ADDRESS
};
#define GPIO_REG_ADDRESS(CHANNEL_ID,REG_OFFSET) (PortsBaseAddressLut[CHANNEL_ID] + REG_OFFSET)

/*Port Control*/
#define GPIODATA_WRITE(DATA,MASK,PORT_ID)  *((GPIO_RegAddType)(GPIO_REG_ADDRESS(PORT_ID,0x000) + (MASK << 2))) = DATA
#define GPIODATA_READ(MASK,PORT_ID)        *((GPIO_RegAddType)(GPIO_REG_ADDRESS(PORT_ID,0x000) + (MASK << 2)))
#define GPIODIR_REG(PORT_ID)               *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x400))

/*Interrupt Control*/
#define GPIOIS_REG(PORT_ID)               *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x404))
#define GPIOIBE_REG(PORT_ID)              *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x408))
#define GPIOIEV_REG(PORT_ID)              *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x40C))
#define GPIOIM_REG(PORT_ID)               *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x410))
#define GPIORIS_REG(PORT_ID)              *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x414))
#define GPIOMIS_REG(PORT_ID)              *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x418))
#define GPIOICR_REG(PORT_ID)              *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x41C))

/*Mode Control*/
#define GPIOAFSEL_REG(PORT_ID)            *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x420))
#define GPIOPCTL_REG(PORT_ID)             *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x52C))
#define GPIOADCCTL_REG(PORT_ID)           *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x530))
#define GPIODMACTL_REG(PORT_ID)           *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x534))

/*Pad control*/
#define GPIODR2R_REG(PORT_ID)           *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x500))
#define GPIODR4R_REG(PORT_ID)           *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x504))
#define GPIODR8R_REG(PORT_ID)           *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x508))
#define GPIOODR_REG(PORT_ID)            *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x50C))
#define GPIOPUR_REG(PORT_ID)            *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x510))
#define GPIOPDR_REG(PORT_ID)            *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x514))
#define GPIOSLR_REG(PORT_ID)            *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x518))
#define GPIODEN_REG(PORT_ID)            *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x51C))
#define GPIOAMSEL_REG(PORT_ID)          *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x528))

/*Commit control*/
#define GPIOLOCK_REG(PORT_ID)          *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x520))
#define GPIOCR_REG(PORT_ID)            *((GPIO_RegAddType)GPIO_REG_ADDRESS(PORT_ID,0x524))

/*Internal symbols*/
#define GPIO_PORT_UNLOCK_VALUE 0x4C4F434B
#define GPIO_INT_SENCE_BIT_NUM 1
#define GPIO_INT_SENCE_MASK (1 << GPIO_INT_EVENT_BIT_NUM)
#define GPIO_INT_EVENT_BIT_NUM 0
#define GPIO_INT_EVENT_MASK (1 << GPIO_INT_EVENT_BIT_NUM)
#define GPIO_INT_BE_BIT_NUM 2
#define GPIO_INT_BR_MASK (1 << GPIO_INT_BE_BIT_NUM)

static uint8_t GPIO_GroupState[GPIO_GROUPS_NUMBER] = {0};

/*A function to initialize all the GPIO Groups in the configurations*/
GPIO_CheckType GPIO_Init(void)
{
	uint8_t LoopIndex;
	uint8_t ErrorFlag = 0;
	GPIO_CheckType RetVar;
	const GPIO_CfgType * CfgPtr;

	for(LoopIndex = 0; (LoopIndex < GPIO_GROUPS_NUMBER) && (ErrorFlag == 0); LoopIndex ++)
	{
		if(GPIO_ConfigParam[LoopIndex].PortId < PORTS_NUMBER)
		{
			/*Enable port clock gate*/
			CfgPtr = & GPIO_ConfigParam[LoopIndex];
			RCGCGPIO_REG |= 1 << CfgPtr->PortId;
			/*Unlock the group*/
			GPIOLOCK_REG(CfgPtr->PortId) = GPIO_PORT_UNLOCK_VALUE;
			GPIOCR_REG(CfgPtr->PortId)  |= (CfgPtr->PortMask);
			/*Data Control*/
			GPIODIR_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->PortDirection);
			/*Pad Control*/
			GPIODR2R_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->Use2mACrt);
			GPIODR4R_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->Use4mACrt);
			GPIODR8R_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->Use8mACrt);

			GPIOPDR_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->UsePullDown);
			GPIOPUR_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->UsePullUp);
			GPIOODR_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->UseOpenDrain);

			GPIODEN_REG(CfgPtr->PortId)   |= (CfgPtr->PortMask & CfgPtr->SetPinType);
			GPIOAMSEL_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & ~CfgPtr->SetPinType);

			/*Mode control*/
			GPIOAFSEL_REG(CfgPtr->PortId)  |= (CfgPtr->PortMask & CfgPtr->UseAlterFun);
			GPIOADCCTL_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->UseACDTrig);
			GPIODMACTL_REG(CfgPtr->PortId) |= (CfgPtr->PortMask & CfgPtr->UseDMATrig);
			GPIO_GroupState[LoopIndex] = 1;  // indication that this group has been initialized
			RetVar = GPIO_OK;
		}
		else
		{
			/*Invalid GroupId*/
			RetVar = GPIO_NOK;
			ErrorFlag = 1;
		}


	}
	return RetVar;
}


/*A function to Digital Write data to a specific group*/
GPIO_CheckType GPIO_Write(uint8_t GroupId,uint8_t GroupData)
{
	const GPIO_CfgType * CfgWrite;
	GPIO_CheckType RetVar;

	if (( GroupId <GPIO_GROUPS_NUMBER))
	{
		CfgWrite =  & GPIO_ConfigParam[GroupId];
		if ((CfgWrite->PortDirection == 0xff)&&(CfgWrite->SetPinType == 0xff)&&(CfgWrite->UseAlterFun==0x00) &&(GPIO_GroupState[GroupId]==1 ))
		{
		GPIODATA_WRITE(GroupData,CfgWrite->PortMask,CfgWrite->PortId);
		RetVar = GPIO_OK ;
	  	}
		else {RetVar = GPIO_NOK ; }
	}
	else 
	{
		RetVar = GPIO_NOK ; 
	}
	return RetVar;
}

/*A function to Digital read data from a specific group*/
GPIO_CheckType GPIO_Read(uint8_t GroupId,uint8_t* GroupDataPtr)
{   const GPIO_CfgType * Cfgread;
    GPIO_CheckType RetVar;
    Cfgread =  & GPIO_ConfigParam[GroupId];

   if (( GroupId <GPIO_GROUPS_NUMBER))
      {
       Cfgread =  & GPIO_ConfigParam[GroupId];
       if ((Cfgread->PortDirection == 0x00)&&(Cfgread->SetPinType == 0xff)&&(Cfgread->UseAlterFun==0x00) &&(GPIO_GroupState[GroupId]==1 ))
          {
           *GroupDataPtr = GPIODATA_READ(Cfgread->PortMask,Cfgread->PortId);
            RetVar = GPIO_OK ;}
       else {RetVar = GPIO_NOK ; }
    }
    else {RetVar = GPIO_NOK ; }

    return RetVar;}


/*A function to select which peripheral will be connected to a GPIO pin*/
GPIO_CheckType GPIO_SetAlternFuntion(uint8_t GroupId,uint8_t AlternFuncId)
{
    const GPIO_CfgType * CfgAltF;
    GPIO_CheckType RetVar;
    uint8_t Mask;
    uint8_t Counter;
    if((GroupId < GPIO_GROUPS_NUMBER))
    {
       CfgAltF = &GPIO_ConfigParam[GroupId];
       if((CfgAltF->UseAlterFun == 0xff) && (GPIO_GroupState[GroupId] == 1))
       {
           Mask = CfgAltF->PortMask;
           Counter = 0;
           while(Mask != 0)
           {
               if(Mask % 2 == 1)
               {
                   GPIOPCTL_REG(CfgAltF->PortId) |= (AlternFuncId<<(Counter*4));
               }
               else
               {
               }
               Mask >>= 1;
               Counter += 1;
           }
       }
       else
       {
           RetVar = GPIO_NOK;
       }
       }
   else
   {
       RetVar = GPIO_NOK;
   }
   return RetVar;
}

/*A function to Select the interrupt event for a specific GPIO Group*/
GPIO_CheckType GPIO_SetInterruptEvent(uint8_t GroupId,GPIO_IntEventType IntEvent,GPIO_IntMaskStatus IntMaskStatus)
{
    const GPIO_CfgType * Cfgint;
    GPIO_CheckType RetVar;
    if (( GroupId <GPIO_GROUPS_NUMBER) && (GPIO_GroupState[GroupId]==1))
    {
        Cfgint =  & GPIO_ConfigParam[GroupId];
        GPIOIM_REG(Cfgint->PortId) &=~ (Cfgint->PortMask);

        if ((IntEvent == EVENT_LOW) || (IntEvent == EVENT_HIGH))
        {
            GPIOIS_REG(Cfgint->PortId) |= (Cfgint->PortMask);
        }
        else GPIOIS_REG(Cfgint->PortId) &=~ (Cfgint->PortMask);

        if ((IntEvent == EVENT_BOTH_EDGE))
        {
            GPIOIBE_REG(Cfgint->PortId) |= (Cfgint->PortMask) ;
        }

        else if ((IntEvent == EVENT_LOW) || (IntEvent == EVENT_FALL_EDGE))
        {
            GPIOIEV_REG(Cfgint->PortId) &=~(Cfgint->PortMask);
        }
        else GPIOIEV_REG(Cfgint->PortId) |= (Cfgint->PortMask);

        if ( IntMaskStatus == MASK_ENABLED)
        {
            GPIOICR_REG(Cfgint->PortId) |= (Cfgint->PortMask);
            GPIOIM_REG (Cfgint->PortId) |= (Cfgint->PortMask);
        }
        RetVar =GPIO_OK ;
    }
    else
    {
        RetVar =GPIO_NOK ;
    }
    return RetVar ;
}

/*A function to clear a specific pin interrupt flag*/
GPIO_CheckType GPIO_ClrInterruptFlag(uint8_t GroupId)
{
    const GPIO_CfgType * Cfgint;
    GPIO_CheckType RetVar;
    if (( GroupId <GPIO_GROUPS_NUMBER) && (GPIO_GroupState[GroupId]==1))
       {
        Cfgint =  & GPIO_ConfigParam[GroupId];
        GPIOICR_REG(Cfgint->PortId) |= (Cfgint->PortMask);
        RetVar =GPIO_OK ;
       }
    else
        {
        RetVar =GPIO_NOK ;
        }
    return RetVar ;
}
/*A function to Get a specific pin interrupt status*/
GPIO_CheckType GPIO_GetInterruptStatus(uint8_t GroupId,GPIO_IntStatus *IntStatusPtr)
{
    const GPIO_CfgType * Cfgint;
    GPIO_CheckType RetVar;
    if (( GroupId <GPIO_GROUPS_NUMBER) && (GPIO_GroupState[GroupId]==1))
       {
        Cfgint =  & GPIO_ConfigParam[GroupId];
        if (( (GPIOMIS_REG(Cfgint->PortId)) & (Cfgint->PortMask)) == (Cfgint->PortMask))
            {
                *IntStatusPtr =  INT_PENDING ;
            }
        else
            {
            *IntStatusPtr =  INT_INACTIVE ;
            }
        RetVar =GPIO_OK ;
       }
    else
        {
        RetVar =GPIO_NOK ;
        }
    return RetVar ;
}

/* This function is used to determine the groupID of each GPIO channel*/
static uint8_t groupID_Search (uint8_t PortID)
{
    uint8_t GPIO_groupID = 0;
    const  GPIO_CfgType* CfgPtr;
    uint8_t LoopIndex = 0;
    if(PortID < PORTS_NUMBER)
    {
        for(LoopIndex = 0; LoopIndex < GPIO_GROUPS_NUMBER; LoopIndex++)
        {
            CfgPtr = &GPIO_ConfigParam[LoopIndex];
            //Checking if the configuration has the required PortID
            if(((CfgPtr->PortId) == PortID) && (CfgPtr->CbkFnPtr!=0))
            {
                GPIO_groupID = LoopIndex;
            }
        }
    }
    return GPIO_groupID;
}

// PortF interrupt handler. Must be configured in the vector table.
void GPIO_PortF_ISR(void)
{
    uint8_t GroupId = groupID_Search(0x05);
    const GPIO_CfgType * CfgPtr;
    GPIO_IntStatus StatusPtr = INT_INACTIVE;
    if (( GroupId <GPIO_GROUPS_NUMBER) && (GPIO_GroupState[GroupId]==1))
       {
        CfgPtr =  & GPIO_ConfigParam[GroupId];
        GPIO_GetInterruptStatus(GroupId, &StatusPtr);
        if (StatusPtr == INT_PENDING )
        {
            CfgPtr->CbkFnPtr();
            GPIO_ClrInterruptFlag(GroupId);
        }
       }
}

static uint8_t GetPinNumber(uint8_t PinMask)
{

}
