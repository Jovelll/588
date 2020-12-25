#ifndef _MSOVD_F_H_
#define _MSOVD_F_H_

#ifdef _MSOVD_F_C_
  #define _MSOVDDEC_F_
#else
  #define _MSOVDDEC_F_ extern
#endif

#define TunePhaseByHistrogram 0
#define DebugPhaseToggle (0 && TunePhaseByHistrogram)

#if TunePhaseByHistrogram
extern BYTE xdata u8Motion,u8Motion_V;
#endif
#if DebugPhaseToggle
extern DWORD xdata g_u32EvenSim_H,g_u32OddSim_H,g_u32EvenSim_V,g_u32OddSim_V;
#endif
#define reg_phase_cur0_pre0_histogram_read_back   		REG_3640
#define reg_phase_cur1_pre1_histogram_read_back			REG_364C
#define reg_phase_cur0_pre1_histogram_read_back			REG_3644
#define reg_phase_cur1_pre0_histogram_read_back			REG_3648
#define reg_phase_cur0_pre0_histogram_read_back_v			REG_3650
#define reg_phase_cur1_pre1_histogram_read_back_v			REG_365C
#define reg_phase_cur0_pre1_histogram_read_back_v			REG_3654
#define reg_phase_cur1_pre0_histogram_read_back_v			REG_3658

//extern DWORD g_dwFactory;
extern void msOD_Histogram_Interrupt(void );

//*************************************************************************************************
// Description: OD look-up table loader.
//
// ex: msLoadODT(tOverDrive)
//
// It provides two kinds of format (normal and compressed) for OD look-up table.
// The look-up table and OD lib should be located in the same bank!!


_MSOVDDEC_F_ void drvmsOverDriverInit(void);
_MSOVDDEC_F_ void drvmsLoadODTable( BYTE *pODTbl);
_MSOVDDEC_F_ void drvmsOverDriveOnOff(Bool bEnable);

#undef _MSOVDDEC_F_
#endif
