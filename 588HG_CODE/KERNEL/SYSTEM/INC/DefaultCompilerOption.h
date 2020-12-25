#ifndef _DEFAULT_COMPILER_OPTION_H_
#define _DEFAULT_COMPILER_OPTION_H_

////////////////////////////////////////////////////////////
// SI
////////////////////////////////////////////////////////////
#ifndef ODM_NAME
#define ODM_NAME                                ODM_AOC
#endif

#ifndef FACTORYALIGN_TYPE
#define FACTORYALIGN_TYPE                       FACTORYALIGN_DDCCI
#endif

#ifndef ENABLE_POWEROFF_1X_MCU_CLK
#define ENABLE_POWEROFF_1X_MCU_CLK              0
#endif
////////////////////////////////////////////////////////////
// APPLICATION
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// MCU
////////////////////////////////////////////////////////////
#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG        0
#endif

#ifndef UART1
#define UART1               0
#endif




/////////////////////////////////////////////////////////////
// ADC
/////////////////////////////////////////////////////////////
#ifndef ENABLE_ADC_RESET
#define ENABLE_ADC_RESET    1 // 120413 coding addition
#endif

#ifndef ADC_HSYNC_LVL_DEF // each main board may have different setting
#define ADC_HSYNC_LVL_DEF   ADC_HSYNC_LVL_5 // 120725 coding, default set to 5 to compatible with old chips
#endif

#ifndef ENABLE_ADC_DITHERING // ADC dither to fix horizontal/Vertical line noise
#define ENABLE_ADC_DITHERING    1
#endif

////////////////////////////////////////////////////////////
// SCALAR
////////////////////////////////////////////////////////////
#ifndef ENABLE_LBCLK_FIXED_216M
#define ENABLE_LBCLK_FIXED_216M 1
#endif

////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
#ifndef MS_VGA_SOG_EN
#define MS_VGA_SOG_EN               0
#endif

#ifndef DVI_RB_SWAP
#define DVI_RB_SWAP                 0
#endif

#ifndef DVI_PN_SWAP
#define DVI_PN_SWAP                 0
#endif

#ifndef HDMI_RB_SWAP
#define HDMI_RB_SWAP                0
#endif

#ifndef HDMI_PN_SWAP
#define HDMI_PN_SWAP                0
#endif

#ifndef ENABLE_MHL_C1
#define ENABLE_MHL_C1               0
#endif

#ifndef ENABLE_MHL_C2
#define ENABLE_MHL_C2               0
#endif

#ifndef ENABLE_MHL_C3
#define ENABLE_MHL_C3               0
#endif

#ifndef ENABLE_MHL
#define ENABLE_MHL                  0
#endif

#ifndef ENABLE_MHL_CTS_TEST
#define ENABLE_MHL_CTS_TEST         0
#endif

#ifndef ENABLE_DP_CTS_TEST
#define ENABLE_DP_CTS_TEST         0
#endif

#ifndef ENABLE_EDP_OUTPUT
#define ENABLE_EDP_OUTPUT         0
#endif

#ifndef ENABLE_DISPLAY_UNDERSCAN
#define ENABLE_DISPLAY_UNDERSCAN    0
#endif

#ifndef DISPLAY_UNDERSCAN_SPACE
#define DISPLAY_UNDERSCAN_SPACE     0// unit: pixel
#endif

#ifndef DISPLAY_UNDERSCAN_FREQ
#define DISPLAY_UNDERSCAN_FREQ      480 // 48Hz
#endif

#ifndef ENABLE_AUTOEQ
#define ENABLE_AUTOEQ               0
#endif

#ifndef ENABLE_RTE
#define ENABLE_RTE                  0
#endif

#ifndef  DP_AUX_PortB_PNSWAP
#define DP_AUX_PortB_PNSWAP      0
#endif


#ifndef  DP_AUX_PortC_PNSWAP
#define DP_AUX_PortC_PNSWAP      0
#endif

#ifndef Enable_Multi_OD
#define Enable_Multi_OD             0
#endif

#ifndef ENABLE_TNR
#define ENABLE_TNR                  0
#endif

#ifndef USE_VCTRL
#define USE_VCTRL                   0
#endif

#ifndef USE_EXTERNAL_LDO
#define USE_EXTERNAL_LDO            0
#endif

#ifndef DHDMI_SIMPLAYHD_PATCH
#define DHDMI_SIMPLAYHD_PATCH       0
#endif

#ifndef ENABLE_SPDIF
#define ENABLE_SPDIF                1
#endif

#ifndef ENABLE_SW_RESET_HDCP_FSM
#define ENABLE_SW_RESET_HDCP_FSM    (CHIP_ID == CHIP_TSUMB || CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMV ||CHIP_ID == CHIP_TSUMJ)
#endif
////////////////////////////////////////////////////////////
// AUTO FUNC.
////////////////////////////////////////////////////////////
#ifndef ENABLE_HWAUTO_ADCOFFSET
#define ENABLE_HWAUTO_ADCOFFSET     1
#endif

#ifndef DISABLE_AUTO_SWITCH
#define DISABLE_AUTO_SWITCH         0
#endif

////////////////////////////////////////////////////////////
// POWERMANAGEMENT
////////////////////////////////////////////////////////////
#ifndef MS_PM
#define MS_PM                       0
#endif

#ifndef PM_StbyMcuAliveForClockExist
#define PM_StbyMcuAliveForClockExist    0
#endif

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#ifndef HDCPKEY_USE_CODE
#define HDCPKEY_USE_CODE            1
#endif

#ifndef _NEW_SOG_DET_
#define _NEW_SOG_DET_               0
#endif

#ifndef ENABLE_WATCH_DOG
#define ENABLE_WATCH_DOG            (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
#endif
#ifndef ENABLE_WATCH_DOG_INT
#define ENABLE_WATCH_DOG_INT            0
#endif

#ifndef ENABLE_R2_2DTO3D
#define ENABLE_R2_2DTO3D            0
#endif

#ifndef ENABLE_USB_INPUT
#define ENABLE_USB_INPUT               0
#endif

#ifndef ENABLE_OSD_3D_CONTROL
#define ENABLE_OSD_3D_CONTROL       0
#endif

#ifndef ENABLE_R2_CODE_ON_DRAM
#define ENABLE_R2_CODE_ON_DRAM      0
#endif

#ifndef ENBLAE_BDMA_INT
#define ENBLAE_BDMA_INT             0
#endif

#ifndef ENABLE_POWER_BOARD_CONTROL
#define ENABLE_POWER_BOARD_CONTROL  0
#endif

#ifndef LGE_DEMO
#define LGE_DEMO  0
#endif

#ifndef ENABLE_FLASH_CURRENT_ADJUSTMENT
#define ENABLE_FLASH_CURRENT_ADJUSTMENT   (CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ||CHIP_ID==CHIP_TSUM9|| CHIP_ID == CHIP_TSUMF)
#endif

#define FLASH_DRIVING_1mA       0
#define FLASH_DRIVING_2mA       1
#define FLASH_DRIVING_3mA       3
#define FLASH_DRIVING_4mA       2

#ifndef FLASH_DEFAULT_CURRENT
#define FLASH_DEFAULT_CURRENT   FLASH_DRIVING_2mA
#endif
////////////////////////////////////////////////////////////
// DDCCI
////////////////////////////////////////////////////////////
#ifndef DDCCI_FILTER_FUNCTION
#define DDCCI_FILTER_FUNCTION       0
#endif

#ifndef DDCCI_REPLY_NULL_MESSAGE // wait for coding
#define DDCCI_REPLY_NULL_MESSAGE    0 // set to reply null message for special case
#endif



////////////////////////////////////////////////////////////
// application
////////////////////////////////////////////////////////////
// input TMDS of some source may change during cpature window or set panel timing
// record input DE/start and apply to display setting of SC0_05 ~ 0C
#ifndef ENABLE_CHECK_TMDS_DE
#define ENABLE_CHECK_TMDS_DE    1
#endif

////////////////////////////////////////////////////////////
// external device
////////////////////////////////////////////////////////////
//============= TOUCH KEY==============================
#ifndef ENABLE_TOUCH_KEY
#define ENABLE_TOUCH_KEY            0
#endif

#define TOUCH_KEY_SOURCE_ITE        0
#define TOUCH_KEY_SOURCE_SMSC       1
#define TOUCH_KEY_SOURCE_CYPRESS    2
#ifndef TOUCH_KEY_SOURCE
#define TOUCH_KEY_SOURCE            TOUCH_KEY_SOURCE_ITE
#endif

#if ENABLE_TOUCH_KEY && (TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_ITE)
#define ENABLE_LOW_CONTACT          1
#else
#define ENABLE_LOW_CONTACT          0
#endif

#ifndef MOBILE_INTERFERENCE
#define MOBILE_INTERFERENCE         0
#endif

#ifndef TOUCH_KEY_CTRL_LED
#define TOUCH_KEY_CTRL_LED          0
#endif

#ifndef TOUCH_KEY_POWER_KEY_DEBOUNCE
#define TOUCH_KEY_POWER_KEY_DEBOUNCE    0
#endif

#define POWER_KEY_DEBOUNCE_PEROID   10 // unit: 50ms
//==========================================================
//==============TOUCH PANEL==================================
#ifndef ENABLE_TOUCH_PANEL
#define ENABLE_TOUCH_PANEL          0
#endif

#ifndef ENABLE_TOUCH_PANEL_DATA_FROM_USB
#define ENABLE_TOUCH_PANEL_DATA_FROM_USB     0
#endif

#ifndef ENABLE_TOUCH_PANEL_CTRL_OSD
#define ENABLE_TOUCH_PANEL_CTRL_OSD     0
#endif
//=============================================================


#ifndef ENABLE_TNR
#define ENABLE_TNR      0 // SPF function with motion mode
#endif

#ifndef ENABLE_VC_8_5
#define ENABLE_VC_8_5       1 // default enable
#endif

#ifndef ENABLE_OSD_ROTATION
#define ENABLE_OSD_ROTATION       1 
#define _OSD_ROTATION_180_       //(1&&ENABLE_OSD_ROTATION)
#define _OSD_ROTATION_270_       //(1&&ENABLE_OSD_ROTATION)
#endif


////////////////////////////////////////////////////////////
// OTHER
////////////////////////////////////////////////////////////
#ifndef PANEL_MINI_LVDS
#define PANEL_MINI_LVDS             0
#endif

#ifndef DEPOP_DCONOFF_FLOW
#define DEPOP_DCONOFF_FLOW             0
#endif

#ifndef AdjustVolume_UseTable
#define AdjustVolume_UseTable       0
#endif

#ifndef ENABLE_DeltaE
#define ENABLE_DeltaE 0
#endif

// 120119 coding test for TPV
// 0: brightness control by PWM duty
// 1: brightness control by string current and fix PWM duty to max
#ifndef LED_CTRL_BRIGHTNESS_BY_CURRENT
#define LED_CTRL_BRIGHTNESS_BY_CURRENT  0
#endif
#ifndef ENABLE_3DLUT
#define ENABLE_3DLUT                0
#endif

#ifndef ENABLE_XTAL_LESS
#define ENABLE_XTAL_LESS            0
#endif

#ifndef ENABLE_FREESYNC
#define ENABLE_FREESYNC            0
#endif

#ifndef ENABLE_CABLE_5V_EDID
#define ENABLE_CABLE_5V_EDID     1//  0
#endif

#ifndef ENABLE_CTS_HPD_PD100K
#define ENABLE_CTS_HPD_PD100K			0
#endif

#ifndef ENABLE_IR_CONTROL
#define ENABLE_IR_CONTROL			0
#endif

#ifndef ENABLE_DAC_DEPOP
#define ENABLE_DAC_DEPOP			0
#endif

#endif

