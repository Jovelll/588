/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: Isr.h
 [Date]:        05-Feb-2004
 [Comment]:
   Remote control function header file.
 [Reversion History]:
*******************************************************************************/
#include "Ir.h"

#ifndef _IRFUNC_H_
#define _IRFUNC_H_

#ifdef _IRFUNC_C_
#define _IRFUNCDEC_
#else
#define _IRFUNCDEC_ extern
#endif

//////////////////////////////////////////////////////
// Definition
//////////////////////////////////////////////////////
// tuner change channel delay time
#define IR_NUMKEY_DELAY_TIME1 (3000 / ISR_TIMER0_INTERVAL) // IR 0-9 key press ending delay time (1st key)
#define IR_NUMKEY_DELAY_TIME2 (10 / ISR_TIMER0_INTERVAL) // IR 0-9 key press ending delay time (3 key)
#define IR_NUMKEY_DELAY_TIME3 (1000 / ISR_TIMER0_INTERVAL) // IR 0-9 key press ending delay time (2 key)

#if (IR_TYPE==IR_ASUS)
// key definition
typedef enum _IrCommandType
{
    IRKEY_POWER = 0x56,
    IRKEY_MENU = 0x5D,
    IRKEY_EXIT = 0x4A,
    IRKEY_OSD_UP = 0x5E,
    IRKEY_OSD_DOWM = 0x5C,
    IRKEY_OSD_RIGHT = 0x5F,
    IRKEY_OSD_LEFT = 0x58,
    IRKEY_SELECT = 0x53,
};


#elif(IR_TYPE==IR_NEC_PRIMA_RC698)
typedef enum _IrCommandType
{
    IRKEY_0             = 0x10,
    IRKEY_1             = 0x11,
    IRKEY_2             = 0x12,
    IRKEY_3             = 0x13,
    IRKEY_4             = 0x14,
    IRKEY_5             = 0x15,
    IRKEY_6             = 0x16,
    IRKEY_7             = 0x17,
    IRKEY_8             = 0x18,
    IRKEY_9             = 0x19,

    IRKEY_100           = 0x1b,     //recall
    IRKEY_INFO          = 0x40,     //info
    IRKEY_POWER         = 0x0a,
    IRKEY_INPUT         = 0x1e,     //epg
    IRKEY_MENU      = 0x1a,
   // IRKEY_OSD_MENU      = 0x1a,
    IRKEY_OSD_LEFT      = 0x03,
    IRKEY_OSD_RIGHT     = 0x02,
    IRKEY_OSD_UP        = 0x00,
    IRKEY_OSD_DOWM      = 0x01,
    IRKEY_SELECT    = 0x1f,    
  //  IRKEY_OSD_SELECT    = 0x1f,
    IRKEY_EXIT          = 0x1c,

    IRKEY_PIP           = 0x58,     //prev
    IRKEY_TTX_REVEAL    = 0x5d,     //next
    IRKEY_TTX_UPDATE    = 0x5f,     //backward
    IRKEY_TTX_SIZE      = 0x5a,     //forward
    IRKEY_TTX_CANCEL    = 0x1f,

    IRKEY_TTX_MIX       = 0x1d,     //ch list
    IRKEY_TTX_INDEX     = 0x0e,     //subt
    IRKEY_TTX_HOLD      = 0x41,     //y/video
    IRKEY_TTX_LIST      = 0x0f,     //16:9/4:3
    IRKEY_TTX_SUBCODE   = 0x59,     //Tv/radio

    IRKEY_TTX_RED       = 0x05,
    IRKEY_TTX_GREEN     = 0x42,
    IRKEY_TTX_YELLOW    = 0x43,
    IRKEY_TTX_CYAN      = 0x09,

    IRKEY_VOL_UP        = 0x07,     //setup
    IRKEY_VOL_DOWM      = 0x0b,     //media
    IRKEY_CH_UP         = 0x5e,     //goto
    IRKEY_CH_DOWM       = 0x5b,     //play

    IRKEY_MUTE          = 0x0d,
    IRKEY_MTS           = 0x0c,
    IRKEY_DISPLAY       = 0xff-1,
    IRKEY_SLEEP         = 0xff-2,
    IRKEY_ZOOM          = 0xff-3,
    IRKEY_PICMODE       = 0xff-4,
    IRKEY_SOUNDMODE     = 0xff-5,
    IRKEY_SCANMODE      = 0xff-6,
    IRKEY_CAPTURE       = 0x57,     //rec


    IRKEY_CECLink       = 0x04,     //fav
    IRKEY_PLAY          = 0x5b,
    IRKEY_STOP          = 0x4b,
    IRKEY_PAUSE         = 0x47,

    IRKEY_DUMY          = 0xff
}IrCommandType;
#elif (IR_TYPE==IR_DTech)
// key definition
typedef enum _IrCommandType
{
    IRKEY_0				= 0x49,
    IRKEY_1				= 0x13,
    IRKEY_2				= 0x10,
    IRKEY_3				= 0x11,
    IRKEY_4				= 0x0F,
    IRKEY_5				= 0x0C,
    IRKEY_6				= 0x0D,
    IRKEY_7				= 0x0B,
    IRKEY_8				= 0x08,
    IRKEY_9				= 0x09,

    IRKEY_100			= 0x4D,

    IRKEY_POWER			= 0x0E,
    IRKEY_INPUT			= 0x1A,

    IRKEY_OSD_MENU		= 0x02,
    IRKEY_OSD_LEFT		= 0xFA,
    IRKEY_OSD_RIGHT		= 0xFB,
    IRKEY_OSD_UP		= 0x42,
    IRKEY_OSD_DOWM		= 0x40,

    IRKEY_VOL_UP			= 0x48,
    IRKEY_VOL_DOWM		= 0x1C,
    IRKEY_CH_UP			= 0x44,
    IRKEY_CH_DOWM		= 0x1D,
    IRKEY_CH_RETURN		= 0x51,
    IRKEY_CH_TYPE		= 0x04,
    IRKEY_MUTE			= 0x45,
    IRKEY_MTS			= 0x07,

    IRKEY_DISPLAY		= 0x05,
    IRKEY_CHANNEL		= 0x0A,

    IRKEY_SLEEP			= 0xFF,
    IRKEY_ZOOM			= 0xFE,
    IRKEY_CC				= 0xFD,
    IRKEY_PIP			= 0x1F,
    IRKEY_PICMODE		= 0xFC,
    IRKEY_NULL			= 0x77
}IrCommandType;
#elif (IR_TYPE==IR_Tatung1)
// key definition
typedef enum _IrCommandType
{
    IRKEY_0				= 0x41,
    IRKEY_1				= 0x14,
    IRKEY_2				= 0x15,
    IRKEY_3				= 0x16,
    IRKEY_4				= 0x18,
    IRKEY_5				= 0x19,
    IRKEY_6				= 0x1A,
    IRKEY_7				= 0x1D,
    IRKEY_8				= 0x1E,
    IRKEY_9				= 0x1F,

    IRKEY_100			= 0x42,

    IRKEY_POWER			= 0x01,
    IRKEY_INPUT			= 0x00,
    IRKEY_INPUT_ANALOG	= 0x0A,
    IRKEY_INPUT_AV		= 0x07,
    IRKEY_INPUT_SV		= 0x08,
    IRKEY_INPUT_YPBPR	= 0x09,

    IRKEY_OSD_MENU		= 0x0D,
    IRKEY_OSD_LEFT		= 0xFA,
    IRKEY_OSD_RIGHT		= 0xFB,
    IRKEY_OSD_UP		= 0x0B,
    IRKEY_OSD_DOWM		= 0x0F,

    IRKEY_VOL_UP			= 0x0E,
    IRKEY_VOL_DOWM		= 0x0C,
    IRKEY_CH_UP			= 0xEA,
    IRKEY_CH_DOWM		= 0xEB,
    IRKEY_CH_RETURN		= 0x05,
    IRKEY_CH_TYPE		= 0x43,
    IRKEY_MUTE			= 0x40,	//0x02,
    IRKEY_MTS			= 0x4E,	//0x12,

    IRKEY_DISPLAY		= 0x04,	//0x06,
    IRKEY_CHANNEL		= 0x02,	//0x4E,

    IRKEY_SLEEP			= 0x06,	//0x40,
    IRKEY_ZOOM			= 0xFE,
    IRKEY_CC				= 0x1B,
    IRKEY_TT_RED			= 0x4C,
    IRKEY_TT_GREEN		= 0x44,
    IRKEY_TT_YELLOW		= 0x1C,
    IRKEY_TT_CYAN		= 0x4D,
    IRKEY_TT_INDEX		= 0x45,
    IRKEY_TT_SIZE		= 0x47,
    IRKEY_TT_TIME		= 0x4A,
    IRKEY_TT_REVAL		= 0x03,	//0x4B,
    IRKEY_TT_SWITCH		= 0x4B,
    IRKEY_TT_HOLD		= 0x46,

    IRKEY_PIP			= 0x1F,
    IRKEY_PICMODE		= 0xFC,
    IRKEY_NULL			= 0x77
}IrCommandType;
#elif (IR_TYPE==IR_UC)
// key definition
typedef enum _IrCommandType
{
    IRKEY_POWER_ON		= 0x05,
    IRKEY_POWER_OFF	= 0x12,
    IRKEY_INPUT_ANALOG	= 0x1E,
    IRKEY_INPUT_YPBPR	= 0x1C,
    IRKEY_INPUT_SV		= 0x0C,
    IRKEY_INPUT_AV		= 0x1F,
    IRKEY_MUTE			= 0x02,
    IRKEY_SEL			= 0x1D,
    IRKEY_MENU			= 0x17,

    IRKEY_OSD_LEFT		= 0x06,//0x0A,
    IRKEY_OSD_RIGHT		= 0x07,//0x01,
    IRKEY_OSD_UP		= 0x0A,//0x06,
    IRKEY_OSD_DOWM		= 0x01,//0x07,
    IRKEY_CHANNEL		= 0xFA,
    IRKEY_NULL			= 0x77
}IrCommandType;

#elif (IR_TYPE==IR_PHILIPS)
// key definition
typedef enum _IrCommandType
{
    IRKEY_POWER_ON		= 0x05,
    IRKEY_POWER_OFF	= 0x12,
    IRKEY_INPUT_ANALOG	= 0x1E,
    IRKEY_INPUT_YPBPR	= 0x1C,
    IRKEY_INPUT_SV		= 0x0C,
    IRKEY_INPUT_AV		= 0x1F,
    IRKEY_MUTE			= 0x02,
    IRKEY_SEL			= 0x1D,
    IRKEY_MENU			= 0x17,

    IRKEY_OSD_LEFT		= 0x06,//0x0A,
    IRKEY_OSD_RIGHT		= 0x07,//0x01,
    IRKEY_OSD_UP		= 0x0A,//0x06,
    IRKEY_OSD_DOWM		= 0x01,//0x07,
    IRKEY_CHANNEL		= 0xFA,
    IRKEY_NULL			= 0x77
}IrCommandType;
#elif (IR_TYPE==IR_OPLAY)
// key definition
typedef enum _IrCommandType
{
    IRKEY_POWER		       = 0x56,
    IRKEY_INPUT_ANALOG	= 0x1E,
    IRKEY_INPUT_YPBPR	= 0x1C,
    IRKEY_INPUT_SV		= 0x0C,
    IRKEY_INPUT_AV		= 0x1F,
    IRKEY_MUTE			= 0x02,
    IRKEY_SEL			= 0x53,
    IRKEY_MENU			= 0x5D,
    IRKEY_EXIT			= 0x44,
    IRKEY_QUICKFIT			= 0x40,

    IRKEY_OSD_LEFT		= 0x06,//0x0A,
    IRKEY_OSD_RIGHT		= 0x07,//0x01,
    IRKEY_OSD_UP		= 0x5E,//0x06,
    IRKEY_OSD_DOWM		= 0x5C,//0x07,
    IRKEY_CHANNEL		= 0xFA,
    IRKEY_NULL			= 0x77
}IrCommandType;
#elif (IR_TYPE==IR_SAMSUNG)
// key definition
typedef enum _IrCommandType
{
    IRKEY_POWER = 0x02,
    IRKEY_MENU = 0x68,
    
    IRKEY_OSD_UP = 0x60,
    IRKEY_OSD_DOWN = 0x61,
    
    IRKEY_OSD_LEFT = 0x65,
    IRKEY_OSD_RIGHT = 0x62

    IRKEY_NULL			= 0x77
}IrCommandType;
#endif

// key definition
typedef enum _IrControlMenuType
{
    IR_MENU_NULL=0,
    IR_MENU_MENU = 0x01,
    IR_MENU_SELECT=0x02,

}IrControlMenuTyp;

///////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////
_IRFUNCDEC_ void irDecodeCommand(void);
_IRFUNCDEC_ void irAdjustNumKey(BYTE ucPressNum);
_IRFUNCDEC_ void irDecodeNumKey(void);
//_IRFUNCDEC_ WORD irCalPassword(BYTE ucPressNum);


#endif

