#ifndef _MSSXA_CONFIG_H
#define _MSSXA_CONFIG_H


#define _MSSXA_ENABLE				1

#define _MSSXA_UART_ADD_STR			"_MSSXA@"
#define _MSSXA_UART_TRI_STR			"@L@"
#define _MSSXA_UART_END_STR			"#"

#define _MSSXA_TOKEN_STRING			{ 0x05, 0x20, 0x13, 0x14 }
#define _MSSXA_TOKEN_STRING_LEN		4


#define MSSXA_MASK_ENABLE_			(0x40)
#define MSSXA_MASK_BLOCK_			(0x80)

#define MSSXA_MASK_ENTER_BP_WAIT_	(0x10)
#define MSSXA_MASK_ENTER_PBP_WAIT_	(0x20)
#define MSSXA_MASK_H_RESET_			(0x40)
#define MSSXA_MASK_RESET_			(0x80)
#define MSSXA_MASK_LAST_PBP_		(0x01)
#define MSSXA_MASK_ISR_READY_		(0x02)
#define MSSXA_MASK_ISR_READY_GO_	(0x04)
#define MSSXA_MASK_ENABLE_COMM_		(0x08)

#define MSSXA_ENABLE_(r)				r &= ~MSSXA_MASK_ENABLE_
#define MSSXA_DISABLE_(r)				r |= MSSXA_MASK_ENABLE_
#define MSSXA_BLOCK_(r)					r &= ~MSSXA_MASK_BLOCK_
#define MSSXA_RELEASE_(r)				r |= MSSXA_MASK_BLOCK_
#define MSSXA_SET_BP_I_(r, i)			do { r = ( i & 0x3F ) | ( r & ~0x3F ); } while( 0 )
#define MSSXA_IS_ENABLE_(r)				(0 == ( (MSSXA_MASK_ENABLE_) & r ))
#define MSSXA_IS_DISABLE_(r)			(0 != ( (MSSXA_MASK_ENABLE_) & r ))
#define MSSXA_IS_BLOKED_(r)				(0 == ( (MSSXA_MASK_BLOCK_) & r ))

#define MSSXA_SET_TO_LOOP_START_(r)		do { MSSXA_CLR_RESET_(r); MSSXA_SET_H_RESET(r); } while( 0 )
#define MSSXA_SET_H_RESET_(r)			r |= MSSXA_MASK_H_RESET_
#define MSSXA_CLR_H_RESET_(r)			r &= ~(MSSXA_MASK_H_RESET_)
#define MSSXA_SET_RESET_(r)				r |= MSSXA_MASK_RESET_
#define MSSXA_CLR_RESET_(r)				r &= ~(MSSXA_MASK_RESET_)
#define MSSXA_SET_ISR_READY_GO_(r)		r |= MSSXA_MASK_ISR_READY_GO_
#define MSSXA_SET_ISR_READY_(r)			r |= MSSXA_MASK_ISR_READY_
#define MSSXA_CLR_ISR_READY_GO_(r)		r &= ~(MSSXA_MASK_ISR_READY_GO_)
#define MSSXA_CLR_ISR_READY_(r)			r &= ~(MSSXA_MASK_ISR_READY_)
#define MSSXA_ENTER_PBP_WAIT_(r)		r &= ~(MSSXA_MASK_ENTER_PBP_WAIT_)
#define MSSXA_EXIT_PBP_WAIT_(r)			r |= MSSXA_MASK_ENTER_PBP_WAIT_
#define MSSXA_ENTER_BP_WAIT_(r)			r &= ~(MSSXA_MASK_ENTER_BP_WAIT_)
#define MSSXA_EXIT_BP_WAIT_(r)			r |= MSSXA_MASK_ENTER_BP_WAIT_
#define MSSXA_ENABLE_COMM_(r)			r &= ~(MSSXA_MASK_ENABLE_COMM_)
#define MSSXA_DISABLE_COMM_(r)			r |= MSSXA_MASK_ENABLE_COMM_
#define MSSXA_IS_DISABLE_COMM_(r)		(0 == ( (MSSXA_MASK_BLOCK_) & r ))
#define MSSXA_IS_OUT_BP_WAIT_(r)		(0 == ( (MSSXA_MASK_ENTER_BP_WAIT_) & r ))
#define MSSXA_IS_OUT_PBP_WAIT_(r)		(0 == ( (MSSXA_MASK_ENTER_PBP_WAIT_) & r ))
#define MSSXA_IS_ISR_READY_(r)			(0 != ( (MSSXA_MASK_ISR_READY_) & r )) 
#define MSSXA_IS_ISR_READY_GO_(r)		(0 != ( (MSSXA_MASK_ISR_READY_GO_) & r )) 
#define MSSXA_IS_RESET_ON_(r)			(0 != ( (MSSXA_MASK_RESET_) & r ))
#define MSSXA_IS_H_RESET_ON_(r)			(0 != ( (MSSXA_MASK_H_RESET_) & r ))


#define _MSSXA_NUM_E	( 64 / 8 )

#define _MSSXA_REF_OFFSET 		0
#define _MSSXA_OPT_OFFSET		( _MSSXA_REF_OFFSET + 1 )
#define _MSSXA_E_OFFSET			( _MSSXA_OPT_OFFSET + 1 )
#define _MSSXA_TOKEN_I_OFFSET	( _MSSXA_E_OFFSET + 1 * _MSSXA_NUM_E )
#define _MSSXA_COMMAND_OFFSET	( _MSSXA_TOKEN_I_OFFSET + 1 )
#define _MSSXA_DELAY1ms_OFFSET	( _MSSXA_COMMAND_OFFSET + 1 * 4 )
#define _MSSXA_DELAY_OFFSET		( _MSSXA_DELAY1ms_OFFSET + 1 )
#define _MSSXA_C_OFFSET			( _MSSXA_DELAY_OFFSET + 2 )
#define _MSSXA_META_SZ			( _MSSXA_C_OFFSET + 2 * _MSSXA_NUM_C + 2 )
#define _MSSXA_EXTRA_OFFSET		( _MSSXA_META_SZ - 2 )

#define MSSXA_USR_INFO()			do { printMsg("Nothing~"); } while( 0 )

#define _MSSXA_BP_ENTER_LOOP		0
#define _MSSXA_BP_LOOP_START		1
#define _MSSXA_BP_SetupInputPort	2

									
#endif

