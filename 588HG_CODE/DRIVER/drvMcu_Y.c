
///////////////////////////////////////
// DRVMCU_Y VERSION: V02
////////////////////////////////////////
#include "global.h"

void Init_WDT( BYTE bEnable )
{
    msWriteByteMask( REG_3C66, BIT2,BIT2);  //clear watchdog reset flag
    msWriteByteMask( REG_3C66, 0x00,BIT2);
    if( bEnable )
    {

#if( ENABLE_WATCH_DOG )
        SetWDTClk(CLK_LIVE_XTAL);
#endif  // end of #if( ENABLE_WATCH_DOG )
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
  CLOCK_36MHZ,       // 0
  CLOCK_43MHZ,       // 1
  CLOCK_54MHZ,       // 2
  CLOCK_86MHZ,       // 3
  CLOCK_27MHZ,       // 4
  CRYSTAL_CLOCK,     // 5
  CLOCK_4MHZ,        // 6
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
    BOOL bPLLEnable;
    if( ucIndex >= IDX_SPI_CLK_XTAL )
        bPLLEnable = 0;
    else
        bPLLEnable = 1;

    //msWriteByteMask(REG_1B4C, 0x3f, 0x3f);      // set spi pad high driving

    if( bPLLEnable )
    {
        msWriteByteMask( REG_08E0, ucIndex<<4,  _BIT7|_BIT6|_BIT5|_BIT4);
        msWriteByteMask( REG_08E0, _BIT3, _BIT3 | _BIT2 );
    }
    else
    {
        if( ucIndex == IDX_SPI_CLK_4MHZ )
            msWriteByteMask( REG_08E0, _BIT2, _BIT3 | _BIT2 );
        else
            msWriteByteMask( REG_08E0, 0x00, _BIT3 | _BIT2 );
    }
  #if ENABLE_DEBUG
    printData( "SPI freq = %d MHz ", g_mcuSpiFreqTable[ucIndex] / 1000 / 1000 );
  #endif
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
            msWriteByteMask(REG_1B05, _BIT3, _BIT3);
            break;

        case SPI_MODE_QAQD:
            ucValue = 0x0B;
            msWriteByteMask( REG_0223, _BIT6, _BIT6 );
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

DWORD code g_mcuPLLFreqTable[] =
{
    CLOCK_MPLL_MHZ,        // 0,
    CLOCK_0MHZ,            // 1,
    CLOCK_144MHZ,          // 2,
    CLOCK_108MHZ,          // 3,
    CLOCK_86MHZ,           // 4,
    CLOCK_54MHZ,           // 5,   mem_clock
    CLOCK_27MHZ,           // 6,   mem_clock/2
    CLOCK_0MHZ,            // 7
    CLOCK_XTAL,            // 8
    CLOCK_4MHZ,            // 9
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
    if( ucSpeedIdx == IDX_MCU_CLK_4MHZ )
        msWriteByteMask( REG_PM_BC, _BIT4, _BIT5 | _BIT4 );
    else if ( ucSpeedIdx == IDX_MCU_CLK_XTAL )
        msWriteByteMask( REG_PM_BC, 0x00, _BIT5 | _BIT4 );
    else
    {
        msWriteByteMask( REG_PM_BB, ucSpeedIdx, ( _BIT2 | _BIT1 | _BIT0 ) );
        msWriteByteMask( REG_PM_BC, _BIT5, _BIT5 | _BIT4 );
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

    if (g_u8SystemSpeedMode!=u8Mode)
    { //MCU speed >= SPI speed
        switch(u8Mode)
        {
            case SPEED_4MHZ_MODE:
                mcuSetSpiSpeed(IDX_SPI_CLK_4MHZ); //spi speed down 1st
                mcuSetSpiMode( SPI_MODE_NORMAL );
                mcuSetMcuSpeed(IDX_MCU_CLK_4MHZ);
                g_bMcuPMClock = 1; // 120618 coding addition
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
                g_bMcuPMClock = 1; // 120618 coding addition
                break;
            default: //normal
                mcuSetSpiMode( SPI_MODE );
                msWriteByte( REG_1EDC, 0x00 );
                msWriteByte( REG_1ED1, BIT2 );
                msWriteByte( REG_1ED2, 0x0F );
                ForceDelay1ms(10);
                mcuSetMcuSpeed( MCU_SPEED_INDEX );
                mcuSetSpiSpeed( SPI_SPEED_INDEX );
                g_bMcuPMClock = 0; // 120618 coding addition
              break;
        }
        g_u8SystemSpeedMode=u8Mode;
        //SetForceDelayLoop();
    }
}

