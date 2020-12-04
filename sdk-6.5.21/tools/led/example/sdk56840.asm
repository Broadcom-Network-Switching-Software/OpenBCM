;
; $Id: sdk56840.asm,v 1.7 2012/03/02 14:34:03 yaronm Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (36 port) BCM56840 SDKs.
; There are two LED processors on BCM56840. 
; To start the first one, use the following commands from BCM:
;
;       led load sdk56840.hex
;       led auto on
;       led start
; To start the second one:
;
;       led 1 load sdk56840.hex
;       led 1 auto on
;       led 1 start
;
; The are 2 LEDs per port one for Link and one for activity.
;
; There are two bits per LED with the following colors:
;       ZERO, ZERO      Black
;       ZERO, ONE       Green
;       ONE, ZERO       Amber
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       A01, L01, A02, L02, ..., A24, L24, A25, L25, ..., A35, L35
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
; L01 reflects port 1 link status:
;       Black: no link
;       Amber: below 10 Gb/s
;       Green: 10 Gb/s and above
;
; A01 reflects port 1 activity (even if port is down)
;       Black: idle
;       Green: RX (pulse extended to 1/3 sec)
;       Amber: TX (pulse extended to 1/3 sec, takes precedence over RX)
;       Green/Amber alternating at 6 Hz: both RX and TX
;
; Scan chain assembly area should start 0x4A and takes 32bytes

TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)

MIN_PORT        EQU     1
MAX_PORT        EQU     36
NUM_PORT        EQU     36

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
        ld      a,MIN_PORT     
up1:
        port    a
        ld      (PORT_NUM),a

        call    activity        ; Right LED for this port
        call    link_status     ; Left LED for this port


        ld      a,(PORT_NUM)
        inc     a
        cmp     a,NUM_PORT+1
        jnz     up1

        ; Update various timers

        ld      b,TXRX_ALT_COUNT
        inc     (b)
        ld      a,(b)
        cmp     a,TXRX_ALT_TICKS
        jc      up4
        ld      (b),0
up4:
        ; The LED test board requires shifting out 256 bits
        send    252     ; 2 * 2 * 36 + all zeros for remaining bytes

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
        ld      a,(PORT_NUM)    ; Check for link down
        call    get_rxact
        jnc     act1

        ld      b,RX_TIMERS     ; Start RX LED extension timer
        add     b,(PORT_NUM)
        ld      a,ACT_EXT_TICKS
        ld      (b),a

act1:
        ld      a,(PORT_NUM)    ; Check for link down
        call    get_txact
        jnc     act2       

        ld      b,TX_TIMERS     ; Start TX LED extension timer
        add     b,(PORT_NUM)
        ld      a,ACT_EXT_TICKS
        ld      (b),a

act2:
        ld      b,TX_TIMERS     ; Check TX LED extension timer
        add     b,(PORT_NUM)

        dec     (b)
        jnc     act3            ; TX active?
        inc     (b)

        ld      b,RX_TIMERS     ; Check RX LED extension timer
        add     b,(PORT_NUM)

        dec     (b)             ; Extend LED green if only RX active
        jnc     led_green
        inc     (b)

        jmp     led_black       ; No activity

act3:                           ; TX is active, see if RX also active
        ld      b,RX_TIMERS
        add     b,(PORT_NUM)

        dec     (b)             ; RX also active?
        jnc     act4
        inc     (b)

        jmp     led_amber       ; Only TX active

act4:                           ; Both TX and RX active
        ld      b,(TXRX_ALT_COUNT)
        cmp     b,TXRX_ALT_TICKS/2
        jc      led_amber       ; Fast alternation of green/amber
        jmp     led_green

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
         ld      a,(PORT_NUM)    ; Check for link down
         pushst LINKEN
         pop
         jnc led_black

;        ld      a,(PORT_NUM)    ; Check for link down
;        call    get_link
;        jnc     led_black

        ; below 10G amber, 10G and above green
        pushst  SPEED_C         ; Check for 100Mb speed
        pop
        jnc     led_amber

        ; both SPEED_C and SPEED_M set, 10G and above
        pushst  SPEED_M         ; Check for 10Mb (i.e. not 100 or 1000)
        pop
        jnc     led_amber
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
; get the tx activity status
;
get_txact:
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,4
        ret

;
; get the rx activity status
;
get_rxact:
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,5
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
; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xf0
PORT_NUM        equ     0xf3

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA        equ     0x80    ; Size 36? bytes

;
; LED extension timers
;

RX_TIMERS       equ     0xa4+0                  ; NUM_PORT bytes
TX_TIMERS       equ     0xc9           ; NUM_PORT bytes

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
