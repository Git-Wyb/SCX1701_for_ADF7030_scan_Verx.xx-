#ifndef __CH376_H__
#define __CH376_H__

#include <string.h>
#include <stdio.h>
#include "CH376INC.H"
#include "type_def.h"
#include "uart.h"
#include "ram.h"
#include "Pin_define.h"
#include <iostm8l151c8.h>
#include "Timer.h"

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
UINT8 CH376_Byte_Write(PUINT8 buf, UINT16 ReqCount);
void CH376WriteVar32( UINT8 var, UINT32 dat );
UINT8 CH376ByteRead( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount );
UINT8 CH376ReadBlock( PUINT8 buf );
void CH376WriteVar8( UINT8 var, UINT8 dat );
UINT8 CH376ReadVar8( UINT8 var );
UINT32	CH376ReadVar32( UINT8 var );
UINT8	CH376SendCmdDatWaitInt( UINT8 mCmd, UINT8 mDat );

#endif
