#ifndef _MSOVD_2_H_
#define _MSOVD_2_H_

#ifdef _MSOVD_2_C_
  #define _MSOVDDEC_2_
#else
  #define _MSOVDDEC_2_ extern
#endif

//*************************************************************************************************
// Description: OD look-up table loader.
//
// ex: msLoadODT(tOverDrive)
//
// It provides two kinds of format (normal and compressed) for OD look-up table.
// The look-up table and OD lib should be located in the same bank!!


_MSOVDDEC_2_ void drvmsOverDriverInit(void);
_MSOVDDEC_2_ void drvmsLoadODTable( BYTE *pODTbl);
_MSOVDDEC_2_ void drvmsOverDriveOnOff(Bool bEnable);
#undef _MSOVDDEC_2_
#endif
