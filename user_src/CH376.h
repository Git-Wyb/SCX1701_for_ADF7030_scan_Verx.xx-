#ifndef __CH376_H__
#define __CH376_H__

typedef unsigned long               *PUINT32;
typedef unsigned char               *PUINT8;
typedef unsigned int                *PUINT16;

UINT8 CH376_HOST_INIT(void);
void xWriteCH376Cmd(UINT8 mCmd);
void xWriteCH376Data(UINT8 mData);
UINT8 xReadCH376Data(void);
UINT8 CH376GetIntStatus( void );
UINT8 CH376DiskConnect(void);
UINT8 CH376SendCmdWaitInt(UINT8 mCmd);
UINT8 CH376DiskMount(void);
UINT8 CH376DiskQuery(PUINT32 DiskFre);
UINT32 CH376Read32bitDat( void );
UINT8 CH376FileCreate( PUINT8 name );
void CH376SetFileName( PUINT8 name );
UINT8 CH376DirCreate( PUINT8 name );
UINT8 CH376FileClose( UINT8 UpdateSz );
UINT8 Wait376Interrupt(void);
UINT8 CH376WriteReqBlock( PUINT8 buf );
UINT8 CH376ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount );
UINT8 CH376_USB_Del(void);
UINT8 Query376Interrupt( void );

#endif
