
#define FACTORYALIGNAPP_C
#include <types.h>
#include <board.h>
#include <mode.h>
#include <UserPrefDef.h>
#include <global.h>
#include <panel.h>
#include <msosd.h>
#include <debug.h>
//#include <uart.h>
#include <menudef.h>
#include <menufunc.h>
#include <DDC2Bi.h>
#include <NVRam.h>
#include <adjust.h>
#include <ms_reg.h>
#include <power.h>
#include <ms_rwreg.h>
#include <misc.h>
#include <i2c.h>
#include <userPref.h>
#include <FactoryAlignAOC.h>
#include <FactoryAlignApp.h>
#include <Menufunc.h>
#include <AutoFunc.h>
//#include <FactoryMenuFunc.h>
#include <Menu.h>
#include <msHdcp.h>
#include <msFlash.h>
#include <gpio_def.h>
#include <ddc.h>

extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;
extern BYTE idata rxInputPort;
#if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMJ||CHIP_ID==CHIP_TSUM9||CHIP_ID==CHIP_TSUMF
#if D2B_XShared_DDCBuffer
extern BYTE *DDCBuffer;
#else
extern BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
#endif
#else
extern BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
#endif
extern void CheckModeSettingRange( void );
extern Bool CheckFactorySettingOutOfRange( void );
extern void Init_FactorySetting( void );
extern Bool xdata bFlashWriteFactory;
extern void DDC2Bi_MessageReady( void );

//BYTE xdata LoadEDIDSelectPort = 0xFF ;
#if 0
Bool appFactoryAdjBrightness( WORD wData )
{
    if( wData > MaxBrightnessValue)
        UserPrefBrightness = MaxBrightnessValue;
    else
        UserPrefBrightness = wData;

    mStar_AdjustBrightness( UserPrefBrightness );

    return TRUE;
}

WORD appFactoryGetBrightness( void )
{
    return UserPrefBrightness;
}

Bool appFactoryAdjContrast( WORD wData )
{
    if( wData > MaxContrastValue)
        UserPrefContrast = MaxContrastValue;
    else
    {
        UserPrefContrast = wData;
        UserPrefContrast = UserPrefContrast + MinContrastValue;//checklater
    }
    
    mStar_AdjustContrast( UserPrefContrast );

    return TRUE;
}

WORD appFactoryGetContrast( void )
{
    return UserPrefContrast ;
}

Bool appFactoryAdjRedGain( WORD wData )
{
    if( wData > MaxColorValue)
        UserPrefRedColor = MaxColorValue;
    else if( wData < MinColorValue )
        UserPrefRedColor = MinColorValue;
    else
        UserPrefRedColor = wData;

    mStar_AdjustRedColor( UserPrefRedColor, UserPrefContrast );

    return TRUE;
}

WORD appFactoryGetRedGain( void )
{
    return UserPrefRedColor;
}

Bool appFactoryAdjGreenGain( WORD wData )
{
    if( wData > MaxColorValue )
        UserPrefGreenColor = MaxColorValue;
    else if( wData < MinColorValue )
        UserPrefGreenColor = MinColorValue;
    else
        UserPrefGreenColor = wData;

    mStar_AdjustGreenColor( UserPrefGreenColor, UserPrefContrast );

    return TRUE;
}

WORD appFactoryGetGreenGain( void )
{
    return UserPrefGreenColor;
}

Bool appFactoryAdjBlueGain( WORD wData )
{
    if( wData > MaxColorValue )
        UserPrefBlueColor = MaxColorValue;
    else if( wData < MinColorValue )
        UserPrefBlueColor = MinColorValue;
    else
        UserPrefBlueColor = wData;

    mStar_AdjustBlueColor( UserPrefBlueColor, UserPrefContrast );

    return TRUE;
}

WORD appFactoryGetBlueGain( void )
{
    return UserPrefBlueColor;
}

WORD appFactoryGetAdcRedGain( void )
{
    return UserPrefAdcRedGain;
}

WORD appFactoryGetAdcGreenGain( void )
{
    return UserPrefAdcGreenGain;
}

WORD appFactoryGetAdcBlueGain( void )
{
    return UserPrefAdcBlueGain;
}

WORD appFactoryGetAdcRedOffset( void )
{
    return UserPrefAdcRedOffset;
}

WORD appFactoryGetAdcGreenOffset( void )
{
    return UserPrefAdcGreenOffset;
}

WORD appFactoryGetAdcBlueOffset( void )
{
    return UserPrefAdcBlueOffset;
}

Bool appFactorySaveColorPreset( drvFactoryColorTempType ucColorTemp )
{
    switch( ucColorTemp )
    {
        case FACTORY_COLOR_SRGB:
            
            UserPrefColorTemp = CTEMP_SRGB;
            UserPrefRedColorSRGB = UserPrefRedColor;
            UserPrefGreenColorSRGB = UserPrefGreenColor;
            UserPrefBlueColorSRGB = UserPrefBlueColor;
            FUserPrefBrightnessSRGB = UserPrefBrightness;
            FUserPrefContrastSRGB = UserPrefContrast;

            break;

        case FACTORY_COLOR_5000K:

            break;

        case FACTORY_COLOR_6500K:
            UserPrefColorTemp = CTEMP_Warm1;
            UserPrefRedColorWarm1 = UserPrefRedColor;
            UserPrefGreenColorWarm1 = UserPrefGreenColor;
            UserPrefBlueColorWarm1 = UserPrefBlueColor;
            UserPrefBrightnessWarm1 = UserPrefBrightness;
            UserPrefContrastWarm1 = UserPrefContrast;
            FUserPrefBrightnessWarm1 = UserPrefBrightness;
            FUserPrefContrastWarm1 = UserPrefContrast;
            SaveFactorySetting();

            break;

        case FACTORY_COLOR_7500K:
            UserPrefColorTemp = CTEMP_Normal;
            UserPrefRedColorNormal = UserPrefRedColor;
            UserPrefGreenColorNormal = UserPrefGreenColor;
            UserPrefBlueColorNormal = UserPrefBlueColor;
            UserPrefBrightnessNormal = UserPrefBrightness;
            UserPrefContrastNormal = UserPrefContrast;
            FUserPrefBrightnessNormal = UserPrefBrightness;
            FUserPrefContrastNormal = UserPrefContrast;
            
            break;

        case FACTORY_COLOR_9300K:
            UserPrefColorTemp = CTEMP_Cool1;
            UserPrefRedColorCool1 = UserPrefRedColor;
            UserPrefGreenColorCool1 = UserPrefGreenColor;
            UserPrefBlueColorCool1 = UserPrefBlueColor;
            UserPrefBrightnessCool1 = UserPrefBrightness;
            UserPrefContrastCool1 = UserPrefContrast;
            FUserPrefBrightnessCool1 = UserPrefBrightness;
            FUserPrefContrastCool1 = UserPrefContrast;
            SaveFactorySetting();
            break;

        case FACTORY_COLOR_USER:
            break;

        case FACTORY_COLOR_SAVE:
            SaveFactorySetting();
            break;

        default:
            return FALSE;
            break;

    }
    return TRUE;
}

Bool appFactoryRecallColorPreset( drvFactoryColorTempType ucColorTemp )
{

    switch( ucColorTemp )
    {
        case FACTORY_COLOR_SRGB:
            
            UserPrefColorTemp = CTEMP_SRGB;
            UserPrefRedColor = UserPrefRedColorSRGB;
            UserPrefGreenColor = UserPrefGreenColorSRGB;
            UserPrefBlueColor = UserPrefBlueColorSRGB;
            ReadColorTempSetting(); // 2006/11/10 6:59PM by KK ReadSRGBSetting();
            mStar_AdjustBrightness( UserPrefBrightness );
            mStar_AdjustContrast( UserPrefContrast );
            break;

        case FACTORY_COLOR_5000K:

            break;

        case FACTORY_COLOR_6500K:
            
            UserPrefColorTemp = CTEMP_Warm1;
            UserPrefRedColor = UserPrefRedColorWarm1;
            UserPrefGreenColor = UserPrefGreenColorWarm1;
            UserPrefBlueColor = UserPrefBlueColorWarm1;
            ReadColorTempSetting(); // 2006/11/10 6:59PM by KK ReadWarm1Setting();
            mStar_AdjustBrightness( UserPrefBrightness );
            mStar_AdjustContrast( UserPrefContrast );

        case FACTORY_COLOR_7500K:
            
            UserPrefColorTemp = CTEMP_Normal;
            UserPrefRedColor = UserPrefRedColorNormal;
            UserPrefGreenColor = UserPrefGreenColorNormal;
            UserPrefBlueColor = UserPrefBlueColorNormal;
            ReadColorTempSetting(); // 2006/11/10 6:59PM by KK ReadNormalSetting();
            mStar_AdjustBrightness( UserPrefBrightness );
            mStar_AdjustContrast( UserPrefContrast );
            break;

        case FACTORY_COLOR_9300K:
            UserPrefColorTemp = CTEMP_Cool1;
            UserPrefRedColor = UserPrefRedColorCool1;
            UserPrefGreenColor = UserPrefGreenColorCool1;
            UserPrefBlueColor = UserPrefBlueColorCool1;
            ReadColorTempSetting(); // 2006/11/10 6:59PM by KK  ReadCool1Setting();
            mStar_AdjustBrightness( UserPrefBrightness );
            mStar_AdjustContrast( UserPrefContrast );
            break;

        case FACTORY_COLOR_USER:

            break;

        default:
            return FALSE;
            break;
    }

    return TRUE;
}

Bool appFactoryAdjLanguage( ucLanguage )
{
#if LANGUAGE_TYPE == ASIA
    if( ucLanguage == LanFrenchCode )
        UserPrefLanguage = LANG_France;
    else if( ucLanguage == LanTChineseCode )
        UserPrefLanguage = LANG_TChina;
    else if( ucLanguage == LanKoreanCode )
        UserPrefLanguage = LANG_Korea;
    else if( ucLanguage == LanSpanishCode )
        UserPrefLanguage = LANG_Spanish;
    else if( ucLanguage == LanPortugueseBrazilCode )
        UserPrefLanguage = LANG_Portugues;
    else if( ucLanguage == LanRussianCode )
        UserPrefLanguage = LANG_Russia;
    else if( ucLanguage == LanSChineseCode )
        UserPrefLanguage = LANG_SChina;
    else if( ucLanguage == LanEnglistCode )                     // default
        UserPrefLanguage = LANG_English;
    //For Select SChinese/TChinese/Korean Language Font Error Issue
    if( UserPrefLanguage == LANG_TChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Korea )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }
#elif LANGUAGE_TYPE == SIXTEEN            //090330 xiandi.yu
    if( ucLanguage == LanGermanCode )
        UserPrefLanguage = LANG_German;
    else if( ucLanguage == LanFrenchCode )
        UserPrefLanguage = LANG_France;
    else if( ucLanguage == LanItalianCode )
        UserPrefLanguage = LANG_Italian;
    else if( ucLanguage == LanSpanishCode )
        UserPrefLanguage = LANG_Spanish;
    else if( ucLanguage == LanSChineseCode )
        UserPrefLanguage = LANG_SChina;
    else if( ucLanguage == LanJapaneseCode )
        UserPrefLanguage = LANG_Japanese;
    else if( ucLanguage == LanRussianCode )
        UserPrefLanguage = LANG_Russia;
    else if( ucLanguage == LanSwedishCode )
        UserPrefLanguage = LANG_Swedish;
    else if( ucLanguage == LanPolishCode )
        UserPrefLanguage = LANG_Polish;
    else if( ucLanguage == LanPortugueseBrazilCode )
        UserPrefLanguage = LANG_Portugues;
    else if( ucLanguage == LanKoreanCode )
        UserPrefLanguage = LANG_Korea;
    else if( ucLanguage == LanEnglistCode )
        UserPrefLanguage = LANG_English;
    else if( ucLanguage == LanNorskCode )
        UserPrefLanguage = LANG_Norsk;
    else if( ucLanguage == LanFinnishCode )
        UserPrefLanguage = LANG_Finnish;
    else if( ucLanguage == LanDanskCode )
        UserPrefLanguage = LANG_Dansk;
    else if( ucLanguage == LanDutchCode )
        UserPrefLanguage = LANG_Dutch;

    //For Select SChinese/TChinese/Korean Language Font Error Issue
    if( UserPrefLanguage == LANG_SChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Japanese )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }

#elif LANGUAGE_TYPE == AllLanguage    //100325 xiandi.yu
    if( ucLanguage == LanTChineseCode )
        UserPrefLanguage = LANG_TChina;
    else if( ucLanguage == LanEnglistCode )
        UserPrefLanguage = LANG_English;
    else if( ucLanguage == LanFrenchCode )
        UserPrefLanguage = LANG_France;
    else if( ucLanguage == LanGermanCode )
        UserPrefLanguage = LANG_German;
    else if( ucLanguage == LanItalianCode )
        UserPrefLanguage = LANG_Italian;
    else if( ucLanguage == LanJapaneseCode )
        UserPrefLanguage = LANG_Japanese;
    else if( ucLanguage == LanKoreanCode )
        UserPrefLanguage = LANG_Korea;
    else if( ucLanguage == LanRussianCode )
        UserPrefLanguage = LANG_Russia;
    else if( ucLanguage == LanSpanishCode )
        UserPrefLanguage = LANG_Spanish;
    else if( ucLanguage == LanSwedishCode )
        UserPrefLanguage = LANG_Swedish;
    else if( ucLanguage == LanTurkishCode )
        UserPrefLanguage = LANG_Turkish;
    else if( ucLanguage == LanSChineseCode )
        UserPrefLanguage = LANG_SChina;
    else if( ucLanguage == LanPortugueseBrazilCode )
        UserPrefLanguage = LANG_Portugues;
    else if( ucLanguage == LanCzechCode )
        UserPrefLanguage = LANG_Czech;
    else if( ucLanguage == LanDutchCode )
        UserPrefLanguage = LANG_Dutch;
    else if( ucLanguage == LanFinnishCode )
        UserPrefLanguage = LANG_Finnish;
    else if( ucLanguage == LanPolishCode )
        UserPrefLanguage = LANG_Polish;
    else
        UserPrefLanguage = LANG_English;
#elif LANGUAGE_TYPE == EUROPE
    if( ucLanguage == LANG_Spanish )
        UserPrefLanguage = LANG_Spanish;
    else if( ucLanguage == LanFrenchCode )
        UserPrefLanguage = LANG_France;
    else if( ucLanguage == LanGermanCode )
        UserPrefLanguage = LANG_German;
    else if( ucLanguage == LanItalianCode )
        UserPrefLanguage = LANG_Italian;
    else if( ucLanguage == LanRussianCode )
        UserPrefLanguage = LANG_Russia;
    else if( ucLanguage == LanSwedishCode )
        UserPrefLanguage = LANG_Swedish;
    else if( ucLanguage == LanPolishCode )
        UserPrefLanguage = LANG_Polish;
    else if( ucLanguage == LanCzechCode )
        UserPrefLanguage = LANG_Czech;
    else if( ucLanguage == LanEnglistCode )
        // default
        UserPrefLanguage = LANG_English;
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( Language ), UserPrefLanguage );
#else
    //SaveMonitorSetting();
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif

    return TRUE;
}

Bool appFactoryAutoColor( void )
{
    Bool result = FALSE;
    if( mStar_AutoColor() )
    {
        if( CheckFactorySettingOutOfRange() )
        {
            Init_FactorySetting();
            result = FALSE;
        }
        else
            result = TRUE;

        SaveFactorySetting();
    }
    return result;

}

Bool appFactoryAutoSetup( void )
{
    Bool result = FALSE;
    if( AutoConfig() )
    {
        CheckModeSettingRange();
        result = TRUE;
    }
    else
        result = FALSE;
    
    SaveModeSetting();
    return result;
}

Bool appFactoryAllReset( void )
{
    Bool result = FALSE;
    if( ProductModeFlag )
    {
        result = ResetAllSetting();
        Delay1ms( 200 ); // for Factory check delay,It's can reduce
        Power_TurnOnAmberLed();
    }
    else
    {
        result = ResetAllSetting();
    }

    return result;
}

Bool appFactorySetFactoryFlag( drvFactoryModeType ucControl )
{
    if( ucControl )
    {
        Set_FactoryModeFlag();
    }
    else
    {
        Clr_FactoryModeFlag();
    }

    return TRUE;
}

Bool appFactorySetBurninFlag( drvFactoryModeType ucControl )
{
    if( ucControl )
    {
        Set_BurninModeFlag();
        Set_DoBurninModeFlag();
    }
    else
    {
        Clr_BurninModeFlag();
        Clr_DoBurninModeFlag();
    }

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#else
    //SaveMonitorSetting();
    Set_FlashForceSaveMonitorFlag();
#endif

    return TRUE;
}

Bool appFactorySetProductModeFlag( drvFactoryModeType ucControl )
{
    if( ucControl )
    {
        Set_ProductModeFlag();
        UserPrefVolume = 100; //PE Request
#if USEFLASH
        FactoryProductModeValue = ProductModeOnValue;
        //SaveFactorySetting();
        UserPref_EnableFlashSaveBit( bFlashSaveFactoryBit );
#else
        {
            WORD u16TempValue = ProductModeOnValue;
            NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &u16TempValue ), 2 );
        }
#endif
    }
    else
    {
        Clr_ProductModeFlag();
#if USEFLASH
        FactoryProductModeValue = ProductModeOffValue;
        //SaveFactorySetting();
        UserPref_EnableFlashSaveBit( bFlashSaveFactoryBit );
#else
        {
            WORD u16TempValue = ProductModeOffValue;
            NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &u16TempValue ), 2 );
        }
#endif
    }

    return TRUE;
}

#if PanelRSDS||PANEL_MINI_LVDS
Bool appFactoryAdjVcomValue( WORD wVcomValue )
{
    if( wVcomValue >= 0x7F )
        g_FactorySetting.VcomValue = 0x7F;
    else
        g_FactorySetting.VcomValue = wVcomValue & 0xFF;

    msWriteMiniLVDSi2c_Vcom( g_FactorySetting.VcomValue );

    NVRam_WriteTbl( nvrFactoryAddr( VcomValue ), g_FactorySetting.VcomValue );

    return TRUE;
}
#endif

Bool appFactorySetForcePowerSaving( void )
{
    Clr_ForcePowerSavingFlag(); // clear force power saving flag

    if( !PowerSavingFlag )
    {
        //SET_DFM_POWERSAVING_FLAG();
        PowerOffSystem();
        MenuPageIndex = StandbyMenu;
        MenuItemIndex = 0;
    }

#if ENABLE_NEWDFM_REQUEST
    DDC2BI_REPLYBYTE = 0x02;
#endif

    return TRUE;

}

#if HDCPKEY_IN_Flash
WORD appFactoryGetHdcpCRC( void )
{
    return TCRCvalue;
}

Bool appFactorySetHdcpWriteStart( void )
{
    Set_WriteHDCPcodeFlag();
    TCRCCounter = 0;
    TCRCvalue = 0;
    hw_ClrAmberLed();
    hw_ClrGreenLed();
    //bFlashWriteFactory = TRUE;
    //Flash_Erase_FreeBuffer(FLASH_KEY_HDCP,0); // jeff add it in 2009 0907
    //FlashSectorErase( TRUE, HDCPKEYSET_START );
    //FlashDisableWP( HDCPKEYSET_START );
    
    return TRUE;
}

void appFactoryHdcpWrite( WORD wIndex, WORD wBufSize, BYTE* buf, BYTE ucLen )
{
#if 1
    BYTE XDATA ucCnt=0;
    #if USEFLASH
    Flash_Write_Factory_KeySet( FLASH_KEY_HDCP, wIndex, wBufSize, buf, ucLen );
    #else
    wIndex = wBufSize = 0; // for eliminating warning messages
    buf = 0; // for eliminating warning messages
    #endif
    #if ENABLE_NEWDFM_REQUEST
    g_wFDCRCValue = 0;
    #endif    
    
    for( ucCnt = 0; ucCnt < ucLen; ucCnt++ )
    {
        if( TCRCCounter < HDCPKEY_SIZE )
        {
            #if USEFLASH            
            TCRCvalue = CRC16( FlashReadByte( g_wKEYSET_START + TCRCCounter ), TCRCvalue ); 
            #if ENABLE_NEWDFM_REQUEST
            g_wFDCRCValue = CRC16( FlashReadByte( g_wKEYSET_START + TCRCCounter ), g_wFDCRCValue );
            #endif
            #endif            
        }

        TCRCCounter++;

        if( TCRCCounter == HDCPKEY_SIZE )
        {
            #if USEFLASH
            //Flash_WriteTbl(TRUE, FDATA_HDCPCRC_ADDR, (BYTE *)&TCRCvalue, 2);    //Write_CRCValue();
            //g_ucKeyIndex = Flash_GetKeyIndex( FLASH_KEY_HDCP, FALSE );
            //Flash_WriteTbl( TRUE, HDCPKEY_CRC_H_ADDR + g_ucKeyIndex * 2, ( BYTE * )&TCRCvalue, 2 );
            #else
            //appNVRamWriteTbl(FDATA_HDCPCRC_ADDR, (BYTE *)&TCRCvalue, 2);
            #endif
            TCRCCounter = 0;
            Set_LoadHDCPKeyFlag();
            bFlashWriteFactory = FALSE;
            hw_SetGreenLed();
            FlashEnableWP();
        }
    }
#else
    BYTE xdata i;
    WORD xdata wAddr;

    for( i = 0; i < ucLen; i++ )
    {
        //SIO_TYPE2==HDCP BANK NO    SIORxBuffer[3+i]==HDCP DATA
        wAddr = (WORD)wIndex*wBufSize  + i;                    
        if( wAddr < HDCPKEY_SIZE )
        {
            FlashHDCPWriteByte(( HDCPKEYSET_START + wAddr ), *(buf+i));
            TCRCvalue = CRC16(FlashReadByte(HDCPKEYSET_START+wBufSize*wIndex+i), TCRCvalue);
        }
    }

    if( SIO_TYPE2 == HDCPKEY_SIZE / HDCP_BLOCK_LENGTH ) //HDCP WRITE END
    {                   
        Set_LoadHDCPKeyFlag();
        bFlashWriteFactory = FALSE;
        FlashEnableWP();
        hw_SetGreenLed();
    }
#endif
}
#endif

#ifdef UseVGACableReadWriteAllPortsEDID
#if ENABLE_NEWDFM_REQUEST
WORD appFactorySetEdidWriteStart( drvFactoryModeType ucControl )
{
    BYTE xdata ucInputEDID, ucStatus;
    //Monitor Feedback: 6f 6e 83 02 EDIDPort Status chksum
    //Return Input Types Information
#if INPUT_TYPE==INPUT_1A
    ucInputEDID = VGA1_EDID;
#elif INPUT_TYPE==INPUT_1A1D
    ucInputEDID  = VGA1_EDID | DVI1_EDID;
#elif INPUT_TYPE==INPUT_1A1H
    ucInputEDID  = VGA1_EDID | HDMI1_EDID;
#elif INPUT_TYPE==INPUT_1A1D1DP
    ucInputEDID  = VGA1_EDID | DVI1_EDID | DP1_EDID;
#elif (INPUT_TYPE==INPUT_1A1H1DP ||INPUT_TYPE == INPUT_1A1D1H1DP)
    ucInputEDID  = VGA1_EDID | HDMI1_EDID | DP1_EDID;
#elif INPUT_TYPE==INPUT_1A1D1H
    ucInputEDID  = VGA1_EDID | DVI1_EDID | HDMI1_EDID;
#elif INPUT_TYPE==INPUT_1A2D
    ucInputEDID  = VGA1_EDID | DVI1_EDID | DVI2_EDID;
#elif INPUT_TYPE==INPUT_1A2H
    ucInputEDID  = VGA1_EDID | HDMI1_EDID | HDMI2_EDID;
#else
    ucInputEDID  = 0x00;
#endif

    //BIT0=0,EDID Saved in EEPRom;BIT0=1,EDID Saved in Flash
    //BIT1:(0:Write Disable;1:Write Enable)
    if( ucControl == 0x01 )
    {
        hw_ClrDDC_WP();//EDID In Flash should not disable ddc ram.
        Set_WriteDeviceFlag();
        ucStatus = BIT1;
    }
    else
    {
#ifdef UseInternalDDCRam
#ifdef DVI_EDID_OnlyINEEPROM
        hw_SetDDC_WP();
#endif
        mStar_SetDDC_WP();
        Set_LoadEDIDFlag();//Load EDID to DDC RAM From Flash
#else
        hw_SetDDC_WP();
#endif
        Clr_WriteDeviceFlag();
        ucStatus = 0;
    }
    return (( WORD )ucInputEDID << 8 | ucStatus );
}

void appFactoryWriteEDID( drvFactoryDeviceType ucDevice, WORD wAddr, BYTE* buf, BYTE ucLen )
{
    BYTE xdata ucType;
#if defined(UseInternalDDCRam)//EDID&HDCP Key In Flash
    if( ucDevice == VGA1 )
        ucType = FLASH_KEY_DDCA;
    else if( ucDevice == DVI1 )
        ucType = FLASH_KEY_DDCD;
    else if( ucDevice == HDMI1 )
        ucType = FLASH_KEY_DDCH;
    else if( ucDevice == HDCP1 )
        ucType = FLASH_KEY_HDCP;
    else
        return;
    if( ucType == FLASH_KEY_HDCP )
    {
        Flash_WriteTbl( TRUE, HDCPKEYSET_START + wAddr, buf, ucLen );
    }
    else
    {
        Flash_Write_Factory_KeySet( ucType, wAddr / ucLen, ucLen, buf, ucLen ); //it takes about 20ms for flash to write 32 Bytes.(PMC 7ms,SST 9ms,EON 18ms,MX2026 19ms,MX2025 12ms)
    }
#else//EDID In EEPROM,But HDCP Key In Flash
    if( ucDevice == HDCP1 )
    {
        ucType = FLASH_KEY_HDCP;
        Flash_WriteTbl( TRUE, HDCPKEYSET_START + wAddr, buf, ucLen );
    }
    else if( ucDevice == DP1 || ucDevice == DP2)    ////111005 Rick add write DP edid condition - A020
    {
        ucType = FLASH_KEY_DDCDP;
        Flash_Write_Factory_KeySet( ucType, wAddr / ucLen, ucLen, buf, ucLen ); //it takes about 20ms for flash to write 32 Bytes.(PMC 7ms,SST 9ms,EON 18ms,MX2026 19ms,MX2025 12ms)
    }
    else
    {
        LoadEDIDSelectPort = ucDevice;//Select Current IIC Port
        i2c_WriteTBL( 0xA0, wAddr, buf, ucLen ); //it takes about 45ms for EEPROM to write 32 Bytes.
        LoadEDIDSelectPort = 0xFF;//Set IIC Port To UserData EEPRom
    }
    hw_SetAmberLed();
    hw_SetGreenLed();
#endif
}

void appFactoryGetEDID( drvFactoryDeviceType ucDevice, WORD wAddr, BYTE* buf, BYTE ucLen )
{

#if 1
    WORD StartAddr;
#if defined(UseInternalDDCRam)//EDID&HDCP Key In Flash
    if( ucDevice == VGA1 )
        StartAddr = DDCAKEYSET_START;
    else if( ucDevice == DVI1 )
        StartAddr = DDCDKEYSET_START;
    else if( ucDevice == HDMI1 )
        StartAddr = DDCHKEYSET_START;
    else if( ucDevice == HDCP1 )
        StartAddr = HDCPKEYSET_START;
    else
        return;
    Flash_ReadTbl(( StartAddr + wAddr ), buf, ucLen );
#else//EDID In EEPROM,But HDCP Key In Flash
    if( ucDevice == HDCP1 )
    {
        StartAddr = HDCPKEYSET_START;
        Flash_ReadTbl(( StartAddr + wAddr ), buf, ucLen );
    }
    else if( ucDevice == DP1 || ucDevice == DP2)        //111005 Rick add write DP edid condition - A020
    {
        //StartAddr = DDCDPKEYSET_START;
        //Flash_ReadTbl(( StartAddr + wAddr ), buf, ucLen );
        Flash_Read_Factory_KeySet( FLASH_KEY_DDCDP, TRUE, (wAddr / ucLen), ucLen, buf, ucLen );
    }
    else
    {
        LoadEDIDSelectPort = ucDevice;//Select Current IIC Port
        i2c_ReadTBL( 0xA0, wAddr, buf, ucLen ); //Read Data From EEProm
        LoadEDIDSelectPort = 0xFF;//Set IIC Port To UserData EEPRom
    }
#endif

#else
    BYTE xdata i;
    if(( ucDevice >= VGA1 && ucDevice <= DP2 ) || ( ucDevice >= EEPROM && ucDevice <= HDCP2 ) )
    {
        ReadDevice( ucDevice, wAddr, ucLen ); //Read EDID or HDCP KEY
        for( i = 0; i < ucLen; i++ )
        {
            *(buf+1) = SIORxBuffer[i];
        }
    }
    else
    {
        ;
    }
#endif
}
#endif
#endif
#endif
