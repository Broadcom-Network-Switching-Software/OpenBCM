;
; $Id: sdk5690.asm,v 1.9 2012/03/02 14:34:03 yaronm Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the dual (24+2 port) BCM5690 SDKs.
; It applies to both the back-to-back higig and the external 10gig boards.
; To start it, use the following commands from BCM:
;
;	led load sdk5690.hex
;	led auto on
;	led start
;
; The BCM5690 SDK has 14 columns of 4 LEDs each as shown below:
;
;               Unit 0                  Unit 1            Unit 0  Unit 1
;       -----------------------  -----------------------  ------  ------
;       L01 L03 L05 L07 L09 L11  L01 L03 L05 L07 L09 L11   E13     E13
;       A01 A03 A05 A07 A09 A11  A01 A03 A05 A07 A09 A11   L13     L13
;       L02 L04 L06 L08 L10 L12  L02 L04 L06 L08 L10 L12   T13     T13
;       A02 A04 A06 A08 A10 A12  A02 A04 A06 A08 A10 A12   R13     R13
;
; There are 6 columns each for unit 0 and unit 1 gigabit ports.
; There is an additional 1 column each for unit 0 and unit 1 higig ports.
;
; There are two bits per gigabit ethernet LED with the following colors:
;	ZERO, ZERO	Black
;	ZERO, ONE	Green
;	ONE, ZERO	Amber
;
; There is one bit per higig LED with the following colors:
;	ZERO		Black
;	ONE		Amber
;
; Each chip drives only its own LEDs and needs to write them in the order:
;	A01, L01, ..., A12, L12, R13, T13, L13, E13
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0x80 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
; On the higig ports, LINKEN and LINKUP are used.
;
; Current implementation:
;
; L01 reflects port 1 link status:
;	Black: no link
;	Amber: 10 Mb/s
;	Green: 100 Mb/s
;	Alternating green/amber: 1000 Mb/s
;	Very brief flashes of black at 1 Hz: half duplex
;	Longer periods of black: collisions in progress
;
; A01 reflects port 1 activity (even if port is down)
;	Black: idle
;	Green: RX (pulse extended to 1/3 sec)
;	Amber: TX (pulse extended to 1/3 sec, takes precedence over RX)
;	Green/Amber alternating at 6 Hz: both RX and TX
;
; E13 reflects higig link enable
; L13 reflects higig link up
; T13 reflects higig transmit activity
; R13 reflects higig receive activity
;
; Note:
;	To provide consistent behavior, the gigabit LED patterns match
;	those in sdk5605.asm and the higig LEDs match those in sdk5670.asm.
;

TICKS		EQU	1
SECOND_TICKS	EQU	(30*TICKS)

MIN_GE_PORT	EQU	0
MAX_GE_PORT	EQU	11
NUM_GE_PORT	EQU	12

MIN_HG_PORT	EQU	12
MAX_HG_PORT	EQU	12
NUM_HG_PORT	EQU	1

MIN_PORT	EQU	0
MAX_PORT	EQU	12
NUM_PORT	EQU	13

; The TX/RX activity lights will be extended for ACT_EXT_TICKS
; so they will be more visible.

ACT_EXT_TICKS	EQU	(SECOND_TICKS/3)
GIG_ALT_TICKS	EQU	(SECOND_TICKS/2)
HD_OFF_TICKS	EQU	(SECOND_TICKS/20)
HD_ON_TICKS	EQU	(SECOND_TICKS-HD_ON_TICKS)
TXRX_ALT_TICKS	EQU	(SECOND_TICKS/6)

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
	sub	a,a		; ld a,MIN_GE_PORT (but only one instr)

up1:
	port	a
	ld	(PORT_NUM),a

	call	activity	; Upper LED for this port
	call	link_status	; Lower LED for this port

	ld	a,(PORT_NUM)
	inc	a
	cmp	a,NUM_GE_PORT
	jnz	up1

	; Put out 8 bits for higig port

	port	a		; 12
	ld	(PORT_NUM),a

	pushst	RX
	tinv
	pack
	pushst	TX
	tinv
	pack
	pushst	LINKUP
	tinv
	pack
	pushst	LINKEN		; For now, copy values directly to LEDs
	tinv
	pack

	pushst	ONE		; There are 4 more unused bits in the chain
	pack
	pushst	ONE
	pack
	pushst	ONE
	pack
	pushst	ONE
	pack

	; Update various timers

	ld	b,GIG_ALT_COUNT
	inc	(b)
	ld	a,(b)
	cmp	a,GIG_ALT_TICKS
	jc	up2
	ld	(b),0
up2:

	ld	b,HD_COUNT
	inc	(b)
	ld	a,(b)
	cmp	a,HD_ON_TICKS+HD_OFF_TICKS
	jc	up3
	ld	(b),0
up3:

	ld	b,TXRX_ALT_COUNT
	inc	(b)
	ld	a,(b)
	cmp	a,TXRX_ALT_TICKS
	jc	up4
	ld	(b),0
up4:

	send	56

;
; activity
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: (PORT_NUM)
;  Outputs: Two bits sent to LED stream
;

activity:
	pushst	RX
	pop
	jnc	act1

	ld	b,RX_TIMERS	; Start RX LED extension timer
	add	b,(PORT_NUM)
	ld	a,ACT_EXT_TICKS
	ld	(b),a

act1:
	pushst	TX
	pop
	jnc	act2

	ld	b,TX_TIMERS	; Start TX LED extension timer
	add	b,(PORT_NUM)
	ld	a,ACT_EXT_TICKS
	ld	(b),a

act2:
	ld	b,TX_TIMERS	; Check TX LED extension timer
	add	b,(PORT_NUM)

	dec	(b)
	jnc	act3		; TX active?
	inc	(b)

	ld	b,RX_TIMERS	; Check RX LED extension timer
	add	b,(PORT_NUM)

	dec	(b)		; Extend LED green if only RX active
	jnc	led_green
	inc	(b)

	jmp	led_black	; No activity

act3:				; TX is active, see if RX also active
	ld	b,RX_TIMERS
	add	b,(PORT_NUM)

	dec	(b)		; RX also active?
	jnc	act4
	inc	(b)

	jmp	led_amber	; Only TX active

act4:				; Both TX and RX active
	ld	b,(TXRX_ALT_COUNT)
	cmp	b,TXRX_ALT_TICKS/2
	jc	led_amber	; Fast alternation of green/amber
	jmp	led_green

;
; link_status
;
;  This routine calculates the link status LED for the current port.
;
;  Inputs: (PORT_NUM)
;  Outputs: Two bits sent to LED stream
;  Destroys: a, b
;

link_status:
	pushst	DUPLEX		; Skip blink code if full duplex
	pop
	jc	ls1

	pushst	COLL		; Check for collision in half duplex
	pop
	jc	led_black

	ld	a,(HD_COUNT)	; Provide blink for half duplex
	cmp	a,HD_OFF_TICKS
	jc	led_black

ls1:
	ld	a,(PORT_NUM)	; Check for link down
	call	get_link
	jnc	led_black

	pushst	SPEED_C		; Check for 100Mb speed
	pop
	jc	led_green

	pushst	SPEED_M		; Check for 10Mb (i.e. not 100 or 1000)
	pop
	jnc	led_amber

	ld	a,(GIG_ALT_COUNT)
	cmp	a,GIG_ALT_TICKS/2
	jc	led_amber

	jmp	led_green

;
; get_link
;
;  This routine finds the link status LED for a port.
;  Link info is in bit 0 of the byte read from PORTDATA[port]
;
;  Inputs: Port number in a
;  Outputs: Carry flag set if link is up, clear if link is down.
;  Destroys: a, b
;

get_link:
	ld	b,PORTDATA
	add	b,a
	ld	b,(b)
	tst	b,0
	ret

;
; led_black, led_amber, led_green
;
;  Inputs: None
;  Outputs: Two bits to the LED stream indicating color
;  Destroys: None
;

led_black:
	pushst	ZERO
	pack
	pushst	ZERO
	pack
	ret

led_green:
	pushst	ZERO
	pack
	pushst	ONE
	pack
	ret

led_amber:
	pushst	ONE
	pack
	pushst	ZERO
	pack
	ret

;
; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT	equ	0xc0
HD_COUNT	equ	0xc1
GIG_ALT_COUNT	equ	0xc2
PORT_NUM	equ	0xc3

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA	equ	0x80	; Size 12 bytes

;
; LED extension timers
;

RX_TIMERS	equ	0xa0+0			; NUM_PORT bytes
TX_TIMERS	equ	0xa0+NUM_PORT		; NUM_PORT bytes

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
