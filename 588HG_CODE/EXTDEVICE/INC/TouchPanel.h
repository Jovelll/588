
#include "global.h"

#ifndef TOUCH_PANEL_H
#define TOUCH_PANEL_H

#ifdef TOUCH_PANEL_C
#define TOUCH_PANEL_DEC
#else
#define TOUCH_PANEL_DEC extern
#endif

#define ARC_SPICMD_SET_ACCESS_MODE           0x00 // w, 0:read mode, 1:write mode 
#define ACCESS_MODE_READ    0
#define ACCESS_MODE_WRITE    1
#define ARC_SPICMD_ADDR_BYTE0                      0x01 // w, haddr[7:0]
#define ARC_SPICMD_ADDR_BYTE1                      0x02 // w, haddr[15:8]
#define ARC_SPICMD_ADDR_BYTE2                      0x03 // w, haddr[23:16]
#define ARC_SPICMD_ADDR_BYTE3                      0x04 // w, haddr[31:24]
#define ARC_SPICMD_WRDATA_BYTE0                 0x05 // 
#define ARC_SPICMD_WRDATA_BYTE1                 0x06
#define ARC_SPICMD_WRDATA_BYTE2                 0x07
#define ARC_SPICMD_WRDATA_BYTE3                 0x08

#define ARC_SPICMD_DATALEN_SELECT             0x09 // w, 0:8bit, 1:16bit, 2:32bit
#define DATALEN_SELECT_8BIT 0
#define DATALEN_SELECT_16BIT 1
#define DATALEN_SELECT_32BIT 2
#define ARC_SPICMD_RDDATA_BYTE0                 0x0a
#define ARC_SPICMD_RDDATA_BYTE1                 0x0b
#define ARC_SPICMD_RDDATA_BYTE2                 0x0c
#define ARC_SPICMD_RDDATA_BYTE3                 0x0d

#define ARC_SPICMD_ACCESS_START                 0x0f // start to access the sram
#define ARC_SPICMD_ACCESS_STATUS               0x0e // r, 0:bus is not ready, 1:bus is ready
#define ACCESS_STATUS_READY  1

#define SPI_SYS_CHK_CODE    0x90403400 // r
#define SPI_SYS_CHK_CODE_DEFAULT    0xdef1c0de

#define SPI_SYS_CTRL    0x90403404 // w, bit0=0: usb mode, bit0=1: spi mode
#define SPI_SYS_STATUS  0x90403408 // r, refer to above
#define SPI_SYS_USB_MODE     0x02
#define SPI_SYS_SPI_MODE     0x01

#define SPI_DATA_CTRL   0x9040340c  // w, bit0=0: stop data reading, bit0=1: start data reading
#define SPI_DATA_STATUS 0x90403410  // r, refer to above
#define SPI_DATA_STOP_READING   0
#define SPI_DATA_START_READING   1
#define SPI_DATA_INDEX  0x90403414 // r

#define SPI_CMD_CTRL   0x90403418  // w, bit0=0: stop data writding, bit0=1: start data writding
#define SPI_CMD_STATUS 0x9040341c  // w, refer to above
#define SPI_CMD_STOP_WRITING   0
#define SPI_CMD_START_WRITING   1
#define SPI_CMD_FINISH  0x90403420 // w

#define SPI_DATA0_3_0   0x90403500 // r
#define SPI_DATA0_7_4   0x90403504 // r
#define SPI_DATA0_11_8   0x90403508 // r
#define SPI_DATA0_15_12   0x9040350c // r

#define SPI_DATA1_3_0   0x90403510 // r
#define SPI_DATA1_7_4   0x90403514 // r
#define SPI_DATA1_11_8   0x90403518 // r
#define SPI_DATA1_15_12   0x9040351c // r

#define SPI_CMD_3_0   0x90403520 // w
#define SPI_CMD_7_4   0x90403524 // w

#if 0//ENABLE_EXTRA_KEY
#define SPI_CMD_MY_Computer_3_0   0x02080800
#define SPI_CMD_My_Computer_7_4   0 
#define SPI_CMD_Go_HomePage_3_0   0x02044a00
#define SPI_CMD_Go_HomePage_7_4   0 
#define SPI_CMD_Go_Prev_3_0   0x02045000
#define SPI_CMD_Go_Prev_7_4   0 
#define SPI_CMD_Go_Next_3_0   0x02044f00
#define SPI_CMD_Go_Next_7_4   0 
#define SPI_CMD_Null_Command_3_0   0x02000000
#define SPI_CMD_Null_Command_7_4   0 
#endif

#define WAIT_BUS_READY_COUNT    2 // 3 // retry for checking spi bus ready, counter range could be set 0~255
#define READ_STATUS_RETRY_COUNT     2 // 10 // retry for reading status, counter range could be set 0~255
#define SPI_POLLING_INTERVAL     20// unit: ms

//extern BYTE xdata TouchPanelPowerFlag;



//-----------------------------------------------
typedef union
{
	BYTE	valB[4]; // 0~3: 0x11, 0x22, 0x33, 0x44
	WORD	valW[2]; // 0~1: 0x1122, 0x3344
	DWORD   valDW; // 0x11223344
}uDWBuffer;

typedef struct
{
    BYTE ContactStatus;
    BYTE ContactID;
    WORD ContactX;
    WORD ContactY;
}sContact;

typedef union
{
    struct
    {
        BYTE Reserved0;
        sContact Contact[2];
        BYTE ActualCount;
        BYTE FrameNumber;
        BYTE Reserved1;
    }sTouch;
    uDWBuffer dwBuffer[4];
}uOutputData;

typedef struct
{
    BYTE StatusFlag;
#if 0//ENABLE_VIRTUAL_KEY_PAD
    BYTE StatusFlag2;
    WORD PowerOffCounter;
#endif
    BYTE PollingCounter;
    uOutputData OutputData;
    WORD FingerPoint;
    BYTE FingerStatus;
    BYTE OffsetX;
    BYTE OffsetY;
    WORD MainMenuStartX;
    WORD MainMenuStartY;
    WORD ValidAreaStartX;
    WORD ValidAreaStartY;
    WORD ValidAreaEndX;
    WORD ValidAreaEndY;
    WORD PreFingerPositionX;
    BYTE SliderActValue;
}sTouchPanelDataType;


typedef enum
{
    INIT_STATUS_FAIL,
    INIT_STATUS_PASS,
}InitStatusType;

#define INVALID_RANGE    0x7F
#define FINGER_IN_NEXT_MENU     BIT7


TOUCH_PANEL_DEC sTouchPanelDataType xdata TPL_Data;

//#define FINGER_LIFT_COUNT	3
//TOUCH_PANEL_DEC BYTE xdata FingerLiftCounter; // if finger lift, send lift status to htc 3 times for reducing dummy work

// HTC format
//#define FINGER_ON	TPL_Data.OutputData.dwBuffer[2].valB[0]
//#define Frame_Count	TPL_Data.OutputData.dwBuffer[3].valB[3]
#define FINGER_ON	TPL_Data.OutputData.dwBuffer[2].valB[3]
#define Frame_Count	TPL_Data.OutputData.dwBuffer[3].valB[0]

#define TPL_StatusFlag        TPL_Data.StatusFlag
#if 0//ENABLE_VIRTUAL_KEY_PAD
#define TPL_StatusFlag2        TPL_Data.StatusFlag2
#define TPL_KeyPowerOffCounter   TPL_Data.PowerOffCounter
#endif
#define TPL_PollingCounter      TPL_Data.PollingCounter
#define TPL_Conatct0_Status   TPL_Data.OutputData.sTouch.Contact[0].ContactStatus
#define TPL_Conatct0_ID     TPL_Data.OutputData.sTouch.Contact[0].ContactID
#define TPL_Conatct0_X      TPL_Data.OutputData.sTouch.Contact[0].ContactX
#define TPL_Conatct0_Y      TPL_Data.OutputData.sTouch.Contact[0].ContactY
#define TPL_Conatct1_Status   TPL_Data.OutputData.sTouch.Contact[1].ContactStatus
#define TPL_Conatct1_ID     TPL_Data.OutputData.sTouch.Contact[1].ContactID
#define TPL_Conatct1_X      TPL_Data.OutputData.sTouch.Contact[1].ContactX
#define TPL_Conatct1_Y      TPL_Data.OutputData.sTouch.Contact[1].ContactY
#define TPL_ActualCount     TPL_Data.OutputData.sTouch.ActualCount
#define TPL_ActualNumber     TPL_Data.OutputData.sTouch.FrameNumber
#define TPL_FingerPoint    TPL_Data.FingerPoint
#define TPL_FingerStatus    TPL_Data.FingerStatus
#define TPL_OffsetX     TPL_Data.OffsetX
#define TPL_OffsetY     TPL_Data.OffsetY
#define TPL_MainMenuStartX      TPL_Data.MainMenuStartX
#define TPL_MainMenuStartY      TPL_Data.MainMenuStartY
#define TPL_ValidAreaStartX     TPL_Data.ValidAreaStartX
#define TPL_ValidAreaStartY     TPL_Data.ValidAreaStartY
#define TPL_ValidAreaEndX     TPL_Data.ValidAreaEndX
#define TPL_ValidAreaEndY     TPL_Data.ValidAreaEndY
#define TPL_PreFingerPositionX  TPL_Data.PreFingerPositionX
#define TPL_SliderActValue      TPL_Data.SliderActValue

// whole osd left-up point
#define TPL_MenuMappingStartX   ((TPL_MainMenuStartX>TPL_OffsetX)?(TPL_MainMenuStartX-TPL_OffsetX):(0))
#define TPL_MenuMappingStartY   ((TPL_MainMenuStartY>TPL_OffsetY)?(TPL_MainMenuStartY-TPL_OffsetY):(0))

// osd valid range, start: left-up point, end: right-down point
#define TPL_ValidMappingStartX  ((TPL_ValidAreaStartX>TPL_OffsetX)?(TPL_ValidAreaStartX-TPL_OffsetX):(0))
#define TPL_ValidMappingStartY  ((TPL_ValidAreaStartY>TPL_OffsetY)?(TPL_ValidAreaStartY-TPL_OffsetY):(0))
#define TPL_ValidMappingEndX  ((TPL_ValidAreaEndX>TPL_OffsetX)?(TPL_ValidAreaEndX-TPL_OffsetX):(0))
#define TPL_ValidMappingEndY  ((TPL_ValidAreaEndY>TPL_OffsetY)?(TPL_ValidAreaEndY-TPL_OffsetY):(0))


/* original sequence
// 0~3
#define TPL_Reserved0   TPL_Data.OutputData.dwBuffer[0].valB[3]
#define TPL_Conatct0_Status   TPL_Data.OutputData.dwBuffer[0].valB[2]
#define TPL_Conatct0_ID     TPL_Data.OutputData.dwBuffer[0].valB[1]
#define TPL_Conatct0_X_L      TPL_Data.OutputData.dwBuffer[0].valB[0]

// 4~7
#define TPL_Conatct0_X_H      TPL_Data.OutputData.dwBuffer[1].valB[3]
#define TPL_Conatct0_Y_L      TPL_Data.OutputData.dwBuffer[1].valB[2]
#define TPL_Conatct0_Y_H      TPL_Data.OutputData.dwBuffer[1].valB[1]
#define TPL_Conatct1_Status   TPL_Data.OutputData.dwBuffer[1].valB[0]

// 8~11
#define TPL_Conatct1_ID     TPL_Data.OutputData.dwBuffer[2].valB[3]
#define TPL_Conatct1_X_L      TPL_Data.OutputData.dwBuffer[2].valB[2]
#define TPL_Conatct1_X_H      TPL_Data.OutputData.dwBuffer[2].valB[1]
#define TPL_Conatct1_Y_L      TPL_Data.OutputData.dwBuffer[2].valB[0]

//12~15
#define TPL_Conatct1_Y_H      TPL_Data.OutputData.dwBuffer[3].valB[3]
#define TPL_ActualCount      TPL_Data.OutputData.dwBuffer[3].valB[2]
#define TPL_ActualNumber      TPL_Data.OutputData.dwBuffer[3].valB[1]
#define TPL_Reserved1      TPL_Data.OutputData.dwBuffer[3].valB[0]
*/

#define InitPassBit _BIT0
#define ReadMenuStartBit   _BIT1
#define MainItemSelectedBit _BIT2
#define SubItemSelectedBit  _BIT3
#define SliderActiveBit     _BIT4
#define SlideRightBit _BIT5
#define GotoSelectedItemBit _BIT6
#if 0//ENABLE_EXTRA_KEY
#define ExtraItemSelectedBit  _BIT7
#endif
#if 0//ENABLE_VIRTUAL_KEY_PAD
#define KeyPadItemSelectedBit  BIT0
#define KeyRepeatActiveBit        BIT1
#define KeyPowerOnBit                 BIT2
#define KeyPowerOffBit            BIT3
#endif

#define TPL_InitPassFlag    (TPL_StatusFlag&InitPassBit) // indicate touch panel init pass
#define TPL_Set_InitPassFlag()  (TPL_StatusFlag|=InitPassBit)
#define TPL_Clr_InitPassFlag()  (TPL_StatusFlag&=~InitPassBit)

#define TPL_ReadMenuStartFlag  (TPL_StatusFlag&ReadMenuStartBit) // indicate it needs to read osd menu start position
#define TPL_Set_ReadMenuStartFlag() (TPL_StatusFlag|=ReadMenuStartBit)
#define TPL_Clr_ReadMenuStartFlag() (TPL_StatusFlag&=~ReadMenuStartBit)

#define TPL_MainItemSelectedFlag    (TPL_StatusFlag&MainItemSelectedBit) // indicate finger is on main item
#define TPL_Set_MainItemSelectedFlag()  (TPL_StatusFlag|=MainItemSelectedBit)
#define TPL_Clr_MainItemSelectedFlag()  (TPL_StatusFlag&=~MainItemSelectedBit)

#define TPL_SubItemSelectedFlag    (TPL_StatusFlag&SubItemSelectedBit) // indicate finger is on sub item
#define TPL_Set_SubItemSelectedFlag()  (TPL_StatusFlag|=SubItemSelectedBit)
#define TPL_Clr_SubItemSelectedFlag()  (TPL_StatusFlag&=~SubItemSelectedBit)

#define TPL_SliderActiveFlag  (TPL_StatusFlag&SliderActiveBit) // indicate adjust is by finger slide
#define TPL_Set_SliderActiveFlag()  (TPL_StatusFlag|=SliderActiveBit)
#define TPL_Clr_SliderActiveFlag()  (TPL_StatusFlag&=~SliderActiveBit)


#define TPL_SlideRightFlag    (TPL_StatusFlag&SlideRightBit) // indicate finger is shifting to right
#define TPL_Set_SlideRightFlag()    (TPL_StatusFlag|=SlideRightBit)
#define TPL_Clr_SlideRightFlag()    (TPL_StatusFlag&=~SlideRightBit)

#define TPL_GotoSelectedItemFlag     (TPL_StatusFlag&GotoSelectedItemBit)
#define TPL_Set_GotoSelectedItemFlag()     (TPL_StatusFlag|=GotoSelectedItemBit)
#define TPL_Clr_GotoSelectedItemFlag()     (TPL_StatusFlag&=~GotoSelectedItemBit)

#if 0//ENABLE_EXTRA_KEY
#define TPL_ExtraItemSelectedFlag    (TPL_StatusFlag&ExtraItemSelectedBit) // indicate finger is on sub item
#define TPL_Set_ExtraItemSelectedFlag()  (TPL_StatusFlag|=ExtraItemSelectedBit)
#define TPL_Clr_ExtraItemSelectedFlag()  (TPL_StatusFlag&=~ExtraItemSelectedBit)
#endif

#if 0//ENABLE_VIRTUAL_KEY_PAD
#define TPL_KeyPadItemSelectedFlag    (TPL_StatusFlag2&KeyPadItemSelectedBit) // indicate finger is on sub item
#define TPL_Set_KeyPadItemSelectedFlag()  (TPL_StatusFlag2|=KeyPadItemSelectedBit)
#define TPL_Clr_KeyPadItemSelectedFlag()  (TPL_StatusFlag2&=~KeyPadItemSelectedBit)

#define TPL_KeyRepeatActiveFlag    (TPL_StatusFlag2&KeyRepeatActiveBit) // indicate finger is on sub item
#define TPL_Set_KeyRepeatActiveFlag()  (TPL_StatusFlag2|=KeyRepeatActiveBit)
#define TPL_Clr_KeyRepeatActiveFlag()  (TPL_StatusFlag2&=~KeyRepeatActiveBit)

#define TPL_KeyPowerOnFlag    (TPL_StatusFlag2&KeyPowerOnBit) // indicate finger is on sub item
#define TPL_Set_KeyPowerOnFlag()  (TPL_StatusFlag2|=KeyPowerOnBit)
#define TPL_Clr_KeyPowerOnFlag()  (TPL_StatusFlag2&=~KeyPowerOnBit)

#define TPL_KeyPowerOffFlag    (TPL_StatusFlag2&KeyPowerOffBit) // indicate finger is on sub item
#define TPL_Set_KeyPowerOffFlag()  (TPL_StatusFlag2|=KeyPowerOffBit)
#define TPL_Clr_KeyPowerOffFlag()  (TPL_StatusFlag2&=~KeyPowerOffBit)
#endif

#define SUB_MAX_ITEM    5
#define FINGER_RANGE_TOLERANCE    10 // unit: pixel
#define FINGER_IS_FIXED   10 // unit: pixel
#define SLIDE_LONG_DISTANCE     20
#define SLIDE_NORMAL_DISTANCE     10
#define SLIDE_SHORT_DISTANCE     5
#define SLIDE_SLOW_DOWN_DIVIDER     5

#define SUBITEM_OFFSET  5 // for sub item index
#define MenuItemIndex_Finger  ((TPL_SubItemSelectedFlag)?(TPL_FingerStatus-SUBITEM_OFFSET):(TPL_FingerStatus))

#if 0//ENABLE_VIRTUAL_KEY_PAD
#define KEY_PAD_MAX_ITEM    5
#define KEYPAD_ITEM_OFFSET  10 // for KEY PAD item index
#define TPL_VirtualKeyStartX    568
#define TPL_VirtualKeyStartY    1107//270
#define TPL_KEY_POWEROFF_INTERVAL       2000 // unit: ms
#endif

#if 0//ENABLE_EXTRA_KEY
#define EXTRA_KEY_MAX_ITEM    4
#define EXTRA_KEY_ITEM_OFFSET  14 // for KEY PAD item index
#endif

#define TPLDischargeTime 30 //unit ms //2011.11.14 cc

TOUCH_PANEL_DEC BOOL SPI_Write32Bit(DWORD addr, DWORD val);
TOUCH_PANEL_DEC BOOL SPI_Read32Bit(DWORD addr, DWORD *val);
TOUCH_PANEL_DEC BOOL TPL_InitSetting(void);
TOUCH_PANEL_DEC BOOL TPL_ReadSurface(void);
TOUCH_PANEL_DEC void GetMenuXYStartPoint(void);

#if ENABLE_TOUCH_PANEL
TOUCH_PANEL_DEC void SI_Cbus_TouchPanel2Mhl(BYTE *buf, BYTE count); //2011.10.31 cc
#endif

//TOUCH_PANEL_DEC void GetValidArea(bit getstartposi, BYTE menupage);
TOUCH_PANEL_DEC BOOL IsFingerOnValidArea(void);
//TOUCH_PANEL_DEC BYTE GetSelectedItem();
TOUCH_PANEL_DEC void ParseFingerStatus(void);
TOUCH_PANEL_DEC BYTE ParseMenuAction(void);
    
#if 0//ENABLE_EXTRA_KEY
TOUCH_PANEL_DEC BOOL TPL_WriteCommand(DWORD addr, DWORD val);
TOUCH_PANEL_DEC void ExecuteExtraCommand(BYTE Command );
#endif

TOUCH_PANEL_DEC void TouchPanelHandler(void);

#endif

