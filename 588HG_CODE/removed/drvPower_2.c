
///////////////////////////////////////
// DRVPOWER_V VERSION: V02
////////////////////////////////////////
#include "global.h"

void Power_ModCtrl(BYTE ucSwitch)
{
    if (ucSwitch)
    {
	    #if PanelLVDS
		    msWriteByte(REG_36DA, 0x55);
		    msWriteByte(REG_36DB, 0x55);
		    msWriteByte(REG_36DC, 0x05);
	        msWriteByteMask(REG_36F0,0,BIT0);		
		#elif PanelMiniLVDS3
		    msWriteByte(REG_36DA, 0x40);
		    msWriteByte(REG_36DB, 0x65);
		    msWriteByte(REG_36DC, 0x05);
	        msWriteByteMask(REG_36F0,0,BIT0);
		#endif   	
    }
    else
    {
        msWriteByteMask(REG_368A, 0x3F, 0x3F);
        msWriteByteMask(REG_36F0,BIT0,BIT0);     
        msWriteByte(REG_36DA, 0x00);
        msWriteByte(REG_36DB, 0x00);
        msWriteByte(REG_36DC, 0x00);
        msWriteByte(REG_36DD, 0x00);
        msWriteByte(REG_36DD, 0x00);

    }
}

