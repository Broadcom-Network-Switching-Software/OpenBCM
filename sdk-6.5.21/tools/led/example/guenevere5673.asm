;
; $Id: guenevere5673.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
; This is the default 5673 program for the Guenevere (BCM95695P24SX).
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
; This program runs on each 5673 and controls only the rightmost
; two columns of LEDs.  The guenevere5670 program runs on 5670.
;
; There is one bit per LED with the following colors:
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	E1, R1, T1, L1
;
; Current implementation:
;       E1 reflects port 1 xe link enable
;       R1 reflects port 1 xe receive activity
;       T1 reflects port 1 xe transmit activity
;       L1 reflects port 1 xe link up
;	L04 reflects port 4 higig (External Higig 0) link up
;	A04 reflects port 4 higig (External Higig 0) receive/transmit activity
;	L05 reflects port 5 higig (External Higig 1) link up
;	A05 reflects port 5 higig (External Higig 1) receive/transmit activity
;
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

RX                      equ	0x0	; received packet
TX                      equ	0x1	; transmitted packet
COLL		        equ	0x2	; collision indicator
SPEED_C                 equ	0x3	; 100 Mbps
SPEED_M                 equ	0x4	; 1000 Mbps
DUPLEX                  equ	0x5	; half/full duplex
FLOW                    equ	0x6	; flow control capable
LINKUP                  equ	0x7	; link down/up status
LINKEN                  equ	0x8	; link disabled/enabled status
ZERO                    equ	0xE	; always 0
ONE                     equ	0xF	; always 1
PORTDATA	        equ	0x80	; Size 2 bytes
TGIG_BLINK_COUNT        equ	0xc0
