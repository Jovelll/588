
#include "global.h"

#define _MSOVD_B_C_


#define BIT0    0x0001
#define BIT1    0x0002
#define BIT2    0x0004
#define BIT3    0x0008
#define BIT4    0x0010
#define BIT5    0x0020
#define BIT6    0x0040
#define BIT7    0x0080


//==================================================================
//#define REGIO ((BYTE xdata*)0x0000)
//#define mStar_ReadByte(addr)            (REGIO[addr])
//#define msWriteByte(addr, value)   (REGIO[addr] = value)
//#define msWriteByteMask(addr, value, mask)   (REGIO[addr] = (REGIO[addr] & ~(mask)) | ((value) & (mask)))
//extern void msWriteByte(BYTE addr, BYTE value);
//extern void msWriteByteMask(BYTE addr, BYTE value, BYTE mask);
extern void Delay1ms(WORD msNums);
extern void printData(char * str, WORD value);

//==================================================================
#if 0
#define MIU_CLK_L_W3_444COM     0x35
#define MIU_CLK_H_W3_444COM     0x89

#define MIU_CLK_L_W3_565        0x15 //190MHz
#define MIU_CLK_H_W3_565        0x48

#define MIU_CLK_L_R_444COM      0x95 //200MHz
#define MIU_CLK_H_R_444COM      0x44

#define MIU_CLK_L_R_565         0x15 //185MHz
#define MIU_CLK_H_R_565         0x4A


#define MIU_REG02_R_444_COMP    0x81
#define OD_REG90_R_444_COMP     (mStar_ReadByte(BK4_90)|BIT1|BIT3)
#define OD_REGE0_R_444_COMP     0xB0

#define MIU_REG02_R_565         0x89
#define OD_REG90_R_565          (mStar_ReadByte(BK4_90)|BIT1)
#define OD_REGE0_R_565          0x30
#endif
#define OD_LIB_VER      0x03
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
/*BYTE ODGetChipID(void)
{
    BYTE ucChipID;
    BYTE ucBank =mStar_ReadByte(BK0_00);

    msWriteByte(BK0_00, REG_BANK_SCALER);
    msWriteByte(BK0_48, mStar_ReadByte(BK0_48)|BIT7);
    ucChipID=mStar_ReadByte(BK0_FD);
    msWriteByte(BK0_48, mStar_ReadByte(BK0_48)&(~BIT7));

    msWriteByte(BK0_00, ucBank);

    return ucChipID;
}*/

////////////////////////////////////////////////////////////////////////
// OverDrive Function Initialize (Load Table)
//
// Arguments: dwMemAddr - Memory base address
//            pODTbl - pointer of OD Table
////////////////////////////////////////////////////////////////////////
#if ENABLE_RTE

void drvmsLoadODTable(BYTE *pODTbl)
{
    WORD wCount;
    BYTE ucTARGET;

    if(0 == pODTbl) return;

    msWriteByte(REG_3100, 0x00);   //?????????????????
    msWriteBit(REG_3120,0, _BIT0); // Disable OD before load code
    msWriteByte(REG_3102, 0x0E); // load code start

    // Uncompressed mode
    ucTARGET=*(pODTbl+9);// 10th
    //printData("Od Data=%x",ucTARGET);
    for(wCount=0; wCount<272; wCount++)
    {
        msWriteByte(REG_3106, (wCount == 9)?ucTARGET:(ucTARGET ^ *(pODTbl+wCount)));//Od table sram1 write data
        msWrite2Byte(REG_3104, wCount|0x8000);//Od table sram1 write enable (toggle type). , 0x3105[0]+ 0x3104[7:0] ==address [8:0]
        while(_bit7_(msReadByte(REG_3105)));//Od table sram1 address
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
   // msWrite2Byte(REG_3174, 0x6E02);
        msWriteByte(REG_3102, 0x00);
}


// OverDrive Function On/Off Control
//
// Arguments: bEnable - On/Off Control
////////////////////////////////////////////////////////////////////////
void drvmsOverDriveOnOff(BOOL bEnable)
{
    if(bEnable)
    {
        msWriteByte(REG_3120, (OD_MODE<<1)|_BIT0);
    }
    else
    {
        msWriteBit(REG_3120, _DISABLE, _BIT0);
    }

}
void drvmsOverDriverInit(void)
{
#if ENABLE_DEBUG
    printMsg("=====>    msOverDriverInit!!  <======\r\n");
#endif
    msWriteByte(REG_3100, 0x00);
    msWriteByte(REG_3132, 0x30); //for od request rfifo active threshold

    msWriteByte(REG_312A, (BYTE)((MIU_OD_ADDR_EVEN)>>1));  //base address for Even
    msWriteByte(REG_312B, (BYTE)((MIU_OD_ADDR_EVEN)>>9));
    msWriteByte(REG_312C, (BYTE)((MIU_OD_ADDR_EVEN)>>17));

    msWriteByte(REG_312E, (BYTE)(MIU_OD_EVEN_LIMIT_ADDR>>1));// od address limit
    msWriteByte(REG_312F, (BYTE)(MIU_OD_EVEN_LIMIT_ADDR>>9));
    msWriteByte(REG_3130, (BYTE)(MIU_OD_EVEN_LIMIT_ADDR>>17));

    msWriteByte(REG_3172, (BYTE)(MIU_OD_SIZE>>1)); //od frame buffer write address limit
    msWriteByte(REG_3173, (BYTE)(MIU_OD_SIZE>>9));
    msWriteByte(REG_3174, (BYTE)(MIU_OD_SIZE>>17));

    msWriteByte(REG_3176, (BYTE)(MIU_OD_SIZE>>1));//od frame buffer read address limit
    msWriteByte(REG_3177, (BYTE)(MIU_OD_SIZE>>9));
    msWriteByte(REG_3178, (BYTE)(MIU_OD_SIZE>>17));
    msWriteByte(REG_31D8, _BIT0);
    msWriteByte(REG_3175, 0x2E);

#if OD_MODE == OD_MODE_333 //333+scaling
    msWriteByteMask(REG_314B, BIT3,BIT3);
    msWriteByte(REG_3150, 0x00);
    msWriteByte(REG_3151, 0x02);
#endif

}
#endif
