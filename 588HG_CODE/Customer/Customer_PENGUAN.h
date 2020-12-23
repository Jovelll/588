
#ifndef _CUSTOMER_PENGUAN_H_
#define _CUSTOMER_PENGUAN_H_

#define MainBoardType     MainBoard_TSUMP589MG_H // MainBoard_TSUMO58FDT9_VSP58C31_3_IN_1   


#define PanelType           PanelM185BGE_L22//PanelBOEMV238FHM_N30// 

#define DisplayLogo             DRAZILPC//NoBrand//SAMPIN//
#define Enable_AdjustDCR         1//0
#define LowBlueLight      		0//  1


#define Def_CTEMP				      CTEMP_5400K//CTEMP_6500K
#define LANG_Default			   HKC_LANG_Turkish//HKC_LANG_English //HKC_LANG_SimChinese//HKC_LANG_Portuguese//   

#define AudioFunc                 		1
#define AudioIC_CS8003_Enable         0

#define VGA_EDID_Write_To_24C02         0//     1                           
#define HDMI_EDID_Write_To_24C02  		0//1
//#define FACTORY_CLEAR_EDID_WP_ENABLE         1
#define LOCKPOWER_USE_OTHER_KEY         1
#define _AUTOCOLOR_IN_SETTING_MENU_ENABLE         0


#if 1   //21.5
#define MinDutyValue  	55//  			
#define MidDutyValue   	140//	
#define MaxDutyValue   	231//360mA 

#define DefBrightness   100

#define Def9300KRedColor		98//100  
#define Def9300KGreenColor   	       103//104
#define Def9300KBlueColor   		110  

#define Def6500KRedColor   		110
#define Def6500KGreenColor		110
#define Def6500KBlueColor  		106

#define Def5400KRedColor		 110
#define Def5400KGreenColor		 106
#define Def5400KBlueColor		 95//97
#else  //23.8
#define MinDutyValue        0x97//0x90
#define MidDutyValue        0xb3//0xa5
#define MaxDutyValue       0xb7 //0xc4 //0xba   //

#define Def9300KRedColor		98  
#define Def9300KGreenColor   	       90 
#define Def9300KBlueColor   		110  

#define Def6500KRedColor   		110
#define Def6500KGreenColor		95
#define Def6500KBlueColor  		98

#define Def5400KRedColor		 110
#define Def5400KGreenColor		 93
#define Def5400KBlueColor		 89

#endif

//user color
#define DefRedColor		108 
#define DefGreenColor		108 
#define DefBlueColor		108  
#define DefColor			0x80

#define _CUSTOMER_VOLUME_TBL           { 0,      0x390, \
									   1,     0x16c,\
									  50,    0x90,\
									 100,  	0x60,\
										}


#endif
