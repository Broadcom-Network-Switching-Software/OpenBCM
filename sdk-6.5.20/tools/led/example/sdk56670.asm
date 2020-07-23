;
; $Id: sdk56670x.asm,v 1.0 2017/11/7 10:47:36 prasanna Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
;
; This is the default program for the BCM56670.
; There s one  LED processors on BCM56670.
; Each LED processor accomodates 64 ports.
; LED processor 0 : Physical Ports 1-64
;
; To start the first one, use the following commands from BCM:
;
;       led 0 load sdk56670.hex
;       led 0 auto on
;       led 0 start
;
; The are 2 LEDs per port one for Link(Top) and one for activity(Bottom).
;
; Each chip drives only its own LEDs and needs to write them in the order:
;       L01/A01, L02/A02, ..., L63/A63
;
; There are two bits per Ethernet LED for activity with the following colors:
;       ZERO, ZERO      Black
;       ZERO, ONE       Amber
;       ONE, ZERO       Green
;
; This program assumes link status is kept current in bit 0 of RAM byte (0xA0 + portnum).
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
;       Black: no activity
;       Blinking Green and Black: TX activity
;       Blinking Amber and Black: RX activity
;       Blinking Green and Amber: TX and RX activity
;
;
TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)

TXRX_ALT_TICKS  EQU     (SECOND_TICKS)

MIN_PORT        EQU     0
NUM_PORT        EQU     64 
MAX_PORT        EQU     63 


; Assumptions
; Link status is injected by SDK from Linkscan task at PORTDATA offset

;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
        ld      a,MIN_PORT
port_loop:

        call    link_status    ; Top LED for this port
        call    chk_activity   ; Bottom LED for this port

        inc     a
        cmp     a,MAX_PORT
        jnz     port_loop

        ; Update various timers
        inc     (TXRX_ALT_COUNT)

        send    248 ; offset to shift bits so as to align LEDs on board intuitively


; Activity status LED update
chk_activity:
        call    get_txact   ;get TX activity
        jnc     act1        ;TX not active, check if RX active
        jmp     act2        ;TX active, check if RX active
        ret
act1:
        call    get_rxact
        jnc     led_black   ;no activity
        jmp     blink_amber ;only RX active
        ret
act2:
        call    get_rxact
        jnc     blink_green ;only TX active
        jmp     blink_amg   ;TX and RX both active
        ret

; Link status LED update
link_status:
        call    get_link
        jnc     led_black
        jmp     led_green
        clc
        ret

;
; get_link
;
;  This routine finds the link status for a port.
;  Link info is in bit 0 of the byte read from PORTDATA[port]
;
;  Inputs: Port number in a
;  Outputs: Carry flag set if link is up, clear if link is down.
;  Destroys: b
;

get_link:
        clc
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,0
        ret

;
; get_txact
;
;  This routine finds the TX activity status for a port.
;  TX activity info is in bit 4 of the byte read from PORTDATA[port]
;
;  Inputs: Port number in a
;  Outputs: Carry flag set if TX activity is in progress, clear if not..
;  Destroys: b
;
get_txact:
        clc
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,4
        ret

;
; get_rxact
;
;  This routine finds the RX activity status for a port.
;  TX activity info is in bit 5 of the byte read from PORTDATA[port]
;
;  Inputs: Port number in a
;  Outputs: Carry flag set if RX activity is in progress, clear if not..
;  Destroys: b
;
get_rxact:
        clc
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,5
        ret

;
; blink_amber, blink_green, blink_amg
; Alteration between respective colors based on timer
;

blink_amber:
        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS
        jnz     led_amber
        jmp     led_black
        clc
        ret

blink_green:
        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS
        jnz     led_green
        jmp     led_black
        clc
        ret

blink_amg:
        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS
        jnz     led_amber
        jmp     led_green
        clc
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

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

;LED scan chain assembly area
;Offset 0x20680 for LED 0
PORTDATA        equ     0xa0    ; Size 4 bytes for each port starting A0,A4 ...

ZERO            equ     0xE     ; always 0
ONE             equ     0xF     ; always 1
