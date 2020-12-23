
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drvDAC_2.h"

#if MS_DAC
bit bToggleGainFlag = 0;
BYTE xdata ToggleGainCntr = 0;

#if ENABLE_DP_AUDIO || ENABLE_HDMI
void msAudio_I2S_SPDIF_Init(void)
{
    msWriteByte( REG_05C8, 0x8A ); // [7]: Frame repetition manual mode; [3]: auto DSD detection; [1]: auto PCM detection
    msWriteByte( REG_05CC, 0xD1 ); // [7]: mute pin to normal(H->L), (set active low for this DAC); [6]:Disable audio mute output; [4]: Global audio mute; [1:0]: 1/4 audio FIFO
    msWriteByte( REG_05CD, 0x0A ); // [7:6]: audio ch 12/34/56/78, [4]: DP/HDMI audio, [3]: auto DSD detection; [1]: auto PCM detection
    msWriteByte( REG_05CF, 0xC0 ); // Fading gain = 0dB
     // I2S and S/PDIF setting
    msWriteByte( REG_05E4, 0x13 ); // [4]: New mode, [1]: Enable CTSN synthesizer clock, [0]: Enable audio FIFO read clock.
    msWriteByte( REG_05E5, 0x6A ); // [7:6]:128Fs, [5]:MCLK output, [3]:I2S encoder clock, [1]:S/PDIF encoder clock
    msWriteByte( REG_05E6, 0x39 ); // [6:5]:24bits to DAC, [4]:enable DAC 256Fs clock, [3]: I2S left-justified, [1:0]:24 bits to I2S
    msWriteByte( REG_05E7, 0x07 ); // [2]:AUMUTE pin, [1]:S/PDIF, [0]:I2S enable
    msWriteByteMask( REG_05F0, 0, BIT3 ); // [3]: AULL power down
    msWriteByte( REG_05F1, 0x0A ); // [3]:FBCLK enable, [1]:DCLK enable
#if CHIP_ID == CHIP_TSUM2
    msWriteByte( REG_05F7, 0x03 ); // [7:4]:DDIV = 2, [3:0]:FBDIV = 6
#else
    msWriteByte( REG_05F7, 0x26 ); // [7:4]:DDIV = 2, [3:0]:FBDIV = 6
#endif
    msWriteByte( REG_05F8, 0x81 ); // [7]: Enable CTS-N synthesizer, [0]: CTS[19:0]
}
#endif
//**************************************************************************
//  [Function Name]:
//                  msAudioDAC_Init()
//  [Description]
//                  Audio DAC Init
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAudioDAC_Init( void )
{
    ToggleGainCntr = 0;
    bToggleGainFlag = 0;

    /////////////de-pop sequence///////////////
    msWrite2ByteMask( REG_2C62, 0x2000, 0x2000 );//enable DAC clock, active high
    #if 1 //Jison 110419 same output w/ lineout for Dell application.
    msAudioEAR_SourceSel(E_EAROUT_FROM_LINEOUT);
    #else
    msWrite2ByteMask( REG_2C6A, 0x0400, 0x0C00 );
    msWrite2ByteMask( REG_2C6C, 0x0400, 0x0C00 );//earphone driver mux selection
    #endif
    Delay1ms(1);
    msWriteByte(REG_2C68, 0x0E); // [3]: AA switch mute, [2]: depop enable, [1:0]: AA switch source, 2'b00: line-in, 2'b10: DAC out
    msWriteByte(REG_2C69, 0x0F); // [2]: RTO, [0]: LTO
    msWriteByte(REG_2C69, 0x0A);
    msWrite2ByteMask( REG_2C62, 0x0000, 0x00F8 );
    //Power on DAC reference opamp, reference generator, audio bias circuit, DAC bias circuit, Analog output buffer
    msWrite2ByteMask( REG_2C64, 0x0000, 0x1800 );//power on left-channel and right-channel DAC
    Delay1ms(1);
    msWrite2ByteMask( REG_2C6A, 0x6006, 0x6046 );
    //[14:13]headphone trickle-charge reference opamp driving select; [6]power on headphone driver vmid buffer
    //[2:1]enable headphone driver output
    //g_u8DePOP4SecCounter=2;//delay 2 second
    ////////////////////////////////////////////

    msWriteByte(REG_2C02, 0x0f);  // software reset
    Delay1ms(1);  // delay 10ms
    msWriteByte(REG_2C02, 0x00);  // software reset

    // Audio clock config
    // [7]: Audio CH1 enable; [6]: Audio Ch1 256FS enable; [5]: Audio CH1 SDM enable; [4]: Audio CH1 MAC enable; [3]: Audio CH1 DAC enable; [2:0]: Audio CH1 SDM clock = 1/36.
    msWriteByte(REG_2C10, 0xF8);
    // [2]: 256 FS IN; [1]: CLK_128_FS_IN; [0]: 216MHz
    msWriteByte(REG_2C11, 0x00);  // clock enable

#if CHIP_ID == CHIP_TSUM2
    msWriteByte(REG_2C12, 0x0F);  // [3];DMA_CLK_EN [2]:SRC_CLK_EN  [1]:I2S_CLK_EN   [0]:SPDIF_CLK_EN
#endif

    // SRAM config
    // [3]: Clear FIFO status bits(Normal); [2]: Clear MAC status bits(Normal); [1]: Filter SRAM chip disable; [0]: Internal DATA SRAM Initialization
    msWriteByte(REG_2C36, 0x01);  // initial SRAM
    Delay1ms(1);
    msWriteByte(REG_2C36, 0x00);  // initial SRAM

    // ADAC configuration
    msWriteByte(REG_2C60, 0x27); // [5:0]: AA swtich L-ch gain
    msWriteByte(REG_2C61, 0x27); // [5:0]: AA swtich R-ch gain
    /*
    msWriteByte(REG_2C62, 0x00);
    msWriteByte(REG_2C63, 0x2F); // for earphone [3:0] = 4'b 1111, [5]DAC clock enable
    msWriteByte(REG_2C64, 0x00);
    msWriteByte(REG_2C65, 0x00);
    msWriteByte(REG_2C66, 0x00);
    msWriteByte(REG_2C67, 0x00);
    msWriteByte(REG_2C6A, 0x86); // for earphone [3:0] = 4'b 0110
    msWriteByte(REG_2C6C, 0x00);
    #if 1 //Jison 110419 same output w/ lineout for Dell application.
    msAudioEAR_SourceSel(E_EAROUT_FROM_LINEOUT);
    #else
    msWriteByte(REG_2C6B, 0x08); // from DAC
    msWriteByte(REG_2C6D, 0x08); //from DAC
    #endif
    Delay1ms(1);
    */

    // DWA config
    // [7]: DWA shift disable; [6:5]: SDM dither disable; [2]: DWA Mode2 enable; [1]: DWA output fix disable; [0]: Dither Mode2 disable
#if DAC_IDLE_TONE_IMPROVEMENT
    msWriteByte(REG_2C32, 0x05); // [0]:dither enable
#else
    msWriteByte(REG_2C32, 0x04);  // DWA dither setting
#endif



    // SDM DC offset
#if DAC_IDLE_TONE_IMPROVEMENT
    msWriteByte(REG_2C3C, 0xFF);  // DC offset
#else
    msWriteByte(REG_2C3C, 0x80);  // DC offset
#endif


    msWriteByte(REG_2C3D, 0x00);  // DC offset

    msWriteByte(REG_2C34, 0x07);  // SDM setting
    msWriteByte(REG_2C35, 0x00);  // SDM setting

    //msWriteByte(REG_2C04, 0x10);  // enable PCM gen(1KHz)
    //msWriteByte(REG_2C05, 0x02);   // enable PCM gen(1KHz)

    //[2]: fading gain of DPGA; [0]: DPGA enable
#if CHIP_ID == CHIP_TSUM2
    msWriteByteMask(REG_2C04, 0,BIT4);  //
    msWriteByteMask(REG_2C20, 0x05,0x1F);  // enable DPGA_1 (fading enable)
    msWriteByteMask(REG_2C22, 0x07,0x07);  // fading step = 1 sample(Lch)
    msWriteByteMask(REG_2C23, 0x07,0x07);  // fading step = 1 sample(Rch)
    msWriteByte(REG_2C70,0xFF);
#else
    msWriteByte(REG_2C20, 0x05);  // enable DPGA_1 (fading enable)
#endif
    //msWriteByte(0x20, 0x03);  // enable DPGA_1 (fading disable for simulate only)
    //msAudioDPGA_Mute();
    //msWriteByte(0x61, 0x30); // toggle LTO and RTO
    //msWriteByte(0x61, 0x3C); // toggle LTO and RTO

    msAudioDPGA_Mute();

    // AA-switch output depop
    Delay1ms(1);
    msWriteByte(REG_2C68, 0x0A); // [3]: AA switch mute, [1:0]: AA switch source, 2'b00: line-in, 2'b10: DAC out
    Delay1ms(100);
    msWriteByte(REG_2C68, 0x02); // [3]: AA switch mute, [1:0]: AA switch source, 2'b00: line-in, 2'b10: DAC out

    // Step size of fading in L-CH and R-CH, 16 samples.
    //msWriteByte(0x22, 0x03);
    //msWriteByte(0x23, 0x03);
}

//**************************************************************************
//  [Function Name]:
//                  msAudioDPGA_Mute()
//  [Description]
//                  Mute Line output
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAudioDPGA_Mute( void )
{
    msWrite2Byte(REG_2C2A, 0x240); // L-CH: -448
    msWrite2Byte(REG_2C2C, 0x240); // R-CH: -448
}


//**************************************************************************
//  [Function Name]:
//                  msAudioDPGA_SetVolume()
//  [Description]
//                  Set Line output volume
//  [Arguments]:
//                  chsel: LR channel selection
//                  volume: volume control(+48 ~ -112dB)
//  [Return]:
//
//**************************************************************************
void msAudioDPGA_SetVolume( AUDIO_CH_SEL chsel, int volume )
{
/* original
    if( volume > 192 ) // +48dB
        volume = 192; //maximum

    if( volume < -448 ) // -112dB
        volume = -448; //minimum
*/

    if (volume > 48)    // +48 * 0.25 dB
        volume = 48;    // maximum

    if (volume < -448)  // -448 * 0.25 dB
        volume = -448;  //minimum

    switch(chsel)
    {
        case E_AUDIO_LEFT_CH:
            msWriteByte(REG_2C2A, (volume&0xFF));
            msWriteByte(REG_2C2B, ((volume>>8)&0x03));
            break;

        case E_AUDIO_RIGHT_CH :
            msWriteByte(REG_2C2C, (volume&0xFF));
            msWriteByte(REG_2C2D, ((volume>>8)&0x03));
            break;

        case E_AUDIO_LEFT_RIGHT_CH:
        default:
            msWriteByte(REG_2C2A, (volume&0xFF));
            msWriteByte(REG_2C2B, ((volume>>8)&0x03));
            msWriteByte(REG_2C2C, (volume&0xFF));
            msWriteByte(REG_2C2D, ((volume>>8)&0x03));
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  msAudioLineout_GetSourceSel()
//  [Description]
//                  Get Line-out source selection
//  [Arguments]:
//
//  [Return]:
//                  Line-out source is from DAC or Line-out
//**************************************************************************
AUDIO_LINEOUT_SOURCE_SEL msAudioLineout_GetSourceSel(void)
{
    AUDIO_LINEOUT_SOURCE_SEL ret_val;

    ret_val = msReadByte(REG_2C68) & 0x03;

    return ret_val;
}

//**************************************************************************
//  [Function Name]:
//                  msAudioLineout_SourceSel()
//  [Description]
//                  Select Line-out source
//  [Arguments]:
//                  src: Line-in0, Line-in1, DAC
//  [Return]:
//
//**************************************************************************
void msAudioLineout_SourceSel( AUDIO_LINEOUT_SOURCE_SEL src )
{
/*
    msWriteByte( BK0_00, REGBANK5 ); // select register bank DAC
    temp = msReadByte( BK5_14 ); //check dolby audio issue  2009-08-06
    if( temp == 0x40 )
        mStar_WriteByteMask( BK5_07, 0x00, (BIT0) );
    else
        mStar_WriteByteMask( BK5_07, BIT0, (BIT0) );
*/

    if(msAudioLineout_GetSourceSel() != src )
    {
        // 091008 decrease line-out gain to -60dB step by step
        BYTE tempCnt = msReadByte( REG_2C60 ) & 0x3F;

        msAudioDPGA_Mute();

        tempCnt = ( tempCnt > 0 ) ? ( tempCnt - 1 ) : ( 0 );
        do
        {
            msWriteByte( REG_2C60, tempCnt );
            msWriteByte( REG_2C61, tempCnt );
            // it needs a rising edge to load gain immediately
            msWriteByte(REG_2C69, 0x0F);
            msWriteByte(REG_2C69, 0x0A);
            Delay1ms( 1 );
        }
        while( --tempCnt < 0x3f );

        // set line-out source
        msWriteByteMask(REG_2C68, src, 0x03);
    }
}

//**************************************************************************
//  [Function Name]:
//                  msAudioEARout_Mute()
//  [Description]
//                  Mute control earphone output
//  [Arguments]:
//                  bEanble: mute on/off
//  [Return]:
//
//**************************************************************************
void msAudioEARout_Mute( Bool EnableMute )
{
    //Jison 110419 HeadPhone Src select to Lineout, this bit can mute both analog linein and HDMI/DP digital output.
    msWriteByteMask(REG_2C68, (EnableMute?BIT3:0),BIT3);
}

//**************************************************************************
//  [Function Name]:
//                  msAudioEAR_SourceSel()
//  [Description]
//                  Select Earphone-out source
//  [Arguments]:
//                  src: Line-in0, Line-in1, Line-out, DAC
//  [Return]:
//
//**************************************************************************
void msAudioEAR_SourceSel( AUDIO_EAROUT_SOURCE_SEL src )
{
    msWriteByteMask(REG_2C6B, (BIT0<<src),0x0F);
    msWriteByteMask(REG_2C6D, (BIT0<<src),0x0F);
}

//**************************************************************************
//  [Function Name]:
//                  msAudioDACPowerDown()
//  [Description]
//                  Audio DAC Power Down Control
//  [Arguments]:
//                  bPowerDN: power down
//  [Return]:
//
//**************************************************************************
void msAudioDACPowerDown( Bool bPowerDN )
{
    ToggleGainCntr = 0;
    bToggleGainFlag = 0;

    if(bPowerDN)
    {
        msWrite2Byte(REG_2C60, 0x2727);
        msWrite2Byte(REG_2C62, 0x00FE);
        msWrite2Byte(REG_2C64, 0x1800);
        msWrite2Byte(REG_2C66, 0x0000);
        msWrite2Byte(REG_2C68, 0x2003);
        msWrite2Byte(REG_2C6A, 0x0850);
        msWrite2Byte(REG_2C6C, 0x0800);
    }
    else
    {
        msAudioDAC_Init();
    }
}
void msAudioLineOutGain( BYTE val )
{
    if( val > 0x3f )
        val = 0x3f;

    msWriteByte( REG_2C60, val );
    msWriteByte( REG_2C61, val );

    #if USE_DAC_ADJ
        msWriteByte(REG_2C69, 0x0F);
        msWriteByte(REG_2C69, 0x0A);
    #else
        ToggleGainCntr = ToggleGainPeriod;
        bToggleGainFlag = 0;
    #endif
}
void msAudioGainToggle( void )
{
    if( bToggleGainFlag )
    {
        msWriteByte(REG_2C69, 0x0F);
        msWriteByte(REG_2C69, 0x0A);
        bToggleGainFlag = 0;
    }
}

void msAudioGainForceToggle(void)
{
    msWriteByte(REG_2C69, 0x0A);
    msWriteByte(REG_2C69, 0x0F);
    msWriteByte(REG_2C69, 0x0A);
}
#endif

