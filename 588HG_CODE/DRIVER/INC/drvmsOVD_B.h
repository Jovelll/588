#ifndef _MSOVD_B_H_
#define _MSOVD_B_H_

#ifdef _MSOVD_B_C_
#define _MSOVDDEC_B_
#else
#define _MSOVDDEC_B_ extern
#endif



//*************************************************************************************************
//Description: OD look-up table loader.
//
//           ex: msLoadODT(tOverDrive)
//
//           It provides two kinds of format (normal and compressed) for OD look-up table.
//           The look-up table and OD lib should be located in the same bank!!
_MSOVDDEC_B_ void drvmsOverDriverInit(void);
_MSOVDDEC_B_ void drvmsLoadODTable( BYTE *pODTbl);
_MSOVDDEC_B_ void drvmsOverDriveOnOff(BOOL bEnable);

//*************************************************************************************************
#undef _MSOVDDEC_B_
#endif
