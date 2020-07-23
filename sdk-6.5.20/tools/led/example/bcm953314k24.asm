;
; $Id: bcm953314k24.asm,v 1.3 2011/05/22 23:38:42 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (24 port) BCM953314K reference board.
;
; To start it, use the following commands from BCM:
;
;       led load bcm953314k24.hex
;       led auto on
;       led start
;
; The are 2 LEDs per FE port one for Link(Left) and one for activity(Right).
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       A01, L01, A02, L02, ..., A24, L24
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0xa0 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;
; Current implementation:
;
; L01 reflects port 1 link status:
;       Black: no link
;       Amber: 10 Mb/s
;       Green: 100 Mb/s
;       Alternating green/amber: 1000 Mb/s
;       Very brief flashes of black at 1 Hz: half duplex
;       Longer periods of black: collisions in progress
;
; A01 reflects port 1 activity (even if port is down)
;       Black: idle
;       Green: RX (pulse extended to 1/3 sec)
;       Amber: TX (pulse extended to 1/3 sec, takes precedence over RX)
;       Green/Amber alternating at 6 Hz: both RX and TX
;

TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)

MIN_FE_PORT     EQU     1
MAX_FE_PORT     EQU     24
NUM_FE_PORT     EQU     24

NUM_PORT        EQU     24

; The TX/RX activity lights will be extended for ACT_EXT_TICKS
; so they will be more visible.

ACT_EXT_TICKS   EQU     (SECOND_TICKS/3)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
        ld      a, MIN_FE_PORT

up1:
        port    a
        ld      (PORT_NUM),a

        call    link_status     ; Left LED for this port
        call    activity        ; Right LED for this port

        ld      a,(PORT_NUM)
        inc     a
        cmp     a, MAX_FE_PORT+1
        jnz     up1

        ; Update various timers

        ld      b,TXRX_ALT_COUNT
        inc     (b)
        ld      a,(b)
        cmp     a,TXRX_ALT_TICKS
        jc      up2
        ld      (b),0
up2:

        send    48 ; 2 * 24

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
        pushst  RX
        pushst  TX
        tor
        pop
        jnc     act1

        ld      b,TXRX_TIMERS     ; Start TXRX LED extension timer
        add     b,(PORT_NUM)
        ld      a,ACT_EXT_TICKS
        ld      (b),a

act1:
        ld      b,TXRX_TIMERS     ; Check TXRX LED extension timer
        add     b,(PORT_NUM)

        dec     (b)
        jnc     act2            ; TXRX active?
        inc     (b)

        jmp     led_black       ; No activity

act2:                           ; Both TX and RX active
        ld      b,(TXRX_ALT_COUNT)
        cmp     b,TXRX_ALT_TICKS/2
        jc      led_green       ; Fast alternation of green/amber
        jmp     led_black

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
        ld      a,(PORT_NUM)
	    call    get_link
        jnc     led_black
        jmp     led_green

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
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,0
        ret

;
; led_black, led_amber, led_green
;
;  Inputs: None
;  Outputs: Two bits to the LED stream indicating color
;  Destroys: None
;

led_green:
        pushst  ZERO
        pack
        ret

led_black:
        pushst  ONE
        pack
        ret

;
; Variables (SDK software initializes LED memory from 0xa0-0xff to 0)
;

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA        equ     0xa0    ; Size 24 + 6? bytes

;
; LED extension timers
;

TXRX_ALT_COUNT  equ     0xe0
PORT_NUM        equ     0xe1
TXRX_TIMERS     equ     0xe2    ; NUM_PORT bytes C0 - DF

;
; Symbolic names for the bits of the port status fields
;

RX              equ     0x0     ; received packet
TX              equ     0x1     ; transmitted packet
COLL            equ     0x2     ; collision indicator
SPEED_C         equ     0x3     ; 100 Mbps
SPEED_M         equ     0x4     ; 1000 Mbps
DUPLEX          equ     0x5     ; half/full duplex
FLOW            equ     0x6     ; flow control capable
LINKUP          equ     0x7     ; link down/up status
LINKEN          equ     0x8     ; link disabled/enabled status
ZERO            equ     0xE     ; always 0
ONE             equ     0xF     ; always 1
