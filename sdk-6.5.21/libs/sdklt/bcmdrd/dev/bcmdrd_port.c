/*! \file bcmdrd_port.c
 *
 * DRD port information management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_internal.h>

static const bcmdrd_port_info_t *
bcmdrd_port_info_get(int unit)
{
    bcmdrd_dev_t *dev;

    dev = bcmdrd_dev_get(unit);
    if (dev == NULL) {
        return NULL;
    }

    return dev->port_info;
}

int
bcmdrd_port_info_init(int unit, const bcmdrd_port_info_t *pi)
{
    bcmdrd_dev_t *dev;

    dev = bcmdrd_dev_get(unit);
    if (dev == NULL) {
        return SHR_E_UNIT;
    }

    dev->port_info = pi;

    return SHR_E_NONE;
}

uint32_t
bcmdrd_port_type_get(int unit, int port)
{
    const bcmdrd_port_info_t *pi = bcmdrd_port_info_get(unit);
    bcmdrd_pbmp_t valid_pbmp;
    int rv;

    if ((unsigned)port >= BCMDRD_CONFIG_MAX_PORTS) {
        return BCMDRD_PORT_TYPE_UNDEF;
    }
    if (pi) {
        while (pi->ptype != 0) {
            if (port == pi->port) {
                return pi->ptype;
            }
            pi++;
        }
    }
    rv = bcmdrd_dev_valid_ports_get(unit, &valid_pbmp);
    if (SHR_SUCCESS(rv)) {
        if (BCMDRD_PBMP_MEMBER(valid_pbmp, port)) {
            /* Assume unspecified valid port to be a front-panel port */
            return BCMDRD_PORT_TYPE_FPAN;
        }
    }
    return BCMDRD_PORT_TYPE_UNDEF;
}

uint32_t
bcmdrd_lport_type_get(int unit, int lport)
{
    const bcmdrd_port_info_t *pi = bcmdrd_port_info_get(unit);

    if (lport < 0) {
        return BCMDRD_PORT_TYPE_UNDEF;
    }
    if (pi) {
        while (pi->ptype != 0) {
            if (lport == pi->lport) {
                return pi->ptype;
            }
            pi++;
        }
    }
    return BCMDRD_PORT_TYPE_UNDEF;
}

int
bcmdrd_port_type_pbmp_get(int unit, bcmdrd_port_type_t ptype,
                          bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_port_info_t *pi = bcmdrd_port_info_get(unit);
    bcmdrd_pbmp_t fpan_pbmp;
    int fport;
    int num_ports = 0;

    if (pbmp) {
        BCMDRD_PBMP_CLEAR(*pbmp);
    }
    BCMDRD_PBMP_CLEAR(fpan_pbmp);
    if (ptype & BCMDRD_PORT_TYPE_FPAN) {
        (void)bcmdrd_dev_valid_ports_get(unit, &fpan_pbmp);
    }
    if (pi) {
        while (pi->ptype != 0) {
            if (ptype & pi->ptype) {
                if (pbmp) {
                    BCMDRD_PBMP_PORT_ADD(*pbmp, pi->port);
                }
                num_ports++;
            }
            /* Remove port_info ports from valid port bitmap */
            if (ptype & BCMDRD_PORT_TYPE_FPAN) {
                BCMDRD_PBMP_PORT_REMOVE(fpan_pbmp, pi->port);
            }
            pi++;
        }
    }
    if (ptype & BCMDRD_PORT_TYPE_FPAN) {
        /* Assume valid ports not in port info are front-panel ports */
        BCMDRD_PBMP_ITER(fpan_pbmp, fport) {
            num_ports++;
        }
        if (pbmp) {
            BCMDRD_PBMP_OR(*pbmp, fpan_pbmp);
        }
    }
    return num_ports;
}

int
bcmdrd_port_p2l(int unit, int port)
{
    const bcmdrd_port_info_t *pi = bcmdrd_port_info_get(unit);

    if (pi && port >= 0) {
        while (pi->ptype != 0) {
            if (port == pi->port) {
                return pi->lport;
            }
            pi++;
        }
    }
    return -1;
}

int
bcmdrd_port_l2p(int unit, int lport)
{
    const bcmdrd_port_info_t *pi = bcmdrd_port_info_get(unit);

    if (pi && lport >= 0) {
        while (pi->ptype != 0) {
            if (lport == pi->lport) {
                return pi->port;
            }
            pi++;
        }
    }
    return -1;
}

int
bcmdrd_port_p2m(int unit, int port)
{
    const bcmdrd_port_info_t *pi = bcmdrd_port_info_get(unit);

    if (pi && port >= 0) {
        while (pi->ptype != 0) {
            if (port == pi->port) {
                return pi->mport;
            }
            pi++;
        }
    }
    return -1;
}

int
bcmdrd_port_m2p(int unit, int mport)
{
    const bcmdrd_port_info_t *pi = bcmdrd_port_info_get(unit);

    if (pi && mport >= 0) {
        while (pi->ptype != 0) {
            if (mport == pi->mport) {
                return pi->port;
            }
            pi++;
        }
    }
    return -1;
}
