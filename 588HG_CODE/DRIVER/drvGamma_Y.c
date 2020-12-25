
///////////////////////////////////////
// DRVGAMMA_Y VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "Panel_GAMMATBL.h"

#if USE_NORMAL_GAMMA_TBL
void drvGammaLoad320ByteTbl(BYTE u8Window, BYTE channel, BYTE *tblPtr)
{
    WORD i;

    u8Window=u8Window;

    msWriteByteMask(SC0_40, BIT1|channel,BIT2|BIT3|BIT1);
    for (i=0; i<320; i++) // 256 + 256/4 =320
    { 
        msWriteByte(SC0_41, *(tblPtr+i));
    }
    msWriteByteMask(SC0_40, 0, BIT1|BIT2|BIT3);
}
#else
void drvGammaLoadCompressedTbl(BYTE u8Window, BYTE channel, BYTE* ucTab1, BYTE* ucTab2)
{
    WORD u16CurVal = 0;
    WORD u16OutCnt = 0;
    BYTE u8Inc, u8Len;
    BYTE i ;

    u8Window=u8Window;

    msWriteByteMask(SC0_40, channel | BIT1, BIT3 | BIT2 | BIT1);

    for( i = 0; u16OutCnt < 256; ++ i )
    {
        u8Inc = ucTab1[i]>>5;
        u8Len = ( ucTab1[i] & 0x1f ) + 1;
        while( u8Len > 0 )
        {
            u16CurVal += u8Inc;
            if( 0 == u16OutCnt % 4 )
            {
                msWriteByte(SC0_41, ucTab2[u16OutCnt/4]);
            }
            msWriteByte(SC0_41, u16CurVal);
            ++u16OutCnt;
            --u8Len;
        }
    }

    msWriteByteMask(SC0_40, 0, BIT1|BIT2|BIT3);
    

}
#endif

void drvGammaLoadTbl( BYTE GamaMode )
{
    BYTE u8TgtChannel = 0;
    
    while( u8TgtChannel < 3 )
    {
#if USE_NORMAL_GAMMA_TBL
            BYTE* pu8Tab1;
            pu8Tab1 = tAllGammaTab[GamaMode][u8TgtChannel];         
            drvGammaLoad320ByteTbl(MAIN_WINDOW, (u8TgtChannel<<2), pu8Tab1);         
#else
            BYTE* pu8Tab1;
            BYTE* pu8Tab2;

            pu8Tab1 = tAllGammaTab[GamaMode][0+2*u8TgtChannel];
            pu8Tab2 = tAllGammaTab[GamaMode][1+2*u8TgtChannel];

            // Write data to gamma channel
            drvGammaLoadCompressedTbl(MAIN_WINDOW,(u8TgtChannel<<2), pu8Tab1, pu8Tab2 );
#endif
        u8TgtChannel += 1;
    }
}

/////////////////////////////////////////////////////////////////
#ifndef GAMMA_DITHER
#define GAMMA_DITHER        0x00
#endif
void drvGammaOnOff(BYTE u8Switch, BYTE u8Window)
{
    if ( u8Window == MAIN_WINDOW )
    {
        if (u8Switch == _ENABLE)
            msWriteByteMask(SC0_40, BIT0, BIT0);
        else
            msWriteByteMask(SC0_40, 0x00,BIT0);
    }
    else // Sub-Window
    {
        if (u8Switch == _ENABLE)
            msWriteByteMask(SC3_40, BIT0, BIT0);
        else
            msWriteByteMask(SC3_40, 0x00 ,BIT0);
    }
}
