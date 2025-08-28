/***********************************************************************/
/*  FILE        :eeprom.c                                              */
/*  DATE        :June, 2011                                            */
/*  DESCRIPTION :routine for VHF60-2011                                */
/*  CPU TYPE    :STM8S207C8                                            */
/*  Programmer	:Gong Dong Sheng                                       */
/*  Mark        :STM8S207C8鐨凜ODE绌洪棿锟�?4K                             */
/*              :STM8S207C8鐨凟EPROM鐨勫ぇ灏忎负1536瀛楄妭,锟�?3锟�?512锟�?锟�?   */
/***********************************************************************/
#include <iostm8l151c8.h> // CPU鍨嬪彿
//#include "stm8l15x.h"
#include "Pin_define.h" // 绠¤剼瀹氫箟
#include "initial.h"    // 鍒濓拷?锟斤拷?? 棰勫畾锟�?
#include "ram.h"        // RAM瀹氫箟
#include "eeprom.h"     // eeprom
#include "ID_Decode.h"
#include "uart.h" // uart

/***********************************************************************/
/*                    FLASH & EEPROM 瀵勫瓨鍣ㄥ強鎺у埗锟�?                   */
/***********************************************************************/
#define FIRST_SECURITY_KEY 0xAE
#define SECOND_SECURITY_KEY 0x56
#define ADD_EEPROM_S8 0x1000

///* FLASH_CR2 */
//#define OPT               7   /* 瀵癸拷?锟介」瀛楄妭杩涳拷?锟藉啓鎿嶏拷??*/
//#define WPRG              6   /* 瀛楃紪锟�?*/
//#define ERASE             5   /* 鍧楁摝锟�?*/
//#define FPRG              4   /* 锟�???锟藉潡缂栫▼ */
////#define NC              3
////#define NC              2
////#define NC              1
//#define PRG               0   /* 鏍囧噯鍧楃紪锟�?*/
//
///* FLASH_NCR2 */
//#define NOPT              7   /* 瀵癸拷?锟介」瀛楄妭杩涳拷?锟藉啓鎿嶏拷??*/
//#define NWPRG             6   /* 瀛楃紪锟�?*/
//#define NERASE            5   /* 鍧楁摝锟�?*/
//#define NFPRG             4   /* 锟�???锟藉潡缂栫▼ */
////#define NC              3
////#define NC              2
////#define NC              1
//#define NPRG              0   /* 鏍囧噯鍧楃紪锟�?*/
//
///* FLASH_FPR */
////#define NC              7
////#define NC              6
//#define WPB5              5   /* 鐢ㄦ埛锟�?鍔ㄤ唬鐮佷繚鎶わ拷??*/
//#define WPB4              4
//#define WPB3              3
//#define WPB2              2
//#define WPB1              1
//#define WPB0              0
//
///* FLASH_NFPR */
////#define NC              7
////#define NC              6
//#define NWPB5             5   /* 鐢ㄦ埛锟�?鍔ㄤ唬鐮佷繚鎶わ拷??*/
//#define NWPB4             4
//#define NWPB3             3
//#define NWPB2             2
//#define NWPB1             1
//#define NWPB0             0
//
///* FLASH_PUKR */
//#define MASS_PRG_KEY7     7   /* 涓荤▼搴忓瓨鍌ㄥ櫒瑙ｉ攣瀵嗛挜 */
//#define MASS_PRG_KEY6     6
//#define MASS_PRG_KEY5     5
//#define MASS_PRG_KEY4     4
//#define MASS_PRG_KEY3     3
//#define MASS_PRG_KEY2     2
//#define MASS_PRG_KEY1     1
//#define MASS_PRG_KEY0     0
//
///* FLASH_DUKR */
//#define MASS_DATA_KEY7    7   /* DATA EEPROM瑙ｉ攣瀵嗛挜 */
//#define MASS_DATA_KEY6    6
//#define MASS_DATA_KEY5    5
//#define MASS_DATA_KEY4    4
//#define MASS_DATA_KEY3    3
//#define MASS_DATA_KEY2    2
//#define MASS_DATA_KEY1    1
//#define MASS_DATA_KEY0    0
//
/* FLASH_IAPSR */
//#define NC              7
#define HVOFF 6 /* 楂樺帇缁撴潫鏍囧織 */
//#define NC              5
//#define NC              4
#define DUL 3       /* DATA EEPROM鍖哄煙瑙ｉ攣鏍囧織 */
#define EOP 2       /* 缂栫▼缁撴潫(鍐欐垨鎿﹂櫎鎿嶄綔)鏍囧織 */
#define PUL 1       /* 锟�???锟界▼搴忓瓨鍌ㄥ櫒缁撴潫鏍囧織 */
#define WR_PG_DIS 0 /* 璇曞浘鍚戯拷??淇濇姢椤佃繘琛屽啓鎿嶄綔鐨勬爣锟�?*/

#define FLASH_CR1_RESET_VALUE ((uchar)0x00)
#define FLASH_CR2_RESET_VALUE ((uchar)0x00)
#define FLASH_NCR2_RESET_VALUE ((uchar)0xFF)
#define FLASH_IAPSR_RESET_VALUE ((uchar)0x40)
#define FLASH_PUKR_RESET_VALUE ((uchar)0x00)
#define FLASH_DUKR_RESET_VALUE ((uchar)0x00)

#define FLASH_PUKR_PUK ((uchar)0xFF) /*!< Flash Program memory unprotection mask */
#define FLASH_DUKR_DUK ((uchar)0xFF) /*!< Data EEPROM unprotection mask */

//#define UNLOCK_FLASH_TYPE       (( uchar )0x00 )
//#define UNLOCK_EEPROM_TYPE      (( uchar )0x01 )

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void InitialFlashReg(void)
{ // 鍒濓拷?锟藉寲锟�?瀛樺瘎瀛樺櫒锟�?
    FLASH_CR1 = FLASH_CR1_RESET_VALUE;
    FLASH_CR2 = FLASH_CR2_RESET_VALUE;
    //FLASH_NCR2 = FLASH_NCR2_RESET_VALUE;
    FLASH_IAPSR &= (uchar)(~(1 << DUL)); // 娓呴櫎锟�?璇籇ATA鍖鸿В锟�?
    FLASH_IAPSR &= (uchar)(~(1 << PUL)); // 娓呴櫎绋嬪簭鍖鸿В锟�?
}
//------------------------------------------------
//  锟�? 2锟�?瀵嗛挜鐨勬搷浣滃簭鍒楋拷?锟藉ソ鐩稿弽
void UnlockFlash(unsigned char Type)
{ // 瑙ｉ攣flash
    if (Type == UNLOCK_FLASH_TYPE)
    { // 瑙ｉ攣绋嬪簭锟�?
        FLASH_DUKR = SECOND_SECURITY_KEY;
        FLASH_DUKR = FIRST_SECURITY_KEY;
    }
    else
    { // 瑙ｉ攣eeprom
        FLASH_DUKR = FIRST_SECURITY_KEY;
        FLASH_DUKR = SECOND_SECURITY_KEY;
    }
}
//------------------------------------------------
void LockFlash(unsigned char Type)
{ // 閿佸畾瀛樺偍锟�?
    if (Type == UNLOCK_FLASH_TYPE)
    {
        FLASH_IAPSR &= ~(1 << PUL);
    }
    else
    {
        FLASH_IAPSR &= ~(1 << DUL);
    }
}
//------------------------------------------------
uchar ReadByteEEPROM(ulong Addr)
{                                    // 浠巈eprom锟�?璇伙拷??瀛楄妭
    return (*((__far uchar *)Addr)); // Read byte
}
//------------------------------------------------
void WriteByteToFLASH(ulong Addr, uchar Dat)
{ // 鍐欏叆锟�?瀛楄妭鍒癳eprom
    *((__far uchar *)Addr) = Dat;
}
//------------------------------------------------
void EraseByteFLASH(uint Addr)
{ // 鎿﹂櫎eeprom锟�?鍐咃拷??
    *((__near uchar *)Addr) = 0x00;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void eeprom_save(void)
{
    unsigned char i, eeprom_sys_buff;

    UnlockFlash(UNLOCK_EEPROM_TYPE);
    for (i = 0; i < 16; i++)
        WriteByteToFLASH(addr_eeprom_sys + i, eeprom_sys_buff);
    LockFlash(UNLOCK_EEPROM_TYPE);
}

void ID_Receiver_DATA_WRITE(u8 *address,u32 Data)
{
    uni_rom_id xn;

    xn.IDL = Data;
    address[0] = xn.IDB[1];
    address[1] = xn.IDB[2];
    address[2] = xn.IDB[3];
}

void eeprom_sys_load(void)
{
    //unsigned char	i;
    //for (i=0;i<0X1F;i++)
    //	eeprom_sys_buff[i] = ReadByteEEPROM( addr_eeprom_sys+i );
    //--------------------------------------

    UINT16 i, j, q, p;
    UINT8 xm[3] = {0};
    uni_rom_id xn;

    His_Num = (0xFF & ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_HisNum));
    if(His_Num > HIS_MAX) His_Num = HIS_MAX;

    i = (0xFF & ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_HisOffsetH));
    j = (0xFF & ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_HisOffsetL));
    His_AddrOffset = (u16)((i << 8) | j);
    if(His_AddrOffset<AddrEeprom_StartHistory || His_AddrOffset>AddrEeprom_MAX) His_AddrOffset = AddrEeprom_StartHistory;

    auto_over_time = (0xFF & ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_AutoOverTime)); //读取设定的自动下降时间
    if(auto_over_time < 1 || auto_over_time > 13) auto_over_time = 4;   //The initial default is 30 seconds

    i = (0xFF & ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_BuzzerSwitch)); //读取设定的蜂鸣器开关
    if(i == Save_Disable_Beep)
    {
        Status_Un.Buzzer_Switch = 0;
    }
    else
    {
        Status_Un.Buzzer_Switch = 1;   //初始默认可以开启蜂鸣器
    }

    xm[0] = ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_CommIDH);
    xm[1] = ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_CommIDM);
    xm[2] = ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_CommIDL);
    xn.IDB[0] = 0;
    xn.IDB[1] = xm[0];
    xn.IDB[2] = xm[1];
    xn.IDB[3] = xm[2];

    if ((xn.IDL == 0) || (xn.IDL == 0xFFFFFF))
        ID_SCX1801_DATA = 0;
    else
        ID_SCX1801_DATA = xn.IDL;

    for (i = 0; i < 256; i++)
        //ID_Receiver_DATA[i] = 0; //ID_Receiver_DATA[ID_DATA_PCS]=0;
        ID_Receiver_DATA_WRITE(ID_Receiver_DATA[i], 0);
    xm[0] = ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_IDPcsH);
    xm[1] = ReadByteEEPROM(addr_eeprom_sys + AddrEeprom_IDPcsL);
    ID_DATA_PCS = xm[0] * 256 + xm[1];
    if (ID_DATA_PCS == 0xFFFF)
        ID_DATA_PCS = 0;
    else if (ID_DATA_PCS > ID_Max_PCS)
        ID_DATA_PCS = ID_Max_PCS;
    q = ID_DATA_PCS;
    p = 0;
    for (i = 0; i < q; i++)
    {
        j = 3 * i;
        xm[0] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[1] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[2] = ReadByteEEPROM(addr_eeprom_sys + j);
        xn.IDB[0] = 0;
        xn.IDB[1] = xm[0];
        xn.IDB[2] = xm[1];
        xn.IDB[3] = xm[2];
        if ((xn.IDL == 0) || (xn.IDL == 0xFFFFFF))
            q++;
        else
            //ID_Receiver_DATA[p++] = xn.IDL;
            ID_Receiver_DATA_WRITE(ID_Receiver_DATA[p++], xn.IDL);
        if (q > 260)
            break;
        ClearWDT(); // Service the WDT
    }
    /*
    for (i = 1; i < 2; i++)
    {
        j = 0x380 + i * 4;
        ROM_adf7030_value[i].byte[0] = ReadByteEEPROM(addr_eeprom_sys + j);
        ROM_adf7030_value[i].byte[1] = ReadByteEEPROM(addr_eeprom_sys + j + 1);
        ROM_adf7030_value[i].byte[2] = ReadByteEEPROM(addr_eeprom_sys + j + 2);
        ROM_adf7030_value[i].byte[3] = ReadByteEEPROM(addr_eeprom_sys + j + 3);
        if ((ROM_adf7030_value[i].whole_reg == 0) || (ROM_adf7030_value[i].whole_reg == 0xFFFFFFFF))
            ROM_adf7030_value[i] = Default_adf7030_value[i];
    }*/
}

void ALL_ID_EEPROM_Erase(void)
{
    UINT16 m2, i;
    UINT8 xm[3] = {0};
    xm[0] = 0;
    xm[1] = 0;
    xm[2] = 0;

    ID_DATA_PCS = 0;
    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsH, xm[1]);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsL, xm[0]);
    LockFlash(UNLOCK_EEPROM_TYPE);

    for (i = 0; i < 256; i++)
    {
        //ID_Receiver_DATA[i] = 0;
        ID_Receiver_DATA_WRITE(ID_Receiver_DATA[i], 0);
        m2 = 3 * i;
        UnlockFlash(UNLOCK_EEPROM_TYPE);
        WriteByteToFLASH(addr_eeprom_sys + m2, xm[0]);
        m2++;
        WriteByteToFLASH(addr_eeprom_sys + m2, xm[1]);
        m2++;
        WriteByteToFLASH(addr_eeprom_sys + m2, xm[2]);
        LockFlash(UNLOCK_EEPROM_TYPE);
        ClearWDT(); // Service the WDT
    }
}
void ID_EEPROM_write(void)
{
    UINT8 xm[3] = {0};
    UINT16 i, j, m1;
    uni_rom_id xn, xd;
    ID_DATA_PCS++;
    xm[0] = ID_DATA_PCS % 256;
    xm[1] = ID_DATA_PCS / 256;

    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsH, xm[1]);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsL, xm[0]);
    LockFlash(UNLOCK_EEPROM_TYPE);

    //ID_Receiver_DATA[ID_DATA_PCS - 1] = ID_Receiver_Login;
    ID_Receiver_DATA_WRITE(ID_Receiver_DATA[ID_DATA_PCS - 1], ID_Receiver_Login);
    xn.IDL = ID_Receiver_Login;

    for (i = 0; i < 256; i++)
    {
        j = 3 * i;
        xm[0] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[1] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[2] = ReadByteEEPROM(addr_eeprom_sys + j);
        xd.IDB[0] = 0;
        xd.IDB[1] = xm[0];
        xd.IDB[2] = xm[1];
        xd.IDB[3] = xm[2];
        if ((xd.IDL == 0) || (xd.IDL == 0xFFFFFF))
            break;
        ClearWDT(); // Service the WDT
    }

    xm[0] = xn.IDB[1];
    xm[1] = xn.IDB[2];
    xm[2] = xn.IDB[3];
    m1 = j - 2;
    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + m1, xm[0]);
    m1++;
    WriteByteToFLASH(addr_eeprom_sys + m1, xm[1]);
    m1++;
    WriteByteToFLASH(addr_eeprom_sys + m1, xm[2]);
    LockFlash(UNLOCK_EEPROM_TYPE);

    if (ID_DATA_PCS >= ID_Max_PCS)
    {
        ID_Login_EXIT_Initial();
        DATA_Packet_Control = 0;
        time_Login_exit_256 = 110;
    }
}

void ID_SCX1801_EEPROM_write(u32 id)
{
    UINT8 xm[3] = {0};
    uni_rom_id xn;

    xn.IDL = id;          //ID_Receiver_Login;
    ID_SCX1801_DATA = id; //ID_Receiver_Login;
    xm[0] = xn.IDB[1];
    xm[1] = xn.IDB[2];
    xm[2] = xn.IDB[3];
    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_CommIDH, xm[0]);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_CommIDM, xm[1]);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_CommIDL, xm[2]);
    LockFlash(UNLOCK_EEPROM_TYPE);
}
void Delete_GeneralID_EEPROM(u32 id)
{
    UINT16 i, j, m2, original_pcs = 0;
    UINT8 xm[3] = {0};
    uni_rom_id xn;

    original_pcs = ID_DATA_PCS;
    for (i = 0; i < ID_DATA_PCS; i++)
    {
		//if ((ID_Receiver_DATA[i] == id)&&(id!=0xFFFFFE)&&(id!=0))
        if ((ID_Receiver_DATA_READ(ID_Receiver_DATA[i]) == id)&&(id!=0xFFFFFE)&&(id!=0))
		{
            for (j = i; j < ID_DATA_PCS; j++)
            {
                //ID_Receiver_DATA[j] = ID_Receiver_DATA[j+1];
                ID_Receiver_DATA_WRITE(ID_Receiver_DATA[j],ID_Receiver_DATA_READ(ID_Receiver_DATA[j+1]));
                ClearWDT(); // Service the WDT
            }
            ID_DATA_PCS--;
            if (ID_DATA_PCS==0)
            {
                ALL_ID_EEPROM_Erase();
                return;
            }
        }
        ClearWDT(); // Service the WDT
    }

    xm[0] = ID_DATA_PCS % 256;
    xm[1] = ID_DATA_PCS / 256;
    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsH, xm[1]);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsL, xm[0]);
    LockFlash(UNLOCK_EEPROM_TYPE);

    for (i = 0; i < original_pcs; i++)
    {
        //xn.IDL = ID_Receiver_DATA[i];
        xn.IDL = ID_Receiver_DATA_READ(ID_Receiver_DATA[i]);
        xm[0] = xn.IDB[1];
        xm[1] = xn.IDB[2];
        xm[2] = xn.IDB[3];
        m2 = 3 * i;
        UnlockFlash(UNLOCK_EEPROM_TYPE);
        WriteByteToFLASH(addr_eeprom_sys + m2, xm[0]);
        m2++;
        WriteByteToFLASH(addr_eeprom_sys + m2, xm[1]);
        m2++;
        WriteByteToFLASH(addr_eeprom_sys + m2, xm[2]);
        LockFlash(UNLOCK_EEPROM_TYPE);
        ClearWDT(); // Service the WDT
    }
}
void ID_EEPROM_write_0x00(void)
{
    UINT8 xm[3] = {0};
    UINT16 i, j, m1, q, p;
    uni_rom_id xn, xd;

    ID_DATA_PCS--;
    xm[0] = ID_DATA_PCS % 256;
    xm[1] = ID_DATA_PCS / 256;

    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsH, xm[1]);
    WriteByteToFLASH(addr_eeprom_sys + AddrEeprom_IDPcsL, xm[0]);
    LockFlash(UNLOCK_EEPROM_TYPE);

    for (i = 0; i < 256; i++)
    {
        j = 3 * i;
        xm[0] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[1] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[2] = ReadByteEEPROM(addr_eeprom_sys + j);
        xd.IDB[0] = 0;
        xd.IDB[1] = xm[0];
        xd.IDB[2] = xm[1];
        xd.IDB[3] = xm[2];
        if (xd.IDL == DATA_Packet_ID)
            break;
        ClearWDT(); // Service the WDT
    }

    xm[0] = 0;
    xm[1] = 0;
    xm[2] = 0;
    m1 = i * 3;
    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + m1, xm[0]);
    m1++;
    WriteByteToFLASH(addr_eeprom_sys + m1, xm[1]);
    m1++;
    WriteByteToFLASH(addr_eeprom_sys + m1, xm[2]);
    LockFlash(UNLOCK_EEPROM_TYPE);

    q = ID_DATA_PCS;
    p = 0;
    for (i = 0; i < q; i++)
    {
        j = 3 * i;
        xm[0] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[1] = ReadByteEEPROM(addr_eeprom_sys + j);
        j++;
        xm[2] = ReadByteEEPROM(addr_eeprom_sys + j);
        xn.IDB[0] = 0;
        xn.IDB[1] = xm[0];
        xn.IDB[2] = xm[1];
        xn.IDB[3] = xm[2];
        if ((xn.IDL == 0) || (xn.IDL == 0xFFFFFF))
            q++;
        else
            //ID_Receiver_DATA[p++] = xn.IDL;
            ID_Receiver_DATA_WRITE(ID_Receiver_DATA[p++], xn.IDL);
        if (q > 260)
            break;
        ClearWDT(); // Service the WDT
    }
}

void ID_learn(void)
{
    //    UINT16 i;
    // #if defined(__Product_PIC32MX2_Receiver__)
    //if (FG_10ms)
    if (FG_10ms)
    { //90==1锟�?
        FG_10ms = 0;
       // if (TIME_TestNo91)
        //    --TIME_TestNo91;
       // else
            FLAG_testNo91 = 0;
        if (TIME_ERROR_Read_once_again)
            --TIME_ERROR_Read_once_again;
        if (Time_error_read_timeout)
            --Time_error_read_timeout;
        if (Time_error_read_gap)
            --Time_error_read_gap;
        if (TIME_APP_TX_fromOUT)
            --TIME_APP_TX_fromOUT;
 //       if (TIME_EMC)
 //           --TIME_EMC;


        if (TIME_OUT_OPEN_CLOSE)
            --TIME_OUT_OPEN_CLOSE;
        if (TIME_Receiver_LED_OUT)
            --TIME_Receiver_LED_OUT;
        if (TIME_Login_EXIT_Button)
            --TIME_Login_EXIT_Button;

        if (time_Login_exit_256)
            --time_Login_exit_256;
        //if (TIME_Fine_Calibration)
        //    --TIME_Fine_Calibration;
        if (TIME_Receiver_Login_restrict)
            --TIME_Receiver_Login_restrict;
        else if ((FLAG_ID_Erase_Login == 1) || (FLAG_ID_Login == 1) || (FLAG_ID_SCX1801_Login == 1))
            ;
        else
        {
            TIME_Receiver_Login = 0;
            COUNT_Receiver_Login = 0;
        }

        if (Receiver_Login == 0)
        {
            if (FLAG_ID_SCX1801_Login != 1)
                TIME_Receiver_Login++;
            TIME_Receiver_Login_restrict = 350;
            if ((COUNT_Receiver_Login >= 2) && (FLAG_ID_Erase_Login == 0) && (FLAG_ID_Login == 0) && (ID_DATA_PCS < ID_Max_PCS))
            {
                FLAG_ID_Login = 1;
                /*BEEP_Module(1800,900);
				BEEP_Module(300,1);*/
                //COUNT_Receiver_Login++; //涓轰粈涔堣鍔犺繖涓紵锛熷洜涓哄姞鍏ヤ簡BEEP_Module鍚庯紝beep鏃堕棿杈冮暱锛岃繖鏃堕噰涓嶅埌鎸夐敭鐨勬椂闂碩IME_Receiver_Login
                TIME_Login_EXIT_rest = 5380;
                TIME_Login_EXIT_Button = 500;
            } //6000
            else if (((FLAG_ID_Erase_Login == 1) && (COUNT_Receiver_Login >= 1)) ||
                     (FLAG_ID_SCX1801_Login == 1) ||
                     ((FLAG_ID_Login == 1) && (COUNT_Receiver_Login >= 3)))
            {
                if (TIME_Login_EXIT_Button == 0)
                    ID_Login_EXIT_Initial();
            }
        }
        if (Receiver_Login == 1)
        {
            if (TIME_Receiver_Login > 3)
            {
                if (COUNT_Receiver_Login < 10)
                    COUNT_Receiver_Login++;
            }
            if (FLAG_ID_Login_EXIT == 1)
            {
                FLAG_ID_Login_EXIT = 0;
                COUNT_Receiver_Login = 0;
            }
            TIME_Receiver_Login = 0;
        }
        if (0) //(TIME_Receiver_Login >= 450)  //590
        {
            FLAG_ID_SCX1801_Login = 1;
            FLAG_ID_Erase_Login = 0;
            TIME_Receiver_Login = 0;
            BEEP_CSR2_BEEPEN = 0;
            FG_ID_SCX1801_Login_BEEP = 0;
            TIME_ID_SCX1801_Login = 130;
            TIME_Login_EXIT_rest = 5380;
            COUNT_Receiver_Login = 0;
            TIME_Login_EXIT_Button = 500;
        }
        else if ((TIME_Receiver_Login >= 250) && (FLAG_ID_Erase_Login == 0) && (FLAG_ID_SCX1801_Login == 0))
        {
            TIME_Receiver_Login = 0;
            FLAG_ID_Erase_Login = 1;
            FLAG_ID_Erase_Login_PCS = 1; //杩藉姞澶氾拷??ID鐧诲綍
            /*BEEP_Module(1800,900);
				BEEP_Module(300,900);
				BEEP_Module(300,1);*/
            //COUNT_Receiver_Login++; //为什么要锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟轿拷锟斤拷锟斤拷锟紹EEP_Module锟斤拷beep时锟斤拷铣锟斤拷锟斤拷锟绞憋拷刹锟斤拷锟斤拷锟斤拷锟斤拷锟绞憋拷锟絋IME_Receiver_Login
            TIME_Login_EXIT_rest = 5380;
            TIME_Login_EXIT_Button = 500;
        }
        if ((FLAG_ID_SCX1801_Login == 1) && (TIME_ID_SCX1801_Login == 0))
        {
            if (FG_ID_SCX1801_Login_BEEP == 0)
            {
                FG_ID_SCX1801_Login_BEEP = 1;
                TIME_ID_SCX1801_Login = 390;
                BEEP_CSR2_BEEPEN = 0;
            }
            else if (FG_ID_SCX1801_Login_BEEP == 1)
            {
                FG_ID_SCX1801_Login_BEEP = 0;
                TIME_ID_SCX1801_Login = 130;
                BEEP_CSR2_BEEPEN = 1;
            }
        }

        if ((FLAG_ID_Erase_Login == 1) || (FLAG_ID_Login == 1) || (FLAG_ID_SCX1801_Login == 1))
        {
            //进入登录模式后,解除自动模式,停止时间监测,退出登录模式后受信机恢复到初始状态
            TIMER1s = 0;
		    FG_auto_out = 0;
            TIME_auto_out = 0;
		    TIME_auto_close = 0;
		    FG_auto_open_time = 0;
            FG_auto_manual_mode = 0;
            Manual_override_TIMER = 0;

            auto_receive_cnt = 0;
            Time_Check_AutoSignal = 0;
            Time_NoCheck_AutoSignal = 0;
            Beep_Switch = 0;
            Allow_BeepOn_Flag = 0;

            TIME_Receiver_Login_led++;
            if (TIME_Receiver_Login_led >= 46)
            {
                TIME_Receiver_Login_led = 0;
                if (TIME_Receiver_LED_OUT > 0)
                    Receiver_LED_OUT = 1;
                else
                    Receiver_LED_OUT = !Receiver_LED_OUT;
            }
            if ((FLAG_ID_Login_OK == 1) && (FLAG_ID_Login_OK_bank == 0))
            {
                if ((ID_Receiver_Login == 0xFFFFFE) && (FLAG_ID_Erase_Login == 1))
                    FLAG_ID_Login_OK_bank = 1; //杩藉姞澶氾拷??ID鐧诲綍
                FLAG_ID_Login_OK = 0;          //杩藉姞澶氾拷??ID鐧诲綍
                if (FLAG_ID_SCX1801_Login == 1)
                {
                    FLAG_ID_SCX1801_Login = 0;
                    FG_ID_SCX1801_Login_BEEP = 0;
                    FLAG_ID_Login = 0;
                    FLAG_ID_Erase_Login = 0;
                    BEEP_and_LED();
                    ID_SCX1801_EEPROM_write(ID_Receiver_Login);
                    if (FLAG_IDCheck_OK == 1)
                    {
                        FLAG_IDCheck_OK = 0;
                        Delete_GeneralID_EEPROM(ID_SCX1801_DATA);
                    }
                    ID_Login_EXIT_Initial();
                }
                else
                {
                    if (FLAG_IDCheck_OK == 1)
                        FLAG_IDCheck_OK = 0;
                    else
                    {
                        BEEP_and_LED();
                        TIME_Login_EXIT_rest = 5380; //杩藉姞澶氾拷??ID鐧诲綍
                        if ((FLAG_ID_Login == 1) && (ID_Receiver_Login != 0xFFFFFE))
                            {
                                if (ID_SCX1801_DATA == 0)
                                    ID_SCX1801_EEPROM_write(ID_Receiver_Login);
                                else ID_EEPROM_write();
                            }
                        else if (FLAG_ID_Erase_Login == 1)
                        {
                            if (FLAG_ID_Erase_Login_PCS == 1)
                            {
                                FLAG_ID_Erase_Login_PCS = 0;
                                ID_DATA_PCS = 0;
                                ALL_ID_EEPROM_Erase();
                                ID_SCX1801_DATA = 0;
                                ID_SCX1801_EEPROM_write(0x00);
                                if(ID_Receiver_Login == 0xFFFFFE)
                                    Status_Un.Exist_ID = 0;
                                    if (ID_Receiver_Login != 0xFFFFFE)
                                        ID_SCX1801_EEPROM_write(ID_Receiver_Login);
                            } //杩藉姞澶氾拷??ID鐧诲綍
                            else if (ID_Receiver_Login != 0xFFFFFE)
                                ID_EEPROM_write();
                        }
                    } //end else
                }
            }
            if (TIME_Login_EXIT_rest)
                --TIME_Login_EXIT_rest;
            else
                ID_Login_EXIT_Initial();
        }
    }
    //#endif
}

void ID_Login_EXIT_Initial(void)
{
    // #if defined(__Product_PIC32MX2_Receiver__)
    if (FLAG_ID_Erase_Login == 1)
    {
        /*BEEP_Module(300,900);
		BEEP_Module(300,900);
		BEEP_Module(1800,1);*/
    }
    else if (FLAG_ID_Login == 1)
    {
        /*BEEP_Module(300,900);
		BEEP_Module(1800,1);*/
    }
    BEEP_CSR2_BEEPEN = 0;
    FLAG_ID_Login_EXIT = 1;
    FLAG_ID_Login_OK = 0;
    FLAG_ID_Login_OK_bank = 0;
    FLAG_ID_Login = 0;
    FLAG_ID_Erase_Login = 0;
    FLAG_ID_SCX1801_Login = 0;
    Receiver_LED_OUT = 0;
    COUNT_Receiver_Login = 0;
    //#endif
    //#if defined(__Product_PIC32MX2_WIFI__)
    //     FLAG_ID_Login_EXIT=1;
    //     FLAG_ID_Login_OK=0;
    //     FLAG_ID_Login_OK_bank=0;
    //     FLAG_ID_Login=0;
    //     FLAG_ID_Erase_Login=0;
    //     WIFI_LED_RX=0;
    //#endif
}

void eeprom_write_byte(u16 addr,u8 data)
{
    UnlockFlash(UNLOCK_EEPROM_TYPE);
    WriteByteToFLASH(addr_eeprom_sys + addr, data);
    LockFlash(UNLOCK_EEPROM_TYPE);
}

void Read_HisData(u16 paddr,u8 rnum)
{
    u8 i,j;
    for(i=0; i < rnum; i++)
    {
        for(j=0; j<11; j++)
        {
            HIS_DATA[i].history_buff[j] = ReadByteEEPROM(addr_eeprom_sys + paddr);
            paddr++;
        }
    }
}

u32 R_ID(u8 *pid)
{
    uni_rom_id xid = {0};
    xid.IDB[1] = pid[0];
    xid.IDB[2] = pid[1];
    xid.IDB[3] = pid[2];

    return xid.IDL;
}
