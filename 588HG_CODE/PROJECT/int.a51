	extrn	data	(SystemFlags)
	extrn	code	(Power_TurnOffPanel)

	public	PowerOff

;CSEG	AT	03H
;	jmp	PowerOff
	
org	13h

PowerOff:
	push	ACC
	push	PSW
	call	Power_TurnOffPanel
;	mov	a,SystemFlags+1
;	orl	a,1
;	mov	SystemFlags+1,a
	pop	PSW
	pop	ACC
	RETI

END
