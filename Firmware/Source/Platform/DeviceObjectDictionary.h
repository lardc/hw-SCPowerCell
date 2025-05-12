#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

//Debug mode
//#define DEBUG_MODE
#define DEBUG_CURRENT_VALUE                     300                             //Значение ударного тока в отладочном режиме, [А]

//ACTIONS
//
#define ACT_DS_NONE						1                               //Переход в состояние ожидания
#define ACT_BAT_START_CHARGE                    2                               //Команда блоку SCPowerCell на заряд батареи конденсаторов
#define ACT_FAULT_CLEAR                         3                               //Очистка fault
#define ACT_WARNING_CLEAR                       4                               //Очистка warning
#define ACT_RESET_DEVICE                        5                               //Перезапуск процессора
//-----------
#define ACT_SC_PULSE_CONFIG                     100                             //Команда блоку SCPowerCell на конфигурацию значения ударного тока
#define ACT_SC_PULSE_START                      101                             //Запуск формирования импульса ударного тока
//-----------
#define ACT_RESET_FOR_PROGRAMMING               320                             //Перезапуск процессора с целью перепрограммирования
//-----------
#define ACT_SAVE_DT_TO_FLASH                    200                             //Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_DT_FROM_FLASH               201                             //Восстановление данных из FLASH
#define ACT_DT_RESET_TO_DEFAULT                 202                             //Сброс DataTable в состояние по умолчанию
#define ACT_FLASH_CLEAR                         203                             //Очистка пользовательской FLASH памяти
//

//REGISTERS
//
//---------------
#define REG_PULSE_OFFSET_VALUE                  0                               //Значение смещения сигнала импульса ударного тока (сигнал с ЦАПа)
#define REG_REGULATOR_OFFSET_VALUE              1                               //Значение смещения сигнала регулятора
#define REG_BAT_VOLTAGE_COEF                    2                               //Калибровочный коэффициент напряжения конденсаторной батареи
#define REG_BAT_VOLTAGE_THRESHOLD               3                               //Порог заряда конденсаторной батареи
#define REG_SC_SINE_PULSE_COEF                  4                               //Калибровочный коэффициент амплитуды полусинусоидального импульса ударного тока
#define REG_SC_TRAP_PULSE_COEF                  5                               //Калибровочный коэффициент амплитуды трапецеидального импульса ударного тока
#define REG_SINE_P_KOEF                         6                               //Пропорциональный коэффициент регулятора для синуса
#define REG_SINE_I_KOEF                         7                               //Интегральный коэффициент регулятора для синуса
#define REG_TOP_TRAP_P_KOEF                     8                               //Пропорциональный коэффициент регулятора для вершины трапеции
#define REG_TOP_TRAP_I_KOEF                     9                               //Интегральный коэффициент регулятора для вершины трапеции
#define REG_FRONT_TRAP_P_KOEF                   10                              //Пропорциональный коэффициент регулятора для фронта трапеции
#define REG_FRONT_TRAP_I_KOEF                   11                              //Интегральный коэффициент регулятора для фронта трапеции
//---------------
#define REG_SC_PULSE_VALUE                      64                              //Значение амплитуды импульса ударного тока, Ампер
#define REG_WAVEFORM_TYPE                       65                              //Задание формы ударного тока (полусинус/трапеция)
#define REG_TRAPEZE_EDGE_TIME                   66                              //Время длительности фронта трапеции, мкС
#define REG_TEST_REGULATOR                      67                              //1-блок в режиме тестирования регулятора, 0-нормальный режим
#define REG_SCPC_VERSION                        95                              //Версия блока SCPC
//---------------
#define REG_BAT_VOLTAGE                         96                              //Напряжение на конденсаторной батарее, Вольт
#define REG_DEV_STATE                           97                              //Статус работы блока
#define REG_FAULT_REASON                        98
#define REG_DISABLE_REASON                      99
#define REG_WARNING                             100
#define REG_PROBLEM                             101
//---------------

#define REG_FWINFO_SLAVE_NID			256	// Device CAN slave node ID
#define REG_FWINFO_MASTER_NID			257	// Device CAN master node ID (if presented)
// 258 - 259
#define REG_FWINFO_STR_LEN				260	// Length of the information string record
#define REG_FWINFO_STR_BEGIN			261	// Begining of the information string record
//

// ENDPOINTS
//
#define EP16_Data_Pulse		                1	                        // Оцифрованные данные прямого напряжения
#define EP_WRITE_COUNT		                1                               
#define EP_COUNT			        1
#define EP_SIZE                                 PULSE_BUFFER_SIZE
//


//Errors
#define ERR_SYNC_TIMEOUT                        1                               //Превышено время нахождения линии SYNC в высоком состоянии
#define ERR_WAVEFORM_TYPE                       2                               //Неправильное задание типа формы ударного тока
#define ERR_SC_TRAPEZE_VALUE                    3                               //Превышено максимальное значение ударного тока при трапецеидальной форме

//Warnings
#define WARNING_SC_CUT_OFF                      1                               //Значение ударного тока обрезано, т.к. превышает предел

//User Errors
#define ERR_NONE                                0 				// Ошибок нет
#define ERR_CONFIGURATION_LOCKED                1 				// Устройство защищено от записи
#define ERR_OPERATION_BLOCKED                   2 				// Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY                    3 				// Устройство не готово для смены состояния
#define ERR_WRONG_PWD                           4 				// Неправильный ключ
//

// Password to unlock non-volatile area for write
//
#define ENABLE_LOCKING				FALSE
//


//
typedef enum __DeviceState
{
  DS_None                                       = 0,                            //Блок в неопределенном состоянии
  DS_Fault                                      = 1,                            //Блок в состоянии Fault
  DS_Disabled                                   = 2,                            //Блок в состоянии Disabled
  DS_WaitTimeOut                                = 3,                            //Блок в ожидании таймаута между импульсами ударного тока
  DS_BatteryChargeWait                          = 4,                            //Блок в состоянии ожидания заряда конденсаторной батареи
  DS_Ready                                      = 5,                            //Блок в состоянии готовности
  DS_PulseConfigReady                           = 6,                            //Блок в в сконфигурированном состоянии
  DS_PulseStart                                 = 7,                            //Блок в состоянии формирования импульса ударного тока
  DS_PulseEnd                                   = 8                             //Блок завершил формирование импульса тока  
} DeviceState;
//


#endif // __DEV_OBJ_DIC_H
