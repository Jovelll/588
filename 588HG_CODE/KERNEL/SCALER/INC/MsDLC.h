#ifndef __MSDLC_H
#define __MSDLC_H
#define DLC_LG_MODE 1
#if DLC_LG_MODE
extern XDATA BYTE g_ucLumaCurve[18];
#else
extern XDATA BYTE g_ucLumaCurve[16];
#endif
#if DLC_LG_MODE 
extern WORD XDATA uwArray[32];
#else
extern WORD XDATA uwArray[8];
#endif
extern XDATA BYTE g_LowStrength;
extern XDATA BYTE g_HighStrength;
extern XDATA BYTE g_bDLCOnOff;
extern XDATA BYTE g_bDynamicBrith;
extern XDATA BYTE DLCStatus;

extern XDATA BYTE BrightData;
extern XDATA BYTE DarkData;
extern XDATA BYTE PreBrightData;
extern XDATA BYTE ReadFrameDataCnt;
extern XDATA BYTE SetDarkestFlag;
extern XDATA BYTE DarkSegmentData;
///////////////////////////////////////////////////////////////////////////////
// DLC(Dynamic luma curve
///////////////////////////////////////////////////////////////////////////////
void InitDLCVar();
extern void msDlcInit( WORD wWidth, WORD wHeight );
// The value of Strength is small, and the effect is strong!
// Lstrength :  0.50(strong)   <------------>   1.70 (weak) ; default = 1.20
// Hstrength:   0.50(strong)   <------------>   2.00 (weak) ; default = 1.00
// **This function must called after msDlcInit()
extern void msSetDlcStrength( BYTE ucLowStrength, BYTE ucHighStrenght );
extern void msDlcHandler(); // 2004/12/8
extern void msDlcOnOff( BYTE bSwitch ); // 2004/12/8
extern void LoadDLCTable( BYTE *DLCtable );

extern void msDCROnOff( BYTE ucSwitch, BYTE win);
extern void msDCRHandler(void);

#define DEBUG_DLC_HISTOGRAM 0
#define DEBUG_DLC_CURVE 0
typedef enum
{
    DLC_WAIT,
    DLC_READ1,
    DLC_READ2,
    DLC_READ3,
    DLC_READ4,
    DLC_READ5,
    DLC_READ6,
    DLC_READ7,
    DLC_READ8,
    DLC_CAL1,
    DLC_CAL2,
    DLC_CAL3,
    DLC_CAL4,
    DLC_CAL5,
    DLC_CAL6,
    DLC_WRITE1,
    DLC_WRITE2,
    DLC_WRITE3,
    DLC_WRITE4,
    DLC_WRITE5,
    DLC_WRITE6,
    DLC_WRITE7,
    DLC_WRITE8
} DLC_STATUS;
#endif
