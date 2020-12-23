
///////////////////////////////////////
// DRVGAMMA_V VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "Panel_GAMMATBL.h"

xdata BYTE u8FormatSel =0, u8CurGmaDiff[4]= {0,0,0,0};
WORD GmaRefDataCompress(BYTE u8GmaL , WORD u16RefData)
{
    BYTE u8GammaLelFormat;
    u8GmaL<<=1;
    //u8GammaLelFormat = scReadByte(SC8_3E) & (3<<(u8GmaL));
    u8GammaLelFormat = msReadByte(SC8_3E) & (3<<(u8GmaL));
    if(u8GammaLelFormat== (3<<(u8GmaL)))
    {
        if(u8FormatSel)
        {
            if(u16RefData < 960)
                u16RefData = 0;
            else
                u16RefData -= 960;
        }
        u16RefData = u16RefData <<8;
    }
    else if(u8GammaLelFormat == (2<<(u8GmaL)))
    {
        if(u8FormatSel)
        {
            if(u16RefData < 768)
                u16RefData = 0;
            else
                u16RefData -= 768;
         }
        u16RefData = u16RefData <<6;
    }
    else if(u8GammaLelFormat == (1<<(u8GmaL)))
    {
        if(u8FormatSel)
        {
            if(u16RefData < 512)
                u16RefData = 0;
            else
                u16RefData -= 512;
        }
        u16RefData = u16RefData <<5;
    }
    else
        u16RefData = u16RefData <<4;

    return u16RefData;
}

void GmaFormatSelectRGBChannel(BYTE GamIndex, BYTE u8PreDiff)
{
    if( ((GamIndex / 2) == 0 && (u8FormatSel == 0) ) ||
        ((GamIndex / 2) == 127 && (u8FormatSel == 1) ) ) // L0~1 and L254~255
    {
        if(u8PreDiff > u8CurGmaDiff[0])
            u8CurGmaDiff[0] = u8PreDiff;
    }
    else if( ((GamIndex / 2) == 1 && (u8FormatSel == 0) ) ||
        ((GamIndex / 2) == 126 && (u8FormatSel == 1) ) )// L2~3 and L252~253
    {
        if(u8PreDiff > u8CurGmaDiff[1])
            u8CurGmaDiff[1] = u8PreDiff;
    }
    else if((((GamIndex / 2) == 2 ||(GamIndex / 2) == 3 )&& (u8FormatSel == 0)) ||
        (((GamIndex / 2) == 125 ||(GamIndex / 2) == 124 )&& (u8FormatSel == 1)))// L4 ~ 7 and L248~251
    {
        if(u8PreDiff > u8CurGmaDiff[2])
            u8CurGmaDiff[2] = u8PreDiff;
    }
}

WORD GmaCompressLargerThanOne(BYTE u8GmaIndex, WORD u16Ref)
{

    if((((u8GmaIndex / 2) == 125 ||(u8GmaIndex / 2) == 124) &&(u8FormatSel == 1) )
        ||(((u8GmaIndex / 2) == 2 ||(u8GmaIndex / 2) == 3) && (u8FormatSel == 0))  ) // for L248~251
        return GmaRefDataCompress(2,u16Ref);
    else if(((u8GmaIndex / 2) == 126 &&(u8FormatSel == 1))
        || ((u8GmaIndex / 2) == 1 &&(u8FormatSel == 0)) )// for L252 ~ 253
        return GmaRefDataCompress(1,u16Ref);
    else if(((u8GmaIndex / 2) == 127 && u8FormatSel == 1)
        ||((u8GmaIndex / 2) == 0 && u8FormatSel == 0 )) // for L254 ~ 255
        return GmaRefDataCompress(0,u16Ref);
    else
        return u16Ref<<4;
}


///////////////////////////////
#if USE_NORMAL_GAMMA_TBL
void drvGammaLoad320ByteTbl(BYTE u8Window, BYTE channel, BYTE *tblPtr, BYTE u8Status )
{
    WORD i;
    BYTE u8Bit0_1;
    xdata BYTE u8DeltaVal, u8TableIndex = 0 ;
    xdata WORD u16RefVal , u16OddlevelGma, u16EvenlevelGma;

    u8Window=(u8Window==MAIN_WINDOW?0:BIT4);
    msWriteByte(SC8_20, 0x00);
    
    u8TableIndex = 0;
    msWriteByteMask( SC8_22, channel , BIT3 | BIT2 );
    msWriteByteMask( SC8_22, BIT0  , BIT0 );
    for (i=0; i<320; i+=5)
    {
        u8Bit0_1=*(tblPtr+i);  
        
        for (u8TableIndex=0;u8TableIndex<4;u8TableIndex++)
        {
            if((u8TableIndex%2)==0)
                {
                    u16EvenlevelGma=*(tblPtr+u8TableIndex+1+i);              
                    u16EvenlevelGma=(u16EvenlevelGma<<2)+((u8Bit0_1>>(u8TableIndex<<1))&0x03);
                }
            else
                {
                    u16OddlevelGma=*(tblPtr+u8TableIndex+1+i);              
                    u16OddlevelGma=(u16OddlevelGma<<2)+((u8Bit0_1>>(u8TableIndex<<1))&0x03);
                }
            
            if((u8TableIndex% 2) == 1)
            {  
                u16RefVal = u16EvenlevelGma;
                if(u16OddlevelGma > u16EvenlevelGma)
                    u8DeltaVal = u16OddlevelGma - u16EvenlevelGma;
                else
                    u8DeltaVal = u16EvenlevelGma - u16OddlevelGma;              
           
                if(u8Status == 0)
                    {
                        if((u8TableIndex / 2) == 0 && (channel ==0)) // Decide Gamma Coefficient larger one whether or not
                        {
                            if(u8DeltaVal >15)
                            u8FormatSel = 0;
                            else
                            u8FormatSel = BIT0;
                            msWriteByteMask( SC8_3F, u8FormatSel  , BIT0 ); // format select for L248 to L255
                        }
                        // Decide gamma format
                        GmaFormatSelectRGBChannel((u8TableIndex+i) , u8DeltaVal);
                    }
                else if(u8Status == 1)
                    {
                        u16RefVal = GmaCompressLargerThanOne((u8TableIndex+i), u16RefVal);
                        msWrite2Byte( SC8_24, u16RefVal | u8DeltaVal );
                    }
             }
        }                
    }
    if(u8Status == 0)
    {
#define u8GammaLelFormat    u8DeltaVal
        u8GammaLelFormat=0;
        for( i = 0 ; i < 3 ; i++)
        {
            if(u8CurGmaDiff[i] < 16)
                ;
            else if(u8CurGmaDiff[i] < 32)
                u8GammaLelFormat |= (BIT0 << (i<<1));
            else if(u8CurGmaDiff[i] < 64)
                u8GammaLelFormat |= (BIT1 << (i<<1));
            else //if(u8CurGmaDiff[i] < 256)
                u8GammaLelFormat |= ((BIT1|BIT0) << (i<<1)); //_scWriteByteMask( SC8_3E, (BIT1|BIT0) << (i*2), (BIT1|BIT0)<< (i*2) );//
        }
        msWriteByte( SC8_3E, u8GammaLelFormat);
#undef  u8GammaLelFormat
    }

    msWriteByteMask( SC8_22, 0, BIT1|BIT0 );
}
#else

void drvGammaLoadCompressedTbl(BYTE u8Window, BYTE channel, BYTE* ucTab1, BYTE* ucTab2, BYTE u8Status )
{
    WORD u16CurVal = 0;
    WORD u16OutCnt = 0;
    BYTE u8Inc, u8Len;
    BYTE i ;
    BYTE u8lsb;

    xdata BYTE u8DeltaVal, u8TableIndex = 0 ;
    xdata WORD u16RefVal , u16OddlevelGma, u16EvenlevelGma;

    u8Window=(u8Window==MAIN_WINDOW?0:BIT4);
    msWriteByte(SC8_20, 0x00);
    
    u8TableIndex = 0;
    msWriteByteMask( SC8_22, channel , BIT3 | BIT2 );
    msWriteByteMask( SC8_22, BIT0  , BIT0 );
    for( i = 0; u16OutCnt < 256; i++ )
    {
        u8Inc = ucTab1[i] >> 5;
        u8Len = ( ucTab1[i] & 0x1f ) + 1;

        while( u8Len > 0 )
        {
            u16CurVal += u8Inc;
            if( 0 == u16OutCnt % 4 )
                u8lsb = ucTab2[u16OutCnt / 4];

            if((u8TableIndex % 2) == 0)
                u16EvenlevelGma = (u16CurVal << 2) | (u8lsb & (BIT1 | BIT0)) ;
            else
                u16OddlevelGma = (u16CurVal << 2) | (u8lsb & (BIT1 | BIT0)) ;
            if((u8TableIndex % 2) == 1)
            {
                u16RefVal = u16EvenlevelGma;
                if(u16OddlevelGma > u16EvenlevelGma)
                    u8DeltaVal = u16OddlevelGma - u16EvenlevelGma;
                else
                    u8DeltaVal = u16EvenlevelGma - u16OddlevelGma;
                if(u8Status == 0)
                {
                    if((u8TableIndex / 2) == 0 && (channel ==0)) // Decide Gamma Coefficient larger one whether or not
                    {
                        if(u8DeltaVal >15)
                            u8FormatSel = 0;
                        else
                            u8FormatSel = BIT0;
                        msWriteByteMask( SC8_3F, u8FormatSel  , BIT0 ); // format select for L248 to L255
                    }
                    // Decide gamma format
                    GmaFormatSelectRGBChannel(u8TableIndex , u8DeltaVal);
                }
                else if(u8Status == 1)
                {
                   u16RefVal = GmaCompressLargerThanOne(u8TableIndex, u16RefVal);
                   msWrite2Byte( SC8_24, u16RefVal | u8DeltaVal );
                }
            }
            u8TableIndex ++ ;
            u8lsb >>= 2;
            u16OutCnt++;
            u8Len--;
        }
    }
    if(u8Status == 0)
    {
#define u8GammaLelFormat    u8DeltaVal
        u8GammaLelFormat=0;
        for( i = 0 ; i < 3 ; i++)
    { 
            if(u8CurGmaDiff[i] < 16)
                ;
            else if(u8CurGmaDiff[i] < 32)
                u8GammaLelFormat |= (BIT0 << (i<<1));
            else if(u8CurGmaDiff[i] < 64)
                u8GammaLelFormat |= (BIT1 << (i<<1));
            else //if(u8CurGmaDiff[i] < 256)
                u8GammaLelFormat |= ((BIT1|BIT0) << (i<<1)); //_scWriteByteMask( SC8_3E, (BIT1|BIT0) << (i*2), (BIT1|BIT0)<< (i*2) );//
    }
        msWriteByte( SC8_3E, u8GammaLelFormat);
#undef  u8GammaLelFormat
    }

    msWriteByteMask( SC8_22, 0, BIT1|BIT0 );
}

#endif


void drvGammaLoadTbl( BYTE GamaMode )
{
    BYTE u8TgtChannel = 0;
    BYTE  u8GmaProcessStatus = 0;
    
    while( u8GmaProcessStatus < 2)
    {
        while( u8TgtChannel < 3 )
        {
#if USE_NORMAL_GAMMA_TBL
            BYTE* pu8Tab1;
            pu8Tab1 = tAllGammaTab[GamaMode][u8TgtChannel];
            drvGammaLoad320ByteTbl(MAIN_WINDOW, (u8TgtChannel<<2), pu8Tab1,u8GmaProcessStatus);         
#else
            BYTE* pu8Tab1;
            BYTE* pu8Tab2;

            pu8Tab1 = tAllGammaTab[GamaMode][0+2*u8TgtChannel];
            pu8Tab2 = tAllGammaTab[GamaMode][1+2*u8TgtChannel];

            // Write data to gamma channel
            drvGammaLoadCompressedTbl(MAIN_WINDOW,(u8TgtChannel<<2), pu8Tab1, pu8Tab2 ,u8GmaProcessStatus);
#endif
            u8TgtChannel += 1;
        }
        u8TgtChannel=0;
        u8GmaProcessStatus+=1;
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
