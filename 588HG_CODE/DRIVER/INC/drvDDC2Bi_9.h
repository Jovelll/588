///////////////////////////////////////
// DRVDDC2BI_9 VERSION: V01
////////////////////////////////////////

#include "global.h"

// Enum of DDC mode provided for selection
#define D2BMode_SingleBuffer                    0 // Old DDC mode with tx/rx each with one byte buffer
#define D2BMode_FIFO_Nornal                     1 // New DDC normal mode using xdata as tx/rx buffer
#define D2BMode_FIFO_Normal_XSharedDDCBuffer    2 // New DDC normal mode using xdata as tx/rx buffer, and xdata shared with DDCBuffer
#define D2BMode_FIFO_Normal_XSharedAll          3 // New DDC normal mode using xdata as tx/rx buffer, and xdata shared with DDCBuffer & All Ports

// DDC mode selection
#define D2BModeSel  D2BMode_FIFO_Nornal//D2BMode_FIFO_Normal_XSharedAll // Be aware of buffer overflow problem, MUST reserved enough buffer size!!!!!

// Enum of DDC FIFO length 
#define D2B_FIFO_8Level     0
#define D2B_FIFO_16Level    1
#define D2B_FIFO_32Level    2
#define D2B_FIFO_64Level    3
#define D2B_FIFO_128Level   4
#define D2B_FIFO_256Level   5

// DDC mode configuration
#define D2B_DOUBLE_FIFO_Mode   1
#if D2BModeSel==D2BMode_SingleBuffer
#define D2B_FIFO_Mode           0
#define D2B_FIFO_Enhance        (0&D2B_FIFO_Mode&(!D2B_DOUBLE_FIFO_Mode))
#define D2B_XShared_DDCBuffer   (0&(!D2B_FIFO_Enhance)&(D2B_FIFO_Mode)) // DDCBuffer need change to circular case in enhancemode(loop)
#define D2B_XShared_InputPors   (0&D2B_FIFO_Mode)
#define D2B_HW_CheckSum         0
#define D2B_FIFOSize_INDEX      D2B_FIFO_64Level
#define DDC_BUFFER_LENGTH       64//MaxBufLen 50 // ddc buffer length
#elif D2BModeSel==D2BMode_FIFO_Nornal
#define D2B_FIFO_Mode           1
#define D2B_FIFO_Enhance        (0&D2B_FIFO_Mode&(!D2B_DOUBLE_FIFO_Mode))
#define D2B_XShared_DDCBuffer   (0&(!D2B_FIFO_Enhance)&(D2B_FIFO_Mode)) // DDCBuffer need change to circular case in enhancemode(loop)
#define D2B_XShared_InputPors   (0&D2B_FIFO_Mode)
#define D2B_HW_CheckSum         1
#define D2B_FIFOSize_INDEX      D2B_FIFO_32Level
#define DDC_BUFFER_LENGTH       64// ddc buffer length
#elif D2BModeSel==D2BMode_FIFO_Normal_XSharedDDCBuffer
#define D2B_FIFO_Mode           1
#define D2B_FIFO_Enhance        (0&D2B_FIFO_Mode&(!D2B_DOUBLE_FIFO_Mode))
#define D2B_XShared_DDCBuffer   (1&(!D2B_FIFO_Enhance)&(D2B_FIFO_Mode)) // DDCBuffer need change to circular case in enhancemode(loop)
#define D2B_XShared_InputPors   (0&D2B_FIFO_Mode)
#define D2B_HW_CheckSum         1
#define D2B_FIFOSize_INDEX      D2B_FIFO_64Level
#define DDC_BUFFER_LENGTH       64 // Same as D2B_FIFO_Size
#elif D2BModeSel==D2BMode_FIFO_Normal_XSharedAll
#define D2B_FIFO_Mode           1
#define D2B_FIFO_Enhance        (0&D2B_FIFO_Mode&(!D2B_DOUBLE_FIFO_Mode))
#define D2B_XShared_DDCBuffer   (1&(!D2B_FIFO_Enhance)&(D2B_FIFO_Mode)) // DDCBuffer need change to circular case in enhancemode(loop)
#define D2B_XShared_InputPors   (1&D2B_FIFO_Mode)
#define D2B_HW_CheckSum         1
#define D2B_FIFOSize_INDEX      D2B_FIFO_128Level
#define DDC_BUFFER_LENGTH       128 // Same as D2B_FIFO_Size
#else
#message "please implement drvDDC2Bi_7 for new applications"
#endif

// DDC debug using xdata to store info, temply keeped for fpga/asic verify
#define D2B_XDATA_DEBUG         0&D2B_FIFO_Mode
// align dbg msg in xdata
#if D2B_XDATA_DEBUG
#if D2B_FIFOSize_INDEX==D2B_FIFO_8Level
#define FIFODBGOFFSET 8
#else
#define FIFODBGOFFSET 0
#endif 
#endif

// DDC FIFO Index mapping to Size
#if D2B_FIFOSize_INDEX==D2B_FIFO_8Level
#define D2B_FIFO_Size   8
#elif D2B_FIFOSize_INDEX==D2B_FIFO_16Level
#define D2B_FIFO_Size   16
#elif D2B_FIFOSize_INDEX==D2B_FIFO_32Level
#define D2B_FIFO_Size   32
#elif D2B_FIFOSize_INDEX==D2B_FIFO_64Level
#define D2B_FIFO_Size   64
#elif D2B_FIFOSize_INDEX==D2B_FIFO_128Level
#define D2B_FIFO_Size   128
#elif D2B_FIFOSize_INDEX==D2B_FIFO_256Level
#define D2B_FIFO_Size   256
#endif

// DDC FIFO mode xdata mapping address
#if D2B_XShared_InputPors
#define D2B_FIFO_XdataAddressStart      (XDATA_DDC_ADDR_START)
#define D2B_FIFO_XdataAddressStart_A0   (D2B_FIFO_XdataAddressStart)
#define D2B_FIFO_XdataAddressStart_D0   (D2B_FIFO_XdataAddressStart)
#define D2B_FIFO_XdataAddressStart_D1   (D2B_FIFO_XdataAddressStart)
#define D2B_FIFO_XdataAddressStart_D2   (D2B_FIFO_XdataAddressStart)
#define D2B_FIFO_XdataWriteAddressStart      (XDATA_DDC_TX_ADDR_START)
#define D2B_FIFO_XdataWriteAddressStart_A0   (D2B_FIFO_XdataWriteAddressStart)
#define D2B_FIFO_XdataWriteAddressStart_D0   (D2B_FIFO_XdataWriteAddressStart)
#define D2B_FIFO_XdataWriteAddressStart_D1   (D2B_FIFO_XdataWriteAddressStart)
#define D2B_FIFO_XdataWriteAddressStart_D2   (D2B_FIFO_XdataWriteAddressStart)
#define D2B_FIFO_XdataSize              (D2B_FIFO_Size*1)
#else
#define D2B_FIFO_XdataAddressStart      (XDATA_DDC_ADDR_START)
#define D2B_FIFO_XdataAddressStart_A0   (D2B_FIFO_XdataAddressStart)
#define D2B_FIFO_XdataAddressStart_D0   (D2B_FIFO_XdataAddressStart_A0+D2B_FIFO_Size)
#define D2B_FIFO_XdataAddressStart_D1   (DDC_Port_D0?(D2B_FIFO_XdataAddressStart_D0+D2B_FIFO_Size):D2B_FIFO_XdataAddressStart_D0)
#define D2B_FIFO_XdataAddressStart_D2   (DDC_Port_D1?(D2B_FIFO_XdataAddressStart_D1+D2B_FIFO_Size):D2B_FIFO_XdataAddressStart_D1)
#define D2B_FIFO_XdataSize              (D2B_FIFO_Size*(1+DDC_Port_D0+DDC_Port_D1+DDC_Port_D2))
#define D2B_FIFO_XdataWriteAddressStart      (XDATA_DDC_TX_ADDR_START)
#define D2B_FIFO_XdataWriteAddressStart_A0   (D2B_FIFO_XdataWriteAddressStart)
#define D2B_FIFO_XdataWriteAddressStart_D0   (D2B_FIFO_XdataWriteAddressStart_A0+D2B_FIFO_Size)
#define D2B_FIFO_XdataWriteAddressStart_D1   (DDC_Port_D0?(D2B_FIFO_XdataWriteAddressStart_D0+D2B_FIFO_Size):D2B_FIFO_XdataWriteAddressStart_D0)
#define D2B_FIFO_XdataWriteAddressStart_D2   (DDC_Port_D1?(D2B_FIFO_XdataWriteAddressStart_D1+D2B_FIFO_Size):D2B_FIFO_XdataWriteAddressStart_D1)
#endif

#if !D2B_DOUBLE_FIFO_Mode
#define D2B_FIFO_XdataSize_Total        D2B_FIFO_XdataSize
#else
#define D2B_FIFO_XdataSize_Total        (2*D2B_FIFO_XdataSize) //allocate another xdata block for TX write
#endif

enum
{
    InputCombo_D0,
    InputCombo_D1,
    InputCombo_D2,
    InputCombo_A0,
};

extern void drvDDC2Bi_Init( void );
extern void drvDDC2Bi_MessageReady( void );
extern void drvDDC2Bi_ParseCommand( void );
extern void drvDoDDCCI( BYTE Source );
	
extern BYTE idata rxInputPort; // Annie 2006.7.5
extern BYTE idata rxComboPort;
extern BYTE rxIndex;
extern BYTE rxStatus;
extern BYTE rxLength;
#if !D2B_HW_CheckSum
extern BYTE rxChkSum;
#endif
extern BYTE txStatus;
extern BYTE *txBufferPtr;
extern BYTE txLength;
//extern BYTE sendData;
extern code BYTE nullMsg1[3];
extern code BYTE nullMsg[3];

#if D2B_XShared_DDCBuffer
extern BYTE *DDCBuffer;
#else
extern BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
#endif

#if D2B_FIFO_Mode
extern WORD idata txIndex;
extern WORD idata Previous_DDC2MCU_CUR_ADR;
extern WORD idata Present_DDC2MCU_CUR_ADR;
extern WORD idata Present_DDC2MCU_START_ADR;
extern BYTE idata D2BErrorFlag;
#if D2B_XDATA_DEBUG
extern BYTE xdata XSRAMBuffer[D2B_FIFO_XdataSize_Total+256];
extern BYTE xdata IntCount;
extern BYTE xdata IntCountTmp;
#else
extern BYTE xdata XSRAMBuffer[D2B_FIFO_XdataSize_Total];
#endif
#endif

//==========Register=======================================
#define INTMASK_EN_ADC      msRegs[REG_3E14]
#define ADC_INT_FLAG        msRegs[REG_3E10]
#define ADC_WBUF_RDP        msRegs[REG_3E00]
#define ADC_RBUF_WDP        msRegs[REG_3E01]
#define ADC_DDCCI_ID        msRegs[REG_3E0A]
#define ADC_INT_CLR         msRegs[REG_3E1C]

#define INTMASK_EN_DVI0     msRegs[REG_3E16]
#define DVI0_INT_FLAG       msRegs[REG_3E12]
#define DVI0_WBUF_RDP       msRegs[REG_3E04]
#define DVI0_RBUF_WDP       msRegs[REG_3E05]
#define DVI0_DDCCI_ID       msRegs[REG_3E0C]
#define DVI0_INT_CLR        msRegs[REG_3E1E]

#define INTMASK_EN_DVI1     msRegs[REG_3E17]
#define DVI1_INT_FLAG       msRegs[REG_3E13]
#define DVI1_WBUF_RDP       msRegs[REG_3E06]
#define DVI1_RBUF_WDP       msRegs[REG_3E07]
#define DVI1_DDCCI_ID       msRegs[REG_3E0D]
#define DVI1_INT_CLR        msRegs[REG_3E1F]

#define INTMASK_EN_DVI2     msRegs[REG_3E55]
#define DVI2_INT_FLAG       msRegs[REG_3E54]
#define DVI2_WBUF_RDP       msRegs[REG_3E50]
#define DVI2_RBUF_WDP       msRegs[REG_3E51]
#define DVI2_DDCCI_ID       msRegs[REG_3E52]
#define DVI2_INT_CLR        msRegs[REG_3E57]

#define DP_DDCCI_ID		    msRegs[REG_1FF7] // Not Confirmed yet in CHIP_TSUMC!!!!
#define DDC2BI_CTRL         msRegs[REG_3E09]

#define DDC_RW_SRAM_FLAG   (msRegs[REG_3EEA]&(BIT3|BIT2))
#define DDC_RW_SRAM_CLR    (msRegs[REG_3EEA] |= (BIT1|BIT0))

#define DDCADC_INTMASK_EN           INTMASK_EN_ADC
#define DDCADC_INT_FLAG             ADC_INT_FLAG
#define DDCADC_WBUF_RDP             ADC_WBUF_RDP
#define DDCADC_RBUF_WDP             ADC_RBUF_WDP
#define DDCADC_DDCCI_ID             ADC_DDCCI_ID
#define DDCADC_INT_CLR              ADC_INT_CLR
#define CLR_ADC_INT_FLAG()          ADC_INT_CLR = 0xFF

#define DDCDVI0_INTMASK_EN           INTMASK_EN_DVI0
#define DDCDVI0_INT_FLAG             DVI0_INT_FLAG
#define DDCDVI0_WBUF_RDP             DVI0_WBUF_RDP
#define DDCDVI0_RBUF_WDP             DVI0_RBUF_WDP
#define DDCDVI0_DDCCI_ID             DVI0_DDCCI_ID
#define DDCDVI0_INT_CLR              DVI0_INT_CLR
#define CLR_DVI0_INT_FLAG()          DVI0_INT_CLR = 0xFF

#define DDCDVI1_INTMASK_EN           INTMASK_EN_DVI1
#define DDCDVI1_INT_FLAG             DVI1_INT_FLAG
#define DDCDVI1_WBUF_RDP             DVI1_WBUF_RDP
#define DDCDVI1_RBUF_WDP             DVI1_RBUF_WDP
#define DDCDVI1_DDCCI_ID             DVI1_DDCCI_ID
#define DDCDVI1_INT_CLR              DVI1_INT_CLR
#define CLR_DVI1_INT_FLAG()          DVI1_INT_CLR = 0xFF

#define DDCDVI2_INTMASK_EN           INTMASK_EN_DVI2
#define DDCDVI2_INT_FLAG             DVI2_INT_FLAG
#define DDCDVI2_WBUF_RDP             DVI2_WBUF_RDP
#define DDCDVI2_RBUF_WDP             DVI2_RBUF_WDP
#define DDCDVI2_DDCCI_ID             DVI2_DDCCI_ID
#define DDCDVI2_INT_CLR              DVI2_INT_CLR
#define CLR_DVI2_INT_FLAG()          DVI2_INT_CLR = 0xFF

#if D2B_FIFO_Mode
#define DDC_ADC_WP_READY()          (msRegs[REG_3EE6] |= BIT2)
#define DDC_DVI0_WP_READY()         (msRegs[REG_3EC8] |= BIT2)
#define DDC_DVI1_WP_READY()         (msRegs[REG_3ECE] |= BIT2)
#define DDC_DVI2_WP_READY()         (msRegs[REG_3EE0] |= BIT2) 
#else
#define DDC_ADC_WP_READY()          //(msRegs[REG_3E08] |= BIT7)
#define DDC_DVI0_WP_READY()          //(msRegs[REG_3E08] |= BIT5)
#define DDC_DVI1_WP_READY()          //(msRegs[REG_3E08] |= BIT4)
#define DDC_DVI2_WP_READY()          //(msRegs[REG_3E08] |= BIT3)
#endif

#if ENABLE_DP_INPUT
#define DDCDP_DDCCI_ID    DP_DDCCI_ID
#endif

// DDC2Bi
#define D2B_SET_ID_VGA(Enable)      (DDCADC_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_VGA()          (DDCADC_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define D2B_SET_ID_DVI0(Enable)     (DDCDVI0_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_DVI0()         (DDCDVI0_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define D2B_SET_ID_DVI1(Enable)     (DDCDVI1_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_DVI1()         (DDCDVI1_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define D2B_SET_ID_DVI2(Enable)     (DDCDVI2_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_DVI2()         (DDCDVI2_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define D2B_INT_MASK_ALL()          (INTMASK_EN_ADC  = 0xFF, INTMASK_EN_DVI0 = 0xFF, INTMASK_EN_DVI1 = 0xFF, INTMASK_EN_DVI2 = 0xFF)
#define D2B_SET_ID_DP(Enable)   (DDCDP_DDCCI_ID = (Enable)?(0xB7):(0))
//A0
#define D2B_SET_FIFOMODE_VGA(Enable)                        (msWriteByteMask( REG_3EE6, Enable, BIT0 )) //Enable XDATA FIFO to replace D2B Write/Read Buffer
#define D2B_SET_FIFOMODE_ENHANCE_VGA(EnhanceMode)           (msWriteByteMask( REG_3EE6, (EnhanceMode)<<1, BIT1 )) //0:Normal mode (default mode) 1:Enhance mode  
#define D2B_SET_FIFOMODE_SIZE_VGA(FIFOSize_INDEX)           (msWriteByteMask( REG_3EE7, FIFOSize_INDEX, BIT2 | BIT1 | BIT0 )) //FiFo Size selection   
#define D2B_SET_FIFOMODE_XDATAMAP_VGA(XdataAddressStart)    (msWrite2ByteMask(REG_3EE8, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#if D2B_DOUBLE_FIFO_Mode
#define D2B_SET_DOUBLE_FIFOMODE_VGA(Enable)                 (msWriteByteMask( REG_3E93, Enable<<7, BIT7 ))
#define D2B_SET_FIFOMODE_TX_SIZE_VGA(FIFOSize_INDEX)        (msWriteByteMask( REG_3E93, FIFOSize_INDEX<<4, BIT6 | BIT5 | BIT4 )) //FiFo Size selection
#define D2B_SET_FIFOMODE_TX_XDATAMAP_VGA(XdataAddressStart) (msWrite2ByteMask(REG_3E96, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#endif
//D0
#define D2B_SET_FIFOMODE_D0(Enable)                         (msWriteByteMask( REG_3EC8, Enable, BIT0 )) //Enable XDATA FIFO to replace D2B Write/Read Buffer
#define D2B_SET_FIFOMODE_ENHANCE_D0(EnhanceMode)            (msWriteByteMask( REG_3EC8, (EnhanceMode)<<1, BIT1 )) //0:Normal mode (default mode) 1:Enhance mode  
#define D2B_SET_FIFOMODE_SIZE_D0(FIFOSize_INDEX)            (msWriteByteMask( REG_3EC9, FIFOSize_INDEX, BIT2 | BIT1 | BIT0 )) //FiFo Size selection   
#define D2B_SET_FIFOMODE_XDATAMAP_D0(XdataAddressStart)     (msWrite2ByteMask(REG_3ECA, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#if D2B_DOUBLE_FIFO_Mode
#define D2B_SET_DOUBLE_FIFOMODE_D0(Enable)                  (msWriteByteMask( REG_3E94, Enable<<3, BIT3 ))
#define D2B_SET_FIFOMODE_TX_SIZE_D0(FIFOSize_INDEX)         (msWriteByteMask( REG_3E94, FIFOSize_INDEX, BIT2 | BIT1 | BIT0 )) //FiFo Size selection
#define D2B_SET_FIFOMODE_TX_XDATAMAP_D0(XdataAddressStart)  (msWrite2ByteMask(REG_3E98, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#endif
//D1
#define D2B_SET_FIFOMODE_D1(Enable)                         (msWriteByteMask( REG_3ECE, Enable, BIT0 )) //Enable XDATA FIFO to replace D2B Write/Read Buffer
#define D2B_SET_FIFOMODE_ENHANCE_D1(EnhanceMode)            (msWriteByteMask( REG_3ECE, (EnhanceMode)<<1, BIT1 )) //0:Normal mode (default mode) 1:Enhance mode  
#define D2B_SET_FIFOMODE_SIZE_D1(FIFOSize_INDEX)            (msWriteByteMask( REG_3ECF, FIFOSize_INDEX, BIT2 | BIT1 | BIT0 )) //FiFo Size selection   
#define D2B_SET_FIFOMODE_XDATAMAP_D1(XdataAddressStart)     (msWrite2ByteMask(REG_3ED0, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#if D2B_DOUBLE_FIFO_Mode
#define D2B_SET_DOUBLE_FIFOMODE_D1(Enable)                  (msWriteByteMask( REG_3E94, Enable<<7, BIT7 ))
#define D2B_SET_FIFOMODE_TX_SIZE_D1(FIFOSize_INDEX)         (msWriteByteMask( REG_3E94, FIFOSize_INDEX<<4, BIT6 | BIT5 | BIT4 )) //FiFo Size selection
#define D2B_SET_FIFOMODE_TX_XDATAMAP_D1(XdataAddressStart)  (msWrite2ByteMask(REG_3E9A, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#endif
//D2
#define D2B_SET_FIFOMODE_D2(Enable)                         (msWriteByteMask( REG_3EE0, Enable, BIT0 )) //Enable XDATA FIFO to replace D2B Write/Read Buffer
#define D2B_SET_FIFOMODE_ENHANCE_D2(EnhanceMode)            (msWriteByteMask( REG_3EE0, (EnhanceMode)<<1, BIT1 )) //0:Normal mode (default mode) 1:Enhance mode  
#define D2B_SET_FIFOMODE_SIZE_D2(FIFOSize_INDEX)            (msWriteByteMask( REG_3EE1, FIFOSize_INDEX, BIT2 | BIT1 | BIT0 )) //FiFo Size selection   
#define D2B_SET_FIFOMODE_XDATAMAP_D2(XdataAddressStart)     (msWrite2ByteMask(REG_3EE2, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#if D2B_DOUBLE_FIFO_Mode
#define D2B_SET_DOUBLE_FIFOMODE_D2(Enable)                  (msWriteByteMask( REG_3E95, Enable<<3, BIT3 ))
#define D2B_SET_FIFOMODE_TX_SIZE_D2(FIFOSize_INDEX)         (msWriteByteMask( REG_3E95, FIFOSize_INDEX, BIT2 | BIT1 | BIT0 )) //FiFo Size selection
#define D2B_SET_FIFOMODE_TX_XDATAMAP_D2(XdataAddressStart)  (msWrite2ByteMask(REG_3E9C, XdataAddressStart-XDATA_ADDR_START, 0x1FFF)) // DDC_xdata mapping address
#endif    
#define D2B_SET_D2B_HW_CheckSum(EnVGA, EnD2, EnD1, EnD0)        (msWriteByteMask( REG_3EF4, (EnVGA<<3)|(EnD2<<2)|(EnD1<<1)|(EnD0), BIT3|BIT2|BIT1|BIT0 ))   

#define D2B_FIFOMODE_INPUT2DDCPORT(InputPort)               ((InputPort==Input_DVI_C1 || InputPort==Input_HDMI_C1)?(0):((InputPort==Input_DVI_C2 || InputPort==Input_HDMI_C2)?(1):((InputPort==Input_DVI_C3 || InputPort==Input_HDMI_C3)?(2):(3))))   
#define D2B_FIFOMODE_DDCPORT2INPUT(ComboPort)               (ComboPort==InputCombo_A0?(Input_VGA):(ComboPort==InputCombo_D0?(Input_DVI^Input_HDMI^Input_Nothing):(ComboPort==InputCombo_D1?(Input_DVI2^Input_HDMI2^Input_Nothing):(Input_DVI3^Input_HDMI3^Input_Nothing))))

extern void msDrvWriteInternalEDID(BYTE u8Block,BYTE u8Addr,BYTE u8Data);
extern BYTE msDrvReadInternalEDID(BYTE u8Block,BYTE u8Addr);

