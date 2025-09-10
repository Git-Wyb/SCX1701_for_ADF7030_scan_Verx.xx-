/***********************************************************************/
/*  FILE        :initial.c                                             */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <stdio.h>
#include <iostm8l151c8.h>
//#include        "stm8l15x.h"
#include "Pin_define.h" // 管脚定义
#include "initial.h"    // 初始�? 预定�?
#include "ram.h"        // RAM定义
#include "ADF7030_1.h"
#include "uart.h" // uart
#include "Timer.h"
uFLAG YellowLedFlag, RedLedFalg;

void RAM_clean(void)
{ // 清除RAM
    //  asm("ldw X,#0");
    //  asm("clear_ram1.l");
    //  asm("clr (X)");
    //  asm("incw X");
    //  asm("cpw X,#0x6ff");
    //  asm("jrule clear_ram1");
}
void WDT_init(void)
{
    IWDG_KR = 0xCC;
    IWDG_KR = 0x55;
    IWDG_PR = 5;      //约862ms触发看门狗复位
    IWDG_KR = 0xAA;
}
void ClearWDT(void)
{
    IWDG_KR = 0xAA;
}
//========================GPIO说明===============================================================
//  GPIO_Mode_In_FL_No_IT      = (uint8_t)0x00,   /*!< Input floating, no external interrupt */
//  GPIO_Mode_In_PU_No_IT      = (uint8_t)0x40,   /*!< Input pull-up, no external interrupt */
//  GPIO_Mode_In_FL_IT         = (uint8_t)0x20,   /*!< Input floating, external interrupt */
//  GPIO_Mode_In_PU_IT         = (uint8_t)0x60,   /*!< Input pull-up, external interrupt */
//  GPIO_Mode_Out_OD_Low_Fast  = (uint8_t)0xA0,   /*!< Output open-drain, low level, 10MHz */
//  GPIO_Mode_Out_PP_Low_Fast  = (uint8_t)0xE0,   /*!< Output push-pull, low level, 10MHz */
//  GPIO_Mode_Out_OD_Low_Slow  = (uint8_t)0x80,   /*!< Output open-drain, low level, 2MHz */
//  GPIO_Mode_Out_PP_Low_Slow  = (uint8_t)0xC0,   /*!< Output push-pull, low level, 2MHz */
//  GPIO_Mode_Out_OD_HiZ_Fast  = (uint8_t)0xB0,   /*!< Output open-drain, high-impedance level, 10MHz */
//  GPIO_Mode_Out_PP_High_Fast = (uint8_t)0xF0,   /*!< Output push-pull, high level, 10MHz */
//  GPIO_Mode_Out_OD_HiZ_Slow  = (uint8_t)0x90,   /*!< Output open-drain, high-impedance level, 2MHz */
//  GPIO_Mode_Out_PP_High_Slow = (uint8_t)0xD0    /*!< Output push-pull, high level, 2MHz */
//===降低功�?�说明：①I/O没用，必须置Input pull-up    ②I/O外围有IC，但没用，必须置Input floating=====

void VHF_GPIO_INIT(void) // CPU端口设置
{
    /****************端口设置说明***************************
    *CR1寄存�? 输出 Output�?=推挽�?=OC�?
    *           输入 Input�?=上拉�?=浮动�?
    ***************end************************************/
    KEY_GPIO_Init();             // 输入 test�?登录�?
    Input_Signal_Init();            //输入管脚初始化

    Receiver_vent_direc = Input; // Input   受信机换气联动ON/OFF
    Receiver_vent_CR1 = 1;

    PIN_BEEP_direc = Output; // Output   蜂鸣�?
    PIN_BEEP_CR1 = 1;
    PIN_BEEP = 0;
    DIP_SW_Init();
    LED_GPIO_Init();
    ADF7030_GPIO_INIT();
    CG2214M6_GPIO_Init();
    Receiver_OUT_GPIO_Init(); // Output   受信机继电器

    PCF8563_RSTI_DDR = Output;
    PCF8563_RSTI_CR1 = 1;
    PCF8563_RSTI = 0;
}
//============================================================================================
void SysClock_Init(void)
{ // 系统时钟（外部时钟）
    //    /* Infinite loop */
    //    CLK_DeInit();                                         //时钟恢复默认
    //    CLK_HSICmd(ENABLE);
    //    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)==RESET);//等待直到LSI稳定
    ////    CLK_HSEConfig(CLK_HSE_ON);
    ////    CLK_HSEConfig(CLK_HSE_ON);
    ////    while(CLK_GetFlagStatus(CLK_FLAG_HSERDY)==RESET);//等待直到HSE稳定
    //    CLK_SYSCLKSourceSwitchCmd(ENABLE);
    ////    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);
    //    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    //    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
    //    CLK_SYSCLKSourceSwitchCmd(DISABLE);
    ////    CLK_LSICmd(ENABLE);
    ////    while(CLK_GetFlagStatus(CLK_FLAG_LSIRDY)==RESET);//等待直到LSI稳定
    ////    CLK_HSEConfig(CLK_HSE_OFF);
    ////    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);
    ////    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
    ////    CLK_SYSCLKSourceSwitchCmd(DISABLE);
    //
    //    //CLK_LSICmd(ENABLE);   //使能LSI时钟给看门狗�?
    //    //while(CLK_GetFlagStatus(CLK_FLAG_LSIRDY)==RESET);//等待直到LSI稳定

    CLK_ICKCR_HSION = 1; // 使能内部RC OSC�?6.00MHz�?
    while ((CLK_ICKCR & 0x02) == 0)
        ;              // �?查内部晶�?
    CLK_SWR = 0x01;    // 指定HSI为主时钟
                       //	while(( CLK_SWCR & 0x08 ) == 0 );		// 等待HSI切换
    CLK_SWCR_SWEN = 1; // 执行切换
    CLK_CKDIVR = 0x00; // 设置时钟分频  f HSI= f HSI RC输出/1    f CPU= f MASTER
    //---------------------------------------- 外设
    //CLK_PCKENR1 = 0x84;						// T1,UART1
    CLK_PCKENR1 = 0x66; //0x64; // T3?T4,UART1,beep
    CLK_PCKENR2 = 0x03; // ADC,T1

    CLK_ICKCR_LSION = 1; // 使能内部LSI OSC�?8KHz�?
    while (CLK_ICKCR_LSIRDY == 0)
        ; // �?查内部LSI OSC
}

void beep_init(void)
{
    //BEEP_CSR=0x4E;
    BEEP_CSR2 = 0;
    BEEP_CSR2_BEEPDIV = 9;
    BEEP_CSR2_BEEPSEL = 2;
    CLK_CBEEPR_CLKBEEPSEL0 = 1;
    CLK_CBEEPR_CLKBEEPSEL1 = 0;

	BEEP_CSR2_BEEPEN = 0;
}

//===================Delayus()延时===============//    Crystal: 16M HSI
void Delayus(unsigned char timer)
{
    unsigned char x; //延时T=((timer-1)*0.313+2 us
    unsigned char timer_cache;
    timer_cache = timer / 3;
    for (x = 0; x < timer_cache; x++)
    {
        __asm("nop");
        TranmissionACK();
    }
}
void Receiver_OUT_GPIO_Init(void)
{
/*
    Inverters_OUT_direc = Input; // 输入   继电器输出信号反�?  低电平有�?
    Inverters_OUT_CR1 = 1;
    if (Inverters_OUT == 1)
    {
        FG_allow_out = 1;
        FG_NOT_allow_out = 0;
    }
    else
    {
        FG_allow_out = 0;
        FG_NOT_allow_out = 1;
    }
*/
	FG_allow_out = 1;
	FG_NOT_allow_out = 0;


    Receiver_OUT_OPEN_direc = Output; // Output   受信机继电器OPEN  高电平有�?
    Receiver_OUT_OPEN_CR1 = 1;
    Receiver_OUT_OPEN = FG_NOT_allow_out;

    Receiver_OUT_CLOSE_direc = Output; // Output   受信机继电器CLOSE  高电平有�?
    Receiver_OUT_CLOSE_CR1 = 1;
    Receiver_OUT_CLOSE = FG_NOT_allow_out;

    Receiver_OUT_STOP_direc = Output; // Output   受信机继电器STOP  高电平有�?
    Receiver_OUT_STOP_CR1 = 1;
    Receiver_OUT_STOP = FG_NOT_allow_out;

    //Receiver_OUT_VENT_direc = Output;
    //Receiver_OUT_VENT_CR1 = 1;
    //Receiver_OUT_VENT = FG_NOT_allow_out;
}
/**
****************************************************************************
* @Function	: void LED_GPIO_Init(void)
* @file		: Initial.c
* @Author	: Xiaowine
* @date		: 2017/4/10
* @version	: V1.0
* @brief
**/
void LED_GPIO_Init(void)
{
    Receiver_LED_OUT_direc = Output; // Output   受信机继电器动作输出  高电平有�?
    Receiver_LED_OUT_CR1 = 1;
    Receiver_LED_OUT = 0;

    Receiver_LED_TX_direc = Output; // Output   受信机�?�信指示  高电平有�?
    Receiver_LED_TX_CR1 = 1;
    Receiver_LED_TX = 0;

    Receiver_LED_RX_direc = Output; // Output   受信机受信指�? 高电平有�?
    Receiver_LED_RX_CR1 = 1;
    Receiver_LED_RX = 0;

    PowerLED_DDR = Output;
    PowerLED_CR1 = 1;
    PowerLED = 0;
}
/**
****************************************************************************
* @Function	: void CG2214M6_GPIO_Init(void)
* @file		: Initial.c
* @Author	: Xiaowine
* @date		: 2017/4/11
* @version	: V1.0
* @brief	:
**/
void CG2214M6_GPIO_Init(void)
{
    CG2214M6_VC1_DDR = Output; /* 设置数据方向寄存�?1为输出，0为输�?-查看STM8寄存器RM0031.pdf 10.9 */
    CG2214M6_VC1_CR1 = 1;      /* 设置推挽输出--查看STM8寄存器RM0031.pdf 10.9*/
    CG2214M6_VC1_CR2 = 1;      /* 设置输出频率 1�?0M�?�?M--查看STM8寄存�?pdf P89 */

    CG2214M6_VC2_DDR = Output; /* 设置数据方向寄存�?1为输出，0为输�?-查看STM8寄存�?RM0031.pdf 10.9 */
    CG2214M6_VC2_CR1 = 1;      /* 设置推挽输出--查看STM8寄存器RM0031.pdf 10.9*/
    CG2214M6_VC2_CR2 = 1;      /* 设置输出频率 1�?0M�?�?M--查看STM8寄存�?pdf P89 */
}
/**
****************************************************************************
* @Function : void ADF7030_GPIO_INIT(void)
* @File     : Initial.c
* @Program  :
* @Created  : 2017/4/12 by Xiaowine
* @Brief    : 芯片外围控制IO初始�?
* @Version  : V1.0
**/
void ADF7030_GPIO_INIT(void)
{
    ADF7030_REST_DDR = Output; //* 设置数据方向寄存�?1为输出，0为输�?-查看STM8寄存器RM0031.pdf 10.9 */
    ADF7030_REST_CR1 = 1;      //* 设置推挽输出--查看STM8寄存器RM0031.pdf 10.9*/
    ADF7030_REST_CR2 = 1;      //* 设置输出频率 1�?0M�?�?M--查看STM8寄存�?pdf P89 */

    ADF7030_GPIO2_DDR = Input;            //输入
    ADF7030_GPIO2_CR1 = Floating;         //1: Input with pull-up 0: Floating input
    ADF7030_GPIO2_CR2 = InterruptDisable; //禁止中断

    ADF7030_GPIO3_DDR = Input; //输入
    ADF7030_GPIO3_CR1 = 0;     //1: Input with pull-up 0: Floating input
    ADF7030_GPIO3_CR2 = 0;     //禁止中断
    if(Receiver_test == 0)
        BerExtiInit();
}
/**
 ****************************************************************************
 * @Function : void BerExtiInit(void)
 * @File     : Initial.c
 * @Program  :
 * @Created  : 2017/6/6 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
void BerExtiInit(void)
{
    ADF7030_GPIO4_DDR = Input; //输入
    ADF7030_GPIO4_CR1 = 0;     //1: Input with pull-up 0: Floating input
    ADF7030_GPIO4_CR2 = 1;     //使能中断
    EXTI_CR2 &= (~MASK_EXTI_CR2_P4IS);
    EXTI_CR2 |= 0x02;

    ADF7030_GPIO5_DDR = Input; //输入
    ADF7030_GPIO5_CR1 = 1;     //1: Input with pull-up 0: Floating input
    ADF7030_GPIO5_CR2 = 0;     //禁止中断
}
/**
 ****************************************************************************
 * @Function : void BerExtiInit(void)
 * @File     : Initial.c
 * @Program  :
 * @Created  : 2017/6/6 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
void BerExtiUnInit(void)
{
    ADF7030_GPIO4_DDR = Input; //输入
    ADF7030_GPIO4_CR1 = 0;     //1: Input with pull-up 0: Floating input
    ADF7030_GPIO4_CR2 = 0;     //禁止中断
    EXTI_CR2 &= (~MASK_EXTI_CR2_P4IS);

    ADF7030_GPIO5_DDR = Input; //输入
    ADF7030_GPIO5_CR1 = 1;     //1: Input with pull-up 0: Floating input
    ADF7030_GPIO5_CR2 = 0;     //禁止中断
}
/**
****************************************************************************
* @Function	: void KEY_GPIO_Init(void)
* @file		: Initial.c
* @Author	: Xiaowine
* @date		: 2017/4/10
* @version	: V1.0
* @brief
**/
void KEY_GPIO_Init(void)
{
    // KEY_SW2_DDR = Input; //输入
    // KEY_SW2_CR1 = 1;     //1: Input with pull-up 0: Floating input
    // KEY_SW2_CR2 = 0;     //禁止中断

    // KEY_SW3_DDR = Input; //输入
    // KEY_SW3_CR1 = 1;     //1: Input with pull-up 0: Floating input
    // KEY_SW3_CR2 = 0;     //禁止中断

     //KEY_SW4_DDR = Input; //输入
     //KEY_SW4_CR1 = 1;     //1: Input with pull-up 0: Floating input
     //KEY_SW4_CR2 = 0;     //禁止中断

    Receiver_Login_direc = Input;          // Input   受信机登录键   低电平有�?
    Receiver_Login_CR1 = Floating;         //1: Input with pull-up 0: Floating input
    Receiver_Login_CR2 = InterruptDisable; //禁止中断
    //   Receiver_test_direc = Input;
    // Receiver_test_CR1 = 1;

    WORK_TEST_DDR = Input;            // 输入     test�?
    WORK_TEST_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
    WORK_TEST_CR2 = InterruptDisable; //禁止中断

    TP4_DDR = Input;            // 输入     test�?
    TP4_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
    TP4_CR2 = InterruptDisable; //禁止中断
}

/*
void KEY_TP3_Init(void)
{
    TP3_DDR = Input;            // 输入     test�?
    TP3_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
    TP3_CR2 = InterruptDisable; //禁止中断
}
*/

void OUT_VENT_Init(void)    //????????????????TP3????
{
    Receiver_OUT_VENT_direc = Output;
    Receiver_OUT_VENT_CR1 = 1;
    Receiver_OUT_VENT = FG_NOT_allow_out;
}


/*拨码开关管脚初始化*/
void DIP_SW_Init(void)
{
     SW2_1_DDR = Input;
     SW2_1_CR1 = Pull_up;
     SW2_1_CR2 = InterruptDisable;

     SW2_2_DDR = Input;
     SW2_2_CR1 = Pull_up;
     SW2_2_CR2 = InterruptDisable;

     SW2_3_DDR = Input;
     SW2_3_CR1 = Pull_up;
     SW2_3_CR2 = InterruptDisable;

     SW2_4_DDR = Input;
     SW2_4_CR1 = Pull_up;
     SW2_4_CR2 = InterruptDisable;
}

void Input_Signal_Init(void)
{
    //异常
     Abnormal_Signal_DDR = Input;
     Abnormal_Signal_CR1 = Pull_up;
     Abnormal_Signal_CR2 = InterruptDisable;

     //下限
     Lower_Limit_Signal_DDR = Input;
     Lower_Limit_Signal_CR1 = Pull_up;
     Lower_Limit_Signal_CR2 = InterruptDisable;

     //动作
     Action_Signal_DDR = Input;
     Action_Signal_CR1 = Pull_up;
     Action_Signal_CR2 = InterruptDisable;

     TF1_INPUT_DDR = Input;
     TF1_INPUT_CR1 = Pull_up;
     TF1_INPUT_CR2 = InterruptDisable;

     TF2_INPUT_DDR = Input;
     TF2_INPUT_CR1 = Pull_up;
     TF2_INPUT_CR2 = InterruptDisable;

     TF1_POWER_DDR = Output;
     TF1_POWER_CR1 = 1;
     TF1_POWER_CR2 = 1;
     TF1_POWER = 0;

     TF2_POWER_DDR = Output;
     TF2_POWER_CR1 = 1;
     TF2_POWER_CR2 = 1;
     TF2_POWER = 0;
}

/**
 ****************************************************************************
 * @Function : void RF_BRE_Check(void)
 * @File     : Initial.c
 * @Program  :
 * @Created  : 2017/5/5 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
void RF_BRE_Check(void)
{
    //char errbuff[10];
    ClearWDT(); // Service the WDT
    if (ADF7030_GPIO3 == 1)
    {
        WaitForADF7030_FIXED_DATA(); //等待芯片空闲/可接受CMD状�??
        DELAY_30U();
        ADF7030_Clear_IRQ();
        WaitForADF7030_FIXED_DATA(); //等待芯片空闲/可接受CMD状�??
        DELAY_30U();
        ADF7030_Wait_GPIO3();
        WaitForADF7030_FIXED_DATA(); //等待芯片空闲/可接受CMD状�??
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_PHY_ON);
        WaitForADF7030_FIXED_DATA(); //等待芯片空闲/可接受CMD状�??
        ADF7030_RECEIVING_FROM_POWEROFF();
    }

    if (X_COUNT >= 1000)
    {
        if (X_ERR >= 50)
            Receiver_LED_RX = 0;
        else
            Receiver_LED_RX = 1;
       // sprintf(errbuff, "%d\r\n", X_ERR);
        //s((u8 *)errbuff);
        //for (j = 0; j < 4; j++)
        //lcd    display_map_xy(70 + j * 6, 45, 5, 8, char_Small + (CacheData[3 - j] - ' ') * 5);
        //        display_map_58_6(70,45,4,CacheData);
        X_ERR = 0;
        X_COUNT = 0;
        X_ERRTimer = 1250;
    }
    if (X_ERRTimer == 0)
        Receiver_LED_RX = 0;
}
void PCF8563_CLKOUT_ON(void);
UINT8 CH376_USB_Del(void);
void RF_test_mode(void)
{
    u8 Flag_TP4 = 0;
    u8 test_time_Base10ms = 0;
    u8 Tx_Rx_mode = 0;
    u8 i=0;
	Receiver_LED_OUT = 1;

    while (Receiver_test == 0)
    {
        ClearWDT();   // Service the WDT
        if(flag_test_mode == 0)
        {
            flag_test_mode = 1;
            PCF8563_CLKOUT_ON();
            UART2_INIT();
        }
        if(Time_SwDetection == 0)    Dip_Sw_Detection();
        flag_tf1_now = TF1_INPUT;
        flag_tf2_now = TF2_INPUT;
        if(flag_sw_usb == 0 && flag_usb_write == 0)
        {
            flag_usb_state = CH376_USB_Del();
            flag_usb_write = 1;
        }
        if((TP4 == 0)&&(Flag_TP4==0))   //不使用TP3，因为测试模式TP3与工作模式换气输出有冲突，冲突为三极管导致TP3的高电平只有0.8V
        {
            if (FG_10ms==1)
            {
                FG_10ms = 0;
                test_time_Base10ms++;
                if (test_time_Base10ms>5)
                {
                    test_time_Base10ms = 5;
                    Flag_TP4 = 1;
                    Tx_Rx_mode++;
                    if (Tx_Rx_mode == 2)
                        Tx_Rx_mode = 3; //屏蔽mode 2，mode 2暂时不使用
                    if (Tx_Rx_mode == 3)
                        X_COUNT = 0;   //切换成mode3时，RX误码率有时会亮
                    if (Tx_Rx_mode > 3)
                        Tx_Rx_mode = 0;
                }
            }
        }
        else if(TP4 == 1)
        {
            Flag_TP4 = 0;
            test_time_Base10ms = 0;
        }

        if ((Tx_Rx_mode == 0) || (Tx_Rx_mode == 1))
        {
            CG2214M6_USE_T;
            FG_test_rx = 0;
            Receiver_LED_RX = 0;
            FG_test_tx_off = 0;
            if (Tx_Rx_mode == 0) //发载波，无调制信�?
            {
                //Receiver_LED_OUT = 1;
                Receiver_LED_TX = 1;
                FG_test_mode = 0;
                FG_test_tx_1010 = 0;
                if (FG_test_tx_on == 0)
                {
                    FG_test_tx_on = 1;
                    ADF7030_TX(TestTXCarrier);
                    //7021_DATA_ ADF7021_DATA_direc = Input;
                    //ttset dd_set_TX_mode_carrier();
                }
            }
            else //发载波，有调制信�?
            {
                if (TIMER1s == 0)
                {
                    TIMER1s = 500;
 //                   Receiver_LED_OUT = !Receiver_LED_OUT;
                    Receiver_LED_TX = !Receiver_LED_TX;
                }
                FG_test_mode = 1;
                FG_test_tx_on = 0;
                if (FG_test_tx_1010 == 0)
                {
                    ADF7030_TX(TestTx_PreamblePattern);
                    FG_test_tx_1010 = 1;

                    //7021_DATA_ ADF7021_DATA_direc = Output;
                    //ttset dd_set_TX_mode_1010pattern();
                }
            }
        }
        //else  {           //test ADF7021 RX
        if ((Tx_Rx_mode == 2) || (Tx_Rx_mode == 3))
        {
            CG2214M6_USE_R;
            FG_test_rx = 1;
//            Receiver_LED_OUT = 0;
            Receiver_LED_TX = 0;
            FG_test_mode = 0;
            FG_test_tx_on = 0;
            FG_test_tx_1010 = 0;
            if (FG_test_tx_off == 0)
            {
                ADF7030_RECEIVING_FROM_POWEROFF_testMode();
                FG_test_tx_off = 1;
            }
            if (Tx_Rx_mode == 2) //packet usart out put RSSI
            {
                if (TIMER1s == 0)
                {
                    TIMER1s = 500;
                    Receiver_LED_RX = !Receiver_LED_RX;
                }
                SCAN_RECEIVE_PACKET(); //扫描接收数据
            }
            if (Tx_Rx_mode == 3) //packet usart out put BER
            {
                RF_BRE_Check();
            }
        }
        //PC_PRG(); // PC控制
        if(flag_uart2_rx)
        {
            flag_uart2_rx = 0;
            if(Uart2_Recv_Buff[1] == 'W') Uart2_RTC_Write();
            else if (Uart2_Recv_Buff[1] == 'R') Uart2_RTC_Read();
        }

        if(((Abnormal_Signal == 0) && (Lower_Limit_Signal != 0) && (Action_Signal != 0) && (flag_tf1_now != 0) && (flag_tf2_now != 0))
           || ((Lower_Limit_Signal == 0) && (Abnormal_Signal != 0) && (Action_Signal != 0) && (flag_tf1_now != 0) && (flag_tf2_now != 0))
           || ((Action_Signal == 0) && (Lower_Limit_Signal != 0) && (Abnormal_Signal != 0) && (flag_tf1_now != 0) && (flag_tf2_now != 0))
           || ((flag_tf1_now == 0) && (Lower_Limit_Signal != 0) && (Abnormal_Signal != 0) && (Action_Signal != 0) && (flag_tf2_now != 0))
           || ((flag_tf2_now == 0) && (Lower_Limit_Signal != 0) && (Abnormal_Signal != 0) && (Action_Signal != 0) && (flag_tf1_now != 0)))
        {
            if(time_sw == 0)
            {
                Receiver_LED_OUT = 1;
                time_sw = 300;
                TIME_power_led = 150;
            }
            if(TIME_power_led == 0)
            {
                 Receiver_LED_OUT = 0;
            }
            i = 0;
        }
        else if(Abnormal_Signal == 0 || Lower_Limit_Signal == 0 || Action_Signal == 0 || flag_tf1_now == 0 || flag_tf2_now == 0)
        {
             Receiver_LED_OUT = 1;
             i = 1;
        }
        else    i = 0;
        if(TIME_power_led == 0 && i == 0)
        {
             Receiver_LED_OUT = 0;
        }
    /*    if(TIME_power_led == 0)   //拨码开关测试
        {
            i = DIP_SW_Code();
            if(code_x != i) //开关有变化
            {
                if(i == DIP_SW_Code())
                {
                    if(code_x==i-1 || code_x==i+1 || i-code_x==0x09 || code_x-i==0x09) //正常变化(开关从0直接拨到9或者从9直接拨到0也视做正常变化)
                    {
                        sum_num = 0;
                        code_x = i;
                        if(code_x % 2 == 0)
                        {
                            flag_odd_num = 1;       //奇数挡
                        }
                        else
                        {
                            flag_even_num = 3;      //偶数挡
                        }
                    }
                    else    //异常变化
                    {
                        code_x = i;
                        Receiver_LED_OUT = 1;
                        sum_num = 1;
                    }
                }
            }
            else
            {
                if(sum_num != 1)
                Receiver_LED_OUT = 0;
            }
            if(flag_odd_num == 1)
            {
                flag_odd_num = 0;
                Receiver_LED_OUT = 1;
            }
            else if(flag_even_num != 0)
            {
                flag_even_num--;
                if(flag_even_num != 1)
                Receiver_LED_OUT = 1;
            }
            time_sw = 150;
            TIME_power_led = 300;
        }*/
    }
    UART2_End();
    PowerLED = 1;
    OUT_VENT_Init();
    BerExtiUnInit();
    FG_test_rx = 0;
    TIMER1s = 0;
    Receiver_LED_TX = 0;
    Receiver_LED_RX = 0;
    FG_Receiver_LED_RX = 0;
    //Receiver_LED_OUT = 0;
    time_sw = 0;
    app_tx_en = 0;
    FLAG_APP_RX = 1;
    //TIME_Fine_Calibration = 900;
    //TIME_EMC = 10;
    flag_test_mode = 0;
}


//返回拨码开关编码
u8 DIP_SW_Code(void)
{
    u8 sw1 = 0,sw2 = 0,sw3 = 0,sw4 = 0;
    sw1 = SW2_1;
    sw2 = SW2_2;
    sw3 = SW2_3;
    sw4 = SW2_4;
    return (sw4 << 3 | sw3 << 2 | sw2 << 1 | sw1);
}

void GetInit_SwState(void)
{
    flag_sw_f429m = SW_F429M_IN;
    flag_sw_tf = SW_TF_IN;
    flag_sw_usb = SW_USB_IN;
    flag_sw2_4 = SW2_4;

    Sw_Un.FlagByte_bit0 = flag_sw_f429m;
    Sw_Un.FlagByte_bit1 = flag_sw_tf;
    Sw_Un.FlagByte_bit2 = flag_sw_usb;
    Sw_Un.FlagByte_bit3 = flag_sw2_4;
}

void Dip_Sw_Detection(void)
{
    Flag_Un sw_un = {0};
    static u8 swcnt = 0;
    sw_un.FlagByte_bit0 = SW_F429M_IN;
    sw_un.FlagByte_bit1 = SW_TF_IN;
    sw_un.FlagByte_bit2 = SW_USB_IN;
    sw_un.FlagByte_bit3 = SW2_4;

    if(Sw_Un.Falg_Byte != sw_un.Falg_Byte)
    {
        swcnt++;
        Time_SwDetection = 20; //200ms
        if(swcnt >= 3)
        {
            swcnt = 0;
            GetInit_SwState();
            if(flag_sw_usb == 1) flag_usb_write = 0;
            if(flag_test_mode == 1)   //test mode
            {
                if(flag_sw_f429m==0 || flag_sw_tf==0 || flag_sw_usb==0 || flag_sw2_4==0)
                {
                    UART2_End();
                    PowerLED = 1;
                }
                else
                    PowerLED = 0;

                if(flag_sw_tf == 0) TF1_POWER = 1;
                else TF1_POWER = 0;

                if(flag_sw2_4 == 0) TF2_POWER = 1;
                else TF2_POWER = 0;
            }
            else  //work mode
            {
                if(flag_sw_tf == 0)  {TF2_POWER = 0; flag_tf2 = 0;}
                else TF2_POWER = 1;

                if(flag_sw_f429m == 1)
                {
                    PROFILE_CH_FREQ_32bit_200002EC = 426075000;
                    PROFILE_RADIO_AFC_CFG1_32bit_2000031C = 0x0005005A;
                    PROFILE_RADIO_DATA_RATE_32bit_200002FC = 0x6400000C;
                    Radio_Date_Type=1;
					Channels=1;
                    ADF7030Init();
                }
            }
        }
    }
    else
    {
        swcnt = 0;
    }
}

void GetInit_TFState(void)
{
    TF1_POWER = 1;
    if(flag_sw_tf == 0)  TF2_POWER = 0;
    else TF2_POWER = 1;
    mDelaymS( 200 );

    flag_tf1_befor = TF1_INPUT;
    flag_tf2_befor = TF2_INPUT;

    if(flag_tf1_befor == 0)
    {
        Time_TF1 = 200;
        flag_tf1_in = 1;
    }
    else
    {
        Time_TF1 = 0;
        flag_tf1_in = 0;
        flag_tf1 = 0;
    }

    if(flag_sw_tf == 0)
    {
        Time_TF2 = 0;
        flag_tf2_in = 0;
        flag_tf2 = 0;
    }
    else
    {
        if(flag_tf2_befor == 0)
        {
            Time_TF2 = 200;
            flag_tf2_in = 1;
        }
        else
        {
            Time_TF2 = 0;
            flag_tf2_in = 0;
            flag_tf2 = 0;
        }
    }
}

void TF1_Detection(void)
{
    static u8 tf1_cnt = 0;

    flag_tf1_now = TF1_INPUT;
    if(flag_tf1_now != flag_tf1_befor)
    {
        tf1_cnt++;
        time_tf1_Detection = 20; //200ms
        if(tf1_cnt > 3)
        {
            tf1_cnt = 0;
            flag_tf1_befor = TF1_INPUT;
            flag_tf1_now   = TF1_INPUT;
            if(flag_tf1_now == 0)
            {
                Time_TF1 = 200;
                flag_tf1_in = 1;
            }
            else if(flag_tf1 == 0)
            {
                Time_TF1 = 0;
                flag_tf1_in = 0;
            }
            else
            {
                Time_TF1 = 1000; //10s
                flag_tf1_in = 0;
            }
        }
    }
    else
    {
        tf1_cnt = 0;
    }
    if(flag_tf1_in == 1 && Time_TF1 == 0)
    {
        flag_tf1 = 1;
        //车辆传感器信号检测有效,解除自动模式,停止时间监测,受信机恢复到初始状态
        FG_auto_out = 0;
        TIME_auto_out = 0;
        TIME_auto_close = 0;
        FG_auto_open_time = 0;
        FG_auto_manual_mode = 0;
        Manual_override_TIMER = 0;
        auto_receive_cnt = 0;
        time_receive_auto = 0;
        Time_Check_AutoSignal = 0;
        Time_NoCheck_AutoSignal = 0;
    }
    else if(flag_tf1_in == 0 && Time_TF1 == 0)
    {
        flag_tf1 = 0;
    }
}

void TF2_Detection(void)
{
    static u8 tf2_cnt = 0;

    flag_tf2_now = TF2_INPUT;
    if(flag_tf2_now != flag_tf2_befor)
    {
        tf2_cnt++;
        time_tf2_Detection = 20; //200ms
        if(tf2_cnt > 3)
        {
            tf2_cnt = 0;
            flag_tf2_befor = TF2_INPUT;
            flag_tf2_now   = TF2_INPUT;
            if(flag_tf2_now == 0)
            {
                Time_TF2 = 200;
                flag_tf2_in = 1;
            }
            else if(flag_tf2 == 0)
            {
                Time_TF2 = 0;
                flag_tf2_in = 0;
            }
            else
            {
                Time_TF2 = 1000; //10s
                flag_tf2_in = 0;
            }
        }
    }
    else
    {
        tf2_cnt = 0;
    }
    if(flag_tf2_in == 1 && Time_TF2 == 0)
    {
        flag_tf2 = 1;
        //车辆传感器信号检测有效,解除自动模式,停止时间监测,受信机恢复到初始状态
        FG_auto_out = 0;
        TIME_auto_out = 0;
        TIME_auto_close = 0;
        FG_auto_open_time = 0;
        FG_auto_manual_mode = 0;
        Manual_override_TIMER = 0;
        auto_receive_cnt = 0;
        time_receive_auto = 0;
        Time_Check_AutoSignal = 0;
        Time_NoCheck_AutoSignal = 0;
    }
    else if(flag_tf2_in == 0 && Time_TF2 == 0)
    {
        flag_tf2 = 0;
    }
}

void SwTf_Input_Detection(void)
{
    if(Time_SwDetection == 0)   Dip_Sw_Detection();
    if(time_tf1_Detection == 0) TF1_Detection();
    if(flag_sw_tf == 0)
    {
        flag_tf2 = 0;
        Time_TF2 = 0;
        flag_tf2_in = 0;
    }
    else
    {
        if(time_tf2_Detection == 0) TF2_Detection();
    }
}
