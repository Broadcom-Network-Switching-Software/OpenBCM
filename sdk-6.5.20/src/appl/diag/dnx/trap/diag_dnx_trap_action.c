/** \file diag_dnx_trap_action.c
 *
 * file for trap action diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "include/bcm_int/dnx/rx/rx_trap.h"
#include "diag_dnx_trap.h"

/*
 * }
 */

/**
 * \brief
 *    Init callback function for "trap action" diag commands
 *    1) Create an ingress predefined trap
 *    2) Set several actions to trap
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
diag_dnx_trap_action_init_cb(
    int unit)
{
    bcm_rx_trap_config_t config = { 0 };
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&config);

    config.flags = BCM_RX_TRAP_UPDATE_ECN_VALUE | BCM_RX_TRAP_UPDATE_VISIBILITY | BCM_RX_TRAP_UPDATE_VSQ |
        BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_DEST;
    config.trap_strength = 10;
    config.ecn_value = 5;
    config.vsq = 2;
    config.latency_flow_id_clear = 1;
    config.visibility_value = 1;
    config.stat_metadata_mask = 0x5555;
    config.stat_clear_bitmap = 0x305;
    config.dest_port = 200;

    config.stat_obj_config_len = 2;

    (config.stat_obj_config_arr)->counter_command_id = 1;
    (config.stat_obj_config_arr)->stat_id = 0x7212;
    (config.stat_obj_config_arr)->is_offset_by_qual_enable = 0;
    (config.stat_obj_config_arr)->stat_object_type = 2;
    (config.stat_obj_config_arr)->is_meter = 1;

    (config.stat_obj_config_arr + 1)->counter_command_id = 7;
    (config.stat_obj_config_arr + 1)->stat_id = 0x6058;
    (config.stat_obj_config_arr + 1)->is_offset_by_qual_enable = 1;
    (config.stat_obj_config_arr + 1)->stat_object_type = 0;
    (config.stat_obj_config_arr + 1)->is_meter = 0;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapLinkLayerSaMulticast, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

exit:
    SHR_FUNC_EXIT;

}

static sh_sand_man_t dnx_trap_action_info_man = {
    .brief = "Print information of trap's action\n",
    .full = "Trap's action information includes, forward and snoop strengths and actions configured (tc, dp, etc)\n",
    .synopsis = "[id=<trap_id>]",
    .examples = "id=3",
    .init_cb = diag_dnx_trap_action_init_cb
};

static sh_sand_option_t dnx_trap_action_info_options[] = {
    {"ID", SAL_FIELD_TYPE_INT32, "Trap id", NULL, NULL},
    {NULL}
};

/**
 * \brief
 *    A function that prints trap snoop action info.
 * \param [in] unit - unit ID
 * \param [in] trap_title - trap title
 * \param [in] trap_type - trap type to print its info
 * \param [in] snp_code - snoop code (HW code) 
 * \param [in] trap_config_p - trap configuration
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_trap_snp_action_info_irpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int snp_code,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control)
{
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", trap_title);
    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Value");

    if (snp_code < DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_ITMH_SNIF_CODE)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Trap type");
        PRT_CELL_SET("bcmRxTrap%s", trap_name);
    }
    else
    {
        trap_config_p->snoop_cmnd = snp_code - DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_ITMH_SNIF_CODE;
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Snoop code");
    PRT_CELL_SET("0x%x", snp_code);

    if (trap_config_p->snoop_strength)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop strength");
        PRT_CELL_SET("%d", trap_config_p->snoop_strength);
    }

    /** Print snoop command only if it is valid */
    if (trap_config_p->snoop_cmnd != -1)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop command");
        PRT_CELL_SET("%d", trap_config_p->snoop_cmnd);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A function that prints trap action info.
 * \param [in] unit - unit ID
 * \param [in] trap_title - trap title
 * \param [in] trap_type - trap type to print its info
 * \param [in] trap_id - trap id (HW code) 
 * \param [in] trap_config_p - trap configuration
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_trap_action_info_irpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control)
{
    char trap_name[DBAL_MAX_STRING_LENGTH];
    uint32 stat_obj;
    bcm_rx_trap_config_t empty_trap_config, copy_trap_config;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", trap_title);
    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Value");
    if (trap_id != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_INVALID_DESTINATION)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
    }
    else
    {
        sal_strncpy(trap_name, "InvalidDestination", DBAL_MAX_STRING_LENGTH);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap type");
    PRT_CELL_SET("bcmRxTrap%s", trap_name);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap id (HW code)");
    PRT_CELL_SET("0x%x", trap_id);

    /** In case this is not last resolved trap, show fwd and snoop action in the same table */
    if (sal_strcmp(trap_title, "IRPP Trap Action") == 0)
    {
        if (trap_config_p->snoop_strength)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Snoop strength");
            PRT_CELL_SET("%d", trap_config_p->snoop_strength);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop command");
        PRT_CELL_SET("%d", trap_config_p->snoop_cmnd);
    }

    if (trap_config_p->trap_strength)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forward strength");
        PRT_CELL_SET("%d", trap_config_p->trap_strength);
    }

    if ((trap_config_p->flags & BCM_RX_TRAP_UPDATE_DEST) && (trap_config_p->dest_port == BCM_GPORT_BLACK_HOLE))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forward action");
        PRT_CELL_SET("Drop");
        PRT_COMMITX;
        SHR_EXIT();
    }
    else
    {
        bcm_rx_trap_config_t_init(&empty_trap_config);
        sal_memcpy(&copy_trap_config, trap_config_p, sizeof(bcm_rx_trap_config_t));
        copy_trap_config.snoop_strength = 0;
        copy_trap_config.trap_strength = 0;

        if (sal_memcmp(&copy_trap_config, &empty_trap_config, sizeof(bcm_rx_trap_config_t)) == 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Forward action");
            PRT_CELL_SET("Default");
            PRT_COMMITX;
            SHR_EXIT();
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Forward action attributes");
            PRT_CELL_SET(":");
        }
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_DEST)
    {
        if (trap_config_p->flags & BCM_RX_TRAP_DEST_MULTICAST)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Destination group");
            PRT_CELL_SET("%d", trap_config_p->dest_group);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Destination port");
            PRT_CELL_SET("%d", trap_config_p->dest_port);
        }
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_PRIO)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Traffic Class (TC)");
        PRT_CELL_SET("%d", trap_config_p->prio);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_COLOR)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Drop Precedence (DP)");
        PRT_CELL_SET("%d", trap_config_p->color);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_TRAP)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("CPU-Trap-Code stamping on PPH");
        PRT_CELL_SET("Enabled");
    }

    if ((dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_snoop_code_clear_action) == TRUE) &&
        (trap_config_p->flags2 & BCM_RX_TRAP_FLAGS2_SNOOP_CODE_CLEAR))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop Code Clear");
        PRT_CELL_SET("Enabled");
    }

    if ((dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_mirror_code_clear_action) == TRUE) &&
        (trap_config_p->flags2 & BCM_RX_TRAP_FLAGS2_MIRROR_CODE_CLEAR))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Mirror Code Clear");
        PRT_CELL_SET("Enabled");
    }

    if ((dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_stat_sampling_code_clear_action) == TRUE) &&
        (trap_config_p->flags2 & BCM_RX_TRAP_FLAGS2_STAT_SAMPLING_CODE_CLEAR))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Statistical Sampling Code Clear");
        PRT_CELL_SET("Enabled");
    }

    if (trap_config_p->flags & BCM_RX_TRAP_BYPASS_FILTERS)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Bypass filters");
        PRT_CELL_SET("Enabled");
    }

    if (trap_config_p->flags & BCM_RX_TRAP_LEARN_DISABLE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Learn Disable");
        PRT_CELL_SET("Enabled");
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_ADD_VLAN)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Add vlan");
        PRT_CELL_SET("Enabled");
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_FORWARDING_HEADER)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forwarding header");
        PRT_CELL_SET("%d", trap_config_p->forwarding_header);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Egress forwarding index");
        PRT_CELL_SET("%d", trap_config_p->egress_forwarding_index);

        if (trap_config_p->egress_forwarding_index == 7
            && dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_oam_offset_actions))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Use OAM Offset");
            PRT_CELL_SET("Enabled");
        }
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_ENCAP_ID)
    {
        if (trap_config_p->encap_id != 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Encap ID");
            PRT_CELL_SET("0x%x", trap_config_p->encap_id);
        }

        if (trap_config_p->encap_id2 != 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Encap ID #2");
            PRT_CELL_SET("0x%x", trap_config_p->encap_id2);
        }
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_METER_CMD)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Meter command");
        PRT_CELL_SET("%d", trap_config_p->meter_cmd);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_ECN_VALUE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("ECN value");
        PRT_CELL_SET("%d", trap_config_p->ecn_value);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_VISIBILITY)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Visibility");
        PRT_CELL_SET("%d", trap_config_p->visibility_value);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_VSQ)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("VSQ");
        PRT_CELL_SET("%d", trap_config_p->vsq);
    }

    if (trap_config_p->latency_flow_id_clear)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Latency flow id clear");
        PRT_CELL_SET("TRUE");
    }

    if (trap_config_p->stat_clear_bitmap)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Statistical objects clear bitmap");
        PRT_CELL_SET("0x%x", trap_config_p->stat_clear_bitmap);
    }

    if (trap_config_p->stat_metadata_mask)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Statistical metadata mask");
        PRT_CELL_SET("0x%x", trap_config_p->stat_metadata_mask);
    }

    for (stat_obj = 0; stat_obj < trap_config_p->stat_obj_config_len; stat_obj++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Counter command id [%d]", stat_obj);
        PRT_CELL_SET("%d", trap_config_p->stat_obj_config_arr[stat_obj].counter_command_id);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Statistical object id [%d]", stat_obj);
        PRT_CELL_SET("0x%x", trap_config_p->stat_obj_config_arr[stat_obj].stat_id);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Statistical object type [%d]", stat_obj);
        PRT_CELL_SET("%d", trap_config_p->stat_obj_config_arr[stat_obj].stat_object_type);

        if (trap_config_p->stat_obj_config_arr[stat_obj].is_offset_by_qual_enable)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Offset by qualifier [%d]", stat_obj);
            PRT_CELL_SET("TRUE");
        }

        if (trap_config_p->stat_obj_config_arr[stat_obj].is_meter)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Is meter [%d]", stat_obj);
            PRT_CELL_SET("TRUE");
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * See diag_dnx_trap.h
 */
shr_error_e
dnx_trap_action_info_erpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control)
{
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%sERPP Trap Forward Action", trap_title);
    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Value");

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap type");
    PRT_CELL_SET("bcmRxTrap%s", trap_name);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap id");
    PRT_CELL_SET("0x%x", trap_id);

    if (trap_config_p->trap_strength)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forward strength");
        PRT_CELL_SET("%d", trap_config_p->trap_strength);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Forward action profile");
    PRT_CELL_SET("%d", DNX_RX_TRAP_ID_TYPE_GET(trap_id));

    if (trap_id == BCM_RX_TRAP_EG_TRAP_ID_DEFAULT)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forward action");
        PRT_CELL_SET("Default");
        PRT_COMMITX;
        SHR_EXIT();
    }
    else if ((trap_config_p->flags & BCM_RX_TRAP_UPDATE_DEST) && (trap_config_p->dest_port == BCM_GPORT_BLACK_HOLE))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forward action");
        PRT_CELL_SET("Drop");
        PRT_COMMITX;
        SHR_EXIT();
    }
    else
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forward action attributes");
        PRT_CELL_SET(":");
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_DEST)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Destination port");
        PRT_CELL_SET("%d", trap_config_p->dest_port);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_PRIO)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Traffic Class (TC)");
        PRT_CELL_SET("%d", trap_config_p->prio);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_COLOR)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Drop Precedence (DP)");
        PRT_CELL_SET("%d", trap_config_p->color);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_QOS_MAP_ID)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("QOS id");
        PRT_CELL_SET("%d", trap_config_p->qos_map_id);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_ENCAP_ID)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Encap id");
        PRT_CELL_SET("0x%x", trap_config_p->encap_id);
    }

    if (trap_config_p->pp_drop_reason > 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("PP drop reason");
        PRT_CELL_SET("%d", trap_config_p->pp_drop_reason);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_COUNTER)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Counter profile [0]");
        PRT_CELL_SET("%d", trap_config_p->stat_obj_config_arr[0].stat_profile);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Counter pointer [0]");
        PRT_CELL_SET("0x%x", trap_config_p->stat_obj_config_arr[0].stat_id);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_UPDATE_COUNTER_2)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Counter profile [1]");
        PRT_CELL_SET("%d", trap_config_p->stat_obj_config_arr[1].stat_profile);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Counter pointer [1]");
        PRT_CELL_SET("0x%x", trap_config_p->stat_obj_config_arr[1].stat_id);
    }

    if (trap_config_p->snoop_cmnd > 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop command");
        PRT_CELL_SET("%d", trap_config_p->snoop_cmnd);
    }

    if (trap_config_p->mirror_profile > 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Mirror profile");
        PRT_CELL_SET("%d", trap_config_p->snoop_cmnd);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A function that prints ETPP trap action info.
 * \param [in] unit - unit ID
 * \param [in] trap_title - trap title to print 
 * \param [in] trap_type - trap type to print its info
 * \param [in] trap_id - trap id encoded
 * \param [in] trap_config_p - trap infouration
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_trap_action_info_etpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control)
{
    char trap_name[DBAL_MAX_STRING_LENGTH];
    int recycle_trap_id, recycle_strength, fwd_act_profile;
    bcm_rx_trap_t recycle_trap_type;
    dnx_rx_trap_class_e trap_class = DNX_RX_TRAP_ID_CLASS_GET(trap_id);
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    fwd_act_profile = DNX_RX_TRAP_ID_TYPE_GET(trap_id);
    if (trap_class == DNX_RX_TRAP_CLASS_OAM)
    {
        fwd_act_profile = DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_GET(fwd_act_profile);
    }

    PRT_TITLE_SET("%sETPP Trap Forward Action", trap_title);
    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Value");

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap type");
    PRT_CELL_SET("bcmRxTrap%s", trap_name);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap id");
    PRT_CELL_SET("0x%x", trap_id);

    if (trap_config_p->trap_strength)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Forward strength");
        PRT_CELL_SET("%d", trap_config_p->trap_strength);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Forward action profile");
    PRT_CELL_SET("%d", DNX_RX_TRAP_ID_TYPE_GET(trap_id));

    switch (trap_id)
    {
        case BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT:
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Forward action");
            PRT_CELL_SET("Default");
            PRT_COMMITX;
            SHR_EXIT();
        }
        case BCM_RX_TRAP_EG_TX_TRAP_ID_DROP:
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Forward action");
            PRT_CELL_SET("Drop");
            PRT_COMMITX;
            SHR_EXIT();
        }
        default:
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Forward action attributes");
            PRT_CELL_SET(":");
        }
    }

    recycle_trap_id = BCM_GPORT_TRAP_GET_ID(trap_config_p->cpu_trap_gport);
    recycle_strength = BCM_GPORT_TRAP_GET_STRENGTH(trap_config_p->cpu_trap_gport);

    if (recycle_strength != 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Recycle Ingress trap id");
        PRT_CELL_SET("0x%x", recycle_trap_id);

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, recycle_trap_id, &recycle_trap_type));
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, recycle_trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Recycle Ingress trap name");
        PRT_CELL_SET("bcmRxTrap%s", trap_name);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Recycle strength");
        PRT_CELL_SET("%d", recycle_strength);
    }

    if (trap_class == DNX_RX_TRAP_CLASS_OAM)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Ingress trap id stamped on FHEI");
        PRT_CELL_SET("0x%x", trap_config_p->stamped_trap_code);
    }

    if (trap_config_p->is_recycle_append_ftmh)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Append FTMH on recycled packet");
        PRT_CELL_SET("Enabled");
    }

    if (trap_config_p->is_recycle_crop_pkt)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Crop recycled packet");
        PRT_CELL_SET("Enabled");
    }

    if (trap_config_p->is_recycle_high_priority)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("High priority recycled packet");
        PRT_CELL_SET("Enabled");
    }

    if (trap_config_p->pp_drop_reason > 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("PP drop reason");
        PRT_CELL_SET("%d", trap_config_p->pp_drop_reason);
    }

    if (trap_config_p->flags & BCM_RX_TRAP_COPY_DROP)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Drop recycle copy");
        PRT_CELL_SET("TRUE");
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function for action info.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_action_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_block_e trap_block;
    bcm_rx_trap_t trap_type;
    uint8 is_allocated;
    int32 trap_id;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", trap_id);

    if (trap_id < 0)
    {
        LOG_CLI_EX("Invalid trap id%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    trap_block = DNX_RX_TRAP_ID_BLOCK_GET(trap_id);
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, trap_id, &trap_type));

    SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, trap_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
        LOG_CLI_EX("bcmRxTrap%s trap id=0x%x is not configured %s%s\n", trap_name, trap_id, EMPTY, EMPTY);
    }
    else
    {
        bcm_rx_trap_config_t_init(&trap_config);
        SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));

        switch (trap_block)
        {
            case DNX_RX_TRAP_BLOCK_INGRESS:
                SHR_IF_ERR_EXIT(dnx_trap_action_info_irpp_print
                                (unit, "IRPP Trap Action", trap_type, trap_id, &trap_config, sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_ERPP:
                if (DNX_RX_TRAP_ID_CLASS_GET(trap_id) != DNX_RX_TRAP_CLASS_USER_DEFINED)
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid trap_id=%d for ERPP block%s%s\n", trap_id, EMPTY, EMPTY);
                }
                SHR_IF_ERR_EXIT(dnx_trap_action_info_erpp_print(unit, "", trap_type, trap_id, &trap_config,
                                                                sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_ETPP:
                SHR_IF_ERR_EXIT(dnx_trap_action_info_etpp_print(unit, "", trap_type, trap_id, &trap_config,
                                                                sand_control));
                break;
            default:
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_trap_action_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"INFo", sh_dnx_trap_action_info_cmd, NULL, dnx_trap_action_info_options, &dnx_trap_action_info_man}
    ,
    {NULL}
};
