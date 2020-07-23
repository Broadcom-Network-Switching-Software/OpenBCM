/** \file diag_dnx_l3_intf.c
 *
 * file for l3 intf diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */

/** bcm */
#include <bcm/l3.h>
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_l3.h"
/*
 * }
 */

/**
 * Callback function for testing the interface diagnostics commands.
 */
static shr_error_e
dnx_l3_intf_cb(
    int unit)
{
    bcm_l3_intf_t intf_entry;
    uint32 vrf = 1;
    uint32 itf_id = 200;
    bcm_mac_t source_mac = { 0x0, 0x5, 0x4, 0x3, 0x2, 0x1 };

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_vrf = vrf;
    intf_entry.l3a_intf_id = itf_id;
    sal_memcpy(intf_entry.l3a_mac_addr, source_mac, sizeof(bcm_mac_t));
    intf_entry.l3a_vid = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &intf_entry));

    itf_id = 250;
    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    sal_memcpy(intf_entry.l3a_mac_addr, source_mac, sizeof(bcm_mac_t));
    intf_entry.l3a_vid = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &intf_entry));

    itf_id = 100;
    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    sal_memcpy(intf_entry.l3a_mac_addr, source_mac, sizeof(bcm_mac_t));
    intf_entry.l3a_vid = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &intf_entry));
exit:
    SHR_FUNC_EXIT;
}

/** Deinit callback function for testing the interface diagnostics commands.*/
static shr_error_e
dnx_l3_intf_deinit_cb(
    int unit)
{
    bcm_l3_intf_t intf_entry;
    uint32 itf_id = 200;

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    intf_entry.l3a_vid = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete(unit, &intf_entry));

    itf_id = 250;
    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    intf_entry.l3a_vid = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete(unit, &intf_entry));

    itf_id = 100;
    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    intf_entry.l3a_vid = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete(unit, &intf_entry));
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_l3_intf_add_man = {
    .brief = "Create an L3 interface\n",
    .full = "Create an L3 interface and set its main properties using its ID as key\n",
    .deinit_cb = dnx_l3_intf_deinit_cb,
    .synopsis = "[SMAC=<Source MAC>], [vid=<VID>], [vrf=<VRF>], [id=<intf ID>], [ttl=<TTL>], [mtu=<MTU>]",
    .examples =
        "id=20 vid=100 smac=00:00:c0:00:ab:ab" "\n" "smac=00:00:c0:00:ab:cd vid=200 ttl=100" "\n"
        "smac=00:00:c0:00:ab:cd vid=250 vrf=10 mtu=20"
};

static sh_sand_man_t dnx_l3_intf_delete_man = {
    .brief = "Delete an L3 interface\n",
    .full = "Delete an L3 interface providing its ID.\n",
    .init_cb = dnx_l3_intf_cb,
    .synopsis = "[id=<intf ID>]",
    .examples = "id=200" "\n" "id=250" "\n" "id=100"
};

static sh_sand_man_t dnx_l3_intf_get_man = {
    .brief = "Receive the information regarding an L3 interface\n",
    .full = "Receive information regarding an L3 interface by providing its ID.\n",
    .init_cb = dnx_l3_intf_cb,
    .deinit_cb = dnx_l3_intf_deinit_cb,
    .synopsis = "id=<Intf ID>",
    .examples = "id=200"
};

static sh_sand_man_t dnx_l3_intf_find_man = {
    .brief = "Find an L3 intf ID based on its SMAC and VLAN ID - not supported",
    .full = "Not supported",
    .synopsis = "smac=<SMAC> vid=<VID> [vrf=<VRF>]",
    .examples = "smac=00:00:00:12:34:56 vid=100" "\n" "smac=00:00:00:12:34:56 vid=250 vrf=10"
};

static sh_sand_man_t dnx_l3_intf_update_man = {
    .brief = "Update an existing L3 interface\n",
    .full = "Update an existing L3 interface providing its ID and new properties to be saved.\n",
    .init_cb = dnx_l3_intf_cb,
    .deinit_cb = dnx_l3_intf_deinit_cb,
    .synopsis = "id=<intf ID>, [vid=<VID>], [smac=<MAC>], [vrf=<VRF>], [ttl=<TTL>], [mtu=<MTU>]",
    .examples = "id=200 smac=00:12:34:56:78:9a" "\n" "id=250 smac=00:00:c0:00:ab:cd vid=250 ttl=100 mtu=50 vrf=10"
};

static sh_sand_option_t dnx_l3_intf_add_options[] = {
    {"vid", SAL_FIELD_TYPE_UINT32, "VID", "0"},
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"SMAC", SAL_FIELD_TYPE_MAC, "Source MAC", NULL},
    {"TTL", SAL_FIELD_TYPE_UINT32, "TTL", "0"},
    {"ID", SAL_FIELD_TYPE_UINT32, "Intf ID", "0"},
    {"MTU", SAL_FIELD_TYPE_UINT32, "MTU", "0"},
    {NULL}
};

static sh_sand_option_t dnx_l3_intf_delete_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "Intf ID", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_intf_get_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "Intf ID", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_intf_find_options[] = {
    {"vid", SAL_FIELD_TYPE_UINT32, "VID", "0"},
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"SMAC", SAL_FIELD_TYPE_MAC, "Source MAC", "00:00:00:00:00:00"},
    {NULL}
};

static sh_sand_option_t dnx_l3_intf_update_options[] = {
    {"vid", SAL_FIELD_TYPE_UINT32, "VID", "0"},
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"SMAC", SAL_FIELD_TYPE_MAC, "Source MAC", "00:00:00:00:00:00"},
    {"TTL", SAL_FIELD_TYPE_UINT32, "TTL", "0"},
    {"ID", SAL_FIELD_TYPE_UINT32, "Intf ID", NULL},
    {"MTU", SAL_FIELD_TYPE_UINT32, "MTU", "0"},
    {NULL}
};

/**
 * \brief
 *    A function that prints the accessed L3 interface entry.
 *    It is valid for add/delete/get/update commands.
 * \param [in] unit - unit ID
 * \param [in] intf_info - The structure that uniquely describes the accessed entry
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] action - string indicatig what the action to the ECMP group was:
 *          * add
 *          * delete
 *          * destroy
 *          * get
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_intf_entry_print(
    int unit,
    bcm_l3_intf_t * intf_info,
    sh_sand_control_t * sand_control,
    char *action)
{
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Intf entry %s", action);

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("VRF");
    PRT_COLUMN_ADD("VID");
    PRT_COLUMN_ADD("Source MAC");
    PRT_COLUMN_ADD("TTL");
    PRT_COLUMN_ADD("MTU");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", intf_info->l3a_intf_id);
    PRT_CELL_SET("0x%04x", intf_info->l3a_vrf);
    PRT_CELL_SET("0x%04x", intf_info->l3a_vid);
    sal_sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", intf_info->l3a_mac_addr[0], intf_info->l3a_mac_addr[1],
                intf_info->l3a_mac_addr[2], intf_info->l3a_mac_addr[3], intf_info->l3a_mac_addr[4],
                intf_info->l3a_mac_addr[5]);
    PRT_CELL_SET("%s", buffer);
    PRT_CELL_SET("0x%04x", intf_info->l3a_ttl);
    PRT_CELL_SET("0x%04x", intf_info->l3a_mtu);

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that creates an L3 interface entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_intf_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_intf_t intf_entry;
    uint32 vrf;
    uint32 vlan_id;
    uint32 ttl;
    uint32 mtu;
    uint32 itf_id;
    bcm_mac_t source_mac;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_UINT32("VID", vlan_id);
    SH_SAND_GET_UINT32("TTL", ttl);
    SH_SAND_GET_UINT32("MTU", mtu);
    SH_SAND_GET_UINT32("ID", itf_id);
    SH_SAND_GET_MAC("SMAC", source_mac);

    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_vrf = vrf;
    intf_entry.l3a_intf_id = itf_id;
    sal_memcpy(intf_entry.l3a_mac_addr, source_mac, sizeof(bcm_mac_t));
    intf_entry.l3a_vid = vlan_id;
    intf_entry.l3a_ttl = ttl;
    intf_entry.l3a_mtu = mtu;

    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &intf_entry));
    SHR_IF_ERR_EXIT(bcm_l3_intf_get(unit, &intf_entry));
    SHR_IF_ERR_EXIT(dnx_l3_intf_entry_print(unit, &intf_entry, sand_control, "add"));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that deletes an L3 interface entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_intf_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_intf_t intf_entry;
    uint32 itf_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", itf_id);

    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_get(unit, &intf_entry));
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete(unit, &intf_entry));

    SHR_IF_ERR_EXIT(dnx_l3_intf_entry_print(unit, &intf_entry, sand_control, "delete"));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that receives the data for an L3 interface entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_intf_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_intf_t intf_entry;
    uint32 itf_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", itf_id);

    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_get(unit, &intf_entry));

    SHR_IF_ERR_EXIT(dnx_l3_intf_entry_print(unit, &intf_entry, sand_control, "get"));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that finds the data for an L3 interface entry based on VID and SMAC.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_intf_find_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The L3 intf find diagnotics is not supported, yet.");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that updates an existing L3 interface entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_intf_update_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_intf_t intf_entry;
    uint32 vrf;
    uint32 vlan_id;
    uint32 ttl;
    uint32 mtu;
    uint32 itf_id;
    int is_present;
    bcm_mac_t source_mac;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_UINT32("VID", vlan_id);
    SH_SAND_GET_UINT32("TTL", ttl);
    SH_SAND_GET_UINT32("MTU", mtu);
    SH_SAND_GET_UINT32("ID", itf_id);
    SH_SAND_GET_MAC("SMAC", source_mac);

    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_get(unit, &intf_entry));

    /** Assign the values of the provided options to the appropriate field in the structure.*/
    SH_SAND_IS_PRESENT("vrf", is_present);
    if (is_present)
    {
        intf_entry.l3a_vrf = vrf;
    }
    SH_SAND_IS_PRESENT("VID", is_present);
    if (is_present)
    {
        intf_entry.l3a_vid = vlan_id;
    }
    SH_SAND_IS_PRESENT("TTL", is_present);
    if (is_present)
    {
        intf_entry.l3a_ttl = ttl;
    }
    SH_SAND_IS_PRESENT("MTU", is_present);
    if (is_present)
    {
        intf_entry.l3a_mtu = mtu;
    }
    SH_SAND_IS_PRESENT("SMAC", is_present);
    if (is_present)
    {
        sal_memcpy(intf_entry.l3a_mac_addr, source_mac, sizeof(bcm_mac_t));
    }

    intf_entry.l3a_flags = BCM_L3_REPLACE | BCM_L3_WITH_ID;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &intf_entry));
    bcm_l3_intf_t_init(&intf_entry);
    intf_entry.l3a_intf_id = itf_id;
    SHR_IF_ERR_EXIT(bcm_l3_intf_get(unit, &intf_entry));
    SHR_IF_ERR_EXIT(dnx_l3_intf_entry_print(unit, &intf_entry, sand_control, "update"));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_l3_intf_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"add", sh_dnx_l3_intf_add_cmd, NULL, dnx_l3_intf_add_options, &dnx_l3_intf_add_man}
    ,
    {"get", sh_dnx_l3_intf_get_cmd, NULL, dnx_l3_intf_get_options, &dnx_l3_intf_get_man}
    ,
    {"find", sh_dnx_l3_intf_find_cmd, NULL, dnx_l3_intf_find_options, &dnx_l3_intf_find_man, NULL, NULL,
     SH_CMD_SKIP_EXEC}
    ,
    {"update", sh_dnx_l3_intf_update_cmd, NULL, dnx_l3_intf_update_options, &dnx_l3_intf_update_man}
    ,
    {"delete", sh_dnx_l3_intf_delete_cmd, NULL, dnx_l3_intf_delete_options, &dnx_l3_intf_delete_man}
    ,
    {NULL}
};
