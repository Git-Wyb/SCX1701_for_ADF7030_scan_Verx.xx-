#include "IIC.h"

#define iic_delay mDelayuS(1)

void IIC_INIT(void)
{
    PIN_PCF8563_SDA_direc = Output;
    PIN_PCF8563_SDA_CR1 = 0;
    PIN_PCF8563_SDAOUT = 1;

    PIN_PCF8563_SCL_direc = Output;
    PIN_PCF8563_SCL_CR1 = 0;
    PIN_PCF8563_SCL = 1;
}

void IIC_PCF8563_INIT(void)
{
    Init8563();
}

void Start(void)
 {
      PIN_PCF8563_SDAOUT=1;
      PIN_PCF8563_SCL=1;
      iic_delay;
      PIN_PCF8563_SDAOUT=0;
      iic_delay;
 }

void Stop(void)
 {
    PIN_PCF8563_SDAOUT=0;
    PIN_PCF8563_SCL=1;
    iic_delay;
    PIN_PCF8563_SDAOUT=1;
    iic_delay;
}

void Reack(void)
 {
    uchar i=0;
    PIN_PCF8563_SDA_direc = Input;
    iic_delay;
    PIN_PCF8563_SCL=1; //准备检测SDA Delay5us();
    while((PIN_PCF8563_SDAIN==1)&&(i<100))
    {
      i++;//SDA=0为应该信号,SDA=1为非应答</P>
    }
    PIN_PCF8563_SDA_direc = Output;
    iic_delay;
    PIN_PCF8563_SCL=0; //准备下一变化数据</P>
 }

void WriteByte(uchar ucByte)
 {
    uchar i;
    PIN_PCF8563_SCL=0;
    for(i=0;i<8;i++)
    {
      if(ucByte&0x80)//先写入高位</P>
      {
        PIN_PCF8563_SDAOUT=1;
      }
      else
      {
         PIN_PCF8563_SDAOUT=0;
      }
      PIN_PCF8563_SCL=1;
      iic_delay;
      PIN_PCF8563_SCL=0;
      ucByte<<=1;
    }
    PIN_PCF8563_SDAOUT=1; //释放数据线</P>
 }

uchar ReadByte(void)
 {
    uchar i,ucByte=0;

     PIN_PCF8563_SDA_direc = Input;
    iic_delay;

    PIN_PCF8563_SCL=0;
    for(i=0;i<8;i++)
    {
        ucByte<<=1;
       if(PIN_PCF8563_SDAIN)
          ucByte++;
       PIN_PCF8563_SCL=1;
       iic_delay;
      PIN_PCF8563_SCL=0;
    }

     PIN_PCF8563_SDA_direc = Output;
    iic_delay;

    return ucByte;
 }

void Write8563(uchar ucAddr,uchar ucData)
 {
       iic_delay;
      Start();
      WriteByte(0xa2);
      Reack();
      WriteByte(ucAddr);
      Reack();
      WriteByte(ucData);
      Reack();
      Stop();
}

uchar Read8563(uchar ucAddr)
 {
      uchar ucData;
       iic_delay;
      Start();
      WriteByte(0xa2); //写器件地址</P>
      Reack();
      WriteByte(ucAddr); //写字节地址</P>
      Reack();
      Start();
      WriteByte(0xa3); //写器件地址,最低为1表示读</P>
      Reack();
      ucData=ReadByte(); //写字节地址</P>
      Stop();
      return ucData; //读数据</P>
}

void Rewrite8563(void)
 {
     uchar now_time[7] = {00,39,16,19,2,8,25};
   /*秒、  分  、时、日期、星期、月、年 */
      uchar i,ucAddr=0x02;

      ClearWDT(); // Service the WDT
     // Write8563(0x00,0x20);
      FLAG_RTC_RTC = 1;
      //Write8563(0x00,0x20);//停时钟
     // Write8563(0x01,0x02);
      Write8563(0x01,0x02);
      for(i=0;i<7;i++)
      {
        Write8563(ucAddr,DecimalToBcd(now_time[i]));
        ucAddr++;
      }
      Write8563(0x00,0x00);
      FLAG_RTC_RTC = 0;
 }


void Init8563(void)
 {
   /*秒、  分  、时、日期、星期、月、年 */
    unsigned char c8563_Store[7]={0,0,0,1,1,1,1}; /*写入时间初值：01年1月1日，星期一 00:00:00*/
    IIC_INIT();
    uchar i,ucAddr=0x02,Tp_data;
    FLAG_RTC_RTC = 1;//这个不重入
    Tp_data = Read8563(1);
    if((Tp_data&0x02)==0)//这样表示PCF8563没有被设置
    {
        ClearWDT(); // Service the WDT
        //ClearWDT(); // Service the WDT
        // Write8563(0x00,0x20);
        Write8563(0x01,0x02);

        for(i=0;i<7;i++)
        {
        Write8563(ucAddr,DecimalToBcd(c8563_Store[i]));
        ucAddr++;
        }
        FLAG_RTC_RTC = 0;
        GetTime();
        FLAG_RTC_RTC = 1;
        ClearWDT(); // Service the WDT
        Write8563(0x00,0x00);
    }
    FLAG_RTC_RTC = 0;
 }

void GetTime(void)
 {
      uchar i,ucAddr=0x02;
      uchar *pTime=g8563_Store;

      ClearWDT(); // Service the WDT


      if(FLAG_RTC_RTC ==1)
      {
         return;
      }

      for(i=0;i<7;i++)
      {
          pTime[i]=Read8563(ucAddr);
          ucAddr++;
      }
      pTime[0]&=0x7f; //Seconds
      pTime[1]&=0x7f; //Minutes
      pTime[2]&=0x3f; //Hours
      pTime[3]&=0x3f; //Days
      pTime[4]&=0x07; //Weeks
      pTime[5]&=0x1f; //Months
      FLAG_RTC_RTC = 0;
      FLAG_CLOCK =1;
}

UINT8 decimal(UINT8 x_Data)
{
   UINT8 Tp_data;
   Tp_data = x_Data%16 + (x_Data/16)*10;
   return Tp_data;
}

UINT8 BcdToDecimal(UINT8 dec)
{
    return (dec - (dec>>4) * 6);
}

UINT8 DecimalToBcd(UINT8 bcd)
{
    return (bcd + (bcd/10) * 6);
}

void PCF8563_CLKOUT_ON(void)
{
    Init8563();
    Write8563(0x0d,0x80);
}

void PCF8563_CLKOUT_OFF(void)
{
    Init8563();
    Write8563(0x0d,0x00);
}

void GetReal_Time(void)
{
    GetTime();
}

void SetReal_Time(UINT8 yy,UINT8 mm,UINT8 dd,UINT8 hh,UINT8 mi,UINT8 ss,UINT8 ww)//年月日时分秒周
{
    uchar i,ucAddr = 0x02;
    /*秒、  分  、时、日期、星期、月、年 */
    uchar now_time[7] = {ss,mi,hh,dd,ww,mm,yy};

    ClearWDT(); // Service the WDT
    FLAG_RTC_RTC = 1;
    Write8563(0x01,0x02);
    for(i=0; i<7; i++)
    {
        ClearWDT();
        Write8563(ucAddr,DecimalToBcd(now_time[i]));
        ucAddr++;
    }
    Write8563(0x00,0x00);
    FLAG_RTC_RTC = 0;
}


