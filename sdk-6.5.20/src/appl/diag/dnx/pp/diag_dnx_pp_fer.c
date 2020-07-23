/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_fer.c
 *
 * FEC PP Diagnostic Command - Retrieves either all FECs in the system,
 * or FECs that were hit in last packet traverse.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include "diag_dnx_pp.h"
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <bcm_int/dnx/instru/instru_visibility.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/switch/switch_load_balancing.h>
/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

/** this array corresponds to the SUPER_FEC result type and describes which gerenral FEC fields is used */
dbal_fields_e super_fec_entry_type_dbal_fields[] =
    { DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_NO_STATS, DBAL_FIELD_FEC_ENTRY_W_PROTECTION_W_1_STATS,
    DBAL_FIELD_FEC_ENTRY_W_PROTECTION_NO_STATS, DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_W_2_STATS
};

#define DIAG_DNX_PP_VIS_FER_NOF_COLUMNS  12
/*
 * Row size in the members table
 */
#define L3_ECMP_MEMBER_TABLE_ROW_WIDTH                 32

/*************
 *  MACROS  *
 */

#define L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(sup_fec_id, instance) ((sup_fec_id << 1) | instance)

#define FEC_TABLE_FROM_HIER_GET(hierarchy) (( hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3) ? DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY : \
        (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2) ? DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY : DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY)

#define L3_EGRESS_GET_MC_RPF_TYPE(flags) ((flags & BCM_L3_MC_RPF_EXPLICIT) == BCM_L3_MC_RPF_EXPLICIT) ? DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT : \
        ((flags & BCM_L3_MC_RPF_EXPLICIT_ECMP) == BCM_L3_MC_RPF_EXPLICIT_ECMP) ? DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT_ECMP : \
        ((flags & BCM_L3_MC_NO_RPF) == BCM_L3_MC_NO_RPF) ? DBAL_ENUM_FVAL_MC_RPF_MODE_DISABLE :\
        ((flags & BCM_L3_MC_RPF_SIP_BASE) == BCM_L3_MC_RPF_SIP_BASE) ? DBAL_ENUM_FVAL_MC_RPF_MODE_USE_SIP : DBAL_ENUM_FVAL_MC_RPF_MODE_RESERVED

#define FEC_NOF_HIERARCHY  DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY
/*************
 * TYPEDEFS  *
 */

/** \brief List of Table types per FEC hierarchy */
typedef enum
{
    NO_PROTECTION_TABLE,
    W_PROTECTION_TABLE,
    NOF_FEC_TABLE_TYPES
} table_ids_t;

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */

static shr_error_e
diag_dnx_pp_vis_fer_config_cb(
    int unit)
{
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    int in_port = 200;
    int out_port = 201;
    int vlan = 100;
    int fec_id;
    int encap_id = 0x1234;
    bcm_mac_t my_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    char *my_mac_str = "00:0c:00:02:00:00";
    char *sa_mac_str = "00:00:07:00:01:00";
    bcm_mac_t next_hop = { 0x00, 0x0c, 0x00, 0x05, 0x00, 0x00 };
    rhhandle_t packet_h = NULL;
    bcm_l2_addr_t l2addr;
    bcm_gport_t gport;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_switch_fec_property_config_t fec_config;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
    fec_id = fec_config.start;
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, in_port, bcmPortClassId, vlan));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, out_port, bcmPortClassId, vlan));

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);
    BCM_PBMP_PORT_ADD(pbmp, out_port);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    bcm_vlan_create(unit, vlan);
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.match_vlan = vlan;
    vlan_port.vsi = vlan;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vlan, in_port, 0));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, 0, in_port, 0));
    SHR_IF_ERR_EXIT(bcm_vswitch_port_add(unit, vlan, vlan_port.vlan_port_id));

    bcm_vlan_port_t_init(&vlan_port);
    bcm_port_match_info_t_init(&match_info);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = out_port;
    SHR_IF_ERR_EXIT(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info));

    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, next_hop, sizeof(bcm_mac_t));
    l3eg.encap_id = encap_id;
    l3eg.vlan = vlan;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null));

    encap_id = l3eg.encap_id;
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = encap_id;
    l3eg.port = out_port;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &fec_id));

    BCM_GPORT_FORWARD_PORT_SET(gport, fec_id);
    bcm_l2_addr_t_init(&l2addr, my_mac, vlan);
    l2addr.port = gport;
    l2addr.flags = BCM_L2_STATIC;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    bcm_l2_addr_t_init(&l2addr, my_mac, 1);
    l2addr.port = gport;
    l2addr.flags = BCM_L2_STATIC;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", my_mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", sa_mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", (uint32 *) &vlan, 12));

    /*
     * Send packet twice, to avoid learning signals
     * Use RESUME flag - to clean signals before sending the packet
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_h, SAND_PACKET_RX | SAND_PACKET_RESUME));
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_h, SAND_PACKET_RX | SAND_PACKET_RESUME));

exit:
    sal_free(packet_h);
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_vis_fer_man = {
    .brief = "Last packet hierarchical data",
    .full = "Present information for each hierarchy on which FECs and which ECMP groups were hit.",
    .init_cb = diag_dnx_pp_vis_fer_config_cb,
    .synopsis = "[core=<core_id>]",
    .examples = "\n" "core=0\n" "short=yes\n" "test_mode=no"
};

/**
 * \brief
 *    Retrieve the data for an ECMP group in a particular hierarchy needed for a single row in the table.
 * \param [in] unit - unit ID
 * \param [in] ecmp_group_id - Index of the ECMP group
 * \param [in] hierarchy_id - Zero-based value of the hierarchy ID.
 * \param [in] core_id - Index of the core.
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [out] selected_fec - Forwarding destination FEC decision which is a member of the current ECMP group
 * \param [out] values - The field values of the ECMP which will be added to the table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_pp_fer_add_ecmp_group_info(
    int unit,
    uint32 ecmp_group_id,
    uint32 hierarchy_id,
    int core_id,
    sh_sand_control_t * sand_control,
    uint32 *selected_fec,
    char *values[DIAG_DNX_PP_VIS_FER_NOF_COLUMNS])
{
    int intf_count;
    int rv;
    int nof_bits_in_hier_hash = 16;
    int enum_hashing_func;
    uint32 hw_hashing_func;
    int ecmp_hier_enum[3] =
        { bcmSwitchECMPHashConfig, bcmSwitchECMPSecondHierHashConfig, bcmSwitchECMPThirdHierHashConfig };
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int nof_signals = 1;
    uint32 hierarchy;
    uint32 fec_offset;
    uint32 used_member;
    uint16 ecmp_lb_keys[3];
    char sub_titles[3][19] = { "1st hierarchy", "2nd hierarchy", "3rd hierarchy" };
    char crc_functions[8][8] =
        { "0x1015d", "0x100d7", "0x10039", "0x10ac5", "0x109e7", "0x10939", "0x12105", "0x1203d" };
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;
    bcm_l3_egress_ecmp_t ecmp_group;
    dnx_l3_ecmp_profile_t ecmp_profile;
    dnx_l3_ecmp_attributes_t ecmp_atr;
    bcm_instru_vis_signal_key_t signal_key[1];
    bcm_instru_vis_signal_result_t signal_result[1];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(signal_key, 0, sizeof(signal_key[0]) * nof_signals);
    sal_memset(signal_result, 0, sizeof(signal_result[0]) * nof_signals);
    /** retrieve the ECMP LB Keys*/
    sal_strncpy(signal_key[0].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD1),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[0].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD1),
                BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[0].signal, "Ecmp_LB_Keys", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));
    if (signal_result[0].status != BCM_E_NONE)
    {
        LOG_CLI(("Could not retrieve signal data for core = %d\n", core_id));
        SHR_EXIT();
    }

    ecmp_lb_keys[2] = signal_result[0].value[1];
    ecmp_lb_keys[1] = signal_result[0].value[0] >> nof_bits_in_hier_hash;
    ecmp_lb_keys[0] = signal_result[0].value[0] & UTILEX_BITS_MASK(nof_bits_in_hier_hash - 1, 0);

    bcm_l3_egress_ecmp_t_init(&ecmp_group);
    ecmp_group.ecmp_intf = ecmp_group_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get
                    (unit, &ecmp_group, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, intf_array, &intf_count));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_ecmp_group_info
                    (unit, ecmp_group_id, 0, &intf_count, intf_array, &ecmp_profile));
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_group_id, (uint32 *) &hierarchy));
    if (hierarchy > hierarchy_id)
    {
        SHR_EXIT();
    }
    sal_strncpy(values[0], sub_titles[hierarchy_id], 19);
    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE,
                                                          DBAL_FIELD_ECMP_ID, (uint32 *) &ecmp_group_id,
                                                          NULL, 0, TRUE, values[1]));

    sal_snprintf(values[2], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%d", ecmp_profile.group_size);

    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                    (unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, DBAL_FIELD_PROTECTION_ENABLE,
                     (uint32 *) &ecmp_profile.protection_flag, NULL, 0, FALSE, values[3]));
    sal_strncpy(values[4], ((ecmp_group.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) ? "Yes" : "No"), 4);
    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                    (unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, DBAL_FIELD_ECMP_MODE,
                     (uint32 *) &ecmp_profile.ecmp_mode, NULL, 0, FALSE, values[5]));

    sal_snprintf(values[6], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%04x", ecmp_lb_keys[hierarchy_id]);
    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE,
                                                          DBAL_FIELD_FEC_POINTER_BASE,
                                                          (uint32 *) &intf_array[0], NULL, 0, FALSE, values[7]));

    /** Calculate FEC offset and selected FEC */
    if (ecmp_profile.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
    {
        fec_offset =
            ((ecmp_lb_keys[hierarchy_id] * ecmp_profile.group_size) >> nof_bits_in_hier_hash) *
            (ecmp_profile.protection_flag + 1);
        used_member = BCM_L3_ITF_VAL_GET(intf_array[0]) + fec_offset;
    }
    else
    {
        /** Will hold the member offset with respect to the beginning of the table */
        uint32 ecmp_member_table_offset;
        uint8 fec_offset_cons;

        if (ecmp_profile.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_SMALL_TABLE)
        {
            ecmp_member_table_offset = ecmp_lb_keys[hierarchy_id] & 0x7F;
        }
        else if (ecmp_profile.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_MEDIUM_TABLE)
        {
            ecmp_member_table_offset = ecmp_lb_keys[hierarchy_id] & 0xFF;
        }
        else
        {
            ecmp_member_table_offset = (ecmp_lb_keys[hierarchy_id] * ecmp_profile.group_size) >> 15;
        }

        SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_group_id, &ecmp_atr));

        /** Add protection multiplication */
        ecmp_member_table_offset = (ecmp_member_table_offset * (ecmp_profile.protection_flag + 1));

        /** get the FEC that is in the calculated offset */
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_fec_members_table.get(unit, ecmp_atr.profile_index, ecmp_member_table_offset,
                                                                &fec_offset_cons));

        fec_offset = fec_offset_cons;
        used_member = BCM_L3_ITF_VAL_GET(intf_array[0]) + fec_offset_cons;
    }
    sal_snprintf(values[8], sizeof(uint32), "%d", fec_offset);
    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE,
                                                          DBAL_FIELD_FEC_POINTER_BASE,
                                                          &used_member, NULL, 0, FALSE, values[9]));
    SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, ecmp_hier_enum[hierarchy_id], &enum_hashing_func));
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_function_enum_to_hw_get(unit, enum_hashing_func, &hw_hashing_func));
    sal_snprintf(values[10], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%s", crc_functions[hw_hashing_func]);
    control_key.type = bcmSwitchHashSeed;
    control_key.index = enum_hashing_func;
    rv = bcm_switch_control_indexed_get(unit, control_key, &control_info);
    if (rv != _SHR_E_NONE)
    {
        LOG_CLI(("Could not retrieve Hashing Seed value\n"));
        SHR_EXIT();
    }
    sal_snprintf(values[11], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%d", control_info.value);

    *selected_fec = used_member;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Retrieve the data for a FEC in a particular hierarchy needed for a single row in the table.
 * \param [in] unit - unit ID
 * \param [in] fec_id - Index of the FEC
 * \param [in] hierarchy_id - Zero-based value of the hierarchy ID.
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [out] fwd_destination - Forwarding destination which is result of the current FEC
 * \param [out] values - The field values of the FEC which will be added to the table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_pp_fer_add_fec_info(
    int unit,
    uint32 *fec_id,
    uint32 hierarchy_id,
    sh_sand_control_t * sand_control,
    uint32 *fwd_destination,
    char *values[DIAG_DNX_PP_VIS_FER_NOF_COLUMNS])
{
    bcm_l3_egress_t egr_entry;
    uint32 sub_resource_id;
    uint32 mc_rpf_type;
    char sub_titles[3][19] = { "1st hierarchy", "2nd hierarchy", "3rd hierarchy" };
    int dbal_table_ids[3] =
        { DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY, DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY, DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY };
    uint32 eei = 0;
    int fail_enabled = 0;
    dbal_enum_value_field_model_type_e qos_model;
    dbal_enum_value_field_model_type_e ttl_model;
    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_egress_t_init(&egr_entry);

    /*
     * Receive the ID of the sub-resource to which the FEC belongs, later to be used as 6bits for FEC hieararchy calculation
     * divide the physical FEC to the number of physical FECs per sub-resource.
     */
    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, *fec_id) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
    if (sub_resource_id > 29)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "the fec_id sub_resource_id is out of 0-29 range: %d!\n", sub_resource_id);
    }

    /*
     *  Get the FEC entry
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, *fec_id, &egr_entry));
    if (egr_entry.failover_id != 0)
    {
        SHR_IF_ERR_EXIT(bcm_failover_get(unit, egr_entry.failover_id, &fail_enabled));
        if (fail_enabled == 0)
        {
            *fec_id = egr_entry.failover_if_id;
            bcm_l3_egress_t_init(&egr_entry);
            SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, *fec_id, &egr_entry));
        }
    }
    sal_strncpy(values[0], sub_titles[hierarchy_id], 19);
    sal_snprintf(values[1], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%d", *fec_id);
    sal_snprintf(values[2], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%04x", egr_entry.stat_id);
    sal_snprintf(values[3], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%04x", egr_entry.stat_pp_profile);

    if (BCM_GPORT_IS_FORWARD_PORT(egr_entry.port))
    {
        sal_snprintf(values[4], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%08x (FEC %d)", egr_entry.port,
                     BCM_GPORT_FORWARD_PORT_GET(egr_entry.port));
    }
    else
    {
        sal_snprintf(values[4], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%08x", egr_entry.port);
    }
    mc_rpf_type = L3_EGRESS_GET_MC_RPF_TYPE(egr_entry.flags);
    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, dbal_table_ids[hierarchy_id],
                                                          DBAL_FIELD_MC_RPF_MODE,
                                                          &mc_rpf_type, NULL, 0, FALSE, values[5]));
    if (egr_entry.egress_qos_model.egress_qos == bcmQosEgressModelUniform)
    {
        qos_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;
    }
    else
    {
        qos_model = DBAL_ENUM_FVAL_MODEL_TYPE_PIPE;
    }
    if (egr_entry.egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
    {
        ttl_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;
    }
    else
    {
        ttl_model = DBAL_ENUM_FVAL_MODEL_TYPE_PIPE;
    }

    SHR_IF_ERR_EXIT(dnx_mpls_eei_php_information_fill
                    (unit, egr_entry.qos_map_id, qos_model, ttl_model, DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS, &eei));
    sal_snprintf(values[6], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%04x", eei);
    sal_snprintf(values[7], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%04x", egr_entry.hierarchical_gport);
    sal_snprintf(values[8], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%04x", egr_entry.intf);
    sal_snprintf(values[9], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%04x", egr_entry.encap_id);
    if (egr_entry.failover_id != 0)
    {
        if (fail_enabled == 0)
        {
            sal_snprintf(values[10], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "SECONDARY");
        }
        else
        {
            sal_snprintf(values[10], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "PRIMARY");
        }
    }
    else
    {
        sal_snprintf(values[10], DBAL_MAX_PRINTABLE_BUFFER_SIZE, "Disabled");
    }
    values[11][0] = 0;
    *fwd_destination = egr_entry.port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that present the ECMP bank allocation data
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_pp_vis_fer_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int hier_id;
    int idx;
    int fwd_type;
    int nof_signals;
    int core_id, core_in;
    const int nof_hierarchies = FEC_NOF_HIERARCHY;
    uint8 is_first = FALSE;
    uint32 fwd_destination = 0;
    uint32 fec_sig_values[FEC_NOF_HIERARCHY];
    uint32 selected_fec[FEC_NOF_HIERARCHY] =
        { dnx_data_l3.fec.nof_fecs_get(unit), dnx_data_l3.fec.nof_fecs_get(unit), dnx_data_l3.fec.nof_fecs_get(unit) };
    uint32 selected_ecmp[FEC_NOF_HIERARCHY] =
        { dnx_data_l3.ecmp.total_nof_ecmp_get(unit), dnx_data_l3.ecmp.total_nof_ecmp_get(unit),
        dnx_data_l3.ecmp.total_nof_ecmp_get(unit)
    };

    int nof_columns = DIAG_DNX_PP_VIS_FER_NOF_COLUMNS;
    char *ecmp_values[FEC_NOF_HIERARCHY][DIAG_DNX_PP_VIS_FER_NOF_COLUMNS] = { {NULL} };
    char *fec_values[FEC_NOF_HIERARCHY][DIAG_DNX_PP_VIS_FER_NOF_COLUMNS] = { {NULL} };
    char *fec_substages[FEC_NOF_HIERARCHY] = { "FEC1", "FEC2", "FEC3" };

    char ecmp_fields[DIAG_DNX_PP_VIS_FER_NOF_COLUMNS][15] =
        { "ECMP Hierarchy", "Group ID", "Group size", "Protection", "Stateful", "Hashing mode", "LB key", "Base FEC",
        "FEC offset",
        "Selected FEC", "CRC function", "Seed"
    };
    char fec_fields[DIAG_DNX_PP_VIS_FER_NOF_COLUMNS][17] =
        { "FEC Hierarchy", "FEC ID", "Statistics ID", "Stat Cmd", "Destination", "MC RPF mode", "EEI", "Hier TM flow",
        "Glob_Out_LIF", "Glob_Out_LIF_2nd", "Protection Path", ""
    };
    bcm_instru_vis_signal_key_t *signal_key = NULL;
    bcm_instru_vis_signal_result_t *signal_result = NULL;

    uint8 short_mode = FALSE;
    uint8 test_mode = FALSE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);
    SH_SAND_GET_BOOL("SHORT", short_mode);
    SH_SAND_GET_BOOL("TEST_MODE", test_mode);

    if (dnx_data_debug.general.mdb_debug_signals_get(unit))
    {
        nof_signals = 4;
    }
    else
    {
        nof_signals = 3;
    }
    signal_key = sal_alloc(sizeof(bcm_instru_vis_signal_key_t) * nof_signals, "signal_key");
    signal_result = sal_alloc(sizeof(bcm_instru_vis_signal_result_t) * nof_signals, "signal_result");

    sal_memset(signal_key, 0, sizeof(bcm_instru_vis_signal_key_t) * nof_signals);
    sal_memset(signal_result, 0, sizeof(bcm_instru_vis_signal_result_t) * nof_signals);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        uint8 exit_flag = FALSE;
        /** check if core_id is valid */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, !short_mode);

        for (hier_id = 0; hier_id < nof_hierarchies; hier_id++)
        {
            for (idx = 0; idx < nof_columns; idx++)
            {
                ecmp_values[hier_id][idx] =
                    sal_alloc(sizeof(char) * DBAL_MAX_PRINTABLE_BUFFER_SIZE, "ECMP values print");
                fec_values[hier_id][idx] = sal_alloc(sizeof(char) * DBAL_MAX_PRINTABLE_BUFFER_SIZE, "FEC values print");
                sal_memset(ecmp_values[hier_id][idx], 0, sizeof(char) * DBAL_MAX_PRINTABLE_BUFFER_SIZE);
                sal_memset(fec_values[hier_id][idx], 0, sizeof(char) * DBAL_MAX_PRINTABLE_BUFFER_SIZE);
            }
        }

        /** retrieve the forward action destination */
        dnx_dbal_fields_enum_value_get(unit, "DESTINATION_ENCODING", "FEC", &fwd_type);

        sal_memset(signal_key, 0, sizeof(bcm_instru_vis_signal_key_t) * nof_signals);
        sal_memset(signal_result, 0, sizeof(bcm_instru_vis_signal_result_t) * nof_signals);

        sal_strncpy(signal_key[0].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                    BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[0].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                    BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[0].signal, "Fwd_Action_Dst.value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

        if (dnx_data_debug.general.mdb_debug_signals_get(unit))
        {
            sal_strncpy(signal_key[1].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                        BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[1].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_FER),
                        BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[1].to, fec_substages[0], BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[1].signal, "If_0_Key", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[2].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                        BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[2].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_FER),
                        BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[2].to, fec_substages[1], BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[2].signal, "If_0_Key", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[3].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                        BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[3].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_FER),
                        BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[3].to, fec_substages[2], BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[3].signal, "If_0_Key", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        }
        else
        {
            sal_strncpy(signal_key[1].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                        BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[1].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                        BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[1].signal, "Fwd_Action_Dst", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

            sal_strncpy(signal_key[2].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                        BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[2].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                        BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[2].signal, "Fwd_Action_Dst.type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        }
        SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

        if (signal_result[0].status == BCM_E_NONE && signal_result[1].status == BCM_E_NONE
            && signal_result[2].status == BCM_E_NONE)
        {
            fwd_destination = signal_result[0].value[0];
        }
        else
        {
            LOG_CLI(("Could not retrieve signal data for core = %d\n", core_id));
            continue;
        }
        if (dnx_data_debug.general.mdb_debug_signals_get(unit))
        {
            int idx;
            for (hier_id = 0, idx = 1; hier_id < nof_hierarchies; hier_id++, idx++)
            {
                fec_sig_values[hier_id] = signal_result[idx].value[0];
            }
        }

        /*
         * If destination encoding is not FEC, then exit, there is no need to check resolution
         * And print of which type and value the destination is
         */
        if (dnx_data_debug.general.mdb_debug_signals_get(unit))
        {
            if (fec_sig_values[0] == 0 && fec_sig_values[1] == 0 && fec_sig_values[2] == 0)
            {
                if (!short_mode)
                {
                    LOG_CLI(("Could not retrieve valid FER signal data for core = %d\n", core_id));
                }
                continue;
            }
            else
            {
                LOG_CLI(("SUPER FECs used in FEC resolution:\nHierarchy 1: 0x%04x\nHierarchy 2: 0x%04x\nHierarchy 3: 0x%04x\n", fec_sig_values[0], fec_sig_values[1], fec_sig_values[2]));
            }
        }
        else
        {
            if (signal_result[2].value[0] != fwd_type)
            {
                uint32 fwd_dest_w_encoding = signal_result[1].value[0];
                char parsed_dest_print[DSIG_MAX_SIZE_STR];
                char dest_field_str[RHNAME_MAX_SIZE];

                sal_strncpy(dest_field_str, dbal_field_to_string(unit, DBAL_FIELD_DESTINATION), RHNAME_MAX_SIZE - 1);

                SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get(unit, dest_field_str,
                                                                         fwd_dest_w_encoding, parsed_dest_print));

                /** print the warning only if in full mode, otherwise just skip*/
                if (!short_mode)
                {
                    LOG_CLI(("Destination encoding is not of (FEC) type, but - (%s)\n", parsed_dest_print));
                }
                continue;
            }
        }

        /*
         * TABLE with FER information
         */
        PRT_TITLE_SET("FER selection information, CORE ID = %d", core_id);
        for (idx = 0; idx < nof_columns; idx++)
        {
            PRT_COLUMN_ADD("");
        }

        for (hier_id = 0; hier_id < nof_hierarchies; hier_id++)
        {
            uint8 is_allocated = FALSE;
            uint32 hierarchy;
            /** Check if is ECMP */
            if (fwd_destination < dnx_data_l3.ecmp.nof_ecmp_get(unit))
            {
                /** Destination is in the valid range for ECMP. */
                SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_is_allocated(unit, fwd_destination, &is_allocated));
                if (!is_allocated)
                {
                    LOG_CLI(("The ECMP group 0x%x isn't allocated but is used as part of the forwarding \n",
                             fwd_destination));
                    /** Change flag so the iterator will continue to the next core */
                    exit_flag = TRUE;
                    break;
                }

                SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, fwd_destination, &hierarchy));

                /** This ECMP is in a higher hierarchy than the current one, continue till this hierarchy will be reached */
                if (hierarchy > hier_id)
                {
                    continue;
                }

                selected_ecmp[hier_id] = fwd_destination;
                SHR_IF_ERR_EXIT(dnx_pp_fer_add_ecmp_group_info
                                (unit, selected_ecmp[hier_id], hier_id, core_id, sand_control, &fwd_destination,
                                 ecmp_values[hier_id]));
            }

            SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(fwd_destination), &is_allocated));

            if (is_allocated == 1)
            {
                if (dnx_data_debug.general.mdb_debug_signals_get(unit))
                {
                    if ((fwd_destination >> 1) != fec_sig_values[hier_id])
                    {
                        LOG_CLI(("WARNING! Incorrect calculation of SUPER FEC ID: expected 0x%04x but is actually 0x%04x\n", (fwd_destination >> 1), fec_sig_values[hier_id]));
                        if (test_mode)
                        {
                            SHR_IF_ERR_EXIT(_SHR_E_FAIL);
                        }
                    }
                }
                SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get
                                (unit, BCM_L3_ITF_VAL_GET(fwd_destination), &hierarchy));
                /** In case the FEC is matching the current hierarchy continue with the FER process. */
                if (hierarchy != hier_id)
                {
                    continue;
                }
                else
                {
                    selected_fec[hier_id] = BCM_L3_ITF_VAL_GET(fwd_destination);
                    SHR_IF_ERR_EXIT(dnx_pp_fer_add_fec_info
                                    (unit, &selected_fec[hier_id], hier_id, sand_control,
                                     &fwd_destination, fec_values[hier_id]));
                }
            }
            else
            {
                break;
            }

            /** If destination can be resolved to a forward port, then it is either FEC or an ECMP group. */
            if (BCM_GPORT_IS_FORWARD_PORT(fwd_destination))
            {
                fwd_destination = BCM_GPORT_FORWARD_PORT_GET(fwd_destination);
            }
            else
            {
                /** If destination is not a forward port, then there is nothing else to add to the table. */
                uint32 fwd_dest_verify;
                dpp_dsig_read(unit, core_id, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                              dnx_pp_stage_name(unit, DNX_PP_STAGE_FER), NULL, "Fwd_action_dst.value", &fwd_dest_verify,
                              1);
                if (fwd_dest_verify != (fwd_destination & 0xFFFFFF))
                {
                    LOG_CLI(("Core_Id=%d: last FEC destination: %d, is not equal to the FER destination: %d!\n",
                             core_id, (fwd_destination & 0xFFFFFF), fwd_dest_verify));
                    if (test_mode)
                    {
                        SHR_IF_ERR_EXIT(_SHR_E_FAIL);
                    }
                }
                break;
            }
        }
        /** Continue to the next core if encountered allocation error */
        if (exit_flag)
        {
            continue;
        }
        /** Add ECMP entries */
        if (selected_ecmp[0] != dnx_data_l3.ecmp.total_nof_ecmp_get(unit)
            || selected_ecmp[1] != dnx_data_l3.ecmp.total_nof_ecmp_get(unit)
            || selected_ecmp[2] != dnx_data_l3.ecmp.total_nof_ecmp_get(unit))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_EQUAL);
            /** Add ECMP column names */
            for (idx = 0; idx < nof_columns; idx++)
            {
                PRT_CELL_SET("%s", ecmp_fields[idx]);
            }
            for (hier_id = 0; hier_id < nof_hierarchies; hier_id++)
            {
                if (selected_ecmp[hier_id] == dnx_data_l3.ecmp.total_nof_ecmp_get(unit))
                {
                    continue;
                }
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                for (idx = 0; idx < nof_columns; idx++)
                {
                    PRT_CELL_SET("%s", ecmp_values[hier_id][idx]);
                }

                is_first = TRUE;
            }
        }

        /** Add FEC entries */
        if (selected_fec[0] != dnx_data_l3.fec.nof_fecs_get(unit)
            || selected_fec[1] != dnx_data_l3.fec.nof_fecs_get(unit)
            || selected_fec[2] != dnx_data_l3.fec.nof_fecs_get(unit))
        {
            /** If any ECMP rows were added to the table, add a row that would serve as a separator */
            if (is_first == TRUE)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_EQUAL);
                PRT_CELL_SKIP(nof_columns);
            }
            is_first = FALSE;

            PRT_ROW_ADD(PRT_ROW_SEP_EQUAL);
            /** Add FEC column names */
            for (idx = 0; idx < nof_columns; idx++)
            {
                PRT_CELL_SET("%s", fec_fields[idx]);
            }

            for (hier_id = 0; hier_id < nof_hierarchies; hier_id++)
            {
                if (selected_fec[hier_id] == dnx_data_l3.fec.nof_fecs_get(unit))
                {
                    continue;
                }
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                for (idx = 0; idx < nof_columns; idx++)
                {
                    PRT_CELL_SET("%s", fec_values[hier_id][idx]);
                }
            }
        }
        PRT_COMMITX;
    } /** of ITERATOR for cores */
exit:
    for (hier_id = nof_hierarchies - 1; hier_id >= 0; hier_id--)
    {
        for (idx = nof_columns - 1; idx >= 0; idx--)
        {
            SHR_FREE(ecmp_values[hier_id][idx]);
            SHR_FREE(fec_values[hier_id][idx]);
        }
    }

    SHR_FREE(signal_key);
    SHR_FREE(signal_result);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_pp_vis_fer_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {"TEST_MODE", SAL_FIELD_TYPE_BOOL, "A mode that tests the diagnostics", "No"}
    ,
    {NULL}
};
