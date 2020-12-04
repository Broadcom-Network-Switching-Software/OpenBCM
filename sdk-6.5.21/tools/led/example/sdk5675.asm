;
; $Id: sdk5675.asm,v 1.3 2011/05/22 23:38:43 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; Adapted from sdk5670.asm
; February 27, 2004
;
; This is the default program for the 5675 SDK.
; To start it, use the following commands from BCM:
;
;	led load sdk5675.hex
;	led auto on
;	led start
;
; The BCM5675 SDK has 10 columns of 4 LEDs each as shown below:
;
;       E1 E2   E3 E4   E5 E6   E7 E8   U1 U5
;       L1 L2   L3 L4   L5 L6   L7 L8   U2 U6
;       T1 T2   T3 T4   T5 T6   T7 T8   U3 U7
;       R1 R2   R3 R4   R5 R6   R7 R8   U4 U8
;
; There is one bit per LED with the following colors:
;
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	E1, L1, T1, R1, E2, L2, T2, R2,
;	E3, L3, T3, R3, E4, L4, T4, R4,
;	E5, L5, T5, R5, E6, L6, T6, R6,
;	E7, L7, T7, R7, E8, L8, T8, R8,
;	U1, U2, U3, U4, U5, U6, U7, U8
;
; Current implementation:
;
;	E1 reflects port 1 higig link enable
;	L1 reflects port 1 higig link up
;	T1 reflects port 1 higig transmit activity
;	R1 reflects port 1 higig receive activity
;
;	U1 through U8 are user-defined LEDs and
;	just display a chasing LED pattern.
;

	ld	a,1		; Start with first column
up1:
	port	a

	pushst	LINKEN
	tinv
	pack
	pushst	LINKUP
	tinv
	pack
	pushst	TX
	tinv
	pack
	pushst	RX
	tinv
	pack

	inc	a		; Next column
	cmp	a,9
	jnz	up1

;
; Put out 8 more values to user LEDs.
; In this case it is a cycling pattern.
;

	inc	(0x7f)
	ld	a,(0x7f)
	and	a,7
	jnz	up2
	inc	(0x7e)
up2:

	ld	b,(0x7e)
	and	b,0x07
	sub	a,a
	stc
	bit	a,b
	ld	b,0x08
up3:
	ror	a
	push	cy
	tinv
	pack
	dec	b
	jnz	up3

	send	0x28

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
