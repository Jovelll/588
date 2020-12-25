/******************************************************************************
 Copyright (c) 2004 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: MsHDMI.h
 [Date]:        28-Jan-2004
 [Comment]:
   HDCP header file.
 [Reversion History]:
*******************************************************************************/
#ifndef     MSHDMI_H
#define     MSHDMI_H

#ifdef _MSHDMI_C_
#define _MSHDMIDEC_
#else
#define _MSHDMIDEC_ extern
#endif

typedef enum
{
    INPUT_RGB    = 0,   // 00
    INPUT_YUV422 = 1,   // 01
    INPUT_YUV444 = 2    // 11
};

typedef enum
{
    INPUT_NONE,
    INPUT_ADC_0,
    INPUT_TMDS_A,
    INPUT_MAX
};
typedef enum
{
    TMDS_DVI,
    TMDS_HDMI
};
typedef enum
{
    HDMI_AUDIO_OFF,
    HDMI_AUDIO_ON
};
typedef enum
{
    HDMI_DATA_OUTPUT_OFF,
    HDMI_DATA_OUTPUT_ON
};

typedef enum _HDMIPktStatusReset
{
    Reset_AVMUTE = BIT0, // GCONTROL:AVMUTE
    Reset_Color = BIT1, // AVI_IF1:Y[1:0]
    Reset_PixelRep = BIT2, // AVI_IF4:PR[3:0]
    Reset_FrameRep = BIT3, // Frame repetition
    Reset_DCInfo = BIT4, // Reset CD, PP, and default phase value of GCP packet.
    Reset_DCFIFO = BIT5, // Reset deep color FIFO.
    Reset_HDMIStatus = BIT7,
} HDMIPktStatusReset;

typedef enum _AudioFreqType
{
    AUD_FREQ_ERROR,
    AUD_FREQ_32K,
    AUD_FREQ_44K,
    AUD_FREQ_48K,
    AUD_FREQ_88K,
    AUD_FREQ_96K,
    AUD_FREQ_192K,
} AudioFreqType;
#if ENABLE_HDMI_1_4
typedef enum
{
    E_HDMI_NO_ADDITIONAL_FORMAT, // 000
    E_HDMI_4Kx2K_FORMAT,                 // 001
    E_HDMI_3D_FORMAT,                      // 010
    E_HDMI_RESERVED,                         // 011 ~ 111
    E_HDMI_NA,                                     // Not available
} E_HDMI_ADDITIONAL_VIDEO_FORMAT;

// 3D info
typedef enum
{
    //range [0000 ~ 1111] reserved for hdmi 3D spec
    E_HDMI_3D_INPUT_FRAME_PACKING                     = 0x00, //0000
    E_HDMI_3D_INPUT_FIELD_ALTERNATIVE                 = 0x01, //0001
    E_HDMI_3D_INPUT_LINE_ALTERNATIVE                  = 0x02, //0010
    E_HDMI_3D_INPUT_SIDE_BY_SIDE_FULL                 = 0x03, //0011
    E_HDMI_3D_INPUT_L_DEPTH                           = 0x04, //0100
    E_HDMI_3D_INPUT_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH   = 0x05, //0101
    E_HDMI_3D_INPUT_TOP_BOTTOM                        = 0x06, //0110
    E_HDMI_3D_INPUT_SIDE_BY_SIDE_HALF                 = 0x08, //1000
    E_HDMI_3D_RESERVED                 = 0x09, // 0111, 1001 ~ 1111
    E_HDMI_3D_NA                 = 0x0A,
} E_HDMI_3D_INPUT_MODE;

typedef enum
{
    E_VIC_4Kx2K_30Hz = 0x01, // 1920(x2)x1080(x2) @ 29.97/30Hz
    E_VIC_4Kx2K_25Hz = 0x02, // 1920(x2)x1080(x2) @ 25Hz
    E_VIC_4Kx2K_24Hz = 0x03, // 1920(x2)x1080(x2) @ 23.976/24Hz
    E_VIC_4Kx2K_24Hz_SMPTE = 0x04, // 2048(x2)x1080(x2) @ 24Hz
    E_VIC_RESERVED,                // 0x00, 0x05 ~ 0xFF
    E_VIC_NA,                             // Not available
} E_HDMI_VIC_4Kx2K_CODE;

typedef enum
{
    E_3D_EXT_DATA_HOR_SUB_SAMPL_0,      //0000
    E_3D_EXT_DATA_HOR_SUB_SAMPL_1,      //0001
    E_3D_EXT_DATA_HOR_SUB_SAMPL_2,      //0010
    E_3D_EXT_DATA_HOR_SUB_SAMPL_3,      //0011
    E_3D_EXT_DATA_QUINCUNX_MATRIX_0,    //0100
    E_3D_EXT_DATA_QUINCUNX_MATRIX_1,    //0101
    E_3D_EXT_DATA_QUINCUNX_MATRIX_2,    //0110
    E_3D_EXT_DATA_QUINCUNX_MATRIX_3,    //0111
    E_3D_EXT_DATA_RESERVE,              //1000 ~ 1111
    E_3D_EXT_DATA_MODE_MAX,
} E_HDMI_3D_EXT_DATA_T;

typedef enum
{
    E_3D_META_DATA_PARALLAX_ISO23002_3,      //0000
    E_3D_META_DATA_RESERVE,              //001 ~ 1111
    E_3D_META_DATA_MAX,
} E_HDMI_3D_META_DATA_TYPE;

typedef struct
{
    Bool b3D_Meta_Present;
    E_HDMI_3D_META_DATA_TYPE t3D_Metadata_Type;
    BYTE u83D_Metadata_Length;
    BYTE u83D_Metadata[23]; // Maximum length of VS is 31 - 8 = 23
} sHDMI_3D_META_FIELD;
#endif // #if EN_HDMI_1_4

typedef struct _sHDMIAudioErrorEvent
{
    BYTE bASPError : 1;
    BYTE bASPParityError : 1;
    BYTE bNonPCM : 1;
    BYTE bAVMUTE : 1;
    BYTE bCTSNOverRange : 1;
    BYTE bNoInputClock : 1;
    BYTE bVideoClockBigChanged : 1;
    BYTE bChannelStatusChanged : 1;
    BYTE b2CHASPPresentBitError : 1;
    BYTE bFlatBitReceived : 1;
} sHDMIAudioErrorEvent;


typedef struct
{
    BYTE    ColorChanged;
    BYTE    ModeChange;
    WORD    wAviPktLossCnt;
    BYTE    InputSource;
    BYTE    InputColor;
    BYTE    InputVideoMode;
    BYTE    InputTmdsType;
    WORD    InputPacketStatus;
    BYTE    AudioOffDebunce;
    BYTE    InputPacketError;
#if ENABLE_HDMI_1_4
    BYTE    NoVsPktCnt;
#endif
#if ENABLE_HDMI_SW_CTS_STABLE
    BYTE    InputCTSValue;
    BYTE    OutputAudioCnt;
#endif
#if ENABLE_HDMI_SW_CTS_STABLE
    BYTE    InputCTSValue;
    BYTE    OutputAudioCnt;
#endif
#if ENABLE_CTS_INT
    BYTE    CTSFilterEN:1;
    DWORD   InputCTSData;
#if (ENABLE_CTSN_FILTER)
    DWORD   InputNData;
    sHDMIAudioErrorEvent ucAudioErrorEvents;
#endif
    BYTE    CTSStableCnt;
#endif

    BYTE    AVI_Colorimetry;
    BYTE    OutputVideoFormat;
    BYTE    OutputAudioState;
    BYTE    OutputDataState;
    BYTE    InputAVIInfoReceived;
    BYTE    InputSPDInfoReceived;
    BYTE    cs0; //2009-08-04


} SC_INFO;


_MSHDMIDEC_ SC_INFO xdata gScInfo;
_MSHDMIDEC_ BYTE xdata HdmiPollingCounter;
//_MSHDMIDEC_ WORD xdata wSwitchPortCounter;
//_MSHDMIDEC_ BYTE idata CSpcPollingCounter;
#define AviPktLossBound      500  //unit:ms
#if ENABLE_CTS_INT
#define CTSSTABLECOUNT              6
#define CTSSTABLERANGE              10

_MSHDMIDEC_ void EnableCTSFileterFunction( void );
_MSHDMIDEC_ void CheckCTSFilterFunction( void );
#endif

#if ENABLE_HDMI_SW_CTS_STABLE
#define AudioDeBounce           5
#define CTSRange                    0x10
#endif

#define CTSN_Range            10

#define ENABLE_DVI_AUTO_EQ    0
#if ENABLE_HDMI_1_4
#define NoVsPktBound         5 //unit:pkt
#endif


_MSHDMIDEC_ void mstar_HDMIInitialVariable( void );
_MSHDMIDEC_ void mstar_HDMIInitialAudio( void );
_MSHDMIDEC_ void mstar_HDMIHandler( void );
_MSHDMIDEC_ void mstar_HDMIRst( BYTE rst );
_MSHDMIDEC_ void mstar_HDMIHandleMode( void );
_MSHDMIDEC_ void mstar_HDMIHandleColor( void );
//_MSHDMIDEC_ void mstar_HDMIResetColorSpace( void );
_MSHDMIDEC_ void mstar_MonitorHDMI( void );
_MSHDMIDEC_ void mstar_HDMICheckAudio( void );
_MSHDMIDEC_ BYTE mstar_HDMIPacketColor( void );
_MSHDMIDEC_ void mstar_HDMIAudioMute( BYTE sw );
_MSHDMIDEC_ void mstar_HDMIGetPktStatus( void );
#if (ENABLE_FREESYNC&&ENABLE_HDMI)
_MSHDMIDEC_ Bool mstar_HDMIGetFreeSyncFlag( void );
#endif
_MSHDMIDEC_ void mstar_HDMIAutoEQProc(void);
_MSHDMIDEC_ BYTE mstar_HDMITmdsGetType( void );
_MSHDMIDEC_ Bool mstar_HDMIIsAudioPCM(void);
_MSHDMIDEC_ void CheckAudioErrorStatus( void );
#if ENABLE_HDMI_1_4
_MSHDMIDEC_ E_HDMI_ADDITIONAL_VIDEO_FORMAT msHDMI_Check_Additional_Format(void);
_MSHDMIDEC_ E_HDMI_3D_INPUT_MODE msHDMI_Get_3D_Structure(void);
_MSHDMIDEC_ E_HDMI_3D_EXT_DATA_T msHDMI_Get_3D_Ext_Data(void);
_MSHDMIDEC_ void msHDMI_Get_3D_Meta_Field(sHDMI_3D_META_FIELD *ptr);
_MSHDMIDEC_ BYTE msHDMI_Get_VIC_Code(void);
_MSHDMIDEC_ E_HDMI_VIC_4Kx2K_CODE msHDMI_Get_4Kx2K_VIC_Code(void);
_MSHDMIDEC_ void msHDMI_Check_3D_Mode_Chg(BYTE SetTimingChg);
#endif // #if EN_HDMI_1_4

#if ENABLE_CTS_HPD_PD100K  
_MSHDMIDEC_ void msHDMI_EnHPDPD100K( BYTE InputType );
#endif


//_MSHDMIDEC_ void SetMainWinVideoDomain( BYTE Domain );
#endif

