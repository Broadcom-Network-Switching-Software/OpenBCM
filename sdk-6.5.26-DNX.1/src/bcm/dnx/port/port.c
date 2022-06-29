/*
 ** \file port.c $Id$ PORT procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ptp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_visibility.h>
#include "src/bcm/dnx/flow/porting/port_pp_flow.h"

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_prt_tcam.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_stat_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_link_dispatch.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx/field/field_init.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/link/link.h>
#include <bcm_int/dnx/cosq/flow_control/flow_control_imp.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/port/port.h>
#include "port_utils.h"
#include <bcm_int/dnx/ptp/ptp.h>
#include <bcm_int/common/link.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/port/port_signal_quality.h>
#include <bcm_int/dnx/tsn/port_preemption.h>
#include "port_ingr_reassembly.h"

#if defined(INCLUDE_GDPLL)
#include <bcm/gdpll.h>
#endif
/*
 * }
 */

/*
 * Macros.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */
/** Default value for PRT PTCH1 layer offset */
#define DNX_PORT_PRT_PTCH1_DEFAULT_LAYER_OFFSET 3
/** Default value for PRT PTCH2 layer offset */
#define DNX_PORT_PRT_PTCH2_DEFAULT_LAYER_OFFSET 2
/** Number of bytes that FFC is able to read from the header */
#define DNX_PORT_PRT_FFC_BUFFER_SIZE_TO_READ 32
/** Used in first header size template manager to indicate that a certain profile is not used for skipping first header size before dedicated header*/
#define DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_DISABLE_SKIP_BEFORE 0
/** Used in first header size template manager to indicate that a certain profile is used for skipping first header size before dedicated header*/
#define DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_ENABLE_SKIP_BEFORE 1
/** Used in first header size template manager to indicate that a certain profile is not used for skipping first header size after dedicated header*/
#define DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_DISABLE_SKIP_AFTER 0
/** Used in first header size template manager to indicate that a certain profile is used for skipping first header size after dedicated header*/
#define DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_ENABLE_SKIP_AFTER 1
#define DNX_PORT_TX_TIMESTAMP_ADJUST_MAX 0x3ffff
/*
 * }
 */

/*
 * Typedefs
 * {
 */
/*
 * Rollback entry for linkscan mode
 */
typedef struct dnx_port_er_linkscan_mode_rollback_s
{
    /** logical port ID */
    bcm_port_t logical_port;
    /** Linkscan mode */
    int linkscan_mode;
} dnx_port_er_linkscan_mode_rollback_t;

/*
 * }
 */
/**
 * \brief - Verify Port, pclass and class_id parameters for BCM-API: bcm_dnx_port_class_get
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Physical port.
 * \param [in] pclass - Typedef enum value for specific port configurations. See .h file.
 * \param [out] class_id - Class id.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_class_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 *class_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(class_id, _SHR_E_PARAM, "class_id");

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the value of  orientation_group
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] orientation_group - Orientation group value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_class_ingress_orientation_set_verify(
    int unit,
    bcm_gport_t port,
    int orientation_group)
{
    int in_lif_profile_orientation_number_of_values;
    SHR_FUNC_INIT_VARS(unit);

    in_lif_profile_orientation_number_of_values = dnx_data_lif.in_lif.in_lif_profile_allocate_orientation_get(unit);
    if ((utilex_msb_bit_on(in_lif_profile_orientation_number_of_values) + 1) <
        (utilex_msb_bit_on(orientation_group) + 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough bits for orientation were allocated!\n");
    }

    if (orientation_group >= in_lif_profile_orientation_number_of_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The value of orientation group should be smaller than %d! The suported values are from 0 to %d\n",
                     in_lif_profile_orientation_number_of_values, (in_lif_profile_orientation_number_of_values - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  Set ingress Lif orientation value
 */
static shr_error_e
dnx_port_class_ingress_orientation_set(
    int unit,
    bcm_gport_t port,
    int orientation)
{
    int old_in_lif_profile = 0, new_in_lif_profile = 0;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    dnx_in_lif_profile_last_info_t last_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_class_ingress_orientation_set_verify(unit, port, orientation));

    /** Flow-lif procedures */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
        SHR_IF_ERR_EXIT(dnx_port_class_flow_lif_property_set(unit, port, bcmPortClassForwardIngress, orientation));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
    /** Allocate handle to the inlif table to get the in_lif_profile value*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &old_in_lif_profile));

    /** Get the data stored for this in_lif_profile*/
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                    (unit, old_in_lif_profile, &in_lif_profile_info, gport_hw_resources.inlif_dbal_table_id, LIF));
    in_lif_profile_info.egress_fields.in_lif_orientation = orientation;

    /** Exchange for new in_lif_profile, based on the new orientation*/
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     gport_hw_resources.inlif_dbal_table_id, &last_profile));

    /** Update inLIF table with the new in_lif_profile */
    if (old_in_lif_profile != new_in_lif_profile)
    {
        /** Update the LIF table with the new in_lif_profile*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, new_in_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_destroy(unit, last_profile, LIF));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get ingress Lif orientation value
 */
shr_error_e static
dnx_port_class_ingress_orientation_get(
    int unit,
    bcm_gport_t port,
    uint32 *orientation)
{
    int in_lif_profile = 0;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Flow-lif procedures */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
        SHR_IF_ERR_EXIT(dnx_port_class_flow_lif_property_get(unit, port, bcmPortClassForwardIngress, orientation));
        SHR_EXIT();
    }

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &in_lif_profile));

    /** Based on the inlif profile - get the orientation*/
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                    (unit, in_lif_profile, &in_lif_profile_info, gport_hw_resources.inlif_dbal_table_id, LIF));
    *orientation = in_lif_profile_info.egress_fields.in_lif_orientation;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get egress Lif orientation value
 */
shr_error_e static
dnx_port_class_egress_orientation_get(
    int unit,
    bcm_gport_t port,
    uint8 is_phy_port,
    uint32 *orientation)
{
    uint32 entry_handle_id = 0, local_out_lif, dbal_table_id, result_type, out_lif_profile_get = 0;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int out_lif_profile = 0;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TUNNEL(port))
    {
        /** Flow-lif procedures */
        if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
        {
            SHR_IF_ERR_EXIT(dnx_port_class_flow_lif_property_get(unit, port, bcmPortClassForwardEgress, orientation));
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
        local_out_lif = gport_hw_resources.local_out_lif;
        dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
        result_type = gport_hw_resources.outlif_dbal_result_type;

        /** Allocate handle to the outlif table and get the outlif profile*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile_get));
    }
    else if ((is_phy_port == TRUE) || BCM_GPORT_IS_LOCAL(port) || BCM_GPORT_IS_SYSTEM_PORT(port))
    {
        /** Get pp port */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        dbal_table_id = DBAL_TABLE_EGRESS_PP_PORT;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[0]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[0]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_OUTLIF_PROFILE, INST_SINGLE,
                                   &out_lif_profile_get);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    out_lif_profile = (int) out_lif_profile_get;

    /** Get the allocated outlif profile info based on the outlif profile*/
    etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    *orientation = out_lif_profile_info.out_lif_orientation;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Verify function for dnx_port_class_egress_orientation_set
 */
shr_error_e static
dnx_port_class_egress_orientation_set_verify(
    int unit,
    bcm_gport_t port,
    int orientation_group)
{
    int out_lif_profile_orientation_number_of_values;
    SHR_FUNC_INIT_VARS(unit);

    out_lif_profile_orientation_number_of_values = dnx_data_lif.out_lif.nof_out_lif_outgoing_orientation_get(unit);

    if (orientation_group < 0 || orientation_group >= out_lif_profile_orientation_number_of_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong orientation value is used: %d! The value of egress orientation group can be from 0 to %d\n",
                     orientation_group, (out_lif_profile_orientation_number_of_values - 1));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 *  Set egress Lif orientation value
 */
shr_error_e static
dnx_port_class_egress_orientation_set(
    int unit,
    bcm_gport_t port,
    uint8 is_phy_port,
    int orientation)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    uint32 local_out_lif = 0;
    uint32 entry_handle_id;
    uint32 res_type = 0;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int old_out_lif_profile = 0, new_out_lif_profile = 0;
    uint32 old_out_lif_profile_get = 0, pp_port_index = 0;
    uint8 is_last = 0;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_class_egress_orientation_set_verify(unit, port, orientation));

    if (BCM_GPORT_IS_TUNNEL(port))
    {
        /** Flow-lif procedures */
        if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
        {
            SHR_IF_ERR_EXIT(dnx_port_class_flow_lif_property_set(unit, port, bcmPortClassForwardEgress, orientation));
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));

        local_out_lif = gport_hw_resources.local_out_lif;
        dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
        res_type = gport_hw_resources.outlif_dbal_result_type;
    }
    else if ((is_phy_port == TRUE) || BCM_GPORT_IS_LOCAL(port) || BCM_GPORT_IS_SYSTEM_PORT(port))
    {
        /** Get pp port */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        dbal_table_id = DBAL_TABLE_EGRESS_PP_PORT;
    }
    /** Allocate handle to the oulif table and get the outlif profile data*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /** write map value  */
    if (BCM_GPORT_IS_TUNNEL(port))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        /** read outlif profile for the given LIF */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &old_out_lif_profile_get));
    }
    else if ((is_phy_port == TRUE) || BCM_GPORT_IS_LOCAL(port) || BCM_GPORT_IS_SYSTEM_PORT(port))
    {

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[0]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[0]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_OUTLIF_PROFILE, INST_SINGLE,
                                   &old_out_lif_profile_get);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    old_out_lif_profile = (int) old_out_lif_profile_get;

    /** Get the allocated outlif profile info based on the outlif profile*/
    etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, old_out_lif_profile, &out_lif_profile_info));
    out_lif_profile_info.out_lif_orientation = orientation;
    /** Exchange the outlif profile info according to the input parameters*/
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, dbal_table_id, 0, &out_lif_profile_info, old_out_lif_profile, &new_out_lif_profile,
                     &is_last));
    if (BCM_GPORT_IS_TUNNEL(port))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                     new_out_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    else if ((is_phy_port == TRUE) || BCM_GPORT_IS_LOCAL(port) || BCM_GPORT_IS_SYSTEM_PORT(port))
    {
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_OUTLIF_PROFILE, INST_SINGLE,
                                         new_out_lif_profile);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    if (is_last)
    {
        /** Clear the old ETPP outlif profile as there are no more references to it*/
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_hw_clear(unit, old_out_lif_profile));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get port classification ID to aggregate a group of ports for further processing such as Vlan
 *  translation and field processing.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Physical port
 * \param [in] pclass - Typedef enum value for specific port configurations. See .h file.
 * \param [out] class_id - Class id.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_class_get(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 *class_id)
{
    uint8 is_phy_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_class_get_verify(unit, port, pclass, class_id));

    /** Check if the port is physical */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));

    /*
     * Check pclass, for each class call appropriate function
     */
    /*
     * Note: Do not add code inside the case!!! Only through separate function!
     */
    switch (pclass)
    {
        case bcmPortClassId:
        {
            if (is_phy_port != FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "GPORT 0x%x is not physical, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassIngress:
        {
            if (BCM_GPORT_IS_TUNNEL(port) || BCM_GPORT_IS_MPLS_PORT(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_ingress_vlan_domain_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port 0x%x is not tunnel/mpls GPORT, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassEgress:
        {
            if (BCM_GPORT_IS_TUNNEL(port) || BCM_GPORT_IS_MPLS_PORT(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_egress_vlan_domain_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port 0x%x is not tunnel/mpls GPORT, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassVlanMember:
        {
            if (is_phy_port != FALSE)
            {
                /** Get Port + Core */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_membership_if_get
                                (unit, gport_info.internal_port_pp_info.core_id[0],
                                 gport_info.internal_port_pp_info.pp_port[0], class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "GPORT 0x%x is not physical, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassFieldIngressVport:
        case bcmPortClassFieldEgressVport:
        case bcmPortClassFieldIngressPMF1PacketProcessingPort:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldIngressPMF3PacketProcessingPort:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldEgressPacketProcessingPort:
        case bcmPortClassFieldEgressPacketProcessingPortCs:
        case bcmPortClassFieldExternalPacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1TrafficManagementPort:
        case bcmPortClassFieldIngressPMF1TrafficManagementPortCs:
        case bcmPortClassFieldIngressPMF3TrafficManagementPort:
        case bcmPortClassFieldIngressPMF3TrafficManagementPortCs:
        case bcmPortClassFieldEgressTrafficManagementPort:
        case bcmPortClassFieldEgressTrafficManagementPortCs:
        case bcmPortClassFieldExternalTrafficManagementPortCs:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_gport_get(unit, pclass, port, class_id));
            break;
        }
        case bcmPortClassForwardIngress:
        {
            if (BCM_GPORT_IS_TUNNEL(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_class_ingress_orientation_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_get for port %d is not supported\n", port);
            }
            break;
        }
        case bcmPortClassMplsForwardIngress:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_vmpi_get(unit, port, class_id));
            break;
        }
        case bcmPortClassForwardEgress:
        {
            if (dnx_data_port_pp.
                general.feature_get(unit, dnx_data_port_pp_general_split_horizon_per_port_outlif_profile_support))
            {
                if (BCM_GPORT_IS_TUNNEL(port) || (is_phy_port == TRUE) || BCM_GPORT_IS_LOCAL(port)
                    || BCM_GPORT_IS_SYSTEM_PORT(port))
                {
                    SHR_IF_ERR_EXIT(dnx_port_class_egress_orientation_get(unit, port, is_phy_port, class_id));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_get for port %d is not supported\n", port);

                }
            }
            else
            {
                if (BCM_GPORT_IS_TUNNEL(port))
                {
                    SHR_IF_ERR_EXIT(dnx_port_class_egress_orientation_get(unit, port, is_phy_port, class_id));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_get for port %d is not supported\n", port);
                }
            }
            break;
        }
        default:
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify Port, pclass and class_id parameters for
 * BCM-API: bcm_dnx_port_class_set()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *  Relevant unit.
 *   \param [in] port -
 *  Port - physical port
 *   \param [in] pclass -
 *  Typedef enum value for specific port configurations. See .h
 *   \param [in] class_id -
 *   Class id.
 * \par INDIRECT INPUT:
     None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
     None
 * \remark
     None
 * \see
     None
 */
static shr_error_e
dnx_port_class_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 class_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PORT_TERMINATION_PTC_PROFILE in INGRESS_PTC_PORT
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port
 * \param [in] ptc_profile - PTC profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_termination_ptc_profile_set(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tm_core_id, ptc;
    bcm_port_t local_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    /** Set the prt ptc_profile per tm port */
    BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
    {
        SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, (bcm_core_t *) & tm_core_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, INST_SINGLE,
                                     ptc_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update PRT configuration
 * Update layer offset in case of ETH port
 * Update layer offset, FFC0, FFC 1 and FFC2 in case of PTCH2 port
 *
 * \param [in] unit - Relevant unit
 * \param [in] prt_configuration - Pointer to entry from PRT dnxdata table
 * \param [in] ptc_profile - PRT PTC profile that is used for configuring INGRESS_PRT_INFO
 * \param [in] switch_header_type - Switch port header type according to BCM_SWITCH_PORT_HEADER_TYPE_*
 * \param [in] skip_first_header_before - If it is set, means to skip first header before certain header
 * \param [in] first_header_size_to_skip - First header size to skip value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_ingress_prt_user_defined_profile_configuration(
    int unit,
    dnx_port_prt_configuration_t * prt_configuration,
    dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile,
    int switch_header_type,
    uint32 skip_first_header_before,
    uint32 first_header_size_to_skip)
{
    uint32 entry_handle_id;
    uint32 layer_offset;
    uint32 ffc_0_input_offset, ffc_1_input_offset, ffc_2_input_offset, ffc_4_input_offset, ffc_5_input_offset;
    uint32 first_header_size_to_skip_in_bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

     /** Update layer_offset  */
    layer_offset = prt_configuration->layer_offset + first_header_size_to_skip;

    /*
     * For PTCH1 and PTCH2 ports should be updated not only the layer offset
     * For PTCH1 ports should be updated FFC0, FFC1, FFC2, FFC4 and FFC5 values
     * For PTCH2 ports should be updated FFC0, FFC1, FFC2 values
     */
    if (((skip_first_header_before != 0) && (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED))
        || (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
        || (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2))
    {
        /** Convert the value of first_header_size_to_skip in bits as FFC is according to bits */
        first_header_size_to_skip_in_bits = BYTES2BITS(first_header_size_to_skip);
        ffc_0_input_offset = prt_configuration->ffc_0_input_offset - first_header_size_to_skip_in_bits;
        ffc_1_input_offset = prt_configuration->ffc_1_input_offset - first_header_size_to_skip_in_bits;
        ffc_2_input_offset = prt_configuration->ffc_2_input_offset - first_header_size_to_skip_in_bits;
        if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
        {
            ffc_4_input_offset = prt_configuration->ffc_4_input_offset - first_header_size_to_skip_in_bits;
            ffc_5_input_offset = prt_configuration->ffc_5_input_offset - first_header_size_to_skip_in_bits;
        }
        else
        {
            ffc_4_input_offset = prt_configuration->ffc_4_input_offset;
            ffc_5_input_offset = prt_configuration->ffc_5_input_offset;
        }
    }
    else
    {
        ffc_0_input_offset = prt_configuration->ffc_0_input_offset;
        ffc_1_input_offset = prt_configuration->ffc_1_input_offset;
        ffc_2_input_offset = prt_configuration->ffc_2_input_offset;
        ffc_4_input_offset = prt_configuration->ffc_4_input_offset;
        ffc_5_input_offset = prt_configuration->ffc_5_input_offset;
    }

    /** Update layer_offset, FFC0, FFC1 and FFC2 values in INGRESS_PRT_INFO table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE,
                               prt_configuration->recycle_cmd_prt_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, ptc_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE, layer_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_MODE, INST_SINGLE,
                                 prt_configuration->prt_qualifier_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_KBR_KEY, INST_SINGLE,
                                 prt_configuration->prt_qualifier_kbr_key);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARSER_CONTEXT_KBR_KEY, INST_SINGLE,
                                 prt_configuration->parser_context_kbr_key);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT_KBR_KEY, INST_SINGLE,
                                 prt_configuration->source_system_port_kbr_key);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VP_TCAM_OR_PP_PORT_DIRECT_KBR_KEY, INST_SINGLE,
                                 prt_configuration->vp_tcam_or_pp_port_direct_kbr_key);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARSING_START_OFFSET_KBR_KEY, INST_SINGLE,
                                 prt_configuration->parsing_start_offset_kbr_key);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_5_OFFSET, INST_SINGLE,
                                 prt_configuration->ffc_5_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_5_WIDTH, INST_SINGLE,
                                 prt_configuration->ffc_5_width);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_5_INPUT_OFFSET, INST_SINGLE, ffc_5_input_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_4_OFFSET, INST_SINGLE,
                                 prt_configuration->ffc_4_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_4_WIDTH, INST_SINGLE,
                                 prt_configuration->ffc_4_width);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_4_INPUT_OFFSET, INST_SINGLE, ffc_4_input_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_3_OFFSET, INST_SINGLE,
                                 prt_configuration->ffc_3_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_3_WIDTH, INST_SINGLE,
                                 prt_configuration->ffc_3_width);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_3_INPUT_OFFSET, INST_SINGLE,
                                 prt_configuration->ffc_3_input_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_2_OFFSET, INST_SINGLE,
                                 prt_configuration->ffc_2_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_2_WIDTH, INST_SINGLE,
                                 prt_configuration->ffc_2_width);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_2_INPUT_OFFSET, INST_SINGLE, ffc_2_input_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_1_OFFSET, INST_SINGLE,
                                 prt_configuration->ffc_1_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_1_WIDTH, INST_SINGLE,
                                 prt_configuration->ffc_1_width);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_1_INPUT_OFFSET, INST_SINGLE, ffc_1_input_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_0_OFFSET, INST_SINGLE,
                                 prt_configuration->ffc_0_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_0_WIDTH, INST_SINGLE,
                                 prt_configuration->ffc_0_width);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_0_INPUT_OFFSET, INST_SINGLE, ffc_0_input_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT_PRT_MODE, INST_SINGLE,
                                 prt_configuration->source_system_port_prt_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT_ENABLE, INST_SINGLE,
                                 prt_configuration->pp_port_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_MODE_FULL_ENABLE, INST_SINGLE,
                                 prt_configuration->tcam_mode_full_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARSER_CONTEXT_SELECT, INST_SINGLE,
                                 prt_configuration->context_prt_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PRT configuration from DBAL
 *
 * \param [in] unit - Relevant unit
 * \param [in] port_termination_ptc_profile - Port Termination PTC profile
 * \param [in] recycle_cmd_prt_profile - Recycle command PRT profile
 * \param [out] prt_configuration - PRT configuration
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_prt_configuration_get(
    int unit,
    dbal_enum_value_field_port_termination_ptc_profile_e port_termination_ptc_profile,
    dbal_enum_value_field_recycle_cmd_prt_profile_e recycle_cmd_prt_profile,
    dnx_port_prt_configuration_t * prt_configuration)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    prt_configuration->port_termination_ptc_profile = port_termination_ptc_profile;
    prt_configuration->recycle_cmd_prt_profile = recycle_cmd_prt_profile;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE,
                               prt_configuration->recycle_cmd_prt_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE,
                               prt_configuration->port_termination_ptc_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_0_OFFSET, INST_SINGLE, &prt_configuration->ffc_0_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_0_WIDTH, INST_SINGLE, &prt_configuration->ffc_0_width));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_0_INPUT_OFFSET, INST_SINGLE,
                     &prt_configuration->ffc_0_input_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_1_OFFSET, INST_SINGLE, &prt_configuration->ffc_1_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_1_WIDTH, INST_SINGLE, &prt_configuration->ffc_1_width));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_1_INPUT_OFFSET, INST_SINGLE,
                     &prt_configuration->ffc_1_input_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_2_OFFSET, INST_SINGLE, &prt_configuration->ffc_2_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_2_WIDTH, INST_SINGLE, &prt_configuration->ffc_2_width));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_2_INPUT_OFFSET, INST_SINGLE,
                     &prt_configuration->ffc_2_input_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_3_OFFSET, INST_SINGLE, &prt_configuration->ffc_3_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_3_WIDTH, INST_SINGLE, &prt_configuration->ffc_3_width));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_3_INPUT_OFFSET, INST_SINGLE,
                     &prt_configuration->ffc_3_input_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_4_OFFSET, INST_SINGLE, &prt_configuration->ffc_4_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_4_WIDTH, INST_SINGLE, &prt_configuration->ffc_4_width));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_4_INPUT_OFFSET, INST_SINGLE,
                     &prt_configuration->ffc_4_input_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_5_OFFSET, INST_SINGLE, &prt_configuration->ffc_5_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_5_WIDTH, INST_SINGLE, &prt_configuration->ffc_5_width));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_5_INPUT_OFFSET, INST_SINGLE,
                     &prt_configuration->ffc_5_input_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PARSING_START_OFFSET_KBR_KEY, INST_SINGLE,
                     &prt_configuration->parsing_start_offset_kbr_key));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VP_TCAM_OR_PP_PORT_DIRECT_KBR_KEY, INST_SINGLE,
                     &prt_configuration->vp_tcam_or_pp_port_direct_kbr_key));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT_KBR_KEY, INST_SINGLE,
                     &prt_configuration->source_system_port_kbr_key));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PARSER_CONTEXT_KBR_KEY, INST_SINGLE,
                     &prt_configuration->parser_context_kbr_key));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_KBR_KEY, INST_SINGLE,
                     &prt_configuration->prt_qualifier_kbr_key));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PP_PORT_ENABLE, INST_SINGLE,
                     &prt_configuration->pp_port_enable));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT_PRT_MODE, INST_SINGLE,
                     &prt_configuration->source_system_port_prt_mode));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PARSER_CONTEXT_SELECT, INST_SINGLE,
                     &prt_configuration->context_prt_mode));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_MODE, INST_SINGLE,
                     &prt_configuration->prt_qualifier_mode));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TCAM_MODE_FULL_ENABLE, INST_SINGLE,
                     &prt_configuration->tcam_mode_full_enable));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE, &prt_configuration->layer_offset));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear a user defined PRT profile from INGRESS_PRT_INFO
 *
 * \param [in] unit - Relevant unit
 * \param [in] port_termination_ptc_profile - Port termination PTC profile
 * \param [in] recycle_cmd_prt_profile - Recycle CMD PRT profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_ingress_prt_user_defined_profile_clear(
    int unit,
    dbal_enum_value_field_port_termination_ptc_profile_e port_termination_ptc_profile,
    dbal_enum_value_field_recycle_cmd_prt_profile_e recycle_cmd_prt_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));
    if (recycle_cmd_prt_profile == -1)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE,
                                         DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, recycle_cmd_prt_profile);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE,
                               port_termination_ptc_profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update user defined PRT profile
 *
 * \param [in] unit - Relevant unit
 * \param [in] ptc_profile - Port termination PTC profile
 * \param [in] switch_header_type - Port header type according to BCM_SWITCH_PORT_HEADER_TYPE_*
 * \param [in] skip_first_header_before - If set to '1' means that the configuration is for skipping header before dedicated header. If set to '0' means skipping after dedicated header.
 * \param [in] first_header_size_to_skip - First header size to skip value
 * \param [in] clear_profile - Indication if to clear the existing entry from INGRESS_PRT_INFO. '1' means clear
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prt_profiles_update(
    int unit,
    dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile,
    int switch_header_type,
    uint32 skip_first_header_before,
    uint32 first_header_size_to_skip,
    uint8 clear_profile)
{
    dnx_port_prt_configuration_t prt_configuration;
    dbal_enum_value_field_recycle_cmd_prt_profile_e recycle_cmd_prt_profile =
        DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
    dbal_enum_value_field_port_termination_ptc_profile_e port_termination_ptc_profile =
        DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
    uint32 max_recycle_cmd_prt_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&prt_configuration, 0, sizeof(dnx_port_prt_configuration_t));

    if (!clear_profile)
    {
        if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
        {
            port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        }
        else if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
        {
            port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1;
        }
        else if ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
                 || (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2))
        {
            port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2;
        }
        if (dnx_data_headers.
            system_headers.feature_get(unit, dnx_data_headers_system_headers_system_headers_ibch1_supported))
        {
            max_recycle_cmd_prt_profile = DBAL_NOF_ENUM_RECYCLE_CMD_PRT_PROFILE_VALUES;
        }
        else
        {
            max_recycle_cmd_prt_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE + 1;
        }
        for (recycle_cmd_prt_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
             recycle_cmd_prt_profile < max_recycle_cmd_prt_profile; recycle_cmd_prt_profile++)
        {
            /** Get existing prt configuration from DBAL */
            SHR_IF_ERR_EXIT(dnx_port_prt_configuration_get
                            (unit, port_termination_ptc_profile, recycle_cmd_prt_profile, &prt_configuration));

            /** Write to INGRESS_PRT_PROFILE table */
            SHR_IF_ERR_EXIT(dnx_port_ingress_prt_user_defined_profile_configuration
                            (unit, &prt_configuration, ptc_profile, switch_header_type, skip_first_header_before,
                             first_header_size_to_skip));
        }
    }
    else
    {
        if (dnx_data_headers.
            system_headers.feature_get(unit, dnx_data_headers_system_headers_system_headers_ibch1_supported))
        {
            recycle_cmd_prt_profile = -1;
        }

        /** Clear the PRT entry from INGRESS_PRT_INFO */
        SHR_IF_ERR_EXIT(dnx_port_ingress_prt_user_defined_profile_clear(unit, ptc_profile, recycle_cmd_prt_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify first_header_size_to_skip_value
 *   parameter for dnx_port_first_header_size_to_skip_set
 */
static shr_error_e
dnx_port_first_header_size_to_skip_set_verify(
    int unit,
    bcm_port_t port,
    int first_header_size_to_skip_value)
{
    int switch_header_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!(BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid port %d! It should be used local port or trunk member!\n", port);
    }

    if (first_header_size_to_skip_value > dnx_data_port.general.max_first_header_size_to_skip_before_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The value exceeds the maximum number of bytes that can be skipped. Configured: %d, maximum %d.",
                     first_header_size_to_skip_value,
                     dnx_data_port.general.max_first_header_size_to_skip_before_get(unit));
    }

    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, port, TRUE, &switch_header_type));
    if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
    {
        if (first_header_size_to_skip_value >
            DNX_PORT_PRT_FFC_BUFFER_SIZE_TO_READ - DNX_PORT_PRT_PTCH1_DEFAULT_LAYER_OFFSET)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The value exceeds the maximum number of bytes that can be skipped. Configured: %d, maximum %d.",
                         first_header_size_to_skip_value,
                         DNX_PORT_PRT_FFC_BUFFER_SIZE_TO_READ - DNX_PORT_PRT_PTCH1_DEFAULT_LAYER_OFFSET);
        }
    }
    else if ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
             || ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)))
    {
        if (first_header_size_to_skip_value >
            DNX_PORT_PRT_FFC_BUFFER_SIZE_TO_READ - DNX_PORT_PRT_PTCH2_DEFAULT_LAYER_OFFSET)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The value exceeds the maximum number of bytes that can be skipped. Configured: %d, maximum %d.",
                         first_header_size_to_skip_value,
                         DNX_PORT_PRT_FFC_BUFFER_SIZE_TO_READ - DNX_PORT_PRT_PTCH2_DEFAULT_LAYER_OFFSET);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify first_header_size_to_skip_value
 *   parameter for dnx_port_first_header_size_to_skip_after_injected_set
 */
static shr_error_e
dnx_port_first_header_size_to_skip_after_injected_set_verify(
    int unit,
    bcm_port_t port,
    int first_header_size_to_skip_value)
{

    SHR_FUNC_INIT_VARS(unit);

    if (first_header_size_to_skip_value > dnx_data_port.general.max_first_header_size_to_skip_after_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The value exceeds the maximum number of bytes that can be skipped. Configured: %d, maximum %d.",
                     first_header_size_to_skip_value,
                     dnx_data_port.general.max_first_header_size_to_skip_after_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_first_header_size_to_skip_port_type_from_port_header_type_convert(
    int unit,
    bcm_port_t port,
    int switch_header_type,
    uint32 *user_defined_port_type)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (switch_header_type)
    {
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
        {
            *user_defined_port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_ETH;
            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED:
        {
            *user_defined_port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_PTCH1;
            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2:
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
        {
            *user_defined_port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_PTCH2;
            break;
        }
        default:
        {
            *user_defined_port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_NONE;
            break;
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_ingress_prt_info_layer_offset_get(
    int unit,
    dbal_enum_value_field_recycle_cmd_prt_profile_e prt_recycle_cmd,
    dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile,
    int *layer_offset)
{
    uint32 entry_handle_id;
    uint32 prt_layer_offset = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get layer_offset */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, prt_recycle_cmd);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, ptc_profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE, &prt_layer_offset);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *layer_offset = prt_layer_offset;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Verify first_header_size_to_skip_value
 *   parameter for dnx_port_first_header_size_to_skip_get
 */
static shr_error_e
dnx_port_first_header_size_to_skip_get_verify(
    int unit,
    bcm_port_t port,
    int *first_header_size_to_skip_value)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(first_header_size_to_skip_value, _SHR_E_PARAM, "first_header_size_to_skip_value");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the size of the first header offset to skip per ptc
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] first_header_size_value - The size of the first header offset to skip
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_first_header_size_to_skip_get(
    int unit,
    bcm_port_t port,
    int *first_header_size_value)
{
    uint32 entry_handle_id;
    uint32 layer_offset = 0;
    dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile = 0;
    int switch_header_type = 0;
    int recycle_command_prt_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_first_header_size_to_skip_get_verify(unit, port, first_header_size_value));

    /** Get the ptc profile */
    SHR_IF_ERR_EXIT(dnx_port_prt_ptc_profile_get(unit, port, &ptc_profile));
    /** Get in port header type */
    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, port, TRUE, &switch_header_type));

    if ((ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD1)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD2)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD3)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD4)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD5)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD6))
    {
        /** Get layer_offset */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE,
                                   recycle_command_prt_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, ptc_profile);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE, &layer_offset);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /** Calculate the size of the first header offset to skip */
        if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
        {
            *first_header_size_value = layer_offset;
        }
        else if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
        {
            *first_header_size_value = layer_offset - DNX_PORT_PRT_PTCH1_DEFAULT_LAYER_OFFSET;
        }
        else
        {
            *first_header_size_value = layer_offset - DNX_PORT_PRT_PTCH2_DEFAULT_LAYER_OFFSET;
        }
    }
    else
    {
        *first_header_size_value = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the size of the first header offset to skip after injected header
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] first_header_size_value - The size of the first header offset to skip after injected header
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_first_header_size_to_skip_after_injected_get(
    int unit,
    bcm_port_t port,
    int *first_header_size_value)
{
    uint32 entry_handle_id;
    uint32 layer_offset = 0;
    dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile = 0;
    int recycle_command_prt_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_first_header_size_to_skip_get_verify(unit, port, first_header_size_value));

    *first_header_size_value = 0;
    /** Get the ptc profile */
    SHR_IF_ERR_EXIT(dnx_port_prt_ptc_profile_get(unit, port, &ptc_profile));
    if ((ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD1)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD2)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD3)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD4)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD5)
        || (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD6))
    {
        /** Get layer_offset */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE,
                                   recycle_command_prt_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, ptc_profile);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE, &layer_offset);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /** Calculate the size of the first header offset to skip */
        *first_header_size_value = layer_offset - DNX_PORT_PRT_PTCH1_DEFAULT_LAYER_OFFSET;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure per port the first header offset to skip
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] first_header_size_value - The size (in bytes) of the first header offset to skip
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_first_header_size_to_skip_set(
    int unit,
    bcm_port_t port,
    uint32 first_header_size_value)
{
    int switch_header_type;
    uint8 first_reference = 0;
    uint8 last_reference = 0;
    int new_profile_index = 0;
    dbal_enum_value_field_port_termination_ptc_profile_e new_ptc_profile = 0;
    int old_profile_index = 0, ref_count = 0;
    dnx_algo_skip_first_header_info_t port_first_header_size_data, default_port_first_header_size_data;
    int rv = BCM_E_NONE;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_first_header_size_to_skip_set_verify(unit, port, first_header_size_value));

    port_first_header_size_data.is_before = DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_ENABLE_SKIP_BEFORE;
    port_first_header_size_data.is_after = DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_DISABLE_SKIP_AFTER;
    port_first_header_size_data.port_type = 0;

    /** Get the old port header type */
    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, port, TRUE, &switch_header_type));

    SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_get
                    (unit, port, (int *) &port_first_header_size_data.first_header_size_to_skip));

    if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
    {
        port_first_header_size_data.port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_ETH;
    }
    else if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
    {
        port_first_header_size_data.port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_PTCH1;
    }
    /** Relevant for INJECTED_2 and INJECTED_2_PP */
    else
    {
        port_first_header_size_data.port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_PTCH2;
    }

    rv = dnx_algo_port_db.skip_first_user_defined_header_size_profile_data.profile_get(unit,
                                                                                       &port_first_header_size_data,
                                                                                       &old_profile_index);
    if (rv != BCM_E_NONE)
    {
        old_profile_index = DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE;
    }

    port_first_header_size_data.first_header_size_to_skip = first_header_size_value;

    if (port_first_header_size_data.first_header_size_to_skip == 0)
    {
        new_profile_index = DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE;
        flags = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
        dnx_algo_port_db.skip_first_user_defined_header_size_profile_data.profile_data_get(unit,
                                                                                           DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE,
                                                                                           &ref_count,
                                                                                           &default_port_first_header_size_data);
        port_first_header_size_data = default_port_first_header_size_data;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_db.
                    skip_first_user_defined_header_size_profile_data.exchange(unit, flags, &port_first_header_size_data,
                                                                              old_profile_index, NULL,
                                                                              &new_profile_index, &first_reference,
                                                                              &last_reference));
    if (first_reference)
    {
         /** Update PRT configuration */
        SHR_IF_ERR_EXIT(dnx_port_prt_profiles_update
                        (unit, new_profile_index, switch_header_type, port_first_header_size_data.is_before,
                         first_header_size_value, 0));
    }

    if (new_profile_index == DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE)
    {
        if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
        {
            new_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        }
        else if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
        {
            new_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1;
        }
        else if ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
                 || (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP))
        {
            new_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2;
        }

    }
    else
    {
        new_ptc_profile = new_profile_index;
    }

    /** Update Port Termination PTC profile per PTC */
    SHR_IF_ERR_EXIT(dnx_port_termination_ptc_profile_set(unit, port, new_ptc_profile));

    if (last_reference)
    {
        /** Set user defined PRT configuration to its default values */
        SHR_IF_ERR_EXIT(dnx_port_prt_profiles_update
                        (unit, old_profile_index, switch_header_type, port_first_header_size_data.is_before, 0, 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure per port the first header offset to skip after injected header
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] first_header_size_value - The size (in bytes) of the first header offset to skip after injected header
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_first_header_size_to_skip_after_injected_set(
    int unit,
    bcm_port_t port,
    uint32 first_header_size_value)
{
    int switch_header_type = 0;
    uint8 first_reference = 0;
    uint8 last_reference = 0;
    int new_profile_index = 0;
    dbal_enum_value_field_port_termination_ptc_profile_e new_ptc_profile = 0;
    int old_profile = 0, ref_count = 0;
    dnx_algo_skip_first_header_info_t port_first_header_size_data, default_port_first_header_size_data;
    int rv = BCM_E_NONE;
    int default_first_header_size = 0;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_first_header_size_to_skip_after_injected_set_verify
                           (unit, port, first_header_size_value));

    port_first_header_size_data.is_before = DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_DISABLE_SKIP_BEFORE;
    port_first_header_size_data.is_after = DNX_PORT_SKIP_FIRST_HEADER_SIZE_DATA_ENABLE_SKIP_AFTER;
    port_first_header_size_data.port_type = DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_PTCH1;

    /** Get the in port header type */
    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, port, TRUE, &switch_header_type));

    SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_after_injected_get(unit, port, &default_first_header_size));
    port_first_header_size_data.first_header_size_to_skip = default_first_header_size;
    /** Get the old ptc profile */
    rv = dnx_algo_port_db.skip_first_user_defined_header_size_profile_data.profile_get(unit,
                                                                                       &port_first_header_size_data,
                                                                                       &old_profile);
    if (rv != BCM_E_NONE)
    {
        old_profile = DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE;
    }
    if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
    {
        port_first_header_size_data.first_header_size_to_skip = first_header_size_value;
        if (port_first_header_size_data.first_header_size_to_skip == 0)
        {
            new_profile_index = DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE;
            flags = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
            dnx_algo_port_db.skip_first_user_defined_header_size_profile_data.profile_data_get(unit,
                                                                                               DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE,
                                                                                               &ref_count,
                                                                                               &default_port_first_header_size_data);
            port_first_header_size_data = default_port_first_header_size_data;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_db.skip_first_user_defined_header_size_profile_data.exchange(unit, flags,
                                                                                                   &port_first_header_size_data,
                                                                                                   old_profile,
                                                                                                   NULL,
                                                                                                   &new_profile_index,
                                                                                                   &first_reference,
                                                                                                   &last_reference));
    }

    if (first_reference)
    {
         /** Update PRT configuration */
        SHR_IF_ERR_EXIT(dnx_port_prt_profiles_update
                        (unit, new_profile_index, switch_header_type, port_first_header_size_data.is_before,
                         first_header_size_value, 0));
    }

    if (new_profile_index == DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE)
    {
        new_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1;
    }
    else
    {
        new_ptc_profile = new_profile_index;
    }

    /** Update Port Termination PTC profile per PTC */
    SHR_IF_ERR_EXIT(dnx_port_termination_ptc_profile_set(unit, port, new_ptc_profile));

    if (last_reference)
    {
        /** Set user defined PRT configuration to its default values */
        SHR_IF_ERR_EXIT(dnx_port_prt_profiles_update
                        (unit, old_profile, switch_header_type, port_first_header_size_data.is_before, 0, 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify ILKN port Rx scheduler priority
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] sch_prio - Scheduler priority
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_ilkn_rx_priority_verify(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t * sch_prio)
{
    bcm_port_t logical_port = 0;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
    {
        SHR_ERR_EXIT(BCM_E_PORT, "port control bcmPortControlRxPriority is supported only for ILKN ports\n");
    }

    if (sch_prio != NULL)
    {
        SHR_RANGE_VERIFY(*sch_prio, bcmPortNifSchedulerLow, bcmPortNifSchedulerHigh, _SHR_E_PARAM,
                         "ILKN scheduler priority can be either bcmPortNifSchedulerLow or bcmPortNifSchedulerHigh, but gut %d",
                         *sch_prio);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ILKN port Rx scheduler priority
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] sch_prio - Scheduler priority
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_ilkn_rx_priority_set(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio)
{
    int port_has_speed = 0;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_ilkn_rx_priority_verify(unit, port, &sch_prio));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /*
     * Set priority in sw state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_set(unit, logical_port, sch_prio));

    /*
     * Update priority reset in NMG, in case the reset is done per
     * ILKN core.
     */
    if (!dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
    {
        SHR_IF_ERR_EXIT(imb_port_scheduler_priority_update(unit, logical_port));
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &port_has_speed));
    if (port_has_speed)
    {
        /*
         * Set priority in HW
         */
        SHR_IF_ERR_EXIT(imb_port_scheduler_config_set(unit, logical_port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ILKN port Rx scheduler priority
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] sch_prio - Scheduler priority
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_ilkn_rx_priority_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t * sch_prio)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_ilkn_rx_priority_verify(unit, port, NULL));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /*
     * Get priority from sw state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, logical_port, sch_prio));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify port, type and for
 * BCM-API: bcm_port_control_set()/ bcm_port_control_get()
 *
 * \param [in] unit - unit id
 * \param [in] port - physical port for updating
 * \param [in] is_phy_port - If the port is a physical port
 * \param [in] type - bcm_port_control_t
 * \param [in] is_set - Boolean. is set operation
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_control_port_and_type_verify(
    int unit,
    bcm_port_t port,
    uint8 is_phy_port,
    bcm_port_control_t type,
    uint32 is_set)
{
    int system_headers_mode;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t local_port;
    dnx_algo_port_info_s port_info;
    int prd_enable_mode;
    int switch_header_type = 0;
    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    switch (type)
    {
        case bcmPortControlFloodUnknownUcastGroup:
        case bcmPortControlFloodUnknownMcastGroup:
        case bcmPortControlFloodBroadcastGroup:
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL,
                         "Port control (%d) is not supported. Call API bcm_port_flood_group_get instead!\n", type);
            break;
        }
        case bcmPortControlEgressFilterDisable:
        {
            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
            {
                SHR_ERR_EXIT(BCM_E_UNAVAIL,
                             "Port control (%d) is not supported when system_headers_mode = %d. \n", type,
                             system_headers_mode);
            }
            else if (is_phy_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "port: 0x%X, physical port is required!\n", port);
            }
            break;
        }
        case bcmPortControl1588P2PDelay:
        {
            if (is_phy_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "port: 0x%X, physical port is required!\n", port);
            }

            if (BCM_GPORT_IS_TRUNK(port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port: 0x%X, Trunk gport is not accepted", port);
            }
            break;
        }
        case bcmPortControlExtenderType:
        {
            /** Verify PRD is disabled for NIF ports */
            if (is_set == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

                BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, local_port, &port_info));
                    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF)))
                    {
                        SHR_IF_ERR_EXIT(imb_prd_enable_get(unit, local_port, &prd_enable_mode));
                        if (prd_enable_mode != bcmCosqIngressPortDropDisable)
                        {
                            SHR_ERR_EXIT(_SHR_E_CONFIG,
                                         "Extender type cannot be configured if PRD is enabled on local port %d.\nPlease disable PRD.\n",
                                         local_port);
                        }
                    }
                }
            }
            break;
        }
        case bcmPortControlFirstHeaderSize:
        {
            if (BCM_GPORT_IS_TRUNK(port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Trunk gport %d is not supported! The configuration should be per trunk member!\n", port);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, port, TRUE, &switch_header_type));
            }

            if ((switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH)
                && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
                && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
                && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "first_header_size cannot be configured if the IN port header type of port %d is different from ETH, INJECTED, INJECTED_2 or INJECTED_2_PP!\n",
                             port);
            }
            break;
        }
        case bcmPortControlFirstHeaderSizeAfterInjected:
        {
            if (BCM_GPORT_IS_TRUNK(port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Trunk gport %d is not supported! The configuration should be per trunk member!\n", port);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, port, TRUE, &switch_header_type));
            }

            if (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "first_header_size cannot be configured if the IN port header type of port %d is different from INJECTED!\n",
                             port);
            }
            break;
        }
        case bcmPortControlDiscardMacSaAction:
        {
            if (is_phy_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "port: 0x%X, physical port is required!\n", port);
            }
            break;
        }
        case bcmPortControlRecycleApp:
        {
            if (is_phy_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid port: %d, physical port is required!\n", port);
            }
            break;
        }
        case bcmPortControlIbch1Enable:
        {
            if (is_phy_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "port: 0x%X, physical port is required!\n", port);
            }
            if (is_set == TRUE)
            {
                if (!dnx_data_headers.
                    system_headers.feature_get(unit, dnx_data_headers_system_headers_system_headers_ibch1_supported))
                {
                    SHR_ERR_EXIT(_SHR_E_PORT, "set bcmPortControlIbch1Enable is not supported for current device \n");
                }
                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
                if (!DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
                {
                    SHR_ERR_EXIT(_SHR_E_PORT, "Invalid port type, must be CPU \n");
                }
            }
            break;
        }
        case bcmPortControlPFCReceive:
        case bcmPortControlPFCTransmit:
        case bcmPortControlPFCRefreshTime:
        case bcmPortControlPFCFromLLFCEnable:
        case bcmPortControlPFCStatus:
        {
            if (!BCM_GPORT_IS_LOCAL(port) && ((port < 0) || (port >= SOC_MAX_NUM_PORTS)))
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port!\n");
            }

            local_port = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, local_port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Port %d has invalid port type, must be NIF ETH!\n", local_port);
            }
            break;
        }
        case bcmPortControlLLFCReceive:
        case bcmPortControlLLFCTransmit:
        {
            if (!BCM_GPORT_IS_LOCAL(port) && ((port < 0) || (port >= SOC_MAX_NUM_PORTS)))
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port!\n");
            }

            local_port = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, local_port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)))
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Port %d has invalid port type, must be NIF ETH!\n", local_port);
            }
            break;
        }
        case bcmPortControlSignalQualityErDegradeThreshold:
        case bcmPortControlSignalQualityErDegradeRecoverThreshold:
        case bcmPortControlSignalQualityErFailThreshold:
        case bcmPortControlSignalQualityErFailRecoverThreshold:
        case bcmPortControlSignalQualityErMeasureMode:
        case bcmPortControlSignalQualityErInterval:
        case bcmPortControlSignalQualityErActionEnable:
        case bcmPortControlSignalQualityErHoldOffInterval:
        {
            dnx_algo_port_info_s port_info;

            /** get port type - can be either fabric or ilkn over fabric */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                SHR_IF_ERR_EXIT(soc_dnxc_port_signal_quality_fabric_feature_verify(unit));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_control_port_and_type_verify(unit, port, type));
            }
            break;
        }
        default:
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify port, type and value for
 * BCM-API: bcm_port_control_set()
 *
 * \param [in] unit - unit id
 * \param [in] port - physical port for updating
 * \param [in] is_phy_port - If the port is a physical port
 * \param [in] type - bcm_port_control_t
 * \param [in] value - changes according to the type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_control_set_verify(
    int unit,
    bcm_port_t port,
    uint8 is_phy_port,
    bcm_port_control_t type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_control_port_and_type_verify(unit, port, is_phy_port, type, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify port, type and value for
 * BCM-API: bcm_port_control_get()
 *
 * \param [in] unit - unit id
 * \param [in] port - physical port for updating
 * \param [in] is_phy_port - If the port is a physical port
 * \param [in] type - bcm_port_control_t
 * \param [in] *value - changes according to the type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_control_get_verify(
    int unit,
    bcm_port_t port,
    uint8 is_phy_port,
    bcm_port_control_t type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "A valid pointer is needed for values!");

    SHR_IF_ERR_EXIT(dnx_port_control_port_and_type_verify(unit, port, is_phy_port, type, FALSE));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set port classification ID to aggregate a
 *      group of ports for further processing such as Vlan
 *      translation and field processing.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Physical port.
 * \param [in] pclass - Typedef enum value for specific port configurations. See .h file.
 * \param [in] class_id - Class id.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_class_set(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 class_id)
{
    uint8 is_phy_port;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_class_set_verify(unit, port, pclass, class_id));

    /*
     * Check pclass, for each class call appropriate function
     */
    /*
     * Note: Do not add code inside the case!!! Only through separate function!
     */
    switch (pclass)
    {
        case bcmPortClassId:
        {

            /** Check if it is a physical port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
            /*
             * If the port is physical then configure pp port vlan domain
             */
            if (is_phy_port != FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The provided port is not physical and cannot be input to the function! Use bcmPortClassIngress or bcmPortClassEgress instead.\n");
            }
            break;
        }
        case bcmPortClassIngress:
        {
            /** Check if it is a physical port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
            if (is_phy_port == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_ingress_vlan_domain_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The provided port is physical and cannot be input to the function! Use bcmPortClassId instead. \n");
            }
            break;
        }
        case bcmPortClassEgress:
        {
            /** Check if it is a physical port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
            if (is_phy_port == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_egress_vlan_domain_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The provided port is physical and cannot be input to the function! Use bcmPortClassId instead.\n");
            }
            break;
        }
        case bcmPortClassVlanMember:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_membership_if_set(unit, port, class_id));
            break;
        }
        case bcmPortClassFieldIngressVport:
        case bcmPortClassFieldEgressVport:
        case bcmPortClassFieldIngressPMF1PacketProcessingPort:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldIngressPMF3PacketProcessingPort:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldEgressPacketProcessingPort:
        case bcmPortClassFieldEgressPacketProcessingPortCs:
        case bcmPortClassFieldExternalPacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1TrafficManagementPort:
        case bcmPortClassFieldIngressPMF1TrafficManagementPortCs:
        case bcmPortClassFieldIngressPMF3TrafficManagementPort:
        case bcmPortClassFieldIngressPMF3TrafficManagementPortCs:
        case bcmPortClassFieldEgressTrafficManagementPort:
        case bcmPortClassFieldEgressTrafficManagementPortCs:
        case bcmPortClassFieldExternalTrafficManagementPortCs:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_gport_set(unit, pclass, port, class_id));
            break;
        }
        case bcmPortClassForwardIngress:
        {
            if (BCM_GPORT_IS_TUNNEL(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_class_ingress_orientation_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_set for port %d is not supported\n", port);
            }
            break;
        }
        case bcmPortClassMplsForwardIngress:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_vmpi_set(unit, port, class_id));
            break;
        }
        case bcmPortClassForwardEgress:
        {
            /** Check if it is a physical port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));

            if (dnx_data_port_pp.
                general.feature_get(unit, dnx_data_port_pp_general_split_horizon_per_port_outlif_profile_support))
            {
                if (BCM_GPORT_IS_TUNNEL(port) || (is_phy_port == TRUE) || BCM_GPORT_IS_LOCAL(port)
                    || BCM_GPORT_IS_SYSTEM_PORT(port))
                {
                    SHR_IF_ERR_EXIT(dnx_port_class_egress_orientation_set(unit, port, is_phy_port, class_id));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_set for port %d is not supported\n", port);
                }
            }
            else
            {
                if (BCM_GPORT_IS_TUNNEL(port))
                {
                    SHR_IF_ERR_EXIT(dnx_port_class_egress_orientation_set(unit, port, is_phy_port, class_id));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_set for port %d is not supported\n", port);
                }
            }
            break;
        }
        default:
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  Enable Same interface Filter per port
 */
shr_error_e
dnx_port_same_interface_filter_set(
    int unit,
    bcm_port_t port,
    int filter_flags)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Analyze the filter flags
     */
    if ((filter_flags > 1) || (filter_flags < 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "value: 0x%X, Invalid filter flags!\n", filter_flags);
    }
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /** Set filter enablers to HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        /** Set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        /** Set vlan_domain */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_ENABLE, INST_SINGLE,
                                     filter_flags);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get the value of Same Interface Filter Enable/Disable per port
 */
static shr_error_e
dnx_port_same_interface_filter_get(
    int unit,
    bcm_port_t port,
    int *filter_flags)
{
    uint32 entry_handle_id;
    uint8 filter_flag;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /** Get the filter enablers from HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_ENABLE, INST_SINGLE, &filter_flag);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *filter_flags = filter_flag;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Enable/ Disable Split Horizon Filter per port
 */
shr_error_e static
dnx_port_split_horizon_set(
    int unit,
    bcm_port_t port,
    int filter_flags)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 set_val;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    set_val = (filter_flags == FALSE) ? 0 : 1;

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Set Split Horizon Enable/Disable for the PP port
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, set_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get Split Horizon Filter per port value
 */
shr_error_e static
dnx_port_split_horizon_get(
    int unit,
    bcm_port_t port,
    int *filter_flags)
{
    uint32 entry_handle_id, filter_flag;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Get Split Horizon Enable/Disable for the PP port
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, &filter_flag);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    *filter_flags = filter_flag;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the GPORT ID for the specified physical port
 *
 * \param [in] unit - unit ID
 * \param [in] port - logical port or gport 'local port'
 * \param [out] gport - MODPORT of the given gport
 * \return
 *     shr_err_e
 *
 * \remark
 *    * None
 * \see
 *    * None
 */
int
bcm_dnx_port_gport_get(
    int unit,
    bcm_port_t port,
    bcm_gport_t * gport)
{
    algo_gpm_gport_verify_type_e allowed_types[] =
        { ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_PORT };
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_core_t core_id;
    uint32 pp_dsp;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, port, COUNTOF(allowed_types), allowed_types));

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /** get modid and tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, gport_info.local_port, &core_id, &pp_dsp));

    SHR_IF_ERR_EXIT(dnx_stk_sys_pp_dsp_to_modport_gport_convert(unit, core_id, pp_dsp, gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_prt_virtual_port_tcam_mapping_set and dnx_port_prt_virtual_port_tcam_mapping_clear
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prt_virtual_port_tcam_mapping_port_verify(
    int unit,
    bcm_port_t port)
{
    int port_is_in_lag = 0;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    if (!((BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS))) || BCM_GPORT_IS_TRUNK(port)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Port %d is neither local nor trunk gport! It should be used local port or trunk gports!\n", port);
    }

    if (!BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is a STIF port! The API does not support STIF ports!\n", port);
        }
        /** Get information if the logical_port is trunk member */
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_is_in_lag));
        if (port_is_in_lag)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is trunk member! The API should be called with the trunk gport!\n",
                         port);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an entry to SW and HW PRT VIRTUAL PORT TCAM
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port or trunk gport
 * \param [in] core_id - Core ID
 * \param [in] pp_port - PP port
 * \param [in] system_port_aggregate - System port aggregate
 * \param [in] prttcam_key_vp_spa - Encoded key for PRT TCAM
 * \param [in] key_mask - Key mask for PRT TCAM entry
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prt_virtual_port_tcam_entry_add(
    int unit,
    bcm_port_t port,
    bcm_core_t core_id,
    uint32 pp_port,
    uint32 system_port_aggregate,
    uint32 prttcam_key_vp_spa,
    uint32 *key_mask)
{
    uint8 is_used = 0;
    uint32 sw_tcam_pp_port = 0;
    uint16 priority = 0, system_port = 0;
    uint32 entry_access_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_get
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp_spa, &entry_access_id, &is_used,
                     &priority, &sw_tcam_pp_port, &system_port, NULL));
    if (is_used)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is already mapped!\n", port);
    }

    /** Alloc access id in SW TCAM */
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_sw_access_id_alloc
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp_spa, &entry_access_id));
    /** Configure PRT_VIRTUAL_PORT_TCAM */
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_hw_set
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp_spa, key_mask, entry_access_id,
                     pp_port, 0, FC_CHAN_ID_INVALID));

    /** Update SW table for TCAM management */
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_set
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp_spa, entry_access_id, 0, pp_port,
                     system_port_aggregate, FC_CHAN_ID_INVALID, 0));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Allocate entry index in SW state for PRT_VIRTUAL_PORT_TCAM and add an entry to the hw in case that
 *  pp port and system ports are already mapped
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port or trunk gport
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prt_virtual_port_tcam_mapping_set(
    int unit,
    bcm_port_t port)
{
    uint32 complete_field_val = 0;
    bcm_core_t core_id = 0;
    uint32 pp_dsp;
    uint32 pp_port = 0, system_port = 0;
    uint32 pp_port_index = 0;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 key_mask = 0x0001FFFF;
    uint32 members_lsb = 0, members_msb = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_prt_virtual_port_tcam_mapping_port_verify(unit, port));

    /** Get system port */
    if (BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, port, 0, &system_port));
    }
    else
    {
         /** Get PP DSP*/
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, port, &core_id, &pp_dsp));
        SHR_IF_ERR_EXIT(dnx_port_dsp_table_mapping_get(unit, core_id, pp_dsp, (int *) &pp_port, &system_port));
    }

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, &system_port,
                     &complete_field_val));

    if (BCM_GPORT_IS_TRUNK(port))
    {
        /** Get trunk members msb and lsb bits from spa */
        SHR_IF_ERR_EXIT(dnx_trunk_utils_members_bits_get(unit, system_port, &members_lsb, &members_msb));
        /** Build key_mask */
         /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(&key_mask, members_lsb, members_msb));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_port_prt_virtual_port_tcam_entry_add
                            (unit, port, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], system_port, complete_field_val,
                             &key_mask));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_prt_virtual_port_tcam_entry_add
                        (unit, port, core_id, pp_port, system_port, complete_field_val, NULL));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map out-pp-port to correct esem-access-cmd for ipfix
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port or trunk gport
 * \param [in] value - Enable=1, Disable=0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_pp_ipfix_rx_set(
    int unit,
    bcm_port_t port,
    int value)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 pp_port;
    uint32 pp_port_index;
    uint32 esem_access_cmd;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    {
        if (value)
        {
            esem_access_cmd = dnx_data_esem.access_cmd.sflow_sample_interface_get(unit);
        }
        else
        {
            esem_access_cmd = dnx_data_esem.access_cmd.no_action_get(unit);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {

        core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_access_cmd);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get if ipfix is supported on out-pp-port
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port or trunk gport
 * \param [out] value - Enable=1, Disable=0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_pp_ipfix_rx_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 pp_port;
    uint32 esem_access_cmd;
    uint32 ipfix_esem_access_cmd;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    ipfix_esem_access_cmd = dnx_data_esem.access_cmd.sflow_sample_interface_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    core = gport_info.internal_port_pp_info.core_id[0];
    pp_port = gport_info.internal_port_pp_info.pp_port[0];

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, &esem_access_cmd);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (esem_access_cmd == ipfix_esem_access_cmd)
    {
        *value = 1;
    }
    else
    {
        *value = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an entry from SW and HW PRT VIRTUAL PORT TCAM
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port or trunk gport
 * \param [in] core_id - Core id
 * \param [in] pp_port - PP port
 * \param [in] system_port_aggregate - System port aggregate
 * \param [in] prttcam_key_vp_spa - Encoded key for PRT TCAM
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prt_virtual_port_tcam_entry_delete(
    int unit,
    bcm_port_t port,
    bcm_core_t core_id,
    uint32 pp_port,
    uint32 system_port_aggregate,
    uint32 prttcam_key_vp_spa)
{
    uint8 is_used = 0;
    uint32 sw_tcam_pp_port = 0;
    uint16 priority = 0, system_port = 0;
    uint32 entry_access_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_get
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp_spa, &entry_access_id, &is_used,
                     &priority, &sw_tcam_pp_port, &system_port, NULL));
    if (!is_used)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "A mapping for port %d does not exist! It cannot be cleared!\n", port);
    }

    /** Get TCAM access id */
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_access_id_get
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp_spa, &entry_access_id));
    /** clear TCAM entry */
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_hw_clear(unit, core_id, entry_access_id));

    /** clear TCAM SW table */
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_set
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp_spa, entry_access_id, 0, 0, 0,
                     FC_CHAN_ID_INVALID, 1));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Clear per port the entries in SW and HW PRT VIRTUAL PORT TCAM
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port or trunk gport
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_prt_virtual_port_tcam_mapping_clear(
    int unit,
    bcm_port_t port)
{
    bcm_core_t core_id = 0;
    uint32 pp_dsp = 0;
    uint32 prttcam_key_vp_spa = 0, pp_port_index = 0;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 system_port = 0;
    uint32 pp_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_prt_virtual_port_tcam_mapping_port_verify(unit, port));

    /** Get system port */
    if (BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, port, 0, &system_port));
    }
    else
    {
         /** Get tm port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, port, &core_id, &pp_dsp));
        SHR_IF_ERR_EXIT(dnx_port_dsp_table_mapping_get(unit, core_id, pp_dsp, (int *) &pp_port, &system_port));
    }

    /** Build SW state key by using the system_ port */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, &system_port,
                     &prttcam_key_vp_spa));

    if (BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_port_prt_virtual_port_tcam_entry_delete
                            (unit, port, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], system_port, prttcam_key_vp_spa));

        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_prt_virtual_port_tcam_entry_delete
                        (unit, port, core_id, pp_port, system_port, prttcam_key_vp_spa));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get information if TCAM access id is allocated for this port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port or trunk_gport
 * \param [in] is_sw_access_id_allocated - Indicate if TCAM access id is allocated for this port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_prt_virtual_port_mapping_get(
    int unit,
    bcm_port_t port,
    int *is_sw_access_id_allocated)
{
    uint32 prttcam_key_vp = 0;
    bcm_core_t core_id = 0;
    uint32 pp_dsp = 0, system_port = 0;
    uint8 is_used = 0;
    uint32 access_id = 0, pp_port = 0;
    uint16 priority = 0, sw_tcam_system_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_prt_virtual_port_tcam_mapping_port_verify(unit, port));

    /** Get system port */
    if (BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, port, 0, &system_port));
    }
    else
    {
        /** Get tm port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, port, &core_id, &pp_dsp));
        SHR_IF_ERR_EXIT(dnx_port_dsp_table_mapping_get(unit, core_id, pp_dsp, (int *) &pp_port, &system_port));
    }

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, &system_port,
                     &prttcam_key_vp));
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_get
                    (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_SPA, prttcam_key_vp, &access_id, &is_used, &priority,
                     &pp_port, &sw_tcam_system_port, NULL));

    *is_sw_access_id_allocated = is_used;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set various features at the port level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] value - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_control_set
 */
bcm_error_t
bcm_dnx_port_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    uint8 is_phy_port = 0;
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Check if it is a physical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));

    /** Verify the inputs*/
    SHR_INVOKE_VERIFY_DNXC(dnx_port_control_set_verify(unit, port, is_phy_port, type, value));

    switch (type)
    {
        case bcmPortControlMplsFRREnable:
        {
            dbal_enum_value_field_llr_cs_port_cs_profile_e llr_cs_profile;
            SHR_IF_ERR_EXIT(dnx_port_pp_llr_cs_profile_get(unit, port, &llr_cs_profile));
            /*
             * We expect llr_profile to be one of the four options below. Any other value
             * is considered illegal.
             */
            if ((llr_cs_profile != DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_TYPICAL_PORT) &&
                (llr_cs_profile != DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR) &&
                (llr_cs_profile != DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SUPPORT_FRR_OPTIMIZATION) &&
                (llr_cs_profile != DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Procedure '%s' was called with 'bcmPortControlMplsFRREnable'\r\n"
                             "==> but llr_profile is none of DEFAULT,802_1_BR,SUPPORT_FRR_OPTIMIZATION,802_1_BR_WITH_FRR_OPTIMIZATION.\r\n"
                             "==> It is %d. This is illegal. Quit.", __func__, llr_cs_profile);
            }
            /*
             * If 'value' is non-zero then enable FRR for MPLS.
             *   If FRR is already enabled, inject error.
             * If 'value' is zero then disable FRR for MPLS.
             *   If FRR is already disabled, inject error.
             */
            if (value != 0)
            {
                if (llr_cs_profile == DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_TYPICAL_PORT)
                {
                    llr_cs_profile = DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SUPPORT_FRR_OPTIMIZATION;
                }
                else if (llr_cs_profile == DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR)
                {
                    llr_cs_profile = DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Requesting to enable FRR when it is already enabled (llr_cs_profile = %d). Reject request with error\r\n",
                                 llr_cs_profile);
                }
            }
            else
            {
                if (llr_cs_profile == DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SUPPORT_FRR_OPTIMIZATION)
                {
                    llr_cs_profile = DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_TYPICAL_PORT;
                }
                else if (llr_cs_profile == DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION)
                {
                    llr_cs_profile = DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Requesting to disable FRR when it is already disabled (llr_cs_profile = %d). Reject request with error\r\n",
                                 llr_cs_profile);
                }
            }
            SHR_IF_ERR_EXIT(dnx_port_pp_llr_cs_profile_set(unit, port, llr_cs_profile));
            break;
        }
        case bcmPortControlDiscardEgress:
        {
            uint8 is_drop = (value != 0) ? TRUE : FALSE;
            SHR_IF_ERR_EXIT(dnx_port_pp_egress_trap_port_profile_drop_set(unit, port, is_drop));
            break;
        }
        case bcmPortControlEgressFilterDisable:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_unknown_filter_set(unit, port, value));
            break;
        }
        case bcmPortControlOuterPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_set(unit, port, QOS_DP_PROFILE_OUTER, value));
            break;
        }

        case bcmPortControlInnerPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_set(unit, port, QOS_DP_PROFILE_INNER, value));
            break;
        }
        case bcmPortControlExtenderType:
        {
            int header_type;
            dbal_enum_value_field_llr_cs_port_cs_profile_e llr_cs_profile, llr_cs_profile_get;
            /** add restriction only in header type is ETH */
            SHR_IF_ERR_EXIT(dnx_switch_header_type_get(unit, port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN, &header_type));
            if (header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port %d is not supported due to port header type is not ETH\n", port);
            }

            SHR_IF_ERR_EXIT(dnx_port_pp_llr_cs_profile_get(unit, port, &llr_cs_profile_get));

            llr_cs_profile =
                (value ==
                 BCM_PORT_EXTENDER_TYPE_SWITCH) ? ((llr_cs_profile_get ==
                                                    DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SUPPORT_FRR_OPTIMIZATION) ?
                                                   DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION
                                                   : DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR) :
                DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_TYPICAL_PORT;
            SHR_IF_ERR_EXIT(dnx_port_pp_llr_cs_profile_set(unit, port, llr_cs_profile));
            break;
        }
        case bcmPortControlBridge:
        {
            if (is_phy_port == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_same_interface_filter_set(unit, port, value));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_lif_same_interface_disable_set(unit, port, value));
            }
            break;
        }
        case bcmPortControlLogicalInterfaceSameFilter:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_in_lif_same_interface_set(unit, port, value));
            break;
        }
        case bcmPortControlPreserveDscpIngress:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_preserve_dscp_set(unit, port, value));
            break;
        }
        case bcmPortControlOamDefaultProfile:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_oam_default_profile_set(unit, port, value));
            break;
        }
        case bcmPortControlForwardNetworkGroup:
        {
            if (is_phy_port == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_split_horizon_set(unit, port, value));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_control_set port %d is not supported\n", port);
            }
            break;
        }
        /** NIF handler */
        case bcmPortControlLinkFaultLocal:
        case bcmPortControlLinkFaultRemote:
        case bcmPortControlLinkFaultRemoteEnable:
        case bcmPortControlLinkFaultLocalEnable:
        case bcmPortControlPadToSize:
        case bcmPortControlPCS:
        case bcmPortControlLinkDownPowerOn:
        case bcmPortControlPrbsMode:
        case bcmPortControlPrbsPolynomial:
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        case bcmPortControlLowLatencyLLFCEnable:
        case bcmPortControlFecErrorDetectEnable:
        case bcmPortControlLlfcCellsCongestionIndEnable:
        case bcmPortControlLLFCAfterFecEnable:
        case bcmPortControlControlCellsFecBypassEnable:
        case bcmPortControlStatOversize:
        case bcmPortControlRxEnable:
        case bcmPortControlTxEnable:
        case bcmPortControlDataRate:
        case bcmPortControlStartTxThreshold:
        case bcmPortControlLinkFaultLocalForce:
        case bcmPortControlLinkFaultRemoteForce:
        case bcmPortControlL1EthToOtnMapTranscodeType:
        case bcmPortControlOtnMapToL1EthTranscodeType:
        case bcmPortControlSarToIngressCellMode:
        case bcmPortControlEgressToSarCellMode:
        case bcmPortControlTXPISDMOverride:
        case bcmPortControlPortmodLinkRecoveryEnable:
        case bcmPortControlL1Eth1588RxEnable:
        case bcmPortControlL1Eth1588TxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_if_control_set(unit, port, type, value));
            break;
        }
        case bcmPortControlPFCRefreshTime:
        {
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(dnx_fc_pfc_refresh_set(unit, port_local, value));
            break;
        }
        case bcmPortControlPFCTransmit:
        case bcmPortControlLLFCTransmit:
        {
            SHR_BOOL_VERIFY(value, _SHR_E_PARAM, "Value must be TRUE or FALSE");
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            if (type == bcmPortControlPFCTransmit)
            {
                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, port_local, bcmCosqFlowControlGeneration, FALSE, value));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, port_local, bcmCosqFlowControlGeneration, value, FALSE));
            }
            break;
        }
        case bcmPortControlPFCReceive:
        case bcmPortControlLLFCReceive:
        {
            SHR_BOOL_VERIFY(value, _SHR_E_PARAM, "Value must be TRUE or FALSE");
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            if (type == bcmPortControlPFCReceive)
            {
                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, port_local, bcmCosqFlowControlReception, FALSE, value));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, port_local, bcmCosqFlowControlReception, value, FALSE));
            }
            break;
        }
        case bcmPortControlPFCFromLLFCEnable:
        {
            SHR_BOOL_VERIFY(value, _SHR_E_PARAM, "Invalid value for type, must be TRUE or FALSE\n");
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(imb_port_gen_pfc_from_llfc_thresh_enable_set(unit, port_local, value));
            break;
        }
        case bcmPortControl1588P2PDelay:
        {
            SHR_IF_ERR_EXIT(dnx_ptp_port_delay_set(unit, port, value));
            break;
        }
        case bcmPortControlIngressQosModelRemark:
        case bcmPortControlIngressQosModelPhb:
        case bcmPortControlIngressQosModelTtl:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_port_model_set(unit, port, type, value));
            break;
        }
        case bcmPortControlOverlayRecycle:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_overlay_recycle_set(unit, port, value));
            break;
        }
        case bcmPortControlExtenderEnable:
        {
            int forward_context_port_profile;
            int term_cxt_port_profile_get, fwd_cxt_port_profile_get;
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_enable_set(unit, port, value));
            /*
             * Following is set for UP MEP, not to include COE TAG for 2nd cycle
             */
            SHR_IF_ERR_EXIT(dnx_port_pp_term_and_forward_context_port_profile_get
                            (unit, port, &term_cxt_port_profile_get, &fwd_cxt_port_profile_get));

            if (value)
            {
                forward_context_port_profile = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_COE_PORT;
            }
            else
            {
                forward_context_port_profile = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_ETHERNET_TYPICAL;
            }
            SHR_IF_ERR_EXIT(dnx_port_pp_term_and_forward_context_port_profile_set
                            (unit, port, term_cxt_port_profile_get, forward_context_port_profile));

            break;
        }
        case bcmPortControlDistributeLearnMessageMode:
        {
            /**
             * At OLP TX:
             * 1. destination decode convert;
             * 2. Nullify the key MSBs;
             * At OLP RX:
             * 1.destination decode convert;
             * 2.Convert the Insert command to Refresh
             */
            int forward_context_port_profile = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_ETHERNET_TYPICAL;
            if (value == 1)
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_term_and_forward_context_port_profile_set
                                (unit, port,
                                 dnx_data_field.init.j1_learning_get(unit) ?
                                 DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_SWITCH_DEST_ENC_JR1_ARAD :
                                 DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS,
                                 forward_context_port_profile));
            }
            else if (value == 2)
            {
                if (dnx_data_field.init.j1_learning_get(unit) == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Value for type bcmPortControlDistributeLearnMessageMode");
                }
                SHR_IF_ERR_EXIT(dnx_port_pp_term_and_forward_context_port_profile_set
                                (unit, port,
                                 DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS,
                                 forward_context_port_profile));
            }
            else if (value == 0)
            {
                /** Reset termination and forward context port profile */
                SHR_IF_ERR_EXIT(dnx_port_pp_term_and_forward_context_port_profile_set
                                (unit, port, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_ETHERNET_TYPICAL,
                                 DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_ETHERNET_TYPICAL));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Value for type bcmPortControlDistributeLearnMessageMode");
            }
            break;
        }
        case bcmPortControlIngressEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlFirstHeaderSize:
        {
            SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_set(unit, port, value));
            break;
        }
        case bcmPortControlFirstHeaderSizeAfterInjected:
        {
            SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_after_injected_set(unit, port, value));
            break;
        }
        case bcmPortControlRxPriority:
        {
            SHR_IF_ERR_EXIT(dnx_port_ilkn_rx_priority_set(unit, port, value));
            break;
        }
        case bcmPortControlDiscardMacSaAction:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_sa_drop_action_profile_set(unit, port, value));
            break;
        }
        case bcmPortControlEEEEnable:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(imb_port_eee_enable_set(unit, port_local, value));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlEEETransmitIdleTime:
        {
            portmod_eee_t eee;

            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(imb_port_eee_get(unit, port_local, &eee));
                eee.tx_idle_time = value;
                SHR_IF_ERR_EXIT(imb_port_eee_set(unit, port_local, &eee));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlEEETransmitWakeTime:
        {
            portmod_eee_t eee;

            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(imb_port_eee_get(unit, port_local, &eee));
                eee.tx_wake_time = value;
                SHR_IF_ERR_EXIT(imb_port_eee_set(unit, port_local, &eee));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlPONEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_pon_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlFlexeDoubleNbTdmSlotEnable:
        {
            uint32 flags = 0;
            /*
             * Verify the value
             */
            SHR_RANGE_VERIFY(value, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
            /*
             * If double slots feature is disabled for the given port,
             * set the single TDM slot flag
             */
            if (value == 1)
            {
                IMB_FLEXE_DOUBLE_NB_TDM_SLOT_SET(flags);
            }
            else
            {
                IMB_FLEXE_SINGLE_NB_TDM_SLOT_SET(flags);
            }
            SHR_IF_ERR_EXIT(dnx_port_flexe_calendar_set(unit, port, flags));
            break;
        }
        case bcmPortControlRecycleApp:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_recycle_app_set_wrapper(unit, port, value));
            break;
        }
        case bcmPortControlSystemPortInjectedMap:
        {
            SHR_RANGE_VERIFY(value, 0, 1, _SHR_E_PARAM, "Value should be 0 or 1");
            if (value == 1)
            {
                SHR_IF_ERR_EXIT(dnx_port_prt_virtual_port_tcam_mapping_set(unit, port));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_prt_virtual_port_tcam_mapping_clear(unit, port));
            }
            break;
        }
        case bcmPortControlMplsSpeculativeParse:
        {
            SHR_RANGE_VERIFY(value, 0, 1, _SHR_E_PARAM, "Value should be 0 or 1");
            SHR_IF_ERR_EXIT(dnx_port_pp_speculative_parsing_set(unit, port, value));
            break;
        }
        case bcmPortControlSaMulticastEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_sa_mc_trap_set(unit, port, value));
            break;
        }
        case bcmPortControlIpFixRxEnable:
        {
            SHR_RANGE_VERIFY(value, 0, 1, _SHR_E_PARAM, "Value should be 0 or 1");
            SHR_IF_ERR_EXIT(dnx_port_pp_ipfix_rx_set(unit, port, value));
            break;
        }
        case bcmPortControlSarJitterToleranceLevel:
        case bcmPortControlFlexeDeskewAlmTriggerLocalFault:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_control_set(unit, port, type, value));
            break;
        }
        case bcmPortControlSignalQualityErDegradeThreshold:
        case bcmPortControlSignalQualityErDegradeRecoverThreshold:
        case bcmPortControlSignalQualityErFailThreshold:
        case bcmPortControlSignalQualityErFailRecoverThreshold:
        case bcmPortControlSignalQualityErMeasureMode:
        case bcmPortControlSignalQualityErInterval:
        case bcmPortControlSignalQualityErActionEnable:
        case bcmPortControlSignalQualityErHoldOffInterval:
        {
            dnx_algo_port_info_s port_info;

            /** get port type - can be either fabric or ilkn over fabric */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            {
                SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_control_set(unit, port, type, value));
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_control_set type %d is not supported\n", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get various features at the port level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] *value_p - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_control_set
 */
bcm_error_t
bcm_dnx_port_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value_p)
{
    uint8 is_phy_port = 0;
    uint32 llfc_enabled;
    uint32 pfc_enabled;
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Check if it is a physical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));

    /** Verify the inputs*/
    SHR_INVOKE_VERIFY_DNXC(dnx_port_control_get_verify(unit, port, is_phy_port, type, value_p));

    switch (type)
    {
        case bcmPortControlMplsFRREnable:
        {
            dbal_enum_value_field_llr_cs_port_cs_profile_e llr_cs_profile;
            SHR_IF_ERR_EXIT(dnx_port_pp_llr_cs_profile_get(unit, port, &llr_cs_profile));
            /*
             * We expect llr_profile to be one of the four options below. Any other value
             * is considered illegal.
             *
             * If 'llr_profile' is either
             *   DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_DEFAULT or
             *   DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR
             * then FRR is disabled for MPLS. Set '*value_p' to '0'.
             * If 'llr_profile' is either
             *   DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_SUPPORT_FRR_OPTIMIZATION or
             *   DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION
             * then FRR is enabled for MPLS.  Set '*value_p' to '1'.
             */
            switch (llr_cs_profile)
            {
                case DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_TYPICAL_PORT:
                {
                    *value_p = 0;
                    break;
                }
                case DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR:
                {
                    *value_p = 0;
                    break;
                }
                case DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SUPPORT_FRR_OPTIMIZATION:
                {
                    *value_p = 1;
                    break;
                }
                case DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION:
                {
                    *value_p = 1;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Procedure '%s' was called with 'bcmPortControlMplsFRREnable'\r\n"
                                 "==> but llr_profile is none of DEFAULT,802_1_BR,SUPPORT_FRR_OPTIMIZATION,802_1_BR_WITH_FRR_OPTIMIZATION.\r\n"
                                 "==> It is %d. This is illegal. Quit.", __func__, llr_cs_profile);
                }
            }
            break;
        }
        case bcmPortControlEgressFilterDisable:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_unknown_filter_get(unit, port, value_p));
            break;
        }
        case bcmPortControlOuterPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_get(unit, port, QOS_DP_PROFILE_OUTER, value_p));
            break;
        }
        case bcmPortControlInnerPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_get(unit, port, QOS_DP_PROFILE_INNER, value_p));
            break;
        }
        case bcmPortControlBridge:
        {
            if (is_phy_port != FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_same_interface_filter_get(unit, port, value_p));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_lif_same_interface_disable_get(unit, port, value_p));
            }
            break;
        }
        case bcmPortControlLogicalInterfaceSameFilter:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_in_lif_same_interface_get(unit, port, value_p));
            break;
        }
        case bcmPortControlPreserveDscpIngress:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_preserve_dscp_get(unit, port, value_p));
            break;
        }
        case bcmPortControlOamDefaultProfile:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_oam_default_profile_get(unit, port, value_p));
            break;
        }
        case bcmPortControlForwardNetworkGroup:
        {
            if (is_phy_port == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_split_horizon_get(unit, port, value_p));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_control_seg port %d is not supported\n", port);
            }
            break;
        }
        case bcmPortControlExtenderType:
        {
            dbal_enum_value_field_llr_cs_port_cs_profile_e llr_cs_profile = 0;

            SHR_IF_ERR_EXIT(dnx_port_pp_llr_cs_profile_get(unit, port, &llr_cs_profile));
            *value_p = ((llr_cs_profile == DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR)
                        || (llr_cs_profile == DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION)) ?
                BCM_PORT_EXTENDER_TYPE_SWITCH : BCM_PORT_EXTENDER_TYPE_NONE;
            break;
        }

        /** NIF handler */
        case bcmPortControlLinkFaultLocal:
        case bcmPortControlLinkFaultRemote:
        case bcmPortControlLinkFaultRemoteEnable:
        case bcmPortControlLinkFaultLocalEnable:
        case bcmPortControlPadToSize:
        case bcmPortControlPCS:
        case bcmPortControlLinkDownPowerOn:
        case bcmPortControlPrbsMode:
        case bcmPortControlPrbsPolynomial:
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        case bcmPortControlPrbsRxStatus:
        case bcmPortControlLowLatencyLLFCEnable:
        case bcmPortControlFecErrorDetectEnable:
        case bcmPortControlLlfcCellsCongestionIndEnable:
        case bcmPortControlLLFCAfterFecEnable:
        case bcmPortControlControlCellsFecBypassEnable:
        case bcmPortControlStatOversize:
        case bcmPortControlRxEnable:
        case bcmPortControlTxEnable:
        case bcmPortControlDataRate:
        case bcmPortControlStartTxThreshold:
        case bcmPortControlLinkFaultLocalForce:
        case bcmPortControlLinkFaultRemoteForce:
        case bcmPortControlSarToIngressCellMode:
        case bcmPortControlEgressToSarCellMode:
        case bcmPortControlTXPISDMOverride:
        case bcmPortControlTXPISDMStatus:
        case bcmPortControlPortmodLinkRecoveryEnable:
        case bcmPortControlL1Eth1588RxEnable:
        case bcmPortControlL1Eth1588TxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_if_control_get(unit, port, type, value_p));
            break;
        }
        case bcmPortControlPFCStatus:
        {
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(dnx_fc_pfc_status_get(unit, port_local, value_p));
            break;
        }
        case bcmPortControlPFCRefreshTime:
        {
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(dnx_fc_pfc_refresh_get(unit, port_local, value_p));
            break;
        }
        case bcmPortControlPFCTransmit:
        case bcmPortControlLLFCTransmit:
        {
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                            (unit, port_local, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
            if (type == bcmPortControlPFCTransmit)
            {
                *value_p = pfc_enabled;
            }
            else
            {
                *value_p = llfc_enabled;
            }
            break;
        }
        case bcmPortControlPFCReceive:
        case bcmPortControlLLFCReceive:
        {
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                            (unit, port_local, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
            if (type == bcmPortControlPFCReceive)
            {
                *value_p = pfc_enabled;
            }
            else
            {
                *value_p = llfc_enabled;
            }
            break;
        }
        case bcmPortControlPFCFromLLFCEnable:
        {
            port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
            SHR_IF_ERR_EXIT(imb_port_gen_pfc_from_llfc_thresh_enable_get(unit, port_local, (uint32 *) value_p));
            break;
        }
        case bcmPortControl1588P2PDelay:
        {
            SHR_IF_ERR_EXIT(dnx_ptp_port_delay_get(unit, port, value_p));
            break;
        }
        case bcmPortControlIngressQosModelRemark:
        case bcmPortControlIngressQosModelPhb:
        case bcmPortControlIngressQosModelTtl:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_port_model_get(unit, port, type, value_p));
            break;
        }
        case bcmPortControlOverlayRecycle:
        {
            uint32 enable = 0;
            SHR_IF_ERR_EXIT(dnx_port_pp_overlay_recycle_get(unit, port, &enable));

            *value_p = enable;
            break;
        }
        case bcmPortControlExtenderEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_enable_get(unit, port, value_p));
            break;
        }
        case bcmPortControlDiscardEgress:
        {
            uint8 is_drop = 0;
            SHR_IF_ERR_EXIT(dnx_port_pp_egress_trap_port_profile_drop_get(unit, port, &is_drop));
            *value_p = (is_drop) ? 1 : 0;
            break;
        }
        case bcmPortControlDistributeLearnMessageMode:
        {
            int term_profile, fwd_profile;
            /** Get termination context port profile */
            SHR_IF_ERR_EXIT(dnx_port_pp_term_and_forward_context_port_profile_get
                            (unit, port, &term_profile, &fwd_profile));
            if (term_profile == DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_SWITCH_DEST_ENC_JR1_ARAD)
            {
                *value_p = 1;
            }
            else if (term_profile ==
                     DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS)
            {
                if (dnx_data_field.init.j1_learning_get(unit))
                {
                    *value_p = 2;
                }
                else
                {
                    *value_p = 1;
                }
            }
            else
            {
                *value_p = 0;
            }
            break;
        }
        case bcmPortControlIngressEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_enable_get(unit, port, value_p));
            break;
        }
        case bcmPortControlFirstHeaderSize:
        {
            SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_get(unit, port, value_p));
            break;
        }
        case bcmPortControlFirstHeaderSizeAfterInjected:
        {
            SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_after_injected_get(unit, port, value_p));
            break;
        }
        case bcmPortControlRxPriority:
        {
            SHR_IF_ERR_EXIT(dnx_port_ilkn_rx_priority_get(unit, port, (bcm_port_nif_scheduler_t *) value_p));
            break;
        }
        case bcmPortControlDiscardMacSaAction:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_sa_drop_action_profile_get(unit, port, value_p));
            break;
        }
        case bcmPortControlEEEEnable:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(imb_port_eee_enable_get(unit, port_local, value_p));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlEEETransmitIdleTime:
        {
            portmod_eee_t eee;

            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(imb_port_eee_get(unit, port_local, &eee));
                *value_p = eee.tx_idle_time;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlEEETransmitWakeTime:
        {
            portmod_eee_t eee;

            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(imb_port_eee_get(unit, port_local, &eee));
                *value_p = eee.tx_wake_time;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlPONEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_pon_enable_get(unit, port, value_p));
            break;
        }
        case bcmPortControlRecycleApp:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_recycle_app_get(unit, port, value_p));
            break;
        }
        case bcmPortControlSystemPortInjectedMap:
        {
            SHR_IF_ERR_EXIT(dnx_port_prt_virtual_port_mapping_get(unit, port, value_p));
            break;
        }
        case bcmPortControlMplsSpeculativeParse:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_speculative_parsing_get(unit, port, value_p));
            break;
        }
        case bcmPortControlSaMulticastEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_sa_mc_trap_get(unit, port, value_p));
            break;
        }
        case bcmPortControlIpFixRxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ipfix_rx_get(unit, port, value_p));
            break;
        }
        case bcmPortControlIbch1Enable:
        {
            *value_p = 0;
            break;
        }
        case bcmPortControlSarJitterToleranceLevel:
        case bcmPortControlFlexeDeskewAlmTriggerLocalFault:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_control_get(unit, port, type, value_p));
            break;
        }
        case bcmPortControlSignalQualityErDegradeThreshold:
        case bcmPortControlSignalQualityErDegradeRecoverThreshold:
        case bcmPortControlSignalQualityErFailThreshold:
        case bcmPortControlSignalQualityErFailRecoverThreshold:
        case bcmPortControlSignalQualityErMeasureMode:
        case bcmPortControlSignalQualityErInterval:
        case bcmPortControlSignalQualityErActionEnable:
        case bcmPortControlSignalQualityErHoldOffInterval:
        {
            dnx_algo_port_info_s port_info;

            /** get port type - can be either fabric or ilkn over fabric */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            {
                SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_control_get(unit, port, type, value_p));
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_control_get type %d is not supported\n", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_phy_get
 * Description:
 *      General PHY register read
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - (OUT) Data that was read
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnx_port_phy_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 *phy_data)
{
    bcm_port_t logical_port = port;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(phy_data, _SHR_E_PARAM, "phy_data");

    if (!(flags & BCM_PORT_PHY_NOMAP))
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_reg_get(unit, logical_port, flags, phy_reg_addr, phy_data));

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnx_port_phy_get: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x, phy_data=0x%08x\n"), unit, port, flags, phy_reg_addr, *phy_data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_phy_set
 * Description:
 *      General PHY register write
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnx_port_phy_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data)
{
    bcm_port_t logical_port = port;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnx_port_phy_set: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x\n"), unit, port, flags, phy_reg_addr, phy_data));

    if (!(flags & BCM_PORT_PHY_NOMAP))
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_reg_set(unit, logical_port, flags, phy_reg_addr, phy_data));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_phy_modify
 * Description:
 *      General PHY register modify
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 *      phy_mask - Bits to modify using phy_data
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnx_port_phy_modify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data,
    uint32 phy_mask)
{
    bcm_port_t logical_port = port;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnx_port_phy_modify: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x phy_mask=0x%08x\n"),
              unit, port, flags, phy_reg_addr, phy_data, phy_mask));

    if (!(flags & BCM_PORT_PHY_NOMAP))
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_reg_modify(unit, logical_port, flags, phy_reg_addr, phy_data, phy_mask));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_linkscan_get
 * Purpose:
 *      Get the link scan state of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - (OUT) Linkscan value (None, S/W, H/W)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnx_port_linkscan_get(
    int unit,
    bcm_port_t port,
    int *linkscan)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(linkscan, _SHR_E_PARAM, "linkscan");

    /*
     * Should not take API lock - bcm_dnx_linkscan_mode_get will take lock
     */
    rv = bcm_dnx_linkscan_mode_get(unit, port, linkscan);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Rollback for linkscan mode configuration
 */
static shr_error_e
dnx_port_er_linkscan_mode_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_port_er_linkscan_mode_rollback_t *rollback_entry = NULL;
    SHR_FUNC_INIT_VARS(unit);

    rollback_entry = (dnx_port_er_linkscan_mode_rollback_t *) payload;
    SHR_IF_ERR_EXIT(bcm_dnx_linkscan_mode_set(unit, rollback_entry->logical_port, rollback_entry->linkscan_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_linkscan_set
 * Purpose:
 *      Set the link scan mode of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - Linkscan mode (None, S/W, H/W)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnx_port_linkscan_set(
    int unit,
    bcm_port_t port,
    int linkscan)
{
    int ori_linkscan_mode;
    dnx_port_er_linkscan_mode_rollback_t rollback_entry;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_linkscan_mode_get(unit, port, &ori_linkscan_mode));
    SHR_IF_ERR_EXIT(bcm_dnx_linkscan_mode_set(unit, port, linkscan));

    rollback_entry.linkscan_mode = ori_linkscan_mode;
    rollback_entry.logical_port = port;

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &rollback_entry,
                                                        sizeof(dnx_port_er_linkscan_mode_rollback_t),
                                                        (uint8 *) &rollback_entry,
                                                        sizeof(dnx_port_er_linkscan_mode_rollback_t),
                                                        &dnx_port_er_linkscan_mode_rollback, FALSE));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/*
 * Function:
 *      bcm_dnx_port_link_status_get
 * Purpose:
 *      Return current Link up/down status, queries linkscan, if unable to
 *      retrieve status queries the PHY.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      status - (OUT) BCM_PORT_LINK_STATUS_DOWN \ BCM_PORT_LINK_STATUS_UP
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnx_port_link_status_get(
    int unit,
    bcm_port_t port,
    int *status)
{
    int link;
    int rc;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");

    /*
     * Get local port from port
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    rc = _bcm_link_get(unit, logical_port, &link);
    if (rc == BCM_E_DISABLED)
    {
        SHR_IF_ERR_EXIT(dnx_port_link_get(unit, port, &link));
    }
    else
    {
        SHR_IF_ERR_EXIT(rc);
    }

    if (link)
    {
        *status = BCM_PORT_LINK_STATUS_UP;
    }
    else
    {
        *status = BCM_PORT_LINK_STATUS_DOWN;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the specified statistics from the device for
 *        the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in]  stat  - SNMP statistics type defined in bcm_port_stat_t.
 * \param [out] val - collected 64-bit statistics values.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_get(
    int unit,
    bcm_gport_t port,
    bcm_port_stat_t stat,
    uint64 *val)
{
    bcm_stat_val_t type;
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port = 0;
    int index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    switch (stat)
    {
        case bcmPortStatIngressPackets:
        {
            type = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1))
                ? snmpEtherStatsRXNoErrors : snmpIfInUcastPkts;
            break;
        }
        case bcmPortStatEgressPackets:
        {
            type =
                DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)) ?
                snmpEtherStatsTXNoErrors : snmpIfOutUcastPkts;
            break;
        }
        case bcmPortStatIngressBytes:
        {
            type = snmpIfInOctets;
            break;
        }
        case bcmPortStatEgressBytes:
        {
            type = snmpIfOutOctets;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_stat_get type %d is not supported\n", stat);
    }

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    if (stat == bcmPortStatIngressBytes &&
        DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
    {
        index = dnx_mib_counter_ilkn_rx_bytes;
        SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, type, index, val));
    }
    else if (stat == bcmPortStatEgressBytes &&
             DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
    {
        index = dnx_mib_counter_ilkn_tx_bytes;
        SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, type, index, val));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, logical_port, type, val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_stat_multi_get() API
 */
static shr_error_e
dnx_port_multi_get_verify(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint64 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * make sure port argumet is local port, not Gport.
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    if (nstat <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nstat");
    }
    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(value_arr, _SHR_E_PARAM, "value_arr");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Get the specified statistics from the device for
 *        the port.
 *
 * \param [in]  unit  - chip unit id.
 * \param [in]  port  - logical port.
 * \param [in]  nstat - number of elements in stat array.
 * \param [in]  stat_arr  - array of SNMP statistics types defined in bcm_port_stat_t.
 * \param [out] value_arr - collected 64-bit statistics values.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_multi_get(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint64 *value_arr)
{
    int i, rv;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_multi_get_verify(unit, port, nstat, stat_arr, value_arr));

    for (i = 0; i < nstat; i++)
    {
        rv = bcm_dnx_port_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_link_get(
    int unit,
    bcm_port_t port,
    int *link)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(link, _SHR_E_PARAM, "link");

    /*
     * Get local port from port
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_link_get(unit, logical_port, 0, link));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_pp_dsp_table_mapping_clear(
    int unit,
    int core,
    int pp_dsp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_pp_dsp_to_out_tm_mapping_set(
    int unit,
    int core,
    int pp_dsp,
    int out_tm_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, INST_SINGLE, (uint32) out_tm_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_dsp_table_mapping_set(
    int unit,
    int pp_dsp,
    int core,
    int out_pp_port,
    uint32 destination_system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, (uint32) out_pp_port);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, destination_system_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_dsp_table_mapping_get(
    int unit,
    int core,
    int pp_dsp,
    int *out_pp_port,
    uint32 *destination_system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    /**
     * when using this function, it is assumed that:
     *                  TM_PORT == PP_DSP
     * and no conversion between the two is required
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, (uint32 *) out_pp_port);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, destination_system_port);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_pp_dsp_get(
    int unit,
    bcm_port_t port,
    int *pp_dsp,
    bcm_core_t * core)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pp_dsp, _SHR_E_PARAM, "pp_dsp");
    SHR_NULL_CHECK(core, _SHR_E_PARAM, "core");

    SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, port, core, (uint32 *) pp_dsp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_pp_mapping_set(
    int unit,
    bcm_port_t logical_port,
    uint32 out_pp_port,
    uint32 system_port,
    uint32 is_lag)
{
    uint32 spa_with_masked_member = 0;
    uint32 access_id = 0;
    uint32 prttcam_key_coe_sw = 0;
    uint8 is_access_id_in_use = 0;
    uint16 priority = 0, prt_tcam_source_system_port = 0;
    uint32 pp_port = 0;
    uint32 flags = 0;
    int rv;
    bcm_core_t core_id;
    uint32 pp_dsp;
    uint32 ptc;
    dnx_algo_port_info_s port_info;
    uint16 fc_chan_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_PP_DSP(unit, port_info))
    {

        /** Get info from port management */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, logical_port, &core_id, &pp_dsp));

        /** Set out_pp_port and destination_system_port */
        SHR_IF_ERR_EXIT(dnx_port_dsp_table_mapping_set(unit, pp_dsp, core_id, out_pp_port, system_port));
    }

    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, logical_port, &flags));

    /** Provide mapping between source system port and gport */
    if (is_lag)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_spa_member_mask(unit, system_port, &spa_with_masked_member));
        SHR_IF_ERR_EXIT(dnx_port_pp_src_system_port_set(unit, core_id, out_pp_port, spa_with_masked_member));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_pp_src_system_port_set(unit, core_id, out_pp_port, system_port));
    }

    if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, logical_port, &ptc, &core_id));
        /** Provide mapping between PTC, source system port and pp port */
        SHR_IF_ERR_EXIT(dnx_port_pp_ingress_ptc_port_mapping_set(unit, ptc, core_id, system_port, out_pp_port, flags));
    }

    /*
     * COE system port set to tcam AD table.
     * When COE port in a LAG, skip here, sspa will updated for add operation.
     */
    if (DNX_PORT_IS_COE_PORT(flags) && !is_lag)
    {
        rv = dnx_port_sit_virtual_port_access_id_get_by_port(unit, logical_port, DBAL_FIELD_PRTTCAM_KEY_VP_COE,
                                                             &prttcam_key_coe_sw, &core_id, &access_id);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_get
                            (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_COE, prttcam_key_coe_sw, &access_id,
                             &is_access_id_in_use, &priority, &pp_port, &prt_tcam_source_system_port, &fc_chan_id));
            if (is_access_id_in_use)
            {
                SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_set
                                (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_COE, prttcam_key_coe_sw, access_id, priority,
                                 pp_port, system_port, ((fc_chan_id == 0) ? FC_CHAN_ID_INVALID : fc_chan_id), 0));
                SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_hw_set
                                (unit, core_id, DBAL_FIELD_PRTTCAM_KEY_VP_COE, prttcam_key_coe_sw, NULL, access_id,
                                 pp_port, system_port, ((fc_chan_id == 0) ? FC_CHAN_ID_INVALID : fc_chan_id)));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_add_pp_init(
    int unit)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the logical port which is about to be added */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &logical_port));

    /**
     * omit non PP ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, 0 /* not in lag */ ))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, logical_port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /** Enable visibility */
    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set
                    (unit, gport_info.internal_port_pp_info.core_id[0],
                     gport_info.internal_port_pp_info.pp_port[0], TRUE));

    /*
     * Currently, there is no init code that needs to be run.
     * PP Port "init" is done on when setting the PP-Port type -
     * See bcm_dnx_switch_control_indexed_port_set and
     * dnx_port_initial_eth_properties_set/dnx_port_pp_egress_set
     */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_remove_pp_init(
    int unit)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    dnx_algo_port_header_mode_e header_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** Get logical port which is about to be removed */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &logical_port));

    /**
     * omit non PP ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if ((!DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, 0 /* not in lag */ )) ||
        DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, logical_port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &phy_gport_info));

    /** Disable visibility */
    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set
                    (unit, phy_gport_info.internal_port_pp_info.core_id[0],
                     phy_gport_info.internal_port_pp_info.pp_port[0], FALSE));
    /*
     * Check if the port set header type was called.
     * See bcm_dnx_switch_control_indexed_port_set that sets the SW data base.
     */
    dnx_algo_port_db.pp.header_mode.get(unit, phy_gport_info.internal_port_pp_info.core_id[0],
                                        phy_gport_info.internal_port_pp_info.pp_port[0], &header_mode);

    if (header_mode != DNX_ALGO_PORT_HEADER_MODE_NON_PP)
    {
        int port_in_header_type;
        int port_out_header_type;
        /*
         * PP Port "de-init" - need to release resources allocated by
         * dnx_port_initial_eth_properties_set/dnx_port_pp_egress_set
         */

        /** Get the PP-Port type: */
        SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                        (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN, &port_in_header_type));
        /** See dnx_port_initial_eth_properties_set  - need to deinit pp_port ingress resources */
        SHR_IF_ERR_EXIT(dnx_port_initial_eth_properties_unset(unit, phy_gport_info.local_port, port_in_header_type));

        /** Get the PP-Port type: */
        SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                        (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT,
                         &port_out_header_type));
        /** See dnx_port_pp_egress_set - need to deinit pp_port egress resources */
        SHR_IF_ERR_EXIT(dnx_port_pp_egress_unset(unit, phy_gport_info.local_port, port_out_header_type));
        SHR_IF_ERR_EXIT(dnx_port_estimate_header_mode_set(unit, phy_gport_info.local_port, port_out_header_type));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_rate_egress_pps_set(
    int unit,
    bcm_port_t port,
    uint32 pps,
    uint32 burst)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_fabric_if_link_rate_egress_pps_set(unit, logical_port, pps, burst));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_rate_egress_pps_get(
    int unit,
    bcm_port_t port,
    uint32 *pps,
    uint32 *burst)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_fabric_if_link_rate_egress_pps_get(unit, logical_port, pps, burst));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_local_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_t * local_port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(local_port, _SHR_E_PARAM, "local_port");

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport_info.local_port));

    *local_port = gport_info.local_port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for the following APIs:
 *
 *    dnx_port_info_set/get
 *    dnx_port_selective_set/get
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] info - port info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_info_verify(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "port info");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the port info according the action mask.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] info - port info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_selective_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    bcm_port_t logical_port = 0;
    uint32 mask = 0, support_action_mask, support_action_mask2;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_info_verify(unit, port, info));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_port_info_support_action_mask_get
                    (unit, logical_port, &support_action_mask, &support_action_mask2));

    mask = info->action_mask & support_action_mask;

    if (mask & BCM_PORT_ATTR_ENABLE_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, logical_port, &info->enable));
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_link_status_get(unit, logical_port, &info->linkstatus));
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK)
    {
        bcm_port_resource_t resource;
        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, logical_port, &resource));

        info->speed = resource.speed;
    }

    /*
     * Get rx pause and tx pause
     */
    if (mask & BCM_PORT_ATTR_PAUSE_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, logical_port, bcmPortControlLLFCTransmit, &info->pause_tx));
        SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, logical_port, bcmPortControlLLFCReceive, &info->pause_rx));
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK)
    {
        
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_linkscan_get(unit, logical_port, &info->linkscan));
        }

    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_learn_get(unit, logical_port, &info->learn));
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_vlan_member_get(unit, logical_port, &info->vlanfilter));
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_untagged_vlan_get(unit, logical_port, &info->untagged_vlan));
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_stp_get(unit, logical_port, &info->stp_state));
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_loopback_get(unit, logical_port, &info->loopback));
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_frame_max_get(unit, logical_port, &info->frame_max));
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_autoneg_get(unit, logical_port, &info->autoneg));
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_duplex_get(unit, logical_port, &info->duplex));
    }

    if (mask & BCM_PORT_ATTR_FAULT_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_fault_get(unit, logical_port, &info->fault));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get all the support port info for the given port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] info - port info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_info_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_info_verify(unit, port, info));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    bcm_port_info_t_init(info);
    SHR_IF_ERR_EXIT(dnx_port_info_support_action_mask_get(unit, logical_port, &info->action_mask, &info->action_mask2));
    SHR_IF_ERR_EXIT(dnx_port_selective_get(unit, logical_port, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set 1588 protocol configuaion for port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] config_count - (IN) config_array size
 * \param [in] config_array - (IN) 1588 port configuration
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *      Disable port 1588 configuration  - config_count = 0
 *      Enable  port 1588 configuration  - config_count = 1
 *                       config_array->flags =
 *                       BCM_PORT_TIMESYNC_DEFAULT            mast be on
 *                       BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP when one step Tranparent clock (TC) is enabled system updates the
 *                                                            correction field in 1588 Event 1588 messages.
 *                       BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP when step step Tranparent clock (TC) is enabled system records the
 *                                                            1588 Event 1588 messages TX time.
 *                         1588 event messages:
 *                           1. SYNC
 *                           2. DELAY_REQ
 *                           3. PDELAY_REQ
 *                           4. PDELAY_RESP
 *                       config_array->pkt_drop, config_array->pkt_tocpu - bit masks indicating wehter to forward (drop-0,tocpu-0),
 *                       drop(drop-1) or trap(tocpu-1) the packet

 * \see
 *   * None
 */

int
bcm_dnx_port_timesync_config_set(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t * config_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_ptp_timesync_config_set_verify(unit, port, config_count, config_array));
    SHR_IF_ERR_EXIT(dnx_ptp_port_timesync_config_set(unit, port, config_count, config_array));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrives two-step PTP timestamp and seqid from PHY FIFO after packet TX.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tx_info - two-step PTP timestamp and seqid info.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_timesync_tx_info_get(
    int unit,
    bcm_port_t port,
    bcm_port_timesync_tx_info_t * tx_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_ptp_timesync_tx_info_get_verify(unit, port, tx_info));
    SHR_IF_ERR_EXIT(dnx_ptp_port_timesync_tx_info_set(unit, port, tx_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify input pararmeters for bcm_port_timestamp_adjust_set
 *
 * \param [in] unit - (IN) Unit number.
 * \param [in] port - (IN) logical port.
 * \param [out] ts_adjust - (IN) configuration for tsts,osts TX timestamp compensation.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_timestamp_adjust_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_timestamp_adjust_t * ts_adjust)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ts_adjust, _SHR_E_PARAM, "ts_adjust");
    if (ts_adjust->osts_adjust > DNX_PORT_TX_TIMESTAMP_ADJUST_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tx timestamp compensation is out-of-range, the max acceptable value is 0x%x",
                     DNX_PORT_TX_TIMESTAMP_ADJUST_MAX);
    }
    if (ts_adjust->tsts_adjust)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tsts_adjust field can't be set, two-step uses the same tx timestamp compensation as one-step");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set TX timestamp compensation.
 *
 * \param [in] unit - (IN) Unit number.
 * \param [in] port - (IN) logical port.
 * \param [in] ts_adjust - (IN) TX timestamp compensation for one-step/two-step.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   Only ts_adjust->osts_adjust is accepted by this API. Both one-step and two-step will use the same TX timestamp compensation value.
 * \see
 *   * None
 */
int
bcm_dnx_port_timestamp_adjust_set(
    int unit,
    bcm_port_t port,
    bcm_port_timestamp_adjust_t * ts_adjust)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify the inputs
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_timestamp_adjust_set_verify(unit, port, ts_adjust));

    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TX timestamp compensation is not supported");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TX timestamp compensation.
 *
 * \param [in] unit - (IN) Unit number.
 * \param [in] port - (IN) logical port.
 * \param [out] ts_adjust - (OUT) TX timestamp compensation for one-step/two-step.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   Only ts_adjust->osts_adjust is set by this API. Both one-step and two-step will use the same TX timestamp compensation value.
 * \see
 *   * None
 */
int
bcm_dnx_port_timestamp_adjust_get(
    int unit,
    bcm_port_t port,
    bcm_port_timestamp_adjust_t * ts_adjust)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify the inputs
     */
    SHR_NULL_CHECK(ts_adjust, _SHR_E_PARAM, "ts_adjust");

    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TX timestamp compensation is not supported");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get 1588 protocol port configuaion
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] array_size - config_array size
 * \param [out] config_array - 1588 port configuration
 * \param [out] array_count - config_array size
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 * output will be the follwing:
 * when port 1588 configuration Disabled - *array_count = 0
 * when port 1588 configuration Enabled  - *array_count = 1, config_array is set with the port configuration
 * \see
 *   * None
 */
int
bcm_dnx_port_timesync_config_get(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t * config_array,
    int *array_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_ptp_timesync_config_get_verify(unit, port, array_size, config_array, array_count));
    SHR_IF_ERR_EXIT(dnx_ptp_port_timesync_config_get(unit, port, array_size, config_array, array_count));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set various PHY timesync controls for port.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [in] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_control_phy_timesync_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_ptp_phy_timesync_set_verify(unit, port, type, value));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_set(unit, port, type, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get various PHY timesync controls for port.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [out] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_control_phy_timesync_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_ptp_phy_timesync_get_verify(unit, port, type, value));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_get(unit, port, type, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set various PHY timesync configuaion for port.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_timesync_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_ptp_phy_timesync_config_set_verify(unit, port, conf));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_config_set(unit, port, conf));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get various PHY timesync configuaion for port.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_timesync_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_ptp_phy_timesync_config_get_verify(unit, port, conf));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_config_get(unit, port, conf));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_port_logical_verify(
    int unit,
    bcm_gport_t port)
{
    algo_gpm_gport_verify_type_e allowed_types[] =
        { ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_PORT,
        ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_FABRIC
    };
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, port, COUNTOF(allowed_types), allowed_types));

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /** verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport_info.local_port));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_port_logical_get(
    int unit,
    bcm_gport_t port,
    bcm_port_t * logical_port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    *logical_port = gport_info.local_port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to set the source MAC address transmitted in MAC
 *           control pause frames for a specified port. MAC control pause frames are transmitted with
 *           a well known destination address (01:80:c2:00:00:01) that is not controllable. However,
 *           the source address can be set and retrieved using the bcm_port_pause_addr_set/get calls.
 *           On switch initialization, the application software should set the pause frame source address
 *           for all ports. These values are persistent across calls that enable or disable the transmission
 *           of pause frames and need only be set once. Only MAC control pause frames generated by the MACs
 *           in the switch device will contain the specified source address. The CPU is able to generate MAC
 *           control packets with any source address that will be transmitted unchanged.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the MAC address
 *   \param [in] mac - MAC address to transmit as the source address in MAC control pause frames
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_addr_set(
    int unit,
    bcm_port_t port,
    bcm_mac_t mac)
{
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify logical port */
    port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port_local));

    /** Set the MAC address */
    SHR_IF_ERR_EXIT(dnx_fc_port_mac_sa_set(unit, port_local, mac));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to get the source MAC address transmitted in MAC
 *           control pause frames for a specified port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the MAC address
 *   \param [out] mac - MAC address
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_addr_get(
    int unit,
    bcm_port_t port,
    bcm_mac_t mac)
{
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify logical port */
    port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port_local));

    /** Get the MAC address */
    SHR_IF_ERR_EXIT(dnx_fc_port_mac_sa_get(unit, port_local, mac));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to enable or disable transmission of pause frames and honoring received pause frames on a port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set pause settings
 *   \param [in] pause_tx - 0 to disable transmission of pause frames, 1 to enable
 *   \param [in] pause_rx - 0 to ignore received pause frames, 1 to honor received pause frames
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_set(
    int unit,
    bcm_port_t port,
    int pause_tx,
    int pause_rx)
{
    bcm_port_t port_ndx;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.local_port_bmp, port_ndx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port_ndx, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported for Interlaken configuration");
        }
    }

    BCM_PBMP_ITER(gport_info.local_port_bmp, port_ndx)
    {
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, port_ndx, bcmCosqFlowControlReception, pause_rx, FALSE));
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, port_ndx, bcmCosqFlowControlGeneration, pause_tx, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to get the settings for transmission of pause frames and honoring received pause frames on a port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set pause settings
 *   \param [out] pause_tx - disabled/enabed transmission of pause frames
 *   \param [out] pause_rx - ignore/honor received pause frames
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_get(
    int unit,
    bcm_port_t port,
    int *pause_tx,
    int *pause_rx)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_port_info_s port_info;
    uint32 dummy_rx_pfc, dummy_tx_pfc;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
    {
        *pause_rx = *pause_tx = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, gport_info.local_port, bcmCosqFlowControlReception, (uint32 *) pause_rx, &dummy_rx_pfc));
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, gport_info.local_port, bcmCosqFlowControlGeneration, (uint32 *) pause_tx,
                         &dummy_tx_pfc));
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_selective_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_port_selective_get(unit, port, info));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_port_info_get(unit, port, info));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_signal_quality_ser_verify(
    int unit,
    bcm_port_t port)
{
    int first_phy = 0;
    bcm_port_resource_t resource;
    dnx_algo_pm_info_t pm_info;

    SHR_FUNC_INIT_VARS(unit);

    /** verify feature */
    if (!dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_supported))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Signal Quality ER is not supported for current device \n");
    }

    /** verify if HW-based solution is applicable for requested PM*/
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));
    SHR_IF_ERR_EXIT(dnx_algo_phy_pm_info_get(unit, first_phy, &pm_info));

    if (0 == dnx_data_nif.signal_quality.hw_based_solution_get(unit, pm_info.dispatch_type)->is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature is not supported to this specific port:%d\r\n", port);
    }

    /** verify if port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /** verify FEC is enabled */
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, port, &resource));
    if (!dnx_data_nif.signal_quality.supported_fecs_get(unit, resource.fec_type)->is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port must use a supported FEC type to work with Signal Quality ER logic\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_signal_quality_ser_config_verify(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "NULL parameter");

    SHR_IF_ERR_EXIT(dnx_port_signal_quality_ser_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_signal_quality_ser_config_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_signal_quality_ser_config_verify(unit, port, config));

    /** verify enable value is within range */
    SHR_MAX_VERIFY(config->enable, 1, _SHR_E_PARAM, "Value should be 0 or 1");

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_signal_quality_ser_status_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(status, _SHR_E_PARAM, "NULL parameter");

    SHR_IF_ERR_EXIT(dnx_port_signal_quality_ser_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_signal_quality_ser_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_status_t * status)
{
    portmod_fec_degraded_ser_status_t portmod_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_fec_degraded_ser_status_get(unit, port, &portmod_status));
    status->fec_degraded_ser = portmod_status.fec_degraded_ser;
    status->rx_local_degraded = portmod_status.rx_local_degraded;
    status->rx_remote_degraded = portmod_status.rx_remote_degraded;
    status->rx_am_status_field = portmod_status.rx_am_status_field;

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_signal_quality_ser_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_config_t * config)
{
    portmod_fec_degraded_ser_config_t portmod_config;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_fec_degraded_ser_get(unit, port, &portmod_config));
    config->enable = portmod_config.enable;
    config->activate_threshold = portmod_config.activate_threshold;
    config->deactivate_threshold = portmod_config.deactivate_threshold;
    config->interval = portmod_config.interval;

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_signal_quality_ser_config_hw_set(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_config_t * config)
{
    bcm_port_signal_quality_ser_config_t get_config;
    int is_set_to_hw = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_signal_quality_ser_config_get(unit, port, &get_config));

    /*
     *  As per user request:
     *  To update the threshold/interval registers is not applicable in case FEC degrade feature enabled.
     *  Hence, If one of the thresholds is not equal to the one configured in HW and Feature is enabled in HW then manually disable the feature.
     */
    if ((config->enable == get_config.enable) &&
        (config->activate_threshold == get_config.activate_threshold) &&
        (config->deactivate_threshold == get_config.deactivate_threshold) && (config->interval == get_config.interval))
    {
        is_set_to_hw = FALSE;
    }

    if (is_set_to_hw)
    {
        portmod_fec_degraded_ser_config_t portmod_config = {
            .enable = config->enable,
            .activate_threshold = config->activate_threshold,
            .deactivate_threshold = config->deactivate_threshold,
            .interval = config->interval
        };

        /*
         * While the feature is enabled, the user attempt to modify the threshold/interval, hence feature will be disabled first to allow the modification process.
         * If the request is to disable the feature, hence no need to execute any preliminary action
         */
        if (config->enable && get_config.enable)
        {
            portmod_config.enable = 0;
            SHR_IF_ERR_EXIT(portmod_fec_degraded_ser_set(unit, port, portmod_config));
            portmod_config.enable = 1;
        }

        SHR_IF_ERR_EXIT(portmod_fec_degraded_ser_set(unit, port, portmod_config));
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_signal_quality_ser_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the inputs*/
    SHR_INVOKE_VERIFY_DNXC(dnx_port_signal_quality_ser_status_get_verify(unit, port, status));

    /** Get FEC degraded ability and SER status from HW */
    SHR_IF_ERR_EXIT(dnx_port_signal_quality_ser_status_get(unit, port, status));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_signal_quality_ser_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the inputs*/
    SHR_INVOKE_VERIFY_DNXC(dnx_port_signal_quality_ser_config_verify(unit, port, config));

    /** Get configuration from HW */
    SHR_IF_ERR_EXIT(dnx_port_signal_quality_ser_config_get(unit, port, config));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_signal_quality_ser_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_ser_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the inputs*/
    SHR_INVOKE_VERIFY_DNXC(dnx_port_signal_quality_ser_config_set_verify(unit, port, config));

    /** Apply HW configuration */
    SHR_IF_ERR_EXIT(dnx_port_signal_quality_ser_config_hw_set(unit, port, config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set various configurations for Preemption.
 *
 * \param [in] unit - the relevant unit
 * \param [in] gport - logical port number
 * \param [in] type - preemption control type defined in bcm_port_preempt_control_t
 * \param [in] arg - the preemption control value
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e
bcm_dnx_port_preemption_control_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_control_t type,
    uint32 arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_port_preemption_control_set(unit, gport, type, arg));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get various configurations for Preemption.
 *
 * \param [in] unit - the relevant unit
 * \param [in] gport - logical port number
 * \param [in] type - preemption control type defined in bcm_port_preempt_control_t
 * \param [out] arg - the preemption control value
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e
bcm_dnx_port_preemption_control_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_control_t type,
    uint32 *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_port_preemption_control_get(unit, gport, type, arg));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the specified preemption status.
 *
 * \param [in] unit - the relevant unit
 * \param [in] gport - logical port number
 * \param [in] type - preemption status type defined in bcm_port_preempt_status_t
 * \param [out] status - the preemption status
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e
bcm_dnx_port_preemption_status_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_status_t type,
    uint32 *status)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_port_preemption_status_get(unit, gport, type, status));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_signal_quality_callback_register(
    int unit,
    bcm_port_signal_quality_callback_t user_func,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Signal quality CB register is not supported\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_signal_quality_callback_unregister(
    int unit,
    bcm_port_signal_quality_callback_t user_func,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Signal quality CB unregister is not supported\n");

exit:
    SHR_FUNC_EXIT;
}
#if defined(INCLUDE_GDPLL)
/**
 * \brief - Verify parameters for Nanosync port map APIs
 *
 * \param [in] unit - Unit id
 * \param [in] logical_port - logical port ID
 * \param [in] lane_num - from 0 to max_lane_num - 1
 * \param [in] event_type - The GDPLL event type
 * \param [in] ns_ports - Nanosync port
 * \param [in] is_input - verify for input or output API
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_to_nanosync_port_get_verify(
    int unit,
    bcm_port_t logical_port,
    int lane_num,
    bcm_gdpll_port_event_t event_type,
    int *ns_port,
    int is_input)
{
    int nof_phys;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Null checks
     */
    SHR_NULL_CHECK(ns_port, _SHR_E_PARAM, "ns_port");
    /*
     * Verify port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) &&
        !DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "The port type is not supported.\n");
    }
    /*
     * Verify lane_num
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &nof_phys));
    SHR_RANGE_VERIFY(nof_phys, 0, nof_phys - 1, _SHR_E_PARAM, "lane_num is out of range.\n");
    /*
     * Verify event type
     */
    if (is_input)
    {
        if ((event_type != bcmGdpllPortEventRXCDR) &&
            (event_type != bcmGdpllPortEventSynthClk) && (event_type != bcmGdpllPortEventTXPI))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The event type is not supported.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Nanosync input port
 *
 * \param [in] unit - Unit id
 * \param [in] logical_port - logical port ID
 * \param [in] lane_num - from 0 to max_lane_num - 1
 * \param [in] event_type - The GDPLL event type
 * \param [out] ns_port - Nanosync port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_port_to_nanosync_input_port_get(
    int unit,
    bcm_port_t logical_port,
    int lane_num,
    bcm_gdpll_port_event_t event_type,
    int *ns_port)
{
    bcm_pbmp_t phys;
    int lane_id, index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_to_nanosync_port_get_verify(unit, logical_port, lane_num, event_type, ns_port, 1));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phys));

    index = 0;
    _SHR_PBMP_ITER(phys, lane_id)
    {
        if (index == lane_num)
        {
            if ((event_type == bcmGdpllPortEventRXCDR) || (event_type == bcmGdpllPortEventTXPI))
            {
                *ns_port = lane_id;
            }
            else
            {
                *ns_port = lane_id + dnx_data_nif.phys.nof_phys_get(unit);
            }
            break;
        }
        ++index;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get Nanosync output port
 *
 * \param [in] unit - Unit id
 * \param [in] logical_port - logical port ID
 * \param [in] lane_num - from 0 to max_lane_num - 1
 * \param [out] ns_port - Nanosync port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_port_to_nanosync_output_port_get(
    int unit,
    bcm_port_t logical_port,
    int lane_num,
    int *ns_port)
{
    bcm_pbmp_t phys;
    int lane_id, index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_to_nanosync_port_get_verify(unit, logical_port, lane_num, 0, ns_port, 0));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phys));

    index = 0;
    _SHR_PBMP_ITER(phys, lane_id)
    {
        if (index == lane_num)
        {
            *ns_port = lane_id;
            break;
        }
        ++index;
    }
exit:
    SHR_FUNC_EXIT;
}
#endif /* INCLUDE_GDPLL */
