#include "global.h"

#define _MSOVD_U_C_


#define BIT0    0x0001
#define BIT1    0x0002
#define BIT2    0x0004
#define BIT3    0x0008
#define BIT4    0x0010
#define BIT5    0x0020
#define BIT6    0x0040
#define BIT7    0x0080

//==================================================================
#define REGIO ((BYTE xdata*)0x0000)
#define mStar_ReadByte(addr)            (REGIO[addr])
#define mStar_WriteByte(addr, value)   (REGIO[addr] = value)
#define mStar_WriteByteMask(addr, value, mask)   (REGIO[addr] = (REGIO[addr] & ~(mask)) | ((value) & (mask)))
//extern BYTE mStar_ReadByte(BYTE addr);
//extern void mStar_WriteByte(BYTE addr, BYTE value);
//extern void mStar_WriteByteMask(BYTE addr, BYTE value, BYTE mask);
extern void Delay1ms(WORD msNums);
extern void printData(char * str, WORD value);

//==================================================================

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
    BYTE ucTARGET, ucReg;

    if (pODTbl == 0) return;

    msWriteByte(REG_3100, 0x00);
    msWriteBit(REG_31E1, _DISABLE, _BIT7); //disable multi-table
    msWriteBit(REG_31E4, _DISABLE, _BIT0); //disable multi-table

#if (GLASSES_TYPE!=GLASSES_NVIDIA)
    msWriteByte(REG_3175, 0x2E);
#endif

   ucReg = msReadByte(REG_3120);
    msWriteByte(REG_3120, 0x00); // Disable OD before load code
    while (msReadByte(REG_3120)&_BIT0);
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
    msWriteByte(REG_3120, ucReg);
}

void drvmsOverDriveOnOff(BOOL bEnable)
{
    msWriteByte(REG_3100, 0x00);
    if(bEnable)
        msWriteByte(REG_3120, (OD_MODE<<1)|_BIT0);
    else
        msWriteBit(REG_3120, _DISABLE, _BIT0);
}

void drvmsOverDriverInit(void)
{
    msWriteByte(REG_3100, 0x00);
    msWriteByte(REG_3132, 0x20); //for od request rfifo active threshold
    msWriteByte(REG_312A, (BYTE)((MIU_OD_ADDR_EVEN)>>4));  //base address for Even
    msWriteByte(REG_312B, (BYTE)((MIU_OD_ADDR_EVEN)>>12));
    msWriteByte(REG_312C, (BYTE)((MIU_OD_ADDR_EVEN)>>20));
    msWriteByte(REG_318E, (BYTE)((MIU_OD_ADDR_ODD)>>4));
    msWriteByte(REG_318F, (BYTE)((MIU_OD_ADDR_ODD)>>12));
    msWriteByte(REG_3190, (BYTE)((MIU_OD_ADDR_ODD)>>20));
    msWriteByte(REG_3172,0xFF);
    msWriteByte(REG_3173,0xFF);
    msWriteByte(REG_3174,0xFF);
    msWriteByte(REG_3176,0xFF);
    msWriteByte(REG_3177,0xFF);
    msWriteByte(REG_3178,0xFF);
#if ENABLE_RTE && (OD_MODE == OD_MODE_333)
    msWriteByte(REG_3132, 0x60); //for od request rfifo active threshold
    msWriteByte(REG_312E, (BYTE)(MIU_OD_EVEN_LIMIT_ADDR>>4));
    msWriteByte(REG_312F, (BYTE)(MIU_OD_EVEN_LIMIT_ADDR>>12));
    msWriteByte(REG_3130, (BYTE)(MIU_OD_EVEN_LIMIT_ADDR>>20));
#if OD_HSCAL_EN
    msWriteByte(REG_3191, 0);
    msWriteByte(REG_3192, 0);
    msWriteByte(REG_3193, 0);
    msWriteByte(REG_31E6, _BIT3|_BIT1);
    msWriteByte(REG_31D8, _BIT0);
#else
    msWriteByte(REG_3191, (BYTE)(MIU_OD_ODD_LIMIT_ADDR>>4));
    msWriteByte(REG_3192, (BYTE)(MIU_OD_ODD_LIMIT_ADDR>>12));
    msWriteByte(REG_3193, (BYTE)(MIU_OD_ODD_LIMIT_ADDR>>20));
    msWriteByte(REG_31E6, _BIT0);
#endif

    msWriteByte(REG_3172, (BYTE)(MIU_OD_SIZE>>4));
    msWriteByte(REG_3173, (BYTE)(MIU_OD_SIZE>>12));
    msWriteByte(REG_3174, (BYTE)(MIU_OD_SIZE>>20));
    msWriteByte(REG_3176, (BYTE)(MIU_OD_SIZE>>4));
    msWriteByte(REG_3177, (BYTE)(MIU_OD_SIZE>>12));
    msWriteByte(REG_3178, (BYTE)(MIU_OD_SIZE>>20));

    msWriteByte(REG_3150, 0x00);
    msWriteByte(REG_3151, 0x02);
    msWriteByte(REG_31B0, 0x60);
#endif
    msWriteByte(REG_31B2, 0x70);
#if (GLASSES_TYPE!=GLASSES_NVIDIA)
    msWriteByte(REG_3175, 0x2E);
#endif

#if OD_MODE == OD_MODE_333 //333+scaling
        msWriteByteMask(REG_314B, BIT3|BIT2,BIT3|BIT2);
#endif

    //if(PanelWidth % 2)
    //    msWriteByteMask(REG_314B, BIT3,BIT3);
    //else
    //    msWriteByteMask(REG_314B, 0,BIT3);

// tracking point
    msWriteByte(REG_3184, 0x80);
    msWriteByteMask(REG_3185, 0x02, 0x0F);
    msWriteByte(REG_3186, 0x80);
    msWriteByteMask(REG_3187, 0x01, 0x0F);
    msWriteByte(REG_3188, 0xc3);
}
#endif
