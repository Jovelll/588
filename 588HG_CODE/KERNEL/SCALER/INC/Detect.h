#ifndef _DETECT_H_
#define _DETECT_H_

extern void mStar_MonitorInputTiming( void );
extern void mStar_ModeHandler( void );
extern Bool mStar_ValidTimingDetect( void );
//extern Bool mStar_IsStatusChanged( void );
extern Bool IsCableNotConnected( void );
#if ENABLE_MHL
    #if(CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
extern Bool CurrentInputMHLCablePlugged(void);
    #endif
#endif
extern WORD GetImageHeight(void);
extern WORD GetImageWidth(void);
#endif

