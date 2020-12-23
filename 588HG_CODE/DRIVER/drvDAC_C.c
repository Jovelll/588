
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drvDAC_C.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define  DAC_DEBUG    0
#define  DAC_DePopTSUMC_V08 0

#if (ENABLE_DEBUG&&DAC_DEBUG)
#define DAC_DPUTSTR(str)        printMsg(str)
#define DAC_DPRINTF(str, x)     printData(str, x)
#else
#define DAC_DPUTSTR(str)
#define DAC_DPRINTF(str, x)
#endif

#if MS_DAC

#if DAC_DePopTSUMC_V08
		#define DAC_DELAY_STEP34    10
		#define DAC_DELAY_STEP45    800
#else
	#if DAC_support_LineoutOnly
		#define DAC_DELAY_STEP23    1000//2000
		#define DAC_DELAY_STEP34    10
		#define DAC_DELAY_STEP45     10
		#define DAC_DELAY_STEP5_1     10
		#define DAC_DELAY_STEP6     10
	#else
		#define DAC_DELAY_STEP23    1000//2000
		#define DAC_DELAY_STEP34    10
		#define DAC_DELAY_STEP45     300
		#define DAC_DELAY_STEP5_1     50
		#define DAC_DELAY_STEP6     200
	#endif
#endif

bit bToggleGainFlag = 0;
BYTE xdata ToggleGainCntr = 0;

#if ENABLE_DP_AUDIO || ENABLE_HDMI
void msAudio_I2S_SPDIF_Init(void)
{
}
#endif

#if ENABLE_DEBUG
void PCM_GEN(Bool bEnable, BYTE u8SinGenRLCHFreqSel, BYTE u8SinGenGAINSel) //high nibble for LCH,low nibble for RCH
{
    if (bEnable==TRUE)
    {
        msWrite2ByteMask( REG_2C14 ,BIT15|BIT14|BIT13                               ,BIT15|BIT14|BIT13 );    //[15] REG_SIN_GEN_PATH_SEL:0=normal path, 1=sin gen path; [14] REG_SIN_GEN_ENABLE_L:Left channel enable;  [13] REG_SIN_GEN_ENABLE_R: Right channel enable;
        msWrite2ByteMask( REG_2C14 ,(u8SinGenRLCHFreqSel>>4|u8SinGenRLCHFreqSel<<4) ,0xFF);             //[7:4] REG_SIN_GEN_FREQ_SEL_L; [3:0] REG_SIN_GEN_FREQ_SEL_R;
        msWrite2ByteMask( REG_2C16 ,(u8SinGenGAINSel|u8SinGenGAINSel<<4)        ,0xFF);             //[7:4] REG_SIN_GEN_GAIN_L; [3:0] REG_SIN_GEN_GAIN_R: Left channel gain = number*(-6dB)
        msWrite2ByteMask( REG_2C1C ,BIT15                                       ,BIT15|BIT14);      //[15] REG_DMA_SYNTH_ENABLE; [14] REG_DMA_SYNTH_CHANGE_FREQ
        msAudioLineout_SourceSel( E_LINEOUT_FROM_DMA );
    }
    else
    {
        msWrite2ByteMask( REG_2C14 ,0x00    ,BIT15|BIT14|BIT13 );    //[15] REG_SIN_GEN_PATH_SEL:0=normal path, 1=sin gen path; [14] REG_SIN_GEN_ENABLE_L:Left channel enable;  [13] REG_SIN_GEN_ENABLE_R: Right channel enable;
        msWrite2ByteMask( REG_2C14 ,0x00    ,0xFF);             //[7:4] REG_SIN_GEN_FREQ_SEL; [3:0] REG_SIN_GEN_FREQ_SEL_R;
        msWrite2ByteMask( REG_2C16 ,0x00    ,0xFF);             //[7:4] REG_SIN_GEN_GAIN_L; [3:0] REG_SIN_GEN_GAIN_R: Left channel gain = number*(-6dB)
        msWrite2ByteMask( REG_2C1C ,0x00    ,BIT15|BIT14);      //[15] REG_DMA_SYNTH_ENABLE; [14] REG_DMA_SYNTH_CHANGE_FREQ
        msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );
    }
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
#if DAC_DePopTSUMC_V08
code RegTbl2Type tDAC_INT_Step1Tbl[]=
{
    {REG_2CDC, BIT13|BIT9|BIT5|BIT4  				, BIT13|BIT9|BIT5|BIT4},
    {REG_2CDE,  BIT11  							, BIT12|BIT11|BIT10},
    {REG_2CE6,  BIT0  								, BIT0},
    {REG_2CE8,  0x00  								, BIT15|BIT14|BIT13},
    {REG_2CE0,  BIT12|BIT11|BIT10|BIT5|BIT4|BIT3 	, BIT12|BIT11|BIT10|BIT5|BIT4|BIT3},
    {REG_2CEA,  BIT1|BIT0  							, BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC,  BIT14|BIT10  						, BIT15|BIT14|BIT11|BIT10},
    {REG_2CEE,  0x00  								, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6},
    {REG_2CF0,  BIT15|BIT14|BIT13  					, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10},
};

code RegTbl2Type tDAC_INT_Step2Tbl[]=
{
    {REG_2CDC, 0x00  								, BIT13|BIT9|BIT5|BIT4},
    {REG_2CDE,  BIT11  							, BIT12|BIT11|BIT10},
    {REG_2CE6,  0x00  								, BIT0},
    {REG_2CE8,  0x00  								, BIT15|BIT14|BIT13},
    {REG_2CE0,  BIT12|BIT11|BIT10|BIT5|BIT4|BIT3 	, BIT12|BIT11|BIT10|BIT5|BIT4|BIT3},
    {REG_2CEA,  BIT1|BIT0  							, BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC,  BIT14|BIT10  						, BIT15|BIT14|BIT11|BIT10},
    {REG_2CEE,  0x00  								, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6},
    {REG_2CF0,  BIT15|BIT14|BIT13  					, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10},

};
code RegTbl2Type tDAC_INT_Step3Tbl[]=
{
    {REG_2CDC, 0x00  								, BIT13|BIT9|BIT5|BIT4},
    {REG_2CDE,  BIT12|BIT11  						, BIT12|BIT11|BIT10},
    {REG_2CE6,  0x00  								, BIT0},
    {REG_2CE8,  0x00  								, BIT15|BIT14|BIT13},
    {REG_2CE0,  BIT11|BIT10		 				, BIT12|BIT11|BIT10|BIT5|BIT4|BIT3},
    {REG_2CEA,  BIT1|BIT0  							, BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC,  BIT14|BIT10  						, BIT15|BIT14|BIT11|BIT10},
    {REG_2CEE,  0x00  								, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6},
    {REG_2CF0,  BIT15|BIT14|BIT13  					, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10},

};
code RegTbl2Type tDAC_INT_Step4Tbl[]=
{
    {REG_2CDC, 0x00  								, BIT13|BIT9|BIT5|BIT4},
    {REG_2CDE,  BIT12|BIT11  						, BIT12|BIT11|BIT10},
    {REG_2CE6,  0x00  								, BIT0},
    {REG_2CE8,  0x00  								, BIT15|BIT14|BIT13},
    {REG_2CE0,  BIT11|BIT10		 				, BIT12|BIT11|BIT10|BIT5|BIT4|BIT3},
    {REG_2CEA,  BIT1|BIT0  							, BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC,  0x00  								, BIT15|BIT14|BIT11|BIT10},
    {REG_2CEE,  0x00  								, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6},
    {REG_2CF0,  BIT15|BIT14|BIT13|BIT12|BIT11  		, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10},

};
code RegTbl2Type tDAC_INT_Step5Tbl[]=
{
    {REG_2CDC, 0x00  								, BIT13|BIT9|BIT5|BIT4},
    {REG_2CDE,  BIT12|BIT11  						, BIT12|BIT11|BIT10},
    {REG_2CE6,  0x00  								, BIT0},
    {REG_2CE8,  0x00  								, BIT15|BIT14|BIT13},
    {REG_2CE0,  BIT11|BIT10		 				, BIT12|BIT11|BIT10|BIT5|BIT4|BIT3},
    {REG_2CEA,  BIT1|BIT0  							, BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC,  0x00  								, BIT15|BIT14|BIT11|BIT10},
    {REG_2CEE,  BIT11|BIT10  						, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6},
    {REG_2CF0,  BIT15|BIT14|BIT13|BIT12|BIT11  		, BIT15|BIT14|BIT13|BIT12|BIT11|BIT10},

};
#else

code RegTbl2Type tDAC_INT_Step1Tbl[]=
{
    {REG_2CDC, 0x3FF0  , BIT15|BIT13|BIT9|BIT5|BIT4|BIT0},
#if I2S_Earout_Simultaneously
    {REG_2CDE, 0x68FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#else
    {REG_2CDE, 0x78FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#endif    
    {REG_2CE0, 0x1C30  , BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4},
    {REG_2CE6, 0x0003  , BIT0},
    {REG_2CE8, 0x8000  , BIT15|BIT14|BIT13|BIT12|BIT4|BIT1|BIT0},
    {REG_2CEA, 0x000B  , BIT10|BIT9|BIT8|BIT7|BIT6|BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC, 0x4400  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8},
#if I2S_Earout_Simultaneously
    {REG_2CEE, BIT12|0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#else    
    {REG_2CEE, 0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#endif    
    {REG_2CF0, 0xE400  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT1|BIT0},
};
code RegTbl2Type tDAC_INT_Step2Tbl[]=
{
    {REG_2CDC, 0x0000 , BIT15|BIT13|BIT9|BIT5|BIT4|BIT0},
#if I2S_Earout_Simultaneously
    {REG_2CDE, 0x68FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#else
    {REG_2CDE, 0x78FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#endif    
    {REG_2CE0, 0x1C30  , BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4},
    {REG_2CE6, 0x0002  , BIT0},
    {REG_2CE8, 0x0000  , BIT15|BIT14|BIT13|BIT12|BIT4|BIT1|BIT0},
    {REG_2CEA, 0x0003  , BIT10|BIT9|BIT8|BIT7|BIT6|BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC, 0x4400  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8},
#if I2S_Earout_Simultaneously
    {REG_2CEE, BIT12|0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#else    
    {REG_2CEE, 0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#endif    
    {REG_2CF0, 0xE400  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT1|BIT0},
};
code RegTbl2Type tDAC_INT_Step3Tbl[]=
{
    {REG_2CDC, 0x0000 , BIT15|BIT13|BIT9|BIT5|BIT4|BIT0},
#if I2S_Earout_Simultaneously
    {REG_2CDE, 0x68FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#else
    {REG_2CDE, 0x78FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#endif    
    {REG_2CE0, 0x0C00  , BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4},
    {REG_2CE6, 0x0002  , BIT0},
    {REG_2CE8, 0x0000  , BIT15|BIT14|BIT13|BIT12|BIT4|BIT1|BIT0},
    {REG_2CEA, 0x0003  , BIT10|BIT9|BIT8|BIT7|BIT6|BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC, 0x4400  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8},
#if I2S_Earout_Simultaneously
    {REG_2CEE, BIT12|0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#else    
    {REG_2CEE, 0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#endif    
    {REG_2CF0, 0xE400  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT1|BIT0},
};
code RegTbl2Type tDAC_INT_Step4Tbl[]=
{
    {REG_2CDC, 0x0000 , BIT15|BIT13|BIT9|BIT5|BIT4|BIT0},
#if I2S_Earout_Simultaneously
    {REG_2CDE, 0x68FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#else
    {REG_2CDE, 0x78FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#endif    
    {REG_2CE0, 0x0C00  , BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4},
    {REG_2CE6, 0x0002  , BIT0},
    {REG_2CE8, 0x0000  , BIT15|BIT14|BIT13|BIT12|BIT4|BIT1|BIT0},
    {REG_2CEA, 0x0003  , BIT10|BIT9|BIT8|BIT7|BIT6|BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC, 0x0000  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8},
#if I2S_Earout_Simultaneously
    {REG_2CEE, BIT12|0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#else    
    {REG_2CEE, 0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#endif    
    {REG_2CF0, 0xFC00  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT1|BIT0},
};
code RegTbl2Type tDAC_INT_Step5Tbl[]=
{
    {REG_2CDC, 0x0000 , BIT15|BIT13|BIT9|BIT5|BIT4|BIT0},
		
#if I2S_Earout_Simultaneously
    {REG_2CDE, 0x68FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#else
    {REG_2CDE, 0x78FC  , BIT15|BIT12|BIT11|BIT10|BIT9|BIT1|BIT0},
#endif    

#if CASE1_LineIn_To_I2S
    {REG_2CE0, 0x0C00|0xF0  , BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4},
#else
    {REG_2CE0, 0x0C00  , BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4},
#endif    
    {REG_2CE6, 0x0002  , BIT0},
    {REG_2CE8, 0x0000  , BIT15|BIT14|BIT13|BIT12|BIT4|BIT1|BIT0},
    {REG_2CEA, 0x0003  , BIT10|BIT9|BIT8|BIT7|BIT6|BIT3|BIT2|BIT1|BIT0},
    {REG_2CEC, 0x0000  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8},
#if I2S_Earout_Simultaneously
    {REG_2CEE, BIT12|0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#else    
    {REG_2CEE, 0x0001  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT1},
#endif    
    {REG_2CF0, 0xF800  , BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT1|BIT0},
};
#endif
void msAudioDAC_Init( void )
{
    BYTE i;
    DAC_DPUTSTR(">>>>msAudioDAC_Init START>>>>");
	//MEM_MSWRITE_BYTE_MASK(REG_022C, 0x00,BIT2);
    #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
    TIME_MEASURE_BEGIN();
    #endif			

    // DAC audio clock
    msWriteByteMask(REG_1EE0, BIT5, BIT5|BIT4);//[5]Set to 42k [4] set to 52k

    //=================================================================
    //Digital PART
    msWriteByte(REG_2C2A, 0x1F);

#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
    msWrite2Byte(REG_2C2E, 0x200);  // 55nm: 0x800, 40nm: 0x800,
#elif CHIP_ID == CHIP_TSUMD
    msWrite2Byte(REG_2C2E, 0x80);  // 55nm: 0x800, 40nm: 0x800,
#endif

#if DAC_IDLE_TONE_IMPROVEMENT
    msWrite2Byte(REG_2C2E, 0x700);  // Adjust DC offset to 0x700
    msWriteByte(REG_2C2B, 0x1F);    // Dither enable
    msWriteByte(REG_2C32,0x20);     // Set dither x 0.5
#endif

	msWrite2Byte(REG_2C20, 0x38C0);	//THRD to protect gain is overflow due to dither
	msWrite2Byte(REG_2C22, 0x3838);	//THRD to protect gain is overflow due to dither	

	//20130705 always enable dither
	msWriteByte(REG_2C2B, 0x1F);	// Dither enable
	msWriteByte(REG_2C32,0x20); 	// Set dither x 0.5

    msWriteByteMask(REG_2CE9, 0x60,0x60);
	
#if I2S_gothrough_DPGA
	msWriteByteMask(  REG_2C81, 0x00	, BIT3); // REG_2C80[11]:I2S BCK invert function
#else
	msWriteByteMask(  REG_2C81, BIT3	, BIT3); // REG_2C80[11]:I2S BCK invert function
#endif
    msWrite2ByteMask( REG_2C00, BIT13    , 0xFFFF);  //SDM/DWA Reset
    ForceDelay1ms(2);
    msWrite2ByteMask( REG_2C00, BIT15    , 0xFFFF);  //toggle reset, RD said Bit15 is used to reset all DAC part , dont need to reset each sub-part.Reset Bit13 still can hear "po" sound

    ForceDelay1ms(2);
    msWrite2ByteMask(  REG_2C00, BIT0    , 0xFFFF);  // [0] REG_AU_RESET_MASK : mask AU_RESET from SDM/DWA RESET (BACKEND_RESET) 0=mask, 1=normal
    msWrite2ByteMask(  REG_2C06, 0xFF00    , 0xFFFF);  //clock enable
    msWrite2ByteMask(  REG_2C46, 0x8000    , 0xFFFF);  //initial audioband's sram
    ForceDelay1ms(1);
    msWrite2ByteMask(  REG_2C46, 0x0000    , 0xFFFF);  //initial audioband's sram
    msWrite2ByteMask(  REG_2C46, 0x0600    , 0xFFFF);  //enable SDM/DWA
#if AUDIDO_ADC_HPF_N
    msWrite2ByteMask(  REG_2C74, 0x00F0    , 0xFFFF);   //enable high pass filter:
#else
    msWrite2ByteMask(  REG_2C74, 0x0000    , 0xFFFF);   //bypass high pass filter
#endif
    msWrite2ByteMask(  REG_2CA4, 0xFF73    , 0xFF7F);  //[15:8] DPGA_1_TIMEOUT_N; [6:4] DPGA_1_STEP=3'd7->N=1;  [2] DPGA_1_ZERO_DET_EN; [1] DPGA_1_FADING_EN;   [0] DPGA_1_DPGA_EN;
    msWriteByteMask(  REG_2C25, BIT5    , BIT5);  //[13].Fix value mode ( SDM input fixs to DC value) //REG_2C32[15:10]REG_2C30[11:0] uses to set DC value and Set it to 0

    msAudioDPGA_Mute();

    //=================================================================
    //Analog
    msWrite2ByteMask( REG_2CDA, BIT9|BIT5|BIT4      , BIT15|BIT14|BIT11|BIT9|BIT7|BIT5|BIT4|BIT1);
    msWrite2ByteMask( REG_2CE2, /*BIT5|*/BIT2       , BIT15|BIT14|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4|BIT3|BIT2);
    msWrite2ByteMask( REG_2CE4, BIT14       , BIT15|BIT14|BIT13);

    for(i=0;i<sizeof(tDAC_INT_Step1Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step1Tbl[i].wReg,tDAC_INT_Step1Tbl[i].wValue,tDAC_INT_Step1Tbl[i].wMask );
    }
    #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
    TIME_MEASURE_END();
    #endif
    ForceDelay1ms(1);	
    for(i=0;i<sizeof(tDAC_INT_Step2Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step2Tbl[i].wReg,tDAC_INT_Step2Tbl[i].wValue,tDAC_INT_Step2Tbl[i].wMask );
    }
#if DAC_DePopTSUMC_V08
    ForceDelay1ms(1);
    #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
    TIME_MEASURE_END();
    #endif
    for(i=0;i<sizeof(tDAC_INT_Step3Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step3Tbl[i].wReg,tDAC_INT_Step3Tbl[i].wValue,tDAC_INT_Step3Tbl[i].wMask );
    }
#endif	
#if 1
#if DAC_DePopTSUMC_V08
    w16DAC_DePopStep = INIT_STEP4;
    bDAC_DePopFlag = 0;
    SetDAC_DePopCounter(DAC_DELAY_STEP34);	
    //w16DAC_DePopCounter = DAC_DELAY_STEP23;
    msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );//this line could be removed and switch mux in detect.c
    #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
    TIME_MEASURE_END();
    #endif
#else
    w16DAC_DePopStep = INIT_STEP3;
    bDAC_DePopFlag = 0;
    SetDAC_DePopCounter(DAC_DELAY_STEP23);	
    //w16DAC_DePopCounter = DAC_DELAY_STEP23;
    msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );//this line could be removed and switch mux in detect.c	
#endif	
#else
    ForceDelay1ms(DELAY_STEP23);//RD suggest 2000mS
    for(i=0;i<sizeof(tDAC_INT_Step3Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step3Tbl[i].wReg,tDAC_INT_Step3Tbl[i].wValue,tDAC_INT_Step3Tbl[i].wMask );
    }

    ForceDelay1ms(DELAY_STEP34);
    for(i=0;i<sizeof(tDAC_INT_Step4Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step4Tbl[i].wReg,tDAC_INT_Step4Tbl[i].wValue,tDAC_INT_Step4Tbl[i].wMask );
    }
    msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );//this line could be removed and switch mux in detect.c

    for(i=0;i<sizeof(tDAC_INT_Step5Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step5Tbl[i].wReg,tDAC_INT_Step5Tbl[i].wValue,tDAC_INT_Step5Tbl[i].wMask );
    }
    ForceDelay1ms(DELAY_STEP5);//RD suggest 800mS
    msWriteByteMask(REG_2CEF,0x0C,0x0C);
#endif

    DAC_DPUTSTR("<<<<msAudioDAC_Init  END<<<<");
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
    //(-96 ~ +911)*-0.125dB = +12dB ~ -113.875dB (mute).
    //Gain = (signed) DPGA_GAIN_L * (-0.125dB).
    #if CheckInitialDACTime
    return;
    #endif
    msWrite2Byte(REG_2CA6, 0x38F); // L-CH: +911
    msWrite2Byte(REG_2CA8, 0x38F); // R-CH: +911
    msWriteByteMask( REG_2CA4, 0  , BIT7);  //toggle DPGA gain
    msWriteByteMask( REG_2CA4, BIT7  , BIT7);  //toggle DPGA gain
    msWriteByteMask( REG_2CA4, 0     , BIT7);
    DAC_DPRINTF("...1 msAudioDPGA_Mute==>%x",0);
}

//**************************************************************************
//  [Function Name]:
//                  msAudioDPGA_SetVolume()
//  [Description]
//                  Set Line output volume
//  [Arguments]:
//                  chsel: LR channel selection
//                  volume: volume control(+12 ~ -113.875dB)
//  [Return]:
//
//**************************************************************************
void msAudioDPGA_SetVolume( AUDIO_CH_SEL chsel, int volume ) //need check
{
    if (volume > 911)    // (+911) * -0.125dB = -113.875dB
        volume = 911;    // maximum

    if (volume < -96)  // (-96 ) * -0.125dB = +12dB
        volume = -96;  //minimum

    switch(chsel)
    {
        case E_AUDIO_LEFT_CH:
            msWriteByte(REG_2CA6, (volume&0xFF));
            msWriteByte(REG_2CA7, ((volume>>8)&0x07));
            break;

        case E_AUDIO_RIGHT_CH :
            msWriteByte(REG_2CA8, (volume&0xFF));
            msWriteByte(REG_2CA9, ((volume>>8)&0x07));
            break;

        case E_AUDIO_LEFT_RIGHT_CH:
        default:
            msWriteByte(REG_2CA6, (volume&0xFF));
            msWriteByte(REG_2CA7, ((volume>>8)&0x07));
            msWriteByte(REG_2CA8, (volume&0xFF));
            msWriteByte(REG_2CA9, ((volume>>8)&0x07));
            break;
    }

    msWrite2ByteMask( 0x2CA4, 0     , BIT7);
    msWrite2ByteMask( 0x2CA4, BIT7  , BIT7);  //toggle DPGA gain
    msWrite2ByteMask( 0x2CA4, 0     , BIT7);
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
    //[14:12] REG_CH_SEL      

    AUDIO_LINEOUT_SOURCE_SEL ret_val;

    ret_val = (msRead2Byte(REG_2C04) & (BIT14|BIT13|BIT12))>>12;

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
    if(msAudioLineout_GetSourceSel() != src )
    {
        //msAudioDPGA_Mute();

        // set line-out source
        msWrite2ByteMask(REG_2C04, (src<<12), BIT14|BIT13|BIT12);
        if(src ==0) //HDMI/DP in
            msWriteByteMask(REG_2CA1, 0x00 , BIT3|BIT2);
        else
            msWriteByteMask(REG_2CA1, BIT3 , BIT3|BIT2);
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
    EnableMute = EnableMute;
    //Jison 110419 HeadPhone Src select to Lineout, this bit can mute both analog linein and HDMI/DP digital output.
    //msWriteByteMask(REG_2C68, (EnableMute?BIT3:0),BIT3);
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
//void msAudioEAR_SourceSel( AUDIO_EAROUT_SOURCE_SEL src )
//{
//msWriteByteMask(REG_2C6B, (BIT0<<src),0x0F);
//msWriteByteMask(REG_2C6D, (BIT0<<src),0x0F);
//}

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
    if(bPowerDN)
    {
#if DAC_DePopTSUMC_V08
        DAC_DPUTSTR(">>>>msAudioDACPowerDown >>>>");

        msWrite2ByteMask(REG_2CDC,0x00					,BIT13|BIT9|BIT5|BIT4);
        msWrite2ByteMask(REG_2CDE,BIT12|BIT11				,BIT12|BIT11|BIT10);
        msWrite2ByteMask(REG_2CE6,0x00					,BIT0);
        msWrite2ByteMask(REG_2CE8,0x00					,BIT15|BIT14|BIT13);
        msWrite2ByteMask(REG_2CE0,BIT11|BIT10				,BIT12|BIT11|BIT10|BIT5|BIT4|BIT3);
        msWrite2ByteMask(REG_2CEA,BIT1|BIT0				,BIT3|BIT2|BIT1|BIT0);
        msWrite2ByteMask(REG_2CEC,0x00					,BIT15|BIT14|BIT11|BIT10);
        msWrite2ByteMask(REG_2CEE,BIT11|BIT10				,BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6);
        msWrite2ByteMask(REG_2CF0,BIT15|BIT14|BIT13		,BIT15|BIT14|BIT13|BIT12|BIT11|BIT10);
        ForceDelay1ms(1);
        msWrite2ByteMask(REG_2CE0,BIT12|BIT11|BIT10|BIT5|BIT4|BIT3		,BIT12|BIT11|BIT10|BIT5|BIT4|BIT3);
        ForceDelay1ms(1);
        msWrite2ByteMask(REG_2CDE,BIT11					,BIT12|BIT11|BIT10);
        ForceDelay1ms(1);
        msWrite2ByteMask(REG_2CDC,BIT13|BIT9|BIT5|BIT4	,BIT13|BIT9|BIT5|BIT4);
        msWrite2ByteMask(REG_2CDE,0x00					,BIT12|BIT11|BIT10);
        msWrite2ByteMask(REG_2CE6,BIT0					,BIT0);
        msWrite2ByteMask(REG_2CEC,BIT15|BIT11				,BIT15|BIT14|BIT11|BIT10);
        msWrite2ByteMask(REG_2CF0,0x00					,BIT15|BIT14|BIT13|BIT12|BIT11|BIT10);
#else

        msWriteByteMask(REG_2CEF,0x00,BIT3|BIT2);
        msWrite2ByteMask(REG_2CF0,0x00,BIT12|BIT11);

        msWriteByteMask(REG_2CE9,BIT7|BIT5,BIT7|BIT5);
        msWriteByteMask(REG_2CEE,BIT6,BIT6);

        msWrite2ByteMask(REG_2CDC,BIT13|BIT9|BIT5|BIT4,BIT13|BIT9|BIT5|BIT4);

        msWriteByteMask(REG_2CE6,BIT0,BIT0);
        msWrite2ByteMask(REG_2CE8,BIT14,BIT14);
        msWrite2ByteMask(REG_2CEE,BIT9|BIT8|BIT7,BIT9|BIT8|BIT7);
        msWriteByteMask(REG_2CEA,BIT3,BIT3);
        msWrite2ByteMask(REG_2CE0,BIT12|BIT5|BIT4,BIT12|BIT11|BIT10|BIT5|BIT4);

        ForceDelay1ms(2);
        msWrite2ByteMask(REG_2CDE,0x00,BIT12|BIT11);
        msWrite2ByteMask(REG_2CE8,0x00,BIT14|BIT13);
        msWrite2ByteMask(REG_2CEE,0x00,BIT9|BIT8|BIT7|BIT6);
        msWrite2ByteMask(REG_2CEC,BIT15|BIT11,BIT15|BIT11);
        msWrite2ByteMask(REG_2CF0,0x00,BIT15|BIT14|BIT13);
        msWriteByteMask(REG_2CDA,BIT1,BIT1);
        msWrite2ByteMask(REG_2CE2,BIT9|BIT8,BIT9|BIT8);

#endif
    }
}
void msAudioLineOutGain( BYTE val )
{
    val=val;

#if 0 //dont have AA gain
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
#endif
}
void msAudioGainToggle( void )
{
    if( bToggleGainFlag )
    {
        msWriteByteMask(REG_2CA4, BIT7, BIT7);
        msWriteByteMask(REG_2CA4, 0, BIT7);
        bToggleGainFlag = 0;
    }
}

void msAudioGainForceToggle(void)
{
    msWriteByteMask(REG_2CA4, 0, BIT7);
    msWriteByteMask(REG_2CA4, BIT7, BIT7);
    msWriteByteMask(REG_2CA4, 0, BIT7);
}

#if  0//(InitialDACByISR)
void msDACInitStepByISR( void )
{
    BYTE i;
    if( bDAC_DePopFlag == 0 )
        return;

    switch(w16DAC_DePopStep)
    {
        case INIT_STEP3:
            for(i=0;i<sizeof(tDAC_INT_Step3Tbl)/sizeof(RegTbl2Type);i++)
            {
			MEM_MSWRITE_BYTE_MASK(tDAC_INT_Step3Tbl[i].wReg,tDAC_INT_Step3Tbl[i].wValue & 0xFF,tDAC_INT_Step3Tbl[i].wMask & 0xFF );
			MEM_MSWRITE_BYTE_MASK(tDAC_INT_Step3Tbl[i].wReg+1,(tDAC_INT_Step3Tbl[i].wValue >>8) &0xFF,(tDAC_INT_Step3Tbl[i].wMask>>8) & 0xFF );
            }
            w16DAC_DePopStep = INIT_STEP4;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = DAC_DELAY_STEP34;
            SetDAC_DePopCounter(DAC_DELAY_STEP34); 
            break;

        case INIT_STEP4:
            for(i=0;i<sizeof(tDAC_INT_Step4Tbl)/sizeof(RegTbl2Type);i++)
            {
			MEM_MSWRITE_BYTE_MASK(tDAC_INT_Step4Tbl[i].wReg,tDAC_INT_Step4Tbl[i].wValue & 0xFF,tDAC_INT_Step4Tbl[i].wMask & 0xFF );
			MEM_MSWRITE_BYTE_MASK(tDAC_INT_Step4Tbl[i].wReg+1,(tDAC_INT_Step4Tbl[i].wValue >>8) &0xFF,(tDAC_INT_Step4Tbl[i].wMask>>8) & 0xFF );
            }		
            for(i=0;i<sizeof(tDAC_INT_Step5Tbl)/sizeof(RegTbl2Type);i++)
            {
			MEM_MSWRITE_BYTE_MASK(tDAC_INT_Step5Tbl[i].wReg,tDAC_INT_Step5Tbl[i].wValue & 0xFF,tDAC_INT_Step5Tbl[i].wMask & 0xFF );
			MEM_MSWRITE_BYTE_MASK(tDAC_INT_Step5Tbl[i].wReg+1,(tDAC_INT_Step5Tbl[i].wValue >>8) &0xFF,(tDAC_INT_Step5Tbl[i].wMask>>8) & 0xFF );
            }			
            w16DAC_DePopStep = INIT_STEP5;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = DAC_DELAY_STEP45;
            SetDAC_DePopCounter(DAC_DELAY_STEP45); 
            break;
			
        case INIT_STEP5:
	{
		WORD u16DC_Offset=MEM_MSREAD_BYTE(REG_2C2E)|(MEM_MSREAD_BYTE(REG_2C2F)<<8);
		WORD u16AddValue;
		#if CHIP_ID == CHIP_TSUMC
		if (u16DC_Offset <= 0x800)
		#elif CHIP_ID == CHIP_TSUMD
		if (u16DC_Offset <= 0x200)
		#endif
		{
	            w16DAC_DePopStep = INIT_STEP5;
	            bDAC_DePopFlag = 0;
	            //w16DAC_DePopCounter = DAC_DELAY_STEP5_1;
	            SetDAC_DePopCounter(DAC_DELAY_STEP5_1); 
				
		#if CHIP_ID == CHIP_TSUMC
	            u16AddValue=(0x800-0x200)/5;
		#elif CHIP_ID == CHIP_TSUMD
	            u16AddValue=(0x200-0x80)/5;
		#endif
	            MEM_MSWRITE_BYTE(REG_2C2E, u16DC_Offset+ u16AddValue);	
	            MEM_MSWRITE_BYTE(REG_2C2F, (u16DC_Offset+ u16AddValue)>>8);	
		}
		else
		{
	            w16DAC_DePopStep = INIT_STEP6;
	            bDAC_DePopFlag = 0;
	            //w16DAC_DePopCounter = DAC_DELAY_STEP6;
	            SetDAC_DePopCounter(DAC_DELAY_STEP6); 
		}
            }
            break;
        case INIT_STEP6:
	#if I2S_Earout_Simultaneously
            MEM_MSWRITE_BYTE_MASK(REG_0214, BIT2, BIT2);	//Enable AUMCK output pad control on pad GPIO00
	#endif
            MEM_MSWRITE_BYTE_MASK(REG_2CEF,  0x0C	,0x0C);
            MEM_MSWRITE_BYTE_MASK(REG_2C25, 0x00	, BIT5);  //[13].Fix value mode ( SDM input fixs to DC value) //REG_2C32[15:10]REG_2C30[11:0] uses to set DC value and Set it to 0
            w16DAC_DePopStep = INIT_FINISH;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = 0;
            SetDAC_DePopCounter(0); 
			
#if CheckInitialDACTime
            MEM_MSWRITE_BYTE_MASK(REG_2C05, (0x01<<4), BIT6|BIT5|BIT4);
            MEM_MSWRITE_BYTE_MASK(REG_2CA1, BIT3 , BIT3|BIT2);

            MEM_MSWRITE_BYTE_MASK(REG_2CA6, 0x00,0xFF);
            MEM_MSWRITE_BYTE_MASK(REG_2CA7, 0x00,0xFF);
            MEM_MSWRITE_BYTE_MASK(REG_2CA8, 0x00,0xFF);
            MEM_MSWRITE_BYTE_MASK(REG_2CA9, 0x00,0xFF);

            MEM_MSWRITE_BYTE_MASK(REG_2CA4, 0, BIT7);
            MEM_MSWRITE_BYTE_MASK(REG_2CA4, BIT7, BIT7);
            MEM_MSWRITE_BYTE_MASK(REG_2CA4, 0, BIT7);
            MEM_MSWRITE_BYTE_MASK(REG_022C, BIT2,BIT2);
#endif
            break;

        case INIT_FINISH:
            break;

        default:
            w16DAC_DePopStep = 0;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = 0;
            SetDAC_DePopCounter(0); 
			
            break;
    }
}

#else

#if DAC_DePopTSUMC_V08
#if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
extern void mStar_AdjustVolume( BYTE volume );
#endif			
void msDACInitStep( void )
{
    BYTE i;

    if( bDAC_DePopFlag == 0 )
        return;

    switch(w16DAC_DePopStep)
    {
        case INIT_STEP4:
            for(i=0;i<sizeof(tDAC_INT_Step4Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step4Tbl[i].wReg,tDAC_INT_Step4Tbl[i].wValue,tDAC_INT_Step4Tbl[i].wMask );

            w16DAC_DePopStep = INIT_STEP5;
            bDAC_DePopFlag = 0;
            SetDAC_DePopCounter(DAC_DELAY_STEP45); 
            #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
            TIME_MEASURE_END();
            #endif			
            break;
			 
        case INIT_STEP5:
            for(i=0;i<sizeof(tDAC_INT_Step5Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step5Tbl[i].wReg,tDAC_INT_Step5Tbl[i].wValue,tDAC_INT_Step5Tbl[i].wMask );

			printData("  ====     REG_2CEF==%d",msReadByte(REG_2CEF));
            msWriteByteMask(REG_2C25, 0x00	, BIT5);  //[13].Fix value mode ( SDM input fixs to DC value) //REG_2C32[15:10]REG_2C30[11:0] uses to set DC value and Set it to 0
            w16DAC_DePopStep = INIT_FINISH;
            bDAC_DePopFlag = 0;
            SetDAC_DePopCounter(0); 
            #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
            mStar_AdjustVolume( UserPrefVolume );
            msAudioGainForceToggle();
            TIME_MEASURE_END();
            #endif			
            break;

        case INIT_FINISH:
            break;

        default:
            w16DAC_DePopStep = 0;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = 0;
            SetDAC_DePopCounter(0); 
            //TIME_MEASURE_END();
			
            break;
    }
}

#else
#if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
extern void mStar_AdjustVolume( BYTE volume );
#endif			

void msDACInitStep( void )
{
    BYTE i;

    if( bDAC_DePopFlag == 0 )
        return;

    switch(w16DAC_DePopStep)
    {
        case INIT_STEP3:
            for(i=0;i<sizeof(tDAC_INT_Step3Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step3Tbl[i].wReg,tDAC_INT_Step3Tbl[i].wValue,tDAC_INT_Step3Tbl[i].wMask );

            w16DAC_DePopStep = INIT_STEP4;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = DAC_DELAY_STEP34;
            SetDAC_DePopCounter(DAC_DELAY_STEP34); 
            //TIME_MEASURE_END();
			
            break;

        case INIT_STEP4:
            for(i=0;i<sizeof(tDAC_INT_Step4Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step4Tbl[i].wReg,tDAC_INT_Step4Tbl[i].wValue,tDAC_INT_Step4Tbl[i].wMask );

            for(i=0;i<sizeof(tDAC_INT_Step5Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step5Tbl[i].wReg,tDAC_INT_Step5Tbl[i].wValue,tDAC_INT_Step5Tbl[i].wMask );

            w16DAC_DePopStep = INIT_STEP5;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = DAC_DELAY_STEP45;
            SetDAC_DePopCounter(DAC_DELAY_STEP45); 
            //TIME_MEASURE_END();
		
            break;

        case INIT_STEP5:
	{
		WORD u16DC_Offset=msRead2Byte(REG_2C2E);
		#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
		if (u16DC_Offset <= 0x800)
		#elif CHIP_ID == CHIP_TSUMD
		if (u16DC_Offset <= 0x200)
		#endif
		{
	            w16DAC_DePopStep = INIT_STEP5;
	            bDAC_DePopFlag = 0;
	            //w16DAC_DePopCounter = DAC_DELAY_STEP5_1;
	            SetDAC_DePopCounter(DAC_DELAY_STEP5_1); 
				TIME_MEASURE_END();
				
	            msWrite2Byte(REG_2C2E, u16DC_Offset+ 0x50);	
		}
		else
		{
	            w16DAC_DePopStep = INIT_STEP6;
	            bDAC_DePopFlag = 0;
	            //w16DAC_DePopCounter = DAC_DELAY_STEP6;
	            SetDAC_DePopCounter(DAC_DELAY_STEP6); 
				//TIME_MEASURE_END();
				
		}
            }
            break;

        case INIT_STEP6:
			//printData(" 6------------------------------------------==%x",CASE1_LineIn_To_I2S);
	#if I2S_Earout_Simultaneously && (CASE1_LineIn_To_I2S==0)
            msWriteByteMask(REG_0214, BIT2, BIT2);	//Enable AUMCK output pad control on pad GPIO00
	#endif
            msWriteByteMask(REG_2CEF,0x0C,0x0C);
            msWriteByteMask(  REG_2C25, 0x00    , BIT5);  //[13].Fix value mode ( SDM input fixs to DC value) //REG_2C32[15:10]REG_2C30[11:0] uses to set DC value and Set it to 0
	
            w16DAC_DePopStep = INIT_FINISH;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = 0;
            SetDAC_DePopCounter(0); 
            //TIME_MEASURE_END();
            #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
            mStar_AdjustVolume( UserPrefVolume );
            msAudioGainForceToggle();
            TIME_MEASURE_END();
            #endif			
            break;

        case INIT_FINISH:
            break;

        default:
            w16DAC_DePopStep = 0;
            bDAC_DePopFlag = 0;
            //w16DAC_DePopCounter = 0;
            SetDAC_DePopCounter(0); 
            //TIME_MEASURE_END();
			
            break;
    }
}
#endif
#endif
#endif

