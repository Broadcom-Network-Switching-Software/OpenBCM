/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_xgs.c
 * Purpose:     XGS Board programming for
 *              White Knight, Merlin, and Lancelot references platforms
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/topo.h>

#include <appl/stktask/topo_brd.h>

#include "topo_int.h"

/*
 * These functions map a (src_unit, dest_modid) pair to the local port
 * on on src_unit which packets destined for dest_modid should exit.  This
 * is based on the current CPU database and topology information if active.
 *
 * Used for mirroring, trunking and TX.
 */

/*
 * Draco board
 *   1 x 5690 (stackable)
 */
STATIC int
_bcm_board_topomap_12g_stk(int src_unit, int dest_modid, bcm_port_t *exit_port)
{
    *exit_port = 12;    /* IPIC_PORT(src_unit) */
    return BCM_E_NONE;
}

int
bcm_board_topo_12g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int mod;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 12GE+1HG board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_12g_stk);

    mod = tp_cpu->local_entry.mod_ids[0];

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, mod));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(0));
    return BCM_E_NONE;
}

/*
 * Merlin/WhiteKnight board (BCM95690R24S)
 * (also chassis lm24g line module board)
 *   5670/71 + 2 x 5690
 *   External stack ports are unit 0 ports 3,6
 */
STATIC int
_bcm_board_topomap_24g_stk(int src_unit, int dest_modid,
                           bcm_port_t *exit_port)
{
    switch (src_unit) {
    case 0:  /* 5670 */
        /* check for local connections */
        BCM_BOARD_MOD_CHECK(1, 8, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(2, 1, dest_modid, exit_port);
        return bcm_board_topomap_stk(src_unit, dest_modid, exit_port);
        
    case 1:  /* 5690s */
    case 2:
        *exit_port = 12;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;
        
    default:
        return BCM_E_UNIT;
    }
}

int
bcm_board_topo_24g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[3];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 24GE+2HG board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_24g_stk);

    unit = 0;           /* board fabric unit */

    unitport[0] = -1;   /* 5670 unit 0 */
    unitport[1] = 8;    /* 5690 unit 1 is on port 8 */
    unitport[2] = 1;    /* 5690 unit 2 is on port 1 */

    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
}



/*
 * Lancelot board (BCM95690R48S)
 *     5670 + 4 x 5690
 * External stack ports are unit 0 ports 3,4,5,6
 */
STATIC int
_bcm_board_topomap_48g_stk(int src_unit, int dest_modid,
                           bcm_port_t *exit_port)
{
    switch (src_unit) {
    case 0:  /* 5670 */
        /* check for local connections */
        BCM_BOARD_MOD_CHECK(1, 7, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(2, 8, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(3, 1, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(4, 2, dest_modid, exit_port);
        return bcm_board_topomap_stk(src_unit, dest_modid, exit_port);
        
    case 1:  /* 5690s */
    case 2:
    case 3:
    case 4:
        *exit_port = 12;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;
        
    default:
        return BCM_E_UNIT;
    }
}

int
bcm_board_topo_48g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[5];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 48GE+4HG board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_48g_stk);

    unit = 0;           /* board fabric unit */

    unitport[0] = -1;   /* 5670 unit 0 */
    unitport[1] = 7;    /* 5690 unit 1 is on port 7 */
    unitport[2] = 8;    /* 5690 unit 2 is on port 8 */
    unitport[3] = 1;    /* 5690 unit 3 is on port 1 */
    unitport[4] = 2;    /* 5690 unit 4 is on port 2 */

    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
}

/*
 * Galahad/Draco-Back-to-back board (BCM95690R24/BCM95690K24)
 *      2 x 5690 (non-stackable)
 */
STATIC int
_bcm_board_topomap_24g(int src_unit, int dest_modid,
                       bcm_port_t *exit_port)
{
    *exit_port = 12;    /* IPIC_PORT(src_unit) */
    return BCM_E_NONE;
}

int
bcm_board_topo_24g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int *mod_ids;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS B2B 569x 24GE board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_24g);

    mod_ids = tp_cpu->local_entry.mod_ids;

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, mod_ids[0]));
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(1, mod_ids[1]));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(0));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(1));
    return BCM_E_NONE;
}

/*
 * Tucana board (BCM95665P48)
 *     1 x 5665 (stackable)
 */
STATIC int
_bcm_board_topomap_48f_stk(int src_unit, int dest_modid,
                           bcm_port_t *exit_port)
{
    *exit_port = 56;    /* IPIC_PORT(src_unit) */
    return BCM_E_NONE;
#if 0
    /* alternate 5665 board: unit 0 is 5670, unit 1 is 5665 */
    switch (src_unit) {
    case 0:  /* 5670 */
        /* check for local connections */
        BCM_BOARD_MOD_CHECK(1, 8, dest_modid, exit_port);
        return bcm_board_topomap_stk(src_unit, dest_modid, exit_port);

    case 1:  /* 5665 */
        *exit_port = 56;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;

    default:
        return BCM_E_UNIT;
    }
#endif
}

int
bcm_board_topo_48f_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int mod;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5665 48FE+4GE+HG board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_48f_stk);

    mod = tp_cpu->local_entry.mod_ids[0];

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, mod));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(0));
    return BCM_E_NONE;
#if 0
    /* alternate 5665 board: unit 0 is 5670, unit 1 is 5665 */
    int    unit;
    int    unitport[2];

    bcm_topo_map_set(_bcm_board_topomap_48f_stk);

    unit = 0;
    unitport[0] = -1;    /* 5670 unit 0 */
    unitport[1] = 8;     /* 5665 unit 1 is on port 8 */
    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
#endif
}

/*
 * Guenevere board (BCM95695P24SX_10)
 *     5675 + 2 x 5695 + 2 x 5673
 * External stack ports are unit 0 ports 4,5
 * Unconnected ports are unit 0 ports 3,6
 */
STATIC int
_bcm_board_topomap_24g2x_stk(int src_unit, int dest_modid,
                             bcm_port_t *exit_port)
{
    switch (src_unit) {
    case 0:  /* 5675 */
        /* check for local connections */
        BCM_BOARD_MOD_CHECK(1, 1, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(2, 2, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(3, 8, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(4, 7, dest_modid, exit_port);
        return bcm_board_topomap_stk(src_unit, dest_modid, exit_port);

    case 1:  /* 5695s */
    case 2:
        *exit_port = 12;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;

    case 3:  /* 5673s */
    case 4:
        *exit_port = 1;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;

    default:
        return BCM_E_UNIT;
    }
}

int
bcm_board_topo_24g2x_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[5];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 24GE+2XE board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_24g2x_stk);

    unit = 0;            /* board fabric unit */

    unitport[0] = -1;    /* 5675 unit 0 */
    unitport[1] = 1;     /* 5695 unit 1 is on port 1 */
    unitport[2] = 2;     /* 5695 unit 2 is on port 2 */
    unitport[3] = 8;     /* 5673 unit 3 is on port 8 */
    unitport[4] = 7;     /* 5673 unit 4 is on port 7 */

    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
}

/*
 * Lynxalot board (BCM95673R48S)
 *     5670 + 2 x 5673 + 4 x 5690
 * External stack ports are unit 2 ports 5,6
 */
STATIC int
_bcm_board_topomap_48g2x_stk(int src_unit, int dest_modid,
                             bcm_port_t *exit_port)
{
    switch (src_unit) {
    case 0:  /* 5673s */
    case 1:
        *exit_port = 1;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;

    case 2:  /* 5670 */
        /* check for local connections */
        BCM_BOARD_MOD_CHECK(0, 3, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(1, 4, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(3, 7, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(4, 8, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(5, 1, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(6, 2, dest_modid, exit_port);
        return bcm_board_topomap_stk(src_unit, dest_modid, exit_port);

    case 3:  /* 5690s */
    case 4:
    case 5:
    case 6:
        *exit_port = 12;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;

    default:
        return BCM_E_UNIT;
    }
}

int
bcm_board_topo_48g2x_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[7];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 48GE+2XE board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_48g2x_stk);

    unit = 2;            /* board fabric unit */

    unitport[0] = 3;     /* 5673 unit 0 is on port 3 */
    unitport[1] = 4;     /* 5673 unit 1 is on port 4 */
    unitport[2] = -1;    /* 5670 unit 2 */
    unitport[3] = 7;     /* 5690 unit 3 is on port 7 */
    unitport[4] = 8;     /* 5690 unit 4 is on port 8 */
    unitport[5] = 1;     /* 5690 unit 5 is on port 1 */
    unitport[6] = 2;     /* 5690 unit 6 is on port 2 */

    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
}

/*
 * Herc8 board (also chassis cfm board)
 *     5670
 * External stack ports are unit 0 ports 1-8
 */
int
bcm_board_topo_8h(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[1];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5670 8HG board topology handler\n")));
    bcm_topo_map_set(bcm_board_topomap_stk);

    unit = 0;            /* board fabric unit */

    unitport[0] = -1;    /* 5670 unit 0 */

    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
}

/*
 * 5695 at Unit 0
 */
STATIC int
_bcm_board_topomap_xgs2_12g(int src_unit, int dest_modid,
                            bcm_port_t *exit_port)
{
    *exit_port = 13;    /* IPIC_PORT(src_unit) */
    return BCM_E_NONE;
}

int
bcm_board_topo_xgs2_12g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    mod;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS2 5695 12GE+1HG SDK\n")));
    bcm_topo_map_set(_bcm_board_topomap_xgs2_12g);

    mod = tp_cpu->local_entry.mod_ids[0];

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, mod));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(0));
    return BCM_E_NONE;
}


