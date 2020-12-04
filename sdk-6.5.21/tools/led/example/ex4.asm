; $Id: ex4.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
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
;          led 2: speed
;		black if disabled
;		blinking amber if  10 Mbps but link down or disabled
;		solid on amber if  10 Mbps and link up
;		blinking green if 100 Mbps but link down or disabled
;		solid on green if 100 Mbps and link up
;                [ ports 24 and 25 are the gig ports and are slightly
;                  different; they are green for 1G, and amber for
;                  anything less, either 10Mbps or 100Mbps ].
;
;    Bicolor LEDs are really two LEDs with two inputs: with one LED
;    of the pair driven, the LED is green; with the other one driven,
;    the LED is red, with both driven, it is amber (yellow).
;
;    The scan out order is RX, TX, inp1, inp2 for port 0, then
;    the same for port 1, and so on through port 25.  Note that
;    ports 24 and 25 need slightly different handling than the
;    other ports.
;
;
;-------------------------- start of program --------------------------

begin:
	; update refresh phase within 1 Hz
	ld	A, phase
	inc	A		; next phase
	cmp	A, 30		; see if 1 sec has gone by
	jc	chkblink
	sub	A, A		; A = 0

chkblink:
	ld	(phase),A
	sub	B, B		; B = 0
	cmp	A, 15		; A already has the phase
	jnc	noblink
	inc	B		; B = 1
noblink:
	ld	(blink),B
	
	sub	A,A		; start with port 0
	ld	(portnum),A

portloop:
	port	A		; specify which port we're dealing with

	pushst	RX		; first put out RX status
	pack			; send to output buffer

	pushst	TX		; next put out RX status
	pack			; send to output buffer

	; here's the tricky part.  to encode bicolor LED speed status,
	; we do the following:
	;    inp1 inp2
	;     0    0    = off
	;     1    0    = green = full speed
	;     1    1    = amber = less than full speed
	;     0    1    = red   = not used

	pushst	LINKEN		; link enabled

	push	(blink)
	tand			; turn off LED if blinking
	pop			; cy = LINKEN & blink
	push	cy		; put it back
	pack			; inp1

	push	cy		; tos = LINKEN & blink
	ld	A,(portnum)
	cmp	A,24
	jnc	dogigs
	pushst	SPEED_C		; test for 100Mbps speed
	jmp	wrapup
; ports 24 and 25 are the gigabit ports
dogigs:
	pushst	SPEED_M		; test for 1000Mbps speed

wrapup:
	tinv			; make it a test for not full speed
	tand			; combine with LINKEN & blink
	pack			; inp2

	inc	(portnum)
	ld	A,(portnum)
	cmp	A,26
	jnz	portloop

	send	104		; send 26*4 LED pulses and halt


; data storage
blink		equ	0x7D	; 1 if blink on, 0 if blink off

phase		equ	0x7E	; phase within 30 Hz
				; should be initialized to 0 on reset

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
ZERO		equ	0xE	; always 0
ONE		equ	0xF	; always 1

;-------------------------- end of program --------------------------
;
;This program is 62 bytes in length, but it is fairly complicatd.

