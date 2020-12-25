#ifndef _ComboApp_H_
#define _ComboApp_H_

#ifdef _ComboApp_C_
#define _ComboApp_
#else
#define _ComboApp_  extern
#endif

extern Bool msAPI_combo_IPGetDDRFlag_ISR();
extern Bool msAPI_combo_IPGetDDRFlag();
extern void msAPI_combo_IPEnableDDRFlag(Bool Enable);
extern WORD msAPI_combo_IPGetPixelClk();
extern WORD msAPI_combo_IPGetGetHTotal();	

#endif

