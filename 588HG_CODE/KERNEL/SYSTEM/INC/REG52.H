/*--------------------------------------------------------------------------
REG52.H

Header file for generic 80C52 and 80C32 microcontroller.
Copyright (c) 1988-2001 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/
#ifndef _REG52_H
#define _REG52_H

#define ES1       0x01
#define RI1       0x01
#define TI1       0x02

/*  BYTE Registers  */
sfr P0    = 0x80;
sfr P1    = 0x90;
sfr P2    = 0xA0;
sfr P3    = 0xB0;
sfr P4    = 0xEB;  // 0xDA for I/O def
sfr PSW   = 0xD0;
sfr ACC   = 0xE0;
sfr B     = 0xF0;
sfr SP    = 0x81;
sfr DPL   = 0x82;
sfr DPH   = 0x83;
sfr PCON  = 0x87;
sfr TCON  = 0x88;
sfr TMOD  = 0x89;
sfr TL0   = 0x8A;
sfr TL1   = 0x8B;
sfr TH0   = 0x8C;
sfr TH1   = 0x8D;
sfr IE    = 0xA8;
sfr IEN1  = 0xB8;
sfr IEN2  = 0x9A;
sfr IP0   = 0xA9;
sfr IP1   = 0xB9;
sfr IP    = 0xB8;
sfr SCON  = 0x98;
sfr SBUF  = 0x99;
sfr S1CON = 0x9B;
sfr S1BUF = 0x9C;

/*  8052 Extensions  */
sfr T2CON  = 0xC8;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2    = 0xCC;
sfr TH2    = 0xCD;

sfr S0RELL = 0xAA;
sfr S0RELH = 0xBA;
sfr S1RELH = 0xBB;
sfr S1RELL = 0x9D;
sfr ADCON  = 0xD8;

//#if MSTMCU
sfr WDTCON = 0xD8;
sfr CKCON = 0x8E;
//#endif MSTMCU

/*  BIT Registers  */
/*  PSW  */
sbit CY    = PSW ^ 7;
sbit AC    = PSW ^ 6;
sbit F0    = PSW ^ 5;
sbit RS1   = PSW ^ 4;
sbit RS0   = PSW ^ 3;
sbit OV    = PSW ^ 2;
sbit P     = PSW ^ 0; //8052 only

/*  TCON  */
sbit TF1   = TCON ^ 7;
sbit TR1   = TCON ^ 6;
sbit TF0   = TCON ^ 5;
sbit TR0   = TCON ^ 4;
sbit IE1   = TCON ^ 3;
sbit IT1   = TCON ^ 2;
sbit IE0   = TCON ^ 1;
sbit IT0   = TCON ^ 0;

/*  IE  */
sbit EA    = IE ^ 7;
sbit ET2   = IE ^ 5; //8052 only
sbit ES    = IE ^ 4;
sbit ET1   = IE ^ 3;
sbit EX1   = IE ^ 2;
sbit ET0   = IE ^ 1;
sbit EX0   = IE ^ 0;

/*  IP  */
sbit PT2   = IP ^ 5;
sbit PS    = IP ^ 4;
sbit PT1   = IP ^ 3;
sbit PX1   = IP ^ 2;
sbit PT0   = IP ^ 1;
sbit PX0   = IP ^ 0;

/*  P3  */
sbit RD    = P3 ^ 7;
sbit WR    = P3 ^ 6;
sbit T1    = P3 ^ 5;
sbit T0    = P3 ^ 4;
sbit INT1  = P3 ^ 3;
sbit INT0  = P3 ^ 2;
sbit TXD   = P3 ^ 1;
sbit RXD   = P3 ^ 0;

/*  SCON  */
sbit SM0   = SCON ^ 7;
sbit SM1   = SCON ^ 6;
sbit SM2   = SCON ^ 5;
sbit REN   = SCON ^ 4;
sbit TB8   = SCON ^ 3;
sbit RB8   = SCON ^ 2;
sbit TI    = SCON ^ 1;
sbit RI    = SCON ^ 0;

/*  P1  */
sbit T2EX  = P1 ^ 1; // 8052 only
sbit T2    = P1 ^ 0; // 8052 only

/*  T2CON  */
sbit TF2    = T2CON ^ 7;
sbit EXF2   = T2CON ^ 6;
sbit RCLK   = T2CON ^ 5;
sbit TCLK   = T2CON ^ 4;
sbit EXEN2  = T2CON ^ 3;
sbit TR2    = T2CON ^ 2;
sbit C_T2   = T2CON ^ 1;
sbit CP_RL2 = T2CON ^ 0;


sbit P0_0   = P0 ^ 0;
sbit P0_1   = P0 ^ 1;
sbit P0_2   = P0 ^ 2;
sbit P0_3   = P0 ^ 3;
sbit P0_4   = P0 ^ 4;
sbit P0_5   = P0 ^ 5;
sbit P0_6   = P0 ^ 6;
sbit P0_7   = P0 ^ 7;

sbit P1_0   = P1 ^ 0;
sbit P1_1   = P1 ^ 1;
sbit P1_2   = P1 ^ 2;
sbit P1_3   = P1 ^ 3;
sbit P1_4   = P1 ^ 4;
sbit P1_5   = P1 ^ 5;
sbit P1_6   = P1 ^ 6;
sbit P1_7   = P1 ^ 7;

sbit P2_0   = P2 ^ 0;
sbit P2_1   = P2 ^ 1;
sbit P2_2   = P2 ^ 2;
sbit P2_3   = P2 ^ 3;
sbit P2_4   = P2 ^ 4;
sbit P2_5   = P2 ^ 5;
sbit P2_6   = P2 ^ 6;
sbit P2_7   = P2 ^ 7;

sbit P3_0   = P3 ^ 0;
sbit P3_1   = P3 ^ 1;
sbit P3_2   = P3 ^ 2;
sbit P3_3   = P3 ^ 3;
sbit P3_4   = P3 ^ 4;
sbit P3_5   = P3 ^ 5;
sbit P3_6   = P3 ^ 6;
sbit P3_7   = P3 ^ 7;

//======================================================================
// For Winbond MCU
sfr CHPENR = 0xF6;//for Winbond Mcu to the Isp mode
sfr CHPCON = 0xBF;//for Winbond Mcu to the Isp mode

#endif
