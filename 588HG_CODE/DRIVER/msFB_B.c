/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: MsFb.c
 [Date]:        11-Nov-2003
 [Comment]:
   Memory relative subroutines.
 [Reversion History]:
*******************************************************************************/
#define _MSFB_C_

#include "global.h"


#if ENABLE_RTE
code BYTE tMemTestMode[] = { 0x07, 0x09, 0x05, 0x03, 0x0B };
void msSetupDDR3(void);
code RegUnitType tInitMemory[] =
{
    {REG_1E25, 0x00},
    {REG_1133, 0x00},
    {REG_1154, 0x00},
    {REG_1160, 0x70},
    {REG_1161, 0x23},
    {REG_1168, 0x20},
    {REG_1169, 0x00},
    {REG_116A, 0x20},
    {REG_116B, 0x00},
    {REG_116E, 0x88},
    {REG_116F, 0x00},
    {REG_1170, 0x08},
    {REG_1172, 0x05},
    {REG_1173, 0x58},
    {REG_110E, 0xA5},

    {REG_1100, 0x01},
    {REG_1101, 0x02},
    {REG_1104, 0x80},
    {REG_1108, 0x3F},
    {REG_1111, 0x04},
    {REG_1120, DDRIP<<4},
    {REG_1138, MEM_DQ_PHASE_CTRL_1},
    {REG_1148, MEM_DQ_PHASE_CTRL_2},
    {REG_1139, MEM_DQS_PHASE_CTRL_1},
    {REG_1149, MEM_DQS_PHASE_CTRL_2},
    {REG_113C, 0x66},
    {REG_114A, 0x00},
    {REG_114B, 0x00},
    {REG_115C, 0xDD},  //driving
    {REG_115D, 0x33},  //driving
    {REG_115E, 0xDD},  //driving
    {REG_115F, 0x33},  //driving

    {REG_113A, 0x00},
    {REG_113B, 0x60},
    {REG_1136, MEM_CLK_PHASE_CTRL},

    {REG_1202, RAM_TYPE},
    {REG_1203, 0x08 | (RAM_TYPE&0x03)},
    {REG_1204, 0x06},
    {REG_1205, 0xC4},
    {REG_1228, 0xB0},
    {REG_1229, 0x00},
    {REG_122C, 0x99},  // Address Balance Mode
    {REG_122D, 0x61},
};

Bool msMemoryBist(void)
{
    BYTE ucMode;
    WORD wdlycnt;

    msWrite2Byte(REG_12E2, 0x0000);
    msWrite2Byte(REG_12E4, 0x100);
    msWriteByte(REG_12E6, 0x00);
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
#if ENABLE_DEBUG
                printData("time out, REG12E1=%x",msReadByte(REG_12E1));
#endif
                return FALSE;
            }
            ForceDelay1ms(1);
        }

        if ((msReadByte(REG_12E1) & 0x60) ) //fail
        {
#if ENABLE_DEBUG
            printData("bist error, mode=%x",ucMode);
#endif
            return FALSE;
        }
    }

    return TRUE;
}

void msSetupDDR3( void )
{
    BYTE reg_val_1,reg_val_2;

    msWriteByteMask(REG_1134,(POST_DIV>>2),0x03);
    msWriteByteMask(REG_1135, LOOP_DIV2,0x0F);
    msWriteByteMask(REG_1137,(GetLog(LOOP_DIV1)<<6)|(GetLog(IN_DIV)<<4),0xF0);
    msWriteByte(REG_1130,DDFSET);
    msWriteByte(REG_1131,DDFSET>>8);
    msWriteByte(REG_1132,DDFSET>>16);

    msWriteByte(REG_1206, (DWORD)tREFI*MIU_ARBITOR_CLK/1000/16);
    msWriteByte( REG_1207, tCKE );
    reg_val_1 = tRP*MEMORY_CLOCK_MHZ/1000 + 1;
    reg_val_2 = tRCD*MEMORY_CLOCK_MHZ/1000 + 1;
    msWriteByte( REG_1208, ((reg_val_1&0x0F)<<4)|(reg_val_2&0x0F));
    msWriteByte( REG_1209, ((reg_val_1&0x10)<<7)|((reg_val_2&0x10)<<6) );
    reg_val_1 = tRAS*MEMORY_CLOCK_MHZ/1000 + 1;
    msWriteByteMask( REG_1209,reg_val_1,0x3F);
    reg_val_1 = tRTP*MEMORY_CLOCK_MHZ/1000 + 1;
    reg_val_2 = tRRD*MEMORY_CLOCK_MHZ/1000 + 1;
    msWriteByte( REG_120A, (reg_val_1<<4) | (reg_val_2));
    reg_val_1 = tRC*MEMORY_CLOCK_MHZ/1000 + 1;
    msWriteByte( REG_120B, reg_val_1 );
    reg_val_1 = tWR*MEMORY_CLOCK_MHZ/1000 + 1;
    reg_val_2 = tWL*MEMORY_CLOCK_MHZ/1000 + 1;
    msWriteByte( REG_120C, (reg_val_1<<4) | (reg_val_2));
    reg_val_1 = tWTR*MEMORY_CLOCK_MHZ/1000 + 1;
    reg_val_2 = tRTW*MEMORY_CLOCK_MHZ/1000 + 1;
    msWriteByte( REG_120D, (reg_val_2<<4)+reg_val_1 );
    reg_val_1 = tRFC*MEMORY_CLOCK_MHZ/1000 + 1;
    msWriteByte( REG_120E, reg_val_1 );
    reg_val_1 = tWR*MEMORY_CLOCK_MHZ/1000 + 1;
    reg_val_1 &=0x10;
    msWriteByte( REG_120F, (reg_val_1<<7)| (tCCD<<4));

    msWrite2Byte( REG_1210, MR0 );
    msWrite2Byte( REG_1212, MR1 );
    msWrite2Byte( REG_1214, MR2 );
    msWrite2Byte( REG_1216, MR3 );
}

void msInitMemory( void )
{
    BYTE ucDDR3InitCount,i;
    BOOL bDDR3InitState = FALSE;

#if ENABLE_DEBUG
    printMsg("=====>    msInitMemory!!  <======\r\n");
#endif

    for( i = 0; i < sizeof( tInitMemory ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tInitMemory[i].u16Reg, tInitMemory[i].u8Value );

    msWriteByte( REG_1200, 0x00 );  // clear initial state
    msWriteByte( REG_1201, 0x00 );
    msWriteByte( REG_12E0, 0x00 );  // clear BIST state
    msWriteByte( REG_1246, 0xF6 );  // MIU Mask
    msWriteByte( REG_1247, 0xFF );
    msWriteByteMask( REG_12C0, 0x00, 0x0F );    //remove protect function
    msSetupDDR3();

#if ENABLE_DDR3_SSC
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

    //Initial MIU
    ForceDelay1ms( 1 );                          //delay 1ms
    msWriteByteMask( REG_121E, 0x01, 0x01 );
    msWriteByteMask( REG_121E, 0x00, 0x01 );
    ForceDelay1ms( 1 );                          //delay 200us
    msWriteByteMask( REG_1200, 0x10, 0x10 );
    ForceDelay1ms( 1 );                          //delay 500us
    msWriteByteMask( REG_1200, 0x08, 0x08 );
    ForceDelay1ms( 1 );                          //delay 500us
    msWriteByteMask( REG_1200, 0x04, 0x04 );
    ForceDelay1ms( 1 );                          //delay 500us
    msWriteByteMask( REG_1200, 0x02, 0x02 );
    ForceDelay1ms( 1 );                                  //delay 1us
    msWriteByteMask( REG_1200, 0x01, 0x01 );

    ucDDR3InitCount = 0;
    while(!bDDR3InitState)
    {
        if(msMemoryBist())
        {
            bDDR3InitState=TRUE;
#if ENABLE_DEBUG
            printMsg("=====>   Memory Init PASS!!  <======\r\n");
#endif
        }

        if(ucDDR3InitCount++>=30)
        {
            if(!bDDR3InitState)
            {
                bDDR3InitState = TRUE;
#if ENABLE_DEBUG
                printMsg("=====>   Memory Init Fail  <======\r\n");
#endif
            }
        }
    }

    //remove mask
    msWriteByte( REG_12E0, 0x00 );
    msWriteByte( REG_1246, 0x00 );  // MIU UnMask
    msWriteByte( REG_1247, 0x00 );

    msWriteByte( REG_1248, 0xFF );  // MRW high priority
    msWriteByte( REG_1249, 0xFF );
}
#endif
