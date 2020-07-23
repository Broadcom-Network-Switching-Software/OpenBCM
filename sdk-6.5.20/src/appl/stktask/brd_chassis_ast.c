/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_chassis_ast.c
 * Purpose:     BCM956000 Chassis asymmetric trunk based load balancing
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/topo.h>
#include <bcm/mirror.h>
#include <bcm/field.h>
#include <appl/stktask/topo_brd.h>
#include <appl/stktask/stktask.h>       /* bcm_st_reserved_modid_enable_get */
#include <appl/cputrans/nh_tx.h>        /* nh_tx_src_get */

#include "topo_int.h"
#include "brd_chassis_int.h"

#if defined(INCLUDE_CHASSIS)

/* CFM and LM Hash Selection


   CFM and LM hashes are different so packets received from an LM has
   an opportunity to be load balanced on the outgoing CFM trunk. This
   is because the LMs use alternate links to connect to each CFM,
   e.g. hg0 and hg2 go to one CFM and hg1 and hg3 go to the other. If
   the same hash is used for both the LM and CFM, then the packets
   egressing the CFM would always use the same link, leaving half the
   available bandwidth unused:

     LM hg0 -> CFM0 trunk-link0
     LM hg1 -> CFM1 trunk-link1
     LM hg2 -> CFM0 trunk-link0
     LM hg3 -> CFM1 trunk-link1

   where "trunk-linkN" is the Nth member of the CFM egress trunk.

   If the CFM uses a different hash than the LM, then there's a
   reasonable chance that the traffic egressing the CFM will use both
   members of the trunk.
   
*/

#define XGS2_CFM_HASH (BCM_TRUNK_PSC_SRCDSTMAC | \
                       BCM_TRUNK_PSC_IPMACSA   | \
                       BCM_TRUNK_PSC_MACSA)

#define XGS2_LM_HASH  (BCM_TRUNK_PSC_SRCDSTMAC | \
                       BCM_TRUNK_PSC_IPMACDA   | \
                       BCM_TRUNK_PSC_MACDA)



static int _chassis_ast_default_lm_psc  = BCM_TRUNK_PSC_DSTMAC;
static int _chassis_ast_default_cfm_psc = BCM_TRUNK_PSC_SRCMAC;

/* CFM Trunk create callback handler */
STATIC int
chassis_ast_cfm_trunk(int unit, int tid,
                      bcm_trunk_info_t *trunk,
                      int member_count,
                      bcm_trunk_member_t *member_array,
                      void *cookie)
{
    COMPILER_REFERENCE(cookie);
    trunk->psc = _chassis_ast_default_cfm_psc;
    return bcm_trunk_set(unit, tid, trunk, member_count, member_array);
}

/* Single device CFM */
STATIC int
chassis_ast_cfm(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                  unit;
    bcm_module_t         mod;
    int                  rv = BCM_E_NONE;
    bcm_port_t           port;
    bcm_port_config_t    config;

    unit = 0;            /* board fabric unit */

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.cpu, port) {
        break;   /* Get CPU port */
    }
    mod = db_ref->local_entry->mod_ids[0];

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, mod));
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, port));
    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(unit,
                                              tp_cpu,
                                              db_ref->local_entry));
    bcm_board_unknown_src_modid_setup(db_ref->local_entry);
    bcm_board_trunk_callback_add(chassis_ast_cfm_trunk, NULL);
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(&tp_cpu->local_entry,
                                               db_ref, BCM_BOARD_TRUNK_NORMAL));
    return rv;
}

/* LM Trunk create callback handler */
STATIC int
chassis_ast_lm_trunk(int unit, int tid,
                     bcm_trunk_info_t *trunk,
                     int member_count,
                     bcm_trunk_member_t *member_array,
                     void *cookie)
{
    int i,j;
    int slot;
    bcm_module_t modid;
    bcm_board_cfm_info_t *cfm_info = (bcm_board_cfm_info_t *)cookie;
    bcm_port_t local_port;

    trunk->psc = _chassis_ast_default_lm_psc;
    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit, tid, trunk, member_count,
                member_array));

    for (i=0; i<member_count; i++) {
        BCM_IF_ERROR_RETURN(
            bcm_port_local_get(unit, member_array[i].gport, &local_port));
        for (j=0; j<cfm_info->count; j++) {
            slot = cfm_info->cfm[j].slot;
            modid = cfm_info->cfm[j].modid;
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META("slot:%d unit:%d tid:%d modid:%d port:%d n:%d\n"),
                         slot,
                         unit,
                         tid,
                         modid,
                         local_port,
                         cfm_info->count));
            /* Override trunking of CFM modid if there is more than
               one CFM */
            BCM_IF_ERROR_RETURN(
                bcm_trunk_override_ucast_set(unit, local_port, tid,
                                             modid,
                                             (cfm_info->count > 1)));
        }
    }

    return BCM_E_NONE;
}

/* LM with a single device that connects to the CFMs */

static int cfm_fp_entries = 0;
static bcm_field_group_t cfm_fp_group ;
static bcm_field_entry_t cfm_fp_entry[NUM_CFM];

STATIC int
install_cfm_fp(int unit, bcm_board_cfm_info_t *cfm_info)
{
    
    if (cfm_info->count > 1) {
        /* Install fp rule to redirect CFM destination packets. This
         is needed because the default L2 switch forwarding will not
         forward a packet on the link or trunk it was received
         on. Because all the LM external stack ports are trunked, that
         means that any packet received from one CFM that is destined
         for another CFM will be dropped.
        */
        bcm_field_qset_t  qset;
        bcm_port_t dstport;
        bcm_pbmp_t dstpbmp;
        int i;

        BCM_FIELD_QSET_INIT(qset);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);

        /* Destroy previous group if needed; ignore errors for now */
        if (cfm_fp_entries > 0) {
            int rv;

            for ( i=0; i<cfm_fp_entries; i++) {
                rv = bcm_field_entry_remove(unit, cfm_fp_entry[i]);
                LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                            (BSL_META("bcm_field_entry_remove(%d, %d)=%d\n"),
                             unit,
                             cfm_fp_entry[i],
                             rv));
            }
            rv = bcm_field_group_destroy(unit, cfm_fp_group);
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META("bcm_field_group_destroy(%d, %d)=%d\n"),
                         unit,
                         cfm_fp_group,
                         rv));
        }
        
        /* Create group */
        BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 
                                                   BCM_FIELD_GROUP_PRIO_ANY,
                                                   &cfm_fp_group));
        /* Create entries for each CFM */

        cfm_fp_entries = 0;

        for ( i=0; i<cfm_info->count; i++) {

            BCM_IF_ERROR_RETURN
                (bcm_field_entry_create(unit, cfm_fp_group, &cfm_fp_entry[i]));

            { int partner = (i == 0) ? 1 : 0;
            /* coverity[copy_paste_error] */
            BCM_IF_ERROR_RETURN
                (bcm_field_qualify_SrcPort(unit, cfm_fp_entry[i], 
                                            cfm_info->cfm[partner].modid,
                                            BCM_FIELD_EXACT_MATCH_MASK, 0, 0));
            }

            BCM_IF_ERROR_RETURN
                (bcm_field_qualify_DstPort(unit, cfm_fp_entry[i], 
                                            cfm_info->cfm[i].modid,
                                            BCM_FIELD_EXACT_MATCH_MASK, 0, 0));

            BCM_PBMP_CLEAR(dstpbmp);
            BCM_IF_ERROR_RETURN
                (bcm_topo_port_get(unit,
                                   cfm_info->cfm[i].modid,
                                   &dstport));
            BCM_PBMP_PORT_SET(dstpbmp, dstport);
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META_U(unit,
                        "port:%d pbmp[0]:%x\n"),
                         dstport,
                         _SHR_PBMP_WORD_GET(dstpbmp, 0)));

            BCM_IF_ERROR_RETURN
                (bcm_field_action_ports_add(unit, cfm_fp_entry[i], 
                                            bcmFieldActionRedirectPbmp,
                                            dstpbmp));

            BCM_IF_ERROR_RETURN
                (bcm_field_entry_install(unit, cfm_fp_entry[i]));

            cfm_fp_entries++;
        }
        
    }
    
    return BCM_E_NONE;
}

STATIC int
chassis_ast_lm(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref,
               bcm_board_port_connection_t *connection)
{
    int                  unit;
    bcm_module_t         mod;
    int                  rv = BCM_E_NONE;
    bcm_port_t           port;
    bcm_port_config_t    config;
    bcm_board_cfm_info_t cfm_info;

    unit = 0;            /* board fabric unit */

    bcm_board_cfm_info(db_ref, &cfm_info);
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.cpu, port) {
        break;   /* Get CPU port */
    }
    mod = db_ref->local_entry->mod_ids[0];

    if (connection) {
        BCM_IF_ERROR_RETURN(
            bcm_board_xgs_local_switch(unit, tp_cpu, &config, connection));
        BCM_IF_ERROR_RETURN(
            bcm_board_xgs_local_fabric(unit, tp_cpu, &config));
    } else {
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, mod));
        BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, port));
    }
    
    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(unit,
                                              tp_cpu,
                                              db_ref->local_entry));

    bcm_board_unknown_src_modid_setup(db_ref->local_entry);
    /* Trunk everything seen with hash 2, trunk create callback
       will handle CFM trunk override if necessary */

    bcm_board_trunk_callback_add(chassis_ast_lm_trunk, (void *)&cfm_info);
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(&tp_cpu->local_entry,
                                               db_ref,
                                               BCM_BOARD_TRUNK_ALL));

    BCM_IF_ERROR_RETURN(install_cfm_fp(unit, &cfm_info));
    
    return rv;
}


/*
 * Function:    chassis_ast_cfm_xgs2
 *
 * Purpose:
 *     Program the XGS2 CFM board using asymmetric trunking.
 */
int
chassis_ast_cfm_xgs2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5675 BCM956000 CFM (Asymmetric Trunk)\n")));
    _chassis_ast_default_cfm_psc = XGS2_CFM_HASH;
    return chassis_ast_cfm(tp_cpu, db_ref);
}


/*
 * Function:    chassis_ast_xgs2_48g
 *
 * Purpose:
 *     Program the XGS2-48G LM board using asymmetric trunking.
 */
int
chassis_ast_xgs2_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    bcm_board_port_connection_t connection[5] = {
        { -1, -1 }, /* 5675 unit 0 */
        { 12, 1  }, /* 5695 unit 1 is on port 1 */
        { 12, 2  }, /* 5695 unit 2 is on port 2 */
        { 12, 3  }, /* 5673 unit 3 is on port 3 */
        { 12, 4  }, /* 5673 unit 4 is on port 4 */
    };

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 569x 48GE BCM956000 LM (Asymmetric Trunk)\n")));
    _chassis_ast_default_lm_psc = XGS2_LM_HASH;
    return chassis_ast_lm(tp_cpu, db_ref, connection);
}

/*
 * Function:    chassis_ast_xgs2_6x
 *
 * Purpose:
 *     Program the XGS2-6X LM board using asymmetric trunking.
 */
int
chassis_ast_xgs2_6x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    bcm_board_port_connection_t connection[7] = {
        { -1, -1 },  /* 5675 unit 0 */
        {  1, 6  },  /* 5673 unit 1 is on port 6 */
        {  1, 7  },  /* 5673 unit 2 is on port 7 */
        {  1, 8  },  /* 5673 unit 3 is on port 8 */
        {  1, 1  },  /* 5673 unit 4 is on port 1 */
        {  1, 2  },  /* 5673 unit 5 is on port 2 */
        {  1, 3  },  /* 5673 unit 6 is on port 3 */
    };

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 5674 6XE BCM956000 LM (Asymmetric Trunk)\n")));
    _chassis_ast_default_lm_psc = XGS2_LM_HASH;
    return chassis_ast_lm(tp_cpu, db_ref, connection);
}

/*
 * Function:    chassis_ast_xgs3_12x
 *
 * Purpose:
 *     Program the XGS3-12X LM board using asymmetric trunking.
 */
int
chassis_ast_xgs3_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    bcm_board_port_connection_t connection[5] = {
        { -1, -1 }, /* 5675 unit 0 - no connection map entry */
        { 27, 7  }, /* 56501 unit 1 port 27 connects to 5675 port 7 */
        { 27, 8  }, /* 56501 unit 2 port 27 connects to 5675 port 8 */
        { 27, 1  }, /* 56501 unit 3 port 27 connects to 5675 port 1 */
        { 27, 2  }, /* 56501 unit 4 port 27 connects to 5675 port 2 */
    };

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56501 12XE BCM956000 LM (Asymmetric Trunk)\n")));
    _chassis_ast_default_lm_psc = XGS2_LM_HASH;
    return chassis_ast_lm(tp_cpu, db_ref, connection);
}


/*
 * Function:    chassis_ast_cfm_xgs3
 *
 * Purpose:
 *     Program the XGS3 CFM board using asymmetric trunking.
 */
int
chassis_ast_cfm_xgs3(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS 56700 BCM956000 CFM (Asymmetric Trunk)\n")));
    return chassis_ast_cfm(tp_cpu, db_ref);
}

/*
 * Function:    chassis_ast_cfm_xgs3
 *
 * Purpose:
 *     Program the 56800 LM board using asymmetric trunking.
 */
int
chassis_ast_56800_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56800 BCM956000 LM (Asymmetric Trunk)\n")));
    return chassis_ast_lm(tp_cpu, db_ref, NULL);
}

/*****************************************************************************

  LM XGS3 48G

  The XGS3 48G Line module is more complex that the others because

   (a) Load balancing is unit based, rather than trunk based due to
       the board connectivity

   (b) Unlike the other LMs, it introduces a loop into the system
       topology, which must be pruned to eliminate multicast storms.

  The two units on the board are labled as 'master' and 'partner'. The
  master unit is the unit that connects directly to the master CFM;
  the partner is the other unit. This relationship varies, depending
  on the CFM and backplane, so make the determination is made
  dynamically.

 */

#define FB_HG0 24 /* External */
#define FB_HG1 25 /* External */
#define FB_HG2 26 /* Internal */
#define FB_HG3 27 /* Internal */

/*

  Determine which unit is connected to the master CFM and which is the
  partner.

 */

STATIC int
_ast_xgs3_48g_unit(topo_cpu_t *tp_cpu,
                  bcm_board_cfm_info_t *cfm_info,
                  int *master,
                  int *partner)
{
    int             i,m;
    topo_stk_port_t *tsp;
    cpudb_entry_t   *l_entry = &tp_cpu->local_entry;
    int             unit, master_unit = -1;
    bcm_module_t    cfm_modid;

    if (cfm_info->count == 0) {
        /* No CFM */
        *master = 0;
        *partner = 1;
        return BCM_E_NONE;
    }

    cfm_modid = cfm_info->cfm[cfm_info->master].modid;
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        if (!(l_entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED)) {
            /* Skip unresolved ports */
            continue;
        }

        tsp = &tp_cpu->tp_stk_port[i];
        unit = l_entry->base.stk_ports[i].unit;

        if (master_unit < 0) {
            for (m = 0; m < tsp->tx_mod_num; m++) {
                if (tsp->tx_mods[m] == cfm_modid) {
                    *master = unit;
                    *partner = (unit == 0) ? 1 : 0;
                    return BCM_E_NONE;
                }
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

/*

   Program the external module mapping. If there's only one CFM,
   then the master unit points to all the external modids, and the
   partner points to the master unit. If there's two CFMs, then both
   units can point to all the external modids.

 */

STATIC int
_ast_xgs3_48g_ext_mod_program(topo_cpu_t *tp_cpu,
                              bcm_board_cfm_info_t *cfm_info,
                              int master, int partner)
{
    int             i,m;
    topo_stk_port_t *tsp;
    cpudb_entry_t   *l_entry = &tp_cpu->local_entry;
    bcm_port_t      port;
    bcm_module_t    mod;
    int             dual = (cfm_info->count > 1);

    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        if (!(l_entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED)) {
            /* Skip unresolved ports */
            continue;
        }

        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(master, mod, port));
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(partner, mod,
                                                    (dual) ? port : FB_HG2));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    chassis_ast_xgs3_48g
 *
 * Purpose:
 *     Program the XGS3-48G LM board using asymmetric trunking.
 */
int
chassis_ast_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    bcm_module_t             master_modid;
    bcm_module_t             partner_modid;
    bcm_board_cfm_info_t     cfm_info;
    bcm_port_config_t        config;
    int                      master, partner;

    bcm_board_cfm_info(db_ref, &cfm_info);

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: XGS3 56504 48GE BCM956000 LM (Asymmetric Trunk)\n")));

    /* Get master and partner units and modids */
    BCM_IF_ERROR_RETURN(_ast_xgs3_48g_unit(tp_cpu, &cfm_info,
                                           &master, &partner));

    if ((master < 0) || (master > 1) || (partner < 0) || (partner > 1)) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("Parameter error: master=%d partner=%d\n"),
                     master, partner));
        return BCM_E_INTERNAL;
    }

    master_modid = tp_cpu->local_entry.mod_ids[master];
    partner_modid = tp_cpu->local_entry.mod_ids[partner];

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Master: %d,%d\n"),
                 master, master_modid));
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Partner: %d,%d\n"),
                 partner, partner_modid));

    /* Assign modids */
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(master, master_modid));
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(partner, partner_modid));

    /* Set external modid mapping */
    BCM_IF_ERROR_RETURN(_ast_xgs3_48g_ext_mod_program(tp_cpu, &cfm_info,
                                                  master, partner));

    /* Set internal modid mapping */
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(master,
                                            partner_modid,
                                            FB_HG2));

    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(partner,
                                            master_modid,
                                            FB_HG2));

    /* Add internal stack ports */
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(master,
                                                        FB_HG2));
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(master,
                                                        FB_HG3));
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(partner,
                                                        FB_HG2));
    BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(partner,
                                                        FB_HG3));

    /* Create trunks */

    bcm_board_trunk_callback_add(chassis_ast_lm_trunk, (void *)&cfm_info);
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(&tp_cpu->local_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));
    BCM_IF_ERROR_RETURN(_bcm_board_setup_trunk_lm_xgs3_48g());

    BCM_IF_ERROR_RETURN(bcm_port_config_get(0, &config));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(0, tp_cpu, db_ref,
                                                &config));
    BCM_IF_ERROR_RETURN(bcm_port_config_get(1, &config));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(1, tp_cpu, db_ref,
                                                &config));
    if (cfm_info.count > 1) {
        /* flood block internal to external stack ports */
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(master,
                                                     FB_HG2, FB_HG0, -1U));
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(master,
                                                     FB_HG2, FB_HG1, -1U));
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(master,
                                                     FB_HG3, FB_HG0, -1U));
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(master,
                                                     FB_HG3, FB_HG1, -1U));
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(partner,
                                                     FB_HG2, FB_HG0, -1U));
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(partner,
                                                     FB_HG2, FB_HG1, -1U));
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(partner,
                                                     FB_HG3, FB_HG0, -1U));
        BCM_IF_ERROR_RETURN(bcm_port_flood_block_set(partner,
                                                     FB_HG3, FB_HG1, -1U));
    }

#ifdef BCM_BOARD_AUTO_E2E
    /* intra-board end to end flow control, if available */
    BCM_IF_ERROR_RETURN(_bcm_board_dual_xgs3_e2e_set(master,  master_modid,
                                                     partner, partner_modid));
#endif  /* BCM_BOARD_AUTO_E2E */

    return BCM_E_NONE;
}



#else
typedef int _brd_chassis_ast_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_CHASSIS */
