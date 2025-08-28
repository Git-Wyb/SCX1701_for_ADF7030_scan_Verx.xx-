#ifndef __IIC_H__
#define __IIC_H__

#include "type_def.h"
#include "Pin_define.h"
#include "Timer.h"
#include <iostm8l151c8.h>
#include "initial.h"
#include "ram.h"

#define  Now_Sec    BcdToDecimal(g8563_Store[0])
#define  Now_Min    BcdToDecimal(g8563_Store[1])
#define  Now_Hour   BcdToDecimal(g8563_Store[2])
#define  Now_Day    BcdToDecimal(g8563_Store[3])
#define  Now_Week   g8563_Store[4]
#define  Now_Mon    BcdToDecimal(g8563_Store[5])
#define  Now_Year   BcdToDecimal(g8563_Store[6])

void GetTime(void);
UINT8 decimal(UINT8 x_Data);
void Rewrite8563(void);
void Write8563(uchar ucAddr,uchar ucData);
void Init8563(void);
UINT8 DecimalToBcd(UINT8 dec);
UINT8 BcdToDecimal(UINT8 bcd);
void PCF8563_CLKOUT_ON(void);
void PCF8563_CLKOUT_OFF(void);
void GetReal_Time(void);
void IIC_INIT(void);
void SetReal_Time(UINT8 yy,UINT8 mm,UINT8 dd,UINT8 hh,UINT8 mi,UINT8 ss,UINT8 ww);
void IIC_PCF8563_INIT(void);

#endif
