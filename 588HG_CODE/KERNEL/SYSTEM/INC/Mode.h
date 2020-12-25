//#include "board.h"
#ifndef _Mode_H_
#define _Mode_H_

#ifdef _MODE_C_
#define _MODEDEC_
#else
#define _MODEDEC_ extern
#endif


#define bVSyncNegative          BIT0
#define bHSyncNegative          BIT1
#define bHSyncLoss              BIT2
#define bVSyncLoss              BIT3
#define bInterlaceMode          BIT4
#define bUnsupportMode          BIT5
#define bUserMode               BIT6
#define bNativeMode         BIT7
#define bShrinkMode         BIT7



#define HPeriod_Torlance                            (CURRENT_INPUT_IS_DISPLAYPORT()?80:10) // for mode change
#define VTotal_Torlance                                4 // 10 // for mode change
#define VTotal_Delta                                    50
#define HFreq_Torlance                              12 // 15 // 1.5 KHz
#define VFreq_Torlance                              12 // 15 // 1.5 Hz
#define HFREQ_TOLERANCE_ENLARGE      150 // 15KHz
#define VFREQ_TOLERANCE_ENLARGE      100 // 10Hz


#define _3D_TIMING       0// 1
enum
{
    IP_MUX_ADC,
    IP_MUX_DVI_A,
    IP_MUX_DVI_B,
    IP_MUX_DP_A,
    IP_MUX_DP_B,
    IP_MUX_RESERVE_D,
    IP_MUX_RESERVE_E,
    IP_MUX_RESERVE_F,
    IP_MUX_RESERVE_G,
    IP_MUX_RESERVE_H,
    IP_MUX_RESERVE_I,
    IP_MUX_RESERVE_J,
};

enum
{
    CLK_ADC_BUFF_DIV2,
    CLK_DVI_BUFF_DIV2,
    DP_STRM_CLK,
    CLK_DVI_BUFF,

};

#if INPUT_TYPE == INPUT_1A1D
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Digital2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Digital,
    Input_HDMI = Input_Nothing,
    Input_Displayport = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Nothing,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;
#elif INPUT_TYPE == INPUT_1A2D
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Digital2,    
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Digital,
    Input_HDMI = Input_Nothing,
    Input_Displayport = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Nothing,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;
#elif INPUT_TYPE == INPUT_1A1H
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Digital2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Nothing,
    Input_HDMI = Input_Digital,
    Input_Displayport = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Nothing,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;
#elif INPUT_TYPE == INPUT_1A2H
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Digital2,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Nothing,
    Input_HDMI = Input_Digital,
    Input_Displayport = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Digital2,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;
#elif INPUT_TYPE == INPUT_1A1D1H
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Digital2,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Digital,
    Input_HDMI = Input_Nothing,
    Input_Displayport = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Digital2,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;

#elif  INPUT_TYPE ==INPUT_1A1D1DP
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Displayport,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Digital2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Digital,
    Input_HDMI = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Nothing,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;
#elif  INPUT_TYPE ==INPUT_1A1H1DP
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Displayport,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Digital2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Nothing,
    Input_HDMI = Input_Digital,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Nothing,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;

#elif  INPUT_TYPE ==INPUT_1A2C
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Digital2,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Nothing,
    Input_VGA           = Input_Analog1,
    Input_DVI           = Input_DVI_C1,
    Input_DVI2          = Input_DVI_C2,
    Input_DVI3          = Input_DVI_C3,
    Input_HDMI          = Input_HDMI_C1,
    Input_HDMI2         = Input_HDMI_C2,
    Input_HDMI3         = Input_HDMI_C3,
    Input_Displayport   = Input_Displayport_C2,
    Input_Displayport3  = Input_Displayport_C3
} InputPortType;
#elif  INPUT_TYPE ==INPUT_1A3C
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Digital2,
    Input_Digital3,
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Nothing,
    Input_VGA           = Input_Analog1,
    Input_DVI           = Input_DVI_C1,
    Input_DVI2          = Input_DVI_C2,
    Input_DVI3          = Input_DVI_C3,
    Input_HDMI          = Input_HDMI_C1,
    Input_HDMI2         = Input_HDMI_C2,
    Input_HDMI3         = Input_HDMI_C3,
    Input_Displayport   = Input_Displayport_C2,
    Input_Displayport3  = Input_Displayport_C3
} InputPortType;
#elif INPUT_TYPE == INPUT_1A1C2
typedef enum
{
    Input_Analog1,
    Input_Digital,
    Input_Nums,
    Input_YPbPr,
    Input_Analog2,
    Input_Digital2,
    Input_Nothing,
    Input_VGA           = Input_Analog1,
    Input_DVI           = Input_DVI_C1,
    Input_DVI2          = Input_DVI_C2,
    Input_DVI3          = Input_DVI_C3,
    Input_HDMI          = Input_HDMI_C1,
    Input_HDMI2         = Input_HDMI_C2,
    Input_HDMI3         = Input_HDMI_C3,
    Input_Displayport   = Input_Displayport_C2,
    Input_Displayport3  = Input_Displayport_C3
} InputPortType;
#elif INPUT_TYPE == INPUT_1A1D1H1DP
typedef enum
{
    Input_Analog1,
    Input_Digital, // dvi daul link
    Input_Digital2,//HDMI
    Input_Displayport,//DP
    Input_Nums,

    Input_YPbPr,
    Input_Analog2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Digital,
    Input_HDMI = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Digital2,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;
#else // 1A
typedef enum
{
    Input_Analog1,
    Input_Nums,

    Input_Digital,
    Input_YPbPr,
    Input_Analog2,
    Input_Digital2,
    Input_Nothing,
    Input_VGA = Input_Analog1,
    Input_DVI = Input_Nothing,
    Input_HDMI = Input_Nothing,
    Input_Displayport = Input_Nothing,
    Input_DVI2        = Input_Nothing,
    Input_DVI3        = Input_Nothing,
    Input_HDMI2       = Input_Nothing,
    Input_HDMI3       = Input_Nothing,
    Input_Displayport3= Input_Nothing
} InputPortType;
#endif

#if INPUT_TYPE == INPUT_1A1D
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Nums,
} InputPortPriorityType;
#elif INPUT_TYPE == INPUT_1A2D
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Digital2,    
    Input_Priority_Nums,
} InputPortPriorityType;
#elif INPUT_TYPE == INPUT_1A1H
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Nums,
} InputPortPriorityType;
#elif INPUT_TYPE == INPUT_1A2H
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Digital2,
    Input_Priority_Nums,
} InputPortPriorityType;
#elif INPUT_TYPE == INPUT_1A1D1H
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Digital2,    
    Input_Priority_Nums,
} InputPortPriorityType;

#elif  INPUT_TYPE ==INPUT_1A1D1DP
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Digital2,    
    Input_Priority_Nums,
} InputPortPriorityType;
#elif  INPUT_TYPE ==INPUT_1A1H1DP
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Digital2,    
    Input_Priority_Nums,
} InputPortPriorityType;
#elif  INPUT_TYPE ==INPUT_1A2C
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Digital2,
    Input_Priority_Nums,
} InputPortPriorityType;
#elif  INPUT_TYPE ==INPUT_1A3C
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Digital2,
    Input_Priority_Digital3,
    Input_Priority_Nums,
} InputPortPriorityType;
#elif INPUT_TYPE == INPUT_1A1C2
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Digital1,
    Input_Priority_Nums,
} InputPortPriorityType;
#elif  INPUT_TYPE == INPUT_1A1D1H1DP
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1, //VGA
    Input_Priority_Digital1,//DVI Dual link
    Input_Priority_Digital2,//HDMI
    Input_Priority_Digital3,//DP
    Input_Priority_Nums,
} InputPortPriorityType;
#else // 1A
typedef enum
{
    Input_Priority_Auto,
    Input_Priority_Analog1,
    Input_Priority_Nums,
} InputPortPriorityType;
#endif

// HpVp->3,  HpVn->2, HnVp->1, HnVn->0

#define SyncHpVp        8  // Sync Polarity
#define SyncHpVn        4
#define SyncHnVp        2
#define SyncHnVn        1


#if 0
typedef enum
{
    Res_640x350,
    Res_640x400,
    Res_720x400,
    Res_640x480,
    Res_800x600,
    Res_832x624,
    Res_884x930, //0909
    Res_1024x768,
    Res_1280x1024,
    Res_1600x1200,
    Res_1152x864,
    Res_1152x870,
    Res_1280x720,
    Res_1280x960,
    Res_720x480,
    Res_800x1209,
    Res_1440x900, // for Wide panel
    Res_1400x1050,
    Res_1680x1050,

    Max_Res
} ResolutionType;
#endif

typedef enum
{
    Res_640x350,
    Res_640x400,
    Res_720x400,
    Res_640x480,
    //Res_720x483,
    Res_700x570,
    Res_720x576,
    Res_800x600,
    Res_832x624,
    Res_884x930, //0909
    Res_1024x768,
    Res_1280x1024,
    Res_1600x1200,
    Res_1152x864,
    Res_1152x870,
    Res_1152x900,
    Res_1280x720,
    Res_1280x960,
    Res_720x480,
    Res_800x1209,
    Res_1792x1344,
    Res_1400x1050,
    Res_1920x1080,
    Res_1920x1200,

    Res_848x480,   //2005-07-26
    Res_1280x768,  //2005-07-26
    Res_1360x768,
    Res_1366x768,
    Res_1440x900,

    Res_1680x1050,
    Res_1280x800,
    Res_1600x900,
    Res_2560x1600,
    Res_2560x1440,
#if _3D_TIMING
    Res_1920x2160,
    Res_1280x720P_FP,
#endif
    Max_Res
} ResolutionType;
typedef enum
{
#if 1//PANEL_WIDTH==1600&&PANEL_HEIGHT==900
MD_1600X900_60_VESA,
MD_1600X900_60_CVT,
MD_1600X900R_60_VESA,
#endif
#if 1//PANEL_WIDTH==1366&&PANEL_HEIGHT==768
MD_1366x768_60,
MD_1360x768_60_VESA,
#endif

MD_1920x1080I_75,

MD_848X480_60_VESA,
MD_848X480_75_CVT,
MD_1280x768R_60_CVT,
MD_1280x768_60_CVT,
MD_1280x768_75_CVT,
MD_1280x768_85_CVT,
MD_1280x960R_60_VESA,
MD_1280x960_60_VESA,
MD_1280x960_70_FORPC,
MD_1280x960_72_FORPC,
MD_1280x960_75_CVT,
MD_1280x960_85_VESA,
MD_1360x768_60,
MD_1440x900R_60_CVT,
MD_1440x900_60_CVT,
MD_1440x900_75_CVT,
MD_1440x900_85_CVT,
MD_1680x1050R_60_CVT,
MD_1680x1050_60_CVT,
MD_1680x1050_57_CVT,
//===============================================
//===== Normal Mode Start ============
//===============================================
MD_640x350_70_IBM,
MD_640x350_85_VESA,
MD_720x400_70,
MD_720x400_85_VESA,
MD_640x400_56_NEC,
MD_640x400_70_IBM,
//===============================================
//===>Conflict with WSVGA(848x480@60Hz HpVp) ====
//===============================================
MD_640x480_60_VESA,
MD_640x480_66_MAC,
MD_640x480_67,
MD_640x480_72_VESA,
MD_640x480_75_VESA,
MD_640x480_85_VESA,
MD_800x600_56_VESA,
MD_800x600_60_VESA,
MD_800x600_72_VESA,
MD_800x600_75_VESA,
MD_800x600_85_VESA,
MD_832x624_75_MAC,
//===============================================
//===>Conflict with WXVGA(1280x768@60Hz HpVp;HnVp) ====
//===============================================
MD_1024x768_60_VESA,
MD_1024x768R_60_VESA,
MD_1024x768_70_VESA,
//===============================================
//===>Conflict with WXVGA(1280x768@75Hz HnVp) ===
//===============================================
MD_1024x768_75_VESA,
MD_1024x768_75_MAC,
//===============================================
//===>Conflict with WXVGA(1280x768@85Hz HnVp) ===
//===============================================
MD_1024x768_85_VESA,
MD_1152x864_60_FORPC,
MD_1152x864_70_FORPC,
MD_1152x864_60_GRAPHICSCARD,//(For ATI Card)
MD_1152x864_75_VESA,
MD_1152x864_85_VESA,
MD_1152x870_75_MAC,
MD_1280x720_60_CVT,
MD_1280x720_70,
MD_1280x720_75,
//========================================================
//===>Conflict with SXVGA+(1400x1050@60Hz HpVn,HnVP ) ====
//========================================================
MD_1280x1024_60_VESA,
MD_1280x1024_75_VESA,
MD_1280x1024_85_VESA,
MD_1600x1200_60_VESA,
MD_1600x1200_65_VESA,
MD_1600x1200_70_VESA,
MD_1600x1200_75_VESA,
MD_1400x1050R_60_CVT,
MD_1400x1050_60_CVT,
MD_1400x1050_75_CVT,
MD_1400x1050_85_CVT,
MD_1792x1344_60_CVT,
MD_1920x1200_60_CVT,
MD_1920x1200R_60_CVT,
#if 1
MD_720x480_50_YCbCr,
#endif
MD_1024x768_72,
MD_1280x1024_72,
MD_1280x1024_70,
MD_1152x900_66,
MD_720x480I_60_YCbCr,     // --> 78  Start of YPbPr
MD_1920x1080I_50_YCbCr,
MD_1920x1080I_50_R_YCbCr,
MD_1920x1080I_60_NTSC_YCbCr,
MD_700x570_50_YCbCr,//(For India TV Box)
MD_1152x870_60_MAC_YCbCr,
MD_720x576_50_YCbCr,
MD_720x576_60_YCbCr,
MD_720x576_75_YCbCr,
MD_720x576I_50_YCbCr,
MD_720x480_60_YCbCr,
MD_720x480I_60_YCbCr_1,
MD_1920x1080_50_YCbCr,
MD_1920x1080_60_YCbCr,
MD_832x624_75_MAC_YCbCr,
MD_1280x800R_60_CVT,
MD_1280x800_60_CVT,
MD_1280x800_60,
MD_1280x800_60_GRAPHICSCARD,//NVIDIA
MD_1280x800_75_CVT,
MD_1280x800_76,
MD_1920x1080_60_138M,// 138.5M
MD_1920x1080_60_148M,//148.5M

MD_1920x1080_24,
MD_1920x1080_25,
MD_1920x1080_30,
Mod_2560x1600_60,
Mod_2560x1440p_60,


#if _3D_TIMING
Mod_1920x1080_24FP,
Mod_1280x720p_60_FP,
Mod_1280x720p_50_FP,
#endif
MD_MAX

} StandardModeType;
typedef struct
{
    BYTE fFlags;
    BYTE InputType;
    WORD HPeiod;
    WORD VTotal;
    BYTE ModeIndex;
    WORD VFreq;
#if ENABLE_CHECK_TMDS_DE
    WORD TMDSWidth;
    WORD TMDSHeight;
    WORD TMDSHStart;
    WORD TMDSVStart;
#endif
} InputTimingType;
typedef struct
{
    WORD DispWidth;
    WORD DispHeight;
} InputResolutionType;
typedef struct
{
    BYTE Flags;
    ResolutionType ResIndex;
    WORD HFreq;
    WORD VFreq;
    WORD HTotal;
    WORD VTotal;
    WORD HStart;
    WORD VStart;
} InputModeType;
_MODEDEC_ WORD GetStandardModeWidth( void );
_MODEDEC_ WORD GetStandardModeHeight( void );
_MODEDEC_ WORD GetStandardModeGroup( void );
_MODEDEC_ WORD GetStandardModeHStart( void );
_MODEDEC_ WORD GetStandardModeVStart( void );
_MODEDEC_ WORD GetStandardModeHTotal( void );
_MODEDEC_ WORD GetStandardModeVTotal( void );
_MODEDEC_ WORD GetStandardModeHFreq( void);
_MODEDEC_ WORD GetStandardModeVFreq( void );
_MODEDEC_ WORD Mode_GetStandardModePxlRate(void);

#endif
