
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm/bfd.h>

#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#ifdef BCM_DNX2_SUPPORT
#include <src/bcm/dnx/oam/oamp_v1/oamp_v1_pe.h>
#include <src/bcm/dnx/oam/oamp_v1/oamp_v1_pe_infra.h>
#endif
#include <src/bcm/dnx/oam/oam_oamp_crc.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_oam.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <include/bcm/vlan.h>
#include <include/bcm_int/dnx/switch/switch_tpid.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_bfd_access.h>
#include <include/bcm/switch.h>
#include <include/bcm_int/dnx_dispatch.h>
#include <include/soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/mdb.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#ifdef BCM_DNX2_SUPPORT

#include <src/bcm/dnx/oam/oamp_v1/bfd_oamp_v1.h>
#include <src/bcm/dnx/oam/oamp_v1/oam_oamp_v1.h>

#endif

#define OAM_PUNT_FHEI_SIZE 2
#define OAM_PUNT_FHEI_JR1_SIZE 1

#define DNX_OAM_ETH_SA_MAC_LSB_SIZE_IN_BITS     (15)
#define DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK UTILEX_BITS_MASK(DNX_OAM_ETH_SA_MAC_LSB_SIZE_IN_BITS - SAL_UINT8_NOF_BITS - 1,0)

#define DEFAULT_MEP_TYPE_MASK 0
#define DEFAULT_GACH_SEL_MASK SAL_UPTO_BIT(1)
#define DEFAULT_GACH_CHAN_MASK SAL_UPTO_BIT(16)
#define DEFAULT_TRAP_CODE_MASK 0

#define NOF_CLS_TRAP_TABLE_ENTRIES SAL_UPTO_BIT(5)

#define DNX_OAM_GET_RDI_GEN_METHOD_FIELD_FROM_MEP_PROFILE(rdi_method, is_rdi_from_rx, is_rdi_from_scanner) \
    do {\
        is_rdi_from_rx = ((rdi_method & 0x2)==0x2);\
        is_rdi_from_scanner = (rdi_method & 0x1);\
    } while (0)

#define DNX_OAM_RSP_TCAM_OPCODE_TO_OAM_TS_FORMAT(opcode_or_channel) \
    (((opcode_or_channel)==DBAL_DEFINE_OAM_OPCODE_LMM) || ((opcode_or_channel)==DBAL_DEFINE_OAM_OPCODE_SLM))? DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT : \
    ((opcode_or_channel)==DBAL_DEFINE_OAM_OPCODE_DMM)? DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588: \
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL

#define _DNX_OAM_GET_OAMP_PENDING_EVENT_BY_DEVICE(unit) dnx_data_oam.oamp.intr_oamp_pending_event_get(unit)

#define _DNX_OAM_GET_OAMP_STAT_PENDING_EVENT_BY_DEVICE(unit) dnx_data_oam.oamp.intr_oamp_stat_pending_event_get(unit)

void
dnx_oam_oamp_statistics_enable(
    int unit,
    uint32 flags,
    dnx_oam_mep_profile_t * mep_profile_data)
{

    mep_profile_data->mep_id_shift = 0;
    mep_profile_data->opcode_mask = FALSE;
    mep_profile_data->opcode_tx_statistics_enable = FALSE;
    mep_profile_data->opcode_rx_statistics_enable = FALSE;

    if (_SHR_IS_FLAG_SET(flags, BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS)
        || _SHR_IS_FLAG_SET(flags, BCM_BFD_ENDPOINT_FLAGS2_TX_STATISTICS))
    {
        mep_profile_data->opcode_tx_statistics_enable = TRUE;
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS)
        || _SHR_IS_FLAG_SET(flags, BCM_BFD_ENDPOINT_FLAGS2_RX_STATISTICS))
    {
        mep_profile_data->opcode_rx_statistics_enable = TRUE;
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS))
    {
        mep_profile_data->opcode_mask = TRUE;
        mep_profile_data->mep_id_shift = 3;
    }

}

static dbal_enum_value_result_type_oamp_rmep_db_e
dnx_oam_oamp_rmep_type_get(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam)
{
    uint32 self_contained_threshold;

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_idx < self_contained_threshold)
    {
        return is_oam ? DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED : DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED;
    }
    else
    {
        return is_oam ? DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_SELF_CONTAINED :
            DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_SELF_CONTAINED;
    }
}

void
dnx_oam_oamp_period_to_lifetime(
    int unit,
    uint32 period_us,
    uint32 *lifetime,
    uint32 *lifetime_units)
{
    uint32 rmep_scan_nof_clocks;
    uint32 rmep_scan_us;

    rmep_scan_nof_clocks =
        (dnx_data_oam.oamp.rmep_max_self_contained_get(unit) * dnx_data_oam.oamp.rmep_pair_sys_clocks_get(unit)) / 2;

    rmep_scan_us = rmep_scan_nof_clocks * 1000 / dnx_data_device.general.core_clock_khz_get(unit);

    if ((period_us / rmep_scan_us) <= 0x3FF)
    {
        *lifetime_units = DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_1;
        *lifetime = ((period_us / rmep_scan_us) < 1) ? 1 : (period_us / rmep_scan_us);
    }
    else if (((period_us / 100) / rmep_scan_us) <= 0x3FF)
    {
        *lifetime_units = DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_100;
        *lifetime = (((period_us / 100) / rmep_scan_us) < 1) ? 1 : ((period_us / 100) / rmep_scan_us);
    }
    else
    {
        *lifetime_units = DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_10000;
        *lifetime =
            (((((period_us / 1000))) / (rmep_scan_us * 10)) < 1) ? 1 : ((((period_us / 1000))) / (rmep_scan_us * 10));
    }
}

static void
dnx_oam_oamp_lifetime_to_period(
    int unit,
    uint32 lifetime,
    uint32 lifetime_units,
    uint32 *period_us)
{
    uint32 rmep_scan_nof_clocks;
    uint32 rmep_scan_us;

    rmep_scan_nof_clocks =
        (dnx_data_oam.oamp.rmep_max_self_contained_get(unit) * dnx_data_oam.oamp.rmep_pair_sys_clocks_get(unit)) / 2;

    rmep_scan_us = rmep_scan_nof_clocks * 1000 / dnx_data_device.general.core_clock_khz_get(unit);

    if (lifetime_units == DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_1)
    {
        *period_us = rmep_scan_us * lifetime;
    }
    else
    {
        if (lifetime_units == DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_100)
        {
            *period_us = rmep_scan_us * lifetime * 100;
        }
        else
        {
            *period_us = rmep_scan_us * lifetime * 10000;
        }
    }

}

shr_error_e
dnx_oam_oamp_ccm_period_to_ccm_interval(
    int unit,
    int ccm_period,
    dbal_enum_value_field_ccm_interval_e * ccm_interval)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (ccm_period)
    {
        case BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_3MS:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_3MS;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10MS:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_10MS;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_100MS:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_100MS;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1S:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_1S;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10S:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_10S;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1M:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_1M;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10M:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_10M;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid ccm_period passed %d.  Use one of the BCM_OAM_ENDPOINT_CCM_PERIOD_ constants.",
                         ccm_period);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_ccm_interval_to_ccm_period(
    int unit,
    dbal_enum_value_field_ccm_interval_e ccm_interval,
    int *ccm_period)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (ccm_interval)
    {
        case DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_3MS:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_10MS:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_100MS:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_1S:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_10S:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10S;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_1M:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1M;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_10M:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10M;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ccm_interval passed %d", ccm_interval);
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_oam_oamp_rmep_compare(
    int is_oam,
    dnx_oam_oamp_rmep_db_entry_t * old_entry_values,
    dnx_oam_oamp_rmep_db_entry_t * new_entry_values)
{

    if (is_oam)
    {
        if (old_entry_values->rmep_state.eth_state.interface_status !=
            new_entry_values->rmep_state.eth_state.interface_status)
        {
            return TRUE;
        }

        if (old_entry_values->rmep_state.eth_state.port_status != new_entry_values->rmep_state.eth_state.port_status)
        {
            return TRUE;
        }
    }
    else
    {
        if (old_entry_values->rmep_state.bfd_state.detect_multiplier !=
            new_entry_values->rmep_state.bfd_state.detect_multiplier)
        {
            return TRUE;
        }

        if (old_entry_values->rmep_state.bfd_state.state != new_entry_values->rmep_state.bfd_state.state)
        {
            return TRUE;
        }

        if (old_entry_values->rmep_state.bfd_state.flags_profile !=
            new_entry_values->rmep_state.bfd_state.flags_profile)
        {
            return TRUE;
        }

        if (old_entry_values->rmep_state.bfd_state.diag_profile != new_entry_values->rmep_state.bfd_state.diag_profile)
        {
            return TRUE;
        }
    }

    if (old_entry_values->oam_id != new_entry_values->oam_id)
    {
        return TRUE;
    }

    if (old_entry_values->period != new_entry_values->period)
    {
        return TRUE;
    }

    if (old_entry_values->punt_profile != new_entry_values->punt_profile)
    {
        return TRUE;
    }
    if (old_entry_values->punt_next_good_packet != new_entry_values->punt_next_good_packet)
    {
        return TRUE;
    }
    if (old_entry_values->punt_good_profile != new_entry_values->punt_good_profile)
    {
        return TRUE;
    }
    if (old_entry_values->loc_clear_threshold != new_entry_values->loc_clear_threshold)
    {
        return TRUE;
    }

    return 0;
}

shr_error_e
dnx_oam_oamp_rmep_add(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam,
    uint32 is_update,
    dnx_oam_oamp_rmep_db_entry_t * rmep_entry)
{
    dbal_tables_e rmep_table;
    dbal_entry_action_flags_e commit_type;
    uint32 entry_handle_id;
    dbal_enum_value_result_type_oamp_rmep_db_e rmep_type;
    uint32 lifetime;
    uint32 lifetime_units;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    commit_type = DBAL_COMMIT;

    rmep_type = dnx_oam_oamp_rmep_type_get(unit, rmep_idx, is_oam);

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB;
    }
    else
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_DB;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, rmep_table, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    if (DBAL_TABLE_OAMP_RMEP_DB == rmep_table)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, rmep_type);
    }

    dnx_oam_oamp_period_to_lifetime(unit, rmep_entry->period, &lifetime, &lifetime_units);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFETIME, INST_SINGLE, lifetime);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFETIME_UNITS, INST_SINGLE, lifetime_units);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_LIMIT, INST_SINGLE,
                                 rmep_entry->loc_clear_threshold);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_ENABLE, INST_SINGLE,
                                 (rmep_entry->loc_clear_threshold == 0) ? 0 : 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, INST_SINGLE, rmep_entry->punt_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, rmep_entry->oam_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_GOOD_PROFILE, INST_SINGLE,
                                 rmep_entry->punt_good_profile);

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_RMEP_DB, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, rmep_type);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LIFETIME_STAMP, INST_SINGLE, lifetime);

    if ((!is_update || !is_oam) && (rmep_entry->last_ccm_lifetime_valid_on_create == 1))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LIFETIME_STAMP_VALID,
                                     INST_SINGLE, rmep_entry->last_ccm_lifetime_valid_on_create);
    }

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_SELF_CONTAINED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH_INTERFACE_STATE, INST_SINGLE,
                                     rmep_entry->rmep_state.eth_state.interface_status);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH_PORT_STATE, INST_SINGLE,
                                     rmep_entry->rmep_state.eth_state.port_status);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.flags_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.state);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.diag_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.detect_multiplier);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_NEXT_GOOD_PACKET, INST_SINGLE,
                                 rmep_entry->punt_next_good_packet);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_type));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_rmep_get(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam,
    dnx_oam_oamp_rmep_db_entry_t * rmep_entry)
{
    dbal_tables_e rmep_table;
    uint32 entry_handle_id;
    uint32 rmep_type;
    uint32 lifetime;
    uint32 lifetime_units;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    rmep_type = dnx_oam_oamp_rmep_type_get(unit, rmep_idx, is_oam);

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB;
    }
    else
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_DB;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, rmep_table, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LIFETIME, INST_SINGLE, &lifetime));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LIFETIME_UNITS, INST_SINGLE, &lifetime_units));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_LIMIT, INST_SINGLE,
                                                       &rmep_entry->loc_clear_threshold));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, INST_SINGLE,
                                                       &rmep_entry->punt_profile));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, &rmep_entry->oam_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_PUNT_GOOD_PROFILE, INST_SINGLE,
                                                       &rmep_entry->punt_good_profile));

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_RMEP_DB, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    }

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_SELF_CONTAINED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_ETH_INTERFACE_STATE, INST_SINGLE,
                         &rmep_entry->rmep_state.eth_state.interface_status));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_ETH_PORT_STATE, INST_SINGLE,
                         &rmep_entry->rmep_state.eth_state.port_status));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                         &rmep_entry->rmep_state.bfd_state.flags_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                         &rmep_entry->rmep_state.bfd_state.state));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                         &rmep_entry->rmep_state.bfd_state.diag_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                                            &rmep_entry->rmep_state.bfd_state.detect_multiplier));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_RDI_RECEIVED, INST_SINGLE,
                                                       &rmep_entry->rdi_received));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_LOC, INST_SINGLE, &rmep_entry->loc));

    dnx_oam_oamp_lifetime_to_period(unit, lifetime, lifetime_units, &rmep_entry->period);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_rmep_remove(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam)
{
    dbal_tables_e rmep_table;
    uint32 entry_handle_id;
    uint32 rmep_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    rmep_type = dnx_oam_oamp_rmep_type_get(unit, rmep_idx, is_oam);

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB;
    }
    else
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_DB;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, rmep_table, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    if (DBAL_TABLE_OAMP_RMEP_DB == rmep_table)
    {

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, rmep_type);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LIFETIME_STAMP_VALID, INST_SINGLE, 0);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, rmep_table, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);
    }

    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_RMEP_DB, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_punt_profile_tbl_get(
    int unit,
    int profile_id,
    dnx_oam_oamp_punt_event_profile_t * punt_profile_data)
{
    uint32 entry_handle_id;
    uint32 punt_rate;
    uint32 punt_enable;
    uint32 rx_state_update_en;
    uint32 profile_scan_state_update_en;
    uint32 mep_rdi_update_loc_en;
    uint32 mep_rdi_update_loc_clear_en;
    uint32 mep_rdi_update_rx_en;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_PUNT_PROFILE_TBL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, profile_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_PUNT_RATE, INST_SINGLE, &(punt_rate));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_PUNT_ENABLE, INST_SINGLE, &(punt_enable));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_RX_STATE_UPDATE_EN, INST_SINGLE, &(rx_state_update_en));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_SCAN_STATE_UPDATE_EN, INST_SINGLE, &(profile_scan_state_update_en));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_EN, INST_SINGLE, &(mep_rdi_update_loc_en));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_CLEAR_EN, INST_SINGLE,
                               &(mep_rdi_update_loc_clear_en));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_RX_EN, INST_SINGLE,
                               &(mep_rdi_update_rx_en));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    punt_profile_data->punt_rate = punt_rate;
    punt_profile_data->punt_enable = punt_enable;
    punt_profile_data->rx_state_update_en = rx_state_update_en;
    punt_profile_data->profile_scan_state_update_en = profile_scan_state_update_en;
    punt_profile_data->mep_rdi_update_loc_en = mep_rdi_update_loc_en;
    punt_profile_data->mep_rdi_update_loc_clear_en = mep_rdi_update_loc_clear_en;
    punt_profile_data->mep_rdi_update_rx_en = mep_rdi_update_rx_en;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_punt_profile_tbl_set(
    int unit,
    int profile_id,
    const dnx_oam_oamp_punt_event_profile_t * punt_profile_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_PUNT_PROFILE_TBL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, profile_id);

    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_PROFILE_PUNT_RATE, INST_SINGLE, punt_profile_data->punt_rate);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_PROFILE_PUNT_ENABLE, INST_SINGLE, punt_profile_data->punt_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_PROFILE_RX_STATE_UPDATE_EN, INST_SINGLE,
                                 punt_profile_data->rx_state_update_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_SCAN_STATE_UPDATE_EN, INST_SINGLE,
                                 punt_profile_data->profile_scan_state_update_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_EN, INST_SINGLE,
                                 punt_profile_data->mep_rdi_update_loc_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_CLEAR_EN, INST_SINGLE,
                                 punt_profile_data->mep_rdi_update_loc_clear_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_RX_EN, INST_SINGLE,
                                 punt_profile_data->mep_rdi_update_rx_en);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_period_to_rmep_db_nof_scans(
    int unit,
    int period,
    uint16 *nof_hw_scans)
{
    COMPILER_UINT64 nof_hw_scans_64_bit;
    COMPILER_UINT64 rmep_db_scan_interval_ns;
    COMPILER_UINT64 rmep_db_half_scan_interval_ns;
    COMPILER_UINT64 work_reg_64_a;
    COMPILER_UINT64 work_reg_64_b;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_SET(rmep_db_scan_interval_ns, 0, dnx_data_oam.oamp.rmep_max_self_contained_get(unit) / 2);
    COMPILER_64_UMUL_32(rmep_db_scan_interval_ns, dnx_data_oam.oamp.rmep_pair_sys_clocks_get(unit));
    COMPILER_64_UMUL_32(rmep_db_scan_interval_ns, 1000000);
    COMPILER_64_UDIV_32(rmep_db_scan_interval_ns, dnx_data_device.general.core_clock_khz_get(unit));

    rmep_db_half_scan_interval_ns = rmep_db_scan_interval_ns;
    COMPILER_64_UDIV_32(rmep_db_half_scan_interval_ns, 2);

    COMPILER_64_SET(nof_hw_scans_64_bit, 0, period);

    COMPILER_64_UMUL_32(nof_hw_scans_64_bit, 1000000);
    COMPILER_64_ADD_64(nof_hw_scans_64_bit, rmep_db_half_scan_interval_ns);
    COMPILER_64_UDIV_64(nof_hw_scans_64_bit, rmep_db_scan_interval_ns);
    COMPILER_64_SUB_32(nof_hw_scans_64_bit, 1);

    COMPILER_64_COPY(work_reg_64_a, nof_hw_scans_64_bit);
    COMPILER_64_SET(work_reg_64_b, 0, 0x0000FFFF);
    COMPILER_64_AND(work_reg_64_a, work_reg_64_b);
    *nof_hw_scans = (uint16) COMPILER_64_LO(work_reg_64_a);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_rmep_db_nof_scans_to_period(
    int unit,
    uint16 nof_hw_scans,
    int *period)
{
    COMPILER_UINT64 period_64_bit;
    COMPILER_UINT64 rmep_db_scan_interval_ns;
    COMPILER_UINT64 work_reg_64_a;
    COMPILER_UINT64 work_reg_64_b;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_SET(rmep_db_scan_interval_ns, 0, dnx_data_oam.oamp.rmep_max_self_contained_get(unit) / 2);
    COMPILER_64_UMUL_32(rmep_db_scan_interval_ns, dnx_data_oam.oamp.rmep_pair_sys_clocks_get(unit));
    COMPILER_64_UMUL_32(rmep_db_scan_interval_ns, 1000000);
    COMPILER_64_UDIV_32(rmep_db_scan_interval_ns, dnx_data_device.general.core_clock_khz_get(unit));

    period_64_bit = rmep_db_scan_interval_ns;
    COMPILER_64_UMUL_32(period_64_bit, (nof_hw_scans + 1));
    COMPILER_64_ADD_32(period_64_bit, 500000);
    COMPILER_64_UDIV_32(period_64_bit, 1000000);

    COMPILER_64_COPY(work_reg_64_a, period_64_bit);
    COMPILER_64_SET(work_reg_64_b, 0, 0x0000FFFF);
    COMPILER_64_AND(work_reg_64_a, work_reg_64_b);
    *period = (uint16) COMPILER_64_LO(work_reg_64_a);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_punt_good_profile_alloc(
    int unit,
    uint8 is_update,
    int punt_good_packet_period,
    int old_profile_id,
    dnx_oam_oamp_punt_good_profile_temp_data_t * hw_write_data)
{
    int profile_id = 0;
    uint16 hw_number_of_scans = 0;
    uint8 write_hw = 0, is_last = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (punt_good_packet_period == 0)
    {
        hw_number_of_scans = 0;
    }
    else
    {

        dnx_oam_oamp_period_to_rmep_db_nof_scans(unit, punt_good_packet_period, &hw_number_of_scans);
    }

    if (is_update && old_profile_id != 0)
    {
        if (hw_number_of_scans == 0)
        {

            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_punt_good_profile.free_single(unit, old_profile_id, &is_last));
        }
        else
        {

            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_punt_good_profile.exchange
                            (unit, 0, &hw_number_of_scans, old_profile_id, NULL, &profile_id, &write_hw, &is_last));
        }

        hw_write_data->old_punt_good_profile = old_profile_id;
        hw_write_data->delete_old_punt_good_profile = is_last;
    }
    else
    {
        if (hw_number_of_scans)
        {

            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_punt_good_profile.allocate_single
                            (unit, 0, &hw_number_of_scans, NULL, &profile_id, &write_hw));
        }
    }

    hw_write_data->new_punt_good_profile = profile_id;
    hw_write_data->write_new_punt_good_profile = write_hw;
    hw_write_data->new_punt_good_profile_data = hw_number_of_scans;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_remote_endpoint_punt_good_profile_hw_configure(
    int unit,
    const dnx_oam_oamp_punt_good_profile_temp_data_t * hw_write_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_PUNT_GOOD_TIMER, &entry_handle_id));

    if (hw_write_data->write_new_punt_good_profile)
    {

        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_PUNT_GOOD_PROFILE,
                                  hw_write_data->new_punt_good_profile);

        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RMEP_PUNT_GOOD_TIMER_INIT_VALUE, INST_SINGLE,
                                     hw_write_data->new_punt_good_profile_data);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (hw_write_data->delete_old_punt_good_profile)
    {

        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_PUNT_GOOD_PROFILE,
                                  hw_write_data->old_punt_good_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_punt_good_profile_get(
    int unit,
    uint8 punt_good_profile,
    int *punt_good_packet_period)
{
    uint16 hw_number_of_scans = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (punt_good_profile)
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_PUNT_GOOD_TIMER, &entry_handle_id));

        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_PUNT_GOOD_PROFILE, punt_good_profile);

        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_RMEP_PUNT_GOOD_TIMER_INIT_VALUE, INST_SINGLE,
                                   &hw_number_of_scans);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (hw_number_of_scans)
        {
            dnx_oam_oamp_rmep_db_nof_scans_to_period(unit, hw_number_of_scans, punt_good_packet_period);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_punt_good_profile_free(
    int unit,
    int punt_good_profile,
    dnx_oam_oamp_punt_good_profile_temp_data_t * hw_write_data)
{
    uint8 is_last = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (punt_good_profile)
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_punt_good_profile.free_single(unit, punt_good_profile, &is_last));
    }

    hw_write_data->delete_old_punt_good_profile = is_last;
    hw_write_data->old_punt_good_profile = punt_good_profile;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_opcode_response_set(
    int unit,
    uint16 oam_opcode_or_channel,
    uint8 is_y_1731,
    uint8 index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OPCODE_RESPONSE_TABLE, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE_CHANNEL, oam_opcode_or_channel, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IS_Y1731, is_y_1731, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_RSP_OPCODE_CHANNEL, INST_SINGLE,
                                 (oam_opcode_or_channel - 1));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_TS_FORMAT, INST_SINGLE,
                                 DNX_OAM_RSP_TCAM_OPCODE_TO_OAM_TS_FORMAT(oam_opcode_or_channel));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GENERATE_RSP, INST_SINGLE, 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESERVED, INST_SINGLE, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_init_response_table(
    int unit)
{
    uint8 index = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_opcode_response_set(unit, DBAL_DEFINE_OAM_OPCODE_LBM, 1, index));
    index++;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_opcode_response_set(unit, DBAL_DEFINE_OAM_OPCODE_LTM, 1, index));
    index++;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_opcode_response_set(unit, DBAL_DEFINE_OAM_OPCODE_EXM, 1, index));
    index++;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_init_general_configuration(
    int unit)
{
    uint32 scanner_limitation;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    scanner_limitation = dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_scanner_limitation);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMR_LMR_RESPONSE_ENABLE, INST_SINGLE, 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMR_LMR_RESPONSE_DROP_ENABLE, INST_SINGLE, 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GOOD_PACKET_PUNT_ENABLE, INST_SINGLE,
                                 (scanner_limitation ? 0 : 1));

    if (dnx_data_oam.oamp.slm_supported_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR_ENABLE, INST_SINGLE,
                                     0);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REPORT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_REPORT_MODE_NORMAL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_OF_RMEPS, INST_SINGLE,
                                 (dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) +
                                  (scanner_limitation ? 0 : 1)));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SEC, INST_SINGLE,
                                 dnx_data_device.general.core_clock_khz_get(unit) * 1000);

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_protection_state_ignore) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, INST_SINGLE,
                                     DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION_DOWN);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, INST_SINGLE,
                                     DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_SET_EVENT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALMOST_LOC_SET_EVENT_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_EVENT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_SET_EVENT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_CLEAR_EVENT_ENABLE, INST_SINGLE, 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_LM_CW, INST_SINGLE, 0xA);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_CW, INST_SINGLE, 0xC);

    if (dnx_data_oam.oamp.oamp_sat_rxi_enable_support_get(unit))
    {

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_RXI_CORE_0_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_RXI_CORE_1_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_SAT_RXI_CORE_0_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_SAT_RXI_CORE_1_EN, INST_SINGLE, 1);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_LM_DW_0, INST_SINGLE, 0x00000034);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_LM_RSP_DW_0, INST_SINGLE, 0x08010034);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_LM_DW_1, INST_SINGLE, 0x00000000);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_LM_RSP_DW_1, INST_SINGLE, 0x00000000);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_DW_0, INST_SINGLE, 0x0000002C);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_RSP_DW_0, INST_SINGLE, 0x0801002C);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_DW_1, INST_SINGLE, 0x33300000);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RFC_6374_DM_RSP_DW_1, INST_SINGLE, 0x33300000);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCANNER_ENABLE, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_init_cpu_port_configuration(
    int unit)
{
    bcm_gport_t oamp_gport[2];
    uint32 entry_handle_id;
    int count;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    int fhei_size;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    sal_memset(oamp_gport, 0, sizeof(oamp_gport));
    SHR_IF_ERR_EXIT(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gport, &count));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, BCM_GPORT_LOCAL_GET(oamp_gport[0]), &flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_CPU_PORT_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_TC, INST_SINGLE, 7);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_DP, INST_SINGLE, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTCH_PP_SSP, INST_SINGLE, mapping_info.pp_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, INST_SINGLE,
                                 DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION);

    fhei_size =
        (system_headers_mode ==
         DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ? OAM_PUNT_FHEI_SIZE : OAM_PUNT_FHEI_JR1_SIZE;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PPH_FHEI_SIZE, INST_SINGLE, fhei_size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_init_rmep_cfg(
    int unit)
{
    uint32 entry_handle_id;
    int rmep_db_threshold;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    rmep_db_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    if ((rmep_db_threshold & 0x1) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "RMEP DB Threshold value should be aligned to full entry. (align 2)");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_RMEP_DB_FULL_ENTRY_THERSHOLD, INST_SINGLE,
                                 rmep_db_threshold);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_RMEP_ACCESS_ENABLE_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, 1);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_DEFINE_OAM_OPCODE_CCM);

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAMP_MEP_TYPE,
                                     DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAMP_MEP_TYPE,
                                     DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_init_opcode_configuration(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_init_mep_db_cfg(
    int unit)
{
    int mep_db_threshold;
    int mep_db_threshold_lim;
    int capacity;
    int max_num_of_entries_in_bank;
    int num_of_short_entry_banks;
    int total_num_of_mep_db_banks;
    int num_of_used_mep_db_banks;
    int num_clocks_scan;
    int ms_multiplier;
    int active_stages;
    int idle_stages;
    int short_entries_scan_valid;
    int i;
    uint32 entry_handle_id;
    uint32 device_frequency;
    uint64 u64_calc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    mep_db_threshold_lim = dnx_data_oam.oamp.oamp_mep_db_threshold_version_limitation_get(unit);

    max_num_of_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    short_entries_scan_valid = dnx_data_oam.oamp.scan_rate_bank_exist_get(unit);

    device_frequency = dnx_data_device.general.core_clock_khz_get(unit);

    if ((device_frequency <= 600000) & (mep_db_threshold != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP DB Threshold for device frequency less than 600MHz should be 0.\n\t"
                     "Short entries are not supported by devices with low frequency");
    }

    if (!mep_db_threshold_lim && (mep_db_threshold % max_num_of_entries_in_bank) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP DB Threshold value should be multiple of 2048*4");
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_RMEP_EM, &capacity));
#ifndef ADAPTER_SERVER_MODE
    if ((capacity < dnx_data_oam.general.oam_mdb_rmep_db_profile_limitation_get(unit)) && (mep_db_threshold != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP DB Threshold should be 0 for the current mdb_profile");
    }
#endif

    if (mep_db_threshold_lim && ((mep_db_threshold != 0) &&
                                 (mep_db_threshold != dnx_data_oam.oamp.max_nof_endpoint_id_get(unit))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP DB Threshold should be 0 or 64K.");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_MEP_DB_FULL_ENTRY_THERSHOLD, INST_SINGLE,
                                 (mep_db_threshold >> 2));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_MEP_DB_BANK_CFG, &entry_handle_id));

    total_num_of_mep_db_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
    num_of_used_mep_db_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get(unit);

    num_of_short_entry_banks = mep_db_threshold / max_num_of_entries_in_bank;

    for (i = 0; i < total_num_of_mep_db_banks; i++)
    {
        if (i < num_of_used_mep_db_banks)
        {
            active_stages = 1;
            idle_stages = 0;
            if (i < num_of_short_entry_banks)
            {
                ms_multiplier = 3330;
            }
            else
            {
                ms_multiplier = 1670;
            }
        }
        else
        {
            active_stages = 0;
            idle_stages = 1;
            ms_multiplier = 1670;
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_DB_BANK, i);

        if (short_entries_scan_valid)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCAN_RATE_BANK, INST_SINGLE,
                                         (i < num_of_short_entry_banks));
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_ACTIVE_STAGES, INST_SINGLE, active_stages);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_IDLE_STAGES, INST_SINGLE, idle_stages);

        COMPILER_64_ZERO(u64_calc);
        COMPILER_64_SET(u64_calc, 0, dnx_data_device.general.core_clock_khz_get(unit));
        COMPILER_64_UMUL_32(u64_calc, ms_multiplier);
        COMPILER_64_UDIV_32(u64_calc, 1000);
        num_clocks_scan = COMPILER_64_LO(u64_calc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SCAN, INST_SINGLE, num_clocks_scan);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_init_trap_map_cfg(
    int unit)
{
    uint32 entry_handle_id;
    uint8 ccm_piggyback_per_priority_with_crc_supported;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    ccm_piggyback_per_priority_with_crc_supported =
        dnx_data_oam.oamp.ccm_piggyback_per_priority_with_crc_supported_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_MAP_OAM_ID, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_ETH_OAM);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_MPLSTP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_PWE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (ccm_piggyback_per_priority_with_crc_supported)
    {
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_ETH_OAM);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_rx_trap_codes_set_verify(
    int unit,
    dbal_enum_value_field_oamp_mep_type_e mep_type,
    uint32 trap_code)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mep_type >= DBAL_NOF_ENUM_OAMP_MEP_TYPE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal MEP type.  Value provided is %d, maximum is %d",
                     mep_type, DBAL_NOF_ENUM_OAMP_MEP_TYPE_VALUES - 1);
    }

    if (trap_code >= DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal trap code.  Value provided is %d, maximum is %d",
                     trap_code, DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_rx_trap_codes_set(
    int unit,
    dbal_enum_value_field_oamp_mep_type_e mep_type,
    uint32 trap_code,
    uint32 index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set_verify(unit, mep_type, trap_code));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_TYPE_TRAP_CODE_TCAM, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_MEP_TYPE, INST_SINGLE, mep_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_SELECT, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_CHANNEL, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_RECEIVED_TRAP_CODE, INST_SINGLE, trap_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_MEP_TYPE_MASK, INST_SINGLE,
                                 DEFAULT_MEP_TYPE_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_SELECT_MASK, INST_SINGLE,
                                 DEFAULT_GACH_SEL_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_CHANNEL_MASK, INST_SINGLE,
                                 DEFAULT_GACH_CHAN_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_RECEIVED_TRAP_CODE_MASK, INST_SINGLE,
                                 DEFAULT_TRAP_CODE_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_VALID, INST_SINGLE, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_init_rx_trap_codes(
    int unit)
{
    dnx_oam_oamp_trap_tcam_entry_t tcam_data;
    int index = 0;
    uint8 dummy;
    SHR_FUNC_INIT_VARS(unit);

    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_ETH_OAM;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_ETH_OAM, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_ETH_OAM;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_ETH_OAM, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_MPLSTP;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_MPLSTP, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_MPLSTP;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_MPLSTP,
                                                   index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_PWE;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_PWE, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_PWE;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_PWE, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_MPLS;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_MPLS, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFDCC_O_MPLSTP;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFDCC_O_MPLSTP, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_PWE;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_PWE, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_PWE;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_PWE, index));
    tcam_data.trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_MPLSTP;
    tcam_data.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
    SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single(unit, 0, &tcam_data, NULL, &index, &dummy));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_PERFORMANCE_Y1731_O_MPLSTP,
                                                   index));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_init_interrupt(
    int unit)
{
    bcm_switch_event_control_t event_control;
    bcm_switch_event_t switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    uint32 enable = 1;
    uint8 dma_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    dma_enable = dnx_data_oam.oamp.oamp_fifo_dma_event_interface_enable_get(unit);

    if (dma_enable == 0)
    {

        event_control.event_id = _DNX_OAM_GET_OAMP_PENDING_EVENT_BY_DEVICE(unit);
        event_control.action = bcmSwitchEventForceUnmask;
        event_control.index = 0;

        SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, event_control, enable));

        event_control.action = bcmSwitchEventMask;

        SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, event_control, !enable));
    }

    dma_enable = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_enable_get(unit);

    if (dma_enable == 0)
    {

        event_control.event_id = _DNX_OAM_GET_OAMP_STAT_PENDING_EVENT_BY_DEVICE(unit);
        event_control.action = bcmSwitchEventForceUnmask;
        event_control.index = 0;

        SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, event_control, enable));

        event_control.action = bcmSwitchEventMask;

        SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, event_control, !enable));
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_oamp_copy_bit_array(
    uint8 *dst_buffer,
    uint32 dst_offset,
    const uint8 *src_buffer,
    uint32 src_offset,
    uint32 nof_bits_to_copy,
    uint8 read_from_hw)
{
    int index, bit_val, src_byte_offset, src_bit_offset, src_bit_in_byte_offset;
    int opposite_offset, dst_byte_offset, dst_bit_in_byte_offset;
    src_bit_offset = src_offset;
    for (index = 0; index < nof_bits_to_copy; index++, src_bit_offset++)
    {

        src_byte_offset = src_bit_offset >> UTILEX_BIT_BYTE_SHIFT;
        if (read_from_hw)
        {

            src_bit_in_byte_offset = UTILEX_GET_BITS_RANGE(src_bit_offset, UTILEX_BIT_BYTE_SHIFT - 1, 0);
        }
        else
        {

            src_bit_in_byte_offset = UTILEX_NOF_BITS_IN_BYTE - 1 -
                UTILEX_GET_BITS_RANGE(src_bit_offset, UTILEX_BIT_BYTE_SHIFT - 1, 0);
        }
        bit_val = UTILEX_GET_BIT(src_buffer[src_byte_offset], src_bit_in_byte_offset);

        opposite_offset = dst_offset + nof_bits_to_copy - 1 - index;
        dst_byte_offset = opposite_offset >> UTILEX_BIT_BYTE_SHIFT;
        if (read_from_hw)
        {

            dst_bit_in_byte_offset = UTILEX_NOF_BITS_IN_BYTE - 1 -
                UTILEX_GET_BITS_RANGE(opposite_offset, UTILEX_BIT_BYTE_SHIFT - 1, 0);
        }
        else
        {

            dst_bit_in_byte_offset = UTILEX_GET_BITS_RANGE(opposite_offset, UTILEX_BIT_BYTE_SHIFT - 1, 0);
        }

        UTILEX_SET_BIT(dst_buffer[dst_byte_offset], bit_val, dst_bit_in_byte_offset);
    }
}

shr_error_e
dnx_oam_oamp_init(
    int unit)
{
    int rv = _SHR_E_NONE;
    uint8 dma_event_interface_enable = 0;
    uint8 dma_statistic_interface_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_opcode_for_count_table_init(unit));

        SHR_IF_ERR_EXIT(dnx_oamp_v1_dbal_dynamic_memory_access_set(unit, TRUE));
    }
#endif

    SHR_IF_ERR_EXIT(dnx_oam_oamp_init_rx_trap_codes(unit));

    rv = dnx_oam_oamp_init_opcode_configuration(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_oamp_init_rmep_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        rv = dnx_oam_oamp_v1_init_y1731_cfg(unit);
        SHR_IF_ERR_EXIT(rv);
    }
#endif

    rv = dnx_oam_oamp_init_mep_db_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_oamp_init_trap_map_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_oamp_init_response_table(unit);
    SHR_IF_ERR_EXIT(rv);

    dma_event_interface_enable = dnx_data_oam.oamp.oamp_fifo_dma_event_interface_enable_get(unit);
    dma_statistic_interface_enable = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_enable_get(unit);

    if ((dma_statistic_interface_enable == 0) || (dma_event_interface_enable == 0))
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_init_interrupt(unit));
    }

    if (dma_event_interface_enable)
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_event_dma_init(unit));
    }

    if (dma_statistic_interface_enable)
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_dma_init(unit));
    }

    rv = dnx_oam_oamp_init_general_configuration(unit);
    SHR_IF_ERR_EXIT(rv);

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_pe_module_init(unit));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_init(unit));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_CONT(dnx_oam_oamp_event_dma_deinit(unit));

    SHR_IF_ERR_CONT(dnx_oam_oamp_stat_dma_deinit(unit));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX2_SUPPORT

static shr_error_e
oam_oamp_get_group_crc(
    int unit,
    bcm_oam_group_t group,
    uint16 *crc)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GROUP_INFO_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, group);

    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_RX_CRC, INST_SINGLE, crc);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dnx_oamp_mep_db_ccm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_ccm_endpoint_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    bcm_oam_group_info_t *group_info = NULL;
#ifdef BCM_DNX2_SUPPORT
    uint8 offloaded, q_entry;
    int system_headers_mode;
#endif
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX2_SUPPORT
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
#endif

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        offloaded = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED);
        q_entry = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY);

        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
        {
            if (q_entry != 1)
            {

                if (offloaded == 1)
                {

                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
                }

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
            }
            else
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_Q_ENTRY_ACCESS, oamp_params));
            }
        }
        else
        {
            oamp_params->exclusive.oam_only.oam_lmm_da_profile = 0;
            oamp_params->flex_lm_dm_ptr = 0;

            oamp_params->flags = 0;
        }

        oamp_params->mep_type = entry_values->mep_type;

        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE),
                       OAMP_MEP_UPDATE);
        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED),
                       OAMP_MEP_LM_DM_OFFLOADED);
        if (!offloaded || !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
        {

            UTILEX_SET_BIT(oamp_params->flags,
                           _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_OFFLOADED_LM_ENABLE),
                           OAMP_MEP_LM_ENABLE);
            UTILEX_SET_BIT(oamp_params->flags,
                           _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_OFFLOADED_LM_ENABLE),
                           OAMP_MEP_LM_STAT_ENABLE);
            UTILEX_SET_BIT(oamp_params->flags,
                           _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_OFFLOADED_DM_ENABLE),
                           OAMP_MEP_DM_ENABLE);
            UTILEX_SET_BIT(oamp_params->flags,
                           _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_OFFLOADED_DM_ENABLE),
                           OAMP_MEP_DM_STAT_ENABLE);
        }
        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY),
                       OAMP_MEP_Q_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP),
                       OAMP_MEP_IS_UPMEP);
        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN),
                       OAMP_MEP_PORT_TLV_EN);
        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL),
                       OAMP_MEP_PORT_TLV_VAL);
        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET),
                       OAMP_MEP_RDI_FROM_PACKET);
        UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_RDI_FROM_SCANNER),
                       OAMP_MEP_RDI_FROM_SCANNER);

        if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {

            UTILEX_SET_BIT(oamp_params->flags, DBAL_ENUM_FVAL_DA_UC_MC_TYPE_MC, OAMP_MEP_MC_UC_SEL);
        }

        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC))
        {
            oamp_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
            UTILEX_SET_BIT(oamp_params->fec_id_or_glob_out_lif, TRUE, FIELD_IS_FEC_ID_FLAG);
        }
        else
        {
            oamp_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
        }

        oamp_params->mpls_pwe.label = entry_values->label;

        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION))
        {
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_EGRESS_INJECTION);
            oamp_params->exclusive.oam_only.vsi = entry_values->vsi;

            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                oamp_params->counter_ptr = entry_values->counter_ptr;
                oamp_params->counter_interface = entry_values->counter_interface;
            }
        }
        else
        {
            oamp_params->counter_ptr = entry_values->counter_ptr;
            oamp_params->counter_interface = entry_values->counter_interface;
        }
        oamp_params->exclusive.oam_only.oam_mep_id = entry_values->mep_id;
        oamp_params->dest_sys_port_agr = entry_values->dest_sys_port_agr;
        oamp_params->extra_data_ptr = entry_values->extra_data_header;
        oamp_params->exclusive.oam_only.oam_maid = entry_values->maid;
        oamp_params->exclusive.oam_only.oam_inner_vid = entry_values->inner_vid;
        oamp_params->exclusive.oam_only.oam_outer_vid = entry_values->outer_vid;
        oamp_params->exclusive.oam_only.oam_ccm_interval = entry_values->ccm_interval;
        oamp_params->exclusive.oam_only.oam_sa_mac_msb_profile = entry_values->sa_gen_msb_profile;
        oamp_params->exclusive.oam_only.oam_sa_mac_lsb = entry_values->sa_gen_lsb;

        oamp_params->exclusive.oam_only.oam_icc_index = entry_values->icc_index;
        oamp_params->exclusive.oam_only.oam_mdl = entry_values->mdl;
        oamp_params->exclusive.oam_only.oam_nof_vlan_tags = entry_values->nof_vlan_tags;
        oamp_params->exclusive.oam_only.oam_inner_tpid_index = entry_values->inner_tpid_index;
        oamp_params->exclusive.oam_only.oam_inner_dei_pcp = entry_values->inner_pcp_dei;
        oamp_params->exclusive.oam_only.oam_outer_tpid_index = entry_values->outer_tpid_index;
        oamp_params->exclusive.oam_only.oam_outer_dei_pcp = entry_values->outer_pcp_dei;
        oamp_params->exclusive.oam_only.oam_interface_status_tlv_code = entry_values->interface_status_tlv_code;

        oamp_params->exclusive.oam_only.oam_prt_qualifier_profile = 0;

        oamp_params->itmh_tc_dp_profile = entry_values->itmh_tc_dp_profile;
        oamp_params->mep_profile = entry_values->mep_profile;
        oamp_params->mep_pe_profile = entry_values->mep_pe_profile;
        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
        {
            oamp_params->unified_port_access.ccm_eth_up_mep_port.pp_port =
                entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port;
            oamp_params->unified_port_access.ccm_eth_up_mep_port.port_core =
                entry_values->unified_port_access.ccm_eth_up_mep_port.port_core;
        }
        else
        {
            oamp_params->unified_port_access.port_profile = entry_values->unified_port_access.port_profile;
        }
        oamp_params->mpls_pwe.push_profile = entry_values->push_profile;
        oamp_params->crps_core_select = entry_values->crps_core_select;
        oamp_params->exclusive.oam_only.server_destination = entry_values->server_destination;
        oamp_params->exclusive.oam_only.tx_signal = entry_values->signal_degradation_tx;

        if ((offloaded == 1) || (q_entry == 1))
        {

            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));
        }

        if (offloaded == 1)
        {

            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));
            UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_Q_ENTRY);
        }

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));

        if (((offloaded == 1) || (q_entry == 1))
            && (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_48B_MAID))
            && !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
        {
            uint16 calculated_crc = 0;
            SHR_ALLOC_SET_ZERO(group_info, sizeof(bcm_oam_group_info_t), "Group info struct", "%s%s%s\r\n", EMPTY,
                               EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(dnx_oam_group_info_get(unit, entry_values->extra_data_header, group_info));

            SHR_IF_ERR_EXIT(oam_oamp_get_group_crc(unit, entry_values->extra_data_header, &calculated_crc));

            SHR_IF_ERR_EXIT(dnx_oamp_v1_mdb_extra_data_set
                            (unit, oam_id, offloaded, q_entry, calculated_crc, DBAL_DEFINE_OAM_OPCODE_CCM,
                             DNX_OAM_OAMP_NOF_MAID_BITS, group_info->name, FALSE));
        }
        else if (entry_values->extra_data.index != DNX_OAM_EXTRA_DATA_HEADER_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_v1_ccm_endpoint_extra_data_set(unit, &entry_values->extra_data));
        }
    }
#endif

exit:
    SHR_FREE(group_info);
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_mep_db_ccm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint32 extra_data_length)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
#ifdef BCM_DNX2_SUPPORT
    int access_index;
    uint32 nof_extra_data_access;
#endif
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t),
              "Endpoint data read to find MDB entries before deleting", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY) == 1)
        {

            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_delete(unit, oam_id, MDB_Q_ENTRY_ACCESS));
        }
        else
        {
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) == 1)
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_delete(unit, oam_id, MDB_PART_1_ACCESS));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_delete(unit, oam_id, MDB_PART_2_ACCESS));
            }
        }

        if (extra_data_length != 0)
        {
            nof_extra_data_access = 1;

            if ((extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) > 63)
            {
                nof_extra_data_access =
                    (extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) / (DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN +
                                                                                1) + 2;
            }

            for (access_index = 0; access_index < nof_extra_data_access; access_index++)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_delete(unit, oam_id, access_index + MDB_EXTRA_DATA_HDR_ACCESS));
            }
        }

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, oam_id));
    }
#endif

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_local_port_to_system_port_set(
    int unit,
    uint16 pp_port_profile,
    uint32 system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, &entry_handle_id));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, pp_port_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, system_port);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_local_port_to_system_port_get(
    int unit,
    uint16 pp_port_profile,
    uint32 *system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, &entry_handle_id));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, pp_port_profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, system_port);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_itmh_priority_profile_set(
    int unit,
    uint8 profile,
    uint8 tc,
    uint8 dp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, INST_SINGLE, tc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, INST_SINGLE, dp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_itmh_priority_profile_get(
    int unit,
    uint8 profile,
    uint8 *tc,
    uint8 *dp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, profile);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TC, INST_SINGLE, tc);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DP, INST_SINGLE, dp);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_sa_mac_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t sa_mac_addr_msb)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_SA_MAC_MSB_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, profile);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAC_MSB, INST_SINGLE, sa_mac_addr_msb);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_sa_mac_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t sa_mac_addr_msb)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_SA_MAC_MSB_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, profile);

    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_MAC_MSB, INST_SINGLE, sa_mac_addr_msb);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_profile_hw_set(
    int unit,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_itmh_priority_profile_set(unit,
                                                               mep_hw_profiles_write_data->itmh_profile,
                                                               mep_hw_profiles_write_data->itmh_priority.tc,
                                                               mep_hw_profiles_write_data->itmh_priority.dp));
    }

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {

        if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_tpid_profile_set(unit,
                                                             mep_hw_profiles_write_data->outer_tpid_index,
                                                             mep_hw_profiles_write_data->outer_tpid));
        }

        if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_tpid_profile_set(unit,
                                                             mep_hw_profiles_write_data->inner_tpid_index,
                                                             mep_hw_profiles_write_data->inner_tpid));
        }
    }
#endif

    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_sa_mac_msb_profile_set(unit,
                                                            mep_hw_profiles_write_data->sa_mac_msb_profile,
                                                            mep_hw_profiles_write_data->sa_mac_msb));
    }

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {

        if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_profile_set(unit,
                                                            mep_hw_profiles_write_data->mep_profile_index,
                                                            ((mep_hw_profiles_write_data->flags &
                                                              DNX_OAM_OAMP_PROFILE_MPLS_LM_DM_SET) ==
                                                             DNX_OAM_OAMP_PROFILE_MPLS_LM_DM_SET),
                                                            &mep_hw_profiles_write_data->mep_profile));
        }

        if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_set(unit,
                                                                         mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile,
                                                                         mep_hw_profiles_write_data->ttl_exp.ttl,
                                                                         mep_hw_profiles_write_data->ttl_exp.exp));
        }

        if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_local_port_to_system_port_set(unit,
                                                                       mep_hw_profiles_write_data->pp_port_profile,
                                                                       mep_hw_profiles_write_data->system_port));
        }
    }
#endif

    if (_SHR_IS_FLAG_SET(mep_hw_profiles_write_data->flags, DNX_OAM_OAMP_PROFILE_TRAP_TCAM_FLAG))
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit,
                                                       mep_hw_profiles_write_data->tcam_entry.mep_type,
                                                       mep_hw_profiles_write_data->tcam_entry.trap_code,
                                                       mep_hw_profiles_write_data->tcam_index));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(
    int unit,
    const bcm_oam_endpoint_type_t bcm_mep_type,
    dbal_enum_value_field_oamp_mep_type_e * mep_db_mep_type)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_mep_type)
    {
        case bcmOAMEndpointTypeEthernet:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM;
            break;
        case bcmOAMEndpointTypeBHHMPLS:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;
            break;
        case bcmOAMEndpointTypeBHHPwe:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;
            break;
        case bcmOAMEndpointTypeBHHPweGAL:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;
            break;
        case bcmOAMEndpointTypeBhhSection:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;
            break;
        case bcmOAMEndpointTypeMplsLmDmPw:
        case bcmOAMEndpointTypeMplsLmDmLsp:
        case bcmOAMEndpointTypeMplsLmDmSection:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: MEP type %d is not supported.\n", bcm_mep_type);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_update_accelarated_endpoint_info_with_group_config(
    int unit,
    const uint8 *group_name,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry)
{
    int i;
    int icc_index;
    dnx_oam_group_icc_profile_data_t icc_profile_data;
    dnx_oam_ma_name_type_e group_name_type = BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_name);

    SHR_FUNC_INIT_VARS(unit);

    if (group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_SHORT)
    {
        mep_db_entry->icc_index = DBAL_DEFINE_SHORT_ICC_INDEX_SHORT_FORMAT;
        mep_db_entry->maid =
            group_name[DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET + 1] + (group_name[DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET] << 8);
    }
    else if (group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_ICC)
    {
        for (i = 0; i < DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH; i++)
        {
            icc_profile_data[i] =
                group_name[DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET + (DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH - 1) - i];
        }

        SHR_IF_ERR_EXIT(algo_oam_db.oam_group_icc_profile.profile_get(unit, icc_profile_data, &icc_index));

        mep_db_entry->icc_index = (uint8) icc_index;

        mep_db_entry->maid =
            group_name[DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_OFFSET + 1] +
            (group_name[DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_OFFSET] << 8);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error:   Error in group name");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_ccm_endpoint_eth_oam_get(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    bcm_mac_t sa_mac_addr_msb;

    SHR_FUNC_INIT_VARS(unit);

    endpoint_info->outer_tpid = 0;
    endpoint_info->vlan = 0;
    endpoint_info->pkt_pri = 0;
    endpoint_info->inner_tpid = 0;
    endpoint_info->inner_vlan = 0;
    endpoint_info->inner_pkt_pri = 0;

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        if (mep_db_entry->nof_vlan_tags == 1)
        {
            endpoint_info->vlan = mep_db_entry->inner_vid;
            endpoint_info->pkt_pri = mep_db_entry->inner_pcp_dei;
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_tpid_profile_get(unit,
                                                             mep_db_entry->inner_tpid_index,
                                                             &endpoint_info->outer_tpid));
        }

        if (mep_db_entry->nof_vlan_tags == 2)
        {
            endpoint_info->inner_vlan = mep_db_entry->inner_vid;
            endpoint_info->inner_pkt_pri = mep_db_entry->inner_pcp_dei;
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_tpid_profile_get(unit,
                                                             mep_db_entry->inner_tpid_index,
                                                             &endpoint_info->inner_tpid));

            endpoint_info->vlan = mep_db_entry->outer_vid;
            endpoint_info->pkt_pri = mep_db_entry->outer_pcp_dei;
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_tpid_profile_get(unit,
                                                             mep_db_entry->outer_tpid_index,
                                                             &endpoint_info->outer_tpid));
        }
    }
#endif

    SHR_IF_ERR_EXIT(dnx_oam_oamp_sa_mac_msb_profile_get(unit, mep_db_entry->sa_gen_msb_profile, sa_mac_addr_msb));
    sal_memcpy(endpoint_info->src_mac_address, sa_mac_addr_msb, DNX_OAM_MAC_SIZE);

    endpoint_info->src_mac_address[5] = (uint8) mep_db_entry->sa_gen_lsb;
    endpoint_info->src_mac_address[4] = (endpoint_info->src_mac_address[4] & ~DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK)
        | (uint8) ((mep_db_entry->sa_gen_lsb >> SAL_UINT8_NOF_BITS) & DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_ccm_endpoint_eth_oam_init(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    bcm_mac_t sa_mac;
    int tpid_index;
    int profile_id;
    uint16 tpid;
    uint8 first_reference;

    SHR_FUNC_INIT_VARS(unit);

    mep_db_entry->flags |= DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ? DNX_OAMP_OAM_CCM_MEP_IS_UPMEP : 0;

    if (endpoint_info->outer_tpid == 0)
    {
        if (endpoint_info->inner_tpid != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Endpoint create error: Inner tpid can not be configured without an outer tpid.\n");
        }
        mep_db_entry->nof_vlan_tags = 0;
    }
    else
    {
        tpid = endpoint_info->outer_tpid;

        SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get(unit, endpoint_info->outer_tpid, &tpid_index));
        if (tpid_index == BCM_DNX_SWITCH_TPID_INDEX_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: TPID %d not configured.\n", endpoint_info->inner_tpid);
        }

        SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.allocate_single
                        (unit, 0, &tpid, NULL, &profile_id, &first_reference));

        mep_db_entry->outer_tpid_index = (uint8) profile_id;
        mep_db_entry->outer_vid = endpoint_info->vlan;
        mep_db_entry->outer_pcp_dei = endpoint_info->pkt_pri;
        if (first_reference)
        {

            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG;

            mep_hw_profiles_write_data->outer_tpid_index = profile_id;
            mep_hw_profiles_write_data->outer_tpid = endpoint_info->outer_tpid;
        }

        if (endpoint_info->inner_tpid != 0)
        {
            tpid = endpoint_info->inner_tpid;

            SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get(unit, endpoint_info->inner_tpid, &tpid_index));
            if (tpid_index == BCM_DNX_SWITCH_TPID_INDEX_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: TPID %d not configured.\n",
                             endpoint_info->inner_tpid);
            }

            SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.allocate_single
                            (unit, 0, &tpid, NULL, &profile_id, &first_reference));

            mep_db_entry->inner_tpid_index = (uint8) profile_id;
            mep_db_entry->inner_vid = endpoint_info->inner_vlan;
            mep_db_entry->inner_pcp_dei = endpoint_info->inner_pkt_pri;
            if (first_reference)
            {

                mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG;

                mep_hw_profiles_write_data->inner_tpid_index = profile_id;
                mep_hw_profiles_write_data->inner_tpid = endpoint_info->inner_tpid;
            }

            mep_db_entry->nof_vlan_tags = 2;
        }
        else
        {
            mep_db_entry->nof_vlan_tags = 1;

            mep_db_entry->inner_tpid_index = mep_db_entry->outer_tpid_index;
            mep_db_entry->inner_vid = mep_db_entry->outer_vid;
            mep_db_entry->inner_pcp_dei = mep_db_entry->outer_pcp_dei;

            mep_db_entry->outer_tpid_index = 0;
            mep_db_entry->outer_vid = 0;
            mep_db_entry->outer_pcp_dei = 0;

        }
    }

    mep_db_entry->sa_gen_lsb = endpoint_info->src_mac_address[5] |
        ((endpoint_info->src_mac_address[4] & DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK) << SAL_UINT8_NOF_BITS);

    sal_memcpy(sa_mac, endpoint_info->src_mac_address, DNX_OAM_MAC_SIZE);

    sa_mac[5] = 0;
    sa_mac[4] &= ~DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK;

    SHR_IF_ERR_EXIT(algo_oam_db.oam_sa_mac_msb_profile_template.allocate_single
                    (unit, 0, sa_mac, NULL, &profile_id, &first_reference));

    if (first_reference)
    {

        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG;

        mep_hw_profiles_write_data->sa_mac_msb_profile = profile_id;
        sal_memcpy(mep_hw_profiles_write_data->sa_mac_msb, sa_mac, DNX_OAM_MAC_SIZE);
    }
    mep_db_entry->sa_gen_msb_profile = profile_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_local_accelerated_endpoint_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 *group_name,
    int is_48B_maid)
{
    dnx_oam_ma_name_type_e group_name_type = group_name ? BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_name)
        : DNX_OAMP_OAM_MA_NAME_TYPE_NOF;
    uint32 flags;
    dnx_oamp_mep_db_memory_type_t memory_type;
    SHR_FUNC_INIT_VARS(unit);

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)
    {
        int nof_umc_entries = dnx_data_oam.oamp.nof_umc_get(unit);
        int nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        int max_allowed_icc_id = nof_umc_entries * nof_mep_db_short_entries;

        if (!is_48B_maid && group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_ICC && endpoint_info->id >= max_allowed_icc_id)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal MEP ID index - "
                         "maximum ID of ICC group based MEP is %u ", max_allowed_icc_id);
        }

        flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {

        flags = 0;

        memory_type.is_extra_pool = FALSE;

        memory_type.is_full = (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeShortEntry);

        memory_type.is_2byte_maid = (group_name_type != DNX_OAMP_OAM_MA_NAME_TYPE_ICC);
    }

    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, flags, &memory_type, (uint32 *) (&endpoint_info->id)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_system_port_profile_sw_update(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_oamp_profile_hw_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    dnx_algo_gpm_gport_phy_info_t *tx_gport_data = NULL;
    int system_port_profile_index;
    uint8 write_hw;
    uint16 system_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(tx_gport_data, sizeof(dnx_algo_gpm_gport_phy_info_t),
                       "Structure for reading data about tx_gport", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, endpoint_info->tx_gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, tx_gport_data));

    system_port = tx_gport_data->sys_port;
    rv = algo_oamp_db.oamp_pp_port_2_sys_port.allocate_single
        (unit, 0, (uint32 *) &system_port, NULL, &system_port_profile_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {

        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free system port profile entries available.\n");
    }
    else
    {

        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->pp_port_profile = system_port_profile_index;

    if (write_hw)
    {

        mep_hw_write_data->system_port = tx_gport_data->sys_port;
        mep_hw_write_data->flags |= DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG;
    }

exit:
    SHR_FREE(tx_gport_data);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_local_acc_endpoint_get(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{

    dnx_oam_oamp_ccm_endpoint_t *mep_db_entry = NULL;
    dnx_oam_itmh_priority_t itmh_priority;
    int counter;
    uint32 dest_val;
    dbal_fields_e dbal_dest_type;
    int is_max_ccm_cnt = 0;
    int system_headers_mode;
#ifdef BCM_DNX2_SUPPORT
    dnx_oam_mep_profile_t mep_profile;
    uint32 system_port;
    uint8 rdi_from_rx = 0, rdi_from_scannner = 0;
#endif
    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_ALLOC(mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
              "Endpoint data read ", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {

        mep_db_entry->mep_type = 0;
    }
    if (dnx_data_oam.oamp.feature_get(unit, dnx_data_oam_oamp_oamp_v2) == 0)
    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_ccm_endpoint_get(unit, (uint16) endpoint_info->id,
                                                                DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION
                                                                (endpoint_info), mep_db_entry));
#endif
    }
    endpoint_info->endpoint_memory_type = ((_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED))
                                           ? bcmOamEndpointMemoryTypeLmDmOffloadedEntry
                                           : (((_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY)) ?
                                               bcmOamEndpointMemoryTypeShortEntry :
                                               bcmOamEndpointMemoryTypeSelfContained)));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        endpoint_info->session_id = mep_db_entry->session_id;
        endpoint_info->timestamp_format = mep_db_entry->timestamp_format;
        endpoint_info->extra_data_index = MEP_DB_ENTRY_TO_OAM_ID(mep_db_entry->flex_lm_dm_entry);
    }

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.profile_data_get
                        (unit, mep_db_entry->itmh_tc_dp_profile, &counter, &itmh_priority));
        endpoint_info->int_pri = (itmh_priority.tc << 2) | itmh_priority.dp;

        if (mep_db_entry->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {
            if (_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC))
            {
                BCM_L3_ITF_SET(endpoint_info->intf_id, BCM_L3_ITF_TYPE_FEC,
                               mep_db_entry->fec_id_or_glob_out_lif.fec_id);
                endpoint_info->tx_gport = BCM_GPORT_INVALID;
            }
            else
            {
                BCM_L3_ITF_SET(endpoint_info->intf_id, BCM_L3_ITF_TYPE_LIF,
                               mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif);
                BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, mep_db_entry->dest_sys_port_agr);
            }
        }
#ifdef BCM_DNX2_SUPPORT
        else if (DNX_OAMP_IS_V1(unit))
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_local_port_to_system_port_get(unit,
                                                                       mep_db_entry->unified_port_access.port_profile,
                                                                       &system_port));
            BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, system_port);

            if (DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info))
            {
                endpoint_info->vpn = mep_db_entry->vsi;
            }
        }
#endif
    }
    else
    {
        endpoint_info->tx_gport = BCM_GPORT_INVALID;
        if (mep_db_entry->server_destination != 0)
        {
            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_DESTINATION_JR1,
                                                                      mep_db_entry->server_destination, &dbal_dest_type,
                                                                      &dest_val));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_DESTINATION,
                                                                      mep_db_entry->server_destination, &dbal_dest_type,
                                                                      &dest_val));
            }
            if (dbal_dest_type == DBAL_FIELD_PORT_ID)
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, dest_val);
            }
        }
    }

#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        counter = 0;
        sal_memset(&mep_profile, 0, sizeof(dnx_oam_mep_profile_t));
        SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get
                        (unit, mep_db_entry->mep_profile, &counter, &mep_profile));
        is_max_ccm_cnt = (mep_profile.ccm_count == DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT) ? 1 : 0;
    }
    else
#endif
    {
    }

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
#ifdef BCM_DNX2_SUPPORT
        if (DNX_OAMP_IS_V1(unit))
        {
            DNX_OAM_GET_RDI_GEN_METHOD_FIELD_FROM_MEP_PROFILE(mep_profile.rdi_gen_method, rdi_from_rx,
                                                              rdi_from_scannner);

            if (!(rdi_from_rx && rdi_from_scannner))
            {
                endpoint_info->flags2 |= rdi_from_rx ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;
                endpoint_info->flags2 |= rdi_from_scannner ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE;
            }

            if (mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN)
            {
                endpoint_info->port_state =
                    (mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL) ? BCM_OAM_PORT_TLV_UP :
                    BCM_OAM_PORT_TLV_BLOCKED;
            }
            else
            {
                endpoint_info->port_state = 0;
            }

            endpoint_info->interface_state = mep_db_entry->interface_status_tlv_code;
        }
#endif

        endpoint_info->name = mep_db_entry->mep_id;

        if ((mep_db_entry->ccm_interval > 0) && is_max_ccm_cnt)
        {

            endpoint_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;
        }
        else
        {

            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period
                            (unit, mep_db_entry->ccm_interval, &endpoint_info->ccm_period));
        }
    }

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_eth_oam_get(unit, mep_db_entry, endpoint_info));
    }
    else
#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_mpls_oam_get(unit, mep_db_entry, endpoint_info));
        }
    }
#endif
exit:
    SHR_FREE(mep_db_entry);
    SHR_FUNC_EXIT;
}
