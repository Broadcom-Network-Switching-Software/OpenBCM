/*! \file bcmlm_types.h
 *
 * bcmlm data types
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLM_TYPES_H
#define BCMLM_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*! Maximum number of devices supported */
#define BCMLM_DEV_NUM_MAX               BCMDRD_CONFIG_MAX_UNITS

/*! Maximun number of logical ports per chip supported */
#define BCMLM_PORT_MAX                  BCMDRD_CONFIG_MAX_PORTS

/*! Minimal linkscan interval */
#define BCMLM_LINKSCAN_INTERVAL_MIN     10000

/*! Default linkscan interval */
#define BCMLM_LINKSCAN_INTERVAL_DEFAULT 250000

/*! Maximun linkscan interval */
#define BCMLM_LINKSCAN_INTERVAL_MAX     ((uint32_t)-1)


/*! Maximun number of LT fields in LM */
#define BCMLM_FIELDS_MAX                32

/*!
 * \brief Linkscan modes.
 */
typedef enum bcmlm_linkscan_mode_e {

    /*! No linkscan */
    BCMLM_LINKSCAN_MODE_NONE,

    /*! Software linkscan. */
    BCMLM_LINKSCAN_MODE_SW,

    /*! Hardware linkscan. */
    BCMLM_LINKSCAN_MODE_HW,

    /*! Application overrides the link status. */
    BCMLM_LINKSCAN_MODE_OVERRIDE,

    BCMLM_LINKSCAN_MODE_INVALID

} bcmlm_linkscan_mode_t;

#endif /* BCMLM_TYPES_H */
