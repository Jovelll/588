#include <stddef.h>
#include "board.h"
#ifndef _UserPrefDef_H_
#define _UserPrefDef_H_

#define ProductModeOnValue 0x55AA
#define ProductModeOffValue 0xBB99

#define UseDefaultPanelValue    0x55AA
#define UseUserPanelValue   0xBB99
/*
typedef enum
{
    VGA_EDID,
    DVI_EDID
} EDIDType;
*/  //mark for 24c02 write edid
#ifdef DATA_LENGTH_20_BYTE
#define HDCP_BLOCK_LENGTH   20
#else
#define HDCP_BLOCK_LENGTH   5
#endif
typedef enum
{
    InputColor_RGB=0,
    InputColor_YPBPR_444,
    InputColor_YPBPR_422,
    InputColor_Max
} InputColorType;


typedef enum
{
    YUV_Colorimetry_ITU601=0,
    YUV_Colorimetry_ITU709,
    YUV_Colorimetry_NoData,
    YUV_Colorimetry_Max
} InputYPBPRColorimetryType;


enum
{
    OVERSCAN_FULL,      	//
    OVERSCAN_4_3,           // overscan adjust by panel H/V 4:3 + Ext H/V
    OVERSCAN_16_9,          // overscan adjust by panel H/V 16:9 + Ext H/V
    OVERSCAN_1_1,   		// display one to one with frame buffer mode only
    OVERSCAN_FIXED,         // no overscan, fixed as input image ratio
    OVERSCAN_FIXED_RATIO,   // fixed cutting ratio of input image
};

typedef struct
{
    BYTE Enable;
    BYTE AspRatio;
    BYTE ScanRatio;         // 0 ~ 100
    BYTE ImageRatio;        // 0 ~ 100
    int ExtH;
    int ExtV;
    WORD OverScanH;
    WORD OverScanV;
    WORD FinalCapWinHST;
    WORD FinalCapWinVST;
    WORD FinalCapWinWidth;
    WORD FinalCapWinHeight;
    BYTE AspRatioEnableFlag;
} OverScanType;

#if Enable_Expansion
#define bAspectRatio4_3_Flag           			BIT0
#define bAspectRatio16_9_Flag					BIT1
#define bAspectRatio1_1_Flag                    BIT2
#define bAspectRatioFixedInputRatio_Flag        BIT3
#define AspRatio4_3_EnisableFlag                (OverScanSetting.AspRatioEnableFlag&bAspectRatio4_3_Flag)
#define AspRatio16_9_EnisableFlag               (OverScanSetting.AspRatioEnableFlag&bAspectRatio16_9_Flag)
#define AspRatio1_1_EnisableFlag                (OverScanSetting.AspRatioEnableFlag&bAspectRatio1_1_Flag)
#define AspRatioFixedInputRatioEnisableFlag     (OverScanSetting.AspRatioEnableFlag&bAspectRatioFixedInputRatio_Flag)
#define Set_Asp4_3EnableFlag()      			(OverScanSetting.AspRatioEnableFlag|=bAspectRatio4_3_Flag )
#define Clr_Asp4_3EnableFlag()      			(OverScanSetting.AspRatioEnableFlag&=~bAspectRatio4_3_Flag  )
#define Set_Asp16_9EnableFlag()      			(OverScanSetting.AspRatioEnableFlag|=bAspectRatio16_9_Flag )
#define Clr_Asp16_9EnableFlag()      			(OverScanSetting.AspRatioEnableFlag&=~bAspectRatio16_9_Flag  )
#define Set_Asp1_1EnableFlag()      			(OverScanSetting.AspRatioEnableFlag|=bAspectRatio1_1_Flag )
#define Clr_Asp1_1EnableFlag()      			(OverScanSetting.AspRatioEnableFlag&=~bAspectRatio1_1_Flag  )
#define Set_AspFixedInputRatioEnableFlag()      (OverScanSetting.AspRatioEnableFlag|=bAspectRatioFixedInputRatio_Flag )
#define Clr_AspFixedInputRatioEnableFlag()      (OverScanSetting.AspRatioEnableFlag&=~bAspectRatioFixedInputRatio_Flag  )
#endif

typedef struct
{
    //BYTE Ident;
    WORD CheckSum;
    //BYTE DitherTpye;
    BYTE SettingFlag;//4//color depth,panel Vdd etc.
    BYTE OnTiming1;     //4//time between panel & data while turn on power
    WORD OnTiming2;     //4//time between data & back light while turn on power
    WORD OffTiming1;    //4//time between back light & data while turn off power
    BYTE OffTiming2;        //4//time between data & panel while turn off power

    WORD HsyncWidth;
    WORD HSyncBackPorch;
    WORD VSyncWidth;
    WORD VsyncBackPorch;
    WORD Width;
    WORD Height;
    WORD HTotal;
    WORD VTotal;
    WORD MaxHTotal;
    WORD MinHTotal;
    WORD MaxVTotal;
    WORD MinVTotal;

    BYTE TypicalDCLK; //2011.9.16 11:14 cc enable
    BYTE MaxDCLK;
    BYTE MinDCLK;
    BYTE Swing; //2011.9.15 21:40 cc add

    BYTE OutputControl1; //4//Scaler Bank 0x42
    BYTE OutputControl2; //4//Scaler Bank 0x43 //2011.9.16 11:14 cc enable
    BYTE OSContol; //4//output signal control       Scaler Bank 0xF2
    BYTE ODRriving; //4//Output DE Driving      Scaler Bank 0xF5

    WORD BurstModeFreq;
    BYTE DefDuty;
    BYTE MinDuty;
    BYTE MaxDuty;
    //4//ssc setting
    BYTE SSCSTEPL;//0xD8
    BYTE SSCSTEPH;//0xD9
    BYTE SSCSPANL;//0xDA
    BYTE SSCSPANH;//0xDB
#if 1//USEFLASH
    WORD PanelTypeNum;
#endif
    //BYTE CheckSum;
} PanelSettingType;
#define PanelSettingSize        sizeof(PanelSettingType)
#define PanelSettingChecksumSize        sizeof(PanelSetting.CheckSum)

typedef struct
{
    BYTE Ident;
    DWORD Backlighttime;
#if ENABLE_BURNIN_MENU
      DWORD Burnintime;	
#endif
    BYTE CheckSum;
} MonitorSettingType2;
#define MonitorSetting2Size sizeof(MonitorSettingType2)
#if USEFLASH

#define MonitorSettingSize  sizeof(MonitorSettingType)
#define FactorySettingSize  sizeof(FactorySettingType)
#define ModeSettingSize     sizeof(ModeSettingType)


typedef struct
{
    BYTE Ident;
    WORD MonitorFlag; // 0
    BYTE Brightness; // 1
    BYTE Contrast; //8
    BYTE RedColor; //a
    BYTE GreenColor; //b
    BYTE BlueColor; //c
    BYTE ColorTemp; //9
    BYTE BrightnessUser; // 1
    BYTE ContrastUser; //8
    BYTE BrightnessTemp;
    BYTE ContrastTemp;
    BYTE BrightnessWarm1;
    BYTE ContrastWarm1;
    BYTE BrightnessNormal;
    BYTE ContrastNormal;
    BYTE BrightnessCool1;
    BYTE ContrastCool1;
    BYTE BrightnesssRGB;
    BYTE ContrastsRGB;
    BYTE RedColorUser; //d
    BYTE GreenColorUser; //e
    BYTE BlueColorUser; //f
#if (AudioFunc && (ENABLE_HDMI || ENABLE_DP_INPUT))
    BYTE AudioSource;
#endif
#if AudioIC_CS8003_Enable
  #if EarphoneDet_Enable  
	  BYTE EarVolume;
  #endif
#endif
#if Audio_Switch
		BYTE HDMIAudioType;
#endif
#if LowBlueLight
		 BYTE LowBlueMode;
  #endif
    BYTE Volume;
    BYTE MuteOnFlag;
    BYTE ECOMode;
    BYTE Language; //14
    BYTE OsdHStart; //15
    BYTE OsdVStart; //16
    BYTE OsdTime; //17
    BYTE OsdTransparency;
#if ENABLE_OSD_ROTATION
    BYTE OSDRotateMode; //17
#endif
#if ENABLE_FREESYNC
    BYTE FreeSyncMode;    //Scar
#endif
    BYTE BankNo; //18
    BYTE Addr; //19
    BYTE Value; //1a
#if Enable_Gamma
    BYTE GamaMode;
    BYTE GamaOnOff;
#endif
    BYTE DcrMode;

    BYTE Hue;
    BYTE Saturation;

#if MWEFunction
    BYTE SubContrast;
    BYTE SubBrightness;
    BYTE ColorFlag;
    BYTE BF_Size;
    BYTE BF_Hstart;
    BYTE BF_Vstart;
    WORD BF_HSize;
    WORD BF_VSize;
#endif

    BYTE InputType; //1b
    BYTE InputSelectType; //1b
    // 2006/10/24 7:17PM by Emily   BYTE BlackLevel; // 2 //1125
    BYTE RedBlackLevel;
    BYTE GreenBlackLevel;
    BYTE BlueBlackLevel;
    BYTE SavedModeIndex;
#if 1//Enable_Expansion
    BYTE ExpansionMode;
#endif
#if ENABLE_DLC
    BYTE DLCMode;
#endif

#if ENABLE_DSC
    BYTE DSCMode;
#endif
#if ENABLE_DPS
    BYTE DPSMode;
#endif
#if ENABLE_RTE
    BYTE OverDriveMode;
    BYTE OverDriveWeighting;
#endif
#if ENABLE_SUPER_RESOLUTION
    BYTE SuperResolutionMode;
#endif
#if ENABLE_SHARPNESS
    BYTE Sharpness;
#endif
#if ENABLE_DeBlocking
    BYTE DeBlocking;
#endif
#if ENABLE_TNR
    BYTE NRmode;
#endif
#if ENABLE_3DLUT
    BYTE RGB3DLUTMode;
#endif
    BYTE InputColorFormat;

    //======for AutoBacklight================
    BYTE Histogram1;
    BYTE Histogram2;
    BYTE ALhaA;
    BYTE BataA;
#if ENABLE_R2_2DTO3D
    BYTE T3DMode;
    BYTE T3DGameMode;
    BYTE T3DGain;
    BYTE T3DOffset;
#endif
    BYTE CheckSum; //1c
} MonitorSettingType;

typedef struct
{
    BYTE Ident;

    BYTE AdcPgaGain;
    WORD AdcRedGain; // 2
    WORD AdcGreenGain; //3
    WORD AdcBlueGain; //4
    WORD AdcRedOffset; // 5
    WORD AdcGreenOffset; //6
    WORD AdcBlueOffset; //7


    BYTE FBrightnessWarm1;
    BYTE FContrastWarm1;
    BYTE RedColorWarm1; //d
    BYTE GreenColorWarm1; //e
    BYTE BlueColorWarm1; //f

    BYTE FBrightnessNormal;
    BYTE FContrastNormal;
    BYTE RedColorNormal; //10
    BYTE GreenColorNormal; //11
    BYTE BlueColorNormal; //12

    BYTE FBrightnessCool1;
    BYTE FContrastCool1;
    BYTE RedColorCool1; //10
    BYTE GreenColorCool1; //11
    BYTE BlueColorCool1; //12

    BYTE FBrightnesssRGB;
    BYTE FContrastsRGB;
    BYTE RedColorsRGB; //10
    BYTE GreenColorsRGB; //11
    BYTE BlueColorsRGB; //12
    BYTE LogoON;
    WORD ProductModeValue;
#if PanelRSDS
    BYTE VcomValue;
#endif

#if ENABLE_FACTORY_SSCADJ // 2011-09-13 CC
    BYTE SSCModulation;
    BYTE SSCPercentage;
#endif
    BYTE CheckSum;
} FactorySettingType;

typedef struct
{
    BYTE Ident;
    BYTE Index;
    WORD HTotal;
    WORD HStart;
    WORD VStart;
    WORD AutoHStart;
    WORD AutoVStart;  //2004/01/15 num11
    BYTE Phase;
    BYTE AutoTimes;

    BYTE ModeIndex;
    WORD HorFreq;   //ModeInfoType
    WORD VerFreq;
    BYTE Flags;
    BYTE CheckSum;
} ModeSettingType;

#define NumberOfMode        18
#define UserModeNumber      10

#if ENABLE_DeltaE
typedef struct
{
   BYTE CM[18];
   BYTE CalculateStatus; // record eeprom status, if done, this bit will be set.
} CSTransferSettingType;

#define ColorMatrixSize     sizeof(CSTransferSettingType)
#define GammaTblSize        64
#define DicomGammaTblSize   76

#define RM_COLORSPACE_TRANS_BASEADDR    (0x7C000+ColorMatrixSize)
#define RM_COLORSPACE_TRANS_BASEADDR_SRGB   RM_COLORSPACE_TRANS_BASEADDR
#define RM_COLORSPACE_TRANS_BASEADDR_Adobe  (RM_COLORSPACE_TRANS_BASEADDR_SRGB+ColorMatrixSize)
#define BGammaTblAddr   (RM_COLORSPACE_TRANS_BASEADDR + ColorMatrixSize * 2)
#define BGammaTblEndAddr    (BGammaTblAddr + GammaTblSize*3)
#define BGammaCheckSumStartAddr (BGammaTblEndAddr+1)
#define DicomGammaTblAddr   (BGammaCheckSumStartAddr)
#define DicomGammaTblEndAddr    (DicomGammaTblAddr + DicomGammaTblSize*3)

#endif

#else

typedef struct
{
    WORD MonitorFlag; // 0
    BYTE Brightness; // 1
    BYTE Contrast; //8
    BYTE RedColor; //a
    BYTE GreenColor; //b
    BYTE BlueColor; //c
    BYTE ColorTemp; //9
    BYTE BrightnessUser; // 1
    BYTE ContrastUser; //8
    BYTE BrightnessTemp;
    BYTE ContrastTemp;
    BYTE BrightnessWarm1;
    BYTE ContrastWarm1;
    BYTE BrightnessNormal;
    BYTE ContrastNormal;
    BYTE BrightnessCool1;
    BYTE ContrastCool1;
    BYTE BrightnesssRGB;
    BYTE ContrastsRGB;
    BYTE RedColorUser; //d
    BYTE GreenColorUser; //e
    BYTE BlueColorUser; //f
#if (AudioFunc && (ENABLE_HDMI || ENABLE_DP_INPUT))
        BYTE AudioSource;
#endif
#if AudioIC_CS8003_Enable
  #if EarphoneDet_Enable  
		  BYTE EarVolume;
  #endif
#endif
#if Audio_Switch
	BYTE HDMIAudioType;
#endif
    BYTE Volume;
    BYTE MuteOnFlag;
    BYTE ECOMode;
    BYTE Language; //14
    BYTE OsdHStart; //15
    BYTE OsdVStart; //16
    BYTE OsdTime; //17
    BYTE OsdTransparency;
#if ENABLE_OSD_ROTATION
    BYTE OSDRotateMode; //17
#endif
#if ENABLE_FREESYNC
    BYTE FreeSyncMode;    //Scar
#endif
    BYTE BankNo; //18
    BYTE Addr; //19
    BYTE Value; //1a
#if Enable_Gamma
    BYTE GamaMode;
    BYTE GamaOnOff;
#endif
    BYTE DcrMode;

    BYTE Hue;
    BYTE Saturation;

#if MWEFunction
    BYTE SubContrast;
    BYTE SubBrightness;
    BYTE ColorFlag;
    BYTE BF_Size;
    BYTE BF_Hstart;
    BYTE BF_Vstart;
    WORD BF_HSize;
    WORD BF_VSize;
#endif

    BYTE InputType; //1b
    BYTE InputSelectType; //1b
    // 2006/10/24 7:17PM by Emily   BYTE BlackLevel; // 2 //1125
    BYTE RedBlackLevel;
    BYTE GreenBlackLevel;
    BYTE BlueBlackLevel;
    BYTE SavedModeIndex;
#if 1//Enable_Expansion
    BYTE ExpansionMode;
#endif
#if ENABLE_DLC
    BYTE DLCMode;
#endif
#if ENABLE_DSC
    BYTE DSCMode;
#endif
#if ENABLE_DPS
    BYTE DPSMode;
#endif
#if ENABLE_RTE
    BYTE OverDriveMode;
    BYTE OverDriveWeighting;
#endif

#if ENABLE_SUPER_RESOLUTION
    BYTE SuperResolutionMode;
#endif
#if ENABLE_SHARPNESS
    BYTE Sharpness;
#endif
#if ENABLE_DeBlocking
    BYTE DeBlocking;
#endif
#if ENABLE_TNR
    BYTE NRmode;
#endif
#if ENABLE_3DLUT
    BYTE RGB3DLUTMode;
#endif
    BYTE InputColorFormat;

    //======for AutoBacklight================
    BYTE Histogram1;
    BYTE Histogram2;
    BYTE ALhaA;
    BYTE BataA;
    DWORD Backlighttime;
#if ENABLE_OSD_3D_CONTROL
    BYTE T3DMode;
    BYTE T3DGameMode;
    BYTE T3DGain;
    BYTE T3DOffset;
#endif
    BYTE CheckSum; //1c
} MonitorSettingType;

typedef struct
{
  BYTE AdcPgaGain;
  WORD AdcRedGain;
  WORD AdcGreenGain;
  WORD AdcBlueGain;
  WORD AdcRedOffset;
  WORD AdcGreenOffset;
  WORD AdcBlueOffset;

    BYTE FBrightnessWarm1;
    BYTE FContrastWarm1;
    BYTE RedColorWarm1; //d
    BYTE GreenColorWarm1; //e
    BYTE BlueColorWarm1; //f

    BYTE FBrightnessNormal;
    BYTE FContrastNormal;
    BYTE RedColorNormal; //10
    BYTE GreenColorNormal; //11
    BYTE BlueColorNormal; //12

    BYTE FBrightnessCool1;
    BYTE FContrastCool1;
    BYTE RedColorCool1; //10
    BYTE GreenColorCool1; //11
    BYTE BlueColorCool1; //12

    BYTE FBrightnesssRGB;
    BYTE FContrastsRGB;
    BYTE RedColorsRGB; //10
    BYTE GreenColorsRGB; //11
    BYTE BlueColorsRGB; //12
    BYTE LogoON;
#if PanelRSDS
    BYTE VcomValue;
#endif

#if ENABLE_FACTORY_SSCADJ // 2011-09-13 CC
    BYTE SSCModulation;
    BYTE SSCPercentage;
#endif
    BYTE CheckSum; //12

} FactorySettingType;
typedef struct
{
    WORD HTotal;
    WORD HStart;
    WORD VStart;
    WORD AutoHStart;
    WORD AutoVStart;
    BYTE Phase;
    BYTE AutoTimes;
    BYTE CheckSum;
} ModeSettingType;
typedef struct
{
    WORD HerFreq;
    WORD VerFreq;
    WORD VTotal;
    BYTE Flags;
} ModeInfoType;
#define BoardChecksum   0xAA

//==========================================================================
#if ENABLE_DeltaE
typedef struct
{
   BYTE CM[18];
   BYTE CalculateStatus; // record eeprom status, if done, this bit will be set.
} CSTransferSettingType;

#define ColorMatrixSize     sizeof(CSTransferSettingType)
#define GammaTblSize        64
#define DicomGammaTblSize   76
#endif

//==========================================================================
//==========================================================================
#ifdef EEPRom_24C04
#define NumberOfMode        15//2006-07-07 Andy //2006-0626 Andy 55 //50  //wmz 20051012 for WidePanel
#else
#define NumberOfMode        50
#endif
#define UserModeNumber      10
// NVRAM address configuration
#define NVRamAddress_Base   0
#define FactorySettingSize  sizeof(FactorySettingType)
#define MonitorSettingSize  sizeof(MonitorSettingType)
#define ModeSettingSize     sizeof(ModeSettingType)
#define ModeInfoSize        sizeof(ModeInfoType)
#if 1//ENABLE_HDCP
#define HDCP_KEY_ROMSIZE      304
#define HDCP_KEY_SIZE1        5       // HDCP key BKSV
#define HDCP_KEY_SIZE2        284     // HDCP key
#define HDCPAddress_Base    0
#endif
typedef enum
{

    ProductModeAddr = NVRamAddress_Base,
    FactorySettingAddr = ProductModeAddr + 2,
    MonitorSettingAddr = FactorySettingAddr + FactorySettingSize,
    ModeSettingAddr = MonitorSettingAddr + MonitorSettingSize,

    ModeInfoAddr = ModeSettingAddr + ModeSettingSize * NumberOfMode,
    EndAddr = ModeInfoAddr + ModeInfoSize * NumberOfMode,
#ifdef EEPRom_24C04
    BlacklitTimeAddr    = 0x1E0,
    CheckBoardAddr = 0x1F0
#else
BlacklitTimeAddr    = 0x7E0,
CheckBoardAddr = 0x7F0,
#endif
#if ENABLE_DeltaE
    RM_COLORSPACE_TRANS_BASEADDR = 0x600,//FACTORY_SETTING_ADDR + FACTORY_SETTING_SIZE,//0x320,//
    RM_COLORSPACE_TRANS_BASEADDR_SRGB=RM_COLORSPACE_TRANS_BASEADDR,
    RM_COLORSPACE_TRANS_BASEADDR_Adobe=RM_COLORSPACE_TRANS_BASEADDR_SRGB+ColorMatrixSize,
    BGammaTblAddr = RM_COLORSPACE_TRANS_BASEADDR + ColorMatrixSize * 2,
    BGammaTblEndAddr = BGammaTblAddr + GammaTblSize,
    BGammaCheckSumStartAddr=BGammaTblEndAddr+1,
    DicomGammaTblAddr =BGammaCheckSumStartAddr, // Dicom gamma address
    DicomGammaTblEndAddr = DicomGammaTblAddr + DicomGammaTblSize*3,
#endif

} NVRamAddress;
typedef enum
{
    RM_HDCP_KEY_BASEADDR1  = HDCPAddress_Base,
    RM_HDCP_KEY_BASEADDR2  = HDCPAddress_Base + HDCP_KEY_SIZE1
} NVRamAddress1;
//#define adrMonitorSetting_Base        NVRamAddress_Base
//#define adrModeSetting_Base       NVRamAddress_Base+MonitorSettingSize
#define nvrFactoryAddr(member)      FactorySettingAddr+offsetof(FactorySettingType, member)
#define nvrMonitorAddr(member)      MonitorSettingAddr+offsetof(MonitorSettingType, member)
#define nvrModeBaseAddr(modeIndex)  ModeSettingAddr+(WORD)modeIndex*ModeSettingSize
#define nvrModeAddr(member)     nvrModeBaseAddr(SaveIndex)+offsetof(ModeSettingType, member)
#define nvrModeInfoAddr(modeIndex)  ModeInfoAddr+(WORD)modeIndex*ModeInfoSize
#endif
#endif
