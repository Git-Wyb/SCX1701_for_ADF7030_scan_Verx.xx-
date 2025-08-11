#include <string.h>
#include "CH376INC.H"
#include "type_def.h"
#include "uart.h"
#include "CH376.h"
#include "ram.h"
#include "Pin_define.h"
#include <iostm8l151g4.h>
#include "Timer.h"

#define RECNT 0
UINT8 buf[64];
UINT16 total;		/* 记录当前缓冲在FILE_DATA_BUF中的数据长度 */
UINT32 NewSize;	/* 临时变量 */
UINT8 FileDataBuf[0x4000];	/* 文件缓冲区,太小则速度慢 */
UINT16 si = 0;
UINT16 sx = 0;
UINT16 slen = 0;
UINT8 CH376_USB_Del(void)
{
    UINT8 s = 0;
    UINT8 sta = 0;

    if(CH376_HOST_INIT() != USB_INT_SUCCESS) return  ERR_USB_UNKNOWN;
    else
    {
RE_WRITE:
        sta++;
        if(CH376DiskConnect() != USB_INT_SUCCESS) return  ERR_USB_UNKNOWN;
        else
        {
            //return 0x55;
            mDelaymS( 200 );/* 延时,可选操作,有的USB存储器需要几十毫秒的延时 */
            for ( s = 0; s < 10; s ++ )
            {  /* 最长等待时间,10*50mS */
                mDelaymS( 50 );
                if ( CH376DiskMount() == USB_INT_SUCCESS ) break;  /* 初始化磁盘并测试磁盘是否就绪 */
            }
            s = CH376DiskQuery( (PUINT32)buf );  /* 查询磁盘剩余空间信息,扇区数 */
            if ( s != USB_INT_SUCCESS ){if(sta > RECNT) return ERR_USB_UNKNOWN;else {goto RE_WRITE;}}

            //s = CH376DirCreate( "/Logs" );  /* 新建或者打开目录,该目录建在根目录下 */
            //if ( s == USB_INT_SUCCESS || s == ERR_FOUND_NAME) {}
            //else {return ERR_USB_UNKNOWN;}
            s = CH376FileCreate( "LOG_2025.TXT" );  /* 在当前目录下新建文件,如果文件已经存在则先删除后再新建 */
            if ( s != USB_INT_SUCCESS ){if(sta > RECNT) return ERR_USB_UNKNOWN;else {goto RE_WRITE;}}
            Receiver_LED_RX = 1;
            strcpy( buf, "Chis is 演示数据\r\n" );
            //strcpy( buf, "aa bb cc" );
            s = CH376ByteWrite( buf, strlen(buf), NULL );  /* 以字节为单位向当前位置写入数据块 */
            if ( s != USB_INT_SUCCESS ){if(sta > RECNT) return ERR_USB_UNKNOWN;else {goto RE_WRITE;}}
            /*
            strcpy( buf, "2025/07/09_14:10:21_STX0031_11111111_OPEN_-74dBm\r\n" );
            s = CH376ByteWrite( buf, strlen(buf), NULL );  // 以字节为单位向当前位置写入数据块
            if ( s != USB_INT_SUCCESS ){if(sta > RECNT) return ERR_USB_UNKNOWN;else {goto RE_WRITE;}}
            for(si=0; si<10; si++)
            {
                strcpy( buf, "2025/07/09_14:22:45_STX0031_22222222_CLOSE_-50dBm\r\n" );
                s = CH376ByteWrite( buf, strlen(buf), NULL );
                if ( s != USB_INT_SUCCESS ){if(sta > RECNT) return ERR_USB_UNKNOWN;else {goto RE_WRITE;}}
                ClearWDT();
                mDelaymS( 100 );
            } */
            s = CH376FileClose( TRUE );  /* 关闭文件,自动更新文件长度 */
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;
        }
        return 0x55;
    }
}


void CH376_PORT_INIT(void)
{
    UART1_INIT();
}
 UINT8	res;
 UINT8	ass;
 UINT8 re_cnt = 0;
UINT8 CH376_HOST_INIT(void)
{
    //UINT8	res;
    re_cnt = 0;
REINIT:
    mDelaymS(100);
    xWriteCH376Cmd(CMD11_CHECK_EXIST);  /* 测试单片机与CH376之间的通讯接口 */
    xWriteCH376Data(0x55);   //返回数据按位取反
    re_cnt++;
    ClearWDT();
    res = xReadCH376Data();
	if (res != 0xAA)
    {
        if(re_cnt < 5) goto REINIT;
        else
        {
            re_cnt = 0;
            return( ERR_USB_UNKNOWN );  /* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),串口波特率,一直在复位,晶振不工作 */
        }
    }
    Receiver_LED_TX = 1;
    re_cnt = 0;

REHOST:
    mDelaymS(100);
    xWriteCH376Cmd( CMD11_SET_USB_MODE );
    xWriteCH376Data( 0x06 );
    re_cnt++;
    ClearWDT();
    mDelayuS(20);
    ass = xReadCH376Data();
	if ( ass == CMD_RET_SUCCESS )
    {
        re_cnt = 0;
        return( USB_INT_SUCCESS );
    }
	else
    {
        if(re_cnt < 5) goto REHOST;
        else
        {
            re_cnt = 0;
            return( ERR_USB_UNKNOWN );  /* 设置模式错误 */
        }
    }
}

UINT8 CH376DiskConnect(void)
{
    UINT16 sta = 0;
    while(1)
    {
        ClearWDT();
        xWriteCH376Cmd( CMD0H_DISK_CONNECT );
        mDelaymS(100);
        if(CH376GetIntStatus() == USB_INT_SUCCESS) return  USB_INT_SUCCESS;
        if(sta++ > 300) return ERR_USB_UNKNOWN;  //30s超时
    }
}


void xWriteCH376Cmd(UINT8 mCmd)  /* 向CH376写命令 */
{
    Send_char(SER_SYNC_CODE1);
    Send_char(SER_SYNC_CODE2);
    Send_char(mCmd);
    mDelayuS(5);
}

void xWriteCH376Data(UINT8 mData)  /* 向CH376写数据 */
{
    Send_char(mData);
}

UINT8 xReadCH376Data(void)  /* 从CH376读数据 */
{
    UINT32	i;
	for ( i = 0; i < 500000; i ++ )
    {  /* 计数防止超时 */
        if(flag_rx_done)
        {
            flag_rx_done = 0;
            return uart_rx_data;
        }
        ClearWDT();
	}
	return(0);  /* 不应该发生的情况 */
}

UINT8 CH376GetIntStatus(void)  /* 获取中断状态并取消中断请求 */
{
	UINT8	s;
	xWriteCH376Cmd( CMD01_GET_STATUS );
	s = xReadCH376Data( );
	return(s);
}

UINT8 CH376SendCmdWaitInt(UINT8 mCmd)  /* 发出命令码后,等待中断 */
{
    UINT8 sta = 0;

    while(1)
    {
        ClearWDT();
        xWriteCH376Cmd( mCmd );
        mDelaymS(10);
        if(CH376GetIntStatus() == USB_INT_SUCCESS) return  USB_INT_SUCCESS;
        if(sta++ > 5) return ERR_USB_UNKNOWN;  //250ms超时
    }
}

UINT8	CH376SendCmdDatWaitInt( UINT8 mCmd, UINT8 mDat )  /* 发出命令码和一字节数据后,等待中断 */
{
	xWriteCH376Cmd( mCmd );
	xWriteCH376Data( mDat );
	return( Wait376Interrupt( ) );
}

UINT8 Wait376Interrupt(void)
{
    //UINT8 cnt = 0;
    //UINT8 red = 0;

    UINT32	i;
	for ( i = 0; i < 5000000; i ++ )
    {  /* 计数防止超时,默认的超时时间,与单片机主频有关 */
		if ( Query376Interrupt( ) )
        {
            return( CH376GetIntStatus( ) );  /* 检测到中断 */
        }
	}
	return( ERR_USB_UNKNOWN );  /* 不应该发生的情况 */
    /*
    while(1)
    {
        ClearWDT();
        red = CH376GetIntStatus();
        //if(red != 0) return red;
        if(cnt++ > 3) return CH376GetIntStatus();  //30ms
        mDelaymS(10);
    }*/
}

UINT8 CH376DiskMount(void)  /* 初始化磁盘并测试磁盘是否就绪 */
{
	return( CH376SendCmdWaitInt( CMD0H_DISK_MOUNT ) );
}

UINT8 CH376ReadVar8( UINT8 var )  /* 读CH376芯片内部的8位变量 */
{
	UINT8 c0;
	xWriteCH376Cmd( CMD11_READ_VAR8 );
	xWriteCH376Data( var );
	c0 = xReadCH376Data( );
	return( c0 );
}

void CH376WriteVar8( UINT8 var, UINT8 dat )  /* 写CH376芯片内部的8位变量 */
{
	xWriteCH376Cmd( CMD20_WRITE_VAR8 );
	xWriteCH376Data( var );
	xWriteCH376Data( dat );
}

UINT32	CH376ReadVar32( UINT8 var )  /* 读CH376芯片内部的32位变量 */
{
	xWriteCH376Cmd( CMD14_READ_VAR32 );
	xWriteCH376Data( var );
	return( CH376Read32bitDat( ) );  /* 从CH376芯片读取32位的数据并结束命令 */
}

void	CH376WriteVar32( UINT8 var, UINT32 dat )  /* 写CH376芯片内部的32位变量 */
{
	xWriteCH376Cmd( CMD50_WRITE_VAR32 );
	xWriteCH376Data( var );
	xWriteCH376Data( (UINT8)dat );
	xWriteCH376Data( (UINT8)( (UINT16)dat >> 8 ) );
	xWriteCH376Data( (UINT8)( dat >> 16 ) );
	xWriteCH376Data( (UINT8)( dat >> 24 ) );
}

UINT32 CH376Read32bitDat( void )  /* 从CH376芯片读取32位的数据并结束命令 */
{
	UINT8	c0, c1, c2, c3;
	c0 = xReadCH376Data( );
	c1 = xReadCH376Data( );
	c2 = xReadCH376Data( );
	c3 = xReadCH376Data( );
	return( c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24 );
}

UINT8 CH376DiskQuery(PUINT32 DiskFre)  /* 查询磁盘剩余空间信息,扇区数 */
{
	UINT8	s;
	UINT8	c0, c1, c2, c3;
	xWriteCH376Cmd( CMD01_GET_IC_VER );
	if ( xReadCH376Data( ) < 0x43 ) {
		if ( CH376ReadVar8( VAR_DISK_STATUS ) >= DEF_DISK_READY ) CH376WriteVar8( VAR_DISK_STATUS, DEF_DISK_MOUNTED );
	}
	s = CH376SendCmdWaitInt( CMD0H_DISK_QUERY );
	if ( s == USB_INT_SUCCESS ) {  /* 参考CH376INC.H文件中CH376_CMD_DATA结构的DiskQuery */
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data( );  /* 长度总是sizeof(CH376_CMD_DATA.DiskQuery) */
		xReadCH376Data( );  /* CH376_CMD_DATA.DiskQuery.mTotalSector */
		xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		c0 = xReadCH376Data( );  /* CH376_CMD_DATA.DiskQuery.mFreeSector */
		c1 = xReadCH376Data( );
		c2 = xReadCH376Data( );
		c3 = xReadCH376Data( );
		*DiskFre = c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24;
		xReadCH376Data( );  /* CH376_CMD_DATA.DiskQuery.mDiskFat */
	}
	else *DiskFre = 0;
	return( s );
}

void	CH376SetFileName( PUINT8 name )  /* 设置将要操作的文件的文件名 */
{
	UINT8	c;
	UINT8	s;
	xWriteCH376Cmd( CMD01_GET_IC_VER );
	if ( xReadCH376Data( ) < 0x43 ) {
		if ( CH376ReadVar8( VAR_DISK_STATUS ) < DEF_DISK_READY ) {
			xWriteCH376Cmd( CMD10_SET_FILE_NAME );
			xWriteCH376Data( 0 );
			s = CH376SendCmdWaitInt( CMD0H_FILE_OPEN );
			if ( s == USB_INT_SUCCESS ) {
				s = CH376ReadVar8( 0xCF );
				if ( s ) {
					CH376WriteVar32( 0x4C, CH376ReadVar32( 0x4C ) + ( (UINT16)s << 8 ) );
					CH376WriteVar32( 0x50, CH376ReadVar32( 0x50 ) + ( (UINT16)s << 8 ) );
					CH376WriteVar32( 0x70, 0 );
				}
			}
		}
	}
	xWriteCH376Cmd( CMD10_SET_FILE_NAME );
	c = *name;
	xWriteCH376Data( c );
	while ( c ) {
		name ++;
		c = *name;
		if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;  /* 强行将文件名截止 */
		xWriteCH376Data( c );
	}
}

UINT8	CH376FileCreate( PUINT8 name )  /* 在根目录或者当前目录下新建文件,如果文件已经存在那么先删除 */
{
	if ( name ) CH376SetFileName( name );  /* 设置将要操作的文件的文件名 */
	return( CH376SendCmdWaitInt( CMD0H_FILE_CREATE ) );
}

UINT8	CH376DirCreate( PUINT8 name )  /* 在根目录下新建目录(文件夹)并打开,如果目录已经存在那么直接打开 */
{
	CH376SetFileName( name );  /* 设置将要操作的文件的文件名 */
	if ( name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2 ) CH376WriteVar32( VAR_CURRENT_CLUST, 0 );
	return( CH376SendCmdWaitInt( CMD0H_DIR_CREATE ) );
}

UINT8	CH376FileClose( UINT8 UpdateSz )  /* 关闭当前已经打开的文件或者目录(文件夹) */
{
	return( CH376SendCmdDatWaitInt( CMD1H_FILE_CLOSE, UpdateSz ) );
}

UINT8	CH376ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )  /* 以字节为单位向当前位置写入数据块 */
{
	UINT8	s;
	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );

	if ( RealCount ) *RealCount = 0;
	while ( 1 ) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_WRITE ) {
			s = CH376WriteReqBlock( buf );  /* 向内部指定缓冲区写入请求的数据块,返回长度 */
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
			buf += s;
			if ( RealCount ) *RealCount += s;
		}
/*		else if ( s == USB_INT_SUCCESS ) return( s );*/  /* 结束 */
		else return( s );  /* 错误 */
	}
}

UINT8	CH376WriteReqBlock( PUINT8 buf )  /* 向内部指定缓冲区写入请求的数据块,返回长度 */
{
	UINT8	s, l;
	xWriteCH376Cmd( CMD01_WR_REQ_DATA );
	s = l = xReadCH376Data( );  /* 长度 */
	if ( l ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- l );
	}
	return( s );
}

UINT8 Query376Interrupt( void )
{
	return( KEY_SW4 ? FALSE : TRUE );  /* 如果连接了CH376的中断引脚则直接查询中断引脚 */
}

