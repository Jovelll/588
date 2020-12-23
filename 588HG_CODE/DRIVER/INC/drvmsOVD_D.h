#ifndef _MSOVD_D_H_
#define _MSOVD_D_H_

#ifdef _MSOVD_D_C_
  #define _MSOVDDEC_D_
#else
  #define _MSOVDDEC_D_ extern
#endif

//*************************************************************************************************
// Description: OD look-up table loader.
//
// ex: msLoadODT(tOverDrive)
//
// It provides two kinds of format (normal and compressed) for OD look-up table.
// The look-up table and OD lib should be located in the same bank!!


_MSOVDDEC_D_ void drvmsOverDriverInit(void);
_MSOVDDEC_D_ void drvmsLoadODTable( BYTE *pODTbl);
_MSOVDDEC_D_ void drvmsOverDriveOnOff(Bool bEnable);

#undef _MSOVDDEC_D_
#endif
