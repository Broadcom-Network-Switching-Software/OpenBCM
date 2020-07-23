/** \file diag_dnx_l2.c
 *
 * Main diagnostics for l2 applications
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_L2TABLE

/*
 * {
 */
/*************
 * INCLUDES  *
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_l2.h>

/** soc */
#include <soc/macipadr.h>
#include <soc/mem.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>

/** bcm */
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/l2/l2_addr.h>
#include <bcm_int/dnx/l2/l2_learn.h>

/** sal */
#include <sal/appl/sal.h>

#include "../dbal/diag_dnx_dbal_internal.h"

extern void increment_macaddr(
    sal_mac_addr_t macaddr,
    int amount);

/** CLEAR command options to delete MAC entries 'by' these arguments */
/** Indicate that deletion of the entries should be done by mac */
#define DELETE_BY_MAC     0x00000001
/** Indicate that deletion of the entries should be done by vlan */
#define DELETE_BY_VLAN    0x00000002
/** Indicate that deletion of the entries should be done by port */
#define DELETE_BY_PORT    0x00000004
/** Indicate that deletion of the entries should be done by tgid */
#define DELETE_BY_TGID    0x00000008

uint32 l2_add_counter = 0;
/**
 * \brief
 * Print l2 learn mode and aging time.
 */
static shr_error_e
sh_dnx_l2_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int learn_mode;
    int age_seconds;
    char learn_mode_string[64];
    uint32 opportunistic_learning;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_OPPORTUNISTIC_LEARNING, INST_SINGLE,
                                   &opportunistic_learning);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (opportunistic_learning == DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_OPPORTUNISTIC_LEARNING)
    {
        LOG_CLI((BSL_META("L2 Opportunistic Learning: Opportunistic learn mode is ON \n")));
    }
    else
    {
        LOG_CLI((BSL_META("L2 Opportunistic Learning: Opportunistic learn mode is OFF \n")));
    }

    SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, bcmSwitchL2LearnMode, &learn_mode));

    switch (learn_mode)
    {
        case BCM_L2_INGRESS_CENT:
            sal_sprintf(learn_mode_string, "Ingress Centralized");
            break;
        case BCM_L2_INGRESS_DIST:
            sal_sprintf(learn_mode_string, "Ingress Distributed");
            break;
        case BCM_L2_EGRESS_DIST:
            sal_sprintf(learn_mode_string, "Egress Distributed");
            break;
        case BCM_L2_EGRESS_CENT:
            sal_sprintf(learn_mode_string, "Egress Centralized");
            break;
        case (BCM_L2_INGRESS_CENT | BCM_L2_LEARN_CPU):
            sal_sprintf(learn_mode_string, "Ingress Centralized with CPU Learning");
            break;
        case (BCM_L2_EGRESS_CENT | BCM_L2_LEARN_CPU):
            sal_sprintf(learn_mode_string, "Egress Centralized with CPU Learning");
            break;
    }

    LOG_CLI((BSL_META("L2 Learning Mode: %s \n"), learn_mode_string));

    LOG_CLI((BSL_META("L2 Learning Limit Mode: %s\n"), LEARN_LIMIT_MODE_NAME(unit)));

    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_get(unit, &age_seconds));

    if (age_seconds == 0)
    {
        LOG_CLI((BSL_META("L2 Aging: Aging is OFF \n")));
    }
    else
    {
        LOG_CLI((BSL_META("L2 Aging: Meta cycle time is %d seconds \n"), age_seconds));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static void
l2_entry_event_handle(
    int unit,
    bcm_l2_addr_t * l2addr,
    int operation,
    void *userdata)
{
    uint32 flags = *((uint32 *) userdata);

    if (operation == BCM_L2_CALLBACK_AGE_EVENT)
    {
        /*
         * Nothing to do here
         */
        return;
    }

    if (flags & L2_LEARN_COUNTER_ONLY)
    {
        l2_add_counter++;
    }
    else
    {
        bcm_l2_addr_add(unit, l2addr);
    }
    if (flags & L2_LEARN_CALLBACK_LOGGER)
    {
        LOG_CLI((BSL_META("Adding MAC address - %02X:%02X:%02X:%02X:%02X:%02X on op:%d\n"),
                 l2addr->mac[0], l2addr->mac[1], l2addr->mac[2], l2addr->mac[3], l2addr->mac[4], l2addr->mac[5],
                 operation));
    }
}

/**
 * \brief
 * Config l2 learn mode
 */
static shr_error_e
sh_dnx_l2_config_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int learn_mode, cfg_learn_mode;
    int logger, callback, counter;
    static uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("mode", cfg_learn_mode);
    SH_SAND_GET_BOOL("callback", callback);
    SH_SAND_GET_BOOL("logger", logger);
    SH_SAND_GET_BOOL("counter", counter);

    SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, bcmSwitchL2LearnMode, &learn_mode));
    if (learn_mode == cfg_learn_mode)
    {
        LOG_CLI((BSL_META("Learn Mode is already:%s\n"),
                 sh_sand_enum_value_text(sand_control, "mode", cfg_learn_mode)));
    }
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchL2LearnMode, cfg_learn_mode));
    if (callback == TRUE)
    {
        if (flags & L2_LEARN_CALLBACK_ACTIVE)
        {
            SHR_IF_ERR_EXIT(bcm_l2_addr_unregister(unit, l2_entry_event_handle, (void *) (&flags)));
        }
        if (logger == TRUE)
        {
            flags |= L2_LEARN_CALLBACK_LOGGER;
        }
        else
        {
            flags &= ~L2_LEARN_CALLBACK_LOGGER;
        }
        if (counter == TRUE)
        {
            flags |= L2_LEARN_COUNTER_ONLY;
        }
        else
        {
            flags &= ~L2_LEARN_COUNTER_ONLY;
        }
        SHR_IF_ERR_EXIT(bcm_l2_addr_register(unit, l2_entry_event_handle, (void *) (&flags)));
        flags |= L2_LEARN_CALLBACK_ACTIVE;
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_l2_addr_unregister(unit, l2_entry_event_handle, (void *) (&flags)));
        flags &= ~L2_LEARN_CALLBACK_ACTIVE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Print l2 entry
 */
static int
dnx_l2addr_dump(
    int unit,
    bcm_l2_addr_t * l2addr,
    void *user_data)
{
    prt_control_t *prt_ctr = (prt_control_t *) user_data;
    dnx_algo_gpm_forward_info_t forward_info;
    bcm_gport_t gport = 0;
    uint32 destination_value;
    dbal_fields_e sub_field_id;
    uint32 outlif;
    uint32 destination;
    uint32 result_type;
    uint32 eei = 0;
    SHR_FUNC_INIT_VARS(unit);

    destination_value = 0;
    outlif = 0;
    destination = 0;
    result_type = 0;
    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    l2addr->flags2 |= BCM_L2_FLAGS2_HIT_GET;

    SHR_IF_ERR_EXIT(bcm_dnx_l2_addr_by_struct_get(unit, l2addr));

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%02X:%02X:%02X:%02X:%02X:%02X", l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
                 l2addr->mac[3], l2addr->mac[4], l2addr->mac[5]);
    PRT_CELL_SET("%d", l2addr->vid);

    if (((l2addr->flags & BCM_L2_TRUNK_MEMBER) == 0) && ((l2addr->flags & BCM_L2_MCAST) == 0))
    {
        if (l2addr->encap_id == -1)
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                            (unit, l2addr->port, l2addr->encap_id, &forward_info));
            SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information(unit, &l2addr->port,
                                                                              (uint32 *) (&l2addr->encap_id),
                                                                              &forward_info, 1));
        }
        PRT_CELL_SET("0x%08x", l2addr->port);
        PRT_CELL_SET("0x%08x", l2addr->encap_id);
    }

    if ((l2addr->flags & BCM_L2_DISCARD_DST) == 0)
    {

        if (_SHR_IS_FLAG_SET(l2addr->flags, BCM_L2_MCAST))
        {
            SHR_IF_ERR_EXIT(dnx_convert_l2_addr_to_dest(unit, l2addr, &destination, &outlif, &result_type, &eei));
            /** Decode the destination to sub_field_id and destination_value */
            SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                            (unit, DBAL_FIELD_DESTINATION, &destination, &sub_field_id, &destination_value));
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                            (unit, l2addr->port, l2addr->encap_id, &forward_info));

            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, forward_info.destination, &gport));

            /** Decode the destination to sub_field_id and destination_value */
            SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                            (unit, DBAL_FIELD_DESTINATION, &forward_info.destination, &sub_field_id,
                             &destination_value));
        }

        if (((l2addr->flags & BCM_L2_TRUNK_MEMBER) != 0) || ((l2addr->flags & BCM_L2_MCAST) != 0))
        {
            PRT_CELL_SET("0x%08x", gport);
            PRT_CELL_SET("0x%08x", l2addr->encap_id);
        }
        /** Print the destination */
        PRT_CELL_SET("0x%x", destination_value);
        /** Print the type of destination */
        PRT_CELL_SET("%s", dbal_field_to_string(unit, sub_field_id));

    }
    else
    {
        /** Print DiscardDst in column Destination in case that BCM_L2_DISCARD_DST is set */
        PRT_CELL_SET("%s", "DiscardDst");
        /** Print the type of destination in case that BCM_L2_DISCARD_DST is set */
        PRT_CELL_SET("%s", "Lif Trap");
    }

    PRT_CELL_SET("%u", forward_info.outlif);
    PRT_CELL_SET("%s", ((l2addr->flags & BCM_L2_STATIC) == 0) ? "D" : "S");
    if (l2addr->modid)
    {
        PRT_CELL_SET("%d", l2addr->modid);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse the MACT and print the l2 entries
 */
static shr_error_e
sh_dnx_l2_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int svl_flag;
    int traverse_flags;
    PRT_INIT_VARS;
    bcm_l2_addr_t match_addr;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("SVL", svl_flag);
    if (svl_flag == FALSE)
    {
        svl_flag = TRUE;
    }
    PRT_TITLE_SET("MACT address table");
    PRT_COLUMN_ADD("MAC address");
    PRT_COLUMN_ADD("vsi");
    PRT_COLUMN_ADD("Gport");
    PRT_COLUMN_ADD("encap_id");
    PRT_COLUMN_ADD("Destination");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("OutLif");
    PRT_COLUMN_ADD("Stat");

    sal_memset((void *) &match_addr, 0, sizeof(bcm_l2_addr_t));

    /** Return all the entries in FWD_MACT regardless of the hit bit. Static and dynamic entries. */
    traverse_flags = BCM_L2_TRAVERSE_IGNORE_DES_HIT | BCM_L2_TRAVERSE_MATCH_STATIC;

    if (svl_flag == TRUE)
    {
        SHR_IF_ERR_EXIT(bcm_l2_matched_traverse(unit, traverse_flags, &match_addr, dnx_l2addr_dump, (void *) prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_l2_clear_all(
    int unit,
    int svl)
{
    bcm_l2_addr_t dummy_addr;
    bcm_mac_t dummy_mac;
    int flags;

    SHR_FUNC_INIT_VARS(unit);
    /** Delete all the entries if no argument is set */
    flags =
        BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS |
        BCM_L2_REPLACE_IGNORE_DES_HIT;

    dummy_mac[0] = dummy_mac[1] = dummy_mac[2] = dummy_mac[3] = dummy_mac[4] = dummy_mac[5] = 0;
    bcm_l2_addr_t_init(&dummy_addr, dummy_mac, 0);
    if (svl == TRUE)
    {
        LOG_CLI((BSL_META("Deleting L2 mac table\n")));
        SHR_IF_ERR_EXIT(bcm_l2_replace(unit, flags, &dummy_addr, 0, 0, 0));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Clear l2 entries from MACT table in accordance with the supplied arguments.
 * The entries could be deleted by mac/vlan/mac and vlan/port/tgid/mac and port/vlan and port/vlan and tgid
 */
static shr_error_e
sh_dnx_l2_clear_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *static_str;
    bcm_mac_t mac_addr;
    bcm_trunk_t tgid;
    bcm_vlan_t vlan;
    bcm_module_t modid;
    bcm_pbmp_t logical_ports;
    bcm_port_t port, logical_port;
    int nof_ports;
    int mac_static;
    int delete_by;
    int all;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(logical_ports);
    logical_port = -1;
    nof_ports = 0;
    delete_by = 0;
    port = -1;

    SH_SAND_GET_MAC("mac", mac_addr);
    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_GET_INT32("tgid", tgid);
    SH_SAND_GET_INT32("modid", modid);
    SH_SAND_GET_BOOL("static", mac_static);
    SH_SAND_GET_BOOL("all", all);

    if (all)
    {
        int svl;
        SH_SAND_GET_BOOL("svl", svl);
        if (svl == FALSE)
        {
            svl = TRUE;
        }

        /** reset virtual counter */
        l2_add_counter = 0;
        /** Delete all the entries if no argument is set */
        SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, svl));
        SHR_EXIT();
    }

    BCM_PBMP_COUNT(logical_ports, nof_ports);
    if (nof_ports != 0)
    {
        BCM_PBMP_ITER(logical_ports, port)
        {
            logical_port = port;
            break;
        }
    }

    /** Determine which arguments were supplied */
    if (ENET_CMP_MACADDR(mac_addr, _soc_mac_all_zeroes))
    {
        delete_by |= DELETE_BY_MAC;
    }
    if (vlan > 0)
    {
        delete_by |= DELETE_BY_VLAN;
    }
    if (logical_port >= 0)
    {
        delete_by |= DELETE_BY_PORT;
    }
    if (tgid >= 0)
    {
        delete_by |= DELETE_BY_TGID;
    }

    static_str = mac_static ? "static and non-static" : "non-static";
    mac_static = mac_static ? BCM_L2_DELETE_STATIC : 0;

    switch (delete_by)
    {
        case DELETE_BY_MAC:
        {
            LOG_CLI((BSL_META("Deleting %s addresses by MAC\n"), static_str));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete_by_mac(unit, mac_addr, mac_static));
            break;
        }
        case DELETE_BY_VLAN:
        {
            LOG_CLI((BSL_META("Deleting %s addresses by VLAN\n"), static_str));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete_by_vlan(unit, vlan, mac_static));
            break;
        }
        case DELETE_BY_MAC | DELETE_BY_VLAN:
        {
            LOG_CLI((BSL_META("Deleting an address by MAC and VLAN\n")));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete(unit, mac_addr, vlan));
            break;
        }
        case DELETE_BY_PORT:
        {
            LOG_CLI((BSL_META("Deleting %s addresses by modid/port\n"), static_str));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete_by_port(unit, modid, port, mac_static));
            break;
        }
        case DELETE_BY_TGID:
        {
            BCM_TRUNK_ID_SET(tgid, 0, tgid);
            LOG_CLI((BSL_META("Deleting %s addresses by trunk ID\n"), static_str));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete_by_trunk(unit, tgid, mac_static));
            break;
        }
        case DELETE_BY_MAC | DELETE_BY_PORT:
        {
            LOG_CLI((BSL_META("Deleting %s addresses by MAC and module/port\n"), static_str));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete_by_mac_port(unit, mac_addr, modid, port, mac_static));
            break;
        }
        case DELETE_BY_VLAN | DELETE_BY_PORT:
        {
            LOG_CLI((BSL_META("Deleting %s addresses by VLAN and modid/port\n"), static_str));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete_by_vlan_port(unit, vlan, modid, port, mac_static));
            break;
        }
        case DELETE_BY_VLAN | DELETE_BY_TGID:
        {
            LOG_CLI((BSL_META("Deleting %s addresses by VLAN and trunk ID\n"), static_str));
            SHR_IF_ERR_EXIT(bcm_l2_addr_delete_by_vlan_trunk(unit, vlan, tgid, mac_static));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown argument combination for deleting\n");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add l2 entry to FWD_MACT.
 *
 * There is a possibility to add incrementing mac addresses associated with port/s.
 */
static shr_error_e
sh_dnx_l2_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mac_t mac_addr;
    uint32 vsi;
    uint32 vsi_value;
    uint32 svl_vsi_value;
    bcm_trunk_t tgid;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    int nof_ports;
    bcm_l2_addr_t l2addr;
    int l2_static;
    int l2_mc_group;
    bcm_port_t trunk_gport = 0;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(logical_ports);
    nof_ports = 0;
    port = -1;

    SH_SAND_GET_MAC("mac", mac_addr);
    SH_SAND_GET_INT32("vsi", vsi_value);
    SH_SAND_GET_INT32("vlan", svl_vsi_value);
    SH_SAND_GET_PORT("portbitmap", logical_ports);
    SH_SAND_GET_INT32("tgid", tgid);
    SH_SAND_GET_BOOL("static", l2_static);
    SH_SAND_GET_INT32("mc_group", l2_mc_group);

    if (((vsi_value != 0) && (svl_vsi_value != 0)) || ((vsi_value == 0) && (svl_vsi_value == 0)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vsi or vlan should be set. Use only one of the fields for SVL.\n");
    }

    /** For SVL can be configured either vlan or vsi for vsi */
    if (svl_vsi_value != 0)
    {
        vsi = svl_vsi_value;
    }
    else
    {
        vsi = vsi_value;
    }

    /** VSI must be always provided, check if it is valid */
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (vsi >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. vsi = %d is not a valid vsi. Expected a number smaller than %d \n", vsi, nof_vsis);
    }
    BCM_PBMP_COUNT(logical_ports, nof_ports);

    if ((nof_ports == 0) && (tgid == -1) && (l2_mc_group == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "It is expected one of the following to be configured: Port or portbitmap/tgid/mc_group!\n");
    }

    if ((ENET_CMP_MACADDR(mac_addr, _soc_mac_all_zeroes)) && (vsi > 0))
    {
        bcm_l2_addr_t_init(&l2addr, mac_addr, vsi);
    }

    if (l2_static)
    {
        l2addr.flags |= BCM_L2_STATIC;
    }
    if ((nof_ports != 0) && (tgid < 0) && (l2_mc_group == 0))
    {
        BCM_PBMP_ITER(logical_ports, port)
        {
            l2addr.port = port;

            /** Adding the l2 entry to FWD_MACT */
            SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

            /*
             * Increment the mac address for the next call
             * There is a possibility to add several l2 entries with incrementing mac addresses associated to the
             * ports in the provided portbitmap.
             */
            increment_macaddr(mac_addr, 1);
            bcm_l2_addr_t_init(&l2addr, mac_addr, vsi);
            if (l2_static)
            {
                l2addr.flags |= BCM_L2_STATIC;
            }
        }
    }
    /** In case that l2_mc_group is provided */
    else if ((nof_ports == 0) && (tgid < 0) && (l2_mc_group > 0))
    {

        l2addr.flags |= BCM_L2_MCAST;
        l2addr.l2mc_group = l2_mc_group;

        /** Adding the l2 entry to FWD_MACT */
        SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));
    }
    /** In case that tgid is provided */
    else if ((nof_ports == 0) && (tgid > 0) && (l2_mc_group == 0))
    {
        BCM_TRUNK_ID_SET(tgid, 0, tgid);
        BCM_GPORT_TRUNK_SET(trunk_gport, tgid);
        l2addr.flags |= BCM_L2_TRUNK_MEMBER;
        l2addr.tgid = tgid;
        l2addr.port = trunk_gport;

        /** Adding the l2 entry to FWD_MACT */
        SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong combination of input parameters. Port/portbitmap, tgid and mc_group should be called separately.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete l2 entry from FWD_MACT.
 */
static shr_error_e
sh_dnx_l2_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mac_t mac_addr;
    uint32 vsi;
    uint32 vsi_value;
    uint32 svl_vsi_value;
    bcm_l2_addr_t l2addr;
    int nof_entries_to_delete;
    int index;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_MAC("mac", mac_addr);
    SH_SAND_GET_INT32("vsi", vsi_value);
    SH_SAND_GET_INT32("vlan", svl_vsi_value);
    SH_SAND_GET_INT32("count", nof_entries_to_delete);

    if (((vsi_value != 0) && (svl_vsi_value != 0)) || ((vsi_value == 0) && (svl_vsi_value == 0)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "vsi or vlan should be set. Use only one of the fields for SVL and only vsi for IVL.\n");
    }

    /** For SVL can be configured either vlan or vsi for vsi, while for IVL can be configured only vsi */
    if (svl_vsi_value != 0)
    {
        vsi = svl_vsi_value;
    }
    else
    {
        vsi = vsi_value;
    }

    /** VSI must be always provided, check if it is valid */
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (vsi >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. vsi = %d is not a valid vsi. Expected a number smaller than %d \n", vsi, nof_vsis);
    }

    for (index = 0; index < nof_entries_to_delete; index++)
    {
        if (ENET_CMP_MACADDR(mac_addr, _soc_mac_all_zeroes))
        {
            bcm_l2_addr_t_init(&l2addr, mac_addr, vsi);
            /** Delete the l2 entry from FWD_MACT */
            SHR_IF_ERR_EXIT(bcm_l2_addr_by_struct_delete(unit, &l2addr));
            increment_macaddr(mac_addr, 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if VSI exists
 */
shr_error_e
sh_dnx_l2_vsi_exists(
    int unit,
    bcm_vlan_t vsi)
{
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the VSI is allocated
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_l2_get_mact_count(
    int unit,
    uint32 *l2_entry_counter_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MACT_ENTRY_COUNTER, INST_SINGLE, l2_entry_counter_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_l2_get_mact_vsi_count(
    int unit,
    int vsi,
    uint32 *l2_entry_counter_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_VSI_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER, INST_SINGLE, l2_entry_counter_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_l2_get_mact_inlif_count(
    int unit,
    bcm_gport_t gport,
    uint32 *l2_entry_counter_p)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Fetch OUT_LIF from gport
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF, &gport_hw_resources));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_VSI_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, gport_hw_resources.global_in_lif);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER, INST_SINGLE, l2_entry_counter_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the number of MACT entries
 */
static shr_error_e
sh_dnx_l2_count_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 l2_entry_counter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &l2_entry_counter));

    LOG_CLI((BSL_META("The number of MACT entries is: %d\n"), l2_entry_counter));
    if (l2_add_counter != 0)
    {
        LOG_CLI((BSL_META("The number of new MAC addresses registered: %d\n"), l2_add_counter));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Present VSI properties
 */
static shr_error_e
sh_dnx_l2_vsi_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 l2_entry_counter;
    uint32 entry_handle_id;
    int is_end = 0;
    int vsi;
    int vsi_profile;
    bcm_l2_learn_limit_t l2_learn_limit;
    dnx_learn_limit_mode_t learn_limit_mode;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit)) != LEARN_LIMIT_MODE_VLAN)
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Supported only in VLAN mode\n");
    }

    PRT_TITLE_SET("VSI Properties");
    PRT_COLUMN_ADD("VSI");
    PRT_COLUMN_ADD("Learn Profile");
    PRT_COLUMN_ADD("Limit");
    PRT_COLUMN_ADD("Count");

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, (uint32 *) &vsi));
        SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_vsi_count(unit, vsi, &l2_entry_counter));

        bcm_l2_learn_limit_t_init(&l2_learn_limit);
        l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_VLAN;
        l2_learn_limit.vlan = vsi;

        SHR_IF_ERR_EXIT(bcm_l2_learn_limit_get(unit, &l2_learn_limit));
        /** Get the vsi learning profile of the vsi */
        SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &vsi_profile));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", vsi);
        PRT_CELL_SET("%d", vsi_profile);
        PRT_CELL_SET("%d", l2_learn_limit.limit);
        PRT_CELL_SET("%d", l2_entry_counter);

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Present VSI properties
 */
static shr_error_e
sh_dnx_l2_lif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 l2_entry_counter;
    uint32 entry_handle_id;
    int is_end = 0;
    int lif;
    bcm_gport_t gport;
    int vsi_profile;
    bcm_l2_learn_limit_t l2_learn_limit;
    dnx_learn_limit_mode_t learn_limit_mode;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit)) != LEARN_LIMIT_MODE_VLAN_PORT)
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Supported only in VLAN_PORT mode\n");
    }

    PRT_TITLE_SET("LIF Properties");
    PRT_COLUMN_ADD("OUTLIF");
    PRT_COLUMN_ADD("Learn Profile");
    PRT_COLUMN_ADD("Limit");
    PRT_COLUMN_ADD("Count");

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
/*
 * Just take for all types for now - may be we'll specific types per specific requests
 * {
    uint32 resultType;
    resultType = DBAL_RESULT_TYPE_TEST_IN_AC_INFO_DB_IN_LIF_AC_MP;

    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                             DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
   }
*/
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, (uint32 *) &lif));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        if (lif == 0)
            continue;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, lif, &gport));

        SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_inlif_count(unit, gport, &l2_entry_counter));

        bcm_l2_learn_limit_t_init(&l2_learn_limit);
        l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_PORT;
        l2_learn_limit.port = gport;

        SHR_IF_ERR_EXIT(bcm_l2_learn_limit_get(unit, &l2_learn_limit));
        /** Get the vsi learning profile of the lif */
        SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, lif, &vsi_profile));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", lif);
        PRT_CELL_SET("%d", vsi_profile);
        PRT_CELL_SET("%d", l2_learn_limit.limit);
        PRT_CELL_SET("%d", l2_entry_counter);

    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_l2_clear_options[] = {
    {"mac", SAL_FIELD_TYPE_MAC, "delete by mac_addr", "00:00:00:00:00:00"},
    {"vlan", SAL_FIELD_TYPE_INT32, "delete by vlan", "0"},
    {"port", SAL_FIELD_TYPE_PORT, "delete by port", "none"},
    {"tgid", SAL_FIELD_TYPE_INT32, "delete by trunk id", "-1"},
    {"modid", SAL_FIELD_TYPE_INT32, "delete by modid", "-1"},
    {"static", SAL_FIELD_TYPE_BOOL,
     "If it is set, l2 static and non-static entries will be deleted. False means that only non-static entries will be deleted.",
     "false"},
    {"all", SAL_FIELD_TYPE_BOOL, "delete all entries from MACT", "false"},
    {"svl", SAL_FIELD_TYPE_BOOL, "Together with all deletes only SVL Entries", "No"},
    {NULL}
};

static sh_sand_enum_t learn_mode[] = {
    {"IngDstr", BCM_L2_INGRESS_DIST, "Ingress Distributed(HW) mode"},
    {"EgDstr", BCM_L2_EGRESS_DIST, "Egress Distributed(HW) mode"},
    {"IngCntr", BCM_L2_INGRESS_CENT, "Ingress Centralized mode"},
    {"EgCntr", BCM_L2_EGRESS_DIST, "Egress Distributed mode"},
    {"IngCntrCPU", BCM_L2_INGRESS_CENT | BCM_L2_LEARN_CPU, "Ingress Centralized mode with CPU"},
    {"EgCntrCPU", BCM_L2_EGRESS_DIST | BCM_L2_LEARN_CPU, "Egress Distributed mode with CPU"},
    {NULL}
};

static sh_sand_option_t dnx_l2_config_options[] = {
    {"mode", SAL_FIELD_TYPE_ENUM, "Learn Mode", NULL, (void *) learn_mode},
    {"callback", SAL_FIELD_TYPE_BOOL, "Insert entry adding callback", "Yes"},
    {"logger", SAL_FIELD_TYPE_BOOL, "Enable/Disable learn message logging", "No"},
    {"counter", SAL_FIELD_TYPE_BOOL, "When enabled no actual add occurs, only count", "No"},
    {NULL}
};

static sh_sand_option_t dnx_l2_show_options[] = {
    {"svl", SAL_FIELD_TYPE_BOOL, "Show only SVL Entries", "No"},
    {NULL}
};

static sh_sand_option_t dnx_l2_add_options[] = {
    {"mac", SAL_FIELD_TYPE_MAC, "mac_addr", NULL},
    {"vsi", SAL_FIELD_TYPE_INT32, "VSI", "0"},
    {"vlan", SAL_FIELD_TYPE_INT32, "VSI", "0"},
    {"portbitmap", SAL_FIELD_TYPE_PORT, "portbitmap or port id", "none"},
    {"tgid", SAL_FIELD_TYPE_INT32, "trunk ID", "-1"},
    {"static", SAL_FIELD_TYPE_BOOL, "Flag BCM_L2_STATIC that by default is set", "true"},
    {"mc_group", SAL_FIELD_TYPE_INT32, "Multicast Group", "0"},
    {NULL}
};

static sh_sand_option_t dnx_l2_delete_options[] = {
    {"mac", SAL_FIELD_TYPE_MAC, "mac_addr", NULL},
    {"vsi", SAL_FIELD_TYPE_INT32, "vsi", "0"},
    {"vlan", SAL_FIELD_TYPE_INT32, "vsi", "0"},
    {"count", SAL_FIELD_TYPE_INT32,
     "Determines the number of entries in the same VLAN that should be deleted. MAC addresses are incremented by 1.",
     "1"},
    {NULL}
};

sh_sand_man_t sh_dnx_l2_info_man = {
    .brief = "L2 information \n",
    .full = "Information for L2 learning and l2 aging\n",
};

sh_sand_man_t sh_dnx_l2_config_man = {
    .brief = "L2 Learning Mode configuration\n",
    .examples = "mode=IngCntrCpu cb lg\n" "mode=IngCntrCpu cb cou\n" "mode=IngDstr"
};

sh_sand_man_t sh_dnx_l2_show_man = {
    .brief = "Show l2 address table",
    .full = "Show the entries from SVL MACT tables",
    .examples = "svl\n",
    .init_cb = diag_sand_packet_send_simple,
    .deinit_cb = sh_deinit_cb_void
};

sh_sand_man_t sh_dnx_l2_clear_man = {
    .brief = "Clear MACT entries",
    .full = "l2 clear entries from MACT by mac_addr/vlan/port/tgid/modid; by combination of parameters or all",
    .examples = "mac=11:22:33:44:55:66 vlan=4\n"
        "port=200\n" "port=200 modid=1\n" "mac=66:55:44:33:22:11 static=true\n" "tgid=2\n" "all\n" "all svl"
};

sh_sand_man_t sh_dnx_l2_add_man = {
    .brief = "Add MACT entry",
    .full = "Add l2 entry to FWD_MACT based on arguments provided",
    .examples =
        "mac=11:22:33:44:55:66 vlan=5 portbitmap=0x200000000000000000000000000000000000000000000000000\n"
        "mac=11:22:33:44:55:77 vlan=1 port=200 static=false\n"
        "mac=22:33:44:55:66:00 vlan=1 portbitmap=cpu\n"
        "mac=00:00:11:22:33:44 vsi=1 port=201\n"
        "mac=11:22:33:44:77:88 vlan=1 tgid=2\n"
        "mac=11:22:33:44:77:88 vsi=1 tgid=2\n"
        "mac=11:22:33:44:77:11 vlan=1 mc_g=100\n"
        "mac=11:22:33:44:55:66 vlan=4 portbitmap=0x200000000000000000000000000000000000000000000000000\n"
        "mac=11:22:33:44:77:00 vsi=1 mc_group=100\n" "mac=11:22:33:44:77:22 vsi=1 mc_group=100"
};

sh_sand_man_t sh_dnx_l2_delete_man = {
    .brief = "Delete MACT entry",
    .full = "Delete l2 entry from FWD_MACT based on arguments provided",
    .examples =
        "mac=11:22:33:44:55:66 vlan=5\n"
        "mac=11:22:33:44:55:77 vlan=1 count=1\n"
        "mac=22:33:44:55:66:00 vlan=1\n"
        "mac=00:00:11:22:33:44 vsi=1\n"
        "mac=11:22:33:44:77:88 vlan=1\n"
        "mac=11:22:33:44:77:11 vsi=1\n" "mac=11:22:33:44:77:00 vlan=1\n" "mac=11:22:33:44:77:22 vsi=1"
};

sh_sand_man_t sh_dnx_l2_count_man = {
    .brief = "Get the number of MACT entries",
    .full = "Get the number of entries from FWD_MACT per given vsi or total"
};

sh_sand_man_t sh_dnx_l2_vsi_man = {
    .brief = "Presents status/modifies properties per VSI",
};

sh_sand_man_t sh_dnx_l2_lif_man = {
    .brief = "Presents status/modifies L2 properties per Global OUTLIF",
};

/**
 * \brief DNX l2 diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 */
sh_sand_cmd_t sh_dnx_l2_cmds[] = {
    /*
     * keyword,     action,    command,           options,                 man
     */
    {"info", sh_dnx_l2_info_cmd, NULL, NULL, &sh_dnx_l2_info_man},
    {"config", sh_dnx_l2_config_cmd, NULL, dnx_l2_config_options, &sh_dnx_l2_config_man},
    {"add", sh_dnx_l2_add_cmd, NULL, dnx_l2_add_options, &sh_dnx_l2_add_man},
    {"show", sh_dnx_l2_show_cmd, NULL, dnx_l2_show_options, &sh_dnx_l2_show_man},
    {"count", sh_dnx_l2_count_cmd, NULL, NULL, &sh_dnx_l2_count_man},
    {"delete", sh_dnx_l2_delete_cmd, NULL, dnx_l2_delete_options, &sh_dnx_l2_delete_man},
    {"clear", sh_dnx_l2_clear_cmd, NULL, dnx_l2_clear_options, &sh_dnx_l2_clear_man},
    {"vsi", sh_dnx_l2_vsi_cmd, NULL, NULL, &sh_dnx_l2_vsi_man},
    {"lif", sh_dnx_l2_lif_cmd, NULL, NULL, &sh_dnx_l2_lif_man},
    {NULL}
};

sh_sand_man_t sh_dnx_l2_man = {
    .brief = "L2 management and diagnostics commands\n",
};

/*
 * }
 */
