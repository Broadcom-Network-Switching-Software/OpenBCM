/*! \file bcmtm_shaper_utils_internal.h
 *
 * Scheduler shaper utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_SHAPER_UTILS_INTERNAL_H
#define BCMTM_SHAPER_UTILS_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_handler.h>


/*!
 * Shaper burst user configuration.
 */
typedef struct bcmtm_shaper_burst{

    /*! maximum  burst size */
    uint16_t max_burst;

    /*! minimum bursts size */
    uint16_t min_burst;

    /*! maximum bandwidth */
    uint32_t max_bandwidth;

    /*! minimum bandwidth */
    uint32_t min_bandwidth;

    /*! auto update enabled */
    uint8_t burst_auto;
} bcmtm_shaper_burst_t;

/*!
 * \brief gets the shaper configuration for ITU Mode.
 *
 * Returns ITU mode settings
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id     Transaction ID
 * \param [out] itu_mode    ITU mode.
 *
 * \retval SHR_E_PARAM      unable to read configuraion from PT
 */
extern int
bcmtm_shaper_conf_get(int unit, uint32_t trans_id, uint32_t *itu_mode);

#endif /* BCMTM_SHAPER_UTILS_INTERNAL_H */

