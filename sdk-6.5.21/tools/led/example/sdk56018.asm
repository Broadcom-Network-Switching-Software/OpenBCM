;
; $Id: sdk56018.asm,v 1.6 2011/05/22 23:38:42 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (48+4+1 port) BCM56018 SDKs.
; To start it, use the following commands from BCM:
;
;       led load sdk56018.hex
;       led auto on
;       led start
;
; The are 2 LEDs per FE port one for Link(Left) and one for activity(Right).
;
; There are two bits per Fast Ethernet LED with the following colors:
;       ZERO, ZERO      Black
;       ZERO, ONE       Amber
;       ONE, ZERO       Green
;
; There are 8 LEDS two each for stack/uplink ports
; There is one bit per stack/uplink LED with the following colors:
;       ZERO      Black
;       ONE       Green
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       A_fe0, L_fe0, A_fe1, L_fe1, ... A_fe47, L_fe47, L_ge3, A_ge3,
;       L_ge4, A_ge4, L_ge0, A_ge0, L_ge1, A_ge1,
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0xa0 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/esw/esw/ledproc.c.
;
; Current implementation:
;
; L reflects port 1 link status:
;       Black: no link
;       Amber: 10 Mb/s
;       Green: 100 Mb/s
;       Very brief flashes of black at 1 Hz: half duplex
;       Longer periods of black: collisions in progress
;
; A reflects port 1 activity (even if port is down)
;       Black: idle
;       Green: RX (pulse extended to 1/3 sec)
;       Amber: TX (pulse extended to 1/3 sec, takes precedence over RX)
;       Green/Amber alternating at 6 Hz: both RX and TX
;
;

TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)

MIN_FE_PORT     EQU     6
MAX_FE_PORT     EQU     53
NUM_FE_PORT     EQU     54

MIN_GE_PORT     EQU     0
MAX_GE_PORT     EQU     4
NUM_GE_PORT     EQU     5

NUM_ALL_PORT    EQU     53

MIN_PORT        EQU     1
MAX_PORT        EQU     53
NUM_PORT        EQU     53

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
        ld      a, MIN_FE_PORT

up1:
        port    a
        ld      (PORT_NUM),a

        call    activity        ; Right LED for this port
        call    link_status     ; Left LED for this port

        ld      a,(PORT_NUM)
        inc     a
        cmp     a,NUM_FE_PORT
        jnz     up1

        ; Put out 8 bits for stack/uplink port
        ld      a, 4
up1_5:
        port    a
        ld      (PORT_NUM),a

        call    get_link
        jnc     no_link
        pushst  ZERO
        pack
        jmp     chk_act
no_link:
        pushst  ONE
        pack
chk_act:
        call    activity_unicolor        ; Down LED for this port

        ld      a,(PORT_NUM)
        inc     a
        cmp     a,6
        jnz     up1_5

        ld      a, 1
up1_6:
        port    a
        ld      (PORT_NUM),a

        call    get_link
        jnc     no_link_1_6
        pushst  ZERO
        pack
        jmp     chk_act_1_6
no_link_1_6:
        pushst  ONE
        pack
chk_act_1_6:
        call    activity_unicolor        ; Down LED for this port

        ld      a,(PORT_NUM)
        inc     a
        cmp     a,3
        jnz     up1_6

        ; Update various timers

        ld      b,GIG_ALT_COUNT
        inc     (b)
        ld      a,(b)
        cmp     a,GIG_ALT_TICKS
        jc      up2
        ld      (b),0
up2:

        ld      b,HD_COUNT
        inc     (b)
        ld      a,(b)
        cmp     a,HD_ON_TICKS+HD_OFF_TICKS
        jc      up3
        ld      (b),0
up3:

        ld      b,TXRX_ALT_COUNT
        inc     (b)
        ld      a,(b)
        cmp     a,TXRX_ALT_TICKS
        jc      up4
        ld      (b),0
up4:

        send    200     ; 2 * 2 * 48 + 2 * 1 * 4

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
        pushst  TX
        pop
        jnc     chk_rx

tx_is_active_chk_rx:
        pushst  RX
        pop
        jnc     led_amber       ; TX Only

tx_and_rx_active:               ; Both TX and RX active
        ld      b,(TXRX_ALT_COUNT)
        cmp     b,TXRX_ALT_TICKS/2
        jc      led_amber       ; Fast alternation of green/amber
        jmp     led_green

chk_rx:
        pushst  RX
        pop
        jnc     led_black           ; No Activity
        jmp     led_green       ; RX Only


;
; activity_unicolor
;
;  This routine calculates the activity LED for the current port.
;
;  Inputs: (PORT_NUM)
;  Outputs: One bit sent to LED stream
;

activity_unicolor:
        pushst  RX
        pop
        jnc     act_u1

        pushst  ZERO
        pack

        ret
act_u1:
        pushst  TX
        pop
        jnc     act_u2

        pushst  ZERO
        pack

        ret
act_u2:
        pushst  ONE
        pack
        ret
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
        pushst  DUPLEX          ; Skip blink code if full duplex
        pop
        jc      ls1

        pushst  COLL            ; Check for collision in half duplex
        pop
        jc      led_black

        ld      a,(HD_COUNT)    ; Provide blink for half duplex
        cmp     a,HD_OFF_TICKS
        jc      led_black

ls1:
        ld      a,(PORT_NUM)    ; Check for link down
        call    get_link
        jnc     led_black

        pushst  SPEED_C         ; Check for 100Mb speed
        pop
        jc      led_green

        pushst  SPEED_M         ; Check for 10Mb (i.e. not 100 or 1000)
        pop
        jnc     led_amber

        ld      a,(GIG_ALT_COUNT)
        cmp     a,GIG_ALT_TICKS/2
        jc      led_amber

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
        pushst  ZERO
        pack
        pushst  ZERO
        pack
        ret

led_amber:
        pushst  ZERO
        pack
        pushst  ONE
        pack
        ret

led_green:
        pushst  ONE
        pack
        pushst  ZERO
        pack
        ret

;
; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xfc
HD_COUNT        equ     0xfd
GIG_ALT_COUNT   equ     0xfe
PORT_NUM        equ     0xff

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA        equ     0xa0    ; Size 54

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

