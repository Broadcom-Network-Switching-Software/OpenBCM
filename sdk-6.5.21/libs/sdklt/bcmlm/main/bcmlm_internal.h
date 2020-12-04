/*! \file bcmlm_internal.h
 *
 * BCM Link Manager APIs and Structures used only internally
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLM_INTERNAL_H
#define BCMLM_INTERNAL_H

#include <sal/sal_sem.h>
#include <shr/shr_thread.h>
#include <sal/sal_mutex.h>
#include <bcmbd/bcmbd_intr.h>
#include <bcmlm/bcmlm_types.h>
#include <bcmlm/generated/bcmlm_ha.h>

/*! Maximum length of thread name */
#define THREAD_NAME_LEN_MAX             16

/*! Linkscan thread priority */
#define LINKSCAN_THREAD_PRI             50

/*! HA subcomponent ID for LM ctrl */
#define BCMLM_CTRL_SUB_COMP_ID          1

/*!
 * Link manager control structure.
 */
typedef struct bcmlm_ctrl_s {
    /*! Synchronization */
    sal_mutex_t lock;

    /*! Linkscan thread name */
    char name[THREAD_NAME_LEN_MAX];

    /*! Linkscan thread control object */
    shr_thread_t tc;

    /*! system is ready for linkscan to start */
    bool ready;

    /*! Time between scans */
    int interval_us;

    /*! Ports enabled with link scan */
    bcmdrd_pbmp_t pbm_linkscan;

    /*! Software link scan ports */
    bcmdrd_pbmp_t pbm_sw;

    /*! Hardware link scan ports */
    bcmdrd_pbmp_t pbm_hw;

    /*! Link override ports */
    bcmdrd_pbmp_t pbm_ovr;

    /*! Link state for ports with override linkscan mode */
    bcmdrd_pbmp_t pbm_ovr_link;

    /*! Ports that newly enabled with link scan */
    bcmdrd_pbmp_t pbm_newly_enabled;

    /*! Ports that newly disabled with link scan */
    bcmdrd_pbmp_t pbm_newly_disabled;

    /*!
     * Ports that suspended in LM. Port is valid in LM_PORT_CONTROL
     * but not in PC_PORT LT
     */
    bcmdrd_pbmp_t pbm_suspend;

    /*! Ports that are valid for LM_PORT_CONTROL */
    bcmdrd_pbmp_t pbm_valid;

    /*! Ports enabled with manual sync. */
    bcmdrd_pbmp_t pbm_manual_sync;

    /*! Port basis manual sync status has changed */
    bcmdrd_pbmp_t pbm_sync_change;

    /*! HW link config has changed */
    bool hw_conf_change;

    /*! HW link status has changed */
    bool hw_change;

    /*! Override link status has changed */
    bool ovr_change;

    /*! Forced link status has changed */
    bool force_change;

    /*! LM pbm kept in HA */
    bcmlm_pbm_ha_t *pbm_ha;

} bcmlm_ctrl_t;

/*!
 * \brief Stop link scan thread.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmlm_ctrl_stop(int unit);

/*!
 * \brief Destroy Link control database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmlm_ctrl_cleanup(int unit);

/*!
 * \brief Initialize Link control database.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmlm_ctrl_init(int unit, bool warm);

/*!
 * \brief Ready to start link scan thread.
 *
 * This function will be called in RUN stage during LM initialization, to
 * indicate that system is ready to start link state scanning.
 * This prevents link scanning at early stage of system boot.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmlm_ctrl_ready(int unit);

/*!
 * \brief Update software linkscan interval in Link Manager control database.
 *
 * \param [in] unit Unit number.
 * \param [in] interval Interval in usecs.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_interval_update(int unit, uint32_t interval);

/*!
 * \brief Enable or disable link scan in Link Manager control database.
 *
 * Start or stop linkscan thread.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmlm_ctrl_linkscan_enable(int unit, bool enable);

/*!
 * \brief Update link scan mode for a given port in Link Manager control database.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode Link scan mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_linkscan_mode_update(int unit,
                                shr_port_t port,
                                bcmlm_linkscan_mode_t mode);

/*!
 * \brief Update override link state for a given port in LM control database.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_override_link_state_update(int unit,
                                      shr_port_t port,
                                      bool link);

/*!
 * \brief Update manual sync state for a given port in LM control database.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] manual_sync manual sync state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_manual_sync_update(int unit,
                              shr_port_t port,
                              bool manual_sync);

/*!
 * \brief Lookup callback function to get logical and physical link state.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] link_state Logical link state.
 * \param [out] phy_link Physical link state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_port_state_lookup(int unit,
                             shr_port_t port,
                             int *link_state,
                             int *phy_link);

/*!
 * \brief Check if the port is valid for LM_LINK_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE The port is valid.
 * \retval SHR_E_PORT The port is invalid.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_port_state_validate(int unit, shr_port_t port);

/*!
 * \brief LM_PORT_CONTROL insert callback function.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_port_add(int unit, shr_port_t port);

/*!
 * \brief LM_PORT_CONTROL delete callback function.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link control is not initialized.
 */
extern int
bcmlm_ctrl_port_delete(int unit, shr_port_t port);

/*!
 * \brief Reload link information from LM LTs.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_ctrl_link_reload(int unit);

/*!
 * \brief Register callbacks to handle port events.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int
bcmlm_port_event_register(int unit);

/*!
 * \brief Unregister callbacks that handle port events.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int
bcmlm_port_event_unregister(int unit);

/*!
 * \brief IMM-based database initialization.
 *
 * Initialize field array local memory manager handler.
 *
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_db_init(void);

/*!
 * \brief IMM-based database cleanup.
 *
 * Delete field array local memory manager handler.
 *
 *
 * \return none.
 */
extern void
bcmlm_imm_db_cleanup(void);

/*!
 * \brief Register callback functions to LM IMM LTs.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_register(int unit);

/*!
 * \brief Restore link information from LM LTs.
 *
 * \param [in] unit Unit number.
 * \param [out] lmctrl Link control.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_imm_link_restore(int unit, bcmlm_ctrl_t *lmctrl);

/*!
 * \brief Inform the application for link change.
 *
 * LM would use this function to notify the application for port link state
 * changes.
 *
 * The application must subscribes LM_LINK_STATE via bcmlt_table_subscribe()
 * to get the notification.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmlm_link_state_tbl_notify(int unit, int port, int link);

#endif /* BCMLM_INTERNAL_H */
