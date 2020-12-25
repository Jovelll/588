#ifndef _DRV_MSHDMI_9_C_
#define _DRV_MSHDMI_9_C_
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
//#include "halRwreg.h"

#define HDMI_DEBUG   1
#if ENABLE_DEBUG &&HDMI_DEBUG
#define HDMI_printData(str, value)   printData(str, value)
#define HDMI_printMsg(str)           printMsg(str)
#else
#define HDMI_printData(str, value)
#define HDMI_printMsg(str)
#endif

void drv_mstar_HDMIInitialAudio( void )
{

    //audio setting
    msWriteByteMask(REG_1A07, 0, BIT0); //[0]: PLL power down
    msWriteByteMask(REG_1A30, 0x81, 0x81); //[7]: reg_en_cts_n_synth, [0]:reg_synth_sel_cts_ref
    msWriteByteMask(REG_1A70, BIT6, BIT6); //[6]: HDMI clk synthesizer enable.
    
	msWriteByteMask(REG_1AD5, 0xC0, 0xFF); //[7:0]: reg_gain
    msWriteByteMask(REG_1AE1, BIT7, BIT7); // [7]: reg_apll_auto_div_en.
     
    msWriteByte( REG_1403, 0xC3 ); // [7]: Output black color in video blanking, [6]: GCP CD val, [3]:Video mute when AVMUTE, [1]: HDMI/DVI auto detect
    msWriteByte( REG_1404, 0x03 ); // [1]: auto reset deep color mode, [0]: deep color mode enable
    //msWriteByteMask(REG_0510, 0, BIT4|BIT3); // [4]:0: select pixel clock, [3]:0: manuel pixel clock mode
    msWriteByteMask( REG_142A, BIT6, BIT7 | BIT6 ); // [7]: 0: select tmds clock, [6]: 1: pixel clock control mode enable.

    msWriteByteMask( REG_1707, 0, BIT0 ); // [0]: HPLL power on
    msWriteByteMask( REG_1426, 0xF0, 0xF0 ); // [5]: Enable Port0~Port3 video clock 2x for HDMI deep color 12 bits //only one port.

    msWriteByteMask(REG_1A84, 0x06, 0x06); //[2:1]: AVMute select to Haydn switch to old mode
    msWriteByteMask(REG_1ACD, 0x70, 0xFF); //[7:0]: fading out gain
    msWriteByteMask(REG_1AE8, BIT6, BIT6); //[6]: Enable remove PCM audio sample DC bias

#if 1 //for i2s and SPDIF
    msWriteByteMask(REG_1A8B, 0x80, 0x80);    
    msWriteByteMask(REG_1AD3, 0x03, 0x03); 
    msWriteByteMask(REG_1A82, 0x10, 0x1C);	//192fs
#endif

#if ENABLE_AUDIO_AUTO_MUTE // auto mute setting
    msWriteByte( REG_1AC3, 0xAF ); // Fading events: [7]: audio sample error, [5]:Channel Status/present bit error, [3]:no input clock, [2]: HDMI AVMUTE, [1]:clcok big change, [0]: CTS/N over range.
    //msWriteByte( REG_05D4, 0x00 ); // Fading events: [9]:flat bit;   120209 coding, default set to disable, enable will cause short voice cannot be heared
#endif
#if ENABLE_AUDIO_AUTO_FADING // auto fading setting
    msWriteByteMask(REG_1AD0, 0x08, 0x0C ); // [3:2]: Automatic fading mode
    msWriteByteMask(REG_1AC7, 0x08, 0x0E ); // [3:1]:fading speed select 16X
    msWriteByte( REG_1AC4, 0xFF ); // Zero crossing threshold = 0xFF
    msWriteByte( REG_1AC5, 0xFF ); // Zero crossing threshold = 0xFF
    msWriteByte( REG_1AC6, 0xAF ); // Fading events: [7]: audio sample error, [5]:Channel Status/present bit error, [3]:no input clock, [2]: HDMI AVMUTE, [1]:clcok big change, [0]: CTS/N over range.
    //msWriteByte( REG_05D6, 0x00 ); // Fading events: [9]:flat bit;   120209 coding, default set to disable, enable will cause short voice cannot be heared
#endif

#if ENABLE_HAYDN_FADING_BY_HDMI_STATUS
    msWriteByteMask(REG_1792, 0x01, 0x01 ); 
    msWriteByteMask(REG_1A84, 0x00, 0x06 ); // [2:1]: fading status to haydn
    msWriteByteMask(REG_1AC3, 0x00, 0xFF ); // [7:0]: disable HDMI mute function
    msWriteByteMask(REG_1AD0, 0x00, 0x0C ); // [3:2]: disable HDMI fading function
#endif
	
    msWriteByteMask( SC0_7A, BIT7, BIT7 ); // [7]: HDMI 444 pixel repetition mode enable
}

void drv_mstar_HDMIRst( BYTE rst )
{
    if ( rst == 0x80 ) //Reset_HDMIStatus
    {
        msWriteByteMask ( REG_1417, BIT7, BIT7 ); // [7]: reset HDMI status.
        msWriteByteMask ( REG_1417, 0x00, BIT7 );
                
        msWriteByteMask ( REG_1410, 0x02, 0x02 ); //[1]: reset packet content 
        msWriteByteMask ( REG_1410, 0x00, 0x02 );
    }   
    else if ( rst == 0x02 ) //Reset_Color
    {
        msWriteByteMask ( REG_1410, 0x02, 0x02 ); //[1]: reset packet content 
        msWriteByteMask ( REG_1410, 0x00, 0x02 );
    }
    else
    {
        msWriteByte( REG_1417, rst );
        msWriteByte( REG_1417, 0 );
    }
}

BYTE drv_mstar_HDMIPacketColor( void )
{
    BYTE temp;

    temp = INPUT_RGB;
    switch(( msReadByte( REG_14A2 ) & 0x60 ) >> 5 )
    {
        case 0:
            temp = INPUT_RGB;
            break;
        case 1:
            temp = INPUT_YUV422;
            break;
        case 2:
            temp = INPUT_YUV444;
            break;
    }

    switch( (msReadByte(REG_14A3)&(BIT6|BIT7)) >>6)
    {
        case 0:
        gScInfo.AVI_Colorimetry=YUV_Colorimetry_NoData;
        break;
        case 1:
        gScInfo.AVI_Colorimetry=YUV_Colorimetry_ITU601;
        break;
        case 2:
        gScInfo.AVI_Colorimetry=YUV_Colorimetry_ITU709;
        break;
    }

    return temp;
}

void drv_mstar_HDMIAudioMute( BYTE sw )
{
    gScInfo.OutputAudioState = sw;
    HDMI_printData( "--gScInfo.OutputAudioState %d", gScInfo.OutputAudioState );

    if( sw == HDMI_AUDIO_ON )
    {
        hw_ClrMute();
        HDMI_printMsg( " HDMI_AUDIO_ON");
    }
    else
    {
        hw_SetMute();
        HDMI_printMsg( " HDMI_AUDIO_OFF");
#if ENABLE_HDMI_SW_AUDCLK_CHECK
    gScInfo.CurOutAudFreq = AUD_FREQ_ERROR;
    mstar_SetAudioClockLock( FALSE ); // chip w
#endif

    }

    msWriteByteMask( REG_1427, sw == HDMI_AUDIO_OFF ? BIT1 : 0, BIT1 ); // global mute


#if ENABLE_CTS_INT
    if( sw == HDMI_AUDIO_ON )
    {
        gScInfo.CTSFilterEN = 1;
       // drv_EnableCTSFileterFunction();
    }
    else
    {
        gScInfo.CTSFilterEN = 0;
    }
#endif
}

void drv_mstar_HDMIGetPktStatus( void )
{
#if ENABLE_HDMI_SW_CTS_STABLE
    BYTE  ucReg;
#endif
#if ENABLE_CTS_INT
#if !ENABLE_CTSN_FILTER
    DWORD dwData;
#endif
#endif
    static BYTE CNTS = 0;

    gScInfo.InputPacketStatus = msRead2Byte( REG_14EA );
    gScInfo.InputPacketError = msReadByte( REG_14EE );
    msWrite2Byte( REG_14EA, gScInfo.InputPacketStatus );
    msWriteByte( REG_14EE, gScInfo.InputPacketError );
#if ENABLE_HDMI_SW_CTS_STABLE
    ucReg = msReadByte( REG_1478 );

    if( abs( ucReg - gScInfo.InputCTSValue ) < CTSRange ) // for Check PC CTS Stable
    {
        if( gScInfo.OutputAudioCnt )
            gScInfo.OutputAudioCnt--;
    }
    else
        gScInfo.OutputAudioCnt = AudioDeBounce;
    gScInfo.InputCTSValue = ucReg;
#endif // #if EN_HDMI_SW_CTS_STABLE

#if ENABLE_CTS_INT
#if !ENABLE_CTSN_FILTER
    if( gScInfo.InputTmdsType == TMDS_HDMI )
    {
        dwData = (( DWORD )( msReadByte(REG_147A ) & 0x0F ) << 16 ) | ( DWORD )msRead2Byte( REG_1478 );
        if( labs( dwData - gScInfo.InputCTSData ) < CTSSTABLERANGE )
        {
            if( gScInfo.CTSStableCnt )
                gScInfo.CTSStableCnt--;
        }
        else
        {
            gScInfo.CTSStableCnt = CTSSTABLECOUNT;
        }
        gScInfo.InputCTSData = dwData;
        CheckCTSFilterFunction();

    }
#endif
#endif

    if( gScInfo.InputPacketStatus & BIT3 )
        gScInfo.InputAVIInfoReceived = 1;

    if( gScInfo.InputPacketStatus & BIT2 )
    {
    	gScInfo.InputSPDInfoReceived = 1;
    	CNTS = 50;
    }
    else
    {
    	if(CNTS != 0)
    		CNTS--;
    	else
    	{
    		gScInfo.InputSPDInfoReceived = 0;
    	}
    }

}

BYTE drv_mstar_HDMITmdsGetType( void )
{
    BYTE ret_type, temp;

    ret_type = TMDS_DVI;;    //default DVI
    temp = msReadByte( REG_1518 );
    if( temp & BIT0 )
        ret_type = TMDS_HDMI;
    return ret_type;
}

#if (ENABLE_FREESYNC&&ENABLE_HDMI)
Bool drv_mstar_HDMIGetFreeSyncFlag(void)
{
    if ((msReadByte(REG_15B8) == 0x1A) && (msReadByte(REG_15B9) == 0x00) && (msReadByte(REG_15BA) == 0x00))
    {
        if(msReadByte(REG_15BD) & 0x01)
        {
            msWriteByteMask(REG_1554, 0x03, 0x0F);
            return TRUE;
        }
        else
        {
            msWriteByteMask(REG_1554, 0x01, 0x0F);
            return FALSE;
        }
    }
    else
       return FALSE;
}
#endif

void drv_mstar_HDMIAutoEQProc(void)
{
    static BYTE counts = 0;
    static BYTE timecouts = 0;
    BYTE temp = 0;
    BYTE count = 0;
    BYTE loop = 0;
    temp = msReadByte(REG_1BB5) & 0x07; //use reserved bit for autoEQ state machine  

    if(temp != 0)
    {
        switch(temp)
        {
            case 0x01: //autoEQ start flag, first time trig autoEQ
                if((msReadByte(REG_1BEB) & 0xE0) == 0xE0) // Freq lock.
                {
                    msWriteByteMask(REG_1F03, 0x80, 0x80); // clr autoEQ done status
                    msWriteByteMask(REG_1F03, 0x00, 0x80); 
                    msWriteByteMask(REG_1B66, 0x04, 0x04); // sw trigger autoEQ
                    msWriteByteMask(REG_1B66, 0x00, 0x04);
                    msWriteByteMask(REG_1BB5, 0x02, 0x07); // go to case 2
					counts = 0;
                    timecouts = 0;
                }
			break;
            case 0x02: //wait autoEq done
                if(msReadByte(REG_1F00) & 0x80)
                {
                    msWriteByteMask(REG_1F03, 0x80, 0x80); // clr autoEQ done status
                    msWriteByteMask(REG_1F03, 0x00, 0x80);
                    for (loop = 0; loop<5; loop++) //check DE stable
                    {
                        if(DVI_DE_STABLE())
        	                count++;
                    }
                    if(count == 0x05)
                    {
                        msWriteByteMask(REG_1B66, 0x04, 0x04); // 2nd sw trigger autoEQ
                        msWriteByteMask(REG_1B66, 0x00, 0x04);
                        msWriteByteMask(REG_1BB5, 0x03, 0x07); // go to case 3
                        timecouts = 0;
                    }
					else
                    {
                        msWriteByteMask(REG_1BB5, 0x01, 0x07);
                    }
                }
				else
				{
                    timecouts++;
                    if(timecouts > 0xA0) // almost 3.2 sec
                    {
                        timecouts = 0;
                        msWriteByteMask(REG_1BB5, 0x00, 0x07); // timeout
                        msWriteByteMask(REG_1F3D, 0x80, 0x80); // Auto Band select EQ
                    }
                }
            break;
            case 0x03: //wait 2nd autoEQ done
                if(msReadByte(REG_1F00) & 0x80)
                {
                    msWriteByteMask(REG_1F03, 0x80, 0x80); // clr autoEQ done status
                    msWriteByteMask(REG_1F03, 0x00, 0x80);
				    msWriteByteMask(REG_1602, 0x80, 0x80); // clr error status
                    msWriteByteMask(REG_1602, 0x00, 0x80);

                    for (loop = 0; loop<5; loop++) //check DE stable
                    {
                        if(DVI_DE_STABLE())
        	                count++;
                    }
                    if(count == 0x05)
                    {
                        msWriteByteMask(REG_1BB5, 0x04, 0x07); // go to case 4
                    }
					else
                    {
                        msWriteByteMask(REG_1BB5, 0x01, 0x07);
                    }
                }
				else
				{
                    timecouts++;
                    if(timecouts > 0xA0) // almost 3.2 sec
                    {
                        timecouts = 0;
                        msWriteByteMask(REG_1BB5, 0x00, 0x07); // timeout
                        msWriteByteMask(REG_1F3D, 0x80, 0x80); // Auto Band select EQ
                    }
                }
            break;
            case 0x04: // check 1.4 error status
                if((gScInfo.InputPacketError == 0x00) && (msReadByte(REG_160D) == 0x00)
                && (msReadByte(REG_160F) == 0x00) && (msReadByte(REG_1611) == 0x00))
                {
                    counts++;
                }
                else
                {
                    msWriteByteMask(REG_1BB5, 0x01, 0x07);
                }
                if(counts>= 0x50)
                {
                    msWriteByteMask(REG_1BB5, 0x00, 0x07); //finished
                }
            break;
            default:
				
            break;
        }
    }
    else
    {
        // do nothing.
    }
}

Bool drv_mstar_HDMIIsAudioPCM(void)
{
    Bool bAudFmtIsPCM;
    
    if (msReadByte(REG_14CA) & _BIT6)
        bAudFmtIsPCM = FALSE;
    else
        bAudFmtIsPCM = TRUE;

    return bAudFmtIsPCM;
}

#if ENABLE_CTS_INT
#if ENABLE_CTSN_FILTER
void drv_EnableCTSNFileterFunction( void )
{
    BYTE ucTemp;

    HDMI_printMsg( "\r\n Enable CTSN Func" );
    //msWriteBit(REG_05CC, FALSE, BIT2); // audio FIFO not reset
    msWrite2Byte( REG_140A, gScInfo.InputCTSData & 0x00FFFF );  // CTS Limited
    msWrite2Byte( REG_140C, gScInfo.InputNData & 0x00FFFF );  // N Limited
    ucTemp = (( gScInfo.InputCTSData >> 16 ) & 0x0F ) | (( gScInfo.InputNData >> 12 ) & 0xF0 );
    msWriteByte( REG_1408, ucTemp );

    msWriteByteMask( REG_1405, BIT6, BIT6 ); // Enable CTS/N filter
    msWrite2Byte( REG_140E, 0xFFFF ); //[15:8]: N range, [7:0]: CTS range

    msWriteByteMask( REG_14EF, BIT7, BIT7 ); // update CTS/N Limited
    //msWriteByteMask( REG_14EC, BIT6, BIT6 ); // clear CTS/N over range
    msWriteByte( REG_14EC, BIT6); // clear CTS/N over range
}

void drv_CheckCTSNFilterFunction( void )
{
    BYTE ucFlag = 0;
    DWORD dwCTSValue;
    DWORD dwNValue;

    msWriteByteMask( REG_14EF, 0x40, 0x40); // toggle to update CTS/N value

    dwCTSValue = (((DWORD)(msReadByte(REG_147A) & 0x0F)) << 16) | msRead2Byte(REG_1478);
    dwNValue = ((((DWORD)(msReadByte( REG_147A ) & 0xF0)) >> 4 ) << 16 ) | msRead2Byte( REG_147C );

    if( labs( dwCTSValue - gScInfo.InputCTSData ) > CTSN_Range ) // CTS over range
        ucFlag |= _BIT2;
    if( labs( dwNValue - gScInfo.InputNData ) > CTSN_Range ) // N over range
        ucFlag |= _BIT3;

    gScInfo.InputCTSData = dwCTSValue;
    gScInfo.InputNData = dwNValue;

    if( ucFlag )
    {
        ucFlag = FALSE;
        gScInfo.CTSStableCnt = CTSSTABLECOUNT;
    }
    else
    {
        ucFlag = FALSE;
        if( gScInfo.CTSStableCnt )
        {
            gScInfo.CTSStableCnt--;
            if( gScInfo.CTSStableCnt == 0 )
                ucFlag = TRUE;
        }
    }

    if( gScInfo.CTSFilterEN == 1 )
    {
        if( ucFlag )
            drv_EnableCTSNFileterFunction();
        // CTS/N is over range
        else if( gScInfo.CTSStableCnt == 0 && ( msReadByte( REG_14EC )& BIT6 ) )
            gScInfo.CTSStableCnt = CTSSTABLECOUNT;
    }
}

#else
void drv_EnableCTSFileterFunction( void )
{

    if( gScInfo.CTSFilterEN == 1 )
    {
        msWriteByteMask( REG_1405, 0, BIT6 ); // [6]: disable CTS/N filter
        msWriteByteMask( REG_1408, gScInfo.InputCTSData >> 16, 0x0F );
        msWrite2Byte( REG_140A, gScInfo.InputCTSData & 0x00FFFF );
        msWriteByteMask( REG_14EF, BIT7, BIT7 ); // [7]: update new CTS value
        msWriteByteMask( REG_1405, BIT6, BIT6 ); // [6]: enable CTS/N filter
        HDMI_printMsg( "  drv_EnableCTSFileterFunction" );
    }

}

void drv_CheckCTSFilterFunction( void )
{

    if( gScInfo.CTSFilterEN == 1 )
    {
        if( gScInfo.InputPacketError & BIT1 )// BCH parity error
        {
            if( gScInfo.CTSStableCnt == 0 && InputTimingStableFlag )
            {
                drv_EnableCTSFileterFunction();
            }
        }
    }
    if(  gScInfo.InputPacketError & 0x30  )// ASP error or AS parity bit error
    {
        msWriteByteMask( REG_1427, BIT1, BIT1 ); // global mute
    }


}
#endif
#endif



#if ENABLE_HDMI_SW_AUDCLK_CHECK   // 20080403 audio clock over spec
//*******************************************************************
// Function Name: mstar_HDMICheckAudioFreq
// Decscription: N/CTS = (128 fs)/(TMDS_Clk)
//*******************************************************************
AudioFreqType drv_mstar_HDMICheckAudioFreq( void )
//BYTE mstar_HDMICheckAudioFreq(void)
{
    AudioFreqType bFreqVaild;
    //BYTE bFreqVaild;
    DWORD dwTemp1, dwTemp2;

    dwTemp2 = (( DWORD )UserPrefHTotal * (( XTAL_CLOCK_KHZ + SrcHPeriod / 2 ) / SrcHPeriod ) ) / 1000;
    //HDMI_printData("--CLK-- %d",dwTemp2);

    //for MHL pp mode, we have to double the tmds clock
    if (msReadByte(REG_1604) & BIT1)
        dwTemp2 = dwTemp2 << 1;

    msWriteByteMask( REG_14EF, 0x40, 0x40); // toggle to update CTS/N value
    dwTemp1 = ((((DWORD)( msReadByte( REG_147A ) & 0xF0 )) >> 4 ) << 16 ) | msRead2Byte( REG_147C ); //N
    if( dwTemp1 < 0x10 )
    {
        return AUD_FREQ_ERROR;
    }
    dwTemp2 = dwTemp1 * dwTemp2 / 128;

    dwTemp1 = (((DWORD)(msReadByte(REG_147A) & 0x0F)) << 16 ) | msRead2Byte( REG_1478 );
    if( dwTemp1 < 0x10 )
    {
        return AUD_FREQ_ERROR;
    }

    dwTemp1 = ( dwTemp1 + 500 ) / 1000;
    dwTemp2 = ( dwTemp2 + dwTemp1 / 2 ) / dwTemp1;
    //HDMI_printData("--HDMI Audio Freq-- %d",dwTemp2);

#if 0
    if( dwTemp2 >= 25 && dwTemp2 <= ( 192 + 4 ) )
        bFreqVaild = dwTemp2;
    else
        bFreqVaild = AUD_FREQ_ERROR;
#else
    if( abs( dwTemp2 - 32 ) < 4 )
        bFreqVaild = AUD_FREQ_32K;
    else if( abs( dwTemp2 - 44 ) < 4 )
        bFreqVaild = AUD_FREQ_44K;
    else if( abs( dwTemp2 - 48 ) < 4 )
        bFreqVaild = AUD_FREQ_48K;
    else if( abs( dwTemp2 - 88 ) < 4 )
        bFreqVaild = AUD_FREQ_88K;
    else if( abs( dwTemp2 - 96 ) < 4 )
        bFreqVaild = AUD_FREQ_96K;
    else if( abs( dwTemp2 - 192 ) < 4 )
        bFreqVaild = AUD_FREQ_192K;
    else
    {
        //HDMI_printData("--HDMI Audio Freq-- %d",dwTemp2);
        bFreqVaild = AUD_FREQ_ERROR;
    }
#endif
    return bFreqVaild;
}

void drv_mstar_SetAudioClockLock( Bool bLock )
{
    BYTE ucDBFC;

    // ucDBFC = msReadByte( SC0_01 );
    ucDBFC=mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask( SC0_01, 0, BIT0 );

    if( bLock )
    {
        msWriteByteMask(REG_1730, BIT3, BIT3);
        HDMI_printData( "  mstar_SetAudioClockLock LOCK[%d]", gScInfo.CurOutAudFreq );
    }
    else
    {
        msWriteByteMask(REG_1730, 0, BIT3);
        HDMI_printMsg( "  mstar_SetAudioClockLock UNLOCK" );
    }
   // msWriteByte( SC0_01, ucDBFC );
   mStar_ScalerDoubleBuffer(ucDBFC&BIT0);
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
void drv_CheckAudioErrorStatus( void )
{
    WORD uAudioMuteEn = 0;

#if ENABLE_CTSN_FILTER
    uAudioMuteEn |= (msReadByte(REG_1AC3) & BIT0) | (msReadByte(REG_1AC6) & BIT0);
#endif
#if ENABLE_AUDIO_AUTO_MUTE
    uAudioMuteEn |= msRead2Byte(REG_1AC2) & 0xFF00; // auto mute events
#endif
#if ENABLE_AUDIO_AUTO_FADING
    uAudioMuteEn |= msRead2Byte(REG_1AC5) & 0xFF00; // auto fading events
#endif

    if( uAudioMuteEn )
    {
        if( uAudioMuteEn & _BIT8 ) // CTS/N over range
        {
            if( msReadByte( REG_14EC ) & _BIT6 ) // error status bit
            {
                gScInfo.ucAudioErrorEvents.bCTSNOverRange = 1;
                msWriteByteMask(REG_1405, 0, BIT6); // SW path for CTSN enable can not receive ACR packet, [6]: turn off CTS/N filter
                drv_CheckCTSNFilterFunction();
            }
            else
                gScInfo.ucAudioErrorEvents.bCTSNOverRange = 0;
        }

        if( uAudioMuteEn & _BIT9 ) // HDMI video clock frequency big change.
        {
            if( msReadByte( REG_1B64 ) & _BIT2 ) // error status bit
            {
                gScInfo.ucAudioErrorEvents.bVideoClockBigChanged = 1;
                if( msReadByte( REG_1B64 ) & _BIT3 ) // video clock is stable
                {
                    msWriteBit( REG_1B64, TRUE, _BIT2 ); // write 1 to clear
                }
            }
            else
                gScInfo.ucAudioErrorEvents.bVideoClockBigChanged = 0;
        }

        if( uAudioMuteEn & _BIT10 ) // AVMUTE, IC would check GC packet directly, no firmware effort
        {
            if( msReadByte( REG_1480 ) & _BIT0 ) // error status bit
            {
                if( !gScInfo.ucAudioErrorEvents.bAVMUTE )
                {
                    gScInfo.ucAudioErrorEvents.bAVMUTE = 1;
                    //mStar_SetupFreeRunMode();
                }
            }
            else
            {
                if( gScInfo.ucAudioErrorEvents.bAVMUTE )
                {
                   gScInfo.ucAudioErrorEvents.bAVMUTE = 0;
                   //mStar_SetupMode();
        #if (GLASSES_TYPE==GLASSES_NVIDIA   )
             if((SrcFlags & bUnsupportMode) ||(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
                {
                  NV_3Dmode(FALSE);
                }
                else
                {
                  NV_3Dmode(TRUE);
                }
        #endif
             mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF); //HDMI av Mute test  can't clear

                }
            }
        }

        if( uAudioMuteEn & _BIT11 ) // DVI no input clock, IC would freeze the previous audio frequency when DVI is no input
        {
            if( msReadByte( REG_1A33 ) & _BIT7 ) // error status bit
                gScInfo.ucAudioErrorEvents.bNoInputClock = 1;
            else
                gScInfo.ucAudioErrorEvents.bNoInputClock = 0;
        }

        if( uAudioMuteEn & _BIT12 ) // audio sample with flat bit
        {
            if( msReadByte( REG_1ADE ) & _BIT7 ) // error status bit
            {
                gScInfo.ucAudioErrorEvents.bFlatBitReceived = 1;
                //msWriteBit( REG_14EC, TRUE, _BIT4 ); // write 1 to clear
                msWriteByte( REG_1ADE, _BIT7 ); // write 1 to clear
            }
            else
                gScInfo.ucAudioErrorEvents.bFlatBitReceived = 0;
        }

        if( uAudioMuteEn & _BIT13 ) // channel status bit change
        {
            if( msReadByte( REG_1ADE ) & _BIT2 ) // error status bit
            {
                gScInfo.ucAudioErrorEvents.b2CHASPPresentBitError = 1;
                msWriteBit( REG_1ADE, TRUE, _BIT2 ); // clear
            }
            else
                gScInfo.ucAudioErrorEvents.b2CHASPPresentBitError = 0;
        }

        if( uAudioMuteEn & _BIT14 ) // audio fifo error
        {
            if( msReadByte( REG_1ADF ) & _BIT0 ) // error status bit
            {
                //gScInfo.ucAudioErrorEvents.b2CHASPPresentBitError = 1;
                msWriteByteMask( REG_1ADF, 0x03, 0x03 ); // clear
            }
            //else
                //gScInfo.ucAudioErrorEvents.b2CHASPPresentBitError = 0;
        }

        if( uAudioMuteEn & _BIT15 ) // audio sample error/audio sample priority error
        {
            if( msReadByte( REG_1ADE ) & 0x03 ) // error status bit
            {
                //gScInfo.ucAudioErrorEvents.b2CHASPPresentBitError = 1;
                msWriteByteMask( REG_1ADE, 0x03, 0x03 ); // clear
            }

        }

    }
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
E_HDMI_ADDITIONAL_VIDEO_FORMAT drv_msHDMI_Check_Additional_Format(void)
{
    E_HDMI_ADDITIONAL_VIDEO_FORMAT val;

    if ((msReadByte(REG_148A) & 0xFF) == 0) // 000
        val = E_HDMI_NO_ADDITIONAL_FORMAT;
    else if ((msReadByte(REG_148A) & 0xFF) == BIT5) // 001
        val = E_HDMI_4Kx2K_FORMAT;
    else if ((msReadByte(REG_148A) & 0xFF) == BIT6) // 010
        val = E_HDMI_3D_FORMAT;
    else // 011 ~ 111
        val = E_HDMI_RESERVED;

    return val;
}

E_HDMI_3D_INPUT_MODE drv_msHDMI_Get_3D_Structure(void)
{
    E_HDMI_3D_INPUT_MODE val;

    if ((msReadByte(REG_148B) & 0xF0) == 0)
        val = E_HDMI_3D_INPUT_FRAME_PACKING;
    else if ((msReadByte(REG_148B) & 0xF0) == 0x10)
        val = E_HDMI_3D_INPUT_FIELD_ALTERNATIVE;
    else if ((msReadByte(REG_148B) & 0xF0) == 0x20)
        val = E_HDMI_3D_INPUT_LINE_ALTERNATIVE;
    else if ((msReadByte(REG_148B) & 0xF0) == 0x30)
        val = E_HDMI_3D_INPUT_SIDE_BY_SIDE_FULL;
    else if ((msReadByte(REG_148B) & 0xF0) == 0x40)
        val = E_HDMI_3D_INPUT_L_DEPTH;
    else if ((msReadByte(REG_148B) & 0xF0) == 0x50)
        val = E_HDMI_3D_INPUT_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH;
    else if ((msReadByte(REG_148B) & 0xF0) == 0x60)
        val = E_HDMI_3D_INPUT_TOP_BOTTOM;
    else if ((msReadByte(REG_148B) & 0xF0) == 0x80)
        val = E_HDMI_3D_INPUT_SIDE_BY_SIDE_HALF;
    else
        val = E_HDMI_3D_RESERVED;

    return val;
}

#if  0
// 3D_Ext_Data: Vendor Specifc Info-frame, PB6[7:4]
//   0000 ~ 0011 : Horizontal sub-sampling
//   0100: Quincunx matrix - Odd/Left  picture, Odd/Right  picture
//   0101: Quincunx matrix - Odd/Left  picture, Even/Right picture
//   0110: Quincunx matrix - Even/Left picture, Odd/Right  picture
//   0111: Quincunx matrix - Even/Left picture, Even/Right picture
//   1000 ~ 1111: Reserved
E_HDMI_3D_EXT_DATA_T drv_msHDMI_Get_3D_Ext_Data(void)
{
    E_HDMI_3D_EXT_DATA_T e_3d_extdata;
    BYTE regval;

    regval = msReadByte(REG_148C) & 0xF0;

    switch(regval)
    {
        case 0x00:
            e_3d_extdata = E_3D_EXT_DATA_HOR_SUB_SAMPL_0;
            break;
        case 0x10:
            e_3d_extdata = E_3D_EXT_DATA_HOR_SUB_SAMPL_1;
            break;
        case 0x20:
            e_3d_extdata = E_3D_EXT_DATA_HOR_SUB_SAMPL_2;
            break;
        case 0x30:
            e_3d_extdata = E_3D_EXT_DATA_HOR_SUB_SAMPL_3;
            break;
        case 0x40:
            e_3d_extdata = E_3D_EXT_DATA_QUINCUNX_MATRIX_0;
            break;
        case 0x50:
            e_3d_extdata = E_3D_EXT_DATA_QUINCUNX_MATRIX_1;
            break;
        case 0x60:
            e_3d_extdata = E_3D_EXT_DATA_QUINCUNX_MATRIX_2;
            break;
        case 0x70:
            e_3d_extdata = E_3D_EXT_DATA_QUINCUNX_MATRIX_3;
            break;
        default:
            e_3d_extdata = E_3D_EXT_DATA_RESERVE;
            break;
    }
    return e_3d_extdata;
}


// 3D_Meta_Field: Vendor Specifc Info-frame, PB5[3], PB7 ~ PB7+N
//   - 3D_Meta_Present: PB5[3]
//   - 3D_Metadata_Type: PB7[7:5]
//       000: parallax information defined in ISO23002-3
//       001 ~ 111: reserved
//   - 3D_Metadata_Length(N, N <= 31-8 = 23): PB7[4:0]
//   - 3D_Metadata[] : PB8 ~ PB7+N
void drv_msHDMI_Get_3D_Meta_Field(sHDMI_3D_META_FIELD *ptr)
{
    BYTE i, regval;

    ptr->b3D_Meta_Present = (msReadByte(REG_148B) & BIT3) > 0 ? TRUE : FALSE;
    regval = msReadByte(REG_148D) & 0xE0;
    switch(regval)
    {
        case 0x00:
            ptr->t3D_Metadata_Type = E_3D_META_DATA_PARALLAX_ISO23002_3;
            break;
        case 0x20:
        case 0x30:
        case 0x40:
        case 0x50:
        case 0x60:
        case 0x70:
            ptr->t3D_Metadata_Type = E_3D_META_DATA_RESERVE;
            break;
        default:
            ptr->t3D_Metadata_Type = E_3D_META_DATA_MAX;
            break;
    }
    ptr->u83D_Metadata_Length = msReadByte(REG_148D) & 0x1F;
    for(i=0;i<ptr->u83D_Metadata_Length;i++)
    {
        ptr->u83D_Metadata[i] = msReadByte(REG_148E+i);
    }
}


// VIC_CODE: Auxiliary Video Information Info-frame, PB4[6:0]
BYTE drv_msHDMI_Get_VIC_Code(void)
{
    BYTE val;

    val = msReadByte(REG_14A5) & 0x7F;
    return val;
}


// 4Kx2K VIC code: Vendor Specifc Info-frame, PB5[7:0]
//   0x01: 1920(x2)x1080(x2) @ 29.97/30Hz
//   0x02: 1920(x2)x1080(x2) @ 25Hz
//   0x03: 1920(x2)x1080(x2) @ 23.976/24Hz
//   0x04: 2048(x2)x1080(x2) @ 24Hz
//   0x00, 0x05 ~ 0xFF: Reserved
E_HDMI_VIC_4Kx2K_CODE drv_msHDMI_Get_4Kx2K_VIC_Code(void)
{
    E_HDMI_VIC_4Kx2K_CODE val;

    if (msReadByte(REG_148B) == 0x01)
        val = E_VIC_4Kx2K_30Hz;
    else if (msReadByte(REG_148B) == 0x02)
        val = E_VIC_4Kx2K_25Hz;
    else if (msReadByte(REG_148B) == 0x03)
        val = E_VIC_4Kx2K_24Hz;
    else if (msReadByte(REG_148B) == 0x04)
        val = E_VIC_4Kx2K_24Hz_SMPTE;
    else
        val = E_VIC_RESERVED;

    return val;
}
#endif

#endif // #if EN_HDMI_1_4

#if ENABLE_CTS_HPD_PD100K  
void drv_mstar_HDMIEnHPDPD100K( BYTE InputType )
{
    switch(InputType)
    {
        case Input_Digital:
            msWriteByteMask(REG_0260, BIT0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 01 -> portB
            msWriteByteMask(REG_0C40, BIT7, BIT7); // REG_PM_MHL_CBUS_20[7], cbus hdmi pd 100k
            break;

        case Input_Digital2:
            msWriteByteMask(REG_0260, 0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 00 -> portA
            msWriteByteMask(REG_0C40, BIT7, BIT7); // REG_PM_MHL_CBUS_20[7], cbus hdmi pd 100k
            break;

        case Input_Digital3:
            msWriteByteMask(REG_0260, BIT1, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 10 -> portC
            msWriteByteMask(REG_0C40, BIT7, BIT7); // REG_PM_MHL_CBUS_20[7], cbus hdmi pd 100k
            break;

        default:
            msWriteByteMask(REG_0260, BIT1 |BIT0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 11 -> reserved
            msWriteByteMask(REG_0C40, 0, BIT7); // REG_PM_MHL_CBUS_20[7], cbus hdmi pd 100k
            break;
     }
}
#endif

#else


#endif//#if ENABLE_HDMI
#endif

