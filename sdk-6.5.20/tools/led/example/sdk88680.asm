; $Id: sdk88680.asm,v 1.2 2016/12/27 06:52:51 junjund Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
;
;
;This is an example program for programming the LEDs for 88680 (Jericho/QMX) chip.
;This example activates the LEDs on the NEGEV chassis.
;
;Negev chassis configuration:
;       In Negev chassis front panel there are 22 ports, 2 x 6 x 100G + 4 x 40G + 2 x 20G + 4 x 10G
;       There are 3 LED processors.
;       LED processor 0 - 6 x 100G Falcon (PMH - port macro high)
;       LED processor 1 - 4 x 40G + 2 x 20G + 4 x 10G Eagle (PML0 - port macro low 0)
;       LED processor 2 - 6 x 100G Falcon (PML1 - port macro low 1)
;
;       The 18 ports have one single green LED per port.
;       The single led will indicate about LINK, TX, RX status.
;		 led off      - link down
;		 led on       - link up and no traffic.
;		 led blinking - link up and traffic is transmitted/received.
;
;
;led processor DATA RAM configuration:
; 
; Offset 0x00-0x7f
; Ports status: maximum 64 ports, 2 bytes per port.
;
; Offset 0x80 (6 bits LED scan chain, requires only 1 byte in data memory)
; LED scan chain assembly
;
; Offset 0xa0 - 0xdf
; Link up/down: maximum 64 ports, one byte per port, only lsb (bit 0) of each byte is valid,
; indicating whether the link is up(1) or down(0).
; Link up/down port data is continuously updated  by main CPU's via linkscan module (See $SDK/src/appl/diag/ledproc.c) .
;
; Each led processor can handle maximum 64 ports, indexed 0-63.
;  88670 chip driver use physical ports (phy_port) indexing, physical ports are indexed 1-144 (0 reserved for CPU port).
;  valid physical port numbers are not sequential (since there are only 72 SerDes), i.e. have gaps.
;  3 led processors handle following physical ports ranges:
;   LED processor 0 - handles ports  1-24.  port 1  resides in index 1 in data ram (for both "Ports status" and "Link up/down" data sections)
;   LED processor 1 - handles ports 25-84.  port 25 resides in index 1 in data ram (for both "Ports status" and "Link up/down" data sections)
;   LED processor 2 - handles ports 85-144. port 85 resides in index 1 in data ram (for both "Ports status" and "Link up/down" data sections)
; 
; Negev front panel 22 ports are configured as following:
; 6 port block:     PML0(4 x 40G + 2 x 20G + 4 x 10G)               PMH(100G)                                  PML1(100G)
; Led proc:                1                                            0                                          2
;           -----------------------------------------------------------------------------------------------------------------------
; phy_port: |    25   |   37    |   73    |  65 | 69  |  |   17    |    9    |    1    |            |  113    |   93    |   85    |
;           -----------------------------------------------------------------------------------------------------------------------
; led index:  /10\ \9/  /8\ \7/  /6\ \5/ /4\\3//2\ \1/|   /6\   \5/ /4\   \3/ /2\   \1/              /6\   \5/ /4\   \3/ /2\   \1/
;           -----------------------------------------------------------------------------------------------------------------------
; phy_port: |    33   |   29    |   57    |  53 | 77  |  |   21    |   13    |    5    |            |  129    |   97    |   89    |
;           -----------------------------------------------------------------------------------------------------------------------
;
;
; To get port led processor data ram indexing, "led status" command can be used from BCM shell.
; For example:
;  BCM.0> led status
;   1  ce1                 100 1G FD 0:1
;  13 xe13                 100 1G FD 2:29
;  14 xe14                 100 1G FD 2:33
;  15 xe15                 100 1G FD 2:45
;  16 xe16                 100 1G FD 2:49
;  17 ce17                 100 1G FD 0:5
; 
; In example above port 13(xe13) led value is "2:29", 
; which means it is handled by led processor 2 and resides in index 29 in data ram (physical port index is 113 [=29+84])

;-------------------------- start of program --------------------------

; Device ports
; led processor 0
;led index                port  phy_port
;PHY_PORT_LED1       EQU     5 ;  5
;PHY_PORT_LED2       EQU     1 ;  1
;PHY_PORT_LED3       EQU    13 ; 13
;PHY_PORT_LED4       EQU     9 ;  9
;PHY_PORT_LED5       EQU    21 ; 21
;PHY_PORT_LED6       EQU    17 ; 17

; led processor 1
; To compile led processor 1 program, comment out led processors 0/2 values and uncomment following 10 values.
;led index                 port index in DATA RAM; phy_port
PHY_PORT_LED1       EQU    53                   ; 77 (minus 24 for data RAM index)
PHY_PORT_LED2       EQU    45                   ; 69 (minus 24 for data RAM index)
PHY_PORT_LED3       EQU    29                   ; 53 (minus 24 for data RAM index)
PHY_PORT_LED4       EQU    41                   ; 65 (minus 24 for data RAM index)
PHY_PORT_LED5       EQU    33                   ; 57 (minus 24 for data RAM index)
PHY_PORT_LED6       EQU    49                   ; 73 (minus 24 for data RAM index)
PHY_PORT_LED7       EQU     5                   ; 29 (minus 24 for data RAM index)
PHY_PORT_LED8       EQU    13                   ; 37 (minus 24 for data RAM index)
PHY_PORT_LED9       EQU     9                   ; 33 (minus 24 for data RAM index)
PHY_PORT_LED10      EQU     1                   ; 25 (minus 24 for data RAM index)

; led processor 2
; To compile led processor 2 program, comment out 6 led processors 0/1 values and uncomment following 6 values.
;led index                 port index in DATA RAM; phy_port
;PHY_PORT_LED1       EQU     5                   ;  89 (minus 84 for data RAM index)
;PHY_PORT_LED2       EQU     1                   ;  85 (minus 84 for data RAM index)
;PHY_PORT_LED3       EQU    13                   ;  97 (minus 84 for data RAM index)
;PHY_PORT_LED4       EQU     9                   ;  93 (minus 84 for data RAM index)
;PHY_PORT_LED5       EQU    45                   ; 129 (minus 84 for data RAM index)
;PHY_PORT_LED6       EQU    29                   ; 113 (minus 84 for data RAM index)

;NUM_LEDS        EQU     6       ;6 ports, 1 LED per port, total 6 bits
NUM_LEDS        EQU     10       ;10 ports, 1 LED per port, total 10 bits PML0

; The TX/RX activity lights will blink TICKS_PER_SECOND/TXRX_TICKS per second (30/6 = 5 blinks per second).
; LED program is called 30 times per second
TICKS_PER_SECOND  EQU     (30)
TXRX_TICKS        EQU     (6)

;
;
; Main Update Routine
;
;  This routine is called once per tick.
;
update:
	    ld	a, PHY_PORT_LED1
        call    update_single_led

	    ld	a, PHY_PORT_LED2
        call    update_single_led

	    ld	a, PHY_PORT_LED3
        call    update_single_led

	    ld	a, PHY_PORT_LED4
        call    update_single_led

	    ld	a, PHY_PORT_LED5
        call    update_single_led

	    ld	a, PHY_PORT_LED6
        call    update_single_led

	    ld	a, PHY_PORT_LED7
        call    update_single_led

	    ld	a, PHY_PORT_LED8
        call    update_single_led

	    ld	a, PHY_PORT_LED9
        call    update_single_led

	    ld	a, PHY_PORT_LED10
        call    update_single_led

; Update various timers
        inc     (TXRX_BLINK_COUNT)

        send    NUM_LEDS

;
; link_activity
;
;  This routine calculates the activity LED for the current port.
;  It extends the activity lights using timers (new activity overrides
;  and resets the timers).
;
;  Inputs: Port number in register a
;  Outputs: one bit sent to LED stream
;

link_activity:
; Declaring next pushst commands to refer to port stored in register a
        port    a
; Pushing RX/TX bits (from bit range 0-15) from ports status range (0x02-0x30) to stack
        pushst  RX
        pushst  TX
; Pop 2 bits from stack, ORing these 2 bits and push the result back to stack
        tor
; Pop 1 bit from stack into Carry flag
        pop

        jnc     led_on       ; in case of No traffic Activity, setting led to on

        ;jmp     led_on       ;DEBUG ONLY

; Otherwise (RX/TX activity) blinking the led, deciding whether to turn on or of the led
        ld      b, (TXRX_BLINK_COUNT)
; In case TXRX_TICKS bits are all '0', turning on led.
        and     b, TXRX_TICKS
        jz      led_on
        jmp     led_off

;
; update_single_led
;
;  This routine will store one led bit in led chain according to port in register a.
;  It uses a, b registers and the carry flag.
;
;  Inputs: Port number in register a
;  Outputs: one bit sent to LED stream
;

update_single_led:
        ;jmp     led_off       ;DEBUG ONLY
        call    get_link
        jnc     led_off        ; In case carry flag is 1     - Green led off, Link down
        jmp     link_activity  ; Otherwize (carry flag is 0) - Green led on , Link up 

;
; get_link
;
;  This routine stores the link status of register a port into the carry flag.
;  Link info input is bit 0 of PORTDATA[port] byte
;
;  Inputs: Port number in register a
;  Outputs: Carry flag is set(1) if link is up, cleared(0) if link is down.
;  Destroys: register b
;

get_link:
        ld      b,PORTDATA
        add     b,a
        ld      b,(b)
        tst     b,0     ;  Sets Carry flag with content of bit 0 of register b
        ret

;
; led_off, led_on
;
;  Inputs: None
;  Outputs: one bit to the LED stream
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

TXRX_BLINK_COUNT equ     0xe0


; Link scan ports data
PORTDATA         equ     0xa0    ; Size is 24 (ports) bytes

;
; Symbolic names for the bits of the port status fields
;

RX               equ     0x0     ; received packet
TX               equ     0x1     ; transmitted packet
ZERO             equ     0xE     ; lsb is 0
ONE              equ     0xF     ; lsb is 1



;-------------------------- end of program --------------------------
;
;This program is 66 bytes in length

