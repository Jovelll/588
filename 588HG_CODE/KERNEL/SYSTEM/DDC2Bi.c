#include <math.h>
#include "types.h"
#include "board.h"
#include "global.h"
#include "Menudef.h"
#include "menufunc.h"
#include "adjust.h"
//#include "msADC.h"
#include "drvadc.h"
#include "ms_reg.h"
#include "msOSD.h"
#include "autofunc.h"
#include "ms_rwreg.h"
#include "Debug.h"
#include "Common.h"
#include "misc.h"
#include "mstar.h"
#include "power.h"
#include "keypaddef.h"
#include "ddc2bi.h"
#include "userpref.h"
#include "msflash.h"
#include "NVRam.h"
#include "ddc.h"
#include "gpio_def.h"
#include "DDCColorHandler.h"

#if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif


#if ENABLE_DPS
#include "drvOSD.h"
#include "drvGPIO.h"
#endif
////////////////////////////////////////////////////
#include "drvDDC2Bi.h"
//#include "halRwreg.h"
///////////////////////////////////////////////////

extern Bool SetColor( void );
extern Bool SetColorTemp();
extern void CheckModeSettingRange( void );
extern Bool CheckFactorySettingOutOfRange( void );
extern void Init_FactorySetting( void );
extern Bool xdata bFlashWriteFactory;

BYTE AlignControl( void );
#define ASSETSTARTADDR          0x600
#define ASSETLENGTH             64

#if ENABLE_DP_INPUT
#if DPENABLEMCCS
BYTE idata ucfinal_status,g_ucDDC_TxRecord ;  // use idata to speedup
WORD idata DDC2BI_INT_STS, DDC2BI_R_BUF, DDC2BI_W_BUF, DDC2BI_INT_CLR;
code BYTE dpNullMsg[3]={0x6e, 0x80, 0xbe};
static BYTE u8DDC_RxRecord = 0;
#endif
#endif
//#if ENABLE_DPS
//BYTE u8BackupOutDE_H,u8BackupOutDE_L;
//#endif

BYTE code MODEL_TYPE[] =
{
    Model
};
extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;
BYTE code CAP_VCP[] =
{
    "("
    "vcp(02 04 05 06 08 0E 10 12 14(01 05 06 08 0B) 16 18 1A 1E 20 30 3E "
    #if INPUT_TYPE!=INPUT_1A
    "60(01 03) "
    #endif
    #if AudioFunc
    "62 "
    #endif
    "6C 6E 70 C8 "
    "B0 B6 DF)"
    "prot(monitor)"
    "type(LCD)"
    "cmds(01 02 03 07 0C F3)"
    "mccs_ver(2.1)"
    "asset_eep(64)"
    "mpu_ver("FWVersion")"
    "model("Model")"
    "mswhql(1)"
    ")"
};
#if INPUT_TYPE!=INPUT_1A
BYTE code DVI_CAP_VCP[] =
{
    "("
    "vcp(02 04 05 08 10 12 14(01 05 06 08 0B) 16 18 1A 60(01 03)"
#if AudioFunc
    "62 "
#endif
    "6C 6E 70 C8 "
    "B0 B6 DF)"
    "prot(monitor)"
    "type(LCD)"
    "cmds(01 02 03 07 0C F3)"
    "mccs_ver(2.1)"
    "asset_eep(64)"
    "mpu_ver("FWVersion")"
    "model("Model")"
    "mswhql(1)"
    ")"
};
#endif
void DDC2Bi_InitRx( void )
{
    rxStatus = DDC2B_CLEAR;
    rxIndex = 0;
}
void DDC2Bi_Init( void )
{
    drvDDC2Bi_Init();
}
void DDC2Bi_GetTxBuffer( void )
{
    BYTE i;
    for( i = MAX_TXWAIT_DELAY; i > 0; i-- )
        if( txStatus == DDC2B_MSGON )
        {
            Delay1ms( 1 );
        }
    txStatus = DDC2B_CLEAR;
}
BYTE DDC2Bi_ComputeChecksum( BYTE count )
{
    BYTE CheckSum;
    BYTE i = 0;
    CheckSum = INITTxCK;
    for( ; count > 0; count-- )
        CheckSum ^= DDCBuffer[i++];
    return CheckSum;
}
void DDC2Bi_MessageReady( void )
{
    drvDDC2Bi_MessageReady();
}
void DDC2Bi_ParseCommand( void )
{
    drvDDC2Bi_ParseCommand();
}
void DDC2Bi_CommandHandler( void )
{
    if( rxStatus == DDC2B_COMPLETED )
    {
        DDC2Bi_ParseCommand();
        Clr_RxBusyFlag();
        DDC2Bi_InitRx();
    }
}
void DoDDCCI( BYTE Source ) // Annie 2006.7.5
{
    drvDoDDCCI(Source);
}
void ISR_DDC2Bi(void)
{
#if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUMJ || CHIP_ID==CHIP_TSUM9 || CHIP_ID==CHIP_TSUMF)
    if( ADC_INT_FLAG &( BIT3 | BIT4 ) )
    {
        DoDDCCI( InputCombo_A0 );
        CLR_ADC_INT_FLAG();
    }
  #if DDC_Port_D0
    else if( DDCDVI0_INT_FLAG &( BIT3 | BIT4 ) )
    {
        DoDDCCI( InputCombo_D0 );
        CLR_DVI0_INT_FLAG();
    }
  #endif
  #if DDC_Port_D1
    else if( DDCDVI1_INT_FLAG &( BIT3 | BIT4 ) )
    {
        DoDDCCI( InputCombo_D1 );
        CLR_DVI1_INT_FLAG();
    }
  #endif
  #if DDC_Port_D2
    else if( DDCDVI2_INT_FLAG &( BIT3 | BIT4 ) )
    {
        DoDDCCI( InputCombo_D2 );
        CLR_DVI2_INT_FLAG();
    }
  #endif

#else // CHIP_ID != CHIP_TSUMC
    if( ADC_INT_FLAG &( BIT3 | BIT4 ) )
    {
        DoDDCCI( Input_Analog1 );
        CLR_ADC_INT_FLAG();
    }
  #if INPUT_TYPE!=INPUT_1A
    else if( DDCDVI_INT_FLAG &( BIT3 | BIT4 ) )
    {
        DoDDCCI( Input_Digital );
        CLR_DVI_INT_FLAG();
    }
  #endif
  #if ENABLE_HDMI
    else if( DDCHDMI_INT_FLAG & ( BIT3 | BIT4 ) )
    {
        DoDDCCI( Input_HDMI );
        CLR_HDMI_INT_FLAG();
    }
  #endif
#endif // #if CHIP_ID == CHIP_TSUMC
}


#if  ENABLE_DP_INPUT
#if  DPENABLEMCCS
#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
//================= DP =====================

void DDC2BI_DP(void)     //Rockman
{

        BYTE u8AUXCH_RxCmd;
        BYTE u8DDC_Length;
        #define u8DDC_RxOnGoing u8DDC_RxRecord

        u8AUXCH_RxCmd = (msRegs[REG_0638 + Aux])&0x0F;
        u8DDC_Length = msRegs[REG_063A+ Aux];
        //===============//<write>PC Read Command============================
         if(u8AUXCH_RxCmd & _BIT0)    //DP Tx
        {

            if (u8DDC_Length)
            {

                if(rxStatus == DDC2B_CLEAR && txLength)
                {
                    while(u8DDC_Length-- && txLength)
                    {
                        if(g_ucDDC_TxRecord)
                        {
                            msRegs[REG_06F3]=DDCBuffer[g_ucDDC_TxRecord-1];
                            //msRegs[REG_06F3]= test[g_ucDDC_TxRecord-1];
                            txLength--;
                        }
                        else
                            msRegs[REG_06F3] =DDC2B_DEST_ADDRESS;

			    msRegs[REG_06F2 ] = BIT0;
			    while( ! (((msRegs[REG_06F2])&BIT2)))		// Wait
	                  {}
	                  msRegs[REG_06F2 ] = BIT1;				       // Clear
                         g_ucDDC_TxRecord++;
                    }
                }
                else
                {
                    while(u8DDC_Length--)
                    {
                         msRegs[REG_06F3]= dpNullMsg[g_ucDDC_TxRecord++];
                         msRegs[REG_06F2 ] = BIT0;
			    while( ! (((msRegs[REG_06F2])&BIT2)))		// Wait
	                  {}
	                  msRegs[REG_06F2 ] = BIT1;				       // Clear


                    }
                }
            }

        }
        //===============//<write>PC Write Command============================
        else if(u8AUXCH_RxCmd==0x00 || u8AUXCH_RxCmd==0x04)    //DP Rx
                       {
                if (u8DDC_Length)
                {
                    if (!u8DDC_RxOnGoing)
                    {
                        BYTE xdata temp;
                        msRegs[REG_06F0]= msRegs[REG_06F0]|_BIT0;
                         while( ! (((msRegs[REG_06F0])&BIT2)))		// Wait
	                  {}
	                 msRegs[REG_06F0 ] = BIT1;				       // Clear
                        temp=msRegs[REG_0675];
                        rxIndex=0;
                        u8DDC_RxOnGoing=TRUE;
                        u8DDC_Length--;
                    }
                    while(u8DDC_Length--)
                    {
                         BYTE xdata temp1=10;
                            msRegs[REG_06F0]= msRegs[REG_06F0]|_BIT0;
                            while( ! (((msRegs[REG_06F0])&BIT2)))		// Wait
	                     {}
	                     msRegs[REG_06F0 ] = BIT1;
                            DDCBuffer[rxIndex++] = msRegs[REG_0675];
                            //DDCData[rxIndex++] = msRegs[REG_0675];
                    }
                }

                if (u8AUXCH_RxCmd==0x00)
                {
                    u8DDC_RxOnGoing=FALSE;
                    g_ucDDC_TxRecord=0;
                    rxStatus = DDC2B_COMPLETED;
                    txLength=0;
                }

        }
                msRegs[REG_0679 + Aux]= msRegs[REG_0679 + Aux]|_BIT7;//Clear Interrupt
                msRegs[REG_0679 + Aux]= 0x00;

        #undef u8DDC_RxOnGoing


}
#else
void DDC2BI_DP(void)
{

        BYTE u8AUXCH_RxCmd;
        BYTE u8DDC_Length;
        #define u8DDC_RxOnGoing u8DDC_RxRecord

        u8AUXCH_RxCmd = (msRegs[REG_0638 + Aux])&0x0F;
        u8DDC_Length = msRegs[REG_063A+ Aux];
        //===============//<write>PC Read Command============================
         if(u8AUXCH_RxCmd & _BIT0)    //DP Tx
        {

            if (u8DDC_Length)
            {
                if(rxStatus == DDC2B_CLEAR && txLength)
                {
                    while(u8DDC_Length-- && txLength)
                    {
                        if(g_ucDDC_TxRecord)
                        {
                            msRegs[REG_06D3 + Aux]=DDCBuffer[g_ucDDC_TxRecord-1];
                            txLength--;
                        }
                        else
                            msRegs[REG_06D3 + Aux]=DDC2B_DEST_ADDRESS;

                            g_ucDDC_TxRecord++;
                    }
                }
                else
                {
                    while(u8DDC_Length--)
                    {
                         msRegs[REG_06D3 + Aux]= dpNullMsg[g_ucDDC_TxRecord++];
                    }
                }
            }

        }
        //===============//<write>PC Write Command============================
        else if(u8AUXCH_RxCmd==0x00 || u8AUXCH_RxCmd==0x04)    //DP Rx
                       {
                if (u8DDC_Length)
                {
                    if (!u8DDC_RxOnGoing)
                    {
                        BYTE xdata temp;
                        msRegs[REG_06FC + Aux]= msRegs[REG_06FC + Aux]|_BIT2;
                        temp=msRegs[REG_0675 + Aux];
                        rxIndex=0;
                        u8DDC_RxOnGoing=TRUE;
                        u8DDC_Length--;
                    }
                    while(u8DDC_Length--)
                    {
                         BYTE xdata temp1=10;
                            msRegs[REG_06FC + Aux]= msRegs[REG_06FC + Aux]|_BIT2;
                            while(temp1--){};
                            temp1=10;
                            DDCBuffer[rxIndex++] = msRegs[REG_0675 + Aux];
                    }
                }

                if (u8AUXCH_RxCmd==0x00)
                {
                    u8DDC_RxOnGoing=FALSE;
                    g_ucDDC_TxRecord=0;
                    rxStatus = DDC2B_COMPLETED;
                    txLength=0;
                }

        }
                msRegs[REG_0679 + Aux]= msRegs[REG_0679 + Aux]|_BIT7;//Clear Interrupt
                msRegs[REG_0679 + Aux]= 0x00;

        #undef u8DDC_RxOnGoing


}
#endif

void msDP_DDC2BI(void)
{
     if(msRegs[REG_0678]&_BIT7 //DP1
#if(( DVI_PORT==TMDS_PORT_C) ||( HDMI_PORT==TMDS_PORT_C))//#if (CInput_Displayport_C3 !=  CInput_Nothing) // DP2
	|| msRegs[REG_0778]&_BIT7
#endif
	 )   //MCCS Interrupt
    {
    	 msRegs[REG_1839] =     msRegs[REG_1839] +1;
        DDC2BI_DP() ;
    }
}

#endif
#endif
#if DDCCI_FILTER_FUNCTION // 111228 coding not ready
Bool DDCCICommandExecutable(void)
{
    BYTE CommandCode, CPCode;
    Bool bresult = TRUE;

    CommandCode = DDCBuffer[1];
    CPCode = DDCBuffer[2];

// check others conditions

    if(!DDCciFlag)
    {
        if( CommandCode == SETVCPFeature || CommandCode == GETVCPFeature )
        {
            CPCode = DDCBuffer[2];
            if( CPCode != 0xFE )
                return FALSE;
        }
        else
        {
            return FALSE;
        }
    }

    if(CommandCode == SETVCPFeature || CommandCode == SETPagedVCP)
    {
        switch(CPCode)
        {
            case Geometry_Reset:
            case ADJ_Clock:
            case Auto_Setup:
            case Auto_Color:
            case ADJ_HorizontalPos:
            case ADJ_VerticalPos:
            case ADJ_Phase:
                if(!CURRENT_INPUT_IS_VGA())
                    bresult = FALSE;
            break;

            case ADJ_Brightness:
            case ADJ_Contrast:
                if(!UserPrefDcrMode&&(UserPrefECOMode==ECO_Standard))
                    bresult = FALSE;
            break;

            default:
            break;
        }
    }
    else if(CommandCode == GETVCPFeature || CommandCode == GETPagedVCP)
    {
        switch(CPCode)
        {
            case Geometry_Reset:
            case ADJ_Clock:
            case Auto_Setup:
            case Auto_Color:
            case ADJ_HorizontalPos:
            case ADJ_VerticalPos:
            case ADJ_Phase:
                if(!CURRENT_INPUT_IS_VGA())
                    bresult = FALSE;
            break;

            default:
            break;
        }
    }

    return bresult;

}
#endif
//===================================================
BYTE AlignControl( void )
{
    BYTE CommandCode, CPCode, ValueH, ValueL, RetValueH, RetValueL, PageValue;
    BYTE TPValue;
    WORD WordValue;
    WORD WordAddr;

    CommandCode = DDCBuffer[1];
    PageValue = 0;
#if DDCCI_ENABLE
    if ( !DDCciFlag )           //We can adjust the W/B By DDC2BI When DDCCI Is Disable
#endif //When DDCCI_ENABLE=0,we use below to disable DDCCI and Use it to adjust W/B
    {
        if( CommandCode == SETVCPFeature || CommandCode == GETVCPFeature )
        {
            CPCode = DDCBuffer[2];
            if( CPCode != 0xFE 
	#if ENABLE_HDMI_DRR_MCCS
		   &&(CPCode != HDMI_FREESYNC_VCP) 
       #endif//wtx add AMD MCCS
)
                return FALSE;
        }
        else
            return FALSE;
    }

#if DDCCI_DEBUG_ENABLE
    Osd_Show();
    Osd_Draw4Num( 15, 0, DDCBuffer[1] );
    Osd_Draw4Num( 15, 1, DDCBuffer[2] );
    Osd_Draw4Num( 15, 2, DDCBuffer[3] );
    Osd_Draw4Num( 15, 3, DDCBuffer[4] );
    Osd_Draw4Num( 15, 4, DDCBuffer[5] );
    //Delay1ms(1000);
#endif

    if( CommandCode == SETVCPFeature )
    {
        CPCode = DDCBuffer[2];
        RetValueH = DDCBuffer[3];
        RetValueL = DDCBuffer[4];
        WordValue = (( WORD )DDCBuffer[3] << 8 ) | (( WORD )DDCBuffer[4] );
    }
    else if( CommandCode == GETVCPFeature )
    {
        CPCode = DDCBuffer[2];
    }
    else if( CommandCode == ModeChangeRequest )
    {
        CPCode = DDCBuffer[2];
        ValueH = DDCBuffer[3];
    }
    else if( CommandCode == GETPagedVCP )
    {
        PageValue = DDCBuffer[2];
        CPCode = DDCBuffer[3];
    }
    else if( CommandCode == SETPagedVCP )
    {
        PageValue = DDCBuffer[2];
        CPCode = DDCBuffer[3];
        RetValueH = DDCBuffer[4];
        RetValueL = DDCBuffer[5];
        WordValue = (( WORD )DDCBuffer[4] << 8 ) | (( WORD )DDCBuffer[5] );
    }
    else if( CommandCode == CapabilitiesRequest )        // 1-6
    {
        RetValueH = DDCBuffer[2];
        RetValueL = DDCBuffer[3];
        WordAddr = (( WORD )DDCBuffer[2] << 8 ) | (( WORD )DDCBuffer[3] );
        #if INPUT_TYPE!=INPUT_1A
        if( CURRENT_INPUT_IS_TMDS()
#if ENABLE_DP_INPUT
        || CURRENT_INPUT_IS_DISPLAYPORT()
#endif
            )
            WordValue = sizeof( DVI_CAP_VCP );
        else
        #endif
            WordValue = sizeof( CAP_VCP );
        if( WordAddr >= WordValue )
        {
            DDCBuffer[0] = 0x83;
            DDCBuffer[1] = 0xE3;
            return 3;
        }
#define CapLen CPCode
        CapLen = ( WordAddr + 32 >= WordValue ) ? ( WordValue - WordAddr ) : 32;
        DDCBuffer[0] = CapLen + 3 + 0x80;
        DDCBuffer[1] = 0xE3;
        DDCBuffer[2] = RetValueH;
        DDCBuffer[3] = RetValueL;
        for( ValueL = 4; ValueL < CapLen + 4; ValueL++ )
        {
            #if INPUT_TYPE!=INPUT_1A
            if( CURRENT_INPUT_IS_TMDS()
        #if ENABLE_DP_INPUT
        || CURRENT_INPUT_IS_DISPLAYPORT()
        #endif
        )
                DDCBuffer[ValueL] = DVI_CAP_VCP[WordAddr + ValueL - 4];
            else
            #endif
                DDCBuffer[ValueL] = CAP_VCP[WordAddr + ValueL - 4];
        }
#undef CapLen
        return DDCBuffer[0] &~0x80;
    }
    else if( CommandCode == EnableApp )
    {
        DDCBuffer[0] = 0x80;
        return 0; //0xFE;
    }
    else if( CommandCode == AppTest )
    {
        DDCBuffer[0] = 0x80;
        return 0; //0xFD;
    }
    else if( CommandCode == GetTimingRequest )
    {
        DDCBuffer[0] = 0x06;
        DDCBuffer[1] = 0x4E;
        DDCBuffer[2] = SyncPolarity( SrcFlags );
        if( UnsupportedModeFlag )
            DDCBuffer[2] |= 0x80;
        if( !InputTimingStableFlag )
            DDCBuffer[2] |= 0x40;
        WordValue = HFreq( SrcHPeriod ) * 10;
        DDCBuffer[3] = WordValue >> 8;
        DDCBuffer[4] = WordValue & 0x00FF;
        WordValue = VFreq( WordValue, SrcVTotal );
        DDCBuffer[5] = WordValue >> 8;
        DDCBuffer[6] = WordValue & 0x00FF;
        return 6;
    }
#if ENABLE_DeltaE
//*******************************************************************************************
//MSTART OnLine AutoColorTool COMMAND***************************************************************************
//*******************************************************************************************
	else if ( CommandCode == MStar_Command )
	{
        return ColorCalibrationHandler();
	}
#endif

#if ENABLE_DPS
     else if( CommandCode == MS_DPS_COMMAND)
    {
        if(DDCBuffer[2] == MS_SET_EnableFrameColor_DPS)
        {
            if(DDCBuffer[3]==0x01)
            {
                bRunToolFlag = 1;
                #if 0//CHIP_ID == CHIP_TSUMV
                msWriteBit(OSD1_02,1,BIT2);
                msWriteBit(OSD1_03,1,BIT3);
                msWriteByte(OSD2_E0,0x05);
                msWriteByte(OSD2_E8,0x80);
                msWriteByte(OSD2_E9,0x07);
                msWriteByte(OSD2_EA,0x38);
                msWriteByte(OSD2_EB,0x04);
                msWriteByte(OSD2_EC,0x3F);
                msWriteByte(OSD2_F4,0x66);
                msWriteByte(OSD2_F5,0x06);
                msWriteBit( SC0_04,0, BIT6 );
                #elif 0//CHIP_ID == CHIP_TSUMU
                msWriteBit(0x1C81, 0 , BIT7);
                msWriteByteMask(0x1C15,BIT3|BIT2, BIT3|BIT2);
                msWriteBit(0x1C16, 1 , BIT7);
                msWriteBit(0x1D04, 1 , BIT2);
                #else
                mStar_SetupFreeRunMode();
                drvOSD_FrameColorEnable(1);
                #endif
                return 3;
            }
            else
            {
                bRunToolFlag = 0;
                #if 0//CHIP_ID == CHIP_TSUMV
                msWriteByte(OSD2_E0,00);
                msWriteBit(OSD1_02,0,BIT2);
                msWriteBit(OSD1_03,0,BIT3);
                msWriteBit( SC0_04,1, BIT6 );
                #elif 0//CHIP_ID == CHIP_TSUMU
                msWriteByteMask(0x1C15,0, BIT3|BIT2);
                msWriteBit(0x1C16, 0 , BIT7);
                msWriteBit(0x1D04, 0 , BIT2);
                #else
                msWriteBit( SC0_02,0, BIT7 );
                drvOSD_FrameColorEnable(0);
                #endif
                return 3;
            }
        }
        else if(DDCBuffer[2] == MS_SET_FrameColor_DPS)
        {
                #if 0//CHIP_ID == CHIP_TSUMV
                msWriteByte(OSD2_F6,DDCBuffer[3]);
                msWriteByte(OSD2_F7,DDCBuffer[4]);
                msWriteByte(OSD2_F8,DDCBuffer[5]);
                #elif 0//CHIP_ID == CHIP_TSUMU
                msWriteByte(0x1D06,DDCBuffer[3]);
                msWriteByte(0x1D07,DDCBuffer[4]);
                msWriteByte(0x1D08,DDCBuffer[5]);
                #else
                drvOSD_FrameColorRGB(DDCBuffer[3],DDCBuffer[4],DDCBuffer[5]);
                #endif
                return 5;
        }
        else if(DDCBuffer[2] == MS_SET_AdjustPWM_DPS)
        {
            mStar_AdjustBrightness(DDCBuffer[3]);
        }
        else if(DDCBuffer[2] == MS_SET_Adjust_REAL_PWM_DPS)
        {
#if ENABLE_LED_CONTROLLER
            msLED_DimmingPWMAdjust(DDCBuffer[3]);
#else
            drvGPIO_SetBacklightDuty(BrightnessPWM,DDCBuffer[3]);
#endif
        }
    }
#endif

        //preliminary judge cmd finish, start detail process--------------------------------------------------

    if( CommandCode == GETVCPFeature || CommandCode == GETPagedVCP )
    {
        if( CommandCode == GETVCPFeature )
        {
            DDCBuffer[0] = 0x88;
            DDCBuffer[1] = 0x02;
            DDCBuffer[2] = 0x00;
            DDCBuffer[3] = CPCode;
            DDCBuffer[4] = 0x01;
            ValueH = 0x00;
            ValueL = 0xFF;
            RetValueH = 0x00;
            TPValue = 0x01;
        }
        else
        {
            DDCBuffer[0] = 0x89;
            DDCBuffer[1] = 0xC7;
            DDCBuffer[2] = 0x00;
            DDCBuffer[3] = PageValue;
            DDCBuffer[4] = CPCode;
            DDCBuffer[5] = 0x01;
            ValueH = 0x00;
            ValueL = 0xFF;
            RetValueH = 0x00;
            TPValue = 0x01;
        }

        if( CPCode == NewControl && PageValue == 0 )
        {
            RetValueL = 1;
            ValueL = 2;
        }
        else if( CPCode == ALL_Reset && PageValue == 0 )
        {
            RetValueL = 0;
            ValueL = 1;
        }

        else if( CPCode == BriCon_Reset && PageValue == 0 )
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == Geometry_Reset && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == Color_Reset && PageValue == 0 )
        {
            RetValueL = 0;
            ValueL = 1;
        }
	#if ENABLE_HDMI_DRR_MCCS
	else if(CPCode == HDMI_FREESYNC_VCP)
	{
		TPValue = 0x00;
		ValueH = 0x00;
       	ValueL = 0x01;
       	RetValueH = 0x00;
		RetValueL = UserprefFreeSyncMode;
	}
	#endif//wtx add AMD MCCS

        else if( CPCode == ADJ_Clock && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueH = 0;
            RetValueL = GetClockValue();
            ValueH = 0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Brightness && PageValue == 0 )
        {
            ValueL = MaxBrightnessValue - MinBrightnessValue;
            RetValueL = UserPrefBrightness - MinBrightnessValue;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Contrast && PageValue == 0 )
        {
            RetValueH = 0;
            RetValueL = GetContrastValue();
            ValueH = 0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == Select_Color_Preset && PageValue == 0 )
        {
            ValueL = 0x0D; //0x0B
            if( UserPrefColorTemp == CTEMP_SRGB )
                RetValueL = 0x01;
            else if( UserPrefColorTemp == CTEMP_6500K )
                RetValueL = 0x05;
            else if( UserPrefColorTemp == CTEMP_9300K )
                RetValueL = 0x08;
            else if( UserPrefColorTemp == CTEMP_USER )
                RetValueL = 0x0B;
            TPValue = 0x00;
        }
        else if(( CPCode == ADJ_Red_Gain ) && PageValue == 0 )
        {
            ValueL = 100;
            RetValueL = GetRedColorValue();
            TPValue = 0x00;
        }
        else if(( CPCode == ADJ_Green_Gain ) && PageValue == 0 )
        {
            ValueL = 100;
            RetValueL = GetGreenColorValue();
            TPValue = 0x00;
        }
        else if(( CPCode == ADJ_Blue_Gain ) && PageValue == 0 )
        {
            ValueL = 100;
            RetValueL = GetBlueColorValue();
            TPValue = 0x00;
        }
        else if( CPCode == Auto_Setup && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == Auto_Color && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == ADJ_HorizontalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            ValueH = 0; //(MaxHStart - MinHStart)>> 8;
            ValueL = 100; // (MaxHStart - MinHStart) & 0x00FF;
            RetValueH = ( MaxHStart - UserPrefHStart ) >> 8;
            RetValueL = ( MaxHStart - UserPrefHStart ) & 0x00FF;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_VerticalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueH = 0x00;
            RetValueL = GetVPositionValue();
            ValueH = 0x0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Phase && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueH = 0x00;
            RetValueL = GetFocusValue();
            ValueH = 0x0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == Select_InputSource )
        {
            ValueH = 0x00;
            ValueL = 0x04;
            if(CURRENT_INPUT_IS_VGA())
                RetValueL = 0x01;
            else
                RetValueH = 0x03;
            TPValue = 0x00;
        }
#if AudioFunc
        else if( CPCode == ADJ_SpeakerVolume && PageValue == 0 )
        {
            ValueH = 0;
            ValueL = 100;
            RetValueL = UserPrefVolume;
            RetValueH = 0;
            TPValue = 0x00;
        }
#endif
        else if( CPCode == ADJ_Red_Blacklevel && PageValue == 0 )
        {
            ValueH = 0x00;
            ValueL = 100;
            RetValueL = UserprefRedBlackLevel;
            TPValue = 0;
        }
        else if( CPCode == ADJ_Green_Blacklevel && PageValue == 0 )
        {
            ValueH = 0x00;
            ValueL = 100;
            RetValueL = UserprefGreenBlackLevel;
            TPValue = 0;
        }
        else if( CPCode == ADJ_Blue_Blacklevel && PageValue == 0 )
        {
            ValueH = 0x00;
            ValueL = 100;
            RetValueL = UserprefBlueBlackLevel;
            TPValue = 0;
        }
        else if( CPCode == H_Frequency && PageValue == 0 )
        {
            ValueH = 0xFF;
            ValueL = 0xFF;
            WordValue = HFreq( SrcHPeriod ) * 10;
            RetValueH = WordValue >> 8;
            RetValueL = WordValue & 0x00ff;
            TPValue = 0x01;
        }
        else if( CPCode == V_Frequency && PageValue == 0 )
        {
            ValueH = 0xFF;
            ValueL = 0xFF;
            WordValue = HFreq( SrcHPeriod ) * 10;
            WordValue = VFreq( WordValue, SrcVTotal );
            RetValueH = WordValue >> 8;
            RetValueL = WordValue & 0x00ff;
            TPValue = 0x01;
        }
        else if( CPCode == FlatPanellayout )
        {
            ValueL = 8;
            RetValueL = 1;
        }
        else if( CPCode == MonitorType && PageValue == 0 )
        {
            RetValueH = 0;
            RetValueL = 3;
            ValueH = 0x00;
            ValueL = 0x08;
            TPValue = 0x00;
        }
        else if( CPCode == DisplayControlType )
        {
            ValueH = 0xFF;
            ValueL = 0x16;
            RetValueH = 0;
            RetValueL = 5; // Mean Mstar
        }
        else if( CPCode == FWLevel )
        {
            ValueH = 0xFF;
            ValueL = 0xFF;
            RetValueL = 0;
            RetValueH = 0;
        }
        else if( CPCode == ADJ_Language && PageValue == 0 )
        {
#if 0//LANGUAGE_TYPE
            ValueL = LANG_Nums;
            if( UserPrefLanguage == LANG_English )
                RetValueL = 2;
            else if( UserPrefLanguage == LANG_Spanish )
                RetValueL = 10;
            else if( UserPrefLanguage == LANG_France )
                RetValueL = 3;
            else if( UserPrefLanguage == LANG_German )
                RetValueL = 4;
            else if( UserPrefLanguage == LANG_Italian )
                RetValueL = 5;
            else if( UserPrefLanguage == LANG_Portugues )
                RetValueL = 8;
            else if( UserPrefLanguage == LANG_Russia )
                RetValueL = 9;
            else if( UserPrefLanguage == LANG_SChina )
                RetValueL = 1;
            TPValue = 0x00;
#endif
        }
        else if( CPCode == VCPVersion && PageValue == 0 )
        {
            RetValueL = 1;
            RetValueH = 2;
            ValueH = 0xFF;
            ValueL = 0xFF;
        }
        else
        {
            DDCBuffer[2] = 0x01;
            RetValueL = 0x00;
            ValueH = 0xFF;
            ValueL = 0xFF;
        }

        if( CommandCode == GETVCPFeature )
        {
            DDCBuffer[4] = TPValue;
            DDCBuffer[5] = ValueH;
            DDCBuffer[6] = ValueL;
            DDCBuffer[7] = RetValueH;
            DDCBuffer[8] = RetValueL;
            return 8;
        }
        else
        {
            DDCBuffer[5] = TPValue;
            DDCBuffer[6] = ValueH;
            DDCBuffer[7] = ValueL;
            DDCBuffer[8] = RetValueH;
            DDCBuffer[9] = RetValueL;
            return 9;
        }

    }

    else if( CommandCode == SETVCPFeature || CommandCode == SETPagedVCP )
    {
        DDCBuffer[2] = 0x00; // Result Code
        ValueH = 0x00;
        ValueL = 0xFF;
        TPValue = 0x01;
        if( CPCode == NewControl && PageValue == 0 )
        {
            ValueL = 2;
        }
        else if( CPCode == ALL_Reset && PageValue == 0 )
        {
            if( WordValue != 0 )
            {
                ResetAllSetting();
                // wait for checking
                //msWriteByte( BK0_2B, 0x80 );
                //msWriteByte( BK0_2C, 0x80 );
                //msWriteByte( BK0_2D, 0x80 );
            }
            ValueL = 1;
        }
        else if( CPCode == BriCon_Reset && PageValue == 0 )
        {
            if( WordValue != 0 )
            {
                UserPrefBrightness = DefBrightness;
                UserPrefContrast = DefContrast;

                UserPrefBrightnessUser = UserPrefBrightness;
                UserPrefContrastUser = UserPrefContrast;

                UserPrefBrightnessCool1 = UserPrefBrightness;
                UserPrefContrastCool1 = UserPrefContrast;

                UserPrefBrightnessWarm1 = UserPrefBrightness;
                UserPrefContrastWarm1 = UserPrefContrast;

                UserPrefBrightnessNormal = UserPrefBrightness;
                UserPrefContrastNormal = UserPrefContrast;

                UserPrefBrightnessSRGB = UserPrefBrightness;
                UserPrefContrastSRGB = UserPrefContrast;


                mStar_AdjustBrightness( UserPrefBrightness );
                mStar_AdjustContrast( UserPrefContrast );
            }
            ValueL = 1;
        }
        else if( CPCode == Geometry_Reset && PageValue == 0 && INPUT_IS_VGA(rxInputPort))
        {
            if( WordValue != 0 && CURRENT_INPUT_IS_VGA())
                ResetGeometry();
            ValueL = 1;
            TPValue = 0x00;
        }
        else if( CPCode == Color_Reset && PageValue == 0 )
        {
            UserPrefRedColorUser = DefRedColor;
            UserPrefGreenColorUser = DefGreenColor;
            UserPrefBlueColorUser = DefBlueColor;
            UserprefRedBlackLevel = 50;
            UserprefGreenBlackLevel = 50;
            UserprefBlueBlackLevel = 50;
            mStar_AdjustUserPrefBlacklevel( UserprefRedBlackLevel, UserprefGreenBlackLevel, UserprefBlueBlackLevel );
            UserPrefColorTemp = CTEMP_5400K; //CTEMP_9300K 20051115 wmz
            if( WordValue != 0 )
                SetColorTemp();
            ValueL = 1;
        }
        else if( CPCode == ADJ_Clock && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            {
                UserPrefHTotal = (( WordValue * ( MaxClock - MinClock ) + 50 ) / 0x64 + MinClock );
                drvADC_AdjustHTotal( UserPrefHTotal );
            }
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Brightness && PageValue == 0 )
        {
           if(!UserPrefDcrMode&&(UserPrefECOMode==ECO_Standard))  //111108 add B/C adjustment condition - ID40189
            {
                UserPrefBrightness = RetValueL + MinBrightnessValue;
                if( UserPrefColorTemp == CTEMP_USER )
                {
                    UserPrefBrightnessUser = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_9300K )
                {
                    UserPrefBrightnessCool1 = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_5400K )
                {
                    UserPrefBrightnessWarm1 = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_6500K )
                {
                    UserPrefBrightnessNormal = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_SRGB )
                {
                    UserPrefBrightnessSRGB = UserPrefBrightness;
                }
                mStar_AdjustBrightness( UserPrefBrightness );
            }
            ValueL = MaxBrightnessValue - MinBrightnessValue;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Contrast && PageValue == 0 )
        {
            //Osd_Hide();
            //if (RetValueL <= (MaxContrastValue - MinContrastValue))
            if(!UserPrefDcrMode&&(UserPrefECOMode==ECO_Standard))  //111108 add B/C adjustment condition - ID40189
            {
                UserPrefContrast = RetValueL ;
                if( UserPrefColorTemp == CTEMP_USER )
                {
                    UserPrefContrastUser = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_9300K )
                {
                    UserPrefContrastCool1 = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_5400K )
                {
                    UserPrefContrastWarm1 = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_6500K )
                {
                    UserPrefContrastNormal = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_SRGB )
                {
                    UserPrefContrastSRGB = UserPrefContrast;
                }
                mStar_AdjustContrast( UserPrefContrast );
            }
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == Select_Color_Preset && PageValue == 0 )
        {
            if( RetValueL == 0x01 )
                UserPrefColorTemp = CTEMP_SRGB;
            else if( RetValueL == 0x05 )
                UserPrefColorTemp = CTEMP_6500K;
            else if( RetValueL == 0x06 )
                UserPrefColorTemp = CTEMP_6500K;
            else if( RetValueL == 0x08 )
                UserPrefColorTemp = CTEMP_9300K;
            else if( RetValueL == 0x0B )
                UserPrefColorTemp = CTEMP_USER;
            SetColorTemp();//Setcolortemp();
            ValueL = 0x0D; //0x0B
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Red_Gain && PageValue == 0 )
        {
            if( UserPrefColorTemp == CTEMP_USER )
            {
                //if(RetValueL<=20)
                // RetValueL=20;
                UserPrefRedColorUser = UserPrefRedColor = ((( DWORD )RetValueL * ( MaxColorValue - MinColorValue ) ) / 100 + MinColorValue );
                //UserPrefRedColorUser=UserPrefRedColor = RetValueL*MaxColorValue/100;
                mStar_AdjustRedColor( UserPrefRedColor, UserPrefContrast );
            }
            ValueL = 100;//(MaxColorValue - MinColorValue);
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Green_Gain && PageValue == 0 )
        {
            if( UserPrefColorTemp == CTEMP_USER )
            {
                //if(RetValueL<=20)
                // RetValueL=20;
                UserPrefGreenColorUser = UserPrefGreenColor = ((( DWORD )RetValueL * ( MaxColorValue - MinColorValue ) ) / 100 + MinColorValue );
                // UserPrefGreenColorUser = UserPrefGreenColor = RetValueL*MaxColorValue/100 ;
                mStar_AdjustGreenColor( UserPrefGreenColor, UserPrefContrast );
            }
            ValueL = 100;//(MaxColorValue - MinColorValue);
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Blue_Gain && PageValue == 0 )
        {
            if( UserPrefColorTemp == CTEMP_USER )
            {
                //if(RetValueL<=20)
                //RetValueL=20;
                UserPrefBlueColorUser = UserPrefBlueColor = ((( DWORD )RetValueL * ( MaxColorValue - MinColorValue ) ) / 100 + MinColorValue );
                //UserPrefBlueColorUser = UserPrefBlueColor = RetValueL*MaxColorValue/100;
                mStar_AdjustBlueColor( UserPrefBlueColor, UserPrefContrast );
            }
            ValueL = 100;//(MaxColorValue - MinColorValue);
            TPValue = 0x00;
        }
        else if( CPCode == Auto_Setup && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            if( WordValue != 0 )
                AutoConfig();
            ValueL = 1;
        }
        else if( CPCode == Auto_Color && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            if( WordValue != 0 )
                mStar_AutoColor();
            ValueL = 1;
        }
        else if( CPCode == ADJ_HorizontalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            {
                UserPrefHStart = MaxHStart - WordValue;
                mStar_AdjustHPosition( UserPrefHStart );
            }
            ValueH = ( MaxHStart - MinHStart ) >> 8;
            ValueL = ( MaxHStart - MinHStart ) & 0x00FF;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_VerticalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            UserPrefVStart = ( WORD )WordValue * ( MaxVStart - MinVStart ) / 100 + MinVStart;
            mStar_AdjustVPosition( UserPrefVStart );
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Phase && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            {
                UserPrefPhase = ( WORD )WordValue * (MAX_PHASE_VALUE+1) / 100 + 0.5;
                if( UserPrefPhase > MAX_PHASE_VALUE )
                    UserPrefPhase = MAX_PHASE_VALUE;
                drvADC_SetPhaseCode( UserPrefPhase );
            }
            TPValue = 0x00;
        }
        else if( CPCode == DDCCI_Settings && PageValue == 0 )
        {
            if(CURRENT_INPUT_IS_VGA())
            {
            #if USEFLASH
                Set_FlashForceSaveModeFlag();
            #else
                SaveModeSetting();
            #endif
            }
            #if USEFLASH
            Set_FlashForceSaveMonitorFlag();
            #else
            SaveMonitorSetting();
            #endif
            TPValue = 0x00;
        }
        else if( CPCode == Select_InputSource && PageValue == 0 )
        {
            if( RetValueL == 0x01 || RetValueL == 0x02 )
            {
 				UserPrefInputType=Input_VGA;
				ChangeSource();
             }

            if( RetValueL == 0x03 || RetValueL == 0x04 )
            {
                #if INPUT_TYPE!=INPUT_1A
				UserPrefInputType=Input_Digital;
				ChangeSource();
				#endif
            }

            TPValue = 0x00;
        }
#if AudioFunc
        else if( CPCode == ADJ_SpeakerVolume && PageValue == 0 )
        {
            UserPrefVolume = RetValueL;
            mStar_AdjustVolume( UserPrefVolume );
            TPValue = 0x00;
        }
#endif
        else if(( CPCode == ADJ_Red_Blacklevel || CPCode == ADJ_Green_Blacklevel || CPCode == ADJ_Blue_Blacklevel ) && PageValue == 0 )
        {
            if( CPCode == ADJ_Red_Blacklevel )
                UserprefRedBlackLevel = RetValueL;
            else if( CPCode == ADJ_Green_Blacklevel )
                UserprefGreenBlackLevel = RetValueL;
            else if( CPCode == ADJ_Blue_Blacklevel )
                UserprefBlueBlackLevel = RetValueL;
            mStar_AdjustUserPrefBlacklevel( UserprefRedBlackLevel, UserprefGreenBlackLevel, UserprefBlueBlackLevel );
            //SaveMonitorSetting();
        }
        else if( CPCode == ADJ_Language && PageValue == 0 )
        {
#if 0//LANGUAGE_TYPE
            if( RetValueL == 2 )
                UserPrefLanguage = LANG_English;
            else if( RetValueL == 10 )
                UserPrefLanguage = LANG_Spanish;
            else if( RetValueL == 3 )
                UserPrefLanguage = LANG_France;
            else if( RetValueL == 4 )
                UserPrefLanguage = LANG_German;
            else if( RetValueL == 5 )
                UserPrefLanguage = LANG_Italian;
            else if( RetValueL == 8 )
                UserPrefLanguage = LANG_Portugues;
            else if( RetValueL == 9 )
                UserPrefLanguage = LANG_Russia;
            else if( RetValueL == 1 )
                UserPrefLanguage = LANG_SChina;
            ValueL = LANG_Nums;
            TPValue = 0x00;
#endif
        }
        else if( CPCode == SyncType && PageValue == 0 )
        {
            ValueL = 3;
            RetValueL = 0;
        }
        else
        {
            DDCBuffer[2] = 0x01;
        }

        if( CommandCode == SETVCPFeature )
        {
                return 0;
        }
        else
        {
            DDCBuffer[0] = 0x89;
            DDCBuffer[1] = 0xC7;
            DDCBuffer[3] = PageValue;
            DDCBuffer[4] = CPCode;
            DDCBuffer[5] = TPValue;
            DDCBuffer[6] = ValueH;
            DDCBuffer[7] = ValueL;
            DDCBuffer[8] = RetValueH;
            DDCBuffer[9] = RetValueL;
            return 9;
        }
    }
    else if( CommandCode == SaveCurrentSetting )
    {
    #if USEFLASH
        Set_FlashForceSaveMonitorFlag();
        Set_FlashForceSaveModeFlag();
    #else
        SaveMonitorSetting();
        SaveModeSetting();
    #endif
        return 0;
    }
    else if( CommandCode == GETVCPPage )
    {
        DDCBuffer[0] = 0x83;
        DDCBuffer[1] = 0xC5;
        DDCBuffer[2] = 0;
        DDCBuffer[3] = 3;
        return 3;
    }
    DDCBuffer[0] = 0x80;
    return 0;
}
