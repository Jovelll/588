#define _DEBUG_C_
#include "types.h"
#include "Common.h"
#include "debugDef.h"
#include "global.h"
#include "board.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
#include "msosd.h"
#include "NVRam.h"
#include "i2c.h"
#include "misc.h"
#include "adjust.h"
//#include "msADC.h"
#include "drvadc.h"
#include "userPref.h"
#include "autofunc.h"
#include "menudef.h"
#include "menufunc.h"
#include "msace.h"
#include "msdlc.h"
#include "msflash.h"
#include "keypaddef.h"
#include "power.h"
#include "debug.h"
#include "Common.h"
#include "MsFlash.h"
#include "gpio_def.h"
#include "AutoGamma.h"
#include "msACE.h"
#if Enable_Gamma
#include "gamma.h"
#endif
#include "COLOR_VERIFY.h"
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
#if ENABLE_DSC
#include "msDSC_Setting.h"
#endif
#include "keypad.h"
#if MS_DAC
#include "drvDAC.h"
#endif

 #if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif

#if !USEFLASH || (defined(UseVGACableReadWriteAllPortsEDID)&&!defined(UseInternalDDCRam))
extern void i2c_WriteTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count );
extern void i2c_ReadTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count );
#endif

#if ENABLE_DEBUG
#message "Please remember to disable the debug function!"//Zhifeng.Wu Please don't canel it,just only a message,thanks.
#endif

#ifndef DEBUG_SELFTEST // 120620 coding addition for test EEPROM address/LVDS SSC
#define DEBUG_SELFTEST  (0&&ENABLE_DEBUG)
#endif

#ifndef OSD_TO_IP_DEBUG
#define OSD_TO_IP_DEBUG	(0&&ENABLE_DEBUG)
#endif

//BYTE xdata SIORxIndex = 0;
//BYTE xdata ReSyncCounter;
extern Bool xdata bFlashWriteFactory;
#define DEBUG_Massage    1
#if ENABLE_DEBUG&&DEBUG_Massage
    #define DB_printData(str, value)   printData(str, value)
    #define DB_printMsg(str)           printMsg(str)
#else
    #define DB_printData(str, value)
    #define DB_printMsg(str)
#endif

#if ENABLE_DEBUG
#if ENABLE_TOUCH_PANEL || ENABLE_TOUCH_PANEL_CTRL_OSD
extern Bool TPL_ReadSurface(void);
#endif
#if ENABLE_TOUCH_PANEL_DATA_FROM_USB
extern Bool USB_SPI_WriteRegister(BYTE addr, BYTE val);
extern Bool USB_SPI_ReadRegister(BYTE addr, BYTE *val, BYTE count);
#endif
#endif

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
bit bTimingChangeFirstPointFlag;
BYTE xdata TimingChangeFirstPoint; // record timing change first point
#endif

void putSIOChar( unsigned char sendData )
{
   if(!UART_READ_ES())
        return;

    UART_BUFFER() = sendData;
    while( 1 )
    {
        if( EA && UART_READ_ES() )
        {
            if( TI0_FLAG )
            {
                TI0_FLAG = 0;
                break;
            }
        }
        else
        {
            if(UART_TI_FLAG())
            {
                UART_CLR_TI_FLAG();
                break;
            }
        }
    }
}

#if DEBUG_SELFTEST
#define FailAddr(Addr)     printData(#Addr" Fail[0x%x]", Addr)
#define READ_LVDS_SSC_STEP() (msRead2Byte(REG_382E))
#define READ_LVDS_SSC_SPAN() (msRead2Byte(REG_3830))
#define READ_LPLL_SET() (((DWORD)msReadByte(REG_3820)<<16)|msRead2Byte(REG_381E))
#define SSC_MODULATION()    ((float)429ul*1024ul*131072ul/READ_LVDS_SSC_SPAN()/READ_LPLL_SET())
#define SSC_PERCENTAGE()    ((float)READ_LVDS_SSC_STEP()*READ_LVDS_SSC_SPAN()*1000ul/READ_LPLL_SET())

void SelfTest(void)
{
    WORD wval;

    if(FactorySettingAddr <= ProductModeAddr)
        FailAddr(FactorySettingAddr);

    if(MonitorSettingAddr <= FactorySettingAddr)
        FailAddr(MonitorSettingAddr);

    if(ModeSettingAddr <= MonitorSettingAddr)
        FailAddr(ModeSettingAddr);

    if(ModeInfoAddr <= ModeSettingAddr)
        FailAddr(ModeInfoAddr);

    if(EndAddr <= ModeSettingAddr)
        FailAddr(EndAddr);

    if(BlacklitTimeAddr <= EndAddr)
        FailAddr(BlacklitTimeAddr);

    if(CheckBoardAddr <= BlacklitTimeAddr)
        FailAddr(CheckBoardAddr);
    DB_printData("===EEPROM FINISH===");

    wval = SSC_MODULATION();
    //if(wval > PANEL_SSC_MODULATION_DEF)
        printData("PANEL_SSC_MODULATION:%d", wval);

    wval = SSC_PERCENTAGE();
    //if(wval > PANEL_SSC_PERCENTAGE_DEF)
        printData("PANEL_SSC_PERCENTAGE:%d", wval);
    DB_printData("===LVDS SSC FINISH===");

}
#endif

#if ENABLE_DEBUG
#if (CHIP_ID==CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID==CHIP_TSUMV)||(CHIP_ID==CHIP_TSUMD)|| (CHIP_ID==CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF)//ring style line buffer
/*
Please enter the position of data you want to check
Ex:
Resolution:800x600

 (0,0)           (799,0)
    _____________
    |           |
    |           |
    |           |
    -------------
 (0,599)         (799,599)
H[11:0] = {ucPara1[7:0], ucPara2[7:4]}
V[11:0] = {ucPara2[3:0], ucPara3[7:0]}
*/
void msLineBufLatchData(BYTE ucPara1, BYTE ucPara2, BYTE ucPara3)
{
    WORD HPosition = ((ucPara1<<4)|(ucPara2>>4))&0x0FFF, VPosition = ((ucPara2<<8)|(ucPara3))&0x0FFF;
    WORD VStart = msRead2Byte(SC0_05);

    msWrite2ByteMask(SC0_05,(VStart+VPosition),0x7FF);
    msWrite2ByteMask(SC5_31,( HPosition / 3 ), 0x0FFF); // H pack address
    msWriteByteMask(SC5_33, ( HPosition % 3 ), 0x03); // H position
    msWriteByteMask(SC5_34, BIT7,BIT7);//1:read from start of frmae, 0:read from end of frame
    msWriteByteMask(SC5_32, BIT6, BIT6); // LB latch data function enable
    Delay1ms(100);
    DB_printData("LB R0:%d",msReadByte(SC5_35)); // LB Latch R0
    DB_printData("LB G0:%d",msReadByte(SC5_36)); // LB Latch G0
    DB_printData("LB B0:%d",msReadByte(SC5_37)); // LB Latch B0
    msWriteByteMask(SC5_32, 0, BIT6); // IP latch data function disable
    msWrite2ByteMask(SC0_05,VStart,0x7FF); //restore V start value
}
#else //TSUM2, TSUMU, TSUMB, TSUMY
    /* H={ucPara1,ucPara2}, V(LB)=ucPara3 */
void msLineBufLatchData(BYTE ucPara1, BYTE ucPara2, BYTE ucPara3)
{
    /* H={ucPara1,ucPara2}, V(LB)=ucPara3 */
    BYTE ucTest = 2;
    msWrite2ByteMask(SC5_31, (((WORD) ucPara1<<8 ) | ucPara2)/2, 0x0FFF); // H position in LB
#if (CHIP_ID==CHIP_TSUMU)
    msWriteByteMask(SC5_33, (ucPara3), 0x03); //  V position in LB
#else
    msWriteByteMask(SC5_33, (ucPara3<<3), 0x18); // [4:3] V position in LB
#endif
    msWriteByteMask(SC5_32, BIT6, BIT6); // MCU read LB function enable

#if (CHIP_ID==CHIP_TSUMU)
    DB_printData("Line Buffer Latch Data - R0:%d",msReadByte(SC5_35)); // LB Latch R0
    DB_printData("Line Buffer Latch Data - G0:%d",msReadByte(SC5_36)); // LB Latch G0
    DB_printData("Line Buffer Latch Data - B0:%d",msReadByte(SC5_37)); // LB Latch B0
    DB_printData("Line Buffer Latch Data - R1:%d",msReadByte(SC5_38)); // LB Latch R1
    DB_printData("Line Buffer Latch Data - G1:%d",msReadByte(SC5_39)); // LB Latch G1
    DB_printData("Line Buffer Latch Data - B1:%d\r\n",msReadByte(SC5_3A)); // LB Latch B1
#else
    DB_printMsg("LB Data:R-G-B");
    do
    {
        msWriteByteMask(SC5_33, ucTest, 0x07); // [2:0] byte select <B G R>
        while(!(msReadByte(SC5_32)&BIT7)); // LB read avaliable status
        DB_printData("%d",msReadByte(SC5_35));
    }while(ucTest--);

#endif
    msWriteByteMask(SC5_32, 0, BIT6); // MCU read LB function disable
    scWriteByte(SC0_00, 0x00);
}
#endif

/*
Please enter the position of data you want to check
Ex:
Resolution:800x600

 (0,0)           (799,0)
    _____________
    |           |
    |           |
    |           |
    -------------
 (0,599)         (799,599)

   H[11:0] = {ucPara1[7:0], ucPara2[7:4]}
   V[11:0] = {ucPara2[3:0], ucPara3[7:0]} */
void msIPLatchData(BYTE ucPara1, BYTE ucPara2, BYTE ucPara3)
{
    WORD HPosition = ((ucPara1<<4)|(ucPara2>>4))&0x0FFF, VPosition = ((ucPara2<<8)|(ucPara3))&0x0FFF;
    msWrite2ByteMask(SC5_31, (HPosition+(msRead2Byte(SC0_07)&0x7FF)-1), 0x0FFF); // Image H position
    msWrite2ByteMask(SC5_33, (VPosition+(msRead2Byte(SC0_05)&0x7FF)), 0x07FF); // Image V position
    msWriteByteMask(SC5_34, BIT5, BIT5); // IP latch data function enable
    Delay1ms(100);
    DB_printData("IP R:%d",msReadByte(SC5_35)); // IP Latch R
    DB_printData("IP G:%d",msReadByte(SC5_36)); // IP Latch G
    DB_printData("IP B:%d\r\n",msReadByte(SC5_37)); // IP Latch B
    msWriteByteMask(SC5_34, 0, BIT5); // IP latch data function disable
}
#if (CHIP_ID==CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID==CHIP_TSUMD)|| (CHIP_ID==CHIP_TSUMJ)|| (CHIP_ID==CHIP_TSUM9)|| (CHIP_ID==CHIP_TSUMF)
/*non-inear scaling curve
      W0   W1   W2   W2    W1    W0
         |    |____|____|_    |
         |   /|    |    | \   |
         |  / |    |    |  \  |
         | /  |    |    |   \ |
    _____|/   |    |    |    \|________
   Initial Offset = ucPara1[6:0]
   Delta1 = ucPara2[7:0]
*/
void msNonLinearScaling(BYTE ucPara1, BYTE ucPara2)
{
    WORD W0, W1, W2;

    if(!(msReadByte(SC0_52)&BIT7))
        msWrite3Byte(SC0_50, 0x900000);
    W1 = ((DWORD)ucPara1<<12)*2/(ucPara2<<3);
    if(W1>(PANEL_WIDTH/2))
    {
        DB_printMsg("Since the initial offset is too large or delta1 is too small");
        DB_printMsg("The non-linear scaling results may be wrong!");
        DB_printMsg("Please correct the settings and try again");
    }
    W0 = ((PANEL_WIDTH/2)-W1)/2;
    W2 = W0;
    msWrite2ByteMask(SCD_20, W0,0x3FF);//width of zone 0 in non-linear
    msWrite2ByteMask(SCD_22, W1,0x3FF);//width of zone 1 in non-linear
    msWrite2ByteMask(SCD_24, W2,0x3FF);//width of zone 2 in non-linear
    msWriteByteMask(SCD_26, ucPara1,0x7F);//value of initial offset
    msWrite2ByteMask(SCD_26, BIT7,BIT7);//0: positive initial offset, 1: negative initial offset"
    msWrite2ByteMask(SCD_27, BIT7, BIT7);//non-linear scaling enable
    msWriteByte(SCD_28, 0);//delta value in zone 0
    msWriteByte(SCD_29, ucPara2);//delta value in zone 1
}

/* test Internal EDID
CHIP_TSUMC: 256*2+128 K Sram

    _____________   0 K - Start
    |  Arranged | 128 K
    |     IN    | 256 K
    |  REG_3EEC | 384 K
    |  REG_3EED | 512 K
    ------------- 640 K - End

256*2: for DP (if DP not used, can be arranged for other ports)
128: for VGA or DVI or shared
*/
void TestInternalEDID_Write(BYTE SramIndex, BYTE StartIndex)
{
    BYTE i;

    msWriteByteMask( REG_3E4B , 0,BIT7);// Set DDC To Write
    msWriteByteMask( REG_3E49 , 0,BIT5);// Clr Write Protect
    msWriteByteMask( REG_3EEB , SramIndex<<4, 0xF0);// Sram Index

    for( i = 0; i < 128; i++ )
    {
        msWriteByte( REG_3E4B ,i );
        msWriteByte( REG_3E4C ,i+StartIndex );

        msWriteByteMask( REG_3E43 , BIT5,BIT5);
        msWriteByteMask( REG_3E43 , 0,BIT5);
    }
    msWriteByteMask( REG_3E4B , BIT7,BIT7);// Set DDC To Read
    msWriteByteMask( REG_3E49,  BIT5 | BIT7, BIT5 | BIT7 ); // [7]ADC DDC enable / [5]Set Write Protect
#if(CHIP_ID == CHIP_TSUMJ ||CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
    msWriteByteMask(REG_3E60, BIT7, BIT7);//reply ACK while source accesses A0_EDID with address is over 128
#endif
}

void TestInternalEDID_Read(BYTE SramIndex)
{
    BYTE i;

    msWriteByteMask( REG_3EEB , SramIndex<<4, 0xF0);// Sram Index

    for( i = 0; i < 128; i++ )
    {
        msWriteByte( REG_3E4B ,i );
        msWriteByteMask( REG_3E43 , BIT4,BIT4);
        msWriteByteMask( REG_3E43 , 0,BIT4);
        DB_printData("%d",msReadByte(REG_3E0E));
    }
}

#if CHIP_ID==CHIP_TSUMJ
//CHIP_TSUME: 256+128 K Sram
/*
    _____________   0 K - Start
    |  Arranged   | 128 K
    |     IN          | 256 K
    |---------- | 384 K - End
*/
void TestInternalEDID_256ByteWrite(BYTE SramIndex, BYTE StartIndex)
{
    WORD i;
    msWriteByteMask( REG_3EEB , SramIndex<<4, 0xF0);// Sram Index

    for( i = 0; i < 256; i++ )
    {
        msWriteByte( REG_3E47 ,i );
        msWriteByte( REG_3E48 ,i+StartIndex );

        msWriteByteMask( REG_3E43 , BIT7,BIT7);
        msWriteByteMask( REG_3E43 , 0,BIT7);
    }
}

void TestInternalEDID_256ByteRead(BYTE SramIndex)
{
    WORD i;
    msWriteByteMask( REG_3EEB , SramIndex<<4, 0xF0);// Sram Index
    for( i = 0; i < 256; i++ )
    {
        msWriteByte( REG_3E47 ,i );
        msWriteByteMask( REG_3E43 , BIT6,BIT6);
        msWriteByteMask( REG_3E43 , 0,BIT6);
        DB_printData("%d",msReadByte(REG_3E0F));
    }
}
#endif

#if ENABLE_RTE//for OD verification
void msReadWriteODTable(void)
{

    WORD wCount;

    DB_printMsg("Load_OD_Table");

    msWriteBit(REG_3120,0, _BIT0); // Disable OD before load code
    msWriteByte(REG_3102, 0x0E); // load code start
    msWriteByte(REG_311C, 0x0E); //od 2nd table sram enable, r,g,b channel
    msWriteByte(REG_311D, 0x00);

    for(wCount=0; wCount<289; wCount++)
    {
        if(wCount<272)
        {
            if(wCount>255)
            {
                 msWriteByte(REG_3106, wCount-255);
                 DB_printData("%x",wCount-255);
            }
            else
            {
                msWriteByte(REG_3106, wCount);//Od table sram1 write data
                DB_printData("%x",wCount);
            }
        }
        else
        {
            msWriteByte(REG_3106, 0x00);
            DB_printData("%x",0);
        }
        msWrite2Byte(REG_3104, wCount|0x8000);//Od table sram1 address
        while(_bit7_(msReadByte(REG_3105)));//Od table sram1 write enable (toggle type). , 0x3105[0]+ 0x3104[7:0] ==address [8:0]
    }
    for(wCount=0; wCount<272; wCount++)
    {
        if(wCount>255)
        {
            msWriteByte(REG_310C, wCount-255);
            DB_printData("%x",wCount-255);
        }
        else
        {
            msWriteByte(REG_310C, wCount);
            DB_printData("%x",wCount);
        }
        msWrite2Byte(REG_310A, wCount|0x8000);
        while(_bit7_(msReadByte(REG_310B)));

    }
    for(wCount=0; wCount<272; wCount++)
    {
        if(wCount<256)
        {
            msWriteByte(REG_3112, wCount);
            DB_printData("%x",wCount);
        }
        else
        {
            msWriteByte(REG_3112, 0x00);
            DB_printData("%x",0);
        }
        msWrite2Byte(REG_3110, wCount|0x8000);
        while(_bit7_(msReadByte(REG_3111)));
    }
    for(wCount=0; wCount<256; wCount++)
    {
        msWriteByte(REG_3118, wCount);
        DB_printData("%x",wCount);
        msWrite2Byte(REG_3116, wCount|0x8000);
        while(_bit7_(msReadByte(REG_3117)));
    }
    DB_printMsg("Read_OD_Table");
    for(wCount=0; wCount<289; wCount++)
    {
        msWrite2Byte(REG_3104, wCount|0x4000);//Od table sram1 address
        while(_bit6_(msReadByte(REG_3105)));//Od table sram1 read enable (toggle type). , 0x3105[0]+ 0x3104[7:0] ==address [8:0]
        DB_printData("%x",msReadByte(REG_3108));
    }
    for(wCount=0; wCount<272; wCount++)
    {
        msWrite2Byte(REG_310A, wCount|0x4000);
        while(_bit6_(msReadByte(REG_310B)));
        DB_printData("%x",msReadByte(REG_310E));
    }
    for(wCount=0; wCount<272; wCount++)
    {
        msWrite2Byte(REG_3110, wCount|0x4000);
        while(_bit6_(msReadByte(REG_3111)));
        DB_printData("%x",msReadByte(REG_3114));
    }
    for(wCount=0; wCount<256; wCount++)
    {
        msWrite2Byte(REG_3116, wCount|0x4000);
        while(_bit6_(msReadByte(REG_3117)));
        DB_printData("%x",msReadByte(REG_311A));
    }
    msWriteByte(REG_3102, 0x00);
}
#endif //ENABLE_RTE
#endif //(CHIP_ID==CHIP_TSUMC)
#if CHIP_ID == CHIP_TSUMU ||CHIP_ID == CHIP_TSUMC||CHIP_ID == CHIP_TSUMK||CHIP_ID == CHIP_TSUMD|| CHIP_ID==CHIP_TSUMJ || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF
/* ucPara1: 0:R, 1:G, 2:B */
void msCTMaxMin(BYTE ucPara1)
{
    msWriteByteMask(SC9_41, ucPara1<<5, _BIT6|_BIT5);
    DB_printData("Max CT: %d", msRead2Byte(SC9_4A)>>2);
    DB_printData("Min CT: %d", msRead2Byte(SC9_4C)>>2);
}

/*
Please enter the position of data you want to check
Ex:
Resolution:800x600
 (0,0)           (799,0)
    _____________
    |           |
    |           |
    |           |
    -------------
 (0,599)         (799,599)
   H[11:0] = {ucPara1[7:0], ucPara2[7:4]}
   V[11:0] = {ucPara2[3:0], ucPara3[7:0]} */
void msCT_CoordinatesSetting(BYTE ucPara1  ,BYTE ucPara2 ,BYTE ucPara3)
{
    BYTE uLength=10;
    WORD wi,wj , HPosition = ((ucPara1<<4)|(ucPara2>>4))&0x0FFF, VPosition = ((ucPara2<<8)|(ucPara3))&0x0FFF;
    //BYTE uHOffset=msRead2Byte(SC0_12)+PanelHSyncWidth;
    BYTE uHOffset=0x6D;//msRead2Byte(SC0_12)+PanelHSyncWidth;

    DB_printData(" CT square size: %d x %d", uLength);

    for( wi = VPosition; wi < (VPosition+uLength); wi = wi + 1 )
    {
    DB_printData(" \n\r CT V_ST_Position: %d", wi);
    msWrite2ByteMask(SC9_46, wi, 0x3FF);            //reg_ct_vst
    msWrite2ByteMask(SC9_48, wi+uLength, 0x3FF);    //reg_ct_vend

    for( wj = HPosition; wj < (HPosition+uLength); wj = wj + 1 )
    {
        msWrite2ByteMask(SC9_42, wj +uHOffset, 0x1FFF);             //reg_ct_hst
        msWrite2ByteMask(SC9_44, wj+uLength+uHOffset, 0x1FFF);      //reg_ct_hend
        DB_printData(" \n\r CT H_ST_Position: %d", wj);

        DB_printData(" \n\r Max CT: %d", msRead2Byte(SC9_4A)>>2);
        DB_printData(" \n\r Min CT: %d", msRead2Byte(SC9_4C)>>2);
    }
    }
}

#endif //  CHIP_ID == CHIP_TSUMU ||CHIP_ID == CHIP_TSUMC

#if OSD_TO_IP_DEBUG
static void TestCommand_OsdToIp(BYTE u8Path)
{
#if CHIP_ID>=CHIP_TSUMC
	switch (u8Path)
	{
		case 0x00:	// OP
			msWriteByteMask(OSD1_02, 0, BIT2);
			msWriteByteMask(OSD1_02, BIT5, BIT5|BIT4|BIT3);
			msWriteByteMask(OSD1_03, 0, BIT4);
			break;
		case 0x01:	// before IP
			msWriteByteMask(OSD1_02, BIT2, BIT2);				// osd to ip enable
			msWriteByteMask(OSD1_02, BIT4|BIT3, BIT5|BIT4|BIT3);// use idclk
			msWriteByteMask(OSD1_03, 0, BIT4);					// befor ip
			break;
		case 0x02:	// before HVSP
			msWriteByteMask(OSD1_02, BIT2, BIT2);				// osd to ip enable
			msWriteByteMask(OSD1_02, BIT4|BIT3, BIT5|BIT4|BIT3);// use idclk
			msWriteByteMask(OSD1_03, BIT4, BIT4);					// befor hvsp
			break;
		default:	// OP
			msWriteByteMask(OSD1_02, 0, BIT2);
			msWriteByteMask(OSD1_02, BIT5, BIT5|BIT4|BIT3);
			msWriteByteMask(OSD1_03, 0, BIT4);
	}
#else
#endif
}
#endif	// OSD_TO_IP_DEBUG

#endif 	// ENABLE_DEBUG

extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;
extern Bool ExecuteKeyEvent( MenuItemActionType menuAction );


extern void msDlcOnOff( BYTE bSwitch );
extern XDATA BYTE g_bDLCOnOff;
extern xdata BOOL  REG_DSC_RUN_Flag;
extern xdata BOOL  REG_DPS_RUN_Flag;

void ExecTestCommand( void )
{
    BYTE ucPara1 = SIORxBuffer[2];
    BYTE ucPara2 = SIORxBuffer[3];
    BYTE ucPara3 = SIORxBuffer[4];

    switch( SIORxBuffer[1] )
    {
#if DEBUG_MCU_GPIO
        BYTE i;
        //Open-drain:REG_1021/22/23/27/2A/2D[1:0] set to 2'b00 and REG_1029/2C/2E[1:0] set to 2'b11
        //Push-pull: REG_1021/22/23[1:0] need set to 2'b00 and REG_1027/2A/2D[1:0] need set to 2'b11
        //P10 and P11: GPIO34/35
        //P20 and P21: GPIO32/33
        //P30 and P31: GPIO40/41
        case 0x01:
            DB_printMsg("MCU Write P1_0=0");
            P1_0=0;
        break;
        case 0x02:
            DB_printMsg("MCU Write P1_0=1");
            P1_0=1;
        break;
        case 0x03:
            DB_printMsg("MCU Write P1_1=0");
            P1_1=0;
        break;
        case 0x04:
            DB_printMsg("MCU Write P1_1=1");
            P1_1=1;
        break;
        case 0x05:
            DB_printMsg("MCU Read P1_0 !!!!!");
            i=P1_0;
            DB_printData("-----P1_0=>%d",i);
        break;
        case 0x06:
            DB_printMsg("MCU Read P1_1 !!!!!");
            i=P1_1;
            DB_printData("-----P1_1=>%d",i);
        break;
        case 0x11:
            DB_printMsg("MCU Write P2_0=0");
            P2_0=0;
        break;
        case 0x12:
            DB_printMsg("MCU Write P2_0=1");
            P2_0=1;
        break;
        case 0x13:
            DB_printMsg("MCU Write P2_1=0");
            P2_1=0;
        break;
        case 0x14:
            DB_printMsg("MCU Write P2_1=1");
            P2_1=1;
        break;
        case 0x15:
            DB_printMsg("MCU Read P2_0 !!!!!");
            i=P2_0;
            DB_printData("-----P2_0=>%d",i);
        break;
        case 0x16:
            DB_printMsg("MCU Read P2_1 !!!!!");
            i=P2_1;
            DB_printData("-----P2_1=>%d",i);
        break;
        case 0x21:
            DB_printMsg("MCU Write P3_0=0");
            P3_0=0;
        break;
        case 0x22:
            DB_printMsg("MCU Write P3_0=1");
            P3_0=1;
        break;
        case 0x23:
            DB_printMsg("MCU Write P3_1=0");
            P3_1=0;
        break;
        case 0x24:
            DB_printMsg("MCU Write P3_1=1");
            P3_1=1;
        break;
        case 0x25:
            DB_printMsg("MCU Read P3_0 !!!!!");
            i=P3_0;
            DB_printData("-----P3_0=>%d",i);
        break;
        case 0x26:
            DB_printMsg("MCU Read P3_1 !!!!!");
            i=P3_1;
            DB_printData("-----P3_1=>%d",i);
        break;
#endif

#if ENABLE_R2_2DTO3D
    case 0x27:
        DB_printMsg("Turn on 2D to 3D");
        UserPref2Dto3D = 1;
        g_SetupPathInfo.b2DTo3DEnable = TRUE;
        mStar_SetupMode();
        Sc2DTo3DInit();
    break;
    case 0x28:
        DB_printMsg("Turn off 2D to 3D");
        UserPref2Dto3D = 0;
        g_SetupPathInfo.b2DTo3DEnable = FALSE;
        mStar_SetupMode();
        Sc2DTo3DOff();
    break;
    case 0x29:
        switch( ucPara1 )
        {
            case 0:
                DB_printMsg("Set Offset: Low2");
                Sc2DTo3DSetPIXOffset(0x80);
                Sc2DTo3DSetBLKOffset(0x20);
            break;
            case 1:
                DB_printMsg("Set Offset: Low");     //near
                Sc2DTo3DSetPIXOffset(0x80);
                Sc2DTo3DSetBLKOffset(0x40);
            break;
            case 2:
                DB_printMsg("Set Offset: Middle");  //middle
                Sc2DTo3DSetPIXOffset(0x80);
                Sc2DTo3DSetBLKOffset(0x80);
            break;
            case 3:
                DB_printMsg("Set Offset: High");    //far
                Sc2DTo3DSetPIXOffset(0x80);
                Sc2DTo3DSetBLKOffset(0xC0);
            break;
            case 4:
                DB_printMsg("Set Offset: High2");
                Sc2DTo3DSetPIXOffset(0x80);
                Sc2DTo3DSetBLKOffset(0xE0);
            break;
            default:
                DB_printMsg("More detaied pixelbased and blockbased gain seetings will provide.");
        }
    break;
    case 0x30:
         switch( ucPara1 )
        {
            case 0:
                DB_printMsg("Set Gain: Low2");
                Sc2DTo3DSetPIXGain(0x18);
                Sc2DTo3DSetBLKGain(0x04);
            break;
            case 1:
                DB_printMsg("Set Gain: Low");   //depth narrow
                Sc2DTo3DSetPIXGain(0x30);
                Sc2DTo3DSetBLKGain(0x08);
            break;
            case 2:
                DB_printMsg("Set Gain: Mid");   //depth middle
                Sc2DTo3DSetPIXGain(0x60);
                Sc2DTo3DSetBLKGain(0x10);
            break;
            case 3:
                DB_printMsg("Set Gain: High");  //depth deep
                Sc2DTo3DSetPIXGain(0x90);
                Sc2DTo3DSetBLKGain(0x18);
            break;
            case 4:
                DB_printMsg("Set Gain: High2");
                Sc2DTo3DSetPIXGain(0xC0);
                Sc2DTo3DSetBLKGain(0x20);
            break;
            default:
                DB_printMsg("More detaied pixelbased and blockbased ofset seetings will provide.");
        }
    break;
    case 0x31:
        if(g_SetupPathInfo.b2DTo3DEnable == TRUE)
        {
            if(msRead2Byte(REG_2102) != 0x1111)
            {
                msWrite2Byte(REG_2102, 0x1111);
                DB_printMsg("Enter 2D to 3D Game Mode");
            }
            else
            {
                msWrite2Byte(REG_2102, 0x0000);
                DB_printMsg("Exit 2D to 3D Game Mode");
            }
        }
        else
        {
            DB_printMsg("Please turn on 2D to 3D first");
        }
        return;
    case 0x32:
        DB_printMsg("Set Pixel-based Gain and Offset");
        Sc2DTo3DSetPIXGain(ucPara1);
        Sc2DTo3DSetPIXOffset(ucPara2);
    break;
    case 0x33:
        DB_printMsg("Set Block-based Gain and Offset");
        Sc2DTo3DSetBLKGain(ucPara1);
        Sc2DTo3DSetBLKOffset(ucPara2);
    break;
#endif

	case 0x34:
		DB_printData("msAdjustVideoSaturation=%d",ucPara2);
		msAdjustVideoSaturation(ucPara1,ucPara2);
		break;
	case 0x35:
		DB_printData("msACESetHDTVMode=%d",ucPara1);
		msACESetHDTVMode(ucPara1);
		break;

#if ENABLE_DEBUG
#if CHIP_ID == CHIP_TSUMD
/* 3D format convert
   	Input Format:ucPara1
    Output Format:ucPara2
    Data Path:ucPara3&0x0F
    3Dto2D:ucPara3&BIT7		*/
        case 0x3F:
            g_SetupPathInfo.bOverrideSCFmtIn = TRUE;
            g_SetupPathInfo.bOverrideSCPathMode = TRUE;
            g_SetupPathInfo.ucSCFmtIn = ucPara1;
            g_SetupPathInfo.ucSCFmtOut = ucPara2;
            g_SetupPathInfo.ucSCPathMode = ucPara3 & 0x0F;
            g_SetupPathInfo.b3DTo2DEnable = ucPara3 & BIT7?1:0;
            if(!g_SetupPathInfo.ucSCPathMode)
                g_SetupPathInfo.bOverrideSCPathMode = FALSE;
            Set_InputTimingChangeFlag();
        break;
#endif
        case 0x40:
            msLineBufLatchData(ucPara1, ucPara2, ucPara3);
        break;
        case 0x41:
            msIPLatchData(ucPara1, ucPara2, ucPara3);
        break;
#if CHIP_ID == CHIP_TSUMU ||CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD|| CHIP_ID==CHIP_TSUMJ|| CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF
        case 0x42:
            msCTMaxMin(ucPara1);
        break;
        case 0x43:
            msCT_CoordinatesSetting(ucPara1 ,ucPara2 ,ucPara3);
        break;
#endif
#if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD|| CHIP_ID==CHIP_TSUMJ|| CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF
        case 0x44:
            msNonLinearScaling(ucPara1, ucPara2);
        break;
#if ENABLE_RTE
        case 0x45:
            msReadWriteODTable();
        break;
#endif
        case 0x46:
            TestInternalEDID_Write(ucPara1,ucPara2);
            DB_printMsg( "Write Internal EDID" );
        break;
        case 0x47:
            TestInternalEDID_Read(ucPara1);
            DB_printMsg( "Read Internal EDID" );
        break;
#if CHIP_ID==CHIP_TSUMJ
    	case 0x48:
        	TestInternalEDID_256ByteWrite(ucPara1,ucPara2);
        	DB_printMsg( "Write Internal EDID" );
    	break;
    	case 0x49:
        	TestInternalEDID_256ByteRead(ucPara1);
        	DB_printMsg( "Read Internal EDID" );
    	break;
#endif
#endif // CHIP_ID==CHIP_TSUMC

#if ENABLE_DEBUG && COLORVERIFY_DEBUG
        case 0x4A:
            msColor_Test_Funct(ucPara1,ucPara2);
        break;
        case 0x4B:
            {
                extern void ms2DHPeakingTest(BYTE bScalerWin ,BYTE ucSharpness, BYTE ucStep );
                DB_printMsg("ms2DHPeakingTest");
                ms2DHPeakingTest(ucPara1,ucPara2,ucPara3);
            }
            break;
    case 0x4C:  //Noise Masking
    {
        extern void msNoiseMasking(BYTE bScalerWin ,BYTE u8Strength, BYTE ucStep );
        DB_printMsg("msNoiseMasking");
        msNoiseMasking(ucPara1,ucPara2,ucPara3);
    }
    break;

        case 0x4D:
            {
                extern void InitialPeaking(void);
                DB_printMsg("InitialPeaking");
                InitialPeaking();
            }
            break;
        case 0x4E:
            {
                extern void msAdjustSharpness(BYTE bScalerWin ,BYTE ucSharpness, BYTE ucStep );
                DB_printMsg("msAdjustSharpness");
                msAdjustSharpness(ucPara1,ucPara2,ucPara3);
            }
            break;
#endif

        case 0x50:
            if( DebugOnlyFlag )
            {
                Clr_DebugOnlyFlag();
                DB_printMsg( "<Free>" );
            }
            else
            {
                Set_DebugOnlyFlag();
                DB_printMsg( "<Debug>" );
            }
            break;
#if ENABLE_DPS
        case 0x51:
            REG_DPS_RUN_Flag=0;//msDPS_Off_Setting();
            DB_printMsg("...Disable DPS Function...");
            break;
        case 0x52:
            msDPS_On_Setting();
            DB_printMsg("...Enable DPS Function...");
            break;
#endif
        case 0x53:
            msDlcOnOff(1);
            DB_printMsg("...DLC ON...");
            break;
        case 0x54:
            //msDlcOnOff(0);
            //msWriteByteMask( SC7_B8, 0x00, BIT6|BIT7 );
            g_bDLCOnOff = 0;
            DB_printMsg("...DLC Off...");
            break;
        case 0x55:
            DB_printData("\r\n g_bDLCOnOff (%d)", g_bDLCOnOff);
            //DB_printData("\r\n REG_DSC_RUN_Flag (%d)", REG_DSC_RUN_Flag);
            //DB_printData("\r\n REG_DPS_RUN_Flag (%d)", REG_DPS_RUN_Flag);
            break;

#if ENABLE_DSC
        case 0x56:
            REG_DSC_RUN_Flag = 0;
            DB_printMsg("...Disable DSC Function...");
            break;
        case 0x57:
            msDSC_On_Setting();
            DB_printMsg("...Enable DSC Function...");
            break;
        case 0x58:
            msDSC_AdjustYgain(ucPara1<<8|ucPara2);
            DB_printMsg("...msDSC_AdjustYgain...");
            break;
        case 0x59:
            {
                extern void msDSC_Handler ( void );
                msDSC_Handler();
                msDSC_Off_Setting();
                DB_printMsg("...msDSC_Handler...");
            }
            break;
        case 0x5A:
            {
                //Check SatHistrogram
                BYTE ucTmp;
                Flag_DSC_Latch_Busy=1;
                for ( ucTmp = 0; ucTmp < 32; ucTmp++ )
                {
                    DB_printData("...1 ISR_SatHistogram32H ==>%x...",ISR_SatHistogram32H[ucTmp] );
                    DB_printData("...2 ISR_SatHistogram32HL==>%x",ISR_SatHistogram32H[ucTmp] );
                }
                Flag_DSC_Latch_Busy=0;
            }
            break;
        case 0x5B:
            {
                extern void msAdjustPCXvyccContrast ( BYTE bScalerWin, WORD uwContrast );
                msAdjustPCXvyccContrast ( MAIN_WINDOW, ucPara1<<8|ucPara2 );
            }
            break;
        case 0x5C:
            {
                extern void msAdjustVideoContrast(BYTE bScalerWin, BYTE ucContrast );
                //msWriteByteMask ( SC7_40, BIT0,  BIT0 );
                DB_printData("...2 msAdjustVideoContrast==>%x",0);
                //msAdjustVideoContrast ( MAIN_WINDOW, ucPara2 );
                msAdjustVideoContrast(MAIN_WINDOW,ucPara1);
            }
            break;
        case 0x5D:
            {
                extern void mStar_AdjustContrast( BYTE contrast );
                //msWriteByteMask ( SC7_40, BIT0,  BIT0 );
                DB_printData("...2 mStar_AdjustContrast==>%x",0);
                mStar_AdjustContrast( ucPara1 );
            }
            break;
#endif

#if ENABLE_DeltaE
        case 0x5E:
            {
                DB_printData("------Load DeltaE color mode: %x------",ucPara1);
                LoadDeltaEColorMode(MAIN_WINDOW, ucPara1);
            }
            break;
#endif
	case 0x5F:
		switch( ucPara1 )
		{
			case 0:
			DB_printMsg("Matrix2 Hue"); // Hue default 50:
			msAdjustXVYccVideoHue( MAIN_WINDOW, ucPara2 );
			break;
			case 1:
			DB_printMsg("Matrix2 Sat"); // Sat default 128
			msAdjustXVYccVideoSaturation( MAIN_WINDOW, ucPara2 );
			case 2 : 
			DB_printMsg("Matrix2Contrast/Gain"); // Con/R/G/B default 128
			msAdjustXVYCCVideoContrastRGB(MAIN_WINDOW, ucPara2, 0x80, 0x80, 0x80);
			break;
			case 3:
			DB_printMsg("Matrix2 Matrix Reset"); 
			msAdjustXVYccVideoHue( MAIN_WINDOW, 50 );
			msAdjustXVYCCVideoContrastRGB(MAIN_WINDOW, 0x80, 0x80, 0x80, 0x80);
			msAdjustXVYccVideoSaturation( MAIN_WINDOW, 128 );
			break;
		}
		break;
#if MS_DAC && (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
            case 0x60:
            {
                PCM_GEN(((ucPara1==1)?(TRUE):(FALSE)),ucPara2,ucPara3); //enable R_FR L_FR GAIN
                DB_printData("...2 PCM_GEN==>%x",0);
            }
            break;
#endif

#if CHIP_ID==CHIP_TSUMD && COLORVERIFY_DEBUG
            case 0x61:   //De-Mosquito
                {
                    extern void msDeMosquito(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD);
                    DB_printMsg("msDeMosquito");
                    msDeMosquito(ucPara1,ucPara2,ucPara3);
                }
                break;
            case 0x62:   //Spike noise filter
                {
                    extern void msSpikeNR(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD);
                    DB_printMsg("msSpikeNR");
                    msSpikeNR(ucPara1,ucPara2,ucPara3);
                }
                break;
            case 0x63:  //De-Blocking
                {
                    extern void msDeBlocking(BYTE u8Enable, /*BYTE u8Strength ,*/BYTE u8THRD);
                    DB_printMsg("msDeBlocking");
                    msDeBlocking(ucPara1,ucPara2);
                }
                break;
            case 0x64:  //SNR
                {
                    extern void msSNR(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD);
                    DB_printMsg("msSNR");
                    msSNR(ucPara1,ucPara2,ucPara3);
                }
                break;
            case 0x65:  //PreNoiseMasking(SPF APN)
                {
                    extern void msPreNoiseMasking(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD);
                    DB_printMsg("msPreNoiseMasking");
                    msPreNoiseMasking(ucPara1,ucPara2,ucPara3);
                }
                break;
            case 0x66:  //Hcoring(2D peaking APN)
                {
                    extern void msHcoring(BYTE u8Enable, BYTE u8Strength );
                    DB_printMsg("msHcoring");
                    msHcoring(ucPara1,ucPara2);
                }
                break;
            case 0x67:  //msShinnyColor
                {
                    extern void msShinnyColor(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD);
                    DB_printMsg("msShinnyColor");
                    msShinnyColor(ucPara1,ucPara2,ucPara3);
                }
                break;

            case 0x68:  //PostSNR
                {
                extern void Peaking_PostSNR(BYTE u8Enable, BYTE u8THRD);
                DB_printMsg("Peaking_PostSNR");
                Peaking_PostSNR(ucPara1,ucPara2);
                }
                break;
#endif
#endif // ENABLE_DEBUG

#if  ENABLE_LED_CONTROLLER
        case 0x69:
        {
            #if LED_CTRL_BRIGHTNESS_BY_CURRENT
            extern void BrightnessPWMSettingToCurrentSetting(BYTE current);
            BYTE i;
            DB_printMsg("++++++++ BrightnessPWMSettingToCurrentSetting");
            for (i=1;i<0xFF;i++)
            {
                #if ENABLE_LED_INT
                Init_LEDIRQEnable(FALSE);
                #endif

                BrightnessPWMSettingToCurrentSetting(i);
                ForceDelay1ms(10);
                #if ENABLE_LED_INT
                Init_LEDIRQEnable(TRUE);
                #endif
            }
            #endif
        }
        break;
        case 0x6A:
        {
            #if LED_CTRL_BRIGHTNESS_BY_CURRENT
#if 1
            extern void msLED_Off(void);
            extern void msLED_On(void);

            UserPrefBrightness = SIORxBuffer[2];
            DB_printData("UserPrefBrightness:%d", UserPrefBrightness);
            DB_printMsg(" ------- msLED_Off");
            msLED_Off();
            DB_printMsg(" ------- msLED_On");
            msLED_On();
#else
            extern void BrightnessPWMSettingToCurrentSetting(BYTE current);
            BYTE i;
            DB_printMsg(" ------- BrightnessPWMSettingToCurrentSetting");
            for (i=0xFF;i>1;i--)
            {
                #if ENABLE_LED_INT
                Init_LEDIRQEnable(FALSE);
                #endif

                BrightnessPWMSettingToCurrentSetting(i);
                ForceDelay1ms(10);
                #if ENABLE_LED_INT
                Init_LEDIRQEnable(TRUE);
                #endif
            }
#endif
            #endif
        }
        break;
        case 0x6B:
        {
            extern void msLED_On(void);
            DB_printMsg(" ------- msLED_On");
            msLED_On();
        }
        break;
        case 0x6C:
        {
            extern void msLED_Off(void);
            DB_printMsg(" ------- msLED_Off");
            msLED_Off();
        }
        break;

	case 0xFC:
        {
            DB_printData(" =====SetDimmingPWMFreq====%d",(ucPara1<<8|ucPara2));
            SetDimmingPWMFreq(BRIGHTNESS_VSYNC_ALIGN, ucPara1<<8|ucPara2);
        }
        break;
        case 0xFD:
        {
		DB_printMsg(" =====>>LED_TEST BUS (BOOST CLK and SAR_CK_SAM)");

		msWriteByte(REG_1EEB,0x51);
		msWriteByte(REG_1E02,0x01);
		msWriteByte(REG_0A9A,0x01);
		msWriteByte(REG_0201,0x10);
		msWriteByte(REG_0202,0x04);
		msWriteByteMask(REG_0A67,0x00,BIT7);
		msWriteByteMask(REG_0A6F,0x00,BIT7);
        }
        break;
#endif



            /*
            case 0x20:
            {
                if(SIORxBuffer[2] == 2)
                    DB_printMsg( "<check> 2" );
                DB_printMsg( "SIORxBuffer[1] = 0x20" );

                break;
            }
            */
#if ENABLE_DEBUG && ENABLE_MHL
        case 0x6D:
        {
            WORD waddr, wdata;
            waddr = (WORD)(SIORxBuffer[2]<<8);
            waddr |= SIORxBuffer[3];
            wdata = (WORD)(SIORxBuffer[4]<<8);
            wdata |= SIORxBuffer[5];
            DB_printData( "<ElandWriteWord_waddr:%x>",  waddr);
            DB_printData( "<ElandWriteWord_wdata:%x>",  wdata);
            break;
        }
        case 0x6E:
        {
            WORD waddr, wdata;
            waddr = (WORD)(SIORxBuffer[2]<<8);
            waddr |= SIORxBuffer[3];
            DB_printData( "<ElandReadWord:%x>",  wdata);
            break;
        }
        case 0x6F:
        {
            mhal_mhl_SetHPD(SIORxBuffer[2]);
            if(SIORxBuffer[2])
                DB_printMsg( "<mhl_SetHPD HIGH>");
            else
                DB_printMsg( "<mhl_SetHPD LOW>");
            break;
        }
        #if CHIP_ID == CHIP_TSUM2
        case 0x70:
            MhlTestfunction(SIORxBuffer[2], SIORxBuffer[3], SIORxBuffer[4], SIORxBuffer[5]);
         break;
        case 0x71:
            MhlTestfunction1(SIORxBuffer[2], SIORxBuffer[3], SIORxBuffer[4], SIORxBuffer[5]);
         break;
         #endif
#endif
#if ENABLE_DEBUG && (ENABLE_TOUCH_PANEL||ENABLE_TOUCH_PANEL_CTRL_OSD)
        case 0x78:
        {
            if(TPL_ReadSurface() == FALSE)
                DB_printMsg( "<TPL_ReadSurface FAIL>");
            break;
        }
        case 0x79:
        {
            break;
        }
#endif

        case 0x72: // Keypad_LEFT
            KeyDebug = KEY_MINUS;
            break;

        case 0x73: // Keypad_MENU
            KeyDebug = KEY_MENU;
            break;

        case 0x74: // Keypad_RIGHT
            KeyDebug = KEY_PLUS;
            break;

        case 0x75: // Keypad_RIGHT
            KeyDebug = KEY_IncVal;//KEY_SELECT;
            break;

        case 0x76: // Keypad_POWER
            KeyDebug = KEY_POWER;
            break;

        case 0x77: // Keypad_Enter
            KeyDebug = KEY_EXIT;
            break;

        case 0x98:
            MenuPageIndex = RootMenu;
            MenuItemIndex = 0;
            ExecuteKeyEvent( MIA_RedrawMenu );
            MenuPageIndex = ucPara1;
            MenuItemIndex = 0;
            ExecuteKeyEvent( MIA_RedrawMenu );
            break;

#if ENABLE_DEBUG && ENABLE_TOUCH_PANEL_DATA_FROM_USB
        case 0x80:
        {
            USB_SPI_WriteRegister(SIORxBuffer[2], SIORxBuffer[3]);
            DB_printData("WriteAddr:%x", SIORxBuffer[2]);
            DB_printData("WriteData:%x", SIORxBuffer[3]);
        }
        case 0x81:
        {
            BYTE xdata tempbuf[0x20], i;
            USB_SPI_ReadRegister(0xC0, tempbuf, 0x20);
            for(i=0; i<0x20; i++)
                DB_printData("tempbuf[%x]", tempbuf[i]);

            break;
        }
#endif
#if ENABLE_DEBUG && ENABLE_TOUCH_KEY
        case 0x90:
        {
            DB_printData("TouchKey:%x", IT7230_GetKeyStatus());
            break;
        }
#endif

#if ENABLE_DEBUG
        case 0x91:
        {
            extern void drvADC_Test_Funct(BYTE tb1, BYTE tb2);
            drvADC_Test_Funct(SIORxBuffer[2], SIORxBuffer[3]);
            break;
        }
#endif

#if OSD_TO_IP_DEBUG
		case 0x92:
		{
			TestCommand_OsdToIp(SIORxBuffer[2]);
			break;
		}
#endif

#if DEBUG_SELFTEST
        case 0x99:
        {
            SelfTest();
            break;
        }
#endif
		case 0xA0:
		{
			AdjustOSDTransparency(ucPara1);
		}
		break;
		
#if ENABLE_MCUTOPTEST
    case 0xF0:
    {
        #define CACHE_ENABLE()  (msWriteBit( REG_1018, 1, _BIT3 ), msWriteBit( REG_2BA0, 0, _BIT0 ))
        #define CACHE_DISABLE()  (msWriteBit( REG_2BA0, 1, _BIT0 ), msWriteBit( REG_1018, 0, _BIT3 ))
        if (ucPara1==0x01)
        {
            CACHE_ENABLE();
            DB_printMsg("AAA CACHE_ENABLE");
        }
        else
        {
            CACHE_DISABLE();
            DB_printMsg("BBB CACHE_DISABLE");
        }
        break;
    }
    case 0xF1:
    {
        extern void mcuSetMcuSpeed( BYTE ucSpeedIdx );
        mcuSetMcuSpeed(ucPara1);
        DB_printData(" mcuSetMcuSpeed ==%d",ucPara1);
        break;
    }
    case 0xF2:
    {
        extern void mcuSetSpiSpeed( BYTE ucIndex );
        mcuSetSpiSpeed(ucPara1);
        DB_printData(" mcuSetSpiSpeed ==%d",ucPara1);
        break;
    }
    case 0xF3:
    {
        extern void mcuSetSpiMode( BYTE ucMode );
        mcuSetSpiMode(ucPara1);
        DB_printData(" mcuSetSpiMode ==%d",ucPara1);
        break;
    }
#endif
#if ENABLE_DP_INPUT&&((CHIP_ID==CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID==CHIP_TSUMD)||(CHIP_ID==CHIP_TSUM9)||(CHIP_ID==CHIP_TSUMF))
    case 0xF4:
            DB_printMsg(" =====HPD====");
            DPRxHPDDisable();                                                                           // HPD ouput enable and set Low
            ForceDelay1ms( 100 );
            DPRxHPDEnable();
         break;

    case 0xF5:
          m_ucState = eSTATE_RETRAIN;
          DB_printMsg(" =====Chg State====");
         break;



        default:
        break;
#endif

#if CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK ||CHIP_ID == CHIP_TSUMD ||CHIP_ID == CHIP_TSUMJ
	case 0xF6:
	{

		#define DELAY_LINES  2 //  default: 2
		#define DELAY_LINE_Offset  1 //  default: 1
		BYTE xdata u8VSyncTime=GetVSyncTime();
		WORD xdata Hi_Act=SC0_READ_AUTO_WIDTH();   //  OK
		WORD xdata Vi_Act=SC0_READ_AUTO_HEIGHT();  //  OK
		WORD xdata Hi_Total=mSTar_GetInputHTotal();   //  OK
		WORD xdata Vi_Total=SC0_READ_VTOTAL();	         //  OK

		WORD xdata Ho_Act=PanelWidth;
		WORD xdata Vo_Act=PanelHeight;
		WORD xdata Ho_Total=PanelMinHTotal;
		WORD xdata Vo_Total=( (DWORD)Vo_Act*Vi_Total/Vi_Act);

		WORD xdata Toha = ((DWORD)Ho_Act * Vi_Total*1000)/((DWORD)Ho_Total*Vo_Total);
		WORD xdata Tiha = ((DWORD)Hi_Act * 1000) /Hi_Total ;

		DB_printData(" == Hi_Act ==%d",Hi_Act);
		DB_printData(" == Vi_Act ==%d",Vi_Act);
		DB_printData(" == Hi_Total ==%d",Hi_Total);
		DB_printData(" == Vi_Total ==%d",Vi_Total);
		DB_printData(" == Vo_Total ==%d",Vo_Total);
		DB_printData(" == Toha ==%d",Toha);
		DB_printData(" == Tiha ==%d",Tiha);

	#if ENABLE_R2_2DTO3D
		if (UserPref2Dto3D)
		{
			msWriteByteMask(SC0_90, (((Toha<=Tiha)?(DELAY_LINES+1+DELAY_LINE_Offset):(DELAY_LINES+DELAY_LINE_Offset)))<<4 ,BIT6|BIT5|BIT4);
			DB_printData(" ==SC0_90 ==%d",(((Toha<=Tiha)?(DELAY_LINES+1+DELAY_LINE_Offset):(DELAY_LINES+DELAY_LINE_Offset))));
		}
		else
	#endif
		{
			msWriteByteMask(SC0_90, (((Toha<=Tiha)?(DELAY_LINES+1):(DELAY_LINES)))<<4 ,BIT6|BIT5|BIT4);
			DB_printData(" ==SC0_90 ==%d",(((Toha<=Tiha)?(DELAY_LINES+1):(DELAY_LINES))));
		}
		//-------------------------------------------------------------
		//SCD_64[1]:"flag 1 once afifo can't receive pixels when IP sends.0: don't happen1: occur"
		//SCD_65[1]:"flag 1 once afifo can't give pixels when OP needs.0: don't happen1: occur"
		//-------------------------------------------------------------
		msWriteByteMask(SCD_64, BIT2 ,BIT2);
		msWriteByteMask(SCD_65, BIT2 ,BIT2);
		Delay1ms(1);
		msWriteByteMask(SCD_64, 0x00 ,BIT2);
		msWriteByteMask(SCD_65, 0x00 ,BIT2);
		Delay1ms(u8VSyncTime);
		if ((msReadByte(SCD_64)&BIT1) || (msReadByte(SCD_65)&BIT1))
			DB_printMsg("FAIL");
		else
			DB_printMsg("PASS");
	}
	 break;
#endif



#if Enable_Expansion
	case 0xFE:
	{
		extern void SetExpansionMode( void );
		UserprefExpansionMode = ucPara1;
		DB_printData(" =====UserprefExpansionMode  ==%x====",UserprefExpansionMode);
		SetExpansionMode();//This Function will mute when reset all setting.
	}
	break;
#endif

#if ENABLE_TIME_MEASUREMENT
        case 0xFF:
        {
            DWORD i;
            TIME_MEASURE_BEGIN();
            for( i = 0; i < 1000; i++ )
                ForceDelay1ms( 1 );
            TIME_MEASURE_END();

            TIME_MEASURE_BEGIN();
            for( i = 0; i < 250000; i++ );
            TIME_MEASURE_END();

            TIME_MEASURE_BEGIN();
            for( i = 0; i < 250000; i++ )
                Delay4us();
            TIME_MEASURE_END();
        }
        break;
#endif
    }
}
void ProcessCommand( void )
{
    switch( UART_CMD )
    {
        case kWrite_MST_Bank_n:
        {
#if ENABLE_MHL && ENABLE_DEBUG// 120403 coding, addition for eland uart debug
            if(UART_CMD_MS_BANK&0x80)
            {
    #if CHIP_ID == CHIP_TSUM2
                ElandWriteHalfWord((UART_CMD_MS_BANK & 0x0F) << 8|UART_CMD_MS_REGINDEX, UART_CMD_MS_REGDATA);
    #endif
            }
            else
#endif
            {
                const volatile BYTE ucBank = scReadByte( SC0_00 ); // store original bank
                scWriteByte( SC0_00, UART_CMD_MS_BANK );
                scWriteByte( UART_CMD_MS_REGINDEX, UART_CMD_MS_REGDATA );
                scWriteByte( SC0_00, ucBank ); // restore original bank
            }
            putSIOChar( 0xF1 );
            break;
        }

        case kRead_MST_Bank_n:
        {
#if ENABLE_MHL && ENABLE_DEBUG// 120403 coding, addition for eland uart debug
            if(UART_CMD_MS_BANK&0x80)
            {
    #if CHIP_ID == CHIP_TSUM2
                UART_CMD_MS_REGDATA = ElandReadHalfWord((UART_CMD_MS_BANK & 0x0F) << 8|UART_CMD_MS_REGINDEX);
    #endif
            }
            else
#endif
            {
                const volatile BYTE ucBank = scReadByte( SC0_00 ); // store original bank
                scWriteByte( SC0_00, UART_CMD_MS_BANK );
                UART_CMD_MS_REGDATA = scReadByte( UART_CMD_MS_REGINDEX );
                scWriteByte( SC0_00, ucBank ); // restore original bank
            }
            putSIOChar( 0xF2 );
            putSIOChar( UART_CMD_MS_REGDATA );
            break;
        }

        case kWrite_MCU_XDATA:
        {
            //msWriteByte( MAKEWORD( UART_CMD_MCU_IDX_H, UART_CMD_MCU_IDX_L ), UART_CMD_MCU_DATA );
            msRegs[MAKEWORD( UART_CMD_MCU_IDX_H, UART_CMD_MCU_IDX_L )] =  UART_CMD_MCU_DATA;
            putSIOChar( 0xF1 );
            break;
        }

        case kRead_MCU_XDATA:
        {
            //UART_CMD_MCU_DATA = msReadByte( MAKEWORD( UART_CMD_MCU_IDX_H, UART_CMD_MCU_IDX_L ) );
            UART_CMD_MCU_DATA = msRegs[( MAKEWORD( UART_CMD_MCU_IDX_H, UART_CMD_MCU_IDX_L ) )];
            putSIOChar( 0xF2 );
            putSIOChar( UART_CMD_MCU_DATA );
            break;
        }

        case kTest_Command:

        {
            ExecTestCommand();
            break;
        }
    #if !USEFLASH
        case kWrite_EEPROM:
        {
            WORD uwEEADDR;
            uwEEADDR = MAKEWORD(SIORxBuffer[1],SIORxBuffer[2]);
            #if 0//ENABLE_HDCP
            if (uwEEADDR>=RM_HDCP_KEY_BASEADDR1)
                g_wHDCP_KeyChkSum += g_UartCommand.Buffer[_UART_CMD_INDEX3_];
            #endif // ENABLE_HDCP
            NVRam_WriteByte(uwEEADDR, SIORxBuffer[3]);
            //if (!g_bDebugASCIICommandFlag)
                putSIOChar(0xF1);
                //printData("EEPROM[0x%x]=", uwEEADDR);
                //printData("0x%x \r\n", SIORxBuffer[_UART_CMD_INDEX3_]);

            break;
        }
        case kRead_EEPROM:
        {
        WORD uwEEADDR;
            uwEEADDR = MAKEWORD(SIORxBuffer[1],SIORxBuffer[2]);
            //uwEEADDR = g_UartCommand.Buffer[_UART_CMD_INDEX1_] & 0x0E;
            //uwEEADDR = (uwEEADDR << 7) + g_UartCommand.Buffer[_UART_CMD_INDEX2_];
        NVRam_ReadByte(uwEEADDR , &SIORxBuffer[3]);
            putSIOChar(0xF2);
            putSIOChar(SIORxBuffer[3]);
            //if (g_bDebugASCIICommandFlag)
            //{
                //printData("EEPROM[0x%x]=", uwEEADDR);
                //printData("0x%x \r\n", SIORxBuffer[_UART_CMD_INDEX3_]);
            //}
            break;
        }
    #endif
        default:
            break;
    }
}

Bool GetCommand( void )
{
#if 1

    if ( (SIORxIndex >= UART_CMD_LENGTH) && (UART_CMD_LENGTH))
    {
        Set_RxBusyFlag();
        return TRUE;
    }
    else
        return FALSE;

#else
    if( u8ReSyncCounter )
    {
        if( SIORxIndex > 2 && SIORxIndex == CommandLength )
        {
            Set_RxBusyFlag();
            return TRUE;
        }
        if( u8ReSyncCounter < 5 )
        {
            Clr_RxBusyFlag();
            SIORxIndex = 0;
            u8ReSyncCounter = 0;
        }
    }
    else if( SIORxIndex )
    {
        SIORxIndex = 0;
    }
    return FALSE;
#endif
}
void DebugHandler( void )
{

    if( !UART_READ_ES())
        return;
#if 1
    if( GetCommand() )
    {
        ProcessCommand();
        SIORxIndex = 0;
        Clr_RxBusyFlag();
    }
#else
    if( GetCommand() )
    {
        ProcessCommand();
        SIORxIndex = 0;
        u8ReSyncCounter = 0;
        Clr_RxBusyFlag();
    }
    else if( SIORxIndex > 7 )
    {
        AOCAlign();
        Clr_RxBusyFlag();
        SIORxIndex = 0;
    }
#endif
}
#if !ENABLE_DEBUG
BYTE code msDebugNullData[] = {0};
void msDebugDummy(void)
{
    BYTE xdata i = msDebugNullData[0];
}
#endif

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
void ResetTimingChangeFirstPoint(void)
{
    bTimingChangeFirstPointFlag = 0;
    TimingChangeFirstPoint = TimingChange_Def;
    bTimingChangeFirstPointFlag = 1;
}

void SetTimingChangeFirstPoint(TimingChangeFirstPointType firstpoint)
{
    if(bTimingChangeFirstPointFlag)
    {
        bTimingChangeFirstPointFlag = 0;
        TimingChangeFirstPoint = firstpoint;
    }
}
#endif


