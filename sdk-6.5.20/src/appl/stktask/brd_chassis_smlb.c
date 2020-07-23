/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_chassis_smlb.c
 * Purpose:     BCM956000 Chassis source-modid based load balancing
 */

#include <shared/bsl.h>
#include <shared/alloc.h>

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/topo.h>
#include <bcm/mirror.h>
#include <appl/stktask/topo_brd.h>
#include <appl/stktask/stktask.h>       /* bcm_st_reserved_modid_enable_get */
#include <appl/cputrans/nh_tx.h>        /* nh_tx_src_get */
#include <appl/cputrans/cpu2cpu.h>      /* nh_tx_src_get */

#include "topo_int.h"
#include "brd_chassis_int.h"

#if defined(INCLUDE_CHASSIS)

/*
 * Chassis cfm board
 *     5675
 * External stack ports are unit 0 ports 1-8
 */

/*
 * Support functions for programming XGS3 switches connected to an XGS
 * fabric. XGS3 switches may have more than one Higig port that could
 * connect to a fabric, so it is no longer possible to rely on
 * bcm_stk_port_set() to return 'the' stack port; board programming
 * has to be told which port to use. The connection array, indexed by
 * unit, contains the connectivity. connection.from is the switch port
 * of the indexed unit, connection.to is the fabric port (on unit
 * 'funit').
 */

#define EVEN_IDX              0
#define ODD_IDX               1
#define IS_ODD(i)             (i % 2)
#define IS_EVEN(i)            (!IS_ODD(i))
#define EVEN_ODD_IDX_GET(i)   ((IS_EVEN(i)) ? EVEN_IDX : ODD_IDX)

/*
 * Function:    topo_chassis_modids_assign
 *
 * Purpose:
 *     Assign module IDs to the specified CPU database.
 * Parameters:
 *     db_ref   - Pointer to CPU database
 * Returns:
 *    BCM_E_NONE
 */

int
topo_chassis_modids_assign(cpudb_ref_t db_ref)
{
    cpudb_entry_t            *entry;
    int                      unit;
    bcm_module_t             modid;

    /* 
     * Notes:
     *    Since the board type can only be inquired locally,
     *    the 'mod_ids_req' field will be used to assign mod ids.
     *
     *    Eventually, we want the master to know the
     *    board type for each slot.  Perhaps, this information
     *    can be passed around during discovery, or, the board
     *    type can be programmed in the EEPROM which can be accessed
     *    by the master module.
     */

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: Assigning chassis mod ids\n")));
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        modid = MODID_OFFSET_GET(entry->base.slot_id);

        if (entry->base.num_units == 2) {
            /* 
             * Special modID assignment for LM XGS3-48G
             *     Unit0 connected to CFM1 needs an ODD modID
             *     Unit1 connected to CFM0 needs an EVEN modID
             * Since the modID for a LM starts at an even number,
             * increment to get odd for unit0.
             */
            modid++;
        }

        /*
         * Special modID assignment for 1 unit LMs based
         * on LM even/odd slot,
         *     LM on odd slot get an ODD modID
         *     LM on even slot get an EVEN modID
         */
        if ((IS_LM_SLOT(entry->base.slot_id)) &&
            (entry->base.num_units == 1)){
            if (IS_ODD(entry->base.slot_id)) {
                /* This assumes that modID for a LM starts at even number */
                modid++;
            }
        }

        for (unit = 0; unit < entry->base.num_units; unit++) {
            if (entry->base.mod_ids_req[unit] > 0) {
                entry->mod_ids[unit] = modid;
                modid += entry->base.mod_ids_req[unit];
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    is_slot_valid
 *
 * Purpose:
 *     Checks if specified slot is valid.
 * Parameters:
 *     slot      - Slot to check
 *     db_ref    - Pointer to CPU database
 * Returns:
 *    TRUE  - If slot is valid
 *    FALSE - If slot is not valid
 * Notes:
 *     Currently, a slot is considered valid if the slot is 
 *     found in the CPU database.
 */
STATIC int
is_slot_valid(int slot, cpudb_ref_t db_ref)
{
    cpudb_entry_t *cur;

    if (db_ref) {
        CPUDB_FOREACH_ENTRY(db_ref, cur) {
            if (cur->base.slot_id == slot) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*
 * Function:    chassis_unit_modid_get
 *
 * Purpose:
 *     Returns the mod ID for given unit.
 *     (-1) indicates an invalid mod ID.
 */
STATIC int
chassis_unit_modid_get(int unit, cpudb_entry_t *entry)
{
    int modid = -1;

    /* 
     * Notes:
     *    If the 'board type' is known for all remote devices,
     *    the module ID can be extracted using the 'slot' and 'unit',
     *    without using the cpudb information.
     */
    if (entry->base.mod_ids_req[unit] > 0) {
        modid = entry->mod_ids[unit];
    }

    return modid;
}


/*
 * The following function wrappers help isolate the device specific
 * BCM call for setting the modport mapping.
 */

/*
 * Function:    xgs2_modport_set_hport
 *
 * Purpose:
 *     Set the dstmod-port mapping on specified HiGig stack port for
 *     given unit.
 * Notes:
 *     The mod-port mapping is ONLY set for the indicated HiGig port.
 */
STATIC int
xgs2_modport_set_hport(int unit, bcm_port_t hport,
                       int dst_modid, bcm_port_t stk_port)
{
    bcm_pbmp_t           empty;
    bcm_pbmp_t           pbmp; 

    BCM_PBMP_CLEAR(empty);
    BCM_PBMP_PORT_SET(pbmp, stk_port);
    BCM_IF_ERROR_RETURN(bcm_stk_ucbitmap_set(unit, hport, dst_modid,
                                             hport == stk_port ?
                                             empty : pbmp));
    return BCM_E_NONE;
}


/*
 * The following function wrappers help isolate the device specific
 * BCM call for setting the modport mapping.
 */

/*
 * Function:    xxx_modport_set
 *
 * Purpose:
 *     Set the dstmod-port mapping on given unit.
 */
STATIC int
xgs2_modport_set(int unit, int dst_modid, bcm_port_t stk_port)
{
    bcm_port_config_t    config;
    bcm_port_t           hport;
    bcm_pbmp_t           empty;
    bcm_pbmp_t           pbmp; 

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_CLEAR(empty);
    BCM_PBMP_PORT_SET(pbmp, stk_port);
    BCM_PBMP_ITER(config.hg, hport) {
        BCM_IF_ERROR_RETURN(bcm_stk_ucbitmap_set(unit, hport, dst_modid,
                                                 hport == stk_port ?
                                                 empty : pbmp));
    }

    return BCM_E_NONE;
}
STATIC int
xgs3_modport_set(int unit, int dst_modid, bcm_port_t stk_port)
{
    return bcm_stk_modport_set(unit, dst_modid, stk_port);
}

/*
 * Function:    xxx_internal_sp_set
 *
 * Purpose:
 *     Notify BCM layer of internal stack ports on given unit.
 */
STATIC int
xgs3_internal_sp_set(int unit, int dst_unit, bcm_port_t stk_port)
{
    COMPILER_REFERENCE(dst_unit);

    /* Notify BCM layer of internal stack ports */
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(unit, stk_port));

    return BCM_E_NONE;
}
STATIC int
xgs2_internal_sp_set(int unit, int dst_unit, bcm_port_t stk_port)
{
    uint32 flags;

    /* Notify BCM layer of internal stack ports */
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(unit, stk_port));

    if (bcm_stk_port_get(dst_unit, BCM_STK_USE_HG_IF, &flags) < 0) {
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META_U(unit,
                 "Could not get IPIC stack info for unit %d\n"),
                  dst_unit));
    } else {
        if (!(flags & BCM_STK_ENABLE)) {
            BCM_IF_ERROR_RETURN(bcm_stk_port_set(dst_unit,
                                                 BCM_STK_USE_HG_IF,
                                                 BCM_BOARD_INT_STK_FLAGS));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_iroute
 *
 * Purpose:
 *     Set the forwarding of UC packets among local units on given board.
 */
STATIC int
chassis_iroute(int num_units, to_unit_t *to_unit, cpudb_ref_t db_ref)
{
    cpudb_entry_t    *l_entry;
    int              src_unit;
    bcm_port_t       stk_port;
    int              dst_unit;
    int              dst_num_units;
    bcm_module_t     modid;
    modport_set_f    modport_set;
    int_sp_set_f     internal_sp_set;
    to_unit_t        *to_unit_ptr;
    int              u, i;
    int              auto_trunk;


    /* Set routes to local units */
    l_entry = db_ref->local_entry;
    to_unit_ptr = to_unit;
    
    BCM_IF_ERROR_RETURN(bcm_board_auto_trunk_get(&auto_trunk));
    for (src_unit = 0; src_unit < num_units; src_unit++, to_unit_ptr++) {
        dst_num_units   = to_unit_ptr->unit_cnt;
        modport_set     = to_unit_ptr->modport_set_fn;
        internal_sp_set = to_unit_ptr->internal_sp_set_fn;

        for (u = 0; u < dst_num_units; u++) {
            dst_unit = to_unit_ptr->unit[u].dst;
            stk_port = to_unit_ptr->unit[u].sp.port[0];
            modid = chassis_unit_modid_get(dst_unit, l_entry);
            if (modid >= 0) {
                BCM_IF_ERROR_RETURN(modport_set(src_unit, modid, stk_port));
            }

            for (i = 0; i < to_unit_ptr->unit[u].sp.port_cnt; i++) {
                stk_port = to_unit_ptr->unit[u].sp.port[i];
                BCM_IF_ERROR_RETURN(internal_sp_set(src_unit, dst_unit, stk_port));

                /* Only allow 1 internal stack port, if auto trunk is disabled */
                if (!auto_trunk) {
                    break;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_eroute_cfm
 *
 * Purpose:
 *     Set the forwarding of UC packets to CFMs on specified LM.
 * Notes:
 *     'sp_list' returns the list of valid stack ports for CFMs:
 *     The port in index 0 will contain the stack port for the even slot CFM,
 *     the port in index 1 will contain the stack port for the odd slot CFM.
 */
STATIC int
chassis_eroute_lm_cfm(int num_units, to_slot_t *to_cfm, sp_list_t *sp_list,
                      cpudb_ref_t db_ref)
{
    int              dst_slot;
    bcm_port_t       stk_port;
    int              cfm;
    int              unit;
    int              sp_cnt;
    int              sp_idx;
    bcm_module_t     dst_modid;
    modport_set_f    modport_set;


    /* Init sp list */
    for (unit = 0; unit < num_units; unit++) {
        sp_list[unit].port_cnt = 0;
        for (sp_idx = 0; sp_idx < STK_PORTS_MAX; sp_idx++) {
            sp_list[unit].port[sp_idx] = -1;
        }
    }

    /* Set routes to CFMs */
    for (unit = 0; unit < num_units; unit++) {
        sp_cnt = sp_idx= 0;

        for (cfm = 0; cfm < to_cfm[unit].slot_cnt; cfm++) {
            dst_slot = to_cfm[unit].slot[cfm].dst;
            if (!is_slot_valid(dst_slot, db_ref)) {
                continue;
            }
            stk_port    = to_cfm[unit].slot[cfm].sp.port[0];
            modport_set = to_cfm[unit].modport_set_fn;

            FOREACH_MODID(dst_slot, dst_modid) {
                BCM_IF_ERROR_RETURN(modport_set(unit, dst_modid, stk_port));
            }

            /* Add port to list of available external stack ports */
            sp_idx = EVEN_ODD_IDX_GET(dst_slot);
            sp_list[unit].port[sp_idx] = stk_port;
            sp_cnt++;
        }
        sp_list[unit].port_cnt = sp_cnt;
    }
    
    return BCM_E_NONE;
}

/*
 * Function:    chassis_xgs2_eroute_lm_lm
 *
 * Purpose:
 *     Set the forwarding of UC packets on given unit (XGS2 fabric)
 *     to other LMs.
 */
STATIC int
chassis_xgs2_eroute_lm_lm(int unit, to_unit_t *to_unit,
                          sp_list_t *sp_list, cpudb_ref_t db_ref)
{
    cpudb_entry_t    *l_entry;
    int              l_slot;
    int              dst_slot;
    bcm_port_t       hport;
    bcm_port_t       stk_port;
    bcm_module_t     src_modid;
    bcm_module_t     dst_modid;
    int              i;

    if (sp_list->port_cnt == 0) {
        return BCM_E_NONE;
    }

    l_entry = db_ref->local_entry;
    l_slot = l_entry->base.slot_id;
    for (i = 0; i < to_unit->unit_cnt; i++) {
        src_modid = chassis_unit_modid_get(to_unit->unit[i].dst, l_entry);
        if (src_modid < 0) {
            continue;
        }

        if (IS_EVEN(src_modid)) {
            if ((stk_port = sp_list->port[EVEN_IDX]) < 0) {
                if ((stk_port = sp_list->port[ODD_IDX]) < 0) {
                    continue;    /* No valid stack port */
                }
            }
        } else {
            if ((stk_port = sp_list->port[ODD_IDX]) < 0) {
                if ((stk_port = sp_list->port[EVEN_IDX]) < 0) {
                    continue;    /* No valid stack port */
                }
            }
        }

        hport = to_unit->unit[i].sp.port[0];
        for (dst_slot = LM_SLOT_OFFSET; dst_slot < NUM_SLOT; dst_slot++) {
            if (dst_slot == l_slot) {    /* Skip local slot */
                continue; 
            }
            FOREACH_MODID(dst_slot, dst_modid) {
                BCM_IF_ERROR_RETURN(xgs2_modport_set_hport(unit, hport,
                                                           dst_modid, stk_port));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_xgs3_eroute_lm_lm
 *
 * Purpose:
 *     Set the forwarding of UC packets on specified unit (XGS3 switch)
 *     with indicated stack port to other LMs.
 */
STATIC int
chassis_xgs3_eroute_lm_lm(int unit, bcm_port_t stk_port, int l_slot)
{
    int              dst_slot;
    bcm_module_t     dst_modid;

    /* Set routes to other LMs */
    for (dst_slot = LM_SLOT_OFFSET; dst_slot < NUM_SLOT; dst_slot++) {
        if (dst_slot == l_slot) {    /* Skip local slot */
            continue; 
        }
        FOREACH_MODID(dst_slot, dst_modid) {
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, dst_modid, stk_port));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_internal_modid_filter
 *
 * Purpose:
 *     Set source mod IDs filtering for internal stack ports.
 */
/*
 * Function:    chassis_internal_modid_filter
 *
 * Purpose:
 *     Set source mod IDs filtering for internal stack ports.
 */
STATIC int
chassis_internal_modid_filter(int num_units,
                              sp_list_t *internal_sp)
{
    int           unit;
    int           i;
    bcm_port_t    stk_port;

    /* Allow any mod IDs for internal stack ports */
    for (unit = 0; unit < num_units; unit++) {
        for(i = 0; i < internal_sp[unit].port_cnt; i++) {
            stk_port = internal_sp[unit].port[i];
            BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                          -1, TRUE));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_external_modid_filter
 *
 * Purpose:
 *     Set mod IDs filtering for external stack ports on
 *     specified unit.
 */
STATIC int
chassis_external_modid_filter(int unit, sp_list_t *external_sp, int l_slot, 
                              cpudb_ref_t db_ref)
{
    bcm_port_t           stk_port;
    bcm_module_t         src_modid;
    int                  slot;
    int                  enable;
    int                  i;


    /* Enable CPU source mod ID */
    BCM_IF_ERROR_RETURN(c2c_src_mod_get(&src_modid));
    for (i = 0; i < external_sp->port_cnt; i++) {
        stk_port = external_sp->port[i];
        BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                      src_modid, TRUE));
    }

    /* Enable source mod IDs from other valid slots */
    for (slot = 0; slot < NUM_SLOT; slot++) {
        if ((is_slot_valid(slot, db_ref)) && (slot != l_slot)) {
            enable = TRUE;
        } else {
            enable = FALSE;
        }

        FOREACH_MODID(slot, src_modid) {
            for(i = 0; i < external_sp->port_cnt; i++) {
                stk_port = external_sp->port[i];
                BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                              src_modid,
                                                              enable));
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:    chassis_nh_modid_filter
 *
 * Purpose:
 *     Enable NH source mod ID in external stack ports.
 */
STATIC int
chassis_nh_modid_filter(int num_units,
                        sp_list_t *external_sp)
{
    int           unit;
    int           i;
    bcm_port_t    stk_port;
    int           nhmod, nhport;
    int           num_modids;

    /* Enable NH mod ID for external stack ports */
    BCM_IF_ERROR_RETURN(bcm_board_num_modid_get(&num_modids));
    for (unit = 0; unit < num_units; unit++) {
        for (i = 0; i < external_sp[unit].port_cnt; i++) {
            stk_port = external_sp[unit].port[i];
            nhmod = -1;
            if (bcm_st_reserved_modid_enable_get() ||
                nh_tx_src_mod_port_get(unit, stk_port, &nhmod, &nhport) < 0 ||
                nhmod < 0) {
                if (nh_tx_src_get(&nhmod, &nhport) < 0 || nhmod < 0) {
                    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                                (BSL_META_U(unit,
                                "TOPO: Could not get default NH src modid.\n")));
                }
            }
            if (nhmod >= 0 && nhmod < num_modids) {
                BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                              nhmod, TRUE));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_xgs2_flood_set
 *
 * Purpose:
 *     Set the appropriate flood block flag in stack ports to
 *     prevent looping.
 */
STATIC int
chassis_xgs2_flood_set(int unit, sp_list_t *external_sp,
                       to_slot_t *to_cfm, cpudb_ref_t db_ref)
{
    int                  rv;
    bcm_pbmp_t           external_all;
    bcm_pbmp_t           external_disable;
    bcm_port_t           port, eport;
    bcm_port_config_t    *config = NULL;
    uint32               flags;
    int                  cfm;
    int                  i;

    config = sal_alloc(sizeof(bcm_port_config_t), "bcm_port_config_t");
    if (config == NULL) {
        return BCM_E_MEMORY;
    }
    bcm_port_config_t_init(config);

    rv = bcm_port_config_get(unit, config);
    if (BCM_FAILURE(rv)) {
        sal_free(config);
        return (rv);
    }

    BCM_PBMP_CLEAR(external_all);
    BCM_PBMP_CLEAR(external_disable);
    for (i = 0; i < external_sp->port_cnt; i++) {
        BCM_PBMP_PORT_ADD(external_all, external_sp->port[i]);
    }

    for (cfm = 0; cfm < to_cfm->slot_cnt; cfm++) {
        if (!is_slot_valid(to_cfm->slot[cfm].dst, db_ref)) {
            for (i = 0; i < to_cfm->slot[cfm].sp.port_cnt; i++) {
                port = to_cfm->slot[cfm].sp.port[i];
                if (BCM_PBMP_MEMBER(external_all, port)) {
                    BCM_PBMP_PORT_ADD(external_disable, port);
                }
            }
        }
    }

    BCM_PBMP_ITER(config->hg, port) {
        BCM_PBMP_ITER(config->hg, eport) {
            if ((BCM_PBMP_MEMBER(external_disable, port)) ||
                (BCM_PBMP_MEMBER(external_disable, eport))) {
                flags = DISABLE_FLOOD;
            } else {
                flags = ENABLE_FLOOD;
            }
            bcm_port_flood_block_set(unit, port, eport, flags);
        }
    }
    sal_free(config);

    return BCM_E_NONE;
}

/*
 * Function:    chassis_xgs3_flood_set
 *
 * Purpose:
 *     Set the appropriate flood block flag in stack ports to
 *     prevent looping.
 */
STATIC int
chassis_xgs3_flood_set(int unit, sp_list_t *external_sp,
                       to_slot_t *to_cfm, cpudb_ref_t db_ref, int num_cfms)
{
    int                  rv;
    bcm_pbmp_t           external_all;
    bcm_pbmp_t           flood_ports;
    bcm_port_t           port, eport;
    bcm_port_config_t    *config = NULL;
    bcm_module_t         src_modid;
    uint32               flags;
    int                  cfm;
    int                  i;

    config = sal_alloc(sizeof(bcm_port_config_t), "bcm_port_config_t");
    if (config == NULL) {
        return BCM_E_MEMORY;
    }
    bcm_port_config_t_init(config);

    /* 0 or 1 CFM */
    if (num_cfms < 2) {    /* Just like the xgs2 fabric */
        sal_free(config);
        return chassis_xgs2_flood_set(unit, external_sp, to_cfm, db_ref);
    }

    /* 2 CFMs */
    rv = bcm_port_config_get(unit, config);
    if (BCM_FAILURE(rv)) {
        sal_free(config);
        return (rv);
    }

    BCM_PBMP_CLEAR(external_all);
    BCM_PBMP_CLEAR(flood_ports);
    for (i = 0; i < external_sp->port_cnt; i++) {
        BCM_PBMP_PORT_ADD(external_all, external_sp->port[i]);
    }

    /* Determine the egress ports for flooding */
    src_modid = chassis_unit_modid_get(unit, db_ref->local_entry);
    if (IS_EVEN(src_modid)) {
        cfm = EVEN_IDX;
    } else {
        cfm = ODD_IDX;
    }
    for (i = 0; i < to_cfm->slot[cfm].sp.port_cnt; i++) {
        BCM_PBMP_PORT_ADD(flood_ports, to_cfm->slot[cfm].sp.port[i]);
    }

    /*
     * Ether Ports
     *     If external stack ports are flood ports, enable egress to
     *     external stack ports, disable otherwise
     */
    BCM_PBMP_ITER(config->e, port) {
        BCM_PBMP_ITER(external_all, eport) {
            if (BCM_PBMP_MEMBER(flood_ports, eport)) {
                flags = ENABLE_FLOOD;
            } else {
                flags = DISABLE_FLOOD;
            }
            bcm_port_flood_block_set(unit, port, eport, flags);
        }
    }

    /* 
     * HiGig Ports
     *     Block internal to external stack port IF external is
     *     member of the flood ports
     */
    BCM_PBMP_ITER(config->hg, port) {
        BCM_PBMP_ITER(config->hg, eport) {
            if ((!BCM_PBMP_MEMBER(external_all, port)) &&
                (BCM_PBMP_MEMBER(external_all, eport)) &&
                (BCM_PBMP_MEMBER(flood_ports, eport))) {
                flags = DISABLE_FLOOD;
            } else {
                flags = ENABLE_FLOOD;
            }
            bcm_port_flood_block_set(unit, port, eport, flags);
        }
    }
    sal_free(config);

    return BCM_E_NONE;
}



/*
 * Function:    chassis_xgs2_mirror_to_set
 *
 * Purpose:
 *     Set the appropriate mirrored-to-port for given unit to send
 *     mirrored packets through correct CFM.  This routine should be
 *     called by LMs XGS-2 Fabric devices with external stack ports.
 *
 * Note:
 *     This routine MUST be called AFTER the external lm routes
 *     are set through chassis_xgs2_eroute_lm_lm() or
 *     chassis_xgs3_eroute_lm_lm().
 */
STATIC int
chassis_xgs2_mirror_to_set(int unit, sp_list_t *external_sp,
                           int l_slot, cpudb_ref_t db_ref)
{
    cpudb_entry_t     *cur;
    bcm_module_t      mirror_modid;
    bcm_port_config_t config;
    bcm_port_t        hport;
    bcm_pbmp_t        pbmp;
    bcm_pbmp_t        pbmp_ext;
    int               i;

    /* Since LB is based on src-mod ID, any valid non-local LM
       dest-mod ID will work to get the righ MTP */
    mirror_modid = -1;
    CPUDB_FOREACH_ENTRY(db_ref, cur) {
        if ((cur->base.slot_id != l_slot) && IS_LM_SLOT(cur->base.slot_id)){
            mirror_modid = MODID_OFFSET_GET(cur->base.slot_id);
            break;
        }
    }
    if (mirror_modid < 0) {
        return BCM_E_NONE;
    }

    /*
     *  XGS-2 Fabric device needs to use the UC bitmap
     *  to set each HiGig port individually
     */

    BCM_PBMP_CLEAR(pbmp_ext);
    for (i = 0; i < external_sp->port_cnt; i++) {
        BCM_PBMP_PORT_ADD(pbmp_ext, external_sp->port[i]);
    }

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.hg, hport) {
        BCM_PBMP_CLEAR(pbmp);
        BCM_IF_ERROR_RETURN(bcm_mirror_to_pbmp_get(unit, hport, &pbmp));

        /* If MTP is set and is an external stack port, update */
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            BCM_PBMP_AND(pbmp, pbmp_ext);
            if (BCM_PBMP_NOT_NULL(pbmp)) {
                BCM_IF_ERROR_RETURN(bcm_stk_ucbitmap_get(unit, hport,
                                                         mirror_modid, &pbmp));
                BCM_IF_ERROR_RETURN(bcm_mirror_to_pbmp_set(unit, hport, pbmp));
            }

        }
    }
    
    return BCM_E_NONE;
}


/*
 * Function:    chassis_xgs3_mirror_to_set
 *
 * Purpose:
 *     Set the appropriate mirrored-to-port for given unit to send
 *     mirrored packets through correct CFM.  This routine should be
 *     called by LMs XGS-3 Switch devices with external stack ports.
 *
 * Note:
 *     This routine MUST be called AFTER the external lm routes
 *     are set through chassis_xgs2_eroute_lm_lm() or
 *     chassis_xgs3_eroute_lm_lm().
 */
STATIC int
chassis_xgs3_mirror_to_set(int unit, sp_list_t *external_sp,
                           int l_slot, cpudb_ref_t db_ref)
{
    cpudb_entry_t     *cur;
    bcm_module_t      mirror_modid;
    bcm_port_config_t config;
    bcm_port_t        mtp;
    bcm_pbmp_t        pbmp_ext;
    int               i;

    /* Since LB is based on src-mod ID, any valid non-local LM
       dest-mod ID will work to get the righ MTP */
    mirror_modid = -1;
    CPUDB_FOREACH_ENTRY(db_ref, cur) {
        if ((cur->base.slot_id != l_slot) && IS_LM_SLOT(cur->base.slot_id)){
            mirror_modid = MODID_OFFSET_GET(cur->base.slot_id);
            break;
        }
    }
    if (mirror_modid < 0) {
        return BCM_E_NONE;
    }


    BCM_IF_ERROR_RETURN(bcm_mirror_to_get(unit, &mtp));

    BCM_PBMP_CLEAR(pbmp_ext);
    for (i = 0; i < external_sp->port_cnt; i++) {
        BCM_PBMP_PORT_ADD(pbmp_ext, external_sp->port[i]);
    }
        
    /* If MTP is set and is an external stack port, update it */
    if ((mtp >= 0) && BCM_PBMP_MEMBER(pbmp_ext, mtp)) {
        BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
        if (BCM_PBMP_MEMBER(config.hg, mtp)) {
            BCM_IF_ERROR_RETURN(bcm_topo_port_get(unit, mirror_modid, &mtp));
            BCM_IF_ERROR_RETURN(bcm_mirror_to_set(unit, mtp));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:    chassis_smlb_cfm_xgs2
 *
 * Purpose:
 *     Program the XGS2 CFM board using Chassis Load Balancing.
 */
int
chassis_smlb_cfm_xgs2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    cpudb_entry_t         *l_entry;
    int                   l_slot;                        
    int                   unit;                          
    bcm_port_t            stk_port;                      
    int                   dst_slot;                      
    int                   src_modid;                      
    int                   peer_cfm_slot;                 
    bcm_module_t          modid;                         
    int                   lm_to_cfm;                     
    int                   lm; 
    int                   i, j;
    int                   enable;
    int                   lb_even_odd;
    bcm_board_cfm_info_t  cfm_info;
    bcm_port_config_t     config;

    static sp_list_t    external_sp = {8, {1, 2, 3, 4, 5, 6, 7, 8}};
    static to_slot_t    to_lm = {
        xgs2_modport_set,
        4, {{2, {2, {1, 2}}},
            {3, {2, {3, 4}}},
            {4, {2, {5, 6}}},
            {5, {2, {7, 8}}}}
    };

    COMPILER_REFERENCE(tp_cpu);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5675 BCM956000 CFM (Load Balancing)\n")));

    /* Only 1 unit */
    unit = 0;
    l_entry = db_ref->local_entry;
    l_slot = l_entry->base.slot_id;
    bcm_board_cfm_info(db_ref, &cfm_info);

    /* Set mod ID for fabric unit */
    modid = l_entry->dest_mod;
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, modid));
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.hg, stk_port) {
        BCM_IF_ERROR_RETURN(bcm_stk_ucbitmap_set(unit, stk_port, modid,
                                                 config.cpu));
    }

    /* Set and enable CPU source mod ID */
    BCM_IF_ERROR_RETURN(c2c_src_mod_get(&src_modid));
    for (i = 0; i < external_sp.port_cnt; i++) {
        stk_port = external_sp.port[i];
        BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                      src_modid, TRUE));
    }

    /*
     * Set source mod ID filtering for LMs
     *     If one CFM
     *         accept all mod IDs,
     *     Else
     *         CFM on even slot should only accept even mod IDs
     *         CFM on odd slot should only accept odd mod IDs
     */

    /* For each external CFM stack port */
    for (i = 0; i < external_sp.port_cnt; i++) {
        stk_port = external_sp.port[i];

        /* For each LM */
        for (lm = 0; lm < to_lm.slot_cnt; lm++) {
            enable = FALSE;
            lb_even_odd = FALSE;
            dst_slot = to_lm.slot[lm].dst;

            /*
             * If LM is valid and external stack port is
             * connected directly to LM:
             *   - If only 1 CFM, allow all mod IDs from LM
             *   - If 2 CFMs, allow mod IDs based on even/odd logic
             */
            if (is_slot_valid(dst_slot, db_ref)) {
                for (j = 0; j < to_lm.slot[lm].sp.port_cnt; j++) {
                    if (stk_port == to_lm.slot[lm].sp.port[j]) {
                        if (cfm_info.count == 1) {
                            enable = TRUE;
                        } else {
                            lb_even_odd = TRUE;
                        }
                        break;
                    }
                }
            }

            /* For each mod ID that corresponds to a given LM */
            FOREACH_MODID(dst_slot, modid) {
                if (lb_even_odd) {
                    if (((IS_EVEN(l_slot)) && (IS_EVEN(modid))) ||
                        ((IS_ODD(l_slot)) && (IS_ODD(modid)))) {
                        enable = TRUE;
                    }
                    else {
                        enable = FALSE;
                    }
                }
                BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                              modid, enable));
            }
        }
    }

    /* Set source module filtering for NH */
    chassis_nh_modid_filter(1, &external_sp);

    /* Set routes to LMs */
    lm_to_cfm = -1;
    for (lm = 0; lm < to_lm.slot_cnt; lm++) {
        dst_slot = to_lm.slot[lm].dst;
        if (!is_slot_valid(dst_slot, db_ref)) {
            continue;
        }

        /* Use first port, other ports will be trunked */
        stk_port = to_lm.slot[lm].sp.port[0];
        lm_to_cfm = lm;
        FOREACH_MODID(dst_slot, modid) {
            BCM_IF_ERROR_RETURN(xgs2_modport_set(unit, modid, stk_port));
        }
    }


    /* If there is an available external stack port, set route to peer CFM */
    if (lm_to_cfm >= 0) {
        if (l_entry->base.slot_id == 0) {
            peer_cfm_slot = 1;
        } else {
            peer_cfm_slot = 0;
        }
        if (is_slot_valid(peer_cfm_slot, db_ref)) {
            /* Enable peer CFM source mod ID in all external stack ports */
            for (i = 0; i < external_sp.port_cnt; i++) {
                stk_port = external_sp.port[i];
                FOREACH_MODID(peer_cfm_slot, modid) {
                    BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit,
                                                                  stk_port,
                                                                  modid,
                                                                  TRUE));
                }
            }
            stk_port = to_lm.slot[lm_to_cfm].sp.port[0];
            FOREACH_MODID(peer_cfm_slot, modid) {
                BCM_IF_ERROR_RETURN(xgs2_modport_set(unit, modid, stk_port));
            }
        }
    }

    /* Set trunks */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    /* Set mod IDs */
    /* Mod ID for fabric is set by caller */
    bcm_board_unknown_src_modid_setup(l_entry);

    return BCM_E_NONE;
}


/*
 * Function:    chassis_smlb_xgs2_48g
 *
 * Purpose:
 *     Program the XGS2-48G LM board using Chassis Load Balancing.
 */
#define NUM_UNIT_XGS2_48G    5
int
chassis_smlb_xgs2_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    cpudb_entry_t    *l_entry;
    int              unit;
    int              num_units;
    bcm_module_t     modid;
    sp_list_t        sp_list;

    static sp_list_t    internal_sp = {4, {1, 2, 3, 4}};
    static sp_list_t    external_sp = {4, {5, 6, 7, 8}};
    static to_unit_t    to_unit = {
        xgs2_modport_set, xgs2_internal_sp_set,    /* Unit 0:5675 */
        4, {{1, {1, {1}}},
            {2, {1, {2}}},
            {3, {1, {3}}},
            {4, {1, {4}}}}
    };
    static to_slot_t    to_cfm = {
        xgs2_modport_set,                          /* Unit 0:5675 */
        2, {{0, {2, {5, 7}}},
            {1, {2, {6, 8}}}}
    };

    COMPILER_REFERENCE(tp_cpu);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 48GE BCM956000 LM (Load Balancing)\n")));

    /* 
     * Note:
     *     Although this LM has 5 units, units 1-4 are 5695s.
     *     The 5695 does not need its stack ports to be programmed.
     */
    num_units = 1;
    l_entry = db_ref->local_entry;

    /* Set packet forwarding tables */
    BCM_IF_ERROR_RETURN(chassis_iroute(num_units, &to_unit, db_ref));
    BCM_IF_ERROR_RETURN(chassis_eroute_lm_cfm(num_units,
                                              &to_cfm, &sp_list, db_ref));
    BCM_IF_ERROR_RETURN(chassis_xgs2_eroute_lm_lm(0, &to_unit, &sp_list, db_ref));


    /* Set source module filtering */
    BCM_IF_ERROR_RETURN(chassis_internal_modid_filter(num_units, &internal_sp));
    BCM_IF_ERROR_RETURN(chassis_external_modid_filter(0, &external_sp,
                                                      l_entry->base.slot_id,
                                                      db_ref));
    BCM_IF_ERROR_RETURN(chassis_nh_modid_filter(num_units, &external_sp));

    /* Set flood blocking */
    BCM_IF_ERROR_RETURN(chassis_xgs2_flood_set(0, &external_sp,
                                               &to_cfm, db_ref));

    /* Set trunks */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    /* Set mirroring */
    BCM_IF_ERROR_RETURN(chassis_xgs2_mirror_to_set(0, &external_sp,
                                                   l_entry->base.slot_id,
                                                   db_ref));

    /* Set mod IDs */
    bcm_board_unknown_src_modid_setup(l_entry);
    for (unit = 0; unit < NUM_UNIT_XGS2_48G; unit++) {
        if ((modid = chassis_unit_modid_get(unit, l_entry)) < 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, modid));
        BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(unit));
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_smlb_xgs2_6x
 *
 * Purpose:
 *     Program the XGS2-6X LM board using Chassis Load Balancing.
 */
#define NUM_UNIT_XGS2_6X 7
int
chassis_smlb_xgs2_6x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    cpudb_entry_t    *l_entry;
    int              unit;
    int              num_units;
    bcm_module_t     modid;
    sp_list_t        sp_list;

    static sp_list_t    internal_sp = {6, {6, 7, 8, 1, 2, 3}};
    static sp_list_t    external_sp = {2, {4, 5}};
    static to_unit_t    to_unit = {
        xgs2_modport_set, xgs2_internal_sp_set,    /* Unit 0:5675 */
        6, {{1, {1, {6}}},
            {2, {1, {7}}},
            {3, {1, {8}}},
            {4, {1, {1}}},
            {5, {1, {2}}},
            {6, {1, {3}}}}
    };
    static to_slot_t    to_cfm = {
        xgs2_modport_set,                          /* Unit 0:5675 */
        2, {{0, {1, {4}}},
            {1, {1, {5}}}}
    };

    COMPILER_REFERENCE(tp_cpu);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5674 6X BCM956000 LM (Load Balancing)\n")));

    /* 
     * Note:
     *     Although this LM has 7 units, units 1-6 are 5674s.
     *     The 5674 does not need its stack ports to be programmed.
     */
    num_units = 1;
    l_entry = db_ref->local_entry;

    /* Set packet forwarding tables */
    BCM_IF_ERROR_RETURN(chassis_iroute(num_units, &to_unit, db_ref));
    BCM_IF_ERROR_RETURN(chassis_eroute_lm_cfm(num_units,
                                              &to_cfm, &sp_list, db_ref));
    BCM_IF_ERROR_RETURN(chassis_xgs2_eroute_lm_lm(0, &to_unit, &sp_list, db_ref));


    /* Set source module filtering */
    BCM_IF_ERROR_RETURN(chassis_internal_modid_filter(num_units, &internal_sp));
    BCM_IF_ERROR_RETURN(chassis_external_modid_filter(0, &external_sp,
                                                      l_entry->base.slot_id,
                                                      db_ref));
    BCM_IF_ERROR_RETURN(chassis_nh_modid_filter(num_units, &external_sp));

    /* Set flood blocking */
    BCM_IF_ERROR_RETURN(chassis_xgs2_flood_set(0, &external_sp,
                                               &to_cfm, db_ref));
    /* Set mirroring */
    BCM_IF_ERROR_RETURN(chassis_xgs2_mirror_to_set(0, &external_sp,
                                                   l_entry->base.slot_id,
                                                   db_ref));

    /* Set mod IDs */
    bcm_board_unknown_src_modid_setup(l_entry);
    for (unit = 0; unit < NUM_UNIT_XGS2_6X; unit++) {
        if ((modid = chassis_unit_modid_get(unit, l_entry)) < 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, modid));
        BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(unit));
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_smlb_xgs3_12x
 *
 * Purpose:
 *     Program the XGS3-12X LM board using Chassis Load Balancing.
 */
#define NUM_UNIT_XGS3_12X    5
int
chassis_smlb_xgs3_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    cpudb_entry_t    *l_entry;
    int              unit;
    bcm_module_t     modid;
    sp_list_t        sp_list[NUM_UNIT_XGS3_12X];

    static sp_list_t    internal_sp_tbl[NUM_UNIT_XGS3_12X] = {
        {4, {1, 2, 7, 8}},                            /* Unit 0 */
        {1, {27}}, {1, {27}}, {1, {27}}, {1, {27}}    /* Units 1-4 */
    };
    static sp_list_t    external_sp_tbl[NUM_UNIT_XGS3_12X] = {
        {4, {3, 4, 5, 6}},      /* Unit 0 */
        {0}, {0}, {0}, {0}      /* Units 1-4 */
    };
    static to_unit_t    to_unit_tbl[NUM_UNIT_XGS3_12X] = {
        { xgs2_modport_set, xgs3_internal_sp_set,    /* Unit 0: 5675 */
          4, {{1, {1, {7}}},
              {2, {1, {8}}}, 
              {3, {1, {1}}}, 
              {4, {1, {2}}}} },
        { xgs3_modport_set, xgs3_internal_sp_set,    /* Unit 1: 56501 */
          4, {{0, {1, {27}}},
              {2, {1, {27}}},
              {3, {1, {27}}},
              {4, {1, {27}}}} },
        { xgs3_modport_set, xgs3_internal_sp_set,    /* Unit 2: 56501 */
          4, {{0, {1, {27}}},
              {1, {1, {27}}},
              {3, {1, {27}}},
              {4, {1, {27}}}} },
        { xgs3_modport_set, xgs3_internal_sp_set,    /* Unit 3: 56501 */
          4, {{0, {1, {27}}},
              {1, {1, {27}}},
              {2, {1, {27}}},
              {4, {1, {27}}}} },
        { xgs3_modport_set, xgs3_internal_sp_set,    /* Unit 4: 56501 */
          4, {{0, {1, {27}}},
              {1, {1, {27}}},
              {2, {1, {27}}},
              {3, {1, {27}}}} },
    };
    static to_slot_t    to_cfm8[NUM_UNIT_XGS3_12X] = {
        { xgs2_modport_set,
          2, {{0, {2, {3, 5}}},   /* Unit 0: 5675 */
              {1, {2, {4, 6}}}} },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 1: 5695 */
              {1, {1, {27}}}}   },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 2: 5695 */
              {1, {1, {27}}}}   },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 3: 5695 */
              {1, {1, {27}}}}   },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 4: 5695 */
              {1, {1, {27}}}}   }
    };
    static to_slot_t    to_cfm16[NUM_UNIT_XGS3_12X] = {
        { xgs2_modport_set,
          2, {{0, {2, {3, 6}}},   /* Unit 0: 5675 */
              {1, {2, {4, 5}}}} },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 1: 5695 */
              {1, {1, {27}}}}   },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 2: 5695 */
              {1, {1, {27}}}}   },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 3: 5695 */
              {1, {1, {27}}}}   },
        { xgs3_modport_set,
          2, {{0, {1, {27}}},     /* Unit 4: 5695 */
              {1, {1, {27}}}}   }
    };

    int               master_slot;
    to_slot_t         *to_cfm;

    COMPILER_REFERENCE(tp_cpu);

    to_cfm = (db_ref->master_entry != NULL &&
              db_ref->master_entry->base.flags & CPUDB_BASE_F_CHASSIS_10SLOT) ?
        to_cfm16 : to_cfm8;

    master_slot = db_ref->master_entry ?
        db_ref->master_entry->base.slot_id : -1;
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56501 12XE BCM956000 LM (Load Balancing)\n")));
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("      master on slot %d\n"),
                 master_slot));

    l_entry = db_ref->local_entry;

    /* Set forwarding to local units */
    BCM_IF_ERROR_RETURN(chassis_iroute(NUM_UNIT_XGS3_12X,
                                       to_unit_tbl, db_ref));
    /* Set forwarding to CFMs */
    BCM_IF_ERROR_RETURN(chassis_eroute_lm_cfm(NUM_UNIT_XGS3_12X, 
                                              to_cfm, sp_list, db_ref));
    /* Set forwarding to LMs */
    unit = 0;
    BCM_IF_ERROR_RETURN(chassis_xgs2_eroute_lm_lm(unit, &to_unit_tbl[unit],
                                                  &sp_list[unit], db_ref));
    for (unit = 1; unit < NUM_UNIT_XGS3_12X; unit++){
        BCM_IF_ERROR_RETURN(chassis_xgs3_eroute_lm_lm(unit,
                                                      internal_sp_tbl[unit].port[0],
                                                      l_entry->base.slot_id));
    }


    /* Set source module filtering */
    unit = 0;
    BCM_IF_ERROR_RETURN(chassis_internal_modid_filter(NUM_UNIT_XGS3_12X,
                                                      internal_sp_tbl));
    BCM_IF_ERROR_RETURN(chassis_external_modid_filter(unit, &external_sp_tbl[unit],
                                                      l_entry->base.slot_id,
                                                      db_ref));
    BCM_IF_ERROR_RETURN(chassis_nh_modid_filter(NUM_UNIT_XGS3_12X,
                                                external_sp_tbl));

    /* Set flood blocking */
    BCM_IF_ERROR_RETURN(chassis_xgs2_flood_set(unit, &external_sp_tbl[unit],
                                               &to_cfm[unit], db_ref));


    /* Set trunks */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));
    
    /* Set mirroring */
    BCM_IF_ERROR_RETURN(chassis_xgs2_mirror_to_set(0, &external_sp_tbl[unit],
                                                   l_entry->base.slot_id,
                                                   db_ref));

    /* Set mod IDs */
    bcm_board_unknown_src_modid_setup(l_entry);
    for (unit = 0; unit < NUM_UNIT_XGS3_12X; unit++) {
        if ((modid = chassis_unit_modid_get(unit, l_entry)) < 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, modid));
    }

    return BCM_E_NONE;
}


/*
 * Function:    chassis_smlb_xgs3_48g
 *
 * Purpose:
 *     Program the XGS3-48G LM board using Chassis Load Balancing.
 */
#define NUM_UNIT_XGS3_48G  2

#define FB_UNIT0 0
#define FB_UNIT1 1

int
chassis_smlb_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    cpudb_entry_t        *l_entry;
    int                  unit;
    bcm_module_t         src_modid;
    bcm_module_t         dst_modid;
    bcm_port_t           stk_port;
    sp_list_t            sp_list[NUM_UNIT_XGS3_48G];
    bcm_board_cfm_info_t cfm_info;
    int                  master_slot;

    static sp_list_t    internal_sp_tbl[NUM_UNIT_XGS3_48G] = {
        {2, {26, 27}},    /* Unit 0 */
        {2, {26, 27}}     /* Unit 1 */
    };
    static sp_list_t    external_sp_tbl[NUM_UNIT_XGS3_48G] = {
        {2, {24, 25}},    /* Unit 0 */
        {2, {24, 25}}     /* Unit 1 */
    };
    static to_unit_t    to_unit_tbl[NUM_UNIT_XGS3_48G] = {
        { xgs3_modport_set, xgs3_internal_sp_set,    /* Unit 0: 56504 */
          1, {{1, {2, {26, 27}}}} },
        { xgs3_modport_set, xgs3_internal_sp_set,    /* Unit 1: 56504 */
          1, {{0, {2, {26, 27}}}} }
    };

    static to_slot_t    to_cfm8[NUM_UNIT_XGS3_48G] = {
        { xgs3_modport_set,
          2, {{0, {2, {26, 27}}},                    /* Unit 0: 56504 */
              {1, {2, {24, 25}}}} },
        { xgs3_modport_set,
          2, {{0, {2, {24, 25}}},                    /* Unit 1: 56504 */
              {1, {2, {26, 27}}}} }
    };

    static to_slot_t    to_cfm16[NUM_UNIT_XGS3_48G] = {
        { xgs3_modport_set,
          2, {{0, {2, {24, 25}}},                    /* Unit 0: 56504 */
              {1, {2, {26, 27}}}} },
        { xgs3_modport_set,
          2, {{0, {2, {26, 27}}},                    /* Unit 1: 56504 */
              {1, {2, {24, 25}}}} }
    };

    to_slot_t     *to_cfm;

    to_cfm = (db_ref->master_entry != NULL &&
              db_ref->master_entry->base.flags & CPUDB_BASE_F_CHASSIS_10SLOT) ?
        to_cfm16 : to_cfm8;
    
    /* If there's no master yet, set self as master */
    master_slot = db_ref->master_entry ?
        db_ref->master_entry->base.slot_id : -1;
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56504 48GE BCM956000 LM (Load Balancing)\n")));
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("      master on slot %d\n"),
                 master_slot));

    l_entry = db_ref->local_entry;
    bcm_board_cfm_info(db_ref, &cfm_info);

    /* Set forwarding to local units */
    BCM_IF_ERROR_RETURN(chassis_iroute(NUM_UNIT_XGS3_48G,
                                       to_unit_tbl, db_ref));
    /* Set forwarding to CFMs */
    sal_memset(sp_list, 0, sizeof(sp_list));
    BCM_IF_ERROR_RETURN(chassis_eroute_lm_cfm(NUM_UNIT_XGS3_48G,
                                              to_cfm, sp_list, db_ref));
    /* Set forwarding to LMs */
    /* Use unit's source mod ID even/odd */
    for (unit = 0; unit < NUM_UNIT_XGS3_48G; unit++){
        src_modid = chassis_unit_modid_get(unit, l_entry);
        if (IS_EVEN(src_modid)) {
            if ((stk_port = sp_list[unit].port[EVEN_IDX]) < 0) {
                if ((stk_port = sp_list[unit].port[ODD_IDX]) < 0) {
                    continue;
                }
            }
        } else {
            if ((stk_port = sp_list[unit].port[ODD_IDX]) < 0) {
                if ((stk_port = sp_list[unit].port[EVEN_IDX]) < 0) {
                    continue;
                }
            }
        }
        BCM_IF_ERROR_RETURN(chassis_xgs3_eroute_lm_lm(unit, stk_port,
                                                      l_entry->base.slot_id));
    }

    /* Set module filtering */
    BCM_IF_ERROR_RETURN(chassis_internal_modid_filter(NUM_UNIT_XGS3_48G,
                                                      internal_sp_tbl));
    for (unit = 0; unit < NUM_UNIT_XGS3_48G; unit++) {
        /* Ignore error, since some FB revs do not support source modID filter */
        (void)chassis_external_modid_filter(unit,
                                      &external_sp_tbl[unit],
                                      l_entry->base.slot_id,
                                      db_ref);
    }
    BCM_IF_ERROR_RETURN(chassis_nh_modid_filter(NUM_UNIT_XGS3_48G,
                                                external_sp_tbl));

    /* Set flood blocking */
    for (unit = 0; unit < NUM_UNIT_XGS3_48G; unit++) {
        BCM_IF_ERROR_RETURN(chassis_xgs3_flood_set(unit, &external_sp_tbl[unit],
                                                   &to_cfm[unit], db_ref,
                                                   cfm_info.count));
    }


    /* Set trunks */
    /* External stack ports */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));
    /* Intraboard trunk */
    BCM_IF_ERROR_RETURN(_bcm_board_setup_trunk_lm_xgs3_48g());

    /* Set mirroring */
    for (unit = 0; unit < NUM_UNIT_XGS3_48G; unit++) {
        BCM_IF_ERROR_RETURN(chassis_xgs3_mirror_to_set(unit,
                                                       &external_sp_tbl[unit],
                                                       l_entry->base.slot_id,
                                                       db_ref));
    }

    /* Set mod IDs */
    for (unit = 0; unit < NUM_UNIT_XGS3_48G; unit++) {
        if ((dst_modid = chassis_unit_modid_get(unit, l_entry)) < 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, dst_modid));
    }

#ifdef BCM_BOARD_AUTO_E2E
    /* intra-board end to end flow control, if available */
    BCM_IF_ERROR_RETURN
        (_bcm_board_dual_xgs3_e2e_set(FB_UNIT0,
                                      tp_cpu->local_entry.mod_ids[0],
                                      FB_UNIT1,
                                      tp_cpu->local_entry.mod_ids[1]));
#endif  /* BCM_BOARD_AUTO_E2E */

    return BCM_E_NONE;
}

#undef FB_UNIT0
#undef FB_UNIT1

/*
 * Function:    chassis_smlb_cfm_xgs3
 *
 * Purpose:
 *     Program the XGS3 CFM board using Chassis Load Balancing.
 */
int
chassis_smlb_cfm_xgs3(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    cpudb_entry_t         *l_entry;
    int                   l_slot;                        
    int                   unit;                          
    bcm_port_t            stk_port;                      
    int                   dst_slot;                      
    int                   src_modid;                      
    int                   peer_cfm_slot;                 
    bcm_module_t          modid;                         
    int                   lm_to_cfm;                     
    int                   lm; 
    int                   i, j;
    int                   enable;
    int                   lb_even_odd;
    bcm_board_cfm_info_t  cfm_info;
    bcm_port_config_t     config;
    bcm_port_t            port;

    static sp_list_t    external_sp = {
        16,
        {0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17}
    };
    static to_slot_t    to_lm = {
        xgs3_modport_set,
        8, {{2, {2, {7, 10}}},
            {3, {2, {6, 11}}},
            {4, {2, {5, 12}}},
            {5, {2, {4, 13}}},
            {6, {2, {3, 14}}},
            {7, {2, {2, 15}}},
            {8, {2, {1, 16}}},
            {9, {2, {0, 17}}}}};


    COMPILER_REFERENCE(tp_cpu);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 56700 BCM956010 CFM (Load Balancing)\n")));

    /* Only 1 unit */
    unit = 0;
    l_entry = db_ref->local_entry;
    l_slot = l_entry->base.slot_id;
    bcm_board_cfm_info(db_ref, &cfm_info);

    /* Set CPU mod ID for fabric unit */
    modid = l_entry->dest_mod;
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.cpu, port) {
        break;   /* Get CPU port */
    }

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, modid));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, modid, port));

    /* Set and enable CPU source mod ID */
    BCM_IF_ERROR_RETURN(c2c_src_mod_get(&src_modid));
    for (i = 0; i < external_sp.port_cnt; i++) {
        stk_port = external_sp.port[i];
        BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                      src_modid, TRUE));
    }

    /*
     * Set source mod ID filtering for LMs
     *     If one CFM
     *         accept all mod IDs,
     *     Else
     *         CFM on even slot should only accept even mod IDs
     *         CFM on odd slot should only accept odd mod IDs
     */

    /* For each external CFM stack port */
    for (i = 0; i < external_sp.port_cnt; i++) {
        stk_port = external_sp.port[i];

        /* For each LM */
        for (lm = 0; lm < to_lm.slot_cnt; lm++) {
            enable = FALSE;
            lb_even_odd = FALSE;
            dst_slot = to_lm.slot[lm].dst;

            /*
             * If LM is valid and external stack port is
             * connected directly to LM:
             *   - If only 1 CFM, allow all mod IDs from LM
             *   - If 2 CFMs, allow mod IDs based on even/odd logic
             */
            if (is_slot_valid(dst_slot, db_ref)) {
                for (j = 0; j < to_lm.slot[lm].sp.port_cnt; j++) {
                    if (stk_port == to_lm.slot[lm].sp.port[j]) {
                        if (cfm_info.count == 1) {
                            enable = TRUE;
                        } else {
                            lb_even_odd = TRUE;
                        }
                        break;
                    }
                }
            }

            /* For each mod ID that corresponds to a given LM */
            FOREACH_MODID(dst_slot, modid) {
                if (lb_even_odd) {
                    if (((IS_EVEN(l_slot)) && (IS_EVEN(modid))) ||
                        ((IS_ODD(l_slot)) && (IS_ODD(modid)))) {
                        enable = TRUE;
                    }
                    else {
                        enable = FALSE;
                    }
                }
                BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, stk_port,
                                                              modid, enable));
            }
        }
    }

    /* Set source module filtering for NH */
    chassis_nh_modid_filter(1, &external_sp);

    /* Set routes to LMs */
    lm_to_cfm = -1;
    for (lm = 0; lm < to_lm.slot_cnt; lm++) {
        dst_slot = to_lm.slot[lm].dst;
        if (!is_slot_valid(dst_slot, db_ref)) {
            continue;
        }

        /* Use first port, other ports will be trunked */
        stk_port = to_lm.slot[lm].sp.port[0];
        lm_to_cfm = lm;
        FOREACH_MODID(dst_slot, modid) {
            BCM_IF_ERROR_RETURN(xgs3_modport_set(unit, modid, stk_port));
        }
    }


    /* If there is an available external stack port, set route to peer CFM */
    if (lm_to_cfm >= 0) {
        if (l_entry->base.slot_id == 0) {
            peer_cfm_slot = 1;
        } else {
            peer_cfm_slot = 0;
        }
        if (is_slot_valid(peer_cfm_slot, db_ref)) {
            /* Enable peer CFM source mod ID in all external stack ports */
            for (i = 0; i < external_sp.port_cnt; i++) {
                stk_port = external_sp.port[i];
                FOREACH_MODID(peer_cfm_slot, modid) {
                    BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit,
                                                                  stk_port,
                                                                  modid,
                                                                  TRUE));
                }
            }
            stk_port = to_lm.slot[lm_to_cfm].sp.port[0];
            FOREACH_MODID(peer_cfm_slot, modid) {
                BCM_IF_ERROR_RETURN(xgs3_modport_set(unit, modid, stk_port));
            }
        }
    }

    /* Set trunks */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    /* Set mod IDs */
    /* Mod ID for fabric is set by caller */
    bcm_board_unknown_src_modid_setup(l_entry);

    return BCM_E_NONE;
}

int
chassis_smlb_56800_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    cpudb_entry_t    *l_entry;
    int              num_units;
    sp_list_t        sp_list;
    bcm_board_cfm_info_t cfm_info;
    bcm_port_config_t    config;
    bcm_port_t           port;
    bcm_port_t       stk_port;
    bcm_module_t         src_modid;

    static sp_list_t    external_sp = {4, {0, 1, 2, 3}};
    static to_slot_t    to_cfm = {
        xgs3_modport_set,                          /* Unit 0:56800 */
        2, {{0, {2, {1, 3}}},
            {1, {2, {0, 2}}}}
    };

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56800 LM (Load Balancing)\n")));

    BCM_IF_ERROR_RETURN(bcm_port_config_get(0, &config));
        
    bcm_board_cfm_info(db_ref, &cfm_info);
    num_units = 1;
    l_entry = db_ref->local_entry;

    /* Set CPU port */
    BCM_PBMP_ITER(config.cpu, port) {
        break;   /* Get CPU port */
    }
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(0, l_entry->mod_ids[0], port));

    
    /* Set packet forwarding tables */
    BCM_IF_ERROR_RETURN(chassis_eroute_lm_cfm(num_units,
                                              &to_cfm, &sp_list, db_ref));

    src_modid = chassis_unit_modid_get(0, l_entry);
    if (IS_EVEN(src_modid)) {
        if ((stk_port = sp_list.port[EVEN_IDX]) < 0) {
            stk_port = sp_list.port[ODD_IDX];
        }
    } else {
        if ((stk_port = sp_list.port[ODD_IDX]) < 0) {
            stk_port = sp_list.port[EVEN_IDX];
        }
    }

    if (stk_port >= 0) {
        BCM_IF_ERROR_RETURN(chassis_xgs3_eroute_lm_lm(0, stk_port,
                                                      l_entry->base.slot_id));
    }
    /* Set source module filtering */
    BCM_IF_ERROR_RETURN(chassis_external_modid_filter(0, &external_sp,
                                                      l_entry->base.slot_id,
                                                      db_ref));
    BCM_IF_ERROR_RETURN(chassis_nh_modid_filter(num_units, &external_sp));

    /* Set flood blocking */
    BCM_IF_ERROR_RETURN(chassis_xgs3_flood_set(0, &external_sp,
                                               &to_cfm, db_ref,
                                               cfm_info.count));

    /* Set trunks */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    /* Set mirroring */
    BCM_IF_ERROR_RETURN(chassis_xgs3_mirror_to_set(0, &external_sp,
                                                   l_entry->base.slot_id,
                                                   db_ref));

    /* Set mod IDs */
    bcm_board_unknown_src_modid_setup(l_entry);
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, l_entry->dest_mod));

    return BCM_E_NONE;
}

#else
typedef int _brd_chassis_smlb_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_CHASSIS */
