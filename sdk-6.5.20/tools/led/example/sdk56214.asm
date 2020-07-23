;
; $Id: sdk56214.asm,v 1.6 2011/05/22 23:38:43 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (24+2+2 port) BCM956214P24
; To start it, use the following commands from BCM:
;
;       0:led load sdk56214.hex
;       0:led auto on
;       0:led start
;
; The are 2 signle color LEDs per GE port.
;
; There is one bits per LED with the following colors:
;       ZERO      Green
;       ONE       Black
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       L06, A06, L07, ..., L29, A29, L05, A05, L04, A04, L01, A01, L02, A02 
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
;       Green: link 
;
; A01 reflects port 1 activity (even if port is down)
;       Black: idle
;       Green: RX/TX (pulse extended to 1/3 sec)
;


MIN_HG_PORT     EQU     1 
MAX_HG_PORT     EQU     2 
NUM_HG_PORT     EQU     2

MIN_UL_PORT     EQU     4
MAX_UL_PORT     EQU     5
NUM_UL_PORT     EQU     2

MIN_GE_PORT     EQU     6 
MAX_GE_PORT     EQU     29 
NUM_GE_PORT     EQU     24

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
        
ge_up:
        ld      (PORT_NUM),a

        call    link_status
        call    activity

        ld      a,(PORT_NUM)
        inc     a
        cmp     a, MAX_GE_PORT+1
        jc      ge_up 

        ld      a, MAX_UL_PORT
ul_up:
        ld      (PORT_NUM),a

        call    link_status
        call    activity

        ld      a,(PORT_NUM)
        dec     a
        cmp     a, MIN_UL_PORT-1
        jnz     ul_up 

        ld      a, MAX_HG_PORT
hg_up:
        ld      (PORT_NUM),a

        call    link_status
        call    activity

        ld      a,(PORT_NUM)
        dec     a
        cmp     a, MIN_HG_PORT-1
        jnz     hg_up 

        inc     (TXRX_ALT_COUNT)
        send    NUM_PORT * 2 

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
        ld      a, (PORT_NUM)
        call    get_link
        jnc     led_black
        port    a
        pushst  RX
        pushst  TX
        tor
        pop

        jnc     led_black

        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS 
        jnz     led_green
        jmp     led_black

link_status:
        ld      a,(PORT_NUM)    ; Check for link down
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

led_black:
        pushst  ONE
        pack
        ret

led_green:
        pushst  ZERO
        pack
        ret

;
; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xe0
PORT_NUM        equ     0xe1

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA        equ     0xa0    ; Size 24 + 6 bytes

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
