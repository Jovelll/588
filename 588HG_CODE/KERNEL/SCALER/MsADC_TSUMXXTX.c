///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    MsADC.c
/// @author MStar Semiconductor Inc.
/// @brief  ADC Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_MSADC_C_
#define _MSADC_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <math.h>
#include <string.h>
#include "board.h"
#include "types.h"
#include "ms_reg.h"
#include "debug.h"
#include "Ms_rwreg.h"
#include "global.h"
#include "Mdrv_adctbl.h"
#include "Mdrv_adctbl.c"
#include "MsADC.h"
#include "Misc.h"
#include "power.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//#define ADC_DEBUG

#ifdef ADC_DEBUG
    #define ADC_DPUTSTR(str)    	printMsg(str)
    #define ADC_DPRINTF(str, x) 	printData(str, x)
#else
    #define ADC_DPUTSTR(str)
    #define ADC_DPRINTF(str, x)
#endif

#define   AUTO_GAIN_RGB_TARGET_CODE       182           //0.5/0.7*255=182.1
#define   AUTO_GAIN_TOLERANCE_CODE           0
#define   AUTO_GAIN_RGB_MAXVAL             (AUTO_GAIN_RGB_TARGET_CODE+AUTO_GAIN_TOLERANCE_CODE )
#define   AUTO_GAIN_RGB_MINVAL             (AUTO_GAIN_RGB_TARGET_CODE-AUTO_GAIN_TOLERANCE_CODE )
#define   DADCMISMATCHLOOP         5  // ADC mismatch calibration loop 

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

typedef struct
{
    BYTE *pTable;
    BYTE u8TabCols;
    BYTE u8TabRows;
    BYTE u8TabIdx;
} TAB_Info;

typedef struct
{
    BYTE PLL_Ictrl; // ADC_ATOP_09_H [1:0]: PLL Ictrl
    BYTE PLL_Mult_Mod_216m; // ADC_ATOP_0A_L [5]: 216m_en, [4:3]: PLL mod, [2:0]: PLL mult
    BYTE LPF_Rch; // low pass filter setting in Rch
    BYTE LPF_Gch; // low pass filter setting in Gch
    BYTE LPF_Bch; // low pass filter setting in Bch
    WORD Blacklevel_Rch; // Rch black level
    WORD Blacklevel_Gch; // Gch black level
    WORD Blacklevel_Bch; // Bch black level
    WORD Offset_Rch; // Rch offset level
    WORD Offset_Gch; // Gch offset level
    WORD Offset_Bch; // Bch offset level
} msADCBackupSetting;

typedef struct
{
    BYTE DBuffer; // scaler double buffer setting
    BYTE ISEL; // Scaler input select
    WORD SPRHST; // Image horizontal sample start point
    WORD SPRHDC; // Image horizontal resolution
    WORD SPRVST; // Image vertical sample start point
    WORD SPRVDC; // Image vertical resolution
} msScalerBackupSetting;

typedef enum
{
    E_Vcal_0_55V = 0,
    E_Vcal_1_05V = 1,
} msADC_InternalVcal;

typedef enum
{
    E_PG_Htotal_256 = 0,
    E_PG_Htotal_512 = 1,
    E_PG_Htotal_1024 = 2,
    E_PG_Htotal_2048 = 3,
} msADCScalerPG_HtotalSel;

typedef struct
{
    BYTE AVG_Rch;
    BYTE AVG_Gch;
    BYTE AVG_Bch;
} msADCAvgVal;

typedef struct
{
    WORD wOffsetVal_Rch0;
    WORD wOffsetVal_Rch1;
    WORD wPivot_Rch;
    WORD wOffsetVal_Gch0;
    WORD wOffsetVal_Gch1;
    WORD wPivot_Gch;
    WORD wOffsetVal_Bch0;
    WORD wOffsetVal_Bch1;
    WORD wPivot_Bch;
    WORD wGainVal_Rch0;
    WORD wGainVal_Rch1;
    WORD wGainVal_Gch0;
    WORD wGainVal_Gch1;
    WORD wGainVal_Bch0;
    WORD wGainVal_Bch1;
} msADCMismatchCalVal;


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

msADCBackupSetting xdata gADCBackupSetting;
msScalerBackupSetting xdata gScalerBackupSetting;
msADCMismatchCalVal xdata gADCMismatchAvgVal;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static void _msADC_LoadTable(TAB_Info* pTab_info);
static void _msADC_Write2BytesMask(WORD u16regadr, WORD u16val, WORD u16mask);
static BYTE _msADC_ReadByte(WORD u16regadr);
static void _msADC_WaitAutoStatusReady(BYTE regAddr, BYTE regFlag);
static void _msADC_SetMux(ADC_MUX_TYPE ipmux_type);
static BYTE _msADC_SearchFreqSetTableIndex(WORD u16OriginalPixClk);
static void _msADC_BackupADCSetting(void);
static void _msADC_RecoverADCSetting(void);
static void _msADC_SetFreeRunClock(void);
static void _msADC_MismatchOffsetCal(void);
static void _msADC_MismatchGainCal(void);
static Bool _msADC_CheckSyncLoss(void);
static Bool _msADC_AutoAdjustAdcGain(BYTE vsyncTime);
static void _msADC_CheckPGAGain(BYTE vsyncTime);

#if DADCPLLPROTECTION
static void _msADC_IsEnableFreeRun(Bool bflag);
static void _msADC_SetAutoProst(Bool bflag);
#endif // #ifdef DADCPLLPROTECTION

#if (defined DGAINCALWITHOUTINPUT || defined DGAINCALWITHINPUT)
static void _msADC_BackupScalerSetting(void);
static void _msADC_RecoverScalerSetting(void);
#ifdef DGAINCALWITHOUTINPUT
static void _msADC_EnableScalerPG(msADCScalerPG_HtotalSel ht_sel);
//static void _msADC_MaskExternalSync(Bool bflag);
#endif
static void _msADC_SetInternalVcal(msADC_InternalVcal vcal);
static Bool _msADC_IsReadLineBuffer(Bool bflag);
static msADCAvgVal _msADC_GetRGBLineBufAverageData(WORD wLineBufAddress, BYTE LineBuf, BYTE AvgCnt);
static Bool _msADC_AutoGainAdjustment(void);
#endif


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  msADC_init()
//  [Description]
//                  ADC init
//  [Arguments]:
//                  IsShareGrd: is used shared negative pin with R/G/B
//  [Return]:
//
//**************************************************************************
void msADC_init(Bool IsShareGrd)
{
    TAB_Info Tab_info;
    BYTE ucBank;

    ucBank = msReadByte(BK0_00); // backup bank

    Tab_info.pTable = (void*)MST_ADCINIT_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_INIT_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCINIT_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = 0;

    //ADC_DPUTSTR("ADC Tbl:init\n");
    _msADC_LoadTable(&Tab_info);

    // Set share ground bit
    _msADC_Write2BytesMask(REG_ADC_ATOP_22_H, IsShareGrd ? BIT1 : 0, BIT1);

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank
}

//**************************************************************************
//  [Function Name]:
//                  msADC_SetInputMux()
//  [Description]
//                  Setup analog input mux
//  [Arguments]:
//                  port_type: InputPortType
//  [Return]:
//
//**************************************************************************
void msADC_SetInputMux(InputPortType port_type )
{
    switch(port_type)
    {
        default:
        case Input_Analog1:
            _msADC_SetMux(ADC_TABLE_MUX_RGB0_Sync);
            _msADC_SetMux(ADC_TABLE_MUX_RGB0_Data);
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  msADC_SetADCSource()
//  [Description]
//                  Setup ADC source is RGB or YPbPr or SOG
//  [Arguments]:
//                  inputsrc_type: RGB / YPbPr / SOG
//  [Return]:
//
//**************************************************************************
void msADC_SetADCSource(ADC_SOURCE_TYPE inputsrc_type)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCSOURCE_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SOURCE_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCSOURCE_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = inputsrc_type;

    ADC_DPRINTF("ADC Tbl:set src %d \n", inputsrc_type);
    _msADC_LoadTable(&Tab_info);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_SetADCModeSetting()
//  [Description]
//                  Set ADC related settings by mode
//  [Arguments]:
//                  enADCInput: RGB / YPbPr
//                  u16PixelClk: Input pixel clock(MHz)
//  [Return]:
//
//**************************************************************************
void msADC_SetADCModeSetting(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk)
{
    TAB_Info Tab_info;

    if ( enADCInput & ADC_INPUTSOURCE_YPBPR ) // YPbPr
    {
        Tab_info.pTable = (void*)MST_ADCSetModeYUV_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetModeYUV_TBL)/Tab_info.u8TabCols;

    }
    else if( enADCInput & ADC_INPUTSOURCE_RGB )// RGB
    {
        Tab_info.pTable = (void*)MST_ADCSetModeRGB_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetModeRGB_TBL)/Tab_info.u8TabCols;
    }

    Tab_info.u8TabIdx = _msADC_SearchFreqSetTableIndex(u16PixelClk);

    ADC_DPRINTF("ADC Tbl: TabIdx %d\n", Tab_info.u8TabIdx);
    _msADC_LoadTable(&Tab_info);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustHTotal()
//  [Description]
//                  Adjust PLL divider
//  [Arguments]:
//                  htotal: Htotal -3
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustHTotal(WORD htotal)
{ 
    _msADC_Write2BytesMask(REG_ADC_DTOP_01_L, htotal-3, 0x1FFF);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustPhase()
//  [Description]
//                  Set ADC phase code
//  [Arguments]:
//                  code: code, 0~127
//  [Return]:
//
//**************************************************************************
void msADC_AdjustPhase(BYTE phase)
{
    _msADC_Write2BytesMask(REG_ADC_ATOP_0F_L, (WORD)phasecode, 0x03FF);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcGain()
//  [Description]
//                  Set ADC R/G/B gain code
//  [Arguments]:
//                  rcode/gcode/bcode: R/G/B gain value, 0~2047
//  [Return]:
//
//**************************************************************************
void msADC_AdjustAdcGain(WORD rcode, WORD gcode, WORD bcode)
{
    _msADC_Write2BytesMask(REG_ADC_DTOP_45_L, rcode, 0x0FFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_48_L, gcode, 0x0FFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_51_L, bcode, 0x0FFF);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcOffset()
//  [Description]
//                  Set ADC R/G/B offset code
//  [Arguments]:
//                  rcode/gcode/bcode: R/G/B offset value, กำ0~1023(S10)
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustAdcOffset(WORD rcode, WORD gcode, WORD bcode)
{
    _msADC_Write2BytesMask(REG_ADC_DTOP_46_L, rcode, 0x07FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_49_L, gcode, 0x07FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_52_L, bcode, 0x07FF);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_ADCOffsetGainMismatchCal()
//  [Description]
//                  Do ADC self mismatch calibration
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msADC_ADCOffsetGainMismatchCal(void)
{
    BYTE loop;
    WORD wRch0diff, wRch1diff, wGch0diff, wGch1diff, wBch0diff, wBch1diff;
	
    memset(&gADCMismatchAvgVal, 0, sizeof(gADCMismatchAvgVal));
    for(loop=0; loop< DADCMISMATCHLOOP ; loop++)
    {
        _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0000, 0x003F); // [5:0]:disable mismatch offsset and gain update enable
        _msADC_Write2BytesMask(REG_ADC_ATOP_51_L, 0x0003, 0x0003); // [1:0]: no ADCA interleve H/V dither
    
        // 1. backup ADC related settings
        _msADC_BackupADCSetting();
        // 2. Set free run clock at 720p
        _msADC_SetFreeRunClock();
        // 3. Do mismatch offset calibration
        _msADC_MismatchOffsetCal();
        // 4. Do mismatch gain calibration
        _msADC_MismatchGainCal();
        // 5. recover ADC related settings & turn off free run clock
        _msADC_RecoverADCSetting();
    
        _msADC_Write2BytesMask(REG_ADC_ATOP_51_L, 0x0000, 0x0003); // [1:0]: ADCA interleve H/V dither in the normal display
        Delay1ms(10);
    }

    gADCMismatchAvgVal.wOffsetVal_Rch0 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wOffsetVal_Rch1 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wPivot_Rch /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wOffsetVal_Gch0 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wOffsetVal_Gch1 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wPivot_Gch /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wOffsetVal_Bch0 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wOffsetVal_Bch1 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wPivot_Bch /= DADCMISMATCHLOOP;
    wRch0diff = gADCMismatchAvgVal.wOffsetVal_Rch0 >= gADCMismatchAvgVal.wOffsetVal_Rch1 ? (gADCMismatchAvgVal.wOffsetVal_Rch1 - gADCMismatchAvgVal.wOffsetVal_Rch0) : 0;
    wRch1diff = gADCMismatchAvgVal.wOffsetVal_Rch1 >= gADCMismatchAvgVal.wOffsetVal_Rch0 ? (gADCMismatchAvgVal.wOffsetVal_Rch0 - gADCMismatchAvgVal.wOffsetVal_Rch1) : 0;
    wGch0diff = gADCMismatchAvgVal.wOffsetVal_Gch0 >= gADCMismatchAvgVal.wOffsetVal_Gch1 ? (gADCMismatchAvgVal.wOffsetVal_Gch1 - gADCMismatchAvgVal.wOffsetVal_Gch0) : 0;
    wGch1diff = gADCMismatchAvgVal.wOffsetVal_Gch1 >= gADCMismatchAvgVal.wOffsetVal_Gch0 ? (gADCMismatchAvgVal.wOffsetVal_Gch0 - gADCMismatchAvgVal.wOffsetVal_Gch1) : 0;
    wBch0diff = gADCMismatchAvgVal.wOffsetVal_Bch0 >= gADCMismatchAvgVal.wOffsetVal_Bch1 ? (gADCMismatchAvgVal.wOffsetVal_Bch1 - gADCMismatchAvgVal.wOffsetVal_Bch0) : 0;
    wBch1diff = gADCMismatchAvgVal.wOffsetVal_Bch1 >= gADCMismatchAvgVal.wOffsetVal_Bch0 ? (gADCMismatchAvgVal.wOffsetVal_Bch0 - gADCMismatchAvgVal.wOffsetVal_Bch1) : 0;
	
    gADCMismatchAvgVal.wGainVal_Rch0 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wGainVal_Rch1 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wGainVal_Gch0 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wGainVal_Gch1 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wGainVal_Bch0 /= DADCMISMATCHLOOP;
    gADCMismatchAvgVal.wGainVal_Bch1 /= DADCMISMATCHLOOP;
    // Set mismatch offset componsate code
    _msADC_Write2BytesMask(REG_ADC_ATOP_56_L, wRch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_57_L, wRch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_75_L, gADCMismatchAvgVal.wPivot_Rch, 0x03FF); // Set ADCR pivot-point value, choose minimum one
    _msADC_Write2BytesMask(REG_ADC_ATOP_5A_L, wGch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5B_L, wGch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_76_L, gADCMismatchAvgVal.wPivot_Gch, 0x03FF); // Set ADCG pivot-point value, choose minimum one
    _msADC_Write2BytesMask(REG_ADC_ATOP_5E_L, wBch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5F_L, wBch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_77_L, gADCMismatchAvgVal.wPivot_Bch, 0x03FF); // Set ADCB pivot-point value, choose minimum one
    // Set mismatch gain componsate code    
    _msADC_Write2BytesMask(REG_ADC_ATOP_58_L, gADCMismatchAvgVal.wGainVal_Rch0, 0x07FF); // Set ADC channel 0 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_59_L, gADCMismatchAvgVal.wGainVal_Rch1, 0x07FF); // Set ADC channel 1 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5C_L, gADCMismatchAvgVal.wGainVal_Gch0, 0x07FF); // Set ADC channel 0 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5D_L, gADCMismatchAvgVal.wGainVal_Gch1, 0x07FF); // Set ADC channel 1 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_60_L, gADCMismatchAvgVal.wGainVal_Bch0, 0x07FF); // Set ADC channel 0 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_61_L, gADCMismatchAvgVal.wGainVal_Bch1, 0x07FF); // Set ADC channel 1 gain compensated code

#if 0
    ADC_DPUTSTR("\nAverage result:\n");
    ADC_DPRINTF("R Ch0 offset code= 0x%x", wRch0diff);
    ADC_DPRINTF("R Ch1 offset code= 0x%x", wRch1diff);
    ADC_DPRINTF("R Pivot-point= 0x%x", gADCMismatchAvgVal.wPivot_Rch);
    ADC_DPRINTF("G Ch0 offset code= 0x%x", wGch0diff);
    ADC_DPRINTF("G Ch1 offset code= 0x%x", wGch1diff);
    ADC_DPRINTF("G Pivot-point= 0x%x", gADCMismatchAvgVal.wPivot_Gch);
    ADC_DPRINTF("B Ch0 offset code= 0x%x", wBch0diff);
    ADC_DPRINTF("B Ch1 offset code= 0x%x", wBch1diff);
    ADC_DPRINTF("B Pivot-point= 0x%x \n", gADCMismatchAvgVal.wPivot_Bch);
	
    ADC_DPRINTF("R Ch0 gain code= 0x%x", gADCMismatchAvgVal.wGainVal_Rch0);
    ADC_DPRINTF("R Ch1 gain code= 0x%x", gADCMismatchAvgVal.wGainVal_Rch1);
    ADC_DPRINTF("G Ch0 gain code= 0x%x", gADCMismatchAvgVal.wGainVal_Gch0);
    ADC_DPRINTF("G Ch1 gain code= 0x%x", gADCMismatchAvgVal.wGainVal_Gch1);
    ADC_DPRINTF("B Ch0 gain code= 0x%x", gADCMismatchAvgVal.wGainVal_Bch0);
    ADC_DPRINTF("B Ch1 gain code= 0x%x", gADCMismatchAvgVal.wGainVal_Bch1);
#endif
}

//**************************************************************************
//  [Function Name]:
//                  msADC_AutoPhaseAdjustment()
//  [Description]
//                  Do ADC good phase adjustment
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
Bool msADC_AutoPhaseAdjustment(BYTE vsyncTime)
{
    BYTE adjPhase, i, ucBank;

    ucBank = msReadByte(BK0_00); // backup bank
    msWriteByte(BK0_00, REGBANK0);

    ADC_DPUTSTR("msADC_AutoPhaseAdjustment\n");
#if (DADCWORSTPHASE)
{
    BYTE worsePhase, worsePhase2;
    DWORD mincksum, cksum;

    mincksum=0xFFFFFFFFul;
    for (adjPhase=0; adjPhase<0x80; adjPhase+=7)
    {
        msADC_AdjustPhase(adjPhase);
        Delay1ms(vsyncTime);
        _msADC_WaitAutoStatusReady(ATPCTRL, ATPR_B);
        cksum=mStar_ReadWord(ATPV4);
        cksum=(cksum<<16)|mStar_ReadWord(ATPV2);
        if (cksum<mincksum)
        {
            mincksum=cksum;
            worsePhase=adjPhase;
        }
        if (_msADC_CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    //  printData("coast phase %d", bestPhase);
    // 2nd Search
    if (worsePhase<7)
        worsePhase=(worsePhase+127)%0x80; // 128-7
    else
        worsePhase-=7;
    mincksum=0xFFFFFFFFul;
    for (adjPhase=worsePhase; adjPhase<worsePhase+10; adjPhase++)
    {
        i=adjPhase%0x80;
        msADC_AdjustPhase(i);
        Delay1ms(vsyncTime);
        _msADC_WaitAutoStatusReady(ATPCTRL, ATPR_B);
        cksum=mStar_ReadWord(ATPV4);
        cksum=(cksum<<16)|mStar_ReadWord(ATPV2);
        if (cksum<mincksum)
        {
            mincksum=cksum;
            worsePhase2=i;
        }
        if (_msADC_CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    UserPrefPhase=(worsePhase2+63)%0x80;
}
#else
{
    BYTE bestPhase, bestPhase2;
    DWORD maxcksum, cksum;

    maxcksum=0;
    for (adjPhase=0; adjPhase<0x80; adjPhase+=7)
    {
        msADC_AdjustPhase(adjPhase);
        ForceDelay1ms(vsyncTime);
        _msADC_WaitAutoStatusReady(ATPCTRL, ATPR_B);
        cksum=mStar_ReadWord(ATPV4);
        cksum=(cksum<<16)|mStar_ReadWord(ATPV2);
        if (cksum>maxcksum)
        {
            maxcksum=cksum;
            bestPhase=adjPhase;
        }
        if (_msADC_CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    //  printData("coast phase %d", bestPhase);
    // 2nd Search
    if (bestPhase<7)
        bestPhase=(bestPhase+121)%0x80; // 128-7
    else
        bestPhase-=7;
    maxcksum=0;
    for (adjPhase=bestPhase; adjPhase<bestPhase+10; adjPhase++)
    {
        i=adjPhase%0x80;
        msADC_AdjustPhase(i);
        ForceDelay1ms(vsyncTime);
        _msADC_WaitAutoStatusReady(ATPCTRL, ATPR_B);
        cksum=mStar_ReadWord(ATPV4);
        cksum=(cksum<<16)|mStar_ReadWord(ATPV2);
        if (cksum>maxcksum)
        {
            maxcksum=cksum;
            bestPhase2=i;
        }
        if (_msADC_CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    UserPrefPhase=bestPhase2;
}
#endif // DADCWORSTPHASE

    msADC_AdjustPhase(UserPrefPhase);
    ADC_DPRINTF("final phase %d", UserPrefPhase);

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  msADC_AutoAdcColor()
//  [Description]
//                  Do the ADC auto color adjustment
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
Bool msADC_AutoAdcColor(BYTE vsyncTime)
{
    BYTE result, ucBank;

    if ( SrcInputType == Input_Digital || SrcInputType == Input_Digital2 || SrcInputType == Input_Digital3 )
        return FALSE;

    // Set ADC offset and gain to default value
    msADC_AdjustAdcGain(0x400, 0x400, 0x400);
    if((bInputIsSOGFlag && bInputSOGisYUV) ||bInputVGAisYUV) //YPbPr or YUV_H+V
        msADC_AdjustAdcOffset(0x200, 0x40, 0x200);
    else
        msADC_AdjustAdcOffset(0, 0, 0);		
    ucBank = msReadByte(BK0_00); // backup bank
    msWriteByte(BK0_00, REGBANK0);

    msWriteByte(BK0_78, 0x01); // enable auto Gain
    _msADC_CheckPGAGain(vsyncTime);
    result = _msADC_AutoAdjustAdcGain(vsyncTime);
    if (!result)
    {
        msADC_AdjustAdcGain(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);
    }
    msWriteByte(BK0_78, 0x00); // enable auto Gain
    msWriteByte(BK0_00, ucBank);        // Switch to Original bank

    return result;
}

//**************************************************************************
//  [Function Name]:
//                  msADC_SetupHsyncPolarity()
//  [Description]
//                  Setup Hsync polarity
//  [Arguments]:
//                  bActiveHigh: TRUE -> active high, FALSE: active low
//  [Return]:
//                  
//**************************************************************************
void msADC_SetupHsyncPolarity(Bool bActiveHigh)
{
    _msADC_Write2BytesMask(REG_ADC_DTOP_07_L, bActiveHigh ? BIT7 : 0, BIT7);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustCalDuring()
//  [Description]
//                  Adjust ADC calibration during for specific reduced blanking timings
//  [Arguments]:
//                  u8CalDur: pixel
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustCalDuring(BYTE u8CalDur)
{
    _msADC_Write2BytesMask(REG_ADC_DTOP_18_L, u8CalDur, 0x00FF);
}

//**************************************************************************
//  [Function Name]:
//                  msADC_SetupADCLPF()
//  [Description]
//                  Setup ADC LPF
//  [Arguments]:
//                  val: 3dB bandwidth
//                  - VGA(RGB(YUV)_H+V): Pixel Clock/1.2 < 3dB BW
//                  - SOG(Sync on G or Y): Pixel Clock/2 < 3dB BW
//  [Return]:
//                  
//**************************************************************************
void msADC_SetupADCLPF(ADCLPFBW_LIST val)
{
    // Bch: ATOP_23[3:0]: +, ATOP_23[7:4]: -
    // Gch: ATOP_23[15:12]: +, ATOP_23[11:8]: -
    // Rch: ATOP_24[3:0]: +, ATOP_24[7:4]: -
    _msADC_Write2BytesMask(REG_ADC_ATOP_23_L, val<<12|val<<8|val<<4|val, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_ATOP_24_L, val<<4|val, 0x00FF);
}

#if DADCPLLPROTECTION
//**************************************************************************
//  [Function Name]:
//                  msADC_SetModewithPLLProtection()
//  [Description]
//                  Mode changed with applying ADC PLL protection mechnism
//  [Arguments]:
//                  enADCInput: RGB / YPbPr
//                  u16PixelClk: Input pixel clock(MHz)
//                  htotal: Htotal -3
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
Bool msADC_SetModewithPLLProtection(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk, WORD htotal)
{
    Bool result;
    BYTE regval1, regval2;
	
    //ADC_DPUTSTR("msADC_SetModewithPLLProtection\n");    
    _msADC_IsEnableFreeRun(TRUE);
    _msADC_SetAutoProst(TRUE);
    msADC_SetADCModeSetting(enADCInput, u16PixelClk);
    _msADC_SetAutoProst(FALSE);
    Delay1ms(1);
    msADC_AdjustHTotal(htotal);
    _msADC_IsEnableFreeRun(FALSE);
    Delay1ms(10); // PLL lock status would depend on input frequency, low frequency would take longer time to lock 
    regval1 = _msADC_ReadByte(REG_ADC_ATOP_0D_H);
    regval2 = _msADC_ReadByte(REG_ADC_DTOP_05_L);
    if(!(regval1 & 0x02) && (regval2 & 0x80))
    {
        result = TRUE;
    }
    else // 1st trial failed, wait 10msec to try 2nd round
    {
        //ADC_DPUTSTR("msADC_SetModewithPLLProtection 1st failed\n");		
        //ADC_DPRINTF("@@HV_flag = 0x%x", regval1);
        //ADC_DPRINTF("@@PLL_flag = 0x%x", regval2);
        Delay1ms(10);
        _msADC_IsEnableFreeRun(TRUE);
        _msADC_SetAutoProst(TRUE);
        msADC_SetADCModeSetting(enADCInput, u16PixelClk);
        _msADC_SetAutoProst(FALSE);
        Delay1ms(1);
        msADC_AdjustHTotal(htotal);
        _msADC_IsEnableFreeRun(FALSE);
        Delay1ms(10); // PLL lock status would depend on input frequency, low frequency would take longer time to lock 
        regval1 = _msADC_ReadByte(REG_ADC_ATOP_0D_H);
        regval2 = _msADC_ReadByte(REG_ADC_DTOP_05_L);
        if(!(regval1 & 0x02) && (regval2 & 0x80))
        {
            result = TRUE;
        }
        else
            result = FALSE;
    }
    // trigger ADC phase dac alignment to avoid phase shift after DC on/off
    _msADC_Write2BytesMask(REG_ADC_ATOP_11_L, BIT8, BIT8);
    _msADC_Write2BytesMask(REG_ADC_ATOP_11_L, 0, BIT8);
    ADC_DPRINTF("msADC_SetModewithPLLProtection result = %d\n", result);	
    return result;
}
#endif //#ifdef DADCPLLPROTECTION

#ifdef DGAINCALWITHOUTINPUT
//**************************************************************************
//  [Function Name]:
//                  msADC_ADCAutoGainCalwithoutInput()
//  [Description]
//                  Do ADC gain calibration without input
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msADC_ADCAutoGainCalwithoutInput(void)
{
    // 1. backup ADC and Scaler related settings
    _msADC_BackupADCSetting();
    _msADC_BackupScalerSetting();

    // 2. Set free run clock at 720p
    _msADC_SetFreeRunClock();

    // 3. Enable scaler pattern generator to generate Hsync and Vsync for line buffer
    _msADC_EnableScalerPG(E_PG_Htotal_256);

    // 4. Set ADC input LPF to lowest bandwidth
    _msADC_Write2BytesMask(REG_ADC_ATOP_23_L, 0xFFFF, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_ATOP_24_L, 0x00FF, 0x00FF);

    // 5. Calibrate ADC R/G/B gain setting value
    // Set R/G/B gain initial setting value at 0x0400
    // 6. Gain calibration target value for RGB color space mode is 182 (8bit data) 255 * 0.5 / 0.7 = 182
    // 7. Get the ADC digital output with Turn-On LDO and Set VCAL to 1.05v
    // 8. Get the ADC digital output with VCAL to 0.55v
    // 9. Calculate the difference between 1.05V and 0.55V of these 3 channels.
    //    Compare the difference value with target value (182 at RGB color space) to adjust R/G/B channel gain register
    _msADC_AutoGainAdjustment();

    // 10. recover ADC and scaler related settings & turn off free run clock & turn off internal Vcal
    _msADC_RecoverADCSetting();
    _msADC_RecoverScalerSetting();
}
#endif

#ifdef DGAINCALWITHINPUT
//**************************************************************************
//  [Function Name]:
//                  msADC_ADCAutoGainCalwithInput()
//  [Description]
//                  Do ADC gain calibration with input
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msADC_ADCAutoGainCalwithInput(void)
{
    // 1. backup ADC and Scaler related settings
    _msADC_BackupADCSetting();
    _msADC_BackupScalerSetting();

    // 2. Set free run clock at 720p
    //_msADC_SetFreeRunClock();

    // 3. Enable scaler pattern generator to generate Hsync and Vsync for line buffer
    //_msADC_EnableScalerPG(E_PG_Htotal_256);

    // 4. Set ADC input LPF to lowest bandwidth
    _msADC_Write2BytesMask(REG_ADC_ATOP_23_L, 0xFFFF, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_ATOP_24_L, 0x00FF, 0x00FF);

    // 5. Calibrate ADC R/G/B gain setting value
    // Set R/G/B gain initial setting value at 0x0400
    // 6. Gain calibration target value for RGB color space mode is 182 (8bit data) 255 * 0.5 / 0.7 = 182
    // 7. Get the ADC digital output with Turn-On LDO and Set VCAL to 1.05v
    // 8. Get the ADC digital output with VCAL to 0.55v
    // 9. Calculate the difference between 1.05V and 0.55V of these 3 channels.
    //    Compare the difference value with target value (182 at RGB color space) to adjust R/G/B channel gain register
    _msADC_AutoGainAdjustment();

    // 10. recover ADC and scaler related settings & turn off free run clock & turn off internal Vcal
    _msADC_RecoverADCSetting();
    _msADC_RecoverScalerSetting();
}

#endif

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//**************************************************************************
//  [Function Name]:
//                  _msADC_Write2BytesMask()
//  [Description]:
//                  Load ADC driver table setting
//  [Precondition]:
//
//  [Arguments]:
//                 u16regadr : 16 bits register address
//                 u16val : 16 bits register value
//                 u16mask : 16 bits register mask
//  [Return]:
//
//**************************************************************************
static void _msADC_Write2BytesMask(WORD u16regadr, WORD u16val, WORD u16mask)
{
    BYTE ucBank, ucCurBank, ucCurReg;

    ucBank = msReadByte(BK0_00); // backup bank

    ucCurBank = (BYTE)(u16regadr >> 8);
    ucCurReg = (BYTE)(u16regadr & 0xFF);

    if(ucCurBank != ucBank)
        msWriteByte(BK0_00, ucCurBank);

    if(u16mask & 0x00FF)
        mStar_WriteByteMask(ucCurReg, (BYTE)(u16val&0xFF), (BYTE)(u16mask&0xFF));
    if(u16mask & 0xFF00)
        mStar_WriteByteMask(ucCurReg+1, (BYTE)(u16val>>8), (BYTE)(u16mask>>8));

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_ReadByte()
//  [Description]:
//                  Read ADC register value
//  [Precondition]:
//
//  [Arguments]:
//                 u16regadr : 16 bits register address
//  [Return]:
//                 register value(1 byte)
//**************************************************************************
static BYTE _msADC_ReadByte(WORD u16regadr)
{
    BYTE ucBank, ucCurBank, ucCurReg, retval;

    ucBank = msReadByte(BK0_00); // backup bank

    ucCurBank = (BYTE)(u16regadr >> 8);
    ucCurReg = (BYTE)(u16regadr & 0xFF);

    if(ucCurBank != ucBank)
        msWriteByte(BK0_00, ucCurBank);
    retval = msReadByte(ucCurReg);

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank

    return retval;
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_WaitAutoStatusReady()
//  [Description]:
//                  Wait ready bit is ready
//  [Precondition]:
//
//  [Arguments]:
//                 regAddr: 8 bit register address
//                 regFlag: Ready bit
//  [Return]:
//
//**************************************************************************
static void _msADC_WaitAutoStatusReady(BYTE regAddr, BYTE regFlag)
{
    WORD waits=5000; // MCU 1x: 1500, 2x:3000, 3x: 4500
    while (waits-- && !(msReadByte(regAddr)&regFlag));
    //ADC_DPRINTF("_msADC_WaitAutoStatusReady= %d\n", waits);
}

//**************************************************************************
//  [Function Name]:
//                  Hal_ADC_LoadTable()
//  [Description]:
//                  Load ADC driver table setting
//  [Precondition]:
//
//  [Arguments]:
//                 pTab_info
//  [Return]:
//
//**************************************************************************
static void _msADC_LoadTable(TAB_Info* pTab_info)
{
    WORD i, u16Addr;
    BYTE u8Mask, u8Value, u8DoNotSet;
    BYTE ucBank, ucCurBank, ucCurReg;

    ucBank = msReadByte(BK0_00); // backup bank
    ucCurBank = ucBank;

    if (pTab_info->pTable == NULL || pTab_info->u8TabRows == 0 || pTab_info->u8TabRows == 1)
        return;

    if (REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE >= pTab_info->u8TabCols)
    {
        ADC_DPUTSTR("Tab_info error\n");
        return;
    }

    for (i=0; i<pTab_info->u8TabRows-1; i++)
    {
         u16Addr =  (WORD)( (pTab_info->pTable[0]<<8) + pTab_info->pTable[1] );

         if(u16Addr == REG_ADC_DTOP_FE_L)
         {
            // delay only, skip to write next register
            u8Value = pTab_info->pTable[REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE + 1];
            //ADC_DPRINTF("ADC tbl delay (%d) ms \n",u8Value);
            ForceDelay1ms(u8Value);
            goto NEXT;
         }

         u8Mask  = pTab_info->pTable[2];
         u8DoNotSet = pTab_info->pTable[REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE];
         u8Value = pTab_info->pTable[REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE + 1];

         //printf("[addr=%06lx, msk=%02x, enb=%02x val=%02x]\n", u32Addr, u8Mask,u8DoNotSet, u8Value);

         if ( !u8DoNotSet )
         {
             if(ucCurBank != pTab_info->pTable[0]) // switch bank if the currnet bank is different with the coming reigster
             {
                 ucCurBank = pTab_info->pTable[0];
                 msWriteByte(BK0_00, ucCurBank);
                 //ADC_DPRINTF("@@ Cur Bank= 0x%x\r\n", ucCurBank);
             }
             //ADC_DPUTSTR("@@ register value Mask = ");
             ucCurReg = pTab_info->pTable[1];
             mStar_WriteByteMask(ucCurReg, u8Value, u8Mask);
             //ADC_DPRINTF("0x%x", ucCurReg);
             //ADC_DPRINTF("0x%x", u8Value);
             //ADC_DPRINTF("0x%x", u8Mask);
         }

NEXT:
         pTab_info->pTable+=pTab_info->u8TabCols; // next
    }

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_SetMux()
//  [Description]
//                  ADC set sync and data mux
//  [Arguments]:
//                  ipmux_type: sync / data
//  [Return]:
//
//**************************************************************************
static void _msADC_SetMux(ADC_MUX_TYPE ipmux_type)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCMUX_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_MUX_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCMUX_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = ipmux_type;

    ADC_DPRINTF("ADC Tbl:set mux %d \n",ipmux_type);
    _msADC_LoadTable(&Tab_info);

}

//**************************************************************************
//  [Function Name]:
//                  _msADC_SearchFreqSetTableIndex()
//  [Description]:
//                  Search the Frequncy table index
//  [Precondition]:
//
//  [Arguments]:
//                 u16OriginalPixClk: xxxMHz
//  [Return]:
//
//**************************************************************************
static BYTE _msADC_SearchFreqSetTableIndex(WORD u16OriginalPixClk)
{
    BYTE  u8ClkIndex;

    for(u8ClkIndex=0; u8ClkIndex<sizeof(MST_ADC_FreqRange_TBL)/sizeof(ADC_FREQ_RANGE); u8ClkIndex++)
    {
        if((u16OriginalPixClk < MST_ADC_FreqRange_TBL[u8ClkIndex].FreqHLimit) &&
            (u16OriginalPixClk >= MST_ADC_FreqRange_TBL[u8ClkIndex].FreqLLimit))
            break;
    }

    return u8ClkIndex;
}


//**************************************************************************
//  [Function Name]:
//                  _msADC_BackupADCSetting()
//  [Description]:
//                  Backup ADC settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _msADC_BackupADCSetting(void)
{
    memset(&gADCBackupSetting, 0, sizeof(gADCBackupSetting));

    gADCBackupSetting.PLL_Ictrl = _msADC_ReadByte(REG_ADC_ATOP_09_H) & 0x03;
    gADCBackupSetting.PLL_Mult_Mod_216m = _msADC_ReadByte(REG_ADC_ATOP_0A_L) & 0x3F;
    gADCBackupSetting.LPF_Rch = _msADC_ReadByte(REG_ADC_ATOP_24_L) & 0x0F;
    gADCBackupSetting.LPF_Gch = _msADC_ReadByte(REG_ADC_ATOP_23_H) & 0x0F;
    gADCBackupSetting.LPF_Bch = _msADC_ReadByte(REG_ADC_ATOP_23_L) & 0x0F;
    gADCBackupSetting.Blacklevel_Rch = (_msADC_ReadByte(REG_ADC_DTOP_44_H) << 8 | _msADC_ReadByte(REG_ADC_DTOP_44_L)) & 0x03FF;
    gADCBackupSetting.Blacklevel_Gch = (_msADC_ReadByte(REG_ADC_DTOP_47_H) << 8 | _msADC_ReadByte(REG_ADC_DTOP_47_L)) & 0x03FF;
    gADCBackupSetting.Blacklevel_Bch = (_msADC_ReadByte(REG_ADC_DTOP_50_H) << 8 | _msADC_ReadByte(REG_ADC_DTOP_50_L)) & 0x03FF;
    gADCBackupSetting.Offset_Rch = (_msADC_ReadByte(REG_ADC_DTOP_46_H) << 8 | _msADC_ReadByte(REG_ADC_DTOP_46_L)) & 0x07FF;
    gADCBackupSetting.Offset_Gch = (_msADC_ReadByte(REG_ADC_DTOP_49_H) << 8 | _msADC_ReadByte(REG_ADC_DTOP_49_L)) & 0x07FF;
    gADCBackupSetting.Offset_Bch = (_msADC_ReadByte(REG_ADC_DTOP_52_H) << 8 | _msADC_ReadByte(REG_ADC_DTOP_52_L)) & 0x07FF;
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_RecoverADCSetting()
//  [Description]:
//                  Recover ADC settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _msADC_RecoverADCSetting(void)
{
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_06_L, 0x0000, 0x0080); // PLL Frequency Force Disable
    _msADC_Write2BytesMask(REG_ADC_DTOP_02_L, 0x0982, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_03_L, 0x0005, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_ATOP_0A_L, gADCBackupSetting.PLL_Mult_Mod_216m, 0x003F);
    _msADC_Write2BytesMask(REG_ADC_ATOP_09_L, gADCBackupSetting.PLL_Ictrl << 8, 0x0300);
    _msADC_Write2BytesMask(REG_ADC_ATOP_23_L, (gADCBackupSetting.LPF_Gch << 8)|gADCBackupSetting.LPF_Bch , 0x0F0F);
    _msADC_Write2BytesMask(REG_ADC_ATOP_24_L, gADCBackupSetting.LPF_Rch, 0x000F);
    _msADC_Write2BytesMask(REG_ADC_DTOP_44_L, gADCBackupSetting.Blacklevel_Rch, 0x03FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_47_L, gADCBackupSetting.Blacklevel_Gch, 0x03FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_50_L, gADCBackupSetting.Blacklevel_Bch, 0x03FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_46_L, gADCBackupSetting.Offset_Rch, 0x07FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_49_L, gADCBackupSetting.Offset_Gch, 0x07FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_52_L, gADCBackupSetting.Offset_Bch, 0x07FF);
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_SetFreeRunClock()
//  [Description]:
//                  Set ADC free-run clock
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _msADC_SetFreeRunClock(void)
{
    //_msADC_Write2BytesMask(REG_ADC_ATOP_01_L, 0x4001, 0xFFFF); // ADC enable
    //_msADC_Write2BytesMask(REG_ADC_ATOP_04_L, 0, 0xFFFF); // Turn on analog power down
    //_msADC_Write2BytesMask(REG_ADC_ATOP_06_L, 0, 0xFFFF); // Turn on gated
    _msADC_Write2BytesMask(REG_ADC_ATOP_0A_L, 0x0008, 0x003F); // PLL_MULT = 0, DIV2_EN = 1'b1
    _msADC_Write2BytesMask(REG_ADC_ATOP_09_L, 0x0300, 0x0300); // ADCPLL_ICTRL = 2'b11
    _msADC_Write2BytesMask(REG_ADC_DTOP_02_L, 0xB82E, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_03_L, 0x0052, 0xFFFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_06_L, 0x0080, 0x0080); // PLL Frequency Force Enable
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_MismatchOffsetCal()
//  [Description]:
//                  ADC mismatch offset calibration
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _msADC_MismatchOffsetCal(void)
{
    WORD wTimeOutCnt = 300;
    WORD wRch0val, wRch1val, wGch0val, wGch1val, wBch0val, wBch1val;
    WORD wRch0diff, wRch1diff, wGch0diff, wGch1diff, wBch0diff, wBch1diff;
    WORD wRchPivot, wGchPivot, wBchPivot;

    // 1. Turn on ADC offset calibration pluse
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0030, 0x0030);

    // 2. Enable ADC data low Vcal accumulator
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0080, 0x0080);

    ForceDelay1ms(1); // delay 1 msec

    // 3. Trigger ADC data low Vcal accumulator start
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0100, 0x0100);

    // 4. Wait ADC data accumulation done, ADC_ATO_74_L[2:0] = 3'b 111
    while ((wTimeOutCnt>0) && (!(_msADC_ReadByte(REG_ADC_ATOP_74_L) & 0x07)))
    {
        wTimeOutCnt--;
    }
    if(wTimeOutCnt == 0)
        ADC_DPUTSTR("ADC data accumulation time out\n");

    // 5. Read ADC channel 0 & 1 average value
    wRch0val = ((_msADC_ReadByte(REG_ADC_ATOP_62_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_62_L);
    wRch1val = ((_msADC_ReadByte(REG_ADC_ATOP_65_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_65_L);
    wGch0val = ((_msADC_ReadByte(REG_ADC_ATOP_68_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_68_L);
    wGch1val = ((_msADC_ReadByte(REG_ADC_ATOP_6B_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_6B_L);
    wBch0val = ((_msADC_ReadByte(REG_ADC_ATOP_6E_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_6E_L);
    wBch1val = ((_msADC_ReadByte(REG_ADC_ATOP_71_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_71_L);

#if 0
    // Aaron test
    wRch0val = 2000;
    wRch1val = 1889;
    wGch0val = 1776;
    wGch1val = 1888;
    wBch0val = 2;
    wBch1val = 400;
#endif

    // 6. Set ADC compensated codes and Pivot-point value
    // R-ch
    if(wRch0val > wRch1val)
    {
        wRch0diff = (WORD)(wRch1val-wRch0val); // negative number, 2's complement
        wRch1diff = 0x0000;
        wRchPivot = wRch1val; // minimum one
    }
    else if (wRch0val < wRch1val)
    {
        wRch0diff = 0x0000;
        wRch1diff = (WORD)(wRch0val-wRch1val); // negative number, 2's complement
        wRchPivot = wRch0val; // minimum one
    }
    else // wRch0val = wRch1val
    {
        wRch0diff = 0x0000;
        wRch1diff = 0x0000;
        wRchPivot = wRch0val;
    }
    // G-ch
    if(wGch0val > wGch1val)
    {
        wGch0diff = (WORD)(wGch1val-wGch0val); // negative number, 2's complement
        wGch1diff = 0x0000;
        wGchPivot = wGch1val; // minimum one
    }
    else if (wGch0val < wGch1val)
    {
        wGch0diff = 0x0000;
        wGch1diff = (WORD)(wGch0val-wGch1val); // negative number, 2's complement
        wGchPivot = wGch0val; // minimum one
    }
    else // wGch0val = wGch1val
    {
        wGch0diff = 0x0000;
        wGch1diff = 0x0000;
        wGchPivot = wGch0val;
    }
    // B-ch
    if(wBch0val > wBch1val)
    {
        wBch0diff = (WORD)(wBch1val-wBch0val); // negative number, 2's complement
        wBch1diff = 0x0000;
        wBchPivot = wBch1val; // minimum one
    }
    else if (wBch0val < wBch1val)
    {
        wBch0diff = 0x0000;
        wBch1diff = (WORD)(wBch0val-wBch1val); // negative number, 2's complement
        wBchPivot = wBch0val; // minimum one
    }
    else // wBch0val = wBch1val
    {
        wBch0diff = 0x0000;
        wBch1diff = 0x0000;
        wBchPivot = wBch0val;
    }

    gADCMismatchAvgVal.wOffsetVal_Rch0 += wRch0val;
    gADCMismatchAvgVal.wOffsetVal_Rch1 += wRch1val;
    gADCMismatchAvgVal.wPivot_Rch += wRchPivot;
    gADCMismatchAvgVal.wOffsetVal_Gch0 += wGch0val;
    gADCMismatchAvgVal.wOffsetVal_Gch1 += wGch1val;
    gADCMismatchAvgVal.wPivot_Gch += wGchPivot;
    gADCMismatchAvgVal.wOffsetVal_Bch0 += wBch0val;
    gADCMismatchAvgVal.wOffsetVal_Bch1 += wBch1val;
    gADCMismatchAvgVal.wPivot_Bch += wBchPivot;
#if 0
    _msADC_Write2BytesMask(REG_ADC_ATOP_56_L, wRch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_57_L, wRch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_75_L, wRchPivot, 0x03FF); // Set ADCR pivot-point value, choose minimum one
    _msADC_Write2BytesMask(REG_ADC_ATOP_5A_L, wGch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5B_L, wGch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_76_L, wGchPivot, 0x03FF); // Set ADCG pivot-point value, choose minimum one
    _msADC_Write2BytesMask(REG_ADC_ATOP_5E_L, wBch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5F_L, wBch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_77_L, wBchPivot, 0x03FF); // Set ADCB pivot-point value, choose minimum one
#endif
#if 0
    ADC_DPRINTF("\nR Ch0 code= 0x%x", wRch0diff);
    ADC_DPRINTF("R Ch1 code= 0x%x", wRch1diff);
    ADC_DPRINTF("R Pivot-point= 0x%x", wRchPivot);
    ADC_DPRINTF("G Ch0 code= 0x%x", wGch0diff);
    ADC_DPRINTF("G Ch1 code= 0x%x", wGch1diff);
    ADC_DPRINTF("G Pivot-point= 0x%x", wGchPivot);
    ADC_DPRINTF("B Ch0 code= 0x%x", wBch0diff);
    ADC_DPRINTF("B Ch1 code= 0x%x", wBch1diff);
    ADC_DPRINTF("B Pivot-point= 0x%x \n", wBchPivot);
#endif
    // 7. Turn on enable offset mismatch compensated codes
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0015, 0x0095); // [7]: acc low disable, [4]: B mismatch offset enable, [2]: G mismatch offset enable, [0]: R mismatch offset enable,

    // 8. Turn off ADC offset calibration pulse
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0x0030);
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_MismatchGainCal()
//  [Description]:
//                  ADC mismatch gain calibration
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _msADC_MismatchGainCal(void)
{
    WORD wTimeOutCnt = 300;
    WORD wRch0val, wRch1val, wGch0val, wGch1val, wBch0val, wBch1val;
    WORD wRch0Div, wRch1Div, wGch0Div, wGch1Div, wBch0Div, wBch1Div;

    // 1. Turn on LDO & select VCAL 1.05V, Turn on ADC gain calibration pulse
    _msADC_Write2BytesMask(REG_ADC_ATOP_34_L, 0x0001, 0x0001); // reg_ldo_en
    // Vcal = 1.05V
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0003, 0x0003); // reg_rgb_cal_ldo_sel_sw
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // reg_rgb_cal_trim_ldo_sel_sw
    // Turn on self Gcal pulse
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // [10:8]: 3'b 110, reg_rgb_self_gcal_pulse_sw

    // 2. Enable ADC data high Vcal accumulator
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0040, 0x0040);

    ForceDelay1ms(1); // delay 1 msec

    // 3. Trigger ADC data high Vcal accumulator start
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0100, 0x0100);

    // 4. Wait ADC data accumulation done, ADC_ATO_74_L[2:0] = 3'b 111
    while ((wTimeOutCnt>0) && (!(_msADC_ReadByte(REG_ADC_ATOP_74_L) & 0x07)))
    {
        wTimeOutCnt--;
    }
    if(wTimeOutCnt == 0)
        ADC_DPUTSTR("ADC data high accumulation time out\n");

    // 5. Select Vcal 0.55V
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0002, 0x0003); // reg_rgb_cal_ldo_sel_sw

    // 6. Disable ADC data high Vcal accumulator & Enable ADC data low Vcal accumulator
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0000, 0x0040);
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0080, 0x0080);

    ForceDelay1ms(1); // delay 1 msec

    // 7. Trigger ADC data low Vcal accumulator start
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0100, 0x0100);

    // 8. Wait ADC data accumulation done, ADC_ATO_74_L[2:0] = 3'b 111
    wTimeOutCnt = 300;
    while ((wTimeOutCnt>0) && (!(_msADC_ReadByte(REG_ADC_ATOP_74_L) & 0x07)))
    {
        wTimeOutCnt--;
    }
    if(wTimeOutCnt == 0)
        ADC_DPUTSTR("ADC data low accumulation time out\n");

    // 9. Read ADC channel 0 & 1 difference value(Vcal_high - Vcal_low)
    wRch0val = ((_msADC_ReadByte(REG_ADC_ATOP_64_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_64_L);
    wRch1val = ((_msADC_ReadByte(REG_ADC_ATOP_67_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_67_L);
    wGch0val = ((_msADC_ReadByte(REG_ADC_ATOP_6A_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_6A_L);
    wGch1val = ((_msADC_ReadByte(REG_ADC_ATOP_6D_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_6D_L);
    wBch0val = ((_msADC_ReadByte(REG_ADC_ATOP_70_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_70_L);
    wBch1val = ((_msADC_ReadByte(REG_ADC_ATOP_73_H)&0x03) << 8) | _msADC_ReadByte(REG_ADC_ATOP_73_L);

#if 0
    // Aaron test
    wRch0val = 3;
    wRch1val = 5;
    wGch0val = 2010;
    wGch1val = 1888;
    wBch0val = 1000;
    wBch1val = 1000;
#endif

    // 10. Set ADC compensated codes (1.10)
    // R-ch
    if(wRch0val > wRch1val)
    {
        if((wRch0val / wRch1val) > 1)
        {
            wRch1Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("_msADC_MismatchGainCal: ADC Rch1 overflow = %f\n", (float)wRch0val/wRch1val);
        }
        else
        {
            wRch1Div = ((wRch0val / wRch1val) << 10) | (WORD)(((DWORD)(wRch0val%wRch1val)<<10)/wRch1val);
        }
        wRch0Div = 0x0400; // ratio is 1, 1<<10
    }
    else if (wRch0val < wRch1val)
    {
        if((wRch1val / wRch0val) > 1)
        {
            wRch0Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("_msADC_MismatchGainCal: ADC Rch0 overflow = %f\n", (float)wRch1val/wRch0val);
        }
        else
        {
            wRch0Div = ((wRch1val / wRch0val) << 10) | (WORD)(((DWORD)(wRch1val%wRch0val)<<10)/wRch0val);
        }
        wRch1Div = 0x0400;
    }
    else // wRch0val = wRch1val
    {
        wRch0Div = 0x0400;
        wRch1Div = 0x0400;
        //ADC_DPUTSTR("ADC R Ch 0 and ch1's gain Componsated code are equal\n");
    }
    // G-ch
    if(wGch0val > wGch1val)
    {
        if((wGch0val / wGch1val) > 1)
        {
            wGch1Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("_msADC_MismatchGainCal: ADC Gch1 overflow = %f\n", (float)wGch0val/wGch1val);
        }
        else
        {
            wGch1Div = ((wGch0val / wGch1val) << 10) | (WORD)(((DWORD)(wGch0val%wGch1val)<<10)/wGch1val);
        }
        wGch0Div = 0x0400;
    }
    else if (wGch0val < wGch1val)
    {
        if((wGch1val / wGch0val) > 1)
        {
            wGch0Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("_msADC_MismatchGainCal: ADC Gch0 overflow = %f\n", (float)wGch1val/wGch0val);
        }
        else
        {
            wGch0Div = ((wGch1val / wGch0val) << 10) | (WORD)(((DWORD)(wGch1val%wRch0val)<<10)/wGch0val);
        }
        wGch1Div = 0x0400;
    }
    else // wGch0val = wGch1val
    {
        wGch0Div = 0x0400;
        wGch1Div = 0x0400;
    }
    // B-ch
    if(wBch0val > wBch1val)
    {
        if((wBch0val / wBch1val) > 1)
        {
            wBch1Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("_msADC_MismatchGainCal: ADC Bch1 overflow = %f\n", (float)wBch0val/wBch1val);
        }
        else
        {
            wBch1Div = ((wBch0val / wBch1val) << 10) | (WORD)(((DWORD)(wBch0val%wBch1val)<<10)/wRch1val);
        }
        wBch0Div = 0x0400;
    }
    else if (wBch0val < wBch1val)
    {
        if((wBch1val / wBch0val) > 1)
        {
            wBch0Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("_msADC_MismatchGainCal: ADC Bch0 overflow = %f\n", (float)wBch1val/wBch0val);
        }
        else
        {
            wBch0Div = ((wBch1val / wBch0val) << 10) | (WORD)(((DWORD)(wBch1val%wBch0val)<<10)/wBch0val);
        }
        wBch1Div = 0x0400;
    }
    else // wBch0val = wBch1val
    {
        wBch0Div = 0x0400;
        wBch1Div = 0x0400;
    }

    gADCMismatchAvgVal.wGainVal_Rch0 += wRch0Div;
    gADCMismatchAvgVal.wGainVal_Rch1 += wRch1Div;
    gADCMismatchAvgVal.wGainVal_Gch0 += wGch0Div;
    gADCMismatchAvgVal.wGainVal_Gch1 += wGch1Div;
    gADCMismatchAvgVal.wGainVal_Bch0 += wBch0Div;
    gADCMismatchAvgVal.wGainVal_Bch1 += wBch1Div;
#if 0
    _msADC_Write2BytesMask(REG_ADC_ATOP_58_L, wRch0Div, 0x07FF); // Set ADC channel 0 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_59_L, wRch1Div, 0x07FF); // Set ADC channel 1 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5C_L, wGch0Div, 0x07FF); // Set ADC channel 0 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_5D_L, wGch1Div, 0x07FF); // Set ADC channel 1 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_60_L, wBch0Div, 0x07FF); // Set ADC channel 0 gain compensated code
    _msADC_Write2BytesMask(REG_ADC_ATOP_61_L, wBch1Div, 0x07FF); // Set ADC channel 1 gain compensated code
#endif
#if 0
    ADC_DPRINTF("R Ch0 gain val= 0x%x", wRch0val);
    //ADC_DPRINTF("R Ch1 gain val= 0x%x", wRch1val);
    ADC_DPRINTF("G Ch0 gain val= 0x%x", wGch0val);
    //ADC_DPRINTF("G Ch1 gain val= 0x%x", wGch1val);
    ADC_DPRINTF("B Ch0 gain val= 0x%x", wBch0val);
    //ADC_DPRINTF("B Ch1 gain val= 0x%x", wBch1val);
    ADC_DPRINTF("R Ch0 gain code= 0x%x", wRch0Div);
    ADC_DPRINTF("R Ch1 gain code= 0x%x", wRch1Div);
    ADC_DPRINTF("G Ch0 gain code= 0x%x", wGch0Div);
    ADC_DPRINTF("G Ch1 gain code= 0x%x", wGch1Div);
    ADC_DPRINTF("B Ch0 gain code= 0x%x", wBch0Div);
    ADC_DPRINTF("B Ch1 gain code= 0x%x", wBch1Div);
#endif
    // 11. Turn on enable gain mismatch compensated codes
    _msADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x002A, 0x00AA); // [7]: acc low disable, [5]: B mismatch gain enable, [3]: G mismatch gain enable, [1]: R mismatch gain enable,

    // 12. Turn off ADC offset calibration pulse
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0x0003); // reg_rgb_cal_ldo_sel_sw
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0x000C); // reg_rgb_cal_trim_ldo_sel_sw
    _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0x0700); // Turn off self Gcal pulse
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_CheckSyncLoss()
//  [Description]:
//                  Check input sync is lost or not
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//                  TRUE: sync lose, FALSE: stable
//**************************************************************************
static Bool _msADC_CheckSyncLoss(void)
{
#if UseINT
    return InputTimingChangeFlag;
#else
    Bool result=FALSE;

    if (abs(SrcHPeriod-(msRead2Byte( SC0_E4 ) & MASK_13BIT))>10)
        result=TRUE;
    else if (abs(SrcVTotal-(msRead2Byte( SC0_E2 ) & MASK_11BIT))>10)
        result=TRUE;
    if (result)
    {
        Power_TurnOffPanel();
        Set_InputTimingChangeFlag();
    }
    return result;
#endif
}

#define DGain_OverFlow    BIT7
#define DGain_NoneOverFlow    BIT6
//**************************************************************************
//  [Function Name]:
//                  _msADC_AutoAdjustAdcGain()
//  [Description]:
//                  Check input sync is lost or not
//  [Precondition]:
//                  Do auto gain adjustment with input source
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
static Bool _msADC_AutoAdjustAdcGain(BYTE vsyncTime)
{
    WORD rGain, gGain, bGain;
    WORD maxRGain, maxGGain, maxBGain;
    WORD minRGain, minGGain, minBGain;
    BYTE retry=12;
    BYTE adcStatus;
    BYTE atgFlags=0;
    BYTE ucBank;

    maxRGain=maxGGain=maxBGain=0xFFF;
    minRGain=minGGain=minBGain=0;

    ucBank=msReadByte(BK0_00);
    msWriteByte(BK0_00, REGBANK0);

    while (retry--)
    {
        rGain=((WORD)maxRGain+minRGain)/2;
        gGain=((WORD)maxGGain+minGGain)/2;
        bGain=((WORD)maxBGain+minBGain)/2;
        msADC_AdjustAdcGain(rGain, gGain, bGain);
        Delay1ms(vsyncTime);
        // Wait scaler auto gain is done, BK0_78[1] = 1
        _msADC_WaitAutoStatusReady(BK0_78, BIT1);
        adcStatus=msReadByte(BK0_78);
    #if ADCRB_SWAP
        if (adcStatus&MAXB_B)
    #else
        if (adcStatus&MAXR_B)
    #endif
        {
            rGain--;
            atgFlags|=DGain_OverFlow;
        }
        else
        {
            rGain++;
            atgFlags|=DGain_NoneOverFlow;
        }

        if (adcStatus&MAXG_B)
        {
            gGain--;
            atgFlags|=DGain_OverFlow;
        }
        else
        {
            gGain++;
            atgFlags|=DGain_NoneOverFlow;
        }
    #if ADCRB_SWAP
        if (adcStatus&MAXR_B)
    #else
        if (adcStatus&MAXB_B)
    #endif
        {
            bGain--;
            atgFlags|=DGain_OverFlow;
        }
        else
        {
            bGain++;
            atgFlags|=DGain_NoneOverFlow;
        }

        msADC_AdjustAdcGain(rGain, gGain, bGain);
        Delay1ms(vsyncTime);
        // Wait scaler auto gain is done, BK0_78[1] = 1
        _msADC_WaitAutoStatusReady(BK0_78, BIT1);
        adcStatus=msReadByte(BK0_78);
    #if ADCRB_SWAP
        if (adcStatus&MAXB_B)
    #else
        if (adcStatus&MAXR_B)
    #endif
        {
            maxRGain=rGain+1;
            atgFlags|=DGain_OverFlow;
        }
        else
        {
            minRGain=rGain-1;
            atgFlags|=DGain_NoneOverFlow;
        }

        if (adcStatus&MAXG_B)
        {
            maxGGain=gGain+1;
            atgFlags|=DGain_OverFlow;
        }
        else
        {
            minGGain=gGain-1;
            atgFlags|=DGain_NoneOverFlow;
        }
    #if ADCRB_SWAP
        if (adcStatus&MAXR_B)
    #else
        if (adcStatus&MAXB_B)
    #endif
        {
            maxBGain=bGain+1;
            atgFlags|=DGain_OverFlow;
        }
        else
        {
            minBGain=bGain-1;
            atgFlags|=DGain_NoneOverFlow;
        }

        if (_msADC_CheckSyncLoss())
            return FALSE;
    }

    if ((atgFlags&0xC0)!=0xC0)
        return FALSE;

    UserPrefAdcRedGain=rGain-1;
    UserPrefAdcGreenGain=gGain-1;
    UserPrefAdcBlueGain=bGain-1;
    msADC_AdjustAdcGain(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);

    msWriteByte(BK0_00, ucBank);

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_CheckPGAGain()
//  [Description]:
//                  Check input signal is over PGA gain or not
//  [Precondition]: 
//                  00: 0.8V, 01: 0.9V, 10: 1.0V, 11: 1.1V
//                  The default PGA gain in Rose is 0.8V. To avoid the input signal is over 0.8V, this function is used to
//                  detect and adjust PGA gain. The maximum PGA gain would be limited in 1.0V
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
static void _msADC_CheckPGAGain(BYTE vsyncTime)
{
    BYTE ucBank, u8CurPGA, adcStatus;

    ucBank = msReadByte(BK0_00); // backup bank
    u8CurPGA = _msADC_ReadByte(REG_ADC_DTOP_55_L) & 0x3F;

    msWriteByte(BK0_00, REGBANK0);
    mStar_WriteByteMask(BK0_78, BIT3, BIT3); // outpu is overflow or underflow result
    // Wait scaler auto gain is done, BK0_78[1] = 1
    _msADC_WaitAutoStatusReady(BK0_78, BIT1);
    adcStatus=msReadByte(BK0_78);

    while(adcStatus & (MAXB_B|MAXG_B|MAXR_B))
    {
        if(u8CurPGA == 0x2A) // limited the maxi VPP = 1.0V
            break;
        else
        {
            u8CurPGA += 0x15;
            _msADC_Write2BytesMask(REG_ADC_DTOP_55_L, u8CurPGA, 0x003F);
            Delay1ms(vsyncTime);
            // Wait scaler auto gain is done, BK0_78[1] = 1
            _msADC_WaitAutoStatusReady(BK0_78, BIT1);
            adcStatus=msReadByte(BK0_78);
        }
    }
    ADC_DPRINTF("PGA Gain = 0x%x\n", u8CurPGA);
    mStar_WriteByteMask(BK0_78, 0, BIT3);
    msWriteByte(BK0_00, ucBank);        // Switch to Original bank
}

#if DADCPLLPROTECTION
//**************************************************************************
//  [Function Name]:
//                  _msADC_IsEnableFreeRun()
//  [Description]:
//                  Enable/Disable ADC free run mode
//  [Precondition]:
//                  Use to protect ADC PLL while mode changing
//  [Arguments]:
//                  bflag: TRUE => enable, FALSE => disable
//  [Return]:
//                  
//**************************************************************************
static void _msADC_IsEnableFreeRun(Bool bflag)
{
    TAB_Info Tab_info;

    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_INIT_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabIdx = 0;
    if(bflag) // enable
    {
        Tab_info.pTable = (void*)MST_ADCFreeRunEn_TBL;
        Tab_info.u8TabRows = sizeof(MST_ADCFreeRunEn_TBL)/Tab_info.u8TabCols;
    }
    else // disable
    {
        Tab_info.pTable = (void*)MST_ADCFreeRunDis_TBL;
        Tab_info.u8TabRows = sizeof(MST_ADCFreeRunDis_TBL)/Tab_info.u8TabCols;        
    }
    _msADC_LoadTable(&Tab_info);
	
    //ADC_DPRINTF("ADC Tbl:_msADC_IsEnableFreeRun %d\n", bflag);
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_SetAutoProst()
//  [Description]:
//                  Set/Release small loop auto porst
//  [Precondition]:
//                  Use to protect ADC PLL while mode changing
//  [Arguments]:
//                  bflag: TRUE => enable, FALSE => disable
//  [Return]:
//                  
//**************************************************************************
static void _msADC_SetAutoProst(Bool bflag)
{
    TAB_Info Tab_info;

    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_INIT_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabIdx = 0;
    if(bflag) // enable
    {
        Tab_info.pTable = (void*)MST_ADCPorstEn_TBL;
        Tab_info.u8TabRows = sizeof(MST_ADCPorstEn_TBL)/Tab_info.u8TabCols;
    }
    else // disable
    {
        Tab_info.pTable = (void*)MST_ADCPorstDis_TBL;
        Tab_info.u8TabRows = sizeof(MST_ADCPorstDis_TBL)/Tab_info.u8TabCols;        
    }

    _msADC_LoadTable(&Tab_info);
	
    //ADC_DPRINTF("ADC Tbl:_msADC_SetAutoProst %d\n", bflag);
}
#endif // #ifdef DADCPLLPROTECTION


#ifdef DGAINCALWITHOUTINPUT
//**************************************************************************
//  [Function Name]:
//                  _msADC_EnableScalerPG()
//  [Description]:
//                  Enable Scaler pattern generatot to generate Htotal = 256/512/1024/2048 and Vtotal=64(fixed)
//  [Precondition]:
//
//  [Arguments]:
//
//                  ht_sel: 0:256, 1:512, 2:1024, 3:2048
//  [Return]:
//
//**************************************************************************
static void _msADC_EnableScalerPG(msADCScalerPG_HtotalSel ht_sel)
{
    BYTE ucBank;
    WORD whact;

    ucBank = msReadByte(BK0_00); // backup bank

    msWriteByte(BK0_00, REGBANK0);
    //mStar_WriteByteMask(BK0_02, 0x00, 0x80); // disable free-run
    mStar_WriteByteMask(BK0_01, 0, 0x01); // disable double buffer
    mStar_WriteByteMask(BK0_02, SCALER_ISEL_ANALOG1|BIT2, 0x07); // [2]:use Hsync only, [1:0]:ISEL = analog1
    mStar_WriteWord(BK0_08, 0x0010); // SPRHST = 0x10
    whact = 256*(ht_sel+1) - 16;
    mStar_WriteWord(BK0_0C, whact); // SPRHDC = htsel - 0x10
    // Vtotal is 64 lines, Vstart = 0x10, Vactive = 0x30
    mStar_WriteWord(BK0_06, 0x0010); // SPRVST = 0x10
    mStar_WriteWord(BK0_0A, 0x0030); // SPRVDC = 0x30
    msWriteByte(BK0_00, REGBANK1);
    mStar_WriteByteMask(BK1_C0, (0x0C|ht_sel), 0x1F); //[4]: OSD to IP disable, [3]: Enable PG's Hsync/Vsync/Hde/Vde to IP, [2]: Pattern gen engine enable, [1:0]: Htotal

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank
}

#if 0
//**************************************************************************
//  [Function Name]:
//                  _msADC_MaskExternalSync()
//  [Description]:
//                  Mask external sync when connecting with source
//  [Precondition]:
//
//  [Arguments]:
//                 bflag: TRUE: Mask, FALSE: non-Mask
//
//  [Return]:
//
//**************************************************************************
static void _msADC_MaskExternalSync(Bool bflag)
{
    if(bflag)
        _msADC_Write2BytesMask(REG_ADC_ATOP_04_L, BIT14, BIT14); // power down Hsync_Cmp
    else
        _msADC_Write2BytesMask(REG_ADC_ATOP_04_L, 0, BIT14); // power on Hsync_Cmp
}
#endif
#endif //#ifdef DGAINCALWITHOUTINPUT

#if (defined DGAINCALWITHINPUT || defined DGAINCALWITHOUTINPUT)
//**************************************************************************
//  [Function Name]:
//                  _msADC_BackupScalerSetting()
//  [Description]:
//                  Backup scaler settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _msADC_BackupScalerSetting(void)
{
    BYTE ucBank;

    ucBank = msReadByte(BK0_00); // backup bank

    memset(&gScalerBackupSetting, 0, sizeof(gScalerBackupSetting));
    msWriteByte(BK0_00, REGBANK0);        // Switch to Original bank
    gScalerBackupSetting.DBuffer = msReadByte(BK0_01);
    gScalerBackupSetting.ISEL = msReadByte(BK0_02);
    gScalerBackupSetting.SPRHST = mStar_ReadWord(BK0_08) & 0x07FF;
    gScalerBackupSetting.SPRHDC = mStar_ReadWord(BK0_0C) & 0x0FFF;
    gScalerBackupSetting.SPRVST = mStar_ReadWord(BK0_06) & 0x07FF;
    gScalerBackupSetting.SPRVDC = mStar_ReadWord(BK0_0A) & 0x07FF;

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_RecoverScalerSetting()
//  [Description]:
//                  Recover scaler settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _msADC_RecoverScalerSetting(void)
{
    BYTE ucBank;

    ucBank = msReadByte(BK0_00); // backup bank

    msWriteByte(BK0_00, REGBANK0);        // Switch to Original bank
    mStar_WriteByteMask(BK0_01, gScalerBackupSetting.DBuffer, 0xFF);
    mStar_WriteByteMask(BK0_02, gScalerBackupSetting.ISEL, 0xFF);
    mStar_WriteWord(BK0_08, (mStar_ReadWord(BK0_08) & 0xF800) | gScalerBackupSetting.SPRHST);
    mStar_WriteWord(BK0_0C, (mStar_ReadWord(BK0_0C) & 0xF000) | gScalerBackupSetting.SPRHDC);
    mStar_WriteWord(BK0_06, (mStar_ReadWord(BK0_06) & 0xF800) | gScalerBackupSetting.SPRVST);
    mStar_WriteWord(BK0_0A, (mStar_ReadWord(BK0_0A) & 0xF800) | gScalerBackupSetting.SPRVDC);
    msWriteByte(BK0_00, REGBANK1);
    mStar_WriteByteMask(BK1_C0, 0x00, 0x1F); //[4]: OSD to IP disable, [3]: disable PG's Hsync/Vsync/Hde/Vde to IP, [2]: Pattern gen engine disable, [1:0]: Htotal is 256

    msWriteByte(BK0_00, ucBank);        // Switch to Original bank
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_SetInternalVcal()
//  [Description]:
//                  Enable internal Vcal(0.55V or 1.05V) for gain calibration
//  [Precondition]:
//
//  [Arguments]:
//                 vcal: 0.55V / 1.05V
//
//  [Return]:
//
//**************************************************************************
static void _msADC_SetInternalVcal(msADC_InternalVcal vcal)
{
    switch(vcal)
    {
        case E_Vcal_0_55V:
            _msADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT0, BIT0); // turn on LDO
            _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0002, 0x0003); // set Vcal = 0.55V
            _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // set Vcal from LDO
            _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // turn on Gcal pulse
            break;

        case E_Vcal_1_05V:
            _msADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT0, BIT0); // turn on LDO
            _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0003, 0x0003); // set Vcal = 1.05V
            _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // set Vcal from LDO
            _msADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // turn on Gcal pulse
            break;
        default:
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_IsReadLineBuffer()
//  [Description]:
//                  It's used to read scaler line buffer
//  [Precondition]:
//
//  [Arguments]:
//                 bflag: TRUE: reading, FALSE: recover to the normal state
//  [Return]:
//
//**************************************************************************
static Bool _msADC_IsReadLineBuffer(Bool bflag)
{
    Bool result=TRUE;
    BYTE ucBank;
    BYTE retry=5;

    ucBank=msReadByte(BK0_00);

    msWriteByte(BK0_00, REGBANK5);

    if(bflag)
    {
        mStar_WriteByteMask(BK5_32, BIT6, BIT6); // latch data in line buffer
        while(retry--)
        {
            ForceDelay1ms(10);
            if(msReadByte(BK5_32) & BIT7) // Check ready bit for reading line buffer
            {
                //ADC_DPUTSTR("Line buffer ready for read\n");
                break;
            }
        }
        if(!retry)
        {
            result = FALSE;
            ADC_DPUTSTR("Reading Line buffer fail !!\n");
        }
    }
    else
    {
        mStar_WriteByteMask(BK5_32, 0, BIT6); // disable latch data in line buffer
        //ADC_DPUTSTR("Turn off reading Line buffer \n");
    }

    msWriteByte(BK0_00,ucBank);
    return result;
}

//                  Adjust gain to approach target value 182(8 bits) => 255 * 0.5/0.7 = 182
//**************************************************************************
//  [Function Name]:
//                  _msADC_GetRGBLineBufAverageData()
//  [Description]:
//                  Get the average line buffer data for R/G/B gain calibration
//                  BK5_32[2:0]31[7:0] = 0 ~ 959(total is 960), each has two pixels and format is following
//                  BK5_33[2:0]: 101(R1) 100(G1) 011(B1) 010(R0) 001(G0) 000(B0)
//  [Precondition]:
//
//  [Arguments]:
//                 wLineBufAddress: line buffer address, it should be less than PG Htotal
//                 LineBuf: choose one line to read, 0 ~ 3
//                 AvgCnt: average pixel count
//  [Return]:
//                 msADCAvgVal: R/G/B average data
//**************************************************************************
static msADCAvgVal _msADC_GetRGBLineBufAverageData(WORD wLineBufAddress, BYTE LineBuf, BYTE AvgCnt)
{
    BYTE i, ucBank;
    WORD wAvgRdata=0, wAvgGdata=0, wAvgBdata=0;
    msADCAvgVal pAutoAdc;

    memset(&pAutoAdc, 0 , sizeof(pAutoAdc));
    ucBank=msReadByte(BK0_00);

    msWriteByte(BK0_00, REGBANK5);
    mStar_WriteByteMask(BK5_33, (LineBuf & 0x03)<<3, 0x18); // select to use which line buffer, 0 ~ 3
    for(i=0; i<AvgCnt; i++)
    {
        if(i%2 == 0)
        {
            mStar_WriteWord(BK5_32, (mStar_ReadWord(BK5_32) & 0xF800) | (wLineBufAddress+(i/2))); // config line buffer address
            // B
            mStar_WriteByteMask(BK5_33, 0, 0x07); // select B0 to read
            wAvgBdata += msReadByte(BK5_35); //read line buffer data
            // G
            mStar_WriteByteMask(BK5_33, 1, 0x07); // select G0 to read
            wAvgGdata += msReadByte(BK5_35); //read line buffer data
            // R
            mStar_WriteByteMask(BK5_33, 2, 0x07); // select R0 to read
            wAvgRdata += msReadByte(BK5_35); //read line buffer data
        }
        else
        {
            // B
            mStar_WriteByteMask(BK5_33, 3, 0x07); // select B1 to read
            wAvgBdata += msReadByte(BK5_35); //read line buffer data
            // G
            mStar_WriteByteMask(BK5_33, 4, 0x07); // select G1 to read
            wAvgGdata += msReadByte(BK5_35); //read line buffer data
            // R
            mStar_WriteByteMask(BK5_33, 5, 0x07); // select R1 to read
            wAvgRdata += msReadByte(BK5_35); //read line buffer data
        }
    }
    pAutoAdc.AVG_Bch = wAvgBdata/AvgCnt;
    pAutoAdc.AVG_Gch = wAvgGdata/AvgCnt;
    pAutoAdc.AVG_Rch = wAvgRdata/AvgCnt;

    msWriteByte(BK0_00,ucBank);

    return pAutoAdc;
}

//**************************************************************************
//  [Function Name]:
//                  _msADC_AutoGainAdjustment()
//  [Description]:
//                  Adjust gain to approach target value 182(8 bits) => 255 * 0.5/0.7 = 182
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//                  status
//**************************************************************************
static Bool _msADC_AutoGainAdjustment(void)
{
    BYTE retry=11;
    BYTE CheckFlag=0x00;
    BYTE Diff;
    msADCAvgVal pAutoAdc_1_05V;
    msADCAvgVal pAutoAdc_0_55V;
    WORD InitADCGainVal = 0x0400;
    WORD rGain = InitADCGainVal;
    WORD gGain = InitADCGainVal;
    WORD bGain = InitADCGainVal;
    WORD GainOffset_R = InitADCGainVal >> 1;
    WORD GainOffset_G = InitADCGainVal >> 1;
    WORD GainOffset_B = InitADCGainVal >> 1;
    BYTE Direction_R = 0;
    BYTE Direction_G = 0;
    BYTE Direction_B = 0;


    // 5. Calibrate ADC R/G/B gain setting value
    // Set R/G/B gain initial setting value at 0x0400
    _msADC_Write2BytesMask(REG_ADC_DTOP_45_L, InitADCGainVal, 0x0FFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_48_L, InitADCGainVal, 0x0FFF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_51_L, InitADCGainVal, 0x0FFF);
    // Set ADC R/G/B Black and offset level to 0.8V(511)
    _msADC_Write2BytesMask(REG_ADC_DTOP_44_L, 0x01FF, 0x03FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_47_L, 0x01FF, 0x03FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_50_L, 0x01FF, 0x03FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_46_L, 0x01FF, 0x07FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_49_L, 0x01FF, 0x07FF);
    _msADC_Write2BytesMask(REG_ADC_DTOP_52_L, 0x01FF, 0x07FF);

    // 6. Gain calibration target value for RGB color space mode is 182 (8bit data) 255 * 0.5 / 0.7 = 182
    // 7. Get the ADC digital output with Turn-On LDO and Set VCAL to 1.05v
    // 8. Get the ADC digital output with VCAL to 0.55v
    // 9. Calculate the difference between 1.05V and 0.55V of these 3 channels.
    //    Compare the difference value with target value (182 at RGB color space) to adjust R/G/B channel gain register
    //_msADC_MaskExternalSync(TRUE);

    while (retry--)
    {
        // Vcal = 1.05V
        _msADC_SetInternalVcal(E_Vcal_1_05V);
        ForceDelay1ms(16);
        _msADC_IsReadLineBuffer(TRUE);
        pAutoAdc_1_05V = _msADC_GetRGBLineBufAverageData(0x40, 0, 32); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240 
        _msADC_IsReadLineBuffer(FALSE);
        // Vcal = 0.55V
        _msADC_SetInternalVcal(E_Vcal_0_55V);
        ForceDelay1ms(16);
        _msADC_IsReadLineBuffer(TRUE);
        pAutoAdc_0_55V = _msADC_GetRGBLineBufAverageData(0x40, 0, 32); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240 
        _msADC_IsReadLineBuffer(FALSE);

        if ( (CheckFlag & BIT0) == 0 ) // Check Rch gain is approached target value or not
        {
            Diff = pAutoAdc_1_05V.AVG_Rch - pAutoAdc_0_55V.AVG_Rch;
            //ADC_DPRINTF("R gain Diff value=%d\n", Diff);
            if( (Diff >= AUTO_GAIN_RGB_MINVAL) && (Diff <= AUTO_GAIN_RGB_MAXVAL) )
            {
                CheckFlag|=BIT0; // Rch is calibrated completely
                //ADC_DPUTSTR("Rch auto gain complete!!\n");
            }
            else
            {
                if (Diff < AUTO_GAIN_RGB_MINVAL )
                {
                    rGain += GainOffset_R;
                    if ( retry == 2)
                    {
                        Direction_R = 1; //DownToUp
                    }
                }
                else   //(u16Diff>AUTO_GAIN_MAX )
                {
                    rGain-=GainOffset_R;
                    if ( retry == 2)
                    {
                        Direction_R = 2; //DownToUp
                    }
                }
                GainOffset_R=GainOffset_R>>1;
            }
        }
        if ( (CheckFlag & BIT1) == 0x00 ) // Check Gch gain is approached target value or not
        {
            Diff = pAutoAdc_1_05V.AVG_Gch - pAutoAdc_0_55V.AVG_Gch;
            //ADC_DPRINTF("G gain Diff value=%d\n", Diff);
            if( (Diff >= AUTO_GAIN_RGB_MINVAL) && (Diff <= AUTO_GAIN_RGB_MAXVAL) )
            {
                CheckFlag|=BIT1;
                //ADC_DPUTSTR("Gch auto gain complete !!\n");
            }
            else
            {
                if (Diff< AUTO_GAIN_RGB_MINVAL )
                {
                    gGain+=GainOffset_G;
                    if ( retry == 2)
                    {
                        Direction_G = 1; //DownToUp
                    }
                }
                else   //(u16Diff>AUTO_GAIN_MAX )
                {
                   gGain-=GainOffset_G;
                   if ( retry == 2)
                   {
                       Direction_G = 2; //DownToUp
                   }
                }
                GainOffset_G=GainOffset_G>>1;
            }
        }
        if ( (CheckFlag & BIT2) == 0x00 )
        {
            Diff = pAutoAdc_1_05V.AVG_Bch - pAutoAdc_0_55V.AVG_Bch;
            //ADC_DPRINTF("B gain Diff value=%d\n", Diff);
            if( (Diff >= AUTO_GAIN_RGB_MINVAL) && (Diff <= AUTO_GAIN_RGB_MAXVAL) )
            {
                CheckFlag|=BIT2;
                //ADC_DPUTSTR("Bch auto gain complete !!\n");
            }
            else
            {
                if (Diff< AUTO_GAIN_RGB_MINVAL )
                {
                    bGain+=GainOffset_B;
                    if ( retry == 2)
                    {
                        Direction_B = 1; //DownToUp
                    }
                }
                else   //(u16Diff>AUTO_GAIN_MAX )
                {
                    bGain-=GainOffset_B;
                    if ( retry == 2)
                    {
                        Direction_B = 2; //DownToUp
                    }
                }
                GainOffset_B=GainOffset_B>>1;
            }
        }
        if(retry==1)
        {
            if (Direction_R == 1)
            {
                rGain-=1;
            }
            else if (Direction_R == 2)
            {
                rGain+=1;
            }
            if (Direction_G == 1)
            {
                gGain-=1;
            }
            else if (Direction_G == 2)
            {
                gGain+=1;
            }
            if (Direction_B == 1)
            {
                bGain-=1;
            }
            else if (Direction_B == 2)
            {
                bGain+=1;
            }
            _msADC_Write2BytesMask(REG_ADC_DTOP_45_L, rGain, 0x0FFF);
            _msADC_Write2BytesMask(REG_ADC_DTOP_48_L, gGain, 0x0FFF);
            _msADC_Write2BytesMask(REG_ADC_DTOP_51_L, bGain, 0x0FFF);
        }
        else
        {
            _msADC_Write2BytesMask(REG_ADC_DTOP_45_L, rGain, 0x0FFF);
            _msADC_Write2BytesMask(REG_ADC_DTOP_48_L, gGain, 0x0FFF);
            _msADC_Write2BytesMask(REG_ADC_DTOP_51_L, bGain, 0x0FFF);
        }
        if((CheckFlag&0x07)==0x07)
            break;
    }

    if ((CheckFlag&0x07)==0x07)
    {
        ADC_DPUTSTR("Auto gain OK");
        UserPrefAdcRedGain = rGain;
        UserPrefAdcGreenGain = gGain;
        UserPrefAdcBlueGain = bGain;
        return TRUE;
    }
    else
    {
        ADC_DPUTSTR("Auto gain fail !!\n");
        return FALSE;
    }
}
#endif //#if (defined DGAINCALWITHINPUT || defined DGAINCALWITHOUTINPUT)

#endif // _MSADC_C_
