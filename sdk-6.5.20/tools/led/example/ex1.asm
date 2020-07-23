; $Id: ex1.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
;Here is an example program for programming the LEDs on the 5605.
;This is about as simple as it gets.
;
;The situation is this:
;
;    26 ports (port 0-25) each have one LED.
;
;    The LED is dark if there has been no activity,
;    or is on if there has been either RX, TX, or Collision.
;
;    We shift out port 0 status first, port 25 last.
;
;
;-------------------------- start of program --------------------------

begin:
	ld	A,0		; start with port 0
	ld	(portnum),A

portloop:
	port	A		; specify which port we're dealing with
	pushst	RX
	pushst	TX
	tor			; RX | TX
	pushst	COLL
	tor			; RX | TX | COLL
	pack			; send to output buffer

	inc	(portnum)
	ld	A,(portnum)
	cmp	A,26
	jnz	portloop

	send	26		; send 26 LED pulses and halt
	

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
;This program is 23 bytes in length and uses one data storage location.
;This wasn't necessary, as the B register could have been used, but
;the point was to show some of the instructions.

