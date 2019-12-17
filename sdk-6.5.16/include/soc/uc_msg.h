/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    uc_msg.h
 */

#ifndef _SOC_UC_MSG_H
#define _SOC_UC_MSG_H

/* Define how the shared msg headers find certain items */
#include <soc/shared/mos_msg_common.h>

/* Macros to set and clear the S/W interrupt bits in the correct CMC */
#define CMIC_CMC_SW_INTR_SET(unit, target)                              \
    soc_pci_write(unit,                                                 \
                  (target == CMICM_SW_INTR_HOST) ? CMIC_CMC0_SW_INTR_CONFIG_OFFSET : \
                  (target == CMICM_SW_INTR_UC0) ? CMIC_CMC1_SW_INTR_CONFIG_OFFSET : \
                  (target == CMICM_SW_INTR_UC1) ? CMIC_CMC2_SW_INTR_CONFIG_OFFSET : \
                  CMIC_CMC2_SW_INTR_CONFIG_OFFSET,                      \
                  0x4 | ((target) == CMICM_SW_INTR_RCPU?                \
                    CMICM_SW_INTR_RCPU : CMICM_SW_INTR_HOST))

/* We only clear our own - but clear the right target */
#define CMIC_CMC_SW_INTR_CLEAR(unit, target)                            \
    soc_pci_write(unit, CMIC_CMC0_SW_INTR_CONFIG_OFFSET, target)

#ifdef BCM_CMICX_SUPPORT
/* Macro to set the S/W interrupt bits as per correct target */
#define CMICX_SW_INTR_SET(unit, target) do { \
    if (target == CMICM_SW_INTR_UC0) { \
        WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, (1<<1)); \
    } else { \
        WRITE_ICFG_CMIC_RCPU_SW_PROG_INTRr(unit, (1<<2)); \
    } \
} while (0)
#endif

#define CMIC_MSG_PBUF_SIZE      256

/* seconds / nanoseconds time, corresponding to TAI/UTC/PTP/... time */
typedef struct soc_cmic_uc_time_s {
    uint64 seconds;
    uint32 nanoseconds;
} soc_cmic_uc_time_t;

/* Timestamp data associated with a particular event type */
typedef struct soc_cmic_uc_ts_data_s {
    uint64 hwts;              /* Most recent timestamp for event type      */
    uint64 prev_hwts;         /* Preceeding timestamp for event type       */
    uint64 hwts_ts1;          /* Corresponding TS1 timestamp (IPROC/CMICd) */
    soc_cmic_uc_time_t time;  /* Corresponding full time from firmware     */
} soc_cmic_uc_ts_data_t;

/* Callback to application code to indicate that messaging/uC is stopping */
typedef enum soc_cmic_uc_shutdown_stage_e {
    SOC_CMIC_UC_SHUTDOWN_STARTING = 1, /* about to shut down */
    SOC_CMIC_UC_SHUTDOWN_HALTED,       /* uC core is halted  */
    SOC_CMIC_UC_SHUTDOWN_NOHALT        /* Apps should run without host */
} soc_cmic_uc_shutdown_stage_t;

typedef int (soc_cmic_uc_appl_cb_t)(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data);

/* Routines */
extern void soc_cmic_sw_intr(int unit, uint32 rupt_num);
extern void soc_cmic_uc_msg_init(int unit);
extern int soc_cmic_uc_msg_start(int unit);
extern int soc_cmic_uc_msg_stop(int unit);
extern int soc_cmic_uc_msg_shutdown_halt(int unit);
extern int soc_cmic_uc_msg_uc_shutdown_halt(int unit, int uC);
extern int soc_cmic_uc_msg_apps_notify(int unit,
               soc_cmic_uc_shutdown_stage_t notification);
extern int soc_cmic_uc_msg_uc_apps_notify(int unit,
               soc_cmic_uc_shutdown_stage_t notification, int uC);
extern int soc_cmic_uc_msg_active_wait(int unit, uint32 host);
extern void soc_cmic_uc_msg_thread(void *unit_vp);
extern int soc_cmic_uc_msg_uc_start(int unit, int uC);
extern int soc_cmic_uc_msg_uc_stop(int unit, int uC);
extern int soc_cmic_uc_msg_send(int unit, int uC, mos_msg_data_t *msg,
                                sal_usecs_t timeout);
extern int soc_cmic_uc_msg_receive(int unit, int uC, uint8 mclass,
                                   mos_msg_data_t *msg, int timeout);
extern int soc_cmic_uc_msg_receive_cancel(int unit, int uC, int msg_class);
extern int soc_cmic_uc_msg_send_receive(int unit, int uC, mos_msg_data_t *send,
                                        mos_msg_data_t *reply,
                                        sal_usecs_t timeout);
extern int soc_cmic_uc_appl_init(int unit, int uC, int msg_class,
                                 sal_usecs_t timeout, uint32 version_info,
                                 uint32 min_appl_version,
                                 soc_cmic_uc_appl_cb_t *shutdown_cb,
                                 void *cb_data);

#if defined (INCLUDE_UC_ULINK)
extern int soc_cmic_uc_pkd_link_notify(int unit);
#endif

#ifdef BCM_KATANA2_SUPPORT
extern uint32 soc_cmic_bs_reg_cache(int unit, int bs_num);
#endif

extern int soc_cmic_uc_msg_timestamp_get(int unit,  int event_id,
                                         soc_cmic_uc_ts_data_t *ts_data);

extern int soc_cmic_uc_msg_timestamp_enable(int unit,  int event_id);
extern int soc_cmic_uc_msg_timestamp_disable(int unit,  int event_id);

extern uint32 soc_cmic_timestamp_base(int unit);

#ifdef BCM_CMICX_SUPPORT
extern int soc_cmicx_uc_msg_init(int unit);
#endif  /* BCM_CMICX_SUPPORT */

#endif
