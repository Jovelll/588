#ifndef _MS_SCALER_2_H_
#define _MS_SCALER_2_H_

#ifdef _MS_SCALER_C_
#define INTERFACE
#else
#define INTERFACE   extern
#endif



#define IS_MRW_FMT_LA()         ((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSH) || \
                                (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSF) || \
                                (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA) )

#define IS_FMT_IN_SBS()         ((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSH) || \
                                (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSF) )

#define IS_VSP_FMT_2IMG()       (((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI) ||  \
                                 (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FA) ||  \
                                 (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP) ||  \
                                 (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF)) &&  \
                                 ((g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3) ||  \
                                 (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_4) ))

#define IS_FMT_OUT_PSV()        ((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV) || \
                                (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M) )


#define SC_LOCK_LINE            8
#define HSC_offset              1
#define SC_V_SC_NON_MINUS1MODE  0
#define SC_H_SC_NON_MINUS1MODE  0

enum 
{
    // MRW in/out src sel
    IP2MRW2VSP =  ((0x00)|(0x01<<2)),
    HSP2MRW2VSP = ((0x02)|(0x01<<2)),
    VSP2MRW2HSP = ((0x01)|(0x00<<2)),
    VSP2MRW2OP1 = ((0x01)|(0x02<<2)),
    
    // VSP in/out src sel
    MRW2VSP2HSP_OP = BIT6, // case1 , case2
    IP_HSP2VSP2MRW = BIT7, // case3, case4
    IP_HSP2VSP2OP  = 0,    // FBL mode
};


//Jison:110428 scWrite3Byte() ucReg was change to mean low addr.
#define MRW_L_WR_MSB_0          SC6_10//SC6_12
#define MRW_L_WR_MSB_1          SC6_13//SC6_15
#define MRW_L_WR_MSB_2          SC6_16//SC6_18
#define MRW_L_WR_MSB_3          SC6_19//SC6_1B
#define MRW_L_WR_MSB_4          SC6_1C//SC6_1E
#define MRW_L_WR_MSB_5          SC6_1F//SC6_21
#define MRW_L_WR_MSB_6          SC6_22//SC6_24
#define MRW_L_WR_MSB_7          SC6_25//SC6_27
#define MRW_R_WR_MSB_0          SC6_28//SC6_2A
#define MRW_R_WR_MSB_1          SC6_2B//SC6_2D
#define MRW_R_WR_MSB_2          SC6_2E//SC6_30
#define MRW_R_WR_MSB_3          SC6_31//SC6_33
#define MRW_R_WR_MSB_4          SC6_34//SC6_36
#define MRW_R_WR_MSB_5          SC6_37//SC6_39
#define MRW_R_WR_MSB_6          SC6_3A//SC6_3C
#define MRW_R_WR_MSB_7          SC6_3D//SC6_3F

#define MRW_L_WR_LSB_0          SC6_40//SC6_42
#define MRW_L_WR_LSB_1          SC6_43//SC6_45
#define MRW_L_WR_LSB_2          SC6_46//SC6_48
#define MRW_L_WR_LSB_3          SC6_49//SC6_4B
#define MRW_L_WR_LSB_4          SC6_4C//SC6_4E
#define MRW_L_WR_LSB_5          SC6_4F//SC6_51
#define MRW_L_WR_LSB_6          SC6_52//SC6_54
#define MRW_L_WR_LSB_7          SC6_55//SC6_57
#define MRW_R_WR_LSB_0          SC6_58//SC6_5A
#define MRW_R_WR_LSB_1          SC6_5B//SC6_5D
#define MRW_R_WR_LSB_2          SC6_5E//SC6_60
#define MRW_R_WR_LSB_3          SC6_61//SC6_63
#define MRW_R_WR_LSB_4          SC6_64//SC6_66
#define MRW_R_WR_LSB_5          SC6_67//SC6_69
#define MRW_R_WR_LSB_6          SC6_6A//SC6_6C
#define MRW_R_WR_LSB_7          SC6_6D//SC6_6F

#define MRW_L_RD_LSB_0          SC6_70//SC6_72
#define MRW_L_RD_LSB_1          SC6_73//SC6_75
#define MRW_L_RD_LSB_2          SC6_76//SC6_78
#define MRW_L_RD_LSB_3          SC6_79//SC6_7B
#define MRW_L_RD_LSB_4          SC6_7C//SC6_7E
#define MRW_L_RD_LSB_5          SC6_7F//SC6_81
#define MRW_L_RD_LSB_6          SC6_82//SC6_84
#define MRW_L_RD_LSB_7          SC6_85//SC6_87
#define MRW_L_RD_MSB_0          SC6_88//SC6_8A
#define MRW_L_RD_MSB_1          SC6_8B//SC6_8D
#define MRW_L_RD_MSB_2          SC6_8E//SC6_90
#define MRW_L_RD_MSB_3          SC6_91//SC6_93
#define MRW_L_RD_MSB_4          SC6_94//SC6_96
#define MRW_L_RD_MSB_5          SC6_97//SC6_99
#define MRW_L_RD_MSB_6          SC6_9A//SC6_9C
#define MRW_L_RD_MSB_7          SC6_9D//SC6_9F

//Jison:110428 scWrite2Byte() ucReg was change to mean low addr.
#define MRW_L_MSB_OFFSET_LINE   SC6_E4//SC6_E5
#define MRW_L_LSB_OFFSET_LINE   SC6_E6//SC6_E7
#define MRW_R_MSB_OFFSET_LINE   SC6_E8//SC6_E9
#define MRW_R_LSB_OFFSET_LINE   SC6_EA//SC6_EB
#define MRW_L_MSB_OFFSET_ACT    SC6_EC//SC6_ED
#define MRW_L_LSB_OFFSET_ACT    SC6_EE//SC6_EF
#define MRW_R_MSB_OFFSET_ACT    SC6_F0//SC6_F1
#define MRW_R_LSB_OFFSET_ACT    SC6_F2//SC6_F3


typedef struct
{
    WORD wImgSizeInH;       /* Input L or R Image Size */
    WORD wImgSizeInV;
    WORD wImgSizeOutH;      /* Output L or R Image Size */
    WORD wImgSizeOutV;
    WORD wMRWSizeInH;
    WORD wMRWSizeInV;
    WORD wMRWSizeOutH;
    WORD wMRWSizeOutV;
    BYTE ucSCFmtIn;
    BYTE ucSCFmtOut;

    BYTE ucSCPathMode;
    BYTE ucIPLRSel;
    BYTE ucOut3DLRSel;

    BYTE bOverrideSCFmtIn:1;
    BYTE bPsvPnlOutLRInvert:1;
    BYTE bScalingUpH:1;
    BYTE bScalingUpV:1;
    BYTE bHSDEnable:1;
    BYTE bMRWDual:1;
    BYTE bMRW422:1;
    BYTE bMRWR2Y:1;

    BYTE bMRWLLRR:1;
    BYTE bVBIExtend:1;
    BYTE bFBMode:1;

#if CHIP_ID == CHIP_TSUM2
    BYTE b2DTo3DEnable:1;
    BYTE bLoadOdTbl:1;
#endif
} SetupPathInfo;

#if 0//CHIP_ID == CHIP_TSUMU
enum
{
    SC_PATH_MODE_0,         /* Frame bufferless */
    SC_PATH_MODE_1,         /* mode 1,2 is auto switch */
//  SC_PATH_MODE_2,         /* mode 1,2 is auto switch */
    SC_PATH_MODE_3,
    SC_PATH_MODE_4,
    SC_PATH_MODE_5,
    SC_PATH_MODE_6,
};
#elif CHIP_ID == CHIP_TSUM2
enum
{
    SC_PATH_MODE_0,         /* Frame bufferless */
    SC_PATH_MODE_1,         
    SC_PATH_MODE_2,        
    SC_PATH_MODE_3,
    SC_PATH_MODE_4,
    SC_PATH_MODE_5,
    SC_PATH_MODE_6,
};
#endif

enum
{
    SC_FMT_IN_NORMAL,
    SC_FMT_IN_3D_PF,        /* page flipping */
    SC_FMT_IN_3D_FP,        /* frame packing */
    SC_FMT_IN_3D_FPI,       /* frame packing interlace */
    SC_FMT_IN_3D_TB,        /* top bottom */
    SC_FMT_IN_3D_SBSH,      /* side by side half */
    SC_FMT_IN_3D_LA,        /* line interleave */
    SC_FMT_IN_3D_SBSF,      /* side by side full */
    SC_FMT_IN_3D_CB,        /* chess board */
    SC_FMT_IN_3D_FA,        /* filed alternative */
};

enum
{
    SC_FMT_OUT_3D_FS,       /* frame sequential */
    SC_FMT_OUT_3D_PSV,      /* passive */
    SC_FMT_OUT_3D_PSV_4M,   /* 4M embedded passive */
};

enum
{
    OUT_3D_FIXED_NORMAL,
    OUT_3D_FIXED_L,
    OUT_3D_FIXED_R,
};


INTERFACE void mStar_SetupPathInit();
INTERFACE void mStar_SetupPath();
INTERFACE void mStar_SetScalingFilter( void );
INTERFACE void mStar_SetScalingFactor( void );

extern xdata SetupPathInfo g_SetupPathInfo;


#undef INTERFACE
#endif
