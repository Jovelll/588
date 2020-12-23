/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: MsFb.c
 [Date]:        11-Nov-2003
 [Comment]:
   Memory relative subroutines.
 [Reversion History]:
*******************************************************************************/
#define _MS_FB_D
#include "global.h"
#include "msEread.h"
#if CHIP_ID==CHIP_TSUMD // ENABLE_RTE
#define MSFB_DEBUG    1
#if ENABLE_DEBUG && MSFB_DEBUG
#define MSFB_printData(str, value)   printData(str, value)
#define MSFB_printMsg(str)           printMsg(str)
#else
#define MSFB_printData(str, value)
#define MSFB_printMsg(str)
#endif

void msSetupDDR3(void);
void msSetupDDR(void);

code BYTE tMemTestMode[] = { 0x07, 0x09, 0x05, 0x03, 0x0B };

code RegUnitType tInitMemory[] =
{
    {REG_1E02, 0x08},
    {REG_1E25, 0x00},
    {REG_1E28, 0x00},
    {REG_1100, 0x12},
    {REG_1101, 0x00},
    {REG_1102, 0xAA},
    {REG_1103, 0xAA},
    {REG_1108, 0x3F},
    {REG_110E, 0xA5},
    {REG_1138, 0x55},
    {REG_1139, 0x00},
    {REG_113A, 0x41},
    {REG_113B, 0x00},
    {REG_113C, 0x00},
    {REG_113D, 0x00},
    {REG_113E, 0x00},
    {REG_113F, 0x00},
    {REG_1133, 0x00},
    {REG_1136, 0x00},
    {REG_1202, 0x11},
    {REG_1203, 0x01},
    {REG_1204, 0x8B},
    {REG_1205, 0x00},
    {REG_1206, 0x30},
    {REG_1207, 0x02},
    {REG_1208, 0x55},
    {REG_1209, 0x0A},
    {REG_120A, 0x74},
    {REG_120B, 0x0E},
    {REG_120C, 0x41},
    {REG_120D, 0x53},
    {REG_120E, 0x12},
    {REG_120F, 0x10},
    {REG_1210, 0x32},
    {REG_1211, 0x00},
    {REG_1212, 0x04},
    {REG_1213, 0x40},
    {REG_1214, 0x00},
    {REG_1215, 0x80},
    {REG_1216, 0x00},
    {REG_1217, 0xC0},
    {REG_1200, 0x10},
    {REG_1201, 0x00},
    {REG_121E, 0x00},
    {REG_121F, 0x0C},
    {REG_121E, 0x01},
    {REG_121E, 0x00},

};


Bool msMemoryBist(void)
{
    BYTE ucMode;
    WORD wdlycnt;

    msWriteByte( REG_1246, 0xFE );  // MIU Mask
    msWriteByte( REG_1247, 0x7F );
    msWrite2Byte(REG_12E2, 0x0000);
    msWrite3Byte(REG_12E4, DRAM_SIZE>>2);  // unit: 4 bytes
    msWrite2Byte(REG_12E8, 0x5AA5);

    for(ucMode=0; ucMode<sizeof(tMemTestMode); ++ucMode)
    {
        msWriteByte(REG_12E0,0x00);
        msWriteByte(REG_12E1,0x00);
        msWriteByte(REG_12E0,tMemTestMode[ucMode]);
        wdlycnt=0;
        while(!(msReadByte(REG_12E1)&_BIT7))
        {
            if(wdlycnt++>=50)
            {
                MSFB_printData("time out, REG12E1=%x",msReadByte(REG_12E1));
                return FALSE;
            }
            ForceDelay1ms(1);
        }

        if ((msReadByte(REG_12E1) & 0x60) ) //fail
        {
            MSFB_printData("bist error, mode=%x",ucMode);
            MSFB_printData("REG_12E1=%x",msReadByte(REG_12E1));
            return FALSE;
        }
    }
    return TRUE;
}

#define DQSN_4(value)   (value>>4)
void msAutoDQSPhase()
{
    BYTE xdata u8Step;
    BYTE xdata u8Mode;
    BYTE xdata u8DqsSum;
    BYTE xdata u8DqsCnt;
    WORD xdata u16PhaseResult = 0;
    BYTE xdata u8Write;
    BYTE xdata u8DqsPhase;
#if ENABLE_DEBUG
    BYTE xdata strDqsPhase[17];
#endif

    msWrite2Byte(REG_12E2, 0x0000);     //SET_AUTO_DQS_START_ADDR );     //(4kbytes)/unit
    msWrite2Byte(REG_12E4, 0x1000);     //SET_AUTO_DQS_VERIFY_SIZE );     //(16 byte)/unit
    msWrite2Byte(REG_12E6, 0x0000);
    msWrite2Byte(REG_12E8, 0x5AA5);     //test data

    u8DqsSum = 0;
    u8DqsCnt = 0;

    u8DqsPhase = msReadByte(REG_113A);
#if ENABLE_DEBUG
    printMsg("[ AUTO DQS READ PHASE ]");
    printData("Def. Read Phase: %x", u8DqsPhase);
#endif

    // for ( u8Step = 0; u8Step < 64; u8Step++ )
    // DDR-16 DQS 0 only
    for ( u8Step = 16; u8Step < 32; u8Step++ )
    {
        if( DQSN_4(u8Step)==1 )
            u8Write = (((u8Step & 0x0F) << 4) | (u8DqsPhase&0x0F));
        else if( DQSN_4(u8Step)==0 )
            u8Write = ((u8Step & 0x0F) | (u8DqsPhase&0xF0));

        msWriteByte(REG_113A, u8Write);

        for ( u8Mode = 0 ; u8Mode < sizeof(tMemTestMode) ; ++u8Mode )
        {
            msWriteByte(REG_12E0, 0x00);
            msWriteByte(REG_12E0, tMemTestMode[u8Mode]);  // pattern from data

            while ( !(msReadByte(REG_12E1)&0x80) );

            if ( msReadByte(REG_12E1)&0x60 )
                break;
        }
        msWrite2Byte(REG_12E0, 0x0000 );

        if ( u8Mode ==  sizeof( tMemTestMode ) )
        {
            u8DqsSum += (u8Step & 0x0F);
            ++u8DqsCnt;
            u16PhaseResult |= (1 << (u8Step & 0x0F));
        }

        if ( (u8Step % 16 == 15) )
        {
            if (u8DqsCnt)
            {
                if(DQSN_4(u8Step)==1) u8DqsPhase=(u8DqsPhase & 0x0F);
                else if(DQSN_4(u8Step)==0) u8DqsPhase=(u8DqsPhase & 0xF0);
                u8DqsPhase |= ((u8DqsSum / u8DqsCnt) << (DQSN_4( u8Step ) * 4));
                u8DqsSum = 0;
                u8DqsCnt = 0;
            }

#if ENABLE_DEBUG
            for( u8Mode = 0 ; u8Mode < 16 ; u8Mode++ )
            {
                if( u16PhaseResult & (1 << u8Mode) )
                    strDqsPhase[u8Mode] = 'O';
                else
                    strDqsPhase[u8Mode] = '-';
            }
            strDqsPhase[u8Mode] = 0;
            printData("%s", (WORD)strDqsPhase);
#endif

            u16PhaseResult = 0;
        }
    }

    //refine the MIU DQS phase code flow for HW issue
    msWriteByte(REG_113A, u8DqsPhase );

#if ENABLE_DEBUG
    printData( "Auto Read Phase: %x", msRead2Byte(REG_113A) );
#endif
}

void msInitMemory( void )
{
    XDATA BYTE i,ucDDRInitCount;
    BOOL bDDR3InitState;
    BYTE ucEData;

    for( i = 0; i < sizeof( tInitMemory ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tInitMemory[i].u16Reg, tInitMemory[i].u8Value );

    ucEData = msEread_GetDataFromEfuse(0x134);
    if( ucEData & _BIT0 )
    {
        ucEData = msEread_GetDataFromEfuse(0x135);
        msWriteByteMask(REG_115C, ucEData, 0x03);
        msWriteByteMask(REG_115C, (ucEData<<2), 0x30);
        msWriteByteMask(REG_115D, (ucEData>>4), 0x03);
        msWriteByteMask(REG_115D, (ucEData>>2), 0x30);

        ucEData = msEread_GetDataFromEfuse(0x136);
        msWriteByteMask(REG_111A, (ucEData<<6), 0x40);
        msWriteByteMask(REG_111B, ucEData, 0x02);
        msWriteByteMask(REG_1136, (ucEData>>4), 0x0F);

        ucEData = msEread_GetDataFromEfuse(0x137);
        msWriteByteMask(REG_1138, ucEData, 0x0F);
        msWriteByteMask(REG_113A, ucEData, 0xF0);
    }

    msWriteByte( REG_1200, 0x00 );  // clear initial state
    msWriteByte( REG_1201, 0x00 );
    msWriteByte( REG_12E0, 0x00 );  // clear BIST state
    msWriteByteMask( REG_12C0, 0x00, 0x0F );    //remove protect function
    msWriteByte( REG_1246, 0xFE );  // MIU Mask
    msWriteByte( REG_1247, 0x7F );
    msWriteByteMask( REG_1231, BIT6 ,BIT6);
    msSetupDDR();

#if ENABLE_DDR_SSC
    ForceDelay1ms(1);
    msWriteByte( REG_1128, DDFSTEP );
    msWriteByte( REG_1129, BIT7|BIT6|(DDFSTEP>>8) );
    msWriteByte( REG_112A, DDFSPAN );
    msWriteByte( REG_112B, DDFSPAN>>8 );
#else
    msWriteByte( REG_1128, 0x00 );
    msWriteByte( REG_1129, BIT7 );
    msWriteByte( REG_112A, 0x00 );
    msWriteByte( REG_112B, 0x00 );
#endif

    msWrite2Byte(REG_1100,0x0012);
    msWriteByteMask( REG_121E, BIT0, BIT0 );
    ForceDelay1ms(1);
    msWriteByteMask( REG_121E, 0x00, BIT0 );
    ForceDelay1ms(1);
    msWrite2Byte(REG_1100,0x0102);
    msWriteByteMask( REG_121E, BIT3, BIT3 );
    ForceDelay1ms(1);                          //delay 500us
    msWriteByteMask( REG_1200, BIT3, BIT3 );
    ForceDelay1ms(1);                          //delay 500us
    msWriteByteMask( REG_1200, BIT2, BIT2 );
    ForceDelay1ms(1);                          //delay 500us
    msWriteByteMask( REG_1200, BIT1, BIT1 );
    ForceDelay1ms(1);                                  //delay 1us
    msWriteByteMask( REG_1200, BIT0, BIT0 );

    ucDDRInitCount = 0;

    msAutoDQSPhase();

    bDDR3InitState = FALSE;
    while(ucDDRInitCount++<10)
    {
        if(msMemoryBist())
        {
            bDDR3InitState = TRUE;
            MSFB_printMsg("=====>   Memory Init PASS!!  <======\r\n");
            break;
        }
    }

    if(bDDR3InitState == FALSE)
    {
    #if ENABLE_DEBUG
        MSFB_printMsg("=====>   Memory Init Fail!!  <======\r\n");
        ForceDelay1ms(3000);
        ForceDelay1ms(3000);
        ForceDelay1ms(3000);
    #endif
    }

    //remove mask
    msWriteByteMask( REG_1231, 0 ,BIT6);
    msWriteByte( REG_12E0, 0x00 );
    msWriteByte( REG_1246, 0x00 );  // MIU UnMask
    msWriteByte( REG_1247, 0x00 );

    if(i!=i) // disable uncall warning message
        msMiuProtectCtrl(0,0,0,0,0);}


void msSetupDDR()
{
    msWriteByteMask(REG_1120, DDRIP<<4,BIT6|BIT5|BIT4);
    msWriteByteMask(REG_1134,0x00,0x0F);
    msWriteByteMask(REG_1135, LOOP_DIV2,0x0F);
    msWriteByteMask(REG_1137,(GetLog(LOOP_DIV1)<<6)|(GetLog(IN_DIV)<<4),0xF0);
    msWrite3Byte(REG_1130,DDFSET);
}

void msMiuProtectCtrl(BYTE ucGroup, BOOL bCtrl, WORD ucID, DWORD wAddrStart, DWORD wAddrEnd)
{
    xdata BYTE ucIDNum ,i,j;
    xdata DWORD ucProtectID;

    ucIDNum =(ucGroup == MIU_PROTECT_0) ? 4:2;
    ucProtectID = 0;
    j = 0;

    for(i=0;i<16;i++)
    {
        if(ucID&(1<<i))
        {
            ucProtectID |=((DWORD)i<<(j*8));
            j++;
        }
        if(j==ucIDNum)
            break;
    }

    switch(ucGroup)
    {
        case MIU_PROTECT_0:
            msWrite2Byte(REG_12C2,ucProtectID);
            msWrite2Byte(REG_12C4,ucProtectID>>16);
            msWrite2Byte(REG_12C6,wAddrStart/0x800);
            msWrite2Byte(REG_12C8,wAddrEnd/0x800-1);
            msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_0);
            break;
        case MIU_PROTECT_1:
            msWrite2Byte(REG_12CA,ucProtectID);
            msWrite2Byte(REG_12CC,wAddrStart/0x800);
            msWrite2Byte(REG_12CE,wAddrEnd/0x800-1);
            msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_1);
            break;
        case MIU_PROTECT_2:
            msWrite2Byte(REG_12D0,ucProtectID);
            msWrite2Byte(REG_12D2,wAddrStart/0x800);
            msWrite2Byte(REG_12D4,wAddrEnd/0x800-1);
            msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_2);
            break;
        case MIU_PROTECT_3:
            msWrite2Byte(REG_12D6,ucProtectID);
            msWrite2Byte(REG_12D8,wAddrStart/0x800);
            msWrite2Byte(REG_12DA,wAddrEnd/0x800-1);
            msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_3);
            break;
    }
}

#endif
