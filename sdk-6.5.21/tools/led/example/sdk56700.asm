;
; $Id: sdk56700.asm,v 1.8 2012/03/02 14:34:03 yaronm Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the (16 port) BCM56700 SDKs.
; To start it, use the following commands from BCM:
;
;       led load sdk56700.hex
;       led auto on
;       led start
;
; The is 1 LED per GX port, which is used for both link and activity.
;
; There are two bits per gigabit Ethernet LED with the following colors:
;       ZERO, ZERO      Black
;       ZERO, ONE       Amber
;       ONE, ZERO       Green
;
; Each chip drives only its own LED and needs to write them in the order:
;       L01, L02, ..., L16
;
; Link up/down info cannot be derived from LINKEN or LINKUP, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0x80 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;
; Link activity info for Higig/XE ports are not updated by LED processor in
; 56700_a0. The counter DMA task reports the activities of Higig/Xe ports
; in bit 1-3 of RAM byte (0x94 + portnum). Bit 1 indicates that the link
; activity is high. Bit 2 indicates that the link activity is medium. Bit
; 3 indicates that the link activity is low. If all three bits are clear, there
; is no activity on the link. See counter DMA task in $SDK/src/soc/counter.c
;
; Current implementation:
;
; L01 reflects port 1 link and activity status:
;       Black: no link
;       Green: link
;   when port speed is higher than 2.5G,
;       Green/Amber alternating at approximately 2 Hz (low activity)
;       Green/Amber alternating at approximately 4 Hz (medium activity)
;       Green/Amber alternating at approximately 8 Hz (high activity)
;
;   when port speed is 2.5G and less,
;       Black: idle
;       Green: RX (pulse extended to 1/3 sec)
;       Amber: TX (pulse extended to 1/3 sec, takes precedence over RX)
;       Green/Amber alternating at 6 Hz: both RX and TX
;

MIN_PORT        EQU     0
MAX_PORT        EQU     15
NUM_PORT        EQU     18      ; Actual number of port 16 + 2 skipped ports.      
SKIP_PORT       EQU      8      ; Skip port 8 and 9.

HI_ACT          EQU      2 	; Hi activity bit mask 
ME_ACT          EQU      4	; Medium activity bit mask
LO_ACT          EQU      8	; Low activity bit mask

; The TX/RX activity lights will be extended for ACT_EXT_TICKS
; so they will be more visible.
SECOND_TICKS    EQU     30
ACT_EXT_TICKS   EQU     (SECOND_TICKS/3)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
        sub     a,a             ; ld a,MIN_PORT (but only one instr)

up1:
        port    a
        ld      (PORT_NUM),a

        call    activity 

        ld      a,(PORT_NUM)
        inc     a
        cmp     a, SKIP_PORT     ; On 16 ports HUMV, port 8 and 9 are 
        jnz     up2              ; disabled. Therefore, we skip
        add     a, 2             ; port 8 and 9.

up2:
        cmp     a,NUM_PORT
        jnz     up1

        ; Update various timers
        inc     (HIGIG_ALT_COUNT)

up3:
        ld      b,TXRX_ALT_COUNT
        inc     (b)
        ld      a,(b)
        cmp     a,TXRX_ALT_TICKS
        jc      up4
        ld      (b),0

up4:
        send    32     ; 2 * (16 )

;
; activity
;
;  This routine calculates the link/activity LED for the current port.
;
;  Inputs: (PORT_NUM)
;  Outputs: Two bits sent to LED stream
;
activity:
        pushst  RX
        pop
        jnc     act1

        ld      b,RX_TIMERS     ; Start RX LED extension timer
        add     b,(PORT_NUM)
        ld      a,ACT_EXT_TICKS
        ld      (b),a

act1:
        pushst  TX
        pop
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

        jmp     xaui_activity   ; If LED processor status indicates no 
                                ; activity, we check the activity status
                                ; reported by counter thread.

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

xaui_activity:
        ld      a, (PORT_NUM)
        ld      b, ACT_DATA
        add     b, a
        ld      b, (b)

        ld      a, HI_ACT
        and     a, b
        jnz     xaui_active
        
        ld      a, ME_ACT
        and     a, b
        jnz     xaui_active

        ld      a, LO_ACT
        and     a, b
        jnz     xaui_active
   
        jmp     link_status

xaui_active:
        and     a, (HIGIG_ALT_COUNT) 
        jnz     led_green
        jmp     led_amber

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
        call    get_link
        jnc     led_black       ; Link down

        jmp     led_green       ; Link up

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
        ld      b,LNK_DATA
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

; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xf0
PORT_NUM        equ     0xf2
HIGIG_ALT_COUNT equ     0xf3 ; HIGIG_ALT_COUNT holds the higig activity
                             ; LED blinking rate. Bit 1 corresponds to
                             ; to high activity, bit 2 corresponds to
                             ; medium activity, and bit 3 corresponds to
                             ; low activity. The high activity blinks twice
                             ; the speed of medium activity and the medium
                             ; activity blinks twice the speed of low
                             ; activity.  This counter is incremented
                             ; every LED refresh cycle.
                              
;
; LNK_DATA must be updated continually by main CPU's linkscan task.
; See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, 
; Bit
; 0   - If set, the link is up. Otherwise, the link is down.
LNK_DATA        equ     0x80               ; 20 bytes

;
; ACT_DATA must be updated continually by counter DMA tasks.  
; See $SDK/src/soc/counter.c for examples.
; Bit
; 1   - If set, the link activity is more than 50% of line rate.
; 2   - If set, the link activity is between 25 to 50% of line rate.
; 3   - If set, the link activity is between 3 to 25% of line  rate.
; If bit 1, 2, and 3 are all clear, the link activity is less than 3%.
ACT_DATA	equ     0x80+0x14          ; 20 bytes

;
; LED extension timers
;
RX_TIMERS       equ     ACT_DATA+0x14  ; NUM_PORT bytes
TX_TIMERS       equ     RX_TIMERS+NUM_PORT ; NUM_PORT bytes

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
