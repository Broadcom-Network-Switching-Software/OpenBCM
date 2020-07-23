
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _DNXC_UTILS_FPGA_H_
#define _DNXC_UTILS_FPGA_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
#endif

#define SOC_DNX_FPGA_BUFF_SIZE (80 * 1024 * 1024)

#ifdef __DUNE_WRX_BCM_CPU__

#define SOC_DNX_GPOUTDR_PIN_REG_ADDR    0x34108
#define SOC_DNX_GPINDR_PIN_REG_ADDR     0x34110

#define GPIO_CONFDONE_PIN_READ 16
#define GPIO_NCONFIG_PIN_WRITE 12
#define GPIO_DATA0_PIN_WRITE   13
#define GPIO_DCLK_PIN_WRITE    14
#define GPIO_NSTATUS_PIN_READ  11
#define GPIO_CONFDONE_MASK     (1 << GPIO_CONFDONE_PIN_READ)
#define GPIO_NCONFIG_MASK      (1 << GPIO_NCONFIG_PIN_WRITE)
#define GPIO_DATA0_MASK        (1 << GPIO_DATA0_PIN_WRITE)
#define GPIO_DCLK_MASK         (1 << GPIO_DCLK_PIN_WRITE)
#define GPIO_NSTATUS_MASK      (1 << GPIO_NSTATUS_PIN_READ)

#define SOC_DNX_GPOUTDR_NCONFIG_BIT   GPIO_NCONFIG_MASK
#define SOC_DNX_GPOUTDR_DATA_BIT      GPIO_DATA0_MASK
#define SOC_DNX_GPOUTDR_DCLK_BIT      GPIO_DCLK_MASK

#define SOC_DNX_GPINDR_CONF_DONE_BIT  GPIO_CONFDONE_MASK
#define SOC_DNX_GPINDR_NSTATUS_BIT    GPIO_NSTATUS_MASK

#else

#define SOC_DNX_GPOUTDR_PIN_REG_ADDR    0x000E0040
#define SOC_DNX_GPINDR_PIN_REG_ADDR        0x000E0050

#define SOC_DNX_GPOUTDR_NCONFIG_BIT        0x00200000
#define SOC_DNX_GPOUTDR_DATA_BIT        0x00100000
#define SOC_DNX_GPOUTDR_DCLK_BIT        0x00080000

#define SOC_DNX_GPINDR_CONF_DONE_BIT    0x00800000
#define SOC_DNX_GPINDR_NSTATUS_BIT        0x00400000

#endif

typedef struct
{
    int prog_reg_addr;
    int status_reg_addr;
    char conf_done_bit_offset;
} SocDppFpgaRegsMapping;

extern int soc_dnx_fpga_load(
    int unit,
    char *file_name);
#endif
