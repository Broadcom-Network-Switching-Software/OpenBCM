;
; $Id: sdk56334.asm,v 1.3 2011/05/22 23:38:43 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (24 GE + 4 XE/HG port) BCM56334 SDKs.
; To start it, use the following commands from BCM:
;
;       led load sdk56334.hex
;       led auto on
;       led start
;
; Assume 2 LEDs per port one for Link(Left) and one for activity(Right).
;
; There is one bit per Ethernet LED with the following colors:
;       ONE 		Black
;       ZERO        Green
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       A05/L05,L06/A06,A07/L07,L08/A08,A01/L01,L02/A02,A03/L03,L04/A04
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0xA0 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;

MIN_GE_PORT     EQU      2

MAX_XE_PORT     EQU     29

NUM_PORT        EQU     28

; The TX/RX activity lights will be extended for ACT_EXT_TICKS
; so they will be more visible.
TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)
ACT_EXT_TICKS   EQU     (SECOND_TICKS/3)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

;
; Main Update Routine
;
;  This routine is called once per tick.
;
update:
        ld      a, MIN_GE_PORT

up1:
        port    a
        ld      (PORT_NUM),a

        call    link_status     ; Left LED for this port
        call    activity        ; Right LED for this port

        ld      a,(PORT_NUM)
        inc     a
        cmp     a, MAX_XE_PORT+1
        jnz     up1

        ; Update various timers

        ld      b,TXRX_ALT_COUNT
        inc     (b)
        ld      a,(b)
        cmp     a,TXRX_ALT_TICKS
        jc      up2
        ld      (b),0
up2:

        send    NUM_PORT*2

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
; led_black, led_green
;
;  Inputs: None
;  Outputs: one bit  to the LED stream indicating color
;  Destroys: None
;

led_black:
        pushst  ONE
        pack
        ret

led_green:
        pushst  ZERO
        pack
        ret

;
; Variables (SDK software initializes LED memory from 0xa0-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xe0
PORT_NUM        equ     0xe1
TXRX_TIMERS     equ     0xe2
;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

;Offset 0x1e00 - 0x80
;LED scan chain assembly area

;Offset 0x1e80 - 0xa0
PORTDATA        equ     0xa0    ; Size 48 + 1 + 4 bytes

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
