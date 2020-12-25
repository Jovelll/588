#ifndef _MS_SCALER_J_H_
#define _MS_SCALER_J_H_

#ifdef _MS_SCALER_C_
#define INTERFACE
#else
#define INTERFACE   extern
#endif

#define SC_LOCK_LINE            8
#define HSC_offset              1
#define SC_V_SC_NON_MINUS1MODE  0
#define SC_H_SC_NON_MINUS1MODE  0

enum
{
    // HSP/VSP/FIFO in/out src sel
    FIFO_VSP_HSP_FIFO = ((0x01<<4)|(0x00<<6)|(0x00<<8)|(0x01<<10)|(0x00<<12)|(0x01<<14)),
    FIFO_HSP_VSP_FIFO = ((0x00<<4)|(0x01<<6)|(0x01<<8)|(0x00<<10)|(0x01<<12)|(0x00<<14)),
};


typedef struct
{
    WORD wImgSizeInH;       /* Input L or R Image Size */
    WORD wImgSizeInV;
    WORD wImgSizeOutH;      /* Output L or R Image Size */
    WORD wImgSizeOutV;
	
    BYTE ucSCFmtIn;
    BYTE ucSCPathMode;

    BYTE bScalingUpH:1;
    BYTE bScalingUpV:1;

} SetupPathInfo;

enum
{
    SC_PATH_MODE_0,         /* Frame bufferless */
};
enum
{
    SC_FMT_IN_NORMAL,
};

INTERFACE void mStar_SetupPathInit();
INTERFACE void mStar_SetupPath();
INTERFACE void mStar_SetScalingFilter( void );
INTERFACE void mStar_SetScalingFactor( void );

extern xdata SetupPathInfo g_SetupPathInfo;


#undef INTERFACE
#endif
