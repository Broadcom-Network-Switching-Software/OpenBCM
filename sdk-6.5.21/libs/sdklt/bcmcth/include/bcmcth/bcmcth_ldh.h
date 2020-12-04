/*! \file bcmcth_ldh.h
 *
 * BCMCTH Latency Distribution Histogram top-level APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LDH_H
#define BCMCTH_LDH_H

/*! Maximum number of collector. */
#define COLL_NUM_MAX       16

/*! Maximum queue of collector. */
#define COLL_QUEUE_MAX     12

/*! Maximum bucket of queue. */
#define COLL_BUCKET_MAX    8

/*!
 * \brief Define data structure of histogram group entry.
 */
typedef struct histo_group_entry_s {
    /*! Queue identifier. */
    uint32_t queue_id;

    /*! Bucket identifier. */
    uint32_t bucket_id;

    /*! Pkt counter. */
    uint32_t pkt;

    /*! Byte counter. */
    uint32_t byte;
} histo_group_entry_t;

/*!
 * \brief Define data structure of histogram group.
 */
typedef struct bcmcth_ldh_histo_group_s {
    /*! Size of histogram group. */
    int len;

    /*! Array of histogram group entry. */
    histo_group_entry_t entry[COLL_QUEUE_MAX * COLL_BUCKET_MAX];
} bcmcth_ldh_histo_group_t;

/*!
 * \brief Histogram group of monitor callback function.
 *
 * This function is a callback function that an application can registor with
 * the LDH component to receive counter of histogram group for specific monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] histo Pointer that was given during registration.
 *
 * \return None.
 */
typedef void (*bcmcth_ldh_histo_group_cb)(int unit,
                                          int mon_id,
                                          bcmcth_ldh_histo_group_t *histo);

/*!
 * \brief Define data structure of histogram group of monitor callback.
 */
typedef struct bcmcth_ldh_histo_s {
    /*! Histogram group of monitor array callback function. */
    bcmcth_ldh_histo_group_cb  ldh_histo_f[COLL_NUM_MAX];
} bcmcth_ldh_histo_t;

/*!
 * \brief Attach LDH device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ldh_attach(int unit, bool warm);

/*!
 * \brief Clean up LDH device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ldh_detach(int unit);

/*!
 * \brief LDH histogram group callback register.
 *
 * Callback registration of specific monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] fn Callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ldh_histo_register(int unit,
                          int mon_id,
                          bcmcth_ldh_histo_group_cb fn);

/*!
 * \brief LDH histogram group callback unregister.
 *
 * Callback unregistration of specific monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] fn Callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ldh_histo_unregister(int unit,
                            int mon_id,
                            bcmcth_ldh_histo_group_cb fn);

#endif /* BCMCTH_LDH_H */
