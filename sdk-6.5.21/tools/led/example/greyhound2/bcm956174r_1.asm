;
; $Id: bcm956174r_1.asm, v 1.0 2017/11/20 15:30:14 leonlee Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
; This example is used for showing the serial LEDs on
; bcm956174r on option 11b.
;
; To start it, use the following commands from BCM:
;
;       led 1 load bcm956174r_1.hex
;       led 1 auto on
;       led 1 start
;
; For CL port needs to output 2 bits stream,
;   bit 0: LED_0 (Link)
;   bit 1: LED_1 (Activity)
; For TSCE 10G port need to output 4 bits stream,
;   bit 0: LED_0 (Link/Activity)
;   bit 1: LED_1 (Link/Activity)
;   bit 2: LED_2 (Speed)
;   bit 3: LED_3 (Speed)
;
; Totally 6 ports need be outputed, i.e. 12 (= 6 * 2 ) bits.
; The output sequence for EGPHY will follow the user port sequence.
;
; The LED sequence is (User Port, Front Panel Order)
;   25-30 (Lport 38-43, Routing 41-38 42 43)
; Mapping onto physical port view, the sequence is:
;   77 76 75 74 82 86
;   21 20 19 18 26 30  (LED1 remap, -56)
;
; The CMIC_LEDUP1_PORT_ORDER_REMAP_XXX can only store 6 bits value for 64 ports, also for
; CMIC_LEDUP1_DATA_RAM it can only store data of 64 ports (128 bytes, each port requires
; 2 bytes). So for LED1 which is for physical ports 58-89, the physical port id has to be
; shifted in order to fit in the capability of the HW tables. GH2 LED driver shifts physical
; ports 58-89 to indices 2-33 (by minus 56) for LED1 tables, so that the shifted value can
; be stored in PORT_ORDER_REMAP and reflects on the DATA_RAM.
;
; The output order should be the inverted sequence of the physical
; mapping view
;   30 26 18 19 20 21
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

; the smaller the TXRX_ALT_TICKS the faster it blinks
TXRX_ALT_TICKS  EQU     5
TXRX_ALT_COUNT  equ     30

NUM_PORTS   equ 6

START_PORT_0 equ 30
END_PORT_0 equ 30
START_PORT_1 equ 26
END_PORT_1 equ 26
START_PORT_2 equ 21
END_PORT_2 equ 18
;
; LED process
;

start_sec0:
    ld a, START_PORT_0
iter_sec0:
    port    a
    ld  (PORT_NUM), a
    call get_link_hw
    call get_activity_hw

    ld  a, (PORT_NUM)
    inc a
    cmp a, END_PORT_0 + 1
    jnz iter_sec0

start_sec1:
    ld a, START_PORT_1
iter_sec1:
    port    a
    ld  (PORT_NUM), a
    call get_link_hw
    call get_activity_hw

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_1 - 1
    jnz iter_sec1

start_sec2:
    ld a, START_PORT_2
iter_sec2:
    port    a
    ld  (PORT_NUM), a
    call get_link1Gspeed_status
    call get_link10Gspeed_status

    ld  a, (PORT_NUM)
    dec a
    cmp a, END_PORT_2 - 1
    jnz iter_sec2

update:
    inc (TXRX_ALT_COUNT)

end:
    send    2*NUM_PORTS

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

    jc led_blink
    jmp led_off

;
; get_linkact_status
;
;  This routine finds the link and activity status for a port from HW.
;  The LED will blink if there is activity.
;  Inputs: a
;  Outputs: LED stream for on or off.
;  Destroys: b
get_linkact_status:
    port   a

    pushst LINKEN
    pop
    jnc     led_off   ; Always black, No LINK

    pushst  RX
    pushst  TX
    tor
    pop

    jc led_blink        ; Show activity
    jmp led_on       ; Show Link


;
; get_link10Gspeed_status
;
;  This routine finds the link and 10G speed status for a port from HW.
;  The LED will blink if there is activity.
;  Inputs: a
;  Outputs: LED stream for on or off.
;  Destroys: b
get_link10Gspeed_status:
    port   a

    pushst LINKEN
    pop
    jnc     led_off   ; Always black, No LINK

    pushst SPEED_C
    pushst SPEED_M
    tand
    pop
    jnc     led_off   ; Always black, No 10G

    pushst  RX
    pushst  TX
    tor
    pop

    jc led_blink        ; Show activity
    jmp led_on       ; Show Link

;
; get_link1Gspeed_status
;
;  This routine finds the link and 1G speed status for a port from HW.
;  The LED will blink if there is activity.
;  Inputs: a
;  Outputs: LED stream for on or off.
;  Destroys: b
get_link1Gspeed_status:
    port   a

    pushst LINKEN
    pop
    jnc     led_off   ; Always black, No LINK

    pushst SPEED_M
    pop
    jnc     led_off   ; Always black, No 1G

    pushst SPEED_C
    pop
    jc     led_off   ; If 100M also up, indicate its 10G not 1G

    pushst  RX
    pushst  TX
    tor
    pop

    jc led_blink        ; Show activity
    jmp led_on       ; Show Link

;
; led_blink
;
;  Making the LED blinking
;
led_blink:
    ld      b, (TXRX_ALT_COUNT)
    and     b, TXRX_ALT_TICKS
    jz     led_on
    jmp     led_off

;
; led_on
;
;  Outputs: Bits to the LED stream indicating ON
;
led_on:
    push 0
    pack
    ret

;
; led_off
;
;  Outputs: Bits to the LED stream indicating OFF
;
led_off:
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

