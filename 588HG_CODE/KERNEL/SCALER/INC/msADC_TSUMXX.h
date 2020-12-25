#ifndef _MSADC_TSUMXX_H_
#define _MSADC_TSUMXX_H_       (1)

#ifdef _MSADC_TSUMXX_C_
#define _MSADC_TSUMXXDEC_
#else
#define _MSADC_TSUMXXDEC_ extern
#endif

#define MAX_PHASE_VALUE         	0x3F
#define MIN_PHASE_VALUE           	0
;
_MSADC_TSUMXXDEC_ void msADC_AdjustHTotal( WORD htotal );
_MSADC_TSUMXXDEC_ void msADC_AdjustPhase( BYTE phase );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcGain( BYTE rGain, BYTE gGain, BYTE bGain );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcOffset( BYTE rOffset, BYTE gOffset, BYTE bOffset );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcRedGain( BYTE gain );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcGreenGain( BYTE gain );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcBlueGain( BYTE gain );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcRedOffset( BYTE offset );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcGreenOffset( BYTE offset );
_MSADC_TSUMXXDEC_ void msADC_AdjustAdcBlueOffset( BYTE offset );


_MSADC_TSUMXXDEC_ Bool msADC_AutoAdjustAdcGain( BYTE vsyncTime );
#if ENABLE_HWAUTO_ADCOFFSET
_MSADC_TSUMXXDEC_ Bool msADC_ADCOffset_AutoCalibraton( void );
#else
_MSADC_TSUMXXDEC_ Bool msADC_AutoAdjustAdcOffset( BYTE vsyncTime );
#endif
_MSADC_TSUMXXDEC_ Bool msADC_AutoAdcColor( BYTE vsyncTime );

#endif 
