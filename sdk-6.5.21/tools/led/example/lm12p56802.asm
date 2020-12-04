;
; $Id: lm12p56802.asm,v 1.5 2011/05/22 23:38:42 iakramov Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the 12 port 56802 based CX4 LM
; To start it, use the following commands from BCM:
;
;       led load lm12p56802.hex
;       led auto on
;       led start
;
; There are 48 LEDS four each for 10G ports
; There is one bit per 10G LED with the following colors:
;       ZERO     Black
;       ONE      Green
;
; The chip drives its LEDs and needs to write them in the order:
;       L01,T01, R01, E01,  ..., L12,T12, R12, E12
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0x80 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;
; Link activity info for Higig/XE ports are not updated by LED processor in
; 56800. The counter DMA task reports the activities of Higig/Xe ports
; in bit 1-5 of RAM byte (0x94 + portnum). Bit 1 indicates that the link
; activity is high. Bit 2 indicates that the link activity is medium. Bit
; 3 indicates that the link activity is low. If all three bits are clear, there
; is no activity on the link. Bit 4 indicates TX activity and Bit 5 indicates
; RX activity. See counter DMA task in $SDK/src/soc/counter.c
;
; Current implementation:
;
; L01-L12 reflects 10G link status:
;       Black: no link
;       Green: Link
; T01-T12 reflects port 1-12 TX activity
;       Black: idle
;       Green: TX (pulse extended to 1/3 sec)
; R01-R12 reflects port 1-12 RX activity
;       Black: idle
;       Green: RX (pulse extended to 1/3 sec)
; E01-E12 reflects 10G link enable
;       Black: Disable
;       Green: Enable
;

TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)

; hg4-hg15 are front panel ports (hg0-hg3 are back-plane ports)
MIN_XE_PORT     EQU     4
SKIP_PORT       EQU     8       ; Ports 8 and 9 are skipped in HUMV
MAX_XE_PORT     EQU     17
NUM_PORT        EQU     18

HI_ACT          EQU      0x2    ; Hi activity bit mask
ME_ACT          EQU      0x4    ; Medium activity bit mask
LO_ACT          EQU      0x8    ; Low activity bit mask
TX_ACT          EQU      0x10   ; TX activity
RX_ACT          EQU      0x20   ; RX activity

; The TX/RX activity lights will be extended for ACT_EXT_TICKS
; so they will be more visible.

ACT_EXT_TICKS   EQU     (SECOND_TICKS/3)

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
        ; Put out 48 bits for 10G port
        ld      a, MIN_XE_PORT
up1:
        port    a

        cmp     a, SKIP_PORT    ; On 16 ports HUMV, port 8 and 9 are
        jnz     valid_port      ; disabled. Therefore, we skip
        add     a, 2            ; port 8 and 9
        port    a

valid_port:
        ;       LINKUP  TX, RX, LINKEN
        pushst  LINKUP
        call    get_link
        jnc     link_down
        tinv
link_down:
        pack

        ld      (PORT_NUM), a
        call    tx_activity     ; get TX activity
        ld      a, (PORT_NUM)

        ld      (PORT_NUM), a
        call    rx_activity     ; get RX activity
        ld      a, (PORT_NUM)

        pushst  LINKEN
        tinv
        pack

        inc     a
        cmp     a, NUM_PORT
        jnz     up1

        send    48     ; 1 * 4 * 12 = 48

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

tx_activity:
        ld      a, (PORT_NUM)
        ld      b, ACT_DATA
        add     b, a
        ld      b, (b)

        ld      a, TX_ACT
        and     a, b
        jz      tx_act1

        ld      b, TX_TIMERS     ; Start TX LED extension timer
        add     b, (PORT_NUM)
        ld      a, ACT_EXT_TICKS
        ld      (b),a

tx_act1:
        ld      b, TX_TIMERS
        add     b, (PORT_NUM)
        dec     (b)
        jnc     led_green       ; TX active?
        inc     (b)
        jmp     led_black       ; No activity

rx_activity:
        ld      a, (PORT_NUM)
        ld      b, ACT_DATA
        add     b, a
        ld      b, (b)

        ld      a, RX_ACT
        and     a, b
        jz      rx_act1

        ld      b, RX_TIMERS     ; Start RX LED extension timer
        add     b, (PORT_NUM)
        ld      a, ACT_EXT_TICKS
        ld      (b),a

rx_act1:
        ld      b, RX_TIMERS
        add     b, (PORT_NUM)
        dec     (b)
        jnc     led_green       ; RX active?
        inc     (b)
        jmp     led_black       ; No activity

led_black:
        pushst  ZERO
        tinv
        pack
        ret

led_green:
        pushst  ONE
        tinv
        pack
        ret

;
; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;


;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA        equ     0x80               ; 20 bytes

;
; ACT_DATA must be updated continually by counter DMA tasks.
; See $SDK/src/soc/counter.c for examples.
; Bit
; 1   - If set, the link activity is more than 50% of line rate.
; 2   - If set, the link activity is between 25 to 50% of line rate.
; 3   - If set, the link activity is between 3 to 25% of line  rate.
; If bit 1, 2, and 3 are all clear, the link activity is less than 3%.
ACT_DATA        equ     0x80+0x14          ; 20 bytes


;
; LED extension timers
;

RX_TIMERS       equ     0xc0+0                  ; NUM_PORT bytes
TX_TIMERS       equ     0xc0+NUM_PORT           ; NUM_PORT bytes
PORT_NUM        equ     TX_TIMERS+NUM_PORT

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

