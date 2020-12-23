
// CHIP_TSUMU
// interrupt
#define INT_FIQ_TIMER0_ENABLE(Enable)    (msWriteByteMask( REG_2B00, ((Enable)?(0):(BIT0)), BIT0 ))   
#define INT_FIQ_WDT_ENABLE(Enable)    (msWriteByteMask( REG_2B00, ((Enable)?(0):(BIT1)), BIT1 ))
#define INT_IRQ_DISP_ENABLE(Enable)    (msWriteByteMask( REG_2B19, ((Enable)?(0):(BIT2)), BIT2 ))
#define INT_STATUS_DISP_ISR()               (_bit2_(msRegs[REG_2B29])) // U, Y
#define INT_IRQ_DVI_ENABLE(Enable)    (msWriteByteMask( REG_2B1B, ((Enable)?(0):(BIT7)), BIT7 ))
#define INT_IRQ_D2B_ENABLE(Enable)  (msWriteByteMask(REG_2B1B, (Enable)?(0):(BIT2), BIT2))
#define INT_STATUS_D2B_ISR()    (_bit2_( msRegs[REG_2B2B]))

#define INT_SYNC_CHANGE_TRIGGER_TYPE() (msWriteByteMask( SC0_CA, BIT1 | BIT0, BIT1 | BIT0)) // BIT0 for level trigger, BIT1 for active high
#define INT_SCALER_A(Enable) (msWriteByte(SC0_CE, Enable))
#define INT_SCALER_B(Enable) (msWriteByte(SC0_CF, Enable))
#define INT_STATUS_CLEAR() (msWriteByte(SC0_CC, 0), msWriteByte(SC0_CD, 0))
#define INT_SCALER_A_ISR(Enable) (MEM_SCWRITE_BYTE(SC0_CE, Enable))
#define INT_SCALER_B_ISR(Enable) (MEM_SCWRITE_BYTE(SC0_CF, Enable))
#define INT_STATUS_A_ISR(Status) (MEM_SCREAD_BYTE(SC0_CC) & (Status))
#define INT_STATUS_B_ISR(Status) (MEM_SCREAD_BYTE(SC0_CD) & (Status))
#define INT_STATUS_CLEAR_ISR() (MEM_SCWRITE_BYTE(SC0_CC, 0), MEM_SCWRITE_BYTE(SC0_CD, 0))

#if UART1
#define UART_READ_ES()  ((IEN2&ES1) == ES1)
#define UART_RI_FLAG()  ((S1CON & RI1) == RI1)
#define UART_TI_FLAG()  ((S1CON & TI1) == TI1)
#define UART_CLR_RI_FLAG()  (S1CON &= ~RI1)
#define UART_CLR_TI_FLAG()  (S1CON &= ~TI1)
#define UART_BUFFER()  (S1BUF)
#else
#define UART_READ_ES()  (ES)
#define UART_RI_FLAG()  (RI)
#define UART_TI_FLAG()  (TI)
#define UART_CLR_RI_FLAG()  (RI = 0)
#define UART_CLR_TI_FLAG()  (TI = 0)
#define UART_BUFFER()  (SBUF)
#endif

// cache
#define CACHE_STATUS()   (_bit3_(msReadByte(REG_1018)))
#define CACHE_ENABLE()  (msWriteBit( REG_1018, 1, _BIT3 ), msWriteBit( REG_2BA0, 0, _BIT0 ))
#define CACHE_DISABLE()  (msWriteBit( REG_2BA0, 1, _BIT0 ), msWriteBit( REG_1018, 0, _BIT3 ))

// watch dog
#define WDT_ENABLE()    (msWriteByte( REG_3C60, 0xAA ), msWriteByte( REG_3C61, 0x55 ))
#define WDT_DISABLE()    (msWriteByte( REG_3C60, 0x55 ), msWriteByte( REG_3C61, 0xAA ))
#define WDT_INTERVAL(Value) (msWriteByte( REG_3C62, LOBYTE( Value ) ), msWriteByte( REG_3C63, HIBYTE( Value ) ))
#define WDT_CLEAR()     (msWriteBit( REG_3C66, 1, _BIT1 ))

// hdcp
#define HDCP_PORT_ENABLE()  (msWriteByteMask(SC0_BD,0,BIT7))
#define HDCP_PORT_DISABLE()  (msWriteByteMask(SC0_BD,BIT7,BIT7))
#define HDCP_CLEAR_STATUS()     (msWriteByte( REG_28C3, 0xFF ), msWriteByte( REG_3BC3, 0xFF )) // HDCP0, 1
#define HDCP_CPU_WRITE_ENABLE(Value, Mask)    (msWriteByteMask(REG_28F3, Value, Mask))
#define HDCP_CPU_RW_ADDRESS(Value)  (msWrite2ByteMask(REG_28EE, Value, 0x03FF))
#define HDCP_LOAD_ADDR_GEN_PULSE()  (msWriteByte(REG_28F2, 0x20))
#define HDCP_CPU_WRITE_BKSV_START() // none

#define HDCP_CPU_WRITE_DATA(Value)   (msWriteByte( REG_28F0, Value ), msWriteByte(REG_28F2, 0x10))
#define HDCP_CPU_WRITE_BKSV(Address, Value)  (msWriteByte( REG_28FA+(Address), Value ))
#define HDCP_CPU_WR_BUSY()    (_bit7_(MEM_MSREAD_BYTE(REG_28F2)))
#define HDCP_CPU_WRITE_BCAPS(Value) (msWriteByte( REG_28FF, Value ))
#define HDCP_MISC_SETTING(Value) (msWrite2Byte( REG_28C0, Value ))
#define HDCP_ENABLE_DDC()   (msWriteByteMask( REG_28EF, 0x04, 0x04 ))
#define HDCP1_CPU_WRITE_BKSV(Address, Value)  (msWriteByte( REG_3BFA+(Address), Value ))
#define HDCP1_CPU_WR_BUSY()    (_bit7_(MEM_MSREAD_BYTE(REG_3BF2)))
#define HDCP1_CPU_WRITE_BCAPS(Value) (msWriteByte( REG_3BFF, Value ))
#define HDCP1_MISC_SETTING(Value) (msWrite2Byte( REG_3BC0, Value ))
#define HDCP1_ENABLE_DDC()   (msWriteByteMask( REG_3BEF, 0x04, 0x04 ))
#define HDCP_RESET(Enable)    (msWriteByteMask( REG_290E, (Enable)?(BIT5):(0),  BIT5))
#define HDCP_FORCE_ACK()    (msWriteByteMask( REG_28F2, 0x02, 0x02 ))

// dvi
#define DVI_A_RCK_CTRL(On)  (msWriteByteMask( REG_PM_B2, ((On) ? 0 : BIT0), BIT0 ))
#define DVI_B_RCK_CTRL(On)  (msWriteByteMask( REG_PM_B2, ((On) ? 0 : BIT1), BIT1 ))  
#define DVI_C_RCK_CTRL(On)  (msWriteByteMask( REG_PM_B3, ((On) ? 0 : BIT0), BIT0 ))
#define DVI_RELATED_CLOCK_CTRL(Value, Mask)    (msWrite2ByteMask( REG_290C, Value, Mask ))
#define DVI_DE_STABLE()  ( (msReadByte( REG_2862 ) & BIT6) == BIT6) // U,Y
#define DVI_CLOCK_IS_MISSING()  ((msReadByte(REG_2FCA)&BIT6) == BIT6)
#define DVI_RESET(Enable)    (msWriteByteMask( REG_290E, (Enable)?(BIT4):(0),  BIT4))

// dp
#define DP_INTERLACED_VIDEO_STREAM()    (msReadByte( REG_1FA5) & BIT2)

// scaler
#define SC0_SUB_BANK_SEL(SubBank)    (msWriteByte( SC0_00, SubBank ))
#define SC0_INPUT_SELECT(Value)  (msWriteByteMask(SC0_02, Value, BIT2|BIT1|BIT0))
#define SC0_READ_INPUT_SETTING()    (msReadByte(SC0_02))
#define SC0_RECOVER_INPUT_SETTING(Value)     (msWriteByte(SC0_02, Value))
#define SC0_OUTPUT_LOCK_MODE(LockInput)   (msWriteByteMask(SC0_02, (LockInput)?(0):(BIT7) , BIT7))    
#define SC0_SCALER_POWER_DOWN(Value, Mask) (msWriteByteMask( SC0_F0, Value, Mask ))
#define SC0_SCALER_RESET(Value)  (msWriteByte( SC0_F1, Value ))
#define SC0_NORMAL_MODE()   (msWriteByte( SC0_F8, 0x00 ))
#define SC0_HPEROID_DETECT_MODE(Enable16lines)  (msWriteByte(SC0_E5, (Enable16lines)?(BIT7):(0)))        
#define SC0_READ_HPEROID()  (msRead2Byte(SC0_E4) & MASK_13BIT)
#define SC0_READ_VTOTAL()   (msRead2Byte(SC0_E2) & MASK_12BIT)///(msRead2Byte(SC0_E2) & MASK_11BIT)
#define SC0_READ_SYNC_STATUS()  (msReadByte(SC0_E1))
#define SC0_READ_AUTO_START_H()  (msRead2Byte(SC0_80))
#define SC0_READ_AUTO_END_H()  (msRead2Byte(SC0_84))
#define SC0_READ_AUTO_START_V()  (msRead2Byte(SC0_7E))
#define SC0_READ_AUTO_END_V()  (msRead2Byte(SC0_82))
#define SC0_READ_AUTO_WIDTH()   (SC0_READ_AUTO_END_H()-SC0_READ_AUTO_START_H()+1)
#define SC0_READ_AUTO_HEIGHT()   (SC0_READ_AUTO_END_V()-SC0_READ_AUTO_START_V()+1)
#define SC0_READ_IMAGE_HEIGHT()  (msRead2Byte(SC0_09)&MASK_12BIT)    
#define SC0_READ_IMAGE_WIDTH()  (msRead2Byte(SC0_0B)&MASK_12BIT)   
#define SC0_SET_IMAGE_HEIGHT(Value)  (msWrite2Byte(SC0_09, (Value)&MASK_12BIT))    
#define SC0_SET_IMAGE_WIDTH(Value)  (msWrite2Byte(SC0_0B, (Value)&MASK_12BIT))    
#define SC0_SET_IMAGE_START_H(Value)    (msWrite2Byte( SC0_07, (Value)&MASK_12BIT ))
#define SC0_SET_IMAGE_START_V(Value)    (msWrite2Byte( SC0_05, (Value)&MASK_12BIT ))
#define SC0_WHITE_SCREEN_ENABLE()  (msWriteByteMask(SC0_43,BIT5,(BIT4|BIT5)))
#define SC0_BLACK_SCREEN_ENABLE()  (msWriteByteMask(SC0_43,BIT4,(BIT4|BIT5)))
#define SC0_BLACK_WHITE_SCREEN_DISABLE()  (msWriteByteMask(SC0_43,0,(BIT4|BIT5)))
#define SC0_SET_OUTPUT_VTOTAL(Value) (msWrite2ByteMask(SC0_1E, Value, MASK_12BIT))
#define SC0_DE_ONLY_MODE()  ((msReadByte(SC0_04)&BIT6) == BIT6)     
#define SC0_VIDEO_FIELD_INVERSION() ((msReadByte(SC0_E9)&BIT3) == BIT3)

#define SC0_READ_POWER_DOWN_STATUS()    (msReadByte(SC0_F0))        
#define SC0_ADC_COAST_ENABLE(Value)  (msWriteByte(SC0_ED, Value))
#define SC0_ADC_COAST_START(Value)  (msWriteByte(SC0_EE, Value))
#define SC0_ADC_COAST_END(Value)  (msWriteByte(SC0_EF, Value))
#define SC0_READ_ADC_COAST_ENABLE_VALUE()   (msReadByte(SC0_ED))
#define SC0_READ_ADC_COAST_START_VALUE()   (msReadByte(SC0_EE))
#define SC0_READ_ADC_COAST_END_VALUE()   (msReadByte(SC0_EF))
#define SC0_GLITCH_REMOVAL_ENABLE(Value)    (msWriteByte(SC0_F3, Value))
#define SC0_SAMPLE_CLOCK_INVERT(Value)   (msWriteByteMask(SC0_F3, ((Value)>165)?(BIT0):(0), BIT0))   
#define SC0_VSYNC_WIDTH_REPORT(Enable)  (msWriteByteMask(SC0_FA, (Enable)?(BIT0):(0),BIT0))  
#define SC0_READ_VSYNC_WIDTH()  (msReadByte(SC0_E2))

// auto
#define AUTO_POSITION_RESULT_READY()    (WaitAutoStatusReady(SC0_7B, BIT1))
#define AUTO_POSITION_SET_VALID_VALUE(Value)    (msWriteByteMask(SC0_7C, (Value) << 4, 0xF0))    
#define AUTO_POSITION_READ_VALID_VALUE()    (msReadByte(SC0_7C)>>4)    
#define AUTO_POSITION_READ_HSTART()     (GetAutoValue(SC0_80))    
#define AUTO_POSITION_READ_HEND()   (GetAutoValue(SC0_84))
#define AUTO_POSITION_READ_VSTART()     (GetAutoValue(SC0_7E))    
#define AUTO_POSITION_READ_VEND()     (GetAutoValue(SC0_82))    
#define AUTO_POSITION_READ_TRANSTION_POSITION(Delaytime) (GetTranstionPosition( Delaytime, SC0_80 ))    
#define AUTO_PHASE_RESULT_READY()   (drvADC_WaitAutoStatusReady(SC0_8B, BIT1))        
#define AUTO_PHASE_READ_VALUE() ((((DWORD)msRead2Byte(SC0_8E))<<16)|msRead2Byte(SC0_8C))   

// OSD
#define OSD_MENU_EXIST()    ( (msReadByte( IOSDC1 )&MWIN_B) == MWIN_B ) // U,Y

// misc
#define MPLL_POWER_UP(Enable) (msWriteByteMask(REG_1ED1, (Enable)?(0):(BIT0), BIT0))
#define LPLL_POWER_UP(Enable) (msWriteByteMask(REG_1ED1, (Enable)?(0):(BIT4), BIT4)) // V, Y
#define MPLL_CLOCK_ADC(Enable)  (msWriteByteMask(REG_1EDC, (Enable)?(0):(BIT4|BIT5), BIT4|BIT5))
#define ADC_PLL_LOCKING_EDGE(TrailingEdge)  (msWriteByteMask(REG_ADC_DTOP_07_L, (TrailingEdge)?(BIT5):(0), BIT5))
#define WRITE_POWER_ON_TABLE()
// DCR
#define HISTOGRAM_WHOLE_VERTICAL_RANGE_EN(Enable)   (msWriteByteMask(SC3_B5, (Enable)?(0):(BIT0), BIT0))
#define HISTOGRAM_RGB_TO_Y_EN(Enable)   (msWriteByteMask(SC7_3B, (Enable)?(BIT6):(0), BIT6)) // check    
#define MWE_FUNCTION_EN(Enable)   (msWriteByteMask(SC0_5C, (Enable)?(BIT3):(0), BIT3))
#define STATISTIC_REQUEST_MAIN_ENABLE()  (msWriteByte(SC7_B8, (BIT2|BIT1)))
#define STATISTIC_REQUEST_SUB_ENABLE()  (msWriteByte(SC7_B8, (BIT2|BIT0)))
#define STATISTIC_REQUEST_EN(Enable) (msWriteByteMask(SC7_B8, (Enable)?(BIT2):(0), BIT2))
#define STATISTIC_ACKNOWLEDGE() ((msReadByte(SC7_B8)&BIT3) == BIT3)
#define Y_CHANNEL_MIN_PIXEL()   (msReadByte( SC7_39 ))
#define Y_CHANNEL_MAX_PIXEL()   (msReadByte( SC7_3A ))
#define TOTAL_PIXEL_COUNT()     (msRead2Byte(SC7_34))
#define TOTAL_PIXEL_WEIGHT()    (msRead2Byte(SC7_36))
#define AVERAGE_LUMINANCE(TotalWeight, TotalCount)  ((((DWORD)(TotalWeight))*256+((TotalCount)>>1))/(TotalCount)) // check

// ICE
#define ICE_COLOR_R     0
#define ICE_COLOR_G     1
#define ICE_COLOR_B     2
#define ICE_COLOR_C     3
#define ICE_COLOR_M     4
#define ICE_COLOR_Y     5
#define ICE_MAIN_BRI_CTRL(Enable)   (msWriteByteMask(SC9_02, (Enable)?(BIT5):(0), BIT5))
#define ICE_MAIN_SAT_CTRL(Enable)   (msWriteByteMask(SC9_02, (Enable)?(BIT4):(0), BIT4))
#define ICE_MAIN_HUE_CTRL(Enable)   (msWriteByteMask(SC9_02, (Enable)?(BIT0):(0), BIT0))
#define ICE_SUB_BRI_CTRL(Enable)   (msWriteByteMask(SC9_03, (Enable)?(BIT5):(0), BIT5))
#define ICE_SUB_SAT_CTRL(Enable)   (msWriteByteMask(SC9_03, (Enable)?(BIT4):(0), BIT4))
#define ICE_SUB_HUE_CTRL(Enable)   (msWriteByteMask(SC9_03, (Enable)?(BIT0):(0), BIT0))
#define ICE_MAIN_CTRL(Enable)   (msWriteByte(SC9_02, (Enable)?(BIT5|BIT4|BIT0):(0)))     
#define ICE_SUB_CTRL(Enable)   (msWriteByte(SC9_03, (Enable)?(BIT5|BIT4|BIT0):(0)))    
#define ICE_DEFINE_RANGE_RGB(Color, Value)  //(msWriteByte(SC9_04+Color, Value)) // check   
#define ICE_ACTIVE_RANGE_RGBCMY(Color, Value)   (msWriteByte(SC9_08+(Color), Value))
#define ICE_SATURATION_RGBCMY(Color, Value) (msWriteByte(SC9_14+(Color), Value))
#define ICE_HUE_RGBCMY(Color, Value)    (msWriteByte(SC9_0E+(Color), Value))
#define ICE_BRIGHTNESS_RGBCMY(Color, Value)    (msWriteByte(SC9_1A+(Color), Value))

#define CSC_MAIN_ENABLE(Enable) (msWriteByteMask(SC7_40, (Enable)?(BIT0):(0), BIT0))   
#define CSC_SUB_ENABLE(Enable) (msWriteByteMask(SC7_40, (Enable)?(BIT4):(0), BIT4))   


//OSD 
//Code/Attr RAM bit8/bit9
#define OSD_TEXT_HI_ADDR_SET_BIT8()     msWriteByteMask(OSD2_AE, BIT3, BIT3|BIT2); //enable bit 8
#define OSD_TEXT_HI_ADDR_SET_BIT9()     msWriteByteMask(OSD2_AE, BIT2, BIT3|BIT2); //enable bit 9
#define OSD_TEXT_HI_ADDR_CLR_TO_0()     msWriteByteMask(OSD2_AE,    0, BIT3|BIT2);

#define OSD_WRITE_FONT_ADDRESS(u8Addr)  msWriteByte(OSD2_A3, u8Addr);
#define PORT_CODE_ADDR  OSD1_66
#define PORT_CODE_DATA  OSD1_68
#define PORT_ATTR_ADDR  OSD1_66
#define PORT_ATTR_DATA  OSD1_6A
#define PORT_FONT_ADDR  OSD1_66
#define PORT_FONT_DATA  OSD1_6C

#define WRITE_CAFSRAM_ADDR()  
#define WRITE_CODE()  
#define WRITE_ATTRIBUTE()  
#define WRITE_FONT()  
#define CAFSRAM_ERASE_TRIG()  
#define CAFSRAM_READ_TRIG()  
#define WRITE_PSRAM0_ADDR()  
#define WRITE_PSRAM0_MASK()  
#define WRITE_PSRAM0_COLOR_KEY()  
#define WRITE_PSRAM0_COLOR()  
#define PSRAM0_ERASE_TRIG()  
#define PSRAM0_READ_TRIG()  
#define WRITE_PSRAM1_ADDR()  
#define WRITE_PSRAM1_COLOR_KEY()  
#define WRITE_PSRAM1_COLOR()  
#define PSRAM1_ERASE_TRIG()  
#define PSRAM1_READ_TRIG()  



