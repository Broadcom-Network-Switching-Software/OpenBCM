;
; $Id: mirror.asm,v 1.3 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This LED program simply mirrors bits from the LED data area directly to
; the LED scan chain, effectively allowing the host CPU to write directly
; to the LEDs as memory-mapped bits.
;
;
; After this program is downloaded and starts executing, it increments
; the byte at address 0x80 once per frame.  This may help the host
; synchronize writes to frame updates, if needed.
;
; Next, the user program should write the length of the scan chain in
; bits to location 0x81.  This only needs to be done once.
;
; Next, the actual LED bits may be updated by writing to bytes to
; addresses 0x82 and above:
;
;	Byte 0x82 bits 7:0 are the first 8 bits on the scan chain
;	Byte 0x83 bits 7:0 are the second 8 bits on the scan chain
;	Etc.
;
; Remember that:
;	LED Data address 0x80 corresponds to PCI offset 0x1e00 (word write).
;	LED Data address 0x81 corresponds to PCI offset 0x1e04 (word write).
;	Etc.
;

BYTE_PTR	equ	0x7f

FRAME_COUNT	equ	0x80
BIT_COUNT	equ	0x81
BIT_DATA	equ	0x82

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
	inc	(FRAME_COUNT)

	ld	a,BIT_DATA
	ld	(BYTE_PTR),a

	ld	b,(BIT_COUNT)

loop:
	ld	a,(BYTE_PTR)
	inc	(BYTE_PTR)
	ld	a,(a)

	call	shift_bit
	call	shift_bit
	call	shift_bit
	call	shift_bit
	call	shift_bit
	call	shift_bit
	call	shift_bit
	call	shift_bit

	cmp	b,0
	jnz	loop

	ld	b,(BIT_COUNT)
	send	b

;
; shift_bit
;	A: current byte
;	B: number of bits remaining
;
; Never shifts more than 'B' bits.
;

shift_bit:
	cmp	b,0
	jnz	sb1
	ret
sb1:
	dec	b
	tst	a,7
	ror	a
	jnc	sb2
	pushst	ZERO
	pack
	ret
sb2:
	pushst	ONE
	pack
	ret

;
; Symbolic names for the bits of the port status fields
;

RX		equ	0x0	; received packet
TX		equ	0x1	; transmitted packet
COLL		equ	0x2	; collision indicator
SPEED_C		equ	0x3	; 100 Mbps
SPEED_M		equ	0x4	; 1000 Mbps
DUPLEX		equ	0x5	; half/full duplex
FLOW		equ	0x6	; flow control capable
LINKUP		equ	0x7	; link down/up status
LINKEN		equ	0x8	; link disabled/enabled status
ZERO		equ	0xE	; always 0
ONE		equ	0xF	; always 1
