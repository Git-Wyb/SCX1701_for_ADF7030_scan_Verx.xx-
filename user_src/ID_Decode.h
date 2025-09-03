#ifndef __ID_DECODE_H__
#define __ID_DECODE_H__
/***********************************************************************/
/*  FILE        :ID_Decode.h                                           */
/*  DATE        :Mar, 2014                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  DESCRIPTION :                                                      */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/

//void EXIT_init(void);
//void ID_Decode_function(void);
void ID_Decode_IDCheck(void);
void Signal_DATA_Decode(UINT8 NUM_Type);
void eeprom_IDcheck(void);
void BEEP_and_LED(void);
void ID_Decode_OUT(void);
void Receiver_BEEP(void);
void TEST_beep(void);
void BEEP_Module(UINT16 time_beepON, UINT16 time_beepOFF);
void TIM3_init(void);
void Tone_OFF(void);
void Tone_ON(void);
void Action_Signal_Detection(void);
void APP429M_Tx_State(void);
u32 ID_Receiver_DATA_READ(u8 *address);
void Beep_Action_On(void);
void GetInitial_State(void);
void sendsta_once(void);
void BEEP_function(void);
void _ReqBuzzer(u16 d_BEEP_on,u16 d_BEEP_off,u16 d_BEEP_freq);
void Set_OperationHistory(u32 RX_ID,u8 code);
void Freq_Scanning(void);
void DataReceive(void);
void Hisdata_Inbuff(u8 *baddr,STRUCT_DATE *pdata);
void Save_OperationHistory(u16 paddr,HIS_STU *pdata,u8 datanum);
u16 Get_IDNums(void);
u8 CheckID_Type(u32 id);
void Login_RssiSet_Init(void);

#endif
