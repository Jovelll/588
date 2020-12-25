#define _ComboAPP_C_
#if 1
#include <math.h>
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "global.h"
#include "Mcu.h"
//#include "panel.h"
#include "adjust.h"
//#include "msADC.h"
#include "ms_rwreg.h"
#include "debug.h"
#include "Common.h"
#include "misc.h"
#include "ddc.h"
#include "power.h"
#include "detect.h"
#include "msOsd.h"
#include "gamma.h"
//#include "extlib.h"
#include "menu.h"
#include "menudef.h"
#include "menufunc.h"
#include "msflash.h"
//#include "msid_v1.h"
#include "drvgpio.h"
#include "drv_adctbl.h"
#include "drvadc.h"
#include "msEread.h"
#if ENABLE_RTE
#include "msovd.h"
#endif
#if ENABLE_FREESYNC
#include "drvDPRxApp.h"
#endif
#if (MS_PM)
#include "Ms_PM.h"
#endif
#include "MsHDCP.h"
#include "MsDLC.h"
#include "MsACE.h"
#include "Userpref.h"
#include "panel.h"
#if ENABLE_HDMI
#include "msHDMI.h"
#include "msHDMIACE.h"

#if ENABLE_CEC
#include "apiCEC.h"
#endif

#endif

#if MS_DAC
#include "drvdac.h"
#endif
#if ENABLE_3DLUT
#include "ms3DLUT.h"
#endif

#if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif

#include "drvmStar.h"

#define  Combo_DEBUG  1

#if (Combo_DEBUG&&ENABLE_DEBUG)
#define Combo_printData(str, value)   printData(str, value)
#define Combo_printMsg(str)               printMsg(str)
#else
#define Combo_printData(str, value)
#define Combo_printMsg(str)
#endif


#if (ENABLE_FREESYNC)
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetDDRFlag_ISR()
//  [Description]
//                  msAPI_combo_IPGetDDRFlag_ISR
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_IPGetDDRFlag_ISR()
{
    Bool ucflag= FALSE;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
          ucflag = DPRxGetDRRFlag_ISR();       
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {
         #if (ENABLE_HDMI)
        ucflag = mstar_HDMIGetFreeSyncFlag();
        #endif
    }

    return ucflag;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetDDRFlag()
//  [Description]
//                  msAPI_combo_IPGetDDRFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_IPGetDDRFlag()
{
    Bool ucflag= FALSE;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
        ucflag = DPRxGetDRRFlag();
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {
        #if (ENABLE_HDMI)
        ucflag = mstar_HDMIGetFreeSyncFlag();
        #endif
    }

    return ucflag;
}
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetDDRFlag()
//  [Description]
//                  msAPI_combo_IPGetDDRFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void msAPI_combo_IPEnableDDRFlag(Bool Enable)
{
    BYTE ucFlag=0;

    ucFlag = Enable;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
        DPRxEnableDRRFunction(Enable);
        DPRxHPDDisable();                                                                           // HPD ouput enable and set Low
        ForceDelay1ms( 100 );
        DPRxHPDEnable();
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {
        Combo_printMsg("============2");
	    Set_InputTimingChangeFlag(); // setup mode failed
	    mStar_SetupFreeRunMode(); // setup freerun mode
	    #if ENABLE_CABLE_5V_EDID && ENABLE_HDMI && ENABLE_FREESYNC &&(!ENABLE_HDMI_DRR_MCCS)	
		drvmStar_EnableHDMIFreeSyncEDID(Enable);
		#endif
		msDisableHDCP();
		ForceDelay1ms(100);
		msEnableHDCP();
    }
}
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetPixelClk()
//  [Description]
//                  msAPI_combo_IPGetPixelClk
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD msAPI_combo_IPGetPixelClk()
{
    WORD Pixel_Clock=0;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
        Pixel_Clock = DPRxGetPixelClk();
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {
        Combo_printMsg("============2");
    }

    return Pixel_Clock;
}
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetPixelClk()
//  [Description]
//                  msAPI_combo_IPGetPixelClk
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD msAPI_combo_IPGetGetHTotal()
{
    WORD Htt=0;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
        Htt = DPRxGetHTotal();
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {
        Combo_printMsg("============2");
    }

    return Htt;
}
#endif

#endif



