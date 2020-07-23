/*! \file bcmlm_fw_linkscan.h
 *
 * Link Manager firmware linkscan definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLM_FW_LINKSCAN_H
#define BCMLM_FW_LINKSCAN_H

#include <sal/sal_spinlock.h>

#include <bcmbd/bcmbd_intr.h>

#include <bcmlm/bcmlm_types.h>
#include <bcmlm/bcmlm_drv_internal.h>

/*!
 * \brief Link Manager firmware driver object.
 *
 * The Link Manager firware driver object is used to initialize and access
 * firmware linkscan for a switch device.
 */
typedef struct bcmlm_fw_drv_s {

    /*! Unit number. */
    int unit;

    /*! Cached link status. */
    bcmdrd_pbmp_t link_stat;

    /*! Link status updating protection. */
    sal_spinlock_t lock;

    /*! Interrupt callback for letting LM to handle link change event. */
    bcmbd_intr_f intr_func;

    /*! Mailbox ID to communicate with linkscan FW. */
    uint32_t mbox_id;

    /*! Flag to indicate FW linkscan driver is initialized or not. */
    bool fw_init;

} bcmlm_fw_drv_t;

/*!
 * \brief Mapping exception bits within linkscan registers.
 *
 * By default, the link bit offset within linkscan register is equal to
 * (physical port number - 1). But sometimes there are some exceptions
 * due to hardware design. This structure is to define those exceptions.
 */
typedef struct bcmlm_fw_pport_to_lsbit_s {

    /*! Physical port number. */
    int pport;

    /*! Bits offset for port within linkscan register. */
    int lsbit;

} bcmlm_fw_pport_to_lsbit_t;

/*!
 * \brief Allocate resources and attach firmware linkscan operations.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Pointer to driver structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmlm_cmicx_fw_linkscan_drv_attach(int unit, bcmlm_drv_t *drv);

/*!
 * \brief Release resources.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Pointer to driver structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmlm_cmicx_fw_linkscan_drv_detach(int unit, bcmlm_drv_t *drv);

#endif /* BCMLM_FW_LINKSCAN_H */
