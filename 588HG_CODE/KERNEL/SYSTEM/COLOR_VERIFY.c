#include "board.h"
#include "global.h"
#include "ms_rwreg.h"
#include "gamma.h"
#include "msHDMI.h"
#include "drv3DLUT_C.h"
#include "color_verify.h"

#if ENABLE_DEBUG && COLORVERIFY_DEBUG
#if CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD|| CHIP_ID==CHIP_TSUMJ
code RegUnitType tb_FCC_TABLE[] =
{
    {SC7_80, 0x64},     //target 1 cb, 100,     Red
    {SC7_81, 0xd4}, //target 1 cr, 212,     Red
    {SC7_82, 0x48},     //target 2 cb, 72,  Green
    {SC7_83, 0x3a},     //target 2 cr, 58,  Green
    {SC7_84, 0xd4},     //target 3 cb, 212,     Blue
    {SC7_85, 0x72},     //target 3 cr, 114,     Blue
    {SC7_86, 0x9c}, //target 4 cb, 156,     Cyan
    {SC7_87, 0x2c},     //target 4 cr, 44,  Cyan
    {SC7_88, 0xb8},     //target 5 cb, 184,     Magenta
    {SC7_89, 0xc6},     //target 5 cr, 198,     Magenta
    {SC7_8A, 0x2c},     //target 6 cb, 44,  Yellow
    {SC7_8B, 0x8e},     //target 6 cr, 142,     Yellow
    {SC7_8C, 0x80},     //target 7 cb, 128,     White
    {SC7_8D, 0x80},     //target 7 cr, 128,     White
    {SC7_8E, 0x80},     //target 8 cb, 128,     Black
    {SC7_8F, 0x80}, //target 8 cr, 128,     Black
    {SC7_92, 0xff}, //target 1 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_93, 0xff}, //target 2 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_94, 0xff}, //target 3 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_95, 0xff}, //target 4 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_96, 0xff}, //target 5 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_97, 0xff}, //target 6 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_98, 0xff}, //target 7 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_99, 0xff}, //target 8 boundary, [7:6] cb_u, [5:4] cb_d, [3:2] cr_u, [1:0] cr_d
    {SC7_9A, 0xff}, //target 9 boundary, [5:3] cb_ud, [2:0] cr_ud,

    {SC7_9B, 0xff}, //target 1 gain, [7:4], equal 15/16             //target 2 gain, [3:0], equal 15/16
    {SC7_9C, 0xff}, //target 3 gain, [7:4], equal 15/16         //target 4 gain, [3:0], equal 15/16
    {SC7_9D, 0xff}, //target 5 gain, [7:4], equal 15/16         //target 6 gain, [3:0], equal 15/16
    {SC7_9E, 0xff}, //target 7 gain, [7:4], equal 15/16         //target 8 gain, [3:0], equal 15/16

    {SC7_90,0xff},  //target 0~9 enable, [0] for target 1 and 2, [1:7] for target 2~9

    {SC0_00, 0x00},
};
#endif

//>>===========================================

void msFCCLoader(void)
{
    xdata BYTE  ucBank,ucTmp;

    ucBank = msReadByte(SC0_00);
#if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD|| CHIP_ID==CHIP_TSUMJ
    msWriteByteMask(SCB_A0, BIT4|BIT0   , BIT4|BIT0);
#endif
    for(ucTmp = 0; ucTmp < sizeof( tb_FCC_TABLE ) / sizeof( RegUnitType ); ucTmp++ )
        msWriteByte( tb_FCC_TABLE[ucTmp].u16Reg, tb_FCC_TABLE[ucTmp].u8Value );

    msWriteByte(SC0_00, ucBank);

}
//<<===========================================================
//>>===============msHistrogram_Test=============================
void msHistrogram_Test(BYTE bWindow)
{
    xdata BYTE  ucBank,ucTmp;
    xdata WORD  uwArray32[32];                                  /*use to Story Histrogram32's DLC data*/

    xdata WORD  h_start =   0,  v_start =   0;
    xdata WORD  h_end   = PanelWidth,  v_end   = PanelHeight;
    //xdata DWORD Total_Count,Total_Weight;                   /*use to Story Temp.Histrogram DLC data */
    xdata DWORD Total_Weight;                   /*use to Story Temp.Histrogram DLC data */

    //PanelWidth
    ucBank = msReadByte(SC0_00);

#if ENABLE_HDMI
    if( gScInfo.InputColor == InputColor_RGB )
        msWriteByteMask(SC7_3B,_BIT6,_BIT6);                    //RGB mode enable
    else
#endif
        msWriteByteMask(SC7_3B,0x00,_BIT6);                     //RGB mode enable

    msWriteByteMask(SC7_B8,_BIT4,_BIT4);                    //New Y' histogram option enable. 0: Histogram in Y. 1: Histogram in Y'.
    msWriteByteMask(SC7_B5,_BIT1,_BIT1);                //Histogram accelerate mode ON
    msWriteByteMask(SC7_B8, 0, (BIT3|BIT2) );               //clear hand shaking
    if( MAIN_WINDOW == bWindow )
        msWriteByteMask( SC7_B8, BIT1, BIT1|BIT0);              //Main Histrogram window enable
    else
        msWriteByteMask( SC7_B8, BIT0, BIT1|BIT0);              //Sub Histrogram window enable

//>>--------------------Range-------------------------------------------------------
    #if 1 //V range doesnt change , but H range is moved to SC7
    msWriteByte( SC3_B6, v_start/8 );                           // set statistic start range (unit: 8 vertical lines)
    msWriteByte( SC3_B7, v_end  /8 );                            // set statistic end range (uint: 8 vertical lines)
    msWriteByteMask( SC3_B5, BIT0, BIT0 );                      // set statistic vertical range is valid

    msWrite2Byte( SC7_24, h_start);             // set statistic start range (unit: 1 pixel)
    msWrite2Byte( SC7_26, h_end );                          // set statistic start range (unit: 1 pixel)
    msWriteByteMask( SC7_27, (MAIN_WINDOW==bWindow)?(BIT7):(BIT6), BIT7|BIT6 );                 // 1 for set manual H range setting enable in Main window

    printData("V_Start_(SC3_B6)==[%d]\n\r",msReadByte(SC3_B6)*8);
    printData("V_End_(SC3_B7)==[%d]\n\r",msReadByte(SC3_B7)*8);
    printData("H_Start_(SC7_2524)==[%d]\n\r",h_start);
    printData("H_End_(SC7_2726)==[%d]\n\r",h_end);
    #endif
//<<-----------------------------------------------------------------------------------
//>>--------------------Enable Histogram Report----------------
    msWriteByteMask( SC7_B8, ((MAIN_WINDOW==bWindow)?(BIT1):(BIT0))|BIT2, BIT2|BIT1|BIT0 );                 // 1 for set manual H range setting enable in Main window
//<<-----------------------------------------------------------------------------------
//>>--------------------32 Section Histogram report----------------
    while(!(msReadByte(SC7_B8) & _BIT3))  /*SC7_B8[3] HS_REQ:Hand Shaking Acknowledge (Read only) */
    {
    }
    printData("111111.(total)==[%x]\n\r",(PanelWidth/64*PanelHeight));
    printData("111111.(Weight)==[%x]\n\r",(PanelWidth/64*PanelHeight*256));
    for (ucTmp = 0; ucTmp < 32; ucTmp++)
    {
        uwArray32[ucTmp] = ((WORD) msRead2Byte(SC8_C0+ucTmp*2));
        printData("ADD[]==[%x]\n\r",(SC8_C0+ucTmp*2));

        printData("uwArray32[]==[%x]\n\r",uwArray32[ucTmp]);
    }

//<<-----------------------------------------------------------------------------------
//>>--------------------Total Weight and Count , Min. and Maximum report----------------
    //Total_Count=msRead2Byte(SC7_34); //removed in Russell
    Total_Weight=msRead2Byte(SC7_36);
    printData("222222.minimum pixel value ==[%x]\n\r",msReadByte(SC7_39));
    printData("222222.maximum pixel value ==[%x]\n\r",msReadByte(SC7_3A));

    //printData("222222.Sum of pixel count==[%x]\n\r",Total_Count);
    printData("222222.Sum of total weight.==[%x]\n\r",Total_Weight);
//<<-----------------------------------------------------------------------------------
msWriteByte(SC0_00, ucBank);

}
//<<===========================================================
//>>===============Linear Gamma Start=============================
#if Enable_Gamma
void msLinearGammaLoader(void)
{
    BYTE xdata ucBank;
    BYTE xdata ucTgtChannel;
    WORD xdata uwData = 0;
    WORD xdata i;
    ucBank = scReadByte( SC0_00);
    msWrite2Byte( SC8_20, 0 );

    for( ucTgtChannel = 0; ucTgtChannel < 3; ucTgtChannel++ )
    {
        msWriteByteMask( SC8_22, 0x03,BIT1|BIT0);// Burst write enable
        msWriteByteMask( SC8_22, (ucTgtChannel<<2) , BIT3 | BIT2 ); //Select R/G/B Channel
        for( i = 0; i < 256; ++ i )
        {
            uwData = i<<2; // 10 bit data
            msWrite2Byte( SC8_24, uwData );
        }
        msWriteByteMask( SC8_22, 0, BIT1 | BIT0 );
    }
    msWriteByte( SC0_00, ucBank );
}
//<<===========================================================
//>>===============DeGamma Start=============================
BYTE code DeGammaTbl[] =
{
     0x00,0x0F,0x1E,0x2C,0x3B,0x4A,0x59,0x67
    ,0x76,0x85,0x94,0xA2,0xB1,0xC0,0xCE,0xDD
    ,0x76,0x7D,0x84,0x8B,0x92,0x99,0xA0,0xA7
    ,0xAE,0xB4,0xBB,0xC2,0xC9,0xCF,0xD6,0xDC
    ,0x72,0x75,0x78,0x7B,0x7E,0x82,0x85,0x88
    ,0x8B,0x8E,0x91,0x94,0x97,0x9A,0x9D,0x9F
    ,0xA2,0xA5,0xA8,0xAB,0xAD,0xB0,0xB3,0xB5
    ,0xB8,0xBB,0xBD,0xC0,0xC2,0xC5,0xC7,0xCA
    ,0x4C,0x4E,0x51,0x53,0x55,0x58,0x5A,0x5C
    ,0x5E,0x60,0x63,0x65,0x67,0x69,0x6B,0x6D
    ,0x6F,0x70,0x72,0x74,0x76,0x78,0x79,0x7B
    ,0x7D,0x7E,0x80,0x82,0x83,0x85,0x86,0x88
    ,0x89,0x8A,0x8C,0x8D,0x8E,0x90,0x91,0x92
    ,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A
    ,0x9B,0x9C,0x9D,0x9E,0x9E,0x9F,0xA0,0xA0
    ,0xA1,0xA1,0xA2,0xA2,0xA3,0xA3,0xA4,0xA4
    ,0xA4,0xA4,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5
    ,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA4,0xA4
    ,0xA4,0xA4,0xA3,0xA3,0xA2,0xA2,0xA1,0xA1
    ,0xA0,0x9F,0x9F,0x9E,0x9D,0x9C,0x9C,0x9B
    ,0x9A,0x99,0x98,0x97,0x96,0x95,0x93,0x92
    ,0x91,0x90,0x8E,0x8D,0x8C,0x8A,0x89,0x87
    ,0x86,0x84,0x82,0x81,0x7F,0x7D,0x7B,0x79
    ,0x77,0x76,0x74,0x72,0x6F,0x6D,0x6B,0x69
    ,0x67,0x64,0x62,0x60,0x5D,0x5B,0x58,0x56
    ,0x53,0x51,0x4E,0x4B,0x48,0x46,0x43,0x40
    ,0x3D,0xBA,0xB7,0xB4,0xB1,0xAE,0xAA,0xA7
    ,0xA4,0xA1,0x9D,0x9A,0x96,0x93,0x8F,0x8C
    ,0x88,0x84,0x81,0x7D,0x79,0x75,0x71,0x6D
    ,0x69,0x65,0x61,0x5D,0x59,0x55,0x50,0x4C
    ,0x48,0x43,0x3F,0x3A,0x36,0x31,0x2C,0x28
    ,0x23,0x1E,0x19,0x14,0x0F,0x0A,0x05,0x00
};

void msLinearDeGammaLoader(void)
{
    BYTE xdata ucBank;
    BYTE xdata ucTgtChannel;
    WORD xdata uwData = 0;
    WORD xdata i;
    ucBank = scReadByte( SC0_00);
    msWrite2Byte( SCE_C0, 0 );
    msWriteByteMask( SCE_02, BIT0, BIT0 );//DE-Gamma enable

    for( ucTgtChannel = 0; ucTgtChannel < 3; ucTgtChannel++ )
    {
        msWriteByteMask( SCE_C2, 0x03,BIT1|BIT0);// Burst write enable
        msWriteByteMask( SCE_C2, (ucTgtChannel<<2) , BIT3 | BIT2 ); //Select R/G/B Channel
        for( i = 0; i < 256; ++ i )
        {
            uwData =( i<<4); // 15 bit data
            printData("DeGamma Data==[%x]\n\r",i<<4);

            msWrite2Byte( SCE_C4, uwData );
            //msWrite2Byte(SCE_C4, DeGammaTbl[i]);
            printData("DeGamma Data==[%x]\n\r",uwData);

        }
        msWriteByteMask( SCE_C2, 0, BIT1 | BIT0 );
    }
    //msWriteByteMask( SCE_02, BIT0, BIT0 );
    msWriteByte( SC0_00, ucBank );
}

void msLinearFixGammaLoader(void)
{
    BYTE xdata ucBank;
    BYTE xdata ucTgtChannel;
    WORD xdata uwData = 0;
    WORD xdata i;
    ucBank = scReadByte( SC0_00);
    msWriteByte( SCE_E0, 0 );
    msWriteByteMask( SCE_02, BIT2, BIT2 ); //Fixed Gamma enable

    for( ucTgtChannel = 0; ucTgtChannel < 3; ucTgtChannel++ )
    {
        msWriteByteMask( SCE_E2, 0x03,BIT1|BIT0);// Burst write enable
        msWriteByteMask( SCE_E2, (ucTgtChannel<<2) , BIT3 | BIT2 ); //Select R/G/B Channel
        for( i = 0; i < 256; ++ i )
        {
            uwData = i<<2; // 10 bit data
            msWrite2Byte( SCE_E4, uwData );
            //msWrite2Byte(SCE_E4, DeGammaTbl[i]);
            printData("FixGamma Data==[%x]\n\r",uwData);

        }
        msWriteByteMask( SCE_E2, 0, BIT1 | BIT0 );
    }
    //msWriteByteMask( SCE_02, BIT2, BIT2 );
    msWriteByte( SC0_00, ucBank );
}
#endif

void ms2DHPeakingTest(BYTE bScalerWin ,BYTE ucSharpness, BYTE ucStep )
{
#define NO_CHANGE_STEP      0x80
#define NO_CHANGE_SHARPNESS 0xFF


    BYTE xdata uci = 0;

    //Over/Under shoort Setting  //main and sub use the same setting
    for( uci = 0; uci < 8; uci ++ )
    {
         msWriteByteMask(SCB_40+uci, 0x00,0xFF);        //overshoort�GSCB_40~SCB_47
         msWriteByteMask(SCB_48+uci, 0xFF,0xFF);        //undershoort�GSCB_48~SCB_4F
    }
    if(bScalerWin == MAIN_WINDOW)
    {
        //>>----main win H peaking initial------------------
        //Enable
        msWriteByteMask( SCB_20, BIT0   , BIT0 );   //main post peaking enable
        msWriteByteMask( SCB_21, 0xFF   , 0xFF );   //main Bank1~8 peaking enable
        //Term Setting
        #if 0
        msWrite2ByteMask( SCB_38, 0x0123    , 0x7777 ); //main win peaking term1~4 select
        msWrite2ByteMask( SCB_3A, 0x0123    , 0x7777 ); //main win peaking term5~8 select
        msWrite2ByteMask( SCB_3C, 0x4567    , 0x7777 ); //main win peaking term9~12 select
        msWrite2ByteMask( SCB_3E, 0x4567    , 0x7777 ); //main win peaking term13~16 select
        #else
        for( uci = 0; uci < 4; uci ++ )
        {   //Term Setting for main win
             msWriteByteMask(SCB_38+uci*2   , (uci*2<<4)|(uci*2+1)  ,0x77);
             msWriteByteMask(SCB_38+uci*2+1, (uci*2<<4)|(uci*2+1) ,0x77);
        }
        #endif

        //Post Coring Setting
        msWriteByteMask( SCB_26, 0x42   , 0xFF );           //[7:4]:Coring_TH_2 ,[3:0]:Coring_TH_1
        for( uci = 0; uci < 8; uci ++ )
        {
             msWriteByteMask(SCB_30+uci, 0x20,0x3F);        //main win band 1~8 peaking coef
        }
        //<<--------------------------
        if( NO_CHANGE_SHARPNESS == ucSharpness )
            ucSharpness = msReadByte(SCB_27)&0x3F;    // MAIN window user sharpness adjust
        else
            ucSharpness&=0x3F;

        msWriteByte(SCB_27,  ucSharpness);
        printData("111 ucSharpness===%x",msReadByte(SCB_27));

        if( NO_CHANGE_STEP == ucStep )
            ucStep = (msReadByte(SCB_22)&0x03);                 // MAIN window peaking step
        else
            ucStep&=0x03;

        printData("111 ucStep===%x",ucStep);
        msWriteByte(SCB_22,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep));  //for band1~4
        msWriteByte(SCB_23,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep));  //for band5~8
        printData("111 ucStep===%x",msReadByte(SCB_22));
    }
    else
    {

        //>>----sub win H peaking initial------------------
        //Enable
        msWriteByteMask( SCB_28, BIT0   , BIT0 );   //sub post peaking enable
        msWriteByteMask( SCB_29, 0xFF   , 0xFF );   //sub Bank1~8 peaking enable
        //Term Setting
        #if 0
        msWrite2ByteMask( SCB_58, 0x0123    , 0x7777 ); //main win peaking term1~4 select
        msWrite2ByteMask( SCB_5A, 0x0123    , 0x7777 ); //main win peaking term5~8 select
        msWrite2ByteMask( SCB_5C, 0x4567    , 0x7777 ); //main win peaking term9~12 select
        msWrite2ByteMask( SCB_5E, 0x4567    , 0x7777 ); //main win peaking term13~16 select
        #else
        for( uci = 0; uci < 4; uci ++ )
        {
             msWriteByteMask(SCB_58+uci*2   , (uci*2<<4)|(uci*2+1)  ,0x77);
             msWriteByteMask(SCB_59+uci*2+1, (uci*2<<4)|(uci*2+1) ,0x77);
        }
        #endif
        //Post Coring Setting
        msWriteByteMask( SCB_2E, 0x42   , 0xFF );           //[7:4]:Coring_TH_2 ,[3:0]:Coring_TH_1
        for( uci = 0; uci < 8; uci ++ )
        {
             msWriteByteMask(SCB_50+uci, 0x20,0x3F);        //sub win band 1~8 peaking coef
        }

        //<<--------------------------
        if( NO_CHANGE_SHARPNESS == ucSharpness )
            ucSharpness = msReadByte(SCB_2F)&0x3F;    // MWE window user sharpness adjust
        else
            ucSharpness&=0x3F;
        msWriteByte(SCB_2F,  ucSharpness);

        if( NO_CHANGE_STEP == ucStep )
            ucStep = (msReadByte(SCB_2A)&0x03);                 // MWE window peaking step
        else
            ucStep&=0x03;

        printData("222 ucStep===%x",ucStep);
        msWriteByte(SCB_2A,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep));  //for band1~4
        msWriteByte(SCB_2B,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep));  //for band5~8
        printData("222 SCB_2B===%x",msReadByte(SCB_2B));
    }
    //msWriteByte(SC0_00, ucBank);
}

void msNoiseMasking(BYTE bScalerWin ,BYTE u8Strength, BYTE u8THRD )
{
	//Command1(BIT0:Main/Sub win, BIT1:NM enable, BIT2:LowY enable, BIT3:STD enable)
	//Command2: NM gain coef
	//Command3:THRD
	//EX:normal mode: 49 03 3f ff (main win,NM enable gain=0x3F)
	//EX:Low Y mode:  49 07 3f 20 (main win,NM enable ,LowY mode enable, gain=0x3F/2 and LowY's gain=0x3F,THRD=0x20)
	
	//1.Normal mode
	if(_bit0_(bScalerWin))
	{	
		printData("111.Main window (bScalerWin&BIT0)==>%x",(bScalerWin&BIT0));
		
		//H direction Noise masking
		msWriteByteMask(SCB_C0, (_bit1_(bScalerWin)?(BIT7):(0x00))		,BIT7);  	// Y noise-masking enable
		msWriteByteMask(SCB_C0, (_bit2_(bScalerWin)?((u8Strength<=0x3F? u8Strength/2:0x3F/2)):((u8Strength<=0x3F? u8Strength:0x3F))) 	,0x3F);  	//BIT5~BIT0:4.2 format
		msWriteByteMask(SCB_C2, 0xFF 	,0xFF); 									//Max/Min value threshold
		msWriteByteMask(SCB_C1, (_bit1_(bScalerWin)?(BIT7):(0x00))		,BIT7);  	//C noise-masking enable
		msWriteByteMask(SCB_C1, (_bit2_(bScalerWin)?((u8Strength<=0x3F? u8Strength/2:0x3F/2)):((u8Strength<=0x3F? u8Strength:0x3F))) 	,0x3F);  	//BIT5~BIT0:4.2 format
		msWriteByteMask(SCB_C3, 0xFF 	,0xFF); 									//Max/Min value threshold
		//V direction Noise masking
		msWriteByteMask(SCB_24, (_bit1_(bScalerWin)?(BIT7):(0x00))			,BIT7);  	// Y noise-masking enable
		msWriteByteMask(SCB_24, (_bit2_(bScalerWin)?((u8Strength<=0x3F? u8Strength/2:0x3F/2)):((u8Strength<=0x3F? u8Strength:0x3F))) 	,0x3F);  	//BIT5~BIT0:4.2 format
		msWriteByteMask(SCB_72, 0xFF 	,0xFF); 									//Max/Min value threshold
		if (_bit1_(bScalerWin))
			printMsg("111.Enable NM");
		else
			printMsg("111. disable NM");
		//2.LOW Y mode
		{	
			//H direction:
 			//Y
			{
				if (_bit2_(bScalerWin))
					printMsg("111. Enable Low Y mode");
				else
					printMsg("111. disable Low Y mode");
				
				msWriteByteMask(SCB_E0, (_bit2_(bScalerWin)?(BIT0):(0x00))	,BIT0); 		//reg_main_nm_low_y_en
				msWriteByteMask(SCB_E2, u8THRD	,0xFF);								//reg_main_nm_low_y_th
				msWriteByteMask(SCB_E4, (u8Strength<=0x3F? u8Strength:0x3F),0x3F);		//reg_main_nm_low_y_gain
				msWriteByteMask(SCB_E5, (0x00),BIT1|BIT0); 								//reg_main_nm_low_y_step
				printData("111.SCB_E0==%x",msReadByte(SCB_E0));
				//C
				msWriteByteMask(SCB_EA, (_bit2_(bScalerWin)?(BIT0):(0x00))	,BIT0); 		//reg_main_nm_low_y_en
				msWriteByteMask(SCB_EC, u8THRD	,0xFF);								//reg_main_nm_low_y_th
				msWriteByteMask(SCB_EE, (u8Strength<=0x3F? u8Strength:0x3F),0x3F);		//reg_main_nm_low_y_gain
				msWriteByteMask(SCB_EF, (0x00)	,BIT1|BIT0); 								//reg_main_nm_low_y_step
				printData("111.SCB_EA==%x",msReadByte(SCB_EA));
				//V direction:
				msWriteByteMask(SCB_F4,(_bit2_(bScalerWin)?(BIT0):(0x00))		,BIT0); 		//reg_main_v_nm_low_y_en
				msWriteByteMask(SCB_F6, u8THRD	,0xFF);								//reg_main_v_nm_low_y_th
				msWriteByteMask(SCB_E8, (u8Strength<=0x3F? u8Strength:0x3F),0x3F);		//reg_main_v_nm_low_y_gain
				msWriteByteMask(SCB_E9, (0x00),BIT1|BIT0); 								//reg_main_v_nm_low_y_step
				printData("111.SCB_F4==%x",msReadByte(SCB_F4));
			}
		}
		#if CHIP_ID==CHIP_TSUMD		
		//3.STD mode
		{	
			if (_bit3_(bScalerWin))
				printMsg("111. Enable STD mode");
			else
				printMsg("111. Disable STD mode");
			//H direction
			msWriteByteMask(SCB_E0, (_bit3_(bScalerWin)?(BIT1):(0x00))	,BIT1); 	//reg_main_nm_low_y_en
			msWriteByteMask(SCB_E1, (0x00),BIT1|BIT0); 								//reg_main_nm_std_step
			msWriteByteMask(SCB_E3, u8THRD,0xFF); 								//reg_main_nm_std_th
			//V direction
			msWriteByteMask(SCB_F4, (_bit3_(bScalerWin)?(BIT1):(0x00))	,BIT1); 		//reg_main_v_nm_std_en
			msWriteByteMask(SCB_F5, (0x00),BIT1|BIT0); 								//reg_main_v_nm_std_step
		}		
		#endif
	}
	else
	{
	
		printMsg("222.Sub window");
		//H direction Noise masking	
		msWriteByteMask(SCB_C8, (_bit1_(bScalerWin)?(BIT7):(0x00))		,BIT7);  	// Y noise-masking enable
		msWriteByteMask(SCB_C8, (_bit2_(bScalerWin)?((u8Strength<=0x3F? u8Strength/2:0x3F/2)):((u8Strength<=0x3F? u8Strength:0x3F))) 	,0x3F);  	//BIT5~BIT0:4.2 format
		msWriteByteMask(SCB_CA, 0xFF 	,0xFF); 			//Max/Min value threshold
		msWriteByteMask(SCB_C9, (_bit1_(bScalerWin)?(BIT7):(0x00))		,BIT7);  	//C noise-masking enable
		msWriteByteMask(SCB_C9, (_bit2_(bScalerWin)?((u8Strength<=0x3F? u8Strength/2:0x3F/2)):((u8Strength<=0x3F? u8Strength:0x3F))) 	,0x3F);  	//BIT5~BIT0:4.2 format		
		msWriteByteMask(SCB_CB, 0xFF 	,0xFF); 			//Max/Min value threshold
		//V direction Noise masking
		msWriteByteMask(SCB_2C, (_bit1_(bScalerWin)?(BIT7):(0x00))		,BIT7);  	// Y noise-masking enable
		msWriteByteMask(SCB_2C, (_bit2_(bScalerWin)?((u8Strength<=0x3F? u8Strength/2:0x3F/2)):((u8Strength<=0x3F? u8Strength:0x3F))) 	,0x3F);  	//BIT5~BIT0:4.2 format		
		msWriteByteMask(SCB_73, 0xFF 	,0xFF); 			//Max/Min value threshold
		if (_bit1_(bScalerWin))
			printMsg("222. Enable NM");
		else
			printMsg("222. Disable NM");
			
		//2.LOW Y mode
		{
			//Y
			{
				if (_bit2_(bScalerWin))
					printMsg("222. Enable LowY mode");
				else
					printMsg("222. Disable LowY mode");
			
				msWriteByteMask(SCB_E0, (_bit2_(bScalerWin)?(BIT4):(0x00))	,BIT4); 		//reg_sub_nm_low_y_en
				msWriteByteMask(SCB_E6, u8THRD	,0xFF);								//reg_sub_nm_low_y_th
				msWriteByteMask(SCB_E8, (u8Strength<=0x3F? u8Strength:0x3F),0x3F);		//reg_sub_nm_low_y_gain
				msWriteByteMask(SCB_E9, (0x00)	,BIT1|BIT0); 								//reg_sub_nm_low_y_step
				printData("2222.SCB_E0==%x",msReadByte(SCB_E0));
				//C
				msWriteByteMask(SCB_EA,(_bit2_(bScalerWin)?(BIT4):(0x00))	,BIT4); 		//reg_sub_nm_low_c_en
				msWriteByteMask(SCB_F0, u8THRD	,0xFF);								//reg_sub_nm_low_c_th
				msWriteByteMask(SCB_F2, (u8Strength<=0x3F? u8Strength:0x3F),0x3F);		//reg_sub_nm_low_c_gain
				msWriteByteMask(SCB_F3, (0x00)	,BIT1|BIT0); 								//reg_sub_nm_low_c_step
				printData("2222.SCB_EA==%x",msReadByte(SCB_EA));	
				//V direction:
				msWriteByteMask(SCB_F4, (_bit2_(bScalerWin)?(BIT4):(0x00))	,BIT4); 		//reg_sub_v_nm_low_y_en
				msWriteByteMask(SCB_FA, u8THRD	,0xFF);								//reg_sub_v_nm_low_y_th
				msWriteByteMask(SCB_EC, (u8Strength<=0x3F? u8Strength:0x3F),0x3F);		//reg_sub_v_nm_low_y_gain
				msWriteByteMask(SCB_ED, (0x00)	,BIT1|BIT0); 								//reg_sub_v_nm_low_y_step
				printData("2222.SCB_F4==%x",msReadByte(SCB_F4));
			}
		}
		#if CHIP_ID==CHIP_TSUMD
		//3.STD mode
		{	
			if (_bit3_(bScalerWin))
				printMsg("111. Enable STD mode");
			else
				printMsg("111. Disable STD mode");
			//H direction
			msWriteByteMask(SCB_E0, (_bit3_(bScalerWin)?(BIT5):(0x00))	,BIT5); 		//reg_sub_nm_low_y_en
			msWriteByteMask(SCB_E1, (0x00),BIT3|BIT2); 								//reg_sub_nm_std_step
			msWriteByteMask(SCB_E7, u8THRD,0xFF); 									//reg_main_nm_std_th
			//V direction				
			msWriteByteMask(SCB_F4, (_bit3_(bScalerWin)?(BIT5):(0x00))	,BIT5); 		//reg_sub_v_nm_std_en
			msWriteByteMask(SCB_F5, (0x00),BIT3|BIT2); 								//reg_sub_v_nm_std_step
		}		
		#endif
	}
}


#if CHIP_ID==CHIP_TSUMD
void msDeMosquito(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD)
{
/*
	1.�}�@�i���r�����ϡA�ΨӮ����r�������T
	2.��SPF bypass mode�]SC2_70[3]=0)
	3.�]�wreg_spf_rgb2y_en(SC2_70[7]=1)
	4.�}debug mode�]SC4_10[7]=1)
	5.�}deMosquito�]SC4_10[0]=1)
	6.��gain�]�j�]SC4_10[12:8]=0x1F�^
	7.�]�wTHRD1�]SC4_11[7:0]�ȷU�p�U����^
	8.�]�wTHRD2�]SC4_12[7:0]�ȷU�j�U����^
*/

    msWriteByteMask(SC2_E0,	0x00		,BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(SC2_E0,	(IsColorspaceRGB()? 0x00:BIT7)		,BIT7);
	
    msWriteByteMask(SC4_20,	BIT1			,BIT1);  //Alpha LPF enable
    
    msWriteByteMask(SC4_20,	u8Enable	,BIT7|BIT0);  //BIT0:enable ,BIT7�Gdebug mode

    msWriteByteMask(SC4_21,	(u8Strength<=0x1F? u8Strength:0x1F) 	,0x1F);  //BIT12~BIT8
    msWriteByteMask(SC4_22,	abs(0xFF-u8THRD) 		,0xFF);  //DMS threshold 1
    msWriteByteMask(SC4_24,	u8THRD 		,0xFF);  //DMS threshold 2	

    //msWriteByteMask(SC4_23, BIT1|BIT0		,BIT1|BIT0);  //reg_lut_step_hor_f2
    //msWriteByteMask(SC4_25, BIT2|BIT1|BIT0		,BIT2|BIT1|BIT0);  //reg_lut_step_cen_f2
	
}

void msSpikeNR(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD)
{
/*
	�}�@�i�����I���ϡA�ΨӮ����Ϥ��W�����I�G 
	1.��SPF bypass mode�]SC2_70[3]=0)
	2.�]�wreg_spf_rgb2y_en(SC2_70[7]=0)
	3.��SPK_NR motion mode�]SC4_50[3]=0�^
	4.�}SPK_NR�}���]SC4_50[0]=1�^
	5.�]�wSPK_NR�j�ס]SC4_50[11:8]=0x0f)
	6.�]�wSPK_NR THRD1�]SC4_51[12:8]�A�ȷU�j�ĪG�U�j�^
	7.�]�wSPK_NR THRD0�]SC4_51[7:0]�A�ȷU�j�ĪG�U�j�^
	8.�]�wSPK_NR THRD2�]SC4_52[7:0]�A�ȷU�p�ĪG�U�j�^
	9.�]�wSPK_NR PTH1 Step�]SC4_53[6:4]�A�ȷU�j�ĪG�U�j�^
	10.�]�wSPK_NR PTH0 Step�]SC4_53[2:0]�A�ȷU�j�ĪG�U�j�^
	11.�]�wSPK_NR PTH2 Step�]SC4_53[10:8]�A�ȷU�j�ĪG�U�j�^
*/

    msWriteByteMask(SC2_E0,	0x00		,BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(SC2_E0,	(IsColorspaceRGB()? 0x00:BIT7)		,BIT7);
    
    msWriteByteMask(SC4_A0,	u8Enable&BIT0	,BIT3|BIT0);  //BIT0:enable ,BIT3 motion mode

    msWriteByteMask(SC4_A1,	(u8Strength<=0x0F? u8Strength:0x0F) 	,0x0F);  //BIT11~BIT8
    
    msWriteByteMask(SC4_A2,	u8THRD 		,0xFF);  // threshold 0  BIT7~0
    msWriteByteMask(SC4_A3,	u8THRD&0x1F 		,0xFF);  // threshold 1  BIT12~8
    msWriteByteMask(SC4_A4,	abs(0xFF-u8THRD) 		,0xFF);  // threshold 2 BIT7~0	

    //msWriteByteMask(SC4_A6, BIT6|BIT5|BIT4|BIT2|BIT1|BIT0 		,BIT6|BIT5|BIT4|BIT2|BIT1|BIT0);  // STEP11 21 31
    //msWriteByteMask(SC4_A7, BIT2|BIT1|BIT0	,BIT2|BIT1|BIT0);  // STEP22	
}

void msDeBlocking(BYTE u8Enable, /*BYTE u8Strength ,*/BYTE u8THRD)
{
/*
	�e���]�����Y�|���u�����T�A�ΨӮ��u�����T
	1.��SPF bypass mode�]SC2_70[3:0]=0)
	2.�]�wreg_spf_rgb2y_en(SC2_70[7]=0)
	
	2.�}de blocking �}���GSC2_10[0]=1
	3.�}debug mode�]SC2_40[7]=1)
	4.��iir mode�]SC2_40[6]=0)
	5.�}enable (SC2_40[0]=1)
	6.�]�wTHRD�]SC2_40[15:8]=0x00)
	7.�]�wSTEP�]SC2_40[3:2]=0~3)
*/

    msWriteByteMask(SC2_E0,	0x00		,BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(SC2_E0,	(IsColorspaceRGB()? 0x00:BIT7)		,BIT7);
    
    msWriteByteMask(SC2_20,	u8Enable&BIT0	,BIT0);  //BIT0:enable 
    msWriteByteMask(SC2_80,	u8Enable&(BIT7|BIT6|BIT0)	,BIT7|BIT6|BIT0);  //BIT7:debug mode ,BIT6: iir mode BIT0:blockiness_en_f2
    //msWriteByteMask(SC2_80,	BIT3|BIT2	,BIT3|BIT2);  //De-blocking coarse detect step F2
	
    msWriteByteMask(SC2_81,	u8THRD	,0xFF);  //De-blocking coarse active threshold F2

	
}
void msSNR(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD)
{
/*
	�����OAPN�̪�SNR�G�@�˥i�H�F��������T���\��A���k�e���]��dot pattern�ա^
	1.��SPF bypass mode�]SC2_70[3:0]=0)
	2.�]�wreg_spf_rgb2y_en(SC2_70[7]=1)
	
	3.�}SNR�}���GSC2_30[0]=1
	4.��SNR motion mode�GSC2_30[1]=0
	5.�]�wSNR active THRD�GSC2_30[15:8]=0xFF�]�@�ΡH�^
	6.�]�wSNR �j�סGSC2_31[3:0]
	7.�]�wSNR Alpha Step�GSC2_31[7:5]�]�@�ΡH�^
	8.�]�wSNR LUT�GSC2_34/35/36/37���̤j�ȡ]0xFFFF�^
*/
    msWriteByteMask(SC2_E0,	0x00		,BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(SC2_E0,	/*(IsColorspaceRGB()? 0x00:BIT7)*/BIT7		,BIT7);

    msWriteByteMask(SC2_60,	u8Enable&BIT0		,BIT1|BIT0);  //BIT0: enable   BIT1:motion mode
    msWriteByteMask(SC2_62,	(u8Strength<=0x0F? u8Strength:0x0F) 	,0x0F);  //BIT3~BIT0
    msWriteByteMask(SC2_61,	u8THRD	,0xFF);  	//SNR active threshold

    msWrite2ByteMask( SC2_68, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SC2_6A, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SC2_6C, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SC2_6E, 0xFFFF , 0xFFFF );
}

void msPreNoiseMasking(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD)
{
/*
	�Mnoise masking�@�˪��@�ΡA����function�O�bscaling�e�]��e�����k�^
	�Ҧp�}64�Ƕ��A���P�������ɳ]�w����|�ܼҽk
	1.��SPF bypass mode�]SC2_70[3:0]=0)
	2.�]�wreg_spf_rgb2y_en(SC2_70[7]=1)
	
	3.����motion �GSC2_50[1]=0
	4.�}pre-Y noise masking�GSC2_50[0]=1
	5.�]�wY NM Gain�GSC2_54[5:0]
	6.�]�wY NM max THRD�GSC2_55[3:0]
	7.�]�wY NM min THRD�GSC2_55[7:4]
	
*/
    msWriteByteMask(SC2_E0,	0x00		,BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(SC2_E0,	/*(IsColorspaceRGB()? 0x00:BIT7)*/BIT7		,BIT7);

    msWriteByteMask(SC2_A0,	u8Enable&BIT0		,BIT1|BIT0);  //BIT0: enable   BIT1:motion mode
    msWriteByteMask(SC2_A8,	(u8Strength<=0x3F? u8Strength:0x3F) 	,0x3F);  //BIT5~BIT0
    msWriteByteMask(SC2_AA,	u8THRD	,0xFF);  	//max/min THRD
}

void msHcoring(BYTE u8Enable, BYTE u8Strength)
{
/*
	New function in Jnior
	main
	1.SCB_08[1:0]=11b		   Y Band1,2 enable 
	2.SCB_08[6:4]=111b		  Step=MAX
	3.SCB_08[7]=1				   Highpass enable HPF �}���]0�Gcoring�A1�Gsharpness)
	sub
	4.SCB_08[9:8]=11b			Y Band1,2 enable 
	5.SCB_08[14:12]=111b	  Step=MAX
	6.SCB_08[15]=1				   Highpass enable
	7.SCB_09~0C=FFFF		Gain table MAX
*/
    msWriteByteMask(SCB_A0,	(IsColorspaceRGB()? 0x00:BIT4|BIT0)		,BIT4|BIT0);

	//main window
    msWriteByteMask(SCB_10,	u8Enable&(BIT7|BIT1|BIT0)		,BIT7|BIT1|BIT0); //BIT1&BIT0: y_band1&band2_h_coring_en BIT7:HPF mode
    msWriteByteMask(SCB_10,	BIT2		,BIT2); //dither
    msWriteByteMask(SCB_10,	(u8Strength<=0x07? u8Strength:0x07)<<4 	,BIT6|BIT5|BIT4);  //BIT6~BIT4
    msWrite2ByteMask( SCB_12, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SCB_14, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SCB_16, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SCB_18, 0xFFFF , 0xFFFF );
	
	//sub window
    msWriteByteMask(SCB_04,	u8Enable&(BIT7|BIT1|BIT0)		,BIT7|BIT1|BIT0); //reg_main_y_band1&band2_h_coring_en
    msWriteByteMask(SCB_04,	BIT2		,BIT2); //dither
    msWriteByteMask(SCB_04,	(u8Strength<=0x07? u8Strength:0x07)<<4 	,BIT6|BIT5|BIT4);  //BIT6~BIT4
    msWrite2ByteMask( SCB_06, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SCB_08, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SCB_0A, 0xFFFF , 0xFFFF );
    msWrite2ByteMask( SCB_0C, 0xFFFF , 0xFFFF );
    
}

void msShinnyColor(BYTE u8Enable, BYTE u8Strength ,BYTE u8THRD)
{
/*
	SC9_4A[11]�Gmain�}��
	SC9_4A[15:12]�Gmain gain
	SC9_4A[10]�Gmain Rounding
	
	SC9_4B[11]�Gmain�}��
	SC9_4B[15:12]�Gmain gain
	SC9_4B[10]�Gmain Rounding
	
	SC9_4C[14]�G �b�p��L�{��gain�i��|�L�j�o��register�i�H�Ψӥdoverflow�Ϊ�
	SC9_4C[13]�G HBC gain mode�]��gain���Ҧ��^
*/
    msWriteByteMask(SC9_98,	u8THRD		,0xFF);  //hbc gain1 threshold
    msWriteByteMask(SC9_99,	BIT6		,BIT6);  //clamp function for HBC gain
    
	//main window
    msWriteByteMask(SC9_95,	( u8Enable > 0 ) ? ( 1<<3 ) : ( 0 )		,BIT3);  //enable
    msWriteByteMask(SC9_95,	(u8Strength<=0x0F? u8Strength:0x0F)<<4 	,BIT7|BIT6|BIT5|BIT4);  //BIT7~BIT4 gain
    msWriteByteMask(SC9_95,	BIT2		,BIT2);  //Rounding
	
	//sub window
    msWriteByteMask(SC9_97,	( u8Enable > 0 ) ? ( 1<<3 ) : ( 0 )		,BIT3);  //enable
    msWriteByteMask(SC9_97,	(u8Strength<=0x0F? u8Strength:0x0F)<<4 	,BIT7|BIT6|BIT5|BIT4);  //BIT7~BIT4 gain
    msWriteByteMask(SC9_97,	BIT2		,BIT2);  //Rounding
    
}


void Peaking_PostSNR(BYTE u8Enable, BYTE u8THRD)
{
	/*   2d Peaking- Post SNR 
	// Post-SNR is non motion adaptive . The user can use port-snr to reduce noise in spatial (2D) domain.
	1.Main win�G
	   SCB_58[0]�GMain window Gaussian SNR enable
	   SCB_58[1]�GMain window Gaussian SNR adaptive factor enable
	   SCB_58[2]�GMain window Gaussian SNR Mean selection :
						 0: 3x3  1: 5x5 �]�]1 LPF�ĪG����j�^
	   SCB_58[5:4]�GMain window Gaussian SNR LUT step
	   SCB_58[12:8]�GMain window Gaussian SNR threshold
	2.Sub win�G
	   SCB_59[0]�GMain window Gaussian SNR enable
	   SCB_59[1]�GMain window Gaussian SNR adaptive factor enable
	   SCB_59[2]�GMain window Gaussian SNR Mean selection :
						 0: 3x3  1: 5x5 
	   SCB_59[5:4]�GMain window Gaussian SNR LUT step
	   SCB_59[12:8]�GMain window Gaussian SNR threshold
	
	3.LUT table�G�]�Ѥp��j�^
	   SCB_59~5F�GGaussian SNR Table 0~7
	*/

	//main window
    msWriteByteMask(SCB_B0,	BIT2|BIT1	,BIT2|BIT1);  		//[2]: SNR Mean selection  [1]:adaptive factor enable
    msWriteByteMask(SCB_B0,	BIT5|BIT4	,BIT5|BIT4);  		//[2]: SNR LUT step	
    msWriteByteMask(SCB_B0,	( u8Enable > 0 ) ? (BIT0 ) : ( 0 )		,BIT0);  //enable
    msWriteByteMask(SCB_B1,	u8THRD&0x0F	,0x0F);  			//[3:0]:Gaussian SNR threshold
	
	//sub window
    msWriteByteMask(SCB_B2,	BIT2|BIT1	,BIT2|BIT1);  		//[2]: SNR Mean selection  [1]:adaptive factor enable
    msWriteByteMask(SCB_B2,	BIT5|BIT4	,BIT5|BIT4);  		//[2]: SNR LUT step	
    msWriteByteMask(SCB_B2,	( u8Enable > 0 ) ? (BIT0 ) : ( 0 )		,BIT0);  //enable
    msWriteByteMask(SCB_B3,	u8THRD&0x0F	,0x0F);  			//[3:0]:Gaussian SNR threshold

	//LUT table
    msWrite2ByteMask(SCB_B8,	0x0608	,0xFFFF);  		
    msWrite2ByteMask(SCB_BA,	0x0204	,0xFFFF);  		
    msWrite2ByteMask(SCB_BC,	0x0000	,0xFFFF);  		
    msWrite2ByteMask(SCB_BE,	0x0000	,0xFFFF);  		
}
#endif


//<<===========================================================
//>>===========================================================

void msColor_Test_Funct(BYTE tb1, BYTE tb2)
{
    BYTE xdata uncallvalue = tb2;
    switch(tb1)
    {
        case 1:
            msHistrogram_Test(tb2);   // 01 ,01, 0 or 1  0-> main; 1->sub
            break;

        #if Enable_Gamma
        case 2:
            {
                extern void drvGammaLoadTbl( BYTE GamaMode );
                drvGammaLoadTbl(tb2);  //01 02 N
            }
            break;
        case 3:
            msLinearGammaLoader();
            break;
        case 4:
            {
                extern void drvGammaOnOff(BYTE u8Switch, BYTE u8Window);
                drvGammaOnOff(tb2,0);
                drvGammaOnOff(tb2,1);
            }
            break;
        case 5:
            printMsg("msLinearDeGammaLoader");
            msLinearDeGammaLoader();
            break;
    case 6:
        printMsg("msLinearFixGammaLoader");
        msLinearFixGammaLoader();
        break;
#endif
    case 7:
        printMsg("msFCCLoader");
        msFCCLoader();
            msWriteByte(SC7_90, ( tb2 ==0 ) ? ( 0x00 ) : ( 0xFF ) );
            msWriteByte(SC0_00, 0x00 );
        break;

#if ENABLE_3DLUT
#if ENABLE_3DLUT_Test
    case 8:
        {
            extern void mStar_Setup3DLUT( void );
            printMsg("mStar_Setup3DLUT");
            mStar_Setup3DLUT();
        }
        break;
    case 9:
        {
            extern void void ms3DLutLoadTbl(BYTE ColorMode);
             ms3DLutLoadTbl(0);
         }
         break;
     case 0x0A:
         {
             extern void drv3DLutWriteReadTest(void);
              drv3DLutWriteReadTest();
          }
          break;
     case 0x0B:
         {
             extern  void drv3DLutEnable(BYTE ucSrcIdx, BYTE enable) ;
              drv3DLutEnable(0, tb2);
              drv3DLutEnable(1, tb2);
          }
          break;
#endif
#endif




    }
}
#else
BYTE code msColorVerifyNullData[] = {0};
void msColorVerifyDummy()
{
    BYTE xdata x = msColorVerifyNullData[0];
}

#endif
