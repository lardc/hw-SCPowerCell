// -----------------------------------------
// System parameters
#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

// Include

// Flash loader options
#define BOOT_LOADER_VARIABLE			(*((volatile uint32_t *)0x20000000))
#define BOOT_LOADER_REQUEST				0x12345678
#define BOOT_LOADER_MAIN_PR_ADDR		0x08004800
//-----------------------------------------------

//System clock
#define SYSCLK                      70000000                        //Тактовая частота системной шины процессора
#define QUARTZ_FREQUENCY            20000000                        //Частота кварца
//

//USART
#define USART_BOUDRATE                          115200                          //Скорость USART
#define USART_FIFOlen                           32                              //Длина FIFO USART
#define	SCCI_TIMEOUT_TICKS  					1000
#define	BCCIM_TIMEOUT_TICKS						50										// Таймаут протоколоа мастер BCCI (в мс)
//

//Периоды таймеров, мкС
#define TIMER6_uS_V20							0.1
#define TIMER15_uS_V20							30
#define TIMER6_uS_V11							10
#define TIMER15_uS_V11							1
#define TIMER7_uS                               1000

//CAN
#define CAN_BAUDRATE                            1000000                          //Битрейт CAN
#define MAILBOXmax                              36                              //Количество MailBox
#define MailBoxLen                              6                               //Длина MailBox


// Параметры конфигурации CAN MailBox
#define ZW_CAN_RECEIVE_MAILBOX		        TRUE
#define ZW_CAN_TRANSMIT_MAILBOX		        FALSE
#define ZW_CAN_MSG_LENGTH	    	        8
#define ZW_CAN_NO_PRIORITY		        0
#define ZW_CAN_STRONG_MATCH		        0


// Флаги конфигурации MailBox
#define ZW_CAN_RTRAnswer                        0x01
#define ZW_CAN_MBProtected                      0x02
#define ZW_CAN_UseExtendedID                    0x04


#define VALUES_x_SIZE_V20				PULSE_BUFFER_SIZE_V20
#define VALUES_x_SIZE_V11				PULSE_BUFFER_SIZE_V11

//Ports definitions
#define LED_ON                                  GPIO_Bit_Rst(GPIOB, Pin_15)
#define LED_OFF                                 GPIO_Bit_Set(GPIOB, Pin_15)
#define LED_STATE                               GPIO_Read_Bit(GPIOB, Pin_15)
#define SYNC_LINE_LOW                           GPIO_Bit_Rst(GPIOB, Pin_3)
#define SYNC_LINE_HIGH                          GPIO_Bit_Set(GPIOB, Pin_3)
#define SYNC_LINE_STATE                         GPIO_Read_Bit(GPIOB, Pin_3)
#define BAT_CHARGE_ON                           GPIO_Bit_Set(GPIOA, Pin_2)
#define BAT_CHARGE_OFF                          GPIO_Bit_Rst(GPIOA, Pin_2)
#define BUTTON_STATE                            GPIO_Read_Bit(GPIOA, Pin_15)


#endif // __SYSCONFIG_H
