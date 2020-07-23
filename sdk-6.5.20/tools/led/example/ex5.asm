; $Id: ex5.asm,v 1.2 2011/04/12 09:05:29 sraj Exp $
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
;    26 ports (port 0-25) each have one LED.  The front panel
;    has an 8-position switch so the user can select which
;    status is displayed on the front panel.  The host processor
;    reads the state of the switch periodically and communicates
;    it to this processor in data location 0x7F.
;
;          0x7F=0: link 
;          0x7F=1: display collision activity
;          0x7F=2: display TX activity
;          0x7F=3: display RX activity
;          0x7F=4: duplex
;          0x7F=5: speed (1=full speed, 0=lower speed)
;          0x7F=6: flow control
;          0x7F=7: link aggregation
;
;    when link status is chosen, it has four different indications:
;
;          link disabled: LED is off
;          link enabled but down: LED blinks on approx 100 ms, off 900ms
;          link enabled and up: LED is on
;
;    link aggregation indicates which ports are part of a trunk group.
;    this information isn't known to the MACs, and so isn't directly
;    known to this processor.  instead, the host CPU sends a string
;    of 32 (well, 26 meaningful) bits to addresses 0x78 to 0x7B to
;    indicate which ports should have their LED set in this case.  port 0
;    corresponds to bit 0 of 0x78, port 1 is bit 1 of 0x78, etc.
;
;    The scan out order is port 25 to port 0.
;
;    Note that the speed indicator requires special handling because
;    the gig ports (ports 24 and 25) run at higher speed.
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
	cmp	A, 3		; 3 of 30 cycles = 10% duty cycle
	jnc	noblink
	inc	B		; B = 1
noblink:
	ld	(blink),B
	
	ld	A,25		; start with port 25
	ld	(portnum),A

	; later we need to scan a 32 bit trunkmap.  we store the
	; address in the form of byte & bit offset explicitly.
	sub	A,A
	ld	(mapbit),A	; bit 0
	ld	A,trunkmap
	ld	(mapbyte),A	; starting at byte "trunkmap"

portloop:
	port	A		; specify which port we're dealing with

	ld	A,(select)
	dec	A
	jc	sel0
	dec	A
	jc	sel1
	dec	A
	jc	sel2
	dec	A
	jc	sel3
	dec	A
	jc	sel4
	dec	A
	jc	sel5
	dec	A
	jc	sel6
	dec	A
	jc	sel7
	ld	B,ZERO
	jmp	simple		; turn off all LEDs

sel0:	; link status: complex
	pushst	LINKEN
	jnt	wrapup		; LED is off if disabled
	pushst	LINKUP
	jt	wrapup
	; port is enabled but the link is down: blink
	push	(blink)		; push (blink) bit 0 to status stack
	jmp	wrapup
sel1:	ld	B,COLL
	jmp	simple
sel2:	ld	B,TX
	jmp	simple
sel3:	ld	B,RX
	jmp	simple
sel4:	ld	B,DUPLEX
	jmp	simple
sel5:	; speed is a bit trickier
	ld	B,SPEED_C	; assume 100 Mbps
	ld	A,(portnum)
	cmp	A,24		; ports 24&25 are 1000 Mbps full speed
	jc	simple
	ld	B,SPEED_M	; inc B would work too
	jmp	simple
sel6:	ld	B,FLOW
	jmp	simple
sel7:	; link aggregation
	ld	A,(mapbyte)	; get byte address we're looking for
	ld	B,(mapbit)
	tst	(A),B		; cy = B'th bit of (A)
	push	CY		; push it on led status stack
	jmp	wrapup

simple:	; handle simple case where we test just one bit
	pushst	B		; test B'th status bit

wrapup: pack			; send bit to LED buffer

	; bump trunk map pointers (used only if select==7)
	inc	B
	cmp	B,8
	jc	bump
	inc	(mapbyte)
	sub	B,B
bump:	ld	(mapbit),B

	; bump port pointer -- note we are scanning 25 to 0
	dec	(portnum)
	jnc	portloop

	send	26		; send 26 LED pulses and halt


; data storage
blink		equ	0x70	; 1 if blink on, 0 if blink off

phase		equ	0x71	; phase within 30 Hz
				; should be initialized to 0 on reset

portnum		equ	0x72	; temp to hold which port we're working on

mapbyte		equ	0x73	; byte pointer to trunkmap
mapbit		equ	0x74	; bit  pointer to trunkmap

trunkmap	equ	0x78	; a bitmap of four bytes

select		equ	0x7F	; indicates which status we are displaying


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
;This program is 130 bytes in length, but it is fairly complicatd.

