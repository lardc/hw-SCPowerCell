#include "stm32f30x.h"
#include "stdbool.h"
#include "LocalDefs.h"
#include "Global.h"
#include "ZwTIM.h"
#include "DataTable.h"
#include "ZwGPIO.h"
#include "ZwEXTI.h"
#include "Controller.h"
#include "ZwNCAN.h"
#include "ZwADC.h"
#include "ZwSCI.h"
//

//Variables
int SurgeCurrent=0;
int RegulatorError=0;
int RegulatorOut=0;
int Qp=0;
long int Qi=0;
//

//------------------------------------------------------------------------------
void ADC3_IRQHandler(void)
{
  if(CheckDeviceState(DS_PulseStart))
  {
    //Алгоритм ПИ регулирования
    uint16_t SurgeCurrent = (uint16_t)(ADC_Read(ADC3)*SC_MEASURE_COEF);
    RegulatorError = PulseDataBuffer[PulseCounter] - SurgeCurrent;

    if(DataTable[REG_WAVEFORM_TYPE]==WAVEFORM_SINE)
    {
      Qp = (int)(RegulatorError*P_Reg_TopPulse);
      Qi += (int)(RegulatorError*I_Reg_TopPulse);
    }
    else
    {
      if(PulseCounter>(EdgePointsCounter+6))
      {
        Qp = (int)(RegulatorError*P_Reg_TopPulse);
        Qi += (int)(RegulatorError*I_Reg_TopPulse);
      }
      else
      {
        Qp = (int)(RegulatorError*P_Reg_FrontPulse);
        Qi += (int)(RegulatorError*I_Reg_FrontPulse);
      }
    }

    RegulatorOut = (int)((PulseDataSetUp[PulseCounter]+DataTable[REG_PULSE_OFFSET_VALUE])+Qp+Qi);


    //Сохраняем данные в endpoint
    if(DataTable[REG_TEST_REGULATOR]==MODE_TEST_REG_ON)
    {
      CONTROL_Values_Pulse[PulseCounter]=(Int16U)SurgeCurrent;
    }
    else
    {
      CONTROL_Values_Pulse[PulseCounter]=(Int16U)RegulatorError;
    }
    CONTROL_Values_Pulse_Counter=EP_SIZE;
    //

    if(RegulatorOut>DAC_MAX_LEVEL)RegulatorOut=DAC_MAX_LEVEL;
    if(RegulatorOut<0)RegulatorOut=DataTable[REG_PULSE_OFFSET_VALUE];

    DAC_CH1_SetValue(RegulatorOut);
    //

    PulseCounter++;
    if(PulseCounter>PULSE_BUFFER_SIZE)
    {
      SYNC_LINE_HIGH;
      SetDeviceState(DS_PulseEnd);
      RegulatorOut_SetLow();
      SC_DelayCounter = CONTROL_TimeCounter;
      Qi = 0;
      PulseCounter = 0;
      DataTable[REG_TEST_REGULATOR]=0;
      TIM_Stop(TIM15);
    }
  }

  ADC_InterruptClearFlag(ADC3, EOC);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void EXTI4_IRQHandler(void)
{
  if(CheckDeviceState(DS_PulseConfigReady))
  {
    SetDeviceState(DS_PulseStart);
    //
    SyncLine_TimeOutCounter = CONTROL_TimeCounter; //Запуск таймера таймаута импульса синхронизации (SYNC)
    //

    Delay_mS(AMPLIFIRE_UNLOCK_TIME);

    //Запуск формирования синуса
    TIM_StatusClear(TIM15);
    TIM_Start(TIM15);
    TIM_Reset(TIM15);
    //
  }

  EXTI_FlagReset(EXTI_4);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void USB_LP_CAN1_RX0_IRQHandler(void)
{
 if (ZwNCAN_RecieveCheck(CAN1))
  {
    ZwNCAN_RecieveData();

    ZwNCAN_RecieveFlagReset(CAN1);
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void USART1_IRQHandler()
{
  if(ZwSCI_RecieveCheck(USART1))
  {
    ZwSCI_RegisterToFIFO(USART1);
  }
  ZwSCI_RecieveFlagClear(USART1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void TIM7_IRQHandler(void)
{
  if (TIM_StatusCheck(TIM7))
  {
    CONTROL_TimeCounter++;

    //Моргаем светодиодом
    if(CONTROL_TimeCounter>(LED_PeriodCounter+LED_PERIOD_BLINK))
    {
      if(LED_STATE)LED_ON;
      else LED_OFF;
      LED_PeriodCounter = CONTROL_TimeCounter;
    }
    //

    //Если линия синхронизации остается в 1 больше чем SYNC_LINE_HIGHSTATE_TIMEOUT
    //то выставляем ошибку SYNC_TIMEOUT
    if((SyncLine_TimeOutCounter!=0)&&(CONTROL_TimeCounter>(SyncLine_TimeOutCounter+SYNC_LINE_HIGHSTATE_TIMEOUT)))
    {
      if(!(SYNC_LINE_STATE))
      {
        SetDeviceState(DS_Disabled);
        DataTable[REG_DISABLE_REASON]=ERR_SYNC_TIMEOUT;
      }
      else
      {
        SyncLine_TimeOutCounter=0;
      }
    }
    //
  }

  TIM_StatusClear(TIM7);
}
//------------------------------------------------------------------------------
