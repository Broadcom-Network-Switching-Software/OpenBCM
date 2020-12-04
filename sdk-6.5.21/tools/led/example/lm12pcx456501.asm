;
; $Id: lm12pcx456501.asm,v 1.6 2011/05/22 23:38:42 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the 12 port 56501 based CX4 LM
; To start it, use the following commands from BCM:
;
;       led load lm12pcx456504.hex
;       led auto on
;       led start
;
; There are 12 LEDS four each for 10G ports
; There is one bit per 10G LED with the following colors:
;       ZERO     Black
;       ONE      Green
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       L25,T25, R25, E25,  ..., L28,T28, R28, E28
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0x80 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;
; Current implementation:
;
; L25-L28 reflects 10G link status:
;       Black: no link
;       Green: Link
; T25-T28 reflects port 25-28 TX activity
;       Black: idle
;       Green: TX (pulse extended to 1/3 sec)
; R25-R28 reflects port 25-28 RX activity
;       Black: idle
;       Green: RX (pulse extended to 1/3 sec)
; E25-E28 reflects 10G link enable
;       Black: Disable
;       Green: Enable
;

TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)

MIN_GE_PORT     EQU     0
MAX_GE_PORT     EQU     23
NUM_GE_PORT     EQU     24

MIN_HG_PORT     EQU     24
MAX_HG_PORT     EQU     27
NUM_HG_PORT     EQU     4
NUM_ALL_PORT    EQU     28

MIN_PORT        EQU     0
MAX_PORT        EQU     27
NUM_PORT        EQU     28

; The TX/RX activity lights will be extended for ACT_EXT_TICKS
; so they will be more visible.

ACT_EXT_TICKS   EQU     (SECOND_TICKS/3)
GIG_ALT_TICKS   EQU     (SECOND_TICKS/2)
HD_OFF_TICKS    EQU     (SECOND_TICKS/20)
HD_ON_TICKS     EQU     (SECOND_TICKS-HD_ON_TICKS)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
        ; Put out 12 bits for 10G port
        ld      a, MIN_HG_PORT
up1_5:
        port    a

        ;       LINKUP  TX, RX, LINKEN
	pushst	LINKUP
        call    get_link
        jnc      link_down
	tinv
link_down:
	pack

	pushst	TX
	tinv
	pack

	pushst	RX
	tinv
	pack

	pushst	LINKEN
	tinv
	pack


        inc     a
        cmp     a,NUM_ALL_PORT
        jnz     up1_5

        send    12     ; 1 * 4 * 3 = 12

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
; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xe0
HD_COUNT        equ     0xe1
GIG_ALT_COUNT   equ     0xe2
PORT_NUM        equ     0xe3

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA        equ     0x80    ; Size 24 + 4? bytes

;
; LED extension timers
;

RX_TIMERS       equ     0xa0+0                  ; NUM_PORT bytes
TX_TIMERS       equ     0xa0+NUM_PORT           ; NUM_PORT bytes

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
