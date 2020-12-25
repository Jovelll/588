
//ccflag vs add
//=================================================================
BYTE code strFactoryAutoColor[] =
{
    "AUTO COLOR"
};
BYTE code strFactoryPass[7]=
{//123456789012345678901234567890
  "PASS"
};

BYTE code strFactoryFail[7]=
{//123456789012345678901234567890
  "FAIL"
};
BYTE code strFactoryBrightness[] =
{
    "BRIGHTNESS" 
};
BYTE code strFactoryContrast[] =
{
    "CONTRAST" 
};
BYTE code strFactoryAdcGain[] =
{
    "GAIN"
};
BYTE code strFactoryAdcOffset[] =
{
    "OFFS"
};
#if FactoryAuto_5400K
BYTE code strFactory5400K[] =
{
    "5400K"
};
#endif
#if FactoryAuto_6500K
BYTE code strFactory6500k[] =
{
    "6500K"
};
#endif
#if FactoryAuto_9300K
BYTE code strFactory9300k[] =
{
    "9300K"
};
#endif
BYTE code strBurnin[] =
{
    "BURNIN"
};
			
BYTE code strExitText[] =
{
    "BEENDEN"
};
BYTE code strSelectText[] =
{
    "AUSW\xAAHLEN"
};

BYTE code strExit1Text[] =
{
    "EXIT"
};
BYTE code strSelect1Text[] =
{
    "SELECT"
};

#if ENABLE_FACTORY_SSCADJ	//120305 Modify
BYTE code strSpreadSpectrum[] =  //
{
    "SPREAD SPECTRUM"
};

BYTE code strFactoryResetText[] =
{
    "RESET"
};

BYTE code strFreq[] =  //
{
    "FREQ"
};
BYTE code strAmp[] =  //
{
    "AMP"
};
#endif
#if DisplayLogo!=NoBrand	//130613 Jun
BYTE code strLogo[] =
{
    "LOGO"
};
#endif
#if DDCCI_ENABLE
BYTE code strDDCCI[] =
{
    "DDCCI"
};
#endif
#if !USEFLASH // 130829 larry
BYTE code strEraseEEPROM[] =
{
    "ERASE EEPROM"
};
#endif
BYTE code strFactoryExitText[]=
{
    "EXIT"
};
BYTE code strOnOff[2][4]=
{
    //  O    f   f
    {"OFF"},
    //  O    n
    {"ON "}
};

BYTE code strFactoryRed[] =
{
    "R"
};
BYTE code strFactoryGreen[] =
{
    "G"
};
BYTE code strFactoryBlue[] =
{
    "B"
};
//-------------------
BYTE code strVersionDate[] =
{
   BDName// {"O58FDT9"}
};



BYTE code strChangeDate[] =
{
    //ChangeDate
    
	  '2',
	  '0',
    ('0' + (CODE_BUILD_TIME_YEAR/10)),
	('0' + (CODE_BUILD_TIME_YEAR%10)),
	('0' + (CODE_BUILD_TIME_MONTH/10)),
	('0' + (CODE_BUILD_TIME_MONTH%10)),
	('0' + (CODE_BUILD_TIME_DATE/10)),
	('0' + (CODE_BUILD_TIME_DATE%10)),
	('0' + (CODE_BUILD_TIME_HOUR/10)),
	('0' + (CODE_BUILD_TIME_HOUR%10)),
	('0' + (CODE_BUILD_TIME_MINUTE/10)),
	('0' + (CODE_BUILD_TIME_MINUTE%10)),
	('0' + (CODE_BUILD_TIME_SECOND/10)),
	('0' + (CODE_BUILD_TIME_SECOND%10)),
	" "
};


BYTE code strVerText[] =
{
    "V00.1"
};


BYTE code strPanel[] =  //
{
    PanelName
};

BYTE code strPanelOn[] =  //
{
    "TIME"
};

BYTE code strTime_HText[] =
{
    "H"
};

BYTE code strTime_MText[] =
{
    "M"
};

BYTE code strTime_SText[] =
{
    "S"
};

BYTE code strChecksumText[] =
{
    "CHECKSUM "
};

#if ENABLE_BURNIN_MENU

BYTE code strEDID_SText[] =  //
{
    EDIDName
};


BYTE code strPanelWH_SText[] =  //
{
    PanelWH
};	
	

#endif

//=================================================================

BYTE code strMainMenu_BrightM7[LANG_Nums][15]=
{
	#if Language_English
	{"BRIGHTNESS"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"LUMINOSIT\x8B"},			// 3 FRENCE
	#endif
	#if Language_German
       {"BILD"},
	#endif
	#if Language_SimChinese
	{"\x60 \x63 \x61 \x62 \x63"},// 4 简体
	#endif
	#if Language_Italian
       {"LUMINOSIT\x98"},
	#endif
	#if Language_Spanish
       {"BRILLO"},
	#endif
	#if Language_Portuguese
       {"BRILHO"},
	#endif
	#if Language_Turkish
       {"PARLAKLIK"},
	#endif
	#if Language_Polish
       {"BRIGHTNESS"},
	#endif
	#if Language_Dutch
	{"HELDERHEID"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x80\x97\x91\x95\x98\x99\xA1"},			
	#else
	{"\x80\x71\x6B\x6F\x72\x73\x7D"},			
	#endif
	#endif
	#if Language_Korean
	{"\x60 \x61"},			 
	#endif
	
	#if Language_TraChinese
	,{"\x60 \x63 \x61 \x62 \x63"},			// 5 繁体
	#endif
};
BYTE code strMainMenu_ImageM7[LANG_Nums][15]=
{
	#if Language_English
	{"IMAGE"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"DES IMAGE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"BILDFORMAT"},
	#endif
	#if Language_SimChinese
	{"\x7F \x80 \x97 \x98"},			// 4 简体
	#endif
	#if Language_Italian
       {"IMMAGINE"},
	#endif
	#if Language_Spanish
       {"IMAGEN"},
	#endif
	#if Language_Portuguese
       {"IMAGEM"},
	#endif
	#if Language_Turkish
       {"GORUNTU"},
	#endif
	#if Language_Polish
       {"IMAGE"},
	#endif
	#if Language_Dutch
	{"BEELD"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x69\x8E\x95\x87\x97\x86\x8D\x8B\x94\x8F\x8B"},	
	#else
	{"\x69\x68\x6F\x61\x71\x60\x67\x65\x6E\x69\x65"},	
	#endif
	#endif
	#if Language_Korean
	{"\x6C \x75"}	,		 
	#endif
	
	#if Language_TraChinese
	,{"\x7F \x80 \x97 \x98"}			// 5 繁体
	#endif
};
BYTE code strMainMenu_ColorTempM7[LANG_Nums][15]=
{
	#if Language_English
	{"COLOR TEMP."}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"TEMP. COULEUR"},			// 3 FRENCE
	#endif
	#if Language_German
       {"FARBTEMPERATUR"},
	#endif
	#if Language_SimChinese
	{"\x85 \x86"},			// 4 简体
	#endif
	#if Language_Italian
       {"TEMP.COLORE"},
	#endif
	#if Language_Spanish
       {"TEMP.COLOR"},
	#endif
	#if Language_Portuguese
       {"TEMP.COR"},
	#endif
	#if Language_Turkish
       {"RENK SICAKLIGI"},
	#endif
	#if Language_Polish
       {"COLOR TEMP."},
	#endif
	#if Language_Dutch
	{"KLEUR TEMP."},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x77\x88\x8B\x99\x86"},	
	#else
	{"\x77\x62\x65\x73\x60"},	
	#endif
	#endif
	#if Language_Korean
	{"\x84 \x85 \x86"}	,		 
	#endif
	
	#if Language_TraChinese
	,{"\x85 \x86"}			// 5 繁体
	#endif
};
BYTE code strMainMenu_OSDM7[LANG_Nums][15]=
{
	#if Language_English
	{"OSD SETTING"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"RéGLAGE OSD"},			// 3 FRENCE
	#endif
	#if Language_German
       {"MEN\xA7 EINSTELL."},
	#endif
	#if Language_SimChinese
	{"OSD \x88 \x7A"},			// 4 简体
	#endif
	#if Language_Italian
       {"MENU OSD"},
	#endif
	#if Language_Spanish
       {"OSD AJUSTE"},
	#endif
	#if Language_Portuguese
       {"OSD AJUSTE"},
	#endif
	#if Language_Turkish
       {"OSD AYARI"},
	#endif
	#if Language_Polish
       {"OSD SETTING"},
	#endif
	#if Language_Dutch
	{"OSD INSTELLEN"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x6E\x86\x98\x99\x97\x95\x90\x91\x8F OSD"},		
	#else
	{"\x6E\x60\x72\x73\x71\x6F\x6A\x6B\x69 OSD"},		
	#endif
	#endif
	#if Language_Korean
	{"\x90 \x91 \x8D \x92"}			 
	#endif
	
	#if Language_TraChinese
	,{"OSD \x88 \x7A"}			// 5 繁体
	#endif
};
BYTE code strMainMenu_ResetM7[LANG_Nums][18]=
{
	#if Language_English
	{"RESET"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"RéINITIALISER"},			// 3 FRENCE
	#endif
	#if Language_German
       {"ZUR\x0A7CKSETZEN"},
	#endif
	#if Language_SimChinese
	{"\x9C \x7A"},			// 4 简体
	#endif
	#if Language_Italian
       {"REGOLAZIONI"},
	#endif
	#if Language_Spanish
       {"REINICIAR"},
	#endif
	#if Language_Portuguese
       {"REINICIAR"},
	#endif
	#if Language_Turkish
       {"SIFIRLA"},
	#endif
	#if Language_Polish
       {"RESET"},
	#endif
	#if Language_Dutch
	{"RESET"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x72\x87\x97\x95\x98"},	
	#else
	{"RESET"},	
	#endif
	#endif
	#if Language_Korean
	{"\x99 \x61 \x6C"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x9C \x7A"}			// 5 繁体
	#endif
};
BYTE code strMainMenu_MiscM7[LANG_Nums][15]=
{
	#if Language_English
	{"MISC"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"AUTRES"},			// 3 FRENCE
	#endif
	#if Language_German
       {"SONSTIGES"},
	#endif
	#if Language_SimChinese
	{"\xA4 \xA5"},			// 4 简体
	#endif
	#if Language_Italian
       {"SETUP"},
	#endif
	#if Language_Spanish
       {"MISCELANEO"},
	#endif
	#if Language_Portuguese
       {"DIVERSOS"},
	#endif
	#if Language_Turkish
       {"DIGER"},
	#endif
	#if Language_Polish
       {"MISC."},
	#endif
	#if Language_Dutch
	{"ANDERE"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x70\x97\x95\x9E\x8B\x8B"},		
	#else
	{"\x70\x71\x6F\x78\x65\x65"},		
	#endif
	#endif
	#if Language_Korean
	{"\x61 \x9A"}			 
	#endif
	
	#if Language_TraChinese
	,{"\xA4 \xA5"}			// 5 繁体
	#endif
};
//---------------

BYTE code strBrtCon_BrightnessM8[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"BRIGHTNESS"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"LUMINOSIT\x8B"},			// 3 FRENCE
	#endif
	#if Language_German
       {"HELLIGKEIT"},
	#endif
	#if Language_SimChinese
	{"\x60 \x63   "},			// 4 简体
	#endif
	#if Language_Italian
       {"LUMINOSIT\x98"},
	#endif
	#if Language_Spanish
       {"BRILLO"},
	#endif
	#if Language_Portuguese
       {"BRILHO"},
	#endif
	#if Language_Turkish
       {"PARLAKLIK"},
	#endif
	#if Language_Polish
       {"BRIGHTNESS"},
	#endif
	#if Language_Dutch
	{"BELDERHEID"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x80\x97\x91\x95\x98\x99\xA1"},		
	#else
	{"\x80\x71\x6B\x6F\x72\x73\x7D"},		
	#endif
	#endif
	#if Language_Korean
	{"\x60 \x61"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x60 \x63   "}			// 5 繁体
	#endif
  };
BYTE code strBrtCon_ContrastM8[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"CONTRAST"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"CONTRASTE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"KONTRAST"},
	#endif
	#if Language_SimChinese
	{"\x61 \x62 \x63  "},			// 4 简体
	#endif
	#if Language_Italian
       {"CONTRASTO"},
	#endif
	#if Language_Spanish
       {"CONTRASTE"},
	#endif
	#if Language_Portuguese
       {"CONTRASTE"},
	#endif
	#if Language_Turkish
       {"KONTRAST"},
	#endif
	#if Language_Polish
       {"KONTRAST"},
	#endif
	#if Language_Dutch
	{"CONTRAST"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x6B\x95\x94\x99\x97\x86\x98\x99"},	
	#else
	{"\x6B\x6F\x6E\x73\x71\x60\x72\x73"},		
	#endif
	#endif
	#if Language_Korean
	{"\x62 \x63"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x61 \x62 \x63  "}			// 5 繁体
	#endif
  };
  #if 1
#if 0  
BYTE code strBrtCon_ECOM8[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"ECO"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"MODE IMAGE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"ECO"},
	#endif
	#if Language_SimChinese
	{"\x64 \x65 \x66 \x67"},			// 4 简体
	#endif
	#if Language_Italian
       {"ECO"},
	#endif
	#if Language_Spanish
       {"ECO"},
	#endif
	#if Language_Portuguese
       {"ECO"},
	#endif
	#if Language_Turkish
       {"EKO"},
	#endif
	#if Language_Polish
       {"ECO"},
	#endif
	#if Language_Dutch
	{"ECO"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"ECO"},			 
	#else
	{"ECO"},			 
	#endif
	#endif
	#if Language_Korean
	{"\x64 \x65 \x66 \x67"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x64 \x65 \x66 \x67"}			// 5 繁体
	#endif
  };
#else
BYTE code strBrtCon_ECOM8[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"MODE IMAGE"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"BILDMODUS"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"\x64 \x65 \x66 \x67"},			// 4 简体
	#endif
	#if Language_Italian
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"RESIM MODU"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#else
	{"PICTURE MODE"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"\x64 \x65 \x66 \x67"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x64 \x65 \x66 \x67"}			// 5 繁体
	#endif
  };
#endif
 BYTE code strStandardM1[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B STANDARD \x3E"}, 			// 1 ENGLISH//12
	#endif
	#if Language_France
	{"\x5B LA NORME \x3E"},			// 3 FRENCE
	#endif
	#if Language_German
       {"\x5B STANDARD \x3E"},
	#endif
	#if Language_SimChinese
	{"\x5B   \x6A  \x6B   \x3E"},			// 4 简体
	#endif
	#if Language_Italian
       {"\x5B STANDARD \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B STANDARD \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B STANDARD \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B STANDART \x3E"},
	#endif
	#if Language_Polish
       {"\x5B STANDARD \x3E"},
	#endif
	#if Language_Dutch
	{"\x5B STANDARD \x3E"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B\x72\x99\x86\x94\x8A\x86\x97\x99\x94\xA3\x90\x3E"},		
	#else
	{"\x5B \x72\x73\x60\x6E\x64\x60\x71\x73 \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x61 \x68    \x3E"}			 
	#endif
	#if Language_TraChinese
	,{"\x5B   \x6A  \x6B   \x3E"}			// 5 繁体
	#endif
};

 BYTE code strTextM1[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B   TEXT   \x3E"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"\x5B   TEXTE  \x3E"},			// 3 FRENCE
	#endif
	#if Language_German
       {"\x5B   TEXT   \x3E"},
	#endif
	#if Language_SimChinese
	{"\x5B   \x6C  \x6D   \x3E"},			// 4 简体
	#endif
	#if Language_Italian
       {"\x5B   TESTO  \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B   TEXTO  \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B   TEXTO  \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   METIN  \x3E"},
	#endif
	#if Language_Polish
       {"\x5B  TEKST   \x3E"},
	#endif
	#if Language_Dutch
	{"\x5B   TEXT   \x3E"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   \x73\x8B\x91\x98\x99   \x3E"},	
	#else
	{"\x5B  \x73\x65\x6B\x72\x73   \x3E"},	
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x69 \x6A    \x3E"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x5B   \x6C  \x6D   \x3E"}			// 5 繁体
	#endif
};
 
 BYTE code strMovieM1[LANG_Nums][14]=
 {//123456789012345678901234567890
	#if Language_English
	{"\x5B   MOVIE  \x3E"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"\x5B   FILM   \x3E"},			// 3 FRENCE
	#endif
	#if Language_German
       {"\x5B   FILM   \x3E"},
	#endif
	#if Language_SimChinese
	{"\x5B   \x6E  \x6F   \x3E"},			// 4 简体
	#endif
	#if Language_Italian
       {"\x5B  FILMATO \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B   MOVIE  \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B   FILME  \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   FILM   \x3E"},
	#endif
	#if Language_Polish
       {"\x5B   FILM   \x3E"},
	#endif
	#if Language_Dutch
	{"\x5B   MOVIE  \x3E"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B    \x6B\x8F\x94\x95   \x3E"},			 
	#else
	{"\x5B   MOVIE  \x3E"},			 
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x6B \x6C    \x3E"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x5B   \x6E  \x6F   \x3E"}			// 5 繁体
	#endif
};

 BYTE code strGameM1[LANG_Nums][14]=
 {//123456789012345678901234567890
	#if Language_English
	{"\x5B   GAME   \x3E"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"\x5B   JEU    \x3E"},			// 3 FRENCE
	#endif
	#if Language_German
       {"\x5B   SPIEL  \x3E"},
	#endif
	#if Language_SimChinese
	{"\x5B   \x70  \x71   \x3E"},			// 4 简体
	#endif
	#if Language_Italian
       {"\x5B   GIOCO  \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B   GAME   \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B   JOGO   \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   OYUN   \x3E"},
	#endif
	#if Language_Polish
       {"\x5B   GAME   \x3E"},
	#endif
	#if Language_Dutch
	{"\x5B   GAME   \x3E"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B    \x69\x89\x97\x86   \x3E"},			 
	#else
	{"\x5B   GAME   \x3E"},			 
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x6D \x6E    \x3E"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x5B   \x70  \x71   \x3E"}			// 5 繁体
	#endif
};
#endif

#if Enable_AdjustDCR
BYTE code strBrtCon_DCRM8[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"DCR"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"DCR"},			// 3 FRENCE
	#endif
	#if Language_German
       {"DCR"},
	#endif
	#if Language_SimChinese
	{"\x68 \x69 \x61 \x62 \x63"},			// 4 简体
	#endif
	#if Language_Italian
       {"DCR"},
	#endif
	#if Language_Spanish
       {"DCR"},
	#endif
	#if Language_Portuguese
       {"DCR"},
	#endif
	#if Language_Turkish
       {"DCR"},
	#endif
	#if Language_Polish
       {"DCR"},
	#endif
	#if Language_Dutch
	{"DCR"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"DCR"},		
	#else
	{"DCR"},		
	#endif
	#endif
	#if Language_Korean
	{"\x6F \x70 \x6B \x63 \x71"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x68 \x69 \x61 \x62 \x63"}			// 5 繁体
	#endif
  };

BYTE code strDCR_OnMode[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B    ON    \x3E"},
	#endif
	#if Language_France
	 {"\x5B  MARCHE  \x3E"},
	 #endif
	#if Language_German
       {"\x5B    EIN   \x3E"},
	#endif
	#if Language_SimChinese
	 {"\x5B   \x72  \x74   \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   ACIK   \x3E"},
	#endif
	#if Language_Polish
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B    ON    \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B    \x62\x91\x92\x2E   \x3E"},			
	#else
	{"\x5B    ON    \x3E"},			
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x72  \x61   \x3E"}			 
	#endif
	 
	#if Language_TraChinese
	 ,{"\x5B   \x72  \x74   \x3E"}
	#endif
};
BYTE code strDCR_OffMode[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B    OFF   \x3E"},
	#endif
	#if Language_France
	 {"\x5B  ARRET   \x3E"},
	 #endif
	#if Language_German
       {"\x5B    AUS   \x3E"},
	#endif
	#if Language_SimChinese
	 {"\x5B   \x73  \x74   \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B  KAPALI  \x3E"},
	#endif
	#if Language_Polish
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B    OFF   \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   \x62\xA3\x91\x92\x2E   \x3E"},		
	#else
	{"\x5B    OFF   \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x73  \x61   \x3E"}			 
	#endif
	 
	#if Language_TraChinese
	 ,{"\x5B   \x73  \x74   \x3E"}
	#endif
};
#endif

#if OSD_OD
BYTE code strBrtCon_ODM8[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"OD"}, 			// 1 ENGLISH
	#else
	{"OD"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"OD"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	,{"OD"}, 			// 1 ENGLISH
	#endif
  };

BYTE code strOD_OnMode[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B    ON    \x3E"},
	#endif
	#if Language_France
	 {"\x5B  MARCHE  \x3E"},
	 #endif
	#if Language_German
       {"\x5B    EIN   \x3E"},
	#endif
	#if Language_SimChinese
	 {"\x5B   \x72  \x74   \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   ACIK   \x3E"},
	#endif
	#if Language_Polish
       {"\x5B    ON    \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B    ON    \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B    \x62\x91\x92\x2E   \x3E"},			
	#else
	{"\x5B    ON    \x3E"},			
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x72  \x61   \x3E"}			 
	#endif
	 
	#if Language_TraChinese
	 ,{"\x5B   \x72  \x74   \x3E"}
	#endif
};
BYTE code strOD_OffMode[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B    OFF   \x3E"},
	#endif
	#if Language_France
	 {"\x5B  ARRET   \x3E"},
	 #endif
	#if Language_German
       {"\x5B    AUS   \x3E"},
	#endif
	#if Language_SimChinese
	 {"\x5B   \x73  \x74   \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B  KAPALI  \x3E"},
	#endif
	#if Language_Polish
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B    OFF   \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   \x62\xA3\x91\x92\x2E   \x3E"},		
	#else
	{"\x5B    OFF   \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x73  \x61   \x3E"}			 
	#endif
	 
	#if Language_TraChinese
	 ,{"\x5B   \x73  \x74   \x3E"}
	#endif
};
#endif

#if LowBlueLight
BYTE code strBlueLightM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"LUMIèRE BLEUE"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#else
	{"BLUE LIGHT"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"\xB2 \xB3 \xB4 \x82 \xB5"}, 			
	#endif
	
	#if Language_TraChinese
	,{"\x75 \x76 \x79 \x7A"}			// 5 繁体
	#endif
};

BYTE code strBlueLight_ShieldM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_France
	{"\x5B  BOUCLIER  \x3E"},   
	#endif
	#if Language_German
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_SimChinese
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_Italian
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_Spanish
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_Portuguese
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_Turkish
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_Polish
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_Dutch
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B  SHIELD  \x3E"},   
	#else
	{"\x5B  SHIELD  \x3E"},   
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \xB6  \xB7   \x3E"},   
	#endif
	
	#if Language_TraChinese
	,{"\x75 \x76 \x79 \x7A"}			// 5 繁体
	#endif
};

BYTE code strBlueLight_MiddleM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_France
	{"\x5B  MILIEU  \x3E"},   
	#endif
	#if Language_German
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_SimChinese
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_Italian
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_Spanish
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_Portuguese
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_Turkish
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_Polish
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_Dutch
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B  MIDDLE  \x3E"},   
	#else
	{"\x5B  MIDDLE  \x3E"},   
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \xB8  \x97   \x3E"},   
	#endif
	
	#if Language_TraChinese
	,{"\x75 \x76 \x79 \x7A"}			// 5 繁体
	#endif
};

BYTE code strBlueLight_StandardM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_France
	{"\x5B LA NORME \x3E"},  
	#endif
	#if Language_German
	{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_SimChinese
	{"\x5B   \x6A  \x6B   \x3E"},//{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_Italian
	{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_Spanish
	{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_Portuguese
	{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_Turkish
	{"\x5B STANDART \x3E"},  
	#endif
	#if Language_Polish
	{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_Dutch
	{"\x5B STANDARD \x3E"},  
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B STANDARD \x3E"},  
	#else
	{"\x5B STANDARD \x3E"},  
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \xB9  \xBA   \x3E"},  
	#endif
	
	#if Language_TraChinese
	,{"\x75 \x76 \x79 \x7A"}			// 5 繁体
	#endif
};

BYTE code strBlueLight_BrowsingM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_France
	{"\x5B Naviguer \x3E"},  
	#endif
	#if Language_German
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_SimChinese
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_Italian
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_Spanish
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_Portuguese
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_Turkish
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_Polish
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_Dutch
	{"\x5B Browsing \x3E"},  
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B Browsing \x3E"},  
	#else
	{"\x5B Browsing \x3E"},  
	#endif
	#endif
	#if Language_Korean
	{"\x5B   Browsing  \x3E"},  
	#endif
	
	#if Language_TraChinese
	{"\x5B   Browsing  \x3E"}, 			// 5 繁体
	#endif
};

BYTE code strBlueLight_ReadM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_France
	{"\x5B Lecture  \x3E"},  
	#endif
	#if Language_German
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_SimChinese
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_Italian
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_Spanish
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_Portuguese
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_Turkish
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_Polish
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_Dutch
	{"\x5B Reading  \x3E"},  
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B Reading  \x3E"},  
	#else
	{"\x5B Reading  \x3E"},  
	#endif
	#endif
	#if Language_Korean
	{"\x5B   Reading   \x3E"},  
	#endif
	
	#if Language_TraChinese
	{"\x5B   Reading   \x3E"}, 			// 5 繁体
	#endif
};
BYTE code strBlueLight_VideoM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B  Video   \x3E"},  
	#endif
	#if Language_France
	{"\x5B  Vidéo   \x3E"},   
	#endif
	#if Language_German
	{"\x5B  Video   \x3E"},   
	#endif
	#if Language_SimChinese
	{"\x5B  Video   \x3E"},   
	#endif
	#if Language_Italian
	{"\x5B  Video   \x3E"},   
	#endif
	#if Language_Spanish
	{"\x5B  Video   \x3E"},  
	#endif
	#if Language_Portuguese
	{"\x5B  Video   \x3E"},   
	#endif
	#if Language_Turkish
	{"\x5B  Video   \x3E"},   
	#endif
	#if Language_Polish
	{"\x5B  Video   \x3E"},   
	#endif
	#if Language_Dutch
	{"\x5B  Video   \x3E"}, 
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B  Video   \x3E"},   
	#else
	{"\x5B  Video   \x3E"},   
	#endif
	#endif
	#if Language_Korean
	{"\x5B  Video   \x3E"},   
	#endif
	
	#if Language_TraChinese
	{"\x5B  Video   \x3E"},  			// 5 繁体
	#endif
};
BYTE code strBlueLight_lowlightM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B LowLight \x3E"},  
	#endif
	#if Language_France
	{"\x5B Lumière faible \x3E"},   
	#endif
	#if Language_German
	{"\x5B LowLight \x3E"},   
	#endif
	#if Language_SimChinese
	{"\x5B LowLight \x3E"},   
	#endif
	#if Language_Italian
	{"\x5B LowLight \x3E"},  
	#endif
	#if Language_Spanish
	{"\x5B LowLight \x3E"},   
	#endif
	#if Language_Portuguese
	{"\x5B LowLight \x3E"},   
	#endif
	#if Language_Turkish
	{"\x5B LowLight \x3E"},   
	#endif
	#if Language_Polish
	{"\x5B LowLight \x3E"},   
	#endif
	#if Language_Dutch
	{"\x5B LowLight \x3E"},  
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B LowLight \x3E"},  
	#else
	{"\x5B LowLight \x3E"},   
	#endif
	#endif
	#if Language_Korean
	{"\x5B LowLight \x3E"},   
	#endif
	
	#if Language_TraChinese
	{"\x5B LowLight \x3E"},  			// 5 繁体
	#endif
};



BYTE code strBlueLight_Off[LANG_Nums][14]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B    OFF   \x3E"},
	#endif
	#if Language_France
	 {"\x5B  ARRET   \x3E"},
	 #endif
	#if Language_German
       {"\x5B    AUS   \x3E"},
	#endif
	#if Language_SimChinese
	 {"\x5B   \x73  \x74   \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B  KAPALI  \x3E"},
	#endif
	#if Language_Polish
       {"\x5B    OFF   \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B    OFF   \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   \x62\xA3\x91\x92\x2E   \x3E"},		
	#else
	{"\x5B    OFF   \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B   \x73  \x61   \x3E"}			 
	#endif
	 
	#if Language_TraChinese
	 ,{"\x5B   \x73  \x74   \x3E"}
	#endif
};

#endif

//-----------
//Image text string
BYTE code strImage_HPositionM9[LANG_Nums][17]=
{//123456789012345678901234567890
	#if Language_English
	{"H. POSITION"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"POSITION H."},			// 3 FRENCE
	#endif
	#if Language_German
       {"H. POSITION"},
	#endif
	#if Language_SimChinese
	{"\x75 \x76 \x79 \x7A"},			// 4 简体
	#endif
	#if Language_Italian
       {"POSIZIONE O."},
	#endif
	#if Language_Spanish
       {"H. POSICION"},
	#endif
	#if Language_Portuguese
       {"H. POSI\x7b\xA0O"},
	#endif
	#if Language_Turkish
       {"H. KONUM"},
	#endif
	#if Language_Polish
       {"H. POSITION"},
	#endif
	#if Language_Dutch
	{"H. POSITIE"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x63\x2D\x96\x95\x8E\x8F\x9D\x8F\xA2"},	
	#else
	{"\x63\x2D\x70\x6F\x68\x69\x77\x69\x80"},	
	#endif
	#endif
	#if Language_Korean
	{"\x74 \x76 \x77 \x78"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x75 \x76 \x79 \x7A"}			// 5 繁体
	#endif
  };

BYTE code strImage_VPositionM9[LANG_Nums][15]=
{//123456789012345678901234567890
	#if Language_English
	{"V. POSITION"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"POSITION V."},			// 3 FRENCE
	#endif
	#if Language_German
       {"V. POSITION"},
	#endif
	#if Language_SimChinese
	{"\x77 \x78 \x79 \x7A"},			// 4 简体
	#endif
	#if Language_Italian
       {"POSIZIONE V."},
	#endif
	#if Language_Spanish
       {"V. POSICION"},
	#endif
	#if Language_Portuguese
       {"V. POSI\x7b\xA0O"},
	#endif
	#if Language_Turkish
       {"V. KONUM"},
	#endif
	#if Language_Polish
       {"V.POSITION"},
	#endif
	#if Language_Dutch
	{"V. POSITIE"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x62\x2D\x96\x95\x8E\x8F\x9D\x8F\xA2"},	
	#else
	{"\x62\x2D\x70\x6F\x68\x69\x77\x69\x80"},	
	#endif
	#endif
	#if Language_Korean
	{"\x74 \x79 \x77 \x78"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x77 \x78 \x79 \x7A"}			// 5 繁体
	#endif
  };

BYTE code strImage_ClockM9[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"CLOCK"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"HORLOGE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"UHR"},
	#endif
	#if Language_SimChinese
	{"\x7B \x7C"},			// 4 简体
	#endif
	#if Language_Italian
       {"CLOCK"},
	#endif
	#if Language_Spanish
       {"RELOJ"},
	#endif
	#if Language_Portuguese
       {"REL\xA5GIO"},
	#endif
	#if Language_Turkish
       {"SAAT"},
	#endif
	#if Language_Polish
       {"CLOCK"},
	#endif
	#if Language_Dutch
	{"KLOK"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x78\x86\x98\x99\x95\x99\x86"},			 
	#else
	{"\x78\x60\x72\x73\x6F\x73\x7C"},			 
	#endif
	#endif
	#if Language_Korean
	{"\x7A \x7B"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x7B \x7C"}			// 5 繁体
	#endif
  };
BYTE code strImage_PhaseM9[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"PHASE"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"PHASE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"PHASE"},
	#endif
	#if Language_SimChinese
	{"\x7D \x7E"},			// 4 简体
	#endif
	#if Language_Italian
       {"FASE"},
	#endif
	#if Language_Spanish
       {"FASE"},
	#endif
	#if Language_Portuguese
       {"FASE"},
	#endif
	#if Language_Turkish
       {"FAZ"},
	#endif
	#if Language_Polish
       {"FAZA"},
	#endif
	#if Language_Dutch
	{"FASE"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x75\x86\x8E\x86"},			
	#else
	{"\x75\x60\x68\x60"},			
	#endif
	#endif
	#if Language_Korean
	{"\x77 \x7C"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x7D \x7E"}			// 5 繁体
	#endif
  }; 

    #if Enable_Expansion
BYTE code strImage_AspectM9[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"ASPECT"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"ASPECT"},			// 3 FRENCE
	#endif
	#if Language_German
       {"BILDFORMAT"},
	#endif
	#if Language_SimChinese
	{"\x7F \x80 \x62 \x81"},			// 4 简体
	#endif
	#if Language_Italian
       {"ASPETTO"},
	#endif
	#if Language_Spanish
       {"ASPECTO"},
	#endif
	#if Language_Portuguese
       {"ASPECTO"},
	#endif
	#if Language_Turkish
       {"DURUM"},
	#endif
	#if Language_Polish
       {"ASPEKT"},
	#endif
	#if Language_Dutch
	{"ASPECT"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x62\x8F\x8A"},		
	#else
	{"\x60\x72\x70\x65\x6B\x73"},		
	#endif
	#endif
	#if Language_Korean
	{"\x7D \x7E \x7F \x80 \x71"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x7F \x80 \x62 \x81"}			// 5 繁体
	#endif
  };

//Aspect Select
BYTE code strAspectMode_Wide[LANG_Nums][15]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B  WIDE  \x3E"},//10
	#endif
	#if Language_France
 	{"\x5B  LARGE \x3E"},
 	#endif
	#if Language_German
       {"\x5B BREITBILD \x3E"},   //  
	#endif
	#if Language_SimChinese
 	{"\x5B  \x20\x82\x83\x20  \x3E"},
 	#endif
	#if Language_Italian
       {"\x5B  WIDE  \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B  ANCHO \x3E"},//10
	#endif
	#if Language_Portuguese
       {"\x5B  LARGO \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B  GENIS \x3E"},
	#endif
	#if Language_Polish
       {"\x5B  WIDE  \x3E"},
	#endif
	#if Language_Dutch
 	{"\x5B  WIDE  \x3E"},
 	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B\x79\x8F\x97\x95\x91\x8F\x90 \x3E"},		
	#else
	{"\x5B\x79\x69\x71\x6F\x6B\x69\x6A \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B  \x81 \x82 \x67 \x3E"}			 
	#endif
	
	#if Language_TraChinese
 	,{"\x5B  \x20\x82\x83\x20  \x3E"},
	#endif
  };

BYTE code strAspectMode_4To3[LANG_Nums][15]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B   4:3  \x3E"},//10
	#endif
	#if Language_France
	 {"\x5B   4:3  \x3E"},
	 #endif
	#if Language_German
       {"\x5B    4:3    \x3E"},
	#endif
	#if Language_SimChinese
	 {"\x5B   4:3  \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B   4:3  \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B   4:3  \x3E"},//10
	#endif
	#if Language_Portuguese
       {"\x5B   4:3  \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   4:3  \x3E"},
	#endif
	#if Language_Polish
       {"\x5B   4:3  \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B   4:3  \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   4:3  \x3E"},		
	#else
	{"\x5B   4:3  \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B  4:3   \x3E"}			 
	#endif
	 
	#if Language_TraChinese
	 ,{"\x5B   4:3  \x3E"},
	#endif
  };
#endif
//---------------------------
//color temperatur
BYTE code strColor_CTempM10[LANG_Nums][15]=
{//123456789012345678901234567890
	#if Language_English
	{"COLOR TEMP."}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"TEMP. COULEUR"},			// 3 FRENCE
	#endif
	#if Language_German
       {"FARBTEMP."},
	#endif
	#if Language_SimChinese
	{"\x85 \x86"},			// 4 简体
	#endif
	#if Language_Italian
       {"TEMP.COLORE"},
	#endif
	#if Language_Spanish
       {"TEMP.COLOR"},
	#endif
	#if Language_Portuguese
       {"TEMP.COR"},
	#endif
	#if Language_Turkish
       {"RENK ISININ"},
	#endif
	#if Language_Polish
       {"COLOR TEMP."},
	#endif
	#if Language_Dutch
	{"KLEUR TEMP."},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x77\x88\x8B\x99\x86"},	
	#else
	{"\x77\x62\x65\x73\x60"},	
	#endif
	#endif
	#if Language_Korean
	{"\x84 \x85 \x86"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x85 \x86"}			// 5 繁体
	#endif
};

BYTE code strColor_RedM10[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"RED"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"ROUGE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"ROT"},
	#endif
	#if Language_SimChinese
	{"\x8C \x85"},			// 4 简体
	#endif
	#if Language_Italian
       {"ROSSO"},
	#endif
	#if Language_Spanish
       {"ROJO"},
	#endif
	#if Language_Portuguese
       {"VERMELHO"},
	#endif
	#if Language_Turkish
       {"KIRMIZI"},
	#endif
	#if Language_Polish
       {"RED"},
	#endif
	#if Language_Dutch
	{"ROOD"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x6B\x97\x86\x98\x94\xA3\x90"},		
	#else
	{"\x6B\x71\x60\x72\x6E\x7C\x6A"},		
	#endif
	#endif
	#if Language_Korean
	{"\x70 \x84"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x8C \x85"}			// 5 繁体
	#endif
};

BYTE code strColor_GreenM10[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"GREEN"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"VERT"},			// 3 FRENCE
	#endif
	#if Language_German
       {"GR\xA7N"},
	#endif
	#if Language_SimChinese
	{"\x8D \x85"},			// 4 简体
	#endif
	#if Language_Italian
       {"VERDE"},
	#endif
	#if Language_Spanish
       {"VERDE"},
	#endif
	#if Language_Portuguese
       {"VERDE"},
	#endif
	#if Language_Turkish
       {"YESIL"},
	#endif
	#if Language_Polish
       {"GREEN"},
	#endif
	#if Language_Dutch
	{"GROEN"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x68\x8B\x92\x8B\x94\xA3\x90"},	
	#else
	{"\x68\x65\x6C\x65\x6E\x7C\x6A"},	
	#endif
	#endif
	#if Language_Korean
	{"\x8E \x84"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x8D \x85"}			// 5 繁体
	#endif
};

BYTE code strColor_BlueM10[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"BLUE"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"BLEU"},			// 3 FRENCE
	#endif
	#if Language_German
       {"BLAU"},
	#endif
	#if Language_SimChinese
	{"\x8E \x85"},			// 4 简体
	#endif
	#if Language_Italian
       {"BLU"},
	#endif
	#if Language_Spanish
       {"AZUL"},
	#endif
	#if Language_Portuguese
       {"AZUL"},
	#endif
	#if Language_Turkish
       {"MAVI"},
	#endif
	#if Language_Polish
       {"BLUE"},
	#endif
	#if Language_Dutch
	{"BLAUW"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x72\x8F\x94\x8F\x90"},	
	#else
	{"\x72\x69\x6E\x69\x6A"},	
	#endif
	#endif
	#if Language_Korean
	{"\x8F \x84"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x8E \x85"}			// 5 繁体
	#endif
};

//color temp select
#if FactoryAuto_5400K
BYTE code strColorTemp_5400k[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B   WARM   \x3E"},//12
	#endif
	#if Language_France
	{"\x5B    CHAUD    \x3E"},
	#endif
	#if Language_German
       {"\x5B   WARM   \x3E"},
	#endif
	#if Language_SimChinese
	{"\x5B   \x8A\x85\x86    \x3E"},
	#endif
	#if Language_Italian
       {"\x5B   CALDO  \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B  CALIDO  \x3E"},//12
	#endif
	#if Language_Portuguese
       {"\x5B  QUENTE  \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   SICAK   \x3E"},
	#endif
	#if Language_Polish
       {"\x5B   WARM   \x3E"},
	#endif
	#if Language_Dutch
	{"\x5B   WARM   \x3E"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   \x73\x8B\x96\x92\xA3\x8B   \x3E"},		
	#else
	{"\x5B   \x73\x65\x70\x6C\x7C\x65   \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B  \x87 \x88 \x89   \x3E"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x5B   \x8A\x85\x86    \x3E"},
	#endif
};
#endif

#if FactoryAuto_6500K
BYTE code strColorTemp_6500k[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#if Language_France
	{"\x5B  ORDINAIRE  \x3E"},//12
	#endif
	#if Language_German
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#if Language_SimChinese
	{"\x5B   \x6A  \x6B   \x3E"},//{"\x5B  NORMAL  \x3E"},//12  
	#endif
	#if Language_Italian
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#if Language_Spanish
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#if Language_Portuguese
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#if Language_Turkish
	{"\x5B  NORMAL   \x3E"},//12
	#endif
	#if Language_Polish
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#if Language_Dutch
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B  NORMAL  \x3E"},//12
	#else
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	#endif
	#if Language_Korean
	{"\x5B  NORMAL  \x3E"},//12
	#endif
	
	#if Language_TraChinese
	,{"\x5B  NORMAL  \x3E"},//12
	#endif
};
#endif

#if FactoryAuto_9300K
BYTE code strColorTemp_9300k[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B   COOL   \x3E"},//12
	#endif
	#if Language_France
	{"\x5B   FROID     \x3E"},//14
	#endif
	#if Language_German
       {"\x5B   KALT   \x3E"},
	#endif
	#if Language_SimChinese
	{"\x5B   \x8B\x85\x86    \x3E"},
	#endif
	#if Language_Italian
       {"\x5B  FREDDO  \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B   FRIO   \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B   FRIO   \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   SOGUK   \x3E"},
	#endif
	#if Language_Polish
       {"\x5B   COOL   \x3E"},
	#endif
	#if Language_Dutch
	{"\x5B   KOEL   \x3E"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B  \x76\x95\x92\x95\x8A\x94\xA3\x8B  \x3E"},		
	#else
	{"\x5B   \x76\x6F\x6C\x6F\x64    \x3E"},		
	#endif
	#endif
	#if Language_Korean
	{"\x5B  \x8A \x8B \x8C   \x3E"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x5B   \x8B\x85\x86    \x3E"},
	#endif
};
#endif
BYTE code strColorTemp_User[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"\x5B   USER   \x3E"},
	#endif
	#if Language_France
	{"\x5B UTILISATEUR \x3E"},//14
	#endif
	#if Language_German
       {"\x5B BENUTZER \x3E"},  
	#endif
	#if Language_SimChinese
	{"\x5B   \x74\x87\x88\x89   \x3E"},
	#endif
	#if Language_Italian
       {"\x5B  UTENTE  \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B  USUARIO \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5BUTILIZADOR\x3E"},
	#endif
	#if Language_Turkish
       {"\x5B KULLANICI \x3E"},
	#endif
	#if Language_Polish
       {"\x5B   USER   \x3E"},
	#endif
	#if Language_Dutch
	{"\x5B GEBRUIKER\x3E"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   \x70\x95\x92\xA1\x8E.   \x3E"},
	#else
	{"\x5B\x70\x6F\x6C\x7D\x68\x6F\x62\x60\x73\x65\x6C\x7D\x3E"},
	#endif
	#endif
	#if Language_Korean
	{"\x5B\x64 \x65 \x83 \x8D \x7E \x3E"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x5B   \x74\x87\x88\x89   \x3E"},
	#endif
};
//---------------------------
BYTE code strOSD_LanguageM11[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"LANGUAGE"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"LANGUE"},			// 4 GERMAN
	#endif	
	#if Language_German
       {"SPRACHE"},
	#endif
	#if Language_SimChinese
	{"\x8F \x90 \x91 \x92"},			// 4 简体
	#endif
	#if Language_Italian
       {"LINGUA"},
	#endif
	#if Language_Spanish
       {"IDIOMA"},
	#endif
	#if Language_Portuguese
       {"IDIOMA"},
	#endif
	#if Language_Turkish
       {"DIL"},
	#endif
	#if Language_Polish
       {"JEZYKA"},
	#endif
	#if Language_Dutch
	{"TAAL"},			// 3 FRENCE
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x80\x8E\x0A3\x91"},		
	#else
	{"\x80\x68\x7C\x6B"},		
	#endif
	#endif
	#if Language_Korean
	{"\x93 \x94"}			 
	#endif
	#if Language_TraChinese
	,{"\x8F \x90 \x91 \x92"}			// 5 繁体
	#endif
};

BYTE code strLanguage[13][11]=
{//123456789012345678901234567890
	#if 1//Language_English
	{"ENGLISH"},//{"English"}, 			// 1 ENGLIS
	#endif
	#if 1//Language_France
	{"FRAN\x05CAIS"}, 			// 3 FRENCE
	#endif
	#if 1//Language_German
       {"DEUTSCH"},
	#endif
	#if 1//Language_SimChinese
	{"\x12\x13\x16\x17"},// 4 简体
	#endif
	#if 1//Language_Italian
       {"ITALIANO"},
	#endif
	#if 1//Language_Spanish
       {"ESPA\x15OL"},
	#endif
	#if 1//Language_Portuguese
       {"PORTUGU\x14S"},
	#endif
	#if 1//Language_Turkish
       {"TURKCE"},
	#endif
	#if 1//Language_Polish
       {"POLSKI"},
	#endif	
	#if 1//Language_Dutch
	{"NEDERLANDS"},//{"Deutsch"},			// 4 GERMAN
	#endif
	#if 1//Language_Russian
	{"\x50\x18\x43\x43\x4B\x19\x1A"},			 
	#endif
	#if 1//Language_Korean
	{"\x5D \x5E"}	,		 
	#endif
	
	#if 1//Language_TraChinese
	{"\x13\x14\x15\x16"}	// 5 繁体
	#endif
};

BYTE code strOSD_HPosM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"OSD H. POS."}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"POSITION H. "},			// 3 FRENCE
	#endif
	#if Language_German
       {"OSD H. POS."},
	#endif
	#if Language_SimChinese
	{"\x75 \x76 \x79 \x7A"},			// 4 简体
	#endif
	#if Language_Italian
       {"POSIZIONE \x40OSD"},
	#endif
	#if Language_Spanish
       {"OSD H. POSICION"},
	#endif
	#if Language_Portuguese
       {"OSD H. POSI\x7b\xA0O"},
	#endif
	#if Language_Turkish
       {"OSD H. KONUM"},
	#endif
	#if Language_Polish
       {"OSD H. POSITION"},
	#endif
	#if Language_Dutch
	{"OSD H. POSITIE"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"OSD \x63\x2D\x70\x95\x8E."},		
	#else
	{"OSD \x63\x2D\x70\x6F\x68."},		
	#endif
	#endif
	#if Language_Korean
	{"\x90 \x91 \x74 \x76 \x77 \x78"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x75 \x76 \x79 \x7A"}			// 5 繁体
	#endif
};
BYTE code strOSD_VPosM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"OSD V. POS."}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"POSITION V."},			// 3 FRENCE
	#endif
	#if Language_German
       {"OSD V. POS."},
	#endif
	#if Language_SimChinese
	{"\x77 \x78 \x79 \x7A"},			// 4 简体
	#endif
	#if Language_Italian
       {"POSIZIONE \x3FOSD"},
	#endif
	#if Language_Spanish
       {"OSD V. POSICION"},
	#endif
	#if Language_Portuguese
       {"OSD V. POSI\x7b\xA0O"},
	#endif
	#if Language_Turkish
       {"OSD V. KONUM"},
	#endif
	#if Language_Polish
       {"OSD V. POSITION"},
	#endif
	#if Language_Dutch
	{"OSD V. POSITIE"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"OSD \x62\x2D\x70\x95\x8E."},			
	#else
	{"OSD \x62\x2D\x70\x6F\x68."},			
	#endif
	#endif
	#if Language_Korean
	{"\x90 \x91 \x74 \x79 \x77 \x78"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x77 \x78 \x79 \x7A"}			// 5 繁体
	#endif
};
BYTE code strOSD_TimerM11[LANG_Nums][13]=
{//123456789012345678901234567890
	#if Language_English
	{"OSD TIMER"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"FIXA.TEMPS"},			// 3 FRENCE
	#endif
	#if Language_German
       {"OSD TIMER"},
	#endif
	#if Language_SimChinese
	{"\x93 \x94 \x88 \x89"},			// 4 简体
	#endif
	#if Language_Italian
       {"TIMEOUT OSD"},
	#endif
	#if Language_Spanish
       {"OSD TIMEOUT"},
	#endif
	#if Language_Portuguese
       {"OSD TIMEOUT"},
	#endif
	#if Language_Turkish
       {"OSD ZAMANI"},
	#endif
	#if Language_Polish
       {"TIMER OSD"},
	#endif
	#if Language_Dutch
	{"OSD KLOK"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x73\x86\x90\x93\x8B\x97 OSD"},	
	#else
	{"\x73\x60\x6A\x6D\x65\x71 OSD"},	
	#endif
	#endif
	#if Language_Korean
	{"\x90 \x91 \x95 \x96 \x96 \x97"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x93 \x94 \x88 \x89"}			// 5 繁体
	#endif
};
BYTE code strOSD_TransM11[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"TRANSPARENCY"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"TRANSPARENCE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"TRANSPARENZ"},
	#endif
	#if Language_SimChinese
	{"\x95 \x96 \x63"},			// 4 简体
	#endif
	#if Language_Italian
       {"TRASPARENZA"},
	#endif
	#if Language_Spanish
       {"TRANSPARENCIA"},
	#endif
	#if Language_Portuguese
       {"TRANSPARENCIA"},
	#endif
	#if Language_Turkish
       {"SAYDAMLIK"},
	#endif
	#if Language_Polish
       {"TRANSPARENCY"},
	#endif
	#if Language_Dutch
	{"TRANSPARANTIE"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x70\x97\x95\x8E\x97\x86\x9E\x94\x95\x98\x99\xA1"},	
	#else
	{"\x70\x71\x6F\x68\x71\x60\x78\x6E\x6F\x72\x73\x7D"},	
	#endif
	#endif
	#if Language_Korean
	{"\x90 \x91 \x98 \x62 \x86"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x95 \x96 \x63"}			// 5 繁体
	#endif
};
//-----------------------------
BYTE code strHotKeyContrast[LANG_Nums][13]=   //  S OK
{//123456789012345678901234567890
	#if Language_English
	{"CONTRAST"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"CONTRASTE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"CONTRAST"},
	#endif
	#if Language_SimChinese
	{"\x61 \x62 \x63"},			// 4 简体
	#endif
	#if Language_Italian
       {"CONTRASTO"},
	#endif
	#if Language_Spanish
       {"CONTRASTE"},
	#endif
	#if Language_Portuguese
       {"CONTRASTE"},
	#endif
	#if Language_Turkish
       {"KONTRAST"},
	#endif
	#if Language_Polish
       {"KONTRAST."},
	#endif
	#if Language_Dutch
	{"CONTRAST"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x6B\x95\x94\x99\x97\x86\x98\x99"},	
	#else
	{"\x6B\x6F\x6E\x73\x71\x60\x72\x73"},	
	#endif
	#endif
	#if Language_Korean
	{"\x62 \x63"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x61 \x62 \x63"}			// 5 繁体
	#endif
};
#if KeyInputSelectMenu

BYTE code strHotKeyInputType_VGA[LANG_Nums][8]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"VGA"}, 			// 1 ENGLISH
	#else
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"VGA"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	,{"VGA"}, 			// 1 ENGLISH
	#endif
  };
  
BYTE code strHotKeyInputType_DVI[LANG_Nums][8]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"DVI"}, 			// 1 ENGLISH
	#else
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"DVI"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	,{"DVI"}, 			// 1 ENGLISH
	#endif
  };

BYTE code strHotKeyInputType_HDMI[LANG_Nums][8]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"HDMI"}, 			// 1 ENGLISH
	#else
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"HDMI"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	,{"HDMI"}, 			// 1 ENGLISH
	#endif
  };

BYTE code strHotKeyInputType_DP[LANG_Nums][8]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"DP"}, 			// 1 ENGLISH
	#else
	{"DP"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"DP"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	,{"DP"}, 			// 1 ENGLISH
	#endif
  };

#endif

#if EN_HotKeyTimes

BYTE code strHotKeyTimes[LANG_Nums][6]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"FOIS"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"\xAB\x7B"},
	#endif
	#if Language_Italian
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"TIMES"}, 			// 1 ENGLISH
	#else
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"TIMES"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	{"TIMES"}, 			// 1 ENGLISH
	#endif
  };

BYTE code strHotKeyTimes_10[LANG_Nums][6]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"10:00"}, 			// 1 ENGLISH
	#else
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"10:00"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	{"10:00"}, 			// 1 ENGLISH
	#endif
  };
  
  BYTE code strHotKeyTimes_20[LANG_Nums][6]=   //	S OK
  {//123456789012345678901234567890
	#if Language_English
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_France
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_German
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_SimChinese
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_Italian
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_Spanish
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_Portuguese
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_Turkish
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_Polish
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_Dutch
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	  {"20:00"},		  // 1 ENGLISH
	#else
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	  {"20:00"},		  // 1 ENGLISH
	#endif
	#if Language_TraChinese
	  {"20:00"},		  // 1 ENGLISH
	#endif
	};

	BYTE code strHotKeyTimes_30[LANG_Nums][6]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"30:00"}, 			// 1 ENGLISH
	#else
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"30:00"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	{"30:00"}, 			// 1 ENGLISH
	#endif
  };
	BYTE code strHotKeyTimes_40[LANG_Nums][6]=	 //   S OK
	{//123456789012345678901234567890
	#if Language_English
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_France
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_German
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_SimChinese
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_Italian
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_Spanish
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_Portuguese
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_Turkish
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_Polish
		{"410:00"},			// 1 ENGLISH
	#endif
	#if Language_Dutch
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
		{"40:00"},			// 1 ENGLISH
	#else
		{"40:00"},			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
		{"40:00"},			// 1 ENGLISH
	#endif
	#if Language_TraChinese
		{"40:00"},			// 1 ENGLISH
	#endif
	  };
	BYTE code strHotKeyTimes_50[LANG_Nums][6]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_German
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Italian
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Spanish
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Turkish
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Polish
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Dutch
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"50:00"}, 			// 1 ENGLISH
	#else
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"50:00"}, 			// 1 ENGLISH
	#endif
	#if Language_TraChinese
	{"50:00"}, 			// 1 ENGLISH
	#endif
  };
	BYTE code strHotKeyTimes_60[LANG_Nums][6]=	 //   S OK
	{//123456789012345678901234567890
	#if Language_English
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_France
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_German
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_SimChinese
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_Italian
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_Spanish
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_Portuguese
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_Turkish
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_Polish
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_Dutch
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
		{"60:00"},			// 1 ENGLISH
	#else
		{"60:00"},			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
		{"60:00"},			// 1 ENGLISH
	#endif
	#if Language_TraChinese
		{"60:00"},			// 1 ENGLISH
	#endif
	  };
BYTE code strHotKeyTimes_90[LANG_Nums][6]=	 //   S OK
	{//123456789012345678901234567890
	#if Language_English
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_France
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_German
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_SimChinese
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_Italian
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_Spanish
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_Portuguese
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_Turkish
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_Polish
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_Dutch
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
		{"90:00"},			// 1 ENGLISH
	#else
		{"90:00"},			// 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
		{"90:00"},			// 1 ENGLISH
	#endif
	#if Language_TraChinese
		{"90:00"},			// 1 ENGLISH
	#endif
	  };

	  
BYTE code strHotKeyShowTimes[LANG_Nums][2]=   //	S OK
  {
	#if Language_English
			 {":"},		  // 1 ENGLISH
	#endif
	#if Language_France
		{":"},		  // 1 ENGLISH
	#endif
	#if Language_German
		 {":"},		  // 1 ENGLISH
	#endif
	#if Language_SimChinese
		 {":"},		  // 1 ENGLISH
	#endif
	#if Language_Italian
		 {":"},		  // 1 ENGLISH
	#endif
	#if Language_Spanish
		 {":"},		  // 1 ENGLISH
	#endif
	#if Language_Portuguese
		{":"},		  // 1 ENGLISH
	#endif
	#if Language_Turkish
		{":"},		  // 1 ENGLISH
	#endif
	#if Language_Polish
		{":"},		  // 1 ENGLISH
	#endif
	#if Language_Dutch
	   {":"},		  // 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
		 {":"},		  // 1 ENGLISH
	#else
		 {":"},		  // 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
		 {":"},		  // 1 ENGLISH
	#endif
	#if Language_TraChinese
		 {":"},		  // 1 ENGLISH
	#endif
		};

#endif

#if EN_HotKeyXINGZHUN


BYTE code strHotKeycj[LANG_Nums][9]=	//	S OK
	  {
	#if Language_English
				 {"GamePlus"}, 	  // 1 ENGLISH
	#endif
	#if Language_France
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_German
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_SimChinese
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_Italian
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_Spanish
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_Portuguese
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_Turkish
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_Polish
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_Dutch
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"GamePlus"},	 // 1 ENGLISH
	#else
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
	{"GamePlus"},	 // 1 ENGLISH
	#endif
	#if Language_TraChinese
	{"GamePlus"},	 // 1 ENGLISH
	#endif
			};

BYTE code strXINGZHUN[LANG_Nums][10]=   //	S OK
  {
	#if Language_English
			 {"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_France
		{"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_German
		 {"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_SimChinese
	     {"\xAA\x6B"},
	#endif
	#if Language_Italian
		 {"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_Spanish
		 {"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_Portuguese
		{"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_Turkish
		{"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_Polish
		{"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_Dutch
	   {"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
		 {"CrossHair"},		  // 1 ENGLISH
	#else
		 {"CrossHair"},		  // 1 ENGLISH
	#endif
	#endif
	#if Language_Korean
		 {"CrossHair"},		  // 1 ENGLISH
	#endif
	#if Language_TraChinese
		 {"CrossHair"},		  // 1 ENGLISH
	#endif
		};

#endif



BYTE code strHotKeyBrightness[LANG_Nums][12]=   //   S OK
{//123456789012345678901234567890
	#if Language_English
	{"BRIGHTNESS"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"LUMINOSIT\x8B"},			// 3 FRENCE
	#endif
	#if Language_German
       {"HELLIGKEIT"},
	#endif
	#if Language_SimChinese
	{"\x60 \x63"},			// 4 简体
	#endif
	#if Language_Italian
       {"LUMINOSIT\x98"},
	#endif
	#if Language_Spanish
       {"BRILLO"},
	#endif
	#if Language_Portuguese
       {"BRILHO"},
	#endif
	#if Language_Turkish
       {"PARLAKLIK"},
	#endif
	#if Language_Polish
       {"BRIGHTNESS"},
	#endif
	#if Language_Dutch
	{"BELDERHEID"},			// 2 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x80\x97\x91\x95\x98\x99\xA1"},		
	#else
	{"\x80\x71\x6B\x6F\x72\x73\x7D"},		
	#endif
	#endif
	#if Language_Korean
	{"\x60 \x61"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x60 \x63"}			// 5 繁体
	#endif
  };
//--------------------------
#if Enable_Menu_LockOSD
BYTE code strOsdLockedText[LANG_Nums][25]=
{//123456789012345678901234567890
	#if Language_English
	{"OSD LOCKED"},
	#endif
	#if Language_France
	{"OSD VERROUILLé"},
	#endif
	#if Language_German
	{"MEN\xA7 GESPERRT"},
	#endif
	#if Language_SimChinese
	{"OSD LOCKED"},
	#endif
	#if Language_Italian
	{"OSD BLOCCATO"},
	#endif
	#if Language_Spanish
	{"OSD BLOQUEADO"},
	#endif
	#if Language_Portuguese
	{"OSD LOCKED"},
	#endif
	#if Language_Turkish
	{"OSD LOCKED"},
	#endif
	#if Language_Polish
	{"OSD LOCKED"},
	#endif
	#if Language_Dutch
	{"OSD LOCKED"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"MEH\x07F \x07EA\x087\x092OKNPOBAHO"},
	#else
	{"OSD LOCKED"},
	#endif
	#endif
	#if Language_Korean
	{"OSD LOCKED"},
	#endif

	#if Language_TraChinese
	,{"OSD LOCKED"},
	#endif
};

BYTE code strOsdUnLockedText[LANG_Nums][25]=
{//123456789012345678901234567890
	#if Language_English
	{"OSD UNLOCKED"},
	#endif
	#if Language_France
	{"OSD DéVERROUILLé"},
	#endif
	#if Language_German
	{"MEN\xA7 ENTSPERRT"},
	#endif
	#if Language_SimChinese
	{"OSD UNLOCKED"},
	#endif
	#if Language_Italian
	{"OSD SBLOCCATO"},
	#endif
	#if Language_Spanish
	{"OSD DESBLOQUEADO"},
	#endif
	#if Language_Portuguese
	{"OSD UNLOCKED"},
	#endif
	#if Language_Turkish
	{"OSD UNLOCKED"},
	#endif
	#if Language_Polish
	{"OSD UNLOCKED"},
	#endif
	#if Language_Dutch
	{"OSD UNLOCKED"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"MEH\x07F PA\x07EA\x087\x092OKNPOBAHO"},
	#else
	{"OSD UNLOCKED"},
	#endif
	#endif
	#if Language_Korean
	{"OSD UNLOCKED"},
	#endif

	#if Language_TraChinese
	,{"OSD UNLOCKED"},
	#endif
};
#endif

#if Enable_LockPowerKey
BYTE code strPowerLockedText[LANG_Nums][25]=
{//123456789012345678901234567890
	#if Language_English
	{"POWER LOCKED"},
	#endif
	#if Language_France
	{"POWER VERROUILLAGE"},
	#endif
	#if Language_German
	{"POWER GESPERRT"},
	#endif
	#if Language_SimChinese
	{"POWER LOCKED"},
	#endif
	#if Language_Italian
	{"POWER BLOCCATO"},
	#endif
	#if Language_Spanish
	{"POWER BLOQUEADO"},
	#endif
	#if Language_Portuguese
	{"POWER LOCKED"},
	#endif
	#if Language_Turkish
	{"POWER LOCKED"},
	#endif
	#if Language_Polish
	{"POWER LOCKED"},
	#endif
	#if Language_Dutch
	{"POWER LOCKED"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x096NTAHNE \x07EA\x087\x092OKNPOBAHO"},
	#else
	{"POWER LOCKED"},
	#endif
	#endif
	#if Language_Korean
	{"POWER LOCKED"},
	#endif

	#if Language_TraChinese
	,{"POWER LOCKED"},
	#endif
};

BYTE code strPowerUnLockedText[LANG_Nums][25]=
{//123456789012345678901234567890
	#if Language_English
	{"POWER UNLOCKED"},
	#endif
	#if Language_France
	{"POWER DEVERROUILLAGE"},
	#endif
	#if Language_German
	{"POWER ENTSPERRT"},
	#endif
	#if Language_SimChinese
	{"POWER UNLOCKED"},
	#endif
	#if Language_Italian
	{"POWER SBLOCCATO"},
	#endif
	#if Language_Spanish
	{"POWER DESBLOQUEADO"},
	#endif
	#if Language_Portuguese
	{"POWER UNLOCKED"},
	#endif
	#if Language_Turkish
	{"POWER UNLOCKED"},
	#endif
	#if Language_Polish
	{"POWER UNLOCKED"},
	#endif
	#if Language_Dutch
	{"POWER UNLOCKED"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x096NTAHNE PA\x07EA\x087\x092OKNPOBAHO"},
	#else
	{"POWER UNLOCKED"},
	#endif
	#endif
	#if Language_Korean
	{"POWER UNLOCKED"},
	#endif

	#if Language_TraChinese
	,{"POWER UNLOCKED"},
	#endif
};

#endif

//--------------------------
BYTE code strRst_AutoAdjM12[LANG_Nums][25]=
{//123456789012345678901234567890
	#if Language_English
	{"IMAGE AUTO ADJUST"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"RéGLAGE AUTO."},				// 3 FRENCE
	#endif
	#if Language_German
       {"BILD AUTO ADJUST"},
	#endif
	#if Language_SimChinese
	{"\x7F \x80 \x84 \x68 \x97 \x98"},			// 4 简体
	#endif
	#if Language_Italian
       {"REG.AUTOMATICA"},
	#endif
	#if Language_Spanish
       {"AUTO CONFIGURACION"},
	#endif
	#if Language_Portuguese
       {"AUTO REGULAR"},
	#endif
	#if Language_Turkish
       {"GORUNTU OTOMATIK"},
	#endif
	#if Language_Polish
       {"IMAGE AUTO ADJUST"},
	#endif
	#if Language_Dutch
	{"AUTO CONFIG"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x60\x88\x99\x95 \x94\x86\x98\x99\x97. \x8F\x8E\x95\x87\x97\x86\x8D."},		
	#else
	{"\x60\x62\x73\x6F \x6E\x60\x72\x73\x71 \x69\x68\x6F\x61\x71\x60\x67\x65\x6E\x69\x65"},		
	#endif
	#endif
	#if Language_Korean
	{"\x6C \x75 \x83 \x6F \x8D \x7E"}			 
	#endif

	#if Language_TraChinese
	,{"\x7F \x80 \x84 \x68 \x97 \x98"}		// 5 繁体
	#endif
};
BYTE code strRst_ColorAdjM12[LANG_Nums][26]=
{//123456789012345678901234567890
	#if Language_English
	{"COLOR AUTO ADJUST"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"RéGLAGE AUTO.COULEURS"},			// 3 FRENCE
	#endif
	#if Language_German
       {"COLOR AUTO ADJUST"},
	#endif
	#if Language_SimChinese
	{"\xA6 \x85 \x84 \x68 \x97 \x98"},			// 4 简体
	#endif
	#if Language_Italian
       {"REG.COLORE"},
	#endif
	#if Language_Spanish
       {"COLOR AUTO"},
	#endif
	#if Language_Portuguese
       {"COR AUTO"},
	#endif
	#if Language_Turkish
       {"RENK OTOMATIK"},
	#endif
	#if Language_Polish
       {"COLOR AUTO ADJUST"},
	#endif
	#if Language_Dutch
	{"AUTO KLEUR"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x60\x88\x99\x95 \x94\x86\x98\x99\x97. \x9D\x88\x8B\x99\x86"},		
	#else
	{"\x60\x62\x73\x6F \x6E\x60\x72\x73\x71 \x77\x62\x65\x73\x60"},		
	#endif
	#endif
	#if Language_Korean
	{"\x84 \x7C \x83 \x6F \x8D \x7E"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x85 \xA6 \x84 \x68 \x97 \x98"}			// 5 繁体
	#endif
};
BYTE code strRst_ResetM12[LANG_Nums][15]=
{//123456789012345678901234567890
	#if Language_English
	{"RESET"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"RéINITIALISER"},			// 3 FRENCE
	#endif
	#if Language_German
       {"WERKSEINSTELL."},   
	#endif
	#if Language_SimChinese
	{"\x9C \x7A"},			// 4 简体
	#endif
	#if Language_Italian
       {"RICHIAMO"},
	#endif
	#if Language_Spanish
       {"REINICIAR"},
	#endif
	#if Language_Portuguese
       {"REINICIAR"},
	#endif
	#if Language_Turkish
       {"SIFIRLA"},
	#endif
	#if Language_Polish
       {"RESET"},
	#endif
	#if Language_Dutch
	{"RESET"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x72\x87\x97\x95\x98"},	
	#else
	{"RESET"},		
	#endif
	#endif
	#if Language_Korean
	{"\x99 \x61 \x6C"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x9C \x7A"}			// 5 繁体
	#endif
};
//---------------------------
BYTE code strMisc_InputSourceM13[LANG_Nums][16]=
{//123456789012345678901234567890
	#if Language_English
	{"SIGNAL SOURCE"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"ENTREE"},			// 3 FRENCE
	#endif
	#if Language_German
       {"SIGNALQUELLE"},
	#endif
	#if Language_SimChinese
	{"\xA0 \xA1 \xA9"},			// 4 简体
	#endif
	#if Language_Italian
       {"INGRESSO"},
	#endif
	#if Language_Spanish
       {"SE\x09FAL"},
	#endif
	#if Language_Portuguese
       {"SINAL"},
	#endif
	#if Language_Turkish
       {"SINYAL"},
	#endif
	#if Language_Polish
       {"SIGNAL SOURCE"},
	#endif
	#if Language_Dutch
	{"SIGNAAL"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x72\x8F\x89\x94\x86\x92"},		
	#else
	{"\x72\x69\x63\x6E\x60\x6C"},		
	#endif
	#endif
	#if Language_Korean
	{"\x9D \x9E \x9F \xA0"}			 
	#endif
	
	#if Language_TraChinese
	,{"\xA0 \xA1 \x91 \x92"}			// 5 繁体
	#endif
};

BYTE code strInputType_VGA[LANG_Nums][15]=
{
#if Language_English
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_France
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_German
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_SimChinese
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_Italian
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif

#if Language_Spanish
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_Portuguese
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_Turkish
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_Polish
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_Dutch
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_Russian
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_Korean
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
#if Language_TraChinese
{"\x5B\x20\x20\x20 VGA\x20\x20\x20\x3E"},
#endif
};

BYTE code strInputType_DVI[LANG_Nums][15]=
{
#if Language_English
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_France
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_German
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_SimChinese
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Italian
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Spanish
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Portuguese
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Turkish
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Polish
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Dutch
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Russian
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_Korean
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
#if Language_TraChinese
{"\x5B\x20\x20\x20 DVI\x20\x20\x20\x3E"},
#endif
};

BYTE code strInputType_HDMI[LANG_Nums][15]=
{
#if Language_English
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_France
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_German
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_SimChinese
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Italian
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Spanish
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Portuguese
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Turkish
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Polish
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Dutch
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Russian
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_Korean
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
#if Language_TraChinese
{"\x5B\x20\x20 HDMI\x20\x20\x20\x3E"},
#endif
};

BYTE code strInputType_DP[LANG_Nums][15]=
{
#if Language_English
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_France
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_German
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_SimChinese
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Italian
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Spanish
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Portuguese
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Turkish
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Polish
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Dutch
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Russian
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_Korean
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
#if Language_TraChinese
{"\x5B\x20\x20\x20 DP\x20\x20\x20\x20\x3E"},
#endif
};

#if AudioFunc
BYTE code strMisc_MuteM13[LANG_Nums][10]=
{//123456789012345678901234567890
	#if Language_English
	{"MUTE"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"MUET"},			// 3 FRENCE
	#endif
	#if Language_German
       {"MUTE"},
	#endif
	#if Language_SimChinese
	{"\x9B \x9D"},			// 4 简体
	#endif
	#if Language_Italian
       {"MUTE"},
	#endif
	#if Language_Spanish
       {"MUTE"},
	#endif
	#if Language_Portuguese
       {"MUTE"},
	#endif
	#if Language_Turkish
       {"SESSIZ"},
	#endif
	#if Language_Polish
       {"MUTE"},
	#endif
	#if Language_Dutch
	{"MUTE"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x61\x8B\x8E \x8E\x88\x9A\x91\x86"},		
	#else
	{"MUTE"},			 
	#endif
	#endif
	#if Language_Korean
	{"\xA1 \x9F \xA2"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x9B \x9D"}			// 5 繁体
	#endif
};
BYTE code strMute_OnMode[LANG_Nums][15]=
{//123456789012345678901234567890
	#if Language_English
 	 {"\x5B     ON    \x3E"},
	#endif
	#if Language_France
	 {"\x5B  MARCHE   \x3E"},
	 #endif
	#if Language_German
        {"\x5B     ON    \x3E"},
	#endif
	#if Language_SimChinese
 	 {"\x5B   \x72  \x74    \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B     ON    \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B     ON    \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B     ON    \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B    ACIK   \x3E"},
	#endif
	#if Language_Polish
       {"\x5B     ON    \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B     ON    \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B    \x62\x91\x92\x2E   \x3E"},			
	#else
	{"\x5B     ON    \x3E"},			
	#endif
	#endif
	#if Language_Korean
 	 {"\x5B     \x72\x61    \x3E"}
	#endif
	 
	#if Language_TraChinese
 	 ,{"\x5B     ON    \x3E"},
	#endif
};

BYTE code strMute_OffMode[LANG_Nums][15]=
{//123456789012345678901234567890
	#if Language_English
	 {"\x5B    OFF    \x3E"},
	#endif
	#if Language_France
	 {"\x5B   ARRET   \x3E"},
	 #endif
	#if Language_German
        {"\x5B    AUS    \x3E"},
	#endif
	#if Language_SimChinese
	 {"\x5B   \x73  \x74    \x3E"},
	 #endif
	#if Language_Italian
       {"\x5B    OFF    \x3E"},
	#endif
	#if Language_Spanish
       {"\x5B    OFF    \x3E"},
	#endif
	#if Language_Portuguese
       {"\x5B    OFF    \x3E"},
	#endif
	#if Language_Turkish
       {"\x5B   KAPALI  \x3E"},
	#endif
	#if Language_Polish
       {"\x5B    OFF    \x3E"},
	#endif
	#if Language_Dutch
	 {"\x5B    OFF    \x3E"},
	 #endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x5B   \x62\xA3\x91\x92\x2E   \x3E"},			
	#else
	{"\x5B    OFF    \x3E"},			
	#endif
	#endif
	#if Language_Korean
 	 {"\x5B    \x73\x61     \x3E"}
	#endif
	 
	#if Language_TraChinese
	 ,{"\x5B    OFF    \x3E"},
	#endif
};
BYTE code strMisc_VolumeM13[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"VOLUME"}, 			// 1 ENGLISH
	#endif
	#if Language_France
	{"LE VOLUME"},			// 3 FRENCE
	#endif
	#if Language_German
       {"LAUTST\xAARKE"},
	#endif
	#if Language_SimChinese
	{"\x9D \x9E"},			// 4 简体
	#endif
	#if Language_Italian
       {"VOLUME"},
	#endif
	#if Language_Spanish
       {"VOLUMEN"},
	#endif
	#if Language_Portuguese
       {"VOLUME"},
	#endif
	#if Language_Turkish
       {"SES"},
	#endif
	#if Language_Polish
       {"VOLUME"},
	#endif
	#if Language_Dutch
	{"VOLUME"},			// 4 GERMAN
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"\x63\x97\x95\x93\x91\x95\x98\x99\xA1"},		
	#else
	{"VOLUME"},			
	#endif
	#endif
	#if Language_Korean
	{"\xA3 \xA4"}			 
	#endif
	
	#if Language_TraChinese
	,{"\x9D \x9E"}			// 5 繁体
	#endif
};
#if Audio_Switch
BYTE code strHDMIAUDIOText[LANG_Nums][12]=
{//123456789012345678901234567890
	#if Language_English
	{"AUDIO INPUT"},
	#endif
	#if Language_France
	{"ENTRéE AUDIO"},
	#endif
	#if Language_German
	{"AUDIOQUELLE"},
	#endif
	#if Language_SimChinese
	{"AUDIO INPUT"},
	#endif
	#if Language_Italian
	{"AUDIO INPUT"},
	#endif
	#if Language_Spanish
	{"AUDIO INPUT"},
	#endif
	#if Language_Portuguese
	{"AUDIO INPUT"},
	#endif
	#if Language_Turkish
	{"AUDIO INPUT"},
	#endif
	#if Language_Polish
	{"AUDIO INPUT"},
	#endif
	#if Language_Dutch
	{"AUDIO INPUT"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"AUDIO INPUT"},
	#else
	{"AUDIO INPUT"},
	#endif
	#endif
	#if Language_Korean
	{"AUDIO INPUT"},
	#endif
	
	#if Language_TraChinese
	,{"AUDIO INPUT"},
	#endif
};

BYTE code strLineIn[LANG_Nums][10]=
{//123456789012345678901234567890
	#if Language_English
	{"ANALOG "},
	#endif
	#if Language_France
	{"ANALOGIQUE "},
	#endif
	#if Language_German
	{"ANALOG "},
	#endif
	#if Language_SimChinese
	{"ANALOG "},
	#endif
	#if Language_Italian
	{"ANALOG "},
	#endif
	#if Language_Spanish
	{"ANALOG "},
	#endif
	#if Language_Portuguese
	{"ANALOG "},
	#endif
	#if Language_Turkish
	{"ANALOG "},
	#endif
	#if Language_Polish
	{"ANALOG "},
	#endif
	#if Language_Dutch
	{"ANALOG "},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"ANALOG "},
	#else
	{"ANALOG"},
	#endif
	#endif
	#if Language_Korean
	{"ANALOG "},
	#endif
	
	#if Language_TraChinese
	,{"ANALOG "},
	#endif
};

BYTE code strAudio_HDMI[LANG_Nums][10]=
{//123456789012345678901234567890
	#if Language_English
	{"DIGITAL"},
	#endif
	#if Language_France
	{"NUMéRIQUE"},
	#endif
	#if Language_German
	{"DIGITAL"},
	#endif
	#if Language_SimChinese
	{"DIGITAL"},
	#endif
	#if Language_Italian
	{"DIGITAL"},
	#endif
	#if Language_Spanish
	{"DIGITAL"},
	#endif
	#if Language_Portuguese
	{"DIGITAL"},
	#endif
	#if Language_Turkish
	{"DIGITAL"},
	#endif
	#if Language_Polish
	{"DIGITAL"},
	#endif
	#if Language_Dutch
	{"DIGITAL"},
	#endif
	#if Language_Russian
	#if Language_Russian_Lowercase
	{"DIGITAL"},
	#else
	{"DIGITAL"},
	#endif
	#endif
	#if Language_Korean
	{"DIGITAL"},
	#endif
	
	#if Language_TraChinese
	,{"DIGITAL"},
	#endif
};


#endif

#endif

#if ShowSourceType
BYTE code strShowInputType[Input_Nums][LANG_Nums][10]=
{
#if InputType_Analog1
{
#if Language_English
 {"\x20\x20 VGA\x20\x20\x20"},
#endif 
#if Language_France
 {"\x20\x20 VGA\x20\x20\x20"},
#endif
#if Language_German
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
 #if Language_SimChinese
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
 #if Language_Italian
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif

 #if Language_Spanish
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
 #if Language_Portuguese
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
 #if Language_Turkish
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
 #if Language_Polish
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
 #if Language_Dutch
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
#if Language_Russian
 {"\x20\x20 VGA\x20\x20\x20"},
 #endif
 #if Language_Korean
 {"\x20\xA5 \xA6 \xA7 \xA8\x20"}
 #endif
 #if Language_TraChinese
 ,{"\x20\xA5 \xA6 \xA7 \xA8\x20"}
 #endif

},
#endif 
#if InputType_Digital
{
#if Language_English
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_France
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_German
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_SimChinese
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_Italian
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_Spanish
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_Portuguese
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_Turkish
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_Polish
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_Dutch
 {"\x20\x20 DVI\x20\x20\x20"},
#endif
#if Language_Russian
 {"\x20\x20 DVI\x20\x20\x20"},
 #endif
 #if Language_Korean
 {"\x20 \xA9 \xAA \xAB \x20"}
 #endif
 #if Language_TraChinese
 ,{"\x20 \xA9 \xAA \xAB \x20"}
 #endif
},
#endif
#if InputType_HDMI
{
#if Language_English
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_France
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_German
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_SimChinese
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_Italian
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_Spanish
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_Portuguese
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_Turkish
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_Polish
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_Dutch
 {"\x20\x20 HDMI\x20\x20"},
#endif
#if Language_Russian
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_Korean
 {"\x20\x20 HDMI\x20\x20"},
 #endif
 #if Language_TraChinese
 ,{"\x20\x20 HDMI\x20\x20"},
 #endif
},
#endif
#if ENABLE_DP_INPUT
{
#if Language_English
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_France
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_German
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_SimChinese
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_Italian
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_Spanish
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_Portuguese
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_Turkish
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_Polish
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_Dutch
 {"\x20\x20\x20 DP\x20\x20\x20"},
#endif
#if Language_Russian
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_Korean
 {"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
 #if Language_TraChinese
 ,{"\x20\x20\x20 DP\x20\x20\x20"},
 #endif
},
#endif

};
#endif
//========================================================
BYTE code strNoSignal[LANG_Nums][16]=
{//123456789012345678901234567890
 #if Language_English
	{"NO SIGNAL"}, 			// 1 ENGLISH
#endif	
 #if Language_France
	{"PAS DE SIGNAL"},			// 3 FRENCE
 #endif	
#if Language_German
     {"KEIN SIGNAL"},
#endif
 #if Language_SimChinese
	{"\x9F \xA0 \xA1"},			// 4 简体
 #endif	
#if Language_Italian
      {"NESSUN SEGNALE"},
#endif
#if Language_Spanish
      {"SIN SE\x09FAL"},
#endif
 #if Language_Portuguese
    {"SEM SINAL"},
 #endif
#if Language_Turkish
   {"SINYAL YOK"},
#endif
 #if Language_Polish
    {"NO SIGNAL"},
 #endif
 #if Language_Dutch
   {"GEEN SIGNAAL"},			// 4 GERMAN
 #endif	
#if Language_Russian
   #if Language_Russian_Lowercase
   {"\x6E\x8B\x99 \x98\x8F\x89\x94\x86\x92\x86"},		
   #else
   {"\x6E\x65\x73 \x72\x69\x63\x6E\x60\x6C\x60"},		
   #endif
#endif
#if Language_Korean
   {"\xAE \xAF \xB0 \xA1"}
#endif
 
 #if Language_TraChinese	
	,{"\x9B \x9C \x9D"}			// 5 繁体
 #endif	
};
BYTE code strOutOfRange[LANG_Nums][21]=
{//123456789012345678901234567890
 #if Language_English
	{"OUT OF RANGE"}, 			// 1 ENGLISH
 #endif	
 #if Language_France
	{"HORS LIMITES"},			// 3 FRENCE
 #endif	
 #if Language_German
     {"OUT OF RANGE"},
#endif
 #if Language_SimChinese
	{"\xA2 \xA3"},			// 4 简体
 #endif	
#if Language_Italian
      {"FUORI SCALA"},
#endif
#if Language_Spanish
      {"FUERA DE RANGO"},
#endif
 #if Language_Portuguese
    {"FORA DE FAIXA"},
 #endif
#if Language_Turkish
   {"COZUNURLUK DISI"},
#endif
 #if Language_Polish
    {"OUT OF RANGE"},
 #endif
 #if Language_Dutch
	{"BUITEN BEREIK"},			// 4 GERMAN
 #endif	
#if Language_Russian
   #if Language_Russian_Lowercase
   {"\x6E\x8B \x96\x95\x8A\x8A\x97\x8B\x8D\x8F\x88\x86\x8B\x99\x98\xA2"},		
   #else
   {"OUT OF RANGE"},		
   #endif
#endif
#if Language_Korean
   {"\xAC \x77 \x99 \xAD"}
#endif
 
 #if Language_TraChinese	
	,{"\x9E \x9F"}			// 5 繁体
 #endif	
};




