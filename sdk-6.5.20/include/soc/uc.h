/* 
 * $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        uc.h
 */

#ifndef _SOC_UC_H_
#define _SOC_UC_H_

#include <sal/core/time.h>
#include <soc/types.h>

extern int soc_uc_init(int unit);
extern int soc_uc_reset(int unit, int uC);
extern int soc_uc_in_reset(int unit, int uC);
extern int soc_uc_preload(int unit, int uC);
extern int soc_uc_load(int unit, int uC, uint32 addr, int len,
                       unsigned char *data);
extern int soc_uc_start(int unit, int uC, uint32 addr);

extern char *soc_uc_firmware_version(int unit, int uC);

extern uint32 soc_uc_mem_read(int unit, uint32 addr);
extern int soc_uc_mem_write(int unit, uint32 addr, uint32 value);

extern int soc_uc_sram_extents(int unit, uint32 *addr, uint32 *size);

extern char *soc_uc_firmware_thread_info(int unit, int uC);

extern int soc_uc_stats_reset(int unit, int uC);
extern int soc_uc_console_log(int unit, int uC, int on);

extern int soc_uc_ping(int unit, int uC, sal_usecs_t timout);
extern int soc_uc_status(int unit, int uC, int *status);

extern int soc_uc_addr_to_pcie(int unit, int uC, uint32 addr);
#endif
