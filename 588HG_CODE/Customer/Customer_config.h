//----------------------------------------------------------------------------------------------------
// ID Code      : Customer_config.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
//****************************************************************************
// Definitions by GLOBAL.h
//****************************************************************************
#ifndef _CUSTOMER_CONFIG_H_
#define _CUSTOMER_CONFIG_H_


#define Customer_VS_VP58C41        			0x01
#define Customer_VS_V8811        			0x02




#define Customer_JieBo_VP58C11      		0x60
#define Customer_JieBo_VSP58C31_3_IN_1      0x61

#define Customer_GUANRONG      				0x62
#define Customer_XINTAO      				0x63
#define Customer_CEHUA						0x64
#define Customer_HONGXUN					0x65
#define Customer_PENGUAN					0x66
#define Customer_TENGSHAN					0X67
#define Customer_KAINENG					0X68
#define Customer_FOSHANFANZHI				0x69


#define Customer_Type_Sel   	Customer_CEHUA//  Customer_FOSHANFANZHI//  Customer_TENGSHAN//Customer_CEHUA//	Customer_KAINENG// Customer_PENGUAN//Customer_JieBo_VSP58C31_3_IN_1//Customer_HONGXUN//

#ifndef Customer_Type_Sel  
#define Customer_Type_Sel         Customer_JieBo_VP58C11
#endif


#if(Customer_Type_Sel==Customer_VS_VP58C41)
#include "Customer_VS_VP58C41.h"

#elif(Customer_Type_Sel==Customer_JieBo_VP58C11)
#include "Customer_JieBo_VP58C11.h"

//#elif(Customer_Type_Sel==Customer_VS_V8811)
//#include "Customer_VS_V8811.h"

#elif(Customer_Type_Sel==Customer_JieBo_VSP58C31_3_IN_1)
#include "Customer_JieBo_VSP58C31_3_IN_1.h"
#elif(Customer_Type_Sel==Customer_GUANRONG)
#include "Customer_GUANRONG.h"
#elif(Customer_Type_Sel==Customer_XINTAO)
#include "Customer_XINTAO.h"
#elif(Customer_Type_Sel==Customer_CEHUA)
#include "Customer_CEHUA.h"
#elif(Customer_Type_Sel==Customer_HONGXUN)
#include "Customer_HONGXUN.h"
#elif(Customer_Type_Sel==Customer_PENGUAN)
#include "Customer_PENGUAN.h"
#elif(Customer_Type_Sel==Customer_TENGSHAN)
#include "Customer_TENGSHAN.h"
#elif(Customer_Type_Sel==Customer_KAINENG)
#include "Customer_KAINENG.h"
#elif(Customer_Type_Sel==Customer_FOSHANFANZHI)
#include "Customer_FOSHANFANZHI.h"


#endif

#ifndef ENABLE_UPDTAE_BURIN  
#define ENABLE_UPDTAE_BURIN       1// _DISABLE  //Ä¬ÈÏ´ò¿ª
#endif

#ifndef ENABLE_FACTORY_AUTOTEST  
#define ENABLE_FACTORY_AUTOTEST       1// _DISABLE
#endif

#ifndef ENABLE_BURNIN_MENU  
#define ENABLE_BURNIN_MENU        0//_DISABLE
#endif

#define EDIDName "MST2020"


#endif
