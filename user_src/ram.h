#ifndef __RAM_H__
#define __RAM_H__
//CPU?????
//?????ADF030???????
//#define TX_TEST_CARRIER_MODE 0
#include "initial.h" // 锟斤拷始锟斤�? 预锟斤拷锟斤�?


#define ID_Max_PCS   255

//#define DEF_test_MAX_32pcs
extern u8 DEF_APP_TX_freq;



extern uFLAG FLAG_test1;
//************************************************
#define FLAG_test1_BYTE FLAG_test1.BYTE
//------------------------------------------------
#define BIT_SIO FLAG_test1.BIT.Bit0
#define FG_auto_out FLAG_test1.BIT.Bit1
#define FG_OUT_OPEN_CLOSE FLAG_test1.BIT.Bit2 //锟斤拷史锟斤拷录   锟斤拷锟斤拷锟绞诧拷锟斤拷锟斤拷
#define FG_auto_open_time FLAG_test1.BIT.Bit4
#define FG_auto_manual_mode FLAG_test1.BIT.Bit5 //1=auto,0=manual
#define FG_Receiver_LED_RX FLAG_test1.BIT.Bit6
#define FG_First_auto FLAG_test1.BIT.Bit7
//************************************************
//************************************************
extern volatile uFLAG FLAG_test;
#define FLAG_test_BYTE FLAG_test.BYTE
//------------------------------------------------
#define FLAG_Receiver_Scanning FLAG_test.BIT.Bit0
#define FG_test_tx_1010 FLAG_test.BIT.Bit1
#define X_HIS FLAG_test.BIT.Bit2 //鍘嗗彶璁板綍   �?鐮佺巼娴嬭瘯�?
#define FG_test_tx_on FLAG_test.BIT.Bit3
#define FG_test_tx_off FLAG_test.BIT.Bit4
#define FG_test_mode FLAG_test.BIT.Bit5
#define FG_test1 FLAG_test.BIT.Bit6
#define FG_test_rx FLAG_test.BIT.Bit7
//************************************************
//************************************************
extern volatile uFLAG FLAG0;
#define FLAG0_BYTE FLAG0.BYTE
//------------------------------------------------
#define FLAG_Receiver_IDCheck FLAG0.BIT.Bit0
#define FLAG_Signal_DATA_OK FLAG0.BIT.Bit1
#define FLAG_APP_RX FLAG0.BIT.Bit2
#define FLAG_IDCheck_OK FLAG0.BIT.Bit3
#define FLAG_ID_Erase_Login FLAG0.BIT.Bit4
#define FLAG_ID_Erase_Login_PCS FLAG0.BIT.Bit5
#define FLAG_ID_Login FLAG0.BIT.Bit6
#define FLAG_ID_Login_OK FLAG0.BIT.Bit7
//************************************************
extern volatile uFLAG FLAG1;
#define FLAG1_BYTE FLAG1.BYTE
//------------------------------------------------
#define FLAG_Receiver_BEEP FLAG1.BIT.Bit0
#define FLAG_ID_Login_EXIT FLAG1.BIT.Bit1
#define FLAG_ID_Login_OK_bank FLAG1.BIT.Bit2
#define FG_beep_on FLAG1.BIT.Bit3
#define FG_beep_off FLAG1.BIT.Bit4
#define FG_allow_out FLAG1.BIT.Bit5
#define FG_NOT_allow_out FLAG1.BIT.Bit6
#define FG_10ms FLAG1.BIT.Bit7
//************************************************
extern volatile uFLAG FLAG2;
#define FLAG2_BYTE FLAG2.BYTE
//------------------------------------------------
#define FLAG_ID_SCX1801_Login FLAG2.BIT.Bit0   //用于与集中�?�信机�?�信的ID，只有一个ID
#define FG_ID_SCX1801_Login_BEEP FLAG2.BIT.Bit1
#define FLAG_APP_TX_fromUART FLAG2.BIT.Bit2
#define FLAG_APP_TX_fromOUT FLAG2.BIT.Bit3
#define FLAG_APP_TX FLAG2.BIT.Bit4
#define FLAG_APP_RXstart FLAG2.BIT.Bit5
#define FLAG_APP_TX_once FLAG2.BIT.Bit6
#define FLAG_Key_TP3 FLAG2.BIT.Bit7

//************************************************
extern volatile uFLAG FLAG3;
#define FLAG3_BYTE FLAG3.BYTE
//------------------------------------------------
#define Flag_ERROR_Read FLAG3.BIT.Bit0
//#define  FLAG3.BIT.Bit1
#define Flag_shutter_stopping FLAG3.BIT.Bit2
#define FLAG_APP_TX_fromUART_err_read FLAG3.BIT.Bit3
//#define  FLAG3.BIT.Bit4
//#define  FLAG3.BIT.Bit5
//#define  FLAG3.BIT.Bit6
//#define  FLAG3.BIT.Bit7

//************************************************


extern u16 X_COUNT;
extern u16 X_ERR; //记录错误的个�?
extern u16 X_ERRTimer;

//extern u8 SIO_cnt;
//extern u8 SIO_buff[16];
//extern u8 SIO_DATA[16];
//extern u8 Tx_Rx_mode;

extern u8 TIME_10ms;
extern u16 TIMER1s;
extern u16 TIMER300ms;
extern u16 TIMER18ms;
extern u16 TIMER250ms_STOP;
extern u16 TIME_auto_out;
extern u16 TIME_auto_close;
//extern u16 time_3sec;
//extern u32 ID_Receiver_DATA[256]; //写入EEPROM ID缓存
extern u8 ID_Receiver_DATA[256][3]; //写入EEPROM ID缓存
extern u32 ID_SCX1801_DATA;
extern u16 ID_DATA_PCS;
extern u32 DATA_Packet_ID;
extern u8 DATA_Packet_Control;
extern u8 DATA_Packet_Contro_buf; //2015.3.24�?�?
extern u32 ID_Receiver_Login;
//extern u8 TIME_EMC;

//extern u16 INquiry;
extern u16 TIME_Receiver_Login_restrict;
extern u8 COUNT_Receiver_Login;
extern u16 TIME_Receiver_Login;
extern u16 TIME_Login_EXIT_rest;
extern u16 TIME_Receiver_Login_led;
extern u16 TIME_ID_SCX1801_Login ;


extern u8 TIME_OUT_OPEN_CLOSE;
extern u16 TIME_Receiver_LED_OUT;
extern u16 TIME_Login_EXIT_Button;
extern u16 Manual_override_TIMER;
extern u16 time_Login_exit_256;
//extern u16 TIME_Fine_Calibration; //绐勫甫涓嬩腑棰戞护娉㈠櫒100KHz绮炬�?

//extern u8 Count_key_SW3;
//extern u8 Display_key_SW3;

//extern u16 TIME_TestNo91;
extern u16 TIME_power_led;


extern ADF70XX_REG_T ROM_adf7030_value[16];
extern const ADF70XX_REG_T Default_adf7030_value[16];
/**********LCD******************/
//extern uFLAG FlagLCDUpdate;

//#define LCDUpdateIDFlag FlagLCDUpdate.BIT.Bit0
//#define LCDUpdateRISSFlag FlagLCDUpdate.BIT.Bit1
//#define LCDFirstDisplayFlag FlagLCDUpdate.BIT.Bit2
/**********RSSI*********************/
extern short RAM_RSSI_AVG;
extern long RAM_RSSI_SUM;
extern u8 RSSI_Read_Counter;
extern u8 Flag_RSSI_Read_Timer;

#define SPI_SEND_BUFF_LONG 300
#define SPI_REV_BUFF_LONG 40

extern const u8 ADF7030Cfg[];
extern const u8 ADF7030Cfg_load[];
extern const u8 ADF7030Cfg_4dot8k[];
extern const u8 *ADF7030Cfg_pointer;

u32 CFG_SIZE(void);

#define OPEN_LONG 12
//extern u8 CONST_TXPACKET_DATA_20000AF0[OPEN_LONG];
extern u8 CONST_TXPACKET_DATA_20000AF0[28];


extern u32 GENERIC_PKT_TEST_MODES0_32bit_20000548;
//extern const u8 TEST_MODES0_para[5];
//extern u32 RADIO_DIG_TX_CFG0_32bit_20000304;
#define AFCCONFIG_LONG 4
//extern const u8 CONST_AFC_Configuration_400041F8[4];
extern u16 PA_POWER_OUT[2][18];
extern u32 PROFILE_CH_FREQ_32bit_200002EC;
extern u32 PROFILE_RADIO_AFC_CFG1_32bit_2000031C;
extern u32 PROFILE_RADIO_DATA_RATE_32bit_200002FC;
extern u32 PROFILE_GENERIC_PKT_FRAME_CFG1_32bit_20000500;
extern const u32 PROFILE_CH1_FREQ_32bit_429HighSpeed;
extern const u32 PROFILE_CH2_FREQ_32bit_429HighSpeed;

extern u8 Flag_429M_EndStop;
extern u8 Channels;

extern Basic_Un Status_Un;
extern Flag_Un StaFlag_Un;
extern Flag_Un flag1_Un;
extern u8 auto_over_time;
extern u16 Time_NoCheck_AutoSignal;
extern u16 Time_Check_AutoSignal;
extern u8 auto_receive_cnt;
extern u8 time_receive_auto;
extern u8 Time_StateDetection;
extern u8 sta_change;
extern u8 time_close_auto_beep;
extern u8 beep_num;
extern u16 TIME_BEEP_on;
extern u16 TIME_BEEP_off;
extern u16 TIME_BEEP_freq;
extern u16 BASE_TIME_BEEP_on;
extern u16 BASE_TIME_BEEP_off;

#define Beep_Switch         StaFlag_Un.FlagByte_bit0    //1:用于标志:打开蜂鸣器
#define Allow_BeepOn_Flag   StaFlag_Un.FlagByte_bit1   //1:用于标志:自动受信结束时执行动作,允许打开蜂鸣器
#define app_tx_en           StaFlag_Un.FlagByte_bit2   //用于开启429MHz发送状态
#define save_fall_time_flag StaFlag_Un.FlagByte_bit3   //用于标志:1 设定了自动下降时间，需写入eeprom
#define save_beep_flag      StaFlag_Un.FlagByte_bit4   //用于标志:1 设定了蜂鸣器开关，需写入eeprom
#define operat_action_flag  StaFlag_Un.FlagByte_bit5   //用于标志:1 正常操作的
#define operat_enter_flag   StaFlag_Un.FlagByte_bit6   //用于标志:1 进入正常操作的动作中.
#define close_action_auto_beep_flag  StaFlag_Un.FlagByte_bit7   //用于标志:1 自动关闭动作中,开启蜂鸣器

#define FG_beep_on_Motor    flag1_Un.FlagByte_bit0
#define FG_beep_off_Motor   flag1_Un.FlagByte_bit1
#define recv_429code_flag   flag1_Un.FlagByte_bit2  //1：接收到429MHz的闭操作指令
#define close_action_beep_flag   flag1_Un.FlagByte_bit3  //1：接收到429MHz的闭操作指令后动作中信号输入，开启蜂鸣器
#define flag_rx_done        flag1_Un.FlagByte_bit4
#define flag_update_his     flag1_Un.FlagByte_bit5
#define flag_rerx           flag1_Un.FlagByte_bit6
#define flag_test_rtc       flag1_Un.FlagByte_bit7

#define Save_Disable_Beep 0xAA

void SPI_INIT(void);
u32 ADF7030_GET_MISC_FW(void); //??MISC_FW?????
void ADF7030_WRITING_PROFILE_FROM_POWERON(void);
void ADF7030_TRANSMITTING_FROM_POWEROFF(void);
void ADF7030_RECEIVING_FROM_POWEROFF(void);
void SCAN_RECEIVE_PACKET(void);
u32 ConfigurationLen(void);

#define BUFFMAX 10
#define HIS_MAX 100

typedef struct
{
    UINT8 YY;
    UINT8 MM;
    UINT8 DD;
    UINT8 HH;
    UINT8 MI;
    UINT8 SS;
    UINT8 IDD[3];
    UINT8 CODE;
    UINT8 RS;
}STRUCT_DATE;

typedef union
{
    u8 history_buff[11];
    STRUCT_DATE History_s;
}HIS_STU;

extern Wireless_Body Struct_DATA_Packet_Contro,Struct_DATA_Packet_Contro_buf;
extern Wireless_Body Uart_Struct_DATA_Packet_Contro,Last_Uart_Struct_DATA_Packet_Contro;
extern u8 Struct_DATA_Packet_Contro_fno;
//extern u16 TIMER_Semi_open;
//extern u16 TIMER_Semi_close;
extern u8 FLAG__Semi_open_T;
extern u8 FLAG__Semi_close_T;
extern u16 TIME_APP_TX_fromOUT;
extern u8 Time_APP_blank_TX;
extern u16 Time_APP_RXstart;

extern u8 TIME_TX_RSSI_Scan;
extern u8 Flag_TX_ID_load;
extern u8 TIME_ERROR_Read_once_again;
extern  unsigned char Send_err_com[7] ;
extern u8 Time_error_read_gap;
extern u16 Time_error_read_timeout;
extern u8 ERROR_Read_sendTX_count;
extern u8 ERROR_Read_sendTX_packet;
extern u8 Flag_ERROR_Read_once_again;

extern u16 time_sw;
extern u8 uart_rx_data;
extern u8 FLAG_RTC_RTC;
extern unsigned char g8563_Store[7];
extern u8 FLAG_CLOCK;
extern STRUCT_DATE History_STU;
extern u8 his_buff[BUFFMAX * 11];
extern u8 b_offset;
extern HIS_STU HIS_DATA[BUFFMAX];
extern u8 His_Num;
extern u16 His_AddrOffset;
extern u16 ID_Nums;

#endif
