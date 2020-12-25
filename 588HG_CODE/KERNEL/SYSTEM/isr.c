#include "types.h"
#include "board.h"
#include "global.h"
#include "power.h"
#include "debug.h"
#include "debugDef.h"
#include "Common.h"
#include "ddc2bi.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Userpref.h"
#include "gpio_def.h"
#include "drvadc.h"
#include "mcu.h"
//#include "halRwreg.h"
 #if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif
#if ENABLE_EDP_OUTPUT
#include "DPTxApp.h"
#endif
#if TimerComboKey
#include "keypad.h"
#endif
#if ENABLE_HDMI||ENABLE_FREESYNC
#include "msHDMI.h"
#endif
#if MS_DAC
#include "drvdac.h"
#endif
#if ENABLE_LED_CONTROLLER
#include "LedControl.h"
#endif
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
#if ENABLE_DSC
#include "msDSC_Setting.h"
#endif
#if ENABLE_IR_CONTROL
#include"Ir.h"
#include "IRfunc.h"
#endif

#if 0//(ENABLE_MHL)
#include "mapi_mhl.h"
#if CHIP_ID == CHIP_TSUM2
extern WORD xdata g_u16S2PatchCnt;
extern bit g_u16S2PatchCntFlag;
extern WORD xdata g_u16MHLHandlerCnt;
extern bit bMHLHandlerCntFlag;
#endif
#endif
#if ENABLE_FREESYNC
extern xdata Bool bHDMIFreesyncChk;
#endif
extern BYTE xdata u8DCRStepTransCnt; // 081205 dcr test

//#define ReSyncTick          255//54 // 0.2 Second for resynchorize dubug process
#if EXT_TIMER0_1MS || (ENABLE_MENULOAD && ML_USE_INT)
#define EnableExt0Interrupt     1
#define EnableTime0Interrupt        0
#elif ENABLE_SW_DOUBLE_BUFFER
#define EnableExt0Interrupt     1
#define EnableTime0Interrupt        1
#else
#define EnableExt0Interrupt     1
#define EnableTime0Interrupt        (!EXT_TIMER0_1MS)
#endif

#if UseINT
#define EnableExt1Interrupt     1
#define EnableSerialPortInterrupt   1 // 0
#else
#define EnableExt1Interrupt     0
#define EnableSerialPortInterrupt   1
#endif

//#define EnableTime1Interrupt        1
//#define EnableTime2Interrupt        0
#if IR_FUNC_EN	//20130824

bit bIrIoStatusFlag = 0;
bit bIrHeadFlag = 0;
bit bIrHoldFlag = 0;
bit bIrEndFlag = 0;
bit bIrPreLevelFlag = 0;	// ???¡ã?a??¦Ì???¡Á¡ä¨¬?;


BYTE idata byIrHighLevelCnt = 0;
BYTE idata byIrLowLevelCnt = 0;
BYTE idata byIrLowLevel_HighLevelCntTotal = 0;
BYTE idata byIrDataCnt = 32; // 32??bit;

unIrDataType idata unIrData;
WORD idata g_IRRawdata[2];

#endif


#if EXT_TIMER0_1MS
static void ms1MsHandler(void); // Jonson 20110713
#endif
#if CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9|| CHIP_ID == CHIP_TSUMF
#if ENABLE_SW_DOUBLE_BUFFER
extern XDATA StuSWDBCtrl SWDBCtrl;

void IsrML(void)
{
    SWDBCtrl.ucSWDB_Counter = 0;
    SWDBCtrl.ucSWDB_OnFire = 0;
    SWDBCtrl.ucSWDB_Num = 1;
    msRegs[REG_3C25] &=0x3F;
}
#endif

#if ENBLAE_BDMA_INT
void IsrBDMA(void)
{
    if(_bit2_(msRegs[REG_3302]))
    {
        msRegs[REG_3302] = BIT2|BIT3;
        msRegs[REG_12C4] = 0x00;
        msRegs[REG_2080] = 0x27;
    }
}
#endif
#endif



#if UseINT
void MuteVideoAndAudio(void)
{
#if ENABLE_HDMI
    if( gScInfo.InputTmdsType == TMDS_HDMI )
        hw_SetMute(); // for changed mode have "po-po"
#endif
#if AudioFunc //111027 Rick modified for avoid bo sound while timingchange - B39440
#if ENABLE_DP_AUDIO
if((CURRENT_INPUT_IS_DISPLAYPORT())&&(UserPrefAudioSource == AUDIO_DIGITAL))
    hw_SetMute();
#endif
#endif

#if ENABLE_LED_CONTROLLER
    MEM_MSWRITE_BYTE(REG_0A80,0x00);
    MEM_MSWRITE_BYTE(REG_0A81,0x00);
    MEM_MSWRITE_BYTE(REG_0A68,0x00);
    MEM_MSWRITE_BYTE(REG_0A69,0x00);
    MEM_MSWRITE_BYTE(REG_0AD0,0x00);
    MEM_MSWRITE_BYTE(REG_0AD1,0x00);


    MEM_MSWRITE_BYTE(REG_0A4E,0xFF);
    MEM_MSWRITE_BYTE(REG_0A4F,0xFF);
    MEM_MSWRITE_BYTE(REG_0A52,0xFF);
    MEM_MSWRITE_BYTE(REG_0A53,0xFF);
    MEM_MSWRITE_BYTE(REG_0AC0,0xFF);
    MEM_MSWRITE_BYTE(REG_0AC1,0xFF);


#else
  //  hw_ClrBlacklit();
#endif

    Set_InputTimingChangeFlag();
    SrcFlags |= SyncLoss;
}

void IsrModeChangeCheck(void)
{
    MEM_SCWRITE_BYTE(0, 0); //bk0
#if ENABLE_R2_2DTO3D
    if(INT_STATUS_A_ISR(BIT2))  //Vsync INT
    {
        MEM_SCWRITE_BYTE(SC0_CC,MEM_SCREAD_BYTE(SC0_CC)&(~BIT2));  //clear Vsync INT final status
        MEM_MSWRITE_BYTE_MASK(REG_2B7C,BIT0,BIT0);  //51 INT R2
    }
#endif
    if (INT_STATUS_A_ISR((MEM_SCREAD_BYTE(SC0_CE))&(BIT4|BIT5|BIT6|BIT7)) || (INT_STATUS_B_ISR((MEM_SCREAD_BYTE(SC0_CF))&(BIT2))))//((MEM_SCREAD_BYTE(SC0_CC)&(BIT4|BIT5|BIT6|BIT7)) || (MEM_SCREAD_BYTE(SC0_CD)&BIT2))
    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        if(INT_STATUS_A_ISR(BIT7))
            SetTimingChangeFirstPoint(TimingChange_ISR_Vchange);
        else if(INT_STATUS_A_ISR(BIT6))
            SetTimingChangeFirstPoint(TimingChange_ISR_Hchange);
        else if(INT_STATUS_A_ISR(BIT5))
            SetTimingChangeFirstPoint(TimingChange_ISR_Vdisappear);
        else if(INT_STATUS_A_ISR(BIT4))
            SetTimingChangeFirstPoint(TimingChange_ISR_Hdisappear);
        else if(INT_STATUS_B_ISR(BIT2))
            SetTimingChangeFirstPoint(TimingChange_ISR_CLKchange);
#endif
        INT_SCALER_A_ISR(0);//MEM_SCWRITE_BYTE(SC0_CE, 0); //disable INT
        INT_SCALER_B_ISR(0);//MEM_SCWRITE_BYTE(SC0_CF, 0); //disable INT
        INT_STATUS_CLEAR_ISR();

#if ENABLE_SW_RESET_HDCP_FSM
        HDCP_RESET_ISR(TRUE);
        HDCP_RESET_ISR(FALSE);
#endif

        if (!SyncLossState())
        {
            MuteVideoAndAudio();
        }
    }
#if (ENABLE_FREESYNC||ENABLE_DPS||ENABLE_DSC ||(ENABLE_RTE && CHIP_ID==CHIP_TSUMF))
    else if (INT_STATUS_A_ISR((MEM_SCREAD_BYTE(SC0_CE))&BIT3))
    {
		scWriteByte ( SC0_CE, scReadByte ( SC0_CE ) & ( ~BIT3 ) ); //scRegs[0xCE] = scRegs[0xCE]&(~BIT3);
		scWriteByte ( SC0_CC, scReadByte ( SC0_CC ) & ( ~BIT3 ) ); //scRegs[0xCC] = scRegs[0xCC]&(~BIT3);
#if CHIP_ID >= CHIP_TSUMC
    	g_bOutputVsyncIsrFlag = 1;
#endif

#if ENABLE_DPS
        msDPS_Interrupt();
#endif
#if ENABLE_DSC
        msDSC_Interrupt();
#endif
#if ENABLE_RTE && CHIP_ID==CHIP_TSUMF
        msOD_Histogram_Interrupt();
#endif
#if ENABLE_FREESYNC
        if(IS_HDMI_FREESYNC_ISR())
        {
            if(bHDMIFreesyncChk)
                MEM_SCWRITE_BYTE(SC0_E8,(MEM_SCREAD_BYTE(SC0_E8)&(~BIT1))|BIT0);    //Enable force P mode
            bHDMIFreesyncChk = FALSE;
        }
#endif
    }
#endif

}
#endif

#if EnableExt0Interrupt
void EXT0Interrupt( void )interrupt 0
{
    BYTE ucStatus;
    ucStatus = msRegs[REG_2B10];
  #if ENABLE_MENULOAD && ML_USE_INT
    if( ucStatus & _BIT5 )
    {
        extern BYTE xdata u8ML_Status;
        u8ML_Status=msRegs[REG_3C27]&(ML_STS_BUSY|ML_STS_TRIG_ERR|ML_STS_CMD_ERR|ML_STS_DONE);
        msRegs[REG_3C25]=msRegs[REG_3C25]&~(BIT7|BIT6); //disable ML
    }
  #endif

#if ENABLE_WATCH_DOG_INT
  if( ucStatus & _BIT1 )
  {
    u8WDT_Status=0x33;
  }
#endif

#if CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF
#if  ENABLE_SW_DOUBLE_BUFFER
    if(_bit5_(ucStatus))
        IsrML();
#endif
#endif

  #if EXT_TIMER0_1MS
    if( ucStatus & _BIT0 )
        ms1MsHandler();
  #endif

    msRegs[REG_2B08] = ucStatus;
    msRegs[REG_2B08] = 0x00;
    IE0 = 0;
}
#endif
#if EnableTime0Interrupt
//extern WORD ms_Counter;
//extern BYTE ReSyncCounter;
//extern BYTE Second;
//extern BYTE xdata OsdCounter;
//extern BYTE xdata PowerDownCounter;
void Time0Interrupt( void )interrupt 1
#else
static void ms1MsHandler(void)
#endif
{
    TH0 = g_ucTimer0_TH0;
    TL0 = g_ucTimer0_TL0;

    ms_Counter--;

#if ENABLE_USB_INPUT
    u32UsbCount++;
#endif

#if ENABLE_DEBUG
    if( u8ReSyncCounter )
    {
        u8ReSyncCounter--;
        if( u8ReSyncCounter == 0)
        {
            Clr_RxBusyFlag();
            SIORxIndex = 0;
        }
    }
#endif

    if( bDelayFlag )
        if( --u16DelayCounter == 0 )
            bDelayFlag = 0;

    if( bTimeOutCounterFlag )
        if( --u16TimeOutCounter == 0 )
            bTimeOutCounterFlag = 0;

#if ENABLE_DP_INPUT
#if((CHIP_ID==CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) ||(CHIP_ID==CHIP_TSUMD) || (CHIP_ID==CHIP_TSUM9)|| (CHIP_ID==CHIP_TSUMF))
    if( bDPRxStableTimeoutFlag )
        if( --u16DPRxStableTimeout == 0 )
            bDPRxStableTimeoutFlag = 0;
#endif
#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||CHIP_ID == CHIP_TSUMD
#if 0
    if( bAudio_CntFlag )
        if( --u16Audio_Cnt == 0 )
            bAudio_CntFlag = 0;

    if( bHDCP_CntFlag )
        if( --u16HDCP_Cnt == 0 )
            bHDCP_CntFlag = 0;
#endif
#endif


#if (CHIP_ID==CHIP_TSUM9 || CHIP_ID==CHIP_TSUMF ||CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||CHIP_ID == CHIP_TSUMD)
      if( bHDCP_CntFlag )
        if( --u16HDCP_Cnt == 0 )
            bHDCP_CntFlag = 0;
#endif

#if (CHIP_ID==CHIP_TSUM9 || CHIP_ID==CHIP_TSUMF)
    if(gDPInfo.bPacketTimer )
          gDPInfo.bPacketTimer --;
#endif

#if DPCTS
#if 1
    if( bCDR_HPDCntFlag )
        if( --u16CDR_HPDCnt == 0 )
            bCDR_HPDCntFlag = 0;
#else
        if(u16CDR_HPDCnt)
           u16CDR_HPDCnt--;
#endif
#if 1
    if( bSkew_HPDCntFlag )
        if( --u16Skew_HPDCnt == 0 )
            bSkew_HPDCntFlag = 0;
#else
        if(Skew_HPDCnt)
           Skew_HPDCnt--;
#endif
#if CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD
#if 1
if( bAudio_BWCntFlag )
    if( --u16Audio_BWCnt == 0 )
        bAudio_BWCntFlag = 0;
#else
    if(Audio_BWCnt)           //20130716
           Audio_BWCnt--;
#endif
#endif
#endif
#endif


#if 0//ENABLE_TIME_MEASUREMENT   //need enable this if test Timer0
    if( g_u16TimeMeasurementCounter )
        g_u16TimeMeasurementCounter--;
#endif
#if MS_DAC && (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUM9||CHIP_ID==CHIP_TSUMF)
#if InitialDACByISR
            if( bDAC_DePopCountFlag )
            {
                if( --w16DAC_DePopCounter == 0 )
                {
                    bDAC_DePopCountFlag = 0;
                    bDAC_DePopFlag = 1;
                }
            }
#endif
#endif

    if(!g_bMcuPMClock) // system runs at normal speed
    {
        if( u8DCRStepTransCnt )
            u8DCRStepTransCnt --;

        if( bPanelOffOnFlag ) //091013
            if( --u16PanelOffOnCounter == 0 )
                bPanelOffOnFlag = 0;

#if ENABLE_MHL
    #if CHIP_ID == CHIP_TSUM2
        if(bS2PatchCntFlag)
        {
            if(--g_u16S2PatchCnt == 0)
                bS2PatchCntFlag = 0;
        }

        if(bMHLHandlerCntFlag)
        {
            if(--g_u16MHLHandlerCnt == 0)
                bMHLHandlerCntFlag = 0;
        }
    #elif (CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
        mapi_mhl_CbusTimerInterrupt();
    #endif
#endif

#if ENABLE_LED_CONTROLLER
        if(bBoostCounterFlag)
        {
            if(--u16BoostCounter == 0)
                bBoostCounterFlag = 0;
        }

    #if ENABLE_LED_OPEN_SHORT_DEBOUNCE
        if(bLEDOpenShortFlag)
        {
            if(LEDOpenShortCounter)
                LEDOpenShortCounter--;

            if(LEDOpenShortCounter)
            {
                if(!bLEDOpenShortIntEnFlag && LEDOpenShortCounter <= LED_ENABLE_INTERRUPT_COUNT)
                    LEDStringEnableInterrupt(LEDOpenShortString);
            }
            else
            {
                bLEDOpenShortFlag = 0;
                LEDShutDownCounter = 0;
            }
        }
    #endif
#endif

    }

#if TimerComboKey
		if ((ms_Counter % 200) == 0)
		{
			if (bEnableComboKeyFlag)
			{
				if (ComboCounter<100)
				{
					if (++ComboCounter==22) 	// 243x200ms = 4.8S
					{
						bDoComboKeyFlag=1;
					}
				}
			}				
		}
  #endif

    if(( ms_Counter % 10 ) == 0 )
    {
        Set_ms10Flag();

        if(( ms_Counter % 50 ) == 0 )
        {
            Set_ms50Flag();

            if(( ms_Counter % 200 ) == 0 )
            {
#if _NEW_SOG_DET_
                if(SyncLossState())
                    NEW_SOG_DET_FLUSH();
#endif
            }

            if(( ms_Counter % 500 ) == 0 )
            {
                Set_ms500Flag();

                if( ms_Counter == 0 )        //
                {
                    #if ENABLE_FREESYNC
                    Set_ms1000Flag();
                    #endif
                    Set_SecondFlag();
                    ms_Counter = SystemTick;
                }
            }
        }

    }

    if(TF0)
    {
        TH0 = g_ucTimer0_TH0;
        TL0 = g_ucTimer0_TL0;
    }
    //msRegs[REG_022C]=msRegs[REG_022C]&(!BIT2);

    TF0 = 0; // timer 1 flag

}
//#endif
#if EnableExt1Interrupt

void EXT1Interrupt( void )interrupt 2
{
    //  EX1=0;

 BYTE ucBank = MEM_SCREAD_BYTE(0x00);       //110926 Rick add to store the current bank - Verify A036, A039

#if (ENABLE_DP_INPUT&& ((CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)))
    if((_bit2_(msRegs[REG_2B2A]))||(_bit3_(msRegs[REG_2B2A])))
    {
            DPISR_AUX0();
    }
#endif

#if (CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)

    if(_bit7_(msRegs[REG_2B29]))
    {
        TMDSISR();
		#if ENABLE_DP_INPUT
        DPISR();
		#endif
    }

	#if ENABLE_MHL
    if(_bit0_(msRegs[REG_2B2A]))
    {
        mapi_mhl_IsrHandler();
    }

	#endif

#endif

#if CHIP_ID == CHIP_TSUMU
#if ENABLE_DP_INPUT
    if(_bit6_(msRegs[REG_2B29])) { DPISR(); }   // read status
#endif
#if ENABLE_EDP_OUTPUT
    if(_bit7_(msRegs[REG_2B29])) { DPISR_TX(); }   // read status
#endif
#endif

#if ENABLE_LED_CONTROLLER
  #if ENABLE_LED_INT
  { extern void msLED_Interrupt(void);
    //BYTE  final_status;                 // use idata to speedup
    //final_status = msRegs[REG_2B28];    // read status
    //msRegs[REG_2B28] = final_status;    // clear status
    //msRegs[REG_2B28] = 0x00;
    if (INT_STATUS_LEDSARINT_ISR()) msLED_Interrupt();
  }
  #endif
#endif

    if( INT_STATUS_D2B_ISR() )
    {
        ISR_DDC2Bi();
    }

#if UseINT
    if(INT_STATUS_DISP_ISR())
    {
        IsrModeChangeCheck();
    }
#endif
    #if ENBLAE_BDMA_INT
#if CHIP_ID == CHIP_TSUM2
    if(_bit0_(msRegs[REG_2B2A]))
        IsrBDMA();
#elif CHIP_ID == CHIP_TSUMD
    if(_bit5_(msRegs[REG_2B2A]))
    {
        IsrBDMA();
    }
    #endif
#endif

    MEM_SCWRITE_BYTE(0x00, ucBank);
    IE1 = 0;
    //  EX1=1;
}

#endif

#if EnableTime1Interrupt
void Time1Interrupt( void )interrupt 3
{
	bIrIoStatusFlag = IR_DataKey;
	TF1=0;
	TH1=g_ucTimer1_TH0;
	TL1=g_ucTimer1_TL0;	
		
	if (bIrIoStatusFlag)
	{ // IR high level;
		bIrPreLevelFlag = 1;
		if (byIrHighLevelCnt < 250)       //  250*200us=50ms
		{
			++byIrHighLevelCnt;
		}
		else
		{	
			unIrData.dwIrData = 0;
			byIrDataCnt = 32;
			bIrHeadFlag = 0;
		}
	}
	else
	{ // IR low level;	
		if (bIrPreLevelFlag)
		{
			byIrLowLevel_HighLevelCntTotal = byIrLowLevelCnt;
		
			
			bIrPreLevelFlag = 0;

			
				if (!bIrHeadFlag)
				{ 
				
					if (byIrHighLevelCnt < 23)                      //  high max is 4.5ms    timer=180us--220us,4.5ms/200us=22
					{ 
				//		IRFLAG=IRFLAG|0x01;
						if ((byIrLowLevelCnt>30) && (byIrLowLevelCnt<46))       //low 9ms   timer=200us--300us
						{ 
					//		IRFLAG=IRFLAG|0x02;
							if ((byIrHighLevelCnt>12) && (byIrHighLevelCnt<23))         //high  4.5ms    timer=200us--300us
							{ 
								bIrHeadFlag = 1;
					//			IRFLAG=IRFLAG|0x04;
								
							}
							else
							{
								if ((byIrHighLevelCnt>6) && (byIrHighLevelCnt<12))    //high  repet 2.25ms   timer=200us--300us
								{ //ÖØ¸´Âë
									bIrHoldFlag = 1;
				//					IRFLAG=IRFLAG|0x80;
								}
							}
						}
					}
				}
				else
				{ 
					if (byIrDataCnt > 0)
					{ 
						--byIrDataCnt;
						unIrData.dwIrData = (unIrData.dwIrData<<1);

						byIrLowLevel_HighLevelCntTotal += byIrHighLevelCnt;
						if ((byIrLowLevel_HighLevelCntTotal>3) && (byIrLowLevel_HighLevelCntTotal<6))     //data 0  1.12ms   timer=200us--300us
						{ 
							unIrData.byData[3] &= 0xfe;////0
						}
						else if ((byIrLowLevel_HighLevelCntTotal>6) &&(byIrLowLevel_HighLevelCntTotal<12))   //data 1  2.25ms   timer=200us--300us
						{ 
							unIrData.byData[3] |= 0x01;/////1
						}
						
						
						if (byIrDataCnt == 0) 
						{ 
							bIrEndFlag = 1;
							bIrHeadFlag = 0;
							g_IRRawdata[0]=unIrData.iIrData[0];
							g_IRRawdata[1]=unIrData.iIrData[1];
						}
						
					}
				}
				byIrLowLevelCnt = 0;
				byIrHighLevelCnt = 0;
		}

		if (byIrLowLevelCnt < 75)     //max 9ms   15ms/200us=75
		{
			++byIrLowLevelCnt;
		}
	}

#if 0//1ms no use	
#if ENABLE_TIME_MEASUREMENT  //need enable this if test Timer1
    if(bTimeMeasurementFlag)
    {
        if(--g_u16TimeMeasurementCounter == 0)
            bTimeMeasurementFlag = 0;
    }
#endif

    g_u16MsCounter1--;
    if (g_u16MsCounter1==0) //
    {
        bSecondTimer1 = TRUE;
        g_u16MsCounter1=1000;
    }
#endif
}
#endif
#if EnableSerialPortInterrupt
void SerialPortInterrupt( void )interrupt 4
{
    BYTE sioData;

    if( RI )
    {
        RI = 0;
        sioData = SBUF;
        if( SIORxIndex < _UART_BUFFER_LENGTH_ )
        {
            SIORxBuffer[SIORxIndex++] = sioData;
            if(SIORxIndex < UART_CMD_LENGTH)
                u8ReSyncCounter = ReSyncTick;
            else
                u8ReSyncCounter = 0xFF;
        }
    }
    if( TI )
    {
        TI = 0;
        TI0_FLAG = 1;
    }
}
#endif
#if EnableTime2Interrupt
void Time2Interrupt( void )interrupt 5
{
    TF2 = 0;
}
#endif
#if UART1
void SerialPort1Interrupt (void)interrupt 16
{
    BYTE sioData;

    if(S1CON & RI1)
    {
        S1CON &= ~RI1;
        sioData = S1BUF;
        if( SIORxIndex < _UART_BUFFER_LENGTH_ )
        {
            u8ReSyncCounter = ReSyncTick;
            SIORxBuffer[SIORxIndex++] = sioData;
        }
    }

    if(S1CON & TI1)
    {
        S1CON &= ~TI1;
        TI0_FLAG = 1;
    }
}
#endif

void EXT3Interrupt( void )interrupt 10 // add for WDT interrupt
{

}

void EXT2Interrupt( void )interrupt 9 // add for timer interrupt
{

}

