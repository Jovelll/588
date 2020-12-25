#ifndef _DRVFACTORYALIGNAOC_H_
#define _DRVFACTORYALIGNAOC_H_

#define ENABLE_NEWDFM_REQUEST                   1

typedef enum drvFactoryModeType
{
    FACTORY_MODE_OFF,
    FACTORY_MODE_ON,
    FACTORY_MODE_MAX
} drvFactoryModeType;

typedef enum drvFactoryColorTempType
{
    FACTORY_COLOR_SAVE,
    FACTORY_COLOR_SRGB,
    FACTORY_COLOR_NATIVE,
    FACTORY_COLOR_4000K,
    FACTORY_COLOR_5000K,
    FACTORY_COLOR_6500K,
    FACTORY_COLOR_7500K,
    FACTORY_COLOR_8200K,
    FACTORY_COLOR_9300K,
    FACTORY_COLOR_10000K,
    FACTORY_COLOR_11500K,
    FACTORY_COLOR_USER,
    FACTORY_COLOR_MAX = 0x14
} drvFactoryColorTempType;

#if 1
#define LanTChineseCode         0x01
#define LanEnglistCode          0x02
#define LanFrenchCode           0x03
#define LanGermanCode           0x04
#define LanItalianCode          0x05
#define LanJapaneseCode         0x06
#define LanKoreanCode           0x07
#define LanPortuguesePortugalCode       0x08
#define LanRussianCode          0x09
#define LanSpanishCode          0x0A
#define LanSwedishCode          0x0B
#define LanTurkishCode          0x0C
#define LanSChineseCode         0x0D
#define LanPortugueseBrazilCode     0x0E
#define LanCzechCode            0x12
#define LanDanskCode            0x13
#define LanDutchCode            0x14
#define LanFinnishCode          0x16
#define LanNorskCode            0x1D
#define LanPolishCode           0x1E
#else
typedef enum drvFactoryLanguageType
{
    FACTORY_LANGUAGE_TRADCHINESE,       
    FACTORY_LANGUAGE_ENGLISH,
    FACTORY_LANGUAGE_FRENCH,
    FACTORY_LANGUAGE_GERMAN,
    FACTORY_LANGUAGE_ITALIAN,           // 4
    FACTORY_LANGUAGE_JAPAN,
    FACTORY_LANGUAGE_KOREAN,
    FACTORY_LANGUAGE_PORTUGAL,
    FACTORY_LANGUAGE_RUSSIAN,
    FACTORY_LANGUAGE_ESPANOL,           //9
    FACTORY_LANGUAGE_SWEDISH,
    FACTORY_LANGUAGE_TURKISH,
    FACTORY_LANGUAGE_SIMPCHINESE,
    FACTORY_LANGUAGE_BRAZIL,
    FACTORY_LANGUAGE_ARABIC,            //14
    FACTORY_LANGUAGE_BULGARIAN,
    FACTORY_LANGUAGE_CROATIAN,
    FACTORY_LANGUAGE_CZECH,
    FACTORY_LANGUAGE_DANISH,
    FACTORY_LANGUAGE_DUTCH,
    FACTORY_LANGUAGE_ESTONIAN,          //19
    FACTORY_LANGUAGE_FINNISH,
    FACTORY_LANGUAGE_GREEK,
    FACTORY_LANGUAGE_HEBREW,
    FACTORY_LANGUAGE_HINDI,
    FACTORY_LANGUAGE_HUNGARIAN,         //24
    FACTORY_LANGUAGE_LATVIAN,
    FACTORY_LANGUAGE_LITHUANIAN,
    FACTORY_LANGUAGE_NORWEGIAN,
    FACTORY_LANGUAGE_POLISH,
    FACTORY_LANGUAGE_ROMANIAN,          //29
    FACTORY_LANGUAGE_SERBIAN,
    FACTORY_LANGUAGE_SLOVAK,
    FACTORY_LANGUAGE_SLOENIAN,
    FACTORY_LANGUAGE_THAI,
    FACTORY_LANGUAGE_UKRAINIAN,         //34
    FACTORY_LANGUAGE_VIETNAMESE,        
    FACTORY_LANGUAGE_NEDERLANDS,
    FACTORY_LANGUAGE_MAX,
} drvFactoryLanguageType;
#endif

typedef enum
{
    DEVICE_VGA1,        // 0
    DEVICE_DVI1,        // 1
    DEVICE_HDMI1,   // 2
    DEVICE_DP1,         // 3
    DEVICE_VGA2,        // 4
    DEVICE_DVI2,        // 5
    DEVICE_HDMI2,   // 6
    DEVICE_DP2,         // 7
    DEVICE_EEPROM = 0x10,
    DEVICE_HDCP1,
    DEVICE_HDCP2,
    DEVICE_OSDSN,
    DEVICE_OSDMODELNAME,

    DEVICE_NUMS = 0xFF
} drvFactoryDeviceType;

typedef enum
{
    VGA1_EDID = BIT7,
    VGA2_EDID = BIT6,
    DVI1_EDID = BIT5,
    DVI2_EDID = BIT4,
    HDMI1_EDID = BIT3,
    HDMI2_EDID = BIT2,
    DP1_EDID = BIT1,
    DP2_EDID = BIT0,
} drvFactoryInputPortEDID;

#define DDC2BI_PKTSIZE              DDCBuffer[0]
#define DDC2BI_PRE1                     DDCBuffer[1]
#define DDC2BI_PRE2                     DDCBuffer[2]
#define DDC2BI_CMD                  DDCBuffer[3]
#define DDC2BI_EXT                      DDCBuffer[4]
#define DDC2BI_DATA1                DDCBuffer[5]
#define DDC2BI_DATA2                DDCBuffer[6]
#define DDC2BI_DATA3                DDCBuffer[7]

#define DDC2BI_TPVALUE              DDCBuffer[4]
#define DDC2BI_VALUEH                   DDCBuffer[5]
#define DDC2BI_VALUEL                   DDCBuffer[6]
#define DDC2BI_RETVALUEH            DDCBuffer[7]
#define DDC2BI_RETVALUEL            DDCBuffer[8]

#if ENABLE_NEWDFM_REQUEST
#define DDC2BI_REPLYBYTE            DDCBuffer[1]
#define DDC2BI_FDCRCHI                  DDCBuffer[2]
#define DDC2BI_FDCRCLO              DDCBuffer[3]

#define DDC2BI_DEVICEADDRHI         DDCBuffer[1]
#define DDC2BI_DEVICEADDRLO             DDCBuffer[2]
#define DDC2BI_DEVICEADDRLEN        DDCBuffer[3]
#endif


#define TPV_GETVCP          0x01
#define TPV_SETVCP          0x03

#define TPV_FCode01         0xE1
#define TPV_FCode02         0xE2

#define TPV_FCode08         0xE8
#define TPV_FCode0A               0xEA
#define TPV_FCode0B         0xEB
#define TPV_FCode0E         0xEE
#define TPV_FCode0F         0xEF

#define TPV_FactoryMode     0xA0




extern BYTE XDATA g_ucProductionFlag;
extern WORD XDATA g_wFDCRCValue;
extern bit bWriteEnableFlag;

extern WORD xdata TCRCvalue;
extern WORD xdata TCRCCounter;

extern bit DFM_WRITECMD_REPLY_FLAG;
#define SET_DFM_WRITECMD_REPLY_FLAG()           (DFM_WRITECMD_REPLY_FLAG = 1)
#define CLR_DFM_WRITECMD_REPLY_FLAG()           (DFM_WRITECMD_REPLY_FLAG = 0)

//extern WORD CRC16( BYTE nData, WORD nCRC );
//extern BYTE drvFactoryAlignAOC( void );
#endif
