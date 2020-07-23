/*! \file bcmsec_event.h
 *
 * BCMSEC Event header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_EVENT_H
#define BCMSEC_EVENT_H

#include <sal/sal_mutex.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <shr/shr_thread.h>

/*! SA expiry event indicator for the encrypt direction. */
#define SA_EXPIRY_ENCRYPT       0
/*! SA expiry event indicator for the decrypt direction. */
#define SA_EXPIRY_DECRYPT       1
/*! The SA status is invalid. */
#define SA_INVALID                  0
/*! The SA status is valid. */
#define SA_VALID                    1
/*!
 * The SA is valid and has soft expired (configured PN
 * threshold reached).
 */
#define SA_VALID_AND_SOFT_EXPIRED   2
/*!
 * The SA is valid and has hard expired (PN reached
 * 2^32 or 2^64).
 */
#define SA_VALID_AND_HARD_EXPIRED   3

/*! Event message type. */
typedef enum {
    /*! Event type for notification messages. */
    SEC_TRIG_NOTIF,

    /*! Event type signalling the thread to exit. */
    SEC_TRIG_EXIT,
} bcmsec_trigger_type_t;

/*!
 * SEC event trigger information.
 */
typedef struct {

    /*! Event message type. */
    bcmsec_trigger_type_t type;

    /*! SEC block index. */
    int blk_id;

    /*! Interrupt number */
    int intr_num;

} bcmsec_event_trigger_t;

/*!
 * SEC event information.
 */
typedef struct {
    /*! Lock for structure. */
    sal_mutex_t event_lock;

    /*! Interrupt message queue handler. */
    sal_msgq_t bcmsec_event_q;

    /*! Semaphore to notify thread exit. */
    sal_sem_t sec_event_thread;

    /*! Thread PID. */
    shr_thread_t pid;

} bcmsec_event_info_t;

/*!
 * SEC IMM event information.
 */
typedef struct {
    /*! SA expiry type. */
    int sa_status_type;

    /*! Macsec block id */
    int blk_id;

    /*! SA index. */
    int sa_index;

    /*! Expiry direction. */
    int dir;

    /*! IPsec? 0 - Macsec, -1 - No information. */
    int ipsec;

} bcmsec_sa_status_imm_update_t;

/*!
 * \brief Event init
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_event_init(int unit, int warm);

/*!
 * \brief Event cleanup
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_event_cleanup(int unit);

/*!
 * \brief Update the IMM table
 *
 * \param [in] unit Unit number.
 * \param [in] sa_status SA expiry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_sa_status_imm_update (int unit,
                        bcmsec_sa_status_imm_update_t *sa_status);
#endif /* BCMSEC_EVENT_H */
