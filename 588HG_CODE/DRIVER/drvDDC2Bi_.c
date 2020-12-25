
///////////////////////////////////////
// DRVDDC2BI_2BUVY VERSION: V02
////////////////////////////////////////
#include "global.h"

BYTE idata rxInputPort; // Annie 2006.7.5
//BYTE xdata CP=0;
BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
BYTE rxIndex = 0;
BYTE rxStatus;
BYTE rxLength = 0;
BYTE rxChkSum = 0;
BYTE txStatus;
BYTE *txBufferPtr;
BYTE txLength;
BYTE sendData;

code BYTE nullMsg1[3] =
{
    0x6e, 0x80, 0xbe
};
code BYTE nullMsg[3] =
{
    0x51, 0x80, 0xbe
};

void drvDDC2Bi_Init( void )
{
    // mask all ddc interrupt
    D2B_INT_MASK_ALL(); // drvDDC2Bi_Init();

    DDC2Bi_InitRx();
    txStatus = DDC2B_CLEAR;
    
    D2B_SET_ID_VGA(TRUE);//DDCADC_DDCCI_ID = 0xB7;
    D2B_INT_MASK_VGA();//DDCADC_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0;
    CLR_ADC_INT_FLAG();

#if INPUT_TYPE!=INPUT_1A
    D2B_SET_ID_DVI(TRUE);//DDCDVI_DDCCI_ID = 0xB7;
    D2B_INT_MASK_DVI();//DDCDVI_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0;
    CLR_DVI_INT_FLAG();

    #if ENABLE_HDMI
    D2B_SET_ID_HDMI(TRUE);//DDCHDMI_DDCCI_ID = 0xB7;
    D2B_INT_MASK_HDMI();//DDCHDMI_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1 | BIT0;
    CLR_HDMI_INT_FLAG();
    #endif
    #if ENABLE_DP_INPUT
    D2B_SET_ID_DP(TRUE);//DDCDP_DDCCI_ID=0xB7;
    #endif	
#endif

#if 0//ENABLE_3D_FUNCTION && GLASSES_TYPE==GLASSES_NVIDIA
    DDCDVI_NV3D_DDCCI_ID = 0xFD;
    DDCDVI_NV3D_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1;
    CLR_DDC_DVI_NV3D_INT_FLAG();
#endif

    txBufferPtr = &nullMsg1[0];
    txLength = sizeof( nullMsg1 );

    DDCADC_RBUF_WDP = *txBufferPtr++;//DDC2B_DEST_ADDRESS;//0;
    DDC_ADC_WP_READY();

#if (INPUT_TYPE!=INPUT_1A)
    DDCDVI_RBUF_WDP = *txBufferPtr++;//DDC2B_DEST_ADDRESS;//0;
    DDC_DVI_WP_READY();
  #if ENABLE_HDMI
    DDCHDMI_RBUF_WDP = *txBufferPtr++;//DDC2B_DEST_ADDRESS;//0;
    DDC_HDMI_WP_READY();
  #endif
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
#if 	INPUT_TYPE!=INPUT_1A	
        if(CURRENT_INPUT_IS_DVI())//( SrcInputType == Input_Digital )
            DDCDVI_RBUF_WDP = DDC2B_DEST_ADDRESS;
#if ENABLE_HDMI
        else if(CURRENT_INPUT_IS_HDMI())//( SrcInputType == Input_HDMI)
            DDCHDMI_RBUF_WDP = DDC2B_DEST_ADDRESS;
#endif		
        else
#endif			
            ADC_RBUF_WDP = DDC2B_DEST_ADDRESS;
    }
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

#if INPUT_TYPE!=INPUT_1A
    if(INPUT_IS_DVI(rxInputPort))//( rxInputPort == Input_Digital )
        DDCDVI_RBUF_WDP = DDC2B_DEST_ADDRESS;
#if ENABLE_HDMI
    else if(INPUT_IS_HDMI(rxInputPort))//( rxInputPort == Input_HDMI)			
        DDCHDMI_RBUF_WDP = DDC2B_DEST_ADDRESS;
#endif		
    else
#endif			
        ADC_RBUF_WDP = DDC2B_DEST_ADDRESS;

}

void drvDoDDCCI( BYTE Source ) // Annie 2006.7.5
{
    BYTE intFlag, rxByte;
#if INPUT_TYPE!=INPUT_1A
    if(INPUT_IS_DVI(Source))//( Source == Input_Digital )
        intFlag = DDCDVI_INT_FLAG;
#if ENABLE_HDMI
    else if(INPUT_IS_HDMI(Source))//( Source == Input_HDMI )
        intFlag = DDCHDMI_INT_FLAG;
#endif    
    else
#endif		
        intFlag = DDCADC_INT_FLAG;

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
#if INPUT_TYPE!=INPUT_1A					
                    if(INPUT_IS_DVI(Source))//( Source == Input_Digital )
                    {
                        DDCDVI_RBUF_WDP = 0;
                        DDC_DVI_WP_READY();
                    }
#if ENABLE_HDMI
                    else if(INPUT_IS_HDMI(Source))//( Source == Input_HDMI )
                    {
                        DDCHDMI_RBUF_WDP = 0;
                        DDC_HDMI_WP_READY();
                    }
#endif                    
                    else
#endif						
                    {
                        ADC_RBUF_WDP = 0;
                        DDC_ADC_WP_READY();
                    }
                    return ;
                }
                // send out the current byte
#if INPUT_TYPE!=INPUT_1A                
                if(INPUT_IS_DVI(Source))//( Source == Input_Digital )
                    DDCDVI_RBUF_WDP =  *txBufferPtr++;
#if ENABLE_HDMI
                else if(INPUT_IS_HDMI(Source))//( Source == Input_HDMI)
                    DDCHDMI_RBUF_WDP = *txBufferPtr++;
#endif                
                else
#endif					
                    ADC_RBUF_WDP =  *txBufferPtr++;
                txLength--;
                break;
            case RCBI:
                // read the received byte
#if INPUT_TYPE!=INPUT_1A                 
                if(INPUT_IS_DVI(Source))//( Source == Input_Digital )
                    rxByte = DDCDVI_WBUF_RDP;
#if ENABLE_HDMI
                else if(INPUT_IS_HDMI(Source))//( Source == Input_HDMI )
                    rxByte = DDCHDMI_WBUF_RDP;
#endif 
		   else
#endif
                    rxByte = ADC_WBUF_RDP;
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
                            rxInputPort = Source;
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
                ADC_RBUF_WDP = ADC_WBUF_RDP;
	#if INPUT_TYPE!=INPUT_1A 			
                DDCDVI_RBUF_WDP =  DDCDVI_WBUF_RDP;
		#if ENABLE_HDMI
                DDCHDMI_RBUF_WDP = DDCHDMI_WBUF_RDP;
		#endif
	#endif	
                txStatus = DDC2B_CLEAR;
                DDC2Bi_InitRx();
                break;

        }
    }
}
