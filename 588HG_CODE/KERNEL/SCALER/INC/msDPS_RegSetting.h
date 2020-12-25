//===================================================================================================================================================================:
#ifndef _MSDPSREG_H_
#define _MSDPSREG_H_
//===================================================================================================================================================================:

#if CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||\
	CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF
#define REG_ADDR_MWE_ENABLE                 SC0_5C
#define REG_ADDR_INTSTA                     SC0_CC
#define REG_ADDR_INTENB                     SC0_CE
#define REG_ADDR_HISTOGRAM_Acceler_ENABLE   SC7_B5
#define REG_ADDR_HSLU_AP                    SC3_B9
#define REG_ADDR_HISTOGRAM_DATA_32          SC8_C0
#define REG_ADDR_MAIN_CSC_ENABLE            SC7_40
#define REG_ADDR_MAIN_COLOR_MATRIX_ENABLE   SC8_81
#ifdef _ML_ENABLE_
#define REG_ADDR_SC_BANK 					0x2F00
#define REG_ADDR_SC_COLOR_MATRIX_MAIN   	0x2F82
#define REG_ADDR_SC_COLOR_MATRIX_SUB    	0x2F6E
#else
#define REG_ADDR_COLOR_MATRIX_Main          SC8_82
#define REG_ADDR_COLOR_MATRIX_Sub           SC8_6E
#endif
#define REG_ADDR_HISTOGRAM_RANGE_H_ENABLE   SC7_27 // [7]&[6]
#define REG_ADDR_HISTOGRAM_RANGE_M_HST      SC7_24 // Low Byte
#define REG_ADDR_HISTOGRAM_RANGE_M_HEN      SC7_26 // Low Byte
#define REG_ADDR_HISTOGRAM_RANGE_V_ENABLE   SC7_B5 // [0]
#define REG_ADDR_HISTOGRAM_RANGE_M_VST      SC7_B6 // Low Byte, High Byte in B9[0]
#define REG_ADDR_HISTOGRAM_RANGE_M_VEN      SC7_B7 // Low Byte, High Byte in B9[2]
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L      SC7_36
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H      SC7_37

#if CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ ||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L    //removed
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H
#else
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L    SC7_34
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H    SC7_35
#endif
#define REG_ADDR_MAIN_MIN_VALUE             SC7_39
#define REG_ADDR_MAIN_MAX_VALUE             SC7_3A
#define REG_ADDR_DLC_HANDSHAKE              SC7_B8
#define REG_ADDR_MAIN_ICC_DITHER_EN         SC9_02 //[2]
#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF
#define REG_ADDR_MAIN_NOISEMASK_DITHER_EN    //removed
#define REG_ADDR_HIST_BF_PK_EN          SC7_B8  //Histogram source before/after peaking. 1: Before peaking. 0: After peaking.
#else
#define REG_ADDR_MAIN_NOISEMASK_DITHER_EN   SC7_D0 //[6]
#endif
#elif CHIP_ID == CHIP_TSUMB || CHIP_ID == CHIP_TSUMY
#define REG_ADDR_MWE_ENABLE                 SC0_5C
#define REG_ADDR_INTSTA                     SC0_CC
#define REG_ADDR_INTENB                     SC0_CE
#define REG_ADDR_HISTOGRAM_Acceler_ENABLE   SC3_B5
#define REG_ADDR_HSLU_AP                    SC3_B9
#define REG_ADDR_HISTOGRAM_ADDR_PORT        SC3_A9
#define REG_ADDR_HISTOGRAM_DATA_PORT        SC3_AA
#define REG_ADDR_MAIN_CSC_ENABLE            SC0_02
#define REG_ADDR_MAIN_COLOR_MATRIX_ENABLE   SC3_70
#define REG_ADDR_COLOR_MATRIX_Main          SC3_5E//SCE_05
#define REG_ADDR_COLOR_MATRIX_Sub           SC3_5E//SCE_25
#define REG_ADDR_HISTOGRAM_RANGE_H_ENABLE   SC3_27 // [7]&[6]
#define REG_ADDR_HISTOGRAM_RANGE_M_HST      SC3_24 // Low Byte
#define REG_ADDR_HISTOGRAM_RANGE_M_HEN      SC3_26 // Low Byte
#define REG_ADDR_HISTOGRAM_RANGE_V_ENABLE   SC3_B5 // [0]
#define REG_ADDR_HISTOGRAM_RANGE_M_VST      SC3_B6 // Low Byte, High Byte in B9[0]
#define REG_ADDR_HISTOGRAM_RANGE_M_VEN      SC3_B7 // Low Byte, High Byte in B9[2]
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L      SC3_36
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H      SC3_37
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L    SC3_34
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H    SC3_35
#define REG_ADDR_MAIN_MIN_VALUE             SC3_39
#define REG_ADDR_MAIN_MAX_VALUE             SC3_3A
#define REG_ADDR_DLC_HANDSHAKE              SC3_B8
// unused register
#define REG_ADDR_HISTOGRAM_DATA_32          SC6_C0
#define REG_ADDR_HISTOGRAM_RGB_ENABLE       SC3_3B // [6]
#define REG_ADDR_DLC_RGB_ENABLE             SC3_BB // [7]
#define REG_ADDR_MAIN_ICC_DITHER_EN         SC6_30 //[3]
#define REG_ADDR_MAIN_NOISEMASK_DITHER_EN   SC3_4B //[6]
#elif CHIP_ID == CHIP_TSUMU
#define REG_ADDR_COLOR_MATRIX_Main          SC8_82
#define REG_ADDR_COLOR_MATRIX_Sub           SC8_6E
#define REG_ADDR_HISTOGRAM_RANGE_H_ENABLE   SC7_27
#define REG_ADDR_HISTOGRAM_RANGE_M_HST      SC7_24
#define REG_ADDR_HISTOGRAM_RANGE_M_HEN      SC7_26
#define REG_ADDR_HISTOGRAM_RANGE_V_ENABLE   SC7_B5
#define REG_ADDR_HISTOGRAM_RANGE_M_VST      SC7_B6
#define REG_ADDR_HISTOGRAM_RANGE_M_VEN      SC7_B7
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L      SC7_36
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H      SC7_37
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L    SC7_34
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H    SC7_35
#define REG_ADDR_MAIN_MIN_VALUE             SC7_39
#define REG_ADDR_MAIN_MAX_VALUE             SC7_3A
#define REG_ADDR_DLC_HANDSHAKE              SC7_B8
#define REG_ADDR_MWE_ENABLE                 SC3_17
#define REG_ADDR_INTSTA                     SC0_CC
#define REG_ADDR_INTENB                     SC0_CE
#define REG_ADDR_HISTOGRAM_Acceler_ENABLE   SC7_B5
#define REG_ADDR_HSLU_AP                    SC7_B9
#define REG_ADDR_HISTOGRAM_DATA_32          SC8_C0
#define REG_ADDR_MAIN_CSC_ENABLE            SC7_40
#define REG_ADDR_MAIN_COLOR_MATRIX_ENABLE   SC8_81
#define REG_ADDR_COLOR_MATRIX_Main          SC8_82
#define REG_ADDR_COLOR_MATRIX_Sub           SC8_6E
// unused register
#define REG_ADDR_HISTOGRAM_RGB_ENABLE       SC3_3B // [6]
#define REG_ADDR_DLC_RGB_ENABLE             SC3_BB // [7]

#define REG_ADDR_MAIN_ICC_DITHER_EN         SC9_02 //[2]
#define REG_ADDR_MAIN_NOISEMASK_DITHER_EN   SC7_D0 //[6
#endif
//===================================================================================================================================================================:
#endif
//===================================================================================================================================================================:
