/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: Ir.h
 [Date]:        04-Feb-2004
 [Comment]:
   Remote control header file.
 [Reversion History]:
*******************************************************************************/
#include "types.h"
#include "board.h"
#include "debug.h"
#include "misc.h"
#include "MS_rwreg.h"

#ifndef _IR_H
#define _IR_H

#ifdef _IR_C_
#define _IRDEC_
#else
#define _IRDEC_ extern
#endif

#ifndef IR_ENABLE_DEBUG
#define IR_ENABLE_DEBUG  1&DEBUG_EN
#endif

//////////////////////////////////////
//   IR Header Code
//////////////////////////////////////

#define IR_DTech		0
#define IR_Tatung1		1
#define IR_UC			2
#define IR_PHILIPS      3
#define IR_OPLAY        4
#define IR_ASUS         5
#define IR_SAMSUNG      6
#define IR_NEC_PRIMA_RC698  7



#define IR_TYPE		IR_NEC_PRIMA_RC698//IR_ASUS//IR_NEC_PRIMA_RC698//	IR_ASUS


#if (IR_TYPE==IR_DTech)
#define IR_HEADER_CODE0  0x06//0x50
#define IR_HEADER_CODE1  0xFB//0xAF
#define IR_CUSTOM_CODE    	0x06FB
#elif (IR_TYPE==IR_Tatung1)
#define IR_HEADER_CODE0  0x42//0x50
#define IR_HEADER_CODE1  0xBD//0xAF
#define IR_CUSTOM_CODE    	0x42BD
#elif (IR_TYPE==IR_UC)
#define IR_HEADER_CODE0  0x00
#define IR_HEADER_CODE1  0xFF
#define IR_CUSTOM_CODE    	0x00FF
#elif (IR_TYPE==IR_PHILIPS)
#define IR_HEADER_CODE0  0x80
#define IR_HEADER_CODE1  0x7F
#define IR_CUSTOM_CODE    	0x807F
#elif (IR_TYPE==IR_OPLAY)
#define IR_HEADER_CODE0  0x6C
#define IR_HEADER_CODE1  0x68
#define IR_CUSTOM_CODE    	0x6C68
#elif (IR_TYPE==IR_ASUS)
#define IR_CUSTOM_CODE    	0x7788
#elif (IR_TYPE==IR_SAMSUNG)
#define IR_CUSTOM_CODE    	0x0707///0xE0E0
#elif(IR_TYPE==IR_NEC_PRIMA_RC698)
#define IR_HEADER_CODE0       0x80
#define IR_HEADER_CODE1       0xFF
#define IR_CUSTOM_CODE    	0x80FF
#endif


//_IRDEC_  BYTE  TEST_XJ0;
//_IRDEC_  BYTE  TEST_XJ1;
//_IRDEC_  BYTE  TEST_XJ2;


_IRDEC_  BYTE  data_flag;


#define pinIr    (_bit5_(msRegs[_REG_PMGPIO0_IN]))  // (_bit6_(MEM_MSREAD_BYTE(_REG_GPIO2_IN)))
  
_IRDEC_ void IR_INT1(void);
_IRDEC_ void IR_Init(void);
//_IRDEC_ void IR_timer_INT(void);

_IRDEC_ BYTE IR_GetIRKeypadStatus( void );
_IRDEC_ void IR_InitVariables(void);

union DW_TEMP
{
    BYTE	valB[4];	//B[]={0x0xaa,0xbb,0xcc,0xdd}
    WORD	valW[2];	//W[]={0xaabb,0xccdd}
    DWORD	valDW;	//DW=0xaabbccdd
};

#define REPEAT_KEY_DEBOUNCE_COUNT       2
#define GETREPEAT_KEY_DEBOUNCE_COUNT_ADJITEM       1
#define GETREPEAT_KEY_DEBOUNCE_COUNT_NORMAL             1
#define GETREPEAT_KEY_DEBOUNCE_COUNT_PM                 20


_IRDEC_ BYTE Timer1_TH1;
_IRDEC_ BYTE Timer1_TL1;
_IRDEC_ BYTE  IRstateHighCount;
_IRDEC_ BYTE  IRstateLowCount;
_IRDEC_ BYTE  xdata IR_Data_Count;
_IRDEC_ BYTE  xdata IR_Step;
//_IRDEC_ BYTE  xdata IR_GotKey;
_IRDEC_ BYTE  xdata IR_RepeatKey;
_IRDEC_ BYTE  xdata IR_RepeatFlag;
_IRDEC_ BYTE  xdata IR_KeyPadStatus;
_IRDEC_ BYTE  xdata IR_GetRepeatKey_DebounceCounter;
//_IRDEC_ BYTE  xdata IR_TransData;
_IRDEC_ union DW_TEMP xdata IR_Value;
_IRDEC_ union DW_TEMP xdata IR_TempValue;
#if IR_ENABLE_DEBUG
_IRDEC_ DWORD xdata IR_DebugKey;
#endif

_IRDEC_ BYTE xdata IRCodeTranCompleteFlag;

#define CURRENT_INPUT_IS_OPLAY() 	(SrcInputType == Input_Digital)

#define IR_CUSTOM   (IR_Value.valB[3]<<8|IR_Value.valB[2])
#define IR_DATA     (IR_Value.valB[1]<<8|IR_Value.valB[0])
#define REPEATABLE_KEY()    (IR_RepeatKey == IR_KEY_PLUS || IR_RepeatKey == IR_KEY_MINUS)

#endif






















