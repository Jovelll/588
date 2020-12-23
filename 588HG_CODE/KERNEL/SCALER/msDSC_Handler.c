#include <math.h>
#include "types.h"
#include "msDSC_Handler.h"
#include "msDSC_Setting.h"
#include "Debug.h"
#include "Common.h"
//===================================================================================================================================================================:
#define  DSC_DEBUG  1
#define  DSC_FAD_MODE 1
#if (DSC_DEBUG&&ENABLE_DEBUG)
#define DSC_printData(str, value)   printData(str, value)
#define DSC_printMsg(str)               printMsg(str)
#else
#define DSC_printData(str, value)
#define DSC_printMsg(str)
#endif

//===================================================================================================================================================================:
#if  ENABLE_DSC
xdata WORD  curSaturationHist[32];
xdata DWORD  TotalPixCnt;
xdata WORD  PseudoMaxThd;
xdata BYTE  PseudoMaxPtr;
xdata BYTE  PseudoMaxRatio=REG_DSC_PseudoMaxRatio; // 0~100 <--> 0%~100%
xdata BYTE  SatGainStep=REG_DSC_SatGainStep; // 0~100 <--> 0%~100%
#if DSC_FAD_MODE
xdata WORD  DSC_Ygain_tar_pre = 1024;
xdata WORD  DSC_Ygain_diff = 0;
xdata WORD  DSC_Ygain_ISR = 1024;
xdata WORD  DSC_Ygain_nxt = 1024;
xdata BYTE  DSC_YgainCycLoopCount = 0;
#endif
//
//===================================================================================================================================================================:
void msDSC_Handler ( void )
{
		xdata WORD	preLumaHist[32] = 0;
		xdata DWORD TempVariable1 = 0;
		xdata DWORD TempVariable2 = 0;
		xdata WORD	Ygain_tar = 0;
		xdata BYTE	i;
		//----------------------------------------------------------------------
		//--check valid DPS loop
		//----------------------------------------------------------------------
#if DSC_FAD_MODE
        xdata DWORD TempVariable3 = 0;
        xdata BYTE  Flag_SceneChangeValid = 0;
        xdata BYTE  Flag_IsGrayPureColor = 0;
        xdata BYTE  Ygain_sign = 0;
        xdata BYTE  HistDiffCount = 0;
#endif        

#if !DEBUG_DSC_withoutInterrupt		
		if ( ISR_curDSCLoopCount == ISR_preDSCLoopCount )
		{
			return;
		}
		ISR_preDSCLoopCount = ISR_curDSCLoopCount;
#endif		
		//----------------------------------------------------------------------
		//--latch current-frame Histogram
		//----------------------------------------------------------------------
		Flag_DSC_Latch_Busy = 1;
		TotalPixCnt=0;
		for ( i = 0; i < 32; i++ )
		{
			preLumaHist[i] = curSaturationHist[i];
			curSaturationHist[i] = ISR_SatHistogram32H[i]; 
			TotalPixCnt+=curSaturationHist[i];	
	   #if DSC_FAD_MODE
       TempVariable1 = ABS2(preLumaHist[i], curSaturationHist[i]);
            if( TempVariable1 > REG_DSC_HistDiffCountThd) 
                HistDiffCount++;
       #endif     
		#if 0
        	DSC_printData ( "\r\n curSaturationHist[i] =%x", ISR_SatHistogram32H[i] );
        	DSC_printData ( "\r\n TotalPixCnt =%x", TotalPixCnt );
        		#endif
		}
		#if 0
    		DSC_printData ( "\r\n curSaturationHist[i] =%x", ISR_SatHistogram32H[i] );
		DSC_printData ( "\r\n TotalPixCnt =%x", TotalPixCnt );
		#endif
		Flag_DSC_Latch_Busy = 0;
		PseudoMaxThd=(TotalPixCnt*PseudoMaxRatio)/100;
		PseudoMaxPtr=0;
		#if 0
			DSC_printData ( "\r\n PseudoMaxThd =%x", PseudoMaxThd);
		#endif
		
		TempVariable1=0;		
		for ( i = 31; i > 0; i-- )
		{
			TempVariable1+=curSaturationHist[i];	
			#if 0			
			printData ( "\r\n  AA i =%d", i );
			printData ( "\r\n  BB TempVariable1 =%x", TempVariable1 );
			printData ( "\r\n  CC PseudoMaxThd =%x", PseudoMaxThd );
			#endif			
			if ((TempVariable1>=PseudoMaxThd) && (i>=PseudoMaxPtr))
			{
				PseudoMaxPtr=i;
				#if 0
				DSC_printData ( "\r\n  PseudoMaxPtr =%d", PseudoMaxPtr );
				#endif
			}
		}
#if DSC_FAD_MODE
        if(PseudoMaxPtr == 0)
            Flag_IsGrayPureColor = 1;
        else
            Flag_IsGrayPureColor = 0;
        
        PseudoMaxPtr ++; 
#endif
		TempVariable2=(1024UL*(PseudoMaxPtr+SatGainStep))/PseudoMaxPtr;
		#if 0
			DSC_printData ( "\r\n  PseudoMaxPtr =%d", PseudoMaxPtr );
			DSC_printData ( "\r\n  TempVariable2 =%d", TempVariable2 );
		#endif
		Ygain_tar=MIN2 (2*1024UL,MIN2 ( 32*1024UL, TempVariable2 ));
    	#if 0
		DSC_printData ( "\r\n  DSC_SatSet Ygain_tar=%x", Ygain_tar );
    	#endif

	 #if 0
        if(HistDiffCount != 0)
        {
        DSC_printData ( "\r\n  ABS =%d", ABS2(Ygain_tar, DSC_Ygain_tar_pre) );
        DSC_printData ( "\r\n  HistDiffCount =%d", HistDiffCount );     
        }
        #endif
#if DSC_FAD_MODE
        Flag_SceneChangeValid = (ABS2(Ygain_tar, DSC_Ygain_tar_pre) > REG_DSC_SCENCE_GainDiff ? 1 : 0 )&&
                                (HistDiffCount > REG_DSC_SceChgHistCntThd);
        #if 0
        if ( Flag_SceneChangeValid ) DSC_printData ( "=======S", Flag_SceneChangeValid );
        if ( Flag_IsGrayPureColor ) DSC_printData ( "=======P", Flag_IsGrayPureColor );
        #endif

        if ((DSC_Ygain_tar_pre != Ygain_tar))
        {
            if (REG_DSC_UpdateFrameCyc == 0)
            {
                    DSC_Ygain_diff = REG_DSC_YgainDiffMinTH;
            }
            else
            {
                   TempVariable3 = ABS2 ( DSC_Ygain_ISR, Ygain_tar );
                   TempVariable1 = TempVariable3/REG_DSC_UpdateFrameCyc;
                   DSC_Ygain_diff = MAX2 ( TempVariable1, REG_DSC_YgainDiffMinTH );
            }
        }
        
        if ( DSC_YgainCycLoopCount == REG_DSC_YgainUpdateCyc )
        {
            if (DSC_Ygain_ISR != Ygain_tar)
            {
                Ygain_sign = ( Ygain_tar > DSC_Ygain_ISR ) ? 1 : 0;
                if ( Ygain_sign )
                {
                        TempVariable1 = DSC_Ygain_ISR + DSC_Ygain_diff;
                        DSC_Ygain_nxt = MIN2(TempVariable1, Ygain_tar);
                }
                else
                {
                        TempVariable1 = DSC_Ygain_ISR >= DSC_Ygain_diff ? DSC_Ygain_ISR - DSC_Ygain_diff : 0;
                        DSC_Ygain_nxt = MAX2(TempVariable1, Ygain_tar);
                }
            }
        }
        
        if ( DSC_YgainCycLoopCount == REG_DSC_YgainUpdateCyc )
        {
            DSC_YgainCycLoopCount = 0;
        }
        else
        {
            DSC_YgainCycLoopCount++;
        }
        
      
        DSC_Ygain_tar_pre = Ygain_tar;
        
        if(Flag_SceneChangeValid)
            DSC_Ygain_nxt = Ygain_tar;
        
            
        
        if(!Flag_IsGrayPureColor)
            DSC_Ygain_ISR = DSC_Ygain_nxt;

        #if 0
        DSC_printData ( "\r\n  Ygain_tar=%x", Ygain_tar );
        DSC_printData ( "\r\n  DSC_Ygain_ISR=%x", DSC_Ygain_ISR );
    	#endif
        
		msDSC_AdjustYgain ( DSC_Ygain_ISR );//
#else
        msDSC_AdjustYgain ( Ygain_tar );//
#endif
}
#else
BYTE code msDSCHandlerNullData[] = {0};
void msDSCHandlerDummy(void)
{
    BYTE xdata i = msDSCHandlerNullData[0];
}
#endif // #if EnableDPS
//===================================================================================================================================================================:
