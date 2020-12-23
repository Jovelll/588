/********************************************************************************/

/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

#ifndef _CUSTOMER_VS_VP58C41_H_
#define _CUSTOMER_VS_VP58C41_H_

#define MainBoardType       MainBoard_TSUMO58FDT9_VP58C41_B 

#define PanelType                   PanelM185BGE_L22  

//#define  HOTKEY_TYPE                    HOTKEY_XYD_5KEY 

#define Def_CTEMP				      CTEMP_6500K
#define LANG_Default			      HKC_LANG_English  


#define OSD_INPUT_SUBMENU_ENABLE         0

#define AudioFunc                 		1
//#define LEDBLModule_OnMainBoard     1


#define EarphoneDet_Enable				1


#define _CUSTOMER_VOLUME_TBL           { 0,      0x390, \
									   1,     0x16c,\
									  50,    0x68,\
									 100,  	0x18,\
										}


#define MinDutyValue        0x00
#define MidDutyValue        0x40
#define MaxDutyValue       0x7b

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

//IR FUNCTION
#define EnableTime1Interrupt  		1

#define IR_FUNC_EN				1

#define	IR_CUSTOMER_CODE		0x20df

#define	IR_KEY_POWER			0x48b7
#define	IR_KEY_MENU			0x4ab5
#define	IR_KEY_LEFT			0x49b6
#define	IR_KEY_RIGHT			0x4bb4
#define	IR_KEY_UP				0x47b8
#define	IR_KEY_DOWN			0x4db2
#define	IR_KEY_EXIT	              0x01fe
#define	IR_KEY_ENTRY	              0x0ef1
#define   IR_Hot43CHING169     	0x0af5
#define   IR_HotChangeSource  	0x40bf
#define   IR_HotMUTE				0x0bf4


#endif
