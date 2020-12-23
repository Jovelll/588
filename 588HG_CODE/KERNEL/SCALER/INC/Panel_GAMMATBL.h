
#ifndef _GAMMATBL_H
#define _GAMMATBL_H
#if (PanelType==PanelLM220WE1)
#include "Gama_PanelLM220WE1.h"
#elif(PanelType==PanelLTM230HT03)
#include "Gama_PanelLTM230HT03.h"
#elif (PanelType == PanelLM230WF5TLD1)
#include  "Gama_PanelLM230WF5TLD1.h"
#elif (PanelType==PanelTPM215HW01_HGE)// ||(PanelType==PanelTPM215HW01_HGJ)      //110828 Rick add
#include "Gama_PanelTPM215HW01_HGE.h"
#elif (PanelType == PanelBOEMV238FHM_N30)
#include  "Gama_PanelBOEMV238FHM_N30.h"
#else
#message "Please Panel Gamma Table!"
#include "Gama_Default.h" // copy from Gama_PanelLM220WE1.h
#endif

#if USE_NORMAL_GAMMA_TBL
code BYTE *tAllGammaTab[][3] =
{
    //  20
    {
        tblGamma20_RN,
        tblGamma20_GN,
        tblGamma20_BN,
    },
    // 22
    {
       tblGamma22_RN,
        tblGamma22_GN,
        tblGamma22_BN,
    },
    //  24
    {
        tblGamma24_RN,
        tblGamma24_GN,
        tblGamma24_BN,
    },
}; 
#else

code BYTE *tAllGammaTab[][6] =
{
    //  20
    {
        tblGamma20_RN, tblGamma20_RN_2,
        tblGamma20_GN, tblGamma20_GN_2,
        tblGamma20_BN,tblGamma20_BN_2
    },
    // 22
    {
       tblGamma22_RN,tblGamma22_RN_2,
        tblGamma22_GN,tblGamma22_GN_2,
        tblGamma22_BN,tblGamma22_BN_2
    },
    //  24
    {
        tblGamma24_RN,tblGamma24_RN_2,
        tblGamma24_GN,tblGamma24_GN_2,
        tblGamma24_BN,tblGamma24_BN_2
    },
}; 

#endif


#endif
