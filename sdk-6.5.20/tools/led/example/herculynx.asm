;
; $Id: herculynx.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the HercuLynx (BCM95673P8)
;
; To start it, use the following commands from BCM:
;
;	1:led load herculynx.hex
;	2:led load herculynx.hex
;	3:led load herculynx.hex
;	4:led load herculynx.hex
;	5:led load herculynx.hex
;	6:led load herculynx.hex
;	7:led load herculynx.hex
;	8:led load herculynx.hex
;	*:led start
;
; On the HercuLynx board, there are eight Lynx chips (unit 1 to unit 8).
; Each is connected to its own scan chain of 4 bits long.
;
; The HercuLynx board has 8 columns of 4 LEDs each, as shown below:
;
;      E1 E2 E3 E4 E5 E6 E7 E8
;      R1 R2 R3 R4 R5 R6 R7 R8
;      T1 T2 T3 T4 T5 T6 T7 T8
;      L1 L2 L3 L4 L5 L6 L7 L8
;
; There is one bit per LED with the following colors:
;	ZERO	Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	E1, R1, T1, L1
;
; Current implementation:
;	E1 reflects port 1 xe link enable
;	R1 reflects port 1 xe receive activity
;	T1 reflects port 1 xe transmit activity
;	L1 reflects port 1 xe link up
;

TICKS               EQU 1
SECOND_TICKS        EQU (30*TICKS)
TGIG_BLINK_TICKS	EQU	(SECOND_TICKS/10)

	ld	b,TGIG_BLINK_COUNT
	inc	(b)
	ld	a,(b)
	cmp	a,TGIG_BLINK_TICKS
	jc	up
	ld	(b),0
up:
	port	0
	call	put
	send	4

put:
	pushst	LINKEN
	tinv
	pack
	ld	a,(TGIG_BLINK_COUNT)
	cmp	a,TGIG_BLINK_TICKS/2
	jc	led_on
led_off:
    pushst  ONE
	pack
    pushst  ONE
	pack
	jmp	led_link
led_on:
	pushst	RX
	tinv
	pack
	pushst	TX
	tinv
	pack
led_link:
	pushst	LINKUP
	tinv
	pack
	ret

;
; Symbolic names for the bits of the port status fields
;

RX		    equ	0x0	; received packet
TX		    equ	0x1	; transmitted packet
COLL		equ	0x2	; collision indicator
SPEED_C     equ	0x3	; 100 Mbps
SPEED_M     equ	0x4	; 1000 Mbps
DUPLEX      equ	0x5	; half/full duplex
FLOW        equ	0x6	; flow control capable
LINKUP      equ	0x7	; link down/up status
LINKEN      equ	0x8	; link disabled/enabled status
ZERO        equ	0xE	; always 0
ONE         equ	0xF	; always 1
PORTDATA	equ	0x80	; Size 2 bytes
TGIG_BLINK_COUNT    equ	0xc0
