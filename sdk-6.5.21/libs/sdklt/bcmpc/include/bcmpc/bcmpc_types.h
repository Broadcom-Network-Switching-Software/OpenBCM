/*! \file bcmpc_types.h
 *
 * BCMPC public types.
 *
 * Declaration of the structures, enumerations which are exported by
 * the Port Control (BCMPC) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_TYPES_H
#define BCMPC_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>


/*! Physical device port number, a.k.a. physical port. */
typedef shr_port_t bcmpc_pport_t;

/*! Logical switch port number, a.k.a. logical port. */
typedef shr_port_t bcmpc_lport_t;

/*! Invalid logical port number. */
#define BCMPC_INVALID_LPORT ((bcmpc_lport_t)-1)

/*! Invalid physical port number. */
#define BCMPC_INVALID_PPORT ((bcmpc_pport_t)-1)

#endif /* BCMPC_TYPES_H */
