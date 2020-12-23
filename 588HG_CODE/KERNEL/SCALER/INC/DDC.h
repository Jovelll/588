extern void mStar_WriteDDC1( void );
extern void mStar_WriteDDC2( void );

#ifdef UseInternalDDCRam
extern void mStar_InitDDC( void );
extern void mStar_CheckRAM( void );
extern void mStar_ClrDDC_WP( void );
extern void mStar_SetDDC_WP( void );

#endif

#ifdef Internal_EDID_Write_To_24C02
extern void Mstar_Init_VGA_DVI_HDMI_24C02_EDID(void);
#endif

