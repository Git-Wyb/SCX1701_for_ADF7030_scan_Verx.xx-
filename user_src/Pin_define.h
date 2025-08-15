#ifndef __PIN_DEFINE_H__
#define __PIN_DEFINE_H__
/***********************************************************************/
/*  File Name   :Pin_Define.h                                          */
/*  DATE        :Mar, 2014                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  DESCRIPTION :                                                      */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include "Timer.h"
//以下是IO定义
/********************LED寄存器*****************************************/
//extern u8 LED_Cache;
#define LED_ON 1
#define LED_OFF 0

#define Receiver_LED_TX PB_ODR_ODR2       //PC_ODR_ODR1       // Output   受信机送信指示  高电平有效
#define Receiver_LED_TX_direc PB_DDR_DDR2 // Output   受信机送信指示  高电平有效
#define Receiver_LED_TX_CR1 PB_CR1_C12    // Output   受信机送信指示  高电平有效
#define Receiver_LED_TX_CR2 PB_CR2_C22    //输出频率

#define Receiver_LED_RX PB_ODR_ODR3       //PC_ODR_ODR0       // Output   受信机受信指示  高电平有效
#define Receiver_LED_RX_direc PB_DDR_DDR3 // Output   受信机受信指示  高电平有效
#define Receiver_LED_RX_CR1 PB_CR1_C13    // Output   受信机受信指示  高电平有效
#define Receiver_LED_RX_CR2 PB_CR2_C23    //输出频率

#define Receiver_LED_OUT PA_ODR_ODR3        //PB_ODR_ODR1              // Output   受信机继电器动作输出  高电平有效
#define Receiver_LED_OUT_direc PA_DDR_DDR3 // Output   受信机继电器动作输出  高电平有效
#define Receiver_LED_OUT_CR1 PA_CR1_C13    // Output   受信机继电器动作输出  高电平有效
#define Receiver_LED_OUT_CR2 PA_CR2_C23    //输出频率

/******************以下是ADF7030-1寄存器************************************/
/* ADF7030-1 register interface */
#define ADF7030_REST PD_ODR_ODR0//ADF7030_REST_Cache
#define ADF7030_REST_DDR PD_DDR_DDR0//ADF7030_REST_Cache
#define ADF7030_REST_CR1 PD_CR1_C10//ADF7030_REST_Cache
#define ADF7030_REST_CR2 PD_CR2_C20//ADF7030_REST_Cache

#define ADF7030_GPIO2 PC_IDR_IDR5
#define ADF7030_GPIO2_DDR PC_DDR_DDR5
#define ADF7030_GPIO2_CR1 PC_CR1_C15
#define ADF7030_GPIO2_CR2 PC_CR2_C25

#define ADF7030_GPIO3 PD_IDR_IDR4
#define ADF7030_GPIO3_DDR PD_DDR_DDR4
#define ADF7030_GPIO3_CR1 PD_CR1_C14
#define ADF7030_GPIO3_CR2 PD_CR2_C24
//
#define ADF7030_GPIO4 PC_IDR_IDR4
#define ADF7030_GPIO4_DDR PC_DDR_DDR4
#define ADF7030_GPIO4_CR1 PC_CR1_C14
#define ADF7030_GPIO4_CR2 PC_CR2_C24

#define ADF7030_GPIO5 PC_IDR_IDR6
#define ADF7030_GPIO5_DDR PC_DDR_DDR6
#define ADF7030_GPIO5_CR1 PC_CR1_C16
#define ADF7030_GPIO5_CR2 PC_CR2_C26

#define ADF7030CLK ADF7030_GPIO4
#define ADF7030DATA ADF7030_GPIO5
/********************天线切换 CG2214M6控制引脚寄存器*****************************************/
#define CG2214M6_VC1 PA_ODR_ODR5//PC_ODR_ODR1     //VC1
#define CG2214M6_VC1_DDR PA_DDR_DDR5//PC_DDR_DDR1 //方向
#define CG2214M6_VC1_CR1 PA_CR1_C15//PC_CR1_C11  //模式MODE
#define CG2214M6_VC1_CR2 PA_CR2_C25//PC_CR2_C21  //输出频率或者中断控制

#define CG2214M6_VC2 PB_ODR_ODR0//PC_ODR_ODR0     //VC2
#define CG2214M6_VC2_DDR PB_DDR_DDR0//PC_DDR_DDR0 //方向
#define CG2214M6_VC2_CR1 PB_CR1_C10//PC_CR1_C10  //模式MODE
#define CG2214M6_VC2_CR2 PB_CR2_C20//PC_CR2_C20  //输出频率或者中断控制

#define CG2214M6_RF1_USE  \
    {                     \
        CG2214M6_VC1 = 0; \
        CG2214M6_VC2 = 1; \
    }
#define CG2214M6_RF2_USE  \
    {                     \
        CG2214M6_VC1 = 1; \
        CG2214M6_VC2 = 0; \
    }
#define CG2214M6_USE_T CG2214M6_RF1_USE
#define CG2214M6_USE_R CG2214M6_RF2_USE
/******************以下是KEY寄存器*******输入*****************************/
//#define KEY_SW2 PA_IDR_IDR4
//#define KEY_SW2_DDR PA_DDR_DDR4
//#define KEY_SW2_CR1 PA_CR1_C14
//#define KEY_SW2_CR2 PA_CR2_C24
//
//#define KEY_SW3 PA_IDR_IDR5
//#define KEY_SW3_DDR PA_DDR_DDR5
//#define KEY_SW3_CR1 PA_CR1_C15
//#define KEY_SW3_CR2 PA_CR2_C25
//
//#define KEY_SW4 PA_IDR_IDR2
//#define KEY_SW4_DDR PA_DDR_DDR2
//#define KEY_SW4_CR1 PA_CR1_C12
//#define KEY_SW4_CR2 PA_CR2_C22

// #define Receiver_Login PC_IDR_IDR6       // Input   受信机登录键   低电平有效
// #define Receiver_Login_direc PC_DDR_DDR6 // Input   受信机登录键   低电平有效
// #define Receiver_Login_CR1 PC_CR1_C16    // Input   受信机登录键   低电平有效

#define Receiver_Login PA_IDR_IDR2       // Input   受信机登录键   低电平有效
#define Receiver_Login_direc PA_DDR_DDR2 // Input   受信机登录键   低电平有效
#define Receiver_Login_CR1 PA_CR1_C12    // Input   受信机登录键   低电平有效
#define Receiver_Login_CR2 PA_CR2_C22    // Input   受信机登录键   低电平有效

#define KEY_Empty 0
#define KEY_SW2_Down 1
#define KEY_SW3_Down 2
#define KEY_SW4_Down 3

/**受信机使用的IO HA 异常信号 寄存器 */
// #define HA_L_signal PA_IDR_IDR4       // Input   HA 下限信号   低电平有效
// #define HA_L_signal_direc PA_DDR_DDR4 // Input   HA 下限信号   低电平有效
// #define HA_L_signal_CR1 PA_CR1_C14    // Input   HA 下限信号   低电平有效
// #define HA_L_signal_CR2 PA_CR2_C24

// #define HA_ERR_signal PA_IDR_IDR5       // Input   HA 异常信号  低电平有效
// #define HA_ERR_signal_direc PA_DDR_DDR5 // Input   HA 异常信号  低电平有效
// #define HA_ERR_signal_CR1 PA_CR1_C15    // Input   HA 异常信号  低电平有效
// #define HA_ERR_signal_CR2 PA_CR2_C25

// #define HA_Sensor_signal PA_IDR_IDR3       // Input   HA 传感器信号  低电平有效
// #define HA_Sensor_signal_direc PA_DDR_DDR3 // Input   HA 传感器信号  低电平有效
// #define HA_Sensor_signal_CR1 PA_CR1_C13    // Input   HA 传感器信号  低电平有效
// #define HA_Sensor_signal_CR2 PA_CR2_C23

/**WORK/TEST切换引脚寄存器*****/
#define WORK_TEST PC_IDR_IDR0//PB_IDR_IDR0     // Input 受信机测试脚  高电平有效
#define WORK_TEST_DDR PC_DDR_DDR0//PB_DDR_DDR0 // Input 受信机测试脚  高电平有效
#define WORK_TEST_CR1 PC_CR1_C10//PB_CR1_C10  // Input 受信机测试脚  高电平有效
#define WORK_TEST_CR2 PC_CR2_C20//PB_CR2_C20  // Input 受信机测试脚  高电平有效

#define Receiver_test PC_IDR_IDR0//PB_IDR_IDR0       // Input 受信机测试脚  高电平有效
#define Receiver_test_direc PC_DDR_DDR0//PB_DDR_DDR0 // Input 受信机测试脚  高电平有效
#define Receiver_test_CR1 PC_CR1_C10 //PB_CR1_C10    // Input 受信机测试脚  高电平有效

/*
#define TP3 PA_IDR_IDR4
#define TP3_DDR PA_DDR_DDR4
#define TP3_CR1 PA_CR1_C14
#define TP3_CR2 PA_CR2_C24
*/

#define TP4     PC_IDR_IDR1 //PA_IDR_IDR5
#define TP4_DDR PC_DDR_DDR1 //PA_DDR_DDR5
#define TP4_CR1 PC_CR1_C11  //PA_CR1_C15
#define TP4_CR2 PC_CR2_C21  //PA_CR2_C25

/******************以下是data寄存器************************************/
#define Receiver_vent   1 //Receiver_vent_Cache       // Input   受信机换气联动ON/OFF
#define Receiver_vent_direc Receiver_vent_Cache // Input   受信机换气联动ON/OFF
#define Receiver_vent_CR1 Receiver_vent_Cache   // Input   受信机换气联动ON/OFF

#define PIN_BEEP PB_ODR_ODR1       //PA_ODR_ODR0       // Output   蜂鸣器
#define PIN_BEEP_direc PB_DDR_DDR1 // Output   蜂鸣器
#define PIN_BEEP_CR1 PB_CR1_C11    // Output   蜂鸣器

#define Receiver_OUT_OPEN PD_ODR_ODR3       // Output   受信机继电器OPEN  高电平有效
#define Receiver_OUT_OPEN_direc PD_DDR_DDR3 // Output   受信机继电器OPEN  高电平有效
#define Receiver_OUT_OPEN_CR1 PD_CR1_C13    // Output   受信机继电器OPEN  高电平有效

#define Receiver_OUT_CLOSE PD_ODR_ODR2       // Output   受信机继电器close  高电平有效
#define Receiver_OUT_CLOSE_direc PD_DDR_DDR2 // Output   受信机继电器close  高电平有效
#define Receiver_OUT_CLOSE_CR1 PD_CR1_C12    // Output   受信机继电器close  高电平有效

#define Receiver_OUT_STOP PD_ODR_ODR1       // Output   受信机继电器stop  高电平有效
#define Receiver_OUT_STOP_direc PD_DDR_DDR1 // Output   受信机继电器stop  高电平有效
#define Receiver_OUT_STOP_CR1 PD_CR1_C11    // Output   受信机继电器stop  高电平有效

#define Receiver_OUT_VENT PA_ODR_ODR4       // Output   受信机继电器VENT  高电平有效
#define Receiver_OUT_VENT_direc PA_DDR_DDR4 // Output 受信机继电器VENT  高电平有效
#define Receiver_OUT_VENT_CR1 PA_CR1_C14    // Output 受信机继电器VENT  高电平有效


// #define Inverters_OUT PA_IDR_IDR2       // 输入   继电器输出信号反向   低电平有效
// #define Inverters_OUT_direc PA_DDR_DDR2 // 输入   继电器输出信号反向   低电平有效
// #define Inverters_OUT_CR1 PA_CR1_C12    // 输入   继电器输出信号反向   低电平有效


/* 拨码开关 <自动选择器从关闭到关闭信号的时间设置功能> */
#define SW_1_DDR PE_DDR_DDR2
#define SW_1_CR1 PE_CR1_C12
#define SW_1_CR2 PE_CR2_C22

#define SW_2_DDR PE_DDR_DDR3
#define SW_2_CR1 PE_CR1_C13
#define SW_2_CR2 PE_CR2_C23

#define SW_3_DDR PE_DDR_DDR4
#define SW_3_CR1 PE_CR1_C14
#define SW_3_CR2 PE_CR2_C24

#define SW_4_DDR PE_DDR_DDR5
#define SW_4_CR1 PE_CR1_C15
#define SW_4_CR2 PE_CR2_C25

#define SW_1   PE_IDR_IDR2
#define SW_2   PE_IDR_IDR3
#define SW_3   PE_IDR_IDR4
#define SW_4   PE_IDR_IDR5
//开关编码
#define SW_CODE_0   0x0F   /* 10s */
#define SW_CODE_1   0x0E   /* 20s */
#define SW_CODE_2   0x0D   /* 30s */
#define SW_CODE_3   0x0C   /* 40s */
#define SW_CODE_4   0x0B   /* 50s */
#define SW_CODE_5   0x0A   /* 60s */
#define SW_CODE_6   0x09   /* 70s */
#define SW_CODE_7   0x08   /* 80s */
#define SW_CODE_8   0x07   /* 90s */
#define SW_CODE_9   0x06   /* 100s */


#define Abnormal_Signal_DDR     PE_DDR_DDR0
#define Abnormal_Signal_CR1     PE_CR1_C10
#define Abnormal_Signal_CR2     PE_CR2_C20
#define Abnormal_Signal         PE_IDR_IDR0     //异常信号，为0表示有异常信号

#define Lower_Limit_Signal_DDR  PE_DDR_DDR1
#define Lower_Limit_Signal_CR1  PE_CR1_C11
#define Lower_Limit_Signal_CR2  PE_CR2_C21
#define Lower_Limit_Signal      PE_IDR_IDR1     //下限信号，为0表示有下限信号

#define Action_Signal_DDR       PE_DDR_DDR2
#define Action_Signal_CR1       PE_CR1_C12
#define Action_Signal_CR2       PE_CR2_C22
#define Action_Signal           PE_IDR_IDR2     //动作中信号，为0表示动作信号
/*
//测试调试用
#define Action_Signal_DDR       PC_DDR_DDR1
#define Action_Signal_CR1       PC_CR1_C11
#define Action_Signal_CR2       PC_CR2_C21
#define Action_Signal           PC_IDR_IDR1     //动作中信号，为0表示动作信号 */


/* 自动下降时间设定 */
#define CLOSE_AUTO_DECLINE  0x90  //不执行自动下降
#define AUTO_TIME_10S       0x88  /* 10s */
#define AUTO_TIME_20S       0x98  /* 20s */
#define AUTO_TIME_30S       0x84  /* 30s */
#define AUTO_TIME_40S       0x94  /* 40s */
#define AUTO_TIME_50S       0x8C  /* 50s */
#define AUTO_TIME_60S       0x9C  /* 60s */
#define AUTO_TIME_70S       0x82  /* 70s */
#define AUTO_TIME_80S       0x92  /* 80s */
#define AUTO_TIME_90S       0x8A  /* 90s */
#define AUTO_TIME_100S      0x9A  /* 100s */
#define AUTO_TIME_110S      0x86  /* 110s */
#define AUTO_TIME_120S      0x96  /* 120s */

#define BUZZER_OFF_COMMAND  0xD0  /* 不允许打开蜂鸣器 */
#define BUZZER_ON_COMMAND   0xC8  /* 允许打开蜂鸣器 */



/* 429MHz送信数据 */
#define Tx_Open_Status         0xD1  /* 开状态 */
#define Tx_Close_Status        0xD2  /* 闭状态 */
#define Tx_Open_Action_Status  0xD3  /* 开动作中状态 */
#define Tx_Close_Action_Status 0xD4  /* 闭动作中状态 */
#define Tx_Open_Action_Auto    0xD5  /* 开动作中状态(自动受信时) */
#define Tx_Close_Action_Auto   0xD6  /* 闭动作中状态(自动受信时) */
#define Tx_Abnormal_Status     0xD7  /* 异常1状态 */
#define Tx_Setting_Status      0xD8  /* 时间设定和蜂鸣器ON/OFF设定时,送信 */
#define Tx_Open_StatusNG       0xD9  /* 开状态(无效),自动信号无效的状态 */
#define Tx_Close_StatusNG      0xDA  /* 闭状态(无效),自动信号无效的状态 */
#define Tx_Open_Action_StatusNG     0xDB  /* 开动作中(无效),自动信号无效的状态 */
#define Tx_Close_Action_StatusNG    0xDC  /* 闭动作中(无效),自动信号无效的状态 */
#define Tx_Abnormal_StatusNG        0xDD  /* 异常1状态(无效),自动信号无效的状态*/

/*********************************************************************************/

#endif

