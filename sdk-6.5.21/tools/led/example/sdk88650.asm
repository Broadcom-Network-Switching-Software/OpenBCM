; $Id: sdk88650.asm,v 1.2 2012/06/27 06:52:51 assaf Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
;Here is an example program for programming the LEDs on the 88650.
;this example is written for activating the links on the Negev chassis.
;
;Negev chassis configuration:
;       4 ports (ports 13-16) each have two LEDs, one green and one orange.
;          led 0 (green) : LINK
;		off when link down
;		on if link up
;          led 1 (orange): TX RX
;       off when link is down or there is no traffic. 
;		blinking if link is up and traffic transmitted/received.
;
;    The scan out order is LINK, TX/RX for port 13, then the same for port 14,
;    and so on through port 16.  
;
;
;led processor DATA RAM configuration:
; 
; Link up/down port data must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c .
; This program assumes bit 0 contains the link up/down status.
;
; Offset 0x01-0x40
; Ports status: 32 ports, 2 bytes per port.
;
; Offset 0x80 (8 bites LED scan chain, requires only 1 byte in data memory)
; LED scan chain assembly
;
; Offset 0xa0 - 0xc0
; Link scan ports data: 32 ports, one byte per port, only lsb of each byte is valid,
; indicating whether the link is up(1) or down(0).
;

        
;-------------------------- start of program --------------------------
; Not used in the program, added for declaring ARAD phisical ports range
MIN_XE_PORT     EQU     1
MAX_XE_PORT     EQU     32

; Device ports
PORT_XE13       EQU     13
PORT_XE14       EQU     14
PORT_XE15       EQU     15
PORT_XE16       EQU     16

NUM_LEDS        EQU     8       ;4 ports, 2 LEDs per port, total 8 bits
        
; The TX/RX activity lights will blink SECOND_TICKS/TXRX_ALT_TICKS per second.
TICKS           EQU     1
SECOND_TICKS    EQU     (30*TICKS)
TXRX_ALT_TICKS  EQU     (SECOND_TICKS/6)

;
;
; Main Update Routine
;
;  This routine is called once per tick.
;
update:
	    ld	a, PORT_XE13
        call    link_status          ; Off if no link
        call    link_activity        ; Off if no link

	    ld	a, PORT_XE14
        call    link_status          ; Off if no link
        call    link_activity        ; Off if no link

	    ld	a, PORT_XE15
        call    link_status          ; Off if no link
        call    link_activity        ; Off if no link

	    ld	a, PORT_XE16
        call    link_status          ; Off if no link
        call    link_activity        ; Off if no link

; Update various timers
        inc     (TXRX_ALT_COUNT)

        send    NUM_LEDS

;
; link_activity
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: Port number in a
;  Outputs: one bit sent to LED stream
;

link_activity:
        ;jmp     led_on       ;DEBUG ONLY
        call    get_link
        jnc     led_off       ; Orange led off, Link down
; Declaring next pushst comansd to refer to port sttored in a
        port    a
; Pushing RX/TX bits (from bit range 0-15) from ports status range (0x01-0x40) to stack
        pushst  RX
        pushst  TX
; Pop 2 bits from stack, ORing these 2 bits and push the result back to stack
        tor
; Pop 1 bit from stack into Carry flag
        pop

        jnc     led_off       ; Orange led off, No Activity

        ;jmp     led_on       ;DEBUG ONLY

; RX/TX blinking, deciding whether to turn on or of the led
        ld      b, (TXRX_ALT_COUNT)
        and     b, TXRX_ALT_TICKS
        jz      led_on
        jmp     led_off 

;
; link_status
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: Port number in a
;  Outputs: one bit sent to LED stream
;

link_status:
        ;jmp     led_off       ;DEBUG ONLY
        call    get_link
        jnc     led_off        ; Green led off, Link down
        jmp     led_on         ; Green led on , Link up 

;
; get_link
;
;  This routine finds the link status LED for a port.
;  Link info is in bit 0 of the byte read from PORTDATA[port]
;
;  Inputs: Port number in a
;  Outputs: Carry flag set if link is up, clear if link is down.
;  Destroys: b
;

get_link:
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,0
        ret

;
; led_off, led_on
;
;  Inputs: None
;  Outputs: one bit  to the LED stream indicating color
;  Destroys: None
;

led_off:
; push 0 to stack
        pushst  ZERO
; pop top bit of stack and add it to the LED scan chain assembly
        pack
        ret

led_on:
; push 1 to stack
        pushst  ONE
; pop top bit of stack and add it to the LED scan chain assembly
        pack
        ret

;
; Variables (SDK software initializes LED memory from 0xa0-0xff to 0)
;

TXRX_ALT_COUNT  equ     0xe0

        
; Link scan ports data
PORTDATA        equ     0xa0    ; Size (32(ports) bytes)

;
; Symbolic names for the bits of the port status fields
;

RX              equ     0x0     ; received packet
TX              equ     0x1     ; transmitted packet
ZERO            equ     0xE     ; always 0
ONE             equ     0xF     ; always 1


        
;-------------------------- end of program --------------------------
;
;This program is 74 bytes in length

