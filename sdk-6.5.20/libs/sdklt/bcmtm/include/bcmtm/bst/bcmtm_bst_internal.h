/*! \file bcmtm_bst_internal.h
 *
 * APIs, definitions for BST internal usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_BST_INTERNAL_H
#define BCMTM_BST_INTERNAL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmtm/bcmtm_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <shr/shr_thread.h>

/*!
 * Enum representing BST Class.
 */
typedef enum
bcmtm_bst_class_e {
    /*! BST ingress class. */
    BST_EVENT_CLASS_ING = 0,

    /*! BST egress class. */
    BST_EVENT_CLASS_EGR,

    /*! BST replication queue class. */
    BST_EVENT_CLASS_REPL_QUEUE,

    /*! BST device class. */
    BST_EVENT_CLASS_DEVICE,

    /*! BST class max count. */
    BST_EVENT_CLASS_MAX
} bcmtm_bst_event_class_t;

/*!
 * BST control user configuration.
 */
typedef struct {

    /*! BST resource class for an event. */
    bcmtm_bst_event_class_t class;

    /*! Tracking mode for the resource class. */
    uint8_t mode;

    /*! Clear on read of BST counters. */
    bool clear_on_read;

    /*! Enable the snapshot trigger for BST. */
    bool snapshot;
} bcmtm_bst_control_cfg_t;

/*!
 * BST event state enumerations.
 */
typedef
enum bst_event_state_e {
    /*! BST inactive */
    BST_EVENT_STATE_OFF = 0,

    /*! BST enabled */
    BST_EVENT_STATE_ARMED,

    /*!
     * BST enabled and interrupt cannot be received.
     * BST counters are enabled.
     */
    BST_EVENT_STATE_ARMED_PASSIVE,

    /*! BST event interrrupt triggered. */
    BST_EVENT_STATE_TRIGGERED,

    /*! This needs to be the last one. Determines the number of states. */
    BST_EVENT_STATE_MAX
} bcmtm_bst_event_state_t;


/*!
 * BST device information.
 */
typedef struct {
    /*! Lock for bst_dev_t structure. */
    sal_mutex_t bst_dev_lock;

    /*! BST interrupt message queue handler. */
    sal_msgq_t bcmtm_bst_trigger_q;

    /*! Semaphore to notify thread exit. */
    sal_sem_t bst_thread;

    /*! BST thread PID. */
    shr_thread_t pid;

    /*! Trigger state by the control table. */
    uint8_t trigger_state[MAX_NUM_ITMS][BST_EVENT_CLASS_MAX];
} bcmtm_bst_dev_info_t;


/*!
 * BST event state configurations..
 */
typedef struct {
    /*! Buffer pool (ITM) */
    uint8_t buffer_pool;

    /*! Class for which event was triggered. */
    bcmtm_bst_event_class_t   class;

    /*! Current state due to the triggered event. */
    bcmtm_bst_event_state_t   state;

} bcmtm_bst_event_cfg_t;

/*!
 * Notification info from BST interrupt routing to
 * BST thread.
 */
typedef struct {

    /*! Interrupt message type. */
    enum {
        /*! BST trriger notification message. */
        BST_TRIG_NOTIF,
        /*! BST exit message. */
        BST_TRIG_EXIT,
    } type;

    /*! Buffer pool (ITM) */
    int buffer_pool;

    /*! Interrupt event class. */
    bcmtm_bst_event_class_t class;

    /*! Ingress THDI interrupts. */
    bool ingress;

    /*! Egress THDO interrupts. */
    bool egress;

    /*! Replication Queue THD interrupts. */
    bool rqe;

    /*! Device specific interrupts. */
    bool cfap;
} bcmtm_bst_trigger_t;

/*! Callback functions. */

/*! BST Event control */
typedef int
(*bcmtm_bst_event_control_pt_set_f)(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmtm_bst_event_cfg_t *bst_event_cfg);

/*! BST event state updates. */
typedef int
(*bcmtm_bst_event_state_update_f)(int unit,
                                bcmtm_bst_event_cfg_t *bst_event_cfg);

/*! BST control */
typedef int
(*bcmtm_bst_control_pt_set_f)(int unit,
                              bcmltd_sid_t ltid,
                              bcmtm_bst_control_cfg_t *bst_ctrl_cfg);

/*! BST Event trigger handler. */
typedef int
(*bcmtm_bst_trigger_f) (int unit,
                        bcmtm_bst_trigger_t *trigger_info);

/*! Driver functions. */
typedef struct bcmtm_bst_driver_s {
    /*! BST event control configuration. */
    bcmtm_bst_event_control_pt_set_f bst_event_control_set;

    /*! BST control configuration. */
    bcmtm_bst_control_pt_set_f bst_control_pt_set;

    /*! BST trigger. */
    bcmtm_bst_trigger_f bst_trigger;
} bcmtm_bst_drv_t;

/*! \brief BST init.
 *
 * This created BST thread, semaphores and message queue needed for BST
 * operations by the device.
 *
 * \param [in] unit Logical unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Internal implementation error.
 */
extern int
bcmtm_bst_init(int unit, bool warm);

/*! \brief BST cleanup.
 *
 * Frees the resource allocated during bcmtm_bst_init.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Internal implementation error.
 */
extern int
bcmtm_bst_cleanup(int unit);

/*!
 * \brief BST event state logical table updates.
 *
 * \param [in] unit Logical unit number.
 * \param [in] bst_event_cfg BST event configuration to be updated for the
 *  logical tables.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Internal implementation error.
 */
extern int
bcmtm_bst_event_state_update(int unit,
                             bcmtm_bst_event_cfg_t *bst_event_cfg);

/*!
 * \brief Get the TM BST driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] dev TM BST dev info to get.
 */
extern void
bcmtm_bst_dev_info_get(int unit, bcmtm_bst_dev_info_t **dev);


/*!
 * \brief BST egress event source LT reset.
 *
 * \param [in] unit Logical unit number.
 * \param [in] in In list which gets updates.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
extern int
bcmtm_bst_event_src_egr_reset(int unit,
                              bcmltd_fields_t *in);

/*!
 * \brief BST ingress event source LT reset.
 *
 * \param [in] unit Logical unit number.
 * \param [in] in In list which gets updates.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
extern int
bcmtm_bst_event_src_ing_reset(int unit,
                              bcmltd_fields_t *in);


#endif /* BCMTM_BST_INTERNAL_H */
