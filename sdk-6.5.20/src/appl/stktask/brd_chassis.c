/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_chassis.c
 * Purpose:     BCM956000 Chassis top level board programming
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_brd.h>

#include "brd_chassis_int.h"

#if defined(INCLUDE_CHASSIS)

#if defined(INCLUDE_ATPTRANS_SOCKET)

#include <appl/cputrans/atptrans_socket.h>
#include <appl/stktask/stktask.h>
#include "topo_int.h"


/************** ATP over Sockets Support ***********/

/* Config variable for ATP over Sockets installation */
#define ATPTRANS_SOCKET  "atptrans_socket"

/***********************************************************************
 *
 * Chassis Default IP Addresses and Interfaces
 *
 *     In a chassis based system, there will be one interface or subnet
 *     defined for each CFM.  Each fabric or line module will
 *     have one unique IP address per interface.
 */

/* Default chassis IP addresses */
#define CHASSIS_IP_DEF_B0      192    /* IP addr 1st byte */
#define CHASSIS_IP_DEF_B1      168    /* IP addr 2nd byte */
#define CHASSIS_IP_DEF_B2_IF0    0    /* IP addr 3rd byte, interface to CFM 0 */
#define CHASSIS_IP_DEF_B2_IF1    1    /* IP addr 3rd byte, interface to CFM 1 */

#define CHASSIS_IP_DEF_B3_S0     1    /* IP addr 4th byte, slot 0 */
#define CHASSIS_IP_DEF_B3_S1     1    /* IP addr 4th byte, slot 1 */
#define CHASSIS_IP_DEF_B3_S2    10
#define CHASSIS_IP_DEF_B3_S3    11
#define CHASSIS_IP_DEF_B3_S4    12
#define CHASSIS_IP_DEF_B3_S5    13

#define CHASSIS_IPADDR_DEF_MASK_IF0         \
        ( (CHASSIS_IP_DEF_B0     << 24) |   \
          (CHASSIS_IP_DEF_B1     << 16) |   \
          (CHASSIS_IP_DEF_B2_IF0 << 8)   )

#define CHASSIS_IPADDR_DEF_MASK_IF1         \
        ( (CHASSIS_IP_DEF_B0     << 24) |   \
          (CHASSIS_IP_DEF_B1     << 16) |   \
          (CHASSIS_IP_DEF_B2_IF1 << 8)   )

STATIC int
bcm_board_chassis_get_ip(cpudb_ref_t new_db,
                         cpudb_base_t  *cpudb_base_ptr, bcm_ip_t *ip_addr)
{
    /* CFM0 Interface */
    static bcm_ip_t    chassis_ip_default_if0[] = {
        CHASSIS_IPADDR_DEF_MASK_IF0 | CHASSIS_IP_DEF_B3_S0,
        CHASSIS_IPADDR_DEF_MASK_IF0 | CHASSIS_IP_DEF_B3_S1,
        CHASSIS_IPADDR_DEF_MASK_IF0 | CHASSIS_IP_DEF_B3_S2,
        CHASSIS_IPADDR_DEF_MASK_IF0 | CHASSIS_IP_DEF_B3_S3,
        CHASSIS_IPADDR_DEF_MASK_IF0 | CHASSIS_IP_DEF_B3_S4,
        CHASSIS_IPADDR_DEF_MASK_IF0 | CHASSIS_IP_DEF_B3_S5
    };
    
    /* CFM1 Interface */
    static bcm_ip_t    chassis_ip_default_if1[] = {
        CHASSIS_IPADDR_DEF_MASK_IF1 | CHASSIS_IP_DEF_B3_S0,
        CHASSIS_IPADDR_DEF_MASK_IF1 | CHASSIS_IP_DEF_B3_S1,
        CHASSIS_IPADDR_DEF_MASK_IF1 | CHASSIS_IP_DEF_B3_S2,
        CHASSIS_IPADDR_DEF_MASK_IF1 | CHASSIS_IP_DEF_B3_S3,
        CHASSIS_IPADDR_DEF_MASK_IF1 | CHASSIS_IP_DEF_B3_S4,
        CHASSIS_IPADDR_DEF_MASK_IF1 | CHASSIS_IP_DEF_B3_S5
    };

    int             slot;
    int             max_slots;
    cpudb_entry_t  *local_entry;
    cpudb_entry_t  *master_entry;
    bcm_ip_t       *ip_addr_tbl;

    if ((local_entry = new_db->local_entry) == NULL) {
        return BCM_E_PARAM;
    }

    /* Interface */

    /* Determine interface to use based on local CPU information as follows:
     * - For a local CPU that is part of a CFM, the interface is
     *   determined by the local CPU type.
     * - For a local CPU that is part of a LM, the interface is
     *   determined by the master CPU.
     *
     * NOTE:  The type of module (CFM or LM) is currently derived from
     *        the slot id.
     */

    if (local_entry->base.slot_id == 0) {         /* CFM0-CFM1, CFM0-LMx */
        ip_addr_tbl = &chassis_ip_default_if0[0];
    } else if (local_entry->base.slot_id == 1) {  /* CFM1-CFM0, CFM1-LMx */
        ip_addr_tbl = &chassis_ip_default_if1[0];
    } else {
        if ((master_entry = new_db->master_entry) == NULL) {
            return BCM_E_PARAM;
        }

        /* For LMs, interface depends on destination */
        if (cpudb_base_ptr->slot_id == 0) {           /* LMx-CFM0 */
            ip_addr_tbl = &chassis_ip_default_if0[0];
        } else if (cpudb_base_ptr->slot_id == 1) {    /* LMx-CFM1 */
            ip_addr_tbl = &chassis_ip_default_if1[0];
        } else {                                      /* LMx-LMx, use Master */
            if (master_entry->base.slot_id == 0) {
                ip_addr_tbl = &chassis_ip_default_if0[0];
            } else {
                ip_addr_tbl = &chassis_ip_default_if1[0];
            }
        }
    }

    /* Get destination slot */
    max_slots = COUNTOF(chassis_ip_default_if0);
    slot = cpudb_base_ptr->slot_id;
    if ((slot < 0) || (slot >= max_slots )) {
        LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                  (BSL_META("ATP-Socket ERROR: Invalid destination slot %d\n"),
                   slot));
        return BCM_E_FAIL;
    }

    *ip_addr = ip_addr_tbl[slot];

    return BCM_E_NONE;
}


STATIC void
bcm_board_chassis_atptrans_socket_update(cpudb_ref_t new_db, cpudb_ref_t old_db)
{
    int              rv;
    cpudb_entry_t    *old_cpu_ptr;
    cpudb_entry_t    *new_cpu_ptr;
    bcm_ip_t         ip_addr;


    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TKS ATP-Socket Update: new_db %p. old_db %p\n"),
                 new_db, old_db));

    if ((new_db == NULL) && (old_db == NULL)) {
        return;
    }

    /*
     * Uninstall socket interface from ATP transport
     * for each CPU no longer in DB
     */
    if (old_db != NULL) {
        new_cpu_ptr = NULL;

        CPUDB_FOREACH_ENTRY(old_db, old_cpu_ptr) {
            /* If local CPU, do nothing */
            if (old_cpu_ptr->flags & CPUDB_F_IS_LOCAL) {
                continue;
            }

            /* Check if CPU is no longer in DB */
            if (new_db != NULL) {
                CPUDB_KEY_SEARCH(new_db, old_cpu_ptr->base.key, new_cpu_ptr);
            }
            if (new_cpu_ptr != NULL) {  /* CPU is still present */
                continue;
            }

            atptrans_socket_uninstall(old_cpu_ptr->base.key);
        }
    }

    /* Install socket interface in ATP transport for CPU in new DB */
    if (new_db != NULL) {

        /* Local CPU must be set first */
        CPUDB_FOREACH_ENTRY(new_db, new_cpu_ptr) {
            if (new_cpu_ptr->flags & CPUDB_F_IS_LOCAL) {
                atptrans_socket_local_cpu_key_set(new_cpu_ptr->base.key);
                break;
            }
        }

        /* Install socket interface on non-local CPUs */
        CPUDB_FOREACH_ENTRY(new_db, new_cpu_ptr) {
            if (new_cpu_ptr->flags & CPUDB_F_IS_LOCAL) {
                continue;
            }
            rv = bcm_board_chassis_get_ip(new_db, &new_cpu_ptr->base, &ip_addr);
            if (BCM_SUCCESS(rv)) {
                atptrans_socket_install(new_cpu_ptr->base.key, ip_addr, 0x0);
            } else {
                LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                          (BSL_META("Invalid IP address.  Cannot install ATP over "
                                    "Sockets for CPU key" CPUDB_KEY_FMT_EOLN),
                           CPUDB_KEY_DISP(new_cpu_ptr->base.key)));
            }
        }
    }

    return;
}

int
bcm_board_chassis_atptrans_socket_transition(bcm_st_state_t from,
                                             bcm_st_event_t event,
                                             bcm_st_state_t to,
                                             cpudb_ref_t disc_db,
                                             cpudb_ref_t cur_db)
{
    int rv;

    rv = bcm_st_transition(from, event, to, disc_db, cur_db);

    if (!atptrans_socket_server_running()) {
        atptrans_socket_server_start();
    }

    if (to == BCM_STS_ATTACH) {
        bcm_board_chassis_atptrans_socket_update(disc_db, cur_db);
    }

    return rv;
}

#endif /* defined(INCLUDE_ATPTRANS_SOCKET) */


STATIC bcm_board_program_f prog[cpudb_board_id_count];

/*
 * Chassis Module ID Assignment
 * 
 *     The following table contains the base (start) mod ID assigned for
 *     each slot in a chassis based system.  The entries are indexed by 
 *     the slot numbers 0 - 9.
 *
 *     Following assumptions are made
 *     - CFMs slot start at 0
 *     - LMs  slot start at slot 2
 *     - There is 1 mod ID per CFM
 *     - There are 6 mod IDs per LM
 */
int topo_chassis_slot_to_mod_base[NUM_SLOT] = {
    2, 3,              /* CFMs get 1 mod-id    */
    4, 10, 16, 22,     /* All LMs get 6 mod-ids */
    28, 34, 40, 46     /* LMs 6-9: 10-slot chassis only */
};

int
bcm_board_cfm_info(cpudb_ref_t db_ref, bcm_board_cfm_info_t *info)
{
    int count = 0;
    cpudb_entry_t *cur;
    int         master_slot;

    master_slot = db_ref->master_entry ?
        db_ref->master_entry->base.slot_id : -1;

    info->is_master = FALSE;
    if (master_slot < 0) {
        info->master = -1;
    } else {
        CPUDB_FOREACH_ENTRY(db_ref, cur) {
            if (cur->base.slot_id < LM_SLOT_ID) {
                info->cfm[count].slot = cur->base.slot_id;
                info->cfm[count].modid = cur->dest_mod;
                if (cur->base.slot_id == master_slot) {
                    info->master = count;
                }
                count++;
                if (count >= NUM_CFM) {
                    break;
                }
            }
        }
        info->is_master = (master_slot == db_ref->local_entry->base.slot_id);
    }
    info->count = count;
    return BCM_E_NONE;
}


STATIC int
chassis_run(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int rv = BCM_E_PARAM;
    int board_id = db_ref->local_entry->base.board_id;

    if (board_id > cpudb_board_id_unknown &&
        board_id < cpudb_board_id_count) {

        if (prog[board_id]) {
            rv = (prog[board_id])(tp_cpu, db_ref);
        }
    }

    return rv;
}

int
bcm_board_chassis_setup(cpudb_ref_t db_ref)
{
    int flags = db_ref->local_entry->base.flags;

    if (CHASSIS_AST(flags)) {
        prog[cpudb_board_id_cfm_xgs2]     = chassis_ast_cfm_xgs2;
        prog[cpudb_board_id_lm_xgs2_6x]   = chassis_ast_xgs2_6x;
        prog[cpudb_board_id_lm_xgs2_48g]  = chassis_ast_xgs2_48g;
        prog[cpudb_board_id_lm_xgs3_12x]  = chassis_ast_xgs3_12x;
        prog[cpudb_board_id_lm_xgs3_48g]  = chassis_ast_xgs3_48g;
        prog[cpudb_board_id_cfm_xgs3]     = chassis_ast_cfm_xgs3;
        prog[cpudb_board_id_lm_56800_12x] = chassis_ast_56800_12x;
    } else if (CHASSIS_SM(flags)) {
        prog[cpudb_board_id_cfm_xgs2]     = chassis_smlb_cfm_xgs2;
        prog[cpudb_board_id_lm_xgs2_6x]   = chassis_smlb_xgs2_6x;
        prog[cpudb_board_id_lm_xgs2_48g]  = chassis_smlb_xgs2_48g;
        prog[cpudb_board_id_lm_xgs3_12x]  = chassis_smlb_xgs3_12x;
        prog[cpudb_board_id_lm_xgs3_48g]  = chassis_smlb_xgs3_48g;
        prog[cpudb_board_id_cfm_xgs3]     = chassis_smlb_cfm_xgs3;
        prog[cpudb_board_id_lm_56800_12x] = chassis_smlb_56800_12x;
    } else {
        prog[cpudb_board_id_cfm_xgs2]     = chassis_cfm_xgs2;
        prog[cpudb_board_id_lm_xgs2_6x]   = chassis_lm6x;
        prog[cpudb_board_id_lm_xgs2_48g]  = chassis_lm_xgs2_48g;
        prog[cpudb_board_id_lm_xgs3_12x]  = chassis_lm_xgs3_12x;
        prog[cpudb_board_id_lm_xgs3_48g]  = chassis_lm_xgs3_48g;
        prog[cpudb_board_id_cfm_xgs3]     = chassis_cfm_xgs3;
        prog[cpudb_board_id_lm_56800_12x] = chassis_lm_56800_12x;
    }

    return BCM_E_NONE;
}

int
bcm_board_topo_cfm_xgs2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    return chassis_run(tp_cpu, db_ref);
}

int
bcm_board_topo_lm6x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    return chassis_run(tp_cpu, db_ref);
}

int
bcm_board_topo_lm_xgs2_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    return chassis_run(tp_cpu, db_ref);
}

int
bcm_board_topo_lm_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    return chassis_run(tp_cpu, db_ref);
}

int
bcm_board_topo_cfm_xgs3(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    return chassis_run(tp_cpu, db_ref);
}

int
bcm_board_topo_lm_56800_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    return chassis_run(tp_cpu, db_ref);
}

#else

int
bcm_board_topo_cfm_xgs2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);
    return BCM_E_UNAVAIL;
}

int
bcm_board_topo_cfm_xgs3(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);
    return BCM_E_UNAVAIL;
}

int
bcm_board_topo_lm6x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);
    return BCM_E_UNAVAIL;
}

int
bcm_board_topo_lm_xgs2_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);
    return BCM_E_UNAVAIL;
}

int
bcm_board_topo_lm_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);
    return BCM_E_UNAVAIL;
}

int
bcm_board_topo_lm2x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);
    return BCM_E_UNAVAIL;
}

int
bcm_board_topo_lm_56800_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);
    return BCM_E_UNAVAIL;
}

#endif /* INCLUDE_CHASSIS */

