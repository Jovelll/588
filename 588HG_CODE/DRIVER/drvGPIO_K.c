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
#include "global.h"
#include "ms_reg.h"
#include "drvgpio.h"

#define GPIO_DEBUG 0
#if ENABLE_DEBUG && GPIO_DEBUG
    #define GPIO_printData(str, value)   printData(str, value)
    #define GPIO_printMsg(str)           printMsg(str)
#else
    #define GPIO_printData(str, value)
    #define GPIO_printMsg(str)
#endif

XDATA WORD u16BackupPWMDuty[_NOUSE_];
XDATA WORD u16BackupPWMShift[_NOUSE_];
XDATA DWORD u32BackupPWMShiftRegVal[_NOUSE_];
XDATA DWORD u32BackupPWMPeriodRegVal[_NOUSE_];

BYTE TransNumToBit(BYTE value)
{
    XDATA BYTE i=1;
    if(value == 0)
        return 1;

    while(value)
    {
        i <<= 1;
        value--;
    }
    return i;
}
//PWM driver
WORD msDrvPWMMaskTransfer(BYTE u8PWMCh)
{
    if((u8PWMCh ==_PWM0_GP0_))
        return _PWM0_;
    else if((u8PWMCh ==_PWM1_GP0_))
        return _PWM1_;
    else if((u8PWMCh ==_PWM2_GP0_)|(u8PWMCh ==_PWM2_GP1_)|(u8PWMCh ==_PWM2_GP2_))
        return _PWM2_;
    else if((u8PWMCh ==_PWM3_GP0_)|(u8PWMCh ==_PWM3_GP1_)|(u8PWMCh ==_PWM3_GP2_))
        return _PWM3_;
    else if((u8PWMCh ==_PWM4_GP0_))
        return _PWM4_;
    else if((u8PWMCh ==_PWM5_GP0_))
        return _PWM5_;
    else if((u8PWMCh ==_PWM6_GP0_))
        return _PWM6_;
    else if((u8PWMCh ==_PWM7_GP0_))
        return _PWM7_;
    else if((u8PWMCh ==_PWM8_GP0_)|(u8PWMCh ==_PWM8_GP1_)|(u8PWMCh ==_PWM8_GP2_))
        return _PWM8_;
    else if((u8PWMCh ==_PWM9_GP0_)|(u8PWMCh ==_PWM9_GP1_)|(u8PWMCh ==_PWM9_GP2_))
        return _PWM9_;
    else if(u8PWMCh ==_DISP_PWM0_GP0_)
        return _DISP_PWM0_;
    else if(u8PWMCh ==_DISP_PWM1_GP0_)
        return _DISP_PWM1_;
    else if(u8PWMCh ==_DISP_PWM2_GP0_)
        return _DISP_PWM2_;
    else if(u8PWMCh ==_DISP_PWM3_GP0_)
        return _DISP_PWM3_;
    else if(u8PWMCh ==_DISP_PWM4_GP0_)
        return _DISP_PWM4_;
    else if(u8PWMCh ==_DISP_PWM5_GP0_)
        return _DISP_PWM5_;
    else
        return _NOUSE_;
}

DWORD msDrvPWMPeriodGet(BYTE u8PWMCh)
{
    XDATA DWORD u32Period;
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);

    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_NOUSE_)
        return 0;

    u32Period = u32BackupPWMPeriodRegVal[u16PWMCh];
    return u32Period;
}

DWORD msDrvPWMShiftGet(BYTE u8PWMCh)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA DWORD u32PWMShift = 0;

    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_NOUSE_)
        return 0;

    u32PWMShift = u32BackupPWMShiftRegVal[u16PWMCh];
    return u32PWMShift;
}

void msAPIPWMShift(BYTE u8PWMCh, DWORD u32PWMShift)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA WORD u16TempDuty=0;
    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_NOUSE_)
        return;

    u16BackupPWMShift[u16PWMCh] = u32PWMShift;

    if ((u32PWMShift>=1)&&(u32PWMShift<=360))
        u32PWMShift = /*(unsigned long long )*/ (unsigned long)msDrvPWMPeriodGet(u8PWMCh) * u32PWMShift / 360;
    else
        u32PWMShift = 1;

    //backup pwm shift register setting
    u32BackupPWMShiftRegVal[u16PWMCh] = u32PWMShift;

    u16TempDuty = u16BackupPWMDuty[u16PWMCh];
    drvGPIO_SetPWMDuty(u8PWMCh,u16TempDuty);
}

void msDrvPWMAlignSync(BYTE u8PWMCh,BYTE u8Config, BYTE u8Counter)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA Bool bVEnable = u8Config&BIT2;
    XDATA Bool bHEnable = u8Config&BIT3;

    if(u16PWMCh >= _NOUSE_)
        return;

    if(bVEnable && bHEnable)
    {
        GPIO_printMsg("Both Hsync and Vsync align are enable, please redefine config \n");
        return;
    }
    if(u16PWMCh >= _DISP_PWM0_)     //DISP PWM
    {
        msWriteBit(REG_3609+6*(u16PWMCh-_DISP_PWM0_), bVEnable|bHEnable, BIT2);     //reset enable

        if(u8Counter == 0&&bVEnable)        //vsync align
        {
            if(u16PWMCh % 2)
                msWriteBit(REG_3628+(u16PWMCh-_DISP_PWM1_), 0, BIT7);
            else
                msWriteBit(REG_3629+(u16PWMCh-_DISP_PWM0_), 0, BIT7);
        }
        else if(bHEnable)        //Hsync align
        {
        	XDATA DWORD u32PWMFreq;
        	XDATA DWORD u32ModHFreq;

        	u32PWMFreq = (unsigned long)XTAL_CLOCK_KHZ*1000/msDrvPWMPeriodGet(u8PWMCh);
		//switch MODpin to TTL mode,to check HV freq only
		#if 0
        	msWrite2Byte(REG_MOD1_34, 0xFFFF);
        	msWrite2Byte(REG_MOD1_20, 0x0000);
        	msWrite2Byte(REG_MOD1_22, 0x0000);
		#endif

        	u32ModHFreq=PanelVTotal*GetVfreq();
        	if (u32PWMFreq>=(u8Counter*u32ModHFreq))
        	{
	            if(u16PWMCh % 2)
	            {
	                msWriteBit(REG_3628+(u16PWMCh-_DISP_PWM1_), 1, BIT7);
	                msWriteByteMask(REG_3628, u8Counter,0x0F);
	            }
	            else
	            {
	                msWriteBit(REG_3629+(u16PWMCh-_DISP_PWM0_), 1, BIT7);
	                msWriteByteMask(REG_3629, u8Counter,0x0F);
	            }
        	}
        	else
        	{
	                msWriteBit(REG_3628+(u16PWMCh-_DISP_PWM1_), 0, BIT7);
	                msWriteBit(REG_3629+(u16PWMCh-_DISP_PWM0_), 0, BIT7);
        	}
        }
        else        //vsync align with counter
        {
            if(u16PWMCh % 2)
            {
                msWriteBit(REG_3628+(u16PWMCh-_DISP_PWM1_), bVEnable|bHEnable, BIT7);
                msWriteByteMask(REG_3628+(u16PWMCh-_DISP_PWM1_), u8Counter, 0x0F);
            }
            else
            {
                msWriteBit(REG_3629+(u16PWMCh-_DISP_PWM0_), bVEnable|bHEnable, BIT7);
                msWriteByteMask(REG_3629+(u16PWMCh-_DISP_PWM0_), u8Counter, 0x0F);
            }
            msWriteBit(REG_3668, bVEnable, TransNumToBit(u16PWMCh-_DISP_PWM0_));
        }
    }
    else        //PWM
    {	 //PM_PWM of CHIP_TSUMK doesnt support Hsync align function.
        if(u16PWMCh < _PWM3_)
        {
            if(bVEnable | bHEnable)
                msWriteBit(REG_0283+(u16PWMCh<<1), 1, (bVEnable? BIT3: BIT4));
            else
                msWriteBit(REG_0283+(u16PWMCh<<1), 0, BIT3|BIT4);
        }
        else
        {
            if(bVEnable | bHEnable)
                msWriteBit(REG_029D+((u16PWMCh-_PWM3_)<<1), 1, (bVEnable? BIT3: BIT4));
            else
                msWriteBit(REG_029D+((u16PWMCh-_PWM3_)<<1), 0, BIT3|BIT4);
        }
    }
}

void drvGPIO_PWMAlignVSyncDelay(BYTE u8PWMCh,BYTE u8DlyNum)
{
    WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    if((u8DlyNum==0xFF)||(u16PWMCh > _PWM9_))
        return;
    msWriteBit(REG_02C4, BIT0, BIT0);
    msWriteByte(REG_02B0+2*u16PWMCh, u8DlyNum);
}

void msDrvPWMDuty(BYTE u8PWMCh,DWORD u32Duty)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA DWORD u32Shift =  msDrvPWMShiftGet(u8PWMCh);
    XDATA DWORD u32Period =  msDrvPWMPeriodGet(u8PWMCh);

    if(u16PWMCh >= _NOUSE_)
        return;

    if(u16PWMCh >= _DISP_PWM0_)
    {
        if ((u32Period-u32Shift)>u32Duty)
        {
            u32Duty+=u32Shift;
        }
        else
        {
            u32Shift=((unsigned long) (u32Period-u32Duty)*360)/u32Period;
            GPIO_printData(" ====>> maximum support shift is %d degree!! \r\n",u32Shift); //unit:n degree ,n=0~360

            //disable phase shift function.
            u32Shift=1;
            u32Duty=u32Duty;
        }

        msWrite2ByteMask(REG_3650+4*(u16PWMCh-_DISP_PWM0_), u32Shift              ,0xFFFF);
        msWriteByteMask(REG_3652+4*(u16PWMCh-_DISP_PWM0_), (u32Shift>>16)&(BIT1|BIT0)   ,BIT1|BIT0);

        msWrite2Byte(REG_3606+6*(u16PWMCh-_DISP_PWM0_), u32Duty);
        msWrite2ByteMask(REG_3642, (u32Duty>>16)<<((u16PWMCh-_DISP_PWM0_)*2), (0x03)<<((u16PWMCh-_DISP_PWM0_)*2));
        GPIO_printData("msDrvPWMDuty              REG_3606==%x\r\n\r\n",msRead2Byte(REG_3606+6*(u16PWMCh-_DISP_PWM0_)));
        GPIO_printData("msDrvPWMDuty              (u32Duty>>16)<<((u16PWMCh-_DISP_PWM0_)*2)==%x\r\n\r\n"   ,(u32Duty>>16)<<((u16PWMCh-_DISP_PWM0_)*2));
        GPIO_printData("msDrvPWMDuty              (0x03)<<((u16PWMCh-_DISP_PWM0_)*2)==%x\r\n\r\n"   ,(0x03)<<((u16PWMCh-_DISP_PWM0_)*2));
    }
    else if(u16PWMCh < _PWM4_)
    {
        msWriteByte(REG_0290+(u16PWMCh), (BYTE)u32Duty);
        GPIO_printData("duty_Add====%x,\r\n",REG_0290+(u16PWMCh));
    }
    else
    {
        msWriteByte(REG_02AA+(u16PWMCh-_PWM4_), (BYTE)u32Duty);
        GPIO_printData("duty_Add====%x,\r\n",REG_02AA+(u16PWMCh-_PWM4_));
    }
}

void drvGPIO_SetPWMDuty(BYTE u8PWMCh, WORD u16ByteDuty)
{
    XDATA DWORD u32Temp=msDrvPWMPeriodGet(u8PWMCh);
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA DWORD u32RealDutyMapToRegSet=0;
    u16BackupPWMDuty[u16PWMCh]=u16ByteDuty;
    GPIO_printData("u16ByteDuty=%x\r\n",u16ByteDuty);
    GPIO_printData("getPreiod=%x\r\n",u32Temp);

    if( u16PWMCh == _NOUSE_ )
        return;

    if(u16PWMCh>=_DISP_PWM0_)
        u32RealDutyMapToRegSet=LINEAR_MAPPING_VALUE(u16ByteDuty, 0, 0xFF, 0, u32Temp);  //input pwm duty 0~0xFF , extend to 0~0x3FFFF;
    else
        u32RealDutyMapToRegSet=u16ByteDuty;

    GPIO_printData(">>>>>>>@@@@@@@@@ u32RealDutyMapToRegSet==%x\r\n",u32RealDutyMapToRegSet);
    msDrvPWMDuty(u8PWMCh, u32RealDutyMapToRegSet);
}

void drvGPIO_SetPWMFreq(BYTE u8PWMCh,DWORD u32PWMFreq)
{
    //_PWM0_GP0_ ~_PWM9_GP1_ :XTAL/((N+1)*256) NOTE:N=0~2048	 Freq:27Hz~55.9KHz
    //_DISP_PWM0_GP0_ ~_DISP_PWM5_GP1_ :	 Freq:~60~6M

    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA WORD u16TempShift=0;
    XDATA DWORD u32PWMPeriod;
    XDATA BYTE u8CheckEnableDB;

    if( u16PWMCh == _NOUSE_ )
        return;

    GPIO_printData("\r\n\r\n\r\nu32PWMFreq==%x\r\n",u32PWMFreq);
    GPIO_printData("\r\n\r\n\r\u16PWMCh==%x\r\n",u16PWMCh);

    //PWM DIV
    if(u16PWMCh>=_DISP_PWM0_)
    {
        u32PWMPeriod = (u32PWMFreq?(((unsigned long)XTAL_CLOCK_KHZ*1000/u32PWMFreq)):0);
        GPIO_printData(" u32PWMPeriod==%x     \r\n",u32PWMPeriod);

        if (u32PWMPeriod<0x40000)
        {
            msWrite2Byte(REG_3604+6*(u16PWMCh-_DISP_PWM0_),u32PWMPeriod);
            msWrite2ByteMask(REG_3640,(u32PWMPeriod>>16)<<((u16PWMCh-_DISP_PWM0_)*2),(0x03)<<((u16PWMCh-_DISP_PWM0_)*2));

            //backup period setting
            u32BackupPWMPeriodRegVal[u16PWMCh] = u32PWMPeriod;
        }

        u16TempShift = u16BackupPWMShift[u16PWMCh];
        msAPIPWMShift(u8PWMCh, u16TempShift);

        #if 1 //if enable DB of disp_pwm, the duty isnt correct if we set duty after setting frequency immediately.
        u8CheckEnableDB=msReadByte(REG_3609+6*(u16PWMCh-_DISP_PWM0_));
        if (u8CheckEnableDB & BIT3)
        {
            ForceDelay1ms(50);
        }
        #endif
    }
    else
    {
        u32PWMPeriod = (u32PWMFreq?(((unsigned long)XTAL_CLOCK_KHZ*1000/(u32PWMFreq*256))-1):0);

        if (u32PWMPeriod<0x800)
        {
            if(u16PWMCh<_PWM3_)
            {
                msWrite2ByteMask(REG_0282+2*(u16PWMCh-_PWM0_),u32PWMPeriod,0x7FF);
                GPIO_printData("u32PWMPeriod=%x\n\r",msRead2Byte(REG_0282+(2*(u16PWMCh-_PWM0_)))&0x7FF);
            }
            else
            {
                msWrite2ByteMask(REG_029C+2*(u16PWMCh-_PWM3_),u32PWMPeriod,0x7FF);
                GPIO_printData("u32PWMPeriod=%x\n\r",msRead2Byte(REG_029C+(2*(u16PWMCh-_PWM3_)))&0x7FF);
            }
        }
    }
}

void drvGPIO_SetBacklightDuty(BYTE u8PWMCh, WORD u16ByteDuty)
{
    drvGPIO_SetPWMDuty(u8PWMCh, u16ByteDuty);
}

void msDrvPWMDoubleBuffer(BYTE u8PWMCh, BYTE u8Config)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA Bool bEnable = u8Config & _PWM_DB_EN_;
    XDATA Bool bDBVsyncAlign = u8Config & _PWM_DB_VSYNC_MODE_;

    if( u16PWMCh == _NOUSE_ )
        return;

    if(u16PWMCh>=_DISP_PWM0_)
    {
        if(bEnable)
        {
            msWriteByteMask(REG_3609+6*(u16PWMCh-_DISP_PWM0_),bDBVsyncAlign?(BIT6|BIT1):(BIT6|BIT3),BIT6|BIT3|BIT1);
        }
        else
        {
            msWriteByteMask(REG_3609+6*(u16PWMCh-_DISP_PWM0_),BIT6,BIT6|BIT3|BIT1);
        }
    }
    else
    {
    	//PM_PWM of CHIP_TSUMK doesnt support double buffer function.
        if(u16PWMCh<_PWM3_)
        {
            msWriteBit(REG_0283+2*u16PWMCh,bEnable,BIT5);
            GPIO_printData("DoubleBuffer_ADD==%x\r\n",REG_0283+2*u16PWMCh);
        }
        else
        {
            msWriteBit(REG_029D+2*(u16PWMCh-_PWM3_),bEnable,BIT5);
            GPIO_printData("DoubleBuffer_ADD==%x\r\n",REG_029D+2*(u16PWMCh-_PWM3_));
        }
    }
}

void msDrvPWMPolarity(BYTE u8PWMCh, BYTE u8Config)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA Bool bEnable = u8Config&BIT4;

    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_NOUSE_)
        return;

    msWriteBit(REG_3609+6*(u16PWMCh-_DISP_PWM0_), bEnable, BIT0);
}

void msAPIPWMConfig(BYTE u8PWMCh, DWORD u32Freq, WORD u16Duty, BYTE u8Config, BYTE u8Counter ,WORD u16Shift)
{
    //enable double buffer before setting duty to avoid incorrected duty after adjust pwm frequency.
    msDrvPWMDoubleBuffer(u8PWMCh,u8Config);
    drvGPIO_SetPWMFreq(u8PWMCh,u32Freq);
    msAPIPWMShift(u8PWMCh,u16Shift);
    drvGPIO_SetPWMDuty(u8PWMCh,u16Duty);
    msDrvPWMAlignSync(u8PWMCh,u8Config,u8Counter);
    msDrvPWMPolarity(u8PWMCh,u8Config);
}

void msDrvPWMEnableSwitch(BYTE u8PWMCh, Bool bEnable)//para1:_PWM0_GP0_~_DISP_PWM5_GP1_
{
    //this function will not disable previous selected PWM
    DWORD u32PWMCh=0;
    if ((u8PWMCh >= _NO_USE_))
    {
        GPIO_printMsg("Can not use this  PWM channel\n");
        return;
    }
    else
    {
        u32PWMCh = ((DWORD)BIT0<<(u8PWMCh));
    }
	
    if((u8PWMCh ==_PWM0_GP0_))
        msWriteBit(REG_0280, bEnable, BIT0);
    else if((u8PWMCh ==_PWM1_GP0_))
        msWriteBit(REG_0280, bEnable, BIT1);
    else if((u8PWMCh ==_PWM2_GP0_)|(u8PWMCh ==_PWM2_GP1_)|(u8PWMCh ==_PWM2_GP2_))
        msWriteBit(REG_0280, bEnable, BIT2);
    else if((u8PWMCh ==_PWM3_GP0_)|(u8PWMCh ==_PWM3_GP1_)|(u8PWMCh ==_PWM3_GP2_))
        msWriteBit(REG_0280, bEnable, BIT7);
    else if((u8PWMCh ==_PWM4_GP0_))
        msWriteBit(REG_0281, bEnable, BIT0);
    else if((u8PWMCh ==_PWM5_GP0_))
        msWriteBit(REG_0281, bEnable, BIT1);
    else if((u8PWMCh ==_PWM6_GP0_))
        msWriteBit(REG_0281, bEnable, BIT2);
    else if((u8PWMCh ==_PWM7_GP0_))
        msWriteBit(REG_0281, bEnable, BIT3);
    else if((u8PWMCh ==_PWM8_GP0_)|(u8PWMCh ==_PWM8_GP1_)|(u8PWMCh ==_PWM8_GP2_))
        msWriteBit(REG_0281, bEnable, BIT4);
    else if((u8PWMCh ==_PWM9_GP0_)|(u8PWMCh ==_PWM9_GP1_)|(u8PWMCh ==_PWM9_GP2_))
        msWriteBit(REG_0281, bEnable, BIT5);

    //PM_PWM : Group 0~2
    if ((u8PWMCh>= _PWM0_GP0_) && (u8PWMCh<=_PWM9_GP0_))
    {
        msWrite2ByteMask((((BACKLIGHT_PWM_CONFIG) & (_PWM_OD_EN_)))?(REG_020A):(REG_0208),(BIT0<<(u8PWMCh-_PWM0_GP0_)),(BIT0<<(u8PWMCh-_PWM0_GP0_))&0x3FF);
    }
    else if ((u8PWMCh>= _PWM2_GP1_) && (u8PWMCh<=_PWM9_GP1_))
    {
        msWriteByteMask((((BACKLIGHT_PWM_CONFIG) & (_PWM_OD_EN_)))?(REG_020D):(REG_020C),(BIT0<<(u8PWMCh-_PWM2_GP1_)),(BIT0<<(u8PWMCh-_PWM2_GP1_))&0x0F);
    }
    else if ((u8PWMCh>= _PWM2_GP2_) && (u8PWMCh<=_PWM9_GP2_))
    {
        msWriteByteMask((((BACKLIGHT_PWM_CONFIG) & (_PWM_OD_EN_)))?(REG_020F):(REG_020E),(BIT0<<(u8PWMCh-_PWM2_GP2_)),(BIT0<<(u8PWMCh-_PWM2_GP2_))&0x0F);
    }
    //DISP PWM
    else if ((u8PWMCh>=_DISP_PWM0_GP0_)&&(u8PWMCh<=_DISP_PWM5_GP0_))
    {
        msWriteByteMask((((BACKLIGHT_PWM_CONFIG) & (_PWM_OD_EN_)))?(REG_026B):(REG_026A),(BIT0<<(u8PWMCh-_DISP_PWM0_GP0_)),(BIT0<<(u8PWMCh-_DISP_PWM0_GP0_))&0x3F);
    }	
}

void drvGPIO_uncall(void)
{
    drvGPIO_PWMAlignVSyncDelay(0, 0);
}



