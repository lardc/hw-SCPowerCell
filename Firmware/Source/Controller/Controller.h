// -----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
#include "stdinc.h"
//
#include "Global.h"
#include "ZwNCAN.h"
#include "BCCIMaster.h"
#include "ZwDAC.h"
#include "ZwIWDG.h"
#include "ZwADC.h"
#include "DeviceObjectDictionary.h"
#include "SIN_PulseGenerator.h"


// Переменные
//
extern volatile Int64U CONTROL_TimeCounter;

extern Int16U CONTROL_Values_Pulse_V20[VALUES_x_SIZE_V20];
extern Int16U CONTROL_Values_Pulse_V11[VALUES_x_SIZE_V11];

extern Int16U CONTROL_Values_Pulse_Counter;
extern Int16U CONTROL_Version;
//


// Функции
//
void CONTROL_Init();                                                            //Инициализация контроллера
void CONTROL_Idle();                                                            //Операции с низшим приоритетом
void BatChargeProcess(void);                                                    //Контроль за напряжением на конденсаторах
void DebugModeInit(void);                                                       //Инициализация контроллера в режиме Debug
void Delay_mS(uint64_t Delay);                                                  //Функция формирования задержки, мС
void IWDG_Control(void);                                                        //Контроль/сброс WatchDog
void SetDeviceState(DeviceState NewState);                                      //Установка состояния блока
bool CheckDeviceState(DeviceState NewState);                                    //Проверить состояние блока
void CAN_Config(Int16U NodeID);
//

#endif // __CONTROLLER_H
