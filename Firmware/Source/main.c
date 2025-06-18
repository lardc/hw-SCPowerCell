#include "main.h"
#include "BCCIxParams.h"
#include "DataTable.h"

int main()
{
	SetVectorTable();

	//Настройка системы тактирования
	SysClk_Config();

	// Конфигурация сервиса работы DataTable и EPROM
	EPROMServiceConfig EPROMService = { (FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT };

	// Инициализация DataTable
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, 0);

	//Проверка версии платы
	for(int i = 6; i <= 11; i++)
	{
		if(DataTable[i] != 0 && DataTable[i] != 65535)
			CONTROL_Version = SCPC_VERSION_V20;
	}

	if(CONTROL_Version == 0)
			CONTROL_Version = SCPC_VERSION_V11;

	//Настройка портов ввода/вывода
	IO_Config();

	//UART configure
	UART_Config();

	CAN_Config();

	//Настройка ЦАПа
	DAC1_Config();

	//Настройка Timer6 для ЦАПа
	Timer6_Config();

	//Настройка Timer15 для цифрового регулятора
	Timer15_Config();

	//Настройка Timer7, системный счетчик, период 1мС
	Timer7_Config();

	//Настройка АЦП
	ADC_Init();

	//WatchDog configure
	WatchDog_Config();

	//Инициализация логики работы контроллера
	CONTROL_Init();

	//Основной цикл
	while(true)
		CONTROL_Idle();

	return 0;
}

//---------------------------System clock config--------------------------------
void SysClk_Config(void)
{
	RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
	RCC_SysCfg_Clk_EN();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void WatchDog_Config(void)
{
  IWDG_Config();
}
//------------------------------------------------------------------------------

//-----------------------------GPIO config--------------------------------------
void IO_Config(void)
{
	//Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA); //GPIOA
	RCC_GPIO_Clk_EN(PORTB); //GPIOB
	//

  //Аналоговые
	GPIO_Config(GPIOA, Pin_3, Analog, NoPull, HighSpeed, NoPull);	//PA3 - вход АЦП (напряжение на конденсаторах)
	GPIO_Config(GPIOA, Pin_4, Analog, NoPull, HighSpeed, NoPull);	//PA4 - выход ЦАПа
	GPIO_Config(GPIOB, Pin_13, Analog, NoPull, HighSpeed, NoPull);	//PB13 - вход АЦП (ОС связь по току)
	GPIO_Config(GPIOA, Pin_5, Analog, NoPull, HighSpeed, NoPull);	//PA5 - выход ЦАПа

	//Входы
	GPIO_Config(GPIOA, Pin_15, Input, NoPull, HighSpeed, NoPull);	//PA15 - вход кнопки запуска ударного тока
	GPIO_Config(GPIOB, Pin_4, Input, NoPull, HighSpeed, NoPull);	//PB4(SYNC)

	//Выходы
	GPIO_Config(GPIOA, Pin_2, Output, PushPull, HighSpeed, NoPull);	//PA2(PS_ON)
	GPIO_Config(GPIOB, Pin_3, Output, OpenDrain, HighSpeed, Pull_Up);	//PB3(SYNC_DRIVE)
	GPIO_Bit_Set(GPIOB, Pin_3);
	GPIO_Config(GPIOB, Pin_15, Output, PushPull, HighSpeed, NoPull);	//PB15(LED)

	//Альтернативные функции
	GPIO_Config(GPIOA, Pin_11, AltFn, PushPull, HighSpeed, NoPull);	//PA11(CAN RX)
	GPIO_AltFn(GPIOA, Pin_11, AltFn_9);

	GPIO_Config(GPIOA, Pin_12, AltFn, PushPull, HighSpeed, NoPull);	//PA12(CAN TX)
	GPIO_AltFn(GPIOA, Pin_12, AltFn_9);

	GPIO_Config(GPIOA, Pin_9, AltFn, PushPull, HighSpeed, NoPull);	//PA9(USART1 TX)
	GPIO_AltFn(GPIOA, Pin_9, AltFn_7);	//Alternate function PA9 enable

	GPIO_Config(GPIOA, Pin_10, AltFn, PushPull, HighSpeed, NoPull);	//PA10(USART1 RX)
	GPIO_AltFn(GPIOA, Pin_10, AltFn_7);	//Alternate function PA10 enable

	//Внешние прерывания
	SYNC_INT_Config();
}
//------------------------------------------------------------------------------

void CAN_Config(void)
{
	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, false);
	NCAN_FIFOInterrupt(true);
	NCAN_FilterInit(0, CAN_SLAVE_FILTER_ID, CAN_SLAVE_NID_MASK);
	NCAN_FilterInit(1, CAN_MASTER_FILTER_ID, CAN_MASTER_NID_MASK);
}

//-----------------------------Timer 7 config-----------------------------------
void Timer7_Config(void)
{
  TIM_Clock_En(TIM_7);
  TIM_Config(TIM7, SYSCLK, TIMER7_uS);
  TIM_Interupt(TIM7, 0, true);
  TIM_Start(TIM7);
}
//------------------------------------------------------------------------------

//-----------------------------Timer 6 config-----------------------------------
void Timer6_Config(void)
{
	TIM_Clock_En(TIM_6);
	TIM_Config(TIM6, SYSCLK, (CONTROL_Version == SCPC_VERSION_V20 ? TIMER6_uS_V20 : TIMER6_uS_V11));
	TIM_MasterMode(TIM6, MMS_UPDATE);

	CONTROL_Version == SCPC_VERSION_V11 ? TIM_DMA(TIM6, DMAEN) : TIM_Start(TIM6);
}
//------------------------------------------------------------------------------

//-----------------------------Timer 4 config-----------------------------------
void Timer15_Config(void)
{
	TIM_Clock_En(TIM_15);
	TIM_Config(TIM15, SYSCLK, (CONTROL_Version == SCPC_VERSION_V20 ? TIMER15_uS_V20 : TIMER15_uS_V11));
	TIM_MasterMode(TIM15, MMS_UPDATE);
	if(CONTROL_Version == SCPC_VERSION_V11)
		TIM_Start(TIM15);
}
//------------------------------------------------------------------------------

//-----------------------------DAC1 config--------------------------------------
void DAC1_Config(void)
{
	DACx_Clk_Enable(DAC_1_ClkEN);
	DACx_Reset();
	DAC_Trigger_Config(TRIG1_TIMER6, TRIG1_ENABLE);
	DAC_Buff(BUFF1, false);
	if(CONTROL_Version == SCPC_VERSION_V11)
	{
		DACx_DMA_Config(DAC_DMA1ENABLE, DAC_DMA1UdIntDISABLE);
		DAC_Trigger_Config(TRIG2_TIMER15, TRIG2_ENABLE);
		DACx_Enable(DAC2ENABLE);
	}
	DACx_Enable(DAC1ENABLE);
}
//------------------------------------------------------------------------------

//-------------------------------ADC config-------------------------------------
void ADC_Init(void)
{
	RCC_ADC_Clk_EN(ADC_12_ClkEN);
	ADC_Calibration(ADC1);
	ADC_SoftTrigConfig(ADC1);
	ADC_Enable(ADC1);
	if(CONTROL_Version == SCPC_VERSION_V20)
	{
		RCC_ADC_Clk_EN(ADC_34_ClkEN);
		ADC_Calibration(ADC3);
		ADC_TrigConfig(ADC3, ADC34_TIM15_TRGO, RISE);
		ADC_ChannelSeqLen(ADC3, 1);
		ADC_ChannelSet_Sequence1_4(ADC3, 5, 1);
		ADC_Interrupt(ADC3, EOCIE, 0, true);
		ADC_Enable(ADC3);
	}
}
//------------------------------------------------------------------------------

//---------------------------EXTI config----------------------------------------
void SYNC_INT_Config(void)
{
	EXTI_Config(EXTI_PB, EXTI_4, FALL_TRIG, 0);
	EXTI_EnableInterrupt(EXTI4_IRQn, 1, true);
}
//------------------------------------------------------------------------------

//------------------------------UART config-------------------------------------
void UART_Config(void)
{
  USART_Init(USART1, SYSCLK, USART_BOUDRATE);
  USART_Recieve_Interupt(USART1, (CONTROL_Version == SCPC_VERSION_V20 ? 2 : 1), true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SetVectorTable()
{
	__disable_irq();
	SCB->VTOR = (uint32_t)BOOT_LOADER_MAIN_PR_ADDR;
	__enable_irq();
}
//------------------------------------------------------------------------------
