#ifndef _MSHDMI_C_
#define _MSHDMI_C_
#include "types.h"
#include "board.h"
#if ENABLE_HDMI
#include "Global.h"
#include "Adjust.h"
#include "ms_Reg.h"
#include "ms_rwreg.h"
#include "msHDMI.h"
#include "Debug.h"
#include "Common.h"
#include "math.h"
#include "misc.h"
#include "GPIO_DEF.h"
#include "Mstar.h"
#if  ENABLE_HDMI_1_4
#include "msScaler.h"
#endif
#include "halRwreg.h"
extern void mStar_SetUserPref( void );

#define HDMI_DEBUG   1
#if ENABLE_DEBUG &&HDMI_DEBUG
#define HDMI_printData(str, value)   printData(str, value)
#define HDMI_printMsg(str)           printMsg(str)
#else
#define HDMI_printData(str, value)
#define HDMI_printMsg(str)
#endif

void mstar_HDMIInitialVariable( void )
{
    gScInfo.InputColor = INPUT_RGB;
    gScInfo.AVI_Colorimetry = YUV_Colorimetry_ITU601;
    gScInfo.InputSource = INPUT_TMDS_A;
    gScInfo.InputTmdsType = TMDS_DVI;
    gScInfo.ColorChanged = 0;
    gScInfo.ModeChange = 0;
    gScInfo.wAviPktLossCnt = 0;
    gScInfo.InputPacketStatus = 0;
#if ENABLE_HDMI_1_4
    gScInfo.NoVsPktCnt = 0;
#endif
    gScInfo.AudioOffDebunce = 0;
    gScInfo.InputPacketError = 0;
    gScInfo.InputVideoMode = 16; // check here
    gScInfo.InputAVIInfoReceived = 0;
    gScInfo.InputSPDInfoReceived = 0;
    gScInfo.OutputAudioState = HDMI_AUDIO_OFF; //Mute
    gScInfo.OutputDataState = HDMI_DATA_OUTPUT_OFF;
#if ENABLE_HDMI_SW_CTS_STABLE
    gScInfo.InputCTSValue = 0;
    gScInfo.OutputAudioCnt = AudioDeBounce;
#endif
    gScInfo.cs0 = 0;
#if  0
    CSpcPollingCounter = 0;
#endif
}

void mstar_HDMIInitialAudio( void )
{
    drv_mstar_HDMIInitialAudio();
}

void mstar_HDMIHandler( void )
{
    mstar_MonitorHDMI();
    mstar_HDMIHandleColor();
    mstar_HDMIHandleMode();
}


void mstar_HDMIRst( BYTE rst )
{
    drv_mstar_HDMIRst(rst);
}

void mstar_HDMIHandleMode( void )
{
    //BYTE    temp;
    if( gScInfo.ModeChange )
    {
        gScInfo.ModeChange = 0;
        Set_InputTimingChangeFlag();
        HDMI_printData( "TMDS mode is=%d", gScInfo.InputTmdsType );
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        SetTimingChangeFirstPoint(TimingChange_HDMIModeChange);
#endif
    }
}
#if  0
void SetMainWinVideoDomain( BYTE Domain ) //void SetMainWinColorControl(BYTE ucControl)
{
    BYTE XDATA ucBank, ucReg;

    ucBank = msReadByte( BK0_00 );
    msWriteByte( BK0_00, REGBANK3 );
    ucReg = msReadByte( BK3_71 );
    if( Domain )
        ucReg |= BIT3;
    else
        ucReg &= ~BIT3;

    msWriteByte( BK0_00, REGBANK0 );
    if( Domain )
    {
        mStar_WaitForDataBlanking();
        msWriteByte( BK0_2B, 0x70 );
        msWriteByte( BK0_2C, 0x70 );
        msWriteByte( BK0_2D, 0x70 );
        mStar_WriteByteMask( BK0_02, 0x00, BIT3 );  //msWriteByte( BK0_02, msReadByte( BK0_02 )&~BIT3 );
        msWriteByte( BK0_00, REGBANK3 );
        msWriteByte( BK3_71, ucReg | BIT2 );
        msWriteByte( BK3_70, 0x15 );
        msWriteByte( BK3_71, ucReg );
        msWriteByte( BK0_00, REGBANK0 );
    }
    else
    {
        mStar_WaitForDataBlanking();
        msWriteByte( BK0_2B, 0x80 );
        msWriteByte( BK0_2C, 0x80 );
        msWriteByte( BK0_2D, 0x80 );
        msWriteByte( BK0_00, REGBANK3 );
        msWriteByte( BK3_71, ucReg | BIT2 );
        msWriteByte( BK3_70, 0x10 );
        msWriteByte( BK3_71, ucReg );
        msWriteByte( BK0_00, REGBANK0 );
    }
    msWriteByte( BK0_00, ucBank );
}
#endif
void mstar_HDMIHandleColor( void )
{
    if( gScInfo.ColorChanged )
    {
        gScInfo.ColorChanged = 0;
        mStar_SetUserPref();
        HDMI_printData( "Handle HDMI CSC%d", gScInfo.InputColor );
    }
}
#if  0
void mstar_HDMIResetColorSpace( void )
{
    BYTE temp, reg_bank;


    HDMI_printMsg( "HDMIResetColorSpace ");

    reg_bank = msReadByte( BK0_00 );
    msWriteByte( BK0_00, REGBANK0 );

    if( msReadByte( BK0_5C )&BIT3 )
    {
        temp = msReadByte( BK0_5A ) | BIT7 | BIT6;
        msWriteByte( BK0_5A, temp );
    }
    temp = msReadByte( BK0_02 ) & ( ~BIT3 );
    msWriteByte( BK0_02, temp );
    SetMainWinVideoDomain( INPUT_RGB );     //MainWin Color Space Change YUV -> RGB
    msWriteByte( BK0_00, reg_bank );
}
#endif
void mstar_MonitorHDMI( void )
{
    BYTE temp, curr_color;

    if( HdmiPollingCounter == 0 )
    {
        HdmiPollingCounter = 20;

        if(CURRENT_INPUT_IS_TMDS())//( SrcInputType == Input_Digital || SrcInputType == Input_Digital2 )
        {
            if( SrcFlags&( SyncLoss | bUnsupportMode ) ) // unsupport mode

            {
                if( gScInfo.OutputAudioState != HDMI_AUDIO_OFF )
                    mstar_HDMIAudioMute( HDMI_AUDIO_OFF ); // mute audio
            }
            //else if( InputTimingStableFlag )
            else if(DVI_DE_STABLE())
            {

                temp = mstar_HDMITmdsGetType();

                if( temp != gScInfo.InputTmdsType )
                {
                    gScInfo.InputTmdsType = temp;
                    gScInfo.InputPacketStatus = 0;
                   //gScInfo.ModeChange = 1;
                    return;
                }

                // 120604 coding modified for reading packet status is unnecessary for DVI mode.
                if(temp == TMDS_DVI)
                {
                    mstar_HDMIRst(Reset_HDMIStatus);
                    return;
                }

            mstar_HDMIGetPktStatus();
            mstar_HDMICheckAudio();
            mstar_HDMIIsAudioPCM(); //temp solution to eliminate warning
            mstar_HDMIAutoEQProc();
				#if (ENABLE_CTS_INT && (ENABLE_CTSN_FILTER || ENABLE_AUDIO_AUTO_MUTE || ENABLE_AUDIO_AUTO_FADING))
                    CheckAudioErrorStatus();
                #endif
          #if ENABLE_HDMI_1_4
            msHDMI_Check_3D_Mode_Chg(1);
          #endif

                if( gScInfo.InputPacketStatus & BIT3 )
                {
                    gScInfo.wAviPktLossCnt = 0;
                    curr_color = mstar_HDMIPacketColor();

                    if( curr_color != gScInfo.InputColor )
                    {
                        gScInfo.InputColor = curr_color;
                        gScInfo.ColorChanged = 1;
                        if(curr_color == INPUT_RGB)
                    	{
                            InputColorFormat = INPUTCOLOR_RGB;
                            UserPrefInputColorFormat = INPUTCOLOR_RGB;
                    	}
                        else
                    	{
                            InputColorFormat = INPUTCOLOR_YUV;
                            UserPrefInputColorFormat = INPUTCOLOR_YUV;
                    	}
					
                    }
                }
                else
                {
                    if( gScInfo.wAviPktLossCnt >= AviPktLossBound )
                    {
                        if( gScInfo.InputColor != INPUT_RGB )
                        {
                            gScInfo.InputColor = INPUT_RGB;
                            InputColorFormat = INPUTCOLOR_RGB;
                            UserPrefInputColorFormat = INPUTCOLOR_RGB;
							
                            mstar_HDMIRst(Reset_Color);
                            gScInfo.ColorChanged = 1;
                        }
                    }
                }

            }
        }
    }
}

void mstar_HDMICheckAudio( void )
{
    BYTE temp;
#if ENABLE_HDMI_SW_AUDCLK_CHECK
    BYTE ucCurInFreq;
#endif

    if( (gScInfo.InputPacketError &( BIT5 | BIT4 )) )//|| gScInfo.cs0 & BIT6 ) //audio sample error
    {
        temp = HDMI_AUDIO_OFF;
    }
    else if(( gScInfo.InputPacketStatus&( BIT6 | BIT5 ) ) == ( BIT6 | BIT5 ) )
    {
        if( UserPrefVolume == 0 )
            temp = HDMI_AUDIO_OFF;
        else
        {
            if( gScInfo.OutputDataState == HDMI_DATA_OUTPUT_ON )
                temp = HDMI_AUDIO_ON;
            else
                temp = HDMI_AUDIO_OFF;
        }

#if ENABLE_HDMI_SW_CTS_STABLE
        if( gScInfo.OutputAudioCnt ) // && gScInfo.OutputAudioState==HDMI_AUDIO_ON)    // Check CTS Value Stable
            temp = HDMI_AUDIO_OFF;
#endif
    }

    else
        temp = HDMI_AUDIO_OFF;

    if( gScInfo.OutputAudioState == HDMI_AUDIO_ON && temp == HDMI_AUDIO_OFF )
    {
        if( ++gScInfo.AudioOffDebunce < 20 )
            return ;
    }
    else
        gScInfo.AudioOffDebunce = 0;
#if ENABLE_HDMI_SW_AUDCLK_CHECK// 20080403 audio clock over spec
    ucCurInFreq = mstar_HDMICheckAudioFreq();
    if( temp == HDMI_AUDIO_ON )
    {
        if(( ucCurInFreq > AUD_FREQ_ERROR ) )
        {
            if( ucCurInFreq != gScInfo.CurOutAudFreq )
            {
                if( ++gScInfo.FreqStableDebunce > 1 ) // > "1" this value can be adjust
                {
                    gScInfo.CurOutAudFreq = ucCurInFreq;
                    mstar_SetAudioClockLock( TRUE ); // chip w
                }
            }
            else
                gScInfo.FreqStableDebunce = 0;
        }
        else
        {
            if( ucCurInFreq != gScInfo.CurOutAudFreq )
            {
                if( ++gScInfo.FreqStableDebunce > 3 )
                {
                    HDMI_printData( "--Input Audio Freq Error[%d]--", gScInfo.CurOutAudFreq );
                    gScInfo.CurOutAudFreq = ucCurInFreq;
                    if( temp == HDMI_AUDIO_ON )
                        temp = HDMI_AUDIO_OFF;
                }
            }
            else
                gScInfo.FreqStableDebunce = 0;
        }
    }
#endif

    if( temp != gScInfo.OutputAudioState )
    {
        mstar_HDMIAudioMute( temp );
    }
}

BYTE mstar_HDMIPacketColor( void )
{
    return drv_mstar_HDMIPacketColor();
}


void mstar_HDMIAudioMute( BYTE sw )
{
    drv_mstar_HDMIAudioMute(sw);
}

void mstar_HDMIGetPktStatus( void )
{
    drv_mstar_HDMIGetPktStatus();
}

#if (ENABLE_FREESYNC&&ENABLE_HDMI)
Bool mstar_HDMIGetFreeSyncFlag( void )
{
	#if 0
	return drv_mstar_HDMIGetFreeSyncFlag();
	#else
    if(gScInfo.InputSPDInfoReceived)
        return drv_mstar_HDMIGetFreeSyncFlag();
    else 
        return FALSE;
	#endif
}
#endif

void mstar_HDMIAutoEQProc(void)
{
    drv_mstar_HDMIAutoEQProc();
}

BYTE mstar_HDMITmdsGetType( void )
{
    return drv_mstar_HDMITmdsGetType();
}

Bool mstar_HDMIIsAudioPCM(void)
{
    // if TRUE, then audio format is PCM, otherwise its non-PCM;
    return drv_mstar_HDMIIsAudioPCM();
}

#if ENABLE_CTS_INT
#if ENABLE_CTSN_FILTER
/*
void EnableCTSNFileterFunction( void )
{
    drv_EnableCTSNFileterFunction();
}

void CheckCTSNFilterFunction( void )
{
    drv_CheckCTSNFilterFunction();
}
*/
#else
void EnableCTSFileterFunction( void )
{
    drv_EnableCTSFileterFunction();
}

void CheckCTSFilterFunction( void )
{
    drv_CheckCTSFilterFunction();
}
#endif
#endif
#if ENABLE_HDMI_SW_AUDCLK_CHECK   // 20080403 audio clock over spec
//*******************************************************************
// Function Name: mstar_HDMICheckAudioFreq
// Decscription: N/CTS = (128 fs)/(TMDS_Clk)
//*******************************************************************
AudioFreqType mstar_HDMICheckAudioFreq( void )
//BYTE mstar_HDMICheckAudioFreq(void)
{
    return drv_mstar_HDMICheckAudioFreq();
}

void mstar_SetAudioClockLock( Bool bLock )
{
    drv_mstar_SetAudioClockLock(bLock);
}
#endif // #if EN_HDMI_SW_AUDCLK_CHECK

#if ENABLE_AUDIO_AUTO_MUTE || ENABLE_AUDIO_AUTO_FADING || ENABLE_CTSN_FILTER

///////////////////////////////////////////////////////////////////////////////
//     Audio mute/fading events:
//     [0]: HDMI audio sample error.
//     [1]: HDMI audio sample parity error.
//     [2]: HDMI non-PCM.
//     [3]: HDMI AVMUTE.
//     [4]: HDMI CTS/N over range.
//     [5]: HDMI no input clock.
//     [6]: HDMI video clock big change.
//     [7]: HDMI audio sample channel status information changes.
//     [8]: HDMI 2-channel audio sample present bit error.
//     [9]: HDMI audio sample flat bit is set.
///////////////////////////////////////////////////////////////////////////////
void CheckAudioErrorStatus( void )
{
    drv_CheckAudioErrorStatus();
}

#endif // #if ENABLE_AUDIO_AUTO_MUTE || ENABLE_AUDIO_AUTO_FADING || ENABLE_CTSN_FILTER
#if ENABLE_HDMI_1_4
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                HDMI 1.4 new feature:
//                                1. 3D format
//                                2. 4K x 2K format
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// HDMI_Video_Format: Vendor Specifc Info-frame, PB4[7:5]
//   000: No additional HDMI video format is presented in this packet
//   001: Extended resolution format (e.g. 4Kx2K video) present
//   010: 3D format indication present
//   011~111: Reserved
E_HDMI_ADDITIONAL_VIDEO_FORMAT msHDMI_Check_Additional_Format(void)
{
    return drv_msHDMI_Check_Additional_Format();
}


// 3D_Structure: Vendor Specifc Info-frame, PB5[7:4]
//   0000: Frame packing
//   0001: Field alternative
//   0010: Line alternative
//   0011: Side-by-Side(Full)
//   0100: L+depth
//   0101: L+depth+graphics+graphics-depth
//   0110: Top-and-Bottom
//   0111: Reserved
//   1000: Side-by-Side(Half)
//   1001 ~ 1111: Reserved
E_HDMI_3D_INPUT_MODE msHDMI_Get_3D_Structure(void)
{
    return drv_msHDMI_Get_3D_Structure();
}

#if  0
// 3D_Ext_Data: Vendor Specifc Info-frame, PB6[7:4]
//   0000 ~ 0011 : Horizontal sub-sampling
//   0100: Quincunx matrix - Odd/Left  picture, Odd/Right  picture
//   0101: Quincunx matrix - Odd/Left  picture, Even/Right picture
//   0110: Quincunx matrix - Even/Left picture, Odd/Right  picture
//   0111: Quincunx matrix - Even/Left picture, Even/Right picture
//   1000 ~ 1111: Reserved
E_HDMI_3D_EXT_DATA_T msHDMI_Get_3D_Ext_Data(void)
{
    return drv_msHDMI_Get_3D_Ext_Data();
}


// 3D_Meta_Field: Vendor Specifc Info-frame, PB5[3], PB7 ~ PB7+N
//   - 3D_Meta_Present: PB5[3]
//   - 3D_Metadata_Type: PB7[7:5]
//       000: parallax information defined in ISO23002-3
//       001 ~ 111: reserved
//   - 3D_Metadata_Length(N, N <= 31-8 = 23): PB7[4:0]
//   - 3D_Metadata[] : PB8 ~ PB7+N
void msHDMI_Get_3D_Meta_Field(sHDMI_3D_META_FIELD *ptr)
{
    drv_msHDMI_Get_3D_Meta_Field(*ptr);
}


// VIC_CODE: Auxiliary Video Information Info-frame, PB4[6:0]
BYTE msHDMI_Get_VIC_Code(void)
{
    return drv_msHDMI_Get_VIC_Code();
}


// 4Kx2K VIC code: Vendor Specifc Info-frame, PB5[7:0]
//   0x01: 1920(x2)x1080(x2) @ 29.97/30Hz
//   0x02: 1920(x2)x1080(x2) @ 25Hz
//   0x03: 1920(x2)x1080(x2) @ 23.976/24Hz
//   0x04: 2048(x2)x1080(x2) @ 24Hz
//   0x00, 0x05 ~ 0xFF: Reserved
E_HDMI_VIC_4Kx2K_CODE msHDMI_Get_4Kx2K_VIC_Code(void)
{
    return drv_msHDMI_Get_4Kx2K_VIC_Code();
}
#endif

void msHDMI_Check_3D_Mode_Chg(BYTE SetTimingChg)
{
    BYTE ucSCFmtIn =SC_FMT_IN_NORMAL;

    gScInfo.NoVsPktCnt++;
    if( gScInfo.InputPacketStatus & BIT7 )
    {
        if( msHDMI_Check_Additional_Format() == E_HDMI_3D_FORMAT )
        {
            switch( msHDMI_Get_3D_Structure() )
            {
                case E_HDMI_3D_INPUT_FRAME_PACKING:
                    ucSCFmtIn = SC_FMT_IN_3D_FP;
                    break;
                case E_HDMI_3D_INPUT_TOP_BOTTOM:
                    ucSCFmtIn = SC_FMT_IN_3D_TB;
                    break;
                case E_HDMI_3D_INPUT_SIDE_BY_SIDE_HALF:
                    ucSCFmtIn = SC_FMT_IN_3D_SBSH;
                    break;
                case E_HDMI_3D_INPUT_LINE_ALTERNATIVE:
                    ucSCFmtIn = SC_FMT_IN_3D_LA;
                    break;
                case E_HDMI_3D_INPUT_SIDE_BY_SIDE_FULL:
                    ucSCFmtIn = SC_FMT_IN_3D_SBSF;
                    break;
                case E_HDMI_3D_INPUT_FIELD_ALTERNATIVE:
                    ucSCFmtIn = SC_FMT_IN_3D_FA;
                    break;
                case E_HDMI_3D_INPUT_L_DEPTH:
                case E_HDMI_3D_INPUT_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH:
                default:
                    ucSCFmtIn = SC_FMT_IN_NORMAL;
                    break;
            }
            gScInfo.NoVsPktCnt = 0;
        }
    }

    if( ((ucSCFmtIn != g_SetupPathInfo.ucSCFmtIn) && ( (gScInfo.NoVsPktCnt==0) || (gScInfo.NoVsPktCnt >= NoVsPktBound))) )
    {
        HDMI_printData("HDMI 3D FMT CHG : %d", ucSCFmtIn);
    	HDMI_printData("g_SetupPathInfo.ucSCFmtIn : %d", g_SetupPathInfo.ucSCFmtIn);

        gScInfo.NoVsPktCnt = 0;
        if(!g_SetupPathInfo.bOverrideSCFmtIn)
        {
        	g_SetupPathInfo.ucSCFmtIn = ucSCFmtIn;
        	if ( SetTimingChg )
     		{
             	HDMI_printData("HDMI 3D do mode setup CHG : %d", SetTimingChg);
				Set_DoModeSettingFlag();
            //Set_InputTimingChangeFlag();
        	}
    	}
	}
}
#endif // #if EN_HDMI_1_4

#else
#define HDMI_DEBUG  0
#endif //#if ENABLE_HDMI

#if (!(ENABLE_DEBUG &&HDMI_DEBUG)) || (!ENABLE_HDMI)
BYTE code HDMINullData[1] = {0};

void HDMIDummy(void)
{
    BYTE xdata i = HDMINullData[0];
}
#endif

#if ENABLE_CTS_HPD_PD100K  
//*******************************************************************
// Function Name: msHDMI_EnHPDPD100K
// Decscription: HPD pin setting  to CBUS pad for CTS
//*******************************************************************
void msHDMI_EnHPDPD100K( BYTE InputType )
{
    drv_mstar_HDMIEnHPDPD100K(InputType);
}
#endif
#endif

