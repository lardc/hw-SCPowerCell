// -----------------------------------------
// Controller logic
// ----------------------------------------

// Header
#include "Controller.h"
//
// Includes
#include "SysConfig.h"
#include "DataTable.h"
#include "SCCISlave.h"
#include "DeviceProfile.h"
#include "BCCITypes.h"
#include "BCCIMaster.h"
#include "ZwTIM.h"
#include "ZwADC.h"
#include "math.h"
#include "ZwNFLASH.h"
#include "BCCIxParams.h"


// Types
//
typedef void (*FUNC_AsyncDelegate)();
//

// Variables
//
Int64U CONTROL_TimeCounter = 0;
static Boolean CycleActive = FALSE;
Int16U CONTROL_Values_Pulse[VALUES_x_SIZE];
Int16U CONTROL_Values_Pulse_Counter = 0;
//
//Forward fucntions
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);


// Functions
//
void CONTROL_Init()
{
  // Переменные для конфигурации EndPoint
  Int16U EPIndexes[EP_COUNT] = {EP16_Data_Pulse};
  Int16U EPSized[EP_COUNT] = {VALUES_x_SIZE};
  pInt16U EPCounters[EP_COUNT] = {(pInt16U)&CONTROL_Values_Pulse_Counter};
  pInt16U EPDatas[EP_COUNT] = {CONTROL_Values_Pulse};
  
  // Конфигурация сервиса работы Data-table и EPROM
  EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};

  // Инициализация data table
  DT_Init(EPROMService, FALSE);
  DT_SaveFirmwareInfo(CAN_SLAVE_NID, CAN_MASTER_NID);

  // Сброс значений 
  DEVPROFILE_ResetControlSection();

  // Инициализация device profile
  DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
  DEVPROFILE_InitEPService(EPIndexes, EPSized, EPCounters, EPDatas);
  
  // Сброс значений 
  DEVPROFILE_ResetControlSection();

  
  RegulatorOut_SetLow();
  
  IWDG_Control();
}
// ----------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
  *pUserError = ERR_NONE;
  
    switch(ActionID)
    {
    case ACT_BAT_START_CHARGE:
      {
        if(CheckDeviceState(DS_None))
        {
          SetDeviceState(DS_BatteryChargeWait);
          BAT_CHARGE_ON;      //Вкл источники питания для заряда батарей 
        }
        else
        {
          *pUserError = ERR_OPERATION_BLOCKED;
        }
        break;
      }
    case ACT_SC_PULSE_CONFIG:
      {
        if(CheckDeviceState(DS_Ready))
        { 
          DEVPROFILE_ResetScopes(0);
          SurgeCurrentConfig();
          DEVPROFILE_ResetEPReadState(); 
        }
        else
        {
          *pUserError = ERR_DEVICE_NOT_READY;
        }
        break;
      }
    case ACT_FAULT_CLEAR:
      {
        SetDeviceState(DS_None);
        DataTable[REG_FAULT_REASON]=0;
        break;
      }
    case ACT_WARNING_CLEAR:
      {
        DataTable[REG_WARNING]=0;
        break;
      }
    case ACT_DS_NONE:
      {
        SetDeviceState(DS_None);
        DataTable[REG_FAULT_REASON]=0;
        DataTable[REG_WARNING]=0;
        DataTable[REG_SC_PULSE_VALUE]=0;
        DataTable[REG_BAT_VOLTAGE]=0;
        RegulatorOut_SetLow();
        BAT_CHARGE_OFF;
        break;
      }

    case ACT_SAVE_DT_TO_FLASH:
      {
        DT_SaveNVPartToEPROM();
        break;
      }
    case ACT_RESTORE_DT_FROM_FLASH:
      {
        DT_RestoreNVPartFromEPROM();
        break;
      }

    case ACT_DT_RESET_TO_DEFAULT:
      {
        DT_ResetNVPartToDefault();
        break;
      }

    case ACT_SC_PULSE_START:
      {
        if(CheckDeviceState(DS_PulseConfigReady))
        {
          SYNC_LINE_LOW;
        }
        else
        {
          SetDeviceState(DS_Fault);
          *pUserError = ERR_DEVICE_NOT_READY;
        }
        break;
      }
    }
    return TRUE;
}
// ----------------------------------------

void CONTROL_Idle()
{
#ifdef DEBUG_MODE
  DebugModeInit();
  SurgeCurrentStart_DebugMode();
#endif
    
    BatChargeProcess();
    
    DEVPROFILE_ProcessRequests();
    
    IWDG_Control();
}
// ---------------------------------------  

//------------------------------------------------------------------------------
void DebugModeInit(void)
{
  if(CheckDeviceState(DS_None))
  {
    SetDeviceState(DS_BatteryChargeWait);
    BAT_CHARGE_ON;
    SYNC_LINE_HIGH;
  }
  
  if(CheckDeviceState(DS_Ready))
  {
    SurgeCurrentConfig();
  }
}
//------------------------------------------------------------------------------

//---------------------Процесс заряда батареи-----------------------------------
void BatChargeProcess(void)
{
  uint16_t Threshold = DataTable[REG_BAT_VOLTAGE_THRESHOLD];
  
  if(DataTable[REG_BAT_VOLTAGE]>=Threshold)
  {
    if(CheckDeviceState(DS_WaitTimeOut)&&(CONTROL_TimeCounter>=(SC_DelayCounter+SC_PULSE_DELAY_VALUE)))
    {
      SetDeviceState(DS_Ready);
    }
    
    if(CheckDeviceState(DS_BatteryChargeWait))
    {
      SetDeviceState(DS_Ready);
    }
  }
  
  //Через 5 сек после импульса меняем состояние на DS_WaitTimeOut
  if(CheckDeviceState(DS_PulseEnd)&&(CONTROL_TimeCounter>=(SC_DelayCounter+CHANGE_STATE_DELAY_TIME)))
  {
    SetDeviceState(DS_WaitTimeOut);
  }
  
  DataTable[REG_BAT_VOLTAGE] = (uint16_t)(ADC_Measure(ADC1, 4)*((float)DataTable[REG_BAT_VOLTAGE_COEF])/1000); //Сохраняем в память в формате [Вольт/10]
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Delay_mS(uint64_t Delay)
{
  TIM_Reset(TIM7);
  
  uint64_t Counter = CONTROL_TimeCounter;
  
  while(CONTROL_TimeCounter<(Counter+Delay)){}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void IWDG_Control(void)
{
  uint32_t McuResetFlag = (*(__IO uint32_t*)ADDRESS_FLAG_REGISTER)&0xFFFF;
  if((McuResetFlag!=FLAG_RESET_FOR_PROG)&&(McuResetFlag!=FLAG_RESET))
  {
    IWDG_Refresh();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SetDeviceState(DeviceState NewState)
{
  DataTable[REG_DEV_STATE] = NewState;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool CheckDeviceState(DeviceState NewState)
{
  if(DataTable[REG_DEV_STATE] == NewState) return 1;
  else return 0;
}
//------------------------------------------------------------------------------
// No more.
