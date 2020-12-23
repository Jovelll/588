
#include "global.h"

#if ENABLE_TOUCH_PANEL_DATA_FROM_USB

#define DEBUG_USB  0

#if DEBUG_USB && ENABLE_DEBUG
#define USB_DPUTSTR(str)    	printMsg(str)
#define USB_DPRINTF(str, x) 	printData(str, x)
#else
#define USB_DPUTSTR(str)
#define USB_DPRINTF(str, x)
#endif

#define USB_SPI_DELAY() {_nop_(); _nop_(); _nop_(); _nop_();} // 4 nop-> clock around 960KHz

Bool USB_SPI_WriteByte(BYTE val)
{
    BYTE i;

    for(i=0; i<8; i++)
    {
        Clr_SPI_SCK();
        if(val&BIT7)
            Set_SPI_MOSI();
        else
            Clr_SPI_MOSI();
        val <<= 1;
        USB_SPI_DELAY();
        Set_SPI_SCK();
        USB_SPI_DELAY();
    }
    
    Set_SPI_MOSI();

    return TRUE;
}

Bool USB_SPI_ReadByte(BYTE *val)
{
    BYTE i;
    BYTE value = 0;

    for(i=0; i<8; i++)
    {
        value <<= 1;
        Clr_SPI_SCK();
        USB_SPI_DELAY();
        if(SPI_MISO_Pin)
            value |= BIT0;
        Set_SPI_SCK();
        USB_SPI_DELAY();
    }

    *val = value;

    return TRUE;
}

Bool USB_SPI_WriteRegister(BYTE addr, BYTE val)
{
    Clr_SPI_SEL();
    USB_SPI_WriteByte(addr);
    USB_SPI_WriteByte(val);
    Set_SPI_SEL();
    
    return TRUE;
}

Bool USB_SPI_ReadRegister(BYTE addr, BYTE *val, BYTE count)
{
    BYTE i;
    
    Clr_SPI_SEL();
    USB_SPI_WriteByte(addr);
    for(i=0; i<count; i++)
        USB_SPI_ReadByte(&val[i]);
    Set_SPI_SEL();
    
    return TRUE;
}

/*
void USB_ReadData(void)
{
    BYTE xdata tmpbuf[16];

    SPI_ReadRegister(0xC0, tmpbuf, 0x10);
    
}
*/

#endif
