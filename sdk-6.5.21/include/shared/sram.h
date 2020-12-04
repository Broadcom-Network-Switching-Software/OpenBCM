/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        sram.h
 */
#ifndef _SHR_SRAM_H
#define _SHR_SRAM_H

/* External DDR2_SRAM interface */
#define _SHR_EXT_SRAM_CSE_MODE     0
#define _SHR_EXT_SRAM_HSE_MODE     1

typedef struct _shr_ext_sram_entry_s {
    int    addr0;       /* Address register for data0 (always valid) */
    int    addr1;       /* Address register for data1 (-1 for invalid) */
    uint32 data0[8];    /* Data register 0 */
    uint32 data1[8];    /* Data register 1 */
    int wdoebr;         /* wdoeb_rise */
    int wdoebf;         /* wdoeb_fall */
    int wdmr;           /* write data mask rise */
    int wdmf;           /* write data mask fall */
    int rdmr;           /* read data mask rise */
    int rdmf;           /* read data mask fall */
    int test_mode;      /* 00:WW, 01:RR, 10:WR 11:WW-RR */
    int adr_mode;       /* 00:ALT_A0A1, 01:INC1, 10:INC2, 11:ONLY_A0 */
    int latency;        /* 0:read data in 8 mclks, 1:7 mclks */
    int em_if_type;     /* 1: ext mem if type qdr2, 0: ddr2 type */
    int em_fall_rise;   /* 1: use fall_rise for compare, 0: use rise_fall for compare */
    int w2r_nops;       /* num nops between write to read - for ddr2 */
    int r2w_nops;       /* num nops between read to write - for ddr2 */
    int err_cnt;        /* Error count */
    int err_bmp;        /* Error bit map */
    uint32 err_addr;    /* Error address */
    uint32 err_data[8]; /* Error data */
} _shr_ext_sram_entry_t;

#endif /* _SHR_SRAM_H */
