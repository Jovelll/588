//===================================================================================================================================================================:
#ifndef _MSDSCREG_H_
#define _MSDSCREG_H_
//===================================================================================================================================================================:

#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF
#define REG_ADDR_MWE_ENABLE                             	SC0_5C
#define REG_ADDR_INTSTA                                   	SC0_CC
#define REG_ADDR_INTENB                                   	SC0_CE
#define REG_ADDR_SATHISTOGRAM_Acceler_ENABLE                SCF_21 // [1]
#define REG_ADDR_SATHISTOGRAM_DATA_32                      	SCF_40
#define REG_ADDR_xvYCC_MAIN_COLOR_MATRIX_ENABLE             SCE_02
#define REG_ADDR_xvYCC_SUB_COLOR_MATRIX_ENABLE              SCE_22
#define REG_ADDR_xvYCC_COLOR_MATRIX_Main                  	SCE_04
#define REG_ADDR_xvYCC_COLOR_MATRIX_Sub                    	SCE_24
#define REG_ADDR_SATHISTOGRAM_RANGE_H_ENABLE                SCF_21 // [7]&[6]
#define REG_ADDR_SATHISTOGRAM_RANGE_M_HST                   SCF_22 // Low Byte
#define REG_ADDR_SATHISTOGRAM_RANGE_M_HEN                   SCF_24 // Low Byte
#define REG_ADDR_SATHISTOGRAM_RANGE_V_ENABLE                SCF_21 // [4]
#define REG_ADDR_SATHISTOGRAM_RANGE_M_VST                   SCF_26 // Low Byte, High Byte in B9[0]
#define REG_ADDR_SATHISTOGRAM_RANGE_M_VEN                   SCF_28 // Low Byte, High Byte in B9[2]
#define REG_ADDR_SATHIS_HANDSHAKE                          	SCF_20
#define REG_ADDR_XVYCC_CM_SAT_HIST                          SCE_60 //[5]
#else
#define REG_ADDR_xvYCC_COLOR_MATRIX_Main
#define REG_ADDR_xvYCC_COLOR_MATRIX_Sub
#define REG_ADDR_SATHISTOGRAM_RANGE_H_ENABLE
#define REG_ADDR_SATHISTOGRAM_RANGE_M_HST
#define REG_ADDR_SATHISTOGRAM_RANGE_M_HEN
#define REG_ADDR_SATHISTOGRAM_RANGE_V_ENABLE
#define REG_ADDR_SATHISTOGRAM_RANGE_M_VST
#define REG_ADDR_SATHISTOGRAM_RANGE_M_VEN
#define REG_ADDR_SATHIS_HANDSHAKE
#define REG_ADDR_MWE_ENABLE
#define REG_ADDR_INTSTA
#define REG_ADDR_INTENB
#define REG_ADDR_SATHISTOGRAM_Acceler_ENABLE
#define REG_ADDR_SATHISTOGRAM_DATA_32
#define REG_ADDR_MAIN_COLOR_MATRIX_ENABLE
// unused register
#define REG_ADDR_HISTOGRAM_RGB_ENABLE        // [6]
#define REG_ADDR_DLC_RGB_ENABLE              // [7]
#endif
//===================================================================================================================================================================:
#endif
//===================================================================================================================================================================:
