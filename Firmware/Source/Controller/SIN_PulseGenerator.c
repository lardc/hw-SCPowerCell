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
  DAC_CH1_SetValue(350);
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
  if((DataTable[REG_WAVEFORM_TYPE]!=WAVEFORM_SINE)&&(DataTable[REG_WAVEFORM_TYPE]!=WAVEFORM_TRAPEZE))
  {
    SetDeviceState(DS_Fault);
    DataTable[REG_FAULT_REASON]=ERR_WAVEFORM_TYPE;
    return;
  }
  //
  
#ifdef DEBUG_MODE      
  DataTable[REG_SC_PULSE_VALUE] = DEBUG_CURRENT_VALUE;  
#endif  

  SkipPulseCounter = DataTable[REG_SYNC_PULSE_COUNT];

  if(DataTable[REG_WAVEFORM_TYPE]==WAVEFORM_SINE)
  {
    if(DataTable[REG_TEST_REGULATOR]==MODE_TEST_REG_ON)
    {
      //Если режим тестирования регулятора включен, то обнуляем коэффициенты регулятора
      P_Reg_TopPulse=0;
      I_Reg_TopPulse=0;
    }
    else
    {
      //Подготовка коэффициентов регулятора
      P_Reg_TopPulse = (float)DataTable[REG_SINE_P_KOEF]/10000;
      I_Reg_TopPulse = (float)DataTable[REG_SINE_I_KOEF]/10000;
      //
    }
    
    SineWaveFormConfig(DataTable[REG_SC_PULSE_VALUE]);
  }
  
  if(DataTable[REG_WAVEFORM_TYPE]==WAVEFORM_TRAPEZE)
  {
    if(DataTable[REG_TEST_REGULATOR]==MODE_TEST_REG_ON)
    {
      //Если режим тестирования регулятора включен, то обнуляем коэффициенты регулятора
      P_Reg_TopPulse=0;
      I_Reg_TopPulse=0;
      P_Reg_FrontPulse=0;
      I_Reg_FrontPulse=0;
    }
    else
    {
      //Подготовка коэффициентов регулятора
      P_Reg_TopPulse = (float)DataTable[REG_TOP_TRAP_P_KOEF]/10000;
      I_Reg_TopPulse = (float)DataTable[REG_TOP_TRAP_I_KOEF]/10000;
      P_Reg_FrontPulse = (float)DataTable[REG_FRONT_TRAP_P_KOEF]/100000;
      I_Reg_FrontPulse = (float)DataTable[REG_FRONT_TRAP_I_KOEF]/100000;
      //
    }
    
    TrapezeWaveFormConfig(DataTable[REG_SC_PULSE_VALUE]);
  }
  
  if((!CheckDeviceState(DS_Fault))&&(!CheckDeviceState(DS_Disabled)))
  {
    SetRegulatorOffset(DataTable[REG_SC_PULSE_VALUE]);
    SetDeviceState(DS_PulseConfigReady);
  }
}
//------------------------------------------------------------------------------
void SineWaveFormConfig(float SurgeCurrent)
{   
  static float DataTemp,X,X2,SurgeCurrentCorrect; 
  Int32U BufferSizeActual = DataTable[REG_PULSE_DURATION] / TIMER15_uS;

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
  for(int cnt=0;cnt<BufferSizeActual;cnt++)
  {
    DataTemp = (float)cnt/BufferSizeActual;
    DataTemp = sin(3.1416*DataTemp);
    
    //Таблица синуса уставки
    PulseDataSetUp[cnt] = (uint16_t)(DataTemp*SurgeCurrentCorrect);
    
    //Таблица синуса с учетом коррекции
    PulseDataBuffer[cnt] = (uint16_t)(DataTemp*SurgeCurrent);
  }
  //
}
//------------------------------------------------------------------------------
void TrapezeWaveFormConfig(float SurgeCurrent)
{
  float X,X2,SurgeCurrentCorrect,SC_Coef;
  uint16_t Counter=0;
  uint16_t CounterTemp=0;
  
  //Проверяем на максимальное значение
  if(DataTable[REG_SC_PULSE_VALUE]>SC_TRAPEZE_MAX_VALUE)
  {
    DataTable[REG_SC_PULSE_VALUE] = SC_TRAPEZE_MAX_VALUE;
    SurgeCurrent = SC_TRAPEZE_MAX_VALUE;
    DataTable[REG_WARNING] = WARNING_SC_CUT_OFF;
  }
  //
  
  //Вычисляем количество точек фронта и самого импульса, в зависимости от заданного значения в мкС
  EdgePointsCounter = (float)(DataTable[REG_TRAPEZE_EDGE_TIME]*PULSE_BUFFER_SIZE)/PULSE_TIME_VALUE;
  PulsePointsCounter = (uint16_t)(PULSE_BUFFER_SIZE-EdgePointsCounter*2);
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
    PulseDataBuffer[Counter] = Delta*Counter;
    PulseDataSetUp[Counter] = DeltaSetUp*Counter;
    
    if((PulseDataBuffer[Counter]>SurgeCurrent)||(PulseDataSetUp[Counter]>SurgeCurrentCorrect))
    {
      PulseDataBuffer[Counter]=(int)SurgeCurrent;
      PulseDataSetUp[Counter]=(int)SurgeCurrentCorrect;
      break;
    }
    
    Counter++;
  }
  //
  
  //Формируем основной импульс трапеции
  CounterTemp = Counter;
  while(Counter<(PULSE_BUFFER_SIZE-CounterTemp-1))
  {
    PulseDataBuffer[Counter] = (uint16_t)(SurgeCurrent); 
    PulseDataSetUp[Counter] = (uint16_t)(SurgeCurrentCorrect); 
    Counter++;
  }
  //
  
  //Формируем спадающий фронт трапеции
  while(1)
  {
    PulseDataBuffer[Counter] = PulseDataBuffer[Counter-1] - Delta;
    PulseDataSetUp[Counter] = PulseDataSetUp[Counter-1] - DeltaSetUp;
    
    if((PulseDataBuffer[Counter]<=0)||(PulseDataSetUp[Counter]<=0))
    {
      PulseDataBuffer[Counter] = 0;
      PulseDataSetUp[Counter] = 0;
      break;
    }
    
    Counter++;
  }
  //
}
//------------------------------------------------------------------------------
