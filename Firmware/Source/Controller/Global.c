#include "LocalDefs.h"
#include "Global.h"

//Переменные
int PulseDataBuffer[PULSE_BUFFER_SIZE];                         //Таблица значений синуса ударного тока
int PulseDataSetUp[PULSE_BUFFER_SIZE];
uint16_t ValueCurrentTable = 0;
bool Timeout_1S = false;
bool Timeout_1mS = true;
uint64_t LED_PeriodCounter=0;
uint64_t SyncLine_TimeOutCounter=0;
uint64_t SC_DelayCounter;
uint16_t PulseCounter=0;
float EdgePointsCounter=0;
uint16_t PulsePointsCounter=0;
float P_Reg_TopPulse=0;
float I_Reg_TopPulse=0;
float P_Reg_FrontPulse=0;
float I_Reg_FrontPulse=0;
//
