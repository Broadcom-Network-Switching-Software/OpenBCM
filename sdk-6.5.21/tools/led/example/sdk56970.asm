;
; $Id: sdk56970x.asm,v 1.0 2014/07/14 16:00:36 meghanak Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
;
; This is the default program for the BCM56970 SDKs.
; To start it, use the following commands from BCM:
;
;       led 0 load sdk56970x.hex
;       led 0 auto on
;       led 0 start
;
; The are 2 LEDs per port one for Link(Top) and one for activity(Bottom).
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       L01/A01, L02/A02, ..., L64/A64
;
; There are two bits per Ethernet LED for activity with the following colors:
;       ZERO, ZERO      Black
;       ZERO, ONE       Amber
;       ONE, ZERO       Green
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
; L01 reflects port 1 link status:
;       Black: no link
;       Green: Link
;
; A01 reflects port 1 activity status:
;       Black: no link
;       Green: Link
;       Amber: Activity
;
;
TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

MIN_PORT        EQU     0
MAX_PORT        EQU     63
NUM_PORT        EQU     64


; Assumptions
; LED status [TX/RX Activity] is organized to map 1:1 for indexing with
; link status data thru' CMIC_LEDUP*_PORT_ORDER_REMAP_* registers.
; Link status is injected by SDK from Linkscan task at PORTDATA offset

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
        ;call led_green
        ;call led_amber
        ; Debug


        ld      a,(PORT_NUM)
        inc     a
        cmp     a,NUM_PORT
        jnz     port_loop

        ; Update various timers
        inc     (TXRX_ALT_COUNT)

        send    252 ; 2 * 2 * 63

;
;
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: (PORT_NUM)
;  Outputs: two bit sent to LED stream
;

;       Activity status LED update
chk_activity:
        port    A
        pushst  RX
        pushst  TX
        tor
        pop

        jnc     led_black       ; Always black, No Activity

        ;jmp     led_green      ;DEBUG ONLY

        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS
        jnz     led_amber
        jmp     led_black       ; Fast alternation of black/amber

; Link status LED update
link_status:
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


get_link_hw:
        port    a
        pushst  LINKUP
        pop
        ret

;
; led_black, led_amber, led_green
;
;  Inputs: None
;  Outputs: Two bits to the LED stream indicating color
;  Destroys: None
;

led_black:
        pushst  ZERO
        pack
        pushst  ZERO
        pack
        ret

led_amber:
        pushst  ONE
        pack
        pushst  ZERO
        pack
        ret

led_green:
        pushst  ZERO
        pack
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

