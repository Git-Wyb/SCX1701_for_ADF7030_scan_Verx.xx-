#include "CH376.h"
#include "CH376FileName.h"
#include "IIC.h"
#include "ID_Decode.h"
#include "eeprom.h"

UINT8 buf[100];
UINT16 si = 0;
UINT8 File_Name[] = {"Log_20250710_162210.txt"}; //文件名长名称
UINT8 reFile_Name[] = "/LOGS/10102938.TXT"; //文件名长名称的替代名,不能重复。文件名 8+3格式 字母必须大写。文件名称最多只能8个字节
UINT8 File_Data0[] = {"<Registered Remote Controls>\r\n"};
UINT8 File_Data1[] = {"<Remote Control ID List>\r\n"};
UINT8 File_Data2[] = {"<Operation History>\r\n"};
UINT8 File_IDNums[5] = {""};
UINT8 File_ID[10] = {""};
UINT8 *TYPE[] = {"STX0011","STX0031","STX1231","STX1531","CTX1531","STX1631","STX1731"};
UINT8 *CTRL[] = {"OPEN","STOP","CLOSE"};

UINT8 CH376FileOpen( PUINT8 name );
UINT8 s = 0;
UINT8 CH376_USB_Del(void)
{
    //STRUCT_DATE H_DATA;
    u16 Paddr = AddrEeprom_StartHistory;
    u32 r_id = 0;
    if(CH376_HOST_INIT() != USB_INT_SUCCESS) return  ERR_USB_UNKNOWN;
    else
    {
        s = CH376DiskConnect();
        if(s != USB_INT_SUCCESS) return  s;
        else
        {
            mDelaymS( 200 );/* 延时,可选操作,有的USB存储器需要几十毫秒的延时 */
            for ( s = 0; s < 10; s ++ )
            {  /* 最长等待时间,10*50mS */
                mDelaymS( 50 );
                if ( CH376DiskMount() == USB_INT_SUCCESS ) break;  /* 初始化磁盘并测试磁盘是否就绪 */
            }
            s = CH376DiskQuery( (PUINT32)buf );  /* 查询磁盘剩余空间信息,扇区数 */
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;

            s = CH376DirCreate( "/LOGS" );  /* 新建或者打开目录,该目录建在根目录下 */
            if ( s == USB_INT_SUCCESS || s == ERR_FOUND_NAME) {}
            else return ERR_USB_UNKNOWN;

            GetTime();
            sprintf(File_Name,"Log_20%0*d%0*d%0*d_%0*d%0*d%0*d.txt",2,Now_Year,2,Now_Mon,2,Now_Day,2,Now_Hour,2,Now_Min,2,Now_Sec);
            sprintf(reFile_Name,"/LOGS/%0*d%0*d%0*d%0*d.TXT",2,Now_Day,2,Now_Hour,2,Now_Min,2,Now_Sec);
            s = CH376_CreateFile_Name(reFile_Name,File_Name); //新建文件
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;
            //Receiver_LED_RX = 1;

            //s = SetFileCreateTime( "LOG_2025.TXT", MAKE_FILE_DATE( 2025, 8, 15 ), MAKE_FILE_TIME( 10, 23, 14 ) );  /* 为指定文件设置创建日期和时间 */
            //if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;
            //mDelaymS( 200 ); &reFile_Name[6]
            s = CH376FileOpen(&reFile_Name[6]); //上面已经新建/打开目录，所以此步不需要文件夹路径了。
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;

            Receiver_LED_RX = 0;
            Receiver_LED_TX = 0;
            Receiver_LED_OUT = 0;
            PowerLED = 0;
            flag_usb_state = 1;
            s = strlen(File_Data0);
            s = CH376ByteWrite( File_Data0, s, NULL);
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;

            ID_Nums = Get_IDNums();
            s = sprintf(File_IDNums,"%d\r\n",ID_Nums);
            s = CH376ByteWrite(File_IDNums, s, NULL);
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;

            s = strlen(File_Data1);
            s = CH376ByteWrite(File_Data1, s, NULL);
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;

            if(ID_Nums > 0)
            {
                s = sprintf(File_ID,"%08ld\r\n",ID_SCX1801_DATA);
                s = CH376ByteWrite(File_ID, s, NULL);
                if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;
                ID_Nums = ID_Nums - 1;
            }
            for(si=0; si<ID_Nums; si++)
            {
                s = sprintf(File_ID,"%08ld\r\n",ID_Receiver_DATA_READ(&ID_Receiver_DATA[si][0]));
                s = CH376ByteWrite(File_ID, s, NULL);
                if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;
            }

            s = strlen(File_Data2);
            s = CH376ByteWrite(File_Data2, s, NULL);
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;

            for(si=0; si<His_Num; si++)
            {
                mDelaymS( 10 );
                Read_HisData(Paddr,1);
                r_id = ID_Receiver_DATA_READ(HIS_DATA[0].History_s.IDD);
                if(HIS_DATA[0].History_s.CODE == 0x08) s = 0;
                else if (HIS_DATA[0].History_s.CODE == 0x04) s = 1;
                else if (HIS_DATA[0].History_s.CODE == 0x02) s = 2;
                else s = 1;
                s = sprintf( buf,"20%0*d/%0*d/%0*d_%0*d:%0*d:%0*d_%s_%08ld_%s_-%ddBm\r\n",2,HIS_DATA[0].History_s.YY,2,HIS_DATA[0].History_s.MM,2,HIS_DATA[0].History_s.DD,
                            2,HIS_DATA[0].History_s.HH,2,HIS_DATA[0].History_s.MI,2,HIS_DATA[0].History_s.SS,TYPE[CheckID_Type(r_id)],r_id,CTRL[s],HIS_DATA[0].History_s.RS);
                s = CH376ByteWrite( buf, s, NULL);
                ClearWDT();
                if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;
                Paddr += 11;
            }

            s = CH376FileClose( TRUE );  /* 关闭文件,自动更新文件长度 */
            if ( s != USB_INT_SUCCESS ) return ERR_USB_UNKNOWN;
        }
        return 0x55;
    }
}
//s = sprintf( buf, "2025/07/09_14:22:45_STX0031_22222222_CLOSE_-50dBm,sta = %d\r\n",sta );


void CH376_PORT_INIT(void)
{
    UART1_INIT();
    PCF8563_RSTI = 1;
    mDelaymS(50);
    PCF8563_RSTI = 0;
}

UINT8 res = 0;
UINT8 re_cnt = 0;
UINT8 CH376_HOST_INIT(void)
{
    CH376_PORT_INIT();
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
    //Receiver_LED_TX = 1;
    re_cnt = 0;

REHOST:
    mDelaymS(100);
    xWriteCH376Cmd( CMD11_SET_USB_MODE );
    xWriteCH376Data( 0x06 );
    re_cnt++;
    ClearWDT();
    mDelayuS(20);
    res = xReadCH376Data();
	if ( res == CMD_RET_SUCCESS )
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
        if(sta++ > 6000) return ERR_USB_UNKNOWN;  //等待10分钟超时
        if(SW_USB_IN == 1) return 2;
    }
}


void xWriteCH376Cmd(UINT8 mCmd)  /* 向CH376写命令 */
{
    Send_char(SER_SYNC_CODE1);
    Send_char(SER_SYNC_CODE2);
    Send_char(mCmd);
}

void xWriteCH376Data(UINT8 mData)  /* 向CH376写数据 */
{
    Send_char(mData);
}

UINT8 xReadCH376Data(void)  /* 从CH376读数据 */
{
    UINT32	i;
	for ( i = 0; i < 500000; i ++ ) //约4s
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
        if(sta++ > 3) return CH376GetIntStatus();
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
    UINT8 cnt = 0;
    UINT8 red = 0;
    /*
    UINT32	i;
	for ( i = 0; i < 5000000; i ++ ) //4s
    {
		if ( Query376Interrupt( ) )
        {
            return( CH376GetIntStatus( ) );
        }
        ClearWDT();
	}
	return( ERR_USB_UNKNOWN );
    */
    while(1)
    {
        ClearWDT();
        mDelaymS(10);
        red = CH376GetIntStatus();
        if(red != 0) return red;
        if(cnt++ > 3) return CH376GetIntStatus();  //30ms
    }
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

UINT8 CH376_Byte_Write(PUINT8 buf, UINT16 ReqCount)
{
    UINT8 s;
    xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );
    while(1)
    {
        s = Wait376Interrupt( );
        if ( s == USB_INT_DISK_WRITE )
        {
            s = CH376WriteReqBlock( buf );
            xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
            buf += s;
        }
        else return s;
    }
}


UINT8	CH376ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )  /* 以字节为单位向当前位置写入数据块 */
{
	UINT8	s,ci;
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
		else if ( s == USB_INT_SUCCESS ) return( s );  /* 结束 */
		else
        {
            for(ci=0; ci<3; ci++)
            {
                s = CH376GetIntStatus( );
                if ( s == USB_INT_SUCCESS ) return( s );
            }
            return( CH376GetIntStatus( ) );
        }
	}
}

UINT8	CH376ReadBlock( PUINT8 buf )  /* 从当前主机端点的接收缓冲区读取数据块,返回长度 */
{
	UINT8	s, l;
	xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
	s = l = xReadCH376Data( );  /* 长度 */
	if ( l ) {
		do {
			*buf = xReadCH376Data( );
			buf ++;
		} while ( -- l );
	}
	return( s );
}

UINT8	CH376ByteRead( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )  /* 以字节为单位从当前位置读取数据块 */
{
	UINT8	s;
	xWriteCH376Cmd( CMD2H_BYTE_READ );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );

	if ( RealCount ) *RealCount = 0;
	while ( 1 ) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_READ ) {
			s = CH376ReadBlock( buf );  /* 从当前主机端点的接收缓冲区读取数据块,返回长度 */
			xWriteCH376Cmd( CMD0H_BYTE_RD_GO );
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
