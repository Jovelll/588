#include "board.h"

#ifdef _USERPREF_C_
#define _USERPREF_DEC_
#else
#define _USERPREF_DEC_ extern
#endif

#if USEFLASH
_USERPREF_DEC_ WORD xdata g_wFlashSaveState;    // Save user Setting flag

#define bFlashSaveMonitorBit    BIT0
#define bFlashSaveModeBit       BIT1
#define bFlashSaveFactoryBit    BIT2
#define bFlashSaveMonitor2Bit   BIT3

#define bFlashForceSaveMonitorBit    BIT4
#define bFlashForceSaveModeBit       BIT5
#define bFlashForceSaveFactoryBit    BIT6
#define bFlashForceSaveMonitor2Bit    BIT7


#define FlashSaveMonitorFlag        (g_wFlashSaveState&bFlashSaveMonitorBit)
#define Set_FlashSaveMonitorFlag()  (g_wFlashSaveState|=bFlashSaveMonitorBit)
#define Clr_FlashSaveMonitorFlag()  (g_wFlashSaveState&=~bFlashSaveMonitorBit)

#define FlashSaveModeFlag           (g_wFlashSaveState&bFlashSaveModeBit)
#define Set_FlashSaveModeFlag()     (g_wFlashSaveState|=bFlashSaveModeBit)
#define Clr_FlashSaveModeFlag()     (g_wFlashSaveState&=~bFlashSaveModeBit)

#define FlashSaveFactoryFlag        (g_wFlashSaveState&bFlashSaveFactoryBit)
#define Set_FlashSaveFactoryFlag()  (g_wFlashSaveState|=bFlashSaveFactoryBit)
#define Clr_FlashSaveFactoryFlag()  (g_wFlashSaveState&=~bFlashSaveFactoryBit)

#define FlashForceSaveMonitorFlag        (g_wFlashSaveState&bFlashForceSaveMonitorBit)
#define Set_FlashForceSaveMonitorFlag()  (g_wFlashSaveState|=bFlashForceSaveMonitorBit)
#define Clr_FlashForceSaveMonitorFlag()  (g_wFlashSaveState&=~bFlashForceSaveMonitorBit)

#define FlashForceSaveModeFlag           (g_wFlashSaveState&bFlashForceSaveModeBit)
#define Set_FlashForceSaveModeFlag()     (g_wFlashSaveState|=bFlashForceSaveModeBit)
#define Clr_FlashForceSaveModeFlag()     (g_wFlashSaveState&=~bFlashForceSaveModeBit)

#define FlashForceSaveFactoryFlag        (g_wFlashSaveState&bFlashForceSaveFactoryBit)
#define Set_FlashForceSaveFactoryFlag()  (g_wFlashSaveState|=bFlashForceSaveFactoryBit)
#define Clr_FlashForceSaveFactoryFlag()  (g_wFlashSaveState&=~bFlashForceSaveFactoryBit)

#define FlashSaveMonitor2Flag        (g_wFlashSaveState&bFlashSaveMonitor2Bit)
#define Set_FlashSaveMonitor2Flag()  (g_wFlashSaveState|=bFlashSaveMonitor2Bit)
#define Clr_FlashSaveMonitor2Flag()  (g_wFlashSaveState&=~bFlashSaveMonitor2Bit)

#define FlashForceSaveMonitor2Flag        (g_wFlashSaveState&bFlashForceSaveMonitor2Bit)
#define Set_FlashForceSaveMonitor2Flag()  (g_wFlashSaveState|=bFlashForceSaveMonitor2Bit)
#define Clr_FlashForceSaveMonitor2Flag()  (g_wFlashSaveState&=~bFlashForceSaveMonitor2Bit)
#endif

_USERPREF_DEC_ void UserPref_EnableFlashSaveBit( WORD ucFlag );
_USERPREF_DEC_ void UserPref_FlashSaveFlagCheck( void );
_USERPREF_DEC_ void UserPref_FlashDataSaving( void );


_USERPREF_DEC_ void Init_MonitorSetting2( void );
_USERPREF_DEC_ void SaveMonitorSetting2( void );
_USERPREF_DEC_ void SaveFactorySetting( void );
_USERPREF_DEC_ void SavePanelSetting( void );
_USERPREF_DEC_ void Init_MonitorSetting( void );
_USERPREF_DEC_ void Init_ModeSetting( void );
_USERPREF_DEC_ void CheckMonitorSettingRange( void );
_USERPREF_DEC_ void SaveMonitorSetting( void );
_USERPREF_DEC_ void SaveModeSetting( void );
_USERPREF_DEC_ void ReadMonitorSetting( void );
_USERPREF_DEC_ void ReadModeSetting( void );
_USERPREF_DEC_ void SaveBlacklitTime( void );
_USERPREF_DEC_ void ReadBlacklitTime( void );
_USERPREF_DEC_ void CheckColorValueRange( void );
_USERPREF_DEC_ Bool CheckMonitorSettingOutOfRange( void );
_USERPREF_DEC_ WORD CalculateSettingCheckSum( BYTE* Array, BYTE Len );


typedef enum
{
    NORMAL,
    BLUE,
    GREEN,
    FLESHTONE,
    Y_FLESHTONE,
    MAX_COLOR_ENHANCE_TYPE
} ColorEnhanceType;

typedef enum
{
    OFF_W,
    WINDOW_W,
    SPLIT_W,
    FULL_W,
    MAX_MWE
} MWEType;

typedef enum
{
    OFF_SKIN,
    NORMAL_SKIN,
    RED_SKIN,
    YELLOW_SKIN,
    MAX_SKIN
} SkinToneType;

typedef enum
{
    OFF_DLC,
    MODE1_DLC,
    MODE2_DLC,
    MODE3_DLC,
    MAX_DLC
} DLCMode;

typedef enum
{
    OFF_MWEMODE,
    MOVIE1_MWEMODE,
    MOVIE2_MWEMODE,
    MAX_MWEMODE
} MWEMode;
