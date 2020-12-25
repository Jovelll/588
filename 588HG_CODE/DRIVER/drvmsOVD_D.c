
#include "global.h"

#define _MSOVD_D_C_

#if DEBUG_PRINT_ENABLE
    #define OD_DPUTSTR(str)    printMsg(str)
    #define OD_DPRINTF(str, x) printData(str, x)
#else
    #define OD_DPUTSTR(str)
    #define OD_DPRINTF(str, x)
#endif
//#define OD_LIB_VER        0x03
//code ==================================================================

//*************************************************************************************************
//Function:  msODMemoryTest()
//Description: For Memory test
//
//           ex:    printData(" msODMemoryTest = %d"  , msODMemoryTest());
//           if the result is 0,
//           if the result is 1,
//
//*************************************************************************************************

//*************************************************************************************************
//Function:  msOD_Reset()
//Description: Reset MIU bank
//caller:        Power_TurnOnPanel of power.c
//
//           It need to reset MIU bank before turn on panel.
//*************************************************************************************************


//*************************************************************************************************
//Function:  msOD_OnOff()
//Description: Enable/Disable OverDrive and setup OD strength
//          ex:
//          msOD_OnOff( _OD_Disable, 0x50 ); //disable OD
//          msOD_OnOff( _OD_Enable, 0x5F ); //enable OD and set strength as 0x5F
//          OD strength: 0x50 (weak)  <------------>  0x5F (strong)
//
//*************************************************************************************************
//*************************************************************************************************
//Function:  msLoadODT()
//Description: OD look-up table loader.
//
//           ex: msLoadODT(tOverDrive)
//
//           It provides two kinds of format (normal and compressed) for OD look-up table.
//           The look-up table and OD lib should be located in the same bank!!
//
//*************************************************************************************************
#if ENABLE_RTE

void drvmsLoadODTable(BYTE *pODTbl)
{
    WORD wCount;
    BYTE ucTARGET;

    if(0 == pODTbl) return;
    OD_DPUTSTR("Load_OD_Table");

    msWriteBit(REG_3120,0, _BIT0); // Disable OD before load code
    msWriteByte(REG_3102, 0x0E); // load code start
    
     // Uncompressed mode
    ucTARGET=*(pODTbl+9);// 10th
    for(wCount=0; wCount<272; wCount++)
    {
        msWriteByte(REG_3106, (wCount == 9)?ucTARGET:(ucTARGET ^ *(pODTbl+wCount)));//Od table sram1 write data
        msWrite2Byte(REG_3104, wCount|0x8000);//Od table sram1 address
        while(_bit7_(msReadByte(REG_3105)));//Od table sram1 write enable (toggle type). , 0x3105[0]+ 0x3104[7:0] ==address [8:0]
    }
    ucTARGET=*(pODTbl+272+19);// 20th
    for(wCount=0; wCount<272; wCount++)
    {
        msWriteByte(REG_310C, (wCount == 19)?ucTARGET:(ucTARGET ^ *(pODTbl+272+wCount)));
        msWrite2Byte(REG_310A, wCount|0x8000);
        while(_bit7_(msReadByte(REG_310B)));
    }
    ucTARGET=*(pODTbl+272*2+29);// 30th
    for(wCount=0; wCount<256; wCount++)
    {
        msWriteByte(REG_3112, (wCount == 29)?ucTARGET:(ucTARGET ^ *(pODTbl+272*2+wCount)));
        msWrite2Byte(REG_3110, wCount|0x8000);
        while(_bit7_(msReadByte(REG_3111)));
    }
    ucTARGET=*(pODTbl+272*2+256+39);// 40th
    for(wCount=0; wCount<256; wCount++)
    {
        msWriteByte(REG_3118, (wCount == 39)?ucTARGET:(ucTARGET ^ *(pODTbl+272*2+256+wCount)));
        msWrite2Byte(REG_3116, wCount|0x8000);
        while(_bit7_(msReadByte(REG_3117)));
    }
    msWriteByte(REG_3102, 0x00);
}

void drvmsOverDriveOnOff(BOOL bEnable)
{
    DB_Mode(eDB_NORMAL_MODE);

    if(bEnable)
        DB_WB(REG_3120, (OD_MODE<<1)|_BIT0);
    else
        DB_WBit(REG_3120, _DISABLE, _BIT0);

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBWriteToRegister();
#endif
}

void drvmsOverDriverInit(void)
{
    msWrite3Byte(REG_312A, (DWORD)(MIU_OD_ADDR_START)>>2);//od request base address
    msWrite3Byte(REG_312E, (DWORD)(MIU_OD_ADDR_END)>>2);//od request address limit
    msWrite3Byte(REG_3172, MIU_OD_SIZE>>2); // OD write max limit, default set to 2MB
    msWrite3Byte(REG_3176, MIU_OD_SIZE>>2); // OD read max limit, default set to 2MB
    msWriteByte(REG_3124, 0x10);//od active threshold
    msWriteByte(REG_3175, 0x2E);//[5:0]od strength gradually slop
    
    msWriteByteMask(REG_31D8, 0, _BIT1);//[1]:od hscaling filter bypass
    msWriteByteMask(REG_31D8, OD_HSCAL_EN, _BIT0);//[0]:od hscaling filter fcuntion enable
    
#if OD_MODE == OD_MODE_333 //333+scaling
    msWriteByteMask(REG_314B, BIT3,BIT3);
    msWriteByte(REG_3150, 0x00);
    msWriteByte(REG_3151, 0x02);
#endif

}
#endif
