/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ukernel_debug.h
 * Purpose:  
 */

#ifndef _UC_DBG_H
#define _UC_DBG_H

#include <bcm/types.h>
#include <bcm/error.h>
#include <soc/shared/mos_msg_common.h>

#if defined(SOC_UKERNEL_DEBUG)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

#define CMIC_UC_DBG_PTP_CORE_COMM_SUCCESS       (1<<0)
#define CMIC_UC_DBG_PTP_STACK_INIT_SUCCESS      (1<<1)

#define CMIC_UCDEBUG_OUTPUT_DISP_TIMEOUT        (1000)

typedef struct _soc_cmic_ucdbg_stats_s {
    uint32                  flags;
    uint32                  sw_intr_count_uc0;
    uint32                  sw_intr_count_uc1;
    uint32                  hostlog_indx_wr;
    uint32                  hostlog_indx_rd;
    uint32                  uclog_indx_rd;
    mos_msg_cmic_ucdbg_t    ucdbg_cache; /* memory cache of SRAM data struct */
    mos_msg_ucdbg_entry_t   hostlog[MOS_UCDBG_MAX_LOG_ENTRIES_HOST]; /* host log buff pointer */
    mos_msg_ucdbg_entry_t   *puclog[MOS_UCDBG_MAX_LOG_ENTRIES_UC]; /* uc log buffer pointer */
}_soc_cmic_ucdbg_stats_t;

extern int 
soc_cmic_ucdebug_init(int unit, int uC);

extern int 
soc_cmic_ucdebug_sw_intr_count_inc(int unit, uint32 rupt_num);


extern int 
soc_cmic_ucdebug_core_communication_status(int unit, uint32 flag);

extern int 
soc_cmic_ucdebug_dump_start(int unit);

extern int 
soc_cmic_ucdebug_dump_stop(int unit);

extern int 
soc_cmic_ucdebug_status(int unit);

extern int
soc_cmic_ucdebug_log_add(int unit, mos_msg_ucdbg_logtype_t logtype,
                     const char *format, ...);

#define SOC_CMIC_UCDBG_LOG_ADD(stuff_) \
    soc_cmic_ucdebug_log_add stuff_;

#else

#define soc_cmic_ucdebug_init(unit, uC)
#define soc_cmic_ucdebug_sw_intr_count_inc(unit, rupt_num)
#define soc_cmic_ucdebug_core_communication_status(unit, flag)
#define soc_cmic_ucdebug_dump_start(unit)
#define soc_cmic_ucdebug_dump_stop(unit)
#define soc_cmic_ucdebug_status(unit)

#define SOC_CMIC_UCDBG_LOG_ADD(stuff_)

#endif

#else

#define soc_cmic_ucdebug_init(unit, uC)
#define soc_cmic_ucdebug_sw_intr_count_inc(unit, rupt_num)
#define soc_cmic_ucdebug_core_communication_status(unit, flag)
#define soc_cmic_ucdebug_dump_start(unit)
#define soc_cmic_ucdebug_dump_stop(unit)
#define soc_cmic_ucdebug_status(unit)

#define SOC_CMIC_UCDBG_LOG_ADD(stuff_)

#endif

#endif /*_UKERNEL_DEBUG_H_*/
