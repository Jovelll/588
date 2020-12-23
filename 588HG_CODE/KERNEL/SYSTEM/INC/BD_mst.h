//#define CPU_CLOCK_KHZ     11059
//#define CPU_CLOCK_KHZ     22118
#define CPU_CLOCK_KHZ       20000
#define CPU_CLOCK_MHZ       (CPU_CLOCK_KHZ/1000)//11//24
#define MST_CLOCK_MHZ       14318
#define SERIAL_BAUD_RATE    57600
//#define SERIAL_BAUD_RATE  9600
//================================================
// Detect Cable
#define hwDVICable_Pin      P2_7
#define hwDSUBCable_Pin     P2_6
//================================================
// Led
#define hwGreenLed_Pin      P0_7
#define hwAmberLed_Pin      P0_7
#define hwBlacklit_Pin      P2_2
#define hwPanel_Pin     P2_1
//================================================
// mStar I2C
#define hwMS_ResetZ_Pin     P2_0
#define hwMS_CSZ_Pin        P2_3
#define hwMS_SCL_Pin        P2_4
#define hwMS_SDA_Pin        P2_5
#define hwMS_ALE_Pin        P2_3
#define hwMS_WRZ_Pin        P2_4
#define hwMS_RDZ_Pin        P2_5
#define hwMS_DDRPort        P3
#define hwMS_DDRShiftBits   4
#define hwMS_DDRBits        0x1E
//================================================
// I2C
#define hwI2C_SCL_Pin       P1_0
#define hwI2C_SDA_Pin       P1_1
#define Set_ms_ResetZ()     hwMS_ResetZ_Pin=1
#define Clr_ms_ResetZ()     hwMS_ResetZ_Pin=0
#define Set_ms_CSZ()        hwMS_CSZ_Pin=1
#define Clr_ms_CSZ()        hwMS_CSZ_Pin=0
#define Set_ms_SDA()        hwMS_SDA_Pin=1
#define Clr_ms_SDA()        hwMS_SDA_Pin=0
#define ms_SDAHi()      hwMS_SDA_Pin==1
#define Set_ms_SCL()        hwMS_SCL_Pin=1
#define Clr_ms_SCL()        hwMS_SCL_Pin=0
#define Set_ms_ALE()        hwMS_ALE_Pin=1
#define Clr_ms_ALE()        hwMS_ALE_Pin=0
#define Set_ms_WRZ()        hwMS_WRZ_Pin=1
#define Clr_ms_WRZ()        hwMS_WRZ_Pin=0
#define Set_ms_RDZ()        hwMS_RDZ_Pin=1
#define Clr_ms_RDZ()        hwMS_RDZ_Pin=0
#define Set_i2c_SDA()       hwI2C_SDA_Pin=1
#define Clr_i2c_SDA()       hwI2C_SDA_Pin=0
#define i2c_SDAHi()     (hwI2C_SDA_Pin)
#define i2c_SDALo()     (!(hwI2C_SDA_Pin))
#define Set_i2c_SCL()       hwI2C_SCL_Pin=1
#define Clr_i2c_SCL()       hwI2C_SCL_Pin=0
#define i2c_SCLHi()     (hwI2C_SCL_Pin)
#define i2c_SCLLo()     (!(hwI2C_SCL_Pin))
#define hw_SetGreenLed()    hwGreenLed_Pin=0
#define hw_ClrGreenLed()    hwGreenLed_Pin=1
#define hw_SetAmberLed()    //hwAmberLed_Pin=0
#define hw_ClrAmberLed()    //hwAmberLed_Pin=1
#if 0
#define hw_SetBlacklit()    hwBlacklit_Pin=1
#define hw_ClrBlacklit()    hwBlacklit_Pin=0
#define hw_SetPanel()       hwPanel_Pin=1
#define hw_ClrPanel()       hwPanel_Pin=0
#else
#define hw_SetBlacklit()    hwBlacklit_Pin=1//0
#define hw_ClrBlacklit()    hwBlacklit_Pin=0//1
#define hw_SetPanel()       hwPanel_Pin=0
#define hw_ClrPanel()       hwPanel_Pin=1
#endif
