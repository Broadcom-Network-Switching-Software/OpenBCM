/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Functions for soft error recovery.
 */

#ifndef _SOC_SER_H
#define _SOC_SER_H

#include <soc/chip.h>

#define _SOC_SER_TYPE_REG 0
#define _SOC_SER_TYPE_MEM 1
#define _SOC_SER_TYPE_BUS 2
#define _SOC_SER_TYPE_BUF 3

typedef struct soc_ser_info_s {
    _soc_mem_ser_en_info_t *ip_mem_ser_info;
    _soc_reg_ser_en_info_t *ip_reg_ser_info;
    _soc_mem_ser_en_info_t *ep_mem_ser_info;
    _soc_reg_ser_en_info_t *ep_reg_ser_info;
    _soc_mem_ser_en_info_t *mmu_mem_ser_info;
    _soc_mem_ser_en_info_t *macsec_mem_ser_info;
    _soc_reg_ser_en_info_t *macsec_reg_ser_info;
    _soc_bus_ser_en_info_t *ip_bus_ser_info;
    _soc_buffer_ser_en_info_t *ip_buffer_ser_info;
    _soc_bus_ser_en_info_t *ep_bus_ser_info;
    _soc_buffer_ser_en_info_t *ep_buffer_ser_info;
} soc_ser_info_t;

#ifdef BCM_CMICX_SUPPORT
extern void soc_cmicx_parity_intr(int unit, void *val);
#endif

extern int soc_ser_single_bit_report_for_bus_buf_set(int unit, int enable);
extern int soc_ser_single_bit_report_for_bus_buf_get(int unit, int *enable);

#endif /* _SOC_SER_H */
