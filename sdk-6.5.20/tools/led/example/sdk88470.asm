; $Id: sdk88470.asm,v 1.2 2015/09/24 06:52:51 junjund Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
;This is an example program for programming the LEDs for 88470 chip.
;This example activates the LEDs on the NEGEV chassis.
;
;Negev chassis configuration:
;       In Negev chassis front panel there are 14 ports, 4 x 10G + 7 x 40G + 1 x 20G + 2 x 10G
;       There are 1 LED processors, The LED processor will activate 14 ports LEDs:
;       LED processor 0 - 4 x 100G Falcon (PMH - port macro high) + 
;                       - 4 x 40G Eagle (PML0 - port macro low 0)
;                       - 3 x 40G  + 1 x 20G + 2 x 10G Eagle (PML1 - port macro low 1)  
;
;       The 14 ports have one single green LED per port.
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
;  88470 chip driver use SerDes_port(SerDesID + 1,0 reserved for CPU port ) indexing, SerDes_port are indexed 1-48 (0 reserved for CPU port).
;  All port are connect to LED0 chain                                              
;  nif port :                                                                      
;  68->72->76->80  -> 84->88->92->96 -> 100->104->108->112 -> 116->117->118->119-> 
;  0->1->2->3 ->      4->5->6->7 ->     8->9->10->11 ->       12->13->14->15 ->    
;  16->20->24->28  -> 32->36->40->44 -> 48->52->56->60 ->     64->65->66->67 ->    
;  SerDes port :                                                                   
;  32->33->34->35  -> 36->37->38->39 -> 40->41->42->43 ->     44->45->46->47 ->    
;  0->1->2->3 ->      4->5->6->7 ->     8->9->10->11 ->       12->13->14->15 ->    
;  16->17->18->19  -> 20->21->22->23 -> 24->25->26->27 ->     28->29->30->31 ->    
;  Data Ram location:                                                              
;  33->34->35->36  -> 37->38->39->40 ->  41->42->43->44->      45->46->47->48 ->   
;  1->2->3->4 ->      5->6->7->8->       9->10->11->12 ->      13->14->15->16 ->   
;  17->18->19->20  -> 21->22->23->24 ->  25->26->27->28 ->     29->30->31->32 -> 
; 
; Negev front panel 14 ports are configured as following:
; 6 port block:        PML0(40G)               PMH(100G)                       PML1(3x 40G + 1 x 20G + 2 x 10G)            
; Led proc:                0                      0                                       0             
;           ----------------------------------------------------------------------------------------------
; SerDes_port: |    29   |   21    |         |   13   |    5    |            |  33    |   41    |   47   |  
;           ----------------------------------------------------------------------------------------------
; led index:    /14\  \13/ /12\  \11/        /10\  \9/ /8\   \7/              /6\  \5/ /4\  \3/   /2\  \1/  
;           ----------------------------------------------------------------------------------------------
; SerDes_port: |    25   |   17    |         |   9    |    1    |            |  37    |   45    |   48   |  
;           ----------------------------------------------------------------------------------------------
; 
; 
; To get port led processor data ram indexing, "led status" command can be used from BCM shell.
; For example:
        
;-------------------------- start of program --------------------------
; Not used in the program, added for declaring ARAD phisical ports range
MIN_XE_PORT     EQU     1
MAX_XE_PORT     EQU     48

; Device ports
; led processor 0
;led index                port  SerDes port
PHY_PORT_LED1       EQU    48 ; 48
PHY_PORT_LED2       EQU    47 ; 47
PHY_PORT_LED3       EQU    45 ; 45
PHY_PORT_LED4       EQU    41 ; 41
PHY_PORT_LED5       EQU    37 ; 37
PHY_PORT_LED6       EQU    33 ; 33
PHY_PORT_LED7       EQU     1 ;  1
PHY_PORT_LED8       EQU     5 ;  5
PHY_PORT_LED9       EQU     9 ;  9
PHY_PORT_LED10      EQU    13 ; 13
PHY_PORT_LED11      EQU    17 ; 17
PHY_PORT_LED12      EQU    21 ; 21
PHY_PORT_LED13      EQU    25 ; 25
PHY_PORT_LED14      EQU    29 ; 29


NUM_LEDS        EQU     14       ;14 ports, 1 LED per port, total 14 bits
        
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

	    ld	a, PHY_PORT_LED11
        call    update_single_led

	    ld	a, PHY_PORT_LED12
        call    update_single_led

	    ld	a, PHY_PORT_LED13
        call    update_single_led

	    ld	a, PHY_PORT_LED14
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
PORTDATA         equ     0xa0    ; Size is 48 (ports) bytes

;
; Symbolic names for the bits of the port status fields
;

RX               equ     0x0     ; received packet
TX               equ     0x1     ; transmitted packet
ZERO             equ     0xE     ; lsb is 0
ONE              equ     0xF     ; lsb is 1


        
;-------------------------- end of program --------------------------
;
;This program is 98 bytes in length


