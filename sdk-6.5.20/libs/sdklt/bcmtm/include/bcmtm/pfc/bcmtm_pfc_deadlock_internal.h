/*! \file bcmtm_pfc_deadlock_internal.h
 *
 * Definitions for PFC deadlock handler structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_PFC_DEADLOCK_INTERNAL_H
#define BCMTM_PFC_DEADLOCK_INTERNAL_H

#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <shr/shr_thread.h>
#include <sal/sal_mutex.h>
#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>

/*! PFC DDR thread scan interval. */
#define PFC_SCAN_INTERVAL 100000

/*! The state types of PFC DDR thread internal state. */
typedef enum {
    INVALID  = -1,
    INIT     = 0,
    ACTIVE   = 1,
    SHUTDOWN = 2
} state_type_t;

/*! PFC deadlock control structure. */
typedef struct bcmtm_pfc_ddr_ctrl_s {

   /*! PFC DDR Info Synchronization. */
    sal_mutex_t info_lock;

   /*! PFC DDR thread PID. */
    shr_thread_t pid;

   /*! PFC DDR thread internal state. */
    state_type_t state;

   /*! Recovery count down for a given priority. */
    uint32_t recovery_count[BCMDRD_CONFIG_MAX_PORTS][MAX_NUM_PFC_PRI];
} bcmtm_pfc_ddr_ctrl_t;

/*!
 * Device specific function pointers for physical table configuration
 * for PFC deadlock handling.
 */
typedef int (*bcmtm_pfc_deadlock_process_f) (int unit);

/*!
 * \brief TM PFC DDR LT field values set.
 */
typedef int (*bcmtm_pfc_ddr_field_commit_f)(int unit,
                                        uint32_t field_id,
                                        uint32_t lport,
                                        uint32_t pfc_pri,
                                        uint32_t value);
/*!
 * \brief TM PFC DDR status get.
 */
typedef int (*bcmtm_pfc_ddr_deadlock_status_get_f)(int unit,
                                        int32_t lport,
                                        uint32_t *mask,
                                        uint32_t *status);

/*!
 * \brief TM PFC DDR recovery function.
 */
typedef int (*bcmtm_pfc_ddr_recovery_trigger_f)(int unit,
                                        int32_t lport,
                                        int32_t pfc_pri);

/*! Device specific functions. */
typedef struct bcmtm_pfc_ddr_drv_s {

    /*! Function for hardware programming. */
    bcmtm_pfc_ddr_field_commit_f pt_set;

    /*! Function for getting the deadlock status from hardware. */
    bcmtm_pfc_ddr_deadlock_status_get_f hw_status_get;

    /*! Function for starting the deadlock recovery. */
    bcmtm_pfc_ddr_recovery_trigger_f hw_recovery_begin;

    /*! Function for ending the deadlock recovery. */
    bcmtm_pfc_ddr_recovery_trigger_f hw_recovery_end;
} bcmtm_pfc_ddr_drv_t;

/*! Data structure to save the input from TM_PFC_PRI_TO_COS_MAP LT entry. */
typedef struct tm_pfc_pri_to_cos_map_config_s {

    /*! PFC Enable/disable control for given PFC priority. */
    uint32_t pfc_enable;

    /*! Optimized PFC Enable/disable control for it. */
    uint32_t pfc_optimized;

    /*! Cos list controlled by this priority. */
    uint32_t cos_list_bmp;

    /*! Optimized PFC Enable/disable operational value. */
    uint32_t optimized_opr;
} tm_pfc_pri_to_cos_map_config_t;

/*!
 * \brief TM PFC ingress port set.
 */
typedef int (*bcmtm_pfc_egr_port_set_f)(int unit,
                                        bcmltd_sid_t sid,
                                        bcmpc_lport_t lport,
                                        uint32_t pfc_enable,
                                        uint32_t profile_id);
/*!
 * \brief TM PFC Rx PFC priority to cos map set.
 */
typedef int (*bcmtm_pfc_rx_pri_cos_map_set_f)(int unit,
                                     int32_t profile_id,
                                     tm_pfc_pri_to_cos_map_config_t map[]);

/*!
 * \brief Get max optimized groups in PFC priority to cos map profile.
 */
typedef int (*bcmtm_pfc_profile_num_opt_group_get_f)(int unit,
                                      int profile_id, int *num);

/*!
 * \brief Get max PFC optimized groups supported by a port.
 */
typedef void (*bcmtm_pfc_port_num_opt_group_get_f)(int unit,
                                     int lport, int *num);
/*!
 * \brief TM PFC call back functions.
 */
typedef struct bcmtm_pfc_drv_s{

    /*! TM PFC ingress port set. */
    bcmtm_pfc_egr_port_set_f egr_port_set;

    /*! TM PFC Rx Priority to Cos map set. */
    bcmtm_pfc_rx_pri_cos_map_set_f rx_pri_cos_map_set;
} bcmtm_pfc_drv_t;


/*!
 * \brief TM_PFC_INGt Logical table internal updates.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] lport Logical port ID.
 * \param [in] action Internal updates action.
 */
extern int
bcmtm_pfc_port_internal_update(int unit,
                               bcmtm_lport_t lport,
                               uint8_t action);

/*!
 * \brief PFC deadlock detection and recovery resource initialization.
 *
 * Allocates memory for internal data structures.

 * \param [in] unit Unit number.
 * \param [in] warm Indicates boot mode.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM programming errors.
 */
extern int
bcmtm_pfc_deadlock_monitor_init(int unit, bool warm);

/*!
 * \brief PFC deadlock detection and recovery resource clean up.
 *
 * Frees up the memory allocated for internal data structures.

 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM programming errors.
 */

extern int
bcmtm_pfc_deadlock_monitor_cleanup(int unit);

/*!
 * \brief Stops the PFC deadlock detection and recovery monitor process.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM programming errors.
 */
extern int
bcmtm_pfc_deadlock_monitor_stop(int unit);

/*!
 * \brief To populate the TM_DEADLOCK_RECOVERY_STATUS LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX  LT populate failed.
 */
int
bcmtm_pfc_deadlock_recovery_status_imm_populate(int unit);

/*!
 * \brief To populate the TM_DEADLOCK_RECOVERY_STATUS LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] pri PFC priority.
 * \param [in] recovery deadlock recovery state.
 * \param [in] deadlock_hdlr true for updates from deadlock monitor thread.
 * \param [in] up Indicates port flex operation of add/delete.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX  LT populate failed.
 */
int
bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(int unit,
                                                      int lport,
                                                      uint32_t pri,
                                                      bool recovery,
                                                      bool deadlock_hdlr,
                                                      bool up);

/*!
 * \brief To populate the TM_DEADLOCK_RECOVERY LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] pri PFC priority.
 * \param [in] up Indicates port flex operation of add/delete.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX  LT populate failed.
 */
int
bcmtm_pfc_deadlock_recovery_populate_imm_entry(int unit ,
                                               int lport,
                                               uint32_t pri,
                                               bool up);
/*!
 * \brief Function to start dead lock monitor polling thread.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot mode.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmtm_pfc_deadlock_monitor_run(int unit, bool warm);
#endif /* BCMTM_PFC_DEADLOCK_INTERNAL_H */
