/*! \file bcmtm_device_rxq_internal.h
 *
 * Definitions for device packet rx queues.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_DEVICE_RXQ_INTERNAL_H
#define BCMTM_DEVICE_RXQ_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * Device specific function pointers for physical table configuration
 * for getting RXQ bitmap configurations.
 */
typedef int (*bcmtm_device_rxq_bmp_pt_get_f) (int unit,
                                              bcmltd_sid_t ltid,
                                              uint32_t channel,
                                              uint64_t *cos_bmp);

/*!
 * Device specific function pointers for physical table configuration
 * for setting RXQ bitmap configurations.
 */
typedef int (*bcmtm_device_rxq_bmp_pt_set_f) (int unit,
                                              bcmltd_sid_t ltid,
                                              uint32_t channel,
                                              uint64_t cos_bmp);


/*! Device specific functions. */
typedef struct bcmtm_device_rxq_drv_s {
    /*! Physical table configuration set. */
    bcmtm_device_rxq_bmp_pt_set_f rxq_bmp_pt_set;
    /*! Physical table configuration get. */
    bcmtm_device_rxq_bmp_pt_get_f rxq_bmp_pt_get;
} bcmtm_device_rxq_drv_t;

/*! \brief IMM callback register function for Device packet RXQ logical tables.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_device_pkt_rxq_imm_register(int unit);


#endif /* BCMTM_DEVICE_RXQ_INTERNAL_H */
