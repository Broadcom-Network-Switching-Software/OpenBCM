;
; $Id: bcm953570k_0.asm, v 1.0 2017/08/29 15:30:14 leonlee Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
; This example is used for showing the serial LEDs on BCM953570K using LED bus.
;
; To start it, use the following commands from BCM:
;
;       led load bcm953570k_0.hex
;       led auto on
;       led start
;
; Each port needs to output 4 bits stream,
;   bit 0: LED_0 (activity)
;   bit 1: LED_0 (activity)
;   bit 2: LED_1 (link up/down)
;   bit 3: LED_1 (link up/down)
;
; Totally 24 ports need be outputed
;
; In physical port view, the output order is:
;   2, 3, 4 ... 25
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

START_PORT_0 equ 2
END_PORT_0 equ 25

;
; LED process
;

start_sec0:
    ld a, START_PORT_0
iter_sec0:
    port    a
    ld  (PORT_NUM), a
    call get_activity_hw

    ld  a, (PORT_NUM)
    port    a
    call get_link_hw
    
    ld  a, (PORT_NUM)
    inc a
    cmp a, END_PORT_0 + 1
    jnz iter_sec0

end:
;    output the maximun bits for the LED bus
;    to avoid undesired repetitive behavior
    send    255

;
; get_activity_hw
;
;  This routine finds the link status LED for a port from HW.
;  Inputs: (PORT_NUM)
;  Outputs: Carry flag set if RX or TX is up, clear if link is down.
;  Destroys: a, b

get_activity_hw:
    pushst RX
    pushst TX
    tor
    pop

    jc led_on_green
    jmp led_off

;
; get_link_hw
;
;  This routine finds the link status LED for a port from HW.
;  Inputs: (PORT_NUM)
;  Outputs: Carry flag set if link is up, clear if link is down.
;  Destroys: a, b
get_link_hw:
    pushst LINKEN
    pop

    jc led_on_green
    jmp led_off

;
; get_link_sw
;
;  This routine finds the link status LED for a port.
;  Link info is in bit 0 of the byte read from PORTDATA[port]
;  Inputs: (PORT_NUM)
;  Outputs: Carry flag set if link is up, clear if link is down.
;  Destroys: a, b
get_link_sw:
    ld  b, PORTDATA
    add b, (PORT_NUM)
    ld  a, (b)
    tst a, 0

    jc led_on_green
    jmp led_off

;
; led_on_green
;
;  Outputs: Bits to the LED stream indicating ON
;
led_on_green:
    push 1
    pack
    push 0
    pack
    ret

;
; led_off
;
;  Outputs: Bits to the LED stream indicating OFF
;
led_off:
    push 0
    pack
    push 0
    pack
    ret

; Variables (SDK software initializes LED memory from 0xA0-0xff to 0)
PORTDATA    equ 0xA0
PORT_NUM    equ 0xE0

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
