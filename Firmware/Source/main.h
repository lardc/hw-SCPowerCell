#ifndef __MAIN_H
#define __MAIN_H

//Includes
#include "stm32f30x.h"
#include "ZwRCC.h"
#include "ZwGPIO.h"
#include "ZwNCAN.h"
#include "Global.h"
#include "ZwTIM.h"
#include "ZwDAC.h"
#include "ZwDMA.h"
#include "ZwADC.h"
#include "ZwEXTI.h"
#include "ZwSCI.h"
#include "ZwIWDG.h"
#include "ZwNFLASH.h"
#include "Controller.h"
#include "SysConfig.h"


//Functions
extern void CONTROL_Idle();
void SysClk_Config();
void IO_Config(void);
void CAN_Config();
void UART_Config();
void InitializeController(Boolean GoodClock);
void CONTROL_Init(void);
void Timer6_Config(void);
void Timer7_Config(void);
void Timer2_Config(void);
void Timer15_Config(void);
void DAC1_Config(void);
void ADC_Init(void);
void SYNC_INT_Config(void);
void Timer3_Config(void);
void UART_Config(void);
void WatchDog_Config(void);
void SetVectorTable();
#endif //__MAIN_H
