
#ifndef _CUSTOMER_JIEBO_VP58C11_H_
#define _CUSTOMER_JIEBO_VP58C11_H_

#define MainBoardType      MainBoard_TSUML88CMC2//MainBoard_TSUMP589MG_H//   MainBoard_TSUMO58FDT9_VP58C11  


#define PanelType                   PanelBOEMV238FHM_N30//PanelAU20SVGA//

//#define  HOTKEY_TYPE                    HOTKEY_XYD_5KEY 
#define DisplayLogo                 SUGON//NANCHANG//NoBrand // TERRA //   


#define Def_CTEMP				      CTEMP_6500K
#define LANG_Default			   HKC_LANG_SimChinese//   HKC_LANG_English 

#define AudioFunc                 		1
//#define AudioIC_CS8003_Enable         1
#define HDMI_EDID_Write_To_24C02  		1

//#define LEDBLModule_OnMainBoard     1

#define MinDutyValue      55//			    65//  0x00
#define MidDutyValue     100//			148//    0x40
#define MaxDutyValue   153//240ma      255//max   231//360ma 0x60

//#define DefBrightness    50
#define DefBrightness    100

#define Def9300KRedColor		128  
#define Def9300KGreenColor   	       119  
#define Def9300KBlueColor   		128  

#define Def6500KRedColor   		128
#define Def6500KGreenColor		111
#define Def6500KBlueColor  		108

#define Def5400KRedColor		 113
#define Def5400KGreenColor		 102
#define Def5400KBlueColor		 83 //96
//user color
#define DefRedColor		108 
#define DefGreenColor		108 
#define DefBlueColor		108  
#define DefColor			0x80

#define _CUSTOMER_VOLUME_TBL           { 0,      0x0390, \
									  50,    0xA5,\
									 100,  	0x45,\
										}


#endif
