/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dbal/dbal.h>
#include "imb_ile_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_ILE_ILKN_SERDES_INVALID (-1)

/**
 * \brief - Enable ILE port in wrapper. This action essentially reset the port in the ILKN core,
 * but without affecting the ILKN core configurations. this reset is done before any access to the PM phys.
 * see .h file
 */
int
imb_ile_port_wrapper_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((flags & (IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX)) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "no direction flag specified: flags=0x%x\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_PORT_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
    /*
     * set value fields
     */
    if (flags & IMB_ILE_DIRECTION_RX)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_ENABLE, INST_SINGLE, enable);
    }
    if (flags & IMB_ILE_DIRECTION_TX)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_ENABLE, INST_SINGLE, enable);
    }
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ILE core, port status in wrapper.
 * see .h file
 */
int
imb_ile_port_wrapper_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    uint32 rx_enable, tx_enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set initial value */
    *enable = 0;

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_PORT_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_ENABLE, INST_SINGLE, &rx_enable);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TX_ENABLE, INST_SINGLE, &tx_enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = (rx_enable && tx_enable);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set number of segments to be used by the port
 * see .h file
 */
int
imb_ile_port_nof_segments_set(
    int unit,
    bcm_port_t port,
    int nof_segments)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_nif_ilkn_segments_enable_e segment_num;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    if ((nof_segments == 0) || (nof_segments == dnx_data_nif.ilkn.segments_half_nof_get(unit)))
    {
        segment_num = DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_ENABLE_HALF_OR_NO_SEGMENTS;
    }
    else if (nof_segments == dnx_data_nif.ilkn.segments_max_nof_get(unit))
    {
        segment_num = DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_ENABLE_MAX_SEGMENTS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nof segments %d\n", nof_segments);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENTS_ENABLE, INST_SINGLE, segment_num);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable segments to be used by the port
 * see .h file
 */
int
imb_ile_port_segments_enable_set(
    int unit,
    bcm_port_t port,
    int nof_segments)
{
    uint32 entry_handle_id;
    uint8 is_low_updated = 0, is_high_updated = 0;
    uint8 low_val, high_val;
    int max_segments;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int ilkn_nof_ports;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    max_segments = dnx_data_nif.ilkn.segments_max_nof_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);

    /*
     * Get the relevant ports in this ilkn core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_nof_ports_get(unit, port, &ilkn_nof_ports));

    /*
     * Low should be updated on Big only
     */
    is_low_updated = (ilkn_access_info.port_in_core == 0) ? TRUE : FALSE;

    /*
     * High should be updated on Small port OR on Single Big port
     */
    is_high_updated = ((ilkn_access_info.port_in_core == 1) || (ilkn_nof_ports == 1)) ? TRUE : FALSE;

    /*
     * Low should be set on Big port if segments != 0
     */
    low_val = (nof_segments > 0) ? 1 : 0;

    /*
     * High should be set on Small port if segments != 0 OR on Big port if max segments
     */
    high_val = (((nof_segments > 0) && (ilkn_access_info.port_in_core == 1)) || (nof_segments == max_segments)) ? 1 : 0;

    if (is_low_updated)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENTS_SELECTION_LOW, INST_SINGLE, low_val);
    }
    if (is_high_updated)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENTS_SELECTION_HIGH, INST_SINGLE, high_val);
    }

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ILKN to operate over fabric links instead of NIF
 *        phys.
 * see .h file
 */
int
imb_ile_port_ilkn_over_fabric_set(
    int unit,
    bcm_port_t port,
    int is_over_fabric)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OVER_FABRIC, INST_SINGLE,
                                 is_over_fabric ?
                                 DBAL_ENUM_FVAL_NIF_ILKN_FABRIC_NIF_SELECT_ILKN_OVER_FABRIC :
                                 DBAL_ENUM_FVAL_NIF_ILKN_FABRIC_NIF_SELECT_ILKN_OVER_NIF);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ILKN port to operate as ELK port (in JR2 this
 *        is the only valid option for ILKN)
 * see .h file
 */
int
imb_ile_port_ilkn_elk_set(
    int unit,
    bcm_port_t port,
    int is_elk)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CORE_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ELK_ENABLE, INST_SINGLE, is_elk);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ILKN ELK data mux: for data or ELK
 * see .h file
 */
int
imb_ile_port_ilkn_elk_data_mux_set(
    int unit,
    bcm_port_t port,
    int is_elk)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ELK_SUPPORT, INST_SINGLE, is_elk);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ILKN credits source : 1 for CDU, 0 for CLUP
 * see .h file
 */
int
imb_ile_port_ilkn_credits_source_set(
    int unit,
    bcm_port_t port)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int credit_source = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    credit_source = (ilkn_access_info.pm_imb_type == imbDispatchTypeImb_cdu) ? 1 : 0;

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_SELECTOR, INST_SINGLE, credit_source);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - reset the ILKN port. this is the main reset for the
 *        port which affect all prior configurations. this
 *        should be the first reset before starting any ILKN
 *        core configurations.
 * see .h file
 */
int
imb_ile_port_reset_set(
    int unit,
    bcm_port_t port,
    int is_core_reset,
    int in_reset)
{
    uint32 entry_handle_id;
    int enable_second_port;
    uint8 is_for_commit = FALSE;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CORE_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    if (is_core_reset)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                     in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

        is_for_commit = TRUE;

    }

    if (ilkn_access_info.port_in_core == 1)
    {
        enable_second_port = (in_reset) ? 0 : 1;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SECOND_PORT_ENABLE, INST_SINGLE,
                                     enable_second_port);
        is_for_commit = TRUE;

    }

    if (is_for_commit)
    {
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Clear the handle before re-use
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILE_CORE_ENABLERS, entry_handle_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_PORT_ENABLERS, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - reset the bypass interface in the CDU.
 * when ILKN port is operating over NIF, the CDU logic needs to
 * be bypassed. this enables the bypass interface to take place.
 * see .h file
 */
int
imb_ile_port_bypass_if_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    uint32 entry_handle_id;
    int iter_phy, lane, ethu_id_in_core, core;
    bcm_pbmp_t ilkn_phys;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));

    if (flags & IMB_ILE_DIRECTION_RX)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(ilkn_phys, iter_phy)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));
            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYPASS_IF_ENABLE, INST_SINGLE, enable);
            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /*
         * Clear the handle before re-use
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, entry_handle_id));
    }

    if (flags & IMB_ILE_DIRECTION_TX)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(ilkn_phys, iter_phy)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));
            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYPASS_IF_ENABLE, INST_SINGLE, enable);
            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ilu_is_xpmd_enabled(
    int unit,
    bcm_port_t port,
    uint8 *is_xpmd_enabled)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    bcm_pbmp_t ilkn_phys;
    int first_phy, ethu_id, core;
    imb_dispatch_type_t imb_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));
    _SHR_PBMP_FIRST(ilkn_phys, first_phy);

    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));

    imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;

    *is_xpmd_enabled = ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_support_xpmd_if))
                        && (imb_type == imbDispatchTypeImb_cdu)) ? TRUE : FALSE;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get latch down indication for the port.
 * see .h file
 */
int
imb_ile_port_async_fifo_reset(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    int iter_phy, lane, ethu_id_in_core, core;
    bcm_pbmp_t ilkn_phys;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    if (flags & IMB_ILE_DIRECTION_RX)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(ilkn_phys, iter_phy)
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));

            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ASYNC_FIFO_RESET, INST_SINGLE, in_reset ?
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /*
         * Clear the handle before re-use
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, entry_handle_id));
    }

    if (flags & IMB_ILE_DIRECTION_TX)
    {

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(ilkn_phys, iter_phy)
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));

            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ASYNC_FIFO_RESET, INST_SINGLE, in_reset ?
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - get latch down indication for the port.
 * see .h file
 */
int
imb_ile_port_xpmd_lane_enable(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    uint32 entry_handle_id;
    int iter_phy, lane, ethu_id_in_core, core, ethu_id, cdu_id;
    bcm_pbmp_t ilkn_phys;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    if (flags & IMB_ILE_DIRECTION_RX)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_XPMD_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(ilkn_phys, iter_phy)
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));
            SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

            cdu_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;
            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CDU, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_ENABLE, INST_SINGLE, enable);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /*
         * Clear the handle before re-use
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILE_XPMD_CTRL, entry_handle_id));
    }

    if (flags & IMB_ILE_DIRECTION_TX)
    {

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_XPMD_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(ilkn_phys, iter_phy)
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));
            SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

            cdu_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;
            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CDU, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_ENABLE, INST_SINGLE, enable);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - get latch down indication for the port.
 * see .h file
 */

int
imb_ile_port_latch_down_get(
    int unit,
    bcm_port_t port,
    uint32 *is_latch_low_aligned)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_LATCH_LOW_ALIGNED, INST_SINGLE,
                               is_latch_low_aligned);
    /*
     * commit the request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the lane map info for ILKN core.
 *
 * see .h file
 */
shr_error_e
imb_ile_port_ilkn_port_lane_map_get(
    int unit,
    bcm_port_t port,
    int nof_lanes,
    int *rx_lane_map,
    int *tx_lane_map)
{
    int is_over_fabric, lane_id;
    int valid_index;
    bcm_pbmp_t lanes;
    bcm_port_t lane_iter;
    dnx_algo_lane_map_type_e type;
    dnx_algo_port_ilkn_lane_info_t ilkn_lane_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &lanes));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));

    type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;

    /*
     * initialize lane maps
     */
    for (lane_id = 0; lane_id < nof_lanes; lane_id++)
    {
        rx_lane_map[lane_id] = IMB_ILE_ILKN_SERDES_INVALID;
        tx_lane_map[lane_id] = IMB_ILE_ILKN_SERDES_INVALID;
    }

    /*
     * Get RX/TX SerDes mappings
     */
    _SHR_PBMP_ITER(lanes, lane_iter)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_ilkn_lane_info_get(unit, port, type, lane_iter, &ilkn_lane_info));
        rx_lane_map[ilkn_lane_info.ilkn_id] = ilkn_lane_info.rx_id;
        tx_lane_map[ilkn_lane_info.ilkn_id] = ilkn_lane_info.tx_id;
    }

    /*
     * compress RX/TX arrays
     */
    valid_index = 0;
    for (lane_id = 0; lane_id < nof_lanes; lane_id++)
    {
        if (rx_lane_map[lane_id] != IMB_ILE_ILKN_SERDES_INVALID)
        {
            rx_lane_map[valid_index] = rx_lane_map[lane_id];
            tx_lane_map[valid_index] = tx_lane_map[lane_id];
            valid_index++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the lane map info for ILKN core.
 *
 * see .h file
 */
shr_error_e
imb_ile_port_ilkn_pm_lane_map_get(
    int unit,
    bcm_port_t port,
    int nof_lanes,
    int first_phy,
    phymod_lane_map_t * phy_lane_map)
{

    int is_over_fabric, ilkn_id, lane_id, last_phy, map_size;
    dnx_algo_lane_map_type_e type;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));

    type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;
    if (is_over_fabric)
    {
        first_phy -= dnx_data_port.general.fabric_phys_offset_get(unit);
    }

    last_phy = first_phy + nof_lanes - 1;

    map_size = (is_over_fabric) ? dnx_data_fabric.links.nof_links_get(unit) : dnx_data_nif.phys.nof_phys_get(unit);

    SHR_ALLOC(lane2serdes, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get all the lane to serdes mapping for all the PMs */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_map_get(unit, type, first_phy, last_phy, lane2serdes));

    for (lane_id = 0; lane_id < nof_lanes; ++lane_id)
    {
        phy_lane_map->lane_map_rx[lane_id] = lane2serdes[lane_id].rx_id;
        phy_lane_map->lane_map_tx[lane_id] = lane2serdes[lane_id].tx_id;
    }

    phy_lane_map->num_of_lanes = nof_lanes;

exit:
    SHR_FREE(lane2serdes);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the FEC units for ILKN port.
 *
 * see .h file
 */
shr_error_e
imb_ile_port_ilkn_fec_units_set(
    int unit,
    bcm_port_t port,
    int is_reset,
    int fec_enable,
    int fec_bypass_valid,
    int fec_bypass_enable)
{
    uint32 entry_handle_id, dbal_field_val;
    int iter_phy, first_lane_in_cdu, ethu_id, ethu_id_in_core, core, phy_in_core, is_over_fabric, pm_index,
        first_phy_in_pm, tx_serdes_source, map_size;
    int rx_lane_map[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    int tx_lane_map[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    int nof_phys_per_core, ilkn_nof_ports, fec_core_reset;
    int ilkn_port_i, fec_pair_phy, fec_unit_used;
    bcm_pbmp_t all_ilkn_ports;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    bcm_pbmp_t ilkn_phys, tx_phys, pm_phys, tmp_bmp, ilkn_port_i_phys;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));
    /*
     * Get the relevant ports in this ilkn core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_nof_ports_get(unit, port, &ilkn_nof_ports));
    fec_core_reset = ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu)) && (ilkn_nof_ports == 1)
                      && (is_reset)) ? TRUE : FALSE;

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    map_size = (is_over_fabric) ? dnx_data_fabric.links.nof_links_get(unit) : dnx_data_nif.phys.nof_phys_get(unit);

    SHR_ALLOC(lane2serdes, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit,
                                                             (is_over_fabric) ? DNX_ALGO_LANE_MAP_FABRIC_SIDE :
                                                             DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, lane2serdes));

    nof_phys_per_core =
        (is_over_fabric) ? dnx_data_fabric.links.nof_links_per_core_get(unit) : dnx_data_nif.
        phys.nof_phys_per_core_get(unit);

    BCM_PBMP_CLEAR(tx_phys);
    _SHR_PBMP_ITER(ilkn_phys, iter_phy)
    {
        /*
         * coverity check: make sure iterator is not out of array bounds
         */
        if (iter_phy >= DNX_DATA_MAX_NIF_PHYS_NOF_PHYS)
        {
            break;
        }
        BCM_PBMP_PORT_ADD(tx_phys, lane2serdes[iter_phy].tx_id);
    }
    SHR_IF_ERR_EXIT(imb_ile_port_ilkn_port_lane_map_get
                    (unit, port, DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF, rx_lane_map, tx_lane_map));

    /*
     * Set global FEC enabler
     */
    if ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu)) && ((fec_enable) || (fec_core_reset)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CORE_ENABLERS, &entry_handle_id));

        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);

        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_GLOBAL_ENABLE, INST_SINGLE, fec_enable);

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Clear the handle before re-use
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILE_CORE_ENABLERS, entry_handle_id));
    }

    /*
     * Iteration only on TX phys, under the assumptuion that rx and tx phys lists are equal
     */
    _SHR_PBMP_ITER(tx_phys, iter_phy)
    {
        fec_unit_used = FALSE;
        phy_in_core = iter_phy % nof_phys_per_core;
        fec_pair_phy =
            (iter_phy % dnx_data_nif.ilkn.nof_lanes_per_fec_unit_get(unit)) ? (iter_phy - 1) : (iter_phy + 1);
        /*
         * If the other phy in the unit is used by another ilkn port - do not reset 
         */

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                         &all_ilkn_ports));
        /*
         * iterate over all Ilkn ports and check if other port uses same fec unit
         */
        BCM_PBMP_ITER(all_ilkn_ports, ilkn_port_i)
        {
            if (ilkn_port_i == port)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, ilkn_port_i, 0, &ilkn_port_i_phys));
            if (_SHR_PBMP_MEMBER(ilkn_port_i_phys, fec_pair_phy))
            {
                fec_unit_used = TRUE;
                break;
            }
        }
        if (fec_unit_used && is_reset)
        {
            continue;
        }

        if ((phy_in_core >=
             (dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) * dnx_data_nif.ilkn.nof_lanes_per_fec_unit_get(unit)))
            && (!fec_enable))
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

        pm_index = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->pms[0];
        pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;

        BCM_PBMP_ASSIGN(tmp_bmp, tx_phys);
        BCM_PBMP_AND(tmp_bmp, pm_phys);
        _SHR_PBMP_FIRST(tmp_bmp, first_phy_in_pm);
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get
                        (unit, core, first_phy_in_pm, &ethu_id_in_core, &first_lane_in_cdu));

        tx_serdes_source =
            (dnx_data_nif.ilkn.feature_get(unit,
                                           dnx_data_nif_ilkn_fec_units_in_cdu)) ? first_lane_in_cdu : tx_lane_map[0];

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_FEC_ENABLE, &entry_handle_id));
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_UNIT,
                                   (phy_in_core / dnx_data_nif.ilkn.nof_lanes_per_fec_unit_get(unit)));
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_TX_SERDES_EN, INST_SINGLE, tx_serdes_source);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_ENCODER_RESET, INST_SINGLE, fec_enable);

        if (fec_bypass_valid)
        {
            if (is_reset)
            {
                dbal_field_val = DBAL_ENUM_FVAL_FEC_DECODER_CONTROL_MODE_IN_RESET;
            }
            else
            {
                dbal_field_val = (fec_bypass_enable) ? DBAL_ENUM_FVAL_FEC_DECODER_CONTROL_MODE_BYPASS :
                    DBAL_ENUM_FVAL_FEC_DECODER_CONTROL_MODE_OUT_OF_RESET;
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_DECODER_CONTROL, INST_SINGLE,
                                         dbal_field_val);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_DECODER_RESET, INST_SINGLE, fec_enable);
        }

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

exit:
    SHR_FREE(lane2serdes);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - prepare to get rx counter for setting ilkn rx ctrl.
 *
 * see .h file
 */
int
imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(
    int unit,
    bcm_port_t port,
    int channel)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_RX_STATS_ACC, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT, port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDR, INST_SINGLE, channel);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - prepare to get tx counter for setting ilkn tx ctrl.
 *
 * see .h file
 */
int
imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(
    int unit,
    bcm_port_t port,
    int channel)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_TX_STATS_ACC, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT, port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDR, INST_SINGLE, channel);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get one ILKN counter from HW.
 *
 * see .h file
 */
int
imb_ile_port_ilkn_counter_dbal_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint32 *val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_STATS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT, port);
    /*
     * get value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, val);
    /*
     *  Preforming the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get one ILKN fec counter from HW.
 *
 * see .h file
 */
int
imb_ile_port_ilkn_fec_counter_dbal_get(
    int unit,
    bcm_port_t port,
    uint32 fec_unit,
    dbal_fields_e field_id,
    uint32 *val)
{
    uint32 entry_handle_id;
    int core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_ELK_FEC_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_UNIT, fec_unit);
    /*
     * get value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, val);
    /*
     *  Preforming the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable ILKN TX traffic pause in reaction to RxLLFC.
 * see .h file
 */
int
imb_ile_port_rx_llfc_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CORE_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_LLFC_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the various mux selectors in the cdu/clu blocks for the ilkn port.
 * see .h file
 */
int
imb_ile_port_mux_selectors_set(
    int unit,
    bcm_port_t port)
{
    uint32 entry_handle_id;
    int iter_phy, lane, ethu_id_in_core, ethu_id, core, imb_type_id, is_over_fabric, pm_index, facing_core_id,
        number_of_non_facing_lanes, pm_id_in_clu, facing_clu, is_facing = 1, rx_direction = 0, map_size;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    imb_dispatch_type_t imb_type;
    bcm_pbmp_t ilkn_phys;
    bcm_pbmp_t rx_phys;
    bcm_pbmp_t tx_phys;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(rx_phys);
    BCM_PBMP_CLEAR(tx_phys);

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    map_size = (is_over_fabric) ? dnx_data_fabric.links.nof_links_get(unit) : dnx_data_nif.phys.nof_phys_get(unit);

    SHR_ALLOC(lane2serdes, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit,
                                                             (is_over_fabric) ? DNX_ALGO_LANE_MAP_FABRIC_SIDE :
                                                             DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, lane2serdes));

    BCM_PBMP_CLEAR(rx_phys);
    BCM_PBMP_CLEAR(tx_phys);
    _SHR_PBMP_ITER(ilkn_phys, iter_phy)
    {
        /*
         * coverity check: make sure iterator is not out of array bounds
         */
        if (iter_phy >= DNX_DATA_MAX_NIF_PHYS_NOF_PHYS)
        {
            break;
        }
        BCM_PBMP_PORT_ADD(rx_phys, lane2serdes[iter_phy].rx_id);
        BCM_PBMP_PORT_ADD(tx_phys, lane2serdes[iter_phy].tx_id);
    }

    _SHR_PBMP_ITER(tx_phys, iter_phy)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));

        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

        imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;
        imb_type_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;

        pm_index =
            dnx_data_nif.eth.phy_map_get(unit, (iter_phy / dnx_data_nif.eth.phy_map_granularity_get(unit)))->pm_index;

        facing_core_id = dnx_data_nif.ilkn.nif_pms_get(unit, pm_index)->facing_core_index;
        is_facing = (facing_core_id == ilkn_access_info.ilkn_core) ? 1 : 0;

        switch (imb_type)
        {
            case imbDispatchTypeImb_cdu:

                if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_cdu_ilkn_selectors))
                {
                    /*
                     * alloc DBAL table handle
                     */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CDU_SELECTORS, &entry_handle_id));
                    /*
                     * set key fields
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, imb_type_id);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);
                    /*
                     * set value fields
                     */
                    if (!is_over_fabric)
                    {
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACING_CORE_SELECT, INST_SINGLE,
                                                     is_facing);
                    }
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ENABLE, INST_SINGLE, 1);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_FABRIC_SELECT, INST_SINGLE,
                                                 is_over_fabric);
                    /*
                     * commit the value
                     */
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                    DBAL_HANDLE_FREE(unit, entry_handle_id);
                }

                if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_12lanes_mode))
                {

                    facing_core_id = dnx_data_nif.ilkn.fabric_pms_get(unit, pm_index)->facing_core_index;
                    is_facing = (facing_core_id == ilkn_access_info.ilkn_core) ? 1 : 0;

                    if (!is_facing)
                    {
                        /*
                         * alloc DBAL table handle
                         */
                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                        (unit, DBAL_TABLE_NIF_ILE_12LANES_MODE_SELECTORS, &entry_handle_id));
                        /*
                         * set key fields
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID,
                                                   ilkn_access_info.ilkn_core);
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                                   DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
                        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, 0, 3);
                        /*
                         * set value fields
                         */

                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_12LANES_MODE_SLECTOR,
                                                     INST_SINGLE, 1);
                        /*
                         * commit the value
                         */
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, facing_core_id);
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                                   DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
                        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, 6, 7);
                        /*
                         * set value fields
                         */

                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_12LANES_MODE_SLECTOR,
                                                     INST_SINGLE, 1);
                        /*
                         * commit the value
                         */

                        DBAL_HANDLE_FREE(unit, entry_handle_id);
                    }
                }
                break;

            case imbDispatchTypeImb_clu:

                if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_clu_ilkn_selectors))
                {
                    /*
                     * alloc DBAL table handle
                     */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CLU_TX_SELECTORS, &entry_handle_id));
                    /*
                     * set key fields
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, imb_type_id);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CLU, lane);
                    /*
                     * set value fields
                     */
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACING_CORE_SELECT, INST_SINGLE,
                                                 is_facing);

                    /*
                     * commit the value
                     */
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                    DBAL_HANDLE_FREE(unit, entry_handle_id);

                    /** open the up 8 ilkn lanes to pass data from facing ile to up/down clup  */
                    if (!is_facing)
                    {
                        facing_clu =
                            dnx_data_nif.ilkn.ilkn_cores_clup_facing_get(unit, ilkn_access_info.ilkn_core)->clu_unit;

                        number_of_non_facing_lanes =
                            dnx_data_nif.ilkn.lanes_max_nof_get(unit) - dnx_data_nif.eth.nof_lanes_in_clu_get(unit);

                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CLU_ENABLERS, &entry_handle_id));
                        /*
                         * set key fields
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, facing_clu);
                        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NON_FACING_LANE, 0,
                                                         (number_of_non_facing_lanes - 1));
                        /*
                         * set value fields
                         */
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_ENABLER, INST_SINGLE, 1);

                        /*
                         * commit the value
                         */
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                        DBAL_HANDLE_FREE(unit, entry_handle_id);
                    }
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ETHU type %d\n", imb_type);

        }
    }

    _SHR_PBMP_ITER(rx_phys, iter_phy)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));

        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

        imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;
        imb_type_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;

        pm_index =
            dnx_data_nif.eth.phy_map_get(unit, (iter_phy / dnx_data_nif.eth.phy_map_granularity_get(unit)))->pm_index;

        facing_core_id = dnx_data_nif.ilkn.nif_pms_get(unit, pm_index)->facing_core_index;
        is_facing = (facing_core_id == ilkn_access_info.ilkn_core) ? 1 : 0;

        switch (imb_type)
        {
            case imbDispatchTypeImb_cdu:

                if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_cdu_ilkn_selectors))
                {
                    /*
                     * alloc DBAL table handle
                     */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CDU_SELECTORS, &entry_handle_id));
                    /*
                     * set key fields
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, imb_type_id);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);
                    /*
                     * set value fields
                     */
                    if (!is_over_fabric)
                    {
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACING_CORE_SELECT, INST_SINGLE,
                                                     is_facing);
                    }
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ENABLE, INST_SINGLE, 1);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_FABRIC_SELECT, INST_SINGLE,
                                                 is_over_fabric);
                    /*
                     * commit the value
                     */
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                    DBAL_HANDLE_FREE(unit, entry_handle_id);
                }
                if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_12lanes_mode))
                {

                    facing_core_id = dnx_data_nif.ilkn.fabric_pms_get(unit, pm_index)->facing_core_index;
                    is_facing = (facing_core_id == ilkn_access_info.ilkn_core) ? 1 : 0;

                    if (!is_facing)
                    {
                        /*
                         * alloc DBAL table handle
                         */
                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                        (unit, DBAL_TABLE_NIF_ILE_12LANES_MODE_SELECTORS, &entry_handle_id));
                        /*
                         * set key fields
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID,
                                                   ilkn_access_info.ilkn_core);
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                                   DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX);
                        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, 0, 5);
                        /*
                         * set value fields
                         */

                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_12LANES_MODE_SLECTOR,
                                                     INST_SINGLE, 1);
                        /*
                         * commit the value
                         */
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                        DBAL_HANDLE_FREE(unit, entry_handle_id);
                    }
                }
                break;

            case imbDispatchTypeImb_clu:

                if ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_clu_ilkn_selectors)) && (!is_facing))
                {
                    pm_id_in_clu =
                        lane / dnx_data_port.imb.imb_type_info_get(unit, imbDispatchTypeImb_clu)->nof_lanes_in_pm;
                    /*
                     * alloc DBAL table handle
                     */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CLU_PM_RX_SELECTORS, &entry_handle_id));
                    /*
                     * set key fields
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, imb_type_id);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM, pm_id_in_clu);
                    /*
                     * set value fields
                     */
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NON_FACING_ENABLE, INST_SINGLE,
                                                 is_facing);

                    /*
                     * commit the value
                     */
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                    DBAL_HANDLE_FREE(unit, entry_handle_id);

                    /** This section is uneeded for Q2A */
                    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_clu_ilkn_direction_selectors))
                    {

                        /** open the up 8 ilkn lanes to pass data from facing ile to up/down clup  */

                        facing_clu =
                            dnx_data_nif.ilkn.ilkn_cores_clup_facing_get(unit, ilkn_access_info.ilkn_core)->clu_unit;

                        rx_direction = (imb_type_id < facing_clu) ? 1 : 0;

                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CLU_ENABLERS, &entry_handle_id));
                        /*
                         * set key fields
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, facing_clu);

                        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NON_FACING_LANE, 0, 7);
                        /*
                         * set value fields
                         */
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_DIRECTION, INST_SINGLE,
                                                     rx_direction);
                        /*
                         * commit the value
                         */
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                        DBAL_HANDLE_FREE(unit, entry_handle_id);
                    }
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ETHU type %d\n", imb_type);

        }
    }

exit:
    SHR_FREE(lane2serdes);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the various mux selectors in the cdu/clu blocks for the ilkn port.
 * Currently only TX enabler MUX which enable/disable  CLUP to transmit for up/down CLUP is relevant.
 * In case it's a single port on the ilkn core, or if all second port's lanes are facing - need to reset this mux for not transmitting for up/down CLUP for power optimization.
 * see .h file
 */
int
imb_ile_port_mux_selectors_reset(
    int unit,
    bcm_port_t port,
    int pm_imb_type,
    int ilkn_nof_ports)
{
    uint32 entry_handle_id;
    int iter_phy, lane, ethu_id_in_core, ethu_id, core, is_over_fabric, pm_index, facing_core_id,
        number_of_non_facing_lanes, facing_clu, is_facing = 1, second_ilkn_port =
        -1, ilkn_port_i, map_size, imb_type_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info, port_i_ilkn_info;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    uint8 disable_tx = FALSE;
    uint8 non_facing_found = FALSE;
    bcm_pbmp_t ilkn_phys, second_port_phys;
    bcm_pbmp_t tx_phys, rx_phys;
    bcm_pbmp_t all_ilkn_ports;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(tx_phys);
    BCM_PBMP_CLEAR(rx_phys);
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    map_size = (is_over_fabric) ? dnx_data_fabric.links.nof_links_get(unit) : dnx_data_nif.phys.nof_phys_get(unit);

    SHR_ALLOC(lane2serdes, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit,
                                                             (is_over_fabric) ? DNX_ALGO_LANE_MAP_FABRIC_SIDE :
                                                             DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, lane2serdes));

    _SHR_PBMP_ITER(ilkn_phys, iter_phy)
    {
        /*
         * coverity check: make sure iterator is not out of array bounds
         */
        if (iter_phy >= DNX_DATA_MAX_NIF_PHYS_NOF_PHYS)
        {
            break;
        }
        BCM_PBMP_PORT_ADD(rx_phys, lane2serdes[iter_phy].rx_id);
        BCM_PBMP_PORT_ADD(tx_phys, lane2serdes[iter_phy].tx_id);
    }

    switch (pm_imb_type)
    {
        case imbDispatchTypeImb_cdu:
            if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_cdu_ilkn_selectors))
            {
                _SHR_PBMP_ITER(tx_phys, iter_phy)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));

                    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

                    imb_type_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;

                    /*
                     * alloc DBAL table handle
                     */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CDU_SELECTORS, &entry_handle_id));
                    /*
                     * set key fields
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, imb_type_id);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ENABLE, INST_SINGLE, 0);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_FABRIC_SELECT, INST_SINGLE, 0);
                    /*
                     * commit the value
                     */
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                    DBAL_HANDLE_FREE(unit, entry_handle_id);
                }

                _SHR_PBMP_ITER(rx_phys, iter_phy)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));

                    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

                    imb_type_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;

                    /*
                     * alloc DBAL table handle
                     */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CDU_SELECTORS, &entry_handle_id));
                    /*
                     * set key fields
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, imb_type_id);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ENABLE, INST_SINGLE, 0);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_FABRIC_SELECT, INST_SINGLE, 0);
                    /*
                     * commit the value
                     */
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                    DBAL_HANDLE_FREE(unit, entry_handle_id);
                }
            }

            break;
        case imbDispatchTypeImb_clu:

            if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_clu_ilkn_selectors))
            {
                /*
                 * If this port is the only port in the ilkn core - change the TX enable mux to not transmit for up/down CLUP
                 */
                if (ilkn_nof_ports == 1)
                {
                    disable_tx = TRUE;
                }
                else
                {
                    /*
                     * check that for the second port all lanes are facing
                     */
                    /*
                     * Retrieve all ilkn ports which were already configured
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                                    (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN,
                                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &all_ilkn_ports));
                    /*
                     * iterate over all Ilkn ports and check if MAC is is the same as given port
                     */
                    BCM_PBMP_ITER(all_ilkn_ports, ilkn_port_i)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, ilkn_port_i, &port_i_ilkn_info));
                        if (port_i_ilkn_info.ilkn_core == ilkn_access_info.ilkn_core)
                        {
                            second_ilkn_port = ilkn_port_i;
                            break;
                        }
                    }

                    /*
                     * Extract ilkn phys for the other port
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, second_ilkn_port, 0, &second_port_phys));

                    _SHR_PBMP_ITER(second_port_phys, iter_phy)
                    {
                        pm_index =
                            dnx_data_nif.eth.phy_map_get(unit,
                                                         (iter_phy /
                                                          dnx_data_nif.eth.phy_map_granularity_get(unit)))->pm_index;

                        facing_core_id = dnx_data_nif.ilkn.nif_pms_get(unit, pm_index)->facing_core_index;
                        is_facing = (facing_core_id == ilkn_access_info.ilkn_core) ? 1 : 0;
                        if (!is_facing)
                        {
                            non_facing_found = TRUE;
                            break;
                        }
                    }
                    /*
                     * If all lanes for second port are facing - change the TX enable mux to not transmit for up/down CLUP
                     */
                    if (!non_facing_found)
                    {
                        disable_tx = TRUE;
                    }
                }

                /*
                 * change the TX enable mux of the current CLUP to not transmit for up/down CLUP
                 */
                if (disable_tx)
                {
                    _SHR_PBMP_ITER(tx_phys, iter_phy)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get
                                        (unit, core, iter_phy, &ethu_id_in_core, &lane));

                        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

                        pm_index =
                            dnx_data_nif.eth.phy_map_get(unit,
                                                         (iter_phy /
                                                          dnx_data_nif.eth.phy_map_granularity_get(unit)))->pm_index;

                        facing_core_id = dnx_data_nif.ilkn.nif_pms_get(unit, pm_index)->facing_core_index;
                        is_facing = (facing_core_id == ilkn_access_info.ilkn_core) ? 1 : 0;

                        if (!is_facing)
                        {
                            facing_clu =
                                dnx_data_nif.ilkn.ilkn_cores_clup_facing_get(unit,
                                                                             ilkn_access_info.ilkn_core)->clu_unit;

                            number_of_non_facing_lanes =
                                dnx_data_nif.ilkn.lanes_max_nof_get(unit) - dnx_data_nif.eth.nof_lanes_in_clu_get(unit);

                            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CLU_ENABLERS, &entry_handle_id));
                            /*
                             * set key fields
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, facing_clu);
                            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NON_FACING_LANE, 0,
                                                             (number_of_non_facing_lanes - 1));
                            /*
                             * set value fields
                             */
                            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_ENABLER, INST_SINGLE, 0);

                            /*
                             * commit the value
                             */
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                            DBAL_HANDLE_FREE(unit, entry_handle_id);
                        }
                    }
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ETHU type %d\n", pm_imb_type);
    }

exit:
    SHR_FREE(lane2serdes);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ILKN alignment status from HW.
 * see .h file
 */
int
imb_ile_port_alignment_live_status_get(
    int unit,
    bcm_port_t port,
    uint32 *live_status)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_PORT_ALIGNMENT_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
    /*
     * get value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIVE_STATUS, INST_SINGLE, live_status);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ILKN alignment lost/achieved counters
 * see .h file
 */
int
imb_ile_port_alignment_counters_get(
    int unit,
    bcm_port_t port,
    uint32 *achieved_counter,
    uint32 *lost_counter)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_PORT_ALIGNMENT_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
    /*
     * get value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACHIEVED_COUNTER, INST_SINGLE, achieved_counter);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LOST_COUNTER, INST_SINGLE, lost_counter);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure power down of ILE
 */
int
imb_ile_power_down_set(
    int unit,
    int ilkn_core_id,
    int power_down)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_POWER_DOWN, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN, INST_SINGLE, power_down);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
