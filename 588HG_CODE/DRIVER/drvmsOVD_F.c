
#include "global.h"

#define _MSOVD_F_C_


#if DEBUG_PRINT_ENABLE
    #define OD_DPUTSTR(str)    printMsg(str)
    #define OD_DPRINTF(str, x) printData(str, x)
#else
    #define OD_DPUTSTR(str)
    #define OD_DPRINTF(str, x)
#endif

#if ENABLE_RTE
#if TunePhaseByHistrogram
BYTE xdata u8Motion=0x0A,u8Motion_V=0x0A;
#if DebugPhaseToggle
DWORD xdata g_u32EvenSim_H,g_u32OddSim_H,g_u32EvenSim_V,g_u32OddSim_V;
#endif
#endif
#endif
//#define OD_LIB_VER        0x03
//code ==================================================================

//*************************************************************************************************
//Function:  msODMemoryTest()
//Description: For Memory test
//
//           ex:    printData(" msODMemoryTest = %d"  , msODMemoryTest());
//           if the result is 0,
//           if the result is 1,
//
//*************************************************************************************************

//*************************************************************************************************
//Function:  msOD_Reset()
//Description: Reset MIU bank
//caller:        Power_TurnOnPanel of power.c
//
//           It need to reset MIU bank before turn on panel.
//*************************************************************************************************


//*************************************************************************************************
//Function:  msOD_OnOff()
//Description: Enable/Disable OverDrive and setup OD strength
//          ex:
//          msOD_OnOff( _OD_Disable, 0x50 ); //disable OD
//          msOD_OnOff( _OD_Enable, 0x5F ); //enable OD and set strength as 0x5F
//          OD strength: 0x50 (weak)  <------------>  0x5F (strong)
//
//*************************************************************************************************
//*************************************************************************************************
//Function:  msLoadODT()
//Description: OD look-up table loader.
//
//           ex: msLoadODT(tOverDrive)
//
//           It provides two kinds of format (normal and compressed) for OD look-up table.
//           The look-up table and OD lib should be located in the same bank!!
//
//*************************************************************************************************
#if ENABLE_RTE

void msDrvOverDriveAdjustWeighting(BYTE u8Weighting)
{
    #if CHIP_ID==CHIP_TSUMF
        msWriteByteMask(REG_3122, u8Weighting,0xFF);
    #else 
        msWriteByteMask(REG_3183, ((u8Weighting&0x80)>>7)<<7,BIT7);
        msWriteByteMask(REG_314B, ((u8Weighting&0x40)>>6)<<4,BIT4);
        msWriteByteMask(REG_3123, ((u8Weighting&0x3C)>>2)<<0,0x0F);
        msWriteByteMask(REG_3187, ((u8Weighting&0x03)>>0)<<6,0xC0);
    #endif
}

void drvmsLoadODTable(BYTE *pODTbl)
{
    WORD wCount;
    BYTE ucTARGET;

    if(0 == pODTbl) return;
    OD_DPUTSTR("Load_OD_Table");

    //msWriteBit(REG_3120,0, _BIT0); // Disable OD before load code
    drvmsOverDriveOnOff(FALSE);  // Disable OD before load code
    
    msWriteByte(REG_3102, 0x0E); // load code start
    msWriteByte(REG_311C, 0x0E); //od 2nd table sram enable, r,g,b channel
    msWriteByte(REG_311D, 0x00);

    // Uncompressed mode
    ucTARGET=*(pODTbl+9);// 10th
    for(wCount=0; wCount<289; wCount++)
    {
        if(wCount<272)
            msWriteByte(REG_3106, (wCount == 9)?ucTARGET:(ucTARGET ^ *(pODTbl+wCount)));//Od table sram1 write data
        else
            msWriteByte(REG_3106, 0x00);
        msWrite2Byte(REG_3104, wCount|0x8000);//Od table sram1 address
        while(_bit7_(msReadByte(REG_3105)));//Od table sram1 write enable (toggle type). , 0x3105[0]+ 0x3104[7:0] ==address [8:0]
    }
    ucTARGET=*(pODTbl+272+19);// 20th
    for(wCount=0; wCount<272; wCount++)
    {
        msWriteByte(REG_310C, (wCount == 19)?ucTARGET:(ucTARGET ^ *(pODTbl+272+wCount)));
        msWrite2Byte(REG_310A, wCount|0x8000);
        while(_bit7_(msReadByte(REG_310B)));
    }
    ucTARGET=*(pODTbl+272*2+29);// 30th
    for(wCount=0; wCount<272; wCount++)
    {
        if(wCount<256)
            msWriteByte(REG_3112, (wCount == 29)?ucTARGET:(ucTARGET ^ *(pODTbl+272*2+wCount)));
        else
            msWriteByte(REG_3112, 0x00);
        msWrite2Byte(REG_3110, wCount|0x8000);
        while(_bit7_(msReadByte(REG_3111)));
    }
    ucTARGET=*(pODTbl+272*2+256+39);// 40th
    for(wCount=0; wCount<256; wCount++)
    {
        msWriteByte(REG_3118, (wCount == 39)?ucTARGET:(ucTARGET ^ *(pODTbl+272*2+256+wCount)));
        msWrite2Byte(REG_3116, wCount|0x8000);
        while(_bit7_(msReadByte(REG_3117)));
    }	
    msWriteByte(REG_3102, 0x00);
}

void drvmsOverDriveOnOff(BOOL bEnable)
{
    #if TunePhaseByHistrogram
    msWriteByteMask ( SC0_CE, (bEnable?BIT3:0), BIT3 ); // Vsync int enable
    #endif

    if(bEnable)
	msDrvOverDriveAdjustWeighting(UserprefOverDriveWeighting);
    else
	msDrvOverDriveAdjustWeighting(0x00);
}


void msDrvOverDriveMode(BYTE mode, BYTE h_scale, BYTE v_scale)
{
    DWORD u32OutputRes;
    WORD u16PGAddrLimit, u16PnlHDE;
    BYTE u8ODBitPerPix = 12;

    u16PnlHDE = 1920ul;  /*PanelWidth*/
    u32OutputRes = (DWORD)u16PnlHDE * 1200ul; /*PanelHeight*/

    drvmsOverDriveOnOff(FALSE);

	//weighting and threshold
    //msWrite2Byte(REG_3122, 0x003F);//weighting ,use default value
    msDrvOverDriveAdjustWeighting(UserprefOverDriveWeighting);
    msWriteByte(REG_3124, 0x10);//od active threshold

	//reg_od_base_adr & reg_od_adr_limit
    msWrite3Byte(REG_312A, (0x000000));// od request base address
    //msWrite3Byte(REG_312E, (MIU_OD_ADDR_END>>2));  // reg_od_adr_limit

	//reg_pg_protect_en ,reg_pg_en ,reg_pg_limit_en
    u16PGAddrLimit = (WORD)(((float)u32OutputRes * u8ODBitPerPix /(h_scale ? 2:1)/(v_scale ? 2:1) +18000)/32/32/16)+1;
    //u16PGAddrLimit = (WORD)(((float)u32OutputRes * u8ODBitPerPix /(2)/(2) +18000)/32/32/16)+1;
	msWrite2ByteMask(REG_314E, u16PGAddrLimit, 0x1FF);
    msWriteByteMask(REG_314F,	BIT2|BIT1,  BIT4|BIT2|BIT1); //[2]:reg_pg_limit_en [1]:reg_pg_en
    msWriteByteMask(REG_3152,	(msReadByte(REG_314F)&BIT2 ? 0:BIT7),  BIT7);

	//reg_pg_length
    msWrite2ByteMask(REG_3153, (msReadByte(REG_314F)&BIT2 ? 0x07:0x0F) ,0x7F);

	//reg_vde_h_size
    msWrite2ByteMask(REG_315E, (PANEL_WIDTH / (h_scale ? 2:1)), 0x0FFF);//tmp decode h size, it's important
    msWriteByteMask(REG_31D8, h_scale, BIT0|BIT1);//[0]:od hscaling filter fcuntion enable [1]:od hscaling filter bypass

	//reg_od_strength_slop
    msWriteByte(REG_3175, 0x2E);//[5:0]od strength gradually slop
    msWriteByte(REG_3157, 0x10);//Encode force h-blanking cycle   //?????
    msWriteByteMask(REG_316A, _BIT3, _BIT3);  //??????

	//reg_od_wadr_max_limit,reg_od_radr_max_limit
	u32OutputRes=((DWORD)PANEL_HEIGHT*PANEL_WIDTH)/(v_scale ? 2:1)/(h_scale ? 2:1)+1;
    msWrite3Byte(REG_3172, u32OutputRes);
    msWrite3Byte(REG_3176, u32OutputRes);

	//od bypass mode h resolution
    msWrite2ByteMask(REG_31DE, PANEL_WIDTH, 0xFFF); //PANEL WIDTH[11:0]

	//OD Vscaling enable
    msWriteByteMask(REG_3602, v_scale<<2, BIT2);   //V scaling enable
    msWrite2ByteMask(REG_3604, PANEL_HEIGHT, 0xFFF); //PANEL WIDTH[11:0]

	//OD still image protection

    msWriteByteMask(REG_31DA,	BIT2|BIT0,  BIT2|BIT1|BIT0);  //[0]:reg_sti_pro_en [2:1]:reg_sti_prot_ext_tap
    msWriteByteMask(REG_31DB,	0x10,  0xFF);  //reg_tdiff_gain
    msWriteByteMask(REG_31DC,	0x00,  0xFF);  //reg_tdiff_th

    //OD mode
    msWriteByte(REG_3120, (mode<<1) |_BIT0);

    //reg_decode_error_enable
    //msWriteByteMask(REG_315F, 0x00 ,BIT7);

    drvmsOverDriveOnOff(UserprefOverDriveSwitch);

    //OD_DPRINTF("===============PANEL_HEIGHT = %x  \r\n", PANEL_HEIGHT);
    //OD_DPRINTF("OD_h_scale = %x", h_scale);
}

void drvmsOverDriverInit(void)
{
    DWORD OutputRes;

    OutputRes = (DWORD)PANEL_WIDTH * PANEL_HEIGHT;

	//REG_3151[4]: previous data , for debugging

    if(PANEL_WIDTH % 2)//od compress mode encode preline force last data write to protect odd pix line
        msWriteByteMask(REG_314B, BIT3, BIT3);
    else
        msWriteByteMask(REG_314B, 0, BIT3);

    msWriteByte(REG_31AB, 0x00);  //??????
	msDrvOverDriveMode(OD_MODE, OD_HSCAL_EN, OD_VSCAL_EN);

}
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///// Function Declaration //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msOD_Histogram_Interrupt( void )
{
	#if TunePhaseByHistrogram


	DWORD xdata u32evenSim_0=0,u32evenSim_1=0,u32oddSim_0=0,u32oddSim_1=0;
	DWORD xdata u32evenSim_ver_0=0,u32evenSim_ver_1=0,u32oddSim_ver_0=0,u32oddSim_ver_1=0;
	BYTE idata ucTmp;
    BYTE xdata ucBank;
    ucBank = scReadByte ( SC0_00 );
    scWriteByte(SC0_00, 0x00);
    scWriteByte ( SC0_CE, scReadByte ( SC0_CE ) & ( ~BIT3 ) ); //scRegs[0xCE] = scRegs[0xCE]&(~BIT3);
    scWriteByte ( SC0_CC, scReadByte ( SC0_CC ) & ( ~BIT3 ) ); //scRegs[0xCC] = scRegs[0xCC]&(~BIT3);

    for ( ucTmp = 0; ucTmp < 4; ucTmp++ )
    {
        u32evenSim_0 += ((DWORD)msReadByte(reg_phase_cur0_pre0_histogram_read_back+ucTmp)<<(8*ucTmp));//20
        u32evenSim_1 += ((DWORD)msReadByte(reg_phase_cur1_pre1_histogram_read_back+ucTmp)<<(8*ucTmp));//26
        u32oddSim_0 += ((DWORD)msReadByte (reg_phase_cur0_pre1_histogram_read_back+ucTmp)<<(8*ucTmp));//22
        u32oddSim_1 += ((DWORD)msReadByte (reg_phase_cur1_pre0_histogram_read_back+ucTmp)<<(8*ucTmp));//24
        u32evenSim_ver_0 += ((DWORD)msReadByte (reg_phase_cur0_pre0_histogram_read_back_v+ucTmp)<<(8*ucTmp));//28
        u32evenSim_ver_1 += ((DWORD)msReadByte (reg_phase_cur1_pre1_histogram_read_back_v+ucTmp)<<(8*ucTmp));//2E
        u32oddSim_ver_0 += ((DWORD)msReadByte (reg_phase_cur0_pre1_histogram_read_back_v+ucTmp)<<(8*ucTmp)); //2A
        u32oddSim_ver_1 += ((DWORD)msReadByte (reg_phase_cur1_pre0_histogram_read_back_v+ucTmp)<<(8*ucTmp)); //2C
    }
	#if DebugPhaseToggle
	g_u32EvenSim_H=u32evenSim_0+ u32evenSim_1;
	g_u32OddSim_H=u32oddSim_0+ u32oddSim_1;
	g_u32EvenSim_V=u32evenSim_ver_0+ u32evenSim_ver_1;
	g_u32OddSim_V=u32oddSim_ver_0+ u32oddSim_ver_1;
	#endif
	//Sint S_Motion,S_Motion_V;
	if ((u32evenSim_0+u32evenSim_1) <= (u32oddSim_0+u32oddSim_1)>>2)
		u8Motion=0;
	else if((u32oddSim_0+u32oddSim_1) <= (u32evenSim_0+u32evenSim_1)>>2)
		u8Motion=1;
	else //not even/odd motion
		u8Motion=2;

	if ((u32evenSim_ver_0+u32evenSim_ver_1) <= (u32oddSim_ver_0+u32oddSim_ver_1))
		u8Motion_V=0;
	else if ((u32oddSim_ver_0+u32oddSim_ver_1) <= (u32evenSim_ver_0+u32evenSim_ver_1))
		u8Motion_V=1;
	else  //not even/odd motion
		u8Motion_V=2;

	//
	msWriteByte(REG_3602, ((u8Motion==0x01)?(msReadByte(REG_3602)&(~BIT3)):(msReadByte(REG_3602)|BIT3)));
	msWriteByte(REG_3602, ((u8Motion_V==0x01)?(msReadByte(REG_3602)&(~BIT4)):(msReadByte(REG_3602)|BIT4)));

    scWriteByte(SC0_00, 0x00);
    scWriteByte ( SC0_CE, scReadByte ( SC0_CE ) | BIT3); //scRegs[0xCE] = scRegs[0xCE]|BIT3;
    scWriteByte ( SC0_00, ucBank );
	#endif
}
#endif
