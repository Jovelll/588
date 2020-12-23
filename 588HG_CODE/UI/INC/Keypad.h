
extern xdata BYTE KeypadButton;
extern BYTE xdata KeyDebug;

extern void Key_ScanKeypad( void );


#if TimerComboKey
extern bit bDoComboKeyFlag;
extern bit bEnableComboKeyFlag;
extern BYTE XDATA ComboCounter;
extern BYTE XDATA ComboKey;
extern BYTE XDATA PreComboKey;
extern BYTE XDATA ComboKeyType;
#endif

