;
; $Id: sdk5665.asm,v 1.8 2012/03/02 14:34:03 yaronm Exp $
;
; This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
; 
; Copyright 2007-2020 Broadcom Inc. All rights reserved.
; 
;
; This is the default program for the 5665 SDK (BCM95665K4S)
;
; To start it, use the following commands from BCM:
;
;	0:led load sdk5665.hex
;	*:led start
;
; The BCM5665 SDK has 26 columns of 4 LEDs each, as shown below:
;
;       L01 L03 L05 L07 L09 L11 L13 L15 L17 L19 ... L47 LG1 LG3 
;       A01 A03 A05 A07 A09 A11 A13 A15 A17 A19 ... A47 AG1 LG3
;       L02 L04 L06 L08 L10 L12 L14 L16 L18 L20 ... L48 LG2 LG4
;       A02 A04 A06 A08 A10 A12 A14 A16 A18 A20 ... A48 AG2 LG4
;
; There is one bit per LED with the following colors:
;	ZERO		Green
;	ONE		Black
;
; The bits are shifted out in the following order:
;	A01, L01, ..., A48, L48, AG1, LG1, ..., AG4, LG4
;
; Current implementation:
;
; L01 reflects port 1 link status
; A01 reflects port 1 activity
;
; On the bringup board, the LED polarity is 0 on, 1 off.
;
;
        
        
MIN_FE_PORT	EQU	0
BREAK_FE_PORT	EQU	24
RESUME_FE_PORT	EQU	32       
MAX_FE_PORT	EQU	55
STOP_FE_PORT	EQU	56
NUM_FE_PORT	EQU	48

MIN_GE_PORT	EQU	24
MAX_GE_PORT	EQU	27
STOP_GE_PORT	EQU	28
NUM_GE_PORT	EQU	4

MIN_PORT	EQU	0
MAX_PORT	EQU	55
NUM_PORT	EQU	52

PACK_SRC        EQU     0
PACK_DST        EQU     128
PACK_NUM        EQU     13
        
;
; Main Update Routine
;
;  This routine is called once per tick.
;

update:
	sub	a,a		; ld a,MIN_FE_PORT (but only one instr)

up1:    
        call    doport

     	cmp	a,BREAK_FE_PORT
	jnz	up1

        ld      a,RESUME_FE_PORT
        
up2:    
        call    doport        
	cmp	a,STOP_FE_PORT
	jnz	up2

        ld      a,MIN_GE_PORT
        
up3:    
        call    doport        
	cmp	a,STOP_GE_PORT
	jnz	up3

;
; This bit is a workaround for a bug in the 5665 LED processor.
; 
; The pack instruction should put the data into data mem at 0x80.
; However, the internal register to store this address is only 7 bits.
; So the data is packed starting at address 0.
; This leads to two caveats:    
; 
; 1) Port 0 data _MUST_ be consumed before the first pack instruction.
; 
; 2) The output data must be moved from location 0 to location 128.
; 
; 
    
                
        sub     b,b             ; ld b,PACK_SRC (but only one instr)
        ld      a,PACK_DST
packmove:
        ld      (a),(b)
        inc     a
        inc     b
        cmp     b,PACK_NUM
        jnz     packmove
        
        send    104             ;  (48 + 4) * 2 bits

doport:
	port	a
	ld	(PORT_NUM),a

	call	link_status	; Lower LED for this port
	call	activity	; Upper LED for this port

        ; We need to cache the output bits to not stomp Port 0 data.
        ld      a,(LINK_CACHE)
        pushst  a
        pack
        
        ld      a,(ACT_CACHE)
        pushst  a
        pack
        
	ld	a,(PORT_NUM)
	inc	a
        ret

activity:
        pushst  RX
        pop
        jc      act_led_set

        pushst  TX
        pop
        jc      act_led_set

        ld      a,ONE             ; Off
        ld      (ACT_CACHE),a
        ret

act_led_set:
        ld      a,ZERO            ; On
        ld      (ACT_CACHE),a
        ret

link_status:

	ld	a,(PORT_NUM)	; Check for link down
	ld	b,PORTDATA
	add	b,a
	ld	b,(b)
	tst	b,0

        jnc     ls_led_clear

        ld      a,ZERO            ; On
        ld      (LINK_CACHE),a
        ret

ls_led_clear:
        ld      a,ONE             ; Off
        ld      (LINK_CACHE),a
        ret


;
; Variables (SDK software initializes LED memory from 0x80-0xff to 0)
;

TXRX_ALT_COUNT	equ	0xe0
HD_COUNT	equ	0xe1
GIG_ALT_COUNT	equ	0xe2
PORT_NUM	equ	0xe3
LINK_CACHE      equ     0xe4
ACT_CACHE       equ     0xe5

;
; Port data, which must be updated continually by main CPU's
; linkscan task.  See $SDK/src/appl/diag/ledproc.c for examples.
; In this program, bit 0 is assumed to contain the link up/down status.
;

PORTDATA	equ	0xa0	; Size 57 bytes

;
; Symbolic names for the bits of the port status fields
;

RX		equ	0x0	; received packet
TX		equ	0x1	; transmitted packet
COLL		equ	0x2	; collision indicator
SPEED_C		equ	0x3	; 100 Mbps
SPEED_M		equ	0x4	; 1000 Mbps
DUPLEX		equ	0x5	; half/full duplex
FLOW		equ	0x6	; flow control capable
LINKUP		equ	0x7	; link down/up status
LINKEN		equ	0x8	; link disabled/enabled status
ZERO		equ	0xE	; always 0
ONE		equ	0xF	; always 1
