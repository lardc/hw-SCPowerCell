// ----------------------------------------

#ifndef __GLOBAL_H
#define __GLOBAL_H


// Include
#include "stdinc.h"
#include "SysConfig.h"
//


//Def
#define SCPC_VERSION                    20                                      //Версия блока SCPC
//------------------------------------------------
#define ADDRESS_FLAG_REGISTER           0x20007FF0                              //Адрес регистра флагов
#define ADDRESS_LOADED_PROGRAMM_START   0x08004800                              //Адрес начала памяти загруженной программы 
#define ADDRESS_FLASH_FIRST_PAGE        0x08000000                              //Адрес начала первой страницы FLASH памяти процессора
#define ADDRESS_FLASH_START_MCU         0x08004000                              //Начальный адрес сектора FLASH памяти MCU для пользовательских задач
//------------------------------------------------
#define FLAG_RESET_FOR_PROG             0x1111                                  //Значение флага при перезагрузке процессора с последующим перепрограммированием
#define FLAG_RESET                      0x4444                                  //Значение флага при перезагрузке процессора без перепрограммирования
//------------------------------------------------
#define MCU_FLASH_SIZE                  0x7FF                                   //Размер FLASH памяти MCU для пользовательских задач
#define CHANGE_STATE_DELAY_TIME         1000                                   //Время после импульса тока, когда блок переходит в состояние DS_WaitTimeOut
#define SC_PULSE_DELAY_VALUE            60000                                   //Время паузы между импульсами ударного тока, мС
#define SC_MIN_VALUE                    300                                       //Мнимальное значение ударного тока, Амперы
#define SC_SINE_MAX_VALUE               3100                                    //Максимальное значение ударного тока при полусинусоидальной форме, Амперы
#define SC_TRAPEZE_MAX_VALUE            3100                                    //Максимальное значение ударного тока при трапецеидальной форме, Амперы
#define TRAPEZE_EDGE_TIME_MIN           100                                     //Минимальное фремя фронта трапеции, мкС
#define TRAPEZE_EDGE_TIME_MAX           1000                                    //Максимальное фремя фронта трапеции, мкС
#define BATTERY_THRESHOLD_MAX           90                                      //Максимальный порог заряда конденсаторов, Вольт
#define PULSE_OFFSET_MAX                2500                                    //Максимальное значение смещения сигнала импульса (с ЦАПа)
#define REGULATOR_OFFSET_MAX            4095                                    //Максимальное значение смещения регулятора
#define PULSE_TIME_VALUE                10000                                   //Длительность импульса ударного тока, мкС
#define PULSE_BUFFER_SIZE               (PULSE_TIME_VALUE/TIMER15_uS)           //Количество точек в буфере формы импульса
#define LED_PERIOD_BLINK                250                                     //Период моргания светодиода, мС
#define SYNC_LINE_HIGHSTATE_TIMEOUT     25                                      //Таймаут пребывания линии синхронизации в высоком состоянии
#define SC_MEASURE_COEF                 0.91                                   //Коэффициент пропорц-ти измерения тока 
#define DAC_MAX_LEVEL                   3800                                    //Ограничение максимального значения ЦАПа
#define AMPLIFIRE_UNLOCK_TIME           10                                      //Время ожидания выхода в рабочий режим аналогового регулятора
//------------------------------------------------
#define MODE_TEST_REG_ON                1                                       //Режим блока для тестирования регулятор включен
#define MODE_TEST_REG_OFF               0                                       //Режим блока для тестирования регулятор выключен
//------------------------------------------------
//Значения калибровочных коэффициентов по умолчанию
#define PULSE_OFFSET_DEFAULT            1950                                    //Начальное смещение формирования импульса
#define REGULATOR_OFFSET_DEFAULT        3300                                    //Задание смещения для регулятора
#define BATTERY_VOLTAGE_COEF_DEFAULT    359                                     //Калибровочный коэффициент напряжения конденсаторов
#define BATTERY_THRESHOLD_DEFAULT       860                                     //Порог заряда конденсаторов
#define SINE_PULSE_COEF_DEFAULT         1023                                    //Калибровочный коэффициент амплитуды импульса ударного тока
#define TRAP_PULSE_COEF_DEFAULT         1010                                    //Коэффициент, изменяющий REGULATOR_OFFSET, в зависимости от амплитуды ударного тока
#define REG_SIN_P_DEFAULT               1000                                    //Значение пропорционального коэффициента для синуса по умолчанию
#define REG_SIN_I_DEFAULT               1300                                    //Значение интегрального коэффициента для синуса по умолчанию
#define REG_TOP_TRAP_P_DEFAULT          1000                                    //Значение пропорционального коэффициента для вершины трапеции по умолчанию
#define REG_TOP_TRAP_I_DEFAULT          1300                                    //Значение интегрального коэффициента для вершины трапеции по умолчанию
#define REG_FRONT_TRAP_P_DEFAULT        1000                                    //Значение пропорционального коэффициента для фронта трапеции по умолчанию
#define REG_FRONT_TRAP_I_DEFAULT        1000                                    //Значение интегрального коэффициента для фронта трапеции по умолчанию
//-----------------------------------------------
#define WAVEFORM_SINE                   0xAAAA                                  //Полусинусоидальная форма ударного тока
#define WAVEFORM_TRAPEZE                0xBBBB                                  //Трапециедальная форма ударного тока
//-----------------------------------------------
//

//Переменные
extern uint8_t Node;
extern bool Timeout_1S;
extern bool Timeout_1mS;
extern uint32_t MasterAddress;
extern uint16_t ValueCurrentTable;
extern int PulseDataBuffer[PULSE_BUFFER_SIZE];
extern int PulseDataSetUp[PULSE_BUFFER_SIZE];
extern uint64_t LED_PeriodCounter;
extern uint64_t SyncLine_TimeOutCounter;
extern uint64_t SC_DelayCounter;
extern uint16_t PulseCounter;
extern float EdgePointsCounter;
extern uint16_t PulsePointsCounter;
extern float P_Reg_TopPulse;
extern float I_Reg_TopPulse;
extern float P_Reg_FrontPulse;
extern float I_Reg_FrontPulse;
//

#endif // __GLOBAL_H
