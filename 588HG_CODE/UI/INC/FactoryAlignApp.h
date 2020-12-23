#ifndef FACTORYALIGNAPP_H
#define FACTORYALIGNAPP_H

#ifdef FACTORYALIGNAPP_C
#define _FACTORYALIGNAPP_DEC_
#else
#define _FACTORYALIGNAPP_DEC_   extern
#endif

//_FACTORYALIGNAPP_DEC_ BYTE xdata LoadEDIDSelectPort;

#if 0
_FACTORYALIGNAPP_DEC_ Bool appFactoryAdjBrightness( WORD wData );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetBrightness( void );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAdjContrast( WORD wData );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetContrast( void );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAdjRedGain( WORD wData );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetRedGain( void );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAdjGreenGain( WORD wData );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetGreenGain( void );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAdjBlueGain( WORD wData );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetBlueGain( void );

_FACTORYALIGNAPP_DEC_ WORD appFactoryGetAdcRedGain( void );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetAdcGreenGain( void );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetAdcBlueGain( void );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetAdcRedOffset( void );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetAdcGreenOffset( void );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetAdcBlueOffset( void );

_FACTORYALIGNAPP_DEC_ Bool appFactorySaveColorPreset( drvFactoryColorTempType ucColorTemp );
_FACTORYALIGNAPP_DEC_ Bool appFactoryRecallColorPreset( drvFactoryColorTempType ucColorTemp );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAdjLanguage( ucLanguage );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAutoColor( void );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAutoSetup( void );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAllReset( void );
_FACTORYALIGNAPP_DEC_ Bool appFactorySetForcePowerSaving( void );

_FACTORYALIGNAPP_DEC_ Bool appFactorySetFactoryFlag( drvFactoryModeType ucControl );
_FACTORYALIGNAPP_DEC_ Bool appFactorySetBurninFlag( drvFactoryModeType ucControl );
_FACTORYALIGNAPP_DEC_ Bool appFactorySetProductModeFlag( drvFactoryModeType ucControl );
_FACTORYALIGNAPP_DEC_ Bool appFactoryAdjVcomValue( WORD wVcomValue );
_FACTORYALIGNAPP_DEC_ WORD appFactorySetEdidWriteStart( drvFactoryModeType ucControl );
_FACTORYALIGNAPP_DEC_ Bool appFactorySetHdcpWriteStart( void );

_FACTORYALIGNAPP_DEC_ void appFactoryHdcpWrite( WORD wIndex, WORD wBufSize, BYTE* buf, BYTE ucLen );
_FACTORYALIGNAPP_DEC_ WORD appFactoryGetHdcpCRC( void );
_FACTORYALIGNAPP_DEC_ void appFactoryWriteEDID( drvFactoryDeviceType ucDevice, WORD wAddr, BYTE* buf, BYTE ucLen );
_FACTORYALIGNAPP_DEC_ void appFactoryGetEDID( drvFactoryDeviceType ucDevice, WORD wAddr, BYTE* buf, BYTE ucLen );


_FACTORYALIGNAPP_DEC_ void appFactoryInitVariablesForWriteCmd( void );
#endif
#endif
