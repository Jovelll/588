
///////////////////////////////////////
// DRVMCU_V VERSION: V01
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
#define    IDX_MCU_CLK_4MHZ      0
#define    IDX_MCU_CLK_RESERVED0 1
#define    IDX_MCU_CLK_RESERVED1 2
#define    IDX_MCU_CLK_108MHZ    3
#define    IDX_MCU_CLK_86MHZ     4
#define    IDX_MCU_CLK_54MHZ     5
#define    IDX_MCU_CLK_27MHZ     6
#define    IDX_MCU_CLK_XTAL      7
//not used
#define    IDX_MCU_CLK_MPLL_DIV  8
#define    IDX_MCU_CLK_144MHZ    9


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
#define    IDX_SPI_CLK_4MHZ     0
#define    IDX_SPI_CLK_27MHZ    1
#define    IDX_SPI_CLK_36MHZ    2
#define    IDX_SPI_CLK_43MHZ    3
#define    IDX_SPI_CLK_54MHZ    4
#define    IDX_SPI_CLK_86MHZ    5
#define    IDX_SPI_CLK_XTAL     6
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
extern void SetWDTClk( BYTE clk_live_sel );
extern void SetSPI_Quad_En(BYTE ucEnable);

