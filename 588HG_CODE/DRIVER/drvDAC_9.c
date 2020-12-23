

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drvDAC_9.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define  DAC_DEBUG    0

#if (ENABLE_DEBUG&&DAC_DEBUG)
#define DAC_DPUTSTR(str)        printMsg(str)
#define DAC_DPRINTF(str, x)     printData(str, x)
#else
#define DAC_DPUTSTR(str)
#define DAC_DPRINTF(str, x)
#endif


#if MS_DAC
		#define DAC_DELAY_STEP23    1
		#define DAC_DELAY_STEP34    2000
		#define DAC_DELAY_STEP45     50
		#define DAC_DELAY_STEP56     900
		#define DAC_DELAY_STEP67     50

bit bToggleGainFlag = 0;
BYTE xdata ToggleGainCntr = 0;

#if ENABLE_DP_AUDIO || ENABLE_HDMI
void msAudio_I2S_SPDIF_Init(void)
{
#if MS_DAC
	msAudioDPGA_Mute();
#endif

#if USE_CASE_SEL == USE_CASE_4	
	//SPDIF
    msWriteByteMask(REG_0205, 0x00 ,BIT7|BIT5);//Enable S/PDIF output pad control on SPDIFO_AU ([7]:GPIO_AU5, [5]:GPIO01).
    msWriteByteMask(REG_0206, 0x00 ,BIT2|BIT0);//Enable S/PDIF output pad control on SPDIFO ([2]:GPIO_AU5, [0]:GPIO01).
    //I2S
    msWriteByteMask(REG_0207, 0x00 ,0x70); //[6]AUMCK on GPIO_AU4, [5]:AUBCK on GPIO_AU1, AUSD on GPIO_AU2 and AUWS on GPIO_AU3, [4] AUMUTE on GPIO_AU0
    msWriteByteMask(REG_0215, 0x70 ,0x70); //[6]AUMCK on GPIO00, [5]:AUBCK on GPIO01, AUSD on GPIO02 and AUWS on GPIO_AU8, [4] AUMUTE on GPIO_AU2
#elif USE_CASE_SEL == USE_CASE_3	
	//SPDIF
    msWriteByteMask(REG_0205, BIT5 ,BIT7|BIT5);//Enable S/PDIF output pad control on SPDIFO_AU (GPIO01).
    msWriteByteMask(REG_0206, 0x00 ,BIT2|BIT0);
    //I2S
    msWriteByteMask(REG_0207, 0x30 ,0x70);//[5]:AUBCK on GPIO_AU1, AUSD on GPIO_AU2 and AUWS on GPIO_AU3, [4] AUMUTE on GPIO_AU0
    msWriteByteMask(REG_0215, 0x40 ,0x70); //[6]AUMCK on GPIO00
#elif USE_CASE_SEL == USE_CASE_2	
	//SPDIF
    msWriteByteMask(REG_0205, BIT7 ,BIT7|BIT5);//Enable S/PDIF output pad control on SPDIFO_AU (GPIO_AU5).
    msWriteByteMask(REG_0206, 0x00 ,BIT2|BIT0);
    //I2S
    msWriteByteMask(REG_0207, 0x70 ,0x70); //[6]AUMCK on GPIO_AU4, [5]:AUBCK on GPIO_AU1, AUSD on GPIO_AU2 and AUWS on GPIO_AU3, [4] AUMUTE on GPIO_AU0
    msWriteByteMask(REG_0215, 0x00 ,0x70);

#elif USE_CASE_SEL == USE_CASE_1
	//SPDIF
    msWriteByteMask(REG_0205, BIT5 ,BIT7|BIT5);//Enable S/PDIF output pad control on SPDIFO_AU (GPIO01).
    msWriteByteMask(REG_0206, 0x00 ,BIT2|BIT0);
    //I2S
    msWriteByteMask(REG_0207, 0x00 ,0x70);
    msWriteByteMask(REG_0215, 0x00 ,0x70);
#else //case 5
	//SPDIF
    msWriteByteMask(REG_0205, 0x00 ,BIT7|BIT5);//disable S/PDIF output pad control on SPDIFO_AU (GPIO01).
    msWriteByteMask(REG_0206, 0x00 ,BIT2|BIT0);
    //I2S
    msWriteByteMask(REG_0207, 0x00 ,0x70);
    msWriteByteMask(REG_0215, 0x00 ,0x70);
#endif
#if MS_DAC
#if AudioIC_CS8003_Enable
    msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x00 );   //111118 Modify
 #endif
#endif
}
#endif

#if ENABLE_DEBUG
//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msDrvAudioSineGen()
/// @brief \b Function \b Description: Audio internal sine gen
/// @param <IN>        \b bEnable, u8SinGenFreq(250Hz~12000Hz), u8SinGenGAIN(-90~0db)
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void PCM_GEN(Bool bEnable, BYTE u8SinGenRLCHFreqSel, BYTE u8SinGenGAINSel) //high nibble for LCH,low nibble for RCH
{
	//00 : PCM_250Hz,
	//01 : PCM_500HZ,
	//02 : PCM_1KHZ,
	//03 : PCM_1500HZ,
	//04 : PCM_2KHZ,
	//05 : PCM_3KHZ,
	//06 : PCM_4KHZ,
	//07 : PCM_6KHZ,
	//08 : PCM_8KHZ,
	//09 : PCM_12KHZ,
	//else : PCM_16KHZ

    BYTE u8BackupSourceSEL;

    u8BackupSourceSEL=(msRead2Byte(REG_2C04)>>12)&0x03;
    msWrite2ByteMask(REG_2C04, ((bEnable)?(E_LINEOUT_FROM_DMA<<12):(u8BackupSourceSEL<<12)), BIT14|BIT13|BIT12);  //inmux switch
    msWrite2ByteMask(REG_2C1C ,((bEnable)?(BIT15):(0)),BIT15|BIT14);      //[15] REG_DMA_SYNTH_ENABLE; [14] REG_DMA_SYNTH_CHANGE_FREQ

    msWrite2ByteMask(REG_2C14,(u8SinGenRLCHFreqSel<<4)|(u8SinGenGAINSel),BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    msWrite2ByteMask(REG_2C16,(u8SinGenGAINSel<<4)|(u8SinGenGAINSel),BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    msWrite2ByteMask(REG_2C14, ((bEnable)?(BIT15|BIT14|BIT13):(0))    ,BIT15|BIT14|BIT13);
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
code RegTbl2Type tDAC_INT_Step1Tbl[]=
{
	{REG_2A00,    (BIT0)  	,BIT0},   	//PD_OPLP 
	{REG_2A00,    (BIT1)  	,BIT1},     //PD_VREF 
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB
	
	{REG_2CDA,    (BIT1)	,BIT1},     //PD_L0_DAC
	{REG_2CDA,    (BIT5)	,BIT5},     //PD_R0_DAC
	{REG_2CDA,    (BIT9) 	,BIT9},     //PD_REF_DAC
	{REG_2CDA,    (BIT10)	,BIT10}, 	//PD_BIAS_DAC
	{REG_2CDA,    (BIT12)	,BIT12}, 	//PD_IBIAS_EAR
	{REG_2CDA,    (BIT13)	,BIT13}, 	//PD_RT_EAR
	{REG_2CDA,    (BIT14)	,BIT14}, 	//PD_LT_EAR
	
	{REG_2CDC,    (BIT3)  	,BIT3},     //PD_VI            
	{REG_2CDC,    (BIT5)  	,BIT5},     //PD_LDO_ADC       
	{REG_2CDC,    (BIT6)  	,BIT6},     //PD_LDO_DAC       
	{REG_2CDC,    (0x00)  	,BIT7},     //QS_LDO_ADC       
	{REG_2CDC,    (0x00)  	,BIT8},     //QS_LDO_DAC       

	{REG_2CDE,    (0x00)    ,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (0x00)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)    ,BIT7},    			//SEL_IN_EAR	
	{REG_2CDE,    (0x00)    ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR
	
	{REG_2CE0     ,0x00    ,0xFF},
	{REG_2CE1     ,0x10    ,0xFF},
	{REG_2CE2     ,0x02    ,0xFF},
	{REG_2CE3     ,0x00    ,0xFF},

	{REG_2CE4,    (0x00)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (0x00)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (0x00)    ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (0x00)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (0x00)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (BIT3)    ,BIT3},     //EN_VMID2GND

	{REG_2CE8     ,0x00    ,0x0F},
	{REG_2CE9     ,0x00    ,BIT1},
	{REG_2CE9     ,0x00    ,0x7C},
};
code RegTbl2Type tDAC_INT_Step2Tbl[]=
{
	{REG_2A00,    (BIT0)  	,BIT0},   	//PD_OPLP 
	{REG_2A00,    (BIT1)  	,BIT1},     //PD_VREF 
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (BIT1)	,BIT1},     //PD_L0_DAC
	{REG_2CDA,    (BIT5)	,BIT5},     //PD_R0_DAC
	{REG_2CDA,    (BIT9) 	,BIT9},     //PD_REF_DAC
	{REG_2CDA,    (BIT10)	,BIT10}, 	//PD_BIAS_DAC
	{REG_2CDA,    (BIT12)	,BIT12}, 	//PD_IBIAS_EAR
	{REG_2CDA,    (BIT13)	,BIT13}, 	//PD_RT_EAR
	{REG_2CDA,    (BIT14)	,BIT14}, 	//PD_LT_EAR

	{REG_2CDC,    (BIT3)  	,BIT3},     //PD_VI             
	{REG_2CDC,    (0x00)  	,BIT5},     //PD_LDO_ADC        
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC        
	{REG_2CDC,    (BIT7)  	,BIT7},     //QS_LDO_ADC        
	{REG_2CDC,    (BIT8)  	,BIT8},     //QS_LDO_DAC        

 	{REG_2CDE,    (0x00)    ,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (0x00)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)    ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR
	{REG_2CDE,    (0x00)    ,BIT7},    			//SEL_IN_EAR	
	
	{REG_2CE0     ,0x00    ,0xFF},
	{REG_2CE1     ,0x10    ,0xFF},
	{REG_2CE2     ,0x02    ,0xFF},
	{REG_2CE3     ,0x00    ,0xFF},

	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (0x00)    ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (0x00)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (0x00)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (BIT3)    ,BIT3},     //EN_VMID2GND
	
	{REG_2CE8     ,0x00    ,0x0F},
	{REG_2CE9     ,0x00    ,BIT1},
	{REG_2CE9     ,0x00    ,0x7C},
};
code RegTbl2Type tDAC_INT_Step3Tbl[]=
{
	{REG_2A00,    (0x00)  	,BIT0},   	//PD_OPLP 
	{REG_2A00,    (0x00)  	,BIT1},     //PD_VREF 
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (0x00)	,BIT1},     //PD_L0_DAC                
	{REG_2CDA,    (0x00)	,BIT5},     //PD_R0_DAC                
	{REG_2CDA,    (0x00) 	,BIT9},     //PD_REF_DAC               
	{REG_2CDA,    (0x00)	,BIT10}, 	//PD_BIAS_DAC                
	{REG_2CDA,    (BIT12)	,BIT12}, 	//PD_IBIAS_EAR               
	{REG_2CDA,    (BIT13)	,BIT13}, 	//PD_RT_EAR                  
	{REG_2CDA,    (BIT14)	,BIT14}, 	//PD_LT_EAR                  

	{REG_2CDC,    (0x00)  	,BIT3},     //PD_VI             
	{REG_2CDC,    (0x00)  	,BIT5},     //PD_LDO_ADC        
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC        
	{REG_2CDC,    (BIT7)  	,BIT7},     //QS_LDO_ADC        
	{REG_2CDC,    (BIT8)  	,BIT8},     //QS_LDO_DAC        

	{REG_2CDE,    (BIT2|BIT1)     		,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (BIT6|BIT5|BIT4)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)     ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR
	{REG_2CDE,    (0x00)     ,BIT7},    			//SEL_IN_EAR	
	
	{REG_2CE0     ,0x00    ,0xFF},
	{REG_2CE1     ,0x10    ,0xFF},
	{REG_2CE2     ,0x02    ,0xFF},
	{REG_2CE3     ,0x00    ,0xFF},

	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (BIT15)   ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (BIT0)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (BIT1)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND
	
	{REG_2CE8     ,0x00    ,0x0F},
	{REG_2CE9     ,0x00    ,BIT1},
	{REG_2CE9     ,0x00    ,0x7C},
};
code RegTbl2Type tDAC_INT_Step4Tbl[]=
{
	{REG_2A00,    (0x00)  	,BIT0},   	//PD_OPLP 
	{REG_2A00,    (0x00)  	,BIT1},     //PD_VREF 
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (0x00)	,BIT1},     //PD_L0_DAC   
	{REG_2CDA,    (0x00)	,BIT5},     //PD_R0_DAC   
	{REG_2CDA,    (0x00) 	,BIT9},     //PD_REF_DAC  
	{REG_2CDA,    (0x00)	,BIT10}, 	//PD_BIAS_DAC   
	{REG_2CDA,    (0x00)	,BIT12}, 	//PD_IBIAS_EAR  
	{REG_2CDA,    (0x00)	,BIT13}, 	//PD_RT_EAR    
#if USE_CASE_SEL == USE_CASE_4	
	{REG_2CDA,    (BIT14) 	,BIT14},   	//PD_LT_EAR
#else
	{REG_2CDA,    (0x00)	,BIT14}, 	//PD_LT_EAR     
#endif
	{REG_2CDC,    (0x00)  	  ,BIT3},     //PD_VI         
	{REG_2CDC,    (0x00)  	  ,BIT5},     //PD_LDO_ADC    
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC      
	{REG_2CDC,    (BIT7)  	,BIT7},     //QS_LDO_ADC      
	{REG_2CDC,    (BIT8)  	,BIT8},     //QS_LDO_DAC      

	{REG_2CDE,    (BIT2|BIT1)     		,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (BIT6|BIT5|BIT4)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)     ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR
#if USE_CASE_SEL == USE_CASE_3	
	{REG_2CDE,    (BIT7)     ,BIT7},    			//SEL_IN_EAR	
#else	
	{REG_2CDE,    (0x00)     ,BIT7},    			//SEL_IN_EAR	
#endif	
	{REG_2CE0     ,0x00    ,0xFF},
	{REG_2CE1     ,0x10    ,0xFF},
	{REG_2CE2     ,0x02    ,0xFF},
	{REG_2CE3     ,0x00    ,0xFF},

	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (BIT15)   ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (BIT0)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (BIT1)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND
	
	{REG_2CE8     ,0x00    ,0x0F},
	{REG_2CE9     ,0x00    ,BIT1},
	{REG_2CE9     ,0x00    ,0x7C},
};

code RegTbl2Type tDAC_INT_Step5Tbl[]=
{
	{REG_2A00,    (0x00)  	,BIT0},   	//PD_OPLP 
	{REG_2A00,    (0x00)  	,BIT1},     //PD_VREF 
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (0x00)	,BIT1},     //PD_L0_DAC       
	{REG_2CDA,    (0x00)	,BIT5},     //PD_R0_DAC       
	{REG_2CDA,    (0x00) 	,BIT9},     //PD_REF_DAC      
	{REG_2CDA,    (0x00)	,BIT10}, 	//PD_BIAS_DAC       
	{REG_2CDA,    (0x00)	,BIT12}, 	//PD_IBIAS_EAR      
	{REG_2CDA,    (0x00)	,BIT13}, 	//PD_RT_EAR      
#if USE_CASE_SEL == USE_CASE_4	
	{REG_2CDA,    (BIT14) 	,BIT14},   	//PD_LT_EAR
#else
	{REG_2CDA,    (0x00)	,BIT14}, 	//PD_LT_EAR    
#endif
	{REG_2CDC,    (0x00)  	,BIT3},     //PD_VI           
	{REG_2CDC,    (0x00)  	,BIT5},     //PD_LDO_ADC      
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC      
	{REG_2CDC,    (0x00)  	,BIT7},     //QS_LDO_ADC      
	{REG_2CDC,    (0x00)  	,BIT8},     //QS_LDO_DAC      

	{REG_2CDE,    (BIT2|BIT1)     		,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (BIT6|BIT5|BIT4)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)     ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR
#if USE_CASE_SEL == USE_CASE_3	
	{REG_2CDE,    (BIT7)     ,BIT7},    			//SEL_IN_EAR	
#else	
	{REG_2CDE,    (0x00)     ,BIT7},    			//SEL_IN_EAR	
#endif
	{REG_2CE0     ,0x00    ,0xFF},
	{REG_2CE1     ,0x10    ,0xFF},
	{REG_2CE2     ,0x02    ,0xFF},
	{REG_2CE3     ,0x00    ,0xFF},

	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (BIT15)   ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (BIT0)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (BIT1)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND

	{REG_2CE8     ,0x0C    ,0x0F},
	{REG_2CE9     ,0x00    ,BIT1},
	{REG_2CE9     ,0x00    ,0x7C},
};
code RegTbl2Type tDAC_INT_Step6Tbl[]=
{
	{REG_2A00,    (0x00)  	,BIT0},   	//PD_OPLP 
	{REG_2A00,    (0x00)  	,BIT1},     //PD_VREF 
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (0x00)	,BIT1},     //PD_L0_DAC      
	{REG_2CDA,    (0x00)	,BIT5},     //PD_R0_DAC      
	{REG_2CDA,    (0x00) 	,BIT9},     //PD_REF_DAC     
	{REG_2CDA,    (0x00)	,BIT10}, 	//PD_BIAS_DAC      
	{REG_2CDA,    (0x00)	,BIT12}, 	//PD_IBIAS_EAR     
	{REG_2CDA,    (0x00)	,BIT13}, 	//PD_RT_EAR      
#if USE_CASE_SEL == USE_CASE_4	
	{REG_2CDA,    (BIT14) 	,BIT14},   	//PD_LT_EAR
#else
	{REG_2CDA,    (0x00)	,BIT14}, 	//PD_LT_EAR  
#endif	
	{REG_2CDC,    (0x00)  	,BIT3},     //PD_VI            
	{REG_2CDC,    (0x00)  	,BIT5},     //PD_LDO_ADC       
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC       
	{REG_2CDC,    (0x00)  	,BIT7},     //QS_LDO_ADC       
	{REG_2CDC,    (0x00)  	,BIT8},     //QS_LDO_DAC       

	{REG_2CDE,    (BIT2|BIT1)     		,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (BIT6|BIT5|BIT4)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)     ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR
#if USE_CASE_SEL == USE_CASE_3	
	{REG_2CDE,    (BIT7)     ,BIT7},    			//SEL_IN_EAR	
#else	
	{REG_2CDE,    (0x00)     ,BIT7},    			//SEL_IN_EAR
#endif
	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (0x00)    ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (BIT0)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (BIT1)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND

	{REG_2CE8     ,0x0F    ,0x0F},
	{REG_2CE9     ,0x00    ,BIT1},
	{REG_2CE9     ,0x00    ,0x7C},
};

code RegTbl2Type tDAC_INT_Step7Tbl[]=
{
	{REG_2A00,    (0x00)  	,BIT0},   	//PD_OPLP 
};
code RegUnitType tblHydanInit[]=
{
	{    REG_0205     ,0x00   }, //CLOCK ENABLE
	
    //{    REG_0206     ,0x00   },
    //{    REG_0207     ,0x00   },
	{    REG_0215     ,0x00   },
	
	{    REG_2CA0     ,0x00   }, //DPGA
	{    REG_2CA1     ,0x08   },

	{    REG_2C06     ,0xBE   }, //CLOCK ENABLE
	{    REG_2C07     ,0x9F   },
	{    REG_2C46     ,0x00   }, //AUDIOBAND SRAM INITIAL
	{    REG_2C47     ,0x86   },
	{    REG_2C46     ,0x00   },
	{    REG_2C47     ,0x06   },

#if (CHIP_ID == CHIP_TSUM9)
	{    REG_2C6E     ,0xD3   }, //2ND-SYNTHESIZER SETTING
	{    REG_2C6F     ,0x8B   },
	{    REG_2C6C     ,0x00   },
	{    REG_2C6D     ,0xC0   },
	{    REG_2C60     ,0x60   }, //AUPLL PD
	{    REG_2C61     ,0x00   },
#elif (CHIP_ID == CHIP_TSUMF)
	{    REG_2C6E     ,0xF7   }, //2ND-SYNTHESIZER SETTING
	{    REG_2C6F     ,0x1B   }, // reference colock = 429MHz
	{    REG_2C6C     ,0x00   },
	{    REG_2C6D     ,0xC0   },
	{    REG_2C60     ,0x06   }, //AUPLL_FBDIV
	{    REG_2C61     ,0x00   }, //AUPLL PD
#endif

	//{    REG_2A00     ,0x00   }, //AUSDM
	//{    REG_2A01     ,0x00   },
	{    REG_2CDA     ,0x00   },
	//{    REG_2CDB     ,0x00   },
	{    REG_2CDC     ,0x80   },
	//{    REG_2CDD     ,0x01   },
	//{    REG_2CDE     ,0x00   },
	//{    REG_2CDF     ,0x00   },
	{    REG_2CE0     ,0x00   },
	{    REG_2CE1     ,0x10   },
	{    REG_2CE2     ,0x02   },
	{    REG_2CE3     ,0x0E   },
	//{    REG_2CE4     ,0x41   },
	{    REG_2CE5     ,0x00   },
	{    REG_2CE6     ,0x03   },
	//{    REG_2CE7     ,0x00   },
	//{    REG_2CE8     ,0x0F   },
	//{    REG_2CE9     ,0x00   },
	{    REG_2CEA     ,0x00   },
	{    REG_2CEB     ,0x00   },
	{    REG_2C00     ,0xFF    },//HAYDN RESET
	{    REG_2C01     ,0xFF   },
	{    REG_2C00     ,0x00   },
	{    REG_2C01     ,0x00   },
	{    REG_2C06     ,0xBE   }, //CLOCK ENABLE
	{    REG_2C07     ,0xFF   },
	{    REG_2C46     ,0x00   },
	{    REG_2C47     ,0x00   },
	{    REG_2C04     ,0x00   },
	{    REG_2C05     ,0x10   },
	{    REG_2C08     ,0x00|BIT1   }, //I2S TX
	{    REG_2C09     ,0x00   },
	{    REG_2C80     ,0x00   },
	{    REG_2C81     ,0x20   }, //[15]REG_I2S_OUT_FMT 0=I2S justified, 1=left justified  [14:13] REG_I2S_BCK_SEL : data width selection 0=16bit, 1=24bit, 2=32bit, 3=reserved
	//{    REG_2C81     ,0xA0   },
	{    REG_2C82     ,0x02   },
	{    REG_2C83     ,0x00   },
	{    REG_2C8A     ,0x00|BIT4   }, //SPDIF TX
	{    REG_2C8B     ,0x00   },
	{    REG_2C46     ,0x00   }, //SDM ENABLE
	{    REG_2C47     ,0x06   },
	{    REG_2C06     ,0xFE   }, //DECIMATION FILTER CLOCK ENABLE
	{    REG_2C07     ,0xFF   },
	{    REG_2C74     ,0x0F   }, //ADC HPF
	{    REG_2C75     ,0x00   },
	//{    REG_2CA6     ,0xFC   },
	//{    REG_2CA7     ,0x0F   },
	//{    REG_2CA8     ,0xFC   },
	//{    REG_2CA9     ,0x0F   },
	#if ENABLE_HAYDN_FADING_BY_HDMI_STATUS
	{    REG_2CA4     ,0x73   },  //DPGA setting 
	//{    REG_2CA1     ,0x00   },   
	{    REG_2CA2     ,0x63   },  //[6:4]unmute fading step [2:0]mute fading step 7->~6.xms 0->~6xxms 
	#else
	{    REG_2CA4     ,0x77   },  //DPGA setting 
	#endif
	{    REG_2CA5     ,0xFF   },
	//SPDIF initial sequence
	{    REG_2C88     ,0x03   },
	{    REG_2C88     ,0x00   },
	{    REG_2C88     ,0x20   },
};

code RegTbl2Type tDAC_PD_Step1Tbl[]=
{
	{REG_2A00,    (0x00)  	,BIT0},   	//PD_OPLP
	{REG_2A00,    (0x00)  	,BIT1},     //PD_VREF
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (0x00)	,BIT1},     //PD_L0_DAC
	{REG_2CDA,    (0x00)	,BIT5},     //PD_R0_DAC
	{REG_2CDA,    (0x00) 	,BIT9},     //PD_REF_DAC
	{REG_2CDA,    (0x00)	,BIT10}, 	//PD_BIAS_DAC
	{REG_2CDA,    (0x00)	,BIT12}, 	//PD_IBIAS_EAR
	{REG_2CDA,    (0x00)	,BIT13}, 	//PD_RT_EAR
	{REG_2CDA,    (0x00)	,BIT14}, 	//PD_LT_EAR

	{REG_2CDC,    (0x00)  	,BIT3},     //PD_VI
	{REG_2CDC,    (0x00)  	,BIT5},     //PD_LDO_ADC
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC
	{REG_2CDC,    (0x00)  	,BIT7},     //QS_LDO_ADC
	{REG_2CDC,    (0x00)  	,BIT8},     //QS_LDO_DAC

	{REG_2CDE,    (BIT2|BIT1)    ,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (0x00)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)    ,BIT7},    			//SEL_IN_EAR
	{REG_2CDE,    (0x00)    ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR

	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (0x00)    ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (BIT0)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (BIT1)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND

	{REG_2CE8     ,(0x0C)    ,0x0F},
	{REG_2CE9     ,(0x00)    ,BIT1},
	{REG_2CE9     ,(0x00)    ,0x7C},
};
code RegTbl2Type tDAC_PD_Step2Tbl[]=
{
	{REG_2A00,    (0x00)  	,BIT0},   	//PD_OPLP
	{REG_2A00,    (BIT1)  	,BIT1},     //PD_VREF
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (0x00)	,BIT1},     //PD_L0_DAC
	{REG_2CDA,    (0x00)	,BIT5},     //PD_R0_DAC
	{REG_2CDA,    (0x00) 	,BIT9},     //PD_REF_DAC
	{REG_2CDA,    (0x00)	,BIT10}, 	//PD_BIAS_DAC
	{REG_2CDA,    (0x00)	,BIT12}, 	//PD_IBIAS_EAR
	{REG_2CDA,    (0x00)	,BIT13}, 	//PD_RT_EAR
	{REG_2CDA,    (0x00)	,BIT14}, 	//PD_LT_EAR

	{REG_2CDC,    (0x00)  	,BIT3},     //PD_VI
	{REG_2CDC,    (0x00)  	,BIT5},     //PD_LDO_ADC
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC
	{REG_2CDC,    (0x00)  	,BIT7},     //QS_LDO_ADC
	{REG_2CDC,    (0x00)  	,BIT8},     //QS_LDO_DAC

	{REG_2CDE,    (BIT2|BIT1)    ,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (0x00)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)    ,BIT7},    			//SEL_IN_EAR
	{REG_2CDE,    (0x00)    ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR

	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (0x00)    ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (BIT0)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (BIT1)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND

	{REG_2CE8     ,(0x0C)    ,0x0F},
	{REG_2CE9     ,(0x00)    ,BIT1},
	{REG_2CE9     ,(0x0C)    ,0x7C},
};
code RegTbl2Type tDAC_PD_Step3Tbl[]=
{
	{REG_2A00,    (BIT0)  	,BIT0},   	//PD_OPLP
	{REG_2A00,    (BIT1)  	,BIT1},     //PD_VREF
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (BIT1)	,BIT1},     //PD_L0_DAC
	{REG_2CDA,    (BIT5)	,BIT5},     //PD_R0_DAC
	{REG_2CDA,    (BIT9) 	,BIT9},     //PD_REF_DAC
	{REG_2CDA,    (BIT10)	,BIT10}, 	//PD_BIAS_DAC
	{REG_2CDA,    (BIT12)	,BIT12}, 	//PD_IBIAS_EAR
	{REG_2CDA,    (BIT13)	,BIT13}, 	//PD_RT_EAR
	{REG_2CDA,    (BIT14)	,BIT14}, 	//PD_LT_EAR

	{REG_2CDC,    (BIT3)  	,BIT3},     //PD_VI
	{REG_2CDC,    (0x00)  	,BIT5},     //PD_LDO_ADC
	{REG_2CDC,    (0x00)  	,BIT6},     //PD_LDO_DAC
	{REG_2CDC,    (0x00)  	,BIT7},     //QS_LDO_ADC
	{REG_2CDC,    (0x00)  	,BIT8},     //QS_LDO_DAC

	{REG_2CDE,    (BIT2|BIT1)    ,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (0x00)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)    ,BIT7},    			//SEL_IN_EAR
	{REG_2CDE,    (0x00)    ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR

	{REG_2CE4,    (BIT6)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (BIT0)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (0x00)    ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (0x00)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (0x00)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND

	{REG_2CE8     ,(0x00)    ,0x0F},
	{REG_2CE9     ,(0x02)    ,BIT1},
	{REG_2CE9     ,(0x7C)    ,0x7C},
};
code RegTbl2Type tDAC_PD_Step4Tbl[]=
{
	{REG_2A00,    (BIT0)  	,BIT0},   	//PD_OPLP
	{REG_2A00,    (BIT1)  	,BIT1},     //PD_VREF
	{REG_2A00,    (0x00)    ,BIT2},     //SEL_DEPOP_SB

	{REG_2CDA,    (BIT1)	,BIT1},     //PD_L0_DAC
	{REG_2CDA,    (BIT5)	,BIT5},     //PD_R0_DAC
	{REG_2CDA,    (BIT9) 	,BIT9},     //PD_REF_DAC
	{REG_2CDA,    (BIT10)	,BIT10}, 	//PD_BIAS_DAC
	{REG_2CDA,    (BIT12)	,BIT12}, 	//PD_IBIAS_EAR
	{REG_2CDA,    (BIT13)	,BIT13}, 	//PD_RT_EAR
	{REG_2CDA,    (BIT14)	,BIT14}, 	//PD_LT_EAR

	{REG_2CDC,    (BIT3)  	,BIT3},     //PD_VI
	{REG_2CDC,    (BIT5)  	,BIT5},     //PD_LDO_ADC
	{REG_2CDC,    (BIT6)  	,BIT6},     //PD_LDO_DAC
	{REG_2CDC,    (0x00)  	,BIT7},     //QS_LDO_ADC
	{REG_2CDC,    (0x00)  	,BIT8},     //QS_LDO_DAC

	{REG_2CDE,    (0x00)    ,BIT2|BIT1},    		//SEL_TC_EAR[1:0]
	{REG_2CDE,    (0x00)   	,BIT6|BIT5|BIT4},   	//SEL_DEPOP_EAR
	{REG_2CDE,    (0x00)    ,BIT7},    			//SEL_IN_EAR
	{REG_2CDE,    (0x00)    ,BIT10|BIT9|BIT8},    	//SEL_IN_EAR

	{REG_2CE4,    (0x00)    ,BIT6},     //EN_SW_DAC
	{REG_2CE4,    (0x00)    ,BIT0},     //EN_CK_DAC
	{REG_2CE4,    (0x00)    ,BIT15}, 	//EN_MSP

	{REG_2CE6,    (0x00)    ,BIT0},     //EN_INSWL_EAR
	{REG_2CE6,    (0x00)    ,BIT1},     //EN_INSWR_EAR
	{REG_2CE6,    (0x00)    ,BIT3},     //EN_VMID2GND

	{REG_2CE8     ,(0x00)    ,0x0F},
	{REG_2CE9     ,(0x00)    ,BIT1},
	{REG_2CE9     ,(0x00)    ,0x7C},
};

void msdrvDecimationFilter( void )
{
    msWriteByteMask( REG_2C06, BIT6  , BIT6); //decimation filter clock enable
    msWriteByteMask( REG_2C08, BIT6  , BIT6); //select decimation clock from 120FS   

    msWriteByteMask( REG_2C4C, BIT4  , BIT4); //deciatmion synthesizer reference clock enable
    msWrite2Byte( REG_2C4E, 0x00F0); //M/N value
    msWrite2Byte( REG_2C50, 0x0600); //M/N value
	//M/N value update
    msWriteByteMask( REG_2C4C, 0  		, BIT0); 
    msWriteByteMask( REG_2C4C, BIT0  	, BIT0); 
    msWriteByteMask( REG_2C4C, 0  		, BIT0); 

    msWriteByteMask( REG_2D40, BIT0  	, BIT0); //Decimation Gain compensation clock enable

    //msWriteByteMask( REG_2D40, 0x00  	, BIT0); //Decimation Gain compensation clock enable
	
    msWrite2Byte( REG_2D42, 0x133&0x3FF); //GC gain setting(1.6dB)
    msWrite2Byte( REG_2D44, 0x133&0x3FF); //GC gain setting(1.6dB)
    //update GC gain value
    msWriteByteMask( REG_2D40, 0  		, BIT1); 
    msWriteByteMask( REG_2D40, BIT1  	, BIT1);
    msWriteByteMask( REG_2D40, 0  		, BIT1);
}

void msAudioDAC_Init( void )
{
    BYTE i;
    #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
    TIME_MEASURE_BEGIN();
    #endif	
	
    msAudioDPGA_Mute();
    for( i = 0; i < sizeof( tblHydanInit ) / sizeof( RegUnitType ); i++ )
    {
        msWriteByte( tblHydanInit[i].u16Reg, tblHydanInit[i].u8Value );
    }
    msdrvDecimationFilter();
	
    #if ENABLE_DP_AUDIO || ENABLE_HDMI
    msAudio_I2S_SPDIF_Init();
    #endif
#if DEPOP_DCONOFF_FLOW
    if (w16DAC_DePopStep==INIT_FINISH) 
    {
		return;
    }
#endif
	
#if USE_CASE_SEL != USE_CASE_2	

    for(i=0;i<sizeof(tDAC_INT_Step1Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step1Tbl[i].wReg,tDAC_INT_Step1Tbl[i].wValue,tDAC_INT_Step1Tbl[i].wMask );
    }
    for(i=0;i<sizeof(tDAC_INT_Step2Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step2Tbl[i].wReg,tDAC_INT_Step2Tbl[i].wValue,tDAC_INT_Step2Tbl[i].wMask );
    }
    ForceDelay1ms(1);	
    for(i=0;i<sizeof(tDAC_INT_Step3Tbl)/sizeof(RegTbl2Type);i++)
    {
        msWrite2ByteMask(tDAC_INT_Step3Tbl[i].wReg,tDAC_INT_Step3Tbl[i].wValue,tDAC_INT_Step3Tbl[i].wMask );
    }
    #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
    TIME_MEASURE_END();
    #endif
#endif
	#if USE_CASE_SEL == USE_CASE_2	
    w16DAC_DePopStep = INIT_FINISH;
    bDAC_DePopFlag = 0;
    SetDAC_DePopCounter(0); 
	#else
    w16DAC_DePopStep = INIT_STEP4;
    bDAC_DePopFlag = 0;
    SetDAC_DePopCounter(DAC_DELAY_STEP34);	
	#endif
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
    //return;
    #endif
	#if 1
    msWrite2Byte(REG_2CA6, 0x38F); // L-CH: +911
    msWrite2Byte(REG_2CA8, 0x38F); // R-CH: +911
    msWriteByteMask( REG_2CA4, 0  , BIT7);  //toggle DPGA gain
    msWriteByteMask( REG_2CA4, BIT7  , BIT7);  //toggle DPGA gain
    msWriteByteMask( REG_2CA4, 0     , BIT7);
	#endif
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
#if DEPOP_DCONOFF_FLOW
void msAudioDACPowerDown( Bool bPowerDN )
{
    if(bPowerDN)
    {
    		u8PowerDownCount=1;    
		msAudioDPGA_Mute();
		ForceDelay1ms(2000);
		msWriteByteMask( REG_2CE8, 0x00,	BIT1 ); //EN_R_STG2_AB  
		msWriteByteMask( REG_2CE8, 0x00,	BIT0 ); //EN_L_STG2_AB    
		msWriteByteMask( REG_2CE8, 0x00,	BIT3 ); //EN_RTX_EAR   
		msWriteByteMask( REG_2CE8, 0x00,	BIT2 ); //EN_LTX_EAR   
		ForceDelay1ms(1);
		msWriteByteMask( REG_2CD8, BIT4,	BIT4 ); //PD_IBIAS_EAR    
		msWriteByteMask( REG_2CD8, BIT5,	BIT5 ); //PD_RT_EAR    
		msWriteByteMask( REG_2CD8, BIT6,	BIT6 ); //PD_LT_EAR    
		msWriteByteMask( REG_2A00, BIT2,	BIT2 ); //SEL_DEPOP_SB    
		ForceDelay1ms(1);
		msWriteByteMask( REG_2CE4, 0x00,	BIT6 ); //EN_SW_DAC    
		ForceDelay1ms(1);
		msWriteByteMask( REG_2CDA, BIT1,	BIT1 ); //PD_L0_DAC    
		msWriteByteMask( REG_2CDA, BIT5,	BIT5 ); //PD_R0_DAC    
		msWriteByteMask( REG_2CDB, BIT1,	BIT1 ); //PD_REF_DAC    
		msWriteByteMask( REG_2CDB, BIT2,	BIT2 ); //PD_BIAS_DAC    
		msWriteByteMask( REG_2CE4, 0x00,	BIT0 ); //EN_CK_DAC    

		msWriteByteMask( REG_2CDC, BIT3 ,	BIT3 ); //PD_VI    
    }
    else
    {
    		u8PowerDownCount=0;    
		msAudioDPGA_Mute();
		ForceDelay1ms(1000);
    		msWriteByteMask( REG_2CE4, BIT0,	BIT0);     //EN_CK_DAC
    		msWriteByteMask( REG_2CE6, BIT0,	BIT0);    //EN_INSWL_EAR 
    		msWriteByteMask( REG_2CE6, BIT1,	BIT1);    //EN_INSWL_EAR 
    		msWriteByteMask( REG_2CDE, BIT2|BIT1,	BIT2|BIT1);    //SEL_TC 
    		msWriteByteMask( REG_2CDC, 0x00,	BIT5);  
    		msWriteByteMask( REG_2CDC, BIT6,	BIT6);  
    		msWriteByteMask( REG_2CDC, 0x00,	BIT7);  
    		msWriteByteMask( REG_2CDD, BIT0,	BIT0);  
    		msWriteByteMask( REG_2CDE, 0x70,	0x70); 
		ForceDelay1ms(1);
    		msWriteByteMask( REG_2CDA, 0x00,	BIT1);  
    		msWriteByteMask( REG_2CDA, 0x00,	BIT5);  
    		msWriteByteMask( REG_2CDB, 0x00,	BIT1);  
    		msWriteByteMask( REG_2CDB, 0x00,	BIT2);  
    		msWriteByteMask( REG_2CDC, 0x00,	BIT3);  
		ForceDelay1ms(1);
    		msWriteByteMask( REG_2CE4, BIT6,	BIT6);  
    		msWriteByteMask( REG_2CDB, 0x00,	BIT4);  
    		msWriteByteMask( REG_2CDB, 0x00,	BIT5);  
    		msWriteByteMask( REG_2CDB, 0x00,	BIT6);  
    		msWriteByteMask( REG_2A00, 0x00,	BIT2);  
		ForceDelay1ms(10);
    		msWriteByteMask( REG_2CDC, 0x00,	BIT6);  
    		msWriteByteMask( REG_2CDD, 0x00,	BIT0);  
    		msWriteByteMask( REG_2CE8, BIT3,	BIT3);  
    		msWriteByteMask( REG_2CE8, BIT2,	BIT2);  
		ForceDelay1ms(300);
    		msWriteByteMask( REG_2CE8, BIT1,	BIT1);  
    		msWriteByteMask( REG_2CE8, BIT0,	BIT0);  
    		msWriteByteMask( REG_2CDA, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CDB, 0x89,	0xFF); 
    		msWriteByteMask( REG_2CDC, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CDD, 0xFE,	0xFF); 
    		msWriteByteMask( REG_2CDE, 0x76,	0xFF); 
    		msWriteByteMask( REG_2CDF, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CE0, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CE1, 0x10,	0xFF); 
    		msWriteByteMask( REG_2CE2, 0x02,	0xFF); 
    		msWriteByteMask( REG_2CE3, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CE4, 0x41,	0xFF); 
    		msWriteByteMask( REG_2CE5, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CE6, 0x03,	0xFF); 
    		msWriteByteMask( REG_2CE7, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CE8, 0x0f,	0xFF); 
    		msWriteByteMask( REG_2CE9, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CEA, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CEB, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CEC, 0x0f,	0xFF); 
    		msWriteByteMask( REG_2CED, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CEE, 0x00,	0xFF); 
    		msWriteByteMask( REG_2CEF, 0x00,	0xFF); 
    }
}
#else
void msAudioDACPowerDown( Bool bPowerDN )
{
    BYTE i;
    if(bPowerDN)
    {
        for(i=0;i<sizeof(tDAC_PD_Step1Tbl)/sizeof(RegTbl2Type);i++)
        {
            msWrite2ByteMask(tDAC_PD_Step1Tbl[i].wReg,tDAC_PD_Step1Tbl[i].wValue,tDAC_PD_Step1Tbl[i].wMask );
        }
        for(i=0;i<sizeof(tDAC_PD_Step2Tbl)/sizeof(RegTbl2Type);i++)
        {
            msWrite2ByteMask(tDAC_PD_Step2Tbl[i].wReg,tDAC_PD_Step2Tbl[i].wValue,tDAC_PD_Step2Tbl[i].wMask );
        }
        ForceDelay1ms(500); //normally need to 2000ms, but need to trade off to speed up dc off flow
        for(i=0;i<sizeof(tDAC_PD_Step3Tbl)/sizeof(RegTbl2Type);i++)
        {
            msWrite2ByteMask(tDAC_PD_Step3Tbl[i].wReg,tDAC_PD_Step3Tbl[i].wValue,tDAC_PD_Step3Tbl[i].wMask );
        }
        ForceDelay1ms(2);
        for(i=0;i<sizeof(tDAC_PD_Step4Tbl)/sizeof(RegTbl2Type);i++)
        {
            msWrite2ByteMask(tDAC_PD_Step4Tbl[i].wReg,tDAC_PD_Step4Tbl[i].wValue,tDAC_PD_Step4Tbl[i].wMask );
        }
    }
}
#endif
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
#if 0
    msWriteByteMask(REG_2CA4, 0, BIT7);
    msWriteByteMask(REG_2CA4, BIT7, BIT7);
    msWriteByteMask(REG_2CA4, 0, BIT7);
	#endif
}


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

            for(i=0;i<sizeof(tDAC_INT_Step4Tbl)/sizeof(RegTbl2Type);i++)
            {
	            DAC_DPRINTF(" 4444---------dwReg ==%x   \r\n",tDAC_INT_Step4Tbl[i].wReg);
	            DAC_DPRINTF(" 4444---------wValue ==%x   \r\n",tDAC_INT_Step4Tbl[i].wValue);
            }
			
            #endif			
            break;
			 
        case INIT_STEP5:
            for(i=0;i<sizeof(tDAC_INT_Step5Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step5Tbl[i].wReg,tDAC_INT_Step5Tbl[i].wValue,tDAC_INT_Step5Tbl[i].wMask );
            w16DAC_DePopStep = INIT_STEP6;
            bDAC_DePopFlag = 0;
            SetDAC_DePopCounter(DAC_DELAY_STEP56); 
            #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
            TIME_MEASURE_END();
            #endif		
			
            for(i=0;i<sizeof(tDAC_INT_Step5Tbl)/sizeof(RegTbl2Type);i++)
            {
	            DAC_DPRINTF(" 5555---------dwReg ==%x   \r\n",tDAC_INT_Step5Tbl[i].wReg);
	            DAC_DPRINTF(" 5555---------wValue ==%x   \r\n",tDAC_INT_Step5Tbl[i].wValue);
            }
            break;
			 
        case INIT_STEP6:
            for(i=0;i<sizeof(tDAC_INT_Step6Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step6Tbl[i].wReg,tDAC_INT_Step6Tbl[i].wValue,tDAC_INT_Step6Tbl[i].wMask );
            w16DAC_DePopStep = INIT_STEP7;
            bDAC_DePopFlag = 0;
			
            SetDAC_DePopCounter(DAC_DELAY_STEP67); 
            #if CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
            TIME_MEASURE_END();
            #endif		
            for(i=0;i<sizeof(tDAC_INT_Step6Tbl)/sizeof(RegTbl2Type);i++)
            {
	            DAC_DPRINTF(" 6666---------dwReg ==%x   \r\n",tDAC_INT_Step6Tbl[i].wReg);
	            DAC_DPRINTF(" 6666---------wValue ==%x   \r\n",tDAC_INT_Step6Tbl[i].wValue);
            }
            break;

        case INIT_STEP7:
            for(i=0;i<sizeof(tDAC_INT_Step7Tbl)/sizeof(RegTbl2Type);i++)
                msWrite2ByteMask(tDAC_INT_Step7Tbl[i].wReg,tDAC_INT_Step7Tbl[i].wValue,tDAC_INT_Step7Tbl[i].wMask );
			DAC_DPRINTF("  ====     REG_2CEF==%d",msReadByte(REG_2CEF));
			
            //msWriteByteMask(REG_2C25, 0x00	, BIT5);  //[13].Fix value mode ( SDM input fixs to DC value) //REG_2C32[15:10]REG_2C30[11:0] uses to set DC value and Set it to 0
            w16DAC_DePopStep = INIT_FINISH;
            bDAC_DePopFlag = 0;
            SetDAC_DePopCounter(0); 
            #if 0//CheckInitialDACTime && ENABLE_TIME_MEASUREMENT
            mStar_AdjustVolume( UserPrefVolume );
            msAudioGainForceToggle();
            TIME_MEASURE_END();
            DAC_DPUTSTR("<<<<msAudioDAC_Init  END<<<<");
            #endif		

            for(i=0;i<sizeof(tDAC_INT_Step7Tbl)/sizeof(RegTbl2Type);i++)
            {
	            DAC_DPRINTF(" 7777---------dwReg ==%x   \r\n",tDAC_INT_Step7Tbl[i].wReg);
	            DAC_DPRINTF(" 7777---------wValue ==%x   \r\n",tDAC_INT_Step7Tbl[i].wValue);
            }
			
            break;

        case INIT_FINISH:
            break;

        default:
            w16DAC_DePopStep = 0;
            bDAC_DePopFlag = 0;
            SetDAC_DePopCounter(0); 
            break;
    }
}

#if EarphoneDet_Enable
void msAudioLineEarSwitch(BYTE AudioOutType)
{

    if(AudioOutType==E_LINE_OUT_)
    {
         msWrite2ByteMask(REG_2CE0, 0, BIT10|BIT11);
         hw_ClrMute();
	  hw_Earphone_SetMute(); 
    }
    else if(AudioOutType==E_EAR_OUT_)
     {
         msWrite2ByteMask(REG_2CE0, BIT10|BIT11, BIT10|BIT11);
         hw_SetMute();
	  hw_Earphone_ClrMute();	 
     }
}
#endif

#endif

