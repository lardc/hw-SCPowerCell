// -----------------------------------------
// Device data table
// ----------------------------------------

// Header
#include "DataTable.h"
//
#include "SysConfig.h"
#include "FirmwareInfo.h"
//

// Constants
//
#define DT_EPROM_ADDRESS	0x08004000u

// Variables
//
static EPROMServiceConfig EPROMServiceCfg;
volatile Int16U DataTable[DATA_TABLE_SIZE];

// Functions
//
void DT_Init(EPROMServiceConfig EPROMService, Boolean NoRestore)
{
	Int16U i;
	
	EPROMServiceCfg = EPROMService;

	for(i = 0; i < DATA_TABLE_SIZE; ++i)
		DataTable[i] = 0;

	if(!NoRestore)
		DT_RestoreNVPartFromEPROM();
}
// ----------------------------------------

void DT_RestoreNVPartFromEPROM()
{
	if(EPROMServiceCfg.ReadService)
		EPROMServiceCfg.ReadService(DT_EPROM_ADDRESS, (pInt16U)&DataTable[DATA_TABLE_NV_START], DATA_TABLE_NV_SIZE);
}
// ----------------------------------------

void DT_SaveNVPartToEPROM()
{
	if(EPROMServiceCfg.WriteService)
		EPROMServiceCfg.WriteService(DT_EPROM_ADDRESS, (pInt16U)&DataTable[DATA_TABLE_NV_START], DATA_TABLE_NV_SIZE);
}
// ----------------------------------------

void DT_ResetNVPart(FUNC_SetDefaultValues SetFunc)
{
	Int16U i;
	
	for(i = DATA_TABLE_NV_START; i < (DATA_TABLE_NV_SIZE + DATA_TABLE_NV_START); ++i)
		DataTable[i] = 0;

	if(SetFunc)
		SetFunc();
}
// ----------------------------------------

void DT_ResetWRPart(FUNC_SetDefaultValues SetFunc)
{
	Int16U i;

	for(i = DATA_TABLE_WR_START; i < DATA_TABLE_WP_START; ++i)
		DataTable[i] = 0;

	if(SetFunc)
		SetFunc();
}

//------------------------------------------
void DT_ResetNVPartToDefault(void)
{ 
  DataTable[REG_PULSE_OFFSET_VALUE] = PULSE_OFFSET_DEFAULT;
  DataTable[REG_REGULATOR_OFFSET_VALUE] = REGULATOR_OFFSET_DEFAULT;
  DataTable[REG_BAT_VOLTAGE_COEF] = BATTERY_VOLTAGE_COEF_DEFAULT;
  DataTable[REG_BAT_VOLTAGE_THRESHOLD] = BATTERY_THRESHOLD_DEFAULT;
  DataTable[REG_SC_SINE_PULSE_COEF] = SINE_PULSE_COEF_DEFAULT;
  DataTable[REG_SC_TRAP_PULSE_COEF] = TRAP_PULSE_COEF_DEFAULT;
  DataTable[REG_SINE_P_KOEF] = REG_SIN_P_DEFAULT;
  DataTable[REG_SINE_I_KOEF] = REG_SIN_I_DEFAULT;
  DataTable[REG_TOP_TRAP_P_KOEF] = REG_TOP_TRAP_P_DEFAULT;
  DataTable[REG_TOP_TRAP_I_KOEF] = REG_TOP_TRAP_I_DEFAULT;
  DataTable[REG_FRONT_TRAP_P_KOEF] = REG_FRONT_TRAP_P_DEFAULT;
  DataTable[REG_FRONT_TRAP_I_KOEF] = REG_FRONT_TRAP_I_DEFAULT;
  
  if(EPROMServiceCfg.WriteService)
		EPROMServiceCfg.WriteService(DT_EPROM_ADDRESS, (pInt16U) &DataTable[DATA_TABLE_NV_START], DATA_TABLE_NV_SIZE);
}
//------------------------------------------

void DT_SaveFirmwareInfo(Int16U SlaveNID, Int16U MasterNID)
{
	if(DATA_TABLE_SIZE > REG_FWINFO_STR_BEGIN)
	{
		DataTable[REG_FWINFO_SLAVE_NID] = SlaveNID;
		DataTable[REG_FWINFO_MASTER_NID] = MasterNID;

		DataTable[REG_FWINFO_STR_LEN] = FWINF_Compose((pInt16U)(&DataTable[REG_FWINFO_STR_BEGIN]),
				(DATA_TABLE_SIZE - REG_FWINFO_STR_BEGIN) * 2);
	}
}
// ----------------------------------------
