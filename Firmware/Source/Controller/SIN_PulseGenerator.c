#include "SIN_PulseGenerator.h"

// Variables
volatile Int16U SkipPulseCounter = 0;

//-------------Старт формирования ударного тока в отладочном режиме-------------
#ifdef DEBUG_MODE
void SurgeCurrentStart_DebugMode(void)
{
  if(CheckDeviceState(DS_PulseConfigReady))
  {
    if(!BUTTON_STATE)
    {  
      //Ждем 5мС и пока не отпустят кнопку
      Delay_mS(5);
      while(!BUTTON_STATE){}
      //
      SYNC_LINE_LOW;
    }
  }
}
#endif
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void RegulatorOut_SetLow(void)
{
	CONTROL_Version == 20 ? DAC_CH1_SetValue(350) : DAC_CH2_SetValue(0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SetRegulatorOffset(uint32_t CurrentValue)
{
  DAC_CH2_SetValue(DataTable[REG_REGULATOR_OFFSET_VALUE]);
}
//------------------------------------------------------------------------------

//-------------------Формирование таблицы формы ударного тока-------------------
void SurgeCurrentConfig(void)
{
	//Преверяем правильно ли задан тип фармы ударного тока
	if((DataTable[REG_WAVEFORM_TYPE] != WAVEFORM_SINE) && (DataTable[REG_WAVEFORM_TYPE] != WAVEFORM_TRAPEZE))
	{
		SetDeviceState(DS_Fault);
		DataTable[REG_FAULT_REASON] = ERR_WAVEFORM_TYPE;
		return;
	}
	//
#ifdef DEBUG_MODE
	DataTable[REG_SC_PULSE_VALUE] = DEBUG_CURRENT_VALUE;
#endif

	SkipPulseCounter = CONTROL_Version == 20 ? DataTable[REG_SYNC_PULSE_COUNT] : 0;

	if(DataTable[REG_WAVEFORM_TYPE] == WAVEFORM_SINE)
	{
		if(CONTROL_Version == 20)
		{
			if(DataTable[REG_TEST_REGULATOR] == MODE_TEST_REG_ON)
			{
				//Если режим тестирования регулятора включен, то обнуляем коэффициенты регулятора
				P_Reg_TopPulse = 0;
				I_Reg_TopPulse = 0;
			}
			else
			{
				//Подготовка коэффициентов регулятора
				P_Reg_TopPulse = (float)DataTable[REG_SINE_P_KOEF] / 10000;
				I_Reg_TopPulse = (float)DataTable[REG_SINE_I_KOEF] / 10000;
				//
			}
			SineWaveFormConfig_V20(DataTable[REG_SC_PULSE_VALUE]);
		}
		else
			SineWaveFormConfig_V11(DataTable[REG_SC_PULSE_VALUE]);
	}

	if(DataTable[REG_WAVEFORM_TYPE] == WAVEFORM_TRAPEZE)
	{
		if(CONTROL_Version == 20)
		{
			if(DataTable[REG_TEST_REGULATOR] == MODE_TEST_REG_ON)
			{
				//Если режим тестирования регулятора включен, то обнуляем коэффициенты регулятора
				P_Reg_TopPulse = 0;
				I_Reg_TopPulse = 0;
				P_Reg_FrontPulse = 0;
				I_Reg_FrontPulse = 0;
			}
			else
			{
				//Подготовка коэффициентов регулятора
				P_Reg_TopPulse = (float)DataTable[REG_TOP_TRAP_P_KOEF] / 10000;
				I_Reg_TopPulse = (float)DataTable[REG_TOP_TRAP_I_KOEF] / 10000;
				P_Reg_FrontPulse = (float)DataTable[REG_FRONT_TRAP_P_KOEF] / 100000;
				I_Reg_FrontPulse = (float)DataTable[REG_FRONT_TRAP_I_KOEF] / 100000;
				//
			}
			TrapezeWaveFormConfig_V20(DataTable[REG_SC_PULSE_VALUE]);
		}
		else
			TrapezeWaveFormConfig_V11(DataTable[REG_SC_PULSE_VALUE]);
	}

	if((!CheckDeviceState(DS_Fault)) && (!CheckDeviceState(DS_Disabled)))
	{
		if(CONTROL_Version == 11)
			HardwareSetup();
		SetRegulatorOffset(DataTable[REG_SC_PULSE_VALUE]);
		SetDeviceState(DS_PulseConfigReady);
	}
}
//------------------------------------------------------------------------------
void SineWaveFormConfig_V20(float SurgeCurrent)
{   
  static float DataTemp,X,X2,SurgeCurrentCorrect; 
  Int32U BufferSizeActual = DataTable[REG_PULSE_DURATION] / TIMER15_uS_V20;

  if(DataTable[REG_TEST_REGULATOR]==MODE_TEST_REG_ON)
  {
    SurgeCurrentCorrect = SurgeCurrent;
  }
  else
  {
    //В режиме теста регулятора все калибровки не задействованы
    float SC_Coef=((float)DataTable[REG_SC_SINE_PULSE_COEF])/1000;
    
    //Коррекция зависимости напряжения тока ОС и выходного тока блока
    X = SurgeCurrent;
    X2 = X*X;
    SurgeCurrentCorrect = X2_GATE_SETUP_CORRECTION*X2 + X_GATE_SETUP_CORRECTION*X + Y_GATE_SETUP_CORRECTION;
    //
    
    //Коррекция задания напряжения затворов
    X = SurgeCurrent;
    X2 = X*X;
    SurgeCurrent = X2_FB_CORRECTION*X2 + X_FB_CORRECTION*X + Y_FB_CORRECTION;
    SurgeCurrent = SurgeCurrent*SC_Coef;
    //
  }

  
  //Построение таблицы
	for(int cnt = 0; cnt < BufferSizeActual; cnt++)
	{
		DataTemp = (float)cnt / BufferSizeActual;
		DataTemp = sin(3.1416 * DataTemp);

		//Таблица синуса уставки
		PulseDataSetUp_V20[cnt] = (uint16_t)(DataTemp * SurgeCurrentCorrect);

		//Таблица синуса с учетом коррекции
		PulseDataBuffer_V20[cnt] = (uint16_t)(DataTemp * SurgeCurrent);
	}
	//
}
//------------------------------------------------------------------------------

void TrapezeWaveFormConfig_V20(float SurgeCurrent)
{
  float X,X2,SurgeCurrentCorrect,SC_Coef;
  uint16_t Counter=0;
  uint16_t CounterTemp=0;
  
  //Проверяем на максимальное значение
  if(DataTable[REG_SC_PULSE_VALUE]>SC_TRAPEZE_MAX_VALUE_V20)
  {
    DataTable[REG_SC_PULSE_VALUE] = SC_TRAPEZE_MAX_VALUE_V20;
    SurgeCurrent = SC_TRAPEZE_MAX_VALUE_V20;
    DataTable[REG_WARNING] = WARNING_SC_CUT_OFF;
  }
  //
  
  //Вычисляем количество точек фронта и самого импульса, в зависимости от заданного значения в мкС
  EdgePointsCounter = (float)(DataTable[REG_TRAPEZE_EDGE_TIME]*PULSE_BUFFER_SIZE_V20)/PULSE_TIME_VALUE_V20;
  PulsePointsCounter = (uint16_t)(PULSE_BUFFER_SIZE_V20-EdgePointsCounter*2);
  //  
  
  if(DataTable[REG_TEST_REGULATOR]==MODE_TEST_REG_ON)
  {
    SC_Coef = 1;
    SurgeCurrentCorrect = SurgeCurrent;
  }
  else
  {
    SC_Coef=((float)DataTable[REG_SC_TRAP_PULSE_COEF])/1000;
    
    //Коррекция задания напряжения затворов
    X = SurgeCurrent;
    X2 = X*X;
    SurgeCurrentCorrect = X2_GATE_SETUP_CORRECTION*X2 + X_GATE_SETUP_CORRECTION*X + Y_GATE_SETUP_CORRECTION;
    // 
    
    //Коррекция зависимости напряжения тока ОС и выходного тока блока
    X = SurgeCurrent;
    X2 = X*X;
    SurgeCurrent = X2_FB_CORRECTION*X2 + X_FB_CORRECTION*X + Y_FB_CORRECTION;
    SurgeCurrent = SurgeCurrent*SC_Coef;
    //
  }
  
  //Определяем dI/dt
  uint16_t Delta = (uint16_t)(SurgeCurrent/EdgePointsCounter);
  uint16_t DeltaSetUp = (uint16_t)(SurgeCurrentCorrect/EdgePointsCounter);
  //
  
  //Формируем нарастающий фронт трапеции
  while(1)
  {
    PulseDataBuffer_V20[Counter] = Delta*Counter;
    PulseDataSetUp_V20[Counter] = DeltaSetUp*Counter;
    
    if((PulseDataBuffer_V20[Counter]>SurgeCurrent)||(PulseDataSetUp_V20[Counter]>SurgeCurrentCorrect))
    {
      PulseDataBuffer_V20[Counter]=(int)SurgeCurrent;
      PulseDataSetUp_V20[Counter]=(int)SurgeCurrentCorrect;
      break;
    }
    
    Counter++;
  }
  //
  
  //Формируем основной импульс трапеции
  CounterTemp = Counter;
  while(Counter<(PULSE_BUFFER_SIZE_V20-CounterTemp-1))
  {
    PulseDataBuffer_V20[Counter] = (uint16_t)(SurgeCurrent);
    PulseDataSetUp_V20[Counter] = (uint16_t)(SurgeCurrentCorrect);
    Counter++;
  }
  //
  
  //Формируем спадающий фронт трапеции
  while(1)
  {
    PulseDataBuffer_V20[Counter] = PulseDataBuffer_V20[Counter-1] - Delta;
    PulseDataSetUp_V20[Counter] = PulseDataSetUp_V20[Counter-1] - DeltaSetUp;
    
    if((PulseDataBuffer_V20[Counter]<=0)||(PulseDataSetUp_V20[Counter]<=0))
    {
      PulseDataBuffer_V20[Counter] = 0;
      PulseDataSetUp_V20[Counter] = 0;
      break;
    }
    
    Counter++;
  }
  //
}
//------------------------------------------------------------------------------
void SineWaveFormConfig_V11(uint16_t SurgeCurrent)
{
	float DataTemp;
	float SC_Coef = ((float)DataTable[REG_SC_PULSE_COEF]) / 1000;

	for(volatile int cnt = 0; cnt < (PULSE_BUFFER_SIZE_V11 - 2); cnt++)
	{
		DataTemp = (float)cnt / (PULSE_BUFFER_SIZE_V11 - 2);
		DataTemp = sin(3.1416 * DataTemp);
		PulseDataBuffer_V11[cnt] = (uint16_t)(DataTemp * SurgeCurrent * SC_Coef);
		DataTemp = DataTable[REG_PULSE_OFFSET_VALUE];
		PulseDataBuffer_V11[cnt] += (uint16_t)DataTemp;

		//Сохраняем в Endpoint
		DataTemp = PulseDataBuffer_V11[cnt];
		CONTROL_Values_Pulse_V11[cnt] = (uint16_t)DataTemp;
	}
	PulseDataBuffer_V11[PULSE_BUFFER_SIZE_V11 - 2] = DataTable[REG_PULSE_OFFSET_VALUE];
	PulseDataBuffer_V11[PULSE_BUFFER_SIZE_V11 - 1] = DataTable[REG_PULSE_OFFSET_VALUE];
	CONTROL_Values_Pulse_V11[PULSE_BUFFER_SIZE_V11 - 2] = DataTable[REG_PULSE_OFFSET_VALUE];
	CONTROL_Values_Pulse_V11[PULSE_BUFFER_SIZE_V11 - 1] = DataTable[REG_PULSE_OFFSET_VALUE];

	CONTROL_Values_Pulse_Counter = EP_SIZE_V11;
}
//------------------------------------------------------------------------------

void TrapezeWaveFormConfig_V11(uint16_t SurgeCurrent)
{
	float DataTemp;
	float EdgePointsCounter = 0;
	uint16_t PulsePointsCounter = 0;

	//Проверяем на максимальное значение
	if(DataTable[REG_SC_PULSE_VALUE] > SC_TRAPEZE_MAX_VALUE_V11)
	{
		DataTable[REG_SC_PULSE_VALUE] = SC_TRAPEZE_MAX_VALUE_V11;
		SurgeCurrent = SC_TRAPEZE_MAX_VALUE_V11;
		DataTable[REG_WARNING] = WARNING_SC_CUT_OFF;
	}
	//

	//Вычисляем количество точек фронта и самого импульса, в зависимости от заданного значения в мкС
	EdgePointsCounter = (float)(DataTable[REG_TRAPEZE_EDGE_TIME] * PULSE_BUFFER_SIZE_V11) / PULSE_TIME_VALUE_V11;
	PulsePointsCounter = (uint16_t)(PULSE_BUFFER_SIZE_V11 - EdgePointsCounter * 2 - 2);
	//

	float SC_Coef = ((float)DataTable[REG_SC_PULSE_COEF]) / 1000;

	//Формируем наростающий фронт трапеции
	for(volatile int cnt = 0; cnt < EdgePointsCounter; cnt++)
	{
		DataTemp = ((float)cnt) / EdgePointsCounter;
		PulseDataBuffer_V11[cnt] = (uint16_t)(DataTemp * SurgeCurrent * SC_Coef);
		DataTemp = DataTable[REG_PULSE_OFFSET_VALUE];
		PulseDataBuffer_V11[cnt] += (uint16_t)DataTemp;

		//Сохраняем в Endpoint
		DataTemp = PulseDataBuffer_V11[cnt];
		CONTROL_Values_Pulse_V11[cnt] = (uint16_t)DataTemp;
	}
	//

	//Формируем основной импульс трапеции
	for(volatile int cnt = (int)EdgePointsCounter; cnt < (int)(EdgePointsCounter + PulsePointsCounter); cnt++)
	{
		PulseDataBuffer_V11[cnt] = (uint16_t)(SurgeCurrent * SC_Coef);
		DataTemp = DataTable[REG_PULSE_OFFSET_VALUE];
		PulseDataBuffer_V11[cnt] += (uint16_t)DataTemp;

		//Сохраняем в Endpoint
		DataTemp = PulseDataBuffer_V11[cnt];
		CONTROL_Values_Pulse_V11[cnt] = (uint16_t)DataTemp;
	}
	//

	//Формируем спадающий фронт трапеции
	for(volatile int cnt = 0; cnt < (int)EdgePointsCounter; cnt++)
	{
		DataTemp = 1.0 - ((float)cnt) / EdgePointsCounter;
		PulseDataBuffer_V11[(int)(EdgePointsCounter + PulsePointsCounter + cnt)] = (uint16_t)(DataTemp * SurgeCurrent
				* SC_Coef);
		DataTemp = DataTable[REG_PULSE_OFFSET_VALUE];
		PulseDataBuffer_V11[(int)(EdgePointsCounter + PulsePointsCounter + cnt)] += (uint16_t)DataTemp;

		//Сохраняем в Endpoint
		DataTemp = PulseDataBuffer_V11[(int)(EdgePointsCounter + PulsePointsCounter + cnt)];
		CONTROL_Values_Pulse_V11[(int)(EdgePointsCounter + PulsePointsCounter + cnt)] = (uint16_t)DataTemp;
	}
	//

	PulseDataBuffer_V11[PULSE_BUFFER_SIZE_V11 - 2] = DataTable[REG_PULSE_OFFSET_VALUE];
	PulseDataBuffer_V11[PULSE_BUFFER_SIZE_V11 - 1] = DataTable[REG_PULSE_OFFSET_VALUE];
	CONTROL_Values_Pulse_V11[PULSE_BUFFER_SIZE_V11 - 2] = DataTable[REG_PULSE_OFFSET_VALUE];
	CONTROL_Values_Pulse_V11[PULSE_BUFFER_SIZE_V11 - 1] = DataTable[REG_PULSE_OFFSET_VALUE];

	CONTROL_Values_Pulse_Counter = EP_SIZE_V11;
}
//------------------------------------------------------------------------------

void HardwareSetup(void)
{
	SYNC_LINE_HIGH;

	TIM_StatusClear(TIM6);
	TIM_Start(TIM6);
	TIM_Reset(TIM6);
	while(!CheckDeviceState(DS_PulseEnd)){}

	SetDeviceState(DS_Ready);
}
//------------------------------------------------------------------------------
