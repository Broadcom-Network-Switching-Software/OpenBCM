/*! \file bcmsec_types.h
 *
 * BCMSEC Exported Types
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_TYPES_H
#define BCMSEC_TYPES_H

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*! Invalid logical port number. */
#define BCMSEC_INVALID_LPORT ((bcmsec_lport_t)-1)

/*! Invalid physical port number. */
#define BCMSEC_INVALID_PPORT ((bcmsec_pport_t)-1)

/*! Invalid ID. */
#define BCMSEC_INVALID_ID ((int)-1)

/*! The max number of units per system */
#define BCMSEC_NUM_UNITS_MAX BCMDRD_CONFIG_MAX_UNITS

/*! The max number of physical ports per chip */
#define BCMSEC_NUM_PPORTS_PER_CHIP_MAX   (256)

/*! The maximum number of physical ports per PM */
#define BCMSEC_NUM_PPORTS_PER_PM_MAX (8)

/*! Assign a bit to the buffer. */
#define SEC_BITASSIGN(_buf, _pos, _data) \
    (_buf = ((_buf & ~(1 << _pos)) | (_data << _pos)))

/*
* Global max values:
* These values should be updated to be the global max over all supported chips.
* They are only used when declaring public arrays or defining public structs.
*/

/*! Physical device port number, a.k.a. physical port. */
typedef shr_port_t bcmsec_pport_t;

/*! Logical switch port number, a.k.a. logical port. */
typedef shr_port_t bcmsec_lport_t;

/*! Port bitmap. */
typedef bcmdrd_pbmp_t pbmp_t;

/*! SEC port info configuration. */
typedef struct sec_port_info_alloc_cfg_s {
    /*! Number of physical ports. */
    int num_pport;

    /*! Number of logical ports. */
    int num_lport;
} sec_port_info_alloc_cfg_t;

#endif /* BCMSEC_TYPES_H */
