; $Id: ex2.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
;Here is an example program for programming the LEDs on the 5605.
;
;The situation is this:
;
;    26 ports (port 0-25) each have three LEDs.
;          led 0: RX
;          led 1: TX
;          led 2: COLL
;
;    The scan out order is RX, TX, COLL status for port 0, then
;    for port 1, ...
;
;
;-------------------------- start of program --------------------------

begin:
	ld	A,0		; start with port 0
	ld	(portnum),A

portloop:
	port	A		; specify which port we're dealing with
	pushst	RX
	pack			; send to output buffer
	pushst	TX
	pack			; send to output buffer
	pushst	COLL
	pack			; send to output buffer

	inc	(portnum)
	ld	A,(portnum)
	cmp	A,26
	jnz	portloop

	send	78		; send 26*3 LED pulses and halt
	

; data storage
portnum		equ	0x7F	; temp to hold which port we're working on


; symbolic labels
; this gives symbolic names to the various bits of the port status fields

RX		equ	0x0	; received packet
TX		equ	0x1	; transmitted packet
COLL		equ	0x2	; collision indicator
SPEED_C		equ	0x3	;  100 Mbps
SPEED_M		equ	0x4	; 1000 Mbps
DUPLEX		equ	0x5	; half/full duplex
FLOW		equ	0x6	; flow control capable
LINKUP		equ	0x7	; link down/up status
LINKEN		equ	0x8	; link disabled/enabled status

;-------------------------- end of program --------------------------
;
;This program is 24 bytes in length and is only slightly more complicated
;than Example #1.
