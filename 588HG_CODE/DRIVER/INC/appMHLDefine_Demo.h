
//---------------------------------------------------------------------------------------------------
// EDID
//---------------------------------------------------------------------------------------------------
#define ENABLE_MHL_EDID_IN_FLASH    0
#define MHL_EDID_START  0xD000ul

#if (!ENABLE_MHL_EDID_IN_FLASH)
#ifdef _MAPI_MHL_C_
code BYTE Mstar_MHL_EDID[256] =
{
#if(MHL_3D_DISPLAY_SUPPORT) // 3D EDID
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x36,0x74,0x30,0x00,0x01,0x00,0x00,0x00,
    0x0A,0x14,0x01,0x03,0x80,0x73,0x41,0x78,0x0A,0xCF,0x74,0xA3,0x57,0x4C,0xB0,0x23,
    0x09,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,0x01,0xFF,0x01,0xFF,0xFF,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x20,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,
    0x55,0x00,0xC4,0x8E,0x21,0x00,0x00,0x1E,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,
    0x58,0x2C,0x25,0x00,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0xFC,0x00,0x4D,
    0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6D,0x6F,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
    0x00,0x3B,0x3C,0x1F,0x2D,0x08,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x2E,
    0x02,0x03,0x2A,0xF2,0x4D,0x05,0x84,0x03,0x01,0x12,0x93,0x14,0x16,0x07,0x10,0x1F,
    0x20,0x22,0x26,0x09,0x07,0x07,0x11,0x07,0x06,0x83,0x01,0x00,0x00,0x6C,0x03,0x0C,
    0x00,0x10,0x00,0xB8,0x2D,0x20,0xA0,0x02,0x01,0x41,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,
    0x2D,0x10,0x10,0x3E,0x96,0x00,0xC4,0x8E,0x21,0x00,0x00,0x18,0x8C,0x0A,0xA0,0x14,
    0x51,0xF0,0x16,0x00,0x26,0x7C,0x43,0x00,0xC4,0x8E,0x21,0x00,0x00,0x99,0x01,0x1D,
    0x00,0xBC,0x52,0xD0,0x1E,0x20,0xB8,0x28,0x55,0x40,0xC4,0x8E,0x21,0x00,0x00,0x1F,
    0x01,0x1D,0x80,0xD0,0x72,0x1C,0x16,0x20,0x10,0x2C,0x25,0x80,0xC4,0x8E,0x21,0x00,
    0x00,0x9E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8,
            
#else
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x36,0x74,0x30,0x00,0x01,0x00,0x00,0x00,
    0x18,0x0F,0x01,0x03,0x80,0x73,0x41,0x78,0x0A,0xCF,0x74,0xA3,0x57,0x4C,0xB0,0x23,
    0x09,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,0x01,0xFF,0x01,0xFF,0xFF,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x20,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,
    0x55,0x00,0xC4,0x8E,0x21,0x00,0x00,0x1E,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,
    0x58,0x2C,0x25,0x00,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0xFC,0x00,0x4D,
    0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6D,0x6F,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
    0x00,0x3B,0x3C,0x1F,0x2D,0x08,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x25,
    0x02,0x03,0x1D,0xF2,0x4A,0x05,0x84,0x03,0x02,0x01,0x12,0x93,0x14,0x16,0x07,0x23,
    0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,0x10,0x00,0x8C,0x0A,0xD0,
    0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,0x00,0xC4,0x8E,0x21,0x00,0x00,0x18,0x8C,
    0x0A,0xA0,0x14,0x51,0xF0,0x16,0x00,0x26,0x7C,0x43,0x00,0xC4,0x8E,0x21,0x00,0x00,
    0x99,0x01,0x1D,0x00,0xBC,0x52,0xD0,0x1E,0x20,0xB8,0x28,0x55,0x40,0xC4,0x8E,0x21,
    0x00,0x00,0x1F,0x01,0x1D,0x80,0xD0,0x72,0x1C,0x16,0x20,0x10,0x2C,0x25,0x80,0xC4,
    0x8E,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x68,
    
#endif    
};
#endif
#endif

//--------------------------------------------------------------------------------------------------
// EQ
//--------------------------------------------------------------------------------------------------
#define MHL_B_CHANNEL_EQ                9

//--------------------------------------------------------------------------------------------------
// ICONTROL
//--------------------------------------------------------------------------------------------------
#define MHL_ICONTROL_VALUE              5
#define MHL_PP_ICONTROL_VALUE           7

//--------------------------------------------------------------------------------------------------
// Force CDR
//--------------------------------------------------------------------------------------------------
#define MHL_FORCE_CDR_COUNTER           100

//--------------------------------------------------------------------------------------------------
// link method selection
//--------------------------------------------------------------------------------------------------
#define MHL_CBUS_AUTO_SEARCH_LINK       0   // Auto search cable detect and link when off line
#define MHL_CBUS_FIXED_PORT_LINK        1   // Just for on line port to link

#define MHL_CBUS_LINK_MODE              MHL_CBUS_AUTO_SEARCH_LINK

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define MHL_DEBUG_MESSAGE           1

#define MHL_VENDOR_ID               0x00 // ?
#define MHL_ADOPTER_ID              0x02CB // Mstar: 715
#define MHL_DEVICE_ID               0x0000 // Chip ID?
#define MHL_RCP_SUPPORT             1   // For CTS1.2 must be set
#define MHL_RAP_SUPPORT             1   // For CTS1.2 must be set

#if ENABLE_MHL_CTS_TEST
#define MHL_CTS_SEND_MSC            1   // Sent MSC command for CTS
#else
#define MHL_CTS_SEND_MSC            0   // Sent MSC command for CTS
#endif

#define MHL_USER_DEVICE_CAPABILITY  0   // User define device capacibility
#define MHL_READ_DEVICE_CAPABILITY  1   // Read source device capacibility
#define MHL_READ_DEVICE_VENDER_ID   0   // Read source device vender ID

//-------------------------------------------------------------------------------------------------
//  Interface Link
//-------------------------------------------------------------------------------------------------
#define MHL_CURRENT_PORT            SrcInputType

#define MHL_SELECT_PORT0            Input_HDMI
#define MHL_SELECT_PORT1            Input_HDMI2
#define MHL_SELECT_PORT2            Input_HDMI3

#define MHL_HDMI_VCO_LOOP_DIV2      HDMI_VCO_LOOP_DIV2
#define MHL_HDMI_VCO_LOOP_DIV4      HDMI_VCO_LOOP_DIV4
#define MHL_HDMI_VCO_LOOP_DIV8      HDMI_VCO_LOOP_DIV8

#define MHL_HDMI_B_CHANNEL_EQ       HDMI_B_CHANNEL_EQ

#define MHL_PLL_LDO_VALUE           HDMI_PLL_LDO_VALUE

#ifdef ENABLE_MHL_C1
#define MHL_SUPPORT_PORT0           ENABLE_MHL_C1
#else
#define MHL_SUPPORT_PORT0           0
#endif

#ifdef ENABLE_MHL_C2
#define MHL_SUPPORT_PORT1           ENABLE_MHL_C2
#else
#define MHL_SUPPORT_PORT1           0
#endif

#ifdef ENABLE_MHL_C3
#define MHL_SUPPORT_PORT2           ENABLE_MHL_C3
#else
#define MHL_SUPPORT_PORT2           0
#endif

#ifdef XDATA_MHL_CBUS_START
#define MHL_CBUS_SRAM_ADDRESS       XDATA_MHL_CBUS_START
#else
#define MHL_CBUS_SRAM_ADDRESS       0x4000
#endif

#if(MHL_SUPPORT_PORT0)
#define MHL_CABLE_SAR_DETECT_PORT0  hwDVI0Cable_Pin
#define SET_VBUS_CHARGE_PORT0()     SET_MHL_PS_CTRL_CHARGE0()
#define CLR_VBUS_CHARGE_PORT0()     CLR_MHL_PS_CTRL_CHARGE0()
#define SET_HDMI_HPD_PORT0()        hw_Set_HdcpHpd()
#define CLR_HDMI_HPD_PORT0()        hw_Clr_HdcpHpd()
#define SET_HPD_TRISTSTE_PORT0()    hw_Set_MHLTriStateHpd()
#define MHL_CABLE_DETECT_PORT0()    (GET_MHL_VBUS_PORT0_FLAG())
#else
#define MHL_CABLE_DETECT_PORT0()    (FALSE)
#endif

#if(MHL_SUPPORT_PORT1)
#define MHL_CABLE_SAR_DETECT_PORT1  hwDVI1Cable_Pin
#define SET_VBUS_CHARGE_PORT1()     SET_MHL_PS_CTRL_CHARGE1()
#define CLR_VBUS_CHARGE_PORT1()     CLR_MHL_PS_CTRL_CHARGE1()
#define SET_HDMI_HPD_PORT1()        hw_Set_HdcpHpd2()
#define CLR_HDMI_HPD_PORT1()        hw_Clr_HdcpHpd2()
#define SET_HPD_TRISTSTE_PORT1()    hw_Set_MHLTriStateHpd2()
#define MHL_CABLE_DETECT_PORT1()    (GET_MHL_VBUS_PORT1_FLAG())
#else
#define MHL_CABLE_DETECT_PORT1()    (FALSE)
#endif

#if(MHL_SUPPORT_PORT2)
#define MHL_CABLE_SAR_DETECT_PORT2  hwDVI2Cable_Pin
#define SET_VBUS_CHARGE_PORT2()     SET_MHL_PS_CTRL_CHARGE2()
#define CLR_VBUS_CHARGE_PORT2()     CLR_MHL_PS_CTRL_CHARGE2()
#define SET_HDMI_HPD_PORT2()        hw_Set_HdcpHpd3()
#define CLR_HDMI_HPD_PORT2()        hw_Clr_HdcpHpd3()
#define SET_HPD_TRISTSTE_PORT2()    hw_Set_MHLTriStateHpd3()
#define MHL_CABLE_DETECT_PORT2()    (GET_MHL_VBUS_PORT2_FLAG())
#else
#define MHL_CABLE_DETECT_PORT2()    (FALSE)
#endif

#define MHL_HW_RTERM_SUPPORT        0
#define MHL_RCP_FUNCTION_SUPPORT    0
#define MHL_RAP_FUNCTION_SUPPORT    0
#define MHL_UCP_FUNCTION_SUPPORT    0
#define MHL_WRITE_BURST_SUPPORT     0
#define MHL_3D_DISPLAY_SUPPORT      0
#define MHL_HID_FUNCTION_SUPPORT    0
#define MHL_PM_USE_RTERM_HW_MODE    1
#define MHL_POWER_DOWN_CBUS_LINK    0

// Cell phone patch
#define MHL_ASUS_PADFONE2_PATCH     1
#define MHL_HTC_CHARGE_PATCH        1
#define MHL_SONY_CHARGE_PATCH       1
#define MHL_HUAWEI_PHONE_PATCH      1
#define MHL_LG_PRADA_DC_PATCH       0

#if(MHL_USER_DEVICE_CAPABILITY)
//-------------------------------------------------------------------------------------------------
//  User define device capacibility
//-------------------------------------------------------------------------------------------------
#define MHL_DEVICE_STATE            0x00
#define MHL_VER_MAJOR               2
#define MHL_VER_MINOR               0
#define MHL_DEVICE_TYPE             1
#define MHL_DEVICE_POW              1
#define MHL_DEVICE_PLIM             1 // 0: 500mA / 1: 900mA / 2: 1.5A
#define MHL_SUPP_RGB444             1
#define MHL_SUPP_YCBCR444           1
#define MHL_SUPP_YCBCR422           1
#define MHL_SUPP_PPIXEL             1
#define MHL_SUPP_ISLANDS            1
#define MHL_SUPP_VGA                1
#define MHL_AUD_2CH                 1
#define MHL_AUD_8CH                 0
#define MHL_VT_GRAPHICS             0
#define MHL_VT_PHOTO                0
#define MHL_VT_CINEMA               0
#define MHL_VT_GAME                 0
#define MHL_SUPP_VT                 0
#define MHL_LD_DISPLAY              1
#define MHL_LD_VIDEO                0
#define MHL_LD_AUDIO                0
#define MHL_LD_MEDIA                0
#define MHL_LD_TUNER                0
#define MHL_LD_RECORD               0
#define MHL_LD_SPEAKER              0
#define MHL_LD_GUI                  0
#define MHL_BANDWIDTH               0x0F
#define MHL_SP_SUPPORT              1
#define MHL_UCP_SEND_SUPPORT        0
#define MHL_UCP_RECV_SUPPORT        0
#define MHL_SCRATCHPAD_SIZE         0x10
#define MHL_INT_SIZE                3
#define MHL_STATE_SIZE              3

#define MHL_DEV_STATE_0x00          MHL_DEVICE_STATE
#define MHL_VERSION_0x01            (MHL_VER_MINOR |(MHL_VER_MAJOR <<4))
#define MHL_DEV_CAT_0x02            (MHL_DEVICE_TYPE |(MHL_DEVICE_POW <<4)| (MHL_DEVICE_PLIM <<5))
#define MHL_ADOPTER_ID_H_0x03       (MHL_ADOPTER_ID >>8)
#define MHL_ADOPTER_ID_L_0x04       (MHL_ADOPTER_ID &0xFF)
#define MHL_VID_LINK_MODE_0x05      (MHL_SUPP_RGB444 |(MHL_SUPP_YCBCR444 <<1) |(MHL_SUPP_YCBCR422 <<2) |(MHL_SUPP_PPIXEL <<3) |(MHL_SUPP_ISLANDS <<4) |(MHL_SUPP_VGA <<5))
#define MHL_AUD_LINK_MODE_0x06      (MHL_AUD_2CH |(MHL_AUD_8CH <<1))
#define MHL_VIDEO_TYPE_0x07         (MHL_VT_GRAPHICS |(MHL_VT_PHOTO <<1) |(MHL_VT_CINEMA <<2) |(MHL_VT_GAME <<3) |(MHL_SUPP_VT <<7))
#define MHL_LOG_DEV_MAP_0x08        (MHL_LD_DISPLAY |(MHL_LD_VIDEO <<1) |(MHL_LD_AUDIO <<2) |(MHL_LD_MEDIA <<3) |(MHL_LD_TUNER <<4) |(MHL_LD_RECORD <<5) |(MHL_LD_SPEAKER <<6) |(MHL_LD_GUI <<7))
#define MHL_BANDWIDTH_0x09          MHL_BANDWIDTH
#define MHL_FEATURE_FLAG_0x0A       (MHL_RCP_SUPPORT |(MHL_RAP_SUPPORT <<1) |(MHL_SP_SUPPORT <<2) |(MHL_UCP_SEND_SUPPORT <<3) |(MHL_UCP_RECV_SUPPORT <<4))
#define MHL_DEVICE_ID_H_0x0B        (MHL_DEVICE_ID >>8)
#define MHL_DEVICE_ID_L_0x0C        (MHL_DEVICE_ID &0xFF)
#define MHL_SCRATCHPAD_SIZE_0x0D    MHL_SCRATCHPAD_SIZE
#define MHL_INT_STAT_0x0E           (MHL_INT_SIZE |(MHL_STATE_SIZE <<4))

#else
#define MHL_DEV_STATE_0x00          0x00
#define MHL_VERSION_0x01            0x20
#define MHL_DEV_CAT_0x02            0x31
#define MHL_ADOPTER_ID_H_0x03       (MHL_ADOPTER_ID >>8)
#define MHL_ADOPTER_ID_L_0x04       (MHL_ADOPTER_ID &0xFF)
#define MHL_VID_LINK_MODE_0x05      0x3F //0x37
#define MHL_AUD_LINK_MODE_0x06      0x01
#define MHL_VIDEO_TYPE_0x07         0x00
#define MHL_LOG_DEV_MAP_0x08        0x01
#define MHL_BANDWIDTH_0x09          0x0F
#define MHL_FEATURE_FLAG_0x0A       0x07
#define MHL_DEVICE_ID_H_0x0B        (MHL_DEVICE_ID >>8)
#define MHL_DEVICE_ID_L_0x0C        (MHL_DEVICE_ID &0xFF)
#define MHL_SCRATCHPAD_SIZE_0x0D    0x10
#define MHL_INT_STAT_0x0E           0x33

#endif

#if(MHL_READ_DEVICE_CAPABILITY)
//-------------------------------------------------------------------------------------------------
//  Read device capacibility
//-------------------------------------------------------------------------------------------------
#define MHL_READ_DEVCAP_0x00        0   // Device state
#define MHL_READ_DEVCAP_0x01        0   // MHL version
#define MHL_READ_DEVCAP_0x02        0   // Device category
#define MHL_READ_DEVCAP_0x03        1   // Adopter ID high byte
#define MHL_READ_DEVCAP_0x04        1   // Adopter ID low byte
#define MHL_READ_DEVCAP_0x05        0   // Video link mode support
#define MHL_READ_DEVCAP_0x06        0   // Audio link mode support
#define MHL_READ_DEVCAP_0x07        0   // Video type support
#define MHL_READ_DEVCAP_0x08        0   // Logical device map
#define MHL_READ_DEVCAP_0x09        0   // Link bandwidth limit
#define MHL_READ_DEVCAP_0x0A        1   // Feature flags
#define MHL_READ_DEVCAP_0x0B        1   // Device ID high byte
#define MHL_READ_DEVCAP_0x0C        1   // Device ID low byte
#define MHL_READ_DEVCAP_0x0D        0   // Scratchpad size
#define MHL_READ_DEVCAP_0x0E        0   // Interrupt /status size
#define MHL_READ_DEVCAP_0x0F        0   // Reserved

#define MHL_READ_DEVCAP_L           (MHL_READ_DEVCAP_0x00 |(MHL_READ_DEVCAP_0x01 <<1) |(MHL_READ_DEVCAP_0x02 <<2) |(MHL_READ_DEVCAP_0x03 <<3)\
                                    |(MHL_READ_DEVCAP_0x04 <<4) |(MHL_READ_DEVCAP_0x05 <<5) |(MHL_READ_DEVCAP_0x06 <<6) |(MHL_READ_DEVCAP_0x07 <<7))
#define MHL_READ_DEVCAP_H           (MHL_READ_DEVCAP_0x08 |(MHL_READ_DEVCAP_0x09 <<1) |(MHL_READ_DEVCAP_0x0A <<2) |(MHL_READ_DEVCAP_0x0B <<3)\
                                    |(MHL_READ_DEVCAP_0x0C <<4) |(MHL_READ_DEVCAP_0x0D <<5) |(MHL_READ_DEVCAP_0x0E <<6) |(MHL_READ_DEVCAP_0x0F <<7))
#define MHL_READ_DEVCAP_MASK        (MHL_READ_DEVCAP_L |(MHL_READ_DEVCAP_H << 8))

#define MHL_DEVICE_CAPABILITY_SIZE  16

#endif

//----------------------------------------------------------------------------------------------
// CTS test, tx send key to monitor
//----------------------------------------------------------------------------------------------
#define MHL_TX_SEND_RCP_KEY_UP()                    (KeypadButton = CBUS_RCP_BTN_Minus, Set_bKeyReadyFlag())
#define MHL_TX_SEND_RCP_KEY_DOWN()              (KeypadButton = CBUS_RCP_BTN_Plus, Set_bKeyReadyFlag())
#define MHL_TX_SEND_RCP_KEY_ROOT_MENU()     (KeypadButton = CBUS_RCP_BTN_Menu, Set_bKeyReadyFlag())
#define MHL_TX_SEND_RCP_KEY_EXIT()                  (KeypadButton = CBUS_RCP_BTN_Exit, Set_bKeyReadyFlag())

//----------------------------------------------------------------------------------------------
// CTS test, monitor send key to tx
//----------------------------------------------------------------------------------------------
#if ENABLE_MHL_CTS_TEST
#define IS_MENU_KEY_PRESSED()   (KeypadButton == BTN_Menu)
#define IS_EXIT_KEY_PRESSED()   (KeypadButton == BTN_Exit)
#define IS_DOWN_KEY_PRESSED()   (KeypadButton == BTN_Plus)
#define IS_PLUS_KEY_PRESSED()   (KeypadButton == BTN_Minus)
#define IS_REPEAT_KEY_PRESSED()   (KeypadButton == BTN_Repeat)
#define IS_NOTHING_KEY_PRESSED()   (KeypadButton == BTN_Nothing)
#endif

