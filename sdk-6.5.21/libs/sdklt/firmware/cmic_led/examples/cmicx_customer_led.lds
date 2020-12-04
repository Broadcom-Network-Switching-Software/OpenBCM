/*! \file cmicx_customer_led.lds
 *
 * Linker script for CMICx LED bit pattern composer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(customer_led_handler)
SECTIONS
{
    /* customer_led_handler is at 0x3800 by default */
    . = 0x3800;

    . = ALIGN(4);
    .text    :
    {
        *(.text)
        *(.data)
        *(.bss)
    }

    /DISCARD/ : { *(.dynstr*) }
    /DISCARD/ : { *(.dynamic*) }
    /DISCARD/ : { *(.plt*) }
    /DISCARD/ : { *(.interp*) }
    /DISCARD/ : { *(.gnu*) }
}
