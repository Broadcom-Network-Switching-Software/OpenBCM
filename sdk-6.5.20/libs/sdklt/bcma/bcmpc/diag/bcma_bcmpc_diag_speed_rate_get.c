/*! \file bcma_bcmpc_diag_speed_rate_get.c
 *
 * Utility function to get speed rate for BCMPC diagnostics.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include "bcma_bcmpc_diag_internal.h"

/*******************************************************************************
 * Local definitions
 */

typedef struct stat_speed_entry_s {

    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of lanes. */
    int num_lanes;

    /*! FEC type. */
    bcmpc_cmd_fec_t fec_type;

    /*! Associated VCO rate of the ability. */
    int vco;

    /*! base rate for speed */
    double rate;

} stat_speed_entry_t;

/* List of speed, lane, fec, VCO, and rate */
static const
stat_speed_entry_t stat_speed_table[] =
{
    /* speed  lane fec_type                vco base rate(Gbps) */
    {10000,     1, BCMPC_CMD_PORT_FEC_NONE,        20, 10.3125},
    {10000,     1, BCMPC_CMD_PORT_FEC_BASE_R,      20, 10.3125},
    {20000,     1, BCMPC_CMD_PORT_FEC_NONE,        20, 10.3125},
    {20000,     1, BCMPC_CMD_PORT_FEC_BASE_R,      20, 10.3125},
    {40000,     4, BCMPC_CMD_PORT_FEC_NONE,        20, 10.3125},
    {40000,     4, BCMPC_CMD_PORT_FEC_BASE_R,      20, 10.3125},
    {40000,     2, BCMPC_CMD_PORT_FEC_NONE,        20, 10.3125},
    {25000,     1, BCMPC_CMD_PORT_FEC_NONE,        25, 25.78125},
    {25000,     1, BCMPC_CMD_PORT_FEC_BASE_R,      25, 25.78125},
    {25000,     1, BCMPC_CMD_PORT_FEC_RS_528,      25, 25.7812},
    {50000,     1, BCMPC_CMD_PORT_FEC_NONE,        25, 51.5625},
    {50000,     1, BCMPC_CMD_PORT_FEC_RS_528,      25, 51.5625},
    {50000,     1, BCMPC_CMD_PORT_FEC_RS_544,      26, 53.125},
    {50000,     1, BCMPC_CMD_PORT_FEC_RS_272,      26, 53.125},
    {50000,     2, BCMPC_CMD_PORT_FEC_NONE,        25, 25.78125},
    {50000,     2, BCMPC_CMD_PORT_FEC_RS_528,      25, 25.78125},
    {50000,     2, BCMPC_CMD_PORT_FEC_RS_544,      26, 26.5625},
    {100000,    2, BCMPC_CMD_PORT_FEC_NONE,        25, 51.5625},
    {100000,    2, BCMPC_CMD_PORT_FEC_RS_528,      25, 51.5625},
    {100000,    2, BCMPC_CMD_PORT_FEC_RS_544,      26, 53.125},
    {100000,    2, BCMPC_CMD_PORT_FEC_RS_272,      26, 53.125},
    {100000,    4, BCMPC_CMD_PORT_FEC_NONE,        25, 25.78125},
    {100000,    4, BCMPC_CMD_PORT_FEC_RS_528,      25, 25.78125},
    {100000,    4, BCMPC_CMD_PORT_FEC_RS_544,      26, 26.5625},
    {200000,    4, BCMPC_CMD_PORT_FEC_NONE,        25, 51.5625},
    {200000,    4, BCMPC_CMD_PORT_FEC_RS_544,      26, 53.125},
    {200000,    4, BCMPC_CMD_PORT_FEC_RS_544_2XN,  26, 53.125},
    {200000,    4, BCMPC_CMD_PORT_FEC_RS_272,      26, 53.125},
    {200000,    4, BCMPC_CMD_PORT_FEC_RS_544_2XN,  26, 53.125},
    {400000,    8, BCMPC_CMD_PORT_FEC_RS_544_2XN,  26, 53.125},
    {400000,    8, BCMPC_CMD_PORT_FEC_RS_272_2XN,  26, 53.125}
};

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmpc_diag_speed_rate_get(uint32_t speed, int num_lanes,
                               bcmpc_cmd_fec_t fec_type, double *rate)
{
    int idx;

    *rate = 0;
    for (idx = 0; idx < COUNTOF(stat_speed_table); idx++) {
        if (stat_speed_table[idx].speed == speed &&
            stat_speed_table[idx].num_lanes == num_lanes &&
            stat_speed_table[idx].fec_type == fec_type) {
            *rate = stat_speed_table[idx].rate;
            break;
        }
    }
    return SHR_E_NONE;
}
