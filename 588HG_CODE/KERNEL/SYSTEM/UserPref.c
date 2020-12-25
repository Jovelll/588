#define _USERPREF_C_
#include <math.h>
#include "types.h"
#include "board.h"
#include "global.h"
#include "nvram.h"
#include "debug.h"
#include "Common.h"
#include "menudef.h"
#include "menufunc.h"
#include "MsDLC.h"
#include "MsACE.h"
#include "UserPref.h"
#include "msflash.h"
#include "misc.h"
#include "ms_reg.h"
#if MS_DAC
#include "drvDAC.h"
#endif
#include "panel.h"
#include "OsdDefault.h"


#define USERPREF_DEBUG    0
#if DEBUG_PRINT_ENABLE && USERPREF_DEBUG
#define USERP_printData(str, value)   printData(str, value)
#define USERP_printMsg(str)           printMsg(str)
#else
#define USERP_printData(str, value)
#define USERP_printMsg(str)
#endif

#define HFreq_Tol   16 // 1.6 KHz
#define VFreq_Tol   8  // 0.8 Hz
void CheckBoard( void );
void ReadBlacklitTime( void );
Bool IsEEPRomEmpty( BYTE EmptyValue );
//Benz 2007.4.24   23:25:55 void SaveBlacklitTime( void );

void Init_FactorySetting( void )
{
    UserPrefAdcPgaGain = DEF_ADC_PGA_GAIN;
    UserPrefAdcRedGain = DEF_ADC_RED_GAIN;
    UserPrefAdcGreenGain = DEF_ADC_GREEN_GAIN;
    UserPrefAdcBlueGain = DEF_ADC_BLUE_GAIN;
    UserPrefAdcRedOffset = DEF_ADC_OFFSET;
    UserPrefAdcGreenOffset = DEF_ADC_OFFSET;
    UserPrefAdcBlueOffset = DEF_ADC_OFFSET;

    FUserPrefBrightnessWarm1 = DefBrightness;
    FUserPrefContrastWarm1 = DefContrast;
    UserPrefRedColorWarm1 = DefWarm_RedColor;
    UserPrefGreenColorWarm1 = DefWarm_GreenColor;
    UserPrefBlueColorWarm1 = DefWarm_BlueColor;
    FUserPrefBrightnessCool1 = DefBrightness;
    FUserPrefContrastCool1 = DefContrast;
    UserPrefRedColorCool1 = DefCool_RedColor;
    UserPrefGreenColorCool1 = DefCool_GreenColor;
    UserPrefBlueColorCool1 = DefCool_BlueColor;
    FUserPrefBrightnessNormal = DefBrightness;
    FUserPrefContrastNormal = DefContrast;
    UserPrefRedColorNormal = DefNormal_RedColor;
    UserPrefGreenColorNormal = DefNormal_GreenColor;
    UserPrefBlueColorNormal = DefNormal_BlueColor;
    UserPrefRedColorSRGB = DefsRGB_RedColor;
    UserPrefGreenColorSRGB = DefsRGB_GreenColor;
    UserPrefBlueColorSRGB = DefsRGB_BlueColor;
    FUserPrefBrightnessSRGB = DefBrightness;
    FUserPrefContrastSRGB = DefContrast;
    UserPrefLogoON=1;	//120420 Modify
#if !USEFLASH
    FactoryCheckSum = FLASH_CHECKSUM;
#else
    FactorySetting.Ident = FLASH_IDENTDATA;
    FactorySetting.CheckSum = FLASH_CHECKSUM;
    FactoryProductModeValue = ProductModeOffValue;
#endif


#if PanelRSDS
    UserPrefVcomValue = DefVcomValue;
#endif

    #if ENABLE_FACTORY_SSCADJ    // 2011-09-16 CC
    FactorySetting.SSCModulation = PANEL_SSC_MODULATION_DEF;
    FactorySetting.SSCPercentage = PANEL_SSC_PERCENTAGE_DEF;
    #endif
#if VGA_EDID_Write_To_24C02	
	Set_LoadVGAEDIDFlag();
#endif
#if DVI_EDID_Write_To_24C02
	Set_LoadDVIEDIDFlag();
#endif
#if HDMI_EDID_Write_To_24C02
	Set_LoadHDMIEDIDFlag();
#endif

#if ENABLE_UPDTAE_BURIN
#if (Customer_Type_Sel==Customer_XINTAO)
//Set_FactoryModeFlag();
#else
Set_FactoryModeFlag();
#endif
Set_BurninModeFlag();
Set_DoBurninModeFlag(); 	//120703 Modify for TPV Request

#if USEFLASH
    //SaveMonitorSetting();
    Set_FlashForceSaveMonitorFlag();
#else
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#endif

#endif



}

void Init_MonitorSetting( void )
{
    UserPrefBrightness = DefBrightness;
    UserPrefContrast = DefContrast;
    UserPrefBrightnessTemp= UserPrefBrightness;
    UserPrefContrastTemp=UserPrefContrast;
    UserPrefRedColor = DefRedColor;
    UserPrefGreenColor = DefGreenColor;
    UserPrefBlueColor = DefBlueColor;
    UserPrefBrightnessUser = DefBrightness;
    UserPrefContrastUser = DefContrast;
#if 1 // 090721
    UserPrefRedColorUser = DefColorUser;
    UserPrefGreenColorUser = DefColorUser;
    UserPrefBlueColorUser = DefColorUser;
#else
    UserPrefRedColorUser = DefRedColor;
    UserPrefGreenColorUser = DefGreenColor;
    UserPrefBlueColorUser = DefBlueColor;
#endif
    UserPrefBrightnessWarm1 = DefBrightness;
    UserPrefContrastWarm1 = DefContrast;
    UserPrefBrightnessCool1 = DefBrightness;
    UserPrefContrastCool1 = DefContrast;
    UserPrefBrightnessNormal = DefBrightness;
    UserPrefContrastNormal = DefContrast;
    UserPrefBrightnessSRGB = DefBrightness;
    UserPrefContrastSRGB = DefContrast;
    UserPrefECOMode = 0;
    UserPrefColorTemp = Def_CTEMP;

    UserPrefHue = DefHue;
    UserPrefSaturation = DefSaturation;

#if MWEFunction
    UserPrefSubContrast = DefSubContrast;
    UserPrefSubBrightness = DefSubBrightness;
    UserPrefColorFlags = 0;
    UserPrefBFSize = 1;
    UserPrefBFVSize = 0;
    UserPrefBFHStart = 0;
    UserPrefBFVStart = 0;
#endif

    UserPrefLanguage = LANG_Default;
#if (AudioFunc && MS_DAC && (ENABLE_HDMI || ENABLE_DP_INPUT))
    UserPrefAudioSource = AUDIO_DIGITAL;
#endif
#if Audio_Switch
	UserPrefHDMIAudioType=DefHDMIAudioType;
 #endif
#if AudioIC_CS8003_Enable
	#if EarphoneDet_Enable	
	 UserPrefEarVolume=DACVolume50Value;
	#endif
#endif
    UserPrefVolume = 50;
    UserPrefOsdHStart = 50;
    UserPrefOsdVStart = 50;
    UserPrefOsdTime = 10;
	UserPrefOsdTransparency = DEF_OSD_TRANSPARENCY;
    UserPrefDcrMode = 0;
#if ENABLE_SUPER_RESOLUTION
    UserPrefSuperResolutionMode = SRMODE_OFF;
#endif
#if ENABLE_TNR
    UserPrefNRmode= NRMODE_OFF;
#endif

#if ENABLE_DeBlocking
    UserPrefDeBlocking = 0;
#endif
#if Enable_Gamma
    UserPrefGamaMode = 0;
#endif
    UserPrefLastMenuIndex = 0;
    UserPrefInputType = DEF_INPUT_TYPE;
    UserPrefInputPriorityType = DEF_INPUT_PRIORITY;
    UserPrefSavedModeIndex = UserModeNumber;
#if DDCCI_ENABLE
    UserprefRedBlackLevel = 50;
    UserprefGreenBlackLevel = 50;
    UserprefBlueBlackLevel = 50;
#endif
#if Enable_Expansion
    UserprefExpansionMode = DefExpansion;
#endif
    //=======for AutoBacklight=====================
    UserprefHistogram1 = 0x40;
    UserprefHistogram2 = 0xB0;
    UserprefALha = 50;
    UserprefBata = 50;
    MSBankNo = 0;
#if ENABLE_R2_2DTO3D
    UserPref2Dto3D = Def2Dto3D;
    UserPref2Dto3DGameMode = Def2Dto3DGameMode;
    UserPref2Dto3DGain = Def2Dto3DGain;
    UserPref2Dto3DOffset = Def2Dto3DOffset;
#endif
    MonitorFlags = bPowerOnBit | bBurninModeBit | bDCCCIONOBit; //Benz 2007.4.23   20:32:00
#if !USEFLASH
    UserprefBacklighttime = 0;
#endif

#if ENABLE_SHARPNESS
    UserPrefSharpness= DefSharpness;
#endif
#if ENABLE_RTE
	UserprefOverDriveSwitch=TRUE;
	UserprefOverDriveWeighting=DEF_OD_Weighting;
#endif

#if ENABLE_FREESYNC
	UserprefFreeSyncMode=FreeSyncMenuItems_On;
#endif
   UserOSDMuteOnFlag=0;
#if LowBlueLight
UserPrefLowBlueMode=BlueLight_Off;
#endif
#if USEFLASH
    MonitorSetting.Ident = FLASH_IDENTDATA;
    MonitorSetting.CheckSum = FLASH_CHECKSUM;
#else
    MonitorSetting.CheckSum = FLASH_CHECKSUM;
#endif
}
void Init_ModeSetting( void )
{
    UserPrefHTotal = StandardModeHTotal;
    UserPrefHStart = StandardModeHStart;
    UserPrefVStart = StandardModeVStart;
    UserPrefAutoHStart = UserPrefHStart;
    UserPrefAutoVStart = StandardModeVStart; //2004/01/15 num11
    UserPrefPhase = 18;
    UserPrefAutoTimes = 0;
#if USEFLASH
    ModeSetting.Index = SaveIndex;
    ModeSetting.Ident = FLASH_IDENTDATA;
    ModeSetting.HorFreq = 0xFFFF;
    ModeSetting.VerFreq = 0xFFFF;
    ModeSetting.Flags = 0xFF;
    ModeSetting.CheckSum = FLASH_CHECKSUM;
#else
    ModeSetting.CheckSum = FLASH_CHECKSUM;
#endif
}
void CheckColorValueRange( void )
{
    if( UserPrefContrast > MaxContrastValue || UserPrefContrast < MinContrastValue )
    {
        UserPrefContrast = DefContrast;
    }
#if 1
    if( UserPrefRedColor > MaxColorValue )
        UserPrefRedColor = DefRedColor;
    if( UserPrefGreenColor > MaxColorValue )
        UserPrefGreenColor = DefGreenColor;
    if( UserPrefBlueColor > MaxColorValue )
        UserPrefBlueColor = DefBlueColor;
#endif
}
Bool CheckMonitorSettingOutOfRange( void )
{
    if(
        UserPrefBrightness > MaxBrightnessValue ||
        UserPrefContrast > MaxContrastValue ||
        UserPrefContrast < MinContrastValue ||
        UserPrefECOMode  >= ECO_Nums ||
        UserPrefColorTemp >= CTEMP_Nums ||
#if AudioFunc
        UserPrefVolume  > 100 ||
#endif
        UserPrefLanguage >= LANG_Nums ||
        UserPrefOsdHStart > 100 ||
        UserPrefOsdHStart % 5 != 0 ||
        UserPrefOsdVStart > 100 ||
        UserPrefOsdVStart % 5 != 0 ||
        UserPrefOsdTime > 100 ||
        UserPrefOsdTime < 5 ||
        UserPrefOsdTime % 5 != 0 ||
        UserPrefDcrMode > 2 ||
        UserPrefHue > 100 ||
#if MWEFunction
        UserPrefSubBrightness < MinSubBrightness ||
        UserPrefSubBrightness > MaxSubBrightness ||
        UserPrefSubContrast < MinSubContrast ||
        UserPrefSubContrast > MaxSubContrast ||
        
        UserPrefBFSize > 7 ||
#endif
#if Enable_Gamma
        UserPrefGamaMode >= GAMA_Nums ||
#endif
        UserPrefInputType >= Input_Nums ||
#if ENABLE_R2_2DTO3D
        UserPref2Dto3D > 1 ||
        UserPref2Dto3DGameMode > 1 ||
        UserPref2Dto3DGain > 4 ||
        UserPref2Dto3DOffset > 4 ||
#endif

#if ENABLE_SUPER_RESOLUTION
        UserPrefSuperResolutionMode >= SRMODE_Nums ||
#endif
#if ENABLE_TNR
        UserPrefNRmode>= NRMODE_Nums ||
#endif

#if ENABLE_SHARPNESS
        UserPrefSharpness>= 100 ||
#endif
#if ENABLE_DeBlocking
        UserPrefDeBlocking > 2 ||
#endif
        MSBankNo > 2
    )
        return TRUE;
    return FALSE;
}
Bool CheckFactorySettingOutOfRange( void )
{

#if 0 // wait for coding
    if(
        UserPrefAdcRedGain <= 50 ||
        UserPrefAdcGreenGain <= 50 ||
        UserPrefAdcBlueGain <= 50 ||
        UserPrefAdcRedOffset <= 50 ||
        UserPrefAdcGreenOffset <= 50 ||
        UserPrefAdcBlueOffset <= 50
    )
        return TRUE;
    if(
        UserPrefAdcRedGain >= 200 ||
        UserPrefAdcGreenGain >= 200 ||
        UserPrefAdcBlueGain >= 200 ||
        UserPrefAdcRedOffset >= 200 ||
        UserPrefAdcGreenOffset >= 200 ||
        UserPrefAdcBlueOffset >= 200
    )
        return TRUE;
#endif

    if(
        FUserPrefBrightnessWarm1 != FUserPrefBrightnessCool1 ||
        FUserPrefBrightnessCool1 != FUserPrefBrightnessNormal ||
        FUserPrefBrightnessNormal != FUserPrefBrightnessSRGB ||
        FUserPrefBrightnessSRGB != FUserPrefBrightnessWarm1
    )
        return TRUE;
    if( FUserPrefContrastWarm1 != FUserPrefContrastCool1 ||
            FUserPrefContrastCool1 != FUserPrefContrastNormal ||
            FUserPrefContrastNormal != FUserPrefContrastSRGB ||
            FUserPrefContrastSRGB != FUserPrefContrastWarm1
      )
        return TRUE;
    if(
        UserPrefRedColorCool1 <= 50 ||
        UserPrefGreenColorCool1 <= 50 ||
        UserPrefBlueColorCool1 <= 50 ||
        UserPrefRedColorNormal <= 50 ||
        UserPrefGreenColorNormal <= 50 ||
        UserPrefBlueColorNormal <= 50 ||
        UserPrefRedColorWarm1 <= 50 ||
        UserPrefGreenColorWarm1 <= 50 ||
        UserPrefBlueColorWarm1 <= 50 ||
        UserPrefRedColorSRGB <= 50 ||
        UserPrefGreenColorSRGB <= 50 ||
        UserPrefBlueColorSRGB <= 50
    )
        return TRUE;
    if(
        abs( UserPrefRedColorCool1 - UserPrefRedColorNormal ) >= 100 ||
        abs( UserPrefRedColorNormal - UserPrefBlueColorCool1 )  >= 100 ||
        abs( UserPrefRedColorCool1 - UserPrefBlueColorCool1 )  >= 100 ||

        abs( UserPrefRedColorNormal - UserPrefGreenColorNormal ) >= 100 ||
        abs( UserPrefGreenColorNormal - UserPrefBlueColorNormal )  >= 100 ||
        abs( UserPrefRedColorNormal - UserPrefBlueColorNormal )  >= 100 ||

        abs( UserPrefRedColorWarm1 - UserPrefGreenColorWarm1 ) >= 100 ||
        abs( UserPrefGreenColorWarm1 - UserPrefBlueColorWarm1 )  >= 100 ||
        abs( UserPrefRedColorWarm1 - UserPrefBlueColorWarm1 )  >= 100 ||

        abs( UserPrefRedColorSRGB - UserPrefGreenColorSRGB ) >= 100 ||
        abs( UserPrefGreenColorSRGB - UserPrefBlueColorSRGB )  >= 100 ||
        abs( UserPrefRedColorSRGB - UserPrefBlueColorSRGB )  >= 100
    )
        return TRUE;

    #if ENABLE_FACTORY_SSCADJ    // 2011-09-16 19:46 CC
    if (FactorySetting.SSCModulation>PANEL_SSC_MODULATION_MAX)
        FactorySetting.SSCModulation = PANEL_SSC_MODULATION_DEF;

    if (FactorySetting.SSCPercentage>PANEL_SSC_PERCENTAGE_MAX)
        FactorySetting.SSCPercentage = PANEL_SSC_PERCENTAGE_DEF;
    #endif

#if !USEFLASH
    if( FactoryCheckSum != FLASH_CHECKSUM )
        return TRUE;
#endif
    return FALSE;
}

void CheckModeSettingRange( void )
{
    if( abs( UserPrefHTotal - StandardModeHTotal ) > 100 )
    {
        UserPrefHTotal = StandardModeHTotal;
    }
    //  if (UserPrefHStart>2*StandardModeHStart)
    //    UserPrefHStart=StandardModeHStart;
    //  if (UserPrefVStart>2*StandardModeVStart)
    //    UserPrefVStart=StandardModeVStart;
    if( abs( UserPrefHStart - UserPrefAutoHStart ) > 50 )
        //2004/02/10 num1
    {
        UserPrefHStart = UserPrefAutoHStart;
    } //2004/02/10 num1
    if( UserPrefVStart > 2 * UserPrefAutoVStart )
        //2004/02/10 num1
    {
        UserPrefVStart = UserPrefAutoVStart;
    } //2004/02/10 num1
    UserPrefPhase &= MAX_PHASE_VALUE;
    if( /*UserPrefAutoTimes == 0 || */abs( UserPrefHStart - UserPrefAutoHStart ) > 50 )
    {
        UserPrefAutoHStart = UserPrefHStart;
    }
}
#if USEFLASH
void UserPref_EnableFlashSaveBit( WORD ucFlag )
{
    g_wFlashSaveState |= ucFlag;
}
void UserPref_FlashSaveFlagCheck( void )
{
    if( FlashSaveMonitorFlag )
    {
        Clr_FlashSaveMonitorFlag();
        Set_FlashForceSaveMonitorFlag();
    }
    if( FlashSaveModeFlag )
    {
        Clr_FlashSaveModeFlag();
        Set_FlashForceSaveModeFlag();
    }
    if( FlashSaveFactoryFlag )
    {
        Clr_FlashSaveFactoryFlag();
        Set_FlashForceSaveFactoryFlag();
    }
    if( FlashSaveMonitor2Flag )
    {
        Clr_FlashSaveMonitor2Flag();
        Set_FlashForceSaveMonitor2Flag();
    }
}

void UserPref_FlashDataSaving( void )
{
    if( FlashForceSaveMonitorFlag )
    {
        USERP_printMsg("Flash--SaveMonitorSetting");
        Clr_FlashForceSaveMonitorFlag();
        SaveMonitorSetting();
    }
    if( FlashForceSaveModeFlag )
    {
        USERP_printMsg("Flash-SaveModeSetting");
        Clr_FlashForceSaveModeFlag();
        SaveModeSetting();
    }
    if( FlashForceSaveFactoryFlag )
    {
        USERP_printMsg("Flash--SaveFactorySetting");
        Clr_FlashForceSaveFactoryFlag();
        SaveFactorySetting();
    }
    if( FlashForceSaveMonitor2Flag )
    {
        USERP_printMsg("Flash--SaveMonitorSetting2");
        Clr_FlashForceSaveMonitor2Flag();
        SaveMonitorSetting2();
    }
}
void Init_MonitorSetting2( void )
{
    MonitorSetting2.Ident = FLASH_IDENTDATA;
    MonitorSetting2.Backlighttime = 0;
#if ENABLE_BURNIN_MENU
	 MonitorSetting2.Burnintime = 0;
#endif	
    MonitorSetting2.CheckSum = FLASH_CHECKSUM;
}
void SaveMonitorSetting( void )
{
    MonitorSetting.Ident = FLASH_IDENTDATA;
    MonitorSetting.CheckSum = FLASH_CHECKSUM;
    Flash_WriteMonitorSet(( BYTE* )&MonitorSetting, MonitorSettingSize );
}
void SaveMonitorSetting2( void )
{
    MonitorSetting2.Ident = FLASH_IDENTDATA;
    MonitorSetting2.CheckSum = FLASH_CHECKSUM;
    Flash_WriteMonitorSet2(( BYTE* )&MonitorSetting2, MonitorSetting2Size );
}

void SaveFactorySetting( void )
{
    FactorySetting.Ident = FLASH_IDENTDATA;
    FactorySetting.CheckSum = FLASH_CHECKSUM;
    Flash_WriteFactorySet(( BYTE* )&FactorySetting, FactorySettingSize );
}

void SaveModeSetting( void )
{
    if(!CURRENT_INPUT_IS_VGA())//( SrcInputType >= Input_Digital )
        return;

    // USERP_printData("UserModeFlag %x",UserModeFlag);
    //USERP_printData("SaveIndex %x",SaveIndex);

    if( UserModeFlag )
    {
        ModeSetting.Index = SaveIndex | BIT7; //|= 0x80;
        ModeSetting.HorFreq = HFreq( SrcHPeriod ); //HFreq(tempHperiod);
        ModeSetting.VerFreq = VFreq( HFreq( SrcHPeriod ), SrcVTotal );
        ModeSetting.Flags = ( SrcFlags & 0x13 );

        USERP_printData( "ModeSetting.Index %x", ModeSetting.Index );

    }
    else
    {
        ModeSetting.Index = SaveIndex = SrcModeIndex;
        ModeSetting.HorFreq = 0xFFFF;
        ModeSetting.VerFreq = 0xFFFF;
        ModeSetting.Flags = 0xFF;
    }

    ModeSetting.Ident = FLASH_IDENTDATA;
    ModeSetting.CheckSum = FLASH_CHECKSUM;

    if( UserModeFlag )
        Flash_WriteModeSet(( SaveIndex | BIT7 ), ( BYTE* )&ModeSetting );
    else
        Flash_WriteModeSet( SaveIndex, ( BYTE* )&ModeSetting );
    ModeSetting.Index &= 0x7F;
}
Bool IsFlashSettingAreaEmpty( BYTE EmptyValue )
{
    BYTE i, j;
    for( i = 0; i < FDATA_DYNAMIC_SECTOR_NUM; i++ )
    {
        for( j = 0; j < 10; j++ )
        {
            if( FlashReadByte( FDATA_DYNAMIC_ADDR + FDATA_SECTOR_SIZE * i + j ) != EmptyValue )
                return FALSE;
        }
    }
    return TRUE;
}

#if Display_Checksum
#define PROTECT_START_ADDR			0x02FFFEul  
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read one byte from flash.
// <Returns> : Data value in the address
// <Parameter>:  -  <Flow>  -  <Description>
//-----------------------------------------------------------------------------
//  wAddr   -   In  -   Only in the same bank ot MsFlash.c
///////////////////////////////////////////////////////////////////////////////
//#define xfr_FlashCode_NVRom ((BYTE code*)0x0000)
void drvFlashWaitSPINotBusy(void);

BYTE FlashReadByte_CheckSum(DWORD address)
{

    SPI_SI_BUF[0]   = (SPI_MODE == SPI_MODE_FR) ? H_READ:READ;
    SPI_SI_BUF[1]   = (address>>16)&0xff;
    SPI_SI_BUF[2]   = (address>>8)&0xff;
    SPI_SI_BUF[3]   = address&0xff;
    //2nd
    SPI_SI_BUF[4]   = RDSR;
    if(SPI_MODE == SPI_MODE_FR)
        SPI_SI_BUF[5]   = RDSR;
    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|RDSR_IN_2ND|EN_AUTO_CHECK);
    SET_WBF_SIZE( (SPI_MODE == SPI_MODE_FR) ? 5:4, 1, 0);
    SET_WBF_SIZE( 5, 1, 0);
    SET_RBF_SIZE(1, 1, 0);
    SPI_TRIGGER     = 0x01; //start flash programming
    
    drvFlashWaitSPINotBusy();
    
    return SPI_SO_BUF[0];
}

void Checksum_Calculator(void)
{
	Code_checksun=(FlashReadByte_CheckSum(PROTECT_START_ADDR));
	
//	printData( " xxxxxxxxxxxxx Code_checksun0=%d", Code_checksun );
	Code_checksun=((Code_checksun<<8)|(FlashReadByte_CheckSum(PROTECT_START_ADDR+1)));
	
//	printData( " xxxxxxxxxxxxx Code_checksun1=%d", Code_checksun );
}
#endif


void ReadMonitorSetting( void )
{
    BYTE xdata FlashEmptyFlag = 0;
    Flash_GetBufferAddr();
    Flash_AllocateBufferAddr();
    if( IsFlashSettingAreaEmpty( 0x00 ) || IsFlashSettingAreaEmpty( 0xFF ) )
    {
        FlashEmptyFlag = 1;
    }
    Flash_CheckBufferType();
#if Display_Checksum
			Checksum_Calculator();
#endif

    if( !Flash_ReadMonitorSet(( BYTE* )&MonitorSetting, MonitorSettingSize ) )
    {
        USERP_printMsg( " RMS MonitorSetting initial" );
        Init_MonitorSetting();
        SaveMonitorSetting();
    }
    if( CheckMonitorSettingOutOfRange() )
    {
        USERP_printMsg( " CheckMonitorSettingOutOfRange initial" );
        Init_MonitorSetting();
        SaveMonitorSetting();
    }

    if( !Flash_ReadFactorySet(( BYTE* )&FactorySetting, FactorySettingSize ) )
    {
        USERP_printMsg( " RMS FactorySetting initial" );
        Init_FactorySetting();
        if( FlashEmptyFlag ) //when flash is empty set product mode on
            FactoryProductModeValue = ProductModeOnValue;
        SaveFactorySetting();
    }

    if( CheckFactorySettingOutOfRange() )
    {
        USERP_printMsg( " CheckFactorySettingOutOfRange initial" );
        Init_FactorySetting();
        SaveFactorySetting();
    }

    if( !Flash_ReadMonitorSet2(( BYTE* )&MonitorSetting2, MonitorSetting2Size ) )
    {
        USERP_printMsg( " RMS MonitorSetting2 initial" );
        Init_MonitorSetting2();
        SaveMonitorSetting2();
    }
    USERP_printData( "   Backlighttime[%d]", MonitorSetting2.Backlighttime );
    if( FactorySetting.ProductModeValue != ProductModeOnValue ) //20091027
        Clr_ProductModeFlag();
    else
    {
        //UserPrefVolume=100;//PE Request
        Set_ProductModeFlag();
    }

#if DISABLE_AUTO_SWITCH
    SrcInputType = UserPrefInputType = FIXED_PORT;
#else
    SrcInputType = UserPrefInputType;
#endif

}

Bool ReadUserModeSetting( void )
{
    BYTE bFound = FALSE;
    WORD hFreq, vFreq;
    BYTE i;

    hFreq = HFreq( SrcHPeriod ); //HFreq(tempHperiod);
    vFreq = VFreq( hFreq, SrcVTotal );

    // 20070426Albert.Search each user mode
    for( i = 0; i < UserModeNumber; i++ )
    {
        Flash_ReadModeSet(( i | BIT7 ), ( BYTE* )&ModeSetting ); // BIT7 == (UserModeFlag<<1)
        if( ModeSetting.Ident == FLASH_IDENTDATA && ModeSetting.CheckSum == FLASH_CHECKSUM )
        {
            if( abs( hFreq - ModeSetting.HorFreq ) < HFreq_Tol &&    abs( vFreq - ModeSetting.VerFreq ) < VFreq_Tol &&
                    ( SrcFlags & 0x13 ) == ModeSetting.Flags )
                break;
        }
    }

    // 20070426Albert,Init new Mode info
    if( i < UserModeNumber )
        bFound = TRUE;
    else
    {
        UserPrefSavedModeIndex = ( UserPrefSavedModeIndex + 1 ) % UserModeNumber;
        SaveIndex = UserPrefSavedModeIndex;
        #if USEFLASH
        Set_FlashSaveMonitorFlag();
        #else
        SaveMonitorSetting();
        #endif
    }
    return bFound;
}

void ReadModeSetting( void )
{
    Bool bFound = FALSE;

#if INPUT_TYPE != INPUT_1A
    if(!CURRENT_INPUT_IS_VGA())//(SrcInputType > Input_Analog1)
    {
        Init_ModeSetting();
        return;
    }
#endif

    if( UserModeFlag )
        bFound = ReadUserModeSetting();
    else
    {
        SaveIndex = SrcModeIndex;
        bFound = Flash_ReadModeSet( SrcModeIndex, ( BYTE* )&ModeSetting );
    }

    if( bFound )
    {
        USERP_printMsg( "mode setting found" );
        USERP_printData( "Index=%d", ModeSetting.Index );
        USERP_printData( "AutoTimes=%d", ModeSetting.AutoTimes );
    }
    else
    {
        USERP_printMsg( "mode setting found fail" );
        Init_ModeSetting();
        SaveModeSetting();
    }
    CheckModeSettingRange();
}
#else//#if !USEFLASH =================================================================================

void SaveFactorySetting( void )
{
    NVRam_WriteTbl( FactorySettingAddr, ( BYTE* ) &FactorySetting, FactorySettingSize );
    #if DEBUG_PRINT_ENABLE
    printMsg( "Save FactorySetting" );
    #endif
}

void SaveMonitorSetting( void )
{
    NVRam_WriteTbl( MonitorSettingAddr, ( BYTE* ) &MonitorSetting, MonitorSettingSize );
    #if DEBUG_PRINT_ENABLE
    printMsg( "Save MonitorSetting" );
    #endif
}

void SaveModeSetting( void )
{
    //NVRam_WriteTbl(nvrModeBaseAddr(SrcModeIndex), (BYTE*)&ModeSetting, ModeSettingSize);
    NVRam_WriteTbl( nvrModeBaseAddr( SaveIndex ), ( BYTE* ) &ModeSetting, ModeSettingSize );
}
void ReadMonitorSetting( void )
{
    BYTE tempValue, i;

    if( IsEEPRomEmpty( 0xFF ) || IsEEPRomEmpty( 0x00 ) )
    {
        WORD temp = ProductModeOnValue;
        NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &temp ), 2 );
        Init_FactorySetting();
        SaveFactorySetting();
        Init_MonitorSetting();
        UserPrefVolume = 100;
        SaveMonitorSetting();
    }
    CheckBoard(); //Benz 2007.4.24   23:28:02 Why?
    NVRam_ReadByte( nvrMonitorAddr( CheckSum ), &tempValue );
    if( tempValue == FLASH_CHECKSUM )
    {
        NVRam_ReadTbl( MonitorSettingAddr, ( BYTE* ) &MonitorSetting, MonitorSettingSize );
    }
    else
    {
        for( i = 0; i < 10; i++ )
        {
            NVRam_ReadByte( nvrMonitorAddr( CheckSum ), &tempValue );
            if( tempValue == FLASH_CHECKSUM ) // double check
            {
                NVRam_ReadTbl( MonitorSettingAddr, ( BYTE* )&MonitorSetting, MonitorSettingSize );
                break;
            }
        }
        if( i == 10 )
        {
            Init_MonitorSetting();
            SaveMonitorSetting();
        }
        //Init_MonitorSetting();
        //SaveMonitorSetting();
    }
    //************************************************************
    NVRam_ReadByte( nvrFactoryAddr( CheckSum ), &tempValue );
    if( tempValue == FLASH_CHECKSUM )
    {
        NVRam_ReadTbl( FactorySettingAddr, ( BYTE* ) &FactorySetting, FactorySettingSize );
    }
    else
    {
        for( i = 0; i < 10; i++ )
        {
            NVRam_ReadByte( nvrFactoryAddr( CheckSum ), &tempValue );
            if( tempValue == FLASH_CHECKSUM ) // double check
            {
                NVRam_ReadTbl( FactorySettingAddr, ( BYTE* )&FactorySetting, FactorySettingSize );
                break;
            }
        }
        if( i == 10 )
        {
            Init_FactorySetting();
            SaveFactorySetting();
        }
    }

    if( CheckMonitorSettingOutOfRange() )
    {
        Init_MonitorSetting();
        SaveMonitorSetting();
    }

    if( CheckFactorySettingOutOfRange() )
    {
        Init_FactorySetting();
        SaveFactorySetting();
    }

    //-----------------------------------------------------------------
    {
        WORD temp;
        NVRam_ReadTbl( ProductModeAddr, ( BYTE* )( &temp ), 2 );
        if( temp == ProductModeOnValue )
            Set_ProductModeFlag();
        else
            Clr_ProductModeFlag();
    }
    //-----------------------------------------------------------------
#if DISABLE_AUTO_SWITCH
    SrcInputType = UserPrefInputType = FIXED_PORT;
#else
    SrcInputType = UserPrefInputType;
#endif
}
void GetModeSaveIndex( void )
{
    ModeInfoType modeInfo;
    BYTE i;
    WORD hFreq, vFreq;
    hFreq = HFreq( SrcHPeriod );
    vFreq = VFreq( hFreq, SrcVTotal );
    for( i = 0; i < NumberOfMode; i++ )
    {
        NVRam_ReadTbl( nvrModeInfoAddr( i ), ( BYTE* ) &modeInfo, ModeInfoSize );
        if( abs( hFreq - modeInfo.HerFreq ) < HFreq_Tol && abs( vFreq - modeInfo.VerFreq ) < VFreq_Tol
        && abs( SrcVTotal - modeInfo.VTotal ) < 3 && ( SrcFlags & 0x13 ) == modeInfo.Flags )
        {
            #if DEBUG_PRINT_ENABLE
            printMsg( "found saveindex" );
            printData( "SaveIndex=%d", i );
            #endif
            break;
        }
    }

    if( i >= NumberOfMode )
    {
        UserPrefSavedModeIndex = ( UserPrefSavedModeIndex + 1 ) % NumberOfMode;
        SaveIndex = UserPrefSavedModeIndex;
        modeInfo.HerFreq = hFreq;
        modeInfo.VerFreq = vFreq;
        modeInfo.VTotal = SrcVTotal;
        modeInfo.Flags = SrcFlags & 0x13;
        NVRam_WriteTbl( nvrModeInfoAddr( SaveIndex ), ( BYTE* ) &modeInfo, ModeInfoSize );
        NVRam_WriteByte( nvrModeAddr( CheckSum ), 0 );
        NVRam_WriteByte( nvrMonitorAddr( SavedModeIndex ), UserPrefSavedModeIndex );
    }
    else
    {
        SaveIndex = i;
    }
}
void ReadModeSetting( void )
{
    BYTE tempValue;
    if(CURRENT_INPUT_IS_VGA())//( SrcInputType < Input_Digital ) //if (SrcInputType != Input_Digital)
    {
        GetModeSaveIndex();
        NVRam_ReadByte( nvrModeAddr( CheckSum ), &tempValue );
        if( tempValue == FLASH_CHECKSUM )
        {
            #if DEBUG_PRINT_ENABLE
            printMsg( "mode setting found" );
            #endif
            NVRam_ReadTbl( nvrModeBaseAddr( SaveIndex ), ( BYTE* ) &ModeSetting, ModeSettingSize );
        }
        else
        {
            #if DEBUG_PRINT_ENABLE
            printMsg( "mode setting not found" );
            #endif
            Init_ModeSetting();
            SaveModeSetting();
        }
        CheckModeSettingRange();
    }
    else
    {
        Init_ModeSetting();
    }
}
void EraseEEPROM( void )
{
    //NVRam_WriteByte(nvrMonitorAddr(CheckSum), 0);
    for( SaveIndex = 0; SaveIndex < NumberOfMode; SaveIndex++ )
    {
        NVRam_WriteByte( nvrModeAddr( CheckSum ), 0 );
    }
    //    SrcFlags |= bUserMode;
    //    for (SaveIndex = 0; SaveIndex < NumberOfMode; SaveIndex++)
    //    {
    //        NVRam_WriteByte(nvrModeInfoAddr(CheckSum), 0);
    //    }
    SrcFlags &= ~bUserMode;
    //  NVRam_WriteByte(Factory9300KBrightnessAddr, DefBrightness);
    //  NVRam_WriteByte(Factory6500KBrightnessAddr, DefBrightness);
    //  NVRam_WriteByte(Factory9300KContrastAddr, DefContrast);
    // NVRam_WriteByte(Factory6500KContrastAddr, DefContrast);
}
/**********
void CheckBoard(void)
{ BYTE tempValue;
NVRam_ReadByte(CheckBoardAddr, &tempValue);
if (tempValue!=BoardChecksum)
{ EraseEEPROM();
BlacklitTime=0;
SaveBlacklitTime();
NVRam_WriteByte(CheckBoardAddr, BoardChecksum);
}
else
ReadBlacklitTime();
}
 ************/
void CheckBoard( void )
{
    BYTE tempValue;
    BYTE i;
    NVRam_ReadByte( CheckBoardAddr, &tempValue );
    if( tempValue != BoardChecksum )
    {
        for( i = 0; i < 10; i++ )
        {
            NVRam_ReadByte( CheckBoardAddr, &tempValue );
            if( tempValue == BoardChecksum )    // double check
            {
                ReadBlacklitTime();
                break;
            }
        }
        if( i == 10 )
        {
            EraseEEPROM();
            BlacklitTime = 0;
            SaveBlacklitTime();
            NVRam_WriteByte( CheckBoardAddr, BoardChecksum );
        }
    }
    else
    {
        ReadBlacklitTime();
    }
}
Bool IsEEPRomEmpty( BYTE EmptyValue )
{
    BYTE tempValue;
	WORD i;

    for( i = 0; i < ModeSettingAddr - 1; i++ )
    {
        NVRam_ReadByte( i, &tempValue );
        if( tempValue == EmptyValue )
            continue;
        else
            return FALSE;
    }
    return TRUE;
}
void ReadBlacklitTime( void )
{
    NVRam_ReadTbl( BlacklitTimeAddr, ( BYTE* )( &BlacklitTime ), 4 );
}

void SaveBlacklitTime( void )
{
    NVRam_WriteTbl( BlacklitTimeAddr, ( BYTE* )( &BlacklitTime ), 4 );
}
#endif
