
///////////////////////////////////////
// DRVMCU_V VERSION: V02
////////////////////////////////////////
#include "global.h"

void Init_WDT( BYTE bEnable )
{
    msWriteByteMask( REG_3C66, BIT2,BIT2);  //clear watchdog reset flag
    msWriteByteMask( REG_3C66, 0x00,BIT2);
    if( bEnable )
    {

#if( ENABLE_WATCH_DOG )
        // initialize the watchdog timer reset interval
        SetWDTClk(CLK_LIVE_XTAL);
#endif  // end of #if( ENABLE_WATCH_DOG )
// initialize the watchdog interrupt timer
#if ENABLE_WATCH_DOG_INT	
		//msWriteBit(REG_2B00, FALSE, _BIT1);//MASK ENABLE 
		INT_FIQ_WDT_ENABLE(TRUE); 		
#else
		msWriteByte( REG_3C64, 0x00 );	//add for WDT INT
		msWriteByte( REG_3C65, 0x00 );	//add for WDT INT
#endif			

        msWriteByte( REG_3C60, 0xAA );
        msWriteByte( REG_3C61, 0x55 );
    }
    else
    {
        msWriteByte( REG_3C60, 0x55 );
        msWriteByte( REG_3C61, 0xAA );
        //msWriteBit(REG_2B00, TRUE, _BIT1);//MASK ENABLE // Jonson 20110713
        INT_FIQ_WDT_ENABLE(FALSE); 		
    }
}

#if( ENABLE_WATCH_DOG )
void SetWDTClk(BYTE clk_live_sel)
{
    WDT_CLEAR();
    if (clk_live_sel == CLK_LIVE_RCOSC_4M)
    {
         WORD wdt_sel_high;
         wdt_sel_high = 65536 - ( CLOCK_4MHZ / 65536 ) * WATCH_DOG_RESET_TIME;
         msWriteByte( REG_3C62, LOBYTE( wdt_sel_high ) );
         msWriteByte( REG_3C63, HIBYTE( wdt_sel_high ) ); // need set from H-byte to L-byte if default is not maximum
#if ENABLE_WATCH_DOG_INT
       	 wdt_sel_high = 65536 - ( CLOCK_4MHZ / 65536 )*( WATCH_DOG_RESET_TIME - WATCH_DOG_Interrupt_TIME );
       	 msWriteByte( REG_3C64, LOBYTE( wdt_sel_high ) );
         msWriteByte( REG_3C65, HIBYTE( wdt_sel_high ) );
#endif
    }
    else // CLK_LIVE_XTAL
    {
         WORD wdt_sel_high;
         wdt_sel_high = 65536 - ( CRYSTAL_CLOCK / 65536 ) * WATCH_DOG_RESET_TIME;
         msWriteByte( REG_3C62, LOBYTE( wdt_sel_high ) );
         msWriteByte( REG_3C63, HIBYTE( wdt_sel_high ) ); // need set from H-byte to L-byte if default is not maximum
#if ENABLE_WATCH_DOG_INT
         wdt_sel_high = 65536 - ( CRYSTAL_CLOCK / 65536 )*( WATCH_DOG_RESET_TIME - WATCH_DOG_Interrupt_TIME );
         msWriteByte( REG_3C64, LOBYTE( wdt_sel_high ) );
         msWriteByte( REG_3C65, HIBYTE( wdt_sel_high ) );
#endif
    }
}
#endif

/*
REG_0212
[0]: Enable GPIO 40, 41 as Uart-0 function
[1]: Enable GPIO 34, 35 as Uart-0 function
[2]: Enable GPIO 02, 00 as Uart-0 function
[3]: Enable GPIO 00, 01 as Uart-1 function ?
[4]: Enable GPIO 20, 21 as Uart-1 function
[5]: Enable GPIO 32, 33 as Uart-1 function

[7]: Enable GPIO 02, 01 as Uart-0 function
*/
#if ENABLE_DEBUG
void mcuSetUartMux( BYTE ucMuxUart0, BYTE ucMuxUart1 )
{
    msWriteByteMask(REG_0212, ucMuxUart0, BIT7|BIT2|BIT1|BIT0);
    msWriteByteMask(REG_0212, ucMuxUart1, BIT5|BIT4);
}
#endif
//-------------------------------------------------------------------------------------------------
// spi clock
// SPI clock setting
//
// reg_reserved0[5:0]
//
// h0027    h0026   31  0   reg_reserved0   "Reserved.reg_reserved0[5:0]: for spi clock selectionreg_reserved0[8]: for spi new cycle"
//
//
// reg_ckg_spi[6]
//  0: Crystal clock
//  1: PLL clock
//
// reg_ckg_spi[4:2], clock selection
//
#if ENABLE_DEBUG
DWORD code g_mcuSpiFreqTable[] =
{
    CLOCK_4MHZ,            // 0
    CLOCK_12MHZ,           // 1
    CLOCK_216MHZ,          // 2
    CLOCK_MPLL_MHZ,        // 3,
    CLOCK_108MHZ,          // 4,
    CLOCK_86MHZ,           // 5,
    CLOCK_54MHZ,           // 6,
    CLOCK_0MHZ,           // 7,
    CLOCK_XTAL,            // 8
};
#endif

// ucIndex ( PLL clock selection, whenever change, please make sure reg_ckg_spi[5]=0;)
//          1: 36   MHz pass
//          2: 43   MHz pass
//          3: 54   MHz, don't use, reserved for future
//          3: 86   MHz, don't use, reserved for future
//          4: Crystal clock
void mcuSetSpiSpeed( BYTE ucIndex )
{
    if( ucIndex == IDX_SPI_CLK_XTAL)
    {
        msWriteByteMask( REG_08E0, 0, _BIT2 );
    }
    else
    {
        msWriteByteMask( REG_08E0, ucIndex << 3, _BIT5 | _BIT4 | _BIT3);
        msWriteByteMask( REG_08E0, _BIT2, _BIT2 );
    }
}
void SetSPI_Quad_En(BYTE ucEnable)
{
	ucEnable=ucEnable;
}
/*
SPI model select.
0x0: Normal mode, (SPI command is 0x03)
0x1: Enable fast read mode, (SPI command is 0x0B)
0x2: Enable address single & data dual mode, (SPI command is 0x3B)
0x3: Enable address dual & data dual mode, (SPI command is 0xBB)
0xa: Enable address single & data quad mode, (SPI command is 0x6B) (Reserved)
0xb: Enable address quad & data quad mode, (SPI command is 0xEB)
(Reserved
*/
void mcuSetSpiMode( BYTE ucMode )
{
    BYTE ucValue;

    switch( ucMode )
    {
        case SPI_MODE_FR:
            ucValue = 0x01;
            break;

        case SPI_MODE_SADD:
            ucValue = 0x02;
            break;

        case SPI_MODE_DADD:
            ucValue = 0x03;
            break;

        case SPI_MODE_SAQD:
            ucValue = 0x0A;
            msWriteByteMask( REG_0205, _BIT3, _BIT3 );
            break;

        case SPI_MODE_QAQD:
            ucValue = 0x0B;
            msWriteByteMask( REG_0205, _BIT3, _BIT3 );
            break;

        case SPI_MODE_NORMAL:
        default:
            ucValue = 0x00;
    }

    msWriteByte( REG_08E4, ucValue );
#if ENABLE_DEBUG
    printData( "SPI Mode = %d ", ucMode );
#endif
}

//mapping to g_mcuPLLFreqTable[]

DWORD code g_mcuPLLFreqTable[] =
{
    CLOCK_4MHZ,            // 0
    CLOCK_12MHZ,           // 1
    CLOCK_216MHZ,          // 2
    CLOCK_MPLL_MHZ,        // 3,
    CLOCK_108MHZ,          // 4,
    CLOCK_86MHZ,           // 5,
    CLOCK_54MHZ,           // 6,
    CLOCK_0MHZ,           // 7,
    CLOCK_XTAL,            // 8
};

void mcuSetMcuSpeed( BYTE ucSpeedIdx )
{
    DWORD u32Freq;
    WORD u16Divider0;
    WORD u16Divider1;

    u32Freq = g_mcuPLLFreqTable[ucSpeedIdx];
   //u16Divider0 = 1024 - (( _SMOD * u32Freq + u32Freq ) / SERIAL_BAUD_RATE ) / 64;
    u16Divider0 = 1024 - ((_SMOD?(u32Freq*2):(u32Freq)) / SERIAL_BAUD_RATE ) / 64;
    u16Divider1 = 1024 - (( u32Freq ) / SERIAL_BAUD_RATE ) / 32;
    if ( ucSpeedIdx == IDX_MCU_CLK_XTAL )
        msWriteByteMask( REG_PM_BC, 0, _BIT0 );
    else
    {
        msWriteByteMask( REG_PM_BB, ucSpeedIdx, ( _BIT2 | _BIT1 | _BIT0 ) );
        msWriteByteMask( REG_PM_BC, _BIT0, _BIT0 );
    }

    // Scaler WDT
    msWriteByte(SC0_00, 0x00);
    msWriteByte(SC0_B2, (u32Freq*4)/CRYSTAL_CLOCK);

#if ENABLE_DEBUG
    ES = 0;
    IEN2 &= ~ES1;

    S0RELH = HIBYTE( u16Divider0 );
    S0RELL = LOBYTE( u16Divider0 );

    // uart1
    S1RELH = HIBYTE( u16Divider1 );
    S1RELL = LOBYTE( u16Divider1 );

  #if UART1
    IEN2 |= ES1;
  #else
    ES = 1;
  #endif

    printData( "MCU freq = %d MHz ", u32Freq / 1000 / 1000 );
#endif

#if !EXT_TIMER0_1MS
    // timer
    TR0 = 0;
    ET0 = 0;
    u32Freq = u32Freq / 1000;
    u16Divider0 = ( 65536 - ( u32Freq * INT_PERIOD + 6 ) / 12 );
    TH0 = g_ucTimer0_TH0 = HIBYTE( u16Divider0 );
    TL0 = g_ucTimer0_TL0 = LOBYTE( u16Divider0 );
    TR0 = 1;
    ET0 = 1;
#endif
}
//=========================================================
void mcuSetSystemSpeed(BYTE u8Mode)
{
#ifdef FPGA
    u8Mode = SPEED_XTAL_MODE;
#endif

    if (g_u8SystemSpeedMode!=u8Mode)
    { //MCU speed >= SPI speed
        switch(u8Mode)
        {
            case SPEED_4MHZ_MODE:
                mcuSetSpiSpeed(IDX_SPI_CLK_4MHZ); //spi speed down 1st
                mcuSetSpiMode( SPI_MODE_NORMAL );
                mcuSetMcuSpeed(IDX_MCU_CLK_4MHZ);
                g_bMcuPMClock = 1; // 120925 coding addition
                break;
            case SPEED_12MHZ_MODE:
                if (g_u8SystemSpeedMode>SPEED_12MHZ_MODE)
                {
                    mcuSetSpiSpeed( IDX_SPI_CLK_12MHZ );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetMcuSpeed( IDX_MCU_CLK_12MHZ);
                }
                else
                {
                    mcuSetMcuSpeed( IDX_MCU_CLK_12MHZ );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetSpiSpeed( IDX_SPI_CLK_12MHZ );
                }
                g_bMcuPMClock = 1; // 120925 coding addition
                break;
            case SPEED_XTAL_MODE:
                if (g_u8SystemSpeedMode>SPEED_XTAL_MODE)
                {
                    mcuSetSpiSpeed( IDX_SPI_CLK_XTAL );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetMcuSpeed( IDX_MCU_CLK_XTAL );
                }
                else
                {
                    mcuSetMcuSpeed( IDX_MCU_CLK_XTAL );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetSpiSpeed( IDX_SPI_CLK_XTAL );
                }
                g_bMcuPMClock = 1; // 120925 coding addition
                break;
            default: //normal
                mcuSetSpiMode( SPI_MODE );
                msWriteByte( REG_1EDC, 0x00 );
                msWriteByte( REG_1ED1, BIT2 );
                msWriteByte( REG_1ED2, 0x42 );
                ForceDelay1ms(10);
                mcuSetMcuSpeed( MCU_SPEED_INDEX );
                mcuSetSpiSpeed( SPI_SPEED_INDEX );
                g_bMcuPMClock = 0; // 120925 coding addition
              break;
        }
        g_u8SystemSpeedMode=u8Mode;
        //SetForceDelayLoop();
    }
}
 
