#define _MS_2DTo3D_C_
#include "global.h"

#if ENABLE_R2_2DTO3D
#define MS2DTo3D_DEBUG    1
#if ENABLE_DEBUG && MS2DTo3D_DEBUG
#define MS2DTo3D_printData(str, value)   printData(str, value)
#define MS2DTo3D_printMsg(str)           printMsg(str)
#else
#define MS2DTo3D_printData(str, value)
#define MS2DTo3D_printMsg(str)
#endif


#if CHIP_ID == CHIP_TSUMD
code BYTE t2Dto3DPIXGain[5] =
{
    0x10, 0x18, 0x30, 0x48, 0x60,
};

code BYTE t2Dto3DBLKGain[5] =
{
    0x04, 0x08, 0x10, 0x18, 0x20,
};

code BYTE t2Dto3DPIXOffset[5] =
{
    0x80, 0x80, 0x80, 0x80, 0x80,
};

code BYTE t2Dto3DBLKOffset[5] =
{
    0x20, 0x40, 0x80, 0xC0, 0xE0,
};
#elif CHIP_ID == CHIP_TSUM2
code BYTE t2Dto3DGain[5] =
{
    0, 8, 16, 24, 31,
};

code BYTE t2Dto3DOffset[5] =
{
    0, 33, 65, 98, 117,
};
#endif

void Sc2DTo3DInit(void)
{
    xdata WORD ucRegVal;
#if CHIP_ID == CHIP_TSUMD
    xdata DWORD dwBase;
#elif CHIP_ID == CHIP_TSUM2
    xdata DWORD dwDDBase,dwDRBase;
#else
    #message "please check if the chip supports 2D to 3D"
#endif

    MS2DTo3D_printMsg("-------------Sc2DTo3DInit------------");
    mcuDMADownloadCode(DMA_TO_DRAM_SOURCE_ADDR, DMA_TO_DRAM_DESTIN_ADDR, DMA_TO_DRAM_BYTE_COUNT);
    //mcuArrangeCodeAddr(_DRAM, MCU_ON_DRAM_START_ADDR , MCU_ON_DRAM_END_ADDR);
    //mcuArrangeCodeAddr(_SPI, MCU_ON_SPI_START_ADDR, MCU_ON_SPI_END_ADDR);

#if CHIP_ID == CHIP_TSUMD
    //DR settings
    msWriteByte(SC12_C2, 0x01);                         //enable depth render
    msWrite2ByteMask(SC12_A2, PANEL_WIDTH, 0x1FFF);     //frame width
    msWrite2ByteMask(SC12_AA, PANEL_WIDTH, 0x1FFF);     //H pixel number
    msWrite2ByteMask(SC12_A4, PANEL_HEIGHT, 0x1FFF);    //frame height

    //set the values of dr miu request control and dr miu last control
    //equal to ceiling( ceiling(PANEL_WIDTH / BLOCK_WIDTH)  * 1 bytes / number of bytes per dr request)
    //e.g. ceiling( ceiling(1920/32) * 1  / 4 )
    if(PANEL_WIDTH & 0x1F)
        ucRegVal = PANEL_WIDTH/32 + 1;
    else
        ucRegVal = PANEL_WIDTH/32;

    if(ucRegVal & 0x03)
        ucRegVal = ucRegVal/4 + 1;
    else
        ucRegVal = ucRegVal/4;
    msWriteByte(SC12_A0, ucRegVal); //dr miu request control
    msWriteByte(SC12_A1, ucRegVal); //dr miu last control

    //set the base address in DRAM for dr blk
    dwBase = (MIU_DRBLK_ADDR_START>>2);             //unit: 4 byte
    msWrite2Byte(SC12_88, dwBase);                   //dr blk rd base low
    msWrite2ByteMask(SC12_8A, dwBase>>16, 0x007F);  //dr blk rd base high
    //set the base address in DRAM for dr pix
    dwBase = (MIU_DRPIX_ADDR_START>>2);             //unit: 4 byte
    msWrite2Byte(SC12_8C, dwBase);                   //dr pix rd base low
    msWrite2ByteMask(SC12_8E, dwBase>>16, 0x007F);  //dr pix rd base high

    msWriteByte(SC12_84,0x80);  //enable dr to read miu

    //DD settings
    msWrite2ByteMask(SC14_02, PANEL_WIDTH, 0x0FFF);     //frame width
    msWrite2ByteMask(SC14_04, PANEL_HEIGHT, 0x0FFF);    //frame height

    //set the values of dd miu request control and dd miu last control
    //equal to ceiling(PANEL_WIDTH / BLOCK_WIDTH)  * (32 bytes / number of bytes per dd request)
    //e.g. ceiling(1920/32) * (32/16)
    if(PANEL_WIDTH & 0x1F)
        ucRegVal = PANEL_WIDTH/32 + 1;
    else
        ucRegVal = PANEL_WIDTH/32;
    ucRegVal *= 2;
    msWriteByte(SC14_0A, ucRegVal); //dd miu request control
    msWriteByte(SC14_0B, ucRegVal); //dd miu last control

    //set the base address in DRAM for dd
    dwBase = (MIU_DD_ADDR_START>>2);                //unit: 4 byte
    msWrite2Byte(SC14_0E, dwBase);                  //dd det wr base low  sw allocate
    msWrite2ByteMask(SC14_0C, dwBase>>16, 0x01FF);  //dd det wr base high sw allocate

    msWriteByte(SC14_0D, 0x80); //enable dd to write miu

    msWrite2ByteMask(SC13_C2, PANEL_WIDTH, 0x3FFF);     //frame width
    msWrite2ByteMask(SC13_C4, PANEL_HEIGHT, 0x1FFF);    //frame height

    msWriteByte(SC13_C0, 0x07); //T3D line buffer delay 1 h-blanking for senting out the last line
    msWriteByte(SC13_C6, 0x03); //[1] depthlb_en,[0] srclb_en
    //[0]reg_render_control (always enable in passive mode),
    //[1]reg_lr_control_l = 0, [2]reg_lr_control_r = 1 (passive mode, first line is L)
    //[1]reg_lr_control_l = 1, [2]reg_lr_control_r = 0 (passive mode, first line is R)
    //[1]reg_lr_control_l = 1, [2]reg_lr_control_r = 1 (active mode)
    msWriteByte(SC13_D0, 0x05);
    //[4]MTV_BYPASS_EN enable effect of gain and offset effect (1), disable effect gain and offset effect (0)
    msWriteByte(SC13_10, 0x30);

    //=======to be done======================
    //The number of levels, default vaules of gains, offsets for OSD
    //are going to disscuss with RD, FAE and customers.
    //The gain and offset values here are just for temporary use.
    Sc2DTo3DSetPIXGain(t2Dto3DPIXGain[UserPref2Dto3DGain]);
    Sc2DTo3DSetBLKGain(t2Dto3DBLKGain[UserPref2Dto3DGain]);
    Sc2DTo3DSetPIXOffset(t2Dto3DPIXOffset[UserPref2Dto3DOffset]);
    Sc2DTo3DSetBLKOffset(t2Dto3DBLKOffset[UserPref2Dto3DOffset]);

    msWriteByteMask(SC0_CE, BIT2, BIT2);    //enable vsync int

#elif CHIP_ID == CHIP_TSUM2
    dwDDBase = (MIU_DD_ADDR_START>>2); //unit: 4 byte
    dwDRBase = (MIU_DR_ADDR_START>>2); //unit: 4 byte

    msWrite2ByteMask(REG_240C,dwDDBase>>16,0x3FF);
    msWrite2Byte(REG_240E,dwDDBase);
    msWrite2Byte(REG_24E8,dwDRBase);
    msWrite2ByteMask(REG_24EA,dwDRBase>>16,0x3FF);
    msWriteByte(REG_24E6,0x10);  //dr memory request
    msWriteByte(REG_24E7,0x10);  //dr memory request
    msWriteByte(REG_24E3,0x00);  //dr memory enable
    msWriteByte(REG_240D,0xA0);  //dd ip/op select and memory enable
    msWriteByte(REG_240D,0xA0);  //dd ip/op select and memory enable
    msWriteByte(REG_24D8,0x01);  //Enable Depth Render

    if(PANEL_WIDTH & 0x0F)
        ucRegVal = PANEL_WIDTH/4 + 1;
    else
        ucRegVal = PANEL_WIDTH/4;
    msWrite2Byte(REG_24C0,ucRegVal);    //MVW   e.g. ceiling(800/16)

    if(PANEL_WIDTH & 0x1F)
        ucRegVal = PANEL_WIDTH/32 + 1;
    else
        ucRegVal = PANEL_WIDTH/32;
    msWrite2Byte(REG_24C4,ucRegVal|0x0F00);  //DPW e.g. ceiling(800/32)

    if(PANEL_HEIGHT & 0x1F)
        ucRegVal = PANEL_HEIGHT/32 + 1;
    else
        ucRegVal = PANEL_HEIGHT/32;
    msWrite2Byte(REG_24C2,ucRegVal);  //DPH e.g. ceiling(600/32)

    msWrite2Byte(REG_24C8, PANEL_WIDTH); //PXW   e.g. 800

    msWrite2Byte(REG_2402,PANEL_WIDTH); // H pixel number(low)
    msWrite2Byte(REG_2404,PANEL_HEIGHT|0x7800);  // V pixel number(low), bit14:11 always 0x0F, 10:0 height
    msWriteByte(REG_24CF, 0x10);    //dr force read bank
    msWriteByte(REG_2480, 0x50);    //dd froce write bank
    msWriteByte(REG_2484, 0x10);    //arbiter enable
    msWriteByte(REG_24B6, 0x05);    //driver control signal
    msWriteByte(REG_24B7, 0x97);    //driver control signal
    if(g_SetupPathInfo.bPsvPnlOutLRInvert)
        msWriteByte(REG_24CE, 0x04);
    else
        msWriteByte(REG_24CE, 0x01);

    msWriteByteMask(REG_24B8,0x0F,0x0F); //artificial detection dominate factor
    #if ENABLE_OSD_3D_CONTROL
    Sc2DTo3DSetDepth(t2Dto3DGain[UserPref2Dto3DGain]);
    Sc2DTo3DSetOffset(t2Dto3DOffset[UserPref2Dto3DOffset]);
    #endif
    msWrite2Byte(REG_24B2,0x1F03);
    msWriteByteMask(REG_24E4,BIT7,BIT7);
#endif
}

void Sc2DTo3DOff(void)
{
    MS2DTo3D_printMsg("-------------Sc2DTo3DOff------------");
#if CHIP_ID == CHIP_TSUMD
    msWriteByte(SC12_C2, 0x00);         //disable depth render
    msWriteBit(SC13_10, 0, BIT4);       //disable effect of gain and offset
#elif CHIP_ID == CHIP_TSUM2
    msWriteByteMask(REG_24E4,0,BIT7);   //disable L/R black
    msWriteByte(REG_24D8,0x00);         //disable depth render
    msWriteByte(REG_24B3,0x00);
#endif
    msWriteByte(REG_2080,0x00);
    msWriteByteMask(SC0_CE, 0,BIT2);    //disable vsync int
    msMiuProtectCtrl(MIU_PROTECT_0,_ENABLE,0, MCU_ON_DRAM_START_ADDR, MCU_CODE_SIZE );
}

#if CHIP_ID == CHIP_TSUMD
void Sc2DTo3DSetPIXGain(WORD pixGain)
{
    //2' complement (9 bits)
    msWrite2ByteMask(SC13_CA, pixGain, 0x01FF);         //pixel left gain (+)
    msWrite2ByteMask(SC13_CC, ~pixGain+0x01, 0x01FF);   //pixel right gain (-)
}

void Sc2DTo3DSetPIXOffset(BYTE pixOffset)
{
    msWriteByte(SC13_CE, pixOffset); //pixel left offset
    msWriteByte(SC13_CF, pixOffset); //pixel right offset
}

void Sc2DTo3DSetBLKGain(BYTE blkGain)
{
    //use MSB as sign bit
    msWriteByte(SC13_0D, blkGain);      //block left gain (+)
    msWriteByte(SC13_1D, blkGain|0x80); //block right gain (-)
}

void Sc2DTo3DSetBLKOffset(BYTE blkOffset)
{
    msWriteByte(SC13_0C, blkOffset); //block left offset
    msWriteByte(SC13_1C, blkOffset); //block right offset

}
#elif CHIP_ID == CHIP_TSUM2
void Sc2DTo3DSetDepth(BYTE ucDepth)
{
    msWrite2ByteMask(REG_24B8, ucDepth<<4,0x01F0);
}

void Sc2DTo3DSetOffset(BYTE offset)
{
    msWrite2ByteMask(REG_24B8, offset<<9,0xFE00);
}
#endif
#else
BYTE code ms2DTo3DNullData[] = {0};
void ms2DTo3DDummy(void)
{
    BYTE xdata i = ms2DTo3DNullData[0];
}
#endif
