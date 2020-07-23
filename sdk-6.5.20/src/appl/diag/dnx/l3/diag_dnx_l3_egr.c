/** \file diag_dnx_l3_egr.c
 *
 * file for l3 egr diagnostics
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
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/mdb.h>
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_l3.h"
/*
 * }
 */
#define FIRST_HIERARCHY  DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1
#define SECOND_HIERARCHY DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2
#define THIRD_HIERARCHY  DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3
#define NO_HIERARCHY     DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY
/*
 * The number of clusters needed for a single FEC allocation
 */
#define DIAG_EGR_FEC_CLUSTERS_WIDTH  2

static sh_sand_enum_t dnx_l3_egr_hierarchy_alloc[] = {
     /**String    Value            Description */
    {"1ST_HIER", FIRST_HIERARCHY, "First hierarchy"},
    {"2ND_HIER", SECOND_HIERARCHY, "Second hierarchy"},
    {"3RD_HIER", THIRD_HIERARCHY, "Third hierarchy"},
    {"NONE", NO_HIERARCHY, "No hierarchy"},
    {NULL}
};

/**
 * Callback function for testing the Egress interface diagnostics commands.
 */
static shr_error_e
dnx_l3_egress_cb(
    int unit)
{
    bcm_l3_egress_t egr_entry;
    uint32 port_id = 201;
    uint32 trunk_id = 100;
    uint32 label = 0xAAAA;
    uint32 encap_id = 0x40001001;
    uint32 fec_id;
    uint32 module_id = 10;
    uint32 flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    int intf_id = 100;
    bcm_mac_t dest_mac = { 0x0, 0x0, 0xc0, 0x0, 0xab, 0xcd };
    int mpls_action = 4;

    SHR_FUNC_INIT_VARS(unit);

    /** fec=60020 ID=100 port=201 encap=0x40001001 */
    bcm_l3_egress_t_init(&egr_entry);
    fec_id = 60020;
    egr_entry.port = port_id;
    egr_entry.encap_id = encap_id;
    egr_entry.intf = intf_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, (int *) &fec_id));

    /** dmac=00:00:c0:00:ab:cd fec=60201 label=0xAAAA mpls_action=4 */
    bcm_l3_egress_t_init(&egr_entry);
    fec_id = 60201;
    sal_memcpy(egr_entry.mac_addr, dest_mac, sizeof(bcm_mac_t));
    egr_entry.mpls_label = label;
    egr_entry.mpls_action = mpls_action;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, (int *) &fec_id));

    /** fec=60021 trunk=100 module=10 hit_clr=No */
    bcm_l3_egress_t_init(&egr_entry);
    fec_id = 60021;
    egr_entry.module = module_id;
    egr_entry.port = port_id;
    egr_entry.trunk = trunk_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, (int *) &fec_id));

exit:
    SHR_FUNC_EXIT;
}

/** Deinit callback function for testing the egress interface diagnostics commands.*/
static shr_error_e
dnx_l3_egress_deinit_cb(
    int unit)
{
    int fec_id;

    SHR_FUNC_INIT_VARS(unit);

    /** fec=60020 ID=100 port=201 encap=0x40001001 */
    fec_id = 60020;
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, fec_id);
    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id));

    /** dmac=00:00:c0:00:ab:cd fec=60201 label=0xAAAA mpls_action=4 */
    fec_id = 60201;
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, fec_id);
    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id));

    /** fec=60021 trunk=100 module=10 hit_clr=No */
    fec_id = 60021;
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, fec_id);
    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t Dnx_l3_egr_add_man = {
    .brief = "Create an L3 egress interface\n",
    .full =
        "Create an L3 interface and set its main properties using its ID as key. Then, read to verify.\n"
        "Select whether or not to clear the 'hit' bit.\n",
    .deinit_cb = dnx_l3_egress_deinit_cb,
    .synopsis = "[fec=<FEC_ID>] [DMAC=<Destination MAC>] [module=<Module ID>] [id=<intf ID>]"
        " [port=<PORT>] [trunk=<Trunk ID>] [label=<Label>] [encap=<Encapsulation ID>] [hit_clr=<Yes | No>]",
    .examples =
        "fec=60020 ID=100 port=201 encap=0x40001001" "\n" "dmac=00:00:c0:00:ab:cd fec=60201 label=0xAAAA mpls_action=4"
        "\n" "fec=60021 trunk=100 module=10 hit_clr=No"
};

static sh_sand_man_t Dnx_l3_egr_delete_man = {
    .brief = "Delete an L3 egress interface\n",
    .full = "Delete an L3 interface providing its ID.\n",
    .init_cb = dnx_l3_egress_cb,
    .synopsis = "[FEC=<FEC ID>]",
    .examples = "FEC=60020"
};

static sh_sand_man_t Dnx_l3_egr_get_man = {
    .brief = "Receive the information regarding an L3 egress interface\n",
    .full =
        "Receive information regarding an L3 interface by providing its ID.\n"
        "Select whether or not to clear the 'hit' bit.\n",
    .init_cb = dnx_l3_egress_cb,
    .deinit_cb = dnx_l3_egress_deinit_cb,
    .synopsis = "FEC=<FEC ID> [hit_clr=<Yes | No>]",
    .examples = "FEC=60020 hit_clr=No"
};

static sh_sand_man_t Dnx_l3_egr_find_man = {
    .brief = "Find an L3 interface pointing to an Egress forwarding object - not supported",
    .full = "Not supported",
};

static sh_sand_man_t Dnx_l3_egr_update_man = {
    .brief = "Update an existing L3 egress interface\n",
    .full =
        "Update an existing L3 interface providing its ID and new properties to be saved. Then, read to verify.\n"
        "Select whether or not to clear the 'hit' bit.\n",
    .init_cb = dnx_l3_egress_cb,
    .deinit_cb = dnx_l3_egress_deinit_cb,
    .synopsis = "fec=<FEC_ID> [ID=<Intf ID>] [smac=<MAC>] [vrf=<VRF>] [ttl=<TTL>] [mtu=<MTU>]",
    .examples =
        "FEC=60020 ID=100 port=202 encap=0x40001002" "\n" "FEC=60201 dmac=00:12:34:56:78:9a label=0xBBBB mpls_action=4"
        "\n" "fec=60021 trunk=105 module=9 hit_clr=No"
};

static sh_sand_man_t Dnx_l3_egr_allocation_man = {
    .brief = "Show available FEC allocation per hierarchy",
    .full = "List the available Macro allocation for the current MDB profile for a given hierarchy",
    .synopsis = "[hierarchy=<hier_level>]",
    .examples = "hierarchy=1ST_HIER advanced=Yes"
};

static sh_sand_option_t Dnx_l3_egr_add_options[] = {
    {"ID", SAL_FIELD_TYPE_INT32, "Intf ID", "0"},
    {"DMAC", SAL_FIELD_TYPE_MAC, "Destination MAC", "00:00:00:00:00:00"},
    {"module", SAL_FIELD_TYPE_UINT32, "Module ID", "0"},
    {"port", SAL_FIELD_TYPE_UINT32, "Port", "0"},
    {"trunk", SAL_FIELD_TYPE_UINT32, "Trunk ID", "0"},
    {"label", SAL_FIELD_TYPE_UINT32, "Label", "0xffffffff"},
    {"mpls_action", SAL_FIELD_TYPE_UINT32, "MPLS_ACTION", "0"},
    {"encap", SAL_FIELD_TYPE_UINT32, "Encapsulation ID", "0"},
    {"fec", SAL_FIELD_TYPE_UINT32, "FEC", NULL},
    {"hit_clr", SAL_FIELD_TYPE_BOOL, "If selected, will clear the 'hit bit'", "No"},
    {NULL}
};

static sh_sand_option_t Dnx_l3_egr_delete_options[] = {
    {"FEC", SAL_FIELD_TYPE_INT32, "FEC ID", NULL},
    {NULL}
};

static sh_sand_option_t Dnx_l3_egr_get_options[] = {
    {"FEC", SAL_FIELD_TYPE_INT32, "FEC ID", NULL},
    {"hit_clr", SAL_FIELD_TYPE_BOOL, "If selected, will clear the 'hit bit'", "No"},
    {NULL}
};

static sh_sand_option_t Dnx_l3_egr_find_options[] = {
    {NULL}
};

static sh_sand_option_t Dnx_l3_egr_allocation_options[] = {
    {"HieRarChy", SAL_FIELD_TYPE_ENUM, "Hierarchy", "NONE", (void *) dnx_l3_egr_hierarchy_alloc},
    {"ADVanced", SAL_FIELD_TYPE_BOOL, "Advanced mode", "No"},
    {NULL}
};

/**
 * \brief
 *    A function that prints the accessed egress interface entry.
 *    It is valid for add/delete/get/update commands.
 * \param [in] unit - unit ID
 * \param [in] egr_info - The structure that uniquely describes the accessed entry
 * \param [in] intf_id - The FEC ID of the entry
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
dnx_l3_egr_entry_print(
    int unit,
    bcm_l3_egress_t * egr_info,
    bcm_if_t intf_id,
    sh_sand_control_t * sand_control,
    char *action)
{
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Egress intf entry %s", action);

    PRT_COLUMN_ADD("FEC ID");
    PRT_COLUMN_ADD("Intf ID");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Destination MAC");
    PRT_COLUMN_ADD("Encap ID");
    PRT_COLUMN_ADD("Label");
    PRT_COLUMN_ADD("Trunk");
    PRT_COLUMN_ADD("Module ID");
    PRT_COLUMN_ADD("Hit");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d (0x%08X)", intf_id, intf_id);
    PRT_CELL_SET("0x%04X", egr_info->intf);
    PRT_CELL_SET("0x%04X", egr_info->port);
    sal_sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", egr_info->mac_addr[0], egr_info->mac_addr[1],
                egr_info->mac_addr[2], egr_info->mac_addr[3], egr_info->mac_addr[4], egr_info->mac_addr[5]);
    PRT_CELL_SET("%s", buffer);
    PRT_CELL_SET("0x%04X", egr_info->encap_id);
    PRT_CELL_SET("0x%04X", egr_info->mpls_label);
    PRT_CELL_SET("0x%04X", egr_info->trunk);
    PRT_CELL_SET("0x%04X", egr_info->module);
    PRT_CELL_SET("%s", ((egr_info->flags & BCM_L3_HIT) ? "YES" : "NO "));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that creates an L3 egress entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_egr_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_t egr_entry;
    uint32 port_id;
    uint32 trunk_id;
    uint32 label;
    uint32 encap_id;
    uint32 fec_id;
    uint32 module_id;
    uint32 flags = BCM_L3_INGRESS_ONLY;
    int intf_id;
    bcm_mac_t dest_mac;
    int hit_clear;
    int mpls_action;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("label", label);
    SH_SAND_GET_UINT32("mpls_action", mpls_action);
    SH_SAND_GET_UINT32("encap", encap_id);
    SH_SAND_GET_UINT32("fec", fec_id);
    SH_SAND_GET_UINT32("trunk", trunk_id);
    SH_SAND_GET_UINT32("port", port_id);
    SH_SAND_GET_UINT32("module", module_id);
    SH_SAND_GET_INT32("ID", intf_id);
    SH_SAND_GET_MAC("DMAC", dest_mac);
    SH_SAND_GET_BOOL("hit_clr", hit_clear);

    bcm_l3_egress_t_init(&egr_entry);
    sal_memcpy(egr_entry.mac_addr, dest_mac, sizeof(bcm_mac_t));
    egr_entry.module = module_id;
    egr_entry.port = port_id;
    egr_entry.trunk = trunk_id;
    egr_entry.encap_id = encap_id;
    egr_entry.mpls_label = label;
    egr_entry.mpls_action = mpls_action;
    egr_entry.intf = intf_id;
    if (fec_id != -1)
    {
        flags |= BCM_L3_WITH_ID;
    }
    else
    {
        fec_id = 0;
    }
    /*
     * Set flag to indicate whether user required 'clear' operation on the 'hit bit'
     */
    if (hit_clear)
    {
        egr_entry.flags2 &= ~BCM_L3_FLAGS2_SKIP_HIT_CLEAR;
    }
    else
    {
        egr_entry.flags2 |= BCM_L3_FLAGS2_SKIP_HIT_CLEAR;
    }
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, (int *) &fec_id));
    bcm_l3_egress_t_init(&egr_entry);
    /*
     * Note: By default, we do not apply 'clear hit bit' on this 'get'
     */
    SHR_IF_ERR_EXIT(bcm_l3_egress_get(unit, fec_id, &egr_entry));
    SHR_IF_ERR_EXIT(dnx_l3_egr_entry_print(unit, &egr_entry, fec_id, sand_control, "add"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that deletes an L3 egress entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_egr_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_t egr_entry;
    int fec_id;
    SHR_FUNC_INIT_VARS(unit);
    bcm_l3_egress_t_init(&egr_entry);
    SH_SAND_GET_INT32("FEC", fec_id);
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(fec_id));
    /*
     * Note: By default, we do not apply 'clear hit bit' on this 'get'
     */
    SHR_IF_ERR_EXIT(bcm_l3_egress_get(unit, fec_id, &egr_entry));
    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id));
    SHR_IF_ERR_EXIT(dnx_l3_egr_entry_print(unit, &egr_entry, fec_id, sand_control, "delete"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that receives the data for an L3 egress entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_egr_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_t egr_entry;
    int fec_id;
    int hit_clear;

    SHR_FUNC_INIT_VARS(unit);
    bcm_l3_egress_t_init(&egr_entry);
    SH_SAND_GET_INT32("FEC", fec_id);
    SH_SAND_GET_BOOL("hit_clr", hit_clear);
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(fec_id));
    /*
     * Set flag to indicate user required 'clear' operation on the 'hit bit'
     */
    if (hit_clear)
    {
        egr_entry.flags |= BCM_L3_HIT_CLEAR;
    }
    SHR_IF_ERR_EXIT(bcm_l3_egress_get(unit, fec_id, &egr_entry));
    SHR_IF_ERR_EXIT(dnx_l3_egr_entry_print(unit, &egr_entry, fec_id, sand_control, "get"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that receives the data for an L3 egress entry based on MAC.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_egr_find_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Layer3 Egr Find diagnostics is not supported, yet.");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that updates an L3 egress entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_egr_update_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_t egr_entry;
    uint32 trunk_id;
    uint32 label;
    uint32 encap_id;
    uint32 fec_id;
    uint32 module_id;
    uint32 port_id;
    uint32 flags = BCM_L3_REPLACE | BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY;
    int intf_id;
    uint8 is_present;
    bcm_mac_t dest_mac;
    int hit_clear;
    int mpls_action;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("label", label);
    SH_SAND_GET_UINT32("encap", encap_id);
    SH_SAND_GET_UINT32("fec", fec_id);
    SH_SAND_GET_UINT32("trunk", trunk_id);
    SH_SAND_GET_UINT32("port", port_id);
    SH_SAND_GET_UINT32("module", module_id);
    SH_SAND_GET_INT32("ID", intf_id);
    SH_SAND_GET_MAC("DMAC", dest_mac);
    SH_SAND_GET_BOOL("hit_clr", hit_clear);
    SH_SAND_GET_UINT32("mpls_action", mpls_action);

    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(fec_id));
    bcm_l3_egress_t_init(&egr_entry);
    /*
     * Note that, by default, we do not clear hit-bit on 'get'
     */
    SHR_IF_ERR_EXIT(bcm_l3_egress_get(unit, fec_id, &egr_entry));

    /** Assign the values of the provided options to the appropriate field in the structure.*/
    SH_SAND_IS_PRESENT("DMAC", is_present);
    if (is_present)
    {
        sal_memcpy(egr_entry.mac_addr, dest_mac, sizeof(bcm_mac_t));
    }
    SH_SAND_IS_PRESENT("module", is_present);
    if (is_present)
    {
        egr_entry.module = module_id;
    }
    SH_SAND_IS_PRESENT("port", is_present);
    if (is_present)
    {
        egr_entry.port = port_id;
    }
    SH_SAND_IS_PRESENT("trunk", is_present);
    if (is_present)
    {
        egr_entry.trunk = trunk_id;
    }
    SH_SAND_IS_PRESENT("encap", is_present);
    if (is_present)
    {
        egr_entry.encap_id = encap_id;
    }
    SH_SAND_IS_PRESENT("label", is_present);
    if (is_present)
    {
        egr_entry.mpls_label = label;
    }
    SH_SAND_IS_PRESENT("ID", is_present);
    if (is_present)
    {
        egr_entry.intf = intf_id;
    }
    SH_SAND_IS_PRESENT("mpls_action", is_present);
    if (is_present)
    {
        egr_entry.mpls_action = mpls_action;
    }
    /*
     * Set flag to indicate whether user required 'clear' operation on the 'hit bit'
     */
    if (hit_clear)
    {
        egr_entry.flags2 &= ~BCM_L3_FLAGS2_SKIP_HIT_CLEAR;
    }
    else
    {
        egr_entry.flags2 |= BCM_L3_FLAGS2_SKIP_HIT_CLEAR;
    }
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, (int *) &fec_id));
    bcm_l3_egress_t_init(&egr_entry);
    /*
     * Note: By default, we do not apply 'clear hit bit' on this 'get'
     */
    SHR_IF_ERR_EXIT(bcm_l3_egress_get(unit, fec_id, &egr_entry));
    SHR_IF_ERR_EXIT(dnx_l3_egr_entry_print(unit, &egr_entry, fec_id, sand_control, "update"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Return information about the NOF elements in a FEC bank.
 * \param [in] unit - unit ID
 * \param [in] bank_id - The FEC bank ID
 * \param [in] nof_elements - Total NOF elements in the FEC bank
 * \param [in] nof_free_elemens - Total NOF free elements in the FEC bank
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_egr_get_elements_in_fec_bank(
    int unit,
    int bank_id,
    int *nof_elements,
    int *nof_free_elemens)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                    fec_res_manager.nof_free_elements_get(unit, bank_id, nof_free_elemens));

    *nof_elements = MDB_CALC_FEC_BANK_SIZE(unit, bank_id);

    *nof_free_elemens -= dnx_data_l3.fec.bank_size_round_up_get(unit) - (*nof_elements);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints the MDB cluster allocation per hierarchy
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_egr_allocation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 hierarchy;
    uint32 bank_id;
    int start_id = 0;
    int end_id = 3;
    int idx, cl_id, nof_banks;
    int nof_macro_a_type = 0;
    int nof_macro_b_type = 0;
    int nof_free_elements;
    int cluster_start_a[MDB_MAX_NOF_CLUSTERS];
    int cluster_start_b[MDB_MAX_NOF_CLUSTERS];
    mdb_cluster_alloc_info_t *clusters;
    int hier_id[3] = { 1, 2, 3 };
    int nof_clusters = 0;
    int addr_id;
    int nof_elements_per_bank;
    uint8 advanced_mode;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    clusters = sal_alloc((sizeof(mdb_cluster_alloc_info_t) * MDB_MAX_NOF_CLUSTERS), "Cluster structure mem alloc");
    SH_SAND_GET_ENUM("hierarchy", hierarchy);
    SH_SAND_GET_BOOL("ADVANCED", advanced_mode);
    switch (hierarchy)
    {
        case FIRST_HIERARCHY:
            start_id = FIRST_HIERARCHY;
            end_id = SECOND_HIERARCHY;
            break;
        case SECOND_HIERARCHY:
            start_id = SECOND_HIERARCHY;
            end_id = THIRD_HIERARCHY;
            break;
        case THIRD_HIERARCHY:
            start_id = THIRD_HIERARCHY;
            end_id = THIRD_HIERARCHY + 1;
            break;
        default:
            start_id = FIRST_HIERARCHY;
            end_id = THIRD_HIERARCHY + 1;
    }

    PRT_TITLE_SET("FEC MDB cluster allocation");
    PRT_COLUMN_ADD("Hierarchy ID");
    if (advanced_mode == TRUE)
    {
        PRT_COLUMN_ADD("MACRO_A type");
        PRT_COLUMN_ADD("MACRO_B type");
    }
    else
    {
        PRT_COLUMN_ADD("Range allocation");
    }
    for (hierarchy = start_id; hierarchy < end_id; hierarchy++)
    {
        addr_id = 0;
        nof_macro_a_type = 0;
        nof_macro_b_type = 0;
        SHR_IF_ERR_EXIT(algo_l3_fec_mdb_table_resources_get(unit, hierarchy, &nof_clusters, clusters));
        for (idx = 0; idx < nof_clusters; idx += 2)
        {
            cluster_start_a[idx] = -1;
            cluster_start_b[idx] = -1;
            if (clusters[idx].macro_type == MDB_MACRO_A)
            {
                nof_macro_a_type++;
            }
            if (clusters[idx].macro_type == MDB_MACRO_B)
            {
                nof_macro_b_type++;
            }

            bank_id =
                DNX_ALGO_L3_PHYSICAL_FEC_GET(unit,
                                             clusters[idx].logical_start_address * DIAG_EGR_FEC_CLUSTERS_WIDTH) /
                dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);

            /*
             * MACRO-A cluster can hold 2 banks of FECs; MACRO-B cluster can hold a single FEC of banks;
             * Go over the used banks and check if the sub-resource
             * to which the FECs are allocated is free.
             */
            nof_banks = MDB_MACRO_NOF_FEC_BANKS_GET(clusters[idx].macro_type);
            for (cl_id = 0; cl_id < nof_banks; cl_id++)
            {
                uint32 temp_hierarchy;

                SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.
                                fec_hierarchy_map.get(unit, bank_id + cl_id, &temp_hierarchy));

                /** If the sub resource is not taken by this hierarchy it means that the FEC cluster is free */
                if (temp_hierarchy == hierarchy)
                {
                    addr_id = 1;
                }
            }

            if (addr_id != 0)
            {
                if (clusters[idx].macro_type == MDB_MACRO_A)
                {
                    cluster_start_a[idx] = clusters[idx].logical_start_address;
                }
                if (clusters[idx].macro_type == MDB_MACRO_B)
                {
                    cluster_start_b[idx] = clusters[idx].logical_start_address;
                }
            }
        }
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        if (advanced_mode == TRUE)
        {
            PRT_CELL_SET("%d", hier_id[hierarchy]);
            PRT_CELL_SET("%d", nof_macro_a_type);
            PRT_CELL_SET("%d", nof_macro_b_type);
            /** If addr_id is 0, then all clusters assigned to this hierarchy are empty */
            if (addr_id != 0)
            {
                for (idx = 0; idx < nof_clusters; idx++)
                {
                    if (cluster_start_a[idx] != -1 || cluster_start_b[idx] != -1)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(1);

                        bank_id =
                            DNX_ALGO_L3_PHYSICAL_FEC_GET(unit,
                                                         clusters[idx].logical_start_address *
                                                         DIAG_EGR_FEC_CLUSTERS_WIDTH) /
                            dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);

                        SHR_IF_ERR_EXIT(dnx_l3_egr_get_elements_in_fec_bank
                                        (unit, bank_id, &nof_elements_per_bank, &nof_free_elements));

                        if (cluster_start_a[idx] != -1)
                        {
                            int nof_free_elements_in_pair;
                            int nof_elements_per_bank_pair;

                            SHR_IF_ERR_EXIT(dnx_l3_egr_get_elements_in_fec_bank
                                            (unit, bank_id + 1, &nof_elements_per_bank_pair,
                                             &nof_free_elements_in_pair));

                            nof_elements_per_bank += nof_elements_per_bank_pair;

                            nof_free_elements += nof_free_elements_in_pair;

                            PRT_CELL_SET("Range %d to %d (%d free)",
                                         clusters[idx].logical_start_address * DIAG_EGR_FEC_CLUSTERS_WIDTH,
                                         clusters[idx].logical_start_address * DIAG_EGR_FEC_CLUSTERS_WIDTH +
                                         nof_elements_per_bank - 1, nof_free_elements);
                            PRT_CELL_SKIP(1);
                        }
                        else
                        {
                            PRT_CELL_SKIP(1);
                            PRT_CELL_SET("Range %d to %d (%d free)",
                                         clusters[idx].logical_start_address * DIAG_EGR_FEC_CLUSTERS_WIDTH,
                                         clusters[idx].logical_start_address * DIAG_EGR_FEC_CLUSTERS_WIDTH +
                                         nof_elements_per_bank - 1, nof_free_elements);
                        }
                    }
                    idx++;
                }
            }
        }
        else
        {
            uint32 fec_start_id;
            uint32 total_nof_free_elements = 0;
            bcm_switch_fec_property_config_t fec_config;
            PRT_CELL_SET("%d", hier_id[hierarchy]);

            if (hierarchy == 0)
            {
                fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
            }
            else if (hierarchy == 1)
            {
                fec_config.flags = BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY;
            }
            else
            {
                fec_config.flags = BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY;
            }

            SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
            PRT_CELL_SET("Range %d to %d (%d FECs)", fec_config.start, fec_config.end,
                         fec_config.end - fec_config.start + 1);
            fec_start_id = fec_config.start;
            while (fec_start_id < fec_config.end)
            {
                bank_id =
                    DNX_ALGO_L3_PHYSICAL_FEC_GET(unit,
                                                 fec_start_id) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
                SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                                fec_res_manager.nof_free_elements_get(unit, bank_id, &nof_free_elements));

                nof_elements_per_bank = MDB_CALC_FEC_BANK_SIZE(unit, bank_id);

                nof_free_elements -= dnx_data_l3.fec.bank_size_round_up_get(unit) - nof_elements_per_bank;

                total_nof_free_elements += nof_free_elements;
                fec_start_id += nof_elements_per_bank;
            }
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("%d allocated", (fec_config.end - fec_config.start + 1) - total_nof_free_elements);
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("%d free", total_nof_free_elements);
        }
    }

    PRT_COMMITX;
exit:
    sal_free(clusters);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_l3_egr_cmds[] = {

    /*
     * keyword, action, command, options, man
     */
    {
     "add", sh_dnx_l3_egr_add_cmd, NULL, Dnx_l3_egr_add_options, &Dnx_l3_egr_add_man}
    ,
    {
     "get", sh_dnx_l3_egr_get_cmd, NULL, Dnx_l3_egr_get_options, &Dnx_l3_egr_get_man}
    ,
    {
     "find", sh_dnx_l3_egr_find_cmd, NULL, Dnx_l3_egr_find_options, &Dnx_l3_egr_find_man, NULL, NULL, SH_CMD_SKIP_EXEC}
    ,
    {
     "update", sh_dnx_l3_egr_update_cmd, NULL, Dnx_l3_egr_add_options, &Dnx_l3_egr_update_man}
    ,
    {
     "delete", sh_dnx_l3_egr_delete_cmd, NULL, Dnx_l3_egr_delete_options, &Dnx_l3_egr_delete_man}
    ,
    {
     "allocation", sh_dnx_l3_egr_allocation_cmd, NULL, Dnx_l3_egr_allocation_options, &Dnx_l3_egr_allocation_man}
    ,
    {
     NULL}
};
