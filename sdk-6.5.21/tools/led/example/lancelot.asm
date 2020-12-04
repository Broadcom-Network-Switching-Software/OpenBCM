;
; $Id: lancelot.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
; 
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the Lancelot (SDK95690R48S).
;
; To start it, use the following commands from BCM:
;
;	0:led load lancelot.hex
;	0:led start
;
; On Lancelot, unit 0 is the 5670 and is connected to 16 LEDs.
; Units 1 through 4 are 5690s and have no LEDs or LED program.
; 5670 ports 1, 2, 7, and 8 go to the 5690s, and have no LEDs.
;
; Lancelot has 4 columns of 4 LEDs each in the rear, as shown below:
;
;      E3 E4 E5 E6
;      R3 R4 R5 R6
;      T3 T4 T5 T6
;      L3 L4 L5 L6
;
; There is one bit per LED with the following colors:
;	ZERO		Green
;	ONE		Black
;
; Due to board layout, the bits are shifted out in the following order:
;	E5, R5, T5, L5, E6, R6, T6, L6, E3, R3, T3, L3, E4, R4, T4, L4
;
; Current implementation:
;	E3 reflects port 3 higig link enable
;	R3 reflects port 3 higig receive activity
;	T3 reflects port 3 higig transmit activity
;	L3 reflects port 3 higig link up
;

	port	5
	call	put
	port	6
	call	put
	port	3
	call	put
	port	4
	call	put

	send	16

put:
	pushst	LINKEN
	tinv
	pack
	pushst	RX
	tinv
	pack
	pushst	TX
	tinv
	pack
	pushst	LINKUP
	tinv
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
