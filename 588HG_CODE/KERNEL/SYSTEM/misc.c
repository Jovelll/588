#include <intrins.h>
#include "types.h"
#include "board.h"
#include "global.h"
#if (MS_PM)
#include "ms_pm.h"
#endif
//#define DelayPeriod ((WORD)CPU_CLOCK_MHZ*78/12)
void Delay4us( void )
{
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
#if (MS_PM)
    if( g_bMcuPMClock )
    {
        BYTE i;
        for( i = 0; i < PM_DELAY4US_LOOP; i++ )
        {
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
        }

    }
#endif
}
void ForceDelay1ms( WORD msNums )
{
    if( !msNums )
        return;

    u16DelayCounter = msNums;
    bDelayFlag = 1;
    while( bDelayFlag );
}

void Delay1ms( WORD msNums )
{
    if( !msNums )
        return;

    u16DelayCounter = msNums;
    bDelayFlag = 1;
    while( bDelayFlag )
    {
        if( InputTimingChangeFlag )
        {
            u16DelayCounter = 0;
            bDelayFlag = 0;
            break;
        }
    };
}


BYTE GetVSyncTime( void )
{
    WORD freq;

#if ENABLE_FREESYNC
    if(IS_DP_FREESYNC())
    {
        WORD u16PixClk, u16Htt;

        u16PixClk = msAPI_combo_IPGetPixelClk();
        u16Htt = msAPI_combo_IPGetGetHTotal();
        if((u16Htt!=0)&&(SrcVTotal!=0))
        {
            freq = ((DWORD)u16PixClk * 100 + u16Htt/2)/u16Htt;
        }
    }
    else
#endif
    {
        freq = HFreq( SrcHPeriod );//(( DWORD )MST_CLOCK_MHZ * 10 + SrcHPeriod / 2 ) / SrcHPeriod; //get hfreq round 5
    }
    freq = (( WORD )SrcVTotal * 10 + ( freq / 2 ) ) / freq;
    return ( BYTE )freq;
}


