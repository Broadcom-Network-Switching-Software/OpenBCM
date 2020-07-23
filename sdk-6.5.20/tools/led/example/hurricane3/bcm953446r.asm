;
; $Id: bcm953446r.asm, v 1.0 2015/08/05 15:30:14 rihuang Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
; This example is used for showing the serial to parallel EGPHY LEDs on 
; BCM953446R.
; To enalbe serial to parallel LED, TOP_PARALLEL_LED_CTRLr.LED_SER2PAR_SELf 
; must be set.
;
; To start it, use the following commands from BCM:
;
;       led load bcm953446r.hex
;       led auto on
;       led start
;
; Each EGPHY port needs to output 4 bits stream,
;   bit 0: LED_0
;   bit 1: LED_1
;   bit 2: Not used. Must be 0. 
;   bit 3: Not used. Must be 0.
;
; Each TSCE port needs to output 2 bits stream,
;   bit 0: LED_0
;   bit 1: LED_1
;
; Totally 96 bits need be outputed. (96 = 24 * 4).
; The first 48 bits (for 16 EGPHY ports) are displayed by serial to parallel LED.
; The last 8 bits are used for TSCE serial LED.
; The other 24 bits are not used for LED.
;
; In physical port view, the output order is:
;   17, 16, ..., 10, 26, 27, ..., 33, 24-dummy-bits, 34, 35, 36, 37
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
START_PORT_0 equ 17
END_PORT_0 equ 10
START_PORT_1 equ 26
END_PORT_1 equ 33
START_PORT_2 equ 37
END_PORT_2 equ 34

;
; LED process
;

start_sec0:
    ld a, START_PORT_0
iter_sec0:
    port    a
    ld  (PORT_NUM), a

    call get_link_hw

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_0 - 1
    jnz iter_sec0

start_sec1:
    ld a, START_PORT_1
iter_sec1:
    port    a
    ld  (PORT_NUM), a

    call get_link_hw

    ld  a, (PORT_NUM)
    inc a
    cmp a, END_PORT_1 + 1
    jnz iter_sec1

start_dummy_bits:
    ld a, 0
iter_dummy_bits:
    call led_off_4bits
    inc a
    cmp a, 6
    jnz iter_dummy_bits

start_sec2:
    ld a, START_PORT_2
iter_sec2:
    port    a
    ld  (PORT_NUM), a

    call get_link_hw

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_2 - 1
    jnz iter_sec2

end:
	send 96

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

    jc led_on
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

    jc led_on
    jmp led_off

;
; led_on
;
;  Outputs: Bits to the LED stream indicating ON
;
led_on:
    ld  b, (PORT_NUM)
    cmp b, 34
    jc led_on_4bits
    jmp led_on_2bits

; Trun on LED for GE port
led_on_4bits:
    push 1
    pack
    push 1
    pack
    push 0
    pack
    push 0
    pack
    ret
; Trun on LED for XE port
led_on_2bits:
    push 0
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
    ld  b, (PORT_NUM)
    cmp b, 34
    jc led_off_4bits
    jmp led_off_2bits

; Trun off LED for GE port
led_off_4bits:
    push 0
    pack
    push 0
    pack
    push 0
    pack
    push 0
    pack
    ret

; Trun off LED for XE port
led_off_2bits:
    push 1
    pack
    push 1
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
