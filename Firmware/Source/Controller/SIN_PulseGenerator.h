#ifndef __SINPULSEGEN_H
#define __SINPULSEGEN_H

#include "Global.h"
#include "DeviceObjectDictionary.h"
#include "ZwTIM.h"
#include "ZwDAC.h"
#include "math.h"
#include "ZwGPIO.h"
#include "DataTable.h"
#include "ZwDMA.h"

//Defines
//
#define X2_FB_CORRECTION                -0.000055
#define X_FB_CORRECTION                 1.079
#define Y_FB_CORRECTION                 198.974

//
#define X2_GATE_SETUP_CORRECTION        -0.000046
#define X_GATE_SETUP_CORRECTION         0.547
#define Y_GATE_SETUP_CORRECTION         217.396

//Variables
extern volatile Int16U SkipPulseCounter;

void SetRegulatorOffset(uint32_t CurrentValue);                                 //Установка offset пунулятора в зависимости от тока
void HardwareSetup(void);                                                       //Аппаратная готовность к формированию импульса тока
void SurgeCurrentStart_DebugMode(void);                                         //Старт формированию импульса тока в режиме Debug
void SurgeCurrentConfig(void);                                                  //Конфигурирование формы импульса ударного тока
void RegulatorOut_SetLow(void);                                                 //Выход регулятора принудительно в нуле, даже при разомкн. ОС
void SineWaveFormConfig(float SurgeCurrent);                                 //Конфигурация под полусинусоидальную форму
void TrapezeWaveFormConfig(float SurgeCurrent);                              //Конфигурация под трапецеидальную форму

#endif // __SINPULSEGEN_H
