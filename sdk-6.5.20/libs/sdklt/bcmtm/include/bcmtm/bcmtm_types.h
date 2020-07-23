/*! \file bcmtm_types.h
 *
 * BCMTM Exported Types
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_TYPES_H
#define BCMTM_TYPES_H

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpc/bcmpc_types.h>


#ifndef MIN
/*! Get the minimum of two numbers. */
#define MIN(_a, _b) (_a) > (_b) ? (_b) : (_a)
#endif

#ifndef MAX
/*! Get the maximum of two numbers. */
#define MAX(_a, _b) (_a) < (_b) ? (_b) : (_a)
#endif

/*! Invalid logical port number. */
#define BCMTM_INVALID_LPORT ((bcmtm_lport_t)-1)

/*! Invalid physical port number. */
#define BCMTM_INVALID_PPORT ((bcmtm_pport_t)-1)

/*
* Global max values:
* These values should be updated to be the global max over all supported chips.
* They are only used when declaring public arrays or defining public structs.
*/

/*! Maximum number of loopback and management ports. */
#define MAX_NUM_LB_MGMT_PORTS       16

/*! Maximum number of CPU ports. */
#define MAX_NUM_CPU_PORTS           1

/*! Maximum number of ports supported by the system. */
#define MAX_NUM_PORTS               BCMDRD_CONFIG_MAX_PORTS

/*! Maximum number of pipes. */
#define MAX_NUM_PIPES               16

/*! Maximum number of ITMs. */
#define MAX_NUM_ITMS                2


/*! Physical device port number, a.k.a. physical port. */
typedef shr_port_t bcmtm_pport_t;

/*! Logical switch port number, a.k.a. logical port. */
typedef shr_port_t bcmtm_lport_t;


/*! Port encap modes. */
typedef enum bcmtm_port_encap_mode_e {
    /*! IEEE 802.3 Ethernet-II  */
    BCMTM_PORTENCAP_IEEE = 0,
    /*! HIGIG2 Header mode      */
    BCMTM_PORTENCAP_HIGIG = 1,
    /*! HIGIG2 Header mode      */
    BCMTM_PORTENCAP_HIGIG2 = 3,
    /*! Maximum encap types. Last entry. */
    BCMTM_PORTENCAP_COUNT
} bcmtm_port_encap_mode_t;


#endif /* BCMTM_TYPES_H */
