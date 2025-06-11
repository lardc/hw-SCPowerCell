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
extern Int16U SkipPulseCounter;

void SetRegulatorOffset(uint32_t CurrentValue);									//Установка offset пунулятора в зависимости от тока
void SurgeCurrentStart_DebugMode(void);											//Старт формированию импульса тока в режиме Debug
void SurgeCurrentConfig(void);													//Конфигурирование формы импульса ударного тока
void RegulatorOut_SetLow(void);													//Выход регулятора принудительно в нуле, даже при разомкн. ОС
//функции для версии 2.0
void SineWaveFormConfig_V20(float SurgeCurrent);								//Конфигурация под полусинусоидальную форму
void TrapezeWaveFormConfig_V20(float SurgeCurrent);								//Конфигурация под трапецеидальную форму
//функции для версии 1.1
void SineWaveFormConfig_V11(uint16_t SurgeCurrent);								//Конфигурация под полусинусоидальную форму
void TrapezeWaveFormConfig_V11(uint16_t SurgeCurrent);							//Конфигурация под трапецеидальную форму
void HardwareSetup(void);														//Аппаратная готовность к формированию импульса тока

#endif // __SINPULSEGEN_H
