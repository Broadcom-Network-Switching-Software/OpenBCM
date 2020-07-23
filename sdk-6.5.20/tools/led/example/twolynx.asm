;
; $Id: twolynx.asm,v 1.3 2011/05/22 23:38:43 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the Lynx SDK (BCM95673K4S)
;
; To start it, use the following commands from BCM:
;
;	0:led load twolynx.hex
;	1:led load twolynx.hex
;	*:led start
;
; On the Dual Lynx SDK board, there are two Lynx chips (unit 0 and 1).
; Each is connected to its own scan chain of 8 bits long.
;
; The Dual Lynx SDK has 4 columns of 4 LEDs each, as shown below:
;
;      E1 E2 E3 E4
;      R1 R2 R3 R4
;      T1 T2 T3 T4
;      L1 L2 L3 L4
;
; There is one bit per LED with the following colors:
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	E1, R1, T1, L1, E2, R2, T2, L2
;
; Current implementation:
;	E1 reflects port 1 higig link enable
;	R1 reflects port 1 higig receive activity
;	T1 reflects port 1 higig transmit activity
;	L1 reflects port 1 higig link up
;

	port	1
	call	put
	port	0
	call	put

	send	8

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

TICKS		EQU	1
SECOND_TICKS	EQU	(30*TICKS)
TXRX_ALT_TICKS	EQU	(SECOND_TICKS/6)
TX_ALT_COUNT	equ	0xc0
RX_ALT_COUNT	equ	0xc1

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
