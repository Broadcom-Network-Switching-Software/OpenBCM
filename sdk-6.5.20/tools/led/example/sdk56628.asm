;
; $Id: sdk56628.asm,v 1.9 2012/03/02 14:34:03 yaronm Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (1 GE + 8 XE/HG port) BCM56628 SDKs.
; To start it, use the following commands from BCM:
;
;       led load sdk56628.hex
;       led auto on
;       led start
;
; The is 1 LEDs per XE port. 
;
; There is one bit per 10 gigabit Ethernet LED with the following colors:
;       ONE 		Black
;       ZERO            Green
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       A01/L01, L02/A02, ..., L08/A08
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0xA0 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;
; Current implementation:
;
; L01/A01 reflects port 1 link status:
;       Black: no link
;       Green: Link/Activity
;
;
MAX_XE_PORT     EQU     31

NUM_PORT        EQU     8

; Device ports
;  2 xe0    
; 14 xe1     
; 26 xe2     
; 27 xe3     
; 28 xe4     
; 29 xe5     
; 30 xe6     
; 31 xe7     

PORT_XE0        EQU      2
PORT_XE1        EQU     14
PORT_XE2        EQU     26
PORT_XE3        EQU     27
PORT_XE4        EQU     28
PORT_XE5        EQU     29
PORT_XE6        EQU     30
PORT_XE7        EQU     31

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
	ld	a, PORT_XE0
        port    a
        call    link_activity        ; Off if no link

	ld	a, PORT_XE1
        port    a
        call    link_activity        ; Off if no link

	ld	a, PORT_XE2
        port    a
        call    link_activity        ; Off if no link

	ld	a, PORT_XE3
        port    a
        call    link_activity        ; Off if no link

	ld	a, PORT_XE4
        port    a
        call    link_activity        ; Off if no link

	ld	a, PORT_XE5
        port    a
        call    link_activity        ; Off if no link

	ld	a, PORT_XE6
        port    a
        call    link_activity        ; Off if no link

	ld	a, PORT_XE7
        port    a
        call    link_activity        ; Off if no link


        ; Update various timers
        inc     (TXRX_ALT_COUNT)

        send    NUM_PORT * 1 

;
; link_activity
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: (PORT_NUM)
;  Outputs: one bit sent to LED stream
;

link_activity:
        ;jmp     led_green       ;DEBUG ONLY
        call    get_link
        jnc     led_black
        port    a
        pushst  RX
        pushst  TX
        tor
        pop

        jnc     led_green       ; Always green Link up, No Activity

        ;jmp     led_green       ;DEBUG ONLY

        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS 
        jnz     led_green
        jmp     led_black

link_status:
        ;jmp     led_black       ;DEBUG ONLY
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
