;
; $Id: merlin5670.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default 5670 program for the Merlin (BCM95690R24S).
;
; To start it, use the following commands from BCM,
; where unit 0 is the 5670 and units 1 and 2 are the 5690s:
;
;	0:led load merlin5670.hex
;	1:led load merlin5690.hex
;	1:led auto on
;	2:led load merlin5690.hex
;	2:led auto on
;	*:led start
;
; The 3 programs should be started all at once so that blinking
; is in sync across the 5690s.
;
; The Merlin has 14 columns of 4 LEDs each as shown below:
;
;            5690 Unit 1              5690 Unit 2          5670 Unit 0
;       -----------------------  -----------------------  --------------
;       L01 L03 L05 L07 L09 L11  L01 L03 L05 L07 L09 L11     E1 E2
;       A01 A03 A05 A07 A09 A11  A01 A03 A05 A07 A09 A11     R1 R2
;       L02 L04 L06 L08 L10 L12  L02 L04 L06 L08 L10 L12     T1 T2
;       A02 A04 A06 A08 A10 A12  A02 A04 A06 A08 A10 A12     L1 L2
;
; The 5670 LEDs are on the back of the unit.
;
; This program runs only on the 5670 and controls only the rightmost
; two columns of LEDs.  The merlin5690 program runs on each 5690.
;
; There is one bit per LED with the following colors:
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	E1, L1, T1, R1, E2, L2, T2, R2
;
; Current implementation:
;	E1 reflects port 1 higig link enable
;	R1 reflects port 1 higig receive activity
;	T1 reflects port 1 higig transmit activity
;	L1 reflects port 1 higig link up
;

	port	3		; 5670 port 3 (right)
	call	put

	port	6		; 5670 port 6 (left)
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
