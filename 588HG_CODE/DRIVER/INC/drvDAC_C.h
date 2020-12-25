///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   msDAC.h
/// @author MStar Semiconductor Inc.
/// @brief  Audio DAC Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MSDAC_H_
#define _MSDAC_H_
extern bit bToggleGainFlag;
extern BYTE xdata ToggleGainCntr;
#define ToggleGainPeriod    50 // unit: ms

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

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


#endif //_MSDAC_H_

