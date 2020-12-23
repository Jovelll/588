#ifndef _VS_COMPILER_OPTION_H_
#define _VS_COMPILER_OPTION_H_

#ifndef EarphoneDet_Enable
#define EarphoneDet_Enable			0
#endif

#ifndef IR_FUNC_EN
#define IR_FUNC_EN				0
#endif

#define Display_Checksum			1
#define BlacklitTimeOnShow         	1

#ifndef DefHDMIAudioType
#define DefHDMIAudioType         1//Audio_HDMI
#endif

#ifndef Audio_Switch
#define Audio_Switch         0
#endif

#ifndef AudioIC_CS8003_Enable
#define AudioIC_CS8003_Enable         0
#endif 

#ifndef OSD_INPUT_SUBMENU_ENABLE
#define OSD_INPUT_SUBMENU_ENABLE         1
#endif

#ifndef FACTORY_CLEAR_EDID_WP_ENABLE
#define FACTORY_CLEAR_EDID_WP_ENABLE         0
#endif

#ifndef VGA_EDID_Write_To_24C02
#define VGA_EDID_Write_To_24C02         0   //     1:  VGA >>24c02      0: VGA BUFFER
#endif

#ifndef DVI_EDID_Write_To_24C02
#define DVI_EDID_Write_To_24C02         0  //      1: DVI >> 24C02      0: DVI BUFFER
#endif

#ifndef HDMI_EDID_Write_To_24C02
#define HDMI_EDID_Write_To_24C02         0  //      1: HDMI >> 24C02      0: HDMI BUFFER
#endif    

#ifndef Enable_AdjustDCR
#define Enable_AdjustDCR         1
#endif 

#ifndef LowBlueLight
#define LowBlueLight        0// 0
#endif 

#ifndef LOCKPOWER_USE_OTHER_KEY
#define LOCKPOWER_USE_OTHER_KEY         0
#endif 

#ifndef _AUTOCOLOR_IN_SETTING_MENU_ENABLE
#define _AUTOCOLOR_IN_SETTING_MENU_ENABLE         1
#endif 












#define EN_HotKeyDCRMenuWithIcon			0
#define EN_HotKeyECOMenuWithIcon			0


#define KeyInputSelectMenu			1
#define Enable_Menu_LockOSD			1
#define Enable_LockPowerKey			1
#define Los_Select			0

#define ShowSourceType			1

#define EN_HotKeyTimes		0//	1

#define EN_HotKeyXINGZHUN	0//	1



#define FactoryAuto_9300K			1
#define FactoryAuto_6500K			1
#define FactoryAuto_5400K			1



#define InputType_Analog1			1
#define InputType_Digital			0
#define InputType_HDMI			1


#define DualInput			1
#define MiscMenuMuteItem			0


#define OSD_OD                            			     ENABLE_RTE
#define Digital_OSDUnShow_PictureAjust			1


#define SevenKey_Function					0
#define ENABLE_SIX_KEY               0


#define OpenAction				1
#define TimerComboKey 			1


#define Enable_Lightsensor                  0
#define ENABLE_SUPERRESOLUTION                  0


#if Customer_Type_Sel == Customer_PENGUAN || Customer_TENGSHAN
#define Enable_DIGITAL_DIMMING		1//默认关闭 数字调光,鹏冠客户打开
#else
#define Enable_DIGITAL_DIMMING		0//默认关闭 数字调光,鹏冠客户打开
#endif


#define Enable_4LINE_KEY		0//	1






#endif


