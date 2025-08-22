#ifndef __CH376FILENAME_H__
#define __CH376FILENAME_H__

#include "CH376.h"

#define	STRUCT_OFFSET( s, m )	( (UINT8)( & ((s *)0) -> m ) )	/* 定义获取结构成员相对偏移地址的宏 */

#define	LONG_NAME_BUF_LEN	( LONG_NAME_PER_DIR * 10 )	/* 自行定义的长文件名缓冲区长度,最小值为LONG_NAME_PER_DIR*1 */
UINT8	CH376FileOpenDir( PUINT8 PathName, UINT8 StopName );
UINT8	CH376FileOpenPath( PUINT8 PathName );
UINT8	CH376SeparatePath( PUINT8 path );
UINT8	CH376FileCreatePath( PUINT8 PathName );
UINT8	CH376SecLocate( UINT32 offset );
UINT8	CH376ReadBlock( PUINT8 buf );
UINT8	CH376LocateInUpDir( PUINT8 PathName );
UINT8	CH376ByteLocate( UINT32 offset );
UINT8	CH376CheckNameSum( PUINT8 DirName );
UINT8	CH376LongNameWrite( PUINT8 buf, UINT16 ReqCount );
UINT8	CH376CreateLongName( PUINT8 PathName, PUINT8 LongName );
UINT8   CH376_CreateFile_Name(PUINT8 PathName, PUINT8 LongName);
UINT8	CH376DirInfoRead( void );
UINT8	CH376DirInfoSave( void );
UINT8	SetFileCreateTime( PUINT8 iFileName, UINT16 iCreateDate, UINT16 iCreateTime );
void	CH376WriteOfsBlock( PUINT8 buf, UINT8 ofs, UINT8 len );

#endif
