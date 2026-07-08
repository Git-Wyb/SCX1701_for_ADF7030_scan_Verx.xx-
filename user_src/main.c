/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    13-May-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <iostm8l151g4.h> // CPU型号
#include "Pin_define.h"   // 管脚定义
#include "initial.h"      // 初始�?  预定�?
#include "ram.h"          // RAM定义
#include "ADF7030_1.h"    // 初始化ADF7021
#include "Timer.h"        // 定时�?
#include "ID_Decode.h"    // ID_Decode处理
#include "eeprom.h"       // eeprom
#include "uart.h"         // uart
/** @addtogroup STM8L15x_StdPeriph_Template
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

void main(void)
{
    _DI();             // 关全�?中断
    RAM_clean();       // 清除RAM
    WDT_init();        //看门狿
    VHF_GPIO_INIT();   //IO初始�?
    SysClock_Init();   //系统时钟初始�?
    InitialFlashReg(); //flash EEPROM
    eeprom_sys_load(); //ID载入
    TIM4_Init();       // 定时�?
    //beep_init();       // 蜂鸣�?
    ClearWDT(); // Service the WDT

    PROFILE_CH_FREQ_32bit_200002EC = 426075000;
    PROFILE_RADIO_AFC_CFG1_32bit_2000031C = 0x0005005A;
    PROFILE_RADIO_DATA_RATE_32bit_200002FC = 0x6400000C;
    //PROFILE_GENERIC_PKT_FRAME_CFG1_32bit_20000500 = 0x0000100C;
    ADF7030Init(); //射频初始�?
    //UART1_INIT();  // UART1 for PC Software
    _EI();         // 允许中断
    ClearWDT(); // Service the WDT
    RF_test_mode();
    TIME_power_led = 500;

    //  FLAG_APP_RX = 1;
    FG_Receiver_LED_RX = 0;
    //TIME_EMC = 10;
    FLAG_testNo91 = 0;
    FLAG_testBEEP = 0;

    GetInitial_State();
    Status_Un.Receive_SignalType = 1;
    APP429M_Tx_State(); //上电发送一次状态

    while (1)
    {
        ClearWDT(); // Service the WDT
        //if (FLAG_testBEEP != 0)
        //    TEST_beep();

        if (time_Login_exit_256 == 0)
            ID_Decode_OUT();
        ID_learn();
        if((ID_SCX1801_DATA != 0) && Receiver_429MHz_mode == 0 && Status_Un.Exist_ID != 1)//有ID登录且不是万能码遥控就发送状态
        {
            APP_TX_PACKET();
        }
        //接收到特殊ID并且有ID登录或者接收到429MHz开闭指令有动作就启动蜂鸣器
        if((Status_Un.Exist_ID == 1))
        {
            BEEP_Module(300,1);
        }
        if (FLAG_APP_RX == 1)
        {
            Freq_Scanning();
            //if(Scan_step==2)
            SCAN_RECEIVE_PACKET(); //ɨ���������?
        }
     //   TranmissionACK();
    //        READ_RSSI_avg();

        if (FG_Receiver_LED_RX == 1)
            Receiver_LED_RX = 1;
        else if (FG_Receiver_LED_RX == 0)
            Receiver_LED_RX = 0;

        /* 必须有ID登录才进行异常、下限、动作中信号检测，若有变化则发送状态 */
        if(ID_SCX1801_DATA != 0 && Time_StateDetection == 0)
        {
            Action_Signal_Detection(); //状态监测
            if(Status_Un.Buzzer_Switch == 0)
            {
                Beep_Switch = 0;
                Allow_BeepOn_Flag = 0;
            }
        }
        if(auto_over_time == 1)   Beep_Switch = 0;
        if((ID_SCX1801_DATA != 0) && (Allow_BeepOn_Flag == 1))
        {
            Beep_Action_On();
        }
        if(ID_SCX1801_DATA == 0)    app_tx_en = 0;
        if(flag_control_signal == 1 && TIMER1s == 0)
        {
            if(flag_state_tx == 0) APP429M_Tx_State();
            else
            {
                flag_state_tx = 0;
                flag_control_signal = 0;
            }
        }
    }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
