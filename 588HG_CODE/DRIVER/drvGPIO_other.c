///////////////////////////////////////////////////////////////////////////////
/// @file drvScalerGPIO.c
/// @brief Scaler GPIO/PWM access
/// @author MStarSemi Inc.
///
/// Driver for Scaler GPIO/PWM access.
///
/// Features
///  -
///  -
///////////////////////////////////////////////////////////////////////////////
#define _DRVGPIO_C

#include "types.h"
#include "board.h"
//#include "global.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
//#include "DebugMsg.h"
#include "Debug.h"
#include "drvgpio.h"

//MST_MODULE(drvGPIO)

#if CHIP_ID==CHIP_TSUMU
//PWM driver
typedef struct
{
    BYTE u8Mask;
    BYTE u8Shift;
} PWMExtBitsType;
PWMExtBitsType code ExtBits[_PWM_NUMS_]=
{   //mask,shift
    {0x03,  16},//PWM0
    {0x0C,  14},//PWM1
    {0x30,  12},//PWM2
    {0xC0,  10},//PWM3
    {0x03,  16},//PWM4
    {0x0C,  14},//PWM5
    {0x03,  16},//PWM6
    {0x0C,  14},//PWM7
};
DWORD drvGPIO_GetPWMPeriod(BYTE u8PWMCh)
{
    BYTE u8Bank=msReadByte(REG_3200);
    DWORD u32Period;

    if (u8PWMCh<_PWM6_) //0~5
        msWriteByte(REG_3200,0x01);// sub bank1
    else if (u8PWMCh<_PWM_NUMS_) //6~7
        msWriteByte(REG_3200,0x02);// sub bank2
    else
        return 0;
     //read period bit[17:16]
    if (u8PWMCh==_PWM4_ || u8PWMCh==_PWM5_)
        u32Period=((DWORD)(msReadByte(REG_3241)&ExtBits[u8PWMCh].u8Mask))<<ExtBits[u8PWMCh].u8Shift;
    else
        u32Period=((DWORD)(msReadByte(REG_3240)&ExtBits[u8PWMCh].u8Mask))<<ExtBits[u8PWMCh].u8Shift;
    //read period bit[15:0]
    u32Period|=msRead2Byte(REG_3204+((u8PWMCh%_PWM6_)*6));
    msWriteByte(REG_3200,u8Bank);
    return u32Period;
}

void drvGPIO_PWMAlignVSync(BYTE u8PWMCh,Bool fEnable)
{
    BYTE u8Bank=msReadByte(REG_3200);
    if (u8PWMCh<_PWM6_) //0~5
        msWriteByte(REG_3200,0x01);// sub bank1
    else if (u8PWMCh<_PWM_NUMS_) //6~7
        msWriteByte(REG_3200,0x02);// sub bank2
    else
        return;
    msWriteByteMask(REG_3209+(u8PWMCh*6), (fEnable?BIT2:0), BIT2);
    msWriteByte(REG_3200,u8Bank);
}

void drvGPIO_SetPWMDuty(BYTE u8PWMCh,DWORD u32Duty)
{
    BYTE u8Bank=msReadByte(REG_3200);
    #if 0//DEBUG_EN
    DWORD u32Period=drvGPIO_GetPWMPeriod(u8PWMCh);
    MST_ASSERT(u32Duty<=u32Period);
    #endif

    if (u8PWMCh<_PWM6_) //0~5
        msWriteByte(REG_3200,0x01);// sub bank1
    else if (u8PWMCh<_PWM_NUMS_) //6~7
        msWriteByte(REG_3200,0x02);// sub bank2
    else
        return;
    //write bit[15:0]
    msWrite2Byte(REG_3206+((u8PWMCh%_PWM6_)*6), (u32Duty&0xFFFF));
    //write bit[17:16]
        if (u8PWMCh==_PWM4_ || u8PWMCh==_PWM5_)
            msWriteByteMask(REG_3243, ((u32Duty>>16)<<((u8PWMCh-_PWM4_)<<1)), ExtBits[u8PWMCh].u8Mask);
        else
        {
            if (u8PWMCh>_PWM5_)
                u8PWMCh-=_PWM6_;
            msWriteByteMask(REG_3242, ((u32Duty>>16)<<(u8PWMCh<<1)) ,ExtBits[u8PWMCh].u8Mask);
        }
        msWriteByte(REG_3200,u8Bank);
}

void drvGPIO_SetPWMFreq(BYTE u8PWMCh,DWORD u32PWMFreq)
{ //****keep divider is default 0****
    BYTE u8Bank=msReadByte(REG_3200);
    if (u8PWMCh<_PWM6_) //0~5
        msWriteByte(REG_3200,0x01);// sub bank1
    else if (u8PWMCh<_PWM_NUMS_) //6~7
        msWriteByte(REG_3200,0x02);// sub bank2
    else
        return;
    #define PWM_PERIOD  u32PWMFreq
    //PWM DIV
    PWM_PERIOD=(u32PWMFreq?(((unsigned long)XTAL_CLOCK_KHZ*1000/u32PWMFreq)):0);
    if (PWM_PERIOD<0x40000)
    {
        //write bit[15:0]
        msWrite2Byte(REG_3204+((u8PWMCh%_PWM6_)*6), PWM_PERIOD);
        //write bit[17:16]
        if (u8PWMCh==_PWM4_ || u8PWMCh==_PWM5_)
            msWriteByteMask(REG_3241, ((PWM_PERIOD>>16)<<((u8PWMCh-_PWM4_)<<1)), ExtBits[u8PWMCh].u8Mask);
        else
        {
            if (u8PWMCh>_PWM5_)
                u8PWMCh-=_PWM6_;
            msWriteByteMask(REG_3240, ((PWM_PERIOD>>16)<<(u8PWMCh<<1)), ExtBits[u8PWMCh].u8Mask);
        }
    }
    msWriteByte(REG_3200,u8Bank);

    #undef PWM_PERIOD
}

void drvGPIO_SetBacklightDuty(BYTE u8PWMCh,BYTE u8ByteDuty)
{
    DWORD u32Temp=drvGPIO_GetPWMPeriod(u8PWMCh);
    //printData("u8ByteDuty=%d",u8ByteDuty);
    u32Temp=LINEAR_MAPPING_VALUE(u8ByteDuty, 0, 0xFF, 0, u32Temp);
    //printData("Duty hi=%xh",u32Temp>>16);
    //printData("Duty lo=%xh",(WORD)u32Temp);
    drvGPIO_SetPWMDuty(u8PWMCh, u32Temp);
}

#if 0//(SICODE_IS_B) //Richard 20110120
DWORD drvGPIO_GetPWMDuty(BYTE u8PWMCh)
{
    DWORD u32Duty=0;

    BYTE u8Bank=msReadByte(REG_3200);
    if (u8PWMCh<_PWM6_) //0~5
        msWriteByte(REG_3200,0x01);// sub bank1
    else if (u8PWMCh<_PWM_NUMS_) //6~7
        msWriteByte(REG_3200,0x02);// sub bank2
    else
        return 0;
    //read bit[17:16]
    if (u8PWMCh==_PWM4_ || u8PWMCh==_PWM5_)
        u32Duty=((DWORD)(msReadByte(REG_3243)&ExtBits[u8PWMCh].u8Mask))<<ExtBits[u8PWMCh].u8Shift;
    else
        u32Duty=((DWORD)(msReadByte(REG_3242)&ExtBits[u8PWMCh].u8Mask))<<ExtBits[u8PWMCh].u8Shift;
    //read bit[15:0]
    u32Duty|=msRead2Byte(REG_3206+((u8PWMCh%_PWM6_)*6));
    msWriteByte(REG_3200,u8Bank);
    return u32Duty ;
}
#if 1
BYTE drvGPIO_GetPWMDuty8Bits(BYTE u8PWMCh)
{
    DWORD u32Temp=drvGPIO_GetPWMPeriod(u8PWMCh);
    u32Temp=LINEAR_MAPPING_VALUE(drvGPIO_GetPWMDuty(u8PWMCh), 0, u32Temp, 0, 0xFF);
    return (BYTE)u32Temp;
}
#endif
#endif

/**************************************************
* The PWM driver functions for CHIP_TSUMC.
* Modify the register mappings for CHIP_TSUMC.
**************************************************/
#elif CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ||CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF
#define _MAX_PWM_ _PWM9_

#if (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF)
void drvGPIO_PWMAlignHSync(BYTE u8PWMCh, Bool fEnable)
{
    if (u8PWMCh > _MAX_PWM_)    /* Check if the u8PWMCh argument is appropriate */
        return;
    else if (u8PWMCh < _PWM3_)
        msWriteByteMask(REG_0283+(u8PWMCh<<1), (fEnable?BIT4:0), BIT4);
    else
        msWriteByteMask(REG_029D+((u8PWMCh-_PWM3_)<<1), (fEnable?BIT4:0), BIT4);
}
#endif

void drvGPIO_PWMAlignVSync(BYTE u8PWMCh, Bool fEnable)
{
    if (u8PWMCh > _MAX_PWM_)    /* Check if the u8PWMCh argument is appropriate */
        return;
    else if (u8PWMCh < _PWM3_)
        msWriteByteMask(REG_0283+(u8PWMCh<<1), (fEnable?BIT3:0), BIT3);
    else
        msWriteByteMask(REG_029D+((u8PWMCh-_PWM3_)<<1), (fEnable?BIT3:0), BIT3);
}

#if (CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
// Line delay can support 1~254 lines
void drvGPIO_PWMAlignVSyncDelay(BYTE u8PWMCh,BYTE u8DlyNum)
{
    if((u8DlyNum==0xFF)||(u8PWMCh > _MAX_PWM_))
        return;
    msWriteBit(REG_02C4,(u8DlyNum!=0), BIT0);
    msWriteByte(REG_02B0+2*u8PWMCh, (u8DlyNum!=0)?(u8DlyNum+1):0);
}
void drvGPIO_PWMInverse(BYTE u8PWMCh,BOOL bEnable)
{
    if (u8PWMCh > _MAX_PWM_)    /* Check if the u8PWMCh argument is appropriate */
        return;
    if(u8PWMCh>_PWM7_)
        msWriteBit(REG_02C7,bEnable, BIT0<<(u8PWMCh-_PWM8_));
    else
        msWriteBit(REG_02C6,bEnable, BIT0<<u8PWMCh);
}
#endif

void drvGPIO_SetPWMDuty(BYTE u8PWMCh, BYTE u8Duty)
{
    if (u8PWMCh > _MAX_PWM_)    /* Check if the u8PWMCh argument is appropriate */
        return;
    else if (u8PWMCh < _PWM4_)
        msWriteByte(REG_0290+(u8PWMCh), u8Duty);
    else
        msWriteByte(REG_02AA+(u8PWMCh-_PWM4_), u8Duty);
}

//**************************************************************************
//  [Function Name]:
//      drvGPIO_SetPWMFreq()
//  [Description]:
//      PWM Freq. Setting
//      Freq. = CLK / ((PM_PWMDIV + 1)*256)
//      !!!!!! Caution !!!!!!
//      As the fomula shown above, real output freq. may not be same as input target Freq because of the integer pwm_div setting
//      Please measure the output waveform to double confirm.
//      XTAL 12M: pwm max 46.875k Hz, min 45 Hz, non-continuously
//      XTAL_14.318M: pwm max 55.959kHz, min 54 Hz, non-continuousely
//  [Arguments]:
//      [BYTE] PWM Channel
//      [DWORD] Target Freq.
//  [Return]:
//      void
//**************************************************************************
void drvGPIO_SetPWMFreq(BYTE u8PWMCh, DWORD u32PWMFreq)
{
    DWORD u32PWMFreqOrig, u32PWMFreqResult, u32PWMFreqMax, u32PWMFreqMin;

    if (u8PWMCh > _MAX_PWM_)    /* Check if the u8PWMCh argument is appropriate */
        return;

    u32PWMFreqMax = (((DWORD)XTAL_CLOCK_KHZ*1000)/256uL);
    u32PWMFreqMin = (((DWORD)XTAL_CLOCK_KHZ*1000)/262144uL);
    if( u32PWMFreq < u32PWMFreqMin )
    {
    #if DEBUG_PRINT_ENABLE
        printData("ERROR PWM Min Freq: %dHz", u32PWMFreqMin);
    #endif
        return;
    }

    if( u32PWMFreq > u32PWMFreqMax )
    {
    #if DEBUG_PRINT_ENABLE
        printData("ERROR PWM Max Freq: %dHz", u32PWMFreqMax);
    #endif
        return;
    }

    u32PWMFreqOrig = u32PWMFreq;

    #define PWM_DIV_VALUE  u32PWMFreq
    PWM_DIV_VALUE = (u32PWMFreq?(((unsigned long)XTAL_CLOCK_KHZ*1000/256/u32PWMFreq)-1):0);

    u32PWMFreqResult = ((unsigned long)XTAL_CLOCK_KHZ*1000/((u32PWMFreq+1)*256));
	#if DEBUG_PRINT_ENABLE
    printData("PWM Channel: %d", u8PWMCh);
    printData("Freq Target: %d K", u32PWMFreqOrig/1000);
    printData("             %d Hz", u32PWMFreqOrig%1000);
    printData("Freq Output: %d K", u32PWMFreqResult/1000);
    printData("             %d Hz\r\n", u32PWMFreqResult%1000);
    #endif
    if (u8PWMCh < _PWM3_)
    {
        msWrite2ByteMask(REG_0282+(u8PWMCh<<1), PWM_DIV_VALUE, 0x07FF);
    }
    else
    {
        msWrite2ByteMask(REG_029C+((u8PWMCh-_PWM3_)<<1), PWM_DIV_VALUE, 0x07FF);
    }

    #undef PWM_DIV_VALUE
}

void drvGPIO_SetBacklightDuty(BYTE u8PWMCh,BYTE u8ByteDuty)
{
    drvGPIO_SetPWMDuty(u8PWMCh, u8ByteDuty);
}

/**************************************************
* CHIP_ID != CHIP_TSUMU && CHIP_ID != CHIP_TSUMC
**************************************************/
#else //CHIP_TSUML, not verify yet!
  #if (CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY)
  #define _MAX_PWM_ _PWM3_
  #else //CHIP_TSUMV
  #define _MAX_PWM_ _PWM6_
  #endif
void drvGPIO_PWMAlignVSync(BYTE u8PWMCh,Bool fEnable)
{
    #if 0//DEBUG_EN
    MST_ASSERT(u8PWMCh<=_MAX_PWM_);
    #endif
    #if CHIP_ID>=CHIP_TSUMV
    if (u8PWMCh>=_PWM4_)
        msWriteByteMask(REG_0283+((u8PWMCh-_PWM4_)<<1), (fEnable?BIT3:0),BIT3);
    else
    #endif
        msWriteByteMask(REG_0289+(u8PWMCh<<1), (fEnable?BIT3:0),BIT3);
}

void drvGPIO_SetPWMDuty(BYTE u8PWMCh,BYTE u8Duty)
{
    #if 0//DEBUG_EN
    MST_ASSERT(u8PWMCh<=_MAX_PWM_);
    #endif
    if (u8PWMCh>_MAX_PWM_)
        return;
    #if CHIP_ID>=CHIP_TSUMV
    if (u8PWMCh>=_PWM4_)
        msWriteByte(REG_0290+((u8PWMCh-_PWM4_)), u8Duty);
    else
    #endif
        msWriteByte(REG_0294+(u8PWMCh<<1), u8Duty);
}
void drvGPIO_SetPWMFreq(BYTE u8PWMCh,DWORD u32PWMFreq)
{
    #if 0//DEBUG_EN
    MST_ASSERT(u8PWMCh<=_MAX_PWM_);
    #endif
    if (u8PWMCh>_MAX_PWM_)
        return;

    #define PWM_DIV_VALUE  u32PWMFreq
    //PWM DIV
    PWM_DIV_VALUE=(u32PWMFreq?(((unsigned long)XTAL_CLOCK_KHZ*1000/256/u32PWMFreq)-1):0);
    #if (CHIP_ID==CHIP_TSUMV || CHIP_ID == CHIP_TSUM2)
    if (u8PWMCh>=_PWM4_)
    {
        MST_ASSERT(PWM_DIV_VALUE<0x0800);
        // 120315 coding, check
        //msWrite2ByteMask(REG_0282+(u8PWMCh<<1), PWM_DIV_VALUE,0x07FF);
        msWrite2ByteMask(REG_0282+((u8PWMCh-_PWM4_)<<1), PWM_DIV_VALUE,0x07FF);
    }
    else
    #endif
    {
        if (PWM_DIV_VALUE<0x800)
        {
            msWriteByte(REG_0295+(u8PWMCh<<1), 8); //scale ratio (8+8)/16
        }
        else if (PWM_DIV_VALUE<0x1000)
        {
            msWriteByte(REG_0295+(u8PWMCh<<1), 0); //scale ratio (0+8)/16
            PWM_DIV_VALUE=(PWM_DIV_VALUE+1)>>1;
        }
        else
        {
            #if 0//DEBUG_EN
            MST_ASSERT(PWM_DIV_VALUE<0x1000);
            #endif
        }
        msWrite2ByteMask(REG_0288+(u8PWMCh<<1), PWM_DIV_VALUE,0x07FF);
    }

    #undef PWM_DIV_VALUE
}
void drvGPIO_SetBacklightDuty(BYTE u8PWMCh,BYTE u8ByteDuty) //ok
{
    drvGPIO_SetPWMDuty(u8PWMCh, u8ByteDuty);
}

#if 0//(SICODE_IS_B) //Richard 20110120
BYTE drvGPIO_GetPWMDuty8Bits(BYTE u8PWMCh)
{
    #if CHIP_ID>=CHIP_TSUMV
    if (u8PWMCh>=_PWM4_)
        return msReadByte(REG_0290+((u8PWMCh-_PWM4_)));
    #endif
    return msReadByte(REG_0294+(u8PWMCh<<1));
}
#endif
#endif


#if ENABLE_3D_FUNCTION

#if GLASSES_TYPE==GLASSES_IR
#define GLASSES_PWM_ON1     64
#define GLASSES_PWM_ON2     45
#define GLASSES_PWM_L_OPEN  34
#define GLASSES_PWM_L_CLOSE 80
#define GLASSES_PWM_R_OPEN  57
#define GLASSES_PWM_R_CLOSE 103
#define GLASSES_PWM_LR_GAP  900
#define PWM_DVI             2
#define PWM_FREQ            100
#define PWM_PERIOD          ((CRYSTAL_CLOCK/PWM_DVI)/(PWM_FREQ/PWM_DVI))
//Glass L on
#define PWM0_SHIFT          ((PWM_PERIOD*3)/20) //shift 3ms , period = 20ms = 20000us
#define PWM0_DUTY           (PWM0_SHIFT+((PWM_PERIOD*GLASSES_PWM_ON1)/20000)) //duty = 64us
#define PWM0_SHIFT2         (PWM0_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_L_OPEN))/20000)) //duty = 64us
#define PWM0_DUTY2          (PWM0_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_L_OPEN+GLASSES_PWM_ON2))/20000)) //duty = 64us
//Glass R off
#define PWM0_SHIFT3         (PWM0_DUTY2+((PWM_PERIOD*GLASSES_PWM_LR_GAP)/20000))
#define PWM0_DUTY3          (PWM0_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1))/20000))
#define PWM0_FOUTH_RISING   (PWM0_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1+GLASSES_PWM_R_CLOSE))/20000))
#define PWM0_FOUTH_FALLING  (PWM0_FOUTH_RISING+ ((PWM_PERIOD*GLASSES_PWM_ON2)/20000))
//Glass R on
#define PWM1_SHIFT          ((PWM_PERIOD*3)/20) //shift 3ms , period = 20ms
#define PWM1_DUTY           (PWM1_SHIFT+((PWM_PERIOD*GLASSES_PWM_ON1)/20000)) //duty = 64us
#define PWM1_SHIFT2         (PWM1_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_R_OPEN))/20000)) //duty = 64us
#define PWM1_DUTY2          (PWM1_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_R_OPEN+GLASSES_PWM_ON2))/20000)) //duty = 64us
//Glass L off
#define PWM1_SHIFT3         (PWM1_DUTY2+((PWM_PERIOD*GLASSES_PWM_LR_GAP)/20000))
#define PWM1_DUTY3          (PWM1_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1))/20000))
#define PWM1_FOUTH_RISING   (PWM1_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1+GLASSES_PWM_L_CLOSE))/20000))
#define PWM1_FOUTH_FALLING  (PWM1_FOUTH_RISING+ ((PWM_PERIOD*GLASSES_PWM_ON2)/20000))

void drvGPIO_3DGlass_LEDBL(void)
{
    //====================================
    // PWM0
    //Enable open-drain PWM0 pad control on GPIO22.
    // Enable open-drain PWM1 pad control on GPIO23.
    msWrite2Byte(REG_1B08 ,0x2084);
    msWrite2Byte(REG_1B0A ,0x2CF0);

    //msWriteByteMask( REG_1B09, BIT2|BIT3, BIT2|BIT3 );
    //PWM BANK
    msWriteByte(REG_3200, 0x01 );
    //PWM0 divider
    msWriteByte(REG_3208, (PWM_DVI>>1)); // DIV=2
    //BIT0: PWM0 polarity
    //BIT2: PWM0 Vsync reset0
    msWriteByteMask(REG_3208+1, BIT0|BIT2,BIT0|BIT2 );
    //PWM0 enable nvsync reset function
    msWriteByteMask( REG_3268, BIT0, BIT0 );
    //PWM0 Hsync reset counter
    msWriteByteMask( REG_3229, 0x01, 0x0F );
    //PWM0 enable nvsync align left flag function
    msWriteByteMask( REG_326A, BIT0, BIT0 );
    //Enable multiple differential pulse whith mode
    msWriteByteMask( REG_3270, BIT3, BIT3 );
    //PWM0 reset mux
    msWriteByteMask( REG_3229, BIT7, BIT7 );

    //PWM0 extra 2 bit period setting
    msWriteByteMask(REG_3240, (PWM_PERIOD>>16),0x03 );
    //PWM0 period = 0x22F4C ((14.318M/DIV) /(100hz/DIV))
    msWrite2Byte( REG_3204, (WORD)PWM_PERIOD);

    //PWM0 rising point shift counter #0x53E5
    msWrite2Byte( REG_3250, PWM0_SHIFT);
    //PWM0 duty = PWM0 rising point shift + PWM0 duty(64us)
    msWrite2Byte( REG_3206, PWM0_DUTY);//0x55AF
    //PWM0 mask enable/reg_shift2[15]
    //PWM0 period hit cnt start for mask/reg_shift2[14:0]
    msWrite2Byte( REG_32A8, PWM0_SHIFT2);//(34 us)
    //PWM0 period hit cnt end for mask/reg_duty2[15:0]
    msWrite2Byte( REG_32AA, PWM0_DUTY2);
    //PWM0 LR mask enable/reg_shift3[15]
    //PWM0 period hit cnt start for mask2[11:0]/reg_shift3[14:0]
    msWrite2Byte( REG_32D0, PWM0_SHIFT3);
    //PWM0 period hit cnt end for mask2[11:0]/reg_duty3[15:0]
    msWrite2Byte( REG_32D2, PWM0_DUTY3);
    //PWM0 fourth rising edge
    msWrite2Byte( REG_32A0, PWM0_FOUTH_RISING);
    //PWM0 fourth falling edge
    msWrite2Byte( REG_32A2, PWM0_FOUTH_FALLING);

    //PWM1
    msWriteByte(REG_320E, (PWM_DVI>>1) );
    msWriteByteMask( REG_320F, BIT2, BIT2 );
    msWriteByteMask( REG_3268, BIT1, BIT1 );
    msWriteByteMask( REG_3228,  0x01, 0x0F );
    msWriteByteMask( REG_3228,  BIT7, BIT7 );
    msWriteByteMask( REG_326A, BIT1, BIT1 );
    msWriteByteMask( REG_3270, BIT3, BIT3 );
    msWriteByteMask( REG_320F, BIT0, BIT0 );
    msWriteByteMask( REG_3269, BIT1, BIT1 );

    msWriteByteMask(REG_3240, (PWM_PERIOD>>14),BIT2|BIT3 );
    msWrite2Byte( REG_320A, (WORD)PWM_PERIOD);

    msWrite2Byte( REG_3254, PWM1_SHIFT);
    msWrite2Byte( REG_320C, PWM1_DUTY);
    msWrite2Byte( REG_32AC, PWM1_SHIFT2);
    msWrite2Byte( REG_32AE, PWM1_DUTY2);

    msWrite2Byte( REG_32D4, PWM1_SHIFT3);
    msWrite2Byte( REG_32D6, PWM1_DUTY3);
    msWrite2Byte( REG_32A4, PWM1_FOUTH_RISING);
    msWrite2Byte( REG_32A6, PWM1_FOUTH_FALLING);

    msWriteByteMask( REG_3227,BIT0, BIT0);
    msWriteByteMask( REG_1B0B, BIT2|BIT3, BIT2|BIT3 );
    msWriteByte(REG_3200, 0x00 );
    //===========================
}
#elif GLASSES_TYPE==GLASSES_INTEL
#define GLASSES_PWM_ON1     64
#define GLASSES_PWM_ON2     45
#define GLASSES_PWM_L_OPEN  34
#define GLASSES_PWM_L_CLOSE 80
#define GLASSES_PWM_R_OPEN  57
#define GLASSES_PWM_R_CLOSE 103
#define GLASSES_PWM_LR_GAP  900
#define PWM_DVI             4
#define PWM_FREQ            20
#define PWM_PERIOD          ((CRYSTAL_CLOCK/PWM_DVI)/(PWM_FREQ))
//Glass L on
#define PWM0_SHIFT          ((PWM_PERIOD*3)/50)//shift 3ms , period = 20ms = 20000us
#define PWM0_DUTY           (PWM0_SHIFT+((PWM_PERIOD*GLASSES_PWM_ON1)/50000)) //duty = 64us
#define PWM0_SHIFT2         (PWM0_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_L_OPEN))/50000)) //duty = 64us
#define PWM0_DUTY2          (PWM0_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_L_OPEN+GLASSES_PWM_ON2))/50000)) //duty = 64us
//Glass R off
#define PWM0_SHIFT3         (PWM0_DUTY2+((PWM_PERIOD*GLASSES_PWM_LR_GAP)/50000))
#define PWM0_DUTY3          (PWM0_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1))/50000))
#define PWM0_FOUTH_RISING   (PWM0_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1+GLASSES_PWM_R_CLOSE))/50000))
#define PWM0_FOUTH_FALLING  (PWM0_FOUTH_RISING+ ((PWM_PERIOD*GLASSES_PWM_ON2)/50000))
//Glass R on
#define PWM1_SHIFT          ((PWM_PERIOD*3)/50) //shift 3ms , period = 20ms
#define PWM1_DUTY           (PWM1_SHIFT+((PWM_PERIOD*GLASSES_PWM_ON1)/50000)) //duty = 64us
#define PWM1_SHIFT2         (PWM1_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_R_OPEN))/50000)) //duty = 64us
#define PWM1_DUTY2          (PWM1_SHIFT+((PWM_PERIOD*(GLASSES_PWM_ON1+GLASSES_PWM_R_OPEN+GLASSES_PWM_ON2))/50000)) //duty = 64us
//Glass L off
#define PWM1_SHIFT3         (PWM1_DUTY2+((PWM_PERIOD*GLASSES_PWM_LR_GAP)/50000))
#define PWM1_DUTY3          (PWM1_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1))/50000))
#define PWM1_FOUTH_RISING   (PWM1_DUTY2+((PWM_PERIOD*(GLASSES_PWM_LR_GAP+GLASSES_PWM_ON1+GLASSES_PWM_L_CLOSE))/50000))
#define PWM1_FOUTH_FALLING  (PWM1_FOUTH_RISING+ ((PWM_PERIOD*GLASSES_PWM_ON2)/50000))

void drvGPIO_3DGlass_LEDBL(void)
{
    //====================================
    // PWM0
    //Enable open-drain PWM0 pad control on GPIO22.
    // Enable open-drain PWM1 pad control on GPIO23.
    msWrite2Byte(REG_1B08 ,0x2084);
    msWrite2Byte(REG_1B0A ,0x2CF0);

    //msWriteByteMask( REG_1B09, BIT2|BIT3, BIT2|BIT3 );
    //PWM BANK
    msWriteByte(REG_3200, 0x01 );
    //PWM0 divider
    msWriteByte(REG_3208, 0x03); // DIV=4
    //BIT0: PWM0 polarity
    //BIT2: PWM0 Vsync reset0
    msWriteByteMask(REG_3208+1, BIT0|BIT2,BIT0|BIT2 );
    //PWM0 enable nvsync reset function
    msWriteByteMask( REG_3268, BIT0, BIT0 );
    //PWM0 Hsync reset counter
    msWriteByteMask( REG_3229, 0x02, 0x0F );
    //PWM0 enable nvsync align left flag function
    msWriteByteMask( REG_326A, BIT0, BIT0 );
    //Enable multiple differential pulse whith mode
    msWriteByteMask( REG_3270, BIT3, BIT3 );
    //PWM0 reset mux
    msWriteByteMask( REG_3229, BIT7, BIT7 );

    //PWM0 extra 2 bit period setting
    msWriteByteMask(REG_3240, (PWM_PERIOD>>16),0x03 );
    //PWM0 period = 0x22F4C ((14.318M/DIV) /(100hz/DIV))
    msWrite2Byte( REG_3204, (WORD)PWM_PERIOD);

    //PWM0 rising point shift counter #0x53E5
    msWrite2Byte( REG_3250, PWM0_SHIFT);
    //PWM0 duty = PWM0 rising point shift + PWM0 duty(64us)
    msWrite2Byte( REG_3206, PWM0_DUTY);//0x55AF
    //PWM0 mask enable/reg_shift2[15]
    //PWM0 period hit cnt start for mask/reg_shift2[14:0]
    msWrite2Byte( REG_32A8, PWM0_SHIFT2);//(34 us)
    //PWM0 period hit cnt end for mask/reg_duty2[15:0]
    msWrite2Byte( REG_32AA, PWM0_DUTY2);
    //PWM0 LR mask enable/reg_shift3[15]
    //PWM0 period hit cnt start for mask2[11:0]/reg_shift3[14:0]
    msWrite2Byte( REG_32D0, PWM0_SHIFT3);
    //PWM0 period hit cnt end for mask2[11:0]/reg_duty3[15:0]
    msWrite2Byte( REG_32D2, PWM0_DUTY3);
    //PWM0 fourth rising edge
    msWrite2Byte( REG_32A0, PWM0_FOUTH_RISING);
    //PWM0 fourth falling edge
    msWrite2Byte( REG_32A2, PWM0_FOUTH_FALLING);

    //PWM1
    msWriteByte(REG_320E, 0x03 );
    msWriteByteMask( REG_320F, BIT2, BIT2 );
    msWriteByteMask( REG_3268, BIT1, BIT1 );
    msWriteByteMask( REG_3228,  0x02, 0x0F );
    msWriteByteMask( REG_3228,  BIT7, BIT7 );
    msWriteByteMask( REG_326A, BIT1, BIT1 );
    msWriteByteMask( REG_3270, BIT3, BIT3 );
    msWriteByteMask( REG_320F, BIT0, BIT0 );
    msWriteByteMask( REG_3269, BIT1, BIT1 );

    msWriteByteMask(REG_3240, (PWM_PERIOD>>14),BIT2|BIT3 );
    msWrite2Byte( REG_320A, (WORD)PWM_PERIOD);

    msWrite2Byte( REG_3254, PWM1_SHIFT);
    msWrite2Byte( REG_320C PWM1_DUTY);
    msWrite2Byte( REG_32AC, PWM1_SHIFT2);
    msWrite2Byte( REG_32AE, PWM1_DUTY2);

    msWrite2Byte( REG_32D4, PWM1_SHIFT3);
    msWrite2Byte( REG_32D6, PWM1_DUTY3);
    msWrite2Byte( REG_32A4, PWM1_FOUTH_RISING);
    msWrite2Byte( REG_32A6+1, PWM1_FOUTH_FALLING);

    msWriteByteMask( REG_3227,BIT0, BIT0);
    msWriteByteMask( REG_1B0B, BIT2|BIT3, BIT2|BIT3 );
    msWrite2Byte( REG_3224, 0x0DDB);


    msWriteByte(REG_3200, 0x00 );
    //===========================

}
#else //GLASSES_TYPE==GLASSES_NVIDIA

#if defined(_NV_LED_BL_3D_EN_)
#define PWM_DIVIDER             0
#define PWM_CLOCK               (14318000ul / (PWM_DIVIDER+1))

#define PWM_VSYNC_START         (PANEL_HEIGHT + 1) // line

#define PWM_PERIOD_120          (PWM_CLOCK /120)
#define PWM_PERIOD_110          (PWM_CLOCK /110)
#define PWM_PERIOD_100          (PWM_CLOCK /100)

#define LED_BL_CONFIG           (_BIT2|_BIT0)  // enable Vsnc reset

// x: LED on line,  y: vtt ; z: pwm period
#define LED_BL_SHIFT(x,y,z)     (((x-PWM_VSYNC_START)*z)/y)
// x: LED off line ; y: vtt ; z: pwm period
#define LED_BL_DUTY(x,y,z)      (((y-PWM_VSYNC_START + x)*z)/y)

// PWM5
LED_BL_PARAM xdata LedParameter _at_ 0x41F0;


extern BYTE Get3DVFreq(void);

void ms_LEDParameter_Init(void)
{
    LedParameter.u16LedOn = LED_BL_ON_120;
    LedParameter.u16LedOff = LED_BL_OFF_120;
}

void ms_LED_Backlight(BOOL bEnable)
{
    DWORD dwPeriod;
    DWORD dwDuty;
    DWORD dwShift;
    //WORD Vtotal = Get3DVtotal();
    WORD VFreq = Get3DVFreq();
    //====================================
    //ms_LEDParameter_Init();
    if(bEnable)
    {
        msWriteByteMask(REG_1B09 ,_BIT7, _BIT7);
    }
    else
    {
        msWriteByteMask(REG_1B09 ,0, _BIT7);
        return;
    }

    switch(VFreq)
    {
        case 120:
            if(LedParameter.u16LedOn < PWM_VSYNC_START)
                LedParameter.u16LedOn += PANEL_VTT_3D_120;

            dwPeriod = PWM_PERIOD_120;
            dwShift = LED_BL_SHIFT(LedParameter.u16LedOn , PANEL_VTT_3D_120, PWM_PERIOD_120);
            dwDuty = LED_BL_DUTY(LedParameter.u16LedOff, PANEL_VTT_3D_120, PWM_PERIOD_120);
            break;

        case 110:
            if(LedParameter.u16LedOn < PWM_VSYNC_START)
                LedParameter.u16LedOn += PANEL_VTT_3D_110;

            dwPeriod = PWM_PERIOD_110;
            dwShift = LED_BL_SHIFT(LedParameter.u16LedOn , PANEL_VTT_3D_110, PWM_PERIOD_110);
            dwDuty = LED_BL_DUTY(LedParameter.u16LedOff, PANEL_VTT_3D_110, PWM_PERIOD_110);
            break;
        case 100:
            if(LedParameter.u16LedOn < PWM_VSYNC_START)
                LedParameter.u16LedOn += PANEL_VTT_3D_100;

            dwPeriod = PWM_PERIOD_100;
            dwShift = LED_BL_SHIFT(LedParameter.u16LedOn, PANEL_VTT_3D_100, PWM_PERIOD_100);
            dwDuty = LED_BL_DUTY(LedParameter.u16LedOff, PANEL_VTT_3D_100, PWM_PERIOD_100);
            break;
    }

    //PWM BANK
    msWriteByte(REG_3200, 0x01 );
    //PWM5 divider
    msWriteByte(REG_3226, PWM_DIVIDER); // DIV=2
    msWriteByte(REG_3249, PWM_DIVIDER>>8); // DIV=2

    //BIT0: PWM polarity
    //BIT2: PWM Vsync reset0
    msWriteByte(REG_3227, LED_BL_CONFIG );

    //PWM extra 2 bit period setting
    msWriteByteMask(REG_3241, (dwPeriod>>16)<<2, 0x0C );
    //PWM period =
    msWrite2Byte( REG_3222, (WORD)dwPeriod);

    //PWM shift counter
    if(dwShift == 0)
        msWrite2Byte( REG_3264, 1);
    else
        msWrite2Byte( REG_3264, dwShift);
    msWriteByte( REG_3266, dwShift >> 16);
    //PWM duty
    msWrite2Byte( REG_3224, dwDuty);//0x55AF
    //PWM extra 2 bit period setting
    msWriteByteMask(REG_3243, (dwDuty>>16)<<2, 0x0C );

    msWriteByte(REG_3200, 0x00 );
    //===========================

}
#endif

#endif


#endif