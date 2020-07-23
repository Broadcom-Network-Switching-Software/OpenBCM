/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_xgs3.c
 * Purpose:     XGS3 Based SDK and Reference Design board programming
 */

#include <shared/bsl.h>

#include <soc/property.h>           /* for Raptor support */
#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/trunk.h>
#include <bcm/topo.h>
#include <bcm/init.h>               /* bcm_info_get */

#include <appl/stktask/topo_brd.h>
#include <sal/appl/config.h>

#include "topo_int.h"

/* Set up a Firebolt device. */
STATIC int
_bcm_board_xgs3_fb(int unit, topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                 i, m;
    topo_stk_port_t     *tsp;
    int                 port;
    cpudb_entry_t       *l_entry;
    bcm_module_t        mod;
    bcm_port_config_t   config;
    
    /*
     * Generate the bitmap of egress ports for each mod id;
     * Do not exclude the higig port from the bitmap b/c the
     * FB ucbitmap set applies to the chip, not the ingress port.
     */

    l_entry = &tp_cpu->local_entry;
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        if (unit != l_entry->base.stk_ports[i].unit) {
            /* Only looking at this device for now */
            continue;
        }

        /* For each mod-id reached by this port, set in device */
        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, port));
        }
    }
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    return BCM_E_NONE;
}

STATIC int
_bcm_board_banner(int unit, char *prefix)
{
    bcm_info_t info;
    uint32 dev;

    BCM_IF_ERROR_RETURN(bcm_info_get(unit, &info));

    /* Make the device ID correspond to the actual part number */

    dev = info.device;
    
    if (dev > 0x5699) {
        dev += 0x4b000;
    }
    
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: %sBCM%x SDK\n"),
                 prefix, dev));

    return BCM_E_NONE;
}

/* Single Fireolt or Firebolt-like device on a board, unit number is 0 */

#define FB_UNIT 0
int
bcm_board_topo_sdk_xgs3_24g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int         modid;

    _bcm_board_banner(FB_UNIT, "");

    /* Use the default stackable mod ID mapping since only one unit on brd */
    bcm_topo_map_set(bcm_board_topomap_stk);

    modid = tp_cpu->local_entry.mod_ids[FB_UNIT];
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(FB_UNIT, modid));

    return _bcm_board_xgs3_fb(FB_UNIT, tp_cpu, db_ref);
}
#undef FB_UNIT

/* Support functions for dual XGS3 SDK and Reference designs */

/* Internal trunk setup */

#define NUMXGS3DEV 2

STATIC int
_bcm_board_setup_trunk_dual_xgs3(xgs3devinfo_t dev[NUMXGS3DEV])
{
    int i;
    bcm_trunk_info_t         trunk;
    bcm_trunk_chip_info_t    ti;
    int                      member_count;
    bcm_trunk_member_t       member_array[2];
    
    sal_memset(&trunk, 0, sizeof(trunk));
    trunk.psc        = -1;
    trunk.dlf_index  = -1;
    trunk.mc_index   = -1;
    trunk.ipmc_index = -1;

    member_count = 2;

    for ( i = 0; i < NUMXGS3DEV; i++ ) {
        
        bcm_trunk_member_t_init(&member_array[0]);
        member_array[0].gport = BCM_GPORT_DEVPORT(dev[i].unit,
                                                  dev[i].intport[0]);
        bcm_trunk_member_t_init(&member_array[1]);
        member_array[1].gport = BCM_GPORT_DEVPORT(dev[i].unit,
                                                  dev[i].intport[1]);

        BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(dev[i].unit, &ti));
        bcm_trunk_destroy(dev[i].unit, ti.trunk_fabric_id_max);
        BCM_IF_ERROR_RETURN(bcm_trunk_create(dev[i].unit,
                                             BCM_TRUNK_FLAG_WITH_ID,
                                             &ti.trunk_fabric_id_max));
        BCM_IF_ERROR_RETURN(bcm_trunk_set(dev[i].unit,
                                          ti.trunk_fabric_id_max, &trunk,
                                          member_count,
                                          member_array));
    }
    
    return BCM_E_NONE;
}


/* Generic board programming for dual XGS3 designs */

STATIC int
_bcm_board_dual_xgs3(xgs3devinfo_t dev[NUMXGS3DEV], topo_cpu_t *tp_cpu,
                     cpudb_ref_t db_ref)
{
    int                 i, m;
    topo_stk_port_t     *tsp;
    int                 port;
    cpudb_entry_t       *l_entry;
    bcm_module_t        mod;
    bcm_port_config_t   config;
    int                 sp, np;
    int                 auto_trunk;

    /* Set devinfo device modids */
    for ( i = 0; i < NUMXGS3DEV; i++ ) {
        dev[i].modid = tp_cpu->local_entry.mod_ids[i];
    }
    
    l_entry = &tp_cpu->local_entry;

    /* Set module mapping based on modids reachable by the stack ports */
    
    for ( i = 0; i < l_entry->base.num_stk_ports; i++) {
        if (!(l_entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED)) {
            /* Skip unresolved ports */
            continue;
        }

        /* Set sp (stack-port-dev) and np (non-stack-port-dev) */
        if (dev[0].unit == l_entry->base.stk_ports[i].unit) {
            /* sp is the dev in devinfo that this stack port is connected to */
            /* np is the other (non stack port) dev */
            sp = 0;
            np = 1;
        } else {
            sp = 1;
            np = 0;
        }

        /* For each modid reached by this port, set both devices (s
           and np). For sp (the device this stack port is on), set the
           modmap to the stack port. For np (the device this stack
           port is *not* on), set the modmap to the internal port. */

        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(dev[sp].unit, mod, port));
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(dev[np].unit, mod,
                                                    dev[np].intport[0]));
        }
    }

    /* Set the module mapping, internal stack ports and module
       filtering for the two devices themselves. */

    BCM_IF_ERROR_RETURN(bcm_board_auto_trunk_get(&auto_trunk));
    for ( i = 0; i < NUMXGS3DEV; i++ ) {
        int j = i^1; /* The other device */
        
        BCM_IF_ERROR_RETURN(bcm_stk_modport_set(dev[i].unit,
                                                dev[j].modid,
                                                dev[i].intport[0]));
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(dev[i].unit,
                                                        dev[i].intport[0]));
        if (dev[i].intport[1] >= 0 && auto_trunk) {
            BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(dev[i].unit,
                                                       dev[i].intport[1]));
        }
        BCM_IF_ERROR_RETURN(bcm_port_config_get(dev[i].unit, &config));
        BCM_IF_ERROR_RETURN(bcm_board_module_filter(dev[i].unit,
                                                    tp_cpu, db_ref, &config));

        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(dev[i].unit,
                                                 dev[i].modid));
    }

    /* Set up internal trunking */

    if (dev[0].intport[1] >= 0 && dev[1].intport[1] >= 0) {
        _bcm_board_setup_trunk_dual_xgs3(dev);
    }
    
    /* Set up external trunking */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

#ifdef BCM_BOARD_AUTO_E2E
    /* intra-board end to end flow control, if available */
    BCM_IF_ERROR_RETURN(_bcm_board_dual_xgs3_e2e_set(dev[0].unit, dev[0].modid,
                                                     dev[1].unit,
                                                     dev[1].modid));
#endif  /* BCM_BOARD_AUTO_E2E */

    return BCM_E_NONE;
}

/* sdk_xgs3_48f */

xgs3devinfo_t xgs3_48f_devinfo[NUMXGS3DEV] = {
    {
        0,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, -1 }      /* internal stack port (not trunked) */
    },
    {
        1,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, -1 }      /* internal stack port (not trunked) */
    }
};


int
bcm_board_topo_xgs3_48f(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 (2)56102 48FE+2GE+2XHG\n")));

    BCM_IF_ERROR_RETURN
        (_bcm_board_dual_xgs3(xgs3_48f_devinfo, tp_cpu, db_ref));

    return BCM_E_NONE;
}

/* sdk_xgs3_48g */

xgs3devinfo_t xgs3_48g_devinfo[NUMXGS3DEV] = {
    {
        0,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, 27 }      /* internal stack ports (trunked) */
    },
    {
        1,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, 27 }      /* internal stack ports (trunked) */
    }
};

int
bcm_board_topo_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    _bcm_board_banner(0, "(2)");

    BCM_IF_ERROR_RETURN
        (_bcm_board_dual_xgs3(xgs3_48g_devinfo, tp_cpu, db_ref));

    return BCM_E_NONE;
}

int
bcm_board_topo_xgs3_20x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                  unit;
    bcm_module_t         mod;
    bcm_port_t           port;
    bcm_port_config_t    config;

    unit = 0;            /* board fabric unit */

    _bcm_board_banner(unit, "");
    
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
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(&tp_cpu->local_entry,
                                               db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    return BCM_E_NONE;
}

int
bcm_board_topo_xgs3_48g5g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int modid;
    bcm_port_config_t    config;
    int unit = 0;

    _bcm_board_banner(unit, "");

    BCM_IF_ERROR_RETURN(bcm_board_num_modid_set(16));

    /* Use the default stackable mod ID mapping since only one unit on brd */
    bcm_topo_map_set(bcm_board_topomap_stk);

    modid = tp_cpu->local_entry.mod_ids[0];
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, modid));
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(unit,
                                              tp_cpu,
                                              db_ref->local_entry));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));

    /* If there are only two CPUs, deal with any possible untrunkable
       stack ports */
    if (db_ref->num_cpus == 2) {
        BCM_IF_ERROR_RETURN(bcm_board_untrunkable_stack_ports(unit, tp_cpu));
    }

    return BCM_E_NONE;

}

/* sdk_xgs3_48g2 */

xgs3devinfo_t xgs3_48g2_devinfo[NUMXGS3DEV] = {
    {
        0,              /* unit */
        -1,             /* modid (dynamic) */
        { 1, -1 }       /* internal stack port (not trunked) */
    },
    {
        1,              /* unit */
        -1,             /* modid (dynamic) */
        { 1, -1 }       /* internal stack port (not trunked) */
    }
};


#if defined(INCLUDE_RCPU) && defined(BCM_ESW_SUPPORT) && !defined(NO_SAL_APPL)
int
bcm_board_topo_xgs3_48g2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    extern int _bcm_esw_rcpu_init(int);
    char modstr[16];
    int rv;
    int mode, uidx, pidx;
    char *rcpu_port_str;
    int rcpu_port, rcpu_higig;

    _bcm_board_banner(0, "(2)");

    rcpu_port_str = sal_config_get(spn_RCPU_PORT);
    if (rcpu_port_str == NULL) {
        return BCM_E_CONFIG;
    }
    rcpu_port = sal_ctoi(rcpu_port_str, NULL);
    rcpu_higig = FALSE;

    /* See if any internal 2.5G port is trunkable */
    xgs3_48g2_devinfo[0].intport[0] = 1;
    xgs3_48g2_devinfo[1].intport[0] = 1;
    xgs3_48g2_devinfo[0].intport[1] = 4;
    xgs3_48g2_devinfo[1].intport[1] = 4;

    for (uidx=0; uidx < 2; uidx++) {
        for (pidx=0; pidx < 2; pidx++) {
            if (xgs3_48g2_devinfo[uidx].intport[pidx] < 0) {
                continue;
            }
            BCM_IF_ERROR_RETURN
                (bcm_port_encap_get(xgs3_48g2_devinfo[uidx].unit,
                                    xgs3_48g2_devinfo[uidx].intport[pidx],
                                    &mode));
            if (mode == BCM_PORT_ENCAP_IEEE) {
                /* If either port is not higig, neither port is trunkable */
                xgs3_48g2_devinfo[0].intport[pidx] = -1;
                xgs3_48g2_devinfo[1].intport[pidx] = -1;
            } else if (xgs3_48g2_devinfo[uidx].intport[pidx] == rcpu_port) {
                rcpu_higig = TRUE;
            }
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_board_dual_xgs3(xgs3_48g2_devinfo, tp_cpu, db_ref));

    /* Disable ge2 */
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(0, 3, 0));
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(1, 3, 0));

    /* Manage RCPU higig properties */
    if (sal_config_set(spn_RCPU_HIGIG_PORT, rcpu_higig ? "1" : "0") != 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: " spn_RCPU_HIGIG_PORT " set error\n")));
    }

    if (sal_config_set(spn_RCPU_RX_PBMP ".1",
                       rcpu_higig ? "0x12" :
                       ((rcpu_port == 4) ? "0x10" : "0x02")) != 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: " spn_RCPU_RX_PBMP ".1 set error\n")));
    }
    
    /* reinit RCPU subsystem with new modids */
    sal_sprintf(modstr, "%d", db_ref->local_entry->mod_ids[0]);
    if (sal_config_set(spn_RCPU_MASTER_MODID, modstr) != 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: " spn_RCPU_MASTER_MODID " set error\n")));
    }
    rv = _bcm_esw_rcpu_init(0);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: rcpu master init error %d (%s)\n"),
                     rv, bcm_errmsg(rv)));
    }

    sal_sprintf(modstr, "%d", db_ref->local_entry->mod_ids[1]);
    if (sal_config_set(spn_RCPU_SLAVE_MODID, modstr) != 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: " spn_RCPU_SLAVE_MODID " set error\n")));
    }
    rv=_bcm_esw_rcpu_init(1);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: rcpu slave init error %d (%s)\n"),
                     rv, bcm_errmsg(rv)));
    }

    return BCM_E_NONE;
}

#else

int
bcm_board_topo_xgs3_48g2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);

    return BCM_E_UNAVAIL;
}

#endif /* defined(INCLUDE_RCPU) && defined(BCM_ESW_SUPPORT)
          && !defined(NO_SAL_APPL) */
