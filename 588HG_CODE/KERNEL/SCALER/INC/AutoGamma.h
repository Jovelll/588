#ifndef _AUTOGAMMA_H

#define _AUTOGAMMA_H




#ifdef _AUTOGAMMA_C

#define INTERFACE

#else

#define INTERFACE   extern

#endif


//Hans120330

#define OPTION_ENABLE_DELTAE 1

#define OPTION_ENABLE_WHITEBAL 0

#define OPTION_ENABLE_CHKSUM 1

#define OPTION_LOADEEPTBLToSRAM 0


//#define GammaTblSize   64*3//76// ((256/(2*CompRate)) ) //add 4 for Gamma vaule "1"

//#define adrBGamma0_Base                0x400//0x300

//#define adrBGamma0_CheckSum_Start      0x3F0

//#define rmColorSpaceTransAddress(item_i) (RM_COLORSPACE_TRANS_BASEADDR + (sizeof(CSTransferSettingType)*item_i))

INTERFACE void LoadPostGammaTbl(BOOL bWindow, BYTE GamIndex);
INTERFACE void msWriteFGammaCM(BYTE  bWindow,  BYTE ucGammaTableIdx);

INTERFACE void msSetFGammaCMOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void msSetFGammaOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void msSetFixGammaOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void msWriteFGammaTbl(BOOL bWindow, BYTE ModeIndex);

INTERFACE void msSetPostGammaOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void LoadDeltaEColorMode(BOOL bWindow, BYTE ModeIndex);

INTERFACE void msLoadEEPGmaTBLToSRAM(WORD GmaTblAddr1, BYTE GmaTblSize); //Hans120330

INTERFACE void msWriteFixGammaTbl(BOOL bWindow, BYTE ModeIndex);

#undef INTERFACE

#endif

