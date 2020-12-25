///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   msDAC.h
/// @author MStar Semiconductor Inc.
/// @brief  Audio DAC Function
///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
//NOTE
//WS:48khz , BCK:16BIT:1536khz,24BIT:2304khz,32BIT:3072khz ,MCK:0: 64fs 1:128fs 2:256fs 3:384fs  4:512fs 5:768fs
//-------------------------------------------------------------------------------------------------

#ifndef _MSDAC_H_
#define _MSDAC_H_

#define USE_CASE_1      0	//LINEIN TO EAROUT/LINEOUT , LINEIN TO SPDIF(GPIO01) , HDMI TO EAROUT/LINEOUT , HDMI TO SPDIF(GPIO01)
#define USE_CASE_2      1	//HDMI TO I2S , HDMI TO SPDIF(AUVRM) , HDMI TO SPDIF(GPIO01)
#define USE_CASE_3      2	//HDMI TO I2S , HDMI TO EAROUT , HDMI TO SPDIF(GPIO01)
#define USE_CASE_4      3  	//LINEIN TO I2S , HDMI TO I2S
#define USE_CASE_5      4	//LINEIN TO EAROUT/LINEOUT , HDMI TO EAROUT/LINEOUT
#ifndef USE_CASE_SEL
#define USE_CASE_SEL                     USE_CASE_5
#endif

extern bit bToggleGainFlag;
extern BYTE xdata ToggleGainCntr;
#define ToggleGainPeriod    50 // unit: ms

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
#if EarphoneDet_Enable
	typedef enum _AUDIO_LINE_EAR_OUT_SEL
	{
	    E_LINE_OUT_ = 0x00,
	    E_EAR_OUT_,
	    E_LINE_EAR_OUT_MAX
	} AUDIO_LINE_EAR_OUT_SEL;
#endif

// Audio channel selection
typedef enum _AUDIO_CH_SEL
{
    E_AUDIO_LEFT_CH = 0,
    E_AUDIO_RIGHT_CH = 1,
    E_AUDIO_LEFT_RIGHT_CH = 2
} AUDIO_CH_SEL;

// Line-out source selection
typedef enum _AUDIO_LINEOUT_SOURCE_SEL
{
    E_LINEOUT_FROM_DAC = 0,
    E_LINEOUT_FROM_LINEIN0 = 1,
    E_LINEOUT_FROM_DMA = 2
} AUDIO_LINEOUT_SOURCE_SEL;

// Earphone-out source selection
typedef enum _AUDIO_EAROUT_SOURCE_SEL
{
    E_EAROUT_FROM_DACOUT = 0,
    E_EAROUT_FROM_LINEIN0 = 1,
    E_EAROUT_FROM_DMA = 2
} AUDIO_EAROUT_SOURCE_SEL;

//Internal PCM generator
typedef enum __AUDIO_PCM_FREQ
{
    PCM_250Hz,
    PCM_500HZ,
    PCM_1KHZ,
    PCM_1500HZ,
    PCM_2KHZ,
    PCM_3KHZ,
    PCM_4KHZ,
    PCM_6KHZ,
    PCM_8KHZ,
    PCM_12KHZ,
    PCM_16KHZ
} AUDIO_PCM_FREQ;

typedef enum __DACINITSTEP
{
    INIT_STEP0,
    INIT_STEP1,
    INIT_STEP2,
    INIT_STEP3,
    INIT_STEP4,
    INIT_STEP5,
    INIT_STEP6,
    INIT_STEP7,
    
    INIT_FINISH,
} DACINITSTEP;

#if ENABLE_HDMI || ENABLE_DP_INPUT
typedef enum _AUDIO_SOURCE_SEL
{
    AUDIO_LINE_IN = 0,
    AUDIO_DIGITAL = 1,
} AUDIO_SOURCE_SEL;
#endif


//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
extern void msAudio_I2S_SPDIF_Init(void);
extern void msAudioDAC_Init( void );
extern void msAudioDPGA_Mute( void );
extern void msAudioDPGA_SetVolume( AUDIO_CH_SEL chsel, int volume );
extern void msAudioLineout_SourceSel( AUDIO_LINEOUT_SOURCE_SEL src );
//extern AUDIO_LINEOUT_SOURCE_SEL msAudioLineout_GetSourceSel(void);
extern void msAudioEARout_Mute( Bool EnableMute );
extern void msAudioEAR_SourceSel( AUDIO_EAROUT_SOURCE_SEL src );
//extern AUDIO_EAROUT_SOURCE_SEL msAudioEARout_GetSourceSel(void);
//extern void msAudioDACPCMGenerator(Bool bEnable, AUDIO_PCM_FREQ eFreq);
extern void msAudioDACPowerDown( Bool bPowerDN );
extern void msAudioLineOutGain( BYTE val );
extern void msAudioGainToggle( void );
extern void PCM_GEN(Bool bEnable, BYTE u8SinGenRLCHFreqSel, BYTE u8SinGenGAINSel);
#if  0//(InitialDACByISR)
extern void msDACInitStepByISR( void );
#else
extern void msDACInitStep( void );
#endif
extern void msAudioGainForceToggle(void);
extern void mStar_AdjustVolume( BYTE volume );
#if EarphoneDet_Enable
extern void msAudioLineEarSwitch(BYTE AudioOutType);
#endif


#endif //_MSDAC_H_

