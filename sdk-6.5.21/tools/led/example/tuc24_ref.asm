;
; $Id: tuc24_ref.asm,v 1.8 2012/03/02 14:34:03 yaronm Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the 5650 reference board (BCM95650R24)
;
; To start it, use the following commands from BCM:
;
;	0:led load tuc24_ref.hex
;	*:led start
;
;
; The BCM5650 reference board has 12 columns of 4 LEDs each, as shown below:
;
;       L02 L04 L06 L08 L10 L12 L14 L16 L18 L20 L22 L24
;       A02 A04 A06 A08 A10 A12 A14 A16 A18 A20 A22 A24
;       L01 L03 L05 L07 L09 L11 L13 L15 L17 L19 L21 L23
;       A01 A03 A05 A07 A09 A11 A13 A15 A17 A19 A21 A23
;
; For the FE ports, there are two bits per LED with the following colors:
;	ZERO, ZERO	Black
;	ZERO, ONE	Amber
;	ONE, ZERO	Green
;
; Also, there are 4 LEDs for the GE ports, as shown below:       
;       LG2 AG2   LG4 AG4
;       LG1 AG1   LG3 AG3
;
; For the GE ports, there is one bit per LED with the following colors:
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	A02, L02, A01, L01,..., A24, L24, A23, L23, LG1, AG1, ..., LG4, AG4
;
; Current implementation:
;
; L01 reflects port 1 link status
; A01 reflects port 1 activity
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not actually have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0x80 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
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
;
; Due to the different treatment and output ordering of the two types of
; ports, there are a number of complex transformations performed on the
; data.  In addition, the limited program space of the LED processor
; necessitates some optimizations for code length that make the following
; assembly somewhat unclear.  Comments have been added to clarify some of
; steps below.
;

MIN_FE_PORT	EQU	0
MAX_FE_PORT	EQU	23
NUM_FE_PORT	EQU	24

MIN_GE_PORT	EQU	24
MAX_GE_PORT	EQU	27
NUM_GE_PORT	EQU	4

MIN_PORT	EQU	0
MAX_PORT	EQU	27
NUM_PORT	EQU	28

PACK_SRC        EQU     0
PACK_DST        EQU     128
PACK_BITS       EQU     ((NUM_FE_PORT*4) + (NUM_GE_PORT*2))
PACK_FE_NUM     EQU     (NUM_FE_PORT/2)
PACK_NUM        EQU     (PACK_BITS/8)

; Instead of an enumeration, these values are the concatenation of the
; ONE/ZERO definitions at the bottom of the file.  This allows the
; "uncache" routine below to be more efficient.
        
CACHE_BLACK     EQU     0xff
CACHE_AMBER     EQU     0xfe
CACHE_GREEN     EQU     0xef
        
TICKS		EQU	1
SECOND_TICKS	EQU	(30*TICKS)

; The TX/RX activity lights will be extended for ACT_EXT_TICKS
; so they will be more visible.

ACT_EXT_TICKS	EQU	(SECOND_TICKS/3)
HD_OFF_TICKS	EQU	(SECOND_TICKS/20)
HD_ON_TICKS	EQU	(SECOND_TICKS-HD_ON_TICKS)
TXRX_ALT_TICKS	EQU	(SECOND_TICKS/6)

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
	sub	a,a		; ld a,MIN_FE_PORT (but only one instr)

up1:
	port	a
	ld	(PORT_NUM),a

        cmp     a,MIN_GE_PORT
        jc      up_fe
        
	call	link_status_ge	; First LED for this port
	call	activity_ge	; Second LED for this port

        jmp     next_port
        
up_fe:          
	call	activity	; Lower LED for this port
	call	link_status	; Upper LED for this port

        ; Copy from cache to pack location
        ld      a,(ACT_CACHE)
        call    uncache
        ld      a,(LINK_CACHE)
        call    uncache
        
next_port:     
	ld	a,(PORT_NUM)
	inc	a
	cmp	a,NUM_PORT
	jnz	up1

	; Update various timers

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
;
; This bit is a workaround for a bug in the 5665 LED processor.
; 
; The pack instruction should put the data into data mem at 0x80.
; However, the internal register to store this address is only 7 bits.
; So the data is packed starting at address 0.
; This leads to two caveats:    
; 
; 1) Port 0 data _MUST_ be consumed before the first pack instruction.
; 
; 2) The output data must be moved from location 0 to location 128.
; 
; 
    
                
        sub     b,b             ; ld b,PACK_SRC (but only one instr)

packflip:       
        ld      a,(b)
        ror     a
        ror     a
        ror     a
        ror     a
        and     a,0xf
        ld      (PACKFLIP_CACHE),a
        ld      a,(b)
        rol     a
        rol     a
        rol     a
        rol     a
        and     a,0xf0
        or      a,(PACKFLIP_CACHE)
        ld      (b),a
          
        inc     b      
        cmp     b,PACK_FE_NUM
        jnz     packflip
        
        sub     b,b             ; ld b,PACK_SRC (but only one instr)
        ld      a,PACK_DST
packmove:
        ld      (a),(b)
        inc     a
        inc     b
        cmp     b,PACK_NUM
        jnz     packmove

        send    PACK_BITS

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
        ld      a,ACT_CACHE
	jnc	led_green
	inc	(b)

	jmp	led_black	; No activity

act3:				; TX is active, see if RX also active
	ld	b,RX_TIMERS
	add	b,(PORT_NUM)

	dec	(b)		; RX also active?
	jnc	act4
	inc	(b)

        ld      a,ACT_CACHE
	jmp	led_amber	; Only TX active

act4:				; Both TX and RX active
	ld	b,(TXRX_ALT_COUNT)
	cmp	b,TXRX_ALT_TICKS/2
        ld      a,ACT_CACHE
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

	ld	a,(HD_COUNT)	; Provide blink for half duplex
	cmp	a,HD_OFF_TICKS
        ld      a,LINK_CACHE    ; Cache address for led setting
	jc	led_black

ls1:
	ld	a,(PORT_NUM)	; Check for link down
	call	get_link
        ld      a,LINK_CACHE    ; Cache address for led setting
	jnc	led_black

	pushst	COLL		; Check for collision
	pop
	jc	led_black

	pushst	SPEED_C		; Check for 100Mb speed
	pop
	jc	led_green       

	pushst	SPEED_M		; Check for 10Mb (i.e. not 100 or 1000)
	pop
	jnc	led_amber


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
;  Inputs: Cache address in a
;  Outputs: byte encoding two bits for color
;  Destroys: None
;

led_black:
        ld (a),CACHE_BLACK
	ret

led_amber:
        ld (a),CACHE_AMBER
	ret

led_green:
        ld (a),CACHE_GREEN
	ret



; uncache
;
; Inputs: LED bit pair byte-encoded in a
; Outputs: Two bits to the LED stream indicating color
; Destroys: None
;

uncache:
        pushst  a
        pack
        ror     a
        ror     a
        ror     a
        ror     a
        pushst  a
        pack
        ret


; GE routines
;
; Inputs: LED bit pair in a
; Outputs: Two bits to the LED stream indicating color
; Destroys: None
;

; It is now safe to pack here, since the GE ports are beyond the pack area.


activity_ge:
        ld      a,ZERO          ; On (ZERO)
        pushst  RX
        pop
        jc      act_led_set

        pushst  TX
        pop
        jc      act_led_set

        inc     a               ; On (ZERO) -> Off (ONE)

act_led_set:
        pushst  a
        pack
        ret

link_status_ge:

	ld	a,(PORT_NUM)	; Check for link down
	ld	b,PORTDATA
	add	b,a
	ld	b,(b)
	tst	b,0

        ld      a,ONE           ; Off (ONE)
        jnc     ls_led_clear

        dec     a               ; Off (ONE) -> On (ZERO)

ls_led_clear:
        pushst  a
        pack
        ret


; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT	equ	0xff
HD_COUNT	equ	0xfe

PORT_NUM	equ	0xfc
LINK_CACHE      equ     0xfb
ACT_CACHE       equ     0xfa
PACKFLIP_CACHE  equ     0xf9

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA	equ	0xa0	; Size 28 bytes

;
; LED extension timers
;

RX_TIMERS	equ	0xc0+0			; NUM_PORT bytes
TX_TIMERS	equ	0xc0+NUM_PORT		; NUM_PORT bytes

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
