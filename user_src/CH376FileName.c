#include "CH376FileName.h"

UINT8 LongNameBuf[ LONG_NAME_BUF_LEN ];	/* 存放长文件名 */
UINT8 GlobalBuf[LONG_NAME_BUF_LEN] = {0};


UINT8 CH376_CreateFile_Name(PUINT8 PathName, PUINT8 LongName)
{
    UINT8	s;
	UINT16	j;

    for ( j = 0; LongName[j] != 0; j++ )
    {  /* 复制并转换长文件名到LongNameBuf长文件名缓冲区 */
			LongNameBuf[j*2] = LongName[j];  /* 将英文字符转换为两字节的小端UNICODE编码 */
			LongNameBuf[j*2+1] = 0x00;
	}
    LongNameBuf[j*2] = 0x00;  // 末尾用两个0表示结束
    LongNameBuf[j*2+1] = 0x00;
//		strcpy( PathNameBuf, "/LONGNAME.H" );  // 复制该长文件名的ANSI编码的全路径的短文件名(8+3格式),短文件名由长文件名生成(只要不冲突,其实随便定)
    s = CH376CreateLongName( PathName, LongNameBuf );  /* 新建具有长文件名的文件 */
    return s;
}

UINT8	CH376FileOpen( PUINT8 name )  /* 在根目录或者当前目录下打开文件或者目录(文件夹) */
{
	CH376SetFileName( name );  /* 设置将要操作的文件的文件名 */
	if ( name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2 ) CH376WriteVar32( VAR_CURRENT_CLUST, 0 );
	return( CH376SendCmdWaitInt( CMD0H_FILE_OPEN ) );
}

UINT8	CH376FileOpenDir( PUINT8 PathName, UINT8 StopName )  /* 打开多级目录下的文件或者目录的上级目录,支持多级目录路径,支持路径分隔符,路径长度不超过255个字符 */
/* StopName 指向最后一级文件名或者目录名 */
{
	UINT8	i, s;
	s = 0;
	i = 1;  /* 跳过有可能的根目录符 */
	while ( 1 ) {
		while ( PathName[i] != DEF_SEPAR_CHAR1 && PathName[i] != DEF_SEPAR_CHAR2 && PathName[i] != 0 ) ++ i;  /* 搜索下一个路径分隔符或者路径结束符 */
		if ( PathName[i] ) i ++;  /* 找到了路径分隔符,修改指向目标文件的最后一级文件名 */
		else i = 0;  /* 路径结束 */
		s = CH376FileOpen( &PathName[s] );  /* 打开文件或者目录 */
		if ( i && i != StopName ) {  /* 路径尚未结束 */
			if ( s != ERR_OPEN_DIR ) {  /* 因为是逐级打开,尚未到路径结束,所以,如果不是成功打开了目录,那么说明有问题 */
				if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );  /* 中间路径必须是目录名,如果是文件名则出错 */
				else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );  /* 中间路径的某个子目录没有找到,可能是目录名称错误 */
				else return( s );  /* 操作出错 */
			}
			s = i;  /* 从下一级目录开始继续 */
		}
		else return( s );  /* 路径结束,USB_INT_SUCCESS为成功打开文件,ERR_OPEN_DIR为成功打开目录(文件夹),其它为操作出错 */
	}
}

UINT8	CH376FileOpenPath( PUINT8 PathName )  /* 打开多级目录下的文件或者目录(文件夹),支持多级目录路径,支持路径分隔符,路径长度不超过255个字符 */
{
	return( CH376FileOpenDir( PathName, 0xFF ) );
}

UINT8	CH376SeparatePath( PUINT8 path )  /* 从路径中分离出最后一级文件名或者目录(文件夹)名,返回最后一级文件名或者目录名的字节偏移 */
{
	PUINT8	pName;
	for ( pName = path; *pName != 0; ++ pName );  /* 到文件名字符串结束位置 */
	while ( *pName != DEF_SEPAR_CHAR1 && *pName != DEF_SEPAR_CHAR2 && pName != path ) pName --;  /*  搜索倒数第一个路径分隔符 */
	if ( pName != path ) pName ++;  /* 找到了路径分隔符,则修改指向目标文件的最后一级文件名,跳过前面的多级目录名及路径分隔符 */
	return( pName - path );
}

UINT8	CH376FileCreatePath( PUINT8 PathName )  /* 新建多级目录下的文件,支持多级目录路径,支持路径分隔符,路径长度不超过255个字符 */
{
	UINT8	s;
	UINT8	Name;
	Name = CH376SeparatePath( PathName );  /* 从路径中分离出最后一级文件名,返回最后一级文件名的偏移 */
	if ( Name ) {  /* 是多级目录 */
		s = CH376FileOpenDir( PathName, Name );  /* 打开多级目录下的最后一级目录,即打开新建文件的上级目录 */
		if ( s != ERR_OPEN_DIR ) {  /* 因为是打开上级目录,所以,如果不是成功打开了目录,那么说明有问题 */
			if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );  /* 中间路径必须是目录名,如果是文件名则出错 */
			else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );  /* 中间路径的某个子目录没有找到,可能是目录名称错误 */
			else return( s );  /* 操作出错 */
		}
	}
	return( CH376FileCreate( &PathName[Name] ) );  /* 在根目录或者当前目录下新建文件 */
}

UINT8	CH376SecLocate( UINT32 offset )  /* 以扇区为单位移动当前文件指针 */
{
	xWriteCH376Cmd( CMD4H_SEC_LOCATE );
	xWriteCH376Data( (UINT8)offset );
	xWriteCH376Data( (UINT8)((UINT16)offset>>8) );
	xWriteCH376Data( (UINT8)(offset>>16) );
	xWriteCH376Data( 0 );  /* 超出最大文件尺寸 */
	return( Wait376Interrupt( ) );
}

UINT8	CH376LocateInUpDir( PUINT8 PathName )  /* 在上级目录(文件夹)中移动文件指针到当前文件目录信息所在的扇区 */
/* 另外,顺便将当前文件目录信息所在的扇区的前一个扇区的LBA地址写入CH376内部VAR_FAT_DIR_LBA变量(为了方便收集长文件名时向前搜索,否则要多移动一次) */
/* 使用了全局缓冲区GlobalBuf的前12个字节 */
{
	UINT8	s;
	xWriteCH376Cmd( CMD14_READ_VAR32 );
	xWriteCH376Data( VAR_FAT_DIR_LBA );  /* 当前文件目录信息所在的扇区LBA地址 */
	for ( s = 4; s != 8; s ++ ) GlobalBuf[ s ] = xReadCH376Data( );  /* 临时保存于全局缓冲区中,节约RAM */
	s = CH376SeparatePath( PathName );  /* 从路径中分离出最后一级文件名或者目录名,返回最后一级文件名或者目录名的偏移 */
	if ( s ) s = CH376FileOpenDir( PathName, s );  /* 是多级目录,打开多级目录下的最后一级目录,即打开文件的上级目录 */
	else s = CH376FileOpen( "/" );  /* 根目录下的文件,则打开根目录 */
	if ( s != ERR_OPEN_DIR ) return( s );
	*(PUINT32)(&GlobalBuf[0]) = 0;  /* 目录扇区偏移扇区数,保存在全局缓冲区中,节约RAM */
	while ( 1 ) {  /* 不断移动文件指针,直到与当前文件目录信息所在的扇区LBA地址匹配 */
		s = CH376SecLocate( *(PUINT32)(&GlobalBuf[0]) );  /* 以扇区为单位在上级目录中移动文件指针 */
		if ( s != USB_INT_SUCCESS ) return( s );
		CH376ReadBlock( &GlobalBuf[8] );  /* 从内存缓冲区读取CH376_CMD_DATA.SectorLocate.mSectorLba数据块,返回长度总是sizeof(CH376_CMD_DATA.SectorLocate) */
		if ( *(PUINT32)(&GlobalBuf[8]) == *(PUINT32)(&GlobalBuf[4]) ) return( USB_INT_SUCCESS );  /* 已到当前文件目录信息扇区 */
		xWriteCH376Cmd( CMD50_WRITE_VAR32 );
		xWriteCH376Data( VAR_FAT_DIR_LBA );  /* 得到前一个扇区,设置为新的文件目录信息扇区LBA地址 */
		for ( s = 8; s != 12; s ++ ) xWriteCH376Data( GlobalBuf[ s ] );
		++ *(PUINT32)(&GlobalBuf[0]);
	}
}

UINT8	CH376ByteLocate( UINT32 offset )  /* 以字节为单位移动当前文件指针 */
{
	xWriteCH376Cmd( CMD4H_BYTE_LOCATE );
	xWriteCH376Data( (UINT8)offset );
	xWriteCH376Data( (UINT8)((UINT16)offset>>8) );
	xWriteCH376Data( (UINT8)(offset>>16) );
	xWriteCH376Data( (UINT8)(offset>>24) );
	return( Wait376Interrupt( ) );
}

UINT8	CH376CheckNameSum( PUINT8 DirName )  /* 计算长文件名的短文件名检验和,输入为无小数点分隔符的固定11字节格式 */
{
	UINT8	NameLen;
	UINT8	CheckSum;
	CheckSum = 0;
	for ( NameLen = 0; NameLen != 11; NameLen ++ ) CheckSum = ( CheckSum & 1 ? 0x80 : 0x00 ) + ( CheckSum >> 1 ) + *DirName++;
	return( CheckSum );
}

UINT8	CH376LongNameWrite( PUINT8 buf, UINT16 ReqCount )  /* 长文件名专用的字节写子程序 */
{
	UINT8	s;
	UINT8	c;
	c = CH376ReadVar8( VAR_DISK_STATUS );
	if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, DEF_DISK_OPEN_DIR );

	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );

	while ( 1 ) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_WRITE ) {
			if ( buf ) buf += CH376WriteReqBlock( buf );  /* 向内部指定缓冲区写入请求的数据块,返回长度 */
			else {
				xWriteCH376Cmd( CMD01_WR_REQ_DATA );  /* 向内部指定缓冲区写入请求的数据块 */
				s = xReadCH376Data( );  /* 长度 */
				while ( s -- ) xWriteCH376Data( 0 );  /* 填充0 */
			}
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
		}
/*		else if ( s == USB_INT_SUCCESS ) return( s );*/  /* 结束 */
		else {
			if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, c );
			return( s );  /* 错误 */
		}
	}
}
UINT8	CH376CreateLongName( PUINT8 PathName, PUINT8 LongName )  /* 新建具有长文件名的文件,关闭文件后返回,LongName输入路径必须在RAM中 */
/* 需要输入短文件名的完整路径PathName(请事先参考FAT规范由长文件名自行产生),需要输入以UNICODE小端编码的以双0结束的长文件名LongName */
/* 使用了全局缓冲区GlobalBuf的前64个字节,sizeof(GlobalBuf)>=sizeof(FAT_DIR_INFO)*2 */
{
	UINT8	s,i;
	UINT8	DirBlockCnt;	/* 长文件名占用文件目录结构的个数 */
	UINT16	count;			/* 临时变量,用于计数,用于字节读文件方式下实际读取的字节数 */
	UINT16	NameCount;		/* 长文件名字节计数 */
	UINT32	NewFileLoc;		/* 当前文件目录信息在上级目录中的起始位置,偏移地址 */
	for ( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) if ( *(PUINT16)(&LongName[count]) == 0 ) break;  /* 到结束位置 */
	if ( count == 0 || count >= LONG_NAME_BUF_LEN || count > LONE_NAME_MAX_CHAR ) return( ERR_LONG_NAME_ERR );  /* 长文件名无效 */
	DirBlockCnt = count / LONG_NAME_PER_DIR;  /* 长文件名占用文件目录结构的个数 */
	i = count - DirBlockCnt * LONG_NAME_PER_DIR;
	if ( i ) {  /* 有零头 */
		if ( ++ DirBlockCnt * LONG_NAME_PER_DIR > LONG_NAME_BUF_LEN ) return( ERR_LONG_BUF_OVER );  /* 缓冲区溢出 */
		count += 2;  /* 加上0结束符后的长度 */
		i += 2;
		if ( i < LONG_NAME_PER_DIR ) {  /* 最末的文件目录结构不满 */
			while ( i++ < LONG_NAME_PER_DIR ) LongName[count++] = 0xFF;  /* 把剩余数据填为0xFF */
		}
	}
    s = CH376FileOpenPath( PathName );  /* 打开多级目录下的文件 */
	if ( s == USB_INT_SUCCESS ) {   /* 短文件名存在则返回错误 */
		s = ERR_NAME_EXIST;
		goto CH376CreateLongNameE;
	}
	if ( s != ERR_MISS_FILE ) return( s );
	s = CH376FileCreatePath( PathName );  /* 新建多级目录下的文件 */
	if ( s != USB_INT_SUCCESS ) return( s );
	i = CH376ReadVar8( VAR_FILE_DIR_INDEX );  /* 临时用于保存当前文件目录信息在扇区内的索引号 */
	s = CH376LocateInUpDir( PathName );  /* 在上级目录中移动文件指针到当前文件目录信息所在的扇区 */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;  /* 没有直接返回是因为如果是打开了根目录那么必须要关闭后才能返回 */
	NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET ) + i * sizeof(FAT_DIR_INFO);  /* 计算当前文件目录信息在上级目录中的起始位置,偏移地址 */
	s = CH376ByteLocate( NewFileLoc );  /* 在上级目录中移动文件指针到当前文件目录信息的位置 */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	s = CH376ByteRead( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO), NULL );  /* 以字节为单位读取数据,获得当前文件的目录信息FAT_DIR_INFO */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	for ( i = DirBlockCnt; i != 0; -- i ) {  /* 搜索空闲的文件目录结构用于存放长文件名 */
		s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count );  /* 以字节为单位读取数据,获得下一个文件目录信息FAT_DIR_INFO */
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
		if ( count == 0 ) break;  /* 无法读出数据,上级目录结束了 */
		if ( GlobalBuf[0] && GlobalBuf[0] != 0xE5 ) {  /* 后面有正在使用的文件目录结构,由于长文件名必须连接存放,所以空间不够,必须放弃当前位置并向后转移 */
			s = CH376ByteLocate( NewFileLoc );  /* 在上级目录中移动文件指针到当前文件目录信息的位置 */
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			GlobalBuf[ 0 ] = 0xE5;  /* 文件删除标志 */
			for ( s = 1; s != sizeof(FAT_DIR_INFO); s ++ ) GlobalBuf[ s ] = GlobalBuf[ sizeof(FAT_DIR_INFO) + s ];
			s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );  /* 写入一个文件目录结构,用于删除之前新建的文件,实际上稍后会将之转移到目录的最末位置 */
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			do {  /* 向后搜索空闲的文件目录结构 */
				s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count );  /* 以字节为单位读取数据,获得下一个文件目录信息FAT_DIR_INFO */
				if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			} while ( count && GlobalBuf[0] );  /* 如果仍然是正在使用的文件目录结构则继续向后搜索,直到上级目录结束或者有尚未使用过的文件目录结构 */
			NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET );  /* 用上级目录的当前文件指针作为当前文件目录信息在上级目录中的起始位置 */
			i = DirBlockCnt + 1;  /* 需要的空闲的文件目录结构的个数,包括短文件名本身一个和长文件名 */
			if ( count == 0 ) break;  /* 无法读出数据,上级目录结束了 */
			NewFileLoc -= sizeof(FAT_DIR_INFO);  /* 倒回到刚才搜索到的空闲的文件目录结构的起始位置 */
		}
	}
	if ( i ) {  /* 空闲的文件目录结构不足以存放长文件名,原因是上级目录结束了,下面增加上级目录的长度 */
		s = CH376ReadVar8( VAR_SEC_PER_CLUS );  /* 每簇扇区数 */
		if ( s == 128 ) {  /* FAT12/FAT16的根目录,容量是固定的,无法增加文件目录结构 */
			s = ERR_FDT_OVER;  /* FAT12/FAT16根目录下的文件数应该少于512个,需要磁盘整理 */
			goto CH376CreateLongNameE;
		}
		count = s * DEF_SECTOR_SIZE;  /* 每簇字节数 */
		if ( count < i * sizeof(FAT_DIR_INFO) ) count <<= 1;  /* 一簇不够则增加一簇,这种情况只会发生于每簇为512字节的情况下 */
		s = CH376LongNameWrite( NULL, count );  /* 以字节为单位向当前位置写入全0数据块,清空新增加的文件目录簇 */
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376ByteLocate( NewFileLoc );  /* 在上级目录中移动文件指针到当前文件目录信息的位置 */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	GlobalBuf[11] = ATTR_LONG_NAME;
	GlobalBuf[12] = 0x00;
	GlobalBuf[13] = CH376CheckNameSum( &GlobalBuf[ sizeof(FAT_DIR_INFO) ] );  /* 计算长文件名的短文件名检验和 */
	GlobalBuf[26] = 0x00;
	GlobalBuf[27] = 0x00;
	for ( s = 0; DirBlockCnt != 0; ) {  /* 长文件名占用的文件目录结构计数 */
		GlobalBuf[0] = s ? DirBlockCnt : DirBlockCnt | 0x40;  /* 首次要置长文件名入口标志 */
		DirBlockCnt --;
		NameCount = DirBlockCnt * LONG_NAME_PER_DIR;
		for ( s = 1; s < sizeof( FAT_DIR_INFO ); s += 2 ) {  /* 输出长文件名,长文件名的字符在磁盘上UNICODE用小端方式存放 */
			if ( s == 1 + 5 * 2 ) s = 14;  /* 从长文件名的第一组1-5个字符跳到第二组6-11个字符 */
			else if ( s == 14 + 6 * 2 ) s = 28;  /* 从长文件名的第二组6-11个字符跳到第三组12-13个字符 */
			GlobalBuf[ s ] = LongName[ NameCount++ ];
			GlobalBuf[ s + 1 ] = LongName[ NameCount++ ];
		}
		s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );  /* 以字节为单位写入一个文件目录结构,长文件名 */
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376LongNameWrite( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO) );  /* 以字节为单位写入一个文件目录结构,这是转移来的之前新建的文件的目录信息 */
    CH376CreateLongNameE:
	//CH376FileClose( FALSE );  /* 对于根目录则必须要关闭 */
	return( s );
}

UINT8	CH376DirInfoRead( void )  /* 读取当前文件的目录信息 */
{
	return( CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, 0xFF ) );
}

void	CH376WriteOfsBlock( PUINT8 buf, UINT8 ofs, UINT8 len )  /* 向内部缓冲区指定偏移地址写入数据块 */
{
	xWriteCH376Cmd( CMD20_WR_OFS_DATA );
	xWriteCH376Data( ofs );  /* 偏移地址 */
	xWriteCH376Data( len );  /* 长度 */
	if ( len ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- len );
	}
}

UINT8	CH376DirInfoSave( void )  /* 保存文件的目录信息 */
{
	return( CH376SendCmdWaitInt( CMD0H_DIR_INFO_SAVE ) );
}

/* 为指定文件设置创建日期和时间 */
/* 输入参数:   原始文件名在iFileName中, 新的创建日期和时间: iCreateDate, iCreateTime */
/* 返回状态码: USB_INT_SUCCESS = 设置成功,
               其它状态码参考CH376INC.H */
UINT8 scct = 0;
UINT8	SetFileCreateTime( PUINT8 iFileName, UINT16 iCreateDate, UINT16 iCreateTime )
{
	UINT8 s;
    UINT8 buff[5];
	s = CH376FileOpen( iFileName );  /* 打开文件 */
	if ( s == USB_INT_SUCCESS ) {
		/* 文件读写操作等... */
		s = CH376DirInfoRead( );  /* 读取当前文件的目录信息FAT_DIR_INFO,将相关数据调到内存中 */
		if ( s == USB_INT_SUCCESS ) {
			buff[0] = (UINT8)iCreateTime;  /* 文件创建的时间,低8位在前 */
			buff[1] = (UINT8)(iCreateTime>>8);  /* 文件创建的时间,高8位在后 */
			CH376WriteOfsBlock( buff, STRUCT_OFFSET( FAT_DIR_INFO, DIR_CrtTime ), 2 );  /* 向内部缓冲区偏移地址DIR_CrtTime写入2个字节 */
            buff[0] = (UINT8)iCreateDate;  /* 文件创建的日期,低8位在前 */
			buff[1] = (UINT8)(iCreateDate>>8);  /* 文件创建的日期,高8位在后 */
			CH376WriteOfsBlock( buff, STRUCT_OFFSET( FAT_DIR_INFO, DIR_CrtDate ), 2 );  /* 向内部缓冲区偏移地址DIR_CrtDate写入2个字节 */
            s = CH376DirInfoSave( );  /* 保存文件的目录信息 */
			if ( s == USB_INT_SUCCESS ) {  /* 成功修改并保存 */
				/* 文件读写操作等... */
			}
		}
		CH376FileClose( FALSE );  /* FALSE关闭文件,不更新文件长度 */
	}
	return( s );
}
