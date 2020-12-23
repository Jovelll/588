#define _MODE_C_
#include <math.h>
#include "types.h"
#include "Board.h"
#include "mode.h"
#include "global.h"

#include "MS_reg.h"
#include "MS_rwreg.h"
#include "detect.h"
#include "Misc.h"
#include "drvadc.h"
#include "AutoFunc.h"
#include "common.h"
//#include "halRwreg.h"
 #if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif
#if ENABLE_FREESYNC
#include "ComboApp.h"
#endif

#define MODE_DEBUG    1
#if ENABLE_DEBUG&&MODE_DEBUG
    #define MODE_printData(str, value)   printData(str, value)
    #define MODE_printMsg(str)           printMsg(str)
#else
    #define MODE_printData(str, value)
    #define MODE_printMsg(str)
#endif

extern BYTE mStar_GetInputStatus( void );

#define SIMILAR_TIMING_BUFFER_MAX   10
BYTE xdata ucSimilarTimingNums;
BYTE xdata ucSimilarTimingBuffer[SIMILAR_TIMING_BUFFER_MAX]; // record similar timing buffer


//=====================================================================================
InputResolutionType code StandardModeResolution[] =
{
    { 640,  350},  // Res_640x350,
    { 640,  400},  // Res_640x400,
    { 720,  400},  // Res_720x400,
    { 640,  480},  // Res_640x480,
    //  { 720,  483},  //Res_720x483,
    {700,   570},//Res_700x570
    { 720,  576},  //Res_720x576,
    { 800,  600},  // Res_800x600,
    { 832,  624},  // Res_832x624,
    { 884,  930},  // Res_884x930,
    {1024,  768},  // Res_1024x768,
    {1280, 1024},  // Res_1280x1024,
    {1600, 1200},  // Res_1600x1200,
    {1152,  864},  // Res_1152x864,
    {1152,  870},  // Res_1152x870,
    {1152,  900}, // RES_1152x900,
    {1280,  720},  // Res_1280x720,
    {1280,  960},  // Res_1280x960,
    { 720,  480},  // Res_720x240
    { 800,  1209}, // Res_800x1209
    {1792,  1344}, // Res_1792x1344
    {1400,  1050}, // Res_1400x1050
    {1920,  1080}, // Res_1920x1080
    {1920,  1200}, // Res_1920x1200
    {848,    480}, // Res_848x480
    {1280,   768}, // Res_1280x768
    {1360,   768}, // Res_1360x768
    {1366,   768}, // Res_1366x768
    {1440,   900}, // Res1440x900
    {1680,  1050}, // Res_1680x1050
    {1280,   800}, // Res_1280x800
    {1600,   900}, // Res_1600x900
    {2560,  1600}, // Res_2560x1600
	{2560,  1440}, // Res_2560x1440
    #if _3D_TIMING
    {1920,   2160}, //Res_1920x2160
    {1280, 1440},//Res_1280x720P_FP
    #endif
};

InputModeType code StandardMode[] =
{
#if 1//PANEL_WIDTH==1600&&PANEL_HEIGHT==900     //110919 Rick modified - A030
    // 1600x900 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1600x900, // ResIndex
        600, 600, // HFreq, VFreq
        1800, 1000,// HTotal, VTotal
        176 - 6, 99 - 3 - 1, // HStart, VStart
    },
    // 1600x900 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1600x900, // ResIndex
        559, 600, // HFreq, VFreq
        2128, 932,// HTotal, VTotal
        176, 99,       // HStart, VStart
    },
    // 1600x900 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1600x900, // ResIndex
        555, 600, // HFreq, VFreq
        1760, 926,// HTotal, VTotal
        112 - 6, 23 - 5 - 1, // HStart, VStart
    },
#endif
#if 1//PANEL_WIDTH==1366&&PANEL_HEIGHT==768
    // 0.1366x768 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1366x768, // ResIndex
        477, 598, // HFreq, VFreq
        1792, 798, // HTotal, VTotal
        356 - 6, 27 - 3 - 1, // HStart, VStart
    },
    // 1. 1360X768 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1360x768, // ResIndex
        477, 600, // HFreq, VFreq
        1792, 795 - 1, // HTotal, VTotal
        368 - 5 - 3, 24 - 6 - 1, // HStart, VStart
    },
#endif

	// 1080p 75hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1920x1080, // ResIndex
        837, 749, // HFreq, VFreq
        2100,1110, // HTotal, VTotal
        202, 32, // HStart, VStart
    },

    //  0  // 2. 848X480 60HZ
    {
        SyncHpVp, // Flags
        Res_848x480, // ResIndex
        310, 600, // HFreq, VFreq
        1088, 517, // HTotal, VTotal
        224 - 5, 31 - 8 - 1, // HStart, VStart
    },
    // 1    // 3. 848X480 75HZ (CVT)
    {
        SyncHnVp,   // Flags
        Res_848x480, // ResIndex
        377, 748, // HFreq, VFreq
        1088, 504, // HTotal, VTotal
        200 - 5, 21 - 5 - 1, // HStart, VStart
    },
    
    // 2// 4.1280X768 60Hz Reduced Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x768, // ResIndex
        474, 599, // HFreq, VFreq
        1440, 790, // HTotal, VTotal
        //112 - 5, 19 - 7 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1440*900
        //132 - 5, 19 - 7 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1280*800
        102 - 5, 19 - 7 - 1, // HStart, VStart
    },
    // 3 // 5.1280X768 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x768, // ResIndex
        477, 598, // HFreq, VFreq
        1664, 798, // HTotal, VTotal
        //320 - 5, 27 - 7 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA
        //340 - 5, 27 - 7 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1280*800
        310 - 5, 27 - 7 - 1, // HStart, VStart
    },
    // 4 // 6.1280X768 75Hz(VESA)
    {
        SyncHnVp, // Flags
        Res_1280x768, // ResIndex
        603, 748, // HFreq, VFreq
        1696, 805, // HTotal, VTotal
        //336 - 7, 34 - 7 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA
        //356 - 7, 34 - 7 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1280*800
        326 - 7, 34 - 7 - 1, // HStart, VStart
    },
    // 5 // 7.1280X768 85HZ(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x768, // ResIndex
        686, 848, // HFreq, VFreq
        1712, 809, // HTotal, VTotal
        352 - 6, 38 - 7 - 1, // HStart, VStart
    },
    
    // 6 // 8. 1280x960 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1280x960, // ResIndex
        600, 600, // HFreq, VFreq
        1696, 996, //1800, 1000, // HTotal, VTotal
        424, 39, // HStart, VStart
    },
    // 7 // 9. 1280x960 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1280x960, // ResIndex
        600, 600, // HFreq, VFreq
        1800, 1000, // HTotal, VTotal
        424 - 5, 39 - 3 - 1, // HStart, VStart
    },
    // 8 // 10. 1280x960 70Hz (Tune For PC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x960, // ResIndex
        700, 700, // HFreq, VFreq
        1728, 1000, // HTotal, VTotal
        354, 34, // HStart, VStart
    },
    // 9 // 11. 1280x960 72Hz(Tune For PC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn , // Flags
        Res_1280x960, // ResIndex
        723, 722, // HFreq, VFreq
        1728, 1002,//1728, 1001, // HTotal, VTotal
        354, 36, // HStart, VStart
    },
    // 10 // 12. 1280x960 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x960, // ResIndex
        750, 750, // HFreq, VFreq
        1728, 1002,//1680, 1000, // HTotal, VTotal
        360 - 5, 39 - 3 - 1, // HStart, VStart
    },
    // 11 // 13. 1280x960 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x960, // ResIndex
        859, 850, // HFreq, VFreq
        1728, 1011, // HTotal, VTotal
        384 - 6, 50 - 3 - 1, // HStart, VStart
    },
    // 12 // 14. 1360X768 60HZ(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,// Flags
        Res_1360x768, // ResIndex
        477, 600, // HFreq, VFreq
        1792, 795 - 1, // HTotal, VTotal
        368 - 5, 24 - 6 - 1, // HStart, VStart
    },
    // 13 // 15. 1440X900 60Hz Reduce Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1440x900, // ResIndex
        554, 599, // HFreq, VFreq
        1600, 926, // HTotal, VTotal
        112 - 5, 23 - 6 - 1, // HStart, VStart
        //JOVE add for TENGSHAN
        //122 - 2, 23 - 6 - 1, // HStart, VStart
        //JOVE add for CEHUA
        //116 - 2, 23 - 6 - 1, // HStart, VStart
    },
    // 14 // 16. 1440X900 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1440x900, // ResIndex
        559, 598, // HFreq, VFreq
        1904, 934, // HTotal, VTotal
        384 - 6, 31 - 6 - 1, // HStart, VStart
        //JOVE add for TENGSHAN 
        //394 - 6, 31 - 6 - 1, // HStart, VStart
        //JOVE add for CEHUA
        //392 - 6, 31 - 6 - 1, // HStart, VStart
    },
    // 15 // 17. 1440X900 75HZ(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1440x900, // ResIndex
        706, 750, // HFreq, VFreq
        1936, 942, // HTotal, VTotal
        400 - 6,  39 - 6 - 1, // HStart, VStart
        //JOVE add for TENGSHAN
        //410 - 6,  39 - 6 - 1, // HStart, VStart
        //JOVE add for CEHUA
        //404 - 6,  39 - 6 - 1, // HStart, VStart
    },
    // 16 // 18. 1440X900 85Hz(VESA)
    {
        SyncHnVp, // Flags
        Res_1440x900, // ResIndex
        804, 848, // HFreq, VFreq
        1952, 948, // HTotal, VTotal
        408 - 6,  45 - 6 - 1, // HStart, VStart
    },
    //Add a new mode for some PC 1080P timing
    // 1920X1080  60Hz        // 103 QD-CVT1960H , ouput DCLK=173MHz
    {
    SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags

       Res_1920x1080, // resolution index
       672, 600,     // HFreq, VFreq
       2576,1120,    // HTotal, VTotal
       0x208,0x1F,//528, 37,      // HStart, VStart
    },
    // 17 // 19. 1680X1050 60Hz Reduced Blanking(VESA)
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags
        Res_1680x1050, // ResIndex
        646,  598, // HFreq, VFreq
        1840, 1080, // HTotal, VTotal
        120 - 8,   27 - 6 - 1, // HStart, VStart
    },
    // 18 // 20. 1680X1050 60Hz Normal Blanking(VESA)
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags
        Res_1680x1050, // ResIndex
        652 , 599, // HFreq, VFreq
        2240, 1089, // HTotal, VTotal
        462 - 8 ,  36 - 6 - 1, // HStart, VStart
    },
    //19 // 21. 1680X1050 75HZ
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags
        Res_1680x1050, // ResIndex
        823 , 748, // HFreq, VFreq
        2272, 1099, // HTotal, VTotal
        472 - 9 ,  46 - 6 - 1, // HStart, VStart
    },
    //====================================
    //===== Normal Mode Start ============
    //====================================
    // 20 // 22. 640x350 70Hz (IMB VGA)
    {
        SyncHpVn, // Flags
        Res_640x350, // ResIndex
        315, 700, // HFreq, VFreq
        800, 449, // HTotal, VTotal
        145, 62 - 2 - 1,//144 - 5, 62 - 2 - 1, // HStart, VStart    //PS. spec 144, 61
    },
    //21    // 23. 640x350 85Hz (VESA)
    {
        SyncHpVn, // Flags
        Res_640x350, // ResIndex
        379, 851, // HFreq, VFreq
        832, 445, // HTotal, VTotal
        160 - 5, 63 - 3 - 1, // HStart, VStart
    },
    //22    // 24. 720x400 70Hz (VGA)
    {
        SyncHpVp | SyncHnVp | SyncHnVn, // Flags
        Res_720x400, // ResIndex
        315, 700, // HFreq, VFreq
        900, 449, // HTotal, VTotal
        162, 37 - 2 - 1,//160 - 6, 37 - 2 - 1, //160 - 6, 37 - 2 - 1, // HStart, VStart     //PS.spec 153, 29
    },
    //23   // 25. 720x400 85Hz (VESA)     //Haven't tune Chroma No this Timing
    {
        SyncHpVp | SyncHnVp | SyncHnVn, // Flags
        Res_720x400, // ResIndex
        379, 850, // HFreq, VFreq
        936, 446, // HTotal, VTotal
        180 - 5, 45 - 3 - 1, // HStart, VStart
    },
    //24   // 26. 640x400 56Hz (NEC)
    {
        SyncHnVn, // Flags
        Res_640x400, // ResIndex
        249, 564, // HFreq, VFreq
        848, 440, // HTotal, VTotal
        144 - 5, 33 - 8 - 1, // HStart, VStart
    },
    //25    // 27. 640x400 70Hz (IMB VGA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x400, // ResIndex
        379, 851, // HFreq, VFreq
        832, 445, // HTotal, VTotal
        160 - 5, 44 - 3 - 1, // HStart, VStart
    },
    //===============================================
    //===>Conflict with WSVGA(848x480@60Hz HpVp) ====
    //===============================================
    //27    // 29. 640x480 60Hz (VESA)
    {
        //SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        315, 599, // HFreq, VFreq
        800, 525, // HTotal, VTotal
        136 + 8, 27 + 5, //136, 27 - 3, //139, 32, // HStart, VStart           //spec 136, 27   //111111 Rick modified
    },

    //28   // 30. 640x480 66Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        350, 667, // HFreq, VFreq
        864, 525, // HTotal, VTotal
        160, 42, // HStart, VStart
    },

    //29  // 31. 640x480 72Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        335, 671, // HFreq, VFreq
        816, 499, // HTotal, VTotal
        153, 14, // HStart, VStart
    },

    //29  // 31. 640x480 72Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        379, 728, // HFreq, VFreq
        832, 520, // HTotal, VTotal
        160 - 5, 23 - 3 - 1, // HStart, VStart
    },
    //30  // 32. 640x480 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        375, 750, // HFreq, VFreq
        840, 500, // HTotal, VTotal
        184 - 5, 19 - 3 - 1, // HStart, VStart
    },
    //31 // 33. 640x480 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        433, 850, // HFreq, VFreq
        832, 509, // HTotal, VTotal
        136 - 5, 28 - 3 - 1, // HStart, VStart
    },
    //32  // 34. 800x600 56Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        352, 562, // HFreq, VFreq
        1024, 625, // HTotal, VTotal
        200 - 5, 24 - 2 - 1, // HStart, VStart
    },
    //33 // 35. 800x600 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        379, 603, // HFreq, VFreq
        1056, 628, // HTotal, VTotal
        216 - 5, 27 - 4 - 1, // HStart, VStart
    },
    // 34 // 36. 800x600 72Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        481, 722, // HFreq, VFreq
        1040, 666, // HTotal, VTotal
        184 - 5, 29 - 6 - 1, // HStart, VStart
    },
    // 35 // 37. 800x600 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        469, 750, // HFreq, VFreq
        1056, 625, // HTotal, VTotal
        240 - 5, 24 - 3 - 1, // HStart, VStart
    },
    // 36  // 38. 800x600 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        537, 851, // HFreq, VFreq
        1048, 631, // HTotal, VTotal
        216 - 5, 30 - 3 - 1, // HStart, VStart
    },
    //37  // 39. 832x624 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_832x624, // ResIndex
        497, 746, // HFreq, VFreq
        1152, 665,//667, // HTotal, VTotal
        288 - 5, 42 - 3 - 1, // HStart, VStart
    },
    //========================================================
    //===>Conflict with WXVGA(1280x768@60Hz HpVp;HnVp) ====
    //========================================================

    // 38 // 40. 1024x768 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flagsc
        Res_1024x768, // ResIndex
        484, 600, // HFreq, VFreq
        1344, 806, // HTotal, VTotal
        296 - 5, 35 - 6 - 1, // HStart, VStart
    },
    //39// 41. 1024x768 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flagsc
        Res_1024x768, // ResIndex
        484, 600, // HFreq, VFreq
        1312, 813, // HTotal, VTotal
        264 - 5, 39 - 3 - 1, // HStart, VStart
    },
    //40 // 42. 1024x768 70Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        565, 700, // HFreq, VFreq
        1328, 806, // HTotal, VTotal
        280 - 6, 35 - 6 - 1, // HStart, VStart
    },

    //===============================================
    //===>Conflict with WXVGA(1280x768@75Hz HnVp) ===
    //===============================================
    // 41 // 43. 1024x768 75Hz(VESA)
    {
        //SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        SyncHpVp | SyncHpVn | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        600, 750, // HFreq, VFreq
        1312, 800, // HTotal, VTotal
        272 - 5, 31 - 3 - 1, // HStart, VStart
    },
    // 42 // 44. 1024x768 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        602, 749, // HFreq, VFreq
        1328, 804, // HTotal, VTotal
        272 - 5, 33 - 3 - 1, // HStart, VStart
    },
    //===============================================
    //===>Conflict with WXVGA(1280x768@85Hz HnVp) ===
    //===============================================
    // 43 // 45. 1024x768 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        683, 850, // HFreq, VFreq
        1376, 808, // HTotal, VTotal
        304 - 5, 39 - 3 - 1, // HStart, VStart
    },
    // 44// 46. 1152x864 60Hz(Tune For PC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        536, 600, // HFreq, VFreq
        1520, 895, // HTotal, VTotal
        300, 26, // HStart, VStart
    },
    //45 // 47. 1152x864 70Hz(Tune For PC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        629, 699, // HFreq, VFreq
        1536, 900,//14, // HTotal, VTotal
        312 - 6, 35 - 3 - 1, // HStart, VStart
    },
    //46 // 48. 1152x864 70Hz (For ATI Card)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        662, 702, // HFreq, VFreq
        1536, 900,// HTotal, VTotal
        288, 49, // HStart, VStart
    },
    // 47 // 49. 1152x864 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        675, 750, // HFreq, VFreq
        1560, 900, // HTotal, VTotal
        384 - 6, 35 - 3 - 1, // HStart, VStart
    },
    //48 // 50. 1152x864 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        771, 850, // HFreq, VFreq
        1576, 907, // HTotal, VTotal
        360, 42, // HStart, VStart
    },
    // 49 // 51. 1152x870 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn  | SyncHnVn, // Flags
        Res_1152x870, // ResIndex
        689, 751, // HFreq, VFreq
        1456, 915, // HTotal, VTotal
        272 - 7, 42 - 3 - 1, // HStart, VStart
    },
    
    // 50 // 52. 1280x720 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,//Flags
        Res_1280x720, // ResIndex
        450, 600, // HFreq, VFreq
        1650, 750, // HTotal, VTotal
        260 - 5, 25 - 5 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1440*900
        //317 - 5, 25 - 5 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1280*800
        //210 - 5, 25 - 5 - 1, // HStart, VStart
    },
    //51 // 55. 1280x720 70Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720, // ResIndex
        524, 700, // HFreq, VFreq
        1688, 750, // HTotal, VTotal
        424, 39, // HStart, VStart
        //JOVE ADD FOR CEHUA 1440*900
        //481, 39, // HStart, VStart
        //JOVE ADD FOR CEHUA 1280*800
        //374, 39, // HStart, VStart
    },
    // 52 // 56. 1280x720 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720, // ResIndex
        563, 750, // HFreq, VFreq
        1696, 755, // HTotal, VTotal
        336 - 5, 32 - 5 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1440*900
        //393 - 5, 32 - 5 - 1, // HStart, VStart
        //JOVE ADD FOR CEHUA 1280*800
        //286 - 5, 32 - 5 - 1, // HStart, VStart
    },
    
    //========================================================
    //===>Conflict with SXVGA+(1400x1050@60Hz HpVn,HnVP ) ====
    //========================================================
    //53 //57. 1280x1024 60Hz (VESA)
    {
        SyncHpVp | SyncHnVn | SyncHpVn | SyncHnVp, // Flags
        Res_1280x1024, // ResIndex
        640, 600, // HFreq, VFreq
        1688, 1066, // HTotal, VTotal
        360 - 6, 41 - 3 - 1, // HStart, VStart
    },

    // 54 //58. 1280x1024 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        800, 750, // HFreq, VFreq
        1688, 1066, // HTotal, VTotal
        392 - 6, 41 - 3 - 1, // HStart, VStart
    },
    // 55 //59. 1280x1024 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        911, 850, // HFreq, VFreq
        1728, 1072, // HTotal, VTotal
        384 - 7, 47 - 3 - 1, // HStart, VStart
    },
    // 56 //60. 1600x1200 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        750, 600, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 7, 49 - 3 - 1, // HStart, VStart
    },
    // 57 //61. 1600x1200 65Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        813, 650, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 9, 49 - 24 - 1, // HStart, VStart
    },
    //58 //62. 1600x1200 70Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        875, 700, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 6, 49 - 24 - 1, // HStart, VStart
    },
    // 59 //63. 1600x1200 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        938, 750, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 6, 49 - 24 - 1, // HStart, VStart
    },
    // 60 //64. 1400x1050 60Hz Reudce Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1400x1050, // ResIndex
        647, 599, // HFreq, VFreq
        1560, 1080, // HTotal, VTotal
        112 - 5, 27 - 4 - 1, // HStart, VStart
    },
    // 61 //65. 1400x1050 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1400x1050, // ResIndex
        653, 599, // HFreq, VFreq
        1864, 1089, // HTotal, VTotal
        376 - 6, 36 - 4 - 1, // HStart, VStart
    },
    // 62  //66. 1400x1050 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1400x1050, // ResIndex
        823, 749,      // HFreq, VFreq
        1896, 1099,    // HTotal, VTotal
        392 - 6, 46 - 4 - 1, // HStart, VStart
    },
    // 63 //67. 1400x1050 85Hz (VESA)
    {
        SyncHnVp,      // Flags
        Res_1400x1050, // ResIndex
        939,  850,      // HFreq, VFreq
        1912, 1105,    // HTotal, VTotal
        408 - 6,  52 - 4 - 1, // HStart, VStart
    },
    // 64 //68. 1792x1344 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1792x1344, // ResIndex
        836, 600, // HFreq, VFreq
        2448, 1394, // HTotal, VTotal
        528 - 6, 49 - 3 - 1, // HStart, VStart
    },
    // 65 //69. 1920x1080 60Hz (VESA)
    /*
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1920x1080, // ResIndex
        672, 600, // HFreq, VFreq
        2576, 1120, // HTotal, VTotal
        456 - 9, 35 - 3 - 1, // HStart, VStart
    },
    */
    // 66 //70. 1920x1200 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1920x1200, // ResIndex
        746, 599, // HFreq, VFreq
        2592, 1245, // HTotal, VTotal
        536 - 6, 42 - 6 - 1, // HStart, VStart
    },
    // 67  //71. 1920x1200 60Hz Reduce Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1920x1200, // ResIndex
        740, 599, // HFreq, VFreq
        2080, 1235, // HTotal, VTotal
        112 - 5, 32 - 6 - 1, // HStart, VStart
    },
#if 1
    // 68 //72. 720x480 50Hz (YCbCr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_720x480, // ResIndex
        156, 500, // HFreq, VFreq
        900, 625, // HTotal, VTotal
        144, 88, // HStart, VStart
    },
#endif
    // 69 // 75. 1024x768 72Hz
    {
        //SyncHpVp | SyncHpVn | SyncHnVp  // Flags
        SyncHpVp | SyncHnVp, // Flags
        Res_1024x768, // ResIndex
        576, 721, // HFreq, VFreq
        1312, 800, // HTotal, VTotal
        272, 31, // HStart, VStart
    },

    // 70 // 76. 1280 X 1024  72 Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        779, 720, // HFreq, VFreq
        1728, 1082, // HTotal, VTotal
        360 - 5, 57 - 5 - 1, // HStart, VStart
    },

    // 71 //77. 1280 X 1024 70 Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        748, 697, // HFreq, VFreq
        1696, 1064, // HTotal, VTotal
        378, 41, // HStart, VStart
    },

    // 72 // 24,1152 X 900 66 Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x900, // enResIndex
        618, 660, // u16HFreq, u16VFreq
        1504, 937, // u16HTotal, u16VTotal
        322 - 7, 35 - 4 - 1, // u16HStart, u16VStart
    },

    //73 //78. 720x480I 60Hz (YPbPr)  --> 78  Start of YPbPr
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_720x480, // resolution index
        157, 599,    // HFreq, VFreq
        858, 525,    // HTotal, VTotal
        238, 18,       // HStart, VStart
    },

    //74 //79. 1920x1080i 50Hz (YPbPr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_1920x1080, // resolution index
        281, 500,      // HFreq, VFreq
        2640, 1125,    // HTotal, VTotal
        186, 24,       // HStart, VStart
    },

    //75 //80. 1920x1080i 50Hz (YPbPr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_1920x1080, // resolution index
        312, 500,     // HFreq, VFreq
        2304, 1250,   // HTotal, VTotal
        186, 24,      // HStart, VStart
    },

    //76 //81. NTSC 1080i
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_1920x1080, // resolution index
        337, 600,      // HFreq, VFreq
        2200, 1125,    // HTotal, VTotal
        232, 27,       // HStart, VStart
    },
    //77 //82  700x570 50Hz (For India TV Box)
    {
        SyncHpVp  , // Flags
        Res_700x570, // ResIndex
        313, 500, // HFreq, VFreq
        884, 625, // HTotal, VTotal
        179, 43, // HStart, VStart
    },
    // 78//83  1152x870 60Hz (MAC)
    {
        SyncHnVp  , // Flags
        Res_1152x870, // ResIndex
        540, 600, // HFreq, VFreq
        1480, 900, // HTotal, VTotal
        264, 24, // HStart, VStart
    },
    //79 //84. 576P(YCbCr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x576, // ResIndex
        312, 500, // HFreq, VFreq
        864, 625, // HTotal, VTotal
        125, 40, // HStart, VStart
    },
    // 80 //720x576 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x576, // ResIndex
        358, 597, // HFreq, VFreq
        912, 600, // HTotal, VTotal
        168 - 4, 23 - 3 - 1, // HStart, VStart
    },
    // 81 //720x576 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x576, // ResIndex
        455, 756, // HFreq, VFreq
        944, 602, // HTotal, VTotal
        129 - 5, 25 - 2 - 1, // HStart, VStart
    },
    // 82 //85. 576i
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode,  // Flags
        Res_720x576, // ResIndex
        156, 500, // HFreq, VFreq
        864, 626, // HTotal, VTotal
        264, 22, // HStart, VStart
    },

    // 83 // 86. 480P 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x480, // ResIndex
        315, 599, // HFreq, VFreq
        858, 525, // HTotal, VTotal
        122, 36,   // HStart, VStart
    },

    // 84 //87.     480i
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode,  // Flags
        Res_640x480, // ResIndex
        157, 599, // HFreq, VFreq
        1716, 525, // HTotal, VTotal
        238, 21, // HStart, VStart
    },
    // 85 //90. PAL 1080p
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        562, 500, // HFreq, VFreq
        2640, 1125, // HTotal, VTotal
        112, 28, // HStart, VStart
    },
    // 86 //91. NTSC 1080p
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        674, 599, // HFreq, VFreq
        2200, 1125,// HTotal, VTotal
        192 - 7, 41 - 5 - 1, // HStart, VStart
    },
    // 87 // 92. 832x624 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_832x624, // ResIndex
        497, 746, // HFreq, VFreq
        1152, 662, // HTotal, VTotal
        288, 42, // HStart, VStart
    },
    //88 //94. 1280x800 60_RHz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        493, 599, // HFreq, VFreq
        //1440, 823,// HTotal, VTotal
        1500, 823,// HTotal, VTotal
        112 - 5, 20 - 6 - 1, // HStart, VStart
        //JOVE add for TENGSHAN
        //122 - 5, 20 - 6 - 1, // HStart, VStart
    },
    //89 //95. 1280x800 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        497, 598, // HFreq, VFreq
        //1680, 831,// HTotal, VTotal
        1740, 831,// HTotal, VTotal
        328 - 5, 28 - 6 - 1, // HStart, VStart
        //JOVE add for TENGSHAN
        //338 - 5, 28 - 6 - 1, // HStart, VStart
    },
    //96. 1280x800 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        497, 598, // HFreq, VFreq
        //1696, 838,// HTotal, VTotal
        1740, 838,// HTotal, VTotal
        336 - 5, 35 - 6 - 1, // HStart, VStart
        //JOVE add for TENGSHAN
        //346 - 5, 35 - 6 - 1, // HStart, VStart
    },
    //1280x800 60Hz        NVIDIA
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        495, 598, // HFreq, VFreq
        //1680, 828,// HTotal, VTotal
        1740, 828,// HTotal, VTotal
        329, 23,       // HStart, VStart
        //JOVE add for TENGSHAN
        //339, 23,       // HStart, VStart
    },
    //97. 1280x800 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        628, 749, // HFreq, VFreq
        1696, 838,// HTotal, VTotal
        336 - 5, 35 - 6 - 1, // HStart, VStart
        //JOVE add for TENGSHAN
        //346 - 5, 35 - 6 - 1, // HStart, VStart
    },
    //98. 1280x800 76Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        647, 764, // HFreq, VFreq
        1568, 846,// HTotal, VTotal
        264, 43,       // HStart, VStart
    },
    //99. 1080p 138.5M
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        666, 599, // HFreq, VFreq
        2080, 1111,// HTotal, VTotal
        112 - 7, 28 - 5 - 1, // HStart, VStart
    },
    //100. 1080p 148.5M
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        663, 599, // HFreq, VFreq
        2240, 1107,// HTotal, VTotal
        256 - 9, 26 - 3 - 1, // HStart, VStart
    },

    // 101, 1920x1080P 24Hz
    {
        SyncHpVp |SyncHpVn |SyncHnVp |SyncHnVn, // Flags
        Res_1920x1080, // ResIndex
        270, 240, // HFreq, VFreq
        2750, 1125, // HTotal, VTotal
        229,60,//230, 60,//59,
    },
    // 102, 1920x1080P 25Hz
    {
        SyncHpVp |SyncHpVn |SyncHnVp |SyncHnVn, // Flags
        Res_1920x1080, // ResIndex
        281, 250, // HFreq, VFreq
        2640, 1125, // HTotal, VTotal
        229,60,//230, 60,//59,
    },
    // 103, 1920x1080P 30Hz
    {
        SyncHpVp |SyncHpVn |SyncHnVp |SyncHnVn, // Flags
        Res_1920x1080, // ResIndex
        338, 300, // HFreq, VFreq
        2200, 1125, // HTotal, VTotal
        229,60,//230, 60,//59,
    },
//, 2560x1600 60Hz
	{
		SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
		Res_2560x1600, // ResIndex
		988, 600, // HFreq, VFreq
		2720, 1646, // HTotal, VTotal
		112, 39,
	},	
//, 2560x1440 60Hz
	{
	SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
	Res_2560x1440, // ResIndex
	888, 600, // HFreq, VFreq
	2720, 1481, // HTotal, VTotal
	112, 39,
},

#if  _3D_TIMING
//1920x1080 24Hz FP
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1920x2160, // ResIndex
        541, 240, // HFreq, VFreq
        2750, 2250, // HTotal, VTotal
        192, 41, // HStart, VStart
    },
    //1280x72P 60Hz FP
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720P_FP, // resolution index
        900, 600,      // HFreq, VFreq
        1650, 1500,    // HTotal, VTotal
        220, 20,       // HStart, VStart
    },
    //76, 1280x72P 50Hz FP
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720P_FP, // resolution index
        750, 500,      // HFreq, VFreq
        1980, 1500,    // HTotal, VTotal
        220, 20,       // HStart, VStart
    },


#endif
    //=====IMPORTANT    DON'T DELETE IT, OR IT WILL CAUSE LOOP INIFINITE
    {
        0, // Flags
        0, // ResIndex
        0, 0, // HFreq, VFreq
        0, 0, // HTotal, VTotal
        0, 0, // HStart, VStart
    }
};
WORD GetStandardModeWidth( void )
{
    return StandardModeResolution[StandardModeGroup].DispWidth;
}
WORD GetStandardModeHeight( void )
{
    return StandardModeResolution[StandardModeGroup].DispHeight;
}
WORD GetStandardModeGroup( void )
{
    return StandardMode[SrcModeIndex].ResIndex;
}
WORD GetStandardModeHStart( void)
{
    return StandardMode[SrcModeIndex].HStart;
}
WORD GetStandardModeVStart( void)
{
    return StandardMode[SrcModeIndex].VStart;
}
WORD GetStandardModeVTotal( void )
{
    return StandardMode[SrcModeIndex].VTotal;
}
WORD GetStandardModeHTotal( void )
{
    return StandardMode[SrcModeIndex].HTotal;
}
WORD GetStandardModeHFreq( void )
{
    return StandardMode[SrcModeIndex].HFreq;
}
WORD GetStandardModeVFreq( void )
{
    return StandardMode[SrcModeIndex].VFreq;
}
#if ENABLE_DVI_DUAL_LINK
WORD Mode_GetStandardModePxlRate(void)
{
    return (WORD)((DWORD)StandardModeHTotal * HFreq( SrcHPeriod ) / 10000);
}
#endif
//========================================================================================
#if 1
#define PHASE_MAX          (0x80)
#define PHASE_STEP         (0x07)
#define PHASE_CENTER    0x40

#define CHECK_MODE_WIDTH   1

void appCheckSimilarTiming(void)
{
#if CHECK_MODE_WIDTH
    WORD xdata wWidth;
#endif
    BYTE vsyncTime, i, ucIndex, uccount;
    DWORD maxcksum, cksum, maxcksum1;
    BYTE adjPhase;

    if(ucSimilarTimingNums <= 1 || (SrcFlags&bUnsupportMode))
        return;

    vsyncTime = GetVSyncTime();
    i = 0;
    maxcksum = maxcksum1 = 0;
    ucIndex = SrcModeIndex;
    uccount = (ucSimilarTimingNums>=SIMILAR_TIMING_BUFFER_MAX)?(SIMILAR_TIMING_BUFFER_MAX):(ucSimilarTimingNums);

            //printData(" ucSimilarTimingNums:%d", ucSimilarTimingNums);

    while(uccount--)
    {
        //111027 Rick add for reload ADC table while check similar timing - B39609
        wWidth = HFreq( SrcHPeriod );//(( DWORD )MST_CLOCK_MHZ * 10 + SrcHPeriod / 2 ) / SrcHPeriod; //calculate hfreq: round 5
        wWidth = (( DWORD )wWidth * StandardModeHTotal + 5000 ) / 10000; //dclk= hfreq * htotal
        drvADC_SetModewithPLLProtection(g_bInputSOGFlag ? ADC_INPUTSOURCE_YPBPR : ADC_INPUTSOURCE_RGB, wWidth, StandardModeHTotal);

        drvADC_AdjustHTotal(StandardModeHTotal);

#if CHECK_MODE_WIDTH
        wWidth = SearchMaxWidth(vsyncTime);

        //printData(" width:%d", wWidth);

        if(abs(wWidth - StandardModeWidth) < 2)     //110930 Rick modified check similar timing condition
        {
            //printData(" similar width:%d", wWidth);

            return;
        }
#endif

        SC0_SET_IMAGE_WIDTH(StandardModeWidth);//msWrite2Byte( SC0_0B, StandardModeWidth);
        SC0_SET_IMAGE_HEIGHT(StandardModeHeight);//msWrite2Byte( SC0_09, StandardModeHeight );

        for( adjPhase = 0; adjPhase < PHASE_MAX; adjPhase += PHASE_STEP)
        {
            drvADC_SetPhaseCode( adjPhase );
            Delay1ms( vsyncTime + 3 );
            AUTO_PHASE_RESULT_READY();//WaitAutoStatusReady(SC0_8B, BIT1);
            //cksum = msRead2Byte(SC0_8E);
            //cksum = ( cksum << 16 ) | msRead2Byte(SC0_8C);
            cksum = AUTO_PHASE_READ_VALUE();
            if( cksum > maxcksum )
            {
                maxcksum = cksum;
            }
        }

        if(maxcksum > maxcksum1)
        {
            maxcksum1 = maxcksum;
            ucIndex = SrcModeIndex;
            //printData(" SrcModeIndex_MaxCksm:%d", SrcModeIndex);
        }

        SrcModeIndex = ucSimilarTimingBuffer[i++];

    }

    SrcModeIndex = ucIndex;

            //printData(" SrcModeIndexFinal:%d", SrcModeIndex);

}

#define DIGITAL_TOLERANCE   3
Bool mStar_FindMode( void )
{
    WORD hFreq, vFreq;
    BYTE htol, vtol;
    WORD tempHeight, tempWidth;
    bit bDigitalInput = 0;

#define fStatus  hFreq
    fStatus = mStar_GetInputStatus();
    if( SyncPolarity( SrcFlags ) != SyncPolarity( fStatus ) )        // Sync polarity changed
    {
        return FALSE;
    }
    SrcFlags &= 0x0F;
#undef fStaus

    hFreq = SC0_READ_HPEROID();//msRead2Byte(SC0_E4) & 0x1FFF;
	#if ENABLE_FREESYNC
	if (!IS_DP_FREESYNC())
	#endif
	{
		if( abs( hFreq - SrcHPeriod ) > HPeriod_Torlance )        // HPeriod changed
		{
			return FALSE;
		}
	}

    vFreq = SC0_READ_VTOTAL();//msRead2Byte(SC0_E2) & 0xFFF;
    #if ENABLE_FREESYNC
	if (!IS_HDMI_FREESYNC())
	#endif
	{
        if( abs( vFreq - SrcVTotal ) > VTotal_Torlance )        // vtotal changed
        {
        	//MODE_printData("--vtotal change--");
            return FALSE;
        }
    }

#if ENABLE_FREESYNC
    if(CURRENT_INPUT_IS_HDMI())
        msDrv_forceIP1VsyncPolarityEnable();
#endif

    /////////////////////////////////////////////////////////

    if( SrcHPeriod > 511 )        // prevent counter overflow when input frequency is very low
    {
        hFreq = SrcHPeriod * 16;
    }
    else
    {
        tempHeight = GetVSyncTime() * 8;
        SC0_HPEROID_DETECT_MODE(TRUE);
        Delay1ms( tempHeight );
        hFreq = SC0_READ_HPEROID();
        SC0_HPEROID_DETECT_MODE(FALSE);
        Delay1ms( tempHeight );
    }

    hFreq=(hFreq+8)/16;
    MODE_printData( " FindMode_Hperoid:%d", hFreq );
    /////////////////////////////////////////////////////////

    SrcVTotal = vFreq;
#if ENABLE_FREESYNC
    if(IS_DP_FREESYNC())
    {
        WORD u16PixClk, u16Htt;

        u16PixClk = msAPI_combo_IPGetPixelClk();
        u16Htt = msAPI_combo_IPGetGetHTotal();
        if((u16Htt!=0)&&(SrcVTotal!=0))
        {
            hFreq = ((DWORD)u16PixClk * 100 + u16Htt/2)/u16Htt;
            vFreq = ((DWORD)hFreq * 1000 + SrcVTotal/2)/SrcVTotal;
        }
    }
    else
#endif
    {
        SrcHPeriod = hFreq;
        hFreq = HFreq( SrcHPeriod );
        vFreq = VFreq( hFreq, SrcVTotal );
    }

    //======== for interlace mode
    if( (SC0_READ_SYNC_STATUS()&INTM_B) //msReadByte( SC0_E1 ) &INTM_B
#if ENABLE_DP_INPUT
    ||(CURRENT_INPUT_IS_DISPLAYPORT() && DP_INTERLACED_VIDEO_STREAM() )
#endif
    )
    {
        SrcFlags |= bInterlaceMode;
        vFreq *= 2;
    }

    SrcVFreq = vFreq;

    MODE_printData( " FindMode_hFreq:%d", hFreq );
    MODE_printData( " FindMode_vFreq:%d", vFreq );
    MODE_printData( " SrcVFreq:%d", SrcVFreq );

    //====================================
    // check if input timing is out of range
#if 0//!ENABLE_MHL
    //110929 Rick modified for detect unsupport mode if input vFreq out of Panel spec range - A040
    if( vFreq > PanelMaxVfreq|| vFreq < PanelMinVfreq )
    {
        SrcFlags |= bUnsupportMode;
        return TRUE;
    }
#endif

    htol = HFreq_Torlance; //0527 add for getting more accurate mode
    vtol = VFreq_Torlance;

    ucSimilarTimingNums = 0;
    SrcModeIndex = 0xFF;

    if(!CURRENT_INPUT_IS_VGA())//(SrcInputType >= Input_Digital)
    {
        bDigitalInput = 1;
#if ENABLE_DP_INPUT     //111027 Rick modified - B39435
        if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType == Input_Displayport)
        {
            tempWidth = (SC0_READ_AUTO_WIDTH()*2);
            tempHeight = (SC0_READ_AUTO_HEIGHT());
        }
        else
#endif
        {
        tempWidth = SC0_READ_AUTO_WIDTH();//msRead2Byte(SC0_84)-msRead2Byte(SC0_80)+1; // get DE width
        tempHeight = SC0_READ_AUTO_HEIGHT();//msRead2Byte(SC0_82)-msRead2Byte(SC0_7E)+1; // get DE Height
        }
    }

#if (CHIP_ID == CHIP_TSUM2) || (CHIP_ID == CHIP_TSUMU) || (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)
    if (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP)
        tempHeight=tempHeight*2-SrcVTotal;
#endif

    // search input mode index
    {
        Bool found = FALSE;
        BYTE ucModeIndex;
        BYTE ucHVtolerance_min = HFreq_Torlance+VFreq_Torlance;
        BYTE ucHVtolerance = HFreq_Torlance+VFreq_Torlance;
        BYTE ucVtotal_min = VTotal_Torlance;
        BYTE ucVtotal = VTotal_Torlance;
        InputModeType *modePtr = StandardMode;
        BYTE ucSimilarindex=0;

        // get standard mode index
        for(ucModeIndex = 0; modePtr->HFreq > 0; modePtr++, ucModeIndex++)
        {
        if(bDigitalInput)
        {
                if((abs(tempWidth - StandardModeResolution[modePtr->ResIndex].DispWidth) > DIGITAL_TOLERANCE) ||
                        (abs(tempHeight - StandardModeResolution[modePtr->ResIndex].DispHeight) > DIGITAL_TOLERANCE))
                    continue;
        }

            if((CURRENT_SOURCE_IS_INTERLACE_MODE()) != (modePtr->Flags & bInterlaceMode))
                continue;

            if(SrcVTotal < StandardModeResolution[modePtr->ResIndex].DispHeight)
                continue;

            if((abs(hFreq - modePtr->HFreq) < HFreq_Torlance) &&
                (abs(vFreq - modePtr->VFreq) < VFreq_Torlance) &&
                (abs(SrcVTotal - modePtr->VTotal) < VTotal_Torlance)&&
                (GetSyncPolarity( SrcFlags )&modePtr->Flags))
            {
                ucHVtolerance = abs(hFreq - modePtr->HFreq) + abs(vFreq - modePtr->VFreq);
                ucVtotal = abs(SrcVTotal - modePtr->VTotal);

                if((ucHVtolerance_min >= ucHVtolerance) && (ucVtotal_min >= ucVtotal))
                {
                    found = TRUE;
                    ucHVtolerance_min = ucHVtolerance;
                    ucVtotal_min = ucVtotal;
                    SrcModeIndex = ucModeIndex;
                    ucSimilarTimingBuffer[ucSimilarindex] = ucModeIndex;
                    ucSimilarTimingNums ++;
                    ucSimilarindex = (ucSimilarindex+1)%SIMILAR_TIMING_BUFFER_MAX;
                }
            }
        }

        if( !found )
        {
            modePtr = StandardMode;
            ucHVtolerance_min = HFREQ_TOLERANCE_ENLARGE + VFREQ_TOLERANCE_ENLARGE;
            ucVtotal_min = VTotal_Delta;

            for(ucModeIndex = 0; modePtr->HFreq > 0; modePtr++, ucModeIndex++)
            {
                if(bDigitalInput)
                {
                    if((abs(tempWidth - StandardModeResolution[modePtr->ResIndex].DispWidth) > DIGITAL_TOLERANCE) ||
                        (abs(tempHeight - StandardModeResolution[modePtr->ResIndex].DispHeight) > DIGITAL_TOLERANCE))
                        continue;
                }

                if((CURRENT_SOURCE_IS_INTERLACE_MODE()) != (modePtr->Flags & bInterlaceMode))
                    continue;

                if(SrcVTotal < StandardModeResolution[modePtr->ResIndex].DispHeight)
                    continue;

                if((abs(hFreq - modePtr->HFreq) < HFREQ_TOLERANCE_ENLARGE) &&
                    (abs(vFreq - modePtr->VFreq) < VFREQ_TOLERANCE_ENLARGE) &&
                    (abs(SrcVTotal - modePtr->VTotal) < VTotal_Delta))
                {
                    ucHVtolerance = abs(hFreq - modePtr->HFreq) + abs(vFreq - modePtr->VFreq);
                    ucVtotal = abs(SrcVTotal - modePtr->VTotal);

                    if((ucHVtolerance_min >= ucHVtolerance) && (ucVtotal_min >= ucVtotal))
                    {
                        found = TRUE;
                        ucHVtolerance_min = ucHVtolerance;
                        ucVtotal_min = ucVtotal;
                        SrcModeIndex = ucModeIndex;
                        ucSimilarTimingBuffer[ucSimilarindex] = ucModeIndex;
                        ucSimilarTimingNums ++;
                        ucSimilarindex = (ucSimilarindex+1)%SIMILAR_TIMING_BUFFER_MAX;
                    }
                }
            }
        }

        if( !found )            // out of standard input range
        {
            modePtr = StandardMode;
            ucVtotal_min = VTotal_Delta;

            for(ucModeIndex = 0; modePtr->HFreq > 0; modePtr++, ucModeIndex++)
            {
                if((CURRENT_SOURCE_IS_INTERLACE_MODE()) != (modePtr->Flags & bInterlaceMode))
                    continue;

                if(SrcVTotal < StandardModeResolution[modePtr->ResIndex].DispHeight)
                    continue;

                if(abs(SrcVTotal - modePtr->VTotal) < VTotal_Delta)
                {
                    ucVtotal = abs(SrcVTotal - modePtr->VTotal);

                    if(ucVtotal_min > ucVtotal)
                    {
                        found = TRUE;
                        SrcFlags |= bUserMode;
                        ucVtotal_min = ucVtotal;
                        SrcModeIndex = ucModeIndex;
                        ucSimilarTimingBuffer[ucSimilarindex] = ucModeIndex;
                        ucSimilarTimingNums ++;
                        ucSimilarindex = (ucSimilarindex+1)%SIMILAR_TIMING_BUFFER_MAX;
                    }
                }
            }
        } // out of standard input range

        if(CURRENT_INPUT_IS_VGA())
            appCheckSimilarTiming();

        if( !found 
#if ENABLE_FREESYNC
			&&(!IS_HDMI_FREESYNC() && !IS_DP_FREESYNC())
#endif
		)
        {

            MODE_printMsg( "Not Found--> not support" );

            SrcFlags |= bUnsupportMode;
        }
    #if 0//!ENABLE_HDMI
        else if(CURRENT_SOURCE_IS_INTERLACE_MODE())
        {
            MODE_printMsg( "Interlace --> not support" );
            SrcFlags |= bUnsupportMode;
        }
    #endif
        else   // found
        {
                MODE_printData("  ucSimilarTimingNums:%d", ucSimilarTimingNums);
                MODE_printData("  SrcModeIndex:%d", SrcModeIndex);
                MODE_printData("  StandardModeWidth:%d", StandardModeWidth);
                MODE_printData("  StandardModeHeight:%d", StandardModeHeight);
        }

    } // search mode index

    return TRUE;
}
#endif





