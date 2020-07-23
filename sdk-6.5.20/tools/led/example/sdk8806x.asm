;
; $Id: led_mt2.asm,v 1.0 2016/10/30 13:00:00 srajesh Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
;
; This is the default program for the BCM8806x Montreal2 board.
; To start it, use the following commands from BCM:
;
;       led 0 load sdk8806x.hex
;       led 0 auto on
;       led 0 start
;
; The are 2 LEDs per port one for Link(Top) and one for activity(Bottom).
;
; Only one chip (16 LEDs) have been populated in the Montreal2 board.
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0xA0 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;
; When Montreal2 inband stats collection is running, the activity is not
; derived from TX and RX. The Software-calculated activity is kept in the
; RAM byte (0xA0 + portnum) in bits 1 & 2
;
TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

MIN_PORT        EQU     0
MAX_PORT        EQU     7
NUM_PORT        EQU     8    ;MAX_PORT+1, for comparing


;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
        ld      A,MIN_PORT
port_loop:
        port    A
        ld      (PORT_NUM),A

        call    link_status    ; Top LED for this port
        call    chk_activity   ; Bottom LED for this port

        ; Debug
        ;call led_on
        ;call led_off
        ; Debug


        ld      a,(PORT_NUM)
        inc     a
        cmp     a,NUM_PORT
        jnz     port_loop

        ; Update various timers
        inc     (TXRX_ALT_COUNT)

        send    16

;
;
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: (PORT_NUM)
;  Outputs: one bit sent to LED stream
;

;       Activity status LED update
chk_activity:
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,1
        push    CY    ; TX
        tst     b,2
        push    CY    ; RX

        tor
        pop

        jnc     led_off       ; Always off, No Activity

        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS
        jnz     led_on
        jmp     led_off       ; Fast blink

; Link status LED update
link_status:
        call    get_link
        jnc     led_off
        jmp     led_on

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


get_link_hw:
        port    a
        pushst  LINKUP
        pop
        ret

led_off:
        pushst  ZERO
        pack
        ret

led_on:
        pushst  ONE
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

