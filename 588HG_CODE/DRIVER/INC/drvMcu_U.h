
///////////////////////////////////////
// DRVMCU_U VERSION: V01
////////////////////////////////////////

#define _SMOD       1
#define S0REL       (1024-((_SMOD*CRYSTAL_CLOCK+CRYSTAL_CLOCK)/SERIAL_BAUD_RATE)/64)
#define S1REL       (1024-(                      CRYSTAL_CLOCK/SERIAL_BAUD_RATE)/32)

#define    CLOCK_MPLL_MHZ  170000000ul
#define    CLOCK_144MHZ    144000000ul
#define    CLOCK_108MHZ    108000000ul
#define    CLOCK_86MHZ      86000000ul
#define    CLOCK_54MHZ      54000000ul
#define    CLOCK_43MHZ      43000000ul
#define    CLOCK_36MHZ      36000000ul
#define    CLOCK_27MHZ      27000000ul
#define    CLOCK_XTAL      CRYSTAL_CLOCK
#define    CLOCK_4MHZ        4000000ul
#define    CLOCK_0MHZ              0ul

//mapping to g_mcuPLLFreqTable[]
#define    IDX_MCU_CLK_MPLL_DIV  0
#define    IDX_MCU_CLK_RESERVED0 1
#define    IDX_MCU_CLK_144MHZ    2
#define    IDX_MCU_CLK_108MHZ    3
#define    IDX_MCU_CLK_86MHZ     4
#define    IDX_MCU_CLK_54MHZ     5
#define    IDX_MCU_CLK_27MHZ     6
#define    IDX_MCU_CLK_RESERVED1 7
#define    IDX_MCU_CLK_XTAL      8
#define    IDX_MCU_CLK_4MHZ      9


#ifndef MCU_SPEED_INDEX
#error "Please define MCU_SPEED_INDEX!!!"
#endif
//Jison, used for decide the cpu clock in compile time
#if MCU_SPEED_INDEX==IDX_MCU_CLK_RESERVED0 || MCU_SPEED_INDEX==IDX_MCU_CLK_RESERVED1
#error "Wrong MCU_SPEED_INDEX define!!!"
#endif
#if CHIP_ID!=CHIP_TSUMB && CHIP_ID!=CHIP_TSUMY && CHIP_ID!=CHIP_TSUMU && CHIP_ID!=CHIP_TSUM2
  #if MCU_SPEED_INDEX==IDX_MCU_CLK_MPLL_DIV || MCU_SPEED_INDEX==IDX_MCU_CLK_144MHZ
  #error "Wrong MCU_SPEED_INDEX define!!!"
  #endif
#endif

// SPI Clock Selection item define
#define    IDX_SPI_CLK_36MHZ    0
#define    IDX_SPI_CLK_43MHZ    1
#define    IDX_SPI_CLK_54MHZ    2
#define    IDX_SPI_CLK_86MHZ    3
#define    IDX_SPI_CLK_XTAL     4
#define    IDX_SPI_CLK_4MHZ     5

typedef enum
{
    SPI_MODE_NORMAL,
    SPI_MODE_FR,
    SPI_MODE_SADD,
    SPI_MODE_DADD,
    SPI_MODE_SAQD,
    SPI_MODE_QAQD
} SPI_ModeType;

typedef enum
{
    SPEED_4MHZ_MODE,
    SPEED_XTAL_MODE,
    SPEED_NORMAL_MODE
} SystemSpeedMode;
enum
{
    CLK_LIVE_XTAL,
    CLK_LIVE_RCOSC_4M
};

extern void Init_WDT( BYTE bEnable );
extern void mcuSetSpiSpeed( BYTE ucIndex );
extern void mcuSetSpiMode( BYTE ucMode );
extern void mcuSetMcuSpeed( BYTE ucSpeedIdx );
extern void mcuSetSystemSpeed(BYTE u8Mode);
extern void mcuInitXdataMapToDRAM(void);
extern void mcuXdataMapToDRAM(DWORD dwADDR);
extern void SetWDTClk( BYTE clk_live_sel );
extern void SetSPI_Quad_En(BYTE ucEnable);

#if 0//ENABLE_DP_INPUT
extern void mcuSetMcuSpeedIsr( BYTE ucSpeed );
#endif

#if FRAME_BFF_SEL==FRAME_BUFFER
#if (ENABLE_HK_CODE_ON_DRAM)
#define MCU_ON_DRAM_START_ADDR      0x00000000UL
#define MCU_ON_DRAM_END_ADDR        0x000EFFFFUL
#define MCU_ON_SPI_START_ADDR       0x000F0000UL
#define MCU_ON_SPI_END_ADDR         0x000FFFFFUL
#define DMA_TO_DRAM_SOURCE_ADDR     MCU_ON_DRAM_START_ADDR
#define DMA_TO_DRAM_DESTIN_ADDR     HK_CODE_ADDR
#define DMA_TO_DRAM_BYTE_COUNT      (MCU_ON_DRAM_END_ADDR-MCU_ON_DRAM_START_ADDR+1)

typedef enum
{
    _SPI,
    _DRAM,
} _CodeType;
_MCUDEC_ void mcuDMADownloadCode(DWORD dwSourceAddr, DWORD dwDestinAddr, DWORD dwByteCount);
_MCUDEC_ void mcuArrangeCodeAddr(BYTE ucType, DWORD dwAddrStart, DWORD dwAddrEnd);
#endif
#endif

#define WIN0_ADDR_START         (0x5000>>10)  // map to xdata 0x5000~0xEFFF
#define WIN0_ADDR_END           (0xF000>>10)
#define WIN1_ADDR_START         WIN0_ADDR_END // map to xdata 0xF000~0xFFFF
#define WIN1_ADDR_END           (0x10000>>10)

