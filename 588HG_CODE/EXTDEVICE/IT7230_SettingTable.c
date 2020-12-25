// ****************************************************************************
// IT7230_SettingTable.c
// Create date  :   20120220
// Modifications:   20120220
// Version      :   IT7230_0004
// Definitions  : 
// Copyright (c) 2009 ITE Tech. Inc. All Rights Reserved.
// ****************************************************************************
#include "IT7230_TouchKey.h"

//This initial table depends on each PCB board,Please make this table form ITE.
//PCB model name:XXX-XXX-XXX
const sInitCapSReg code asInitCapSReg[] = 
{
#if 1 // 120411 coding test
{ PAGE_1,  CAPS_PCR     ,0x0001},  
{ PAGE_1,  CAPS_PSR     ,0x0001},  
{ PAGE_1,  CAPS_PMR      ,0x0008},
{ PAGE_1,  CAPS_RTR      ,0x007F},
{ PAGE_1,  CAPS_CTR      ,0x003F},
{ PAGE_1,  CAPS_CRMR     ,0x0020},
{ PAGE_1,  CAPS_PDR      ,0x0200},//0x0200 -->0x0100//modify c.c.lin 20120106 ==> adjust power key's sensitivity
{ PAGE_1,  CAPS_DR       ,0x0050},
{ PAGE_1,  CAPS_S0CR    ,0xC073},
{ PAGE_1,  CAPS_C3COR   ,0x68C0},
{ PAGE_1,  CAPS_C7COR   ,0x40C5},
{ PAGE_1,  CAPS_ICR0     ,0xFFBF},
{ PAGE_1,  CAPS_ICR1     ,0x0FFF},
{ PAGE_1,  CAPS_COER0    ,0xFFFF},
{ PAGE_1,  CAPS_COER1    ,0x03FF},
{ PAGE_1,  CAPS_CGCR     ,0x0001},
{ PAGE_1,  CAPS_LEDBR   ,0x0000},
{ PAGE_1,  CAPS_GPIODR   ,0x0000},
{ PAGE_1,  CAPS_GPIOOR   ,0x0000},
{ PAGE_1,  CAPS_GPIOMR   ,0x0000},
{ PAGE_1,  CAPS_GPIOLR   ,0x0000},
{ PAGE_1,  CAPS_GPIOER   ,0x0000},
{ PAGE_1,  CAPS_LEDCMR0  ,0xDDDD},
{ PAGE_1,  CAPS_LEDCMR1  ,0xDDDD},
{ PAGE_1,  CAPS_LEDCMR2  ,0xDDDD},
{ PAGE_1,  CAPS_LEDCMR3  ,0x0DDD},
{ PAGE_1,  CAPS_LEDRPR   ,0x3030},
{ PAGE_1,  CAPS_LEDBR    ,0x001F},
{ PAGE_1,  CAPS_LEDCGCR  ,0x0000},
{ PAGE_1,  CAPS_LEDPR0   ,0x3333},
{ PAGE_1,  CAPS_LEDPR1   ,0x3333},
{ PAGE_1,  CAPS_LEDPR2   ,0x3333},
{ PAGE_1,  CAPS_LEDPR3   ,0x0333},
{ PAGE_1,  CAPS_GPIOMSR  ,0x0000},
{ PAGE_0,  CAPS_S0DLR   ,0x8000},
{ PAGE_0,  CAPS_S0OHCR  ,0x0400},//0x0400 --> 0x200//modify c.c.lin 20120106 ==> main adjust power key's sensitivity, least 200
{ PAGE_0,  CAPS_S0OLCR  ,0x7000},
{ PAGE_0,  CAPS_S0SR    ,0xCC88},
{ PAGE_0,  CAPS_SXCHAIER ,0x0001},
{ PAGE_0,  CAPS_SXCHRIER ,0x0001},
{ PAGE_0,  CAPS_SXCLAIER ,0x0001},
{ PAGE_0,  CAPS_SXCLRIER ,0x0001},
{ PAGE_1,  CAPS_GPIONPCR     ,0x1FFF},
{ PAGE_1,  CAPS_CFER     ,0xC000},
{ PAGE_1,  CAPS_PCR      ,0x0C06}
#else
    { PAGE_1,  CAPS_PCR         ,0x0001},
    { PAGE_1,  CAPS_PSR         ,0x0001},  
	{ PAGE_1,  CAPS_PMR         ,0x0000},
	{ PAGE_1,  CAPS_RTR         ,0x007F},	
	{ PAGE_1,  CAPS_CTR         ,0x003F},
	{ PAGE_1,  CAPS_CRMR        ,0x0020},
	{ PAGE_1,  CAPS_PDR         ,0x0150},
	{ PAGE_1,  CAPS_DR          ,0x0050},
    { PAGE_1,  CAPS_I2CODCR     ,0x0000},
    { PAGE_1,  CAPS_IODCR       ,0x0000},
    { PAGE_1,  CAPS_PDCR        ,0x0000},
        
	{ PAGE_1,  CAPS_S0CR        ,0x8012},
	{ PAGE_1,  CAPS_S1CR        ,0x80B2},
	{ PAGE_1,  CAPS_S2CR        ,0x8000},
	{ PAGE_1,  CAPS_S3CR        ,0x8092},
	{ PAGE_1,  CAPS_S4CR        ,0x8072},
	{ PAGE_1,  CAPS_S5CR        ,0x8042},
	{ PAGE_1,  CAPS_S6CR        ,0x8032},
        
	{ PAGE_1,  CAPS_C0COR       ,0x7AEA},
    { PAGE_1,  CAPS_C1COR       ,0x7ABE},
    { PAGE_1,  CAPS_C2COR       ,0x7AEA},
    { PAGE_1,  CAPS_C3COR       ,0x7ABC},
    { PAGE_1,  CAPS_C4COR       ,0x7AC7},
    { PAGE_1,  CAPS_C7COR       ,0x7ACE},
    { PAGE_1,  CAPS_C9COR       ,0x7ACF},
	{ PAGE_1,  CAPS_C11COR      ,0x7AC8},
        
	{ PAGE_1,  CAPS_ICR0        ,0xFFEF},
	{ PAGE_1,  CAPS_ICR1        ,0x0FFF},
	{ PAGE_1,  CAPS_COER0       ,0xFFFF},
	{ PAGE_1,  CAPS_COER1       ,0x03FF},
	{ PAGE_1,  CAPS_CGCR        ,0x0001},
	{ PAGE_1,  CAPS_LEDBR       ,0x0000},
        
	{ PAGE_1,  CAPS_GPIODR      ,0x0000},
	{ PAGE_1,  CAPS_GPIOOR      ,0x0000},
	{ PAGE_1,  CAPS_GPIOMR      ,0x0000},
	{ PAGE_1,  CAPS_GPIOLR      ,0x7FFF},
	{ PAGE_1,  CAPS_GPIOER      ,0x0000},
        
	{ PAGE_1,  CAPS_LEDCMR0     ,0xFFFF},
	{ PAGE_1,  CAPS_LEDCMR1     ,0xFFFF},
	{ PAGE_1,  CAPS_LEDCMR2     ,0xFFFF},
	{ PAGE_1,  CAPS_LEDCMR3     ,0x0FFF},
        
	{ PAGE_1,  CAPS_LEDRPR      ,0x3030},	
	{ PAGE_1,  CAPS_LEDBR       ,0x001F},
	{ PAGE_1,  CAPS_LEDCGCR     ,0x0000},
	{ PAGE_1,  CAPS_LEDPR0      ,0x3333},
	{ PAGE_1,  CAPS_LEDPR1      ,0x3333},
	{ PAGE_1,  CAPS_LEDPR2      ,0x3333},
	{ PAGE_1,  CAPS_LEDPR3      ,0x0333},
	{ PAGE_1,  CAPS_GPIOMSR     ,0x7FFF}, 
	{ PAGE_0,  CAPS_S0DLR       ,0x8000},
	{ PAGE_0,  CAPS_S0OHCR      ,0x0500}, 
	{ PAGE_0,  CAPS_S0OLCR      ,0x7000},
	{ PAGE_0,  CAPS_S0SR        ,0xCC88},
	{ PAGE_0,  CAPS_S1DLR       ,0x8000},
	{ PAGE_0,  CAPS_S1OHCR      ,0x0500},
	{ PAGE_0,  CAPS_S1OLCR      ,0x7000},
	{ PAGE_0,  CAPS_S1SR        ,0xCC88},
	{ PAGE_0,  CAPS_S2DLR       ,0x8000},
	{ PAGE_0,  CAPS_S2OHCR      ,0x7000},
	{ PAGE_0,  CAPS_S2OLCR      ,0x7000},
	{ PAGE_0,  CAPS_S2SR        ,0xCF8F},
	{ PAGE_0,  CAPS_S3DLR       ,0x8000},
    { PAGE_0,  CAPS_S3OHCR      ,0x0500},
	{ PAGE_0,  CAPS_S3OLCR      ,0x7000},
	{ PAGE_0,  CAPS_S3SR        ,0xCC88},
	{ PAGE_0,  CAPS_S4DLR       ,0x8000},
    { PAGE_0,  CAPS_S4OHCR      ,0x0500},
	{ PAGE_0,  CAPS_S4OLCR      ,0x7000},
	{ PAGE_0,  CAPS_S4SR        ,0xCC88},
	{ PAGE_0,  CAPS_S5DLR       ,0x8000},
	{ PAGE_0,  CAPS_S5OHCR      ,0x0500},
	{ PAGE_0,  CAPS_S5OLCR      ,0x7000},
	{ PAGE_0,  CAPS_S5SR        ,0xCC88},
	{ PAGE_0,  CAPS_S6DLR       ,0x8000},
    { PAGE_0,  CAPS_S6OHCR      ,0x0500},
	{ PAGE_0,  CAPS_S6OLCR      ,0x7000},
	{ PAGE_0,  CAPS_S6SR        ,0xCC88},
	{ PAGE_0,  CAPS_SXCHAIER    ,0x007F},
	{ PAGE_0,  CAPS_SXCHRIER    ,0x007F},
	{ PAGE_0,  CAPS_SXCLAIER    ,0x0000},
	{ PAGE_0,  CAPS_SXCLRIER    ,0x0000},
	{ PAGE_0,  CAPS_SXCIER      ,0x0000},
	{ PAGE_1,  CAPS_GPIONPCR    ,0x1FFF},
	{ PAGE_1,  CAPS_CFER        ,0xC000},
	{ PAGE_1,  CAPS_PCR         ,0x6C06}
#endif    
};


//
