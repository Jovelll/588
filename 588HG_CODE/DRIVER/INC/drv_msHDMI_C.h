
#ifndef     DRV_MSHDMI_7_H
#define     DRV_MSHDMI_7_H

#ifdef _DRV_MSHDMI_7_C_
#define _DRV_MSHDMIDEC_
#else
#define _DRV_MSHDMIDEC_ extern
#endif

_DRV_MSHDMIDEC_ void drv_mstar_HDMIInitialAudio( void );
_DRV_MSHDMIDEC_ void drv_mstar_HDMIRst( BYTE rst );
_DRV_MSHDMIDEC_ BYTE drv_mstar_HDMIPacketColor( void );
_DRV_MSHDMIDEC_ void drv_mstar_HDMIAudioMute( BYTE sw );
_DRV_MSHDMIDEC_ void drv_mstar_HDMIGetPktStatus( void );
_DRV_MSHDMIDEC_ BYTE drv_mstar_HDMITmdsGetType( void );
_DRV_MSHDMIDEC_ void drv_mstar_HDMIAutoEQProc(void);
#if (ENABLE_FREESYNC&&ENABLE_HDMI)
_DRV_MSHDMIDEC_ Bool drv_mstar_HDMIGetFreeSyncFlag(void);
#endif
_DRV_MSHDMIDEC_ Bool drv_mstar_HDMIIsAudioPCM(void);

#if ENABLE_CTS_INT
#if ENABLE_CTSN_FILTER
//_DRV_MSHDMIDEC_ void drv_EnableCTSNFileterFunction( void );
//_DRV_MSHDMIDEC_ void drv_CheckCTSNFilterFunction( void );
#else
_DRV_MSHDMIDEC_ void drv_EnableCTSFileterFunction( void );
_DRV_MSHDMIDEC_ void drv_CheckCTSFilterFunction( void );
#endif//ENABLE_CTSN_FILTER
#endif//ENABLE_CTS_INT

#if ENABLE_HDMI_SW_AUDCLK_CHECK
_DRV_MSHDMIDEC_ AudioFreqType drv_mstar_HDMICheckAudioFreq( void );
_DRV_MSHDMIDEC_ void drv_mstar_SetAudioClockLock( Bool bLock );
#endif//ENABLE_HDMI_SW_AUDCLK_CHECK

#if ENABLE_AUDIO_AUTO_MUTE || ENABLE_AUDIO_AUTO_FADING || ENABLE_CTSN_FILTER
_DRV_MSHDMIDEC_ void drv_CheckAudioErrorStatus( void );
#endif//ENABLE_AUDIO_AUTO_MUTE || ENABLE_AUDIO_AUTO_FADING || ENABLE_CTSN_FILTER

#if ENABLE_HDMI_1_4
_DRV_MSHDMIDEC_ E_HDMI_ADDITIONAL_VIDEO_FORMAT drv_msHDMI_Check_Additional_Format(void);
_DRV_MSHDMIDEC_ E_HDMI_3D_INPUT_MODE drv_msHDMI_Get_3D_Structure(void);
#if  0
_DRV_MSHDMIDEC_ E_HDMI_3D_EXT_DATA_T drv_msHDMI_Get_3D_Ext_Data(void);
_DRV_MSHDMIDEC_ void drv_msHDMI_Get_3D_Meta_Field(sHDMI_3D_META_FIELD *ptr);
_DRV_MSHDMIDEC_ BYTE drv_msHDMI_Get_VIC_Code(void);
_DRV_MSHDMIDEC_ E_HDMI_VIC_4Kx2K_CODE drv_msHDMI_Get_4Kx2K_VIC_Code(void);
#endif


#endif//ENABLE_HDMI_1_4

#if ENABLE_CTS_HPD_PD100K  
_DRV_MSHDMIDEC_ void drv_mstar_HDMIEnHPDPD100K( BYTE InputType );
#endif

#endif


