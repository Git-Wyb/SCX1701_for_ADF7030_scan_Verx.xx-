#ifndef __EEPROM_H__
#define __EEPROM_H__
/************************************************/
/*  FILE        :eeprom.h                       */
/*  DATE        :June, 2011                     */
/*  DESCRIPTION :routine for VHF2011            */
/*  CPU TYPE    :STM8S207C8                     */
/*  Programmer	:Gong Dong Sheng                */
/*  Mark        :0x4000~0x47FF                  */
/************************************************/
void InitialFlashReg(void);
void UnlockFlash(unsigned char Type); // ½âËøflash
void LockFlash(unsigned char Type);   // Ëø¶¨´æ´¢Æ÷

unsigned char ReadByteEEPROM(unsigned long Addr);             // ¶ÁÈ¡1×Ö½Ú
void WriteByteToFLASH(unsigned long Addr, unsigned char Dat); // Ð´ÈëÒ»×Ö½Ú
void EraseByteFLASH(unsigned int Addr);                       // ²Á³ýeeprom

void eeprom_save(void);
void eeprom_sys_load(void);

void ALL_ID_EEPROM_Erase(void);
void ID_EEPROM_write(void);
void ID_SCX1801_EEPROM_write(u32 id);
void Delete_GeneralID_EEPROM(u32 id);
void ID_EEPROM_write_0x00(void);
void ID_Login_EXIT_Initial(void);
void ID_learn(void);
void eeprom_write_byte(u16 addr,u8 data);
void Read_HisData(u16 padd,u8 rnum);
u32 R_ID(u8 *pid);
/************************************************/

#define UNLOCK_FLASH_TYPE ((unsigned char)0x00)
#define UNLOCK_EEPROM_TYPE ((unsigned char)0x01)

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%                  EEPROM MAP
/*
  2Kbyte EEPROM ADDR:                  0x1000 - 0x17FF.
  store Login 255 ID ADDR:             0x1000 - 0x12FC.
  <Free Space ADDR>:                   0x12FD - 0x13A8.
  store Set Automatic CLOSE time ADDR: 0x13AA.
  store Set Buzzer Switch ADDR:        0x13AB.
  store Communication ID ADDR:         0x13AC - 0x13AE.
  store Login ID Number ADDR:          0x13AF - 0x13B0.
  store Operation History Number ADDR: 0x13B1
  store History ADDR offset:           0x13B2 - 0x13B3
  store Operation History ADDR:        0x13B4 - 0x17FF. 100 records
*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define addr_eeprom_sys 0x1000 // SYS=0x4000~0x400F

#define AddrEeprom_RssiSet       0x3A9

#define AddrEeprom_AutoOverTime  0x3AA
#define AddrEeprom_BuzzerSwitch  0x3AB

#define AddrEeprom_CommIDH       0x3AC //store Communication ID
#define AddrEeprom_CommIDM       0x3AD //Communication ID
#define AddrEeprom_CommIDL       0x3AE //Communication ID
#define AddrEeprom_IDPcsH        0x3AF //store ID Number
#define AddrEeprom_IDPcsL        0x3B0 //ID Number
#define AddrEeprom_HisNum        0x3B1 //store Operation History Number
#define AddrEeprom_HisOffsetH    0x3B2 //store History ADDR offset
#define AddrEeprom_HisOffsetL    0x3B3
#define AddrEeprom_StartHistory  0x3B4 //store Operation History ADDR Start
#define AddrEeprom_MAX           0x7FF

#endif
