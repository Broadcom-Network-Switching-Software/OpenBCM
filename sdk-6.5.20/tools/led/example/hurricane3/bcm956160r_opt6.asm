;
; $Id: bcm956160r_opt6.asm, v 1.0 2015/07/12 15:30:14 rihuang Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
; This example is used for BCM956160R.
;
; To start it, use the following commands from BCM:
;
;       led load bcm956160r_opt6.hex
;       led auto on
;       led start
;
; For TSCE (10G+) ports, each port needs to output 2 bits for 2 LEDs
;   LED_0: bit 0
;   LED_1: bit 1
;
; For QTC (2.5G) ports, each port needs to output 2 bits for 1 LED
;   LED_0: bit 0-1 
;
; For EGPHY (1G) ports, each port needs to output 2 bits for 2 LEDs
;   LED_0: bit 0
;   LED_1: bit 1
;
; Totally 26 ports will be outputed, i.e. 52 (= 26 * 2) bits.
; The output sequence is TSCE->QTC->EGPHY, and high port is first.
;
; In physical port view, the output order is:
;   36->34(TSCE), 21->18(QTC), 5->2(QTC), 33->26(EGPHY), 10->17(EGPHY)
;
; Link up/down info cannot be derived from LINKEN, as the LED
; processor does not always have access to link status.  This program
; assumes link status is kept current in bit 0 of RAM byte (0xA0 + portnum).
; Generally, a program running on the main CPU must update these
; locations on link change; see linkscan callback in
; $SDK/src/appl/diag/ledproc.c.
;

;
; Constants
;
TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)
ACT_EXT_TICKS   EQU     (SECOND_TICKS/3)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

NUM_PORTS   equ 27
START_PORT_0 equ 36
END_PORT_0 equ 34
START_PORT_1 equ 21
END_PORT_1 equ 18
START_PORT_2 equ 5
END_PORT_2 equ 2
START_PORT_3 equ 33
END_PORT_3 equ 26
START_PORT_4 equ 10
END_PORT_4 equ 17

;
; LED process
;

start_sec0:
    ld a, START_PORT_0
iter_sec0:
    port    a
    ld  (PORT_NUM), a

    call get_activity
    call get_link_sw

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_0 - 1
    jnz iter_sec0

start_sec1:
    ld a, START_PORT_1
iter_sec1:
    port    a
    ld  (PORT_NUM), a

    call get_port_status

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_1 - 1
    jnz iter_sec1

start_sec2:
    ld a, START_PORT_2
iter_sec2:
    port    a
    ld  (PORT_NUM), a

    call get_port_status

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_2 - 1
    jnz iter_sec2

start_sec3:
    ld a, START_PORT_3
iter_sec3:
    port    a
    ld  (PORT_NUM), a

    call get_activity
    call get_link_sw

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_3 - 1
    jnz iter_sec3

start_sec4:
    ld a, START_PORT_4
iter_sec4:
    port    a
    ld  (PORT_NUM), a

    call get_activity
    call get_link_sw

    ld  a, (PORT_NUM)
    inc a
    cmp a, END_PORT_4 + 1
    jnz iter_sec4

end:
    inc     (TXRX_ALT_COUNT)
	send    2*NUM_PORTS

;
; get_link_sw
;
;  This routine finds the link status LED for a port.
;  Link info is in bit 0 of the byte read from PORTDATA[port]
;  Inputs: (PORT_NUM)
;  Outputs: LED stream for on or off.
;  Destroys: a, b
get_link_sw:
    ld  b, PORTDATA
    add b, (PORT_NUM)
    ld  a, (b)
    tst a, 0

    jc led_on
    jmp led_off

;
; get_link_hw
;
;  This routine finds the link status LED for a port from HW.
;  Inputs: a
;  Outputs: LED stream for on or off.
;  Destroys: b
get_link_hw:
    port   a
    pushst LINKEN
    pop

    jc led_on
    jmp led_off

;
; get_activity
;
;  This routine finds the activity for a port from HW.
;  Inputs: a
;  Outputs: LED stream for on or off.
;  Destroys: b
get_activity:
        port    a
        pushst  RX
        pushst  TX
        tor
        pop

        jnc     led_off       ; Always black, No Activity

        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS
        jnz     led_on
        jmp     led_off

;
; get_port_status
;
;  This routine finds the link and activity status for a port from HW.
;  Inputs: a
;  Outputs: LED stream for on or off.
;  Destroys: b
get_port_status:
    port   a

    pushst LINKEN
    pop
    jnc     led_black   ; Always black, No LINK

    pushst  RX
    pushst  TX
    tor
    pop

    jc led_blink        ; Show activity
    jmp led_green       ; Show Link
led_blink:
    ld      b, (TXRX_ALT_COUNT)
    and     b, TXRX_ALT_TICKS
    jnz     led_green
    jmp     led_black

;
; led_on: 
;
;  For 10G+ and 1G port LED, outputs one bit
;  Outputs: Bits to the LED stream indicating ON
;
led_on:
    push 0
    pack
    ret

;
; led_off
;  For 10G+ and 1G port LED, outputs one bit
;  Outputs: Bits to the LED stream indicating OFF
;
led_off:
    push 1
    pack
    ret

;
; led_black
;  For 2.5G port LED, outputs two bit
;  Outputs: Bits to the LED stream indicating BLACK(OFF)
;
led_black:
    push 1
    pack
    push 1
    pack
    ret

;
; led_green
;  For 2.5G port LED, outputs two bit
;  Outputs: Bits to the LED stream indicating GREEN
;
led_green:
    push 0
    pack
    push 0
    pack
    ret

; Variables (SDK software initializes LED memory from 0xA0-0xff to 0)
PORTDATA    equ 0xA0
TXRX_ALT_COUNT  equ     0xE0
PORT_NUM        equ     0xE1

; Symbolic names for the bits of the port status fields
RX      equ 0x0 ; received packet
TX      equ 0x1 ; transmitted packet
COLL    equ 0x2 ; collision indicator
SPEED_C equ 0x3 ; 100 Mbps
SPEED_M equ 0x4 ; 1000 Mbps
DUPLEX  equ 0x5 ; half/full duplex
FLOW    equ 0x6 ; flow control capable
LINKUP  equ 0x7 ; link down/up status
LINKEN  equ 0x8 ; link disabled/enabled status
ZERO    equ 0xE ; always 0
ONE     equ 0xF ; always 1
