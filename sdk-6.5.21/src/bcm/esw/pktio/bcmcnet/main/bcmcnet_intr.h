/*! \file bcmcnet_intr.h
 *
 * Generic data structure definitions for interrupt processing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCNET_INTR_H
#define BCMCNET_INTR_H

/*! Interrupt callback function */
typedef int (*pdma_intr_cb_t)(struct intr_handle *hdl, void *ck);

/*!
 * \brief Interrupt source description.
 */
struct pdma_intr_source {
    /*! Interrupt handle */
    struct intr_handle *intr_hdl;

    /*! Callback */
    pdma_intr_cb_t intr_cb;

    /*! Callback parameter */
    void *intr_ck;

    /*! Interrupt is active */
    int active;

    /*! Interrupt is enabled */
    int enabled;

    /*! Interrupt spinlock */
    sal_spinlock_t lock;
};

/*!
 * \brief Interrupt control data structure.
 */
struct pdma_intr_ctrl {
    /*! Interrupt sources */
    struct pdma_intr_source intr_src[NUM_QUE_MAX];

    /*! Interrupt active state */
    uint32_t poll_active;

    /*! Interrupt thread control */
    shr_thread_ctrl_t *tc;

    /*! Interrupt abort */
    int abort;
};

/*!
 * \brief Register callback.
 *
 * \param [in] hdl Interrupt handle.
 * \param [in] cb Callback.
 * \param [in] ck Callback parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_intr_cb_register(struct intr_handle *hdl, pdma_intr_cb_t cb, void *ck);

/*!
 * \brief Unregister callback.
 *
 * \param [in] hdl Interrupt handle.
 * \param [in] cb Callback.
 * \param [in] ck Callback parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_intr_cb_unregister(struct intr_handle *hdl, pdma_intr_cb_t cb, void *ck);

/*!
 * \brief Interrupt control initialization.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_intr_ctrl_init(struct pdma_dev *dev);

/*!
 * \brief Interrupt control clean up.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_intr_ctrl_cleanup(struct pdma_dev *dev);

/*!
 * \brief Poll enable.
 *
 * \param [in] hdl Interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern void
bcmcnet_poll_enable(struct intr_handle *hdl);

/*!
 * \brief Poll disable.
 *
 * \param [in] hdl Interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern void
bcmcnet_poll_disable(struct intr_handle *hdl);

/*!
 * \brief Poll complete.
 *
 * \param [in] hdl Interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern void
bcmcnet_poll_complete(struct intr_handle *hdl);

/*!
 * \brief Poll schedule.
 *
 * \param [in] hdl Interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern void
bcmcnet_poll_schedule(struct intr_handle *hdl);

/*!
 * \brief Poll check.
 *
 * \param [in] hdl Interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_poll_check(struct intr_handle *hdl);

#endif /* BCMCNET_INTR_H */

