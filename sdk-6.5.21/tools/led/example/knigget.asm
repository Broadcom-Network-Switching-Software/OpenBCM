;
; $Id: knigget.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
; This is the default program for the Black Knight.
; To start it, use the following commands from BCM,
; where unit 0 is the 5670:
;
;	0:led load knigget.hex
;	0:led auto on
;	0:led start
;
; The Black Knight has 2 columns of 4 LEDs each on the rear:
;
;       E1 E2
;       L1 L2
;       T1 T2
;       R1 R2
;
; There is one bit per LED with the following colors:
;
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	E1, L1, T1, R1, E2, L2, T2, R2
;
; Current implementation:
;
;	E1 reflects port 1 higig link enable
;	L1 reflects port 1 higig link up
;	T1 reflects port 1 higig transmit activity
;	R1 reflects port 1 higig receive activity
;

	port	3		; 5670 port 3 (right)
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

	port	6		; 5670 port 6 (left)
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

	send	8

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
