///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvADC.c
/// @author MStar Semiconductor Inc.
/// @brief  ADC Function
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _DRVADC_C_
#define _DRVADC_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <math.h>
#include <string.h>
#include "board.h"
#include "types.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
//#include "DebugMsg.h"
#include "global.h"
#include "drvADC.h"
#include "Misc.h"
#include "power.h"
#if ENABLE_DEBUG
#include "gpio_def.h"
#endif

#include "msEread.h"
#include "AutoFunc.h"

#if 1//ENABLE_VGA_INPUT

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//#define ADC_DEBUG

#ifdef ADC_DEBUG
    #define ADC_DPUTSTR(str)    printMsg(str)
    #define ADC_DPRINTF(str, x) printData(str, x)
#else
    #define ADC_DPUTSTR(str)
    #define ADC_DPRINTF(str, x)
#endif

#define   AUTO_GAIN_RGB_TARGET_CODE         182           //0.5/0.7*255=182.1
#define   AUTO_GAIN_TOLERANCE_CODE          0
#define   AUTO_GAIN_RGB_MAXVAL              (AUTO_GAIN_RGB_TARGET_CODE+AUTO_GAIN_TOLERANCE_CODE )
#define   AUTO_GAIN_RGB_MINVAL              (AUTO_GAIN_RGB_TARGET_CODE-AUTO_GAIN_TOLERANCE_CODE )
#define   DADCMISMATCHLOOP                  5  // ADC mismatch calibration loop

#define drvADC_Write2BytesMask  msWrite2ByteMask
#define drvADC_ReadByte         msReadByte

void drvADC_Test_Funct(BYTE tb1, BYTE tb2);
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
#if ENABLE_XTAL_LESS
    BYTE BWCoef; // ADC PLL Bandwidth Coefficient, ADC_DTOP_02_L[7:0]
    BYTE DampCoef; // ADC PLL damping coefficient, ADC_DTOP_03_L[7:0]
#endif // #if ENABLE_XTAL_LESS
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

static void drvADC_LoadTable(TAB_Info* pTab_info);
//static void drvADC_Write2BytesMask(WORD u16regadr, WORD u16val, WORD u16mask);
//static BYTE drvADC_ReadByte(WORD u16regadr);
//static BYTE drvADC_WaitAutoStatusReady(WORD regAddr, BYTE regFlag);
static void drvADC_SetMux(ADC_MUX_TYPE ipmux_type);
static BYTE drvADC_SearchFreqSetTableIndex(WORD u16OriginalPixClk);
static void drvADC_BackupADCSetting(void);
static void drvADC_RecoverADCSetting(void);
static void drvADC_SetFreeRunClock(void);
#if ((CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
static void drvADC_MismatchLinearityCal(void);
#endif
static void drvADC_MismatchOffsetCal(void);
static void drvADC_MismatchGainCal(void);
static Bool drvADC_AutoAdjustAdcGain(BYTE vsyncTime);
static BYTE drvADC_CheckPGAGain(BYTE vsyncTime);
static void drvADC_SetPGAGainCode(BYTE ucAdcPgaGain);  //Steven110329, Solve check PGA gain issue.

#if DADCPLLPROTECTION
static void drvADC_IsEnableFreeRun(Bool bflag);
static void drvADC_SetAutoProst(Bool bflag);
#endif // #ifdef DADCPLLPROTECTION

#ifdef _DGAIN_CAL_WITHOUT_INPUT_
static void drvADC_EnableScalerPG(msADCScalerPG_HtotalSel ht_sel);
static void _msADC_MaskExternalSync(Bool bflag);
static void drvADC_SetInternalVcal(msADC_InternalVcal vcal);
static Bool drvADC_AutoGainAdjustment(void);
#endif // #ifdef (_DGAIN_CAL_WITHOUT_INPUT_)
#ifdef _DGAIN_CAL_WITH_INPUT_
static WORD drvADC_FindWhiteRegion(BYTE LineBuf, BYTE Step);
static Bool drvADC_AutoGainAdjustment_WithInput(WORD wLineBufAddress, BYTE LineBuf, BYTE AvgCnt);
#endif
#if (defined(_DGAIN_CAL_WITHOUT_INPUT_) || defined(_DGAIN_CAL_WITH_INPUT_))
static void drvADC_BackupScalerSetting(void);
static void drvADC_RecoverScalerSetting(void);
static Bool drvADC_IsReadLineBuffer(Bool bflag);
static msADCAvgVal drvADC_GetRGBLineBufAverageData(WORD wLineBufAddress, BYTE LineBuf, BYTE AvgCnt);
#endif // #if (defined(_DGAIN_CAL_WITHOUT_INPUT_) || defined(_DGAIN_CAL_WITH_INPUT_))


#define OffsetValue     20 // ADC offset adjustment

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
extern Bool CheckSyncLoss(void);

//**************************************************************************
//  [Function Name]:
//                  drvADC_init()
//  [Description]
//                  ADC init
//  [Arguments]:
//                  IsShareGrd: is used shared negative pin with R/G/B
//  [Return]:
//
//**************************************************************************
void drvADC_init(Bool IsShareGrd)
{
    TAB_Info Tab_info;
    xdata BYTE no_use = 0;
#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)|| (CHIP_ID==CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
    BYTE u8ADCBandgapTrimVal = 0;
#endif

    Tab_info.pTable = (void*)MST_ADCINIT_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_INIT_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCINIT_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = 0;

    //ADC_DPUTSTR("ADC Tbl:init\n");
    drvADC_LoadTable(&Tab_info);

    // Set share ground bit
    drvADC_Write2BytesMask( REG_ADC_ATOP_22_L, IsShareGrd ? BIT9 : 0, BIT9 );


#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)|| (CHIP_ID==CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))

#ifndef FPGA

#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ))
    u8ADCBandgapTrimVal = msEread_GetADCBandgapTrimValFromEfuse();
#elif (CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
    u8ADCBandgapTrimVal = msEread_GetDataFromEfuse(0x0001);
#endif

    if(u8ADCBandgapTrimVal & BIT7)
    {
        drvADC_Write2BytesMask( REG_ADC_ATOP_34_L, ((u8ADCBandgapTrimVal & 0x1F)<<8)|BIT7, 0x1F80);
        ADC_DPRINTF("$$ ADC Bandgap Trim value = 0x%x\n", u8ADCBandgapTrimVal);
    }
	
#endif

#endif // #if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)|| (CHIP_ID==CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9))

    drvADC_SetPGAGainCode(UserPrefAdcPgaGain);  //Steven110329, Solve check PGA gain issue.

    drvADC_SetHsyncRefLevel(ADC_HSYNC_LVL_DEF); // REFH=1.87V, REFL=0.88V // 120725 coding, moves here
    
    if(no_use)
    {
        drvADC_Test_Funct(0 ,0);
    }
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetInputMux()
//  [Description]
//                  Setup analog input mux
//  [Arguments]:
//                  port_type: InputPortType
//  [Return]:
//
//**************************************************************************
void drvADC_SetInputMux(InputPortType port_type )
{
    switch(port_type)
    {
        default:
        case Input_Analog1:
            drvADC_SetMux(ADC_TABLE_MUX_RGB0_Sync);
#if ((CHIP_ID != CHIP_TSUMC) && (CHIP_ID!=CHIP_TSUMK) && (CHIP_ID != CHIP_TSUMD)&& (CHIP_ID != CHIP_TSUMJ) && (CHIP_ID != CHIP_TSUM9)&& (CHIP_ID != CHIP_TSUMF))
            drvADC_SetMux(ADC_TABLE_MUX_RGB0_Data);
#endif
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetADCSource()
//  [Description]
//                  Setup ADC source is RGB or YPbPr or SOG
//  [Arguments]:
//                  inputsrc_type: RGB / YPbPr / SOG
//  [Return]:
//
//**************************************************************************
void drvADC_SetADCSource(ADC_SOURCE_TYPE inputsrc_type)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCSOURCE_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SOURCE_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCSOURCE_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = inputsrc_type;

    //ADC_DPRINTF("ADC Tbl:set src %d \n", inputsrc_type);
    drvADC_LoadTable(&Tab_info);
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetADCModeSetting()
//  [Description]
//                  Set ADC related settings by mode
//  [Arguments]:
//                  enADCInput: RGB / YPbPr
//                  u16PixelClk: Input pixel clock(MHz)
//  [Return]:
//
//**************************************************************************
void drvADC_SetADCModeSetting(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk)
{
    TAB_Info xdata Tab_info;

    if ( enADCInput & ADC_INPUTSOURCE_YPBPR ) // YPbPr
    {
#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF) && (ENABLE_XTAL_LESS)
        Tab_info.pTable = (void*)MST_ADCSetMode_OSCYUV_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetMode_OSCYUV_TBL)/Tab_info.u8TabCols;
#else
        Tab_info.pTable = (void*)MST_ADCSetModeYUV_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetModeYUV_TBL)/Tab_info.u8TabCols;
#endif // #if (CHIP_ID == CHIP_TSUM9) && (ENABLE_XTAL_LESS)
    }
    else if( enADCInput & ADC_INPUTSOURCE_RGB )// RGB
    {
#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF) && (ENABLE_XTAL_LESS)
        Tab_info.pTable = (void*)MST_ADCSetMode_OSCRGB_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetMode_OSCRGB_TBL)/Tab_info.u8TabCols;
#else
        Tab_info.pTable = (void*)MST_ADCSetModeRGB_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetModeRGB_TBL)/Tab_info.u8TabCols;
#endif // #if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF) && (ENABLE_XTAL_LESS)
    }

    Tab_info.u8TabIdx = drvADC_SearchFreqSetTableIndex(u16PixelClk);

    ADC_DPRINTF("ADC Tbl: TabIdx %d\n", Tab_info.u8TabIdx);
    drvADC_LoadTable(&Tab_info);
#if (CHIP_ID == CHIP_TSUM9) && ENABLE_XTAL_LESS
        gADCBackupSetting.BWCoef = drvADC_ReadByte(REG_ADC_DTOP_02_L);
        gADCBackupSetting.DampCoef = drvADC_ReadByte(REG_ADC_DTOP_03_L);
#endif
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_AdjustHTotal()
//  [Description]
//                  Adjust PLL divider
//  [Arguments]:
//                  htotal: Htotal -3
//  [Return]:
//
//**************************************************************************
#define StepDelay 1 //unit:ms
#define Step 3 		//unit:pixel
void drvADC_AdjustHTotal(WORD u16HTotal)
{
 	BYTE ucStatus=SC0_READ_SYNC_STATUS();
    WORD u16Temp=msRead2Byte(REG_ADC_DTOP_01_L);
 	BYTE u8Step=Step;
	
    if (ucStatus&(SOGD_B))
    {
		if ((abs(u16Temp-u16HTotal)/u8Step) == 0x00)
			u8Step=1;

	    while(u16Temp != u16HTotal)
	    {

			u16Temp=(u16Temp>u16HTotal)? (u16Temp=u16Temp-u8Step):(u16Temp=u16Temp+u8Step);
			#if ENABLE_MENULOAD
			    msML_WaitReady();
			    msML_WriteByte(ML_MODE_NORMAL ,REG_ADC_DTOP_01_L, (u16Temp-3));
			    msML_WriteByte(ML_MODE_NORMAL ,REG_ADC_DTOP_01_H, (u16Temp-3)>>8);
			    msML_Trigger(ML_TRIG_OUT_VDE_END);// Vde-end-OP
			#else
			    drvADC_Write2BytesMask(REG_ADC_DTOP_01_L, u16Temp-3, 0x1FFF);
			#endif
			
			ForceDelay1ms(StepDelay);
			if ((abs(u16Temp-u16HTotal))<u8Step && (u16Temp!=u16HTotal))
			{
		    	drvADC_Write2BytesMask(REG_ADC_DTOP_01_L, u16HTotal-3, 0x1FFF);
				break;
			}
		}
	}
	else
	{
		#if ENABLE_MENULOAD
		    msML_WaitReady();
		    msML_WriteByte(ML_MODE_NORMAL ,REG_ADC_DTOP_01_L, (u16HTotal-3));
		    msML_WriteByte(ML_MODE_NORMAL ,REG_ADC_DTOP_01_H, (u16HTotal-3)>>8);
		    msML_Trigger(ML_TRIG_OUT_VDE_END);// Vde-end-OP
		#else
		    drvADC_Write2BytesMask(REG_ADC_DTOP_01_L, u16HTotal-3, 0x1FFF);
		#endif
	}
}


//**************************************************************************
//  [Function Name]:
//                  drvADC_SetPhaseCode()
//  [Description]
//                  Set ADC phase code
//  [Arguments]:
//                  code: code, 0~127
//  [Return]:
//
//**************************************************************************
void drvADC_SetPhaseCode(BYTE u8PhaseCode)
{
#if (CHIP_ID == CHIP_TSUMF)
    #define BIT(_bit_)                  (1 << (_bit_))
    #define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))
	
    BYTE u8ADCPllMod = (drvADC_ReadByte(REG_ADC_ATOP_0A_L) & BMASK(4:3)) >> 3; // ADC_ATOP_09_L[12:11]: reg_adc_pll_mod
    WORD u16Mask = BMASK((6+u8ADCPllMod):0) ;
    WORD u16PhaseCode = u8PhaseCode * (1<<u8ADCPllMod);

    // fix ADC phase not be set while AC/DC On/Off
    drvADC_Write2BytesMask(REG_ADC_ATOP_15_L, (WORD)u16PhaseCode-(1<<u8ADCPllMod), u16Mask);
    drvADC_Write2BytesMask(REG_ADC_ATOP_15_L, (WORD)u16PhaseCode, u16Mask);
#else
    // fix ADC phase not be set while AC/DC On/Off
    drvADC_Write2BytesMask(REG_ADC_ATOP_0F_L, (WORD)u8PhaseCode-1, 0x03FF);
    drvADC_Write2BytesMask(REG_ADC_ATOP_0F_L, (WORD)u8PhaseCode, 0x03FF);
#endif // #if (CHIP_ID == CHIP_TSUMF)
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetRGBGainCode()
//  [Description]
//                  Set ADC R/G/B gain code
//  [Arguments]:
//                  rcode/gcode/bcode: R/G/B gain value, 0~2047
//  [Return]:
//
//**************************************************************************
void drvADC_SetRGBGainCode(WORD u16RCode, WORD u16GCode, WORD u16BCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_45_L, u16RCode, 0x0FFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_48_L, u16GCode, 0x0FFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_51_L, u16BCode, 0x0FFF);
}
void drvADC_SetRedGainCode(WORD u16RCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_45_L, u16RCode, 0x0FFF);
}
void drvADC_SetGreenGainCode(WORD u16GCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_48_L, u16GCode, 0x0FFF);
}
void drvADC_SetBlueGainCode(WORD u16BCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_51_L, u16BCode, 0x0FFF);
}
//**************************************************************************
//  [Function Name]:
//                  drvADC_SetRGBOffsetCode()
//  [Description]
//                  Set ADC R/G/B offset code
//  [Arguments]:
//                  rcode/gcode/bcode: R/G/B offset value, ¡Ó0~1023(S10)
//  [Return]:
//
//**************************************************************************
#if 1//CHIP_ID==CHIP_TSUML
void drvADC_SetRGBOffsetCode(WORD u16RCode, WORD u16GCode, WORD u16BCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_46_L, u16RCode, 0x07FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_49_L, u16GCode, 0x07FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_52_L, u16BCode, 0x07FF);
}
#endif

#if 1
void drvADC_SetRedOffsetCode(WORD u16RCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_46_L, u16RCode, 0x07FF);
}
void drvADC_SetGreenOffsetCode(WORD u16GCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_49_L, u16GCode, 0x07FF);
}
void drvADC_SetBlueOffsetCode(WORD u16BCode)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_52_L, u16BCode, 0x07FF);
}
#endif
//**************************************************************************
//  [Function Name]:
//                  drvADC_ADCOffsetGainMismatchCal()
//  [Description]
//                  Do ADC self mismatch calibration
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void drvADC_ADCOffsetGainMismatchCal(void)
{
    BYTE loop;
    WORD wRch0diff, wRch1diff, wGch0diff, wGch1diff, wBch0diff, wBch1diff;

    memset(&gADCMismatchAvgVal, 0, sizeof(gADCMismatchAvgVal));

    drvADC_Write2BytesMask(REG_ADC_ATOP_51_L, 0x0003, 0x0003); // [1:0]: no ADCA interleve H/V dither
    drvADC_Write2BytesMask(REG_ADC_ATOP_22_L, 0x0800, 0x0C00); // [11:10]: CLAMP_RGB_OV[1:0] = 2'b10 disable clamp

#if ((CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF))
#if (CHIP_ID == CHIP_TSUM9)
    drvADC_SetPGAGainCode(0x00);  // for ADC linearity calibration, PGA gain must be set to 0
#elif (CHIP_ID == CHIP_TSUMF)
    drvADC_SetPGAGainCode(0x2A);  // for ADC linearity calibration, PGA gain must be set to 0
#endif // #if (CHIP_ID == CHIP_TSUM9)
    // Do mismatch linearity calibration
    drvADC_MismatchLinearityCal();
    drvADC_SetPGAGainCode(UserPrefAdcPgaGain);  //for ADC linearity calibration
#endif // #if ((CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))

    for(loop=0; loop< DADCMISMATCHLOOP ; loop++)
    {
        drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0000, 0x003F); // [5:0]:disable mismatch offsset and gain update enable

        // 1. backup ADC related settings
        drvADC_BackupADCSetting();
        // 2. Set free run clock at 720p
        drvADC_SetFreeRunClock();
        // 3. Do mismatch offset calibration
        drvADC_MismatchOffsetCal();
        // 4. Do mismatch gain calibration
        drvADC_MismatchGainCal();
        // 5. recover ADC related settings & turn off free run clock
        drvADC_RecoverADCSetting();

        Delay1ms(10);
    }
	
    // Disable ADC dither to fix horizontal/Vertical line noise in some specific panels 
    //drvADC_Write2BytesMask(REG_ADC_ATOP_51_L, 0x0003, 0x0003); // [1:0]: ADCA interleve H/V dither in the normal display
    drvADC_Write2BytesMask(REG_ADC_ATOP_22_L, 0x0000, 0x0C00); // [11:10]: CLAMP_RGB_OV[1:0] = 2'b00 enable clamp

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

    // Restrict mismatch gain couldn't less than 0.94. otherwise it will make iclamp engine unstable
    // The maxi code is 1023, the middle point is 512 and iclamp code is 32
    // Input signal will be (1)-512 (2) x mismatch gain (3) +512, 
    // if input signal less than iclamp code, iclamp engine will be charge and discharge iteratively then makes engine crash
    // so 32/512 = 0.0625, mismatch gain must be large than (1-0.0625), that is 0.94 = 0x3C2
    gADCMismatchAvgVal.wGainVal_Rch0 = (gADCMismatchAvgVal.wGainVal_Rch0 > 0x3C2) ? gADCMismatchAvgVal.wGainVal_Rch0 : 0x3C2;
    gADCMismatchAvgVal.wGainVal_Rch1 = (gADCMismatchAvgVal.wGainVal_Rch1 > 0x3C2) ? gADCMismatchAvgVal.wGainVal_Rch1 : 0x3C2;
    gADCMismatchAvgVal.wGainVal_Gch0 = (gADCMismatchAvgVal.wGainVal_Gch0 > 0x3C2) ? gADCMismatchAvgVal.wGainVal_Gch0 : 0x3C2;
    gADCMismatchAvgVal.wGainVal_Gch1 = (gADCMismatchAvgVal.wGainVal_Gch1 > 0x3C2) ? gADCMismatchAvgVal.wGainVal_Gch1 : 0x3C2;
    gADCMismatchAvgVal.wGainVal_Bch0 = (gADCMismatchAvgVal.wGainVal_Bch0 > 0x3C2) ? gADCMismatchAvgVal.wGainVal_Bch0 : 0x3C2;
    gADCMismatchAvgVal.wGainVal_Bch1 = (gADCMismatchAvgVal.wGainVal_Bch1 > 0x3C2) ? gADCMismatchAvgVal.wGainVal_Bch1 : 0x3C2;
    
    // Set mismatch offset componsate code
    drvADC_Write2BytesMask(REG_ADC_ATOP_56_L, wRch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_57_L, wRch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_75_L, gADCMismatchAvgVal.wPivot_Rch, 0x03FF); // Set ADCR pivot-point value, choose minimum one
    drvADC_Write2BytesMask(REG_ADC_ATOP_5A_L, wGch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5B_L, wGch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_76_L, gADCMismatchAvgVal.wPivot_Gch, 0x03FF); // Set ADCG pivot-point value, choose minimum one
    drvADC_Write2BytesMask(REG_ADC_ATOP_5E_L, wBch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5F_L, wBch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_77_L, gADCMismatchAvgVal.wPivot_Bch, 0x03FF); // Set ADCB pivot-point value, choose minimum one
    // Set mismatch gain componsate code
    drvADC_Write2BytesMask(REG_ADC_ATOP_58_L, gADCMismatchAvgVal.wGainVal_Rch0, 0x07FF); // Set ADC channel 0 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_59_L, gADCMismatchAvgVal.wGainVal_Rch1, 0x07FF); // Set ADC channel 1 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5C_L, gADCMismatchAvgVal.wGainVal_Gch0, 0x07FF); // Set ADC channel 0 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5D_L, gADCMismatchAvgVal.wGainVal_Gch1, 0x07FF); // Set ADC channel 1 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_60_L, gADCMismatchAvgVal.wGainVal_Bch0, 0x07FF); // Set ADC channel 0 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_61_L, gADCMismatchAvgVal.wGainVal_Bch1, 0x07FF); // Set ADC channel 1 gain compensated code

#if 1
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

#if 0 // wait for re-manege
//**************************************************************************
//  [Function Name]:
//                  drvADC_AutoPhaseAdjustment()
//  [Description]
//                  Do ADC good phase adjustment
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
Bool drvADC_AutoPhaseAdjustment(BYTE u8VSyncTime)
{
    BYTE u8AdjPhase, i;

    ADC_DPUTSTR("drvADC_AutoPhaseAdjustment\n");
#if (DADCWORSTPHASE)
{
    BYTE worsePhase, worsePhase2;
    DWORD mincksum, cksum;

    mincksum=0xFFFFFFFFul;
    for (u8AdjPhase=0; u8AdjPhase<0x80; u8AdjPhase+=7)
    {
        drvADC_SetPhaseCode(u8AdjPhase);
        Delay1ms(u8VSyncTime);
        drvADC_WaitAutoStatusReady(SC0_8B, BIT1);
        cksum=msRead2Byte(SC0_8E);
        cksum=(cksum<<16)|msRead2Byte(SC0_8C);
        if (cksum<mincksum)
        {
            mincksum=cksum;
            worsePhase=u8AdjPhase;
        }
        if (CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    //  printData("coast phase %d", bestPhase);
    // 2nd Search
    if (worsePhase<7)
        worsePhase=(worsePhase+127)%0x80; // 128-7
    else
        worsePhase-=7;
    mincksum=0xFFFFFFFFul;
    for (u8AdjPhase=worsePhase; u8AdjPhase<worsePhase+10; u8AdjPhase++)
    {
        i=u8AdjPhase%0x80;
        drvADC_SetPhaseCode(i);
        Delay1ms(u8VSyncTime);
        drvADC_WaitAutoStatusReady(SC0_8B, BIT1);
        cksum=msRead2Byte(SC0_8E);
        cksum=(cksum<<16)|msRead2Byte(SC0_8C);
        if (cksum<mincksum)
        {
            mincksum=cksum;
            worsePhase2=i;
        }
        if (CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    u8AdjPhase=(worsePhase2+63)%0x80;
    #define FINAL_PHASE u8AdjPhase
}
#else
{
    BYTE bestPhase, bestPhase2;
    DWORD maxcksum, cksum;

    maxcksum=0;
    for (u8AdjPhase=0; u8AdjPhase<0x80; u8AdjPhase+=7)
    {
        drvADC_SetPhaseCode(u8AdjPhase);
        ForceDelay1ms(u8VSyncTime);
        drvADC_WaitAutoStatusReady(SC0_8B, BIT1);
        cksum=msRead2Byte(SC0_8E);
        cksum=(cksum<<16)|msRead2Byte(SC0_8C);
        if (cksum>maxcksum)
        {
            maxcksum=cksum;
            bestPhase=u8AdjPhase;
        }
        if (CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    //  printData("coast phase %d", bestPhase);
    // 2nd Search
    if (bestPhase<7)
        bestPhase=(bestPhase+121)%0x80; // 128-7
    else
        bestPhase-=7;
    maxcksum=0;
    for (u8AdjPhase=bestPhase; u8AdjPhase<bestPhase+10; u8AdjPhase++)
    {
        i=u8AdjPhase%0x80;
        drvADC_SetPhaseCode(i);
        ForceDelay1ms(u8VSyncTime);
        drvADC_WaitAutoStatusReady(SC0_8B, BIT1);
        cksum=msRead2Byte(SC0_8E);
        cksum=(cksum<<16)|msRead2Byte(SC0_8C);
        if (cksum>maxcksum)
        {
            maxcksum=cksum;
            bestPhase2=i;
        }
        if (CheckSyncLoss())//||CheckPowerKeyStatus())
            return FALSE;
    }
    u8AdjPhase=bestPhase2;
    #define FINAL_PHASE u8AdjPhase
}
#endif // DADCWORSTPHASE

    drvADC_SetPhaseCode(FINAL_PHASE);
    ADC_DPRINTF("final phase %d", FINAL_PHASE);
    //UPPHASE_SET_BY_REAL_VALUE(FINAL_PHASE);
    #undef FINAL_PHASE
    return TRUE;
}
#endif
//**************************************************************************
//  [Function Name]:
//                  drvADC_AutoAdcColor()
//  [Description]
//                  Do the ADC auto color adjustment
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
Bool drvADC_AutoAdcColor(BYTE u8VSyncTime)
{
    BYTE xdata result;
    BYTE xdata u8AdcPgaGain;

    if (!(CURRENT_INPUT_IS_VGA()))//(SrcInputType!=Input_Analog1)
        return FALSE;

    ADC_DPUTSTR("drvADC_AutoAdcColor\n");
    //drvADC_SetPGAGainCode(0);
    //if(USER_PREF_INPUT_COLOR_FORMAT==INPUT_COLOR_YPBPR) //YPbPr or YUV_H+V
    //    drvADC_SetRGBOffsetCode(0x200, 0x40, 0x200);
    //else
        drvADC_SetRGBOffsetCode(0, 0, 0);
    drvADC_SetRGBGainCode(0x421, 0x421, 0x421);

    msWriteByte(SC0_78, 0x01); // enable auto Gain
    u8AdcPgaGain=drvADC_CheckPGAGain(u8VSyncTime);

    result = drvADC_AutoAdjustAdcGain(u8VSyncTime);

    if(result)
    {
        ADC_DPUTSTR("$$ Auto color pass! \n");
        UserPrefAdcPgaGain = u8AdcPgaGain;
        UserPrefAdcRedOffset = 0x800-OffsetValue;
        UserPrefAdcGreenOffset = 0x800-OffsetValue;
        UserPrefAdcBlueOffset = 0x800-OffsetValue;
        drvADC_SetRGBOffsetCode(UserPrefAdcRedOffset, UserPrefAdcGreenOffset, UserPrefAdcBlueOffset);        
    }
    else
    {
        drvADC_SetPGAGainCode( UserPrefAdcPgaGain );
        drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);
        drvADC_SetRGBOffsetCode(UserPrefAdcRedOffset, UserPrefAdcGreenOffset, UserPrefAdcBlueOffset);
    }

/*    
    if (!result)
    {
        ADC_DPUTSTR( "Auto color failed !!");
        drvADC_SetPGAGainCode(UserPrefAdcPgaGain);
        drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);
    }
    else
    {
        UserPrefAdcPgaGain=u8AdcPgaGain;
    }
*/    
    
    msWriteByte(SC0_78, 0); // disable auto Gain

    #if 1
    ADC_DPRINTF("adc gain red 0x%x", UserPrefAdcRedGain);
    ADC_DPRINTF("adc gain green 0x%x", UserPrefAdcGreenGain);
    ADC_DPRINTF("adc gain blue 0x%x", UserPrefAdcBlueGain);
    ADC_DPRINTF("adc gain PGA 0x%x", UserPrefAdcPgaGain);
    #endif

    return result;
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetupHsyncPolarity()
//  [Description]
//                  Setup Hsync polarity
//  [Arguments]:
//                  bActiveHigh: TRUE -> active high, FALSE: active low
//  [Return]:
//
//**************************************************************************
void drvADC_SetupHsyncPolarity(Bool IsActiveHigh)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_07_L,(IsActiveHigh?BIT7:0),(BIT7));
}
//**************************************************************************
//  [Function Name]:
//                  drvADC_AdjustCalDuring()
//  [Description]
//                  Adjust ADC calibration during for specific reduced blanking timings
//  [Arguments]:
//                  u8CalDur: pixel
//  [Return]:
//
//**************************************************************************
void drvADC_AdjustCalDuring(BYTE u8CalDur)
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_18_L, u8CalDur, 0x00FF);
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetupADCLPF()
//  [Description]
//                  Setup ADC LPF
//  [Arguments]:
//                  val: 3dB bandwidth
//                  - VGA(RGB(YUV)_H+V): Pixel Clock/1.2 < 3dB BW
//                  - SOG(Sync on G or Y): Pixel Clock/2 < 3dB BW
//  [Return]:
//
//**************************************************************************
#if 0
void drvADC_SetupADCLPF(ADCLPFBW_LIST val)
{
    // Bch: ATOP_23[3:0]: +, ATOP_23[7:4]: -
    // Gch: ATOP_23[15:12]: +, ATOP_23[11:8]: -
    // Rch: ATOP_24[3:0]: +, ATOP_24[7:4]: -
    drvADC_Write2BytesMask(REG_ADC_ATOP_23_L, val<<12|val<<8|val<<4|val, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_ATOP_24_L, val<<4|val, 0x00FF);
}
#endif
#if DADCPLLPROTECTION
//**************************************************************************
//  [Function Name]:
//                  drvADC_SetModewithPLLProtection()
//  [Description]
//                  Mode changed with applying ADC PLL protection mechnism
//  [Arguments]:
//                  enADCInput: RGB / YPbPr
//                  u16PixelClk: Input pixel clock(MHz)
//                  htotal: Htotal -3
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
Bool drvADC_SetModewithPLLProtection(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk, WORD u16HTotal)
{
    Bool result;
    BYTE regval1, regval2;

    //ADC_DPUTSTR("drvADC_SetModewithPLLProtection\n");
    drvADC_IsEnableFreeRun(TRUE);
    drvADC_SetAutoProst(TRUE);
    drvADC_SetADCModeSetting(enADCInput, u16PixelClk);
    drvADC_SetAutoProst(FALSE);
    Delay1ms(1);
    drvADC_AdjustHTotal(u16HTotal);
    drvADC_IsEnableFreeRun(FALSE);
    Delay1ms(10); // PLL lock status would depend on input frequency, low frequency would take longer time to lock
    regval1 = drvADC_ReadByte(REG_ADC_ATOP_0D_H);
    regval2 = drvADC_ReadByte(REG_ADC_DTOP_05_L);
    if(!(regval1 & 0x02) && (regval2 & 0x80))
    {
        result = TRUE;
    }
    else // 1st trial failed, wait 10msec to try 2nd round
    {
        //ADC_DPUTSTR("drvADC_SetModewithPLLProtection 1st failed\n");
        //ADC_DPRINTF("@@HV_flag = 0x%x", regval1);
        //ADC_DPRINTF("@@PLL_flag = 0x%x", regval2);
        Delay1ms(10);
        drvADC_IsEnableFreeRun(TRUE);
        drvADC_SetAutoProst(TRUE);
        drvADC_SetADCModeSetting(enADCInput, u16PixelClk);
        drvADC_SetAutoProst(FALSE);
        Delay1ms(1);
        drvADC_AdjustHTotal(u16HTotal);
        drvADC_IsEnableFreeRun(FALSE);
        Delay1ms(10); // PLL lock status would depend on input frequency, low frequency would take longer time to lock
        regval1 = drvADC_ReadByte(REG_ADC_ATOP_0D_H);
        regval2 = drvADC_ReadByte(REG_ADC_DTOP_05_L);
        if(!(regval1 & 0x02) && (regval2 & 0x80))
        {
            result = TRUE;
        }
        else
            result = FALSE;
    }
#if ENABLE_ADC_RESET 
    // ADC software reset for °ª·Å¥Õ«Ìissue
    drvADC_Write2BytesMask(REG_ADC_ATOP_07_L, BIT0, BIT0);
    drvADC_Write2BytesMask(REG_ADC_ATOP_07_L, 0, BIT0);
    Delay1ms(2);
#if CHIP_ID == CHIP_TSUMF
	// Reset PLLA after set mode
    drvADC_Write2BytesMask(REG_ADC_ATOP_07_L, BIT1, BIT1);
    drvADC_Write2BytesMask(REG_ADC_ATOP_07_L, 0, BIT1);
    Delay1ms(2);
#endif	
#endif
    // trigger ADC phase dac alignment to avoid phase shift after DC off/on
    drvADC_Write2BytesMask(REG_ADC_ATOP_11_L, BIT8, BIT8);
    drvADC_Write2BytesMask(REG_ADC_ATOP_11_L, 0, BIT8);
    ADC_DPRINTF("drvADC_SetModewithPLLProtection result = %d\n", result);
    return result;
}
#endif //#ifdef DADCPLLPROTECTION

#ifdef _DGAIN_CAL_WITHOUT_INPUT_
//**************************************************************************
//  [Function Name]:
//                  drvADC_ADCAutoGainCalwithoutInput()
//  [Description]
//                  Do ADC gain calibration without input
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void drvADC_ADCAutoGainCalwithoutInput(void)
{
#if DADCNEWGAINCALMETHOD == 1

    UserPrefAdcRedGain  = 0x421;
    UserPrefAdcGreenGain = 0x421;
    UserPrefAdcBlueGain = 0x421;
    drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);

#elif DADCNEWGAINCALMETHOD == 2

    #if (CHIP_ID == CHIP_TSUMF)

    // Digital target gain: 1024/(1024-32)*PGA gain/0.7
    // ADC PGA gain:
    //  00: 1.0 / 01: 1.2 / 10: 0.8 / 11: 0.96
    switch(UserPrefAdcPgaGain)
    {
        case 0x00: // 1.474654
            UserPrefAdcRedGain = 1510;
            UserPrefAdcGreenGain = 1510;
            UserPrefAdcBlueGain = 1510;
            break;
        case 0x15: // 1.769584
            UserPrefAdcRedGain = 1812;
            UserPrefAdcGreenGain = 1812;
            UserPrefAdcBlueGain = 1812;
            break;
        case 0x2A: // 1.179723
            UserPrefAdcRedGain = 1208;
            UserPrefAdcGreenGain = 1208;
            UserPrefAdcBlueGain = 1208;
            break;
        case 0x3F: // 1.415667
            UserPrefAdcRedGain = 1449;
            UserPrefAdcGreenGain = 1449;
            UserPrefAdcBlueGain = 1449;
            break;
        default:
            break;
    }

    #else
    // Digital target gain: 1024/(1024-32)*PGA gain/0.7
    // ADC PGA gain:
    //  00: 0.861538 / 01: 0.969231 / 10: 1.076923 / 11: 1.184615
    switch(UserPrefAdcPgaGain)
    {
        case 0x00:
            UserPrefAdcRedGain = 0x514;
            UserPrefAdcGreenGain = 0x514;
            UserPrefAdcBlueGain = 0x514;
            break;
        case 0x15:
            UserPrefAdcRedGain = 0x5B7;
            UserPrefAdcGreenGain = 0x5B7;
            UserPrefAdcBlueGain = 0x5B7;
            break;
        case 0x2A:
            UserPrefAdcRedGain = 0x65A;
            UserPrefAdcGreenGain = 0x65A;
            UserPrefAdcBlueGain = 0x65A;
            break;
        case 0x3F:
            UserPrefAdcRedGain = 0x6FC;
            UserPrefAdcGreenGain = 0x6FC;
            UserPrefAdcBlueGain = 0x6FC;
            break;
        default:
            break;
    }


    #endif // #if (CHIP_ID == CHIP_TSUMF)
    drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);

#else

    // 1. backup ADC and Scaler related settings
    drvADC_BackupADCSetting();
    drvADC_BackupScalerSetting();

    // 2. Set free run clock at 720p
    _msADC_MaskExternalSync(TRUE);
    drvADC_SetFreeRunClock();

    // 3. Enable scaler pattern generator to generate Hsync and Vsync for line buffer
    drvADC_EnableScalerPG(E_PG_Htotal_256);

    // 4. Set ADC input LPF to lowest bandwidth
    drvADC_Write2BytesMask(REG_ADC_ATOP_23_L, 0xFFFF, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_ATOP_24_L, 0x00FF, 0x00FF);

    // 5. Calibrate ADC R/G/B gain setting value
    // Set R/G/B gain initial setting value at 0x0400
    // 6. Gain calibration target value for RGB color space mode is 182 (8bit data) 255 * 0.5 / 0.7 = 182
    // 7. Get the ADC digital output with Turn-On LDO and Set VCAL to 1.05v
    // 8. Get the ADC digital output with VCAL to 0.55v
    // 9. Calculate the difference between 1.05V and 0.55V of these 3 channels.
    //    Compare the difference value with target value (182 at RGB color space) to adjust R/G/B channel gain register
    drvADC_AutoGainAdjustment();

    // 10. recover ADC and scaler related settings & turn off free run clock & turn off internal Vcal
    _msADC_MaskExternalSync(FALSE);
    drvADC_RecoverADCSetting();
    drvADC_RecoverScalerSetting();

#endif // #if DADCNEWGAINCALMETHOD
}
#endif

#ifdef _DGAIN_CAL_WITH_INPUT_
//**************************************************************************
//  [Function Name]:
//                  drvADC_ADCAutoGainCalwithInput()
//  [Description]
//                  Do ADC gain calibration with input
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void drvADC_ADCAutoGainCalwithInput(void)
{
    WORD wStartAddress;

    // 1. backup ADC and Scaler related settings
    drvADC_BackupADCSetting();
    drvADC_BackupScalerSetting();

    // 2. Set free run clock at 720p
    //drvADC_SetFreeRunClock();

    // 3. Enable scaler pattern generator to generate Hsync and Vsync for line buffer
    //drvADC_EnableScalerPG(E_PG_Htotal_256);

    // 4. Set ADC input LPF to lowest bandwidth
    drvADC_Write2BytesMask(REG_ADC_ATOP_23_L, 0xFFFF, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_ATOP_24_L, 0x00FF, 0x00FF);
    drvADC_SetRGBOffsetCode(0, 0, 0);
    drvADC_SetRGBGainCode(0x421, 0x421, 0x421);

    // Find the white pattern region in line buffer
    wStartAddress = drvADC_FindWhiteRegion(0, 16);
    // 5. Calibrate ADC R/G/B gain setting value
    // Set R/G/B gain initial setting value at 0x0400
    // 6. Gain calibration target value for RGB color space mode is 182 (8bit data) 255 * 0.5 / 0.7 = 182
    // 7. Get the ADC digital output with Turn-On LDO and Set VCAL to 1.05v
    // 8. Get the ADC digital output with VCAL to 0.55v
    // 9. Calculate the difference between 1.05V and 0.55V of these 3 channels.
    //    Compare the difference value with target value (182 at RGB color space) to adjust R/G/B channel gain register
    drvADC_AutoGainAdjustment_WithInput(wStartAddress, 0, 32);

    // 10. recover ADC and scaler related settings & turn off free run clock & turn off internal Vcal
    drvADC_RecoverADCSetting();
    drvADC_RecoverScalerSetting();
}

#endif

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

#if 0
//**************************************************************************
//  [Function Name]:
//                  drvADC_Write2BytesMask()
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
static void drvADC_Write2BytesMask(WORD u16regadr, WORD u16val, WORD u16mask)
{
#if 1//CHIP_ID == CHIP_TSUMU
    msWrite2ByteMask( u16regadr, u16val, u16mask );
#else

    BYTE ucBank, ucCurBank, ucCurReg;

    ucBank = mStar_ReadByte(REGBK); // backup bank

    ucCurBank = (BYTE)(u16regadr >> 8);
    ucCurReg = (BYTE)(u16regadr & 0xFF);

    if(ucCurBank != ucBank)
        mStar_WriteByte(REGBK, ucCurBank);

    if(u16mask & 0x00FF)
        mStar_WriteByteMask(ucCurReg, (BYTE)(u16val&0xFF), (BYTE)(u16mask&0xFF));
    if(u16mask & 0xFF00)
        mStar_WriteByteMask(ucCurReg+1, (BYTE)(u16val>>8), (BYTE)(u16mask>>8));

    mStar_WriteByte(REGBK, ucBank);        // Switch to Original bank
#endif
}
#endif
#if 0
//**************************************************************************
//  [Function Name]:
//                  drvADC_ReadByte()
//  [Description]:
//                  Read ADC register value
//  [Precondition]:
//
//  [Arguments]:
//                 u16regadr : 16 bits register address
//  [Return]:
//                 register value(1 byte)
//**************************************************************************
static BYTE drvADC_ReadByte(WORD u16regadr)
{
#if 1//CHIP_ID == CHIP_TSUMU
    return msReadByte( u16regadr );
#else

    BYTE ucBank, ucCurBank, ucCurReg, retval;

    ucBank = mStar_ReadByte(REGBK); // backup bank

    ucCurBank = (BYTE)(u16regadr >> 8);
    ucCurReg = (BYTE)(u16regadr & 0xFF);

    if(ucCurBank != ucBank)
        mStar_WriteByte(REGBK, ucCurBank);
    retval = mStar_ReadByte(ucCurReg);

    mStar_WriteByte(REGBK, ucBank);        // Switch to Original bank

    return retval;
#endif
}
#endif
//**************************************************************************
//  [Function Name]:
//                  drvADC_WaitAutoStatusReady()
//  [Description]:
//                  Wait ready bit is ready
//  [Precondition]:
//
//  [Arguments]:
//                 regAddr: 16 bit register address
//                 regFlag: Ready bit
//  [Return]:
//
//**************************************************************************
BYTE drvADC_WaitAutoStatusReady(WORD regAddr, BYTE regFlag)
{
#if 0
    WORD waits=5000; // MCU 1x: 1500, 2x:3000, 3x: 4500
    while (waits-- && !(mStar_ReadByte(regAddr)&regFlag));
#else
    BYTE u8Value;
    SetTimOutConter(50);
    //bTimeOutCounterFlag=0;
    //u16TimeOutCounter=50;// 50 msec, at least two frames
    //bTimeOutCounterFlag=1;
    while( u16TimeOutCounter && !( (u8Value=msReadByte( regAddr ))&regFlag ) );
#endif
    //ADC_DPRINTF("drvADC_WaitAutoStatusReady= %d\n", u8Delay);
    return u8Value;
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
static void drvADC_LoadTable(TAB_Info* pTab_info)
{
    WORD i, u16Addr;
    BYTE u8Mask, u8Value, u8DoNotSet;

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
            //ADC_DPUTSTR("@@ register value Mask = ");
            //ADC_DPRINTF("0x%x", u16Addr);
            //ADC_DPRINTF("0x%x", u8Value);
            //ADC_DPRINTF("0x%x", u8Mask);
            msWriteByteMask( u16Addr, u8Value, u8Mask );
         }
NEXT:
         pTab_info->pTable+=pTab_info->u8TabCols; // next
    }
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetMux()
//  [Description]
//                  ADC set sync and data mux
//  [Arguments]:
//                  ipmux_type: sync / data
//  [Return]:
//
//**************************************************************************
static void drvADC_SetMux(ADC_MUX_TYPE ipmux_type)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCMUX_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_MUX_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCMUX_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = ipmux_type;

    ADC_DPRINTF("ADC Tbl:set mux %d \n",ipmux_type);
    drvADC_LoadTable(&Tab_info);

}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SearchFreqSetTableIndex()
//  [Description]:
//                  Search the Frequncy table index
//  [Precondition]:
//
//  [Arguments]:
//                 u16OriginalPixClk: xxxMHz
//  [Return]:
//
//**************************************************************************
static BYTE drvADC_SearchFreqSetTableIndex(WORD u16OriginalPixClk)
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
//                  drvADC_BackupADCSetting()
//  [Description]:
//                  Backup ADC settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_BackupADCSetting(void)
{
    memset(&gADCBackupSetting, 0, sizeof(gADCBackupSetting));
#if ENABLE_XTAL_LESS
    gADCBackupSetting.BWCoef = drvADC_ReadByte(REG_ADC_DTOP_02_L);
    gADCBackupSetting.DampCoef = drvADC_ReadByte(REG_ADC_DTOP_03_L);
#endif
    gADCBackupSetting.PLL_Ictrl = drvADC_ReadByte(REG_ADC_ATOP_09_H) & 0x03;
    gADCBackupSetting.PLL_Mult_Mod_216m = drvADC_ReadByte(REG_ADC_ATOP_0A_L) & 0x3F;
    gADCBackupSetting.LPF_Rch = drvADC_ReadByte(REG_ADC_ATOP_24_L) & 0x0F;
    gADCBackupSetting.LPF_Gch = drvADC_ReadByte(REG_ADC_ATOP_23_H) & 0x0F;
    gADCBackupSetting.LPF_Bch = drvADC_ReadByte(REG_ADC_ATOP_23_L) & 0x0F;
    gADCBackupSetting.Blacklevel_Rch = (drvADC_ReadByte(REG_ADC_DTOP_44_H) << 8 | drvADC_ReadByte(REG_ADC_DTOP_44_L)) & 0x03FF;
    gADCBackupSetting.Blacklevel_Gch = (drvADC_ReadByte(REG_ADC_DTOP_47_H) << 8 | drvADC_ReadByte(REG_ADC_DTOP_47_L)) & 0x03FF;
    gADCBackupSetting.Blacklevel_Bch = (drvADC_ReadByte(REG_ADC_DTOP_50_H) << 8 | drvADC_ReadByte(REG_ADC_DTOP_50_L)) & 0x03FF;
    gADCBackupSetting.Offset_Rch = (drvADC_ReadByte(REG_ADC_DTOP_46_H) << 8 | drvADC_ReadByte(REG_ADC_DTOP_46_L)) & 0x07FF;
    gADCBackupSetting.Offset_Gch = (drvADC_ReadByte(REG_ADC_DTOP_49_H) << 8 | drvADC_ReadByte(REG_ADC_DTOP_49_L)) & 0x07FF;
    gADCBackupSetting.Offset_Bch = (drvADC_ReadByte(REG_ADC_DTOP_52_H) << 8 | drvADC_ReadByte(REG_ADC_DTOP_52_L)) & 0x07FF;
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_RecoverADCSetting()
//  [Description]:
//                  Recover ADC settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_RecoverADCSetting(void)
{
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_06_L, 0x0000, 0x0080); // PLL Frequency Force Disable
#if ENABLE_XTAL_LESS
    drvADC_Write2BytesMask(REG_ADC_DTOP_02_L, 0x0900|gADCBackupSetting.BWCoef, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_03_L, gADCBackupSetting.DampCoef, 0xFFFF);
#else
    drvADC_Write2BytesMask(REG_ADC_DTOP_02_L, 0x0982, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_03_L, 0x0005, 0xFFFF);
#endif // #if ENABLE_XTAL_LESS
    drvADC_Write2BytesMask(REG_ADC_ATOP_0A_L, gADCBackupSetting.PLL_Mult_Mod_216m, 0x003F);
    drvADC_Write2BytesMask(REG_ADC_ATOP_09_L, gADCBackupSetting.PLL_Ictrl << 8, 0x0300);
    drvADC_Write2BytesMask(REG_ADC_ATOP_23_L, (gADCBackupSetting.LPF_Gch << 8)|gADCBackupSetting.LPF_Bch , 0x0F0F);
    drvADC_Write2BytesMask(REG_ADC_ATOP_24_L, gADCBackupSetting.LPF_Rch, 0x000F);
    drvADC_Write2BytesMask(REG_ADC_DTOP_44_L, gADCBackupSetting.Blacklevel_Rch, 0x03FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_47_L, gADCBackupSetting.Blacklevel_Gch, 0x03FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_50_L, gADCBackupSetting.Blacklevel_Bch, 0x03FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_46_L, gADCBackupSetting.Offset_Rch, 0x07FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_49_L, gADCBackupSetting.Offset_Gch, 0x07FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_52_L, gADCBackupSetting.Offset_Bch, 0x07FF);
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetFreeRunClock()
//  [Description]:
//                  Set ADC free-run clock to 40MHz
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_SetFreeRunClock(void)
{
    //drvADC_Write2BytesMask(REG_ADC_ATOP_01_L, 0x4001, 0xFFFF); // ADC enable
    //drvADC_Write2BytesMask(REG_ADC_ATOP_04_L, 0, 0xFFFF); // Turn on analog power down
    //drvADC_Write2BytesMask(REG_ADC_ATOP_06_L, 0, 0xFFFF); // Turn on gated
#if 0
    drvADC_Write2BytesMask(REG_ADC_ATOP_0A_L, 0x0008, 0x003F); // PLL_MULT = 0, DIV2_EN = 1'b1
    drvADC_Write2BytesMask(REG_ADC_ATOP_09_L, 0x0300, 0x0300); // ADCPLL_ICTRL = 2'b11
    drvADC_Write2BytesMask(REG_ADC_DTOP_02_L, 0xB82E, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_03_L, 0x0052, 0xFFFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_06_L, 0x0080, 0x0080); // PLL Frequency Force Enable
#else // revise the correct free-run 40MHz setting
    drvADC_Write2BytesMask( REG_ADC_ATOP_0A_L, 0x0010, 0x003F ); // [5]:DIV2_EN = 1'b0, [4:3]:APLL_MOD=2'b10, [2:0]: APLL_MULT = 0
#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
    drvADC_Write2BytesMask( REG_ADC_ATOP_09_L, 0x0200, 0x0700 ); // ADCPLL_ICTRL = 2'b010
#else
    drvADC_Write2BytesMask( REG_ADC_ATOP_09_L, 0x0200, 0x0300 ); // ADCPLL_ICTRL = 2'b10
#endif // #if (CHIP_ID == CHIP_TSUMC)
    drvADC_Write2BytesMask( REG_ADC_DTOP_02_L, 0x6656, 0xFFFF );
    drvADC_Write2BytesMask( REG_ADC_DTOP_03_L, 0x0066, 0xFFFF );
    drvADC_Write2BytesMask( REG_ADC_DTOP_06_L, 0x0080, 0x0080 ); // PLL Frequency Force Enable
#endif
}


#if ((CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
//**************************************************************************
//  [Function Name]:
//                  drvADC_MismatchOffsetCal()
//  [Description]:
//                  ADC mismatch offset calibration
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_MismatchLinearityCal(void)
{
    SetTimOutConter(10);
    //u16TimeOutCounter = 10; // 10 msec
    //bTimeOutCounterFlag=1;
    // Set Vcal 
    //drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0xF800, 0xF800); // [15]:ADC VCAL force enable, [14:11]: set Vcal
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0xF600, 0xFF00); // [15]:ADC VCAL force enable, [14:11]: set Vcal, [10:8]: 3'b 110, reg_rgb_self_gcal_pulse_sw

    do
    {
        // Enable linearity calibration
        drvADC_Write2BytesMask(REG_ADC_DTOP_66_L, 0, 0x0070); // [6:4]: Enable linearity calibration for R/G/B
        drvADC_Write2BytesMask(REG_ADC_DTOP_66_L, 0x0070, 0x0070); // [6:4]: Enable linearity calibration for R/G/B
        // Trigger
        drvADC_Write2BytesMask(REG_ADC_DTOP_68_L, 0x0000, 0x0700); // [10:8]: Rising trigger for R/G/B (Auto/Manual mode)
        drvADC_Write2BytesMask(REG_ADC_DTOP_68_L, 0x0700, 0x0700); // [10:8]: Rising trigger for R/G/B (Auto/Manual mode)
        drvADC_Write2BytesMask(REG_ADC_DTOP_68_L, 0x0000, 0x0700); // [10:8]: Rising trigger for R/G/B (Auto/Manual mode)
        // wait done
        ForceDelay1ms(1);
    } while( u16TimeOutCounter && ((drvADC_ReadByte(REG_ADC_DTOP_69_H) & 0x70) != 0x70) ); // [14:12]: linearity calibration done
    // Set Vcal 
    //drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0, 0xF800); // [15]:ADC VCAL force enable, [14:11]: set Vcal
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0, 0xFF00); // [15]:ADC VCAL force enable, [14:11]: set Vcal, [10:8]: 3'b 110, reg_rgb_self_gcal_pulse_sw

#if 1
    if((drvADC_ReadByte(REG_ADC_DTOP_69_H) & 0x70) == 0x70)
    {
        ADC_DPRINTF("@@ L_T= %d\n", u16TimeOutCounter);
        ADC_DPRINTF("@@ ADC R_ACCU_CH0 = 0x%x\n", msRead2Byte(REG_ADC_DTOP_6A_L));
        ADC_DPRINTF("@@ ADC R_ACCU_CH1 = 0x%x\n", msRead2Byte(REG_ADC_DTOP_6B_L));
        ADC_DPRINTF("@@ ADC G_ACCU_CH0 = 0x%x\n", msRead2Byte(REG_ADC_DTOP_6C_L));
        ADC_DPRINTF("@@ ADC G_ACCU_CH1 = 0x%x\n", msRead2Byte(REG_ADC_DTOP_6D_L));
        ADC_DPRINTF("@@ ADC B_ACCU_CH0 = 0x%x\n", msRead2Byte(REG_ADC_DTOP_6E_L));
        ADC_DPRINTF("@@ ADC B_ACCU_CH1 = 0x%x\n", msRead2Byte(REG_ADC_DTOP_6F_L));
    }
#endif
}
#endif // #if ((CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))

//**************************************************************************
//  [Function Name]:
//                  drvADC_MismatchOffsetCal()
//  [Description]:
//                  ADC mismatch offset calibration
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_MismatchOffsetCal(void)
{
    WORD wTimeOutCnt = 300;
    WORD wRch0val, wRch1val, wGch0val, wGch1val, wBch0val, wBch1val;
    WORD wRch0diff, wRch1diff, wGch0diff, wGch1diff, wBch0diff, wBch1diff;
    WORD wRchPivot, wGchPivot, wBchPivot;

    // 1. Turn on ADC offset calibration pluse
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0030, 0x0030);

    // 2. Enable ADC data low Vcal accumulator
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0080, 0x0080);

    ForceDelay1ms(1); // delay 1 msec

    // 3. Trigger ADC data low Vcal accumulator start
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0100, 0x0100);

    // 4. Wait ADC data accumulation done, ADC_ATO_74_L[2:0] = 3'b 111
    while ((wTimeOutCnt>0) && (!(drvADC_ReadByte(REG_ADC_ATOP_74_L) & 0x07)))
    {
        wTimeOutCnt--;
    }
    if(wTimeOutCnt == 0)
        ADC_DPUTSTR("ADC data accumulation time out\n");

    // 5. Read ADC channel 0 & 1 average value
    wRch0val = ((drvADC_ReadByte(REG_ADC_ATOP_62_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_62_L);
    wRch1val = ((drvADC_ReadByte(REG_ADC_ATOP_65_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_65_L);
    wGch0val = ((drvADC_ReadByte(REG_ADC_ATOP_68_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_68_L);
    wGch1val = ((drvADC_ReadByte(REG_ADC_ATOP_6B_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_6B_L);
    wBch0val = ((drvADC_ReadByte(REG_ADC_ATOP_6E_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_6E_L);
    wBch1val = ((drvADC_ReadByte(REG_ADC_ATOP_71_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_71_L);

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

    gADCMismatchAvgVal.wOffsetVal_Rch0  += wRch0val;
    gADCMismatchAvgVal.wOffsetVal_Rch1  += wRch1val;
    gADCMismatchAvgVal.wPivot_Rch       += wRchPivot;
    gADCMismatchAvgVal.wOffsetVal_Gch0  += wGch0val;
    gADCMismatchAvgVal.wOffsetVal_Gch1  += wGch1val;
    gADCMismatchAvgVal.wPivot_Gch       += wGchPivot;
    gADCMismatchAvgVal.wOffsetVal_Bch0  += wBch0val;
    gADCMismatchAvgVal.wOffsetVal_Bch1  += wBch1val;
    gADCMismatchAvgVal.wPivot_Bch       += wBchPivot;
#if 0
    drvADC_Write2BytesMask(REG_ADC_ATOP_56_L, wRch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_57_L, wRch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_75_L, wRchPivot, 0x03FF); // Set ADCR pivot-point value, choose minimum one
    drvADC_Write2BytesMask(REG_ADC_ATOP_5A_L, wGch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5B_L, wGch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_76_L, wGchPivot, 0x03FF); // Set ADCG pivot-point value, choose minimum one
    drvADC_Write2BytesMask(REG_ADC_ATOP_5E_L, wBch0diff, 0x07FF); // Set ADC channel 0 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5F_L, wBch1diff, 0x07FF); // Set ADC channel 1 offset compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_77_L, wBchPivot, 0x03FF); // Set ADCB pivot-point value, choose minimum one
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
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0015, 0x0095); // [7]: acc low disable, [4]: B mismatch offset enable, [2]: G mismatch offset enable, [0]: R mismatch offset enable,

    // 8. Turn off ADC offset calibration pulse
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0x0030);
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_MismatchGainCal()
//  [Description]:
//                  ADC mismatch gain calibration
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_MismatchGainCal(void)
{
    WORD wTimeOutCnt = 300;
    WORD wRch0val, wRch1val, wGch0val, wGch1val, wBch0val, wBch1val;
    WORD wRch0Div, wRch1Div, wGch0Div, wGch1Div, wBch0Div, wBch1Div;
#if DADCNEWGAINCALMETHOD
    WORD wTargetCode;
#endif

    // 1. Turn on LDO & select VCAL 1.05V, Turn on ADC gain calibration pulse
#if DADCCALBYBANDGAP
    drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, 0, BIT0); // turn off LDO
    drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, 0, BIT2); // Bandgap
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0, 0x000F); // disable LDO overwrite
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0xF800, 0xF800); // [15]:ADC VCAL force enable, [14:11]: set Vcal = 1.05V, 
#else
    drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT0, BIT0); // turn on LDO
    drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT2, BIT2); // LDO
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0003, 0x0003); // set Vcal = 1.05V
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // set Vcal from LDO
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0, 0xF800); // disable bandgap overwrite
#endif // DADCCALBYBANDGAP
    // Turn on self Gcal pulse
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // [10:8]: 3'b 110, reg_rgb_self_gcal_pulse_sw

    // 2. Enable ADC data high Vcal accumulator
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0040, 0x0040);

    ForceDelay1ms(1); // delay 1 msec

    // 3. Trigger ADC data high Vcal accumulator start
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0100, 0x0100);

    // 4. Wait ADC data accumulation done, ADC_ATO_74_L[2:0] = 3'b 111
    while ((wTimeOutCnt>0) && (!(drvADC_ReadByte(REG_ADC_ATOP_74_L) & 0x07)))
    {
        wTimeOutCnt--;
    }
    if(wTimeOutCnt == 0)
        ADC_DPUTSTR("ADC data high accumulation time out\n");

    // 5. Select Vcal 0.55V
#if DADCCALBYBANDGAP
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0xD800, 0xF800); // [15]:ADC VCAL force enable, [14:11]: set Vcal = 0.55V, 
#else
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0002, 0x0003); // reg_rgb_cal_ldo_sel_sw
#endif // DADCCALBYBANDGAP

    // 6. Disable ADC data high Vcal accumulator & Enable ADC data low Vcal accumulator
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0000, 0x0040);
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0080, 0x0080);

    ForceDelay1ms(1); // delay 1 msec

    // 7. Trigger ADC data low Vcal accumulator start
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x0100, 0x0100);

    // 8. Wait ADC data accumulation done, ADC_ATO_74_L[2:0] = 3'b 111
    wTimeOutCnt = 300;
    while ((wTimeOutCnt>0) && (!(drvADC_ReadByte(REG_ADC_ATOP_74_L) & 0x07)))
    {
        wTimeOutCnt--;
    }
    if(wTimeOutCnt == 0)
        ADC_DPUTSTR("ADC data low accumulation time out\n");

    // 9. Read ADC channel 0 & 1 difference value(Vcal_high - Vcal_low)
    wRch0val = ((drvADC_ReadByte(REG_ADC_ATOP_64_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_64_L);
    wRch1val = ((drvADC_ReadByte(REG_ADC_ATOP_67_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_67_L);
    wGch0val = ((drvADC_ReadByte(REG_ADC_ATOP_6A_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_6A_L);
    wGch1val = ((drvADC_ReadByte(REG_ADC_ATOP_6D_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_6D_L);
    wBch0val = ((drvADC_ReadByte(REG_ADC_ATOP_70_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_70_L);
    wBch1val = ((drvADC_ReadByte(REG_ADC_ATOP_73_H)&0x03) << 8) | drvADC_ReadByte(REG_ADC_ATOP_73_L);

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
#if DADCNEWGAINCALMETHOD == 1

    // target code : 1024 * 0.5 / 0.7 = 731
    //wTargetCode = 731;
    // target code : (1024-32) * 0.5 / 0.7 = 708
    wTargetCode = 708;

    //ADC_DPRINTF("Target Code = %d\n", wTargetCode);
    // R-ch
    wRch0Div = ((wTargetCode / wRch0val) << 10) | (WORD)(((DWORD)(wTargetCode%wRch0val)<<10)/wRch0val);
    wRch1Div = ((wTargetCode / wRch1val) << 10) | (WORD)(((DWORD)(wTargetCode%wRch1val)<<10)/wRch1val);
    // G-ch
    wGch0Div = ((wTargetCode / wGch0val) << 10) | (WORD)(((DWORD)(wTargetCode%wGch0val)<<10)/wGch0val);
    wGch1Div = ((wTargetCode / wGch1val) << 10) | (WORD)(((DWORD)(wTargetCode%wGch1val)<<10)/wGch1val);
    // B-ch
    wBch0Div = ((wTargetCode / wBch0val) << 10) | (WORD)(((DWORD)(wTargetCode%wBch0val)<<10)/wBch0val);
    wBch1Div = ((wTargetCode / wBch1val) << 10) | (WORD)(((DWORD)(wTargetCode%wBch1val)<<10)/wBch1val);

#elif DADCNEWGAINCALMETHOD == 2

    #if (CHIP_ID == CHIP_TSUMF)

    // ADC PGA gain:
    //  00: 1.0 / 01: 1.2 / 10: 0.8 / 11: 0.96
    //  target code : (1024-32)*0.5/PGA gain
    switch(UserPrefAdcPgaGain)
    {
        case 0x00:
            wTargetCode = 496;
            break;
        case 0x15:
            wTargetCode = 413;
            break;
        case 0x2A:
            wTargetCode = 620;
            break;
        case 0x3F:
            wTargetCode = 516;
            break;
        default:
            break;
    }

    #else
	
    // ADC PGA gain:
    //  00: 0.861538 / 01: 0.969231 / 10: 1.076923 / 11: 1.184615
    //  target code : (1024-32)*0.5/PGA gain
    switch(UserPrefAdcPgaGain)
    {
        case 0x00:
            wTargetCode = 576;
            break;
        case 0x15:
            wTargetCode = 512;
            break;
        case 0x2A:
            wTargetCode = 461;
            break;
        case 0x3F:
            wTargetCode = 419;
            break;
        default:
            break;
    }
	
    #endif // #if (CHIP_ID == CHIP_TSUMF) 
    
    //ADC_DPRINTF("Target Code = %d\n", wTargetCode);
    // R-ch
    wRch0Div = ((wTargetCode / wRch0val) << 10) | (WORD)(((DWORD)(wTargetCode%wRch0val)<<10)/wRch0val);
    wRch1Div = ((wTargetCode / wRch1val) << 10) | (WORD)(((DWORD)(wTargetCode%wRch1val)<<10)/wRch1val);
    // G-ch
    wGch0Div = ((wTargetCode / wGch0val) << 10) | (WORD)(((DWORD)(wTargetCode%wGch0val)<<10)/wGch0val);
    wGch1Div = ((wTargetCode / wGch1val) << 10) | (WORD)(((DWORD)(wTargetCode%wGch1val)<<10)/wGch1val);
    // B-ch
    wBch0Div = ((wTargetCode / wBch0val) << 10) | (WORD)(((DWORD)(wTargetCode%wBch0val)<<10)/wBch0val);
    wBch1Div = ((wTargetCode / wBch1val) << 10) | (WORD)(((DWORD)(wTargetCode%wBch1val)<<10)/wBch1val);

#else

    // R-ch
    if(wRch0val > wRch1val)
    {
        if((wRch0val / wRch1val) > 1)
        {
            wRch1Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("drvADC_MismatchGainCal: ADC Rch1 overflow = %f\n", (float)wRch0val/wRch1val);
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
            //ADC_DPRINTF("drvADC_MismatchGainCal: ADC Rch0 overflow = %f\n", (float)wRch1val/wRch0val);
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
            //ADC_DPRINTF("drvADC_MismatchGainCal: ADC Gch1 overflow = %f\n", (float)wGch0val/wGch1val);
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
            //ADC_DPRINTF("drvADC_MismatchGainCal: ADC Gch0 overflow = %f\n", (float)wGch1val/wGch0val);
        }
        else
        {
            wGch0Div = ((wGch1val / wGch0val) << 10) | (WORD)(((DWORD)(wGch1val%wGch0val)<<10)/wGch0val);
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
            //ADC_DPRINTF("drvADC_MismatchGainCal: ADC Bch1 overflow = %f\n", (float)wBch0val/wBch1val);
        }
        else
        {
            wBch1Div = ((wBch0val / wBch1val) << 10) | (WORD)(((DWORD)(wBch0val%wBch1val)<<10)/wBch1val);
        }
        wBch0Div = 0x0400;
    }
    else if (wBch0val < wBch1val)
    {
        if((wBch1val / wBch0val) > 1)
        {
            wBch0Div = 0x0400; // ratio is 1, 1<<10
            //ADC_DPRINTF("drvADC_MismatchGainCal: ADC Bch0 overflow = %f\n", (float)wBch1val/wBch0val);
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
    
#endif // #if DADCNEWGAINCALMETHOD

    gADCMismatchAvgVal.wGainVal_Rch0 += (wRch0Div & 0x07FF);
    gADCMismatchAvgVal.wGainVal_Rch1 += (wRch1Div & 0x07FF);
    gADCMismatchAvgVal.wGainVal_Gch0 += (wGch0Div & 0x07FF);
    gADCMismatchAvgVal.wGainVal_Gch1 += (wGch1Div & 0x07FF);
    gADCMismatchAvgVal.wGainVal_Bch0 += (wBch0Div & 0x07FF);
    gADCMismatchAvgVal.wGainVal_Bch1 += (wBch1Div & 0x07FF);
#if 0
    drvADC_Write2BytesMask(REG_ADC_ATOP_58_L, wRch0Div, 0x07FF); // Set ADC channel 0 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_59_L, wRch1Div, 0x07FF); // Set ADC channel 1 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5C_L, wGch0Div, 0x07FF); // Set ADC channel 0 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_5D_L, wGch1Div, 0x07FF); // Set ADC channel 1 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_60_L, wBch0Div, 0x07FF); // Set ADC channel 0 gain compensated code
    drvADC_Write2BytesMask(REG_ADC_ATOP_61_L, wBch1Div, 0x07FF); // Set ADC channel 1 gain compensated code
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
    drvADC_Write2BytesMask(REG_ADC_ATOP_55_L, 0x002A, 0x00AA); // [7]: acc low disable, [5]: B mismatch gain enable, [3]: G mismatch gain enable, [1]: R mismatch gain enable,

    // 12. Turn off ADC offset calibration pulse
    drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0xFF0F); // [15]:ADC VCAL force enable, [14:11]: set Vcal, [10:8]: Turn off self Gcal pulse, [3:2]: reg_rgb_cal_trim_ldo_sel_sw, [1:0]: reg_rgb_cal_ldo_sel_sw
}


#define DGain_OverFlow_R_ch         BIT7
#define DGain_NoneOverFlow_R_ch     BIT6
#define DGain_OverFlow_G_ch         BIT5
#define DGain_NoneOverFlow_G_ch     BIT4
#define DGain_OverFlow_B_ch         BIT3
#define DGain_NoneOverFlow_B_ch     BIT2
#ifndef ADCRB_SWAP
#define ADCRB_SWAP 0
#endif
#ifndef ADJ_GAIN_RATIO
#define ADJ_GAIN_RATIO      1
#endif
//**************************************************************************
//  [Function Name]:
//                  drvADC_AutoAdjustAdcGain()
//  [Description]:
//                  Check input sync is lost or not
//  [Precondition]:
//                  Do auto gain adjustment with input source
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
static Bool drvADC_AutoAdjustAdcGain(BYTE u8VSyncTime)
{
    WORD rGain, gGain, bGain;
    WORD maxRGain, maxGGain, maxBGain;
    WORD minRGain, minGGain, minBGain;
    BYTE retry=12;
    BYTE adcStatus;
    BYTE atgFlags=0;

    maxRGain=maxGGain=maxBGain=0xFFF;
    minRGain=minGGain=minBGain=0;

    while (retry--)
    {
        rGain=((WORD)maxRGain+minRGain)/2;
        gGain=((WORD)maxGGain+minGGain)/2;
        bGain=((WORD)maxBGain+minBGain)/2;
        drvADC_SetRGBGainCode(rGain, gGain, bGain);
        Delay1ms(u8VSyncTime);
        // Wait scaler auto gain is done, SC0_78[1] = 1
        adcStatus=drvADC_WaitAutoStatusReady(SC0_78, BIT1);
        //adcStatus=msReadByte(SC0_78);

        if( adcStatus & BIT7 )
        {
            rGain--;
            atgFlags|=DGain_OverFlow_R_ch;
        }
        else
        {
            rGain++;
            atgFlags|=DGain_NoneOverFlow_R_ch;
        }

        if( adcStatus & BIT6 )
        {
            gGain--;
            atgFlags|=DGain_OverFlow_G_ch;
        }
        else
        {
            gGain++;
            atgFlags|=DGain_NoneOverFlow_G_ch;
        }

        if( adcStatus & BIT5 )
        {
            bGain--;
            atgFlags|=DGain_OverFlow_B_ch;
        }
        else
        {
            bGain++;
            atgFlags|=DGain_NoneOverFlow_B_ch;
        }

        drvADC_SetRGBGainCode(rGain, gGain, bGain);
        Delay1ms(u8VSyncTime);
        // Wait scaler auto gain is done, SC0_78[1] = 1
        adcStatus=drvADC_WaitAutoStatusReady(SC0_78, BIT1);
        //adcStatus=msReadByte(SC0_78);

        if( adcStatus & BIT7 )
        {
            maxRGain=rGain+1;
            atgFlags|=DGain_OverFlow_R_ch;
        }
        else
        {
            minRGain=rGain-1;
            atgFlags|=DGain_NoneOverFlow_R_ch;
        }

        if( adcStatus & BIT6 )
        {
            maxGGain=gGain+1;
            atgFlags|=DGain_OverFlow_G_ch;
        }
        else
        {
            minGGain=gGain-1;
            atgFlags|=DGain_NoneOverFlow_G_ch;
        }

        if( adcStatus & BIT5 )
        {
            maxBGain=bGain+1;
            atgFlags|=DGain_OverFlow_B_ch;
        }
        else
        {
            minBGain=bGain-1;
            atgFlags|=DGain_NoneOverFlow_B_ch;
        }

        if (CheckSyncLoss())
            return FALSE;
    }

    if ((atgFlags&0xFC)!=0xFC)
        return FALSE;

    if (rGain>4000 || gGain>4000 || bGain>4000) //Jison 101029
        return FALSE;

#if 0  //Steven101220
    UserPrefAdcRedGain = ((DWORD)rGain-1)*(100+ADJ_GAIN_RATIO)/100;
    UserPrefAdcGreenGain = ((DWORD)gGain-1)*(100+ADJ_GAIN_RATIO)/100;
    UserPrefAdcBlueGain = ((DWORD)bGain-1)*(100+ADJ_GAIN_RATIO)/100;
#else
    UserPrefAdcRedGain  = rGain-1;
    UserPrefAdcGreenGain = gGain-1;
    UserPrefAdcBlueGain = bGain-1;
#endif
    drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);

    return TRUE;
}

static void drvADC_SetPGAGainCode(BYTE ucAdcPgaGain)  //Steven110329, Solve check PGA gain issue.
{
    drvADC_Write2BytesMask(REG_ADC_DTOP_55_L, ucAdcPgaGain, 0x03F);
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_CheckPGAGain()
//  [Description]:
//                  Check input signal is over PGA gain or not
//  [Precondition]:
//                  00: 1.0V, 01: 1.2V, 10: 0.8V, 11: 0.96V  => TSUMF
//                  00: 0.8V, 01: 0.9V, 10: 1.0V, 11: 1.1V
//                  The default PGA gain in Rose is 0.8V. To avoid the input signal is over 0.8V, this function is used to
//                  detect and adjust PGA gain. The maximum PGA gain would be limited in 1.0V
//  [Arguments]:
//                  vsyncTime: frame rate of the current input
//  [Return]:
//                  TRUE: done, FALSE: fail
//**************************************************************************
static BYTE drvADC_CheckPGAGain(BYTE vsyncTime)
{
    BYTE u8CurPGA, adcStatus;

#if (CHIP_ID == CHIP_TSUMF)
    u8CurPGA = 0x2A;
#else
    u8CurPGA = 0x00;
#endif //#if (CHIP_ID == CHIP_TSUMF)

    drvADC_Write2BytesMask(REG_ADC_DTOP_55_L, u8CurPGA, 0x003F);
    msWriteByteMask(SC0_78, BIT3, BIT3); // output is overflow or underflow result
    Delay1ms(vsyncTime);
    // Wait scaler auto gain is done, BK0_78[1] = 1
    adcStatus=drvADC_WaitAutoStatusReady( SC0_78, BIT1 );
    ADC_DPRINTF("adcStatus = 0x%x\n", adcStatus);
    ADC_DPRINTF("--adcStatus = 0x%x\n", msReadByte(REG_ADC_DTOP_55_L) & 0x3F);

    while( adcStatus & ( BIT5 | BIT6 | BIT7 ) )
    {
#if (CHIP_ID == CHIP_TSUMF)
        if(u8CurPGA == 0x00) // limited the maxi VPP = 1.0V
            break;
        else
        {
            u8CurPGA = ((u8CurPGA == 0x3F) ? 0x00 : (u8CurPGA+0x15));
            drvADC_Write2BytesMask( REG_ADC_DTOP_55_L, u8CurPGA, 0x003F );
            Delay1ms(vsyncTime);
            // Wait scaler auto gain is done, SC0_78[1] = 1
            adcStatus=drvADC_WaitAutoStatusReady( SC0_78, BIT1 );
            //adcStatus=msReadByte(SC0_78);
        }
#else
        if(u8CurPGA == 0x2A) // limited the maxi VPP = 1.0V
            break;
        else
        {
            u8CurPGA += 0x15;
            drvADC_Write2BytesMask( REG_ADC_DTOP_55_L, u8CurPGA, 0x003F );
            Delay1ms(vsyncTime);
            // Wait scaler auto gain is done, SC0_78[1] = 1
            adcStatus=drvADC_WaitAutoStatusReady( SC0_78, BIT1 );
            //adcStatus=msReadByte(SC0_78);
        }
#endif // #if (CHIP_ID == CHIP_TSUMF)
        ADC_DPRINTF("adcStatus = 0x%x\n", adcStatus);
    }
    ADC_DPRINTF("PGA Gain = 0x%x\n", u8CurPGA);
    msWriteByteMask(SC0_78, 0, BIT3);
    return u8CurPGA;
}

#if DADCPLLPROTECTION
//**************************************************************************
//  [Function Name]:
//                  drvADC_IsEnableFreeRun()
//  [Description]:
//                  Enable/Disable ADC free run mode
//  [Precondition]:
//                  Use to protect ADC PLL while mode changing
//  [Arguments]:
//                  bflag: TRUE => enable, FALSE => disable
//  [Return]:
//
//**************************************************************************
static void drvADC_IsEnableFreeRun(Bool bflag)
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
    drvADC_LoadTable(&Tab_info);

#if ENABLE_XTAL_LESS
    if(!bflag)
    {
        drvADC_Write2BytesMask(REG_ADC_DTOP_02_L, gADCBackupSetting.BWCoef, 0x00FF);
        drvADC_Write2BytesMask(REG_ADC_DTOP_03_L, gADCBackupSetting.DampCoef, 0x00FF);
    }
#endif

    //ADC_DPRINTF("ADC Tbl:drvADC_IsEnableFreeRun %d\n", bflag);
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetAutoProst()
//  [Description]:
//                  Set/Release small loop auto porst
//  [Precondition]:
//                  Use to protect ADC PLL while mode changing
//  [Arguments]:
//                  bflag: TRUE => enable, FALSE => disable
//  [Return]:
//
//**************************************************************************
static void drvADC_SetAutoProst(Bool bflag)
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

    drvADC_LoadTable(&Tab_info);

    //ADC_DPRINTF("ADC Tbl:drvADC_SetAutoProst %d\n", bflag);
}
#endif // #ifdef DADCPLLPROTECTION


#ifdef _DGAIN_CAL_WITHOUT_INPUT_ //????
//**************************************************************************
//  [Function Name]:
//                  drvADC_EnableScalerPG()
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
static void drvADC_EnableScalerPG(msADCScalerPG_HtotalSel ht_sel)
{
    WORD whact;

    //msWriteByteMask(SC0_02, 0x00, 0x80); // disable free-run
    msWriteByteMask(SC0_01, 0, 0x01); // disable double buffer
    msWriteByteMask(SC0_02, 0x00|BIT2, 0x07); // [2]:use Hsync only, [1:0]:ISEL = analog1
    msWrite2Byte(SC0_07, 0x0010); // SPRHST = 0x10
    whact = 256*(ht_sel+1) - 16;
    msWrite2ByteMask(SC0_0B, whact,SC_MASK_H); // SPRHDC = htsel - 0x10
    // Vtotal is 64 lines, Vstart = 0x10, Vactive = 0x30
    msWrite2Byte(SC0_05, 0x0010); // SPRVST = 0x10
    msWrite2ByteMask(SC0_09, 0x0030,SC_MASK_V); // SPRVDC = 0x30
#if (CHIP_ID == CHIP_TSUMU)
    msWriteByteMask(SC0_63, (ht_sel<<6)|0x30, 0xF0); //[7:6]: Htotal, [5]: Enable PG's Hsync/Vsync/Hde/Vde to IP, [4]: Pattern gen engine enable
#elif (CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUM2 || CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY) //
    msWriteByteMask(REG_2E42, ht_sel<<1|0x19, 0x1F); // Rosen: [4]: No input ADC calibration enable, [3]: Enable PG's Hsync/Vsync/Hde/Vde to IP, [2:1]: Htotal, [0]: Pattern gen engine enable
#elif ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
    msWrite2ByteMask(REG_2E24, whact, 0x1FFF); // reg_hde
    msWriteByteMask(REG_2E26, 0x05, 0xFF); // reg_hfp_w
    msWriteByteMask(REG_2E28, 0x06, 0xFF); // reg_hsync_w
    msWrite2ByteMask(REG_2E2A, 256*(ht_sel+1), 0x1FFF); // reg_htotal
    msWrite2ByteMask(REG_2E2C, 0x30, 0x1FFF); // reg_vde
    msWriteByteMask(REG_2E2E, 0x05, 0x7F); // reg_vfp_w
    msWriteByteMask(REG_2E30, 0x06, 0x3F); // reg_vsync_w
    msWrite2ByteMask(REG_2E32, 0x40, 0x1FFF); // reg_vtotal
    msWrite2ByteMask(REG_2E20, BIT0, BIT0); // Timing gen enable
    msWriteByteMask(REG_2E42, 0x18, 0x18); // Rosen: [4]: No input ADC calibration enable, [3]: Enable PG's Hsync/Vsync/Hde/Vde to IP
#endif
}

#if 1
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
        drvADC_Write2BytesMask(REG_ADC_ATOP_04_L, BIT14, BIT14); // power down Hsync_Cmp
    else
        drvADC_Write2BytesMask(REG_ADC_ATOP_04_L, 0, BIT14); // power on Hsync_Cmp
}
#endif

//**************************************************************************
//  [Function Name]:
//                  drvADC_SetInternalVcal()
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
static void drvADC_SetInternalVcal(msADC_InternalVcal vcal)
{
    switch(vcal)
    {
        case E_Vcal_0_55V:
        #if DADCCALBYBANDGAP
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, 0, BIT0); // turn off LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, 0, BIT2); // Bandgap
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0, 0x000F); // disable LDO overwrite
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0xD800, 0xF800); // [15]:ADC VCAL force enable, [14:11]: set Vcal = 0.55V, 
        #else
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT0, BIT0); // turn on LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT2, BIT2); // LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0002, 0x0003); // set Vcal = 0.55V
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // set Vcal from LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0, 0xF800); // disable bandgap overwrite
        #endif // DADCCALBYBANDGAP
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // turn on Gcal pulse
            break;

        case E_Vcal_1_05V:
        #if DADCCALBYBANDGAP
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, 0, BIT0); // turn off LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, 0, BIT2); // Bandgap
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0000, 0x000F); // disable LDO overwrite
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0xF800, 0xF800); // [15]:ADC VCAL force enable, [14:11]: set Vcal = 1.05V, 
        #else
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT0, BIT0); // turn on LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_34_L, BIT2, BIT2); // LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0003, 0x0003); // set Vcal = 1.05V
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // set Vcal from LDO
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0, 0xF800); // disable bandgap overwrite
        #endif // DADCCALBYBANDGAP
            drvADC_Write2BytesMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // turn on Gcal pulse
            break;
        default:
            break;
    }
}
#endif // #ifdef _DGAIN_CAL_WITHOUT_INPUT_


#if (defined(_DGAIN_CAL_WITH_INPUT_) || defined(_DGAIN_CAL_WITHOUT_INPUT_))
//**************************************************************************
//  [Function Name]:
//                  drvADC_BackupScalerSetting()
//  [Description]:
//                  Backup scaler settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_BackupScalerSetting(void)
{
    memset(&gScalerBackupSetting, 0, sizeof(gScalerBackupSetting));
    gScalerBackupSetting.DBuffer = msReadByte(SC0_01);
    gScalerBackupSetting.ISEL = msReadByte( SC0_02 );
    gScalerBackupSetting.SPRHST = msRead2Byte( SC0_07 ) & 0x07FF; //???? 0xFFF
    gScalerBackupSetting.SPRHDC = msRead2Byte( SC0_0B ) & SC_MASK_H;
    gScalerBackupSetting.SPRVST = msRead2Byte( SC0_05 ) & 0x07FF; //???? 0xFFF
    gScalerBackupSetting.SPRVDC = msRead2Byte( SC0_09 ) & SC_MASK_V;
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_RecoverScalerSetting()
//  [Description]:
//                  Recover scaler settings
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void drvADC_RecoverScalerSetting(void)
{
    msWriteByteMask(SC0_01, gScalerBackupSetting.DBuffer, 0xFF);
    msWriteByteMask( SC0_02, gScalerBackupSetting.ISEL, 0xFF );
    msWrite2ByteMask( SC0_07, gScalerBackupSetting.SPRHST, 0x07FF ); //????
    msWrite2ByteMask( SC0_0B, gScalerBackupSetting.SPRHDC, SC_MASK_H );
    msWrite2ByteMask( SC0_05, gScalerBackupSetting.SPRVST, 0x07FF );  //????
    msWrite2ByteMask( SC0_09, gScalerBackupSetting.SPRVDC, SC_MASK_V );
    //msWriteByteMask(REG_2FC0, 0x00, 0x1F); //[4]: OSD to IP disable, [3]: disable PG's Hsync/Vsync/Hde/Vde to IP, [2]: Pattern gen engine disable, [1:0]: Htotal is 256
#if ( (CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY) || (CHIP_ID == CHIP_TSUMU) ) //
    msWriteByteMask( SC0_63, 0, 0xF0 ); //[7:6]: Htotal, [5]: Enable PG's Hsync/Vsync/Hde/Vde to IP, [4]: Pattern gen engine enable
#elif (CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUM2) //
    msWriteByteMask(REG_2E42, 0, 0x1F); // Rosen: [4]: No input ADC calibration enable, [3]: Enable PG's Hsync/Vsync/Hde/Vde to IP, [2:1]: Htotal, [0]: Pattern gen engine enable
#elif ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
    msWrite2ByteMask(REG_2E20, 0, BIT0); // Timing gen enable
    msWriteByteMask(REG_2E42, 0, 0x18); // Rosen: [4]: No input ADC calibration enable, [3]: Enable PG's Hsync/Vsync/Hde/Vde to IP
#endif
}

//**************************************************************************
//  [Function Name]:
//                  drvADC_IsReadLineBuffer()
//  [Description]:
//                  It's used to read scaler line buffer
//  [Precondition]:
//
//  [Arguments]:
//                 bflag: TRUE: reading, FALSE: recover to the normal state
//  [Return]:
//
//**************************************************************************
static Bool drvADC_IsReadLineBuffer(Bool bflag)
{
    Bool result=TRUE;
    BYTE retry=5;

    if(bflag)
    {
        msWriteByteMask( SC5_32, BIT6, BIT6 ); // latch data in line buffer
        while(retry--)
        {
            ForceDelay1ms( 20 );
            if( msReadByte( SC5_32 ) & BIT7 ) // Check ready bit for reading line buffer
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
        msWriteByteMask( SC5_32, 0, BIT6 ); // disable latch data in line buffer
        //ADC_DPUTSTR("Turn off reading Line buffer \n");
    }
    return result;
}

//                  Adjust gain to approach target value 182(8 bits) => 255 * 0.5/0.7 = 182
//**************************************************************************
//  [Function Name]:
//                  drvADC_GetRGBLineBufAverageData()
//  [Description]:
//                  Get the average line buffer data for R/G/B gain calibration
//                  SC5_32[4:0]31[7:0] = 0 ~ 2560(total is 2560), each has two pixels and format is following
//                  SC5_35[7:0]: R0, SC5_36[7:0]: G0, SC5_37[7:0]: B0
//                  SC5_38[7:0]: R1, SC5_39[7:0]: G1, SC5_3A[7:0]: B1
//  [Precondition]:
//
//  [Arguments]:
//                 wLineBufAddress: line buffer address, it should be less than PG Htotal
//                 LineBuf: choose one line to read, 0 ~ 3
//                 AvgCnt: average pixel count
//  [Return]:
//                 msADCAvgVal: R/G/B average data
//**************************************************************************
static msADCAvgVal drvADC_GetRGBLineBufAverageData(WORD wLineBufAddress, BYTE LineBuf, BYTE AvgCnt)
{
    BYTE i;
    WORD wAvgRdata=0, wAvgGdata=0, wAvgBdata=0;
    msADCAvgVal pAutoAdc;

    memset(&pAutoAdc, 0 , sizeof(pAutoAdc));

#if (CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY) //

    msWriteByteMask( SC5_33, ( LineBuf & 0x03 ) << 3, 0x18 ); // select to use which line buffer, 0 ~ 3
    for (i=0; i<AvgCnt; i++)
    {
        if (i%2 == 0)
        {
            msWrite2ByteMask(SC5_31, wLineBufAddress+(i/2), 0x0FFF); // config line buffer address
            // B
            msWriteByteMask(SC5_33, 0, 0x07); // select B0 to read
            wAvgBdata += msReadByte(SC5_35); //read line buffer data
            // G
            msWriteByteMask(SC5_33, 1, 0x07); // select G0 to read
            wAvgGdata += msReadByte(SC5_35); //read line buffer data
            // R
            msWriteByteMask(SC5_33, 2, 0x07); // select R0 to read
            wAvgRdata += msReadByte(SC5_35); //read line buffer data
        }
        else
        {
            // B
            msWriteByteMask(SC5_33, 3, 0x07); // select B1 to read
            wAvgBdata += msReadByte(SC5_35); //read line buffer data
            // G
            msWriteByteMask(SC5_33, 4, 0x07); // select G1 to read
            wAvgGdata += msReadByte(SC5_35); //read line buffer data
            // R
            msWriteByteMask(SC5_33, 5, 0x07); // select R1 to read
            wAvgRdata += msReadByte(SC5_35); //read line buffer data
        }
    }

#elif (CHIP_ID == CHIP_TSUMU) //

    msWriteByteMask( SC5_33, ( LineBuf & 0x03 ) << 3, 0x18 ); // select to use which line buffer, 0 ~ 3
    if(AvgCnt == 1)
    {
        msWrite2ByteMask( SC5_31, wLineBufAddress, 0x1FFF ); // config line buffer address
        wAvgRdata = msReadByte( SC5_35 ); //read R0 line buffer data
        wAvgGdata = msReadByte( SC5_36 ); //read G0 line buffer data
        wAvgBdata = msReadByte( SC5_37 ); //read B0 line buffer data		
    }
    else
    {
        for( i = 0; i < AvgCnt/2; i++ )
        {
            msWrite2ByteMask( SC5_31, wLineBufAddress + ( i / 2 ), 0x1FFF ); // config line buffer address
            wAvgRdata = wAvgRdata + msReadByte( SC5_35 ) + msReadByte( SC5_38 ); //read R0+R1 line buffer data
            wAvgGdata = wAvgGdata + msReadByte( SC5_36 ) + msReadByte( SC5_39 ); //read G0+G1 line buffer data
            wAvgBdata = wAvgBdata + msReadByte( SC5_37 ) + msReadByte( SC5_3A ); //read B0+B1 line buffer data
        }
    }

#elif (CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF)) //

    for( i = 0; i < AvgCnt; i++ )
    {
        msWrite2ByteMask( SC5_31, LineBuf*1500+wLineBufAddress + ( i / 3 ), 0x7FF ); // config line buffer address
        msWriteByteMask( SC5_33, i%3, 0x03 ); // [1:0] : LB packed pixel selection, 00: 1st pixel, 01: 2nd pixel, 10: 3rd pixel
        wAvgRdata += msReadByte( SC5_35 ); //read R0 line buffer data
        wAvgGdata += msReadByte( SC5_36 ); //read G0 line buffer data
        wAvgBdata += msReadByte( SC5_37 ); //read B0 line buffer data
    }

#endif

    pAutoAdc.AVG_Bch = wAvgBdata/AvgCnt;
    pAutoAdc.AVG_Gch = wAvgGdata/AvgCnt;
    pAutoAdc.AVG_Rch = wAvgRdata/AvgCnt;

    return pAutoAdc;
}

#endif // #if (defined(_DGAIN_CAL_WITH_INPUT_) || defined(_DGAIN_CAL_WITHOUT_INPUT_))

#ifdef _DGAIN_CAL_WITHOUT_INPUT_
//**************************************************************************
//  [Function Name]:
//                  drvADC_AutoGainAdjustment()
//  [Description]:
//                  Adjust gain to approach target value 182(8 bits) => 255 * 0.5/0.7 = 182
//  [Precondition]:
//
//  [Arguments]:
//
//  [Return]:
//                  status
//**************************************************************************
static Bool drvADC_AutoGainAdjustment(void)
{
    xdata BYTE retry=11;
    xdata BYTE CheckFlag=0x00;
    xdata BYTE Diff;
    xdata msADCAvgVal pAutoAdc_1_05V;
    xdata msADCAvgVal pAutoAdc_0_55V;
    xdata WORD InitADCGainVal = 0x0400;
    xdata WORD rGain = InitADCGainVal;
    xdata WORD gGain = InitADCGainVal;
    xdata WORD bGain = InitADCGainVal;
    xdata WORD GainOffset_R = InitADCGainVal >> 1;
    xdata WORD GainOffset_G = InitADCGainVal >> 1;
    xdata WORD GainOffset_B = InitADCGainVal >> 1;
    xdata BYTE Direction_R = 0;
    xdata BYTE Direction_G = 0;
    xdata BYTE Direction_B = 0;


    // 5. Calibrate ADC R/G/B gain setting value
    // Set R/G/B gain initial setting value at 0x0400
    drvADC_Write2BytesMask(REG_ADC_DTOP_45_L, InitADCGainVal, 0x0FFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_48_L, InitADCGainVal, 0x0FFF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_51_L, InitADCGainVal, 0x0FFF);
    // Set ADC R/G/B Black and offset level to 0.8V(511)
    drvADC_Write2BytesMask(REG_ADC_DTOP_44_L, 0x01FF, 0x03FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_47_L, 0x01FF, 0x03FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_50_L, 0x01FF, 0x03FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_46_L, 0x01FF, 0x07FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_49_L, 0x01FF, 0x07FF);
    drvADC_Write2BytesMask(REG_ADC_DTOP_52_L, 0x01FF, 0x07FF);

    // 6. Gain calibration target value for RGB color space mode is 182 (8bit data) 255 * 0.5 / 0.7 = 182
    // 7. Get the ADC digital output with Turn-On LDO and Set VCAL to 1.05v
    // 8. Get the ADC digital output with VCAL to 0.55v
    // 9. Calculate the difference between 1.05V and 0.55V of these 3 channels.
    //    Compare the difference value with target value (182 at RGB color space) to adjust R/G/B channel gain register
    //_msADC_MaskExternalSync(TRUE);

    while (retry--)
    {
        // Vcal = 1.05V
        drvADC_SetInternalVcal(E_Vcal_1_05V);
        ForceDelay1ms(16);
        drvADC_IsReadLineBuffer(TRUE);
        pAutoAdc_1_05V = drvADC_GetRGBLineBufAverageData(0x40, 0, 32); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
        drvADC_IsReadLineBuffer(FALSE);
        // Vcal = 0.55V
        drvADC_SetInternalVcal(E_Vcal_0_55V);
        ForceDelay1ms(16);
        drvADC_IsReadLineBuffer(TRUE);
        pAutoAdc_0_55V = drvADC_GetRGBLineBufAverageData(0x40, 0, 32); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
        drvADC_IsReadLineBuffer(FALSE);

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
            drvADC_Write2BytesMask(REG_ADC_DTOP_45_L, rGain, 0x0FFF);
            drvADC_Write2BytesMask(REG_ADC_DTOP_48_L, gGain, 0x0FFF);
            drvADC_Write2BytesMask(REG_ADC_DTOP_51_L, bGain, 0x0FFF);
        }
        else
        {
            drvADC_Write2BytesMask(REG_ADC_DTOP_45_L, rGain, 0x0FFF);
            drvADC_Write2BytesMask(REG_ADC_DTOP_48_L, gGain, 0x0FFF);
            drvADC_Write2BytesMask(REG_ADC_DTOP_51_L, bGain, 0x0FFF);
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
        ADC_DPRINTF("Rgain = 0x%x", rGain);
        ADC_DPRINTF("Ggain = 0x%x", gGain);
        ADC_DPRINTF("Bgain = 0x%x", bGain);
        return TRUE;
    }
    else
    {
        ADC_DPUTSTR("Auto gain fail !!\n");
        ADC_DPRINTF("Rgain = 0x%x", rGain);
        ADC_DPRINTF("Ggain = 0x%x", gGain);
        ADC_DPRINTF("Bgain = 0x%x", bGain);
        return FALSE;
    }
}

#endif // #ifdef(_DGAIN_CAL_WITHOUT_INPUT_)

#ifdef _DGAIN_CAL_WITH_INPUT_
//**************************************************************************
//  [Function Name]:
//                  drvADC_FindWhiteRegion()
//  [Description]:
//                  Find the white region in line buffer
//  [Precondition]:
//
//  [Arguments]:
//                 LineBuf: choose one line to read, 0 ~ 3
//                 Step: comparsion step
//  [Return]:
//                  status
//**************************************************************************
static WORD drvADC_FindWhiteRegion(BYTE LineBuf, BYTE Step)
{
    BYTE i, eqcount=0;
    msADCAvgVal pAutoAdc_val, pAutoAdc_preval;
    WORD wStartAdd_White = 0;

    pAutoAdc_preval.AVG_Rch = pAutoAdc_preval.AVG_Gch = pAutoAdc_preval.AVG_Bch = 0;

    drvADC_IsReadLineBuffer(TRUE);
    for(i=0;i<255;i++)
    {
        pAutoAdc_val = drvADC_GetRGBLineBufAverageData(wStartAdd_White+(i*Step), LineBuf, 1); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
        if(pAutoAdc_preval.AVG_Gch > (pAutoAdc_val.AVG_Gch + 5))
        {
            pAutoAdc_val = drvADC_GetRGBLineBufAverageData(wStartAdd_White+(i*Step)+2, LineBuf, 1);
            if(pAutoAdc_preval.AVG_Gch > (pAutoAdc_val.AVG_Gch + 5))
            {
                pAutoAdc_val = drvADC_GetRGBLineBufAverageData(wStartAdd_White+(i*Step)+4, LineBuf, 1);          
                if(pAutoAdc_preval.AVG_Gch > (pAutoAdc_val.AVG_Gch + 5))
                {		
                    if(eqcount>=1)
                        i -= (eqcount > 1 ? (eqcount/2) : 1);
                    wStartAdd_White += ((i-1)*Step);
                    ADC_DPRINTF("@@ pre_val = %d", pAutoAdc_val.AVG_Gch);
                    break;
                }
            }
            eqcount = 0;
            pAutoAdc_preval.AVG_Rch = pAutoAdc_val.AVG_Rch;
            pAutoAdc_preval.AVG_Gch = pAutoAdc_val.AVG_Gch;
            pAutoAdc_preval.AVG_Bch = pAutoAdc_val.AVG_Bch;
        }
        else if( (abs(pAutoAdc_preval.AVG_Rch -pAutoAdc_val.AVG_Rch) <=2) && (abs(pAutoAdc_preval.AVG_Gch -pAutoAdc_val.AVG_Gch) <=2) &&
                        (abs(pAutoAdc_preval.AVG_Bch -pAutoAdc_val.AVG_Bch) <=2) )
        {
            eqcount++;
        }
        else
        {
            eqcount = 0;
            pAutoAdc_preval.AVG_Rch = pAutoAdc_val.AVG_Rch;
            pAutoAdc_preval.AVG_Gch = pAutoAdc_val.AVG_Gch;
            pAutoAdc_preval.AVG_Bch = pAutoAdc_val.AVG_Bch;
            if((i%5) == 0)
                ADC_DPRINTF("@@ R_val = %d", pAutoAdc_preval.AVG_Gch);
        }
    }
	
    if( (wStartAdd_White == 0) && (eqcount > 0) )
    {
        i -= (eqcount > 1 ? (eqcount/2) : 1);
        wStartAdd_White = ((i-1)*Step);
    }

    drvADC_IsReadLineBuffer(FALSE);
    {
        ADC_DPRINTF("@@EQCount = %d\r\n", eqcount);
        ADC_DPRINTF("@@EQVal_R = %d\r\n", pAutoAdc_preval.AVG_Rch);
        ADC_DPRINTF("@@EQVal_G = %d\r\n", pAutoAdc_preval.AVG_Gch);
        ADC_DPRINTF("@@EQVal_B = %d\r\n", pAutoAdc_preval.AVG_Bch);
    }
    ADC_DPRINTF("@@Line Buffer Start Address = %d\r\n", wStartAdd_White);
    return wStartAdd_White;
}


//**************************************************************************
//  [Function Name]:
//                  _msADC_AutoGainAdjustment()
//  [Description]:
//                  Adjust gain to approach target value 182(8 bits) => 255 * 0.5/0.7 = 182
//  [Precondition]:
//
//  [Arguments]:
//                 wLineBufAddress: line buffer address, it should be less than PG Htotal
//                 LineBuf: choose one line to read, 0 ~ 3
//                 AvgCnt: average pixel count
//  [Return]:
//                  status
//**************************************************************************
static Bool drvADC_AutoGainAdjustment_WithInput(WORD wLineBufAddress, BYTE LineBuf, BYTE AvgCnt)
{
    msADCAvgVal pAutoAdc_val;
    WORD rGain, gGain, bGain;
    WORD maxRGain, maxGGain, maxBGain;
    WORD minRGain, minGGain, minBGain;
    BYTE retry=12;
    BYTE atgFlags=0;

    maxRGain=maxGGain=maxBGain=0xFFF;
    minRGain=minGGain=minBGain=0;

    while (retry--)
    {
        rGain=((WORD)maxRGain+minRGain)/2;
        gGain=((WORD)maxGGain+minGGain)/2;
        bGain=((WORD)maxBGain+minBGain)/2;
        drvADC_SetRGBGainCode(rGain, gGain, bGain);
        Delay1ms(20);
        drvADC_IsReadLineBuffer(TRUE);
        pAutoAdc_val = drvADC_GetRGBLineBufAverageData(wLineBufAddress, LineBuf, AvgCnt); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
        drvADC_IsReadLineBuffer(FALSE);

        if (pAutoAdc_val.AVG_Rch >= 255)
        {
            rGain--;
            atgFlags |= DGain_OverFlow_R_ch;
        }
        else
        {
            rGain++;
            atgFlags |= DGain_NoneOverFlow_R_ch;
        }

        if (pAutoAdc_val.AVG_Gch >= 255)
        {
            gGain--;
            atgFlags |= DGain_OverFlow_G_ch;
        }
        else
        {
            gGain++;
            atgFlags |= DGain_NoneOverFlow_G_ch;
        }

        if (pAutoAdc_val.AVG_Bch >= 255)
        {
            bGain--;
            atgFlags |= DGain_OverFlow_B_ch;
        }
        else
        {
            bGain++;
            atgFlags |= DGain_NoneOverFlow_B_ch;
        }

        drvADC_SetRGBGainCode(rGain, gGain, bGain);
        Delay1ms(20);
        drvADC_IsReadLineBuffer(TRUE);
        pAutoAdc_val = drvADC_GetRGBLineBufAverageData(wLineBufAddress, LineBuf, AvgCnt); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
        drvADC_IsReadLineBuffer(FALSE);

        if (pAutoAdc_val.AVG_Rch >= 255)
        {
            maxRGain=rGain+1;
            atgFlags |= DGain_OverFlow_R_ch;
        }
        else
        {
            minRGain=rGain-1;
            atgFlags |= DGain_NoneOverFlow_R_ch;
        }

        if (pAutoAdc_val.AVG_Gch >= 255)
        {
            maxGGain=gGain+1;
            atgFlags |= DGain_OverFlow_G_ch;
        }
        else
        {
            minGGain=gGain-1;
            atgFlags |= DGain_NoneOverFlow_G_ch;
        }

        if (pAutoAdc_val.AVG_Bch >= 255)
        {
            maxBGain=bGain+1;
            atgFlags |= DGain_OverFlow_B_ch;
        }
        else
        {
            minBGain=bGain-1;
            atgFlags |= DGain_NoneOverFlow_B_ch;
        }
		if (CheckSyncLoss())//||CheckPowerKeyStatus())				
        //if (_msADC_CheckSyncLoss())
            return FALSE;
    }



    UserPrefAdcRedGain=rGain-1;
    UserPrefAdcGreenGain=gGain-1;
    UserPrefAdcBlueGain=bGain-1;
    drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);
	
    if ((atgFlags&0xFC)!=0xFC)
    {
        ADC_DPUTSTR("Auto gain fail !!\n");
        ADC_DPRINTF("Rgain = 0x%x", rGain);
        ADC_DPRINTF("Ggain = 0x%x", gGain);
        ADC_DPRINTF("Bgain = 0x%x", bGain);
        return FALSE;
    }
    else
    {
        ADC_DPUTSTR("Auto gain OK");
        ADC_DPRINTF("Rgain = 0x%x", rGain);
        ADC_DPRINTF("Ggain = 0x%x", gGain);
        ADC_DPRINTF("Bgain = 0x%x", bGain);
        return TRUE;
    }
}

#endif //#ifdef(_DGAIN_CAL_WITH_INPUT_)

void drvADC_SetHsyncRefLevel(ADC_HSYNCLVL_T level)
{
    msWriteByteMask(REG_ADC_ATOP_20_L, level, 0x07);
}

#if 1

void drvADC_Test_Funct(BYTE tb1, BYTE tb2)
{
    BYTE uncallvalue = tb2;

    switch(tb1)
    {
        case 0: // mismatch calibration
            drvADC_ADCOffsetGainMismatchCal();
            break;
        case 1: // auto phase
            mStar_AutoPhase(40);
            ADC_DPRINTF("@@ Auto Phase = 0x%x", UserPrefPhase);
            break;
#if 0
        case 1: // auto gain with input
            msADC_ADCAutoGainCalwithInput();
            break;
        case 2: // auto gain without input
            switch(tb2)
            {
                case 0:
                    // 1. backup ADC and Scaler related settings
                    _msADC_BackupADCSetting();
                    _msADC_BackupScalerSetting();

                    // 2. Set free run clock at 720p
                    _msADC_SetFreeRunClock();
                    break;
                case 1:
                    // 3. Enable scaler pattern generator to generate Hsync and Vsync for line buffer
                    _msADC_EnableScalerPG( E_PG_Htotal_256 );
                    break;
                case 2:
                    // 4. Set ADC input LPF to lowest bandwidth
                    _msADC_Write2BytesMask( REG_ADC_ATOP_23_L, 0xFFFF, 0xFFFF );
                    _msADC_Write2BytesMask( REG_ADC_ATOP_24_L, 0x00FF, 0x00FF );
                    break;
                case 3:
                    // 5. Calibrate ADC R/G/B gain setting value
                    // Set R/G/B gain initial setting value at 0x0400
                    // 6. Gain calibration target value for RGB color space mode is 182 (8bit data) 255 * 0.5 / 0.7 = 182
                    // 7. Get the ADC digital output with Turn-On LDO and Set VCAL to 1.05v
                    // 8. Get the ADC digital output with VCAL to 0.55v
                    // 9. Calculate the difference between 1.05V and 0.55V of these 3 channels.
                    //    Compare the difference value with target value (182 at RGB color space) to adjust R/G/B channel gain register
                    _msADC_AutoGainAdjustment();
                    break;
                case 4:
                    // 10. recover ADC and scaler related settings & turn off free run clock & turn off internal Vcal
                    _msADC_RecoverADCSetting();
                    _msADC_RecoverScalerSetting();
                    break;

                case 5:
                    msADC_ADCAutoGainCalwithoutInput();
                    break;
            }
            break;
#endif
        case 3:
            drvADC_AutoAdcColor(20);
            break;
#ifdef _DGAIN_CAL_WITH_INPUT_
        case 4:
	     drvADC_ADCAutoGainCalwithInput();
            break;
#endif
#ifdef _DGAIN_CAL_WITHOUT_INPUT_
        case 5:
                drvADC_AutoGainAdjustment();
            break;

        case 6:
            drvADC_ADCAutoGainCalwithoutInput();
            break;
            
        case 7:
            ADC_DPUTSTR("LDO:");
            //drvADC_ADCAutoGainCalwithoutInput();
            //_msADC_SetInternalVcal(E_Vcal_1_05V);
            {
            msADCAvgVal ptmp;
            msADCAvgVal pAutoAdc_val;

            // 1. backup ADC and Scaler related settings
            drvADC_BackupADCSetting();
            drvADC_BackupScalerSetting();
            
            // 2. Set free run clock at 720p
            drvADC_SetFreeRunClock();
            _msADC_MaskExternalSync(TRUE);            
            
            // 3. Enable scaler pattern generator to generate Hsync and Vsync for line buffer
            drvADC_EnableScalerPG(E_PG_Htotal_256);
            
            // 4. Set ADC input LPF to lowest bandwidth
            drvADC_Write2BytesMask(REG_ADC_ATOP_23_L, 0xFFFF, 0xFFFF);
            drvADC_Write2BytesMask(REG_ADC_ATOP_24_L, 0x00FF, 0x00FF);

            msWrite2ByteMask(REG_ADC_ATOP_34_L, BIT0, BIT0); // turn on LDO
            msWrite2ByteMask(REG_ADC_ATOP_54_L, 0, 0xF800); // disable bandgap overwrite
            msWrite2ByteMask(REG_ADC_ATOP_34_L, BIT2, BIT2); // LDO, Aaron test
            msWrite2ByteMask(REG_ADC_ATOP_54_L, 0x0003, 0x0003); // set Vcal = 1.05V
            msWrite2ByteMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // set Vcal from LDO
            msWrite2ByteMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // turn on Gcal pulse

            Delay1ms(1500);
            drvADC_IsReadLineBuffer(TRUE);
            ptmp = drvADC_GetRGBLineBufAverageData(0x40, 0, 32); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
            drvADC_IsReadLineBuffer(FALSE);
            
            msWrite2ByteMask(REG_ADC_ATOP_34_L, BIT0, BIT0); // turn on LDO
            msWrite2ByteMask(REG_ADC_ATOP_34_L, BIT2, BIT2); // LDO, Aaron test
            msWrite2ByteMask(REG_ADC_ATOP_54_L, 0x0002, 0x0003); // set Vcal = 0.55V
            msWrite2ByteMask(REG_ADC_ATOP_54_L, 0x000C, 0x000C); // set Vcal from LDO
            msWrite2ByteMask(REG_ADC_ATOP_54_L, 0x0600, 0x0700); // turn on Gcal pulse

            Delay1ms(1500);
            drvADC_IsReadLineBuffer(TRUE);
            pAutoAdc_val = drvADC_GetRGBLineBufAverageData(0x40, 0, 32); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
            drvADC_IsReadLineBuffer(FALSE);

            // 10. recover ADC and scaler related settings & turn off free run clock & turn off internal Vcal
            _msADC_MaskExternalSync(FALSE);
            drvADC_RecoverADCSetting();
            drvADC_RecoverScalerSetting();
            
            ADC_DPRINTF("AVG_R_1.05V= 0x%x", ptmp.AVG_Rch);
            ADC_DPRINTF("AVG_G_1.05V = 0x%x", ptmp.AVG_Gch);
            ADC_DPRINTF("AVG_B_1.05V = 0x%x", ptmp.AVG_Bch);
            ADC_DPRINTF("AVG_R_0.55V = 0x%x", pAutoAdc_val.AVG_Rch);
            ADC_DPRINTF("AVG_G_0.55V = 0x%x", pAutoAdc_val.AVG_Gch);
            ADC_DPRINTF("AVG_B_0.55V = 0x%x", pAutoAdc_val.AVG_Bch);    
            ADC_DPRINTF("AVG_R_diff = 0x%x", ptmp.AVG_Rch - pAutoAdc_val.AVG_Rch);
            ADC_DPRINTF("AVG_G_diff = 0x%x", ptmp.AVG_Gch - pAutoAdc_val.AVG_Gch);
            ADC_DPRINTF("AVG_B_diff = 0x%x", ptmp.AVG_Bch - pAutoAdc_val.AVG_Bch);  
            }
            break;
        case 8:
            ADC_DPUTSTR("Get:");
            //drvADC_ADCAutoGainCalwithoutInput();
            //_msADC_SetInternalVcal(E_Vcal_1_05V);
            {
            msADCAvgVal ptmp;

            drvADC_IsReadLineBuffer(TRUE);
            ptmp = drvADC_GetRGBLineBufAverageData(0x40, 0, 32); // (2*line buffer address + average data) < SPRHDC: (2*64+32) < 240
            drvADC_IsReadLineBuffer(FALSE);
            
            ADC_DPRINTF("AVG_R = 0x%x", ptmp.AVG_Rch);
            ADC_DPRINTF("AVG_G = 0x%x", ptmp.AVG_Gch);
            ADC_DPRINTF("AVG_B = 0x%x", ptmp.AVG_Bch);
            }
            break;
#endif
    }
}
#endif // #if DEBUG_EN


#else

#if DUMMY_CODE_4_LINK_FILE
BYTE code DrvADCDummy[]={0xFF};
#endif

#endif //ENABLE_VGA_INPUT
//Jison 110314
void drvADC_PowerCtrl(BYTE u8State)
{
    if(u8State==ADC_POWER_ON)
    {
        msWrite2Byte( REG_ADC_ATOP_04_L, 0x0000 );  // set power-on default value to power down ADC
        msWriteByteMask(REG_ADC_ATOP_06_L, 0, 0xFF); // ADC clock power on
        msWriteByteMask(REG_ADC_ATOP_30_L, 0, BIT0); // on-line SOG MUX power on
#if _NEW_SOG_DET_ //(SC0_ED[5]==0 while power saving)
        msWrite2Byte( REG_ADC_ATOP_78_L, 0x0002 );  // set power-on default value to power down ADC
        msWrite2Byte(REG_ADC_ATOP_79_L, 0x00C8); // ADC clock power down
        msWriteByteMask(REG_ADC_ATOP_7A_L, BIT2, BIT2|BIT1); // on-line SOG MUX power on
#endif
        Delay1ms(10);
        msWrite2ByteMask(REG_ADC_ATOP_11_L, BIT8, BIT8); // ADC phase alignment trigger
        msWrite2ByteMask(REG_ADC_ATOP_11_L, 0, BIT8);    // ADC phase alignment trigger
#if	CHIP_ID == CHIP_TSUMF
        msWriteByteMask(REG_ADC_ATOP_05_H, 0, BIT3); // phase digitizer to sync HSYNC (related to phase performance)
#endif
    }
    else if(u8State==ADC_POWER_STANDBY)
    {
      #if !MS_PM //seperate only
        msWrite2Byte( REG_ADC_ATOP_04_L, 0x3FFF );  // set power-on default value to power down ADC
        msWriteByteMask(REG_ADC_ATOP_06_L, 0xFF, 0xFF); // ADC clock power on
        msWriteByteMask(REG_ADC_ATOP_30_L, BIT0, BIT0); // on-line SOG MUX power down
      #else //saperate & SOG support
        #if _NEW_SOG_DET_ //(SC0_ED[5]==0 while power saving)
        msWrite2Byte( REG_ADC_ATOP_78_L, 0x0002 );  // set power-on default value to power down ADC
        msWrite2Byte(REG_ADC_ATOP_79_L, 0x00C8); // ADC clock power down
        msWriteByteMask(REG_ADC_ATOP_7A_L, BIT2, BIT2|BIT1); // on-line SOG MUX power on
        #endif
        msWrite2Byte( REG_ADC_ATOP_04_L, 0x79F9 );  // set power-on default value to power down ADC
        msWriteByteMask(REG_ADC_ATOP_06_L, 0, 0xFF); // ADC clock power on
        msWriteByteMask(REG_ADC_ATOP_30_L, 0, BIT0); // on-line SOG MUX power on
#if	CHIP_ID == CHIP_TSUMF
		msWriteByteMask(REG_ADC_ATOP_05_H, BIT3, BIT3);
#endif

      #endif
    }
    else //ADC_POWER_DOWN
    {
#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)|| (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
        msWrite2Byte( REG_ADC_ATOP_04_L, 0xFFFF );  //  power down ADC
#else
        msWrite2Byte( REG_ADC_ATOP_04_L, 0x7FFF );  //  power down ADC
#endif
        
#if _NEW_SOG_DET_ //(SC0_ED[5]==0 while power saving)
        msWrite2Byte( REG_ADC_ATOP_78_L, 0x0000 );  // set power-on default value to power down ADC
        msWrite2Byte(REG_ADC_ATOP_79_L, 0x0000); // ADC clock power down
        msWriteByteMask(REG_ADC_ATOP_7A_L, 0, BIT2|BIT1); // on-line SOG MUX power on
#endif
        msWriteByteMask(REG_ADC_ATOP_06_L, 0xFF, 0xFF); // ADC clock power down
        msWriteByteMask(REG_ADC_ATOP_30_L, BIT0, BIT0); // on-line SOG MUX power down
#if	CHIP_ID == CHIP_TSUMF
		msWriteByteMask(REG_ADC_ATOP_05_H, BIT3, BIT3);
#endif
    }
}

#endif // _DRVADC_C_
