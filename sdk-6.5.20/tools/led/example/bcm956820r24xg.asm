;
; $Id: bcm956820r24xg.asm,v 1.3 2011/05/22 23:38:42 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (24+4 port) BCM956820R24XG board.
; To start it, use the following commands from BCM:
;
;       led load bcm956820r24xg.hex
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
;       A01/L01, L02/A02, ..., L24/A24
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
MIN_XE_PORT     EQU     1
MAX_XE_PORT     EQU     24
NUM_XE_PORT     EQU     24

NUM_ALL_PORT    EQU     24

MIN_PORT        EQU     1
MAX_PORT        EQU     24
NUM_PORT        EQU     24

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
quad1:          ; 1 - 2
	ld	a, 1
        port    a
        call    link

	ld	a, 2
        port    a
        call    activity
        call    link

	ld	a, 1
        port    a
        call    activity

quad2:          ; 3 - 4
	ld	a, 4
        port    a
        call    link

	ld	a, 3
        port    a
        call    activity
        call    link

	ld	a, 4
        port    a
        call    activity

quad3:          ; 5 - 6
	ld	a, 5
        port    a
        call    link

	ld	a, 6
        port    a
        call    activity
        call    link

	ld	a, 5
        port    a
        call    activity

quad4:          ; 7 - 8
	ld	a, 7
        port    a
        call    link

	ld	a, 8
        port    a
        call    activity
        call    link

	ld	a, 7
        port    a
        call    activity
quad5:          ; 9 - 10
	ld	a, 9
        port    a
        call    link

	ld	a, 10
        port    a
        call    activity
        call    link

	ld	a, 9
        port    a
        call    activity

quad6:          ; 11 - 12
	ld	a, 12
        port    a
        call    link

	ld	a, 11
        port    a
        call    activity
        call    link

	ld	a, 12
        port    a
        call    activity

quad7:          ; 13 - 14
	ld	a, 13
        port    a
        call    link

	ld	a, 14
        port    a
        call    activity
        call    link

	ld	a, 13
        port    a
        call    activity

quad8:          ; 15 - 16
	ld	a, 15
        port    a
        call    link

	ld	a, 16
        port    a
        call    activity
        call    link

	ld	a, 15
        port    a
        call    activity
quad9:          ; 17 - 18
	ld	a, 17
        port    a
        call    link

	ld	a, 18
        port    a
        call    activity
        call    link

	ld	a, 17
        port    a
        call    activity

quad10:          ; 19 - 20
	ld	a, 20
        port    a
        call    link

	ld	a, 19
        port    a
        call    activity
        call    link

	ld	a, 20
        port    a
        call    activity

quad11:          ; 21 - 22
	ld	a, 21
        port    a
        call    link

	ld	a, 22
        port    a
        call    activity
        call    link

	ld	a, 21
        port    a
        call    activity

quad12:          ; 23 - 24
	ld	a, 23
        port    a
        call    link

	ld	a, 24
        port    a
        call    activity
        call    link

	ld	a, 23
        port    a
        call    activity

        ; Update various timers
        inc     (TXRX_ALT_COUNT)

        send    NUM_PORT * 2 

;
; link
;
;  Inputs: (PORT_NUM)
;  Outputs: one bit sent to LED stream
;

link:
        ;jmp     led_green       ;DEBUG ONLY
        call    get_link
        jnc     led_black
        jmp     led_green

;
; activity
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: (PORT_NUM)
;  Outputs: one bit sent to LED stream
;

activity:
        ;jmp     led_green       ;DEBUG ONLY
        call    get_link
        jnc     led_black
        port    a
        pushst  RX
        pushst  TX
        tor
        pop

        jnc     led_black       ; Always black Link up, No Activity

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
