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
volatile Int64U CONTROL_TimeCounter = 0;
static Boolean CycleActive = FALSE;
Int16U CONTROL_Values_Pulse_V20[VALUES_x_SIZE_V20];
Int16U CONTROL_Values_Pulse_V11[VALUES_x_SIZE_V11];
Int16U CONTROL_Values_Pulse_Counter = 0;
Int16U CONTROL_Version = 0;
//
//Forward fucntions
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);


// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U EPIndexes[EP_COUNT] = {EP16_Data_Pulse};
	Int16U EPSized_V11[EP_COUNT] = {VALUES_x_SIZE_V11};
	Int16U EPSized_V20[EP_COUNT] = {VALUES_x_SIZE_V20};
	pInt16U EPCounters[EP_COUNT] = {(pInt16U)&CONTROL_Values_Pulse_Counter};
	pInt16U EPDatas_V11[EP_COUNT] = {CONTROL_Values_Pulse_V11};
	pInt16U EPDatas_V20[EP_COUNT] = {CONTROL_Values_Pulse_V20};

	// Инициализация функций связанных с CAN NodeID
	Int16U NodeID = 0;
	if(DataTable[REG_CFG_NODE_ID] == 0 || DataTable[REG_CFG_NODE_ID] == 65535)
		NodeID = CAN_SLAVE_NID;
	else
		NodeID = DataTable[REG_CFG_NODE_ID];

	DT_SaveFirmwareInfo(NodeID, 0);
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive, NodeID);
	CAN_Config(NodeID);

	// Инициализация device profile
	DEVPROFILE_InitEPService(EPIndexes, (CONTROL_Version == SCPC_VERSION_V20 ? EPSized_V20 : EPSized_V11), EPCounters,
				(CONTROL_Version == SCPC_VERSION_V20 ? EPDatas_V20 : EPDatas_V11));
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
	if(DataTable[REG_BAT_VOLTAGE] >= DataTable[REG_BAT_VOLTAGE_THRESHOLD])
	{
		if(CheckDeviceState(DS_WaitTimeOut) && (CONTROL_TimeCounter >= (SC_DelayCounter + SC_PULSE_DELAY_VALUE)))
		{
			SetDeviceState(DS_Ready);
		}

		if(CheckDeviceState(DS_BatteryChargeWait))
		{
			SetDeviceState(DS_Ready);
		}
	}
	//Через 5 сек после импульса меняем состояние на DS_WaitTimeOut
	if(CheckDeviceState(DS_PulseEnd))
		if(CONTROL_TimeCounter >= (SC_DelayCounter +(CONTROL_Version == SCPC_VERSION_V20 ? CHANGE_STATE_DELAY_TIME_V20 : CHANGE_STATE_DELAY_TIME_V11)))
				SetDeviceState(DS_WaitTimeOut);

	DataTable[REG_BAT_VOLTAGE] = (uint16_t)(ADC_Measure(ADC1, 4) * ((float)DataTable[REG_BAT_VOLTAGE_COEF]) / 1000); //Сохраняем в память в формате [Вольт/10]
}
//------------------------------------------------------------------------------

void IWDG_Control(void)
{
	uint32_t McuResetFlag = (*(__IO uint32_t*)ADDRESS_FLAG_REGISTER) & 0xFFFF;

	if((McuResetFlag != FLAG_RESET_FOR_PROG) && (McuResetFlag != FLAG_RESET))
	{
		IWDG_Refresh();
	}
}
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

//------------------------------------------------------------------------------
void CAN_Config(Int16U NodeID)
{
	Int32U Mask = ((Int32U)NodeID) << CAN_SLAVE_NID_MPY;

	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, FALSE);
	NCAN_FIFOInterrupt(TRUE);
	NCAN_FilterInit(0, Mask, Mask);
	NCAN_InterruptSetPriority(0);
}
//------------------------------------------------------------------------------
// No more.
