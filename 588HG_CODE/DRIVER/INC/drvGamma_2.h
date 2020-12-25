
#if 0
#if USE_NORMAL_GAMMA_TBL
extern void drvGammaLoad320ByteTbl(BYTE u8Window, BYTE channel, BYTE *tblPtr);
#else
extern void drvGammaLoadCompressedTbl(BYTE u8Window, BYTE channel, BYTE* ucTab1, BYTE* ucTab2);
#endif
#endif
extern void drvGammaLoadTbl( BYTE GamaMode );
extern void drvGammaOnOff(BYTE u8Switch, BYTE u8Window);


