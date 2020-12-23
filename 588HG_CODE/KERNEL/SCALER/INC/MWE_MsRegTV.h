/******************************************************************************
Copyright (c) 2003 MStar Semiconductor, Inc.
All rights reserved.
[Module Name]: MsReg.h
[Date]:        06-Nov-2003
[Comment]:
PICASSO registers definition.
[Reversion History]:
 *******************************************************************************/
//#include "..\inc\define.h"
#ifndef _MSREG_H_
#define _MSREG_H_
///////////////////////////////////////////////////////
// Register bank select
///////////////////////////////////////////////////////
#define BK0_00  0x00
#define REG_BANK_MASK       0x07     //
#define REG_BANK_SCALER     0x00     // Scaler bank
#define REG_BANK_ADC_DVI    0x01     // ADC/DVI receiver bank
#define REG_BANK_TCON       0x02     // Timing control bank
#define REG_BANK_PIP        0x03     // Video control bank
#define REG_BANK_MIU        0x04     // Video & MIU control bank
///////////////////////////////////////////////////////
// [Bank = 001]
// ADC/DVI receiver bank
///////////////////////////////////////////////////////
// Double buffer control
#define BK1_01  0x01 // enable dobule buffer while vertical blanking
// PLL divider control
#define BK1_02  0x02 // PLL divider ratio (horizontal total)
#define BK1_03  0x03
// Input R/G/B Gain
#define BK1_04  0x04
#define BK1_05  0x05
#define BK1_06  0x06
// Input R/G/B Offset
#define BK1_07  0x07
#define BK1_08  0x08
#define BK1_09  0x09
// Clamp timing
#define BK1_0A  0x0A // clamping placement based on ADC clock
#define BK1_0B  0x0B // clamping duration based on ADC clock
// General control
#define BK1_0C  0x0C
// PLL coefficient
#define BK1_0D  0x0D // PLL loop filter control
#define BK1_0E  0x0E
#define BK1_0F  0x0F
// Clock phase control
#define BK1_10  0x10 // b5-0: clock phase adjust(PHASECC + 8)
#define BK1_11  0x11 // b5-0: PHASECC(clock phase adjust)
// VCO control
#define BK1_12  0x12
#define BK1_13  0x13
// SOG/HSYNC programming level
#define BK1_14  0x14
#define BK1_15  0x15
// Status
#define BK1_16  0x16 //
#define BK1_18  0x18 // DVI termination resistor status in 2's complement(DVI feature only)
#define BK1_19  0x19 // DVI phase status indicator in 2's complement(DVI feature only)
#define BK1_1A  0x1A
// DVI override(DVI feature only)
#define BK1_1B  0x1B // freeze & override DVI red channel PLL phase selection
#define BK1_1C  0x1C // freeze & override DVI green channel PLL phase selection
#define BK1_1D  0x1D // freeze & override DVI blue channel PLL phase selection
#define BK1_1E  0x1E // DVI bit error status indicator
#define BK1_1F  0x1F // DVI bit error tolerance threshold
#define BK1_20  0x20
// b5-4: channel select for DVI error status indicator(DVI feature only)
#define DVI_RED_CHANNEL     0x00
#define DVI_GREEN_CHANNEL   0x10
#define DVI_BLUE_CHANNEL    0x20
#define BK1_21  0x21
#define BK1_22  0x22
#define BK1_23  0x23
#define BK1_24  0x24
#define BK1_25  0x25
#define BK1_26  0x26
#define BK1_27  0x27
#define BK1_28  0x28
#define BK1_29  0x29
#define BK1_2A  0x2A
#define BK1_2B  0x2B
#define BK1_2C  0x2C
#define BK1_2D  0x2D
#define BK1_2E  0x2E
#define BK1_2F  0x2F
// PLL control for video input
#define BK1_30  0x30
#define BK1_31  0x31
#define BK1_32  0x32
#define BK1_33  0x33
#define BK1_34  0x34
#define BK1_35  0x35
#define BK1_36  0x36
#define BK1_37  0x37
#define BK1_38  0x38
#define BK1_3E  0x3E   // Rex 100706    
#define BK1_3F  0x3F
#define BK1_40  0x40
#define BK1_41  0x41
#define BK1_42  0x42
#define BK1_43  0x43
// HDMI status registers
#define BK1_80  0x80
#define BK1_81  0x81
// HDMI error registers
#define BK1_83  0x83
#define BK1_84  0x84
// HDMI config registers
#define BK1_86  0x86
#define BK1_87  0x87
#define BK1_88  0x88
// HDMI test Mux control registers
#define BK1_8A  0x8A
// HDMI audio PLL control registers
#define BK1_8C  0x8C
#define BK1_8D  0x8D     // audio PLL test controll [7:0]
#define BK1_8E  0x8E     // audio PLL test controll [15:8]
// HDMI frequency compare registers
#define BK1_8F  0x8F     // 50M count value [7:0]
#define BK1_90  0x90     // 50M count value [15:8]
#define BK1_91  0x91     // 100M count value [7:0]
#define BK1_92  0x92     // 100M count value [15:8]
#define BK1_93  0x93     // 200M count value [7:0]
#define BK1_94  0x94     // 200M count value [15:8]
// HDMI packet type registers
#define BK1_95  0x95
// HDMI pixel clock frquency registers
#define BK1_98  0x98     // counter outputs plus overflow [7:0]
#define BK1_99  0x99
// HDMI audio clock registers
#define BK1_9A  0x9A // CTS for audio clock regeneration [7:0]
#define BK1_9B  0x9B // CTS for audio clock regeneration [15:8]
#define BK1_9C  0x9C // CTS for audio clock regeneration [19:16]
#define BK1_9D  0x9D // N for audio clock regeneration [7:0]
#define BK1_9E  0x9E // N for audio clock regeneration [15:8]
#define BK1_9F  0x9F // N for audio clock regeneration [19:16]
// HDMI general control registers
#define BK1_A0  0xA0
// HDMI AVI IF registers
#define BK1_A1  0xA1
#define BK1_A2  0xA2
#define BK1_A3  0xA3
#define BK1_A4  0xA4
#define BK1_A5  0xA5 // line number of end of top bar [7:0]
#define BK1_A6  0xA6 // line number of end of top bar [15:8]
#define BK1_A7  0xA7 // line number of start of bottom bar [7:0]
#define BK1_A8  0xA8 // line number of start of bottom bar [15:8]
#define BK1_A9  0xA9 // pixel number of end of left bar [7:0]
#define BK1_AA  0xAA // pixel number of end of left bar [15:8]
#define BK1_AB  0xAB // pixel number of start of right bar [7:0]
#define BK1_AC  0xAC // pixel number of start of right bar [15:8]
// HDMI audio IF registers
#define BK1_AD  0xAD
#define BK1_AE  0xAE
#define BK1_AF  0xAF // max bit rate depending on AUD_CT
#define BK1_B0  0xB0 // channel allocation
#define BK1_B1  0xB1
// HDMI SPD IF registers
#define BK1_B2  0xB2 // b6-0: (SPD_VN1) vendor name character 1
#define BK1_B3  0xB3 // b6-0: (SPD_VN2) vendor name character 2
#define BK1_B4  0xB4 // b6-0: (SPD_VN3) vendor name character 3
#define BK1_B5  0xB5 // b6-0: (SPD_VN4) vendor name character 4
#define BK1_B6  0xB6 // b6-0: (SPD_VN5) vendor name character 5
#define BK1_B7  0xB7 // b6-0: (SPD_VN6) vendor name character 6
#define BK1_B8  0xB8 // b6-0: (SPD_VN7) vendor name character 7
#define BK1_B9  0xB9 // b6-0: (SPD_VN8) vendor name character 8
#define BK1_BA  0xBA // b6-0: (SPD_PD1) product description character 1
#define BK1_BB  0xBB // b6-0: (SPD_PD2) product description character 2
#define BK1_BC  0xBC // b6-0: (SPD_PD3) product description character 3
#define BK1_BD  0xBD // b6-0: (SPD_PD4) product description character 4
#define BK1_BE  0xBE // b6-0: (SPD_PD5) product description character 5
#define BK1_BF  0xBF // b6-0: (SPD_PD6) product description character 6
#define BK1_C0  0xC0 // b6-0: (SPD_PD7) product description character 7
#define BK1_C1  0xC1 // b6-0: (SPD_PD8) product description character 8
#define BK1_C2  0xC2 // b6-0: (SPD_PD9) product description character 9
#define BK1_C3  0xC3 // b6-0: (SPD_PD10) product description character 10
#define BK1_C4  0xC4 // b6-0: (SPD_PD11) product description character 11
#define BK1_C5  0xC5 // b6-0: (SPD_PD12) product description character 12
#define BK1_C6  0xC6 // b6-0: (SPD_PD13) product description character 13
#define BK1_C7  0xC7 // b6-0: (SPD_PD14) product description character 14
#define BK1_C8  0xC8 // b6-0: (SPD_PD15) product description character 15
#define BK1_C9  0xC9 // b6-0: (SPD_PD16) product description character 16
#define BK1_CA  0xCA // (SPD_SDI) source device information
// HDMI MPEG IF registers
#define BK1_CB  0xCB // (MPEG_MB) MPEG bit rate [7:0]
#define BK1_CC  0xCC // (MPEG_MB) MPEG bit rate [15:8]
#define BK1_CD  0xCD // (MPEG_MB) MPEG bit rate [23:16]
#define BK1_CE  0xCE // (MPEG_MB) MPEG bit rate [31:24]
#define BK1_CF  0xCF
// HDMI vendor specific IF registers
#define BK1_D0  0xD0 // vendor specific InfoFrame length
#define BK1_D1  0xD1 // payload byte 0
#define BK1_D2  0xD2 // payload byte 1
#define BK1_D3  0xD3 // payload byte 2
#define BK1_D4  0xD4 // payload byte 3
#define BK1_D5  0xD5 // payload byte 4
#define BK1_D6  0xD6 // payload byte 5
#define BK1_D7  0xD7 // payload byte 6
#define BK1_D8  0xD8 // payload byte 7
#define BK1_D9  0xD9 // payload byte 8
#define BK1_DA  0xDA // payload byte 9
#define BK1_DB  0xDB // payload byte 10
#define BK1_DC  0xDC // payload byte 11
#define BK1_DD  0xDD // payload byte 12
#define BK1_DE  0xDE // payload byte 13
#define BK1_DF  0xDF // payload byte 14
#define BK1_E0  0xE0 // payload byte 15
#define BK1_E1  0xE1 // payload byte 16
#define BK1_E2  0xE2 // payload byte 17
#define BK1_E3  0xE3 // payload byte 18
#define BK1_E4  0xE4 // payload byte 19
#define BK1_E5  0xE5 // payload byte 20
#define BK1_E6  0xE6 // payload byte 21
#define BK1_E7  0xE7 // payload byte 22
#define BK1_E8  0xE8 // payload byte 23
#define BK1_E9  0xE9 // payload byte 24
#define BK1_EA  0xEA // payload byte 25
#define BK1_EB  0xEB // payload byte 26
#define BK1_EC  0xEC // payload byte 27
///////////////////////////////////////////////////////
// [Bank = 000]
// Scaler register
///////////////////////////////////////////////////////
// double buffer control
#define BK0_01  0x01
// [Graphic port register]
// input ssource select/control
#define BK0_02  0x02
// b1-0: input select
#define SCALER_ISEL_MASK    0x07
#define SCALER_ISEL_ANALOG1 0x00
#define SCALER_ISEL_ANALOG2 0x01
#define SCALER_ISEL_ANALOG3 0x02
#define SCALER_ISEL_DVI     0x03
#define SCALER_ISEL_VIDEO   0x04
#define SCALER_ISEL_HDMI    0x07
// b6-5: input sync type
#define SCALER_STYPE_SOG    0x60
#define BK0_03  0x03
#define BK0_04  0x04
// input image sample range
#define BK0_05  0x05 // image vertical sample start point, count by input HSync
#define BK0_06  0x06
#define BK0_07  0x07 // image horizontal sample start point, count by input dot clock
#define BK0_08  0x08
#define BK0_09  0x09 // image vertical resolution(vertical display enable area count by line)
#define BK0_0A  0x0A
#define BK0_0B  0x0B // image horizontal resolution(horizontal display enable area count by pixel)
#define BK0_0C  0x0C
// digital video input control
#define BK0_0D  0x0D
// input lock pointer
#define BK0_0F  0x0F // b3-0: lock Y line
// [display timing register]
// ouput DE size
#define BK0_10  0x10 // output DE vertical start
#define BK0_11  0x11
#define BK0_12  0x12 // oouput DE horizontal start
#define BK0_13  0x13
#define BK0_14  0x14 // output DE vertical end
#define BK0_15  0x15
#define BK0_16  0x16 // oouput DE horizontal end
#define BK0_17  0x17
// scaling image window size
#define BK0_18  0x18 // scaling image window horizontal start
#define BK0_19  0x19
#define BK0_1A  0x1A // scaling image window vertical end
#define BK0_1B  0x1B
#define BK0_1C  0x1C // scaling image window horizontal end
#define BK0_1D  0x1D
// output sync timing
#define BK0_1E  0x1E // ouput vertical total
#define BK0_1F  0x1F
#define BK0_20  0x20 // ouput VSync start
#define BK0_21  0x21
#define BK0_22  0x22 // ouput VSync end
#define BK0_23  0x23
#define BK0_24  0x24 // ouput horizontal total
#define BK0_25  0x25
#define BK0_26  0x26 // output HSync pulse width
// output sync control
#define BK0_27  0x27
#define BK0_28  0x28
// Mode 3 control
#define BK0_29  0x29 // mode 3 lock point [7:0]
#define BK0_2A  0x2A // b2-0: mode 3 lock point [10:8]
#define BK0_2B  0x2B // small lock range
#define BK0_2C  0x2C
#define BK0_2D  0x2D // mode 3 VTOTAL [7:0]
#define BK0_2E  0x2E // b2-0: mode 3 VTOTAL [10:8]
#define BK0_2F  0x2F
#define BK0_30  0x30 // mode 3 error [7:0]
#define BK0_31  0x31 // b2-0: mode 3 error [10:8]
// border color
#define BK0_32  0x32
#define BK0_33  0x33 // red
#define BK0_34  0x34 // green
#define BK0_35  0x35 // blue
// dither control
#define BK0_36  0x36
#define BK0_37  0x37 // dither coefficient
#define BK0_38  0x38
// peaking control
#define BK0_3B  0x3B
#define BK0_3C  0x3C
#define BK0_3D  0x3D
#define BK0_3E  0x3E
#define BK0_3F  0x3F
// gamma control
#define BK0_40  0x40
#define BK0_41  0x41 // gamma data port
// output control
#define BK0_42  0x42
#define BK0_43  0x43
#define BK0_44  0x44
// frame rate convert for fail-safe mode / LVDS/RSDS test mode data
#define BK0_47  0x47
// short-line tuning
#define BK0_48  0x48
#define BK0_49  0x49
#define BK0_4A  0x4A
// [OSD overlay register]
// OSD alpha blending control
#define BK0_4B  0x4B // b6-3: color index of color key
// b2-0: alpha blending mode
#define BK0_4C  0x4C // b2-0: OSD alpha blending level
// scaling ratio
#define BK0_50  0x50 // horizontal scaling factor
#define BK0_51  0x51
#define BK0_52  0x52
#define BK0_53  0x53 // vertical scaling factor
#define BK0_54  0x54
#define BK0_55  0x55
// scaling filter control
#define BK0_56  0x56 // horizontal scaling filter
#define BK0_57  0x57 // vertical scaling filter
#define BK0_58  0x58 // horizontal DSUS scaling parameter
#define BK0_59  0x59
#define BK0_5A  0x5A // vertical DSUS scaling parameter
#define BK0_5B  0x5B
#define BK0_5C  0x5C
// color matrix coefficient
#define BK0_5E  0x5E // coefficient 11
#define BK0_5F  0x5F
#define BK0_60  0x60 // coefficient 12
#define BK0_61  0x61
#define BK0_62  0x62 // coefficient 13
#define BK0_63  0x63
#define BK0_64  0x64 // coefficient 21
#define BK0_65  0x65
#define BK0_66  0x66 // coefficient 22
#define BK0_67  0x67
#define BK0_68  0x68 // coefficient 23
#define BK0_69  0x69
#define BK0_6A  0x6A // coefficient 31
#define BK0_6B  0x6B
#define BK0_6C  0x6C // coefficient 32
#define BK0_6D  0x6D
#define BK0_6E  0x6E // coefficient 33
#define BK0_6F  0x6F
// color matrix control
#define BK0_70  0x70
// brightness control
#define BK0_71  0x71
#define BK0_72  0x72
#define BK0_73  0x73
#define BK0_74  0x74
// interlaced mode line shift
#define BK0_75  0x75
// auto gain function adjust
#define BK0_78  0x78 // auto gain function control
#define BK0_79  0x79 // auto gain status
// auto gain value select
#define BK0_7A  0x7A
// auto position function and jitter function control
#define BK0_7B  0x7B
#define BK0_7C  0x7C // auto position valid data value
#define BK0_7D  0x7D // auto gain value
// auto position function result
#define BK0_7E  0x7E  // auto position detected result vertical starting point
#define BK0_7F  0x7F
#define BK0_80  0x80  // auto position detected result horizontal starting point
#define BK0_81  0x81
#define BK0_82  0x82  // auto position detected result vertical end point
#define BK0_83  0x83
#define BK0_84  0x84  // auto position detected result horizontal end point
#define BK0_85  0x85
// jitter function detecting result
#define BK0_86  0x86 // jitter function detected left/right most point state(previous frame)
#define BK0_87  0x87
// auto noise reduction function
#define BK0_88  0x88
// auto phase and detecting result
#define BK0_89  0x89
#define BK0_8A  0x8A // auto phase text threshold for BK0_8F
#define BK0_8B  0x8B
// auto phase value
#define BK0_8C  0x8C
#define BK0_8D  0x8D
#define BK0_8E  0x8E
#define BK0_8F  0x8F
// VSync status
#define BK0_90  0x90
#define BK0_91  0x91 // locking short line vertical position
#define BK0_92  0x92
#define BK0_93  0x93 // locking short line width
#define BK0_94  0x94
#define BK0_95  0x95
#define BK0_96  0x96
#define BK0_97  0x97
#define BK0_98  0x98
#define BK0_99  0x99 // locking fraction status
#define BK0_9A  0x9A
#define BK0_9B  0x9B // locking htotal margin
#define BK0_9C  0x9C // locking read start value
#define BK0_9D  0x9D //
#define BK0_9E  0x9E // locking ssc margin
#define BK0_9F  0x9F
// [ OSD control ]
// OSD I/O access
#define BK0_A0  0xA0
// OSD address/data port
#define BK0_A1  0xA1
#define BK0_A2  0xA2
// OSD RAM font address/data port
#define BK0_A3  0xA3
#define BK0_A4  0xA4
// OSD display code address/data port
#define BK0_A5  0xA5
#define BK0_A6  0xA6
#define BK0_A7  0xA7
// OSD display attribute address/data port
#define BK0_A8  0xA8
#define BK0_A9  0xA9
#define BK0_AA  0xAA
#define BK0_AB  0xAB
#define BK0_AC  0xAC
#define BK0_AD  0xAD
#define BK0_AE  0xAE
#define BK0_AF  0xAF
// [I/O and PWM]
// watch dog timer
#define BK0_B0  0xB0
#define BK0_B1  0xB1
#define BK0_B2  0xB2
// DDC control
#define BK0_B3  0xB3
#define BK0_B4  0xB4
#define BK0_B5  0xB5
#define BK0_B6  0xB6
#define BK0_B7  0xB7
#define BK0_B8  0xB8
#define BK0_B9  0xB9
#define BK0_BA  0xBA
#define BK0_BB  0xBB
// output and PWM control
#define BK0_BC  0xBC
// HDCP control
#define BK0_BD  0xBD
#define BK0_BE  0xBE
#define BK0_BF  0xBF
// DVI DPMS control (only valid when F0h[10]=2'b10)
#define BK0_C0  0xC0
#define BK0_C1  0xC1
// PWM control
#define BK0_C2  0xC2
#define BK0_C3  0xC3
#define BK0_C4  0xC4
#define BK0_C5  0xC5
#define BK0_C6  0xC6
#define BK0_C7  0xC7
// interrupt control
#define BK0_CA  0xCA
#define BK0_CB  0xCB // interrupt pulse width by reference click
// intput status
#define BK0_CC  0xCC // input VSync changed
#define BK0_CD  0xCD // input HSync changed
#define BK0_CE  0xCE // input VSync disappear
#define BK0_CF  0xCF // input HSync disappear
// clock generator
#define BK0_D0  0xD0
#define BK0_D1  0xD1
// master & output PLL control
#define BK0_D2  0xD2 // master PLL divider
#define BK0_D3  0xD3
#define BK0_D4  0xD4
// frequency synthesizer & SSC control
#define BK0_D5  0xD5 // output PLL set
#define BK0_D6  0xD6
#define BK0_D7  0xD7
#define BK0_D8  0xD8 // output PLL spread spectrum step
#define BK0_D9  0xD9
#define BK0_DA  0xDA // output PLL spread spectrum span
#define BK0_DB  0xDB
// PLL test control
#define BK0_DC  0xDC
#define BK0_DD  0xDD
#define BK0_DE  0xDE
#define BK0_DF  0xDF
// [Mode detect status]
#define BK0_E0  0xE0 // input sync monitor
#define BK0_E1  0xE1
#define BK0_E2  0xE2 // input vertical total, count by HSync
#define BK0_E3  0xE3
#define BK0_E4  0xE4 // input horizontal period, count by reference clock
#define BK0_E5  0xE5
// Sync change tolerance
#define BK0_E6  0xE6 // Hsync tolerance
#define BK0_E7  0xE7 // VSync tolerance
// status override/interlace detect
#define BK0_E8  0xE8
#define BK0_E9  0xE9
// SOG HSYNC pulse width (when scaler bank 0x02[1:0] <> 2'b10 (not DVI input))
// DVI low clock frequency detection (when scaler bank 0x02[1:0] = 2'b10 (DVI feature only))
#define BK0_EA  0xEA
#define BK0_EB  0xEB
// [Misc. control]
// coast control
#define BK0_ED  0xED
#define BK0_EE  0xEE // end tuning
#define BK0_EF  0xEF // front tuning
// power down control/ software reset
#define BK0_F0  0xF0
#define BK0_F1  0xF1
#define SWRST_ALL   0x3f
// output signal control
#define BK0_F2  0xF2
// intput signal contorl
#define BK0_F3  0xF3
// output tri-state control
#define BK0_F4  0xF4
// output driving current control
#define BK0_F5  0xF5
// even clock delay control
#define BK0_F6  0xF6
// test mode control
#define BK0_F8  0xF8
// SRAM test status
#define BK0_F9  0xF9
// HDMI/HDCP SWRST
#define BK0_FB  0xFB
// chip ID
#define BK0_FD  0xFD // default = 0x00
#define BK0_FE  0xFE // default = 0xF3
///////////////////////////////////////////////////////
// [OSD register]
///////////////////////////////////////////////////////
// OSD double buffer control
#define OSD_01  0x01
// OSD start position
#define OSD_02  0x02 // OSD window horizontal start postion = 4 * OHSTA + 48(pixels)
#define OSD_03  0x03
#define OSD_04  0x04 // OSD window vertical start postion = 4 * OVSTA(lines)
#define OSD_05  0x05
// OSD size controol
#define OSD_06  0x06 // OSD window width = OSD_06 + 1(column), maximum 70 columns
#define OSD_07  0x07 // OSD window height = OSD_07 + 1(row), maximum 64 rows
// OSD space control
#define OSD_08  0x08 // OSD window horizontal space start position = OSD_08 + 1(row)
#define OSD_09  0x09 // OSD window vertical space start position = OSD_09 + 1(column)
#define OSD_0A  0x0A // OSD space width = 8 * OSD_0A(pixel)
#define OSD_0B  0x0B // OSD space height = 8 * OSD_0B(lline)
// internal osd control
#define OSD_0C  0x0C
#define OSD_0D  0x0D
#define OSD_0E  0x0E
// OSD window shadow control
#define OSD_0F  0x0F // OSD shadow height/width
// OSD code buffer offset/ base address
#define OSD_12  0x12 // b7: OSD code buffer offset select
#define OSD_13  0x13 // OSD code base address
#define OSD_14  0x14
// OSD gradually color control
#define OSD_15  0x15
// OSD starting gradually color
#define OSD_16  0x16
// OSD horizontal gradullay color
#define OSD_17  0x17 // increase/decrease value of Red color
#define OSD_18  0x18 // increase/decrease value of Green color
#define OSD_19  0x19 // increase/decrease value of Blue color
#define OSD_1A  0x1A // horizontal gradually strp of Red color
#define OSD_1B  0x1B // horizontal gradually strp of Green color
#define OSD_1C  0x1C // horizontal gradually strp of Blue color
// OSD vertical gradullay color
#define OSD_1D  0x1D // increase/decrease value of Red color
#define OSD_1E  0x1E // increase/decrease value of Green color
#define OSD_1F  0x1F // increase/decrease value of Blue color
#define OSD_20  0x20 // vertical gradually strp of Red color
#define OSD_21  0x21 // vertical gradually strp of Green color
#define OSD_22  0x22 // vertical gradually strp of Blue color
// OSD sub window 0 control
#define OSD_23  0x23
// OSD sub window 0 horizontal/vertical position
#define OSD_24  0x24 // sub window 0 horizontal start position
#define OSD_25  0x25 // sub window 0 horizontal end position
#define OSD_26  0x26 // sub window 0 vertical start position
#define OSD_27  0x27 // sub window 0 vertical end position
#define OSD_28  0x28 // OSD sub window 0 attribute
// OSD sub window 1 control
#define OSD_29  0x29
// OSD sub window 1 horizontal/vertical position
#define OSD_2A  0x2A // sub window 1 horizontal start position
#define OSD_2B  0x2B // sub window 1 horizontal end position
#define OSD_2C  0x2C // sub window 1 vertical start position
#define OSD_2D  0x2D // sub window 1 vertical end position
#define OSD_2E  0x2E // OSD sub window 1 attribute
// OSD sub window 2 control
#define OSD_2F  0x2F
// OSD sub window 2 horizontal/vertical position
#define OSD_30  0x30 // sub window 2 horizontal start position
#define OSD_31  0x31 // sub window 2 horizontal end position
#define OSD_32  0x32 // sub window 2 vertical start position
#define OSD_33  0x33 // sub window 2 vertical end position
#define OSD_34  0x34 // OSD sub window 2 attribute
// OSD sub window 3 control
#define OSD_35  0x35
// OSD sub window 3 horizontal/vertical position
#define OSD_36  0x36 // sub window 3 horizontal start position
#define OSD_37  0x37 // sub window 3 horizontal end position
#define OSD_38  0x38 // sub window 3 vertical start position
#define OSD_39  0x39 // sub window 3 vertical end position
#define OSD_3A  0x3A // OSD sub window 3 attribute
// OSD color key
#define OSD_3D  0x3D
#define OSD_3E  0x3E
#define OSD_3F  0x3F
// OSD button index
#define OSD_40  0x40
// OSD blink speed and scroll function
#define OSD_41  0x41
#define OSD_42  0x42
#define OSD_43  0x43
// OSD CPU write control
#define OSD_70  0x70
// OSD random test pattern
#define OSD_71  0x71
///////////////////////////////////////////////////////
// [Bank = 010]
// TCON register
///////////////////////////////////////////////////////
// output format control(TCON feature only)
#define BK2_02  0x02
#define BK2_03  0x03
// output drive/polarity control
#define BK2_04  0x04
#define BK2_05  0x05
// GPO4 (OE) active delay frame (TCON feature only)
#define BK2_06  0x06
// input format control
#define BK2_07  0x07
// GPO0(OPOL)
#define BK2_08  0x08 // line number that GPO0 start
#define BK2_09  0x09
#define BK2_0A  0x0A // line number that GPO0 end
#define BK2_0B  0x0B
#define BK2_0C  0x0C // pixel number that GPO0 start
#define BK2_0D  0x0D
#define BK2_0E  0x0E // pixel number that GPO0 end
#define BK2_0F  0x0F
#define BK2_10  0x10
// GPO1(EPOL)
#define BK2_11  0x11 // line number that GPO1 start
#define BK2_12  0x12
#define BK2_13  0x13 // line number that GPO1 end
#define BK2_14  0x14
#define BK2_15  0x15 // pixel number that GPO1 start
#define BK2_16  0x16
#define BK2_17  0x17 // pixel number that GPO1 end
#define BK2_18  0x18
#define BK2_19  0x19
// GPO2(RSP2)
#define BK2_1A  0x1A // line number that GPO2 start
#define BK2_1B  0x1B
#define BK2_1C  0x1C // line number that GPO2 end
#define BK2_1D  0x1D
#define BK2_1E  0x1E // pixel number that GPO2 start
#define BK2_1F  0x1F
#define BK2_20  0x20 // pixel number that GPO2 end
#define BK2_21  0x21
#define BK2_22  0x22
// GPO3(RSP3)
#define BK2_23  0x23 // line number that GPO3 start
#define BK2_24  0x24
#define BK2_25  0x25 // line number that GPO3 end
#define BK2_26  0x26
#define BK2_27  0x27 // pixel number that GPO3 start
#define BK2_28  0x28
#define BK2_29  0x29 // pixel number that GPO3 end
#define BK2_2A  0x2A
#define BK2_2B  0x2B
// GPO4(RCLK)
#define BK2_2C  0x2C // line number that GPO4 start
#define BK2_2D  0x2D
#define BK2_2E  0x2E // line number that GPO4 end
#define BK2_2F  0x2F
#define BK2_30  0x30 // pixel number that GPO4 start
#define BK2_31  0x31
#define BK2_32  0x32 // pixel number that GPO4 end
#define BK2_33  0x33
#define BK2_34  0x34
// GPO5(ROE)
#define BK2_35  0x35 // line number that GPO5 start
#define BK2_36  0x36
#define BK2_37  0x37 // line number that GPO5 end
#define BK2_38  0x38
#define BK2_39  0x39 // pixel number that GPO5 start
#define BK2_3A  0x3A
#define BK2_3B  0x3B // pixel number that GPO5 end
#define BK2_3C  0x3C
#define BK2_3D  0x3D
// GPO6(ROE2)
#define BK2_3E  0x3E // line number that GPO6 start
#define BK2_3F  0x3F
#define BK2_40  0x40 // line number that GPO6 end
#define BK2_41  0x41
#define BK2_42  0x42 // pixel number that GPO6 start
#define BK2_43  0x43
#define BK2_44  0x44 // pixel number that GPO6 end
#define BK2_45  0x45
#define BK2_46  0x46
// GPO7(ROE3)
#define BK2_47  0x47 // line number that GPO7 start
#define BK2_48  0x48
#define BK2_49  0x49 // line number that GPO7 end
#define BK2_4A  0x4A
#define BK2_4B  0x4B // pixel number that GPO7 start
#define BK2_4C  0x4C
#define BK2_4D  0x4D // pixel number that GPO7 end
#define BK2_4E  0x4E
#define BK2_4F  0x4F
// GPO8(DHS/TCON_LP)
#define BK2_50  0x50 // line number that GPO8 start
#define BK2_51  0x51
#define BK2_52  0x52 // line number that GPO8 end
#define BK2_53  0x53
#define BK2_54  0x54 // pixel number that GPO8 start
#define BK2_55  0x55
#define BK2_56  0x56 // pixel number that GPO8 end
#define BK2_57  0x57
#define BK2_58  0x58
// GPO9(DVS/TCON_FSYNC)
#define BK2_59  0x59 // line number that GPO9 start
#define BK2_5A  0x5A
#define BK2_5B  0x5B // line number that GPO9 end
#define BK2_5C  0x5C
#define BK2_5D  0x5D // pixel number that GPO9 start
#define BK2_5E  0x5E
#define BK2_5F  0x5F // pixel number that GPO9 end
#define BK2_60  0x60
#define BK2_61  0x61
// GPOA
#define BK2_62  0x62 // line number that GPOA start
#define BK2_63  0x63
#define BK2_64  0x64 // line number that GPOA end
#define BK2_65  0x65
#define BK2_66  0x66 // pixel number that GPOA start
#define BK2_67  0x67
#define BK2_68  0x68 // pixel number that GPOA end
#define BK2_69  0x69
#define BK2_6A  0x6A
///////////////////////////////////////////////////////
// [Bank = 011]
// MIU/PIP bank
///////////////////////////////////////////////////////
// sub-window image 's enable
//#define BK0_5C  0x5C  // [3] : MWE function enable
// sub-window image 's CSC function (RGB->YUV)
//#define BK0_5A  0x5A  // [6] : MWE window CSC enable      , when input is RGB to enable it let color correct.
// [7] : Main window CSC enable
//#define BK0_02  0x02  // [3] : CSC function (YCbCr->RGB)  , when input is YPbPr, mould be disable it let color correct ???
// sub-window image 's size
//#define BK3_18  0x18  // [7:0] : left[ 7:0] : PanelHStart + Hstart
//#define BK3_19  0x19  // [2:0] : left[10:8]
//#define BK3_1A  0x1A  // [7:0] : down[ 7:0] : 0 + Vend
//#define BK3_1B  0x1B  // [2:0] : down[10:8]
//#define BK3_1C  0x1C  // [7:0] : right[ 7:0] : PanelHStart + Hend
//#define BK3_1D  0x1D  // [2:0] : right[10:8]
//#define BK3_1E  0x1E  // [7:0] : upper[ 7:0] : 0 + Vstart
//#define BK3_1F  0x1F  // [2:0] : upper[10:8]
// sub-window miage 's border enable
//#define BK0_32  0x32  // [4] : ACC window border color enable
// [0] : may be need enable ???
// sub-window miage 's border size
//#define BK3_10  0x10  // left
//#define BK3_11  0x11  // right
//#define BK3_12  0x12  // upper
//#define BK3_13  0x13  // down
// sub-window image 's border color
//#define BK3_47  0x47
// peaking control
//#define BK3_3B  0x3B  // default : 0x03
//#define BK3_3C  0x3C  // default : 0x44
//#define BK3_3D  0x3D  // default : 0x90
// sub-sharpness control
//#define BK3_3F  0x3F  // default : 0x10
// [4:0] : YPK_COEF[4:0] : sub-sharpness    , try it to get best
// [7:6] : YPK_STP[1:0] : 0(0~3)            , try it to get best
// brightness control
//#define BK3_71  0x71  // enable : 0x01
//#define BK3_72  0x72  // r    : 0x80
//#define BK3_73  0x73  // g    : 0x80
//#define BK3_74  0x74  // b    : 0x80
// Gamma control
//#define BK3_40  0x40  // [0] : for gamma enable, the table is used the same with main-window.
// first color matrix coefficient   //  coefficient low     : Low [7:0] ;
//  coefficient high    : High [3:0] ; Sign [4]
//  value               : 1.000 is 0x400 ;  Max is 0xFFF
//#define BK3_5E  0x5E  // coefficient 11 low
//#define BK3_5F  0x5F  // coefficient 11 high
//#define BK3_60  0x60  // coefficient 12
//#define BK3_61  0x61
//#define BK3_62  0x62  // coefficient 13
//#define BK3_63  0x63
//#define BK3_64  0x64  // coefficient 21
//#define BK3_65  0x65
//#define BK3_66  0x66  // coefficient 22
//#define BK3_67  0x67
//#define BK3_68  0x68  // coefficient 23
//#define BK3_69  0x69
//#define BK3_6A  0x6A  // coefficient 31
//#define BK3_6B  0x6B
//#define BK3_6C  0x6C  // coefficient 32
//#define BK3_6D  0x6D
//#define BK3_6E  0x6E  // coefficient 33
//#define BK3_6F  0x6F
// color matrix control
//#define BK3_70  0x70  // default : 0x15
// double buffer control
#define BK3_01  0x01
// input ssource select/control
#define BK3_02  0x02
#define BK3_03  0x03
#define BK3_04  0x04
// input image sample range
#define BK3_05  0x05 // image vertical sample start point, count by input HSync
#define BK3_06  0x06
#define BK3_07  0x07 // image horizontal sample start point, count by input dot clock
#define BK3_08  0x08
#define BK3_09  0x09 // image vertical resolution(vertical display enable area count by line)
#define BK3_0A  0x0A
#define BK3_0B  0x0B // image horizontal resolution(horizontal display enable area count by pixel)
#define BK3_0C  0x0C
// digital video input control
#define BK3_0D  0x0D
// sub-window miage 's border size
#define BK3_10  0x10    // left
#define BK3_11  0x11    // right
#define BK3_12  0x12    // upper
#define BK3_13  0x13    // down
#define BK3_14  0x14
#define BK3_15  0x15
#define BK3_16  0x16
#define BK3_17  0x17
// sub-window image 's size
#define BK3_18  0x18    // left[ 7:0] : PanelHStart + Hstart
#define BK3_19  0x19    // left[10:8]
#define BK3_1A  0x1A    // down[ 7:0] : 0 + Vend
#define BK3_1B  0x1B    // down[10:8]
#define BK3_1C  0x1C    // right[ 7:0] : PanelHStart + Hend
#define BK3_1D  0x1D    // right[10:8]
#define BK3_1E  0x1E    // upper[ 7:0] : 0 + Vstart
#define BK3_1F  0x1F    // upper[10:8]
// peaking control
#define BK3_3A  0x3A
#define BK3_3B  0x3B    // default : 0x03
#define BK3_3C  0x3C    // default : 0x44
#define BK3_3D  0x3D    // default : 0x90
#define BK3_3E  0x3E
#define BK3_3F  0x3F    // default : 0x10
// YPK_COEF[4:0] : sub-sharpness, YPK_STP[1:0] : 0(0~3)
// Gamma control
#define BK3_40  0x40
// PIP registers
// PIP window border color
#define BK3_47  0x47    // sub-window image 's border color
// PIP control
#define BK3_48  0x48
// scaling function registers
// non-linear scaling control
#define BK3_49  0x49
#define BK3_4A  0x4A
#define BK3_4B  0x4B
#define BK3_4C  0x4C
#define BK3_4D  0x4D
#define BK3_4E  0x4E
#define BK3_4F  0x4F
// scaling ratio
#define BK3_50  0x50 // horizontal scaling factor
#define BK3_51  0x51
#define BK3_52  0x52
#define BK3_53  0x53 // vertical scaling factor
#define BK3_54  0x54
#define BK3_55  0x55
// scaling filter control
#define BK3_56  0x56 // horizontal scaling filter
#define BK3_57  0x57 // vertical scaling filter
#define BK3_5A  0x5A // scaling parameter
// first color matrix coefficient
#define BK3_5E  0x5E // coefficient 11
#define BK3_5F  0x5F
#define BK3_60  0x60 // coefficient 12
#define BK3_61  0x61
#define BK3_62  0x62 // coefficient 13
#define BK3_63  0x63
#define BK3_64  0x64 // coefficient 21
#define BK3_65  0x65
#define BK3_66  0x66 // coefficient 22
#define BK3_67  0x67
#define BK3_68  0x68 // coefficient 23
#define BK3_69  0x69
#define BK3_6A  0x6A // coefficient 31
#define BK3_6B  0x6B
#define BK3_6C  0x6C // coefficient 32
#define BK3_6D  0x6D
#define BK3_6E  0x6E // coefficient 33
#define BK3_6F  0x6F
// color matrix control
#define BK3_70  0x70
// brightness control
#define BK3_71  0x71
#define BK3_72  0x72
#define BK3_73  0x73
#define BK3_74  0x74
// flesh tone detection control
#define BK3_75  0x75
#define BK3_76  0x76
// YC delay control
#define BK3_77  0x77
// NR
#define BK3_7A  0x7A
#define BK3_7B  0x7B
#define BK3_7C  0x7C
#define BK3_7D  0x7D
#define BK3_7E  0x7E
#define BK3_7F  0x7F
// audio noise reduction function
#define BK3_80  0x80
#define BK3_81  0x81
#define BK3_82  0x82
#define BK3_83  0x83
#define BK3_84  0x84
#define BK3_85  0x85
#define BK3_86  0x86
// line buffer configuration
#define BK3_87  0x87
#define BK3_88  0x88
// De-interlace
// De-interlace control
#define BK3_89  0x89
#define BK3_8A  0x8A
#define BK3_8B  0x8B
#define BK3_8C  0x8C
#define BK3_8D  0x8D
#define BK3_8E  0x8E
#define BK3_8F  0x8F
#define BK3_90  0x90
#define BK3_91  0x91
#define BK3_92  0x92
#define BK3_93  0x93
#define BK3_94  0x94
#define BK3_95  0x95
// VBI sample range
#define BK3_96  0x96
#define BK3_97  0x97
#define BK3_98  0x98
#define BK3_99  0x99
#define BK3_9A  0x9A
#define BK3_9B  0x9B
#define BK3_9C  0x9C
#define BK3_9D  0x9D
#define BK3_9E  0x9E
#define BK3_9F  0x9F
//#ifdef PICASSO // ===>
// OSD-MIU interface
#define BK3_A0  0xA0
#define BK3_A1  0xA1
#define BK3_A2  0xA2
#define BK3_A3  0xA3
#define BK3_A4  0xA4
#define BK3_A5  0xA5
#define BK3_A7  0xA7
#define BK3_A8  0xA8
// OSD block clear register
#define BK3_A9  0xA9
#define BK3_AA  0xAA
#define BK3_AB  0xAB
#define BK3_AC  0xAC
#define BK3_AD  0xAD
#define BK3_AE  0xAE
#define BK3_AF  0xAF
#define BK3_B0  0xB0
//#endif // <===
// Luma curve and Histogram function control
#define BK3_B5  0xB5
#define BK3_B6  0xB6
#define BK3_B7  0xB7
#define BK3_B8  0xB8
#define BK3_B9  0xB9
#define BK3_BA  0xBA
#define BK3_BB  0xBB
#define BK3_BC  0xBC
#define BK3_BD  0xBD
#define BK3_BE  0xBE
#define BK3_BF  0xBF
// Memory clock synthesizer, interface
// memory PLL synthesizer interface
#define BK3_C0  0xC0
#define BK3_C1  0xC1
#define BK3_C2  0xC2
#define BK3_C3  0xC3
#define BK3_C4  0xC4
#define BK3_C5  0xC5
#define BK3_C6  0xC6
#define BK3_C7  0xC7
#define BK3_C8  0xC8
#define BK3_C9  0xC9
#define BK3_CA  0xCA
#define BK3_CB  0xCB
#define BK3_CC  0xCC
#define BK3_CD  0xCD
// second (Flesh Tone) color matrix coefficient
#define BK3_CE  0xCE // coefficient 11
#define BK3_CF  0xCF
#define BK3_D0  0xD0 // coefficient 12
#define BK3_D1  0xD1
#define BK3_D2  0xD2 // coefficient 13
#define BK3_D3  0xD3
#define BK3_D4  0xD4 // coefficient 21
#define BK3_D5  0xD5
#define BK3_D6  0xD6 // coefficient 22
#define BK3_D7  0xD7
#define BK3_D8  0xD8 // coefficient 23
#define BK3_D9  0xD9
#define BK3_DA  0xDA // coefficient 31
#define BK3_DB  0xDB
#define BK3_DC  0xDC // coefficient 32
#define BK3_DD  0xDD
#define BK3_DE  0xDE // coefficient 33
#define BK3_DF  0xDF
// Mode Detect Status
// [Mode detect status]
#define BK3_E0  0xE0 // input sync monitor
#define BK3_E1  0xE1
#define BK3_E2  0xE2 // input vertical total, count by HSync
#define BK3_E3  0xE3
#define BK3_E4  0xE4 // input horizontal period, count by reference clock
#define BK3_E5  0xE5
// Sync change tolerance
#define BK3_E6  0xE6 // Hsync tolerance
#define BK3_E7  0xE7 // VSync tolerance
// status override/interlace detect
#define BK3_E8  0xE8
#define BK3_E9  0xE9
// Misc. Control
// coast control
#define BK3_ED  0xED
// GPO control
#define BK3_EE  0xEE
#define BK3_EF  0xEF
#define BK3_F0  0xF0
#define BK3_F1  0xF1
// input signal control
#define BK3_F3  0xF3
// Double buffer control
#define BK4_01  0x01
// Memory configuration
#define BK4_02  0x02
#define BK4_03  0x03
#define BK4_04  0x04
#define BK4_05  0x05
#define BK4_06  0x06
#define BK4_07  0x07
#define BK4_08  0x08
#define BK4_09  0x09
#define BK4_0A  0x0A
#define BK4_0B  0x0B
// DDR PLL and IO register
#define BK4_10  0x10
#define BK4_11  0x11
#define BK4_12  0x12
#define BK4_13  0x13
#define BK4_14  0x14
#define BK4_15  0x15
#define BK4_16  0x16
#define BK4_17  0x17
#define BK4_18  0x18
#define BK4_19  0x19
#define BK4_1A  0x1A
#define BK4_1B  0x1B
#define BK4_1C  0x1C
// Video Input Base address for Main window
#define BK4_20  0x20
#define BK4_21  0x21
#define BK4_22  0x22
#define BK4_23  0x23
#define BK4_24  0x24
#define BK4_25  0x25
#define BK4_26  0x26
#define BK4_27  0x27
#define BK4_28  0x28
#define BK4_29  0x29
#define BK4_2A  0x2A
#define BK4_2B  0x2B
#define BK4_2C  0x2C
#define BK4_2D  0x2D
#define BK4_2E  0x2E
#define BK4_2F  0x2F
#define BK4_30  0x30
#define BK4_31  0x31
#define BK4_32  0x32
#define BK4_33  0x33
// Video Input Base address for Sub window
#define BK4_40  0x40
#define BK4_41  0x41
#define BK4_42  0x42
#define BK4_43  0x43
#define BK4_44  0x44
#define BK4_45  0x45
#define BK4_46  0x46
#define BK4_47  0x47
#define BK4_48  0x48
#define BK4_49  0x49
#define BK4_4A  0x4A
#define BK4_4B  0x4B
#define BK4_4C  0x4C
#define BK4_4D  0x4D
#define BK4_4E  0x4E
#define BK4_4F  0x4F
#define BK4_50  0x50
#define BK4_51  0x51
#define BK4_52  0x52
#define BK4_53  0x53
// OSD MIU interface
#define BK4_60  0x60
#define BK4_61  0x61
#define BK4_62  0x62
#define BK4_63  0x63
#define BK4_64  0x64
#define BK4_65  0x65
#define BK4_66  0x66
#define BK4_67  0x67
// Audio Delay
#define BK4_68  0x68
#define BK4_69  0x69
#define BK4_6A  0x6A
#define BK4_6B  0x6B
#define BK4_6C  0x6C
#define BK4_6D  0x6D
#define BK4_6E  0x6E
#define BK4_6F  0x6F
// OSD write interface
#define BK4_80  0x80
#define BK4_81  0x81
#define BK4_82  0x82
#define BK4_83  0x83
#define BK4_84  0x84
#define BK4_85  0x85
#define BK4_86  0x86
#define BK4_87  0x87
#define BK4_88  0x88
#define BK4_89  0x89
// OSD display interface
#define BK4_8A  0x8A
#define BK4_8B  0x8B
#define BK4_8C  0x8C
#define BK4_8D  0x8D
#define BK4_8E  0x8E
#define BK4_8F  0x8F
#define BK4_90  0x90
#define BK4_91  0x91
#define BK4_92  0x92
#define BK4_93  0x93
#define BK4_94  0x94
#define BK4_95  0x95
#define BK4_96  0x96
#define BK4_97  0x97
#define BK4_98  0x98
#define BK4_99  0x99
#define BK4_9A  0x9A
// VBI for Main window
#define BK4_A0  0xA0
#define BK4_A1  0xA1
#define BK4_A2  0xA2
#define BK4_A3  0xA3
#define BK4_A4  0xA4
// VBI for Sub window
#define BK4_A5  0xA5
#define BK4_A6  0xA6
#define BK4_A7  0xA7
#define BK4_A9  0xA9




#define BKA_42 0x42
#endif
