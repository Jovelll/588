#include "types.h"
#include "board.h"
#include "global.h"
#include "keypaddef.h"
#include "menudef.h"
#include "adjust.h"
//#include "msADC.h"
#include "ms_reg.h"
#include "debug.h"
#include "Common.h"
#include "msosd.h"
#include "power.h"
#include "misc.h"
#include "autofunc.h"
#include "nvram.h"
#include "userpref.h"
#include "mstar.h"
#include "ms_rwreg.h"
#include "mcu.h"
#include "menustrprop.h"
#include "gamma.h"
#include "ddc2bi.h"
#include "MsDLC.H"
#include "MsACE.H"
//#include "MsRegTV.h" // 110927 coding reserved
//#include "Panel.h"
#include "OsdDefault.h"

#include "drvgamma.h"
#include "MsHDCP.h"

#include "msflash.h"
#include "menufunc.h"
#include "drvosd.h"
#include "drvadc.h"
#include "colorpalette.h"
#if ENABLE_HDMI
#include "MsHDMI.h"
#endif
#if ENABLE_DP_INPUT
  #include "drvDpRxAPP.h"
#endif
#if ENABLE_RTE
#include "msOVD.h"
#endif
#if ENABLE_DSC
#include "msDSC_Setting.h"
#endif
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
#if ENABLE_3DLUT
#include "ms3DLUT.h"
#endif
#if CHIP_ID==CHIP_TSUMU
#include "msscaler.h"
#elif CHIP_ID == CHIP_TSUM2
#include "msscaler_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "msscaler_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "msscaler_D.h"
#elif CHIP_ID == CHIP_TSUMJ
#include "msscaler_J.h"
#endif

#include "drvDPRxApp.h"

#if (MS_PM)
#include "Ms_PM.h"
#endif
#include "gpio_def.h"
////////////////////////////////////
//#include "drvMcu.h"
#include "drvmStar.h"
//#include "halRwreg.h"
///////////////////////////////////
#if MS_DAC
#include "drvDAC.h"
#endif
#if ENABLE_FREESYNC
#include "ComboApp.h"
#endif
#define MenuFunc_DEBUG    1
#if ENABLE_DEBUG&&MenuFunc_DEBUG
    #define MenuFunc_printData(str, value)   printData(str, value)
    #define MenuFunc_printMsg(str)           printMsg(str)
#else
    #define MenuFunc_printData(str, value)
    #define MenuFunc_printMsg(str)
#endif

void ReadWarm1Setting( void );
void ReadWarm2Setting( void );
void ReadNormalSetting( void );
void ReadCool1Setting( void );
void ReadCool2Setting( void );
void ReadSRGBSetting( void );

void SetYCMColor( MenuItemActionType action, WORD color, WORD value );
void SetECO( void );
Bool SetBFSize( BYTE ucSize );
void ReadColorTempSetting( void );

#if Enable_Expansion       // For 4:3 Mode BrightnessFrame Position
WORD xdata DispalyWidth = PANEL_WIDTH;
WORD xdata HStartOffset = 0;
#define DISPLAY_WIDTH   DispalyWidth
#define HSTART_OFFSET   HStartOffset
#else       // Normal Mode
#define DISPLAY_WIDTH   PanelWidth
#define HSTART_OFFSET   0
#endif

#if ENABLE_OSD_3D_CONTROL
#if CHIP_ID == CHIP_TSUMD
extern code BYTE t2Dto3DPIXGain[5];
extern code BYTE t2Dto3DBLKGain[5];
extern code BYTE t2Dto3DPIXOffset[5];
extern code BYTE t2Dto3DBLKOffset[5];
#elif CHIP_ID == CHIP_TSUM2
extern code BYTE t2Dto3DGain[5];
extern code BYTE t2Dto3DOffset[5];
#endif
#endif
#if EN_HotKeyTimes
extern BYTE xdata HotKeyTimes_Minutes;
extern BYTE xdata HotKeyTimes_Second;
//extern Bool ExecuteKeyEvent(MenuItemActionType menuAction);

#endif

void ReadUserSetting( void );
//void Save9300KSetting(void);
//void Save6500KSetting(void);

//vs ccflag add
#if Enable_Expansion
void SetExpansionMode( void );
#endif
#if Audio_Switch
Bool SetAudioType(AudioType Type);
#endif
WORD DecIncValue( MenuItemActionType action, WORD value, WORD minValue, WORD maxValue, BYTE step );
WORD GetScale100Value( WORD value, WORD minValue, WORD maxValue );
extern BYTE xdata MenuItemIndex;
extern BYTE xdata MenuPageIndex;

code short tNormalColorCorrectionMatrix[3][3] =
{
    { 1024,    0,       0},
    {     0,    1024,       0},
    {    0,     0,      1024}
};
//DLC Table......
BYTE code t_MWEDLC_Linear_Table[] =
{
    0x07, 0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87, 0x97, 0xA7, 0xB7, 0xC7, 0xD7, 0xE7, 0xF7,
    0x87, 0x9B, 0x0F, 0xC2, 0xBD, 0x4E, 0xC6, 0xFD, 0xF3, 0x3E, 0xE9, 0x35, 0x75, 0xED, 0x76, 0x8D,
    0xC1, 0x74, 0xD4, 0x3C, 0x3A, 0x18, 0xE0, 0x0A, 0xD6, 0xE3, 0x33, 0x0C, 0x0A, 0x84, 0xB7, 0xA0,
};

/*
BYTE code t_Normal_ColorSettingTable[] =
{
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x00, 0xD4, 0x55, 0xED, 0xDC, 0xEA, 0x20, 0x81, 0x11, 0x1B, 0x01, 0xFE,
    0xDC, 0xC2, 0xAD, 0xDE, 0x23, 0x63, 0x0E, 0x58, 0x10, 0x21, 0xF1, 0xFA, 0x5D, 0x7B, 0xF1, 0x87,
};
*/

//////////////////////////////////////////////////////////////////////////
// Matrix for VIDEO color correcttion
//////////////////////////////////////////////////////////////////////////
//code short tStandardColorCorrectionMatrix[][3]=
code short t_Normal_ColorSettingTable[][3]=
{
0x03FF, 0x0000, 0x0000, 0x0000, 0x03FF, 0x0000, 0x0000, 0x0000,
0x03FF,-0x0369, 0x01AB,-0x051B, 0x0012,-0x01D9, 0x0777,-0x0378,
-0x0735, 0x017A, 0x019E,-0x026E, 0x0274, 0x0687,-0x07A6, 0x0280,
-0x0831, 0x0000,-0x0500, 0x0500,-0x0005, 0x0000, 0x0000, 0x0000,
};


BYTE* code ColorTempPtr[5][3] =
{
    {&UserPrefRedColorWarm1, &UserPrefGreenColorWarm1, &UserPrefBlueColorWarm1},  
    {&UserPrefRedColorNormal, &UserPrefGreenColorNormal, &UserPrefBlueColorNormal},
    {&UserPrefRedColorCool1, &UserPrefGreenColorCool1, &UserPrefBlueColorCool1},
    {&UserPrefRedColorUser, &UserPrefGreenColorUser, &UserPrefBlueColorUser},	     
    {&UserPrefRedColorSRGB, &UserPrefGreenColorSRGB, &UserPrefBlueColorSRGB},
};


WORD GetNonFuncValue( void )
{
    return 50;
}
WORD GetBlueColorValue( void )
{
    return GetScale100Value( UserPrefBlueColor, MinColorValue, MaxColorValue );
}
//====================================================

WORD GetGreenColorValue( void )
{
    return GetScale100Value( UserPrefGreenColor, MinColorValue, MaxColorValue );
}
//====================================================

WORD GetRedColorValue( void )
{
    return GetScale100Value( UserPrefRedColor, MinColorValue, MaxColorValue );
}
Bool ChangeSource( void )
{
	if (UserPrefInputPriorityType == MenuItemIndex)
		return FALSE;
	UserPrefInputPriorityType = MenuItemIndex;

    if( UserPrefInputPriorityType == Input_Priority_Auto )
    {
    	SrcInputType=SrcInputType;
		UserPrefInputType=SrcInputType;			
    }
    else if( UserPrefInputPriorityType == Input_Priority_Analog1 )
    {
    	SrcInputType=Input_Analog1;
		UserPrefInputType=SrcInputType;	
    }	
#if INPUT_TYPE == INPUT_1A1D || INPUT_TYPE == INPUT_1A1H || INPUT_TYPE == INPUT_1A1H || INPUT_TYPE == INPUT_1A1C2
	else if ( UserPrefInputPriorityType == Input_Priority_Digital1 )
    {
		SrcInputType=Input_Digital;
		UserPrefInputType=SrcInputType;			
    }
#endif
#if INPUT_TYPE == INPUT_1A2D || INPUT_TYPE == INPUT_1A2H || INPUT_TYPE == INPUT_1A1D1H || INPUT_TYPE == INPUT_1A1D1DP || INPUT_TYPE == INPUT_1A1H1DP || INPUT_TYPE == INPUT_1A2C
	else if ( UserPrefInputPriorityType == Input_Priority_Digital1 )
    {
		SrcInputType=Input_Digital;
		UserPrefInputType=SrcInputType;			
    }
	else if ( UserPrefInputPriorityType == Input_Priority_Digital2 )
    {
		SrcInputType=Input_Digital2;
		UserPrefInputType=SrcInputType;			
    }
#endif
#if INPUT_TYPE == INPUT_1A3C
	else if ( UserPrefInputPriorityType == Input_Priority_Digital1 )
    {
		SrcInputType=Input_Digital;
		UserPrefInputType=SrcInputType;			
    }
	else if ( UserPrefInputPriorityType == Input_Priority_Digital2 )
    {
		SrcInputType=Input_Digital2;
		UserPrefInputType=SrcInputType;			
    }
	else if ( UserPrefInputPriorityType == Input_Priority_Digital3 )
    {
		SrcInputType=Input_Digital3;
		UserPrefInputType=SrcInputType;			
    }
#endif
#if INPUT_TYPE == INPUT_1A1D1H1DP
	else if ( UserPrefInputPriorityType == Input_Priority_Digital1 )
	{
		SrcInputType=Input_Digital;
		UserPrefInputType=SrcInputType; 		
	}
	else if ( UserPrefInputPriorityType == Input_Priority_Digital2 )
	{
		SrcInputType=Input_Digital2;
		UserPrefInputType=SrcInputType; 		
	}
	else if ( UserPrefInputPriorityType == Input_Priority_Digital3 )
	{
		SrcInputType=Input_Displayport;
		UserPrefInputType=SrcInputType; 		
	}
#endif


	Set_SaveMonitorSettingFlag();
    Power_TurnOffPanel();
	
    mStar_SetupInputPort();
    Set_InputTimingChangeFlag();
    //SrcFlags |= SyncLoss;
    //if( !ProductModeFlag )
    //    Set_ShowInputInfoFlag();
    return TRUE;
}
#if ENABLE_DPS
BYTE GetSubWndRealContrast(BYTE ucContrast)
{
#if MWEFunction
    return (UserPrefSubContrast+ucContrast-DefContrast);
#else
    return (UserPrefContrast+ucContrast-DefContrast);
#endif
}
#endif

void MenuFuncuncall(void)
{
   //AdjustAudioSource();
    AdjustDcrMode(0);

#if 0	
    AdjustGamaMode(0);
#if ENABLE_DPS
    AdjustDPSMode(0);
#endif
#if ENABLE_DSC
    AdjustDSCMode(0);
#endif
#if ENABLE_DLC
    AdjustDLCMode(0);
#endif
#if ENABLE_TNR
    AdjustNRMode(0);
#endif
#if ENABLE_SUPER_RESOLUTION
    AdjustSuperResolutionMode(0);
#endif
    //AdjustLanguage();
#if ENABLE_SHARPNESS
    GetSharpnessValue();
#endif
#endif

#if ENABLE_FREESYNC
    GetSrcFreesyncFPS();
    SetDRRFunction();
#endif

#if (CHIP_ID == CHIP_TSUMF)
msDPS_On_Setting();
msDSC_On_Setting();
#endif
}


#if 0
//////////////////////////////////////////////////////////////////////////
// Matrix for convert to sRGB space
//////////////////////////////////////////////////////////////////////////
code short tSRGB[3][3] =
{
    { 1096, -43, -28 }, // R  1.0694, -0.0424, -0.0270
    { - 21,   1063, -18 }, // G -0.0204,  1.0376, -0.0172
    { - 1,   34,   991 }  // B -0.0009,  0.0330,  0.9679

};

//================================================================================




#if MWEFunction
void IndependentColorControl( void )
{
    BYTE code tColorRangeTbl[] = {0x20, 0x28, 0x20, 0x20, 0x28, 0x20, 0x19, 0x29, 0x19};
    BYTE i, j;

    BYTE code tColorControlTbl[][18] = //0x3A~0x4B
    {
        //  3A  3B   3C   3D   3E   3F   40   41   42   43   44   45   46   47   48   49   4A   4B
        {0x08, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //FullEnhanceFlag
        {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //SkinFlag
        {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //BlueFlag
        {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //GreenFlag
        {0x08, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} //ColorAutoDetectFlag
    };
    msWriteByte( BK0_00, REGBANK6 );

    for( i = 0; i < 9; i++ ) //0x31~0x39
        msWriteByte( BK6_31 + i, tColorRangeTbl[i] );

    if( FullEnhanceFlag )
        j = 0;
    else if( SkinFlag )
        j = 1;
    else if( BlueFlag )
        j = 2;
    else if( GreenFlag )
        j = 3;
    else if( ColorAutoDetectFlag )
        j = 4;

    for( i = 0; i < 18; i++ ) //0x3A~0x4B
        msWriteByte( BK6_3A + i, tColorControlTbl[j][i] );
    if( DemoFlag )
        msWriteByte( BK6_30, 0x07 );
    else
    {
        SetBFSize( UserPrefBFSize ); //SetBFSize();
        msWriteByte( BK6_30, 0x70 );
    }
    msWriteByte( BK0_00, REGBANK0 );
}
#if !ENABLE_HDMI
void SetColorBoostWindow( BYTE mode )
{
    if( DemoFlag )
        mode = COLORBOOST_HALF;
    if( mode != COLORBOOST_OFF )
    {
        // 2006/11/10 10:43PM by KK         msAdjustSubBrightness( UserPrefSubBrightness );
        if( DemoFlag )
            msAdjustVideoContrast( DefContrastBase + DefContrast );
        else
            msAdjustVideoContrast( DefContrastBase + UserPrefContrast );
        if( mode == COLORBOOST_HALF )
        {
            UserPrefBFHSize = PanelWidth / 2;
            UserPrefBFVSize = PanelHeight;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }
        else if( mode == COLORBOOST_FULL )
        {
            UserPrefBFHSize = PanelWidth;
            UserPrefBFVSize = PanelHeight;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }
        if( DemoFlag )
        {
            msAccSetup( UserPrefBFHStart, UserPrefBFHSize, UserPrefBFVStart, UserPrefBFVSize );
            msAccOnOff( 1 );
        }

    }
    else
        // COLORBOOST_OFF
    {
        msAccOnOff( 0 );
    }
}
#endif
void SetColorEnhance( bit enable )
{
    BYTE *pColorTable;
    enable = 1;
    if( UserPrefDcrMode )
    {
        pColorTable = (BYTE *)t_Normal_ColorSettingTable;
        //LoadACETable( pColorTable, DefHue, DefSaturation, 0x80 ); //  UserPrefSubContrast  );
        LoadACETable(MAIN_WINDOW, pColorTable, DefHue, DefSaturation, 0x80 ); //  UserPrefSubContrast  );
        msAccOnOff( 1 );
        msDlcOnOff( 1 );
    }
#if 1 // wait for coding

#else
    else if( FullEnhanceFlag || SkinFlag || BlueFlag || GreenFlag || DemoFlag || ColorAutoDetectFlag )

    {

        if( FullEnhanceFlag || ColorAutoDetectFlag || GreenFlag || BlueFlag || SkinFlag )
            IndependentColorControl();
        else
        {
            msWriteByte( BK0_00, REGBANK6 );
            msWriteByte( BK6_30, 0x00 );
            msWriteByte( BK0_00, REGBANK0 );
            //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
        }
        if( GreenFlag || SkinFlag )
        {
            msWriteByte( BK0_00, REGBANK3 );
            msWriteByte( BK3_72, 0x6A );
            msWriteByte( BK3_73, 0x6A );
            msWriteByte( BK3_74, 0x6A );
            msWriteByte( BK0_00, REGBANK0 );
        }
        else if( ColorAutoDetectFlag )
        {
            msWriteByte( BK0_00, REGBANK3 );
            msWriteByte( BK3_72, 0x60 );
            msWriteByte( BK3_73, 0x60 );
            msWriteByte( BK3_74, 0x60 );
            msWriteByte( BK0_00, REGBANK0 );
        }
        else
        {
            msWriteByte( BK0_00, REGBANK3 );
            msWriteByte( BK3_72, 0x70 );
            msWriteByte( BK3_73, 0x70 );
            msWriteByte( BK3_74, 0x70 );
            msWriteByte( BK0_00, REGBANK0 );
        }
    }
    else
    {
        msWriteByte( BK0_00, REGBANK6 );
        msWriteByte( BK6_30, 0x00 );
        msWriteByte( BK0_00, REGBANK0 );
        //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
    }
#endif

}
#if ENABLE_HDMI
void SetColorBoostWin( BYTE mode )
{
    if( DemoFlag )
        mode =  COLORBOOST_HALF;

    if( mode != COLORBOOST_OFF )
    {
        msAdjustVideoContrast( DefContrastBase + UserPrefContrast );

        if( mode == COLORBOOST_HALF )
        {
            UserPrefBFHSize = PanelWidth / 2;
            UserPrefBFVSize = PanelHeight;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }
        else if( mode == COLORBOOST_FULL )
        {
            UserPrefBFHSize = PanelWidth ;
            UserPrefBFVSize = PanelHeight;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }


        if( DemoFlag )
        {
            msAccSetup( UserPrefBFHStart, UserPrefBFHSize, UserPrefBFVStart, UserPrefBFVSize );
            if( gScInfo.InputColor == INPUT_RGB )
                msAccOnOff( 1 );
            else
                mStar_WriteByteMask( BK0_5C, BIT3, BIT3 ); //msWriteByte( BK0_5C, msReadByte( BK0_5C ) | BIT3 );
        }
        else
        {
            if( gScInfo.InputColor == INPUT_RGB )
                msAccOnOff( 0 );
            else
                mStar_WriteByteMask( BK0_5C, 0x00, BIT3 );  //msWriteByte( BK0_5C, msReadByte( BK0_5C )&~BIT3 );
        }

    }
    else // COLORBOOST_OFF
    {
        msAccSetup( 0, 0, 0, 0 );
        //msAccOnOff( 0 );
    }

}

void SetColorMatrix( void )
{
    BYTE *pColorTable;

    if( UserPrefDcrMode )
    {
        pColorTable = (BYTE *)t_Normal_ColorSettingTable;
        //LoadACETable( pColorTable, UserPrefHue, UserPrefSaturation, 0x80 ); //  UserPrefSubContrast  );
        LoadACETable( MAIN_WINDOW, pColorTable, UserPrefHue, UserPrefSaturation, 0x80 ); //  UserPrefSubContrast  );
        //msAccOnOff( 1 );
        //msDlcOnOff( 1 );
    }
    else  if( FullEnhanceFlag || SkinFlag || BlueFlag || GreenFlag || DemoFlag || ColorAutoDetectFlag )  //NormalSkin+GreenENhance+BlueEnhance
    {

        if( FullEnhanceFlag || ColorAutoDetectFlag || GreenFlag || BlueFlag || SkinFlag )
            IndependentColorControl();
        else
        {
            msWriteByte( BK0_00, REGBANK6 );
            msWriteByte( BK6_30, 0x00 );
            msWriteByte( BK0_00, REGBANK0 );
            //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
        }
        if( GreenFlag || SkinFlag )
        {
            msWriteByte( BK0_00, REGBANK3 );
            msWriteByte( BK3_72, 0x6A );
            msWriteByte( BK3_73, 0x6A );
            msWriteByte( BK3_74, 0x6A );
            msWriteByte( BK0_00, REGBANK0 );
        }
        else if( ColorAutoDetectFlag )
        {
            msWriteByte( BK0_00, REGBANK3 );
            msWriteByte( BK3_72, 0x60 );
            msWriteByte( BK3_73, 0x60 );
            msWriteByte( BK3_74, 0x60 );
            msWriteByte( BK0_00, REGBANK0 );
        }
        else
        {
            msWriteByte( BK0_00, REGBANK3 );
            msWriteByte( BK3_72, 0x70 );
            msWriteByte( BK3_73, 0x70 );
            msWriteByte( BK3_74, 0x70 );
            msWriteByte( BK0_00, REGBANK0 );
        }
    }
    else
    {
        msWriteByte( BK0_00, REGBANK6 );
        msWriteByte( BK6_30, 0x00 );
        msWriteByte( BK0_00, REGBANK0 );
        //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
    }
}

void SetColorSpace( void )
{
#if ENABLE_HDMI
    BYTE XDATA ucDomain;
#endif

#if ENABLE_HDMI
    ucDomain = 0x00;
    if( gScInfo.InputColor != INPUT_RGB )
        ucDomain = 0xFF;
#endif

    if( DemoFlag || BlueFlag || SkinFlag || GreenFlag || ColorAutoDetectFlag || FullEnhanceFlag )
    {
#if ENABLE_HDMI
        if( ucDomain )
        {
            if( DemoFlag )
            {
                SetMainWinVideoDomain( ucDomain );      //SetMainWinColorControl(ucDomain);
                mStar_WriteByteMask( BK0_5C, BIT3, BIT3 ); //msWriteByte( BK0_5C, msReadByte( BK0_5C ) | BIT3 );
            }
            else
            {
                mStar_WriteByteMask( BK0_02, BIT3, BIT3 ); //msWriteByte( BK0_02, msReadByte( BK0_02 ) | BIT3 );
                mStar_WriteByteMask( BK0_5C, 0x00, BIT3 );  //msWriteByte( BK0_5C, msReadByte( BK0_5C )&~BIT3 );
            }
        }
        else
#endif
            if( DemoFlag )
                msAccOnOff( 1 );
    }
    else
    {
#if ENABLE_HDMI
        if( ucDomain )
            mStar_WriteByteMask( BK0_5C, 0x00, BIT3 );  //msWriteByte( BK0_5C, msReadByte( BK0_5C )&~BIT3 );
        else
#endif
            msAccOnOff( 0 );
#if ENABLE_HDMI
        SetMainWinVideoDomain( 0 ); //SetMainWinColorControl(0);
        if( ucDomain )
            mStar_WriteByteMask( BK0_02, BIT3, BIT3 ); //msWriteByte( BK0_02, msReadByte( BK0_02 ) | BIT3 );
#endif
    }
}
#endif

//================================================================================
Bool AdjustColorBoostMode( MenuItemActionType action )
{
    action = 0;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    Clr_SkinFlag();
    Clr_GreenFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_PictureBoostFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
    UserPrefECOMode = ECO_Standard;
    SetECO();
#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( FullEnhanceFlag )        // clr
    {
        Clr_FullEnhanceFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else        // set
    {
        Set_FullEnhanceFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:19      Osd_Draw4Num(7,1,UserPrefColorFlags);
    return TRUE;
}

//================================================================================
Bool AdjustSkinProtectMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    Clr_GreenFlag();
    Clr_FullEnhanceFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_PictureBoostFlag();

    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( SkinFlag )
    {
        Clr_SkinFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_SkinFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:24  Osd_Draw4Num(6,1,UserPrefColorFlags);
    return TRUE;
}
Bool AdjustGreenEnhanceMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    Clr_SkinFlag();
    Clr_FullEnhanceFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_PictureBoostFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( GreenFlag )
    {
        Clr_GreenFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_GreenFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:28      Osd_Draw4Num(5,1,UserPrefColorFlags);
    return TRUE;
}
Bool AdjustBlueEnhanceMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    // color boost menu
    Clr_FullEnhanceFlag();
    Clr_GreenFlag();
    Clr_SkinFlag();
    Clr_ColorAutoDetectFlag();
    // picture boost menu
    Clr_PictureBoostFlag();
    // DCR function
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( BlueFlag )
    {
        Clr_BlueFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_BlueFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:33      Osd_Draw4Num(4,1,UserPrefColorFlags);
    return TRUE;
}

Bool AdjustAutoDetectMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
        // when demo on, don't clear the demo flag
    {
        Clr_DemoFlag();
    }
    // color boost menu
    Clr_FullEnhanceFlag();
    Clr_GreenFlag();
    Clr_BlueFlag();
    Clr_SkinFlag();
    // picture boost menu
    Clr_PictureBoostFlag();
    // DCR function
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( ColorAutoDetectFlag )
    {
        Clr_ColorAutoDetectFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        msDlcOnOff( _DISABLE );
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_ColorAutoDetectFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        LoadDLCTable( t_MWEDLC_Linear_Table );
        msDlcOnOff( _DISABLE );
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:40      Osd_Draw4Num(3,1,UserPrefColorFlags);
    return TRUE;
}
Bool AdjustDemoMode( MenuItemActionType action )
{
    action = action;
    Clr_PictureBoostFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    UserPrefBFSize = 1;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( DemoFlag )
    {
        Clr_DemoFlag();
        if( BlueFlag || SkinFlag || GreenFlag || ColorAutoDetectFlag || FullEnhanceFlag )
#if ENABLE_HDMI
            SetColorBoostWin( COLORBOOST_OFF );
#else
            SetColorBoostWindow( COLORBOOST_OFF );
#endif
        else
#if ENABLE_HDMI
            SetColorBoostWin( COLORBOOST_OFF );
#else
            SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Clr_PictureBoostFlag();
        Set_DemoFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_HALF );
#else
        SetColorBoostWindow( COLORBOOST_HALF );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:44      Osd_Draw4Num(2,1,UserPrefColorFlags);
    return TRUE;
}
//-----------------------------------------------------

#if 1  //2006-08-29 Andy
//Bool SetBFSize(void)
Bool SetBFSize( BYTE ucSize )
{
#if ENABLE_HDMI
    BYTE XDATA ucDomain;
#endif

#if ENABLE_HDMI
    ucDomain = 0x00;
    if( gScInfo.InputColor != INPUT_RGB )
        ucDomain = 0xFF;
#endif

#if 1
    if( !PictureBoostFlag )
    {
#if !ENABLE_HDMI
        msAccOnOff( 0 );
        msDlcOnOff( 0 );
#endif
        UserPrefBFHSize = 0;
        UserPrefBFVSize = 0;
        UserPrefBFHStart = 0;
        UserPrefBFVStart = 0;
        UserPrefBFSize = ucSize = 1; //Benz 2007.4.14   14:34:23
    }
    else
    {
#if !ENABLE_HDMI
        msAccOnOff( 1 );
#endif
        if( ucSize == 1 ) // H:5 V:5
        {
            UserPrefBFHSize = ( DISPLAY_WIDTH + 4 ) / 8; // / 5;
            UserPrefBFVSize = PanelHeight / 8; // / 5;
        }
        else if( ucSize == 2 )         // H:5 V:3
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 5;
            UserPrefBFVSize = PanelHeight / 5; // / 3;
        }
        else if( ucSize == 3 )    //  H4 V:4
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 4;
            UserPrefBFVSize = PanelHeight / 4;
        }
        else if( ucSize == 4 )      //  H3 V:3
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 3;
            UserPrefBFVSize = PanelHeight / 3;
        }
        else if( ucSize == 5 )       //  H2 V:2
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 2;
            UserPrefBFVSize = PanelHeight / 2;
        }
        else if( ucSize == 6 )       //  H2 V:1
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 2;
            UserPrefBFVSize = PanelHeight;
        }
        else if( ucSize == 7 )      //  Full
        {
            UserPrefBFHSize = DISPLAY_WIDTH;
            UserPrefBFVSize = PanelHeight;
        }
        UserPrefBFHStart = 0;
        UserPrefBFVStart = 0;
        msAdjustVideoContrast( UserPrefSubContrast );
        msAccSetup( UserPrefBFHStart + HSTART_OFFSET, UserPrefBFHSize + HSTART_OFFSET, UserPrefBFVStart, UserPrefBFVSize );
#if ENABLE_HDMI
        msAdjustSubBrightness( UserPrefSubBrightness );
#else
        msAccOnOff( 1 );
#endif
    }
#endif

#if ENABLE_HDMI
    if( PictureBoostFlag )
    {
#if ENABLE_HDMI
        if( ucDomain )
        {
            SetMainWinVideoDomain( ucDomain );  //SetMainWinColorControl(ucDomain);
            mStar_WriteByteMask( BK0_5C, BIT3, BIT3 ); //msWriteByte( BK0_5C, msReadByte( BK0_5C ) | BIT3 );
        }
        else
#endif
            msAccOnOff( 1 );
    }
    else
    {
#if ENABLE_HDMI
        if( ucDomain )
            mStar_WriteByteMask( BK0_5C, 0x00, BIT3 ); //msWriteByte( BK0_5C, msReadByte( BK0_5C )&~BIT3 );
        else
#endif
            msAccOnOff( 0 );
#if ENABLE_HDMI
        SetMainWinVideoDomain( 0 ); //SetMainWinColorControl(0);
        if( ucDomain )
            mStar_WriteByteMask( BK0_02, BIT3, BIT3 ); //msWriteByte( BK0_02, msReadByte( BK0_02 ) | BIT3 );
#endif
    }
#endif
    return TRUE;
}

#if ENABLE_HDMI
Bool AdjustBrightFrameSize( BYTE ucSize )
{
    if( ucSize == 1 )         // H:5 V:5
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 8; // / 5;
        UserPrefBFVSize = PanelHeight / 8; // / 5;
    }
    else if( ucSize == 2 )  // H:5 V:3
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 5;
        UserPrefBFVSize = PanelHeight / 5; // / 3;
    }
    else if( ucSize == 3 )  //  H4 V:4
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 4;
        UserPrefBFVSize = PanelHeight / 4;
    }
    else if( ucSize == 4 )  //  H3 V:3
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 3;
        UserPrefBFVSize = PanelHeight / 3;
    }
    else if( ucSize == 5 )  //  H2 V:2
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 2;
        UserPrefBFVSize = PanelHeight / 2;
    }
    else if( ucSize == 6 )  //  H2 V:1
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 2;
        UserPrefBFVSize = PanelHeight;
    }
    else if( ucSize == 7 )  //  Full
    {
        UserPrefBFHSize = DISPLAY_WIDTH;
        UserPrefBFVSize = PanelHeight;
    }

    UserPrefBFHStart = 0;
    UserPrefBFVStart = 0;
    //msAdjustVideoContrast( UserPrefSubContrast );
    msAccSetup( UserPrefBFHStart + HSTART_OFFSET, UserPrefBFHSize + HSTART_OFFSET, UserPrefBFVStart, UserPrefBFVSize );
    //msAccOnOff( 1 );

    return TRUE;
}
#endif
//---------------------------------------------------------------
Bool AdjustBFSize( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBFSize, 1, 7, 1 );
    if( tempValue == UserPrefBFSize )
    {
        return FALSE;
    }
    UserPrefBFSize = tempValue;
    //printData("UserPrefBFSize=%d",UserPrefBFSize);
#if ENABLE_HDMI
    AdjustBrightFrameSize( UserPrefBFSize ); //
#else
    SetBFSize( UserPrefBFSize ); //SetBFSize();
#endif
    return TRUE;
}
WORD GetBFSizeValue( void )
{
    return GetScale100Value( UserPrefBFSize, 0, 7 );
}

//---------------------------------------------------------------
Bool AdjustBFHstart( MenuItemActionType action )
{
    WORD tempValue;
    if( UserPrefBFSize == 1 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 7, 1 );
    }
    else if( UserPrefBFSize == 2 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 4, 1 );
    }
    else if( UserPrefBFSize == 3 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 3, 1 );
    }
    else if( UserPrefBFSize == 4 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 2, 1 );
    }
    else if( UserPrefBFSize == 5 || UserPrefBFSize == 6 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 1, 1 );
    }
    else
    {
        tempValue = UserPrefHStart;
    }
    if( tempValue == UserPrefHStart )
    {
        return FALSE;
    }
    //printData("tempValue11=%d",tempValue);

    UserPrefBFHStart = tempValue;
    tempValue = UserPrefBFHStart * UserPrefBFHSize;
    msAccSetup( tempValue + HStartOffset, ( UserPrefBFHSize + tempValue ) + HStartOffset, UserPrefBFVStart * UserPrefBFVSize, ( UserPrefBFVStart * UserPrefBFVSize + UserPrefBFVSize ) );
    return TRUE;
}

WORD GetBFHstartValue( void )
{
    if( UserPrefBFSize == 2 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 4 );
    }
    else if( UserPrefBFSize == 3 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 3 );
    }
    else if( UserPrefBFSize == 4 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 2 );
    }
    else if( UserPrefBFSize == 5 || UserPrefBFSize == 6 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 1 );
    }
    else if( UserPrefBFSize == 1 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 7 );
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------
Bool AdjustBFVstart( MenuItemActionType action )
{
    WORD tempValue;
    if( UserPrefBFSize == 6 || UserPrefBFSize == 7 )
    {
        tempValue = 0;
    }
    else if( UserPrefBFSize == 1 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 7, 1 );
    }
    else if( UserPrefBFSize == 2 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 4, 1 );
    }
    else if( UserPrefBFSize == 4 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 2, 1 );
    }
    else if( UserPrefBFSize == 3 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 3, 1 );
    }
    else if( UserPrefBFSize == 5 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 1, 1 );
    }
    /*
    if ( tempValue == UserPrefVStart )
    {
    return FALSE;
    }
     */
    UserPrefBFVStart = tempValue;
    tempValue = UserPrefBFVStart * UserPrefBFVSize;
    msAccSetup( UserPrefBFHStart * UserPrefBFHSize + HStartOffset, UserPrefBFHStart * UserPrefBFHSize + UserPrefBFHSize + HStartOffset, tempValue, ( tempValue + UserPrefBFVSize ) );
    return TRUE;
}
WORD GetBFVstartValue( void )
{
    if( UserPrefBFSize == 1 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 7 );
    }
    else if( UserPrefBFSize == 2 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 4 );
    }
    else if( UserPrefBFSize == 4 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 2 );
    }
    else if( UserPrefBFSize == 3 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 3 );
    }
    else if( UserPrefBFSize == 5 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 1 );
    }
    else
    {
        return 0;
    }
}
//================================================================================
Bool AdjustBF_Switch( MenuItemActionType action )
{
    action = action;
    Clr_ColorAutoDetectFlag();
    Clr_FullEnhanceFlag();
    Clr_GreenFlag();
    Clr_BlueFlag();
    Clr_SkinFlag();
    Clr_DemoFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( PictureBoostFlag )
    {
        Clr_PictureBoostFlag();
#if ENABLE_HDMI
        UserPrefBFSize = 1 ;    // recall default setting=> BFSize=1, position(0,0)
#endif
        SetBFSize( UserPrefBFSize ); //SetBFSize();
    }
    else
    {
#if !ENABLE_HDMI
        //msAdjustSubBrightness( UserPrefSubBrightness );
        //LoadACETable( t_Normal_ColorSettingTable, UserPrefHue, UserPrefSaturation, UserPrefSubContrast );
        msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
        LoadACETable( MAIN_WINDOW, (BYTE *)t_Normal_ColorSettingTable, UserPrefHue, UserPrefSaturation, UserPrefSubContrast );
#endif
        Set_PictureBoostFlag();
#if !ENABLE_HDMI
        UserPrefBFSize = 1; // recall default setting=> BFSize=1, position(0,0)
#endif
        SetBFSize( UserPrefBFSize ); //SetBFSize();
#if !ENABLE_HDMI
        SetColorEnhance( 1 );
#endif
    }
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:48  Osd_Draw4Num(1,1,UserPrefColorFlags);
    return TRUE;
}
//================================================================================
Bool AdjustSubContrast( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefSubContrast, MinSubContrast, MaxSubContrast, 2 );
    if( tempValue == UserPrefSubContrast )
    {
        return FALSE;
    }
    UserPrefSubContrast = tempValue;
    msAdjustVideoContrast( UserPrefSubContrast );
    //NVRam_WriteByte( nvrMonitorAddr( SubContrast ), UserPrefSubContrast);
    return TRUE;
}
WORD GetSubContrast100Value( void )
{
    return GetScale100Value( UserPrefSubContrast, MinSubContrast, MaxSubContrast );
}
//================================================================================
Bool AdjustSubBrightness( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefSubBrightness, MinSubBrightness, MaxSubBrightness, 2 );
    if( tempValue == UserPrefSubBrightness )
    {
        return FALSE;
    }
    UserPrefSubBrightness = tempValue;
    msAdjustSubBrightness( UserPrefSubBrightness );
    return TRUE;
}
WORD GetSubBrightness100Value( void )
{
    return GetScale100Value( UserPrefSubBrightness, MinSubBrightness, MaxSubBrightness );
}
#endif
#endif
//-----------------------------------------------------

//extern void DPRxDCHPD( Bool enable );



//================================================================================
//================================================================================
//================================================================================

//=============================================================================

//====================================================

//=====================================================

//================================================================================

//================================================================================

//====================================================




Bool SetColorFormat(void)
{
    if (UserPrefInputColorFormat == MenuItemIndex)
        return FALSE;
    UserPrefInputColorFormat = MenuItemIndex;
    SetInputColorFormat();
    Set_SaveMonitorSettingFlag();
    return TRUE;
}

//====================================================

//Bool AdjustLanguage( MenuItemActionType action )




#if 1
/*
Bool ResetBriConValue(void)
{
UserPrefBrightness=UserPrefBrightness6500K;
UserPrefContrast=UserPrefContrast6500K;
mStar_AdjustBrightness(UserPrefBrightness);
mStar_AdjustContrast(UserPrefContrast);
SaveMonitorSetting();
return TRUE;
}
 */

#endif
/*void SaveUserPref(void)
{
    // 2006/10/24 7:5PM by Emily      printData("SaveUserPref", 0);
    SaveMonitorSetting();
    if (SrcInputType < Input_Digital)
    {
        SaveModeSetting();
    }
}*/

#if !USEFLASH
void SaveFactorySettingByItem( BYTE itemIndex )
{
    if( itemIndex == 2 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcRedGain ), UserPrefAdcRedGain );
    }
    else if( itemIndex == 4 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcGreenGain ), UserPrefAdcGreenGain );
    }
    else if( itemIndex == 6 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcBlueGain ), UserPrefAdcBlueGain );
    }
    else if( itemIndex == 8 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcRedOffset ), UserPrefAdcRedOffset );
    }
    else if( itemIndex == 10 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcGreenOffset ), UserPrefAdcGreenOffset );
    }
    else if( itemIndex == 12 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcBlueOffset ), UserPrefAdcBlueOffset );
    }
    else if( itemIndex == 14 )
    {
        NVRam_WriteByte( nvrFactoryAddr( RedColorCool1 ), UserPrefRedColorCool1 );
    }
    else if( itemIndex == 16 )
    {
        NVRam_WriteByte( nvrFactoryAddr( GreenColorCool1 ), UserPrefGreenColorCool1 );
    }
    else if( itemIndex == 18 )
    {
        NVRam_WriteByte( nvrFactoryAddr( BlueColorCool1 ), UserPrefBlueColorCool1 );
    }
    else if( itemIndex == 20 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnessCool1 ), FUserPrefBrightnessCool1 );
    }
    else if( itemIndex == 22 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastCool1 ), FUserPrefContrastCool1 );
    }
    else if( itemIndex == 24 )
    {
        NVRam_WriteByte( nvrFactoryAddr( RedColorNormal ), UserPrefRedColorNormal );
    }
    else if( itemIndex == 26 )
    {
        NVRam_WriteByte( nvrFactoryAddr( GreenColorNormal ), UserPrefGreenColorNormal );
    }
    else if( itemIndex == 28 )
    {
        NVRam_WriteByte( nvrFactoryAddr( BlueColorNormal ), UserPrefBlueColorNormal );
    }
    else if( itemIndex == 30 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnessNormal ), FUserPrefBrightnessNormal );
    }
    else if( itemIndex == 32 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastNormal ), FUserPrefContrastNormal );
    }
    else if( itemIndex == 34 )
    {
        NVRam_WriteByte( nvrFactoryAddr( RedColorWarm1 ), UserPrefRedColorWarm1 );
    }
    else if( itemIndex == 36 )
    {
        NVRam_WriteByte( nvrFactoryAddr( GreenColorWarm1 ), UserPrefGreenColorWarm1 );
    }
    else if( itemIndex == 38 )
    {
        NVRam_WriteByte( nvrFactoryAddr( BlueColorWarm1 ), UserPrefBlueColorWarm1 );
    }
    else if( itemIndex == 40 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnessWarm1 ), FUserPrefBrightnessWarm1 );
    }
    else if( itemIndex == 42 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastWarm1 ), FUserPrefContrastWarm1 );
    }
    else if( itemIndex == 44 )
        NVRam_WriteByte( nvrFactoryAddr( RedColorsRGB ), UserPrefRedColorSRGB );
    else if( itemIndex == 46 )
        NVRam_WriteByte( nvrFactoryAddr( GreenColorsRGB ), UserPrefGreenColorSRGB );
    else if( itemIndex == 48 )
        NVRam_WriteByte( nvrFactoryAddr( BlueColorsRGB ), UserPrefBlueColorSRGB );
    else if( itemIndex == 50 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnesssRGB ), FUserPrefBrightnessSRGB );
    }
    else if( itemIndex == 52 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastsRGB ), FUserPrefContrastSRGB );
    }
    //else if (itemIndex == 54)
    //{
    //    NVRam_WriteByte(nvrMonitorAddr(MonitorFlag), MonitorFlags);
    //}
}
#endif

//=======================================================================================
// For Factory alignment
#if 1

#if !USEFLASH
Bool EraseAllEEPROMCell( void )
{
    if( UserPrefBrightness == 0 && UserPrefContrast == MinContrastValue )
    {
        WORD  i;
#ifdef EEPRom_24C04
        for( i = 0; i < 0x1ff; i++ )
#else
        for( i = 0; i < 0x7ff; i++ )
#endif
            NVRam_WriteByte( i, 0xFF );
        Power_TurnOnAmberLed();
    }
    return TRUE;
}
#else
Bool EraseFlashUserData(void) //2011.9.13 14:54 CC - A022
{

    if( UserPrefBrightness != 0|| UserPrefContrast!= MinContrastValue )
    {   //printMsg("EraseFlashUserData(1)");
        Delay1ms( 200 ); // for Factory check delay,It's can reduce
        return FALSE;
    }

    if( UserPrefBrightness == 0 && UserPrefContrast == MinContrastValue )
    {   //printMsg("EraseFlashUserData(2)");
        FlashSectorErase( TRUE, FLASH_FactorySettingAddr );
        FlashSectorErase( TRUE, FLASH_MonitorSettingAddr );
        FlashSectorErase( TRUE, FLASH_MonitorSetting2Addr );
        FlashSectorErase( TRUE, FLASH_TimingModeAddr );
        FlashSectorErase( TRUE, FLASH_FreeBufferAddr);
        Power_TurnOnAmberLed();
        Delay1ms( 1000 );
    }

    return TRUE;
}
#endif
#if 1
//====================================================

#endif



#if Enable_Gamma
Bool AdjustGamaMode( MenuItemActionType action )
{
    //if (UserPrefColorTemp==CTEMP_SRGB)  //0617
    //  return FALSE;
    if( action == MIA_DecValue )
    {
        UserPrefGamaMode = ( UserPrefGamaMode + GAMA_Nums - 1 ) % GAMA_Nums;
    }
    else
    {
        UserPrefGamaMode = ( UserPrefGamaMode + 1 ) % GAMA_Nums;
    }
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( GamaMode ), UserPrefGamaMode );
#else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#endif

    // 2006/11/9 11:44PM by Emily     if(!BlueFlag||!SkinFlag||!GreenFlag||!ColorAutoDetectFlag)
    {
        //msWriteByte( BK0_00, REGBANK3 );
       // msWriteByte( BK3_40, 0x00 );
       // msWriteByte( BK0_00, REGBANK0 );
        mStar_SetupGamma( UserPrefGamaMode );
#if MWEFunction
        if( UserPrefColorFlags != 0 || UserPrefDcrMode )
#else
        if( UserPrefDcrMode )
#endif
        {
#if !ENABLE_HDMI
            msAccOnOff( 1 );
#endif
           // msWriteByte( BK0_00, REGBANK3 );
           // msWriteByte( BK3_40, 0x01 );
           // msWriteByte( BK0_00, REGBANK0 );
        }
    }
    return TRUE;
}
#endif


Bool SetDCRmode(void)
{
    if (UserPrefDcrMode == MenuItemIndex)
        return FALSE;
    UserPrefDcrMode = MenuItemIndex;
    //printData("========UserprefDSCMode====%x",UserPrefDcrMode);

    msDCROnOff(UserPrefDcrMode, MAIN_WINDOW);

    Set_SaveMonitorSettingFlag();
    return TRUE;
}

#if ENABLE_DSC
Bool AdjustDSCMode( MenuItemActionType action )
{
    action=action;

    if( UserprefDSCMode )
    {
        UserprefDSCMode = 0;
        msDSC_Off_Setting();
    }
    else
    {
        UserprefDSCMode = 1;
        msDSC_On_Setting();
    }

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( DSCMode ), UserprefDSCMode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}
Bool SetDSCmode(void)
{
    if (UserprefDSCMode == MenuItemIndex)
        return FALSE;
    UserprefDSCMode = MenuItemIndex;
    //printData("========UserprefDSCMode====%x",UserprefDSCMode);
    if( UserprefDSCMode )
    {
        msDSC_On_Setting();
    }
    else
    {
        msDSC_Off_Setting();
    }
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif
#if ENABLE_DPS
Bool AdjustDPSMode( MenuItemActionType action )
{
    action=action;

    if( UserprefDPSMode )
    {
        UserprefDPSMode = 0;
        msDPS_Off_Setting();
    }
    else
    {
        UserprefDPSMode = 1;
        msDPS_On_Setting();
    }

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( DPSMode ), UserprefDPSMode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}
Bool SetDPSmode(void)
{
    if (UserprefDPSMode == MenuItemIndex)
        return FALSE;
    UserprefDPSMode = MenuItemIndex;
    //printData("========UserprefDPSMod====%x",UserprefDPSMode);
    if( UserprefDPSMode )
    {
        msDPS_On_Setting();
    }
    else
    {
        msDPS_Off_Setting();
    }
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif
#if ENABLE_DLC
Bool AdjustDLCMode( MenuItemActionType action )
{
    action=action;

    if( UserprefDLCMode )
    {
        UserprefDLCMode = 0;
        g_bDLCOnOff=0;

        msDlcOnOff(1);
    }
    else
    {
        UserprefDLCMode = 1;
        g_bDLCOnOff=0;
        msDlcOnOff(0);
    }

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( DLCMode ), UserprefDLCMode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}
Bool SetDLCmode(void)
{
    if (UserprefDLCMode == MenuItemIndex)
        return FALSE;
    UserprefDLCMode = MenuItemIndex;
    //printData("========UserprefDLCMode====%x",UserprefDLCMode);
    if( UserprefDLCMode )
    {
        g_bDLCOnOff=0;
        msDlcOnOff(1);
    }
    else
    {
        //UserprefDLCMode = 1;
        g_bDLCOnOff=0;
        msDlcOnOff(0);
    }
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif

#if ENABLE_3DLUT
Bool Adjust3DLUTMode( MenuItemActionType action )
{
    action=action;

    UserPref3DLUTmode=MenuItemIndex;
    ms3DLutEnable(MAIN_WINDOW, UserPref3DLUTmode);

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( RGB3DLUTMode ), UserPref3DLUTmode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}
#endif

#if ENABLE_OSD_3D_CONTROL
Bool AdjustT3DMode( MenuItemActionType action )
{
    action=action;

    if( UserPref2Dto3D )
    {
        UserPref2Dto3D = 0;
        g_SetupPathInfo.b2DTo3DEnable = FALSE;
        mStar_SetupMode();
        Sc2DTo3DOff();
    }
    else
    {
        UserPref2Dto3D = 1;
        g_SetupPathInfo.b2DTo3DEnable = TRUE;
        mStar_SetupMode();
/*
        if (InputColorFormat == INPUTCOLOR_YUV)
            LoadACETable( MAIN_WINDOW, (BYTE *)t_Off_ColorSettingTable, DefHue, DefSaturation, UserPrefContrast);
        else if (InputColorFormat == InputColor_RGB)
            LGE_T3D_Photo_2_ColorMatrix();
*/
        Sc2DTo3DInit();
    }

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( T3DMode ), UserPref2Dto3D );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}

Bool AdjustT3DGameMode( MenuItemActionType action )
{
    action=action;

    if ( UserPref2Dto3DGameMode )
    {
        UserPref2Dto3DGameMode = 0;
        msWrite2Byte(REG_2102, 0x0000);
        MenuFunc_printMsg("Exit 2D to 3D Game Mode");
    }
    else
    {
        UserPref2Dto3DGameMode = 1;
        msWrite2Byte(REG_2102, 0x1111);
        MenuFunc_printMsg("Enter 2D to 3D Game Mode");
    }

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( T3DGameMode ), UserPref2Dto3DGameMode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif

    return TRUE;
}

Bool AdjustT3DGain( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPref2Dto3DGain, 0, MAX_T3DGAIN, 1 );
    if( tempValue == UserPref2Dto3DGain )
    {
        return FALSE;
    }
    UserPref2Dto3DGain = tempValue;

    Sc2DTo3DSetPIXGain(t2Dto3DPIXGain[UserPref2Dto3DGain]);
    Sc2DTo3DSetBLKGain(t2Dto3DBLKGain[UserPref2Dto3DGain]);

    return TRUE;
}

WORD GetT3DGainValue( void )
{
        return GetScale100Value(UserPref2Dto3DGain,0,MAX_T3DGAIN);
}

Bool AdjustT3DOffset( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPref2Dto3DOffset, 0, MAX_T3DOFFSET, 1 );
    if( tempValue == UserPref2Dto3DOffset )
    {
        return FALSE;
    }
    UserPref2Dto3DOffset = tempValue;

    Sc2DTo3DSetPIXOffset(t2Dto3DPIXOffset[UserPref2Dto3DOffset]);
    Sc2DTo3DSetBLKOffset(t2Dto3DBLKOffset[UserPref2Dto3DOffset]);

    return TRUE;
}

WORD GetT3DOffsetValue( void )
{
    return GetScale100Value(UserPref2Dto3DOffset,0,MAX_T3DOFFSET);
}
#endif
#if ENABLE_SUPER_RESOLUTION
Bool AdjustSuperResolutionMode( MenuItemActionType action )
{
    if( action == MIA_DecValue )
    {
        UserPrefSuperResolutionMode = ( UserPrefSuperResolutionMode + SRMODE_Nums - 1 ) % SRMODE_Nums;
    }
    else
    {
        UserPrefSuperResolutionMode = ( UserPrefSuperResolutionMode + 1 ) % SRMODE_Nums;
    }

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( SuperResolutionMode ), UserPrefSuperResolutionMode );
#else
    Set_FlashSaveMonitorFlag();
#endif
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
    return TRUE;
}
Bool SetSuperResolutionMode(void)
{
    if (UserPrefSuperResolutionMode== MenuItemIndex)
        return FALSE;
    UserPrefSuperResolutionMode = MenuItemIndex;
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif
#if ENABLE_TNR
Bool AdjustNRMode( MenuItemActionType action )
{
    if( action == MIA_DecValue )
    {
        UserPrefNRmode = ( UserPrefNRmode + NRMODE_Nums - 1 ) % NRMODE_Nums;
    }
    else
    {
        UserPrefNRmode = ( UserPrefNRmode + 1 ) % NRMODE_Nums;
    }

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( NRmode ), UserPrefNRmode );
#else
    Set_FlashSaveMonitorFlag();
#endif
    mStar_SetupNRmode( UserPrefNRmode );
    return TRUE;
}
#endif

#if ENABLE_DeBlocking
Bool AdjustDeBlockingMode( MenuItemActionType action )
{
    action=action;

    // temp, TNR always on/off with deblocking function.
    AdjustTNR(0);

    if( UserPrefDeBlocking)
        UserPrefDeBlocking = 0;
    else
        UserPrefDeBlocking = 1;

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( DeBlocking ), UserPrefDeBlocking );
    #else
        Set_FlashSaveMonitorFlag();
    #endif

    msDeBlockingOnOff(UserPrefDeBlocking, MAIN_WINDOW);

    return TRUE;
}
#endif

//=====================================================================
#if 1

Bool SetFactoryColorTempSRGB( void )
{
    UserPrefColorTemp = CTEMP_SRGB;
    ReadColorTempSetting(); // 2006/11/10 6:58PM by KK ReadSRGBSetting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}

#endif
#if 1

//====================================================

Bool AdjustBrightnessCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightnessCool1, 0, 100, 1 );
    if( tempValue == UserPrefBrightnessCool1 )
    {
        return FALSE;
    }
    UserPrefBrightnessCool1 = tempValue;
    UserPrefBrightness = UserPrefBrightnessCool1;
    if( FactoryModeFlag )
        FUserPrefBrightnessCool1 = UserPrefBrightnessCool1;
    mStar_AdjustBrightness( UserPrefBrightnessCool1 );
    return TRUE;
}
Bool AdjustContrastCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrastCool1, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrastCool1 )
    {
        return FALSE;
    }
    UserPrefContrastCool1 = tempValue;
    UserPrefContrast = UserPrefContrastCool1;
    if( FactoryModeFlag )
        FUserPrefContrastCool1 = UserPrefContrastCool1;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}

WORD GetBrightnessCool1Value( void )
{
    return FUserPrefBrightnessCool1 % 101;
}
WORD GetContrastCool1Value( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastCool1, MinContrastValue, MaxContrastValue );
    return temp;
}
//====================================================

Bool AdjustBrightnessWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightnessWarm1, 0, 100, 1 );
    if( tempValue == UserPrefBrightnessWarm1 )
    {
        return FALSE;
    }
    UserPrefBrightnessWarm1 = tempValue;
    UserPrefBrightness = UserPrefBrightnessWarm1;
    if( FactoryModeFlag )
        FUserPrefBrightnessWarm1 = UserPrefBrightnessWarm1;
    mStar_AdjustBrightness( UserPrefBrightnessWarm1 );
    return TRUE;
}
Bool AdjustContrastWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrastWarm1, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrastWarm1 )
    {
        return FALSE;
    }
    UserPrefContrastWarm1 = tempValue;
    UserPrefContrast = UserPrefContrastWarm1;
    if( FactoryModeFlag )
        FUserPrefContrastWarm1 = UserPrefContrastWarm1;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}

WORD GetBrightnessWarm1Value( void )
{
    return FUserPrefBrightnessWarm1 % 101;
}
WORD GetContrastWarm1Value( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastWarm1, MinContrastValue, MaxContrastValue );
    return temp;
}
//===============================================================

Bool AdjustBrightnessNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightnessNormal, 0, 100, 1 );
    if( tempValue == UserPrefBrightnessNormal )
    {
        return FALSE;
    }
    UserPrefBrightnessNormal = tempValue;
    UserPrefBrightness = UserPrefBrightnessNormal;
    if( FactoryModeFlag )
        FUserPrefBrightnessNormal = UserPrefBrightnessNormal;
    mStar_AdjustBrightness( UserPrefBrightnessNormal );
    return TRUE;
}
Bool AdjustContrastNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrastNormal, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrastNormal )
    {
        return FALSE;
    }
    UserPrefContrastNormal = tempValue;
    UserPrefContrast = UserPrefContrastNormal;
    if( FactoryModeFlag )
        FUserPrefContrastNormal = UserPrefContrastNormal;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}

WORD GetBrightnessNormalValue( void )
{
    //   return FUserPrefBrightnessNormal % 101;
    return FUserPrefBrightnessNormal % 101;
}
WORD GetContrastNormalValue( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastNormal, MinContrastValue, MaxContrastValue );
    return temp;
}
//====================================================
Bool AdjustRedColorSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorSRGB, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorSRGB )
    {
        return FALSE;
    }
    UserPrefRedColorSRGB = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustRedColor( UserPrefRedColorSRGB, UserPrefContrastSRGB );
#endif
    return TRUE;
}
Bool AdjustGreenColorSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorSRGB, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorSRGB )
    {
        return FALSE;
    }
    UserPrefGreenColorSRGB = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustGreenColor( UserPrefGreenColorSRGB, UserPrefContrastSRGB );
#endif
    return TRUE;
}
Bool AdjustBlueColorSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorSRGB, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorSRGB )
    {
        return FALSE;
    }
    UserPrefBlueColorSRGB = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustBlueColor( UserPrefBlueColorSRGB, UserPrefContrastSRGB );
#endif
    return TRUE;
}
Bool AdjustBrightnessSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, FUserPrefBrightnessSRGB, 0, 100, 1 );
    if( tempValue == FUserPrefBrightnessSRGB )
    {
        return FALSE;
    }
    FUserPrefBrightnessSRGB = tempValue;
    UserPrefBrightness = FUserPrefBrightnessSRGB;
    mStar_AdjustBrightness( UserPrefBrightness );
    return TRUE;
}
Bool AdjustContrastSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, FUserPrefContrastSRGB, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == FUserPrefContrastSRGB )
    {
        return FALSE;
    }
    FUserPrefContrastSRGB = tempValue;
    UserPrefContrast = FUserPrefContrastSRGB;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
WORD GetRColorSRGBValue( void )
{
    return UserPrefRedColorSRGB;
}
WORD GetGColorSRGBValue( void )
{
    return UserPrefGreenColorSRGB;
}
WORD GetBColorSRGBValue( void )
{
    return UserPrefBlueColorSRGB;
}
WORD GetBrightnessSRGBValue( void )
{
    return FUserPrefBrightnessSRGB % 101;
}
WORD GetContrastSRGBValue( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastSRGB, MinContrastValue, MaxContrastValue );
    return temp;
}
#endif
//======================================================================================



//====================================================
WORD GetProductionValue( void )
{
    return ( ProductModeFlag) ? ( 1 ) : ( 0 );
}
Bool AdjustProduction( MenuItemActionType action )
{
    action = action;
    if( UserPrefBrightness == 0 && UserPrefContrast == MinContrastValue )
    {
        if( ProductModeFlag )
        {
            #if USEFLASH
            FactoryProductModeValue = ProductModeOffValue;
            #else
            WORD u16TempValue = ProductModeOffValue;
            NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &u16TempValue ), 2 );
            #endif
            Clr_ProductModeFlag();
        }
        else
        {
            #if USEFLASH
            FactoryProductModeValue = ProductModeOnValue;
            #else
            WORD u16TempValue = ProductModeOnValue;
            NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &u16TempValue ), 2 );
            #endif
            Set_ProductModeFlag();
        }
        #if USEFLASH
        //SaveMonitorSetting();
        Set_FlashForceSaveMonitor2Flag();
        #else
        NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), System2Flags );
        #endif
        Delay1ms( 200 );
     }
    return TRUE;
}

WORD GetDDCWPActiveValue( void )
{
    return (( DDCWPActiveFlag) ? ( 1 ) : ( 0 ));
}
Bool AdjustDDCWP( MenuItemActionType action )
{
    action = action;
    if( DDCWPActiveFlag )
    {
        g_bServiceEDIDUnLock = 1;
        hw_ClrDDC_WP();
    }
    else
    {
       g_bServiceEDIDUnLock = 0;
       hw_SetDDC_WP();
    }
    Delay1ms( 200 );
    return TRUE;
}
//====================================================
Bool AdjustBankNo( MenuItemActionType action )
{
    if( action == MIA_IncValue )
    {
        MSBankNo = ( MSBankNo + 1 ) % 3;
    }
    else
    {
        MSBankNo = ( MSBankNo + 2 ) % 3;
    }
    return TRUE;
}
Bool AdjustAddr( MenuItemActionType action )
{
    if( action == MIA_IncValue )
    {
        MSAddr = ( MSAddr + 1 ) % 0x100;
    }
    else
    {
        MSAddr = ( MSAddr + 255 ) % 0x100;
    }
    return TRUE;
}
WORD GetAddrValue( void )
{
    return MSAddr;
}
Bool AdjustRegValue( MenuItemActionType action )
{
    if( action == MIA_IncValue )
    {
        MSValue = ( MSValue + 1 ) % 0x100;
    }
    else
    {
        MSValue = ( MSValue + 255 ) % 0x100;
    }
    return TRUE;
}
WORD GetAddrRegValue( void )
{
    if( MSBankNo == 0 )        // scaler
    {
        MSValue = msReadByte( MSAddr );
    }
    else if( MSBankNo == 1 )        // adc
    {
        // wait for coding
        //msWriteByte( BK0_00, REGBANK1 );
        MSValue = msReadByte( MSAddr );
        //msWriteByte( BK0_00, REGBANK0 );
    }
    else if( MSBankNo == 2 )        // TCON
    {
        // wait for coding
        //msWriteByte( BK0_00, REGBANK2 );
        MSValue = msReadByte( MSAddr );
        //msWriteByte( BK0_00, REGBANK0 );
    }
    return MSValue;
}
WORD GetRegValue( void )
{
    return MSValue;
}
Bool WriteRegValue( void )
{
    if( MSBankNo == 0 )        // scaler
    {
        msWriteByte( MSAddr, MSValue );
    }
    else if( MSBankNo == 1 )        // adc
    {
        // wait for coding
        //msWriteByte( BK0_00, REGBANK1 );
        msWriteByte( MSAddr, MSValue );
        //msWriteByte( BK0_00, REGBANK0 );
    }
    else if( MSBankNo == 2 )        // TCON
    {
        // wait for coding
        //msWriteByte( BK0_00, REGBANK2 );
        msWriteByte( MSAddr, MSValue );
        //msWriteByte( BK0_00, REGBANK0 );
    }
    return TRUE;
}


#endif
WORD GetPanelOnTimeValue( void )
{
    WORD hour;
    if( BlacklitTime > 235926000ul )
        // 655365*3600
        hour = 0xFFFF;
    else
        hour = ( DWORD )BlacklitTime / 3600;
    return hour;
}

/*
WORD GetSSCStepValue( void )
{
    WORD StepValue;
    StepValue=msRead2Byte(BK0_D8);
    return StepValue;
}
WORD GetSSCSpanValue( void )
{
    WORD SpanValue;
    SpanValue=msRead2Byte(BK0_DA);
    return SpanValue;
}
*/


#if ENABLE_TNR
Bool AdjustTNR( MenuItemActionType action )
{
    action = action;
    if( TNROnFlag )
    {
        Clr_TNROnFlag();
        msTNROnOff( FALSE );
    }
    else
    {
        Set_TNROnFlag();
        msTNROnOff( TRUE );
#if ENABLE_RTE && (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||CHIP_ID == CHIP_TSUMJ)
        if(UserprefOverDriveSwitch)
        {
            UserprefOverDriveSwitch = 0;
            msOverDriveOnOff( FALSE );
        }
#endif
    }
#if USEFLASH
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#else
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#endif
    Delay1ms( 200 );
    return TRUE;
}
#endif
#if PanelRSDS
WORD GetVCOMValue( void )
{
    return UserPrefVcomValue;
}

Bool AdjustVCOMValue( MenuItemActionType action )
{
    BYTE tempBank = msReadByte( BK0_00 );
    msWriteByte( BK0_00, REGBANK0 );

    if( action == MIA_IncValue )
    {
        UserPrefVcomValue = UserPrefVcomValue + 1;
    }
    else
    {
        UserPrefVcomValue = UserPrefVcomValue + 255;
    }
    msWriteByte( BK0_C5, UserPrefVcomValue );

    msWriteByte( BK0_00, tempBank );
    return TRUE;
}
#endif
#if ENABLE_SUPER_RESOLUTION
BYTE xdata SRmodeContent = SRMODE_Nums;
code RegUnitType tSR_Mode_OFF_Tbl[]=
{
    //STD_Off
    { REG_2F00,     0x0B},
    { REG_2F20,     0x00},
    { REG_2F60,     0x00},
    { REG_2F21,     0x01},
    { REG_2F30,     0x10},
    { REG_2F22,     0x00},
    { REG_2F31,     0x10},
    { REG_2F32,     0x10},
    { REG_2F33,     0x10},
    { REG_2F62,     0x00},
    { REG_2F64,     0xFF},
    { REG_2F80,     0x15},
    { REG_2F81,     0x15},
    { REG_2F82,     0x10},
    { REG_2F83,     0x10},
    { REG_2F84,     0x15},
    { REG_2F85,     0x15},
    { REG_2F86,     0x20},
    { REG_2F87,     0x20},
    { REG_2F88,     0x20},
    { REG_2F89,     0x20},
    { REG_2F8A,     0x20},
    { REG_2F8B,     0x20},
    { REG_2F8C,     0x20},
    { REG_2F8D,     0x20},
    { REG_2F8E,     0x20},
    { REG_2F8F,     0x20},
    { REG_2F40,     0xFF},
    { REG_2F48,     0xFF},
    { REG_2F41,     0xFF},
    { REG_2F49,     0xFF},
    { REG_2F42,     0xFF},
    { REG_2F4A,     0xFF},
    { REG_2F43,     0xFF},
    { REG_2F4B,     0xFF},
    { REG_2F38,     0x10},
    { REG_2F39,     0x32},
    { REG_2F3C,     0x10},
    { REG_2F3D,     0x32},
    { REG_2F27,     0x10},
    //{ REG_2FA0,   0x00},
    { REG_2FA1,     0x00},
    { REG_2F00,     0x00},
    { REG_2F10,     0x00},
    { REG_2F14,   ( PANEL_HEIGHT - 1 ) & 0xFF},
    { REG_2F15,   ( PANEL_HEIGHT - 1 ) >> 8},
    { REG_2F00,     0x0B},
    { REG_2F25,     0x00},
    { REG_2F34,     0x08},
    { REG_2F23,     0xFF},
    { REG_2F37,     0x08},
    { REG_2F65,     0xEF},
    { REG_2F90,     0x1C},
    { REG_2F91,     0x1C},
    { REG_2F92,     0x20},
    { REG_2F93,     0x20},
    { REG_2F94,     0x24},
    { REG_2F95,     0x24},
    { REG_2F96,     0x28},
    { REG_2F97,     0x28},
    { REG_2F98,     0x24},
    { REG_2F99,     0x24},
    { REG_2F9A,     0x20},
    { REG_2F9B,     0x20},
    { REG_2F9C,     0x18},
    { REG_2F9D,     0x18},
    { REG_2F9E,     0x10},
    { REG_2F9F,     0x10},
    { REG_2F44,     0xFF},
    { REG_2F4C,     0xFF},
    { REG_2F47,     0xFF},
    { REG_2F4F,     0xFF},
    { REG_2F3A,     0x54},
    { REG_2F3B,     0x76},
    { REG_2F3E,     0x54},
    { REG_2F3F,     0x76},
    { REG_2F26,     0x42},
    { REG_2F61,     0x33},
    { REG_2F35,     0x08},
    { REG_2F36,     0x08},
    { REG_2F68,     0x00},
    { REG_2F69,     0x00},
    { REG_2F45,     0xFF},
    { REG_2F4D,     0xFF},
    { REG_2F46,     0xFF},
    { REG_2F4E,     0xFF},
    { REG_2FC0,     0x00},
    { REG_2FC2,     0x00},
    { REG_2FE0,     0x00},
    { REG_2FE4,     0x00},
    { REG_2FE5,     0x00},
    { REG_2FE2,     0x00},
    { REG_2FC1,     0x00},
    { REG_2FC3,     0x00},
    { REG_2FEA,     0x00},
    { REG_2FEE,     0x00},
    { REG_2FEF,     0x00},
    { REG_2FEC,     0x00},
    { REG_2F24,     0x00},
    { REG_2F72,     0x00},
    { REG_2FF4,     0x00},
    { REG_2FF8,     0x00},
    { REG_2FF9,     0x00},
    { REG_2FF6,     0x00}
};
code RegUnitType tSR_Mode_H_Tbl[]=
{
    //STD_H for T3D
    {REG_2F00 ,     0x0B},
    {REG_2F20 ,     0x81},
    {REG_2F60 ,     0x00},
    {REG_2F21 ,     0x83},
    {REG_2F30 ,     0x2F},
    {REG_2F22 ,     0x57},
    {REG_2F31 ,     0x2F},
    {REG_2F32 ,     0x10},
    {REG_2F33 ,     0x10},
    {REG_2F62 ,     0x9F},
    {REG_2F64 ,     0xFF},
    {REG_2F80 ,     0x15},
    {REG_2F81 ,     0x15},
    {REG_2F82 ,     0x10},
    {REG_2F83 ,     0x10},
    {REG_2F84 ,     0x15},
    {REG_2F85 ,     0x15},
    {REG_2F86 ,     0x20},
    {REG_2F87 ,     0x20},
    {REG_2F88 ,     0x20},
    {REG_2F89 ,     0x20},
    {REG_2F8A ,     0x20},
    {REG_2F8B ,     0x20},
    {REG_2F8C ,     0x20},
    {REG_2F8D ,     0x20},
    {REG_2F8E ,     0x20},
    {REG_2F8F ,     0x20},
    {REG_2F40 ,     0xD0},
    {REG_2F48 ,     0xD0},
    {REG_2F41 ,     0xD0},
    {REG_2F49 ,     0xD0},
    {REG_2F42 ,     0x40},
    {REG_2F4A ,     0x40},
    {REG_2F43 ,     0x40},
    {REG_2F4B ,     0x40},
    {REG_2F38 ,     0x10},
    {REG_2F39 ,     0x32},
    {REG_2F3C ,     0x10},
    {REG_2F3D ,     0x32},
    {REG_2F27 ,     0x10},
    {REG_2FA1 ,     0x00},
    {REG_2F00 ,     0x00},
    {REG_2F10 ,     0x02},
    {REG_2F14 ,    ( PANEL_HEIGHT - 1 + 2 ) & 0xFF},
    {REG_2F15 ,    ( PANEL_HEIGHT - 1 + 2 ) >> 8},
    {REG_2F00 ,     0x0B},
    {REG_2F25 ,     0x00},
    {REG_2F34 ,     0x1C},
    {REG_2F23 ,     0xFF},
    {REG_2F37 ,     0x1C},
    {REG_2F65 ,     0xEF},
    {REG_2F90 ,     0x1C},
    {REG_2F91 ,     0x1C},
    {REG_2F92 ,     0x20},
    {REG_2F93 ,     0x20},
    {REG_2F94 ,     0x24},
    {REG_2F95 ,     0x24},
    {REG_2F96 ,     0x28},
    {REG_2F97 ,     0x28},
    {REG_2F98 ,     0x24},
    {REG_2F99 ,     0x24},
    {REG_2F9A ,     0x20},
    {REG_2F9B ,     0x20},
    {REG_2F9C ,     0x18},
    {REG_2F9D ,     0x18},
    {REG_2F9E ,     0x10},
    {REG_2F9F ,     0x10},
    {REG_2F44 ,     0x40},
    {REG_2F4C ,     0x40},
    {REG_2F47 ,     0x60},
    {REG_2F4F ,     0x60},
    {REG_2F3A ,     0x54},
    {REG_2F3B ,     0x76},
    {REG_2F3E ,     0x54},
    {REG_2F3F ,     0x76},
    {REG_2F26 ,     0x88},
    {REG_2F61 ,     0x03},
    {REG_2F35 ,     0x1F},
    {REG_2F36 ,     0x1F},
    {REG_2F68 ,     0x70},
    {REG_2F69 ,     0x07},
    {REG_2F45 ,     0x40},
    {REG_2F4D ,     0x40},
    {REG_2F46 ,     0x40},
    {REG_2F4E ,     0x40},
    {REG_2FC0 ,     0x04},
    {REG_2FC2 ,     0xFF},
    {REG_2FE0 ,     0x00},
    {REG_2FE4 ,     0x00},
    {REG_2FE5 ,     0x00},
    {REG_2FE2 ,     0x00},
    {REG_2FC1 ,     0x04},
    {REG_2FC3 ,     0xFF},
    {REG_2FEA ,     0x00},
    {REG_2FEE ,     0x00},
    {REG_2FEF ,     0x00},
    {REG_2FEC ,     0x00},
    {REG_2F24 ,     0x04},
    {REG_2F72 ,     0x00},
    {REG_2FF4 ,     0x00},
    {REG_2FF8 ,     0x00},
    {REG_2FF9 ,     0x00}
};

code RegUnitType tSR_Mode_M_Tbl[]=
{
    //STD_M
    {REG_2F00 ,    0x0B },
    {REG_2F20 ,    0x81 },
    {REG_2F60 ,    0x00 },
    {REG_2F21 ,    0xF3 },
    {REG_2F30 ,    0x1F },
    {REG_2F22 ,    0x57 },
    {REG_2F31 ,    0x13 },
    {REG_2F32 ,    0x10 },
    {REG_2F33 ,    0x10 },
    {REG_2F62 ,    0x9F },
    {REG_2F64 ,    0xFF },
    {REG_2F80 ,    0x15 },
    {REG_2F81 ,    0x15 },
    {REG_2F82 ,    0x10 },
    {REG_2F83 ,    0x10 },
    {REG_2F84 ,    0x15 },
    {REG_2F85 ,    0x15 },
    {REG_2F86 ,    0x20 },
    {REG_2F87 ,    0x20 },
    {REG_2F88 ,    0x20 },
    {REG_2F89 ,    0x20 },
    {REG_2F8A ,    0x20 },
    {REG_2F8B ,    0x20 },
    {REG_2F8C ,    0x20 },
    {REG_2F8D ,    0x20 },
    {REG_2F8E ,    0x20 },
    {REG_2F8F ,    0x20 },
    {REG_2F40 ,    0x20 },
    {REG_2F48 ,    0x20 },
    {REG_2F41 ,    0x20 },
    {REG_2F49 ,    0x20 },
    {REG_2F42 ,    0x20 },
    {REG_2F4A ,    0x20 },
    {REG_2F43 ,    0x20 },
    {REG_2F4B ,    0x20 },
    {REG_2F38 ,    0x10 },
    {REG_2F39 ,    0x32 },
    {REG_2F3C ,    0x10 },
    {REG_2F3D ,    0x32 },
    {REG_2F27 ,    0x10 },
    //{REG_2FA0 ,    0x00 },
    {REG_2FA1 ,    0x00 },
    {REG_2F00 ,    0x00 },
    {REG_2F10 ,    0x02 },
    {REG_2F14 ,    ( PANEL_HEIGHT - 1 + 2 ) & 0xFF},
    {REG_2F15 ,    ( PANEL_HEIGHT - 1 + 2 ) >> 8},
    {REG_2F00 ,    0x0B},
    {REG_2F25 ,    0x00 },
    {REG_2F34 ,    0x0F },
    {REG_2F23 ,    0xFF },
    {REG_2F37 ,    0x0F },
    {REG_2F65 ,    0xEF },
    {REG_2F90 ,    0x1C },
    {REG_2F91 ,    0x1C },
    {REG_2F92 ,    0x20 },
    {REG_2F93 ,    0x20 },
    {REG_2F94 ,    0x24 },
    {REG_2F95 ,    0x24 },
    {REG_2F96 ,    0x28 },
    {REG_2F97 ,    0x28 },
    {REG_2F98 ,    0x24 },
    {REG_2F99 ,    0x24 },
    {REG_2F9A ,    0x20 },
    {REG_2F9B ,    0x20 },
    {REG_2F9C ,    0x18 },
    {REG_2F9D ,    0x18 },
    {REG_2F9E ,    0x10 },
    {REG_2F9F ,   0x10 },
    {REG_2F44 ,    0x18 },
    {REG_2F4C ,    0x18 },
    {REG_2F47 ,    0x18 },
    {REG_2F4F ,    0x18 },
    {REG_2F3A ,    0x54 },
    {REG_2F3B ,    0x76 },
    {REG_2F3E ,    0x54 },
    {REG_2F3F ,    0x76 },
    {REG_2F26 ,    0x66 },
    {REG_2F61 ,    0x03 },
    {REG_2F35 ,    0x0F },
    {REG_2F36 ,    0x0F },
    {REG_2F68 ,    0x40 },
    {REG_2F69 ,    0x04 },
    {REG_2F45 ,    0x20 },
    {REG_2F4D ,    0x20 },
    {REG_2F46 ,    0x20 },
    {REG_2F4E ,    0x20 },
    {REG_2FC0 ,    0x84 },
    {REG_2FC2 ,    0xFF },
    {REG_2FE0 ,    0x00 },
    {REG_2FE4 ,    0x00 },
    {REG_2FE5 ,    0x00 },
    {REG_2FE2 ,    0x00 },
    {REG_2FC1 ,    0x84 },
    {REG_2FC3 ,    0xFF },
    {REG_2FEA ,    0x00 },
    {REG_2FEE ,    0x00 },
    {REG_2FEF ,    0x00 },
    {REG_2FEC ,    0x00 },
    {REG_2F24 ,    0x84 },
    {REG_2F72 ,    0x00 },
    {REG_2FF4 ,    0x00 },
    {REG_2FF8 ,    0x00 },
    {REG_2FF9 ,    0x00 },
    {REG_2FF6 ,    0x00 }
};
code RegUnitType tSR_Mode_L_Tbl[]=
{
    //STD_L
    {REG_2F00 ,    0x0B},
    {REG_2F20 ,    0x81},
    {REG_2F60 ,    0x00},
    {REG_2F21 ,    0xF3},
    {REG_2F30 ,    0x1D},
    {REG_2F22 ,    0x57},
    {REG_2F31 ,    0x07},
    {REG_2F32 ,    0x10},
    {REG_2F33 ,    0x10},
    {REG_2F62 ,    0x9F},
    {REG_2F64 ,    0xFF},
    {REG_2F80 ,    0x15},
    {REG_2F81 ,    0x15},
    {REG_2F82 ,    0x10},
    {REG_2F83 ,    0x10},
    {REG_2F84 ,    0x15},
    {REG_2F85 ,    0x15},
    {REG_2F86 ,    0x20},
    {REG_2F87 ,    0x20},
    {REG_2F88 ,    0x20},
    {REG_2F89 ,    0x20},
    {REG_2F8A ,    0x20},
    {REG_2F8B ,    0x20},
    {REG_2F8C ,    0x20},
    {REG_2F8D ,    0x20},
    {REG_2F8E ,    0x20},
    {REG_2F8F ,    0x20},
    {REG_2F40 ,    0x20},
    {REG_2F48 ,    0x20},
    {REG_2F41 ,    0x20},
    {REG_2F49 ,    0x20},
    {REG_2F42 ,    0x20},
    {REG_2F4A ,    0x20},
    {REG_2F43 ,    0x20},
    {REG_2F4B ,    0x20},
    {REG_2F38 ,    0x10},
    {REG_2F39 ,    0x32},
    {REG_2F3C ,    0x10},
    {REG_2F3D ,    0x32},
    {REG_2F27 ,    0x10},
    //{REG_2FA0 ,    0x00},
    {REG_2FA1 ,    0x00},
    {REG_2F00,     0x00},
    {REG_2F10 ,    0x02},
    {REG_2F14,    ( PANEL_HEIGHT - 1 + 2 ) & 0xFF},
    {REG_2F15,    ( PANEL_HEIGHT - 1 + 2 ) >> 8},
    {REG_2F00,     0x0B},
    {REG_2F25 ,    0x00},
    {REG_2F34 ,    0x09},
    {REG_2F23 ,    0xFF},
    {REG_2F37 ,    0x09},
    {REG_2F65 ,    0xEF},
    {REG_2F90 ,    0x1C},
    {REG_2F91 ,    0x1C},
    {REG_2F92 ,    0x20},
    {REG_2F93 ,    0x20},
    {REG_2F94 ,    0x24},
    {REG_2F95 ,    0x24},
    {REG_2F96 ,    0x28},
    {REG_2F97 ,    0x28},
    {REG_2F98 ,    0x24},
    {REG_2F99 ,    0x24},
    {REG_2F9A ,    0x20},
    {REG_2F9B ,    0x20},
    {REG_2F9C ,    0x18},
    {REG_2F9D ,    0x18},
    {REG_2F9E ,    0x10},
    {REG_2F9F ,    0x10},
    {REG_2F44 ,    0x18},
    {REG_2F4C ,    0x18},
    {REG_2F47 ,    0x18},
    {REG_2F4F ,    0x18},
    {REG_2F3A ,    0x54},
    {REG_2F3B ,    0x76},
    {REG_2F3E ,    0x54},
    {REG_2F3F ,    0x76},
    {REG_2F26 ,    0x33},
    {REG_2F61 ,    0x03},
    {REG_2F35 ,    0x0F},
    {REG_2F36 ,    0x0F},
    {REG_2F68 ,    0x40},
    {REG_2F69 ,    0x04},
    {REG_2F45 ,    0x20},
    {REG_2F4D ,    0x20},
    {REG_2F46 ,    0x20},
    {REG_2F4E ,    0x20},
    {REG_2FC0 ,    0x84},
    {REG_2FC2 ,    0xFF},
    {REG_2FE0 ,    0x00},
    {REG_2FE4 ,    0x00},
    {REG_2FE5 ,    0x00},
    {REG_2FE2 ,    0x00},
    {REG_2FC1 ,    0x84},
    {REG_2FC3 ,    0xFF},
    {REG_2FEA ,    0x00},
    {REG_2FEE ,    0x00},
    {REG_2FEF ,    0x00},
    {REG_2FEC ,    0x00},
    {REG_2F24 ,    0x84},
    {REG_2F72 ,    0x00},
    {REG_2FF4 ,    0x00},
    {REG_2FF8 ,    0x00},
    {REG_2FF9 ,    0x00},
    {REG_2FF6 ,    0x00}
};

void mStar_SetupSuperResolution( BYTE SuperResolutionMode )
{
    BYTE i=0;
    if((SRmodeContent == SuperResolutionMode))
    {
        if(SuperResolutionMode!=SRMODE_OFF)
        {
             msWriteByte(SC0_14, (msReadByte(SC0_14)+2));
        }
	 return;
    }

    switch(SuperResolutionMode)
    {
        case SRMODE_STRONG:
        MenuFunc_printMsg("tSR_Mode_H_Tbl");
        for(i=0;i<sizeof(tSR_Mode_H_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tSR_Mode_H_Tbl[i].u16Reg,tSR_Mode_H_Tbl[i].u8Value);
            break;

        case SRMODE_MIDDLE :
        MenuFunc_printMsg("tSR_Mode_M_Tbl");
        for(i=0;i<sizeof(tSR_Mode_M_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tSR_Mode_M_Tbl[i].u16Reg,tSR_Mode_M_Tbl[i].u8Value);
            break;

        case SRMODE_WEAK:
        MenuFunc_printMsg("tSR_Mode_L_Tbl");
        for(i=0;i<sizeof(tSR_Mode_L_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tSR_Mode_L_Tbl[i].u16Reg,tSR_Mode_L_Tbl[i].u8Value);
            break;

        default:
        MenuFunc_printData("tSR_Mode_OFF_Tbl",0);
        for(i=0;i<sizeof(tSR_Mode_OFF_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tSR_Mode_OFF_Tbl[i].u16Reg,tSR_Mode_OFF_Tbl[i].u8Value);
            break;
    }
    SRmodeContent = SuperResolutionMode;
}
#endif

#if ENABLE_TNR
static BYTE xdata NRmodeContent = NRMODE_Nums;
code RegUnitType tNR_Mode_OFF_Tbl[]=
{
    //STD_Off
    {REG_2F00   ,0x0C},
    {REG_2F42   ,0x00},
    {REG_2F80   ,0x00},
    {REG_2F81   ,0x00},
    {REG_2F82   ,0x00},
    {REG_2F83   ,0x00},
    {REG_2F84   ,0x00},
    {REG_2F85   ,0x00},
    {REG_2F86   ,0x00},
    {REG_2F87   ,0x00},
    {REG_2F00   ,0x02},
    {REG_2FA0   ,0x00},
    {REG_2FA8   ,0x00},
    {REG_2FAA   ,0x00},
    {REG_2FB8   ,0x10},
    {REG_2FB9   ,0x32},
    {REG_2FBA   ,0x54},
    {REG_2FBB   ,0x76},
    {REG_2FBC   ,0x98},
    {REG_2FBD   ,0xBA},
    {REG_2FBE   ,0xDC},
    {REG_2FBF   ,0xFE},
    {REG_2F00   ,0x04},
    {REG_2F20   ,0x00},
    {REG_2F21   ,0x0F},
    {REG_2F22   ,0x00},
    {REG_2F23   ,0x00},
    {REG_2F24   ,0xFF},
    {REG_2F25   ,0x07},
    {REG_2F80   ,0x01},
    {REG_2F81   ,0x0F},
    {REG_2F82   ,0x00},
    {REG_2F84   ,0xFF},
    {REG_2FA0   ,0x08},
    {REG_2FA1   ,0x07},
    {REG_2FA2   ,0x0A},
    {REG_2FA3   ,0x08},
    {REG_2FA4   ,0x00},
    {REG_2FA5   ,0x00},
    {REG_2FA6   ,0x00},
    {REG_2FA7   ,0x03},
    {REG_2FAA   ,0x10},
    {REG_2FAB   ,0x32},
    {REG_2FAC   ,0x54},
    {REG_2FAD   ,0x76},
    {REG_2FAE   ,0x98},
    {REG_2FAF   ,0xBA},
    {REG_2FB1   ,0xDC},
    {REG_2FB2   ,0xFE},
    {REG_2F00   ,0x02},
    {REG_2F60   ,0x10},
    {REG_2F61   ,0xFF},
    {REG_2F62   ,0x00},
    {REG_2F68   ,0xEF},
    {REG_2F69   ,0xCD},
    {REG_2F6A   ,0xAB},
    {REG_2F6B   ,0x89},
    {REG_2F6C   ,0x67},
    {REG_2F6D   ,0x45},
    {REG_2F6E   ,0x23},
    {REG_2F6F   ,0x01},
    {REG_2FE0   ,0xE0},
    {REG_2F20   ,0x00},
    {REG_2F21   ,0x30},
    {REG_2F22   ,0xEF},
    {REG_2F23   ,0x00},
    {REG_2F28   ,0xEF},
    {REG_2F29   ,0xCD},
    {REG_2F2A   ,0xAB},
    {REG_2F2B   ,0x89},
    {REG_2F2C   ,0x67},
    {REG_2F2D   ,0x45},
    {REG_2F2E   ,0x23},
    {REG_2F2F   ,0x01},
    {REG_2F72   ,0x07},
    {REG_2F73   ,0x09},
    {REG_2FE2   ,0x00}

};
code RegUnitType tNR_Mode_H_Tbl[]=
{
    //STD_H
    {REG_2F00   ,0x0C},
    {REG_2F42   ,0x03},
    {REG_2F4A   ,0x02},
    {REG_2F80   ,0xFF},
    {REG_2F81   ,0xF6},
    {REG_2F82   ,0xB4},
    {REG_2F83   ,0x84},
    {REG_2F84   ,0x62},
    {REG_2F85   ,0x10},
    {REG_2F86   ,0x05},
    {REG_2F87   ,0x00},
    {REG_2F00   ,0x02},
    {REG_2FA0   ,0x03},
    {REG_2FA8   ,0x03},
    {REG_2FAA   ,0xFF},
    {REG_2FB8   ,0x10},
    {REG_2FB9   ,0x32},
    {REG_2FBA   ,0x54},
    {REG_2FBB   ,0x76},
    {REG_2FBC   ,0x98},
    {REG_2FBD   ,0xBA},
    {REG_2FBE   ,0xDC},
    {REG_2FBF   ,0xFE},
    {REG_2F00   ,0x04},
    {REG_2F20   ,0x03},
    {REG_2F21   ,0x1F},
    {REG_2F22   ,0x20},
    {REG_2F23   ,0x00},
    {REG_2F24   ,0x10},
    {REG_2F25   ,0x00},
    {REG_2F80   ,0x01},
    {REG_2F81   ,0x0F},
    {REG_2F82   ,0x00},
    {REG_2F84   ,0xFF},
    {REG_2FA0   ,0x2B},
    {REG_2FA1   ,0x0F},
    {REG_2FA2   ,0x0A},
    {REG_2FA3   ,0x08},
    {REG_2FA4   ,0x00},
    {REG_2FA5   ,0x00},
    {REG_2FA6   ,0x00},
    {REG_2FA7   ,0x03},
    {REG_2FAA   ,0x10},
    {REG_2FAB   ,0x32},
    {REG_2FAC   ,0x54},
    {REG_2FAD   ,0x76},
    {REG_2FAE   ,0x98},
    {REG_2FAF   ,0xBA},
    {REG_2FB1   ,0xDC},
    {REG_2FB2   ,0xFE},
    {REG_2F00   ,0x02},
    {REG_2F60   ,0x13},
    {REG_2F61   ,0xFF},
    {REG_2F62   ,0xEF},
    {REG_2F68   ,0xEF},
    {REG_2F69   ,0xCD},
    {REG_2F6A   ,0xAB},
    {REG_2F6B   ,0x89},
    {REG_2F6C   ,0x67},
    {REG_2F6D   ,0x45},
    {REG_2F6E   ,0x23},
    {REG_2F6F   ,0x01},
    {REG_2FE0   ,0xE0},
    {REG_2F20   ,0x11},
    {REG_2F21   ,0x30},
    {REG_2F22   ,0xEF},
    {REG_2F23   ,0x00},
    {REG_2F28   ,0xEF},
    {REG_2F29   ,0xCD},
    {REG_2F2A   ,0xAB},
    {REG_2F2B   ,0x89},
    {REG_2F2C   ,0x67},
    {REG_2F2D   ,0x45},
    {REG_2F2E   ,0x23},
    {REG_2F2F   ,0x01},
    {REG_2F72   ,0x07},
    {REG_2F73   ,0x09},
    {REG_2FE2   ,0x00}
};
code RegUnitType tNR_Mode_M_Tbl[]=
{
    //STD_M
    {REG_2F00   ,0x0C},
    {REG_2F42   ,0x03},
    {REG_2F4A     ,0x02},
    {REG_2F80   ,0xB9},
    {REG_2F81   ,0xA9},
    {REG_2F82   ,0x76},
    {REG_2F83   ,0x34},
    {REG_2F84   ,0x12},
    {REG_2F85   ,0x00},
    {REG_2F86   ,0x00},
    {REG_2F87   ,0x00},
    {REG_2F00   ,0x02},
    {REG_2FA0   ,0x03},
    {REG_2FA8   ,0x02},
    {REG_2FAA   ,0xFF},
    {REG_2FB8   ,0x10},
    {REG_2FB9   ,0x32},
    {REG_2FBA   ,0x54},
    {REG_2FBB   ,0x76},
    {REG_2FBC   ,0x98},
    {REG_2FBD   ,0xBA},
    {REG_2FBE   ,0xDC},
    {REG_2FBF   ,0xFE},
    {REG_2F00   ,0x04},
    {REG_2F20   ,0x03},
    {REG_2F21   ,0x0F},
    {REG_2F22   ,0x20},
    {REG_2F23   ,0x00},
    {REG_2F24   ,0x10},
    {REG_2F25   ,0x00},
    {REG_2F80   ,0x01},
    {REG_2F81   ,0x0F},
    {REG_2F82   ,0x00},
    {REG_2F84   ,0xFF},
    {REG_2FA0   ,0x2B},
    {REG_2FA1   ,0x07},
    {REG_2FA2   ,0x0A},
    {REG_2FA3   ,0x08},
    {REG_2FA4   ,0x00},
    {REG_2FA5   ,0x00},
    {REG_2FA6   ,0x00},
    {REG_2FA7   ,0x03},
    {REG_2FAA   ,0x10},
    {REG_2FAB   ,0x32},
    {REG_2FAC   ,0x54},
    {REG_2FAD   ,0x76},
    {REG_2FAE   ,0x98},
    {REG_2FAF   ,0xBA},
    {REG_2FB1   ,0xDC},
    {REG_2FB2   ,0xFE},
    {REG_2F00   ,0x02},
    {REG_2F60   ,0x13},
    {REG_2F61   ,0xFF},
    {REG_2F62   ,0xE7},
    {REG_2F68   ,0xEF},
    {REG_2F69   ,0xCD},
    {REG_2F6A   ,0xAB},
    {REG_2F6B   ,0x89},
    {REG_2F6C   ,0x67},
    {REG_2F6D   ,0x45},
    {REG_2F6E   ,0x23},
    {REG_2F6F   ,0x01},
    {REG_2FE0   ,0xE0},
    {REG_2F20   ,0x11},
    {REG_2F21   ,0x30},
    {REG_2F22   ,0xE7},
    {REG_2F23   ,0x00},
    {REG_2F28   ,0xEF},
    {REG_2F29   ,0xCD},
    {REG_2F2A   ,0xAB},
    {REG_2F2B   ,0x89},
    {REG_2F2C   ,0x67},
    {REG_2F2D   ,0x45},
    {REG_2F2E   ,0x23},
    {REG_2F2F   ,0x01},
    {REG_2F72   ,0x07},
    {REG_2F73   ,0x09},
    {REG_2FE2   ,0x00}
};
code RegUnitType tNR_Mode_L_Tbl[]=
{
    //STD_L
    {REG_2F00   ,0x0C},
    {REG_2F42   ,0x03},
    {REG_2F4A   ,0x02},
    {REG_2F80   ,0x80},
    {REG_2F81   ,0x7B},
    {REG_2F82   ,0x48},
    {REG_2F83   ,0x06},
    {REG_2F84   ,0x00},
    {REG_2F85   ,0x00},
    {REG_2F86   ,0x00},
    {REG_2F87   ,0x00},
    {REG_2F00   ,0x02},
    {REG_2FA0   ,0x03},
    {REG_2FA8   ,0x01},
    {REG_2FAA   ,0xFF},
    {REG_2FB8   ,0x10},
    {REG_2FB9   ,0x32},
    {REG_2FBA   ,0x54},
    {REG_2FBB   ,0x76},
    {REG_2FBC   ,0x98},
    {REG_2FBD   ,0xBA},
    {REG_2FBE   ,0xDC},
    {REG_2FBF   ,0xFE},
    {REG_2F00   ,0x04},
    {REG_2F20   ,0x03},
    {REG_2F21   ,0x07},
    {REG_2F22   ,0x20},
    {REG_2F23   ,0x00},
    {REG_2F24   ,0x10},
    {REG_2F25   ,0x00},
    {REG_2F80   ,0x01},
    {REG_2F81   ,0x0F},
    {REG_2F82   ,0x00},
    {REG_2F84   ,0xFF},
    {REG_2FA0   ,0x2B},
    {REG_2FA1   ,0x03},
    {REG_2FA2   ,0x0A},
    {REG_2FA3   ,0x08},
    {REG_2FA4   ,0x00},
    {REG_2FA5   ,0x00},
    {REG_2FA6   ,0x00},
    {REG_2FA7   ,0x03},
    {REG_2FAA   ,0x10},
    {REG_2FAB   ,0x32},
    {REG_2FAC   ,0x54},
    {REG_2FAD   ,0x76},
    {REG_2FAE   ,0x98},
    {REG_2FAF   ,0xBA},
    {REG_2FB1   ,0xDC},
    {REG_2FB2   ,0xFE},
    {REG_2F00   ,0x02},
    {REG_2F60   ,0x13},
    {REG_2F61   ,0xFF},
    {REG_2F62   ,0xE3},
    {REG_2F68   ,0xEF},
    {REG_2F69   ,0xCD},
    {REG_2F6A   ,0xAB},
    {REG_2F6B   ,0x89},
    {REG_2F6C   ,0x67},
    {REG_2F6D   ,0x45},
    {REG_2F6E   ,0x23},
    {REG_2F6F   ,0x01},
    {REG_2FE0   ,0xE0},
    {REG_2F20   ,0x11},
    {REG_2F21   ,0x30},
    {REG_2F22   ,0xE3},
    {REG_2F23   ,0x00},
    {REG_2F28   ,0xEF},
    {REG_2F29   ,0xCD},
    {REG_2F2A   ,0xAB},
    {REG_2F2B   ,0x89},
    {REG_2F2C   ,0x67},
    {REG_2F2D   ,0x45},
    {REG_2F2E   ,0x23},
    {REG_2F2F   ,0x01},
    {REG_2F72   ,0x07},
    {REG_2F73   ,0x09},
    {REG_2FE2   ,0x00}
};

void mStar_SetupNRmode( BYTE NRMode )
{
    BYTE i=0;
    if(NRmodeContent == NRMode)
        return;

    switch(NRMode)
    {
        case NRMODE1:
        MenuFunc_printMsg("tNR_Mode_H_Tbl");
        for(i=0;i<sizeof(tNR_Mode_H_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tNR_Mode_H_Tbl[i].u16Reg,tNR_Mode_H_Tbl[i].u8Value);
            break;

        case NRMODE2 :
        MenuFunc_printMsg("tNR_Mode_M_Tbl");
        for(i=0;i<sizeof(tNR_Mode_M_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tNR_Mode_M_Tbl[i].u16Reg,tNR_Mode_M_Tbl[i].u8Value);
            break;

        case NRMODE3:
        MenuFunc_printMsg("tNR_Mode_L_Tbl");
        for(i=0;i<sizeof(tNR_Mode_L_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tNR_Mode_L_Tbl[i].u16Reg,tNR_Mode_L_Tbl[i].u8Value);
            break;

        default:
        MenuFunc_printData("tNR_Mode_OFF_Tbl",0);
        for(i=0;i<sizeof(tNR_Mode_OFF_Tbl)/sizeof(RegUnitType);i++)
            msWriteByte(tNR_Mode_OFF_Tbl[i].u16Reg,tNR_Mode_OFF_Tbl[i].u8Value);
            break;
    }
    NRmodeContent = NRMode;
}
#endif

#if ENABLE_DeBlocking
void msDeBlockingOnOff( BYTE ucSwitch, BYTE win)
{
    win=win;
    msWriteByteMask(SC2_20,  (ucSwitch)?(BIT0):(0), BIT0);  //BIT0:enable
}

void msInitDeBlocking( void )
{
    BYTE i=0;
    //SC2_60[3]=1: motion debug mode
    //SC2_E0[3]=0:SPF all bypass
    msWriteByteMask(SC2_E0, 0x00        ,BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(SC2_E0, 0x00        ,BIT7);

    msWriteByteMask(SC2_80, BIT0        ,BIT7|BIT6|BIT0);  //BIT7:debug mode ,BIT6: iir mode BIT0:blockiness_en_f2
    //msWriteByteMask(SC2_80,   BIT3|BIT2   ,BIT3|BIT2);  //De-blocking coarse detect step F2
    msWriteByteMask(SC2_81, 0x30    ,0xFF);  //De-blocking coarse active threshold F2

    MenuFunc_printMsg("msInitDeBlocking");
}
#endif
#if ENABLE_SHARPNESS
//=============================================================================
#define MaxSharpness    0x1F   //user sharpness adjust gain: 0x10~0x1F==>1.0~ 1.9
#define MinSharpness    0x00     //user sharpness adjust gain  0x00~0x0F==>0.0~ 0.9

Bool AdjustSharpness( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefSharpness, MinSharpness, MaxSharpness, 1 );
    if( tempValue == UserPrefSharpness )
    {
        return FALSE;
    }
    UserPrefSharpness = tempValue;

#if ENABLE_SUPER_RESOLUTION
    mStar_SetupSuperResolution( SRMODE_OFF);
#endif
    msAdjustSharpness( MAIN_WINDOW, UserPrefSharpness, 0 );

    MenuFunc_printData("UserPrefSharpness===%x",UserPrefSharpness);

    return TRUE;
}
WORD GetSharpnessValue( void )
{
    return GetScale100Value( UserPrefSharpness, MinSharpness, MaxSharpness );
}
#endif




#if ENABLE_OSD_ROTATION
Bool SetOSDRotateMode(void)
{
    if (UserPrefOsdRotateMode == MenuItemIndex)
        return FALSE;
    UserPrefOsdRotateMode = MenuItemIndex;

    if (UserPrefOsdRotateMode!=0)
        SET_MENU_ROTATION_FLAG();
    else
        CLR_MENU_ROTATION_FLAG();

    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_90 )
    {

        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, GET_CABASE_0_90 ( OSD_MAIN_WND_CA_BASE, OsdWindowWidth ) );
        Osd_SetPosition ( UserPrefHStart, UserPrefVStart );
    }
    #if defined (_OSD_ROTATION_180_)
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_180 )
    {
        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, GET_CABASE_0_180 ( OSD_MAIN_WND_CA_BASE, OsdWindowWidth, OsdWindowHeight ) );
        Osd_SetPosition ( UserPrefHStart, UserPrefVStart );
    }
    else
    #endif
    #if defined (_OSD_ROTATION_270_)
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_270 )
    {
        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, GET_CABASE_0_270 ( OSD_MAIN_WND_CA_BASE, OsdWindowWidth, OsdWindowHeight ) );
        Osd_SetPosition ( UserPrefHStart, UserPrefVStart );
    }
    else
    #endif
    {
        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, OSD_MAIN_WND_CA_BASE );
        Osd_SetPosition ( UserPrefOsdHStart, UserPrefOsdVStart);
    }
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif

Bool OSD_SetColorTemp(void)
{
    if (UserPrefColorTemp== MenuItemIndex)
        return FALSE;
    UserPrefColorTemp = MenuItemIndex;

	SetColorTemp();
	
    Set_SaveMonitorSettingFlag();
    return TRUE;
}

Bool AdjustVideoSaturation(MenuItemActionType enAction)
{
    WORD u16TempValue;

    u16TempValue = DecIncValue(enAction, UserPrefSaturation, 0, 0xFF, 1);


	
    if (u16TempValue == UserPrefSaturation)
        return FALSE;

    UserPrefSaturation = u16TempValue;


    msAdjustVideoSaturation(MAIN_WINDOW,UserPrefSaturation);
    return TRUE;
}
WORD GetSaturationValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefSaturation, 0x00, 0xFF );
    return temp;
}


Bool AdjustVideoHUE(MenuItemActionType enAction)
{
    WORD u16TempValue;

    u16TempValue = DecIncValue(enAction, UserPrefHue, 0, 100, 1);
	
    if (u16TempValue == UserPrefHue)
        return FALSE;

    UserPrefHue = u16TempValue;

    msAdjustVideoHue(MAIN_WINDOW,UserPrefHue);
    return TRUE;
}
WORD GetHueValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefHue, 0x00, 100 );
    return temp;
}

Bool SetGammaOnOff(void)
{
    if (UserPrefGamaOnOff == MenuItemIndex)
        return FALSE;
    UserPrefGamaOnOff = MenuItemIndex;
    drvGammaOnOff(UserPrefGamaOnOff, MAIN_WINDOW);
    drvGammaOnOff(UserPrefGamaOnOff, SUB_WINDOW);

    Set_SaveMonitorSettingFlag();
    return TRUE;
}

#endif
//vs add ccflag
///////////////////////////////////////////////////////////////////////////
//=======================================================
#if ENABLE_FREESYNC
BYTE GetSrcFreesyncFPS(void)
{
    BYTE xdata u8Vfreq = 0xFF;
    DWORD xdata u32OvsPeriod,u8DivN;
    WORD xdata u16TimeOutCnt;
    WORD xdata u16IvsPrd_L;
    BYTE xdata u8IvsPrd_H,u8DivN_L,u8DivN_H;

    u16TimeOutCnt = 600;
    while(u16TimeOutCnt!=0)
    {
        u16TimeOutCnt--;
        u16IvsPrd_L = msRead2Byte(REG_3842);
        u8IvsPrd_H = msReadByte(REG_3844);

        u8DivN_L = msReadByte(REG_3819);
        u8DivN_H = msReadByte(REG_381C);
        u8DivN=(DWORD)(((u8DivN_H&BIT0)<<4)|(u8DivN_L&0x0F));
        u32OvsPeriod = ((DWORD)u8IvsPrd_H<<16)|u16IvsPrd_L;
        if(u32OvsPeriod!=0)
        {
            u8Vfreq = (((DWORD)CRYSTAL_CLOCK )*(u8DivN+1)) / u32OvsPeriod;
            break;
        }
    }
    return u8Vfreq;
}
Bool SetDRRFunction(void)
{
    if (UserprefFreeSyncMode == MenuItemIndex)
        return FALSE;
    UserprefFreeSyncMode = MenuItemIndex;
    Set_SaveMonitorSettingFlag();

    msAPI_combo_IPEnableDDRFlag(UserprefFreeSyncMode);
    return TRUE;
}
#endif


WORD DecIncValue( MenuItemActionType action, WORD value, WORD minValue, WORD maxValue, BYTE step )
{
    if( action == MIA_IncValue || action == MIA_InvDecValue )
    {
        if( value >= maxValue )
        {
            return value;
        }
        minValue = value + step;
        if( minValue > maxValue )
        {
            minValue = maxValue;
        }
        value = minValue;
    }
    else
        // Decrease value
    {
        if( value <= minValue )
        {
            return value;
        }
        maxValue = value - step;
        if( maxValue < minValue )
        {
            maxValue = minValue;
        }
        value = maxValue;
    }
    return value;
}
WORD GetScale100Value( WORD value, WORD minValue, WORD maxValue )
{
    maxValue = ( maxValue <= minValue ) ? minValue : ( maxValue - minValue );
    //maxValue = maxValue - minValue;
    value = value - minValue;
    if( value > 650 )
    {
        value /= 2;
        maxValue /= 2;
    }
    value = (( WORD )value * 100 + maxValue / 2 ) / maxValue;
    value %= 101;
    return value;
}

void SetOSDTimer(BYTE osdtime)
{	OsdCounter=osdtime;
   	Clr_OsdTimeoutFlag();
}
Bool ResetOsdTimer( void )
{
    if( !FactoryModeFlag )
    {
        OsdCounter = UserPrefOsdTime;
    }
    if( MenuPageIndex == MainMenu && !FactoryModeFlag )
    {
        UserPrefLastMenuIndex = MenuItemIndex;
    }

#if EN_HotKeyTimes
	   if(MenuPageIndex == HotKeyShowTimes 
#if EN_HotKeyXINGZHUN
		||(MenuPageIndex==HotXINGZHUN1)
	||(MenuPageIndex==HotXINGZHUN2)
	||(MenuPageIndex==HotXINGZHUN3)
	||(MenuPageIndex==HotXINGZHUN4)
	||(MenuPageIndex==HotXINGZHUN5)
	||(MenuPageIndex==HotXINGZHUN6)
#endif
	   )
	   	{
		   OsdCounter = 0;
	   }

#endif   	
	
    //OsdCounter=0;
    return TRUE;
}
Bool EnablePowerDownCounter( void )
{
    PowerDownCounter = 3;

    return TRUE;
}

Bool PowerOffSystem( void )
{
    Clr_PowerOnFlag();

	if(!PowerOnFlag)
	{
		hw_SetAmberLed();
		hw_SetGreenLed();
	}
   else
   	{
		Power_TurnOffLed();
		printMsg("788888");
   }
		
#if AudioFunc
    mStar_AdjustVolume(0);
#endif

#if ENABLE_HDMI
    mstar_HDMIAudioMute( 0 );
    hw_SetMute();
#endif


    Clr_FactoryModeFlag();
    Clr_DisplayLogoFlag();
    Clr_DoModeSettingFlag();
    Clr_DoBurninModeFlag(); //2009-10-13  add
    //msWriteByte( BK0_32, 0 ); // disable background color function.
    drvOSD_FrameColorEnable(FALSE);
    Clr_PowerSavingFlag(); // 091014

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#else
#if (MS_PM)
    SaveMonitorSetting();   //110921 Rick modified - A032
#else
    Set_FlashForceSaveMonitorFlag();
#endif
#endif

#if 0//ENABLE_HDCP//1
    msPullLowHPD2();
#endif


#if ENABLE_DP_INPUT
      #if ((CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||( CHIP_ID == CHIP_TSUMD)||( CHIP_ID == CHIP_TSUM9)||( CHIP_ID == CHIP_TSUMF))
                MenuFunc_printMsg(" DP -----DC OFF ");
                DPOffSQ();
                #if DP_DCOFFHPDLOW
                DPRxHPDDisable();
                #endif
      #else
                //DPRxDCHPD( TRUE );
      #endif
#endif

    Power_PowerOffSystem();

    Clr_PowerSavingFlag();

#if (MS_PM)
    msPM_SetFlag_PMDCoff();
#else

    #if (ENABLE_POWEROFF_1X_MCU_CLK)
    mStar_MCU_Clock_Select(FALSE);
    #endif

    #if ENABLE_POWER_BOARD_CONTROL
    Set_PowerBoardSaving_Pin();
    #endif

#endif


    return TRUE;
}

Bool PowerOnSystem( void )
{
#if ENABLE_POWER_BOARD_CONTROL
    Clr_PowerBoardSaving_Pin();
#endif

    Set_PowerOnFlag();
    Clr_InputTimingChangeFlag();

#if !MS_PM
    #if (ENABLE_POWEROFF_1X_MCU_CLK)
    mStar_MCU_Clock_Select(TRUE);
    #endif
#endif

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#else
    #if (MS_PM)
    SaveMonitorSetting();
    #else
    Set_FlashForceSaveMonitorFlag();
    #endif
#endif

#if Enable_LED
	Power_TurnOnAmberLed();//红灯
	printMsg("8888");
#else
	Power_TurnOnGreenLed();
#endif
    
    Power_PowerOnSystem();


#if ENABLE_DP_INPUT
#if ((CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||( CHIP_ID == CHIP_TSUMD)||( CHIP_ID == CHIP_TSUM9)||( CHIP_ID == CHIP_TSUMF))
    #if  !DP_DCOFFHPDLOW
       if(DP_GetSquelch( 0x2FFFE ) && !(DPFastLink(TRUE)))
#endif
#endif
        {
            DPRxHPDDisable();
            ForceDelay1ms(100);
            DPRxHPDEnable();
            MenuFunc_printMsg(" DC On/Off HPD ");
        }

#endif

    Clr_InputTimingChangeFlag();
   
    mStar_SetupInputPort();

#if DisplayPowerOnLogo
    if(!(FactoryModeFlag))
    {
        Set_DisplayLogoFlag();
        msWriteByte( IOSDC2, 0x02 ); //Disable color bit setting
    }
#endif
    mStar_SetupFreeRunMode();
    Set_InputTimingChangeFlag();
#if ENABLE_LED_CONTROLLER
  #if LED_AUTO_RECOVER
    msLED_InitRecoveryVariables();
  #endif
#endif

    return TRUE;
}

Bool EnterRootMenu( void )
{
    OsdCounter = 0;
    return TRUE;
}

Bool AdjustContrast( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrast, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrast )
    {
        return FALSE;
    }
    UserPrefContrast = tempValue;
     UserPrefContrastTemp=UserPrefContrast;	
    mStar_AdjustContrast( UserPrefContrast );
#if 0	
    if( UserPrefColorTemp == CTEMP_USER )
    {
        UserPrefContrastUser = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_9300K )
    {
        UserPrefContrastCool1 = UserPrefContrast;
        if( FactoryModeFlag )
            FUserPrefContrastCool1 = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_5400K )
    {
        UserPrefContrastWarm1 = UserPrefContrast;
        if( FactoryModeFlag )
            FUserPrefContrastWarm1 = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_6500K )
    {
        UserPrefContrastNormal = UserPrefContrast;
        if( FactoryModeFlag )
            FUserPrefContrastNormal = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_SRGB )
    {
        UserPrefContrastSRGB = UserPrefContrast;
    }
#endif	
#if ENABLE_DPS
    if(!REG_DPS_RUN_Flag)
    {
        if(UserPrefECOMode>ECO_Standard)
        {
            msAdjustVideoContrast(SUB_WINDOW,GetSubWndRealContrast(tempValue) );
        }
        else
        {
            mStar_AdjustContrast(tempValue);
        }
    }
#endif

    return TRUE;
}

WORD GetContrastValue( void )
{
    //return GetScale100Value(UserPrefContrast, MinContrastValue, MaxContrastValue);
    WORD temp;
    temp = GetScale100Value( UserPrefContrast, MinContrastValue, MaxContrastValue );
    return temp;
}
//====================================================
Bool AdjustBrightness( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightness, 0, 100, 1 );
    if( tempValue == UserPrefBrightness )
    {
        return FALSE;
    }
    UserPrefBrightness = tempValue;
    UserPrefBrightnessTemp=UserPrefBrightness;
    mStar_AdjustBrightness( UserPrefBrightness );
#if 0	
    if( UserPrefColorTemp == CTEMP_USER )
        UserPrefBrightnessUser = UserPrefBrightness;
    else if( UserPrefColorTemp == CTEMP_9300K )
    {
        UserPrefBrightnessCool1 = UserPrefBrightness;
        if( FactoryModeFlag )
            FUserPrefBrightnessCool1 = UserPrefBrightness;
    }
    else if( UserPrefColorTemp == CTEMP_5400K )
    {
        UserPrefBrightnessWarm1 = UserPrefBrightness;
        if( FactoryModeFlag )
            FUserPrefBrightnessWarm1 = UserPrefBrightness;
    }
    else if( UserPrefColorTemp == CTEMP_6500K )
    {
        UserPrefBrightnessNormal = UserPrefBrightness;
        if( FactoryModeFlag )
            FUserPrefBrightnessNormal = UserPrefBrightness;
    }
    else if( UserPrefColorTemp == CTEMP_SRGB )
    {
        UserPrefBrightnessSRGB = UserPrefBrightness;
    }
#endif
#if ENABLE_DPS
if(!REG_DPS_RUN_Flag)
    mStar_AdjustBrightness(tempValue);
#endif
#if ENABLE_DPS
#if DPS_UserBrightControl_EN
    REG_DPS_UserBrightContlGain = UserPrefBrightness;//(float)USER_PREF_BRIGHTNESS * 256 / 100;
#else
    REG_DPS_UserBrightContlGain = 100;
#endif
#endif
    return TRUE;
}

WORD GetBrightnessValue( void )
{
    return UserPrefBrightness; //GetScale100Value(UserPrefBrightness, 0, 100);
}

Bool AdjustFocus( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefPhase, 0, MAX_PHASE_VALUE, 1 );
    if( tempValue == UserPrefPhase )
    {
        return FALSE;
    }
    UserPrefPhase = tempValue;
    //msADC_AdjustPhase( UserPrefPhase );
    drvADC_SetPhaseCode(UserPrefPhase);
    return TRUE;
}

WORD GetFocusValue( void )
{
    return (!CURRENT_INPUT_IS_VGA()) ? 50 : GetScale100Value( UserPrefPhase, 0, MAX_PHASE_VALUE );
}

//====================================================

Bool AdjustClock( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefHTotal, MinClock, MaxClock, 1 );
    if( tempValue == UserPrefHTotal )
    {
        return FALSE;
    }
    UserPrefHTotal = tempValue;
    //msADC_AdjustHTotal( UserPrefHTotal );
#if !ENABLE_MENULOAD        //111110 Rick re-Enable wait Blanking before VTOTAL adjustment while disable Menuload function - B40141
    mStar_WaitForDataBlanking();            //110930 Rick add for avoid noise while adjust clock value - C_FOS_5
#endif
    drvADC_AdjustHTotal(UserPrefHTotal);
    return TRUE;
}

WORD GetClockValue( void ) // adjust range
{
    return (!CURRENT_INPUT_IS_VGA()) ? 50 : GetScale100Value( UserPrefHTotal, MinClock, MaxClock );
}
//#define MaxHStart   UserPrefAutoHStart+50
//#define MinHStart   UserPrefAutoHStart-50
//====================================================
Bool AdjustHPosition( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefHStart, MinHStart, MaxHStart, 1 );
    if( tempValue == UserPrefHStart )
    {
        return FALSE;
    }
    UserPrefHStart = tempValue;
    mStar_AdjustHPosition( UserPrefHStart );
    return TRUE;
}

WORD GetHPositionValue( void )
{
    return (!CURRENT_INPUT_IS_VGA()) ? 50 : 100 - GetScale100Value( UserPrefHStart, MinHStart, MaxHStart ); //, 0);
}
//====================================================
//#define MaxVStart 2*UserPrefAutoVStart-1
//#define MinVStart 1  // 2006/10/26 4:18PM by Emily  0
//====================================================
Bool AdjustVPosition( MenuItemActionType action )
{
    WORD tempValue;
        tempValue = DecIncValue( action, UserPrefVStart, MinVStart, MaxVStart, 1 );
    if( tempValue == UserPrefVStart )
    {
        return FALSE;
    }
    UserPrefVStart = tempValue;
    mStar_AdjustVPosition( UserPrefVStart );
    return TRUE;
}

WORD GetVPositionValue( void )
{
        return (!CURRENT_INPUT_IS_VGA()) ? 50 : GetScale100Value( UserPrefVStart, MinVStart, MaxVStart );
}
#if 0//EN_HotKeyTimes
Bool AdjustTimes(void)
{
	  if (MenuPageIndex==HotKeyTimes)
		{
	
			if(MenuItemIndex==Times10TextItem)
				HotKeyTimes_Minutes=10;//UserPrefSelectInputType=Input_DVI;
			else if(MenuItemIndex==Times20TextItem)
				HotKeyTimes_Minutes=20;//UserPrefSelectInputType=Input_HDMI;
			else if (MenuItemIndex==Times30TextItem)
				HotKeyTimes_Minutes=30;//UserPrefSelectInputType=Input_Displayport;
		   else if(MenuItemIndex==Times40TextItem)
				HotKeyTimes_Minutes=40;//UserPrefSelectInputType=Input_Analog1;
			else if(MenuItemIndex==Times50TextItem)
				HotKeyTimes_Minutes=50;//UserPrefSelectInputType=Input_HDMI;
			else if (MenuItemIndex==Times60TextItem)
				HotKeyTimes_Minutes=60;//UserPrefSelectInputType=Input_Displayport;
			else if (MenuItemIndex==Times90TextItem)
				HotKeyTimes_Minutes=90;//UserPrefSelectInputType=Input_Displayport;

			
           printData("\n xxxxxHotKeyTimes_Minutes=%d \n",HotKeyTimes_Minutes);
		//	ExecuteKeyEvent( MIA_HotShowTimes);

			//  MenuPageIndex=HotKeyShowTimes;
				//MenuPageIndex=HotKeyShowTimes;
				//menuAction=MIA_RedrawMenu;
				//KeypadButton=BTN_Repeat;
				//MenuItemIndex=0;
			
	  	}
	  	return TRUE;
}

#endif

#if DualInput
#if OSD_INPUT_SUBMENU_ENABLE
Bool AdjustInputType(MenuItemActionType action)
{
	if(action==MIA_IncValue||action==MIA_DecValue)
		UserPrefSelectInputType=(UserPrefSelectInputType+1)%Input_Nums;	
	return TRUE;	
}
#endif
Bool SwitchInput(void)
{
#if KeyInputSelectMenu
  if (MenuPageIndex==HotKeyInputSelectMenu)
	{
		#if InputType_Digital
		if(MenuItemIndex==mDVITextItem)
			UserPrefSelectInputType=Input_DVI;
		else 
		#endif
		#if InputType_HDMI
			if(MenuItemIndex==mHDMITextItem)
			 UserPrefSelectInputType=Input_HDMI;
		#endif
		#if ENABLE_DP_INPUT
		else if (MenuItemIndex==mDPTextItem)
			UserPrefSelectInputType=Input_Displayport;
		#endif
	#if(InputType_Analog1)
	  else if(MenuItemIndex==mVGATextItem)
			UserPrefSelectInputType=Input_Analog1;
    #endif
	}
#endif
#if ENABLE_DEBUG
printData("SwitchInput SrcInputType==%d", SrcInputType);
printData("UserPrefInputPriorityType==%d", UserPrefSelectInputType);
#endif	

	if (SrcInputType== UserPrefSelectInputType)
		return FALSE;
	else
	SrcInputType=UserPrefSelectInputType;  
	//UserPrefInputType=SrcInputType = Input_Analog1;	
	
    #if 0 //AudioFunc
	#if Audio_Switch
		if((SrcInputType==Input_HDMI)||(SrcInputType==Input_DP))
			UserPrefHDMIAudioType=Audio_HDMI;	
		else
			UserPrefHDMIAudioType=Audio_LineIn;
		
			SetAudioType(UserPrefHDMIAudioType);
	#endif
    #endif

	//Set_SaveSettingFlag();
	Power_TurnOffPanel();
	mStar_SetupInputPort();
	Set_InputTimingChangeFlag();
	SrcFlags |= SyncLoss;
	
	return TRUE;
}
#endif

#if AudioFunc
Bool AdjustVolume( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefVolume, MinVolume, MaxVolume, 1 );
    if( tempValue == UserPrefVolume )
    {
        return FALSE;
    }
    UserPrefVolume = tempValue;
    mStar_AdjustVolume( UserPrefVolume );
    return TRUE;
}
WORD GetVolumeValue( void )
{
    return GetScale100Value( UserPrefVolume, MinVolume, MaxVolume );
}
#if (ENABLE_HDMI || ENABLE_DP_INPUT) && MS_DAC
Bool AdjustAudioSource( void )
{
    mStar_AdjustVolume(0);
    UserPrefAudioSource = ( UserPrefAudioSource + 1 ) % 2;

    if (UserPrefAudioSource == AUDIO_DIGITAL)
    {
    #if ENABLE_HDMI
        if( gScInfo.InputTmdsType == TMDS_HDMI && CURRENT_INPUT_IS_HDMI())//SrcInputType >= Input_Digital )
        {
            msAudioLineout_SourceSel( E_LINEOUT_FROM_DAC );
            msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x00 );   //111118 Modify
            #if !USE_DAC_ADJ
            msAudioLineOutGain( 0x3D );
            #endif
        }
    #endif
    #if ENABLE_DP_INPUT
        if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType==Input_Displayport)
        {
            msAudioLineout_SourceSel( E_LINEOUT_FROM_DAC );
            msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x00 );   //111118 Modify
            #if !USE_DAC_ADJ
            msAudioLineOutGain( 0x3D );
            #endif
        }

    #endif
    }
    else
    {
        msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );
        msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x00 );   //111118 Modify
        #if !USE_DAC_ADJ
        msAudioLineOutGain( 0x27 );
        #endif
    }
    mStar_AdjustVolume( UserPrefVolume );
    return TRUE;
}
#endif

Bool SwitchMute(void)
{
	if (UserOSDMuteOnFlag)
	{
		hw_ClrMute();
		UserOSDMuteOnFlag=0;
	}
	else
	{
		hw_SetMute();
		UserOSDMuteOnFlag=1;

	}
		SaveMonitorSetting(); // need changed again
		return TRUE;
}

#endif

#if Audio_Switch
Bool SetAudioType(AudioType Type)
{
	if (Type == Audio_LineIn)
	{
		msAudioLineout_SourceSel(E_LINEOUT_FROM_LINEIN0);
		msAudioLineOutGain(0x27);	// 091014 coding, max 1.4Vrms
	}
	else if (Type == Audio_HDMI)
	{
		msAudioLineout_SourceSel(E_LINEOUT_FROM_DAC);
		msAudioLineOutGain(0x3D);	// 091014 coding, max 1.4Vrms
	}

         //msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x0000 );  //20121004
         mStar_AdjustVolume(UserPrefVolume);
	return TRUE;
}
Bool AdjustAudioType(MenuItemActionType action)
{
	BYTE temp;
	if((SrcInputType==Input_Analog1)||(SrcInputType==Input_DVI))
		return FALSE;
	temp = UserPrefHDMIAudioType;
	if (action == MIA_DecValue)
	{
		UserPrefHDMIAudioType = (UserPrefHDMIAudioType + AUdioType_Nums - 1) % AUdioType_Nums;
	}
	else
	{
		UserPrefHDMIAudioType = (UserPrefHDMIAudioType + 1) % AUdioType_Nums;
	}
	if (temp == UserPrefHDMIAudioType)
		return FALSE;

	SetAudioType(UserPrefHDMIAudioType);
	
  	#if USEFLASH
	UserPref_EnableFlashSaveBit(bFlashSaveMonitorBit); 
  	#else
	NVRam_WriteByte(NVRAM_ADDR_MointorMember(OsdSelectItemFlag), OsdSelectItemFlags);	
	#endif
	return TRUE;
}
#endif

Bool AdjustOSDHPosition( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefOsdHStart, 0, 100, 5 ); //0729 New Spec
    if( tempValue == UserPrefOsdHStart )
    {
        return FALSE;
    }
    UserPrefOsdHStart = tempValue;
    mStar_WaitForDataBlanking();
    Osd_SetPosition( UserPrefOsdHStart, UserPrefOsdVStart );
    return TRUE;
}
WORD GetOSDHPositionValue( void )
{
    return GetScale100Value( UserPrefOsdHStart, 0, 100 );
}
//================================================================================
Bool AdjustOSDVPosition( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefOsdVStart, 0, 100, 5 ); //0729 New Spec
    if( tempValue == UserPrefOsdVStart )
    {
        return FALSE;
    }
    UserPrefOsdVStart = tempValue;
    mStar_WaitForDataBlanking();
    if (UserPrefOsdVStart == 100)
        Delay1ms(8);
    Osd_SetPosition( UserPrefOsdHStart, UserPrefOsdVStart );
    return TRUE;
}
WORD GetOSDVPositionValue( void )
{
    return GetScale100Value( UserPrefOsdVStart, 0, 100 );
}

Bool AdjustOSDTime( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefOsdTime, 5, 60, 5 );
    if( tempValue == UserPrefOsdTime )
    {
        return FALSE;
    }
    UserPrefOsdTime = tempValue;
    return TRUE;
}
WORD GetOSDTimeValue(void)
{
    return UserPrefOsdTime;
}
WORD GetOSDTime100Value( void )
{
    return GetScale100Value(UserPrefOsdTime, 0, 60);;
}

Bool AdjustOSDTransparency( MenuItemActionType action )
{
    WORD tempValue;

    tempValue=DecIncValue( action, UserPrefOsdTransparency, 0, 4, 1 );

    if (tempValue==UserPrefOsdTransparency)
        return FALSE;
    UserPrefOsdTransparency=tempValue;

    Osd_SetTransparency(UserPrefOsdTransparency);

    return TRUE;
}
WORD GetOSDTransparencyValue( void )
{
    return GetScale100Value(UserPrefOsdTransparency, 0, 4); //UserPrefOsdTransparency;
}

Bool AdjustRedColor( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColor, MinColorValue, MaxColorValue, (( UserPrefColorTemp == CTEMP_USER ) ? 2 : 1 ) );
    if( tempValue == UserPrefRedColor )
    {
        return FALSE;
    }
    UserPrefRedColor = tempValue;
    UserPrefRedColorUser = UserPrefRedColor;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustRedColor( UserPrefRedColorUser, UserPrefContrast );
#endif

    Set_SaveSettingFlag();
    return TRUE;
}
Bool AdjustGreenColor( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColor, MinColorValue, MaxColorValue, (( UserPrefColorTemp == CTEMP_USER ) ? 2 : 1 ) );
    if( tempValue == UserPrefGreenColor )
    {
        return FALSE;
    }
    UserPrefGreenColor = tempValue;
    UserPrefGreenColorUser = UserPrefGreenColor;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustGreenColor( UserPrefGreenColorUser, UserPrefContrast );
#endif

	Set_SaveSettingFlag();
    return TRUE;
}
Bool AdjustBlueColor( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColor, MinColorValue, MaxColorValue, (( UserPrefColorTemp == CTEMP_USER ) ? 2 : 1 ) );
    if( tempValue == UserPrefBlueColor )
    {
        return FALSE;
    }
    UserPrefBlueColor = tempValue;
    UserPrefBlueColorUser = UserPrefBlueColor;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustBlueColor( UserPrefBlueColorUser, UserPrefContrast );
#endif

	Set_SaveSettingFlag();
    return TRUE;
}

WORD GetRedColorValue100(void)
{
     if(UserPrefColorTemp==CTEMP_6500K)
     return GetScale100Value(UserPrefRedColorNormal, MinColorValue, MaxColorValue);
     else if(UserPrefColorTemp==CTEMP_9300K)
     return GetScale100Value(UserPrefRedColorCool1, MinColorValue, MaxColorValue);
	#if FactoryAuto_5400K	 
     else if(UserPrefColorTemp==CTEMP_5400K)
     return GetScale100Value(UserPrefRedColorWarm1, MinColorValue, MaxColorValue);
	 #endif
     else
    	 return GetScale100Value(UserPrefRedColorUser, MinColorValue, MaxColorValue);
}

WORD GetGreenColorValue100(void)
{
     if(UserPrefColorTemp==CTEMP_6500K)
     return GetScale100Value(UserPrefGreenColorNormal, MinColorValue, MaxColorValue);
     else if(UserPrefColorTemp==CTEMP_9300K)
     return GetScale100Value(UserPrefGreenColorCool1, MinColorValue, MaxColorValue);
#if FactoryAuto_5400K
     else if(UserPrefColorTemp==CTEMP_5400K)
     return GetScale100Value(UserPrefGreenColorWarm1, MinColorValue, MaxColorValue);
#endif
     else
     return GetScale100Value(UserPrefGreenColorUser, MinColorValue, MaxColorValue);

}

WORD GetBlueColorValue100(void)
{
     if(UserPrefColorTemp==CTEMP_6500K)
     return GetScale100Value(UserPrefBlueColorNormal, MinColorValue, MaxColorValue);
     else if(UserPrefColorTemp==CTEMP_9300K)
     return GetScale100Value(UserPrefBlueColorCool1, MinColorValue, MaxColorValue);
    #if FactoryAuto_5400K
     else if(UserPrefColorTemp==CTEMP_5400K)
     return GetScale100Value(UserPrefBlueColorWarm1, MinColorValue, MaxColorValue);
    #endif
     else
  	   return GetScale100Value(UserPrefBlueColorUser, MinColorValue, MaxColorValue);

}

Bool SetColorTemperature9300K(void)
{
	if (!UserPrefDcrMode)
      mStar_AdjustBrightness(UserPrefBrightness);
      mStar_AdjustRedColor(UserPrefRedColorCool1, UserPrefContrast);
      mStar_AdjustGreenColor(UserPrefGreenColorCool1, UserPrefContrast);
      mStar_AdjustBlueColor(UserPrefBlueColorCool1, UserPrefContrast);

	return TRUE;
}

Bool SetColorTemperature6500K(void)
{
	if (!UserPrefDcrMode)
       mStar_AdjustBrightness(UserPrefBrightness);
       mStar_AdjustRedColor(UserPrefRedColorNormal, UserPrefContrast);
       mStar_AdjustGreenColor(UserPrefGreenColorNormal, UserPrefContrast);
       mStar_AdjustBlueColor(UserPrefBlueColorNormal, UserPrefContrast);
	return TRUE;
}
#if FactoryAuto_5400K	 
Bool SetColorTemperature5400K(void)
{
	if (!UserPrefDcrMode)
       mStar_AdjustBrightness(UserPrefBrightness);
       mStar_AdjustRedColor(UserPrefRedColorWarm1, UserPrefContrast);
       mStar_AdjustGreenColor(UserPrefGreenColorWarm1, UserPrefContrast);
       mStar_AdjustBlueColor(UserPrefBlueColorWarm1, UserPrefContrast);
	return TRUE;
}
#endif

Bool SetColorTemperatureUser(void)
{
	if (!UserPrefDcrMode)
	     mStar_AdjustBrightness(UserPrefBrightness);
#if LowBlueLight
	if(UserPrefLowBlueMode==BlueLight_Browsing)
	{
		UserPrefBlueColorUser=97;		/// 114*85
	}
	else if (UserPrefLowBlueMode==BlueLight_Read)
	{
		UserPrefBlueColorUser=80;   /// /// 114*70
	}
	else if(UserPrefLowBlueMode==BlueLight_Video)
	{
		UserPrefBlueColorUser=57;    /// 114*50
	}
	else if (UserPrefLowBlueMode==BlueLight_lowlight)
	{
		UserPrefBlueColorUser=34;   /// 114*30
	}
	else  //BlueLight_Off
	{
		UserPrefBlueColorUser=DefColorUser;  
	}
#endif		
      mStar_AdjustRedColor(UserPrefRedColorUser, UserPrefContrast);
      mStar_AdjustGreenColor(UserPrefGreenColorUser, UserPrefContrast);
      mStar_AdjustBlueColor(UserPrefBlueColorUser, UserPrefContrast);
	return TRUE;
}

Bool SetColorTemp( void )
{
    ReadColorTempSetting();
   // if( UserPrefColorTemp == CTEMP_SRGB )
   // {
     //   UserPrefECOMode = ECO_Standard;
    //}
    return TRUE;
}
Bool AdjustColorTempMode( MenuItemActionType action )
{
    BYTE temp;
    temp = UserPrefColorTemp;
    if( action == MIA_DecValue )
    {
        UserPrefColorTemp = ( UserPrefColorTemp + CTEMP_Nums - 1 ) % CTEMP_Nums;
    }
    else
    {
        UserPrefColorTemp = ( UserPrefColorTemp + 1 ) % CTEMP_Nums;
    }

    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
        #if !ENABLE_HDMI
        #if MWEFunction
        if( UserPrefColorFlags )
            msAccOnOff( 1 );
        else
        #endif
            msAccOnOff( 0 );
        #endif
        msDlcOnOff( 0 );
    }

    if( temp == UserPrefColorTemp )
        return FALSE;

	 if ( UserPrefColorTemp == CTEMP_6500K )
		{
			SetColorTemperature6500K();
		}
	else if ( UserPrefColorTemp == CTEMP_9300K )
		{
			SetColorTemperature9300K();
		}
	#if FactoryAuto_5400K
	else if ( UserPrefColorTemp == CTEMP_5400K )
		{
			SetColorTemperature5400K();
		}
	#endif	
	else if ( UserPrefColorTemp == CTEMP_USER )
		{
			SetColorTemperatureUser();
		}

    #if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorTemp ), UserPrefColorTemp );
    #else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
    #endif
    //msAccOnOff(0); // 2006/11/9 11:27PM by Emily test


    ReadColorTempSetting();
    //if( UserPrefECOMode != ECO_Standard )        //jeff add in 1112
        //UserPrefECOMode = ECO_Standard;
    return TRUE;
}

Bool AdjustLanguage(void)
{
   
  if (UserPrefLanguage == MenuItemIndex)
        return FALSE;
    UserPrefLanguage = MenuItemIndex;

    Set_SaveSettingFlag();
    return TRUE;
}


Bool ResetAllSetting( void )   //澶嶄綅
{
	BYTE BackupLanguage;
    WORD hFreq, vFreq;
    hFreq = HFreq( SrcHPeriod );
    vFreq = VFreq( hFreq, SrcVTotal );
   
   Osd_Hide();
   
   UserPrefLanguage = LANG_Default; //xhq
   
   BackupLanguage = UserPrefLanguage;

   
#if 1
	   if(FactoryModeFlag )  //宸ュ巶鑿滃崟妯″紡
	   {
   
	   Init_FactorySetting();
   
	   
		  // UserPrefRedColorUser = DefColorUser;
		  // UserPrefGreenColorUser = DefColorUser;
		  // UserPrefBlueColorUser = DefColorUser;
	   }
#else
	   if( FactoryModeFlag )
	   {
#if 1 // 090721
		   UserPrefRedColorUser = DefColorUser;
		   UserPrefGreenColorUser = DefColorUser;
		   UserPrefBlueColorUser = DefColorUser;
#else
	   UserPrefRedColorUser = DefColor;
	   UserPrefGreenColorUser = DefColor;
	   UserPrefBlueColorUser = DefColor;
#endif
	   }
#endif
   Init_MonitorSetting();
   UserPrefLanguage = BackupLanguage;
#if AudioFunc
    if(UserOSDMuteOnFlag)
	{
		UserOSDMuteOnFlag=0;
	}
    mStar_AdjustVolume( UserPrefVolume );    
#if Audio_Switch
	
		if((SrcInputType==Input_HDMI)||(SrcInputType==Input_Displayport))
		{			
			SetAudioType(UserPrefHDMIAudioType);
		}		
#endif
#endif
	Osd_SetTransparency(UserPrefOsdTransparency);
#if ENABLE_RTE
	msOverDriveOnOff(UserprefOverDriveSwitch);
#endif
	SetECO();

#if Enable_Gamma
    UserPrefGamaMode = GAMA1;
    mStar_SetupGamma( UserPrefGamaMode );
		UserPrefGamaOnOff=FALSE;
		drvGammaOnOff(UserPrefGamaOnOff, MAIN_WINDOW );
		drvGammaOnOff(UserPrefGamaOnOff, SUB_WINDOW );	
#endif

//ccadd end


#if ENABLE_DPS
    UserprefDPSMode = 0;
    msDPS_Off_Setting( );
#endif
#if ENABLE_DSC
    UserprefDSCMode = 0;
    msDSC_Off_Setting( );
#endif
#if ENABLE_DLC
    UserprefDLCMode = 0;
    g_bDLCOnOff = 0;
#endif



#if ENABLE_FREESYNC
    UserprefFreeSyncMode = FreeSyncMenuItems_On;
    msAPI_combo_IPEnableDDRFlag(UserprefFreeSyncMode);
#endif

#if ENABLE_R2_2DTO3D
    UserPref2Dto3D = Def2Dto3D;
    UserPref2Dto3DGameMode = Def2Dto3DGameMode;
    UserPref2Dto3DGain = Def2Dto3DGain;
    UserPref2Dto3DOffset = Def2Dto3DOffset;
#endif

	UserPrefInputPriorityType=DEF_INPUT_PRIORITY;


#if ENABLE_SUPER_RESOLUTION
    UserPrefSuperResolutionMode = SRMODE_MIDDLE;
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
#endif
#if ENABLE_DeBlocking
    UserPrefDeBlocking= 0;
    //msDPS_Off_Setting( );
#endif


#if ENABLE_TNR
    UserPrefNRmode= NRMODE2;
    mStar_SetupNRmode( UserPrefNRmode );
#endif
   
    Set_DDCciFlag();
    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
#if !ENABLE_HDMI
        msAccOnOff( 0 );
#endif
        msDlcOnOff( 0 );
        SetECO();
    }

	UserPrefHue = DefHue;
    UserPrefSaturation = DefSaturation;
    msAdjustVideoHue( SUB_WINDOW ,UserPrefHue );
    msAdjustVideoSaturation(SUB_WINDOW, UserPrefSaturation );
	
#if MWEFunction
    UserPrefColorFlags = 0;
    UserPrefSubContrast = DefSubContrast;
    UserPrefSubBrightness = DefSubBrightness;
    UserPrefBFSize = 1;
    msAdjustSubBrightness( UserPrefSubBrightness );
    msAdjustVideoContrast( DefContrastBase + UserPrefContrast );
    SetBFSize( UserPrefBFSize );
#endif
    UserprefHistogram1 = 0x40;
    UserprefHistogram2 = 0xB0;
    UserprefALha = 50;
    UserprefBata = 50;


//cc add start
    if(CURRENT_INPUT_IS_VGA())//(( SrcInputType < Input_Digital ) )
    {
        if(( !FactoryModeFlag ) && !ProductModeFlag )   //for nw aoc factorymode
        {
            //mStar_AutoGeomtry();
            //UserPrefAutoTimes=0;
            if( mStar_AutoGeomtry() == TRUE )
                //0707
            {
                UserPrefAutoTimes = 1;
                UserPrefAutoHStart = UserPrefHStart;
                UserPrefAutoVStart = UserPrefVStart;
            }
        }
        // UserPrefAutoHStart = UserPrefHStart;
        {
            BYTE modeIndex, flag;
            flag = SrcFlags; //UserModeFlag
            modeIndex = SaveIndex; //SrcModeIndex;
            SrcFlags &= ~bUserMode;
#if !USEFLASH
            for( SaveIndex = 0; SaveIndex < NumberOfMode; SaveIndex++ )
            {
                NVRam_WriteByte( nvrModeAddr( CheckSum ), 0 );
            }
            for( SaveIndex = 0; SaveIndex < NumberOfMode; SaveIndex++ )
            {
                modeInfo.HerFreq = 0;
                modeInfo.VerFreq = 0;
                modeInfo.VTotal = 0;
                modeInfo.Flags = 0;
                NVRam_WriteTbl( nvrModeInfoAddr( SaveIndex ), ( BYTE* ) &modeInfo, ModeInfoSize );
            }
            SaveIndex = 0;
            SrcFlags = flag;
            modeInfo.HerFreq = hFreq;
            modeInfo.VerFreq = vFreq;
            modeInfo.VTotal = SrcVTotal;
            modeInfo.Flags = SrcFlags & 0x13;
            NVRam_WriteTbl( nvrModeInfoAddr( SaveIndex ), ( BYTE* ) &modeInfo, ModeInfoSize );
            NVRam_WriteByte( nvrModeAddr( CheckSum ), 0 );
            NVRam_WriteByte( nvrMonitorAddr( SavedModeIndex ), UserPrefSavedModeIndex );
            SaveModeSetting();
#else
            SaveIndex = SrcModeIndex;//SaveIndex = 0;
            SrcFlags = flag;
            Flash_ClearModeSet();
            SaveModeSetting();
#endif
        }
    }
    else
    {
        Delay1ms( 500 );
    }
    Osd_SetPosition(UserPrefOsdHStart, UserPrefOsdVStart);
#if Enable_Expansion            //111221 modified Expansion mode condition - B42048, B42029
    UserprefExpansionMode = DefExpansion;
    OverScanSetting.AspRatio = UserprefExpansionMode; //20150728	
    SetExpansionMode();//This Function will mute when reset all setting.
#endif

    if( FactoryModeFlag )
    {
#if USEFLASH
        UserprefBacklighttime = BlacklitTime = 0;
        SaveMonitorSetting2();
#else
        BlacklitTime = 0;
        SaveBlacklitTime();
#endif
    }
    Clr_BurninModeFlag();
    Clr_DoBurninModeFlag(); // 091014 coding
    SaveMonitorSetting(); //1020
    if( ProductModeFlag )
    {
#if USEFLASH
        FactoryProductModeValue = ProductModeOffValue;
        SaveFactorySetting();
#else
        WORD temp = ProductModeOffValue;
        NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &temp ), 2 );
#endif
    }

    if( ProductModeFlag )
    {
        //Power_TurnOnAmberLed();canceled PE Request
        Clr_ProductModeFlag();//20091022
    }
#if 0//DECREASE_V_SCALING       //111103 Rick modified for avoid auto fail after factory reset - A070
    DecVScaleValue = 0;
#endif
    return TRUE;
}

Bool AutoConfig( void )
{
    Bool result;
    result = mStar_AutoGeomtry();
    //UserPrefAutoHStart=UserPrefHStart;
    if( result )
    {
        UserPrefAutoHStart = UserPrefHStart;
        UserPrefAutoVStart = UserPrefVStart;
        UserPrefAutoTimes = 1;
        SaveModeSetting();
    }
    else
    {
        UserPrefHTotal = StandardModeHTotal;
        UserPrefHStart = StandardModeHStart;
        UserPrefVStart = StandardModeVStart;
        UserPrefAutoHStart = UserPrefHStart;
        UserPrefAutoVStart = UserPrefVStart;
        UserPrefPhase = 18;
        UserPrefAutoTimes = 0;
        drvADC_AdjustHTotal( UserPrefHTotal );
        drvADC_SetPhaseCode( UserPrefPhase );
        mStar_AdjustHPosition(UserPrefHStart);//msWrite2Byte( SC0_07, UserPrefHStart );
        mStar_AdjustVPosition(UserPrefAutoVStart);
    }

    return result;
}

Bool ResetGeometry( void )
{
    UserPrefHStart = StandardModeHStart;
    mStar_AdjustHPosition( UserPrefHStart );
    //NVRam_WriteByte(nvrModeAddr(HStart), UserPrefHStart);
    UserPrefVStart = StandardModeVStart;
    mStar_AdjustVPosition( UserPrefVStart );
    //NVRam_WriteByte(nvrModeAddr(VStart), UserPrefVStart);
    UserPrefHTotal = StandardModeHTotal;
    drvADC_AdjustHTotal( UserPrefHTotal );
    //NVRam_WriteByte(nvrModeAddr(HTotal), UserPrefHTotal);
    UserPrefPhase = 0;
    drvADC_SetPhaseCode( UserPrefPhase );
    //NVRam_WriteByte(nvrModeAddr(Phase), UserPrefPhase);
    AutoConfig();
    return TRUE;
}

void SaveUserPref(void)
{
    #if USEFLASH
    UserPref_EnableFlashSaveBit(bFlashSaveMonitorBit); 
    if (SrcInputType == Input_Analog1)
    {
        UserPref_EnableFlashSaveBit(bFlashSaveModeBit);
    }
    #else
    SaveMonitorSetting();
    if (SrcInputType == Input_Analog1)
    {
        SaveModeSetting();
    }
    #endif
}

void SaveUserPrefSetting(BYTE menuPageIndex)
{
    menuPageIndex = 0;
    SaveUserPref();
    Clr_SaveSettingFlag();
}

Bool AutoColor( void )
{
    Bool result;
    if(!CURRENT_INPUT_IS_VGA())//( SrcInputType == Input_Digital || SrcInputType == Input_Digital2 )
        return TRUE;
    result = mStar_AutoColor();
    Osd_SetTextColor( CP_RedColor, CP_WhiteColor );
    if( result )
    {
        //Osd_DrawStr(10, 2, PassText());
        SaveFactorySetting();
    }
    return result;
}

Bool AdjustRedGain( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcRedGain, 0, 0xFFF, 1 );
    if( tempValue == UserPrefAdcRedGain )
    {
        return FALSE;
    }
    UserPrefAdcRedGain = tempValue;
    //msADC_AdjustAdcRedGain( UserPrefAdcRedGain );
    drvADC_SetRedGainCode(UserPrefAdcRedGain);
    return TRUE;
}
WORD GetRedGainValue( void )
{
    return UserPrefAdcRedGain;
}
Bool AdjustGreenGain( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcGreenGain, 0, 0xFFF, 1 );
    if( tempValue == UserPrefAdcGreenGain )
    {
        return FALSE;
    }
    UserPrefAdcGreenGain = tempValue;
    //msADC_AdjustAdcGreenGain( UserPrefAdcGreenGain );
    drvADC_SetGreenGainCode(UserPrefAdcGreenGain);
    return TRUE;
}
WORD GetGreenGainValue( void )
{
    return UserPrefAdcGreenGain;
}
Bool AdjustBlueGain( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcBlueGain, 0, 0xFFF, 1 );
    if( tempValue == UserPrefAdcBlueGain )
    {
        return FALSE;
    }
    UserPrefAdcBlueGain = tempValue;
    //msADC_AdjustAdcBlueGain( UserPrefAdcBlueGain );
    drvADC_SetBlueGainCode(UserPrefAdcBlueGain);
    return TRUE;
}
WORD GetBlueGainValue( void )
{
    return UserPrefAdcBlueGain;
}
//====================================================
//====================================================
Bool AdjustAdcRedOffset( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcRedOffset, 0, 0x7FF, 1 );
    if( tempValue == UserPrefAdcRedOffset )
    {
        return FALSE;
    }
    UserPrefAdcRedOffset = tempValue;
    //msADC_AdjustAdcRedOffset( UserPrefAdcRedOffset );
    drvADC_SetRedOffsetCode(UserPrefAdcRedOffset);
    return TRUE;
}
WORD GetAdcRedOffsetValue( void )
{
    return UserPrefAdcRedOffset;
}
Bool AdjustAdcGreenOffset( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcGreenOffset, 0, 0x7FF, 1 );
    if( tempValue == UserPrefAdcGreenOffset )
    {
        return FALSE;
    }
    UserPrefAdcGreenOffset = tempValue;
    //msADC_AdjustAdcGreenOffset( UserPrefAdcGreenOffset );
    drvADC_SetGreenOffsetCode(UserPrefAdcGreenOffset);
    return TRUE;
}
WORD GetAdcGreenOffsetValue( void )
{
    return UserPrefAdcGreenOffset;
}
Bool AdjustAdcBlueOffset( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcBlueOffset, 0, 0x7FF, 1 );
    if( tempValue == UserPrefAdcBlueOffset )
    {
        return FALSE;
    }
    UserPrefAdcBlueOffset = tempValue;
    //msADC_AdjustAdcBlueOffset( UserPrefAdcBlueOffset );
    drvADC_SetBlueOffsetCode(UserPrefAdcBlueOffset);
    return TRUE;
}
WORD GetAdcBlueOffsetValue( void )
{
    return UserPrefAdcBlueOffset;
}


void GetColorTempRGB(ColorType *pstColor)
{
    pstColor->u8Red     =*(ColorTempPtr[UserPrefColorTemp][0]);
    pstColor->u8Green   =*(ColorTempPtr[UserPrefColorTemp][1]);
    pstColor->u8Blue    =*(ColorTempPtr[UserPrefColorTemp][2]);
}

void ReadColorTempSetting( void )
{
    UserPrefRedColor =  *ColorTempPtr[UserPrefColorTemp][0];
    UserPrefGreenColor =  *ColorTempPtr[UserPrefColorTemp][1];
    UserPrefBlueColor =  *ColorTempPtr[UserPrefColorTemp][2];
    CheckColorValueRange();
    UserPrefBrightness %= 101;
    // 2006/11/10 11:13PM by KK   if(!UserPrefDcrMode) // for under DCR mode, the picture show filter
    mStar_AdjustBrightness( UserPrefBrightness );
    mStar_AdjustContrast( UserPrefContrast );
    //msAdjustVideoContrast(DefContrastBase + UserPrefContrast);
}


void SetECO( void )
{
    SetColorTemp();
   if (UserPrefECOMode == ECO_Text)
	{
		UserPrefBrightness = 60;   
		UserPrefContrast = DefContrast;   
	}
	else if (UserPrefECOMode == ECO_Game)
	{
		UserPrefBrightness = 85;   
		UserPrefContrast = 60;
	}
	else if (UserPrefECOMode == ECO_Movie)
	{
		UserPrefBrightness = 100;   
		UserPrefContrast = 80;   
	}
	else if (UserPrefECOMode == ECO_Standard)
	{
		UserPrefBrightness=UserPrefBrightnessTemp;  
		UserPrefContrast =UserPrefContrastTemp;     //  保存亮度调节后的值
	}
    mStar_AdjustBrightness( UserPrefBrightness );
    mStar_AdjustContrast( UserPrefContrast );
}
Bool AdjustECOMode( MenuItemActionType action )
{
    UserPrefDcrMode = 0;
#if MWEFunction
    Clr_GreenFlag();
    Clr_SkinFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_FullEnhanceFlag();
    Clr_DemoFlag();
    Clr_PictureBoostFlag();
    SetBFSize( UserPrefBFSize );
#endif
    //Benz 2007.4.23   15:35:12
    //Benz 2007.4.23   16:40:20     Osd_Draw4Num(1,1,UserPrefECOMode);
    if( action == MIA_DecValue )
    {
        UserPrefECOMode = ( UserPrefECOMode + ECO_Nums - 1 ) % ECO_Nums; //Benz have some bug issue
    }
    else
    {
        UserPrefECOMode = ( UserPrefECOMode + 1 ) % ECO_Nums;
    }
    //Benz 2007.4.23   16:40:10     Osd_Draw4Num(2,1,UserPrefECOMode);
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ECOMode ), UserPrefECOMode );
#if MWEFunction
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#endif
#else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#endif
    SetECO();
    return TRUE;
}


Bool AdjustDcrMode( MenuItemActionType action )
{
#if ENABLE_HDMI
    BYTE XDATA ucDomain;
#endif

    action = action;

#if ENABLE_HDMI
    ucDomain = 0x00;
    if( gScInfo.InputColor != INPUT_RGB )
        ucDomain = 0xFF;
#endif

#if 1

    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
        SetECO();
    }
    else
    {
        UserPrefDcrMode = 1;
        UserPrefColorTemp = CTEMP_6500K;

        UserPrefECOMode = ECO_Standard;
        ReadColorTempSetting();

    #if MWEFunction
    #if 0//!ENABLE_HDMI // wait for coding
        //msAdjustSubBrightness( UserPrefSubBrightness );
        msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
        msAdjustVideoContrast( DefContrastBase + UserPrefContrast );
    #endif
        Clr_PictureBoostFlag();
        Clr_ColorAutoDetectFlag();
        Clr_FullEnhanceFlag();
        Clr_SkinFlag();
        Clr_BlueFlag();
        Clr_GreenFlag();
        Clr_DemoFlag();
    #endif

    }
    msDCROnOff(UserPrefDcrMode, MAIN_WINDOW);

#else

    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
        //msWriteByte( BK0_00, REGBANK0 );
        mStar_AdjustBrightness( UserPrefBrightness );
#if !ENABLE_HDMI
#if MWEFunction
        if( UserPrefColorFlags )
            msAccOnOff( 1 );
        else
#endif
            msAccOnOff( 0 );
#endif
        msDlcOnOff( 0 );
        SetECO();
    }
    else
    {

        UserPrefDcrMode = 1;

        //if ( UserPrefColorTemp == CTEMP_SRGB ) // 100817
        UserPrefColorTemp = CTEMP_Warm1;

        UserPrefECOMode = ECO_Standard;
        ReadColorTempSetting();
        #if MWEFunction
        #if !ENABLE_HDMI
        //msAdjustSubBrightness( UserPrefSubBrightness );
        msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
        msAdjustVideoContrast( DefContrastBase + UserPrefContrast );
        #endif
        Clr_PictureBoostFlag(); // 2006/10/20 6:21PM by Emily for aoc request
        Clr_ColorAutoDetectFlag();
        Clr_FullEnhanceFlag();
        Clr_SkinFlag();
        Clr_BlueFlag();
        Clr_GreenFlag();
        Clr_DemoFlag();
#endif

        //msWriteByte( BK0_00, REGBANK3 );
        //msWriteByte( BK3_72, 0x70 );
        //msWriteByte( BK3_73, 0x70 );
        //msWriteByte( BK3_74, 0x70 );
        //msWriteByte( BK0_00, REGBANK0 );

        LoadACETable(MAIN_WINDOW, (BYTE *)t_Normal_ColorSettingTable, DefHue, DefSaturation, 0x80 ); // /UserPrefSubContrast
        msAccSetup( 0, PanelWidth, 0, PanelHeight );
#if !ENABLE_HDMI
        msAccOnOff( 1 );
#endif
        msDlcInit( PanelWidth, PanelHeight );
        msSetDlcStrength( g_LowStrength, g_HighStrength );
        LoadDLCTable( t_MWEDLC_Linear_Table );
#if !ENABLE_HDMI
        msDlcOnOff( 1 );
#endif
    }
#if ENABLE_HDMI
#if MWEFunction
    if( UserPrefDcrMode || UserPrefColorFlags )
#else
    if( UserPrefDcrMode )
#endif
    {
#if ENABLE_HDMI
        if( ucDomain )
        {
            SetMainWinVideoDomain( ucDomain );      //SetMainWinColorControl(ucDomain);
            mStar_WriteByteMask( BK0_5C, BIT3, BIT3 ); //msWriteByte( BK0_5C, msReadByte( BK0_5C ) | BIT3 );
        }
        else
#endif
            msAccOnOff( _ENABLE );

        if( UserPrefDcrMode )
            msDlcOnOff( _ENABLE );
    }
    else
    {
#if ENABLE_HDMI
        if( ucDomain )
            mStar_WriteByteMask( BK0_5C, 0x00, BIT3 ); //msWriteByte( BK0_5C, msReadByte( BK0_5C )&~BIT3 );
        else
#endif
            msAccOnOff( 0 );

        msDlcOnOff( _DISABLE );

#if ENABLE_HDMI
        SetMainWinVideoDomain( 0 ); //SetMainWinColorControl(0);
        if( ucDomain )
            mStar_WriteByteMask( BK0_02, BIT3, BIT3 ); //msWriteByte( BK0_02, msReadByte( BK0_02 ) | BIT3 );
#endif
    }
#endif

    msDCROnOff(UserPrefDcrMode, 0);
#endif


#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( DcrMode ), UserPrefDcrMode );
    NVRam_WriteByte( nvrMonitorAddr( ECOMode ), UserPrefECOMode );
#else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#endif
    return TRUE;
}


Bool SetFactoryColorTempCool1( void )
{
    UserPrefColorTemp = CTEMP_9300K;
    ReadColorTempSetting(); // 2006/11/10 6:58PM by KK ReadCool1Setting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}

Bool SetFactoryColorTempNormal( void )
{
    UserPrefColorTemp = CTEMP_6500K;
    ReadColorTempSetting(); // 2006/11/10 6:58PM by KK ReadNormalSetting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
Bool SetFactoryColorTempWarm1( void )
{
    UserPrefColorTemp = CTEMP_5400K;
    ReadColorTempSetting(); // 2006/11/10 6:57PM by KK  ReadWarm1Setting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}

Bool AdjustRedColorWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorWarm1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorWarm1 )
    {
        return FALSE;
    }
    UserPrefRedColorWarm1 = tempValue;
	UserPrefColorTemp=CTEMP_5400K;
	SetColorTemperature5400K();
    return TRUE;
}
Bool AdjustGreenColorWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorWarm1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorWarm1 )
    {
        return FALSE;
    }
    UserPrefGreenColorWarm1 = tempValue;
	UserPrefColorTemp=CTEMP_5400K;
	SetColorTemperature5400K();
    return TRUE;
}
Bool AdjustBlueColorWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorWarm1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorWarm1 )
    {
        return FALSE;
    }
    UserPrefBlueColorWarm1 = tempValue;
	UserPrefColorTemp=CTEMP_5400K;
	SetColorTemperature5400K();
    return TRUE;
}

WORD GetRColorWarm1Value( void )
{
    return UserPrefRedColorWarm1;
}
WORD GetGColorWarm1Value( void )
{
    return UserPrefGreenColorWarm1;
}
WORD GetBColorWarm1Value( void )
{
    return UserPrefBlueColorWarm1;
}

Bool AdjustRedColorNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorNormal, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorNormal )
    {
        return FALSE;
    }
    UserPrefRedColorNormal = tempValue;
	UserPrefColorTemp=CTEMP_6500K;
	SetColorTemperature6500K();
    return TRUE;
}
Bool AdjustGreenColorNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorNormal, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorNormal )
    {
        return FALSE;
    }
    UserPrefGreenColorNormal = tempValue;
	UserPrefColorTemp=CTEMP_6500K;
		SetColorTemperature6500K();
    return TRUE;
}
Bool AdjustBlueColorNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorNormal, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorNormal )
    {
        return FALSE;
    }
    UserPrefBlueColorNormal = tempValue;
	UserPrefColorTemp=CTEMP_6500K;
		SetColorTemperature6500K();
    return TRUE;
}
WORD GetRColorNormalValue( void )
{
    return UserPrefRedColorNormal;
}
WORD GetGColorNormalValue( void )
{
    return UserPrefGreenColorNormal;
}
WORD GetBColorNormalValue( void )
{
    return UserPrefBlueColorNormal;
}

Bool AdjustRedColorCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorCool1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorCool1 )
    {
        return FALSE;
    }
    UserPrefRedColorCool1 = tempValue;
    UserPrefColorTemp=CTEMP_9300K;
    SetColorTemperature9300K();
    return TRUE;
}
Bool AdjustGreenColorCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorCool1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorCool1 )
    {
        return FALSE;
    }
    UserPrefGreenColorCool1 = tempValue;
	UserPrefColorTemp=CTEMP_9300K;
	   SetColorTemperature9300K();
    return TRUE;
}
Bool AdjustBlueColorCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorCool1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorCool1 )
    {
        return FALSE;
    }
    UserPrefBlueColorCool1 = tempValue;
	UserPrefColorTemp=CTEMP_9300K;
	   SetColorTemperature9300K();
    return TRUE;
}
WORD GetRColorCool1Value( void )
{
    return UserPrefRedColorCool1;
}
WORD GetGColorCool1Value( void )
{
    return UserPrefGreenColorCool1;
}
WORD GetBColorCool1Value( void )
{
    return UserPrefBlueColorCool1;
}


#if ENABLE_FACTORY_SSCADJ
Bool AdjustFactorySSCModulation( MenuItemActionType action )
{
    WORD xdata tempValue;

    tempValue = DecIncValue( action, FactorySetting.SSCModulation, 0, PANEL_SSC_MODULATION_MAX, 1 );

    if( tempValue == FactorySetting.SSCModulation )
        return FALSE;

    FactorySetting.SSCModulation = tempValue;

    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);

    #if USEFLASH
    Set_FlashForceSaveFactoryFlag();
    #else
    NVRam_WriteByte( nvrFactoryAddr( SSCModulation ), FactorySetting.SSCModulation );
    #endif

    return TRUE;
}

WORD GetFactorySSCModulationValue( void )
{
    return FactorySetting.SSCModulation;
}

Bool AdjustFactorySSCPercentage( MenuItemActionType action )
{
    WORD xdata tempValue;

    tempValue = DecIncValue( action, FactorySetting.SSCPercentage, 0, PANEL_SSC_PERCENTAGE_MAX, 1 );
    if( tempValue == FactorySetting.SSCPercentage )
        return FALSE;

    FactorySetting.SSCPercentage = tempValue;

    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);
    #if USEFLASH
    Set_FlashForceSaveFactoryFlag();
    #else
    NVRam_WriteByte( nvrFactoryAddr( SSCPercentage ), FactorySetting.SSCPercentage );
    #endif
    return TRUE;
}

WORD GetFactorySSCPercentageValue( void )
{
    return FactorySetting.SSCPercentage;
}
#endif

Bool EnableBurninMode( void )
{
    PatternNo = 0;
    //msWriteByte( BK0_32, BIT0 );
    drvOSD_FrameColorEnable(TRUE);
    return TRUE;
}

WORD GetBurninValue( void )
{
    return ( BurninModeFlag ) ? ( 1 ) : ( 0 );
}
Bool AdjustBurnin( MenuItemActionType action )
{
    action = action;
    if( BurninModeFlag )
    {
        Clr_BurninModeFlag();
        Clr_DoBurninModeFlag();     //110928 Rick modified for do burnin mode while burnin on - A027
    }
    else
    {
        Set_BurninModeFlag();
        Set_DoBurninModeFlag();      //110928 Rick modified for do burnin mode while burnin on - A027
    }
#if USEFLASH
    //SaveMonitorSetting();
    Set_FlashForceSaveMonitorFlag();
#else
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#endif
    Delay1ms( 200 );
    return TRUE;
}

#if DisplayLogo!=NoBrand	//130613 Jun
WORD GetLogoValue(void)
{
	return (UserPrefLogoON) ? (1): (0);
}
Bool AdjustLogo(MenuItemActionType action)
{
	action = action;	
	if (UserPrefLogoON)
	{
		UserPrefLogoON=0;//130613 Modify for TPV Request
	}
	else
	{
		UserPrefLogoON=1;//130613 Modify for TPV Request
	}	
	#if USEFLASH
	SaveFactorySetting();
	#else
	NVRam_WriteByte(nvrFactoryAddr(LogoON), UserPrefLogoON);
	#endif
	
	return TRUE;
}
#endif

#if DDCCI_ENABLE
WORD GetDDCCIValue(void)
{
	return (DDCciFlag) ? (1): (0);
}

Bool AdjustDDCCI( MenuItemActionType action )
{
    action = action;
    if( DDCciFlag )
        Clr_DDCciFlag();
    else
        Set_DDCciFlag();
    return TRUE;
}
#endif

Bool EnterDisplayLogo( void )
{
	if(UserPrefLogoON)
	{
	    OsdCounter = 3;
	     mStar_SetupFreeRunMode();
	     mStar_AdjustBrightness(UserPrefBrightness);   //  80
#if !ENABLE_EDP_OUTPUT // Leger eDP Tx output without this process to avoide garbage display
	    Power_TurnOnPanel();
#endif
	}
	else
 	 	OsdCounter = 1;	
	
    return TRUE;
}

#if Enable_Expansion
void SetExpansionMode( void )
{
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);
    {
        #if UseINT
        mStar_EnableModeChangeINT(FALSE);
        #endif
        mStar_SetupMode();
        #if UseINT
        mStar_EnableModeChangeINT(TRUE);
        #endif
    }
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);
}

Bool AdjustExpansionMode(void)
{

        UserprefExpansionMode++;
        UserprefExpansionMode = (UserprefExpansionMode) % Expansion_Nums;

		drvOSD_FrameColorRGB(0, 0, 0); //切个黑底解决 4：3不正常

	  OverScanSetting.AspRatio = UserprefExpansionMode; //20150728	
        SetExpansionMode();

    Set_SaveMonitorSettingFlag();	
    return TRUE;
}
#endif

#if OSD_OD 
#if 0
        WORD GetOverDriveValue(void)
        {
            return (UserprefOverDriveSwitch) ? (1): (0);
        }
#endif
Bool AdjustOverDrive(MenuItemActionType action)
{
	action = action;
	if (UserprefOverDriveSwitch)
	{
		UserprefOverDriveSwitch = 0;
		msOverDriveOnOff( FALSE );
	}
	else
	{
		UserprefOverDriveSwitch = 1;
		msOverDriveOnOff( TRUE );
	}
#if USEFLASH
	Set_FlashSaveMonitorFlag();
#else
	NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#endif	
	Delay1ms(200);
	return TRUE;
}
#endif

#if LowBlueLight
Bool AdjustBlueLightMode(MenuItemActionType action)
{

    if( action == MIA_DecValue )
    {
        UserPrefLowBlueMode = ( UserPrefLowBlueMode + BlueLight_Nums - 1 ) % BlueLight_Nums;
    }
    else
    {
        UserPrefLowBlueMode = ( UserPrefLowBlueMode + 1 ) % BlueLight_Nums;
    }

   UserPrefColorTemp=CTEMP_USER;

	 if ( UserPrefColorTemp == CTEMP_USER )
	  {
		SetColorTemperatureUser();
	  }
	 
	ReadColorTempSetting();
#if USEFLASH
	Set_FlashSaveMonitorFlag();
#else
	NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#endif	
	Delay1ms(200);
	return TRUE;
}
#endif

