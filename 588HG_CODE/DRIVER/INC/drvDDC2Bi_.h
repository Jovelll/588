
///////////////////////////////////////
// DRVDDC2BI_2BUVY VERSION: V01
////////////////////////////////////////
#define DDC_BUFFER_LENGTH   50//MaxBufLen 50 // ddc buffer length

extern void drvDDC2Bi_Init( void );
extern void drvDDC2Bi_MessageReady( void );
extern void drvDDC2Bi_ParseCommand( void );
extern void drvDoDDCCI( BYTE Source );


extern BYTE idata rxInputPort; // Annie 2006.7.5
//extern BYTE xdata CP=0;
extern BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
extern BYTE rxIndex;
extern BYTE rxStatus;
extern BYTE rxLength;
extern BYTE rxChkSum;
extern BYTE txStatus;
extern BYTE *txBufferPtr;
extern BYTE txLength;
extern BYTE sendData;
extern code BYTE nullMsg1[3];
extern code BYTE nullMsg[3];

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

#define DP_DDCCI_ID     msRegs[REG_1FF7]
#define DDC2BI_CTRL         msRegs[REG_3E09]

#define DDCADC_INTMASK_EN           INTMASK_EN_ADC
#define DDCADC_INT_FLAG             ADC_INT_FLAG
#define DDCADC_WBUF_RDP             ADC_WBUF_RDP
#define DDCADC_RBUF_WDP             ADC_RBUF_WDP
#define DDCADC_DDCCI_ID             ADC_DDCCI_ID
#define DDCADC_INT_CLR              ADC_INT_CLR
#define DDC_ADC_WP_READY()      //    msRegs[REG_3E08] |= BIT7;
#define CLR_ADC_INT_FLAG()          ADC_INT_CLR = 0x7F

#if 1//ENABLE_DVI_INPUT
#if DVI_DDC_PORT==TMDS_PORT_A
#define DDCDVI_INTMASK_EN           INTMASK_EN_DVI0
#define DDCDVI_INT_FLAG             DVI0_INT_FLAG
#define DDCDVI_WBUF_RDP             DVI0_WBUF_RDP
#define DDCDVI_RBUF_WDP             DVI0_RBUF_WDP
#define DDCDVI_DDCCI_ID             DVI0_DDCCI_ID
#define DDCDVI_INT_CLR              DVI0_INT_CLR
#define DDC_DVI_WP_READY()        //  msRegs[REG_3E08] |= BIT5;
#define CLR_DVI_INT_FLAG()          DVI0_INT_CLR = 0x7F
#elif DVI_DDC_PORT==TMDS_PORT_B
#define DDCDVI_INTMASK_EN           INTMASK_EN_DVI1
#define DDCDVI_INT_FLAG             DVI1_INT_FLAG
#define DDCDVI_WBUF_RDP             DVI1_WBUF_RDP
#define DDCDVI_RBUF_WDP             DVI1_RBUF_WDP
#define DDCDVI_DDCCI_ID             DVI1_DDCCI_ID
#define DDCDVI_INT_CLR              DVI1_INT_CLR
#define DDC_DVI_WP_READY()     //     msRegs[REG_3E08] |= BIT4;
#define CLR_DVI_INT_FLAG()          DVI1_INT_CLR = 0x7F
#else //TMDS_PORT_C
#define DDCDVI_INTMASK_EN           INTMASK_EN_DVI2
#define DDCDVI_INT_FLAG             DVI2_INT_FLAG
#define DDCDVI_WBUF_RDP             DVI2_WBUF_RDP
#define DDCDVI_RBUF_WDP             DVI2_RBUF_WDP
#define DDCDVI_DDCCI_ID             DVI2_DDCCI_ID
#define DDCDVI_INT_CLR              DVI2_INT_CLR
#define DDC_DVI_WP_READY()          //msRegs[REG_3E08] |= BIT3;
#define CLR_DVI_INT_FLAG()          DVI2_INT_CLR = 0x7F
#endif
#endif

#if DVI_SLAVE_PORT==TMDS_PORT_A//GLASSES_TYPE==GLASSES_NVIDIA
#define DDCDVI_NV3D_INTMASK_EN      INTMASK_EN_DVI0//msRegs[REG_3E16]
#define DDCDVI_NV3D_INT_FLAG        DVI0_INT_FLAG//msRegs[REG_3E12]
#define DDCDVI_NV3D_WBUF_RDP        DVI0_WBUF_RDP//msRegs[REG_3E04]
#define DDCDVI_NV3D_RBUF_WDP        DVI0_RBUF_WDP//msRegs[REG_3E05]
#define DDCDVI_NV3D_DDCCI_ID        DVI0_DDCCI_ID//msRegs[REG_3E0C]
#define DDC_DVI_NV3D_WP_READY()     //msRegs[REG_3E08] |= BIT5;
#define CLR_DDC_DVI_NV3D_INT_FLAG() DVI0_INT_CLR = 0x7F
#else
#endif

#if ENABLE_HDMI
#if HDMI_DDC_PORT==TMDS_PORT_A
#define DDCHDMI_INTMASK_EN  INTMASK_EN_DVI0
#define DDCHDMI_INT_FLAG    DVI0_INT_FLAG
#define DDCHDMI_WBUF_RDP    DVI0_WBUF_RDP
#define DDCHDMI_RBUF_WDP    DVI0_RBUF_WDP
#define DDCHDMI_DDCCI_ID    DVI0_DDCCI_ID
#define DDCHDMI_INT_CLR     DVI0_INT_CLR
#define DDC_HDMI_WP_READY() //msRegs[REG_3E08] |= BIT5;
#define CLR_HDMI_INT_FLAG() DVI0_INT_CLR = 0x7F
#elif HDMI_DDC_PORT==TMDS_PORT_B
#define DDCHDMI_INTMASK_EN  INTMASK_EN_DVI1
#define DDCHDMI_INT_FLAG    DVI1_INT_FLAG
#define DDCHDMI_WBUF_RDP    DVI1_WBUF_RDP
#define DDCHDMI_RBUF_WDP    DVI1_RBUF_WDP
#define DDCHDMI_DDCCI_ID    DVI1_DDCCI_ID
#define DDCHDMI_INT_CLR     DVI1_INT_CLR
#define DDC_HDMI_WP_READY() //msRegs[REG_3E08] |= BIT4;
#define CLR_HDMI_INT_FLAG() DVI1_INT_CLR = 0x7F
#else
#define DDCHDMI_INTMASK_EN  INTMASK_EN_DVI2
#define DDCHDMI_INT_FLAG    DVI2_INT_FLAG
#define DDCHDMI_WBUF_RDP    DVI2_WBUF_RDP
#define DDCHDMI_RBUF_WDP    DVI2_RBUF_WDP
#define DDCHDMI_DDCCI_ID    DVI2_DDCCI_ID
#define DDCHDMI_INT_CLR     DVI2_INT_CLR
#define DDC_HDMI_WP_READY() //msRegs[REG_3E08] |= BIT3;
#define CLR_HDMI_INT_FLAG() DVI2_INT_CLR = 0x7F
#endif
#endif
#if ENABLE_DP_INPUT
#define DDCDP_DDCCI_ID    DP_DDCCI_ID
#endif


// DDC2Bi
#define D2B_SET_ID_VGA(Enable)   (DDCADC_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_VGA()   (DDCADC_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define D2B_SET_ID_DVI(Enable)   (DDCDVI_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_DVI()   (DDCDVI_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)

#if CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMB || CHIP_ID == CHIP_TSUMD
#define D2B_SET_ID_HDMI(Enable)   (DDCHDMI_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_HDMI()   (DDCHDMI_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define D2B_INT_MASK_ALL()  (INTMASK_EN_ADC  = 0xFF, INTMASK_EN_DVI0 = 0xFF, INTMASK_EN_DVI1 = 0xFF)
#elif CHIP_ID == CHIP_TSUMU
#define D2B_SET_ID_HDMI(Enable)   (DDCHDMI_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_HDMI()   (DDCHDMI_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0)
#define D2B_SET_ID_DP(Enable)   (DDCDP_DDCCI_ID = (Enable)?(0xB7):(0))
#define D2B_INT_MASK_ALL()  (INTMASK_EN_ADC  = 0xFF, INTMASK_EN_DVI0 = 0xFF, INTMASK_EN_DVI1 = 0xFF, INTMASK_EN_DVI2 = 0xFF)
#elif CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUMY
#define D2B_INT_MASK_ALL()  (INTMASK_EN_ADC  = 0xFF, INTMASK_EN_DVI0 = 0xFF, INTMASK_EN_DVI1 = 0xFF)
#endif
