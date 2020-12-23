#define _MCU_C_
#include "board.h"
#include "types.h"
#include "misc.h"
#include "debug.h"
#include "Common.h"
#include "ms_rwreg.h"
#include "Reg52.h"
#include "global.h"
#include "mstar.h"
#include "ms_reg.h"
#include "mcu.h"
#include "msHDCP.h"
#include "gpio_def.h"
//////////////////////////////////
//#include "drvMcu.h"
//#include "halRwreg.h"
/////////////////////////////////

void Init_MCU( void )
{
    EA=0;

#if( ENABLE_WATCH_DOG )
    Init_WDT( _ENABLE );
#else
    Init_WDT( _DISABLE );
#endif  // end of #if( ENABLE_WATCH_DOG )

    Init_IOPorts();
    Init_ExternalInterrupt();
    Init_Timer();
    Init_SerialPort();

#if ENABLE_DEBUG && ((CHIP_ID==CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID==CHIP_TSUMD)||(CHIP_ID==CHIP_TSUMJ)||(CHIP_ID==CHIP_TSUM9)||(CHIP_ID==CHIP_TSUMF))
    mcuSetUartMux(DFT_MUX_Uart0, DFT_MUX_Uart1);
#endif

    mcuSetSystemSpeed(SPEED_XTAL_MODE);

#if Enable_Cache
    CACHE_ENABLE();
#else
    CACHE_DISABLE();
#endif

    EA=1;
}

/*
#if( ENABLE_WATCH_DOG )
void ClearWDT( void )
{
    msWriteBit( REG_3C66, 1, _BIT1 );
}
#endif  // end of #if( ENABLE_WATCH_DOG )
*/

// Initialize I/O setting
void Init_IOPorts( void )
{
    #if ENABLE_POWER_BOARD_CONTROL
    Clr_PowerBoardSaving_Pin();
    Init_PowerBoardSaving_Pin();
    #endif

    hw_SetDDC_WP();
    hw_SetFlashWP();
    #if !ENABLE_LED_CONTROLLER
    hw_ClrBlacklit();
    #endif
    hw_ClrPanel();
#if Enable_LED
 	hw_ClrGreenLed();
	hw_SetAmberLed();
#else
	hw_ClrGreenLed();
    hw_ClrAmberLed();
#endif

    Init_hwDDC_WP_Pin();
    Init_hwFlash_WP_Pin();
    Init_hwBlacklit_Pin();
    Init_hwPanel_Pin();
    Init_hwGreenLed_Pin();
    Init_hwAmberLed_Pin();
#if IR_FUNC_EN	//20130824
	 Init_IR_DataKey_Pin();
#endif

    Init_PowerKey();
    Init_hwDSUBCable_Pin();
#if (CHIP_ID==CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID==CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF)
    Init_hwDVI0Cable_Pin();
    Init_hwDVI1Cable_Pin();
    Init_hwDVI2Cable_Pin();
#else
    Init_hwDVICable_Pin();
#if ENABLE_HDMI
    Init_hwHDMICable_Pin();
#endif
#endif

#if HDMI_PRETEST
    Init_hwCEC_Pin();
#endif

#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
    DVI5V_GPIO_Sel();
#endif

#if !USEFLASH
    Set_EEPROM_WP();
    Init_hwI2C_SCL_Pin();
    Init_hwI2C_SDA_Pin();
    Init_hwEEPROM_WP();
#endif

#ifdef Internal_EDID_Write_To_24C02
		if(!LoadHDMIEDIDFlag)
		{
        Init_NVRAM_I2C_SCL_Pin();
        Init_NVRAM_I2C_SDA_Pin();
		}
#endif


#if ENABLE_HDCP
#if ENABLE_MHL
#if ENABLE_MHL_C1
    hw_Set_MHLTriStateHpd();
#endif
#if ENABLE_MHL_C2
    hw_Set_MHLTriStateHpd2();
#endif
#if ENABLE_MHL_C3
    hw_Set_MHLTriStateHpd3();
#endif
#else
    Init_hwHDCP_Hpd_Pin();
    Init_hwHDCP_Hpd_Pin2();
    Init_hwHDCP_Hpd_Pin3();
#endif // #if ENABLE_MHL
#endif // #if ENABLE_HDCP

#if AudioFunc
    hw_SetMute();
    Init_hwMute_Pin();
    hw_ClrAudio_SD();
    Init_Audio_SD_Pin();
  #if ENABLE_SPDIF // SPDIF GPIO CAN'T shared with analog audio functions (line in/line out/ear out)
    Init_SPIFGPIO_Pin();
  #endif
#endif

#if ENABLE_USB_INPUT
    hw_USB_LOW();
    Init_hwUSB_Pin();
#endif

#if ENABLE_TOUCH_PANEL || ENABLE_TOUCH_PANEL_DATA_FROM_USB||ENABLE_TOUCH_PANEL_CTRL_OSD
#if ENABLE_TOUCH_PANEL_DATA_FROM_USB
    Init_USB_INT();
#endif
    // keep all output pin at low state before turning on touch panel power
    Clr_TOCUCH_PANEL_POWER();
    Init_TOCUCH_PANEL_POWER_Pin();
    Init_SPI_MISO_Pin();
    Clr_SPI_SCK();
    Init_SPI_SCK_Pin();
    Clr_SPI_MOSI();
    Init_SPI_MOSI_Pin();
    Clr_SPI_SEL();
    Init_SPI_SEL_Pin();
#endif

#if ENABLE_MHL
    Init_SWI2C_SCL();
    Init_SWI2C_SDA();
    Init_MHL_CABLE_DETECT_Pin();
    CLR_MHL_PS_CTRL_CHARGE0();
    CLR_MHL_PS_CTRL_CHARGE1();
    CLR_MHL_PS_CTRL_CHARGE2();
#endif

#if ENABLE_TOUCH_KEY
    SET_CPK_KEYVCC();
    SET_CPK_LEDVCC();
    Init_HW_CPK_KEYVCC_Pin();
    Init_HW_CPK_LEDVCC_Pin();
    Init_SK_INT();
    Init_SK_SCL();
    Init_SK_SDA();
#endif

#if PANEL_VCOM_ADJUST
    HW_CLR_VCOM_I2C_SCL();
    HW_CLR_VCOM_I2C_SDA();
    Init_VCOM_I2C_SCL_Pin();
    Init_VCOM_I2C_SDA_Pin();
#endif
#if EarphoneDet_Enable
INIT_HW_HarphoneDet_PIN();
Init_Earphone_hwMute_Pin();
#endif

}
/* initialize 8051 CPU timer & interrupt routine */
/* TCON.7(   TF1): Timer 1 overflow flag */
/* TCON.6(   TR1): Timer 1 stop/start bit */
/* TCON.5(   TF0): Timer 0 overflow flag */
/* TCON.4(   TR0): Timer 0 stop/start bit */
/* TCON.3(   IE1): Timer 1 INT1 interrupt flag */
/* TCON.2(   IT1): Timer 1 INT1 interrupt style setup*/
/* TCON.1(   IE0): Timer 0 /INT0 interrupt flag */
/* TCON.0(   IT0): Timer 0 /INT0 interrupt style setup */
void Init_ExternalInterrupt( void )
{
    //INT0
#if EXT_TIMER0_1MS || (ENABLE_MENULOAD && ML_USE_INT) ||ENABLE_SW_DOUBLE_BUFFER
    #if EXT_TIMER0_1MS
    INT_FIQ_TIMER0_ENABLE(TRUE);//msWriteByteMask( REG_2B00, 0, _BIT0 );
    #endif
    #if (ENABLE_MENULOAD && ML_USE_INT)
    msWriteByteMask( REG_2B00, 0, _BIT5 );
    #endif
    IT0 = 0; // 0:Level  1: falling edge trigger
    IE0 = 0;
    EX0 = 1; // enable external interrupt 0
#else
    IT0 = 0;
    IE0 = 0;
    EX0 = 0;  // disable external interrupt 0
#endif

    //INT1
#if UseINT
    INT_SYNC_CHANGE_TRIGGER_TYPE();//msWriteByteMask( SC0_CA, _BIT1 | _BIT0, _BIT1 | _BIT0); // BIT0 for level trigger, BIT1 for active high

    /*
    #if CHIP_ID>=CHIP_TSUMV
    msWriteByteMask( REG_2B18, 0, _BIT1 ); // disp_int
    #else
    msWriteByteMask( REG_2B19, 0, _BIT2 ); // disp_int
    #endif
    */
    INT_IRQ_DISP_ENABLE(TRUE); // disp_int

    IT1 = 0;//1; // set external interrupt 0 & 1 as falling edge trigger
    IE1 = 0;  // interrupt flag
    EX1 = 1;  // enable external interrupt 1
#else
    IT1 = 0;
    IE1 = 0;
    EX1 = 0;
#endif

    PX0=1;  //
    PX1=1;  //

#if (MS_PM)&& ENABLE_DEBUG
    PS=1;
#endif

#if (ENABLE_DEBUG)
    IP0 = _BIT0 | _BIT4;
    IP1 = _BIT0 | _BIT4;
#else
    IP0 = _BIT2;
    IP1 = _BIT0 | _BIT2 | _BIT4;
#endif
}
void Init_ExtTimerCount( void )
{
    #if EXT_TIMER0_1MS
    DWORD u32ExtTimerCount;
    u32ExtTimerCount=((DWORD)(EXT_TIMER0_CounterTime)*((msReadByte(REG_PM_BC)&BIT6)?(CLOCK_4MHZ/1000ul):(CLOCK_XTAL/1000ul)));
    msWrite2Byte(REG_3C80,u32ExtTimerCount&0xFFFF);
    msWrite2Byte(REG_3C82,(u32ExtTimerCount>>16)&0xFFFF);
    msWriteByte( REG_3C89, 0x03 );

    #else
    msWriteByte( REG_3C89, 0x00 );
    #endif
}

void Init_Timer( void )
{
    WORD clock_period = ClockPeriod;

  /* -------------initialize Timer 0 -----------------------------*/
  g_ucTimer0_TH0 = clock_period >> 8;
  g_ucTimer0_TL0 = clock_period & 0xFF;
  ET0=1;  // enable timer 0 interrupt
  TH0 = g_ucTimer0_TH0;
  TL0 = g_ucTimer0_TL0; // timer 0 counter
  TF0=0; // timer 1 flag

 /* -------------initialize Timer 1 -----------------------------*/
  #if IR_FUNC_EN  //20130824
	g_ucTimer1_TH0 = clock_period >> 8;
	g_ucTimer1_TL0 = clock_period & 0xFF;
	ET1=1;	
	TH1=g_ucTimer1_TH0;  
	TL1=g_ucTimer1_TL0;	
	TF1=0;
	PT1=1;
#else
	ET1=0;  // disable timer 1 interrupt
	TH1=0;
	TL1=0; // timer 1 counter
	TF1=0; // timer 1 flag
#endif
	// setup Timer mode
	// TMOD=0x11;  // set timer 1 as timer(1) , timer 0 as counter(5)
	TMOD=0x11;
	// enable/disable timer
	TR0=1;  // disable timer 0
#if IR_FUNC_EN 	
	TR1=1;  // enable timer 1
#else
	TR1=0;  // disable timer 1
#endif


    Init_ExtTimerCount();
#if 0//EnableTime1Interrupt
    TR1 = 0;
    ET1 = 0;
    TH1 = g_ucTimer0_TH0;
    TL1 = g_ucTimer0_TL0;
    TMOD=0x11;
    TR1 = 1;
    ET1 = 1;
#endif

}
/*----------Timer 2 -------------------*/
/* T2CON.7(   TF2): overflow flag */
/* T2CON.6(  EXF2): extern enable flag */
/* T2CON.5(  RCLK): receive clock */
/* T2CON.4(  TCLK): transfer clock */
/* T2CON.3( EXEN2): extern enable flag */
/* T2CON.2(   TR2): stop/start timer 2 */
/* T2CON.1(  C_T2): intern clock(0)/extern counter(1) switch */
/* T2CON.0(CP_RL2): capture flag */
void Init_SerialPort( void )
{
    ADCON |= _BIT7;             // use S0RELH, S0RELL as baudrate generator

  #if UART1
    S1CON = (_BIT7 | _BIT4);
  #else
    SCON = 0x50;                // mode 1, 8-bit UART, enable receive
  #endif
    PCON |= _BIT7;
    TI = 0;                     // clear transfer flag
    RI = 0;

  #if ENABLE_DEBUG

    #if UART1
    IEN2 |= ES1;
    #else
    ES = 1;                     // enable uart interrupt
    #endif

    // default baudrate-xtal
    // uart0
    S0RELH = HIBYTE( S0REL );
    S0RELL = LOBYTE( S0REL );
    // uart1
    S1RELH = HIBYTE( S1REL );
    S1RELL = LOBYTE( S1REL );

  #else
    ES = 0;                     // disable uart interrupt
    IEN2 &= ~ES1;
  #endif

  #if UART1
    S1CON &= ~TI1;    // clear TI1
    S1CON &= ~RI1;    // clear RI1
  #endif

    TI0_FLAG = 0;
    u8ReSyncCounter = 0;
    SIORxIndex = 0;
/*
    g_UartCommand.Index = 0;
    g_bUartDetected=0;

    g_bDebugASCIICommandFlag = _DISABLE;
    CLR_DEBUG_ONLY_FLAG();
*/
}


