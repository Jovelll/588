
///////////////////////////////////////
// DRVGAMMA_U VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "Panel_GAMMATBL.h"
#if USE_NORMAL_GAMMA_TBL
void drvGammaLoad320ByteTbl(BYTE u8Window, BYTE channel, BYTE *tblPtr)
{
    WORD i,u16Data;
    BYTE j,u8Bit0_1;
    u8Window=(u8Window==MAIN_WINDOW?0:BIT4);
    msWriteByteMask( SC8_22, u8Window|BIT1|BIT0,BIT1|BIT0|BIT4);
    msWriteByteMask( SC8_22, channel , BIT3 | BIT2 );
    msWrite2Byte( SC8_20, 0 );
	
    for (i=0; i<320; i+=5) // 256 + 256/4 =320
    {
        u8Bit0_1=*(tblPtr+i);
        for (j=0;j<4;j++)
        {
            u16Data=0;
            u16Data=*(tblPtr+j+1+i);  
            u16Data=(u16Data<<2)+((u8Bit0_1>>(j<<1))&0x03);  
            msWrite2Byte(SC8_24, u16Data);
        }      
    }
    msWriteByteMask( SC8_22, 0, BIT1 | BIT0 );
}
#else
void drvGammaLoadCompressedTbl(BYTE u8Window, BYTE channel, BYTE* ucTab1, BYTE* ucTab2)
{
    WORD u16CurVal = 0;
    WORD u16OutCnt = 0;
    BYTE u8Inc, u8Len;
    BYTE i ;
    BYTE u8lsb;

    u8Window=(u8Window==MAIN_WINDOW?0:BIT4);

    msWriteByteMask( SC8_22, 0x03|u8Window,BIT4|BIT1|BIT0);
    msWriteByteMask( SC8_22, channel , BIT3 | BIT2 );
    msWrite2Byte( SC8_20, 0 );
	
    for( i = 0; u16OutCnt < 256; i++ )
    {
        u8Inc = ucTab1[i] >> 5;
        u8Len = ( ucTab1[i] & 0x1f ) + 1;

		//printData(" @@@@@ u8Inc===%d",u8Inc);
		//printData(" %%%%% u8Len===%d",u8Len);
			
		
        while( u8Len > 0 )
        {

		
            u16CurVal += u8Inc;
            if( 0 == u16OutCnt % 4 )
                u8lsb = ucTab2[u16OutCnt / 4];
            msWrite2Byte( SC8_24, (u16CurVal << 2) | (u8lsb & (BIT1 | BIT0)) );
			
			//printData(" !!!!! u8lsb===%d",u8lsb);
			//printData(" ##### u16CurVal===%d",u16CurVal);
			
            u8lsb >>= 2;
            u16OutCnt++;
            u8Len--;
        }
    }
    msWriteByteMask( SC8_22, 0, BIT1 | BIT0 );

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
    if( MAIN_WINDOW == u8Window )
    {
        if(u8Switch)
            msWriteByteMask(SC8_3F ,BIT7 , BIT7 );
        else
            msWriteByteMask(SC8_3F ,0 , BIT7 );
    }
    else //
    {
        if(u8Switch)
            msWriteByteMask(SC8_3F ,BIT6 , BIT6 );
        else
            msWriteByteMask(SC8_3F ,0 , BIT6 );
    }
}

