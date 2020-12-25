
#ifndef _DRVOSD_H
#define _DRVOSD_H

#ifdef _DRVOSD_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

#define SPACE_FONT      0x00

#define OSD_WIN0        0
#define OSD_WIN1        1
#define OSD_WIN2        2
#define OSD_WIN3        3
#define OSD_WIN4        4
#define OSD_WIN5        5
#define OSD_WIN6        6
#define OSD_WIN7        7

#define OSD_WND_CTRL0               0
#define OSD_WND_CTRL1               1
#define OSD_WND_CTRL2               2
#define OSD_WND_CTRL3               3

#define OSD_WND_INFO_H_START        0
#define OSD_WND_INFO_V_START        1
#define OSD_WND_INFO_V_END          2
#define OSD_WND_INFO_LINE_OFFSET    3
#define OSD_WND_INFO_CABASE         4

#define OSD_GD_WIN0_EN_BIT          BIT0
#define OSD_GD_WIN1_EN_BIT          BIT1
#define OSD_GD_WIN2_EN_BIT          BIT2
#define OSD_GD_WIN3_EN_BIT          BIT3
#define OSD_GD_WIN4_EN_BIT          BIT4
#define OSD_GD_WIN5_EN_BIT          BIT5
#define OSD_GD_WIN6_EN_BIT          BIT6
#define OSD_GD_WIN7_EN_BIT          BIT7
#define OSD_GD_TG0_EN_BIT           BIT8
#define OSD_GD_TG1_EN_BIT           BIT9
#define OSD_GD_TG2_EN_BIT           BIT10
#define OSD_GD_TG3_EN_BIT           BIT11

#define OSD_GD_EG0                  0x0000ul
#define OSD_GD_EG1                  0x0001ul
#define OSD_GD_EG2                  0x0002ul
#define OSD_GD_EG3                  0x0003ul
#define OSD_GD_EG4                  0x0004ul
#define OSD_GD_EG5                  0x0005ul
#define OSD_A_FIX                   0x0006ul
#define OSD_COLOR_FIX               0x0006ul

#define OSD_ALPHA_LV1               0
#define OSD_ALPHA_LV2               1
#define OSD_ALPHA_OUT               2

//OSD Wnd Ctrl 0
#define OWC0_CA_DUP_MODE            BIT1
#define OWC0_FP_MODE                BIT2
#define OWC0_GROUP_SEL0             0
#define OWC0_GROUP_SEL1             BIT3
#define OWC0_OUT_ALPHA_SRC(GD_EG)   ((GD_EG)<<4)
#define OWC0_LV1_ALPHA_SRC(GD_EG)   ((GD_EG)<<8)
#define OWC0_LV2_ALPHA_SRC(GD_EG)   ((GD_EG)<<12)
#define OWC0MASK_CA_DUP_MODE        OWC0_CA_DUP_MODE
#define OWC0MASK_FP_MODE            OWC0_FP_MODE
#define OWC0MASK_GROUP_SEL          OWC0_GROUP_SEL1
#define OWC0MASK_OUT_ALPHA_SRC      (BIT6|BIT5|BIT4)
#define OWC0MASK_LV1_ALPHA_SRC      (BIT10|BIT9|BIT8)
#define OWC0MASK_LV2_ALPHA_SRC      (BIT14|BIT13|BIT12)
#define OWC0MASK_ALL                (OWC0MASK_CA_DUP_MODE|OWC0MASK_FP_MODE|OWC0MASK_GROUP_SEL|OWC0MASK_OUT_ALPHA_SRC|OWC0MASK_LV1_ALPHA_SRC|OWC0MASK_LV2_ALPHA_SRC)
//OSD Wnd Ctrl 1
#define OWC1_FONT_HEIGHT(height)    (height-1)
#define OWC1_44MODE_TRS_EN          BIT14
#define OWC1_1BP_44MODE_EN          BIT15
#define OWC1MASK_FONT_HEIGHT        (0x001F)
#define OWC1MASK_44MODE_TRS_EN      BIT14
#define OWC1MASK_1BP_44MODE_EN      BIT15
#define OWC1MASK_ALL                (OWC1MASK_FONT_HEIGHT|OWC1MASK_44MODE_TRS_EN|OWC1MASK_1BP_44MODE_EN)
//OSD Wnd Ctrl 2
#define OWC2_GD_COLOR_R(GD_EG)      ((GD_EG))
#define OWC2_GD_COLOR_G(GD_EG)      ((GD_EG)<<4)
#define OWC2_GD_COLOR_B(GD_EG)      ((GD_EG)<<8)
#define OWC2MASK_GD_COLOR_R         (BIT0|BIT1|BIT2)
#define OWC2MASK_GD_COLOR_G         (BIT4|BIT5|BIT6)
#define OWC2MASK_GD_COLOR_B         (BIT8|BIT9|BIT10)
#define OWC2MASK_ALL                (OWC2MASK_GD_COLOR_R|OWC2MASK_GD_COLOR_G|OWC2MASK_GD_COLOR_B)
//OSD Wnd Ctrl 3
#define OWC3_1BPP_ATTR_BIT1         BIT8
#define OWC3_LV1_KM_SWITCH          BIT11
#define OWC3_TRANS_INV              BIT12
#define OWC3_CKEY_INV               BIT13
#define OWC3MASK_1BPP_ATTR_BIT1     BIT8
#define OWC3MASK_LV1_KM_SWITCH      BIT11
#define OWC3MASK_TRANS_INV          BIT12
#define OWC3MASK_CKEY_INV           BIT13
#define OWC3MASK_ALL                (OWC3MASK_1BPP_ATTR_BIT1|OWC3MASK_LV1_KM_SWITCH|OWC3MASK_TRANS_INV|OWC3MASK_CKEY_INV)



// for OSD rotation
#define GET_CABASE_0_90(u16Base,u8LineOffset)           (u16Base+u8LineOffset-1)
#define GET_CABASE_90_0(u16Base,u8LineOffset)           (u16Base+1-u8LineOffset)

#define GET_CABASE_0_180(u16Base,u8LineOffset,u8H)      (u16Base+(WORD)u8LineOffset*u8H-1)
#define GET_CABASE_180_0(u16Base,u8LineOffset,u8H)      (u16Base+1-(WORD)u8LineOffset*u8H)

#define GET_CABASE_0_270(u16Base,u8LineOffset,u8H)      (u16Base+(WORD)u8LineOffset*(u8H-1))
#define GET_CABASE_270_0(u16Base,u8LineOffset,u8H)      (u16Base-(WORD)u8LineOffset*(u8H-1))

#if ENABLE_OSD_ROTATION
INTERFACE WORD drvOSD_GetTxtCAStart(BYTE u8Wnd, BYTE u8XPos, BYTE u8YPos);
#define GET_TXT_CASTART(u8Wnd,u8XPos,u8YPos) drvOSD_GetTxtCAStart(u8Wnd,u8XPos,u8YPos)
#else //u8Wnd*0x20=u8Wnd<<5
#define GET_TXT_CASTART(u8Wnd,u8XPos,u8YPos) (msRead2Byte((u8Wnd<<5)+OSD2_10)+(WORD)u8YPos*msReadByte((u8Wnd<<5)+OSD2_12)+u8XPos)
#endif
INTERFACE void drvOSD_DrawRealChar(BYTE u8Wnd, BYTE u8XPos, BYTE u8YPos, BYTE u8Char);
INTERFACE void drvOSD_DrawContinuousChar(BYTE u8Wnd,BYTE u8XPos, BYTE u8YPos, BYTE u8Char, BYTE u8Num);
INTERFACE void drvOSD_DrawVContinuousChar(BYTE u8Wnd, BYTE u8XPos, BYTE u8YPos, BYTE u8Char, BYTE u8Num);
INTERFACE void drvOSD_DrawBlankPlane(BYTE u8Wnd,BYTE u8XPos,BYTE u8YPos,BYTE u8Width,BYTE u8Height);
INTERFACE void drvOSD_SetWndLineOffset(BYTE u8Wnd, BYTE u8LineOffset);
#if !ENABLE_MENULOAD
INTERFACE void drvOSD_SetWndHPosition(BYTE u8Wnd,WORD u16HStart, WORD u16HEnd);
INTERFACE void drvOSD_SetWndVPosition(BYTE u8Wnd,WORD u16VStart, WORD u16VEnd);
#endif
INTERFACE WORD drvOSD_GetWndInfo(BYTE u8Wnd,BYTE u8Dir);
INTERFACE void drvOSD_SetWndOn(BYTE u8Wnd,Bool u8On);
INTERFACE void drvOSD_SetWndCtrl(BYTE u8Wnd, BYTE u8CtrlType,WORD u16Value,WORD u16Mask);
INTERFACE void drvOSD_SetWndFixAlpha(BYTE u8Wnd,BYTE u8Lvl,BYTE u8Value);
INTERFACE void drvOSD_SetWndFixColor(BYTE u8Wnd,BYTE u8Red,BYTE u8Green,BYTE u8Blue);
INTERFACE void drvOSD_SetWndCABaseAddr(BYTE u8Wnd,WORD u16Addr);
INTERFACE void drvOSD_SetWndFixAttr(BYTE u8Wnd,BYTE u8Enable,BYTE u8Attr);
INTERFACE void drvOSD_SetWndShiftPixel(BYTE u8Wnd,u8X,u8Y);
INTERFACE WORD drvOSD_GetWndInfo(BYTE u8Wnd,BYTE u8Dir);
INTERFACE void drvOSD_SetGDEngineEnableWnd(BYTE u8GDEngine, WORD u16EnableFlags,WORD u16Mask);
INTERFACE void drvOSD_FrameColorEnable(Bool bEnable);
INTERFACE void drvOSD_FrameColorRGB(BYTE u8Red,BYTE u8Green,BYTE u8Blue);
INTERFACE void drvOSD_WaitOsdDeEndInterrupt(void);
INTERFACE void drvOSD_uncall(void);

#undef INTERFACE
#endif ///_DRVOSD_H

