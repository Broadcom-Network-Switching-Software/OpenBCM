;
; $Id: guenevere5670.asm,v 1.3 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default 5670 program for the Guenevere (BCM95695P24SX).
;
; To start it, use the following commands from BCM,
; where unit 0 is the 5670 and units 3 and 4 are the 5673s:
;
;	0:led load guenevere5670.hex
;	0:led auto on
;	3:led load guenevere5673.hex
;	3:led auto on
;	4:led load guenevere5673.hex
;	4:led auto on
;	*:led start
;
; The 3 programs should be started all at once so that blinking
; is in sync across the 5673s.
;
; The Guenevere has 3 columns of 4 LEDs each as shown below:
;
;       5670 Unit 0    5673 Unit 3    5673 Unit 4
;       -------------  -------------  --------------
;       L04            E1             E1 
;       A04            R1             R1 
;       L05            T1             T1 
;       A05            L1             L1 
;
; This program runs only on the 5670 and controls only the leftmost
; column of LEDs.  The guenevere5673 program runs on each 5673.
;
; There is one bit per LED with the following colors:
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	L04, A04, L05, A05
;
; Current implementation:
;	L04 reflects port 4 higig (External Higig 0) link up
;	A04 reflects port 4 higig (External Higig 0) receive/transmit activity
;	L05 reflects port 5 higig (External Higig 1) link up
;	A05 reflects port 5 higig (External Higig 1) receive/transmit activity
;       E1 reflects port 1 xe link enable
;       R1 reflects port 1 xe receive activity
;       T1 reflects port 1 xe transmit activity
;       L1 reflects port 1 xe link up
;

	port	4		; 5670 port 4 (left)
	call	put

	port	5		; 5670 port 5 (right)
	call	put

	send	4

put:
	pushst	LINKUP
	tinv
	pack
	pushst	RX
	pushst	TX
	tor			; RX | TX
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
