///////////////////////////////////////
// DRVDDC2BI_9 VERSION: V01
////////////////////////////////////////

#include "global.h"

BYTE idata rxInputPort; // Annie 2006.7.5
BYTE idata rxComboPort; // Combo port index
BYTE rxIndex = 0;
BYTE rxStatus;
BYTE rxLength = 0;
#if !D2B_HW_CheckSum
BYTE rxChkSum = 0;
#endif
BYTE txStatus;
BYTE *txBufferPtr;
BYTE txLength;
//BYTE sendData;

code BYTE nullMsg1[3] =
{
    0x6e, 0x80, 0xbe
};
code BYTE nullMsg[3] =
{
    0x51, 0x80, 0xbe
};

#if D2B_XShared_DDCBuffer
BYTE *DDCBuffer;
#else
BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
#endif

#if D2B_FIFO_Mode
WORD idata txIndex;
WORD idata Previous_DDC2MCU_CUR_ADR;
WORD idata Present_DDC2MCU_CUR_ADR;
WORD idata Present_DDC2MCU_START_ADR;
BYTE idata D2BErrorFlag;
#if D2B_XDATA_DEBUG
BYTE xdata XSRAMBuffer[D2B_FIFO_XdataSize_Total+256] _at_ XDATA_DDC_ADDR_START;
BYTE xdata IntCount;
BYTE xdata IntCountTmp;
#else
BYTE xdata XSRAMBuffer[D2B_FIFO_XdataSize_Total] _at_ XDATA_DDC_ADDR_START;
#endif
#endif

#if D2B_FIFO_Mode
BYTE DDC2Bi_FIFOMODE_READ_CUR(BYTE InputPort)
{
    BYTE CurValue=0;

    if(InputPort==InputCombo_A0) // A0
        CurValue = msRegs[REG_3EE4];
#if DDC_Port_D0    
    else if(InputPort==InputCombo_D0) // D0
        CurValue = msRegs[REG_3EC6];
#endif
#if DDC_Port_D1
    else if(InputPort==InputCombo_D1) // D1
        CurValue = msRegs[REG_3ECC];
#endif
#if DDC_Port_D2    
    else //if(InputPort==InputCombo_D2) // D2
        CurValue = msRegs[REG_3ED2];
#endif
    return CurValue;
}

BYTE DDC2Bi_FIFOMODE_READ_START(BYTE InputComboPort)
{
    BYTE StartValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        StartValue = msRegs[REG_3EE5];
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        StartValue = msRegs[REG_3EC7];
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        StartValue = msRegs[REG_3ECD];
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2)// D2
        StartValue = msRegs[REG_3ED3];
#endif
    return StartValue;
}

WORD DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(BYTE InputComboPort, BYTE u8TXBuffer)
{
    WORD XdataAddrStart=0;
#if !D2B_DOUBLE_FIFO_Mode
        u8TXBuffer = 0;
#endif

    if(InputComboPort==InputCombo_A0) // A0
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_A0:D2B_FIFO_XdataAddressStart_A0;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_D0:D2B_FIFO_XdataAddressStart_D0;
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_D1:D2B_FIFO_XdataAddressStart_D1;
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2) // D2
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_D2:D2B_FIFO_XdataAddressStart_D2;
#endif
    return XdataAddrStart-D2B_FIFO_XdataAddressStart;
}

void DDC2Bi_FIFOMODE_SET_RW_DONE(BYTE InputComboPort)
{
    if(InputComboPort==InputCombo_A0) // A0
        msRegs[REG_3EE6] |= (BIT3);
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        msRegs[REG_3EC8] |= (BIT3);
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        msRegs[REG_3ECE] |= (BIT3);
#endif
#if DDC_Port_D2
    else //if(InputComboPort==InputCombo_D2) // D2
        msRegs[REG_3EE0] |= (BIT3);        
#endif	
}

void DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(BYTE InputComboPort)
{
    if(InputComboPort==InputCombo_A0) // A0
        msRegs[REG_3EE6] |= (BIT5|BIT4);
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        msRegs[REG_3EC8] |= (BIT5|BIT4);
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        msRegs[REG_3ECE] |= (BIT5|BIT4);
#endif
#if DDC_Port_D2
    else //if(InputComboPort==InputCombo_D2) // D2
        msRegs[REG_3EE0] |= (BIT5|BIT4);        
#endif	
}

BYTE DDC2Bi_FIFOMODE_READ_FULL_FLAG(BYTE InputComboPort)
{
    BYTE FlagValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        FlagValue = msRegs[REG_3EE6];
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        FlagValue = msRegs[REG_3EC8];
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        FlagValue = msRegs[REG_3ECE];
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2)// D2
        FlagValue = msRegs[REG_3EE0];
#endif
    return FlagValue&BIT6;
}

#if !D2B_XShared_DDCBuffer
BYTE DDC2Bi_FIFOMODE_READ_EMPTY_FLAG(BYTE InputComboPort)
{
    BYTE FlagValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        FlagValue = msRegs[REG_3EE6];
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        FlagValue = msRegs[REG_3EC8];
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        FlagValue = msRegs[REG_3ECE];
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2)// D2
        FlagValue = msRegs[REG_3EE0];
#endif
    return FlagValue&BIT7;
}
#endif

BYTE DDC2Bi_FIFOMODE_READ_HW_CHKSUM_FLAG(BYTE InputComboPort)
{
    BYTE HWCSValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        HWCSValue = msRegs[REG_3EC2]&BIT0;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        HWCSValue = msRegs[REG_3EC2]&BIT1;
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        HWCSValue = msRegs[REG_3EF0]&BIT0;
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2)// D2
        HWCSValue = msRegs[REG_3EF0]&BIT1;
#endif
    return HWCSValue;
}

void DDC2Bi_CLR_INT(BYTE InputComboPort, BYTE ClrBits)
{    
    if(InputComboPort==InputCombo_A0) // A0
        DDCADC_INT_CLR |= ClrBits;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        DDCDVI0_INT_CLR |= ClrBits;
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        DDCDVI1_INT_CLR |= ClrBits;
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2)// D2
        DDCDVI2_INT_CLR |= ClrBits;
#endif
}
#endif

void DDC2Bi_SET_RBUF(BYTE InputComboPort, BYTE RBufData)
{
    if(InputComboPort==InputCombo_A0) // A0
        ADC_RBUF_WDP = RBufData;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        DDCDVI0_RBUF_WDP = RBufData;
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        DDCDVI1_RBUF_WDP = RBufData;
#endif
#if DDC_Port_D2
    else //if(InputComboPort==InputCombo_D2) // D2
        DDCDVI2_RBUF_WDP = RBufData;        
#endif	
}

void DDC2Bi_SET_WP_READY(BYTE InputComboPort)
{
    if(InputComboPort==InputCombo_A0) // A0
        DDC_ADC_WP_READY();
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        DDC_DVI0_WP_READY();
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        DDC_DVI1_WP_READY();
#endif
#if DDC_Port_D2
    else //if(InputComboPort==InputCombo_D2) // D2
        DDC_DVI2_WP_READY();      
#endif	
}

BYTE DDC2Bi_Read_INT(BYTE InputComboPort)
{
    BYTE IntData=0;
    
    if(InputComboPort==InputCombo_A0) // A0
        IntData = DDCADC_INT_FLAG;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        IntData = DDCDVI0_INT_FLAG;
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        IntData = DDCDVI1_INT_FLAG;
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2)// D2
        IntData = DDCDVI2_INT_FLAG;
#endif
    return IntData;
}

BYTE DDC2Bi_Read_WBUF(BYTE InputComboPort)
{
    BYTE WbufData=0;
    
    if(InputComboPort==InputCombo_A0) // A0
        WbufData = ADC_WBUF_RDP;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        WbufData = DDCDVI0_WBUF_RDP;
#endif
#if DDC_Port_D1    
    else if(InputComboPort==InputCombo_D1) // D1
        WbufData = DDCDVI1_WBUF_RDP;
#endif
#if DDC_Port_D2    
    else //if(InputComboPort==InputCombo_D2)// D2
        WbufData = DDCDVI2_WBUF_RDP;
#endif
    return WbufData;
}

void drvDDC2Bi_Init( void )
{
    // mask all ddc interrupt
    D2B_INT_MASK_ALL(); // drvDDC2Bi_Init();

    DDC2Bi_InitRx();
    txStatus = DDC2B_CLEAR;

    txBufferPtr = &nullMsg1[0];
    txLength = sizeof( nullMsg1 );
    
    D2B_SET_ID_VGA(TRUE);
    D2B_INT_MASK_VGA();
    CLR_ADC_INT_FLAG();
    DDCADC_RBUF_WDP = *txBufferPtr++;
    DDC_ADC_WP_READY();    

#if DDC_Port_D0
    D2B_SET_ID_DVI0(TRUE);
    D2B_INT_MASK_DVI0();
    CLR_DVI0_INT_FLAG();
    DDCDVI0_RBUF_WDP = *txBufferPtr++;
    DDC_DVI0_WP_READY();    
#endif
#if DDC_Port_D1
    D2B_SET_ID_DVI1(TRUE);
    D2B_INT_MASK_DVI1();
    CLR_DVI1_INT_FLAG();
    DDCDVI1_RBUF_WDP = *txBufferPtr++;
    DDC_DVI1_WP_READY();    
#endif
#if DDC_Port_D2
    D2B_SET_ID_DVI2(TRUE);
    D2B_INT_MASK_DVI2();
    CLR_DVI2_INT_FLAG();
    DDCDVI2_RBUF_WDP = *txBufferPtr++;
    DDC_DVI2_WP_READY();    
#endif

#if ENABLE_DP_INPUT
    D2B_SET_ID_DP(TRUE);//DDCDP_DDCCI_ID=0xB7;
#endif	

#if 0//ENABLE_3D_FUNCTION && GLASSES_TYPE==GLASSES_NVIDIA
    DDCDVI_NV3D_DDCCI_ID = 0xFD;
    DDCDVI_NV3D_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1;
    CLR_DDC_DVI_NV3D_INT_FLAG();
#endif

#if D2B_FIFO_Mode
  #if D2B_XShared_DDCBuffer
    DDCBuffer = &XSRAMBuffer[0];
  #endif
    //DDC_Port_A0
    D2B_SET_FIFOMODE_VGA(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_VGA(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_VGA(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_VGA(D2B_FIFO_XdataAddressStart_A0);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_VGA(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_VGA(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_VGA(D2B_FIFO_XdataWriteAddressStart_A0);
#endif
  #if DDC_Port_D0
    D2B_SET_FIFOMODE_D0(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_D0(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_D0(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_D0(D2B_FIFO_XdataAddressStart_D0);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_D0(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_D0(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_D0(D2B_FIFO_XdataWriteAddressStart_D0);
#endif
  #endif
  #if DDC_Port_D1
    D2B_SET_FIFOMODE_D1(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_D1(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_D1(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_D1(D2B_FIFO_XdataAddressStart_D1);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_D1(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_D1(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_D1(D2B_FIFO_XdataWriteAddressStart_D1);
#endif
  #endif
  #if DDC_Port_D2
    D2B_SET_FIFOMODE_D2(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_D2(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_D2(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_D2(D2B_FIFO_XdataAddressStart_D2);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_D2(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_D2(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_D2(D2B_FIFO_XdataWriteAddressStart_D2);
#endif
  #endif  
#endif
#if D2B_HW_CheckSum
    D2B_SET_D2B_HW_CheckSum(_ENABLE, DDC_Port_D2, DDC_Port_D1, DDC_Port_D0);
#endif

    INT_IRQ_D2B_ENABLE(TRUE); //enable d2b int
}

void drvDDC2Bi_MessageReady( void )
{
    DDCBuffer[0] |= DDC2Bi_CONTROL_STATUS_FLAG;
    DDCBuffer[( DDCBuffer[0] &~DDC2Bi_CONTROL_STATUS_FLAG ) + 1] = DDC2Bi_ComputeChecksum(( DDCBuffer[0] &~DDC2Bi_CONTROL_STATUS_FLAG ) + 1 );
    DDC2Bi_GetTxBuffer();
    txBufferPtr = &DDCBuffer[0];
#if DDCCI_DEBUG_ENABLE
    Osd_Show();
    Osd_Draw4Num( 1, 0, DDCBuffer[0] );
    Osd_Draw4Num( 1, 1, DDCBuffer[1] );
    Osd_Draw4Num( 1, 2, DDCBuffer[2] );
    Osd_Draw4Num( 1, 3, DDCBuffer[3] );
    Osd_Draw4Num( 1, 4, DDCBuffer[4] );
    Osd_Draw4Num( 1, 5, DDCBuffer[5] );
    Osd_Draw4Num( 1, 6, DDCBuffer[6] );
    Osd_Draw4Num( 1, 7, DDCBuffer[7] );
    Osd_Draw4Num( 1, 8, DDCBuffer[8] );
    Osd_Draw4Num( 1, 9, DDCBuffer[9] );
    Delay1ms( 1000 );
#endif
    txLength = ( DDCBuffer[0] & ( ~DDC2Bi_CONTROL_STATUS_FLAG ) ) + 2;

 if(!CURRENT_INPUT_IS_DISPLAYPORT())    //20121030
  {
#if D2B_FIFO_Mode
    if( txLength > 0 )
    {
#if !D2B_XShared_DDCBuffer 
        WORD i=0;

#endif
        BYTE WriteXdataST_Add;

#if D2B_FIFO_Enhance
        WriteXdataST_Add = DDC2Bi_FIFOMODE_READ_CUR(rxComboPort) + 1;
#else
        WriteXdataST_Add= 0;
#endif
        
        XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(rxComboPort,1)+(WriteXdataST_Add%D2B_FIFO_Size)] = 0x6E;
#if !D2B_XShared_DDCBuffer                
        for( i = 0; i < txLength; i++ )
        {            
            XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(rxComboPort,1)+(( WriteXdataST_Add+i+1 )%D2B_FIFO_Size)] = DDCBuffer[i];

            if( i==(D2B_FIFO_Size-2) )
            {
                txIndex = D2B_FIFO_Size-1;
                break;
            }
        }
#endif        
        DDC2Bi_SET_WP_READY(rxComboPort);
    }
#endif
  }
}

void drvDDC2Bi_ParseCommand( void )
{
    BYTE length; //, command;
    length = DDCBuffer[LENGTH] & 0x7F;
    if( rxIndex <= 0 )
        return ;
    if( length <= 0 )
    {
        txBufferPtr = &nullMsg1[1];
        txLength = sizeof( nullMsg1 );

        DDC2Bi_SET_RBUF(rxComboPort, DDC2B_DEST_ADDRESS);
    }

#if D2B_XDATA_DEBUG
    {
        BYTE ii;
        for (ii=0;ii</*16*/24;ii++)
            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+24/*32*/+ii] = DDCBuffer[ii];
    }
#endif

#if DDCCI_FILTER_FUNCTION
    if(DDCCICommandExecutable())
#endif        
    length = AlignControl();
    
    if( length <= 0 )
    {
        DDCBuffer[0] = 0x80;
        //return ;
    }
    else if( length == 0xFE || length == 0xFD )
    {
        DDCBuffer[0] = 0x80;
    }
    DDC2Bi_MessageReady();


    DDC2Bi_SET_RBUF(rxComboPort, DDC2B_DEST_ADDRESS);        
}

#if D2B_FIFO_Mode
void drvDoDDCCI( BYTE Source ) // Annie 2006.7.5
{
    BYTE intFlag;
    WORD u8AddPoint;

    intFlag = DDC2Bi_Read_INT(Source);

    if( intFlag )
    {
        if(DDC_RW_SRAM_FLAG) // DDC access xdata fail
        {
            D2BErrorFlag=1;
            DDC2Bi_InitRx();

#if D2B_XDATA_DEBUG
            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x33;
            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x33;
            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EEA];
            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
            IntCount++;
#endif
            DDC_RW_SRAM_CLR;
            DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
            DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source);
            if( _bit7_( DDC2Bi_Read_INT(Source)) )
                DDC2Bi_CLR_INT(Source, BIT7);
            return;            
        }
    
        switch( intFlag &( TXBI | RCBI ) )
        {
            // TX int
            case TXBI:
                //txStatus = DDC2B_MSGON;                // no characters left
                Previous_DDC2MCU_CUR_ADR = Present_DDC2MCU_CUR_ADR;           
                Present_DDC2MCU_CUR_ADR = DDC2Bi_FIFOMODE_READ_CUR(Source); // MUST
#if D2B_XDATA_DEBUG 
                if(IntCount>=32)
                {
                    IntCount=31;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] = IntCount|0xC0;
                }else
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] = IntCount|0x80;
                
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x77;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = txIndex;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];

                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+9] = D2BErrorFlag;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+10] = (msRegs[REG_3EE6]);  
#endif

                if(txIndex) // Remaining Tx
                {
#if !D2B_XShared_DDCBuffer                                    
                    if( (DDC2Bi_FIFOMODE_READ_EMPTY_FLAG(Source)) || (D2BErrorFlag==1) ) // Buffer Read Empty
                    {
                        D2BErrorFlag=1;
                        //u8TxStatus=DDC2B_CLEAR;
#if D2B_XDATA_DEBUG
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x77;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x77;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0x77;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                        IntCount++;                        
#endif
                        DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
                        DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source);                                               
                        return;
                    }

                    Present_DDC2MCU_START_ADR = DDC2Bi_FIFOMODE_READ_START(Source);
               

                    if( _bit7_( DDC2Bi_Read_INT(Source)) )
                    {
                        if( Present_DDC2MCU_CUR_ADR < Present_DDC2MCU_START_ADR )
                        {
                            Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                        }                                               
#if D2B_XDATA_DEBUG
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR|0x80;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR|0x80;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR|0x80;                  
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
#endif        
                        for( u8AddPoint = Present_DDC2MCU_START_ADR; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                        {
                            XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,1)+u8AddPoint%D2B_FIFO_Size] = DDCBuffer[txIndex++] ;
                            if( txIndex >= txLength )
                            {
                                txIndex=0;
                                Present_DDC2MCU_CUR_ADR &= (D2B_FIFO_Size-1);
                                break;
                            }
                        }
                    }
                    else
                    {
                        while( Present_DDC2MCU_CUR_ADR<Previous_DDC2MCU_CUR_ADR )
                        {
                            Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                        }
#if D2B_XDATA_DEBUG                        
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR;  
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
#endif                    
                        for( u8AddPoint = Previous_DDC2MCU_CUR_ADR + 1; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                        {
                            XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,1)+u8AddPoint%D2B_FIFO_Size] = DDCBuffer[txIndex++] ;
                            if( txIndex >= txLength )
                            {
                                txIndex=0;
                                Present_DDC2MCU_CUR_ADR &= (D2B_FIFO_Size-1);
                                break;
                            }
                        }
                    }      

                    if(DDC2Bi_FIFOMODE_READ_EMPTY_FLAG(Source)) // Buffer Read Empty at Race condition
                    {
                        D2BErrorFlag=1;
                        //u8TxStatus=DDC2B_CLEAR;
#if D2B_XDATA_DEBUG
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0x66;
                        XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
#endif
                        DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source); // clear
                    }
#endif // #if !D2B_XShared_DDCBuffer                    
                }
                else
                {
                   DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source); // clear
                }
                
                DDC2Bi_FIFOMODE_SET_RW_DONE(Source); // To prevent Full_Error_flag
                if( _bit7_( DDC2Bi_Read_INT(Source)) )
                    DDC2Bi_CLR_INT(Source, BIT7);
                
                break;

            // RX int    
            case RCBI:
                Previous_DDC2MCU_CUR_ADR = Present_DDC2MCU_CUR_ADR;
                Present_DDC2MCU_CUR_ADR = DDC2Bi_FIFOMODE_READ_CUR(Source);
                Present_DDC2MCU_START_ADR = DDC2Bi_FIFOMODE_READ_START(Source);

#if D2B_XDATA_DEBUG
                if( _bit7_( DDC2Bi_Read_INT(Source)) )//( _bit7_( msRegs[REG_3E10] ) == 1 )
                    IntCount=0;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+11] = D2BErrorFlag;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+12] = (msRegs[REG_3EE6]);                
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] = IntCount;               
#endif

                if(DDC2Bi_FIFOMODE_READ_FULL_FLAG(Source)) // Buffer Write Overflow
                {
                    D2BErrorFlag=1;
                    DDC2Bi_InitRx();

#if D2B_XDATA_DEBUG
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                    IntCount++;                    
#endif
                    DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
                    DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source);
                    if( _bit7_( DDC2Bi_Read_INT(Source)) )
                        DDC2Bi_CLR_INT(Source, BIT7);

                    return;
                }
                else if( (D2BErrorFlag==1) && ( _bit7_( DDC2Bi_Read_INT(Source))==0 ) ) // Not New Command
                {
                    DDC2Bi_InitRx();

#if D2B_XDATA_DEBUG
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x88;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x88;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                    IntCount++;                    
#endif
                    DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
                    
                    return;
                }
                else
                {
                    D2BErrorFlag=0;
                }

#if D2B_XDATA_DEBUG
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+13] = Previous_DDC2MCU_CUR_ADR;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+14] = Present_DDC2MCU_CUR_ADR;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+15] = Present_DDC2MCU_START_ADR;
#endif                     

                if( _bit7_( DDC2Bi_Read_INT(Source)) )
                {
                    rxIndex = 0;
                    rxLength = 0;
#if !D2B_HW_CheckSum                    
                    rxChkSum = INITRxCK;
#endif
#if !D2B_XShared_DDCBuffer
                    if( Present_DDC2MCU_CUR_ADR < Present_DDC2MCU_START_ADR )
                    {
                        Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                    }
#endif

#if D2B_XDATA_DEBUG
                    {
                        BYTE xdata ii;
                        for (ii=0;ii</*16*/24;ii++)
                            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+24/*32*/+ii] = 0;

                        for (ii=0;ii<32;ii++)
                        {
                            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(ii%16)+(ii/16)*80] = 0;
                            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(ii%16)+(ii/16)*80] = 0;
                            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(ii%16)+(ii/16)*80] = 0;     
                            XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(ii%16)+(ii/16)*80] = 0;
                        }
                    }                   
#endif


#if D2B_XDATA_DEBUG
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR|0x80;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR|0x80;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR|0x80;     
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];                          
#endif
#if D2B_XShared_DDCBuffer              
                    DDCBuffer = &XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+1];
#endif
                    for( u8AddPoint = Present_DDC2MCU_START_ADR + 1; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                    {
#if !D2B_XShared_DDCBuffer                    
                        DDCBuffer[rxIndex++] = XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
#else
                        rxIndex++;
#endif
#if !D2B_HW_CheckSum                              
                        rxChkSum ^= XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
#endif
                    }
                }
                else
                {
#if D2B_XDATA_DEBUG
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR; 
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
#endif
#if !D2B_XShared_DDCBuffer
                    while( Present_DDC2MCU_CUR_ADR<=Previous_DDC2MCU_CUR_ADR )
                    {
                        Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                    }
#endif                    
                
                    for( u8AddPoint = Previous_DDC2MCU_CUR_ADR + 1; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                    {
#if !D2B_XShared_DDCBuffer                                        
                        DDCBuffer[rxIndex++] = XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
#else
                        rxIndex++;
#endif
#if !D2B_HW_CheckSum                          
                        rxChkSum ^= XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
#endif
                    }
                }

                // CheckSum
                if(rxIndex>0 && rxLength==0)
                    rxLength = DDCBuffer[0] & ~DDC2Bi_CONTROL_STATUS_FLAG;

#if D2B_HW_CheckSum
                if( ( (rxLength+ 2) == rxIndex ) && ( DDC2Bi_FIFOMODE_READ_HW_CHKSUM_FLAG(Source)==0 ) )
#else
                if(( rxChkSum == 0x00 && rxIndex>0 ) )
#endif
                {
                    Set_RxBusyFlag();
                    rxStatus = DDC2B_COMPLETED;
#if !D2B_XShared_DDCBuffer
                    Present_DDC2MCU_CUR_ADR &= (D2B_FIFO_Size-1);
#endif
                    rxInputPort = D2B_FIFOMODE_DDCPORT2INPUT(Source);
                    rxComboPort = Source;
                }
                else
                {
                    rxStatus = DDC2B_CLEAR;
                    Clr_RxBusyFlag();
#if D2B_XDATA_DEBUG              
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] |= 0x40;
#endif                    
                }


                if(DDC2Bi_FIFOMODE_READ_FULL_FLAG(Source)) // Buffer Write Overflow at Race condition
                {
                    D2BErrorFlag=1;
                    DDC2Bi_InitRx();

#if D2B_XDATA_DEBUG
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0x55;
                    XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
#endif

                    DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source); // clear
                }
                
                DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
                if( _bit7_( DDC2Bi_Read_INT(Source)) )
                    DDC2Bi_CLR_INT(Source, BIT7);

                break;
                
            // Default
            default:
#if D2B_XDATA_DEBUG
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+3] = 0x44;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x44;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x44;
                XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = intFlag; 
#endif                
                DDC2Bi_SET_RBUF(Source, DDC2Bi_Read_WBUF(Source));
                txStatus = DDC2B_CLEAR;
                DDC2Bi_InitRx();
                break;
        }
    }

#if D2B_XDATA_DEBUG    
    IntCount++;
    if(rxStatus == DDC2B_COMPLETED)
        IntCountTmp = IntCount;
#endif
}
#else
void drvDoDDCCI( BYTE Source ) // Annie 2006.7.5
{
    BYTE xdata intFlag, rxByte;

    intFlag = DDC2Bi_Read_INT(Source);

    if( intFlag )
    {
        switch( intFlag &( TXBI | RCBI ) )
        {
            case TXBI:                //
                txStatus = DDC2B_MSGON;                // no characters left
                if( !( txLength ) )
                {
                    // clear the transmit status
                    txStatus = DDC2B_CLEAR;

                    DDC2Bi_SET_RBUF(Source, 0);
                    DDC2Bi_SET_WP_READY(Source);
                    
                    return ;
                }
                // send out the current byte
                DDC2Bi_SET_RBUF(Source, *txBufferPtr++);
                txLength--;
                break;
                
            case RCBI:
                // read the received byte
                rxByte = DDC2Bi_Read_WBUF(Source);

                // depending of the message status
                switch( rxStatus )
                {
                        // in case there is nothing received yet
                    case DDC2B_CLEAR:
                        if( rxByte == DDC2B_SRC_ADDRESS )
                        {
                            rxStatus++; // = DDC2B_SRCADDRESS;
                            rxChkSum = INITRxCK;
                        }
                        else
                        {
                            DDC2Bi_InitRx();
                        }
                        break;
                        //
                    case DDC2B_SRCADDRESS:
                        // get the length
                        rxLength = rxByte &~DDC2Bi_CONTROL_STATUS_FLAG;
                        // put the received byte in DDCBuffer
                        DDCBuffer[rxIndex++] = rxByte;
                        rxChkSum ^= rxByte;
                        // set the receive body state
                        rxStatus++; // = DDC2B_COMMAND;
                        //if it is a NULL message
                        if (rxLength == 0)
                        {
                            rxStatus= DDC2B_COMPLETED;                            
                            Set_RxBusyFlag();
                        }
                        else if (rxLength >= DDC_BUFFER_LENGTH)
                        {
                            DDC2Bi_InitRx();
                        }
                        break;
                        // get the command
                    case DDC2B_COMMAND:
                        // save the commandbyte
                        rxStatus++; // = DDC2B_RECBODY;
                        // get the message body
                    case DDC2B_RECBODY:
                        DDCBuffer[rxIndex++] = rxByte;
                        rxChkSum ^= rxByte;
                        rxLength--;
                        // the last byte in the message body
                        if( rxLength == 0 )
                        {
                            rxStatus++; // = DDC2B_WAITFORCK;
                        }
                        break;
                        // ...here we are waiting for CheckSum...
                    case DDC2B_WAITFORCK:
                        // if CheckSum match
                        if( rxChkSum == rxByte )
                        {
                            rxInputPort = D2B_FIFOMODE_DDCPORT2INPUT(Source);
                            rxComboPort = Source;
                            rxStatus = DDC2B_COMPLETED;
                            Set_RxBusyFlag();
                        }
                        // elsechecksum error
                        else
                        {
                            // if CheckSum error re-initialize the receive buffer
                            DDC2Bi_InitRx();
                        }
                        break;
                    default:
                        // clear the rxState and the current buffer for a new message
                        if( !RxBusyFlag )
                            DDC2Bi_InitRx();
                        break;
                }
                break;

            default:
                DDC2Bi_SET_RBUF(Source, DDC2Bi_Read_WBUF(Source));
                txStatus = DDC2B_CLEAR;
                DDC2Bi_InitRx();
                break;

        }
    }
}
#endif

void msDrvWriteInternalEDID(BYTE u8Block, BYTE u8Addr, BYTE u8Data)
{
    msWriteByteMask( REG_3EEB, u8Block, 0x1F);    	// Select sram base address for cpu read/write
    msWriteByte( REG_3E4B, u8Addr );    			// DDC address port for CPU read/write
    msWriteByte( REG_3E4C, u8Data);  				// DDC Data Port for cpu write
    msWriteByteMask( REG_3E43, BIT5, BIT5);    		// ADC sram write data pulse gen when cpu write
    while((msReadByte(REG_3E7B) & BIT5));
}

BYTE msDrvReadInternalEDID(BYTE u8Block, BYTE u8Addr)
{
    BYTE ucValue = 0;

	msWriteByteMask( REG_3EEB, u8Block, 0x1F);    // Select sram base address for cpu read/write
    msWriteByte( REG_3E4B, u8Addr );    			// DDC address port for CPU read/write
    msWriteByteMask( REG_3E43, BIT4, BIT4);    	// ADC sram read data pulse gen when cpu read
    while((msReadByte(REG_3E7B) & BIT4));

    ucValue = msReadByte( REG_3E0E );  			// DDC Data Port for cpu read
    return ucValue;
}

