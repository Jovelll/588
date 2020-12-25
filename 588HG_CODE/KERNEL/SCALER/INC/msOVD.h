#ifndef _MSOVD_H_
#define _MSOVD_H_

#ifdef _MSOVD_C_
  #define _MSOVDDEC_
#else
  #define _MSOVDDEC_ extern
#endif


typedef enum
{
   _OD_Disable,
   _OD_Enable

}ODStatusType;

typedef enum
{
   _MIU_SSC_OFF,
   _MIU_SSC_40K_ZERO_POINT_FIVE_PERCENT,    //40k, 0.5%
   _MIU_SSC_40K_ONE_PERCENT,                //40K,1%
   _MIU_SSC_40K_TWO_PERCENT,                //40K,2%
   _MIU_SSC_40K_THREE_PERCENT               //40K,3%

}MIUSSCType;

//*************************************************************************************************
//Description: OD look-up table loader.
//
//           ex: msLoadODT(tOverDrive)
//
//           It provides two kinds of format (normal and compressed) for OD look-up table.
//           The look-up table and OD lib should be located in the same bank!!

_MSOVDDEC_ code BYTE tOverDrive[];
_MSOVDDEC_ code BYTE* tOverDrive_AllTbl[];

_MSOVDDEC_ void msOverDriverInit(void);
_MSOVDDEC_ void msRTE_LoadLUT(void);
_MSOVDDEC_ void msLoad_MultiODTable(void);
_MSOVDDEC_ void msOverDriveOnOff(BOOL bEnable);
#if (GLASSES_TYPE!=GLASSES_NONE)
_MSOVDDEC_ void msODWatchTrackPointValue(BOOL bAfterOD);
#endif
#endif
