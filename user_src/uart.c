/***********************************************************************/
/*  FILE        :Uart.c                                                */
/*  DATE        :Mar, 2014                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  DESCRIPTION :                                                      */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <iostm8l151g4.h> // CPU型号
#include "Pin_define.h"   // 管脚定义
#include "initial.h"	  // 初始�? 预定�?
#include "ram.h"		  // RAM定义
#include "eeprom.h"		  // eeprom
#include "uart.h"
#define TXD1_enable (USART1_CR2 = 0x08) // 允许发�??
#define RXD1_enable (USART1_CR2 = 0x24) // 允许接收及其中断

u8 u1busyCache = 0;
u8 u1InitCompleteFlag = 0;

UINT8 UartStatus = FrameHeadSataus;
UINT8 UartLen = 0;
UINT8 UartCount = 0;
UINT8 UART_DATA_buffer[41] = {0};
UINT8 UART_DATA_ID98[41] = {0};

__Databits_t Databits_t;
__U1Statues U1Statues;
UINT8 ACKBack[3] = {0x02, 0x03, 0x00};
unsigned int U1AckTimer = 0;

UINT8 FLAG_testNo91=0;
UINT8 FLAG_testBEEP=0;
//UINT8 FLAG_testNo91_step=0;
UINT8 FLAG_testNo91SendUart=0;



//********************************************
void UART1_INIT(void)
{
	unsigned int baud_div = 0;
	u1InitCompleteFlag = 0;

	SYSCFG_RMPCR1_USART1TR_REMAP = 0;
	USART1_CR1_bit.M = 0;
	USART1_CR1_bit.PCEN = 0;
	USART1_CR1_bit.PS = 0;
	USART1_CR2_bit.TIEN = 0;
	USART1_CR2_bit.TCIEN = 0;
	USART1_CR2_bit.RIEN = 1;
	USART1_CR2_bit.ILIEN = 0;
	USART1_CR2_bit.TEN = 1;
	USART1_CR2_bit.REN = 1;

	/*设置波特�? */
	baud_div = 16000000 / 9600; /*求出分频因子*/
	USART1_BRR2 = baud_div & 0x0f;
	USART1_BRR2 |= ((baud_div & 0xf000) >> 8);
	USART1_BRR1 = ((baud_div & 0x0ff0) >> 4); /*先给BRR2赋�??�?后再设置BRR1*/

	u1InitCompleteFlag = 1;
}
void UART1_end(void)
{ //
	SYSCFG_RMPCR1_USART1TR_REMAP = 0;

	USART1_CR1 = 0; // 1个起始位,8个数据位
	USART1_CR3 = 0; // 1个停止位
	USART1_CR4 = 0;
	USART1_CR5 = 0x00;  // 半双工模�?
	USART1_BRR2 = 0x00; // 设置波特�?600
	USART1_BRR1 = 0x00; // 3.6864M/9600 = 0x180
						//16.00M/9600 = 0x683
	USART1_CR2 = 0x00;  //禁止串口
}
//--------------------------------------------
void UART1_RX_RXNE(void)
{ // RXD中断服务程序
	unsigned char dat;
    if(USART1_SR_bit.RXNE == 1)
    {
        uart_rx_data = USART1_DR; // 接收数据
        flag_rx_done = 1;
    }
					 //Send_char(dat);
	//ReceiveFrame(dat);
	// if (dat == '(')
	// 	SIO_cnt = 0;
	// SIO_buff[SIO_cnt] = dat;
	// SIO_cnt = (SIO_cnt + 1) & 0x1F;
	// if (dat == ')')
	// {
	// 	for (dat = 0; dat < SIO_cnt; dat++)
	// 	{
	// 		SIO_DATA[dat] = SIO_buff[dat];
	// 	}
	// 	BIT_SIO = 1; // 标志
	// 				 //SIO_TOT = 20;
	// }
}

//--------------------------------------------
void Send_char(unsigned char ch)
{
    flag_rx_done = 0;
	TXD1_enable; // 允许发�??
	while (!USART1_SR_TXE)
		;
	USART1_DR = ch; // 发�??
	while (!USART1_SR_TC)
		;		 // 等待完成发�??
	RXD1_enable; // 允许接收及其中断
}
//--------------------------------------------
void Send_String(unsigned char *string)
{ // 发�?�字符串
	unsigned char i = 0;
	TXD1_enable; // 允许发�??
	while (string[i])
	{
		while (!USART1_SR_TXE)
			;				   // �?查发送OK
		USART1_DR = string[i]; // 发�??
		i++;
	}
	while (!USART1_SR_TC)
		;		 // 等待完成发�??
	RXD1_enable; // 允许接收及其中断
				 //	BIT_SIO = 0;							// 标志
}
void Send_Data(unsigned char *P_data, unsigned int length)
{ // 发�?�字符串
	unsigned int i = 0;
	TXD1_enable; // 允许发�??
	for (i = 0; i < length; i++)
	{
		while (!USART1_SR_TXE)
			;					   // �?查发送OK
		USART1_DR = *(P_data + i); // 发�??
	}
	while (!USART1_SR_TC)
		;		 // 等待完成发�??
	RXD1_enable; // 允许接收及其中断
				 //	BIT_SIO = 0;							// 标志
}

/***********************************************************************/
unsigned char asc_hex(unsigned char asc) // HEX
{
	unsigned char i;
	if (asc < 0x3A)
		i = asc & 0x0F;
	else
		i = asc - 0x37;
	return i;
}

unsigned char hex_asc(unsigned char hex)
{
	unsigned char i;
	hex = hex & 0x0F;
	if (hex < 0x0A)
		i = hex | 0x30;
	else
		i = hex + 0x37;
	return i;
}

unsigned char asc_hex_2(unsigned char asc1, unsigned char asc0)
{
	unsigned char i;
	i = (asc_hex(asc1) << 4) + (asc_hex(asc0) & 0x0F);
	return i;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*
void PC_PRG(void) // 串口命令
{
	unsigned int i, j;
	unsigned char d3, d2, d1, d0;

	if (BIT_SIO)
	{
		BIT_SIO = 0;
		//SIO_TOT = 20;
		switch (SIO_DATA[1])
		{
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		//%                 写操�?              %
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		case 'W':
			//==================================== ADF7012
			if (SIO_DATA[2] == 'I') // (WIxd0d1d2d3)
			{
				i = asc_hex_2(0x30, SIO_buff[3]);
				d0 = asc_hex_2(SIO_buff[4], SIO_buff[5]);
				d1 = asc_hex_2(SIO_buff[6], SIO_buff[7]);
				d2 = asc_hex_2(SIO_buff[8], SIO_buff[9]);
				d3 = asc_hex_2(SIO_buff[10], SIO_buff[11]);

				//write Rx
				ROM_adf7030_value[i].byte[0] = d0;
				ROM_adf7030_value[i].byte[1] = d1;
				ROM_adf7030_value[i].byte[2] = d2;
				ROM_adf7030_value[i].byte[3] = d3;
				//ttset dd_write_7021_reg(&ROM_adf7030_value[i].byte[0]);
				Delayus(122); //delay 40us

				//-------------------------------- 保存
				if (i == 1)
				{
					j = 0x380 + i * 4;
					UnlockFlash(UNLOCK_EEPROM_TYPE);
					WriteByteToFLASH(addr_eeprom_sys + j, d0);
					WriteByteToFLASH(addr_eeprom_sys + j + 1, d1);
					WriteByteToFLASH(addr_eeprom_sys + j + 2, d2);
					WriteByteToFLASH(addr_eeprom_sys + j + 3, d3);
					LockFlash(UNLOCK_EEPROM_TYPE);

					ClearWDT(); // Service the WDT
				}
				//-------------------------------返回  (WHx)
				d1 = '(';
				d2 = 'W';
				Send_char(d1);
				Send_char(d2);
				d1 = 'I';
				d2 = ')';
				Send_char(d1);
				Send_char(d2);
				Send_char(SIO_buff[3]);
			}

			//==================================== ADF7012 TX/RX调试
			if (SIO_DATA[2] == 'J') // (WJx)
			{
				Tx_Rx_mode = asc_hex_2(0x30, SIO_buff[3]);
				//-------------------------------返回  (WHx)
				d1 = '(';
				d2 = 'W';
				Send_char(d1);
				Send_char(d2);
				d1 = 'J';
				d2 = ')';
				Send_char(d1);
				Send_char(d2);
				Send_char(SIO_buff[3]);
			}
			break;
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		//%                 读操�?              %
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		case 'R':
			//==================================== ADF7012   //(RIx)
			if (SIO_DATA[2] == 'I') // (RI)d0d1d2d3
			{
				i = asc_hex_2(0x30, SIO_buff[3]);
				d1 = '(';
				d2 = 'R';
				Send_char(d1);
				Send_char(d2);
				d1 = 'I';
				d2 = ')';
				Send_char(d1);
				Send_char(d2);
				for (j = 0; j < 4; j++)
				{
					d1 = hex_asc(ROM_adf7030_value[i].byte[j] / 16);
					d2 = hex_asc(ROM_adf7030_value[i].byte[j] % 16);
					Send_char(d1);
					Send_char(d2);
				}
			}
			break;
		default:
			break;
		}
	}
} */
void ReceiveFrame(UINT8 Cache)
{
	switch (UartStatus)
	{
	case FrameHeadSataus:
	{
		UART_DATA_buffer[0] = UART_DATA_buffer[1];
		UART_DATA_buffer[1] = UART_DATA_buffer[2];
		UART_DATA_buffer[2] = Cache;
		if ((UART_DATA_buffer[0] == FrameHead) &&
			(UART_DATA_buffer[2] == FrameSingnalID))
		{
			U1Statues = ReceivingStatues;
			UartStatus++;
			UartLen = UART_DATA_buffer[1];
		}
	}
	break;
	case DataStatus:
	{
		UART_DATA_buffer[UartCount + 3] = Cache;
		UartCount++;
		if (UartCount >= (UartLen - 3))
			UartStatus++;
	}
	break;
	default:
		UartStatus = 0;
		U1Statues = IdelStatues;
		break;
	}
	if (UartStatus == FrameEndStatus) //接收完一帧处理数�?
	{
		//add Opration function
		OprationFrame();
		UartStatus = 0;
		UartCount = 0;
		//        Receiver_LED_OUT_INV = !Receiver_LED_OUT_INV;
		if((Databits_t.ID_No == 147)||(Databits_t.ID_No == 152)) U1Statues = IdelStatues;
		else
		{
			U1Statues = ReceiveDoneStatues;
		    U1AckTimer = U1AckDelayTime;
		    U1Busy_OUT = 1;
		}

	}
}

void OprationFrame(void)
{
	unsigned char i;
	for (i = 0; i < 4; i++)
		Databits_t.Data[i] = UART_DATA_buffer[3 + i];
	if (Databits_t.ID_No == 146)  //0x92
	{
	    FLAG_APP_TX_fromUART=1;
		if(TIMER1s);
		else Uart_Struct_DATA_Packet_Contro.Fno_Type.UN.fno=0;
		//for(i=0;i<3;i++)Uart_Struct_DATA_Packet_Contro.data[i/2].uc[i%2]=Databits_t.Data[i+1];
		//for(i=3;i<8;i++)Uart_Struct_DATA_Packet_Contro.data[i/2].uc[i%2]=0x00;

		for(i=0;i<2;i++)Uart_Struct_DATA_Packet_Contro.data[i/2].uc[i%2]=Databits_t.Data[i+1];
		if((Databits_t.Statues==3)||(Databits_t.Statues==4))Flag_shutter_stopping=1;
		else Flag_shutter_stopping=0;
		ACKBack[2] = 0;
		switch (Databits_t.Mode)
		{
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			break;
		default:
			ACKBack[2] = 1;
			return;
			break;
		}
		switch (Databits_t.Statues)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			break;
		default:
			ACKBack[2] = 1;
			return;
			break;
		}
		switch (Databits_t.Abnormal)
		{
		case 0x00:
		case 0x04:
		case 0x06:
		case 0x45:
		case 0x46:
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B:
		case 0x4C:
		case 0x4D:
			break;
		default:
			ACKBack[2] = 1;
			return;
			break;
		}
	}
	else if (Databits_t.ID_No == 152)  //0x98
	{
	   	Flag_ERROR_Read_once_again=0;
		TIME_ERROR_Read_once_again=0;
		for (i = 0; i < 41; i++)UART_DATA_ID98[i]=UART_DATA_buffer[i];
		FLAG_APP_TX_fromUART_err_read=1;
		Time_error_read_timeout=(UART_DATA_ID98[1]+1)*7;
		ERROR_Read_sendTX_count=0;
		ERROR_Read_sendTX_packet=0;
		Time_error_read_gap=38;
	}
	else if (Databits_t.ID_test_No91or93 == 145)  //0x91
	{
	    if(ID_DATA_PCS==0)
	    	{
			ACKBack[2] = 0;
			FLAG_testNo91=1;
				//if(FLAG_testNo91_step<3)
			     //  FLAG_testNo91_step++;
			FLAG_testNo91SendUart=0;
			//TIME_TestNo91=1000;
	    	}
		else
			{
			ACKBack[2] = 1;
			FLAG_testNo91=2;
			//TIME_TestNo91=1000;
			FLAG_testBEEP=1;
			}
	}
	else if (Databits_t.ID_test_No91or93 == 147)  //0x93
	{
		switch (Databits_t.SWorOUT)
		{
		case 0x01:
			DATA_Packet_Control=0x08;
			TIMER1s = 1000;
			break;
		case 0x02:
			DATA_Packet_Control=0x04;
			TIMER1s = 1000;
			break;
		case 0x04:
			DATA_Packet_Control=0x02;
			TIMER1s = 1000;
			break;
		case 0xFA:
			FLAG_testBEEP=1;
			break;
		case 0xFB:
			FLAG_testBEEP=2;
			break;
		case 0xFC:
			FLAG_testBEEP=3;
			break;
		default:
			break;
		}

	}
	else
	{
		ACKBack[2] = 1;
		return;
	}
}

void TranmissionACK(void)
{
	if (u1InitCompleteFlag)
	{
		if ((U1Statues == ReceiveDoneStatues) && (U1AckTimer == 0))
		{
			U1Busy_OUT = 1;
			U1Statues = ACKingStatues;
			Send_Data(ACKBack, 3);
			U1Statues = IdelStatues;
			U1Busy_OUT = 1;
		}
	}

	if((Flag_ERROR_Read_once_again==1)&&(TIME_ERROR_Read_once_again==0))
	{
		Send_Data(Send_err_com, 7);
		Flag_ERROR_Read_once_again=0;
		TIME_ERROR_Read_once_again=0;
	}
}
