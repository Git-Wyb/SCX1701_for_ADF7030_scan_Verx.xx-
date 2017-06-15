
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> /* For true/false definition                      */
#include <stdio.h>
#include "spi.h"
#include "Pin_define.h" // оƬ���Ŷ���
#include "ram.h"
#include "ADF7030_1.h"
#include "ID_Decode.h"
#include "type_def.h"
#include "uart.h"
u8 FilterChar[2][1] = {
    {' '},
    {'G'}};
/**ADF7030_REST**/ u8 ADF7030_REST_Cache;
/**Receiver_vent**/ u8 Receiver_vent_Cache;
u8 SPI_SEND_BUFF[SPI_SEND_BUFF_LONG] = {0X55};
u8 SPI_RECEIVE_BUFF[SPI_REV_BUFF_LONG] = {0};
u32 SPI_Receive_DataForC[6]; //C��
u16 Head_0x5515_or_0x5456 = 0;
u32 ADF7030_RESIGER_VALUE_READ = 0;
u8 ADF7030_Read_OneByte = 0;
u8 RX_COUNT = 0;
u8 Radio_State = 0;
u32 CCA_READBACK_Cache = 0;
/************Address: 0x20000500, Name: GENERIC_PKT_FRAME_CFG1***************************************/
u8 TRX_IRQ1_TYPE = 0x80;
u8 TRX_IRQ0_TYPE = 0x04;
u8 GENERIC_PKT_FRAME_CFG1_13_15 = 0;
u8 PREAMBLE_UNIT = 1;
u16 PAYLOAD_SIZE = 0x0C;
/***************************************************/
u8 Flag_FREQ_Scan = 0;
u8 BREState = 0;
void DELAY_30U(void)
{
    u8 Tp_i = 0;
    for (Tp_i = 0; Tp_i < 17; Tp_i++)
        ; //???????30U
}

void DELAY_XX(void)
{
    u32 cyw_delay = 0;
    for (cyw_delay = 0; cyw_delay < 0x20000; cyw_delay++)
        ;
}

/**
****************************************************************************
* @Function : void ADF7030Init(void)
* @File     : ADF7030_1.c
* @Program  : none
* @Created  : 2017/4/12 by Xiaowine
* @Brief    : ��ʼ��ADF7030��оƬ
* @Version  : V1.0
**/
void ADF7030Init(void)
{
    SPI_conf();             //��ʼ��spi
                            //    ADF7030_GPIO_INIT();
    ADF7030ParameterInit(); //������ʼ��
    ADF7030_REST = 0;       //ADF7030оƬ��ʼ��
    Delayus(50);
    ClearWDT();
    ADF7030_REST = 1; //ADF7030оƬ��ʼ�����
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    ADF7030_CHANGE_STATE(STATE_PHY_OFF);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬

    ClearWDT(); // Service the WDT
    ADF7030_WRITING_PROFILE_FROM_POWERON();
    ClearWDT(); // Service the WDT
    if (WORK_TEST == 1)
        ADF7030_RECEIVING_FROM_POWEROFF();
    ClearWDT(); // Service the WDT
    CONFIGURING_THE_POINTERS_FOR_POINTER_BASED_ACCESSES();
    ClearWDT(); // Service the WDT
}
/**
 ****************************************************************************
 * @Function : void ADF7030ParameterInit(void)
 * @File     : ADF7030_1.c
 * @Program  : none
 * @Created  : 2017/6/6 by Xiaowine
 * @Brief    : ������ʼ��
 * @Version  : V1.0
**/
void ADF7030ParameterInit(void)
{
    //static u32 cache;
    /************Address: 0x20000500, Name: GENERIC_PKT_FRAME_CFG1***************************************/
    TRX_IRQ1_TYPE = 0x80;                 // the full packet has been received (Rx) or transmitted(Tx).
    TRX_IRQ0_TYPE = 0x04;                 // the programmed number of bits of Sync Word 0 has been received and matched(Rx).or transmitted(Tx).
    GENERIC_PKT_FRAME_CFG1_13_15 = 0;     //Generated by the ADF7030-1 design center
    PREAMBLE_UNIT = 1;                    //PREAMBLE_LEN is in units of octets.
    PAYLOAD_SIZE = RX_PayLoadSizeNOLogin; //number of payload bytes in the Tx packet(Tx) or incoming packet payload(Rx).
    /**************************************************/
}

void ADF7030_CHANGE_STATE(u8 x_state)
{
    SPI_SEND_BUFF[0] = x_state;
    WAIT_SPI_IDEL();
    SPI_SendString(1, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    WAIT_SPI_IDEL();
}
/**
****************************************************************************
* @Function : u8 RadioChangeState(ADF7030_RADIO_STATE CMD)
* @File     : ADF7030_1.c
* @Program  :
* @Created  : 2017/4/25 by Xiaowine
* @Brief    :
* @Version  : V1.0
**/
u8 RadioChangeState(u8 STATE_CMD)
{
    u8 state;
    ADF7030_1_STATUS_TYPE STATUSCache;
    STATUSCache.VALUE = 0;
    ADF7030_CHANGE_STATE(STATE_CMD);
    while ((STATUSCache.CMD_READY == 0) && (STATUSCache.FW_STATUS == TRANSITION_STATUS_TRANSITION))
    {
        STATUSCache = GET_STATUE_BYTE();
    }
    ADF7030_READ_REGISTER_NOPOINTER_LONGADDR(ADDR_MISC_FW, 6); //??7030????
    state = (ADF7030_RESIGER_VALUE_READ >> 8) & 0x00003F;
    ClearWDT(); // Service the WDT
    if (state == (STATE_CMD & 0x0f))
        return SUCESS;
    else
        return FAILURE;
}
/**
 ****************************************************************************
 * @Function : u8 ADF7030_GET_FW_STATE(void)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/9 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
u8 ADF7030_GET_FW_STATE(void)
{
    u8 state;
    while (GET_STATUE_BYTE().CMD_READY == 0)
        ;
    DELAY_30U();
    ADF7030_READ_REGISTER_NOPOINTER_LONGADDR(ADDR_MISC_FW, 6); //??7030????
    state = (ADF7030_RESIGER_VALUE_READ >> 8) & 0x00003F;
    return state;
}

void ADF7030_FIXED_DATA(void)
{
    SPI_SEND_BUFF[0] = 0xff;
    SPI_SEND_BUFF[1] = 0xff;
    WAIT_SPI_IDEL();
    SPI_SendString(2, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    WAIT_SPI_IDEL();
}

/**
 ****************************************************************************
 * @Function : u32 CONFIGURING_THE_POINTERS_FOR_POINTER_BASED_ACCESSES(void)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/11 by Xiaowine
 * @Brief    : ���� pointer ��ַ
 * @Version  : V1.0
**/
u32 CONFIGURING_THE_POINTERS_FOR_POINTER_BASED_ACCESSES(void)
{
    u8 Tp_sub = 0;
    SPI_SEND_BUFF[Tp_sub++] = 0x28;
    SPI_SEND_BUFF[Tp_sub++] = (0x200004F4 >> 24) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0x200004F4 >> 16) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0x200004F4 >> 8) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0x200004F4) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0x20000AF0 >> 24) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0x20000AF0 >> 16) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0x20000AF0 >> 8) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0x20000AF0) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0X20000C18 >> 24) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0X20000C18 >> 16) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0X20000C18 >> 8) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (0X20000C18) & 0XFF;

    SPI_SendString(Tp_sub, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    return 0;
}
/**
 ****************************************************************************
 * @Function : u8 Memory_Read_Block_Pointer_Short_Address(ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID,u8 num)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/11 by Xiaowine
 * @Brief    : Memory Write/Read, Block, Pointer, Short Address
 * @Version  : V1.0
**/
u8 Memory_Read_Block_Pointer_Short_Address(ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID, u8 num)
{
    u8 Tp_sub = 0, i;
    SPI_SEND_BUFF[Tp_sub++] = ADF703x_SPI_MEM_READ | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_SHORT | PNTR_ID;
    for (i = 0; i < num + 1; i++)
        SPI_SEND_BUFF[Tp_sub++] = 0xFF;
    SPI_SendString(Tp_sub, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    return 0;
}
/**
 ****************************************************************************
 * @Function : u8 Memory_Read_Block_Pointer_Long_Address(ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID,u8 num)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/11 by Xiaowine
 * @Brief    : Memory Write/Read, Block, Pointer, Short Address
 * @Version  : V1.0
**/
u8 Memory_Read_Block_Pointer_Long_Address(ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID, u8 num)
{
    u8 Tp_sub = 0, i;
    SPI_SEND_BUFF[Tp_sub++] = ADF703x_SPI_MEM_READ | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_LONG | PNTR_ID;
    for (i = 0; i < num + 2; i++)
        SPI_SEND_BUFF[Tp_sub++] = 0xFF;
    SPI_SendString(Tp_sub, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    return 0;
}
/**
 ****************************************************************************
 * @Function : u8 Memory_Write_Block_Pointer_Short_Address(u8 *x_data,ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID,u8 num)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/11 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
u8 Memory_Write_Block_Pointer_Short_Address(const u8 *x_data, ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID, u8 num)
{
    u8 Tp_sub = 0, i;
    SPI_SEND_BUFF[Tp_sub++] = ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_SHORT | PNTR_ID;
    for (i = 0; i < num; i++)
        SPI_SEND_BUFF[Tp_sub++] = *x_data++;
    SPI_SendString(Tp_sub, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    return 0;
}
/**
 ****************************************************************************
 * @Function : u8 Memory_Write_Block_Pointer_Long_Address(u8 *x_data,ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID,u8 num)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/11 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
u8 Memory_Write_Block_Pointer_Long_Address(const u8 *x_data, ADI_ADF7030_1_RADIO_SPI_PNTR_TYPE PNTR_ID, u8 num)
{
    u8 Tp_sub = 0, i;
    SPI_SEND_BUFF[Tp_sub++] = ADF703x_SPI_MEM_WRITE | ADF703x_SPI_MEM_BLOCK | ADF703x_SPI_MEM_POINTER | ADF703x_SPI_MEM_LONG | PNTR_ID;
    for (i = 0; i < num; i++)
        SPI_SEND_BUFF[Tp_sub++] = *x_data++;
    SPI_SendString(Tp_sub, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    return 0;
}

//???ADF7030????
//x_fnum ??0xff???
u32 ADF7030_READ_REGISTER_NOPOINTER_LONGADDR(u32 x_ADDR, u8 x_fnum)
{
    u8 Tp_sub = 0;
    u8 Tp_i = 0;
    SPI_SEND_BUFF[Tp_sub++] = 0X78;
    SPI_SEND_BUFF[Tp_sub++] = (x_ADDR >> 24) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (x_ADDR >> 16) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (x_ADDR >> 8) & 0XFF;
    SPI_SEND_BUFF[Tp_sub++] = (x_ADDR)&0XFF;

    for (Tp_i = 0; Tp_i < x_fnum; Tp_i++)
        SPI_SEND_BUFF[Tp_sub++] = 0xff;
    WAIT_SPI_IDEL();
    SPI_SendString(Tp_sub, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    WAIT_SPI_IDEL();
    return ADF7030_RESIGER_VALUE_READ;
}

//????
//?? x_ADDR ??   x_data ???  x_long????
void ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR(u32 x_ADDR, const u8 *x_data, u16 x_long)
{
    u16 Tp_long;
    SPI_SEND_BUFF[0] = 0X38;
    SPI_SEND_BUFF[1] = (x_ADDR >> 24) & 0XFF;
    SPI_SEND_BUFF[2] = (x_ADDR >> 16) & 0XFF;
    SPI_SEND_BUFF[3] = (x_ADDR >> 8) & 0XFF;
    SPI_SEND_BUFF[4] = (x_ADDR)&0XFF;
    for (Tp_long = 0; Tp_long < x_long; Tp_long = Tp_long + 4) //??????????
    {
        SPI_SEND_BUFF[5 + Tp_long] = x_data[Tp_long + 3];
        SPI_SEND_BUFF[5 + Tp_long + 1] = x_data[Tp_long + 2];
        SPI_SEND_BUFF[5 + Tp_long + 2] = x_data[Tp_long + 1];
        SPI_SEND_BUFF[5 + Tp_long + 3] = x_data[Tp_long];
    }
    WAIT_SPI_IDEL();

    SPI_SendString(x_long + 5, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    WAIT_SPI_IDEL();
}
u8 ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_CFGFILE_MSB(const u8 *pCONFIG, u32 Size)
{
    u32 array_position = 0;
    u32 length;
    do
    {
        length = ((u32) * (pCONFIG + array_position) << 16) |
                 ((u32) * (pCONFIG + array_position + 1) << 8) |
                 ((u32) * (pCONFIG + array_position + 2));
        if (length > 0xffff)
            return FAILURE;
        WAIT_SPI_IDEL();
        SPI_SendString(length - 3, (pCONFIG + array_position + 3), SPI_RECEIVE_BUFF);
        WAIT_SPI_IDEL();
        array_position += length;
    } while (array_position < Size);
    return SUCESS;
}
u8 ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(const u8 *pCONFIG, u32 Size, u32 base, u8 offset, u8 len)
{
    u32 array_position = 0;
    u32 length, address;
    u8 i;
    do
    {
        length = ((u32) * (pCONFIG + array_position) << 16) |
                 ((u32) * (pCONFIG + array_position + 1) << 8) |
                 ((u32) * (pCONFIG + array_position + 2));
        if (length > 0xffff)
            return FAILURE;
        address = ((u32) * (pCONFIG + array_position + 4) << 24) |
                  ((u32) * (pCONFIG + array_position + 5) << 16) |
                  ((u32) * (pCONFIG + array_position + 6) << 8) |
                  ((u32) * (pCONFIG + array_position + 7));
        if (address == base)
        {
            SPI_SEND_BUFF[0] = 0x38;
            SPI_SEND_BUFF[1] = (u8)(((address + offset) >> 24) & 0xff);
            SPI_SEND_BUFF[2] = (u8)(((address + offset) >> 16) & 0xff);
            SPI_SEND_BUFF[3] = (u8)(((address + offset) >> 8) & 0xff);
            SPI_SEND_BUFF[4] = (u8)(((address + offset) >> 0) & 0xff);
            for (i = 0; i < len; i++)
            {
                SPI_SEND_BUFF[5 + i] = *(pCONFIG + array_position + 8 + offset + i);
            }
            WAIT_SPI_IDEL();
            SPI_SendString(len + 5, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
            WAIT_SPI_IDEL();
            return SUCESS;
        }
        array_position += length;
    } while (array_position < Size);
    return SUCESS;
}
/**
 ****************************************************************************
 * @Function : void ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(u32 xx_ADDR,u32 x_data)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/2 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
void ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(u32 x_ADDR, u32 x_data)
{
    SPI_SEND_BUFF[0] = 0X38;
    SPI_SEND_BUFF[1] = (x_ADDR >> 24) & 0XFF;
    SPI_SEND_BUFF[2] = (x_ADDR >> 16) & 0XFF;
    SPI_SEND_BUFF[3] = (x_ADDR >> 8) & 0XFF;
    SPI_SEND_BUFF[4] = (x_ADDR)&0XFF;
    SPI_SEND_BUFF[5] = (x_data >> 24) & 0XFF;
    SPI_SEND_BUFF[6] = (x_data >> 16) & 0XFF;
    SPI_SEND_BUFF[7] = (x_data >> 8) & 0XFF;
    SPI_SEND_BUFF[8] = x_data & 0XFF;
    WAIT_SPI_IDEL();
    SPI_SendString(9, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    WAIT_SPI_IDEL();
}
/**
 ****************************************************************************
 * @Function : void ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(u32 xx_ADDR,u32 x_data)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/5/2 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
void ADF7030_Clear_IRQ(void)
{
    SPI_SEND_BUFF[0] = 0X38;
    SPI_SEND_BUFF[1] = (0x40003808 >> 24) & 0XFF;
    SPI_SEND_BUFF[2] = (0x40003808 >> 16) & 0XFF;
    SPI_SEND_BUFF[3] = (0x40003808 >> 8) & 0XFF;
    SPI_SEND_BUFF[4] = (0x40003808) & 0XFF;
    SPI_SEND_BUFF[5] = 0XFF;
    SPI_SEND_BUFF[6] = 0XFF;
    SPI_SEND_BUFF[7] = 0XFF;
    SPI_SEND_BUFF[8] = 0XFF;
    SPI_SEND_BUFF[9] = 0XFF;
    SPI_SEND_BUFF[10] = 0XFF;
    SPI_SEND_BUFF[11] = 0XFF;
    SPI_SEND_BUFF[12] = 0XFF;
    WAIT_SPI_IDEL();
    SPI_SendString(13, SPI_SEND_BUFF, SPI_RECEIVE_BUFF);
    WAIT_SPI_IDEL();
}
//
u32 ADF7030_GET_MISC_FW(void) //??MISC_FW?????
{
    DELAY_30U();
    ADF7030_FIXED_DATA();
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    DELAY_30U();
    ADF7030_FIXED_DATA();
    DELAY_30U();
    ADF7030_READ_REGISTER_NOPOINTER_LONGADDR(ADDR_MISC_FW, 6); //??7030????
    return 0;
}

void ADF7030_WRITING_PROFILE_FROM_POWERON(void)
{
    ADF7030_REST = 0; //ADF7030оƬ��ʼ��
    Delayus(50);
    ClearWDT();
    ADF7030_REST = 1; //ADF7030оƬ��ʼ�����
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_OFF);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_CFGFILE_MSB(ADF7030Cfg, CFG_SIZE());
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_Clear_IRQ();
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    if (WORK_TEST == 0)
    {

        ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(ADDR_TESTMODE0, GENERIC_PKT_TEST_MODES0_32bit_20000548);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(ADDR_RADIO_DIG_TX_CFG0, RADIO_DIG_TX_CFG0_32bit_20000304);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
    }
    if (BREState == 1)
    {
        ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(0x400041F8, 0x00000000);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(0x20000378, 0x06C00043);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
    }
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(ADDR_PROFILE_RADIO_AFC_CFG1, PROFILE_RADIO_AFC_CFG1_32bit_2000031C);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_CFG_DEV);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_OFF);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    while (ADF7030_GPIO3 == 1) //���ж� GPIO3���ø�
        ;
}

void ADF7030_TRANSMITTING_FROM_POWEROFF(void)
{
    while (GET_STATUE_BYTE().CMD_READY == 0)
        ;
    ADF7030_CHANGE_STATE(STATE_PHY_OFF);
    while (GET_STATUE_BYTE().FW_STATUS == 0)
        ;
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    while (GET_STATUE_BYTE().FW_STATUS == 0)
        ;
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_GENERIC_FIELDS, 8, 24);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_CHANNEL_FERQUENCY, 8, 4);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    Memory_Write_Block_Pointer_Short_Address(CONST_TXPACKET_DATA_20000AF0, PNTR_CUSTOM1_ADDR, 12);
    //    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR(ADDR_TXPACKET_DATA,CONST_TXPACKET_DATA_20000AF0,OPEN_LONG);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_TX);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    /*   while((PORTRead(ADF7030_GPIO3_PORT)&ADF7030_GPIO3_PIN)==0)//????????
    {
    DELAY_30U();
    }*/
    DELAY_30U();
    ADF7030_Clear_IRQ();
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    while (GET_STATUE_BYTE().FW_STATUS == 0)
        ;
}
/*RECEIVE A SINGLE PACKET FROM POWER OFF*/
void ADF7030_RECEIVING_FROM_POWEROFF(void)
{
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    //    ADF7030_CHANGE_STATE(STATE_PHY_OFF);
    //    WaitForADF7030_FIXED_DATA();  //�ȴ�оƬ����/�ɽ���CMD״̬
    //    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_GENERIC_FIELDS, 8, 24);
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(ADDR_GENERIC_PKT_FRAME_CFG1, GENERIC_PKT_FRAME_CFG1);    //
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(ADDR_CHANNEL_FERQUENCY, PROFILE_CH_FREQ_32bit_200002EC); //
    WaitForADF7030_FIXED_DATA();                                                                           //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_Clear_IRQ();
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    while (ADF7030_GPIO3 == 1) //���ж� GPIO3���ø� �ȴ���λ
        ;
    ADF7030_CHANGE_STATE(STATE_PHY_RX);
    while (GET_STATUE_BYTE().FW_STATUS == 0)
        ;
    DELAY_30U();
}

/*RECEIVE A SINGLE PACKET FROM POWER OFF*/
void ADF7030_ACC_FROM_POWEROFF(void)
{
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_GENERIC_FIELDS, 8, 24);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_CHANNEL_FERQUENCY, 8, 4);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR(ADDR_TXPACKET_DATA, CONST_TXPACKET_DATA_20000AF0, OPEN_LONG);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(0x20000378, 0x06C01043);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_CMD_CCA);
    while (GET_STATUE_BYTE().FW_STATUS == 0)
        ;
    ADF7030_CHANGE_STATE(STATE_CMD_CCA);
    while (GET_STATUE_BYTE().FW_STATUS == 0)
        ;
    ClearWDT(); // Service the WDT
}

void RX_ANALYSIS(void)
{
    u8 i;
    TIMER300ms = 500;
    FG_Receiver_LED_RX = 1;
    // if (WORK_TEST == 0)
    //     Send_Data(&SPI_RECEIVE_BUFF[3], PAYLOAD_SIZE);
    for (i = 0; i < 6; i++)
    {
        SPI_Receive_DataForC[i] = (u32)SPI_RECEIVE_BUFF[i * 4 + 3] |
                                  (u32)SPI_RECEIVE_BUFF[i * 4 + 4] << 8 |
                                  (u32)SPI_RECEIVE_BUFF[i * 4 + 5] << 16 |
                                  (u32)SPI_RECEIVE_BUFF[i * 4 + 6] << 24;
    }
    FLAG_Receiver_IDCheck = 1;
    ID_Decode_IDCheck();
}

void SCAN_RECEIVE_PACKET(void)
{
    short Cache;
    if (ADF7030_GPIO3 == 1)
    {
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_Clear_IRQ();
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        Memory_Read_Block_Pointer_Long_Address(PNTR_CUSTOM2_ADDR, PAYLOAD_SIZE);
        RX_ANALYSIS(); //��������
        while (ADF7030_GPIO3 == 1)
            ;
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_PHY_ON);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        ADF7030_RECEIVING_FROM_POWEROFF();
        RAM_RSSI_AVG = RAM_RSSI_SUM / RSSI_Read_Counter;
        RSSI_Read_Counter = 0;
        RAM_RSSI_SUM = 0;
        TIMER18ms = 28;
        Flag_FREQ_Scan = 0;
    }
    else if ((ADF7030_GPIO3 == 0) && (ADF7030_GPIO2 == 1))
    {
        if (RSSI_Read_Counter == 0)
        {
            TIMER18ms = PAYLOAD_SIZE * 7;
        }
        if ((Flag_RSSI_Read_Timer == 0) && (RSSI_Read_Counter < 5))
        {
            Flag_RSSI_Read_Timer = 10;
            ADF7030_READ_REGISTER_NOPOINTER_LONGADDR(ADDR_GENERIC_PKT_LIVE_LINK_QUAL, 6);
            Cache = (short)((ADF7030_RESIGER_VALUE_READ & 0x07ff0000) >> 11);
            RAM_RSSI_SUM += Cache;
            RSSI_Read_Counter++;
        }
    }
}
/**
****************************************************************************
* @Function : void WaitForADF7030_FIXED_DATA(void)
* @File     : ADF7030_1.c
* @Program  : none
* @Created  : 2017/4/18 by Xiaowine
* @Brief    : �ȴ�ADF7030оƬ�ɲ���(����ִ�����/����) ���ÿ��Ź�
* @Version  : V1.0
**/
void WaitForADF7030_FIXED_DATA(void)
{
    do
    {
        DELAY_30U();
        ADF7030_FIXED_DATA();
        ClearWDT();
    } while (((ADF7030_Read_OneByte & 0x20) != 0x20) || ((ADF7030_Read_OneByte & 0x06) != 0x04));
}
/**
****************************************************************************
* @Function : ADF7030_1_STATUS_TYPE GET_STATUE_BYTE(void)
* @File     : ADF7030_1.c
* @Program  :
* @Created  : 2017/4/25 by Xiaowine
* @Brief    :
* @Version  : V1.0
**/
ADF7030_1_STATUS_TYPE GET_STATUE_BYTE(void)
{
    ADF7030_1_STATUS_TYPE StatusCache;
    DELAY_30U();
    ADF7030_FIXED_DATA();
    DELAY_30U();
    ADF7030_FIXED_DATA();
    StatusCache.VALUE = ADF7030_Read_OneByte;
    return StatusCache;
}
/**
****************************************************************************
* @Function : void TX_DataLoad(u32 IDCache,u8 CtrCmd,u8 *Packet)
* @File     : ADF7030_1.c
* @Program  : IDCache:ID CtrCmd:���� *Packet�����Ͱ�
* @Created  : 2017/4/18 by Xiaowine
* @Brief    :
* @Version  : V1.0
**/
void TX_DataLoad(u32 IDCache, u8 CtrCmd, u8 *Packet)
{
    u8 i;
    u16 CRCTemp = 0;
    CRCTemp = (IDCache & 0xffff) + (((IDCache >> 16) & 0xff) + ((u16)CtrCmd << 8));
    for (i = 0; i < 24; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((IDCache & ((u32)1 << i)) ? 2 : 1);
    }
    for (i = 24; i < 32; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CtrCmd & ((u8)1 << (i - 24))) ? 2 : 1);
    }
    for (i = 32; i < 48; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CRCTemp & ((u16)1 << (i - 32))) ? 2 : 1);
    }
    ClearWDT();
}

/**
****************************************************************************
* @Function : void ReceiveTestModesCFG(void)
* @File     : ADF7030_1.c
* @Program  :
* @Created  : 2017/4/21 by Xiaowine
* @Brief    : ���óɽ��ղ���״̬  FROM_POWERON
* @Version  : V1.0
**/
void ReceiveTestModesCFG(void)
{
    // ADF7030_WRITING_PROFILE_FROM_POWERON();
    ADF7030_ACC_FROM_POWEROFF();
}

/**
 ****************************************************************************
 * @Function : void TestFunV2(u8 KeyVel)
 * @File     : ADF7030_1.c
 * @Program  : KeyVel: ����ֵ
 * @Created  : 2017/5/2 by Xiaowine
 * @Brief    : ���� �����л�
 * @Version  : V1.0
**/
void TestFunV2(u8 KeyVel)
{
    static u8 StatePoint = 0, PowerdBm = 0, TestState = 0, SendFlag = 0, ACCFlag = 0;
    //    char TestStatewords[] = {'R', 'C', '+', '-', 'P'};
    u8 Cache;
    if (StateReadTimer == 0)
    {
        Radio_State = ADF7030_GET_FW_STATE();
        DELAY_30U();
        while (GET_STATUE_BYTE().CMD_READY != 1)
            ;
        DELAY_30U();
        ADF7030_READ_REGISTER_NOPOINTER_LONGADDR(0X4000380C, 6); //PROFILE_CCA_READBACK,6);
        CCA_READBACK_Cache = ADF7030_RESIGER_VALUE_READ;
        StateReadTimer = 500;
    }
    if (SendFlag == 0)
    {
        if (KeyVel == KEY_SW2_Down)
        {
            //lcd            display_map_xy(1, 9 * (StatePoint + 1), 5, 8, char_Small);
            if (StatePoint > 4)
                StatePoint = 0;
            else
                StatePoint++;
            //lcd            display_map_xy(1, 9 * (StatePoint + 1), 5, 8, char_Small + ('*' - ' ') * 5);
        }
        else if (KeyVel == KEY_SW3_Down)
        {
            switch (StatePoint)
            {
            case 0:
            { //GFSK
                RADIO_DIG_TX_CFG0_32bit_20000304 ^= 2;
                Cache = (RADIO_DIG_TX_CFG0_32bit_20000304 & 0x02) >> 1;
                //lcd                display_map_xy(52, 9, 5, 8, char_Small + (FilterChar[Cache][0] - ' ') * 5);
                break;
            }
            case 1:
            { //PAx
                RADIO_DIG_TX_CFG0_32bit_20000304 ^= 0x40000000;
                Cache = (RADIO_DIG_TX_CFG0_32bit_20000304 >> 30) & 1;
                //lcd                display_map_xy(52, 18, 5, 8, char_Small + ('1' - ' ' + Cache) * 5);
                RADIO_DIG_TX_CFG0_32bit_20000304 &= 0xff000fff;
                RADIO_DIG_TX_CFG0_32bit_20000304 |= ((u32)PA_POWER_OUT[Cache][PowerdBm] << 12);
                break;
            }
            case 2:
            { //dBm
                if (PowerdBm > 16)
                    PowerdBm = 0;
                else
                    PowerdBm++;
                Cache = PowerdBm / 10;
                //lcd                display_map_xy(52, 27, 5, 8, char_Small + ('0' - ' ' + Cache) * 5);
                Cache = PowerdBm % 10;
                //lcd                display_map_xy(58, 27, 5, 8, char_Small + ('0' - ' ' + Cache) * 5);
                Cache = (RADIO_DIG_TX_CFG0_32bit_20000304 >> 30) & 1;
                RADIO_DIG_TX_CFG0_32bit_20000304 &= 0xff000fff;
                RADIO_DIG_TX_CFG0_32bit_20000304 |= ((u32)PA_POWER_OUT[Cache][PowerdBm] << 12);
                break;
            }
            case 3:
            { //TX_Mode
                if (TestState > 3)
                    TestState = 0;
                else
                    TestState++;
                GENERIC_PKT_TEST_MODES0_32bit_20000548 &= 0xfff8ffff;
                GENERIC_PKT_TEST_MODES0_32bit_20000548 |= ((u32)TEST_MODES0_para[TestState] << 16);
                //lcd                display_map_xy(52, 36, 5, 8, char_Small + (TestStatewords[TestState] - ' ') * 5);
                break;
            }
            case 4:
            { //BER
                if (BREState == 1)
                    BREState = 0;
                else
                    BREState = 1;
                //lcd                display_map_xy(52, 45, 5, 8, char_Small + ('0' - ' ' + BREState) * 5);
                break;
            }
            case 5:
            { //ACC
                if (ACCFlag == 1)
                    ACCFlag = 0;
                else
                    ACCFlag = 1;
                //lcd                display_map_xy(52, 54, 5, 8, char_Small + ('0' - ' ' + ACCFlag) * 5);
                break;
            }
            default:
                break;
            }
        }
        else if (KeyVel == KEY_SW4_Down)
        {
            if (SendFlag == 0)
            {
                if (TestState != 0)
                {
                    ADF7030_WRITING_PROFILE_FROM_POWERON();
                    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
                    DELAY_30U();
                    ADF7030_CHANGE_STATE(STATE_PHY_ON);
                    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
                    DELAY_30U();
                    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_GENERIC_FIELDS, 8, 24);
                    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
                    DELAY_30U();
                    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_CHANNEL_FERQUENCY, 8, 4);
                    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
                    DELAY_30U();
                    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR(ADDR_TXPACKET_DATA, CONST_TXPACKET_DATA_20000AF0, OPEN_LONG);
                    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
                    DELAY_30U();
                    ADF7030_CHANGE_STATE(STATE_PHY_TX);
                    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
                }
                else
                {
                    if (ACCFlag == 0)
                    {
                        ADF7030_WRITING_PROFILE_FROM_POWERON();
                        ADF7030_RECEIVING_FROM_POWEROFF();
                        if (BREState == 1)
                        {
                        }
                    }
                    else
                    {
                        ADF7030_WRITING_PROFILE_FROM_POWERON();

                        ADF7030_ACC_FROM_POWEROFF();
                    }
                }
                //lcd                display_map_58_6(10, 63, 7, "Startup");
                SendFlag = 1;
            }
        }
    }
    else
    {
        if (TestState == 0)
        {
            if (ACCFlag == 0)
            {
                if (BREState == 1)
                {
                    RF_BRE_Check();
                }
                else
                    SCAN_RECEIVE_PACKET();
            }
        }
        if (KeyVel == KEY_SW4_Down)
        {
            //lcd            display_map_58_6(10, 63, 7, "END    ");
            ADF7030_GET_MISC_FW();
            SendFlag = 0;
        }
    }
}
/**
 ****************************************************************************
 * @Function : void ADF7030_TX(u8 mode)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/6/6 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
void ADF7030_TX(u8 mode)
{
    GENERIC_PKT_TEST_MODES0_32bit_20000548 &= 0xfff8ffff;
    GENERIC_PKT_TEST_MODES0_32bit_20000548 |= ((u32)mode << 16);
    ADF7030_WRITING_PROFILE_FROM_POWERON();
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_ON);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_GENERIC_FIELDS, 8, 24);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_OFFSET_MSB(ADF7030Cfg, CFG_SIZE(), ADDR_CHANNEL_FERQUENCY, 8, 4);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR(ADDR_TXPACKET_DATA, CONST_TXPACKET_DATA_20000AF0, OPEN_LONG);
    WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
    DELAY_30U();
    ADF7030_CHANGE_STATE(STATE_PHY_TX);

    while (GET_STATUE_BYTE().FW_STATUS != 1)
        ;
    // WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
}
/**
 ****************************************************************************
 * @Function : u32 ADF7030_Read_(u32 addr,u32 Para,u8 offset)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/6/7 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
u32 ADF7030_Read_RESIGER(u32 addr, u32 Para, u8 offset)
{
    u32 Cache;
    while (GET_STATUE_BYTE().CMD_READY != 1)
        ;
    ADF7030_READ_REGISTER_NOPOINTER_LONGADDR(addr, 6);
    Cache = ADF7030_RESIGER_VALUE_READ;

    return Cache & (Para << offset);
}
/**
 ****************************************************************************
 * @Function : void 0adf7030_Change_Channel(void)
 * @File     : ADF7030_1.c
 * @Program  :
 * @Created  : 2017/6/12 by Xiaowine
 * @Brief    :
 * @Version  : V1.0
**/
void ADF7030_Change_Channel(void)
{
    switch (PROFILE_CH_FREQ_32bit_200002EC)
    {
    case 426075000:
        PROFILE_CH_FREQ_32bit_200002EC = 429175000;
        PROFILE_RADIO_AFC_CFG1_32bit_2000031C = 0x0005005B;
        ClearWDT(); // Service the WDT
        ADF7030_CHANGE_STATE(STATE_PHY_ON);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_PHY_OFF);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(ADDR_PROFILE_RADIO_AFC_CFG1, PROFILE_RADIO_AFC_CFG1_32bit_2000031C);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_CFG_DEV);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_PHY_OFF);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ClearWDT(); // Service the WDT
        break;
    case 429175000:
        PROFILE_CH_FREQ_32bit_200002EC = 429200000;
        break;
    case 429200000:
        PROFILE_CH_FREQ_32bit_200002EC = 429225000;
        break;
    case 429225000:
        PROFILE_CH_FREQ_32bit_200002EC = 426075000;
        PROFILE_RADIO_AFC_CFG1_32bit_2000031C = 0x0005005A;
        ClearWDT(); // Service the WDT
        ADF7030_CHANGE_STATE(STATE_PHY_ON);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_PHY_OFF);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_WRITE_REGISTER_NOPOINTER_LONGADDR_MSB(ADDR_PROFILE_RADIO_AFC_CFG1, PROFILE_RADIO_AFC_CFG1_32bit_2000031C);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_CFG_DEV);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ADF7030_CHANGE_STATE(STATE_PHY_OFF);
        WaitForADF7030_FIXED_DATA(); //�ȴ�оƬ����/�ɽ���CMD״̬
        DELAY_30U();
        ClearWDT(); // Service the WDT
        break;
    default:
        PROFILE_CH_FREQ_32bit_200002EC = 426075000;
        break;
    }
}
