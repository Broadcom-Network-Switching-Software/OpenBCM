/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_chassis_nolb.c
 * Purpose:     XGS2/XGS3 Chassis Non-load-balanced board programming
 */

#include <shared/bsl.h>
#include <shared/alloc.h>

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/topo.h>
#include <bcm/trunk.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_brd.h>

#include "topo_int.h"
#include "brd_chassis_int.h"

#if defined(INCLUDE_CHASSIS)


STATIC int
_bcm_board_xgs3_sw_modport_set(int funit, topo_cpu_t *tp_cpu,
                               bcm_board_port_connection_t *connection)
{
    int i, m, mod,unit, port;
    cpudb_entry_t *l_entry;
    topo_stk_port_t *tsp;

    l_entry = &tp_cpu->local_entry;

    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        port = connection[unit].to;
        if (port < 0) { /* Skip fabric */
            continue;
        }
        /* Set module mapping for this leaf unit for modules outside
           of this board. */
        for (i = 0; i < l_entry->base.num_stk_ports; i++) {
            /* For each mod-id reached by the fabric, set in leaf. */
            tsp = &tp_cpu->tp_stk_port[i];

            /* Set modport for each module reachable outside of this board */
            for (m = 0; m < tsp->tx_mod_num; m++) {
                mod = tsp->tx_mods[m];
                BCM_IF_ERROR_RETURN(
                    bcm_stk_modport_set(unit,
                                        mod,
                                        connection[unit].from));
            }
        }
        /* Set the module mapping for this unit so it knows how to get
           to every other local unit. */
        for (m = 0; m < l_entry->base.num_units; m++) {
            if (connection[m].to < 0 || m == unit) {
                /* Skip device if it's a fabric module or the current
                   unit */
                continue;
            }
            mod = l_entry->mod_ids[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit,
                                                    mod,
                                                    connection[unit].from));
        }
    }
    return BCM_E_NONE;
}



STATIC int
_bcm_board_xgs3_sw_stk_port_add(int f_unit, topo_cpu_t *tp_cpu,
                                bcm_board_port_connection_t *connection)
{
    int s_unit, f_port, s_port;
    cpudb_entry_t *l_entry;

    l_entry = &tp_cpu->local_entry;

    for (s_unit = 0; s_unit < l_entry->base.num_units; s_unit++) {

        f_port = connection[s_unit].to;
        if (f_port < 0) { /* Skip device (eg fabric module) */
            continue;
        }

        /* Set fabric stack port state */
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(f_unit, f_port));

        /* Set switch stack port state */
        s_port = connection[s_unit].from;
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(s_unit, s_port));
    }
    return BCM_E_NONE;
}


int
bcm_board_xgs3_sw_xgs_fabric(int funit, topo_cpu_t *tp_cpu,
                             cpudb_ref_t db_ref,
                             bcm_board_port_connection_t *connection)
{
    int                i, m, unit;
    bcm_pbmp_t         pbm, empty;
    cpudb_entry_t      *l_entry;
    bcm_port_t         port, hport;
    bcm_module_t       mod;
    bcm_port_config_t  config;
    topo_stk_port_t    *tsp;

    l_entry = &tp_cpu->local_entry;

    /* Notify BCM layer of internal stack ports */
    BCM_IF_ERROR_RETURN(_bcm_board_xgs3_sw_stk_port_add(funit, tp_cpu,
                                                    connection));
    BCM_IF_ERROR_RETURN(_bcm_board_xgs3_sw_modport_set(funit, tp_cpu,
                                                    connection));

    BCM_IF_ERROR_RETURN(bcm_port_config_get(funit, &config));

    /* program fabric for how to get to local units */
    BCM_PBMP_CLEAR(empty);
    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        port = connection[unit].to;
        if (port < 0) {
            continue;
        }
        mod = l_entry->mod_ids[unit];
        if (mod < 0) {
            continue;
        }
        m = l_entry->base.mod_ids_req[unit];
        BCM_PBMP_PORT_SET(pbm, port);
        BCM_PBMP_ITER(config.hg, hport) {
            for (i = 0; i < m; i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_stk_ucbitmap_set(funit, hport, mod+i,
                                          hport == port ? empty : pbm));
                
            }
        }
    }
    
    /* set up module routing for fabric ports */
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;
        
        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_PBMP_ITER(config.hg, hport) {
                BCM_PBMP_CLEAR(pbm);
                if (hport != port) {
                    BCM_PBMP_PORT_ADD(pbm, port);
                }
                BCM_IF_ERROR_RETURN(
                                    bcm_stk_ucbitmap_set(funit, hport, mod, pbm));
            }
        }
    }
    
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));
    
    /*
     * Transient state setup: program  dflt modid if ports
     * are not resolved
     */
    bcm_board_unknown_src_modid_setup(l_entry);

    /*
     * Module programming for switches
     */
    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        if (unit == funit) {    /* skip fabric */
            continue;
        }
        mod = l_entry->mod_ids[unit];
        if (mod >= 0) {
            BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, mod));
        }
    }
    
#ifdef BCM_BOARD_AUTO_E2E
    /* intra-board end to end flow control, if available */
    BCM_IF_ERROR_RETURN(bcm_board_e2e_set());
#endif  /* BCM_BOARD_AUTO_E2E */

    return BCM_E_NONE;
}

int
chassis_cfm_xgs2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                  unit;
    int                  unitport[1];
    bcm_module_t         mod;
    int                  rv;
    bcm_port_t           iport, eport;
    bcm_board_cfm_info_t info;
    bcm_port_config_t    *config;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5675 BCM956000 CFM\n")));

    config = sal_alloc(sizeof(bcm_port_config_t), "bcm_port_config_t");
    if (NULL == config) {
        return BCM_E_MEMORY;
    }
    bcm_port_config_t_init(config);

    bcm_board_cfm_info(db_ref, &info);

    unit = 0;            /* board fabric unit */

    unitport[0] = -1;    /* 5675 unit 0 */

    mod = db_ref->local_entry->dest_mod;

    rv = bcm_port_config_get(unit, config);
    if (BCM_FAILURE(rv)) {
        sal_free(config);
        return (rv);
    }

    BCM_PBMP_ITER(config->hg, iport) {
        rv = bcm_stk_ucbitmap_set(unit, iport, mod, config->cpu);
        if (BCM_FAILURE(rv)) {
            sal_free(config);
            return (rv);
        }
    }

    rv = bcm_stk_my_modid_set(0, mod);
    if (BCM_FAILURE(rv)) {
        sal_free(config);
        return (rv);
    }
    if ((rv=bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport))
        == BCM_E_NONE) {
        if (!info.is_master) {
            /* If this is not the master CFM, then block multicasts */
            BCM_PBMP_ITER(config->hg, iport) {
                BCM_PBMP_ITER(config->hg, eport) {
                    rv = bcm_port_flood_block_set(unit, iport, eport, -1U);
                    if (rv < 0) {
                        break;
                    }
                }
            }
        }
    }
    sal_free(config);
    return rv;
}

int
chassis_cfm_xgs3(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                  unit;
    bcm_module_t         mod;
    int                  rv = BCM_E_NONE;
    bcm_port_t           port;
    bcm_port_t           iport, eport;
    bcm_board_cfm_info_t info;
    bcm_port_config_t    config;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56700 BCM956010 CFM\n")));
    bcm_board_cfm_info(db_ref, &info);

    unit = 0;            /* board fabric unit */

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.cpu, port) {
        break;   /* Get CPU port */
    }
    mod = db_ref->local_entry->mod_ids[0];

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, mod));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, port));
    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(unit,
                                              tp_cpu,
                                              db_ref->local_entry));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref, &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(&tp_cpu->local_entry,
                                               db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));
    if (!info.is_master) {
        /* If this is not the master CFM, then block multicasts */
        BCM_PBMP_ITER(config.hg, iport) {
            BCM_PBMP_ITER(config.hg, eport) {
                rv = bcm_port_flood_block_set(unit, iport, eport, -1U);
                if (rv < 0) {
                    break;
                }
            }
        }
    }

    return rv;
}

/*
 * Chassis line module (lm2x) board
 *     5671 + 2x5673
 * External stack ports are unit 0 ports 3,6
 */
STATIC int
_bcm_board_topomap_lm2x(int src_unit, int dest_modid,
                        bcm_port_t *exit_port)
{
    switch (src_unit) {
    case 0:  /* 5670 */
        /* check for local connections */
        BCM_BOARD_MOD_CHECK(1, 8, dest_modid, exit_port);
        BCM_BOARD_MOD_CHECK(2, 1, dest_modid, exit_port);
        return bcm_board_topomap_stk(src_unit, dest_modid, exit_port);

    case 1:  /* 5673s */
    case 2:
        *exit_port = 1;    /* IPIC_PORT(src_unit) */
        return BCM_E_NONE;

    default:
        return BCM_E_UNIT;
    }
}

int
bcm_board_topo_lm2x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[3];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5674 2XE board topology handler\n")));
    bcm_topo_map_set(_bcm_board_topomap_lm2x);

    unit = 0;            /* board fabric unit */

    unitport[0] = -1;    /* 5671 unit 0 */
    unitport[1] = 8;     /* 5673 unit 1 */
    unitport[2] = 1;     /* 5673 unit 2 */

    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
}

int
chassis_lm6x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[7];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5674 6XE board topology handler\n")));

    unit = 0;            /* board fabric unit */

    unitport[0] = -1;    /* 5671 unit 0 */
    unitport[1] = 6;     /* 5673 unit 1 */
    unitport[2] = 7;     /* 5673 unit 2 */
    unitport[3] = 8;     /* 5673 unit 3 */
    unitport[4] = 1;     /* 5673 unit 4 */
    unitport[5] = 2;     /* 5673 unit 5 */
    unitport[6] = 3;     /* 5673 unit 6 */

    return bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport);
}

int
chassis_lm_xgs2_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int    unit;
    int    unitport[5];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 48GE BCM956000 Chassis Line Card\n")));

    unit = 0;            /* board fabric unit */

    unitport[0] = -1;    /* 5675 unit 0 */
    unitport[1] = 1;     /* 5695 unit 1 is on port 1 */
    unitport[2] = 2;     /* 5695 unit 2 is on port 2 */
    unitport[3] = 3;     /* 5673 unit 3 is on port 3 */
    unitport[4] = 4;     /* 5673 unit 4 is on port 4 */

    BCM_IF_ERROR_RETURN(bcm_board_xgs_common(unit, tp_cpu, db_ref, unitport));

    return BCM_E_NONE;
}

/*
 * Firebolt Line Module
 *
 *          cfm0                       cfm1
 *
 *        |     |                    |     | 
 *     +--+-----+--+              +--+-----+--+
 *     |  0     1  |              |  0     1  |
 *     |           |              |           |
 *     |         2 +--------------+ 2         |
 *     |           |              |           |
 *     |         3 +--------------+ 3         |
 *     |   U1700   |              |    U1600  |
 *     +-----------+              +-----------+
 *
 *  Assumptions:
 *
 *  1) Each device only references Higig links 0 and 2.  This works
 *  for both untrunked and fabric trunked links.  If untrunked, links
 *  1 and 3 should be disabled. If trunked, 0 must be trunked with 1,
 *  and 2 with 3.
 *
 *  2) Links are symmetric, i.e., rxmod == txmod
 *
 *  3) CFM0 is always assigned slot 0, CFM1 is slot 1
 *
 *  4) Traffic is routed through the master. This assumption will
 *  change when traffic load balancing is implemented.
 *
 *  Therefore, if CFM0 is the master
 *
 *    U1700:MODPORT_MAP[modid(U1600)] = pbm(hg2)
 *    U1700:MODPORT_MAP[modid( cfm1)] = pbm(hg2)
 *    U1700:MODPORT_MAP[modid(other)] = pbm(hg0)
 *    U1600:MODPORT_MAP[modid( cfm1)] = pbm(hg0)
 *    U1600:MODPORT_MAP[modid(other)] = pbm(hg2)
 *
 *  Else if CFM1 is the master
 *
 *    U1600:MODPORT_MAP[modid(U1700)] = pbm(hg2)
 *    U1600:MODPORT_MAP[modid( cfm0)] = pbm(hg2)
 *    U1600:MODPORT_MAP[modid(other)] = pbm(hg0)
 *    U1700:MODPORT_MAP[modid( cfm0)] = pbm(hg0)
 *    U1700:MODPORT_MAP[modid(other)] = pbm(hg2)
 *
 *  Else if self is master
 *
 *    U1600:MODPORT_MAP[ modid(U1700)] = pbm(hg2)
 *    U1700:MODPORT_MAP[ modid(U1600)] = pbm(hg2)
 */

STATIC int
bcm_board_xgs3_fb_lm(int master_unit,       /* Unit connected to CFM master */
                     int master_modid,      /* Modid of master unit */
                     int partner_unit,      /* Unit connected to CFM !master */
                     int partner_modid,     /* Modid of partner unit */
                     int internal_port,     /* Internal port */
                     topo_cpu_t *tp_cpu,    /* Topology */
                     cpudb_ref_t db_ref)    /* Database information */
{
    int                 i, m;
    topo_stk_port_t     *tsp;
    int                 port;
    cpudb_entry_t       *l_entry;
    bcm_module_t        mod;
    bcm_port_config_t   config;
    int                 spunit, npunit;
    int                 auto_trunk;

    l_entry = &tp_cpu->local_entry;
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        if (!(l_entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED)) {
            /* Skip unresolved ports */
            continue;
        }

        /* spunit is the unit that this stack port is connected to */
        spunit = l_entry->base.stk_ports[i].unit;
        
        /* npunit is the unit of the other device */
        npunit = (spunit == master_unit) ? partner_unit : master_unit;
        
        /* For each mod-id reached by this port, set both devices
           (spunit and npunit). For spunit (the unit this stack port
           is on), set the modmap to the stack port. For npunit (the
           unit this stack port is *not on), set the modmap to the
           internal port. */

        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(spunit, mod, port));
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(npunit, mod,
                                                    internal_port));
        }
    }

    /* The internal_port must be the same on both devices. */
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(master_unit,
                                            partner_modid,
                                            internal_port));
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(master_unit,
                                                        internal_port));
    BCM_IF_ERROR_RETURN(bcm_board_auto_trunk_get(&auto_trunk));
    if (auto_trunk) {
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(master_unit,
                                                            internal_port+1));
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(partner_unit,
                                                            internal_port+1));
    }

    BCM_IF_ERROR_RETURN(bcm_port_config_get(master_unit, &config));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(master_unit, tp_cpu, db_ref,
                                                &config));

    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(partner_unit,
                                            master_modid,
                                            internal_port));
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(partner_unit,
                                                        internal_port));
    BCM_IF_ERROR_RETURN(bcm_port_config_get(partner_unit, &config));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(partner_unit, tp_cpu, db_ref,
                                                &config));

    
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    return BCM_E_NONE;
}


#define FB_UNIT0 0
#define FB_UNIT1 1
#define FB_HG0 24
#define FB_HG1 25
#define FB_HG2 26
#define FB_HG3 27

int
_bcm_board_setup_trunk_lm_xgs3_48g(void)
{
    bcm_trunk_info_t     trunk;
    bcm_trunk_chip_info_t    ti;
    bcm_trunk_member_t   member_array[2];
    
    sal_memset(&trunk, 0, sizeof(trunk));
    trunk.psc        = -1;
    trunk.dlf_index  = -1;
    trunk.mc_index   = -1;
    trunk.ipmc_index = -1;

    bcm_trunk_member_t_init(&member_array[0]);
    member_array[0].gport = BCM_GPORT_DEVPORT(FB_UNIT0, FB_HG2);
    bcm_trunk_member_t_init(&member_array[1]);
    member_array[1].gport = BCM_GPORT_DEVPORT(FB_UNIT0, FB_HG3);

    BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(FB_UNIT0, &ti));
    bcm_trunk_destroy(FB_UNIT0, ti.trunk_fabric_id_max);
    BCM_IF_ERROR_RETURN(bcm_trunk_create(FB_UNIT0,
                                         BCM_TRUNK_FLAG_WITH_ID,
                                         &ti.trunk_fabric_id_max));
    BCM_IF_ERROR_RETURN(bcm_trunk_set(FB_UNIT0,
                                      ti.trunk_fabric_id_max, &trunk,
                                      2, member_array));

    bcm_trunk_member_t_init(&member_array[0]);
    member_array[0].gport = BCM_GPORT_DEVPORT(FB_UNIT1, FB_HG2);
    bcm_trunk_member_t_init(&member_array[1]);
    member_array[1].gport = BCM_GPORT_DEVPORT(FB_UNIT1, FB_HG3);

    BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(FB_UNIT1, &ti));
    bcm_trunk_destroy(FB_UNIT1, ti.trunk_fabric_id_max);
    BCM_IF_ERROR_RETURN(bcm_trunk_create(FB_UNIT1,
                                         BCM_TRUNK_FLAG_WITH_ID,
                                         &ti.trunk_fabric_id_max));
    BCM_IF_ERROR_RETURN(bcm_trunk_set(FB_UNIT1,
                                      ti.trunk_fabric_id_max, &trunk,
                                      2, member_array));

    return BCM_E_NONE;
}

/* Two FBs on a BCM956000 LM */
int
chassis_lm_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                      unit0_modid;
    int                      unit1_modid;
    int                      master_slot;

    /* If there's no master yet, set self as master */
    master_slot = db_ref->master_entry ?
        db_ref->master_entry->base.slot_id : -1;
    unit0_modid = tp_cpu->local_entry.mod_ids[FB_UNIT0];
    unit1_modid = tp_cpu->local_entry.mod_ids[FB_UNIT1];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56504 48GE BCM956000 Chassis Line Card\n")));
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("      master on slot %d\n"),
                 master_slot));

    switch (master_slot) {
    case 0:
        /* CFM0 Master */
        BCM_IF_ERROR_RETURN
            (bcm_board_xgs3_fb_lm(FB_UNIT1, unit1_modid,
                                  FB_UNIT0, unit0_modid,
                                  FB_HG2, tp_cpu, db_ref));
        break;
    case 1:
        /* CFM1 Master */
        BCM_IF_ERROR_RETURN
            (bcm_board_xgs3_fb_lm(FB_UNIT0, unit0_modid,
                                  FB_UNIT1, unit1_modid,
                                  FB_HG2, tp_cpu, db_ref));
        break;
    default:
        /* LM (self) Master */
        BCM_IF_ERROR_RETURN
            (bcm_board_xgs3_fb_lm(FB_UNIT1,
                                  unit1_modid,
                                  FB_UNIT0,
                                  unit0_modid, FB_HG2, tp_cpu, db_ref));
        /* If master not elected, don't set up trunks yet */
        break;
    }    

    /* The CFM trunk was handled by bcm_board_xgs3_fb_lm() above.
       Set up the intraboard trunk here. */
    BCM_IF_ERROR_RETURN(_bcm_board_setup_trunk_lm_xgs3_48g());
    
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(FB_UNIT0, unit0_modid));
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(FB_UNIT1, unit1_modid));

#ifdef BCM_BOARD_AUTO_E2E
    /* intra-board end to end flow control, if available */
    BCM_IF_ERROR_RETURN(_bcm_board_dual_xgs3_e2e_set(FB_UNIT0, unit0_modid,
                                                     FB_UNIT1, unit1_modid));
#endif  /* BCM_BOARD_AUTO_E2E */

    return BCM_E_NONE;
}

#undef FB_UNIT0
#undef FB_UNIT1

/* Four 56501 FBs on a BCM956000 LM */
int
chassis_lm_xgs3_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int               unit;
    int               rv = BCM_E_NONE;
    bcm_board_port_connection_t connection[5];
    int               master_slot;

    master_slot = db_ref->master_entry ?
        db_ref->master_entry->base.slot_id : -1;
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56501 12XE BCM956000 Chassis Line Card\n")));
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("      master on slot %d\n"),
                 master_slot));

    unit = 0;    /* board fabric unit */

    /* 5675 unit 0 - no connection map entry */
    connection[0].from = -1;
    connection[0].to   = -1;

    /* 56501 unit 1 port 27 connects to 5675 port 7 */
    connection[1].from = FB_HG3;
    connection[1].to   = 7;

    /* 56501 unit 2 port 27 connects to 5675 port 8 */
    connection[2].from = FB_HG3;
    connection[2].to   = 8;

    /* 56501 unit 3 port 27 connects to 5675 port 1 */
    connection[3].from = FB_HG3;
    connection[3].to   = 1;

    /* 56501 unit 4 port 27 connects to 5675 port 2 */
    connection[4].from = FB_HG3;
    connection[4].to   = 2;
    rv = bcm_board_xgs3_sw_xgs_fabric(unit, tp_cpu, db_ref, connection);

    return rv;
}

int
chassis_lm_56800_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                  unit;
    bcm_module_t         mod;
    bcm_port_t           port;
    bcm_port_config_t    config;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56800 LM\n")));

    unit = 0;            /* board fabric unit */

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.cpu, port) {
        break;   /* Get CPU port */
    }
    mod = db_ref->local_entry->mod_ids[0];
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, port));
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, mod));
    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(unit,
                                              tp_cpu,
                                              db_ref->local_entry));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref, &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(&tp_cpu->local_entry,
                                               db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    return BCM_E_NONE;
}

#else
typedef int _brd_chassis_nolb_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_CHASSIS */

