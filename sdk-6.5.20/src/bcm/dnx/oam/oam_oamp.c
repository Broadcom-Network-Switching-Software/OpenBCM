
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
#include <src/bcm/dnx/oam/oamp_pe.h>
#include <src/bcm/dnx/oam/oam_oamp_crc.h>
#include <src/bcm/dnx/oam/oamp_pe_infra.h>
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
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>
#include <include/bcm/switch.h>
#include <include/bcm_int/dnx_dispatch.h>
#include <include/soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/mdb.h>





#define OAM_PUNT_FHEI_SIZE 2
#define OAM_PUNT_FHEI_JR1_SIZE 1

#define MPLS_LABEL_GAL_TTL 64
#define MPLS_LABEL_GAL_EXP 0
#define MPLS_LABEL_GAL_BOS 1

#define DNX_OAM_ETH_SA_MAC_LSB_SIZE_IN_BITS     (15)
#define DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK UTILEX_BITS_MASK(DNX_OAM_ETH_SA_MAC_LSB_SIZE_IN_BITS - SAL_UINT8_NOF_BITS - 1,0)


#define IS_LM_DM_TYPE(x) ((x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB) ||\
                          (x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT) ||\
                          (x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT) ||\
                          (x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374) ||\
                          (x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY))


#define DEFAULT_MEP_TYPE_MASK 0
#define DEFAULT_GACH_SEL_MASK SAL_UPTO_BIT(1)
#define DEFAULT_GACH_CHAN_MASK SAL_UPTO_BIT(16)
#define DEFAULT_TRAP_CODE_MASK 0


#define NOF_CLS_TRAP_TABLE_ENTRIES SAL_UPTO_BIT(5)


#define NOF_NUM_CLOCKS_SCAN_BITS 22
#define NUM_CLOCKS_SCAN_MASK SAL_UPTO_BIT(NOF_NUM_CLOCKS_SCAN_BITS)


#define DNX_OAM_SET_RDI_GEN_METHOD_FIELD_ON_MEP_PROFILE(rdi_method, rx_disable, scanner_disable) \
               rdi_method = ( (rx_disable)==0) <<1 | ((scanner_disable)==0)


#define DNX_OAM_IS_CCM_PHASE_ODD(ccm_transmit_phase_is_odd, is_short_entry_type, endpoint_id) \
    ccm_transmit_phase_is_odd = is_short_entry_type ? 0 : (endpoint_id >> 2) & 1


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


#define DNX_OAM_OAMP_CRC_BIT 64
#define DNX_OAM_OAMP_CRC_BYTE 120


typedef struct
{
    
    uint32 first_entry;

    
    uint32 param_entry;

     
    uint32 resulting_entry;

     
    uint8 update_entry;

     
    uint8 clear_last_entry;

     
    dbal_enum_value_field_oamp_mep_type_e search_type;

} dnx_oam_oamp_lm_dm_search_t;


typedef struct
{

    
    
    uint32 oamp_mep_pe_profile;
    uint32 oamp_mep_pe_profile_mask;

    
    uint32 opcode_or_gach_channel;
    uint32 opcode_or_gach_channel_mask;

    
    uint32 opcode_or_gach_select;
    uint32 opcode_or_gach_select_mask;

   
    
    uint8 crc;

    
    uint8 crc_mask_index;

    
    uint8 valid;

} dnx_oam_oamp_flex_crc_tcam_t;


typedef struct
{

    
    uint8 flex_crc_per_bit_mask[BITS2BYTES(DNX_OAM_OAMP_CRC_BIT)];

    
    uint8 flex_crc_per_byte_mask[BITS2BYTES(DNX_OAM_OAMP_CRC_BYTE)];

} dnx_oam_oamp_flex_crc_pdu_mask_t;












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
        dnx_data_oam.oamp.rmep_max_self_contained_get(unit) * dnx_data_oam.oamp.rmep_sys_clocks_get(unit);
    
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
        dnx_data_oam.oamp.rmep_max_self_contained_get(unit) * dnx_data_oam.oamp.rmep_sys_clocks_get(unit);
     
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


static shr_error_e
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


shr_error_e
dnx_oam_oamp_rmep_index_db_add(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 rmep_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, rmep_idx);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_rmep_index_db_get(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 *rmep_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, rmep_idx);

    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_rmep_index_db_remove(
    int unit,
    uint16 oam_id,
    uint16 mep_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
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

    
    COMPILER_64_SET(rmep_db_scan_interval_ns, 0, dnx_data_oam.oamp.rmep_max_self_contained_get(unit));
    COMPILER_64_UMUL_32(rmep_db_scan_interval_ns, dnx_data_oam.oamp.rmep_sys_clocks_get(unit));
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

    
    COMPILER_64_SET(rmep_db_scan_interval_ns, 0, dnx_data_oam.oamp.rmep_max_self_contained_get(unit));
    COMPILER_64_UMUL_32(rmep_db_scan_interval_ns, dnx_data_oam.oamp.rmep_sys_clocks_get(unit));
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
dnx_oam_oamp_init_y1731_cfg(
    int unit)
{
    uint32 entry_handle_id;
    uint32 bos;
    uint32 mpls_label;
    uint32 exp;
    uint32 ttl;
    uint32 mpls = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    
    mpls_label = MPLS_LABEL_GAL;
    ttl = MPLS_LABEL_GAL_TTL;
    exp = MPLS_LABEL_GAL_EXP;
    bos = MPLS_LABEL_GAL_BOS;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_BOS, &bos, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_MPLS_LABEL, &mpls_label, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_EXP, &exp, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_TTL, &ttl, &mpls));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Y1731_MPLSTP_GAL, INST_SINGLE, mpls);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 0, BFD_PWE_DEFAULT_CHANNEL));
    SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 1, BFD_MPLS_TP_CC_GACH_DEFAULT_CHANNEL));

    
    SHR_IF_ERR_EXIT(dnx_oam_y1731_mpls_tp_channel_value_set(unit, Y1731_MPLS_TP_GACH_DEFAULT_CHANNEL));

    
    SHR_IF_ERR_EXIT(dnx_oam_y1731_pwe_channel_value_set(unit, Y1731_PWE_GACH_DEFAULT_CHANNEL));

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
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
        num_clocks_scan &= NUM_CLOCKS_SCAN_MASK;
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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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


static void
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


static shr_error_e
dnx_oam_oamp_flexible_crc_tcam_set(
    int unit,
    uint8 tcam_index,
    const dnx_oam_oamp_flex_crc_tcam_t * flex_tcam_entry)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_CLS_FLEX_CRC_TCAM, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, tcam_index));

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_GACH_CHANNEL,
                                      flex_tcam_entry->opcode_or_gach_channel,
                                      flex_tcam_entry->opcode_or_gach_channel_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_GACH_SELECT,
                                      flex_tcam_entry->opcode_or_gach_select,
                                      flex_tcam_entry->opcode_or_gach_select_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE,
                                      flex_tcam_entry->oamp_mep_pe_profile, flex_tcam_entry->oamp_mep_pe_profile_mask);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_MASK_INDEX, INST_SINGLE,
                                 flex_tcam_entry->crc_mask_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_INDEX, INST_SINGLE, flex_tcam_entry->crc);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, flex_tcam_entry->valid);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_flexible_crc_mask_set(
    int unit,
    uint8 crc_index,
    dnx_oam_oamp_flex_crc_pdu_mask_t * flex_crc_mask)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_FLEX_CRC_MASK, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_MASK_INDEX, crc_index);

    
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PER_BIT_MASK, INST_SINGLE,
                                    flex_crc_mask->flex_crc_per_bit_mask);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PER_BYTE_MASK, INST_SINGLE,
                                    flex_crc_mask->flex_crc_per_byte_mask);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_flexible_crc_init(
    int unit)
{
    uint32 mep_pe_profile = 0;
    uint8 tcam_index = 1;
    uint8 *mask = NULL;
    dnx_oam_oamp_flex_crc_tcam_t *flex_tcam_entry = NULL;
    dnx_oam_oamp_flex_crc_pdu_mask_t *flex_crc_mask = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(flex_tcam_entry, sizeof(dnx_oam_oamp_flex_crc_tcam_t), "Param struct for flexible crc tcam",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(flex_crc_mask, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t), "Param struct for flexible crc mask",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(mask, BITS2BYTES(DNX_OAM_OAMP_CRC_BYTE), "Size of CRC per Bytes field",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(mask, 0xFF, BITS2BYTES(DNX_OAM_OAMP_CRC_BYTE));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get(unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &mep_pe_profile));

    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index = 0;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));
    
    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 70, mask, 0, BCM_OAM_GROUP_NAME_LENGTH, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48, &mep_pe_profile));

    tcam_index++;
    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));
    
    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 70, mask, 0, BCM_OAM_GROUP_NAME_LENGTH, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6, &mep_pe_profile));

    tcam_index++;
    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    flex_tcam_entry->opcode_or_gach_channel = 0;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0x1ffff;
    flex_tcam_entry->opcode_or_gach_select = 0;
    flex_tcam_entry->opcode_or_gach_select_mask = 1;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));

    
    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 88, mask, 0, 16, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48, &mep_pe_profile));

    tcam_index++;
    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));
    
    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 70, mask, 0, BCM_OAM_GROUP_NAME_LENGTH, FALSE);
    
    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_bit_mask, 46, mask, 0, 1, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT, &mep_pe_profile));

    tcam_index++;
    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));
    
    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_bit_mask, 46, mask, 0, 1, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

exit:
    SHR_FREE(mask);
    SHR_FREE(flex_tcam_entry);
    SHR_FREE(flex_crc_mask);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_dbal_dynamic_memory_access_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_dbal_dynamic_memory_access_get(
    int unit,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_init(
    int unit)
{
    int rv = _SHR_E_NONE;
    uint8 dma_event_interface_enable = 0;
    uint8 dma_statistic_interface_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oamp_dbal_dynamic_memory_access_set(unit, TRUE));

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_init_rx_trap_codes(unit));

    
    rv = dnx_oam_oamp_init_opcode_configuration(unit);
    SHR_IF_ERR_EXIT(rv);

    
    rv = dnx_oam_oamp_init_rmep_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    
    rv = dnx_oam_oamp_init_y1731_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    
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
        
        if (!soc_is(unit, J2P_DEVICE))
        {
            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_init_interrupt(unit));
        }
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

    SHR_IF_ERR_EXIT(dnx_oamp_pe_module_init(unit));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_flexible_crc_init(unit));

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

shr_error_e
dnx_oamp_mdb_extra_data_set(
    int unit,
    uint16 oam_id,
    uint8 is_offloaded,
    uint8 is_q_entry,
    uint16 calculated_crc,
    int opcode,
    uint32 nof_data_bits,
    uint8 *data,
    uint8 is_update)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    int pl_index;
    int extra_data_len = 1;
    int extra_data_hdr_seg_len;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
    UTILEX_SET_BIT(oamp_params->flags, is_offloaded, OAMP_MEP_LM_DM_OFFLOADED);
    UTILEX_SET_BIT(oamp_params->flags, is_q_entry, OAMP_MEP_Q_ENTRY);
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR;
    oamp_params->exclusive.extra_data.crc_val1 = calculated_crc;
    oamp_params->exclusive.extra_data.opcode_bmp = opcode;

    if (nof_data_bits > DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN)
    {
        extra_data_len +=
            (nof_data_bits - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN - 1) / DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1;
        extra_data_hdr_seg_len = DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN;
    }
    else
    {
        
        sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                   sizeof(oamp_params->exclusive.extra_data.data_segment));
        extra_data_hdr_seg_len = nof_data_bits;
    }
    dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, 0, data, 0, extra_data_hdr_seg_len, FALSE);

    if (is_update)
    {
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD;

    
    for (pl_index = 1; pl_index < extra_data_len; pl_index++)
    {
        int bits_left = 0, bits_to_copy = 0, start_from = 0;
        sal_memset(oamp_params, 0, DATA_SEG_LEN);

        
        oamp_params->exclusive.extra_data.extra_data_len = MDB_EXTRA_DATA_PLD1_ACCESS + pl_index - 1;

        bits_left =
            nof_data_bits - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN - DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN * (pl_index - 1);
        if (bits_left < DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN)
        {
            bits_to_copy = bits_left;
            start_from = DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN - bits_left;
            
            sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                       sizeof(oamp_params->exclusive.extra_data.data_segment));
        }
        else
        {
            bits_to_copy = DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN;
        }
        dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, start_from,
                                data,
                                DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN + (pl_index -
                                                                      1) * DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN,
                                bits_to_copy, FALSE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_mdb_extra_data_get(
    int unit,
    uint16 oam_id,
    uint32 extra_data_length,
    uint8 *data)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 nof_payload_access = 0;
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Parameter structure for single entry",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_EXTRA_DATA_HDR_ACCESS, oamp_params));

    dnx_oamp_copy_bit_array(data, 0, oamp_params->exclusive.extra_data.data_segment, 0,
                            DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN, TRUE);

    if (extra_data_length > DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN)
    {
        nof_payload_access =
            ((extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) / (DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1)) + 1;
    }

    for (ii = 0; ii < nof_payload_access; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_EXTRA_DATA_PLD1_ACCESS + ii, oamp_params));

        dnx_oamp_copy_bit_array(data,
                                DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN +
                                (ii * DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN),
                                oamp_params->exclusive.extra_data.data_segment, 0,
                                DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN, TRUE);
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_ccm_endpoint_extra_data_set(
    int unit,
    dnx_oamp_ccm_endpoint_extra_data_t * extra_data)
{
    int i = 0, nof_bits = 0;
    int is_maid = 0, nof_maid_bits = 0;
    uint16 calculated_crc = 0;
    uint8 *extra_header_data = NULL;
    uint8 *data_for_crc_calculation = NULL;
    uint32 crc_length;

    uint32 entry_idx, next_bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(extra_header_data, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(data_for_crc_calculation, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID20))
    {
        is_maid = 1;
        nof_maid_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID40))
    {
        is_maid = 1;
        nof_maid_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID48))
    {
        is_maid = 1;
        nof_maid_bits = DNX_OAM_OAMP_NOF_MAID_BITS;
    }

    if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC))
    {
        sal_memcpy(extra_header_data, extra_data->dmac, sizeof(bcm_mac_t));
        nof_bits += DNX_LMDM_FLEXIBLE_DA_BITS;
        if (is_maid)
        {
            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_crc16(unit, (uint8 *) extra_data->maid48,
                                               DNX_OAMP_OAM_GROUP_MAID_OFFSET / UTILEX_NOF_BITS_IN_BYTE,
                                               BCM_OAM_GROUP_NAME_LENGTH, &calculated_crc));

            sal_memcpy(extra_header_data + DNX_LMDM_FLEXIBLE_DA_LENGTH, extra_data->maid48, BCM_OAM_GROUP_NAME_LENGTH);
            nof_bits += nof_maid_bits;
        }
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_set
                        (unit, extra_data->index, nof_bits,
                         extra_header_data, extra_data->opcodes_to_prepend, calculated_crc, 0,
                         _SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE)));
    }
    else
    {
        if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_SD))
        {
            nof_bits = 0;
            crc_length = (DNX_OAMP_OAM_GROUP_MAID_OFFSET / UTILEX_NOF_BITS_IN_BYTE);
            data_for_crc_calculation[2] = (extra_data->signal_degradation_rx << 6);
            if (is_maid)
            {
                sal_memcpy(data_for_crc_calculation + (DNX_OAMP_OAM_GROUP_MAID_OFFSET / UTILEX_NOF_BITS_IN_BYTE),
                           extra_data->maid48, BCM_OAM_GROUP_NAME_LENGTH);
                crc_length += BCM_OAM_GROUP_NAME_LENGTH;
                sal_memcpy(extra_header_data, extra_data->maid48, BCM_OAM_GROUP_NAME_LENGTH);
                nof_bits += nof_maid_bits;
            }
            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_crc16(unit, (uint8 *) data_for_crc_calculation, 0,
                                               crc_length, &calculated_crc));
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_set
                            (unit, extra_data->index, nof_bits,
                             extra_header_data, extra_data->opcodes_to_prepend, calculated_crc, 0,
                             _SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE)));
        }
        else
        {
            next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));
            for (i = 0; i < extra_data->length; i++)
            {
                entry_idx = MEP_DB_ENTRY_TO_OAM_ID(extra_data->index) + i * next_bank_offset;
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, entry_idx));

                
                SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry_idx));
            }
        }
    }

exit:
    SHR_FREE(extra_header_data);
    SHR_FREE(data_for_crc_calculation);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_ccm_endpoint_extra_data_get(
    int unit,
    dnx_oamp_ccm_endpoint_extra_data_t * extra_data)
{
    uint8 *extra_header_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(extra_header_data, OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (extra_data->index != DNX_OAM_EXTRA_DATA_HEADER_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_get(unit, extra_data->index, extra_header_data, NULL, NULL));
    }
    sal_memcpy(extra_data->dmac, extra_header_data, sizeof(bcm_mac_t));
    sal_memcpy(extra_data->maid48, extra_header_data + DNX_LMDM_FLEXIBLE_DA_LENGTH, BCM_OAM_GROUP_NAME_LENGTH);

exit:
    SHR_FREE(extra_header_data);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_ccm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_ccm_endpoint_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    bcm_oam_group_info_t *group_info = NULL;
    uint8 offloaded, q_entry;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    offloaded = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED);
    q_entry = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY);

    
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {
        if (q_entry != 1)
        {
            
            if (offloaded == 1)
            {
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
            }

            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
        }
        else
        {
            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_Q_ENTRY_ACCESS, oamp_params));
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
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));
    }

    
    if (offloaded == 1)
    {
        
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_Q_ENTRY);
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

    if (((offloaded == 1) || (q_entry == 1)) && (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_48B_MAID))
        && !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {
        uint16 calculated_crc = 0;
        SHR_ALLOC_SET_ZERO(group_info, sizeof(bcm_oam_group_info_t), "Group info struct", "%s%s%s\r\n", EMPTY, EMPTY,
                           EMPTY);

        

        SHR_IF_ERR_EXIT(dnx_oam_group_info_get(unit, entry_values->extra_data_header, group_info));

        SHR_IF_ERR_EXIT(oam_oamp_get_group_crc(unit, entry_values->extra_data_header, &calculated_crc));

        SHR_IF_ERR_EXIT(dnx_oamp_mdb_extra_data_set
                        (unit, oam_id, offloaded, q_entry, calculated_crc, DBAL_DEFINE_OAM_OPCODE_CCM,
                         DNX_OAM_OAMP_NOF_MAID_BITS, group_info->name, FALSE));
    }
    else if (entry_values->extra_data.index != DNX_OAM_EXTRA_DATA_HEADER_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_ccm_endpoint_extra_data_set(unit, &entry_values->extra_data));
    }

exit:
    SHR_FREE(group_info);
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_ccm_endpoint_get(
    int unit,
    uint16 oam_id,
    uint8 is_down_mep_egress_injection,
    dnx_oam_oamp_ccm_endpoint_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    dnx_oam_oamp_mep_db_args_t *oamp_params_rfc_6374 = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(oamp_params_rfc_6374, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for rfc_6347 entry",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {
        
        oamp_params->extra_data_ptr = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get
                        (unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr), oamp_params_rfc_6374));
        entry_values->mep_type = oamp_params_rfc_6374->mep_type;
    }
    else
    {
        entry_values->mep_type = oamp_params->mep_type;
    }

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_Q_ENTRY_ACCESS, oamp_params));
    }

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    }

    
    entry_values->flags = 0;

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_Q_ENTRY;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE)
        && UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_STAT_ENABLE))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_OFFLOADED_LM_ENABLE;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE)
        && UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_STAT_ENABLE))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_OFFLOADED_DM_ENABLE;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_IS_UPMEP))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_IS_UPMEP;
        
        entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port =
            oamp_params->unified_port_access.ccm_eth_up_mep_port.pp_port;
        entry_values->unified_port_access.ccm_eth_up_mep_port.port_core =
            oamp_params->unified_port_access.ccm_eth_up_mep_port.port_core;
    }
    else
    {
        entry_values->unified_port_access.port_profile = oamp_params->unified_port_access.port_profile;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_RDI_FROM_SCANNER))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_RDI_FROM_SCANNER;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_RDI_FROM_PACKET))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_PORT_TLV_EN))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_PORT_TLV_VAL))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_ACH))
    {
        entry_values->flags |= DNX_OAMP_OAM_RF6374_ACH;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_GAL))
    {
        entry_values->flags |= DNX_OAMP_OAM_RF6374_GAL;
    }

    if (oamp_params->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        
        if (UTILEX_GET_BIT(oamp_params->fec_id_or_glob_out_lif, FIELD_IS_FEC_ID_FLAG) != 0)
        {
            entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC;
            entry_values->fec_id_or_glob_out_lif.fec_id = oamp_params->fec_id_or_glob_out_lif;
                        
            UTILEX_SET_BIT(entry_values->fec_id_or_glob_out_lif.fec_id, 0, FIELD_IS_FEC_ID_FLAG);
        }
        else
        {
            entry_values->fec_id_or_glob_out_lif.glob_out_lif = oamp_params->fec_id_or_glob_out_lif;
        }
    }

    if (is_down_mep_egress_injection)
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION;
        entry_values->vsi = oamp_params->exclusive.oam_only.vsi;
    }

    entry_values->label = oamp_params->mpls_pwe.label;
    entry_values->counter_ptr = oamp_params->counter_ptr;
    entry_values->dest_sys_port_agr = oamp_params->dest_sys_port_agr;
    entry_values->extra_data_header = oamp_params->extra_data_ptr;
    entry_values->itmh_tc_dp_profile = oamp_params->itmh_tc_dp_profile;
    entry_values->mep_profile = oamp_params->mep_profile;
    entry_values->mep_pe_profile = oamp_params->mep_pe_profile;
    entry_values->push_profile = oamp_params->mpls_pwe.push_profile;
    entry_values->counter_interface = oamp_params->counter_interface;
    entry_values->crps_core_select = oamp_params->crps_core_select;
    entry_values->flex_lm_dm_entry = oamp_params->flex_lm_dm_ptr;
    if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {
        entry_values->session_id = oamp_params_rfc_6374->exclusive.rfc_6374_only.session_id;
        entry_values->timestamp_format = oamp_params_rfc_6374->exclusive.rfc_6374_only.timestamp_format;
    }
    else
    {
        entry_values->mep_id = oamp_params->exclusive.oam_only.oam_mep_id;
        entry_values->maid = oamp_params->exclusive.oam_only.oam_maid;
        entry_values->inner_vid = oamp_params->exclusive.oam_only.oam_inner_vid;
        entry_values->outer_vid = oamp_params->exclusive.oam_only.oam_outer_vid;
        entry_values->ccm_interval = oamp_params->exclusive.oam_only.oam_ccm_interval;
        entry_values->sa_gen_lsb = oamp_params->exclusive.oam_only.oam_sa_mac_lsb;
        entry_values->sa_gen_msb_profile = oamp_params->exclusive.oam_only.oam_sa_mac_msb_profile;
        entry_values->icc_index = oamp_params->exclusive.oam_only.oam_icc_index;
        entry_values->mdl = oamp_params->exclusive.oam_only.oam_mdl;
        entry_values->nof_vlan_tags = oamp_params->exclusive.oam_only.oam_nof_vlan_tags;
        entry_values->inner_tpid_index = oamp_params->exclusive.oam_only.oam_inner_tpid_index;
        entry_values->inner_pcp_dei = oamp_params->exclusive.oam_only.oam_inner_dei_pcp;
        entry_values->outer_tpid_index = oamp_params->exclusive.oam_only.oam_outer_tpid_index;
        entry_values->outer_pcp_dei = oamp_params->exclusive.oam_only.oam_outer_dei_pcp;
        entry_values->interface_status_tlv_code = oamp_params->exclusive.oam_only.oam_interface_status_tlv_code;
        entry_values->server_destination = oamp_params->exclusive.oam_only.server_destination;
        entry_values->signal_degradation_tx = oamp_params->exclusive.oam_only.tx_signal;
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FREE(oamp_params_rfc_6374);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_ccm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 extra_data_length)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    int access_index;
    uint32 nof_extra_data_access;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t),
              "Endpoint data read to find MDB entries before deleting", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY) == 1)
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, MDB_Q_ENTRY_ACCESS));
    }
    else
    {
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) == 1)
        {
                
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, MDB_PART_1_ACCESS));
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, MDB_PART_2_ACCESS));
        }
    }

    if (extra_data_length != 0)
    {
        nof_extra_data_access = 1;

        if ((extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) > 63)
        {
            nof_extra_data_access =
                (extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) / (DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1) + 2;
        }

        for (access_index = 0; access_index < nof_extra_data_access; access_index++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, access_index + MDB_EXTRA_DATA_HDR_ACCESS));
        }
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, oam_id));

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_rfc_6374_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_ccm_endpoint_t * entry_values,
    uint8 is_last_entry,
    uint8 is_bfd_exist)
{
    dnx_oam_oamp_mep_db_args_t *oamp_bfd_pwe_params = NULL;
    dnx_oam_oamp_mep_db_args_t *oamp_rfc_6374_params = NULL;
    uint8 bfd_update;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_bfd_pwe_params, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Param struct for creating single entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(oamp_rfc_6374_params, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Param struct for creating single entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    bfd_update = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE) || is_bfd_exist;
    if (bfd_update)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_bfd_pwe_params));
    }
    else
    {
        oamp_bfd_pwe_params->exclusive.oam_only.oam_lmm_da_profile = 0;
        oamp_bfd_pwe_params->flex_lm_dm_ptr = 0;
        
        oamp_bfd_pwe_params->flags = 0;
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {
         
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get
                        (unit, MEP_DB_ENTRY_TO_OAM_ID(entry_values->flex_lm_dm_entry), oamp_rfc_6374_params));
    }
    else
    {
        
        UTILEX_SET_BIT(oamp_bfd_pwe_params->flags,
                       _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_RF6374_ACH), OAMP_MEP_ACH);
        UTILEX_SET_BIT(oamp_bfd_pwe_params->flags,
                       _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_RF6374_GAL), OAMP_MEP_GAL);
    }

    
    oamp_bfd_pwe_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;

    UTILEX_SET_BIT(oamp_bfd_pwe_params->flags, bfd_update, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_rfc_6374_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE),
                   OAMP_MEP_UPDATE);

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC))
    {
        oamp_bfd_pwe_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
        UTILEX_SET_BIT(oamp_bfd_pwe_params->fec_id_or_glob_out_lif, TRUE, FIELD_IS_FEC_ID_FLAG);
    }
    else
    {
        oamp_bfd_pwe_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
    }

    oamp_bfd_pwe_params->flex_lm_dm_ptr = entry_values->flex_lm_dm_entry;
    oamp_bfd_pwe_params->mpls_pwe.label = entry_values->label;
    oamp_bfd_pwe_params->itmh_tc_dp_profile = entry_values->itmh_tc_dp_profile;
    oamp_bfd_pwe_params->mep_profile = entry_values->mep_profile;
    oamp_bfd_pwe_params->mep_pe_profile = entry_values->mep_pe_profile;
    oamp_bfd_pwe_params->mpls_pwe.push_profile = entry_values->push_profile;
    oamp_bfd_pwe_params->crps_core_select = entry_values->crps_core_select;
    oamp_bfd_pwe_params->dest_sys_port_agr = entry_values->dest_sys_port_agr;

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_bfd_pwe_params));

    
    oamp_rfc_6374_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    oamp_rfc_6374_params->counter_ptr = entry_values->counter_ptr;
    oamp_rfc_6374_params->counter_interface = entry_values->counter_interface;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.session_id = entry_values->session_id;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.timestamp_format = entry_values->timestamp_format;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.dm_t_flag = entry_values->dm_t_flag;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.dm_ds = entry_values->dm_priority;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.lm_cw_choose = 0;
    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {
        UTILEX_SET_BIT(oamp_rfc_6374_params->flags, is_last_entry, OAMP_MEP_LAST_ENTRY);
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add
                    (unit, MEP_DB_ENTRY_TO_OAM_ID(entry_values->flex_lm_dm_entry), oamp_rfc_6374_params));

exit:
    SHR_FREE(oamp_bfd_pwe_params);
    SHR_FREE(oamp_rfc_6374_params);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_lm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    const dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    uint8 set_part2_ptr = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
    oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR))
    {
        oamp_params->counter_ptr = entry_values->slm_counter_pointer;
        oamp_params->counter_interface = entry_values->slm_core_id;
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    
    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for LM/DM offloaded MEPs, LM statistics are "
                     "automatically calculated, ADD_STAT_ENTRY flag must be set.\n");
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    if (oamp_params->flex_lm_dm_ptr == 0)
    {
        if (entry_values->flex_lm_entry == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: the second MEP DB entry has not been created "
                         "yet for this LM/DM offloaded MEP, so the flex_lm_entry must be "
                         "set to the target entry, which cannot be 0.");
        }

        set_part2_ptr = TRUE;
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
        
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_values->flex_lm_entry, oamp_params));
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_STAT_ENABLE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_ENABLE);
    if (set_part2_ptr)
    {
        
        oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_lm_entry);
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_find_or_add_lm_dm_entry(
    int unit,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    dnx_oam_oamp_lm_dm_search_t * search_params,
    int is_find_only)
{
    uint32 lm_dm_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    if (!IS_LM_DM_TYPE(search_params->search_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %d is not an LM type nor a DM TYPE.\n", search_params->search_type);
    }

    search_params->update_entry = FALSE;
    search_params->clear_last_entry = TRUE;
    lm_dm_entry = search_params->first_entry;

    if (lm_dm_entry != 0)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_dm_entry, oamp_params));
    }

    if ((lm_dm_entry == 0) || !IS_LM_DM_TYPE(oamp_params->mep_type))
    {
        
        search_params->resulting_entry = search_params->param_entry;
        search_params->clear_last_entry = FALSE;
    }
    else
    {
        while (TRUE)
        {
            if (oamp_params->mep_type == search_params->search_type)
            {
                search_params->resulting_entry = lm_dm_entry;
                search_params->update_entry = TRUE;
                search_params->clear_last_entry = FALSE;
                break;
            }
            lm_dm_entry += next_bank_offset;
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LAST_ENTRY))
            {
                if (is_find_only == LM_DM_FIND_ONLY)
                {
                    search_params->resulting_entry = lm_dm_entry - next_bank_offset;
                }
                else
                {
                    search_params->resulting_entry = lm_dm_entry;
                }
                break;
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_dm_entry, oamp_params));
            if (!IS_LM_DM_TYPE(oamp_params->mep_type))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: an entry of type %d found before last entry.\n",
                             oamp_params->mep_type);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_db_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *update_ptr)
{
    uint16 flex_lm_dm_ptr = 0;
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_db_entry, next_bank_offset;
    int num_short_entries_in_bank;
    SHR_FUNC_INIT_VARS(unit);

    
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    
    flex_lm_dm_ptr = oamp_params->flex_lm_dm_ptr;

    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(flex_lm_dm_ptr);
    search_params.param_entry = entry_values->flex_lm_entry;
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    lm_db_entry = search_params.resulting_entry;

    if (search_params.update_entry)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }
    else
    {
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
        
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
    }
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    if ((oam_id / num_short_entries_in_bank) == (lm_db_entry / num_short_entries_in_bank))

    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: adding a loss entry will result in that entry "
                     "being in the same bank as the endpoint entry.\n");
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_db_entry, oamp_params));
    entry_values->flex_lm_entry = lm_db_entry;

    if (search_params.clear_last_entry)
    {
        
        lm_db_entry -= next_bank_offset;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_db_entry, oamp_params));
    }

    if (!search_params.update_entry)
    {
        if (flex_lm_dm_ptr == 0)
        {
            
            oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(lm_db_entry);
        }
        else
        {
            
            oamp_params->flex_lm_dm_ptr = flex_lm_dm_ptr;
        }
    }
    
    *update_ptr = !search_params.update_entry && !search_params.clear_last_entry;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_stat_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_stat_entry, dm_entry_idx, next_bank_offset;
    int num_short_entries_in_bank;
    SHR_FUNC_INIT_VARS(unit);

    
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = entry_values->flex_lm_entry;
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    lm_stat_entry = search_params.resulting_entry;

    if (search_params.update_entry)
    {
        
        if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY))
        {
            
            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_DM_STAT_EXISTS) &&
                (entry_values->flex_lm_entry == MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr)))
            {
                
                dm_entry_idx = (lm_stat_entry + next_bank_offset);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_entry_idx, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_DM_STATISTICS);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_stat_entry, oamp_params));

                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, dm_entry_idx));
            }
            else
            {
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_stat_entry));

                lm_stat_entry -= next_bank_offset;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_stat_entry, oamp_params));
            }
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_UPDATE) &&
            !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_LM_STAT_EXISTS) &&
            _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_DM_STAT_EXISTS) &&
            (entry_values->flex_lm_entry == MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr)))
        {
            
            dm_entry_idx = entry_values->flex_lm_entry + next_bank_offset;
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_entry_idx, oamp_params));
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_DM_STATISTICS);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, dm_entry_idx + next_bank_offset, oamp_params));

            
            lm_stat_entry = dm_entry_idx;
            sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
            UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            search_params.clear_last_entry = FALSE;
        }
        else
        {
            sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

            
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
        }

        oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
        num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
            dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        if ((oam_id / num_short_entries_in_bank) == (lm_stat_entry / num_short_entries_in_bank))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: adding a loss statistics entry will result in that entry "
                         "being in the same bank as the endpoint entry.\n");
        }
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_stat_entry, oamp_params));

        if (search_params.clear_last_entry)
        {
            
            lm_stat_entry -= next_bank_offset;
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
            UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_stat_entry, oamp_params));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_lm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint8 update_ptr = FALSE;
    uint8 current_oam_lmm_da_profile = 0;
    uint8 current_oam_mep_profile = 0;
    uint8 current_counter_interface = 0;
    uint32 current_counter_pointer = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    
    UTILEX_SET_BIT(oamp_params->flags,
                   _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_UPDATE), OAMP_MEP_UPDATE);

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_offloaded_endpoint_set(unit, oam_id, entry_values, oamp_params));
    }
    else
    {
        
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            
            current_oam_lmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;
            current_oam_mep_profile = oamp_params->mep_profile;
            current_counter_interface = oamp_params->counter_interface;
            current_counter_pointer = oamp_params->counter_ptr;

            
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_db_set(unit, oam_id, entry_values,
                                                                              oamp_params, &update_ptr));

            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY) ||
                (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_UPDATE) &&
                 _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_LM_STAT_EXISTS)))
            {
                
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_stat_set
                                (unit, oam_id, entry_values, oamp_params));
            }

            
            if (update_ptr || (current_oam_lmm_da_profile != entry_values->lmm_dmm_da_profile)
                || (current_oam_mep_profile != entry_values->mep_profile)
                || (current_counter_interface != entry_values->slm_core_id)
                || (current_counter_pointer != entry_values->slm_counter_pointer))
            {
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                if (update_ptr)
                {
                    
                    oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_lm_entry);
                }
                oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
                oamp_params->mep_profile = entry_values->mep_profile;
                if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR))
                {
                    oamp_params->counter_ptr = entry_values->slm_counter_pointer;
                    oamp_params->counter_interface = entry_values->slm_core_id;
                }
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_lm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists)
{
    uint32 second_hl_entry;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    if (!UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_STAT_ENABLE))
    {
        *lm_exists = 0;
        SHR_EXIT();
    }

    *lm_exists = 1;
    entry_values->lm_my_tx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_my_rx = oamp_params->exclusive.lm_dm.lm_my_rx;
    entry_values->lm_peer_tx = oamp_params->exclusive.lm_dm.lm_peer_tx;
    entry_values->lm_peer_rx = oamp_params->exclusive.lm_dm.lm_peer_rx;
    entry_values->last_lm_far = oamp_params->exclusive.lm_dm.last_lm_far;
    entry_values->last_lm_near = oamp_params->exclusive.lm_dm.last_lm_near;
    entry_values->max_lm_far = oamp_params->exclusive.lm_dm.max_lm_far;

    if (oamp_params->flex_lm_dm_ptr != 0)
    {
        second_hl_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, second_hl_entry, oamp_params));
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
            UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            entry_values->acc_lm_far = oamp_params->exclusive.lm_dm.acc_lm_far;
            entry_values->acc_lm_near = oamp_params->exclusive.lm_dm.acc_lm_near;
            entry_values->max_lm_near = oamp_params->exclusive.lm_dm.max_lm_near;
        }
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_db_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_db_entry;
    SHR_FUNC_INIT_VARS(unit);

    
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
    lm_db_entry = search_params.resulting_entry;

    if (!search_params.update_entry)
    {
        *lm_exists = 0;
        SHR_EXIT();
    }
    *lm_exists = 1;
    entry_values->lm_db_entry_idx = lm_db_entry;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));

    entry_values->lm_my_tx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_my_rx = oamp_params->exclusive.lm_dm.lm_my_rx;
    entry_values->lm_peer_tx = oamp_params->exclusive.lm_dm.lm_peer_tx;
    entry_values->lm_peer_rx = oamp_params->exclusive.lm_dm.lm_peer_rx;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_stat_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_stat_entry;
    SHR_FUNC_INIT_VARS(unit);

    
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
    lm_stat_entry = search_params.resulting_entry;

    *lm_stat_exists = 0;
    if (search_params.update_entry)
    {
        *lm_stat_exists = 1;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
        entry_values->lm_stat_entry_idx = lm_stat_entry;

        entry_values->acc_lm_far = oamp_params->exclusive.lm_dm.acc_lm_far;
        entry_values->acc_lm_near = oamp_params->exclusive.lm_dm.acc_lm_near;
        entry_values->last_lm_far = oamp_params->exclusive.lm_dm.last_lm_far;
        entry_values->last_lm_near = oamp_params->exclusive.lm_dm.last_lm_near;
        entry_values->max_lm_far = oamp_params->exclusive.lm_dm.max_lm_far;
        entry_values->max_lm_near = oamp_params->exclusive.lm_dm.max_lm_near;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_lm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    uint8 *lm_exists,
    uint8 *lm_stat_exists)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;
    entry_values->flex_lm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    entry_values->mep_profile = oamp_params->mep_profile;

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_offloaded_endpoint_get(unit, oam_id, entry_values, oamp_params, lm_exists));
    }
    else
    {
        
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            

            
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_db_get
                            (unit, oam_id, entry_values, oamp_params, lm_exists));

            
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_stat_get(unit, oam_id, entry_values,
                                                                                oamp_params, lm_stat_exists));
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_lm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 is_mpls_lm_dm_entry_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 entry_index = 0, dm_stat_entry, flex_lm_dm_entry, second_mep_db_entry, next_bank_offset;
    uint8 dm_entry_found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LM_STAT_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LM_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_LM_STATISTICS);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

        
        if (oamp_params->flex_lm_dm_ptr != 0)
        {
            second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE) &&
                UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_STAT_ENABLE))
            {
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, second_mep_db_entry, oamp_params));
                if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
                    UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
                {
                   
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_LM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, second_mep_db_entry, oamp_params));
                }
            }
            else
            {
                
                oamp_params->flex_lm_dm_ptr = 0;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, second_mep_db_entry));
            }
        }
    }
    else
    {
        
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            
            if (is_mpls_lm_dm_entry_exists)
            {
                entry_index = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
                flex_lm_dm_entry = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(entry_index);
            }
            else
            {
                flex_lm_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            }
            
            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
            dm_stat_entry = search_params.resulting_entry;

            if (!search_params.update_entry)
            {
                
                search_params.first_entry = flex_lm_dm_entry;
                search_params.param_entry = flex_lm_dm_entry;
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
                dm_stat_entry = search_params.resulting_entry;
            }

            dm_entry_found = search_params.update_entry;

            
            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));

            if (dm_entry_found)
            {
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_DM_STATISTICS);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, flex_lm_dm_entry, oamp_params));
                if (flex_lm_dm_entry != dm_stat_entry)
                {
                    
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, dm_stat_entry));
                }
                else
                {
                    
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, dm_stat_entry + next_bank_offset));
                }
            }
            else
            {
                
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

                if (is_mpls_lm_dm_entry_exists)
                {
                    
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, entry_index, oamp_params));
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_index, oamp_params));
                    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                }

                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                if (!is_mpls_lm_dm_entry_exists)
                {
                    oamp_params->flex_lm_dm_ptr = 0;
                }
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, flex_lm_dm_entry));
            }

            if (search_params.update_entry)
            {
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, search_params.resulting_entry));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_dm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    uint32 second_mep_db_entry;
    uint8 set_part2_ptr = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for LM/DM offloaded MEPs, DM measurement can"
                     "only be two-way, so two way flag must be set.\n");
    }
    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
    oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    

    
    second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);

    
    if (second_mep_db_entry != 0)
    {
        
        if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_UPDATE))
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, second_mep_db_entry, oamp_params));
            
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, second_mep_db_entry, oamp_params));
        }
    }
    else
    {
        
        if (entry_values->flex_dm_entry == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: the second MEP DB entry has not been created "
                         "yet for this LM/DM offloaded MEP, so the flex_dm_entry must be"
                         "set to the target entry, which cannot be 0.");
        }

        set_part2_ptr = TRUE;
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
        oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;
                
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
        
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_values->flex_dm_entry, oamp_params));
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_DM_STAT_ENABLE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_DM_ENABLE);
    if (set_part2_ptr)
    {
        
        oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_dm_entry);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_dm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 dm_stat_entry, next_bank_offset;
    int num_short_entries_in_bank;
    uint8 update_ptr = FALSE;
    uint8 current_oam_lmm_da_profile = 0;
    uint8 current_oam_mep_profile = 0;
    uint8 current_oam_mep_pe_profile = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_offloaded_endpoint_set(unit, oam_id, entry_values, oamp_params));
    }
    else
    {
        
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            
            current_oam_lmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;
            current_oam_mep_profile = oamp_params->mep_profile;
            current_oam_mep_pe_profile = oamp_params->mep_pe_profile;

            
            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY) ==
                _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Choose either one-way or two-way measurement.\n");
            }

            
            search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            search_params.param_entry = entry_values->flex_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
            dm_stat_entry = search_params.resulting_entry;

            if (!search_params.update_entry)
            {
                
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
                dm_stat_entry = search_params.resulting_entry;
            }

            
            if (search_params.update_entry)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            }
            else
            {
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

                
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);

                
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
            }
            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
            {
                oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            }
            else
            {
                oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            }
            num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
                dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
            if ((oam_id / num_short_entries_in_bank) == (dm_stat_entry / num_short_entries_in_bank))

            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: adding a delay entry will result in that entry "
                             "being in the same bank as the endpoint entry.\n");
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, dm_stat_entry, oamp_params));

            if (search_params.clear_last_entry)
            {
                 
                dm_stat_entry -= next_bank_offset;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, dm_stat_entry, oamp_params));
            }

            update_ptr = !search_params.update_entry && !search_params.clear_last_entry;

            
            if (update_ptr || (current_oam_lmm_da_profile != entry_values->lmm_dmm_da_profile)
                || (current_oam_mep_profile != entry_values->mep_profile)
                || (current_oam_mep_pe_profile != entry_values->mep_pe_profile))
            {
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                if (update_ptr)
                {
                    
                    oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_dm_entry);
                }
                oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
                oamp_params->mep_profile = entry_values->mep_profile;
                oamp_params->mep_pe_profile = entry_values->mep_pe_profile;
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_dm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    if (!UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_STAT_ENABLE))
    {
        *is_found = 0;
        SHR_EXIT();
    }

    *is_found = 1;

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr), oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
        UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        sal_memcpy(entry_values->last_delay, oamp_params->exclusive.lm_dm.last_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->max_delay, oamp_params->exclusive.lm_dm.max_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->min_delay, oamp_params->exclusive.lm_dm.min_delay, TWO_WAY_DELAY_SIZE);
        entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
        entry_values->flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error!  This endpoint has no DM entry.\n");
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_dm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 dm_stat_entry;
    SHR_FUNC_INIT_VARS(unit);

    entry_values->flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);

    
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    dm_stat_entry = search_params.resulting_entry;

    *is_found = 0;
    if (search_params.update_entry)
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
        *is_found = 1;
        entry_values->dm_stat_entry_idx = dm_stat_entry;
        sal_memcpy(entry_values->last_delay, oamp_params->exclusive.lm_dm.last_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->max_delay, oamp_params->exclusive.lm_dm.max_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->min_delay, oamp_params->exclusive.lm_dm.min_delay, TWO_WAY_DELAY_SIZE);
        entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
    }
    else
    {
        search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
        dm_stat_entry = search_params.resulting_entry;
        if (search_params.update_entry)
        {
            *is_found = 1;
            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
            entry_values->dm_stat_entry_idx = dm_stat_entry;
            sal_memcpy(entry_values->last_delay_one_way, oamp_params->exclusive.lm_dm.last_delay_one_way,
                       ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->max_delay_one_way, oamp_params->exclusive.lm_dm.max_delay_one_way,
                       ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->min_delay_one_way, oamp_params->exclusive.lm_dm.min_delay_one_way,
                       ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->last_delay_one_way_near_end,
                       oamp_params->exclusive.lm_dm.last_delay_one_way_near_end, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->max_delay_one_way_near_end,
                       oamp_params->exclusive.lm_dm.max_delay_one_way_near_end, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->min_delay_one_way_near_end,
                       oamp_params->exclusive.lm_dm.min_delay_one_way_near_end, ONE_WAY_DELAY_SIZE);
            entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_dm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    uint8 *is_found)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;
    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_offloaded_endpoint_get(unit, oam_id, entry_values, oamp_params, is_found));
    }
    else
    {
        
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            

            
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_dm_stat_get(unit, oam_id, entry_values,
                                                                                oamp_params, is_found));
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_dm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 is_mpls_lm_dm_entry_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 entry_index, lm_db_entry, lm_stat_entry, flex_lm_dm_entry, second_mep_db_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_DM_STAT_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_DM_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

        
        if (oamp_params->flex_lm_dm_ptr != 0)
        {
            second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE) &&
                UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_STAT_ENABLE))
            {
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, second_mep_db_entry, oamp_params));
                if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
                    UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
                {
                    
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, second_mep_db_entry, oamp_params));
                }
            }
            else
            {
                
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, second_mep_db_entry));
                oamp_params->flex_lm_dm_ptr = 0;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }
    else
    {
        
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            
            if (is_mpls_lm_dm_entry_exists)
            {
                entry_index = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
                flex_lm_dm_entry = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(entry_index);
            }
            else
            {
                flex_lm_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            }

            
            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
            lm_db_entry = search_params.resulting_entry;

            if (search_params.update_entry)
            {
                
                search_params.first_entry = flex_lm_dm_entry;
                search_params.param_entry = flex_lm_dm_entry;
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
                lm_stat_entry = search_params.resulting_entry;

                if (search_params.update_entry)
                {
                    
                    if (flex_lm_dm_entry == lm_stat_entry)
                    {
                        
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add
                                        (unit, flex_lm_dm_entry + next_bank_offset, oamp_params));
                    }
                    else
                    {
                        
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, flex_lm_dm_entry, oamp_params));

                        
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add
                                        (unit, flex_lm_dm_entry + next_bank_offset, oamp_params));

                        if (flex_lm_dm_entry != lm_db_entry)
                        {
                            
                            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_stat_entry));
                        }
                        else
                        {
                            
                            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_stat_entry + next_bank_offset));
                        }
                    }
                }
                else
                {
                    
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, flex_lm_dm_entry, oamp_params));

                    if (flex_lm_dm_entry != lm_db_entry)
                    {
                        
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_db_entry));
                    }
                    else
                    {
                        
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_db_entry + next_bank_offset));
                    }
                }
            }
            else
            {
                
                if (flex_lm_dm_entry != 0)
                {
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, flex_lm_dm_entry, oamp_params));
                    if (IS_LM_DM_TYPE(oamp_params->mep_type))
                    {
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, flex_lm_dm_entry));
                    }
                    
                    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                    if (!is_mpls_lm_dm_entry_exists)
                    {
                        oamp_params->flex_lm_dm_ptr = 0;
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
                    }
                    if (is_mpls_lm_dm_entry_exists)
                    {
                        
                        entry_index = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
                        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, entry_index, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_index, oamp_params));
                    }
                }
            }
        }
    }

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
dnx_oam_oamp_mep_profile_set(
    int unit,
    uint8 profile,
    uint8 is_rfc_6374,
    const dnx_oam_mep_profile_t * mep_profile)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_ccm_interval_e dmm_interval, lmm_interval, opcode_0_interval, opcode_1_interval;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);

    
    if (is_rfc_6374)
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, mep_profile->dmm_rate);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, mep_profile->lmm_rate);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->dmm_rate, &dmm_interval));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, dmm_interval);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->lmm_rate, &lmm_interval));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, lmm_interval);
    }

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_DUAL_ENDED_LM, INST_SINGLE,
                                mep_profile->piggy_back_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_SLM, INST_SINGLE, mep_profile->slm_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_LM, INST_SINGLE,
                                mep_profile->report_mode_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_DM, INST_SINGLE,
                                mep_profile->report_mode_dm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RDI_GEN_METHOD, INST_SINGLE,
                                mep_profile->rdi_gen_method);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_OFFSET, INST_SINGLE, mep_profile->dmm_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMR_OFFSET, INST_SINGLE, mep_profile->dmr_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_OFFSET, INST_SINGLE, mep_profile->lmm_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMR_OFFSET, INST_SINGLE, mep_profile->lmr_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_BIT_MAP, INST_SINGLE,
                                mep_profile->opcode_bit_map);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DM_MEASUREMENT_TYPE, INST_SINGLE,
                                mep_profile->dm_measurement_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_COUNT, INST_SINGLE, mep_profile->ccm_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMM_COUNT, INST_SINGLE, mep_profile->dmm_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_COUNT, INST_SINGLE, mep_profile->lmm_count);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->opcode_0_rate, &opcode_0_interval));
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 0, opcode_0_interval);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->opcode_1_rate, &opcode_1_interval));
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 1, opcode_1_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 0, mep_profile->opcode_0_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 1, mep_profile->opcode_1_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_MAID_CHECK, INST_SINGLE,
                                 mep_profile->maid_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_MDL_CHECK, INST_SINGLE,
                                 mep_profile->mdl_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_SRC_IP_CHECK, INST_SINGLE,
                                 mep_profile->src_ip_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_UDP_SRC_PORT_CHECK, INST_SINGLE,
                                 mep_profile->udp_src_port_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_YOUR_DISCR_CHECK, INST_SINGLE,
                                 mep_profile->your_disc_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, INST_SINGLE,
                                 mep_profile->oamp_mac_da_oui_prof);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_COUNTER_ENABLE, INST_SINGLE,
                                mep_profile->opcode_tx_statistics_enable);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RX_COUNTER_ENABLE, INST_SINGLE,
                                mep_profile->opcode_rx_statistics_enable);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_MSK, INST_SINGLE, mep_profile->opcode_mask);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID_SHIFT, INST_SINGLE, mep_profile->mep_id_shift);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MEASUREMENT_PERIOD_PROFILE, INST_SINGLE,
                                mep_profile->slm_measurement_period_index);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mep_profile_get(
    int unit,
    uint8 profile,
    dnx_oam_mep_profile_t * mep_profile)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_ccm_interval_e dmm_interval = 0, lmm_interval = 0, opcode_0_interval = 0, opcode_1_interval =
        0;
    
    int dmm_rate_temp, lmm_rate_temp, opcode_0_rate_temp, opcode_1_rate_temp;
    uint32 src_ip_chk_dis_temp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);

    
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, (uint8 *) &dmm_interval);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, (uint8 *) &lmm_interval);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_DUAL_ENDED_LM, INST_SINGLE,
                              &mep_profile->piggy_back_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_SLM, INST_SINGLE, &mep_profile->slm_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_LM, INST_SINGLE,
                              &mep_profile->report_mode_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_DM, INST_SINGLE,
                              &mep_profile->report_mode_dm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_RDI_GEN_METHOD, INST_SINGLE,
                              &mep_profile->rdi_gen_method);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMM_OFFSET, INST_SINGLE, &mep_profile->dmm_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMR_OFFSET, INST_SINGLE, &mep_profile->dmr_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMM_OFFSET, INST_SINGLE, &mep_profile->lmm_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMR_OFFSET, INST_SINGLE, &mep_profile->lmr_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_BIT_MAP, INST_SINGLE,
                              &mep_profile->opcode_bit_map);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DM_MEASUREMENT_TYPE, INST_SINGLE,
                              &mep_profile->dm_measurement_type);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DISABLE_MDL_CHECK, INST_SINGLE,
                              &mep_profile->mdl_check_dis);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CCM_COUNT, INST_SINGLE, &mep_profile->ccm_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DMM_COUNT, INST_SINGLE, &mep_profile->dmm_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LMM_COUNT, INST_SINGLE, &mep_profile->lmm_count);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 0, (uint8 *) &opcode_0_interval);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 1, (uint8 *) &opcode_1_interval);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 0, &mep_profile->opcode_0_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 1, &mep_profile->opcode_1_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DISABLE_SRC_IP_CHECK, INST_SINGLE,
                               &src_ip_chk_dis_temp);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_MEASUREMENT_PERIOD_PROFILE, INST_SINGLE,
                              &mep_profile->slm_measurement_period_index);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, dmm_interval, &(dmm_rate_temp)));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, lmm_interval, &(lmm_rate_temp)));
    mep_profile->dmm_rate = dmm_rate_temp;
    mep_profile->lmm_rate = lmm_rate_temp;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, opcode_0_interval, &(opcode_0_rate_temp)));
    mep_profile->opcode_0_rate = opcode_0_rate_temp;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, opcode_1_interval, &(opcode_1_rate_temp)));
    mep_profile->opcode_1_rate = opcode_1_rate_temp;
    mep_profile->src_ip_check_dis = src_ip_chk_dis_temp;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mep_profile_clear(
    int unit,
    uint8 profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));

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
dnx_oam_oamp_mpls_pwe_exp_ttl_profile_set(
    int unit,
    uint8 profile,
    uint8 ttl,
    uint8 exp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, ttl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, INST_SINGLE, exp);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mpls_pwe_exp_ttl_profile_get(
    int unit,
    uint8 profile,
    uint8 *exp,
    uint8 *ttl)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);

    
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, ttl);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EXP, INST_SINGLE, exp);

    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mpls_pwe_exp_ttl_profile_clear(
    int unit,
    uint8 profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_tpid_profile_set(
    int unit,
    uint8 profile,
    uint16 tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, profile);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_tpid_profile_get(
    int unit,
    uint8 profile,
    uint16 *tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, profile);

    
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid);

    
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

    
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     mep_hw_profiles_write_data->mep_profile_index,
                                                     ((mep_hw_profiles_write_data->flags &
                                                       DNX_OAM_OAMP_PROFILE_MPLS_LM_DM_SET) ==
                                                      DNX_OAM_OAMP_PROFILE_MPLS_LM_DM_SET),
                                                     &mep_hw_profiles_write_data->mep_profile));
    }

    
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_set(unit,
                                                      mep_hw_profiles_write_data->outer_tpid_index,
                                                      mep_hw_profiles_write_data->outer_tpid));
    }

    
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_set(unit,
                                                      mep_hw_profiles_write_data->inner_tpid_index,
                                                      mep_hw_profiles_write_data->inner_tpid));
    }

    
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_sa_mac_msb_profile_set(unit,
                                                            mep_hw_profiles_write_data->sa_mac_msb_profile,
                                                            mep_hw_profiles_write_data->sa_mac_msb));
    }

    
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_set(unit,
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


static shr_error_e
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

    if (mep_db_entry->nof_vlan_tags == 1)
    {
        endpoint_info->vlan = mep_db_entry->inner_vid;
        endpoint_info->pkt_pri = mep_db_entry->inner_pcp_dei;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit,
                                                      mep_db_entry->inner_tpid_index, &endpoint_info->outer_tpid));
    }

    if (mep_db_entry->nof_vlan_tags == 2)
    {
        endpoint_info->inner_vlan = mep_db_entry->inner_vid;
        endpoint_info->inner_pkt_pri = mep_db_entry->inner_pcp_dei;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit,
                                                      mep_db_entry->inner_tpid_index, &endpoint_info->inner_tpid));

        endpoint_info->vlan = mep_db_entry->outer_vid;
        endpoint_info->pkt_pri = mep_db_entry->outer_pcp_dei;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit,
                                                      mep_db_entry->outer_tpid_index, &endpoint_info->outer_tpid));
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_sa_mac_msb_profile_get(unit, mep_db_entry->sa_gen_msb_profile, sa_mac_addr_msb));
    sal_memcpy(endpoint_info->src_mac_address, sa_mac_addr_msb, DNX_OAM_MAC_SIZE);

    endpoint_info->src_mac_address[5] = (uint8) mep_db_entry->sa_gen_lsb;
    endpoint_info->src_mac_address[4] = (endpoint_info->src_mac_address[4] & ~DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK)
        | (uint8) ((mep_db_entry->sa_gen_lsb >> SAL_UINT8_NOF_BITS) & DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_ccm_endpoint_mpls_oam_get(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mep_db_entry->label == MPLS_LABEL_GAL)
    {
        if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
        {
            endpoint_info->type = bcmOAMEndpointTypeMplsLmDmSection;
        }
        else
        {
            endpoint_info->type = bcmOAMEndpointTypeBhhSection;
        }
    }
    else
    {
        endpoint_info->egress_label.label = mep_db_entry->label;
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_get(unit,
                                                              mep_db_entry->push_profile,
                                                              &endpoint_info->egress_label.exp,
                                                              &endpoint_info->egress_label.ttl));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
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


static shr_error_e
dnx_oam_oamp_ccm_endpoint_mpls_oam_init(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    int profile_id;
    uint8 first_reference;
    dnx_oam_ttl_exp_profile_t mpls_profile;

    SHR_FUNC_INIT_VARS(unit);

    mep_db_entry->label = (endpoint_info->type == bcmOAMEndpointTypeBhhSection
                           || endpoint_info->type ==
                           bcmOAMEndpointTypeMplsLmDmSection) ? MPLS_LABEL_GAL : endpoint_info->egress_label.label;

    mpls_profile.ttl = endpoint_info->egress_label.ttl;
    mpls_profile.exp = endpoint_info->egress_label.exp;

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.allocate_single
                    (unit, 0, &mpls_profile, NULL, &profile_id, &first_reference));

    
    if (first_reference)
    {
        
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG;

        
        mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile = profile_id;
        mep_hw_profiles_write_data->ttl_exp.exp = mpls_profile.exp;
        mep_hw_profiles_write_data->ttl_exp.ttl = mpls_profile.ttl;
    }

    mep_db_entry->push_profile = (uint8) profile_id;

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


static shr_error_e
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
    uint32 system_port, dest_val;
    dnx_oam_mep_profile_t mep_profile;
    uint8 rdi_from_rx = 0, rdi_from_scannner = 0;
    dbal_fields_e dbal_dest_type;
    int system_headers_mode;

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
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, (uint16) endpoint_info->id,
                                                     DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info),
                                                     mep_db_entry));

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
        else
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

    counter = 0;
    sal_memset(&mep_profile, 0, sizeof(dnx_oam_mep_profile_t));
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get
                    (unit, mep_db_entry->mep_profile, &counter, &mep_profile));

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        DNX_OAM_GET_RDI_GEN_METHOD_FIELD_FROM_MEP_PROFILE(mep_profile.rdi_gen_method, rdi_from_rx, rdi_from_scannner);
        
        if (!(rdi_from_rx && rdi_from_scannner))
        {
            endpoint_info->flags2 |= rdi_from_rx ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;
            endpoint_info->flags2 |= rdi_from_scannner ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE;
        }

        
        endpoint_info->name = mep_db_entry->mep_id;

        
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

        if ((mep_db_entry->ccm_interval > 0) && (mep_profile.ccm_count == DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT))
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
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mpls_oam_get(unit, mep_db_entry, endpoint_info));
    }

exit:
    SHR_FREE(mep_db_entry);
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_oam_oamp_ccm_endpoint_mpls_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.free_single
                    (unit, (int) mep_db_entry->push_profile, &last_reference));

    if (last_reference)
    {
        
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG;

        
        mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile = mep_db_entry->push_profile;
        mep_hw_profiles_write_data->ttl_exp.exp = 0;
        mep_hw_profiles_write_data->ttl_exp.ttl = 0;
    }

exit:
    SHR_FUNC_EXIT;

}


static shr_error_e
dnx_oam_oamp_ccm_endpoint_eth_oam_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;
    bcm_mac_t sa_mac;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.free_single
                        (unit, mep_db_entry->unified_port_access.port_profile, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG;
            mep_hw_profiles_write_data->pp_port_profile = mep_db_entry->unified_port_access.port_profile;
            mep_hw_profiles_write_data->system_port = 0;
        }
    }
    if (mep_db_entry->nof_vlan_tags != 0)
    {
        SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.free_single
                        (unit, (int) mep_db_entry->inner_tpid_index, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG;

            
            mep_hw_profiles_write_data->inner_tpid_index = mep_db_entry->inner_tpid_index;
            mep_hw_profiles_write_data->inner_tpid = 0;

        }
        if (mep_db_entry->nof_vlan_tags == 2)
        {

            SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.free_single
                            (unit, (int) mep_db_entry->outer_tpid_index, &last_reference));
            if (last_reference)
            {
                mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG;

                
                mep_hw_profiles_write_data->outer_tpid_index = mep_db_entry->outer_tpid_index;
                mep_hw_profiles_write_data->outer_tpid = 0;
            }
        }

        SHR_IF_ERR_EXIT(algo_oam_db.oam_sa_mac_msb_profile_template.free_single
                        (unit, (int) mep_db_entry->sa_gen_msb_profile, &last_reference));

        if (last_reference)
        {
            sal_memset(sa_mac, 0, sizeof(bcm_mac_t));

            
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG;

            
            mep_hw_profiles_write_data->sa_mac_msb_profile = mep_db_entry->sa_gen_msb_profile;
            sal_memcpy(mep_hw_profiles_write_data->sa_mac_msb, sa_mac, DNX_OAM_MAC_SIZE);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_ccm_endpoint_mep_db_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {

        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.free_single
                        (unit, (int) mep_db_entry->itmh_tc_dp_profile, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG;

            mep_hw_profiles_write_data->itmh_profile = mep_db_entry->itmh_tc_dp_profile;
            mep_hw_profiles_write_data->itmh_priority.dp = 0;
            mep_hw_profiles_write_data->itmh_priority.tc = 0;
        }
    }

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.free_single(unit, (int) mep_db_entry->mep_profile, &last_reference));

    if (last_reference)
    {
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG;
        mep_hw_profiles_write_data->mep_profile_index = mep_db_entry->mep_profile;
        sal_memset(&mep_hw_profiles_write_data->mep_profile, 0, sizeof(dnx_oam_mep_profile_t));
    }

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_eth_oam_profiles_free
                        (unit, mep_db_entry, mep_hw_profiles_write_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mpls_profiles_free(unit, mep_db_entry, mep_hw_profiles_write_data));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_ccm_endpoint_mep_db_create(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_delete_data)
{

    int prev_ccm_period = 0;
    uint8 ccm_rx_without_tx = 0;
    uint8 slm_enabled = 0;
    uint8 is_dual_ended_lm = 0;
    int profile_id;
    uint8 first_reference;
    dnx_oam_itmh_priority_t itmh_priority;
    dnx_algo_gpm_gport_phy_info_t *tx_gport_data = NULL;
    dnx_oam_oamp_mep_db_args_t *group_entry_info = NULL;
    dnx_bfd_oamp_endpoint_t *bfd_entry_values = NULL;
    bcm_vlan_port_t vlan_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_oam_mep_profile_t mep_profile;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int counter = 0;
    int num_short_entries_in_bank, entry_bank, extra_data_header_bank;
    uint32 mep_pe_profile;
    int system_headers_mode, index;
    uint32 udh_header_size;
    uint32 sys_port;
    dnx_oam_oamp_trap_tcam_entry_t tcam_data;
    uint8 last_reference;
    uint8 is_1dm_enabled = 0, is_mpls_lm_dm_without_bfd_endpoint = 0;
    uint32 mpls_tp_mdl_ignore = 0;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

    
    bcm_vlan_port_t_init(&vlan_port);
    SHR_ALLOC_SET_ZERO(tx_gport_data, sizeof(dnx_algo_gpm_gport_phy_info_t),
                       "Structure for reading data about tx_gport", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(group_entry_info, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Structure for reading data about group", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(&mep_profile, 0, sizeof(dnx_oam_mep_profile_t));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_without_bfd_endpoint
                        (unit, endpoint_info->id, &is_mpls_lm_dm_without_bfd_endpoint));
    }

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) && !is_mpls_lm_dm_without_bfd_endpoint)
    {
        
        SHR_ALLOC_SET_ZERO(bfd_entry_values, sizeof(dnx_bfd_oamp_endpoint_t),
                           "Structure for reading data about BFD EP", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_get(unit, (uint16) endpoint_info->id, bfd_entry_values));

        if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
        {
            
            SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, bfd_entry_values->mep_profile, &counter,
                                                                         &mep_profile));

            
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_mep_profile.free_single(unit, (int) bfd_entry_values->mep_profile, &last_reference));
        }
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        

        if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
        }
        else
        {
            
            mep_db_entry->mep_type = 0;
        }
        
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, (uint16) endpoint_info->id,
                                                         DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info),
                                                         mep_db_entry));

        
        SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, mep_db_entry->mep_profile, &counter,
                                                                     &mep_profile));

        
        slm_enabled = mep_profile.slm_lm;

        
        is_dual_ended_lm = mep_profile.piggy_back_lm;

        
        if (mep_profile.dm_measurement_type == DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT4)
        {
            is_1dm_enabled = TRUE;
        }

        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mep_db_profiles_free
                        (unit, mep_db_entry, mep_hw_profiles_delete_data));

        
        mep_db_entry->flags = DNX_OAMP_OAM_CCM_MEP_UPDATE;
    }

   
    mep_db_entry->flags |= ((endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeLmDmOffloadedEntry) ?
                            DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED :
                            ((endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeShortEntry) ?
                             DNX_OAMP_OAM_CCM_MEP_Q_ENTRY : 0));

    mep_db_entry->flags |= (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) ? DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET : 0;

    
    if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_48B_MAID;
    }

    
    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
    {
        mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION;
        mep_db_entry->vsi = endpoint_info->vpn;
    }

    
    if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info))
    {
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))
            {
                mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_SIGNAL_DEGRADATION_ENABLE;
                mep_db_entry->signal_degradation_tx = endpoint_info->tx_signal;
            }
            else
            {
                mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_LMDM_FLEXIBLE_DA;
            }
        }
    }

   
    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type
                    (unit, endpoint_info->type, &mep_db_entry->mep_type));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        if (is_mpls_lm_dm_without_bfd_endpoint)
        {
            mep_db_entry->ach_sel = 0;
        }
        else
        {
            mep_db_entry->ach_sel = bfd_entry_values->ach_sel;
        }
        
        if (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp)
        {
            mep_db_entry->flags |= DNX_OAMP_OAM_RF6374_GAL;
        }
        mep_db_entry->flags |= DNX_OAMP_OAM_RF6374_ACH;
        mep_db_entry->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
    }

    
    mep_db_entry->server_destination = 0;
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
    {
        
        
        itmh_priority.tc = (endpoint_info->int_pri & 0x1F) >> 2;
        itmh_priority.dp = (endpoint_info->int_pri & 0x3);
        
        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.allocate_single
                        (unit, 0, &itmh_priority, NULL, &profile_id, &first_reference));
        mep_db_entry->itmh_tc_dp_profile = profile_id;
        if (first_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG;

            mep_hw_profiles_write_data->itmh_profile = profile_id;
            mep_hw_profiles_write_data->itmh_priority.dp = itmh_priority.dp;
            mep_hw_profiles_write_data->itmh_priority.tc = itmh_priority.tc;
        }

        if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {
            
            SHR_IF_ERR_EXIT(dnx_oam_system_port_profile_sw_update(unit, endpoint_info, mep_hw_profiles_write_data));
            mep_db_entry->unified_port_access.port_profile = mep_hw_profiles_write_data->pp_port_profile;

            
            if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) && udh_header_size != 0)
            {
                if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48, &mep_pe_profile));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP, &mep_pe_profile));
                }
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
        }
        else
        {
            
            
            if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->intf_id))
            {
                mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC;
                mep_db_entry->fec_id_or_glob_out_lif.fec_id = BCM_L3_ITF_VAL_GET(endpoint_info->intf_id);
                mep_db_entry->dest_sys_port_agr = BCM_GPORT_INVALID;
            }
            else
            {
                
                if (BCM_L3_ITF_TYPE_IS_LIF(endpoint_info->intf_id))
                {
                    mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = BCM_L3_ITF_VAL_GET(endpoint_info->intf_id);
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, endpoint_info->tx_gport,
                                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                                                                    tx_gport_data));

                    mep_db_entry->dest_sys_port_agr = tx_gport_data->sys_port;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error: provided interface is not a valid FEC nor an out-LIF.\n");
                }
            }
            
            if ((dnx_data_oam.property.oam_injected_over_lsp_cnt_get(unit) == 1)
                && (endpoint_info->type == bcmOAMEndpointTypeBHHMPLS)
                && (endpoint_info->mpls_out_gport == endpoint_info->intf_id))
            {
                
                mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;

                if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, endpoint_info->mpls_out_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF,
                                     &gport_hw_resources));
                    mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = gport_hw_resources.global_out_lif;
                }
            }
        }
    }
    else
    {
        
        if (IS_OAM_SERVER(endpoint_info))
        {
            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.pp_port = endpoint_info->gport;
            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.port_core = 0;
            if (endpoint_info->tx_gport != BCM_GPORT_INVALID)
            {
                sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(endpoint_info->tx_gport);
                
                if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_DESTINATION_JR1,
                                                                         DBAL_FIELD_PORT_ID, &sys_port,
                                                                         &mep_db_entry->server_destination));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_DESTINATION,
                                                                         DBAL_FIELD_PORT_ID, &sys_port,
                                                                         &mep_db_entry->server_destination));
                }
            }
        }
        else
        {
            
            vlan_port.vlan_port_id = endpoint_info->gport;
            SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

            
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, vlan_port.port,
                                                            DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                            &gport_info));

            
            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.pp_port = gport_info.internal_port_pp_info.pp_port[0];
            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.port_core =
                gport_info.internal_port_pp_info.core_id[0];
        }

    }

    
    if ((!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) && !slm_enabled)
    {

        if (!((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN)) &&
              (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)))
        {
            
            mep_db_entry->counter_interface = endpoint_info->lm_counter_if + 1;
            mep_db_entry->counter_ptr = endpoint_info->lm_counter_base_id;
        }
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, endpoint_info->gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF, &gport_hw_resources));
        mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = gport_hw_resources.global_out_lif;
    }

    

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_eth_oam_init
                        (unit, endpoint_info, mep_db_entry, mep_hw_profiles_write_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mpls_oam_init
                        (unit, endpoint_info, mep_db_entry, mep_hw_profiles_write_data));
    }

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
        {
            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period
                            (unit, mep_db_entry->ccm_interval, &prev_ccm_period));

            if (endpoint_info->ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED
                && prev_ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)
            {
                ccm_rx_without_tx = 1;
            }
        }

        if (!ccm_rx_without_tx)
        {
            
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval
                            (unit, endpoint_info->ccm_period, &mep_db_entry->ccm_interval));
        }

        
        DNX_OAM_SET_RDI_GEN_METHOD_FIELD_ON_MEP_PROFILE(mep_profile.rdi_gen_method,
                                                        (endpoint_info->flags2 &
                                                         BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE),
                                                        (endpoint_info->flags2 &
                                                         BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE));
    }
    

    mep_profile.opcode_bit_map = 0xff;
    
    if ((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS))
        || (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS)))
    {
        dnx_oam_oamp_statistics_enable(unit, endpoint_info->flags2, &mep_profile);
    }
    
    if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        mep_profile.maid_check_dis = TRUE;
    }
    
    SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, &mpls_tp_mdl_ignore));
    if (mpls_tp_mdl_ignore)
    {
        if (endpoint_info->type == bcmOAMEndpointTypeBhhSection ||
            endpoint_info->type == bcmOAMEndpointTypeBHHMPLS || endpoint_info->type == bcmOAMEndpointTypeBHHPwe)
        {
            mep_profile.mdl_check_dis = TRUE;
        }
    }
    
    mep_profile.lmm_offset =
        dnx_oam_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 0  , 0  ,
                                 (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                 _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                  BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));
    mep_profile.lmr_offset =
        dnx_oam_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 0  , 1  ,
                                 (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                 _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                  BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));
    mep_profile.dmm_offset =
        dnx_oam_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 1  , 0  ,
                                 (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                 _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                  BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));
    mep_profile.dmr_offset =
        dnx_oam_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 1  , 1  ,
                                 (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                 _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                  BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit,
                                                     endpoint_info->id, 0,
                                                     (endpoint_info->endpoint_memory_type ==
                                                      bcmOamEndpointMemoryTypeShortEntry), mep_db_entry->ccm_interval,
                                                     ccm_rx_without_tx, OAMP_MEP_TX_OPCODE_CCM_BFD, &mep_profile));
    }
    else
    {
        
        if (is_mpls_lm_dm_without_bfd_endpoint)
        {
            mep_profile.ccm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
        }
    }

    
    mep_profile.cluster_id = 0xFF;

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.allocate_single
                    (unit, 0, &mep_profile, NULL, &profile_id, &first_reference));
    if (first_reference)
    {
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG;
        mep_hw_profiles_write_data->mep_profile_index = profile_id;
        sal_memcpy(&(mep_hw_profiles_write_data->mep_profile), &mep_profile, sizeof(dnx_oam_mep_profile_t));
    }

    mep_db_entry->mep_profile = profile_id;

    

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        mep_db_entry->session_id = endpoint_info->session_id;
        mep_db_entry->timestamp_format = endpoint_info->timestamp_format;
    }

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        
        mep_db_entry->mdl = endpoint_info->level;
        
        mep_db_entry->mep_id = endpoint_info->name;

        if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) &&
            !DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
        {
            
            SHR_IF_ERR_EXIT(dnx_oam_update_accelarated_endpoint_info_with_group_config
                            (unit, group_info->name, mep_db_entry));
        }

        
        if (endpoint_info->port_state != 0)
        {
            
            mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN;
            
            mep_db_entry->flags |=
                ((endpoint_info->port_state) == BCM_OAM_PORT_TLV_UP) ? DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL : 0;
        }

        
        mep_db_entry->interface_status_tlv_code = endpoint_info->interface_state;

        
        
        if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
        {
            
            if (!((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
                  (DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) && (udh_header_size != 0)))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }

            if (endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeSelfContained)
            {
                num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
                    dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
                entry_bank = endpoint_info->id / num_short_entries_in_bank;
                extra_data_header_bank = group_info->group_name_index / num_short_entries_in_bank;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, group_info->group_name_index, group_entry_info));
                if ((entry_bank >= extra_data_header_bank) &&
                    (entry_bank < extra_data_header_bank + group_entry_info->exclusive.extra_data.extra_data_len))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error: for self contained accelerated OAM endpoint with 48B MAID, all extra data \n"
                                 "entries must not be in the same bank as the main endpoint entry.\n");
                }

                
                mep_db_entry->extra_data_header = OAM_ID_TO_MEP_DB_ENTRY(group_info->group_name_index);

                if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,
                                     &mep_pe_profile));
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                }

                if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info))
                {
                    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))
                    {
                        SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                        (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48,
                                         &mep_pe_profile));
                        mep_db_entry->extra_data.flags =
                            DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_SD | DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE;
                        mep_db_entry->extra_data.opcodes_to_prepend = DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
                        mep_db_entry->extra_data.signal_degradation_rx = endpoint_info->rx_signal;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                        (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48,
                                         &mep_pe_profile));
                        if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_ENABLE(endpoint_info, sw_endpoint_info))
                        {
                            
                            mep_db_entry->extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC;
                            mep_db_entry->extra_data.opcodes_to_prepend =
                                DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM | DNX_OAMP_EXTRA_DATA_TO_PREPEND_LMM |
                                DNX_OAMP_EXTRA_DATA_TO_PREPEND_DMM;
                            sal_memset(mep_db_entry->extra_data.dmac, 0, sizeof(bcm_mac_t));
                        }
                    }
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                    mep_db_entry->extra_data_header = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
                    mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
                    sal_memcpy(mep_db_entry->extra_data.maid48, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);
                    mep_db_entry->extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID48;

                }
                else if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_DISABLE(endpoint_info, sw_endpoint_info))
                {
                    
                    mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info->extra_data_index);
                    mep_db_entry->extra_data.flags = 0;
                    mep_db_entry->extra_data.length = sw_endpoint_info->extra_data_length;
                }

                if (is_1dm_enabled)
                {
                    
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48, &mep_pe_profile));
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                }
            }
            else
            {
                mep_db_entry->extra_data_header = group_info->id;
            }
            if (IS_OAM_SERVER(endpoint_info) && DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
            {
                
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
        }
        
        else
        {
            if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }

            if (IS_OAM_SERVER(endpoint_info) && DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
            {
                
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
            if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info))
            {
                if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))
                {
                    
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT, &mep_pe_profile));
                    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
                    {
                        mep_db_entry->extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE;
                    }
                    mep_db_entry->extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_SD;
                    mep_db_entry->extra_data.opcodes_to_prepend = DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;

                    mep_db_entry->extra_data.signal_degradation_rx = endpoint_info->rx_signal;
                }
                else
                {
                    
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA, &mep_pe_profile));
                    if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_ENABLE(endpoint_info, sw_endpoint_info))
                    {
                        
                        mep_db_entry->extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC;
                        mep_db_entry->extra_data.opcodes_to_prepend =
                            DNX_OAMP_EXTRA_DATA_TO_PREPEND_LMM | DNX_OAMP_EXTRA_DATA_TO_PREPEND_DMM;
                        sal_memset(mep_db_entry->extra_data.dmac, 0, sizeof(bcm_mac_t));
                    }
                }
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
                mep_db_entry->extra_data_header = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
            }
            else if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_DISABLE(endpoint_info, sw_endpoint_info))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                mep_db_entry->extra_data_header = endpoint_info->extra_data_index;
                
                mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info->extra_data_index);
                mep_db_entry->extra_data.flags = 0;
                mep_db_entry->extra_data.length = sw_endpoint_info->extra_data_length;
            }

            if (is_1dm_enabled)
            {
                
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
        }
    }
    
    tcam_data.trap_code = BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport);

    if (IS_OAM_SERVER(endpoint_info) &&
        (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) ||
         (mep_hw_profiles_delete_data->tcam_entry.trap_code != tcam_data.trap_code)))
    {
        tcam_data.mep_type = mep_db_entry->mep_type;
        SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single
                        (unit, 0, &tcam_data, NULL, &index, &first_reference));
        mep_hw_profiles_write_data->tcam_index = index;
        if (first_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_TRAP_TCAM_FLAG;
            mep_hw_profiles_write_data->tcam_entry.trap_code = tcam_data.trap_code;
            mep_hw_profiles_write_data->tcam_entry.mep_type = tcam_data.mep_type;
        }

        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
        {
            SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.free_single(unit,
                                                                        mep_hw_profiles_delete_data->tcam_index,
                                                                        &last_reference));
            if (last_reference)
            {
                mep_hw_profiles_delete_data->flags |= DNX_OAM_OAMP_PROFILE_TRAP_TCAM_FLAG;
            }
        }
    }

exit:
    SHR_FREE(group_entry_info);
    SHR_FREE(tx_gport_data);
    SHR_FREE(bfd_entry_values);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mac_da_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t msb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_msb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);

    if (profile >= nof_da_mac_msb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_MSB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, profile);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DA_MSB, INST_SINGLE, msb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mac_da_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t msb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_msb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);

    if (profile >= nof_da_mac_msb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_MSB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, profile);

    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_DA_MSB, INST_SINGLE, msb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mac_da_lsb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t lsb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_lsb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);
    if (profile >= nof_da_mac_lsb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_LSB_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, profile);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DA_LSB, INST_SINGLE, lsb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_mac_da_lsb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t lsb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_lsb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);

    if (profile >= nof_da_mac_lsb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_LSB_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, profile);

    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_DA_LSB, INST_SINGLE, lsb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_short_entry_type_check_mep_db(
    int unit,
    uint16 oam_id,
    dbal_enum_value_field_oamp_mep_type_e req_mep_type,
    int *sub_index,
    uint8 *is_leader_alloc,
    uint8 *is_leader_type_match)
{
    dnx_oam_oamp_mep_db_args_t *leader_info = NULL;
    uint32 leader_endpoint_id;
    int nof_mep_db_short_entries;
    SHR_FUNC_INIT_VARS(unit);

    *is_leader_type_match = TRUE;

    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    *sub_index = (oam_id % nof_mep_db_short_entries);
    if (*sub_index != 0)
    {
        
        SHR_ALLOC_SET_ZERO(leader_info, sizeof(dnx_oam_oamp_mep_db_args_t),
                           "Short entry read for MEP-type comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        leader_endpoint_id = oam_id - *sub_index;
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated(unit, leader_endpoint_id, is_leader_alloc));
        if (*is_leader_alloc == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, leader_endpoint_id, leader_info));
            if (req_mep_type == leader_info->mep_type)
            {
                *is_leader_type_match = TRUE;
            }
            else
            {
                *is_leader_type_match = FALSE;
            }
        }
    }

exit:
    SHR_FREE(leader_info);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_short_entry_type_verify(
    int unit,
    uint16 oam_id,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    int sub_index;
    dbal_enum_value_field_oamp_mep_type_e req_mep_type;
    uint8 is_leader_alloc, is_leader_type_match;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(unit, endpoint_info->type, &req_mep_type));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_short_entry_type_check_mep_db
                    (unit, oam_id, req_mep_type, &sub_index, &is_leader_alloc, &is_leader_type_match));

    if (sub_index != 0)
    {
        if (is_leader_alloc == FALSE)
        {
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index can only "
                         "be added after the zero sub-index of the same full entry was "
                         "added, so you must first add entry 0x%08X before you can add entry "
                         "0x%08X", endpoint_info->id - sub_index, endpoint_info->id);
        }

        if (is_leader_type_match == FALSE)
        {
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index must have "
                         "the same MEP-type as the zero sub-index of the same full entry, "
                         "so entry 0x%08X must have the same MEP type as entry 0x%08X",
                         endpoint_info->id, endpoint_info->id - sub_index);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_extra_data_length_get(
    int unit,
    int index,
    uint32 *length)
{
    uint32 entry_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE, length));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_extra_data_get(
    int unit,
    int index,
    uint8 *data,
    uint16 *crc_1,
    uint16 *crc_2)
{
    uint32 entry_id, next_bank_offset;
    int i;
    uint32 entry_handle_id, nof_entries, check_crc_value_1, check_crc_value_2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    uint8 *tmp_data = NULL;

    SHR_ALLOC_SET_ZERO(tmp_data, DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY / UTILEX_NOF_BITS_IN_BYTE,
                       "Array for over 32-bit dbal fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    
    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE, tmp_data));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE, &nof_entries));
    if (crc_1 != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_1, INST_SINGLE, &check_crc_value_1));
        *crc_1 = (uint16) check_crc_value_1;
    }
    if (crc_2 != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_2, INST_SINGLE, &check_crc_value_2));
        *crc_2 = (uint16) check_crc_value_2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_MEP_DB, entry_handle_id));

    dnx_oamp_copy_bit_array(data, 0, tmp_data, 0, DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY, TRUE);

    

    for (i = 1; i != nof_entries; ++i)
    {
        int bits_to_copy = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY, start_from = 0;

        
        entry_id += next_bank_offset;

        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

        
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE, tmp_data));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_MEP_DB, entry_handle_id));

        dnx_oamp_copy_bit_array(data,
                                DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                                DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i - 1), tmp_data, start_from, bits_to_copy,
                                TRUE);
    }

exit:
    SHR_FREE(tmp_data);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_lm_with_priority_session_map_get(
    int unit,
    int oam_id,
    uint8 priority,
    uint32 *session_oam_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, session_oam_id);
    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_lm_with_priority_session_map_clear(
    int unit,
    int oam_id,
    uint8 priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_extra_data_set(
    int unit,
    int index,
    uint32 nof_data_bits,
    uint8 *data,
    int opcode,
    uint16 calculated_crc_1,
    uint16 calculated_crc_2,
    uint8 is_update)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    dnx_oamp_mep_db_memory_type_t memory_type;
    uint32 entry_id, next_bank_offset, max_nof_endpoint_id;
    int i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    
    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    

    if (!is_update)
    {
        memory_type.is_2byte_maid = FALSE;
        memory_type.is_extra_pool = (entry_id >= max_nof_endpoint_id);
        memory_type.is_full = TRUE;
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &entry_id));
    }

    if (is_update)
    {
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR;
    oamp_params->exclusive.extra_data.extra_data_len = (nof_data_bits == 0) ? 1 :
        ((nof_data_bits - 1) / DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + 1);
    oamp_params->exclusive.extra_data.opcode_bmp = opcode;
    oamp_params->exclusive.extra_data.crc_val1 = calculated_crc_1;
    oamp_params->exclusive.extra_data.crc_val2 = calculated_crc_2;
    dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, 0, data, 0,
                            DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY, FALSE);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_id, oamp_params));

    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD;

    

    for (i = 1; i != oamp_params->exclusive.extra_data.extra_data_len; ++i)
    {
        int bits_left = 0, bits_to_copy = 0, start_from = 0;
        entry_id += next_bank_offset;
        
        if (!is_update)
        {
            memory_type.is_extra_pool = (entry_id >= max_nof_endpoint_id);
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &entry_id));
        }
        bits_left =
            nof_data_bits - DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY - DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i -
                                                                                                                  1);
        if (bits_left < DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY)
        {
            bits_to_copy = bits_left;
            start_from = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY - bits_left;
            
            sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                       sizeof(oamp_params->exclusive.extra_data.data_segment));
        }
        else
        {
            bits_to_copy = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY;
        }
        dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment,
                                start_from, data,
                                DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                                DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i - 1), bits_to_copy, FALSE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_id, oamp_params));

    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_48b_maid_set(
    int unit,
    bcm_oam_group_info_t * group_info,
    uint16 calculated_crc)
{
    uint8 *extra_header_data = NULL;
    uint32 nof_data_bits = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(extra_header_data, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE))
    {
        nof_data_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE))
    {
        nof_data_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
    {
        nof_data_bits = DNX_OAM_OAMP_NOF_MAID_BITS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags indicate no extra data for MAID storation");
    }

    sal_memcpy(extra_header_data, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);

    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_set
                    (unit, OAM_ID_TO_MEP_DB_ENTRY(group_info->group_name_index),
                     nof_data_bits, extra_header_data, DBAL_DEFINE_OAM_OPCODE_CCM,
                     calculated_crc, 0, _SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_REPLACE)));
exit:
    SHR_FREE(extra_header_data);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_mep_db_48b_maid_clear(
    int unit,
    uint32 header_entry_index)
{
    int i;
    uint32 next_bank_offset, current_entry, extra_data_len;
    SHR_FUNC_INIT_VARS(unit);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_length_get
                    (unit, OAM_ID_TO_MEP_DB_ENTRY(header_entry_index), &extra_data_len));

    current_entry = header_entry_index;
    for (i = 0; i < extra_data_len; i++)
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, current_entry));

        
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, current_entry));

        current_entry += next_bank_offset;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_scan_count_req_in_6_count_cycle(
    int unit,
    dbal_enum_value_field_ccm_interval_e new_rate,
    uint32 *scan_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (new_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX)
    {
        
        *scan_count = 0;
    }
    else if (new_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
    {
        
        *scan_count = 3;
    }
    else if (new_rate >= DBAL_ENUM_FVAL_CCM_INTERVAL_10MS && new_rate < DBAL_NOF_ENUM_CCM_INTERVAL_VALUES)
    {
        
        *scan_count = 1;
    }
    else
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid rate passed %d", new_rate);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_get_existing_phase_cnt_for_opcode(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    const dnx_oam_mep_profile_t * mep_profile_data,
    uint32 *phase_cnt)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            *phase_cnt = mep_profile_data->ccm_count;
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
            *phase_cnt = mep_profile_data->lmm_count;
            break;
        case OAMP_MEP_TX_OPCODE_DMM:
            *phase_cnt = mep_profile_data->dmm_count;
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
            *phase_cnt = mep_profile_data->opcode_0_count;
            break;
        case OAMP_MEP_TX_OPCODE_OP1:
            *phase_cnt = mep_profile_data->opcode_1_count;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid opcode passed %d", opcode);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_get_transmit_rate_for_opcode(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    const dnx_oam_mep_profile_t * mep_profile_data,
    dbal_enum_value_field_ccm_interval_e * ccm_interval)
{
    SHR_FUNC_INIT_VARS(unit);
    *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            
            *ccm_interval = ccm_rate;
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile_data->lmm_rate, ccm_interval));
            break;
        case OAMP_MEP_TX_OPCODE_DMM:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile_data->dmm_rate, ccm_interval));
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval
                            (unit, mep_profile_data->opcode_0_rate, ccm_interval));
            break;
        case OAMP_MEP_TX_OPCODE_OP1:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval
                            (unit, mep_profile_data->opcode_1_rate, ccm_interval));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid opcode passed %d", opcode);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_scan_count_verify_max_per_mep(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dnx_oam_oamp_tx_opcode opcode_i;
    uint32 total_scan_count = 0, scan_count;
    dbal_enum_value_field_ccm_interval_e rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (opcode_i = OAMP_MEP_TX_OPCODE_CCM_BFD; opcode_i < OAMP_MEP_TX_OPCODE_MAX_SUPPORTED; opcode_i++)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_get_transmit_rate_for_opcode(unit, opcode_i, ccm_rate, mep_profile_data, &rate));
        scan_count = 0;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_req_in_6_count_cycle(unit, rate, &scan_count));
        total_scan_count += scan_count;
    }

    if (total_scan_count > 6)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Unsupported number of packets requested to be transmitted from a single MEP");
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_scan_count_calc_verify(
    int unit,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dnx_oam_oamp_tx_opcode other_opcode;
    dbal_enum_value_field_ccm_interval_e opcode_rate = 0;
    dbal_enum_value_field_ccm_interval_e other_opcode_rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (is_short_entry_type)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_CCM_BFD)
        {
            
            SHR_EXIT();
        }
        else
        {
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Short entry endpoints support only CCM %d", opcode);
        }
    }

    
    if (opcode == OAMP_MEP_TX_OPCODE_OP0 || opcode == OAMP_MEP_TX_OPCODE_OP1)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_OP0)
        {
            other_opcode = OAMP_MEP_TX_OPCODE_OP1;
        }
        else
        {
            other_opcode = OAMP_MEP_TX_OPCODE_OP0;
        }

        SHR_IF_ERR_EXIT(dnx_oam_oamp_get_transmit_rate_for_opcode
                        (unit, opcode, 0  , mep_profile_data, &opcode_rate));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_get_transmit_rate_for_opcode
                        (unit, other_opcode, 0  , mep_profile_data, &other_opcode_rate));

        if ((opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX)
            && (other_opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "We do not support case of both opcode 0 and 1 rate being non-zero %d %d",
                         opcode_rate, other_opcode_rate);
        }

        if ((opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX) && (opcode_rate < DBAL_ENUM_FVAL_CCM_INTERVAL_1S))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "We do not support case of opcode 0 or 1 rate being less than 1s %d", opcode_rate);
        }
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_verify_max_per_mep(unit, opcode, ccm_rate, mep_profile_data));
exit:
    SHR_FUNC_EXIT;
}


static void
dnx_oam_oamp_scan_count_calc_ccm_bfd(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_short_entry_type,
    uint8 ccm_rx_without_tx,
    dnx_oam_mep_profile_t * mep_profile_data)
{

    if (ccm_rx_without_tx)
    {
        
        mep_profile_data->ccm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
    }
    else
    {
        if (is_short_entry_type)
        {
            
            mep_profile_data->ccm_count = 0;
        }
        else
        {
            
            mep_profile_data->ccm_count = (endpoint_id >> 2) & 1;
        }
    }

}


shr_error_e
dnx_oam_oamp_scan_count_calc_rfc_6374_lm_dm(
    int unit,
    uint32 tx_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data,
    uint8 is_mpls_lm_dm_without_bfd_endpoint)
{
    dnx_oam_oamp_tx_opcode other_opcode;
    uint32 my_cnt, other_cnt;

    SHR_FUNC_INIT_VARS(unit);

    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_DMM;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_LMM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_existing_phase_cnt_for_opcode(unit, opcode, mep_profile_data, &my_cnt));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_existing_phase_cnt_for_opcode(unit, other_opcode, mep_profile_data, &other_cnt));

    if (tx_rate != 0)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_LMM)
        {
            my_cnt = 0;
        }
        else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
        {
            my_cnt = 1;
        }
    }
    else
    {
        
        my_cnt = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
    }

    
    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        mep_profile_data->lmm_count = my_cnt;
        mep_profile_data->dmm_count = other_cnt;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        mep_profile_data->dmm_count = my_cnt;
        mep_profile_data->lmm_count = other_cnt;
    }

    
    if (is_mpls_lm_dm_without_bfd_endpoint)
    {
        mep_profile_data->ccm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_scan_count_calc_lm_dm(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_jumbo_tlv_dm,
    uint8 ccm_transmit_phase_is_odd,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dbal_enum_value_field_ccm_interval_e my_rate = 0, other_rate = 0;
    dnx_oam_oamp_tx_opcode other_opcode;
    uint32 my_cnt, other_cnt;
    int dmm_phase = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_DMM;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_LMM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
    }

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_transmit_rate_for_opcode
                    (unit, opcode, 0  , mep_profile_data, &my_rate));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_transmit_rate_for_opcode
                    (unit, other_opcode, 0  , mep_profile_data, &other_rate));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_existing_phase_cnt_for_opcode(unit, opcode, mep_profile_data, &my_cnt));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_existing_phase_cnt_for_opcode(unit, other_opcode, mep_profile_data, &other_cnt));

    
    if (is_jumbo_tlv_dm)
    {
        
        dmm_phase =
            (((endpoint_id / dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit)) / JUMBO_TLV_ENTRIES_STEPS) % 8);
        mep_profile_data->dmm_count =
            (dmm_phase ? ((2 * dmm_phase - 2) % 14) : 14) + (mep_profile_data->ccm_count == 0);

        if (ccm_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
        {
            
            if (opcode == OAMP_MEP_TX_OPCODE_LMM)
            {
                mep_profile_data->lmm_count = (ccm_transmit_phase_is_odd) ? 3 : 2;
            }
        }
        else
        {
            
            if (opcode == OAMP_MEP_TX_OPCODE_LMM)
            {
                mep_profile_data->lmm_count = (ccm_transmit_phase_is_odd) ? 16 : 17;
            }
        }

        
        if (opcode == OAMP_MEP_TX_OPCODE_DMM)
        {
            mep_profile_data->dmm_count = my_cnt;
            mep_profile_data->lmm_count = other_cnt;
        }
        SHR_EXIT();
    }

    if (my_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
    {
        
        my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
        if (other_rate)
        {
            
            other_cnt = (ccm_transmit_phase_is_odd) ? 3 : 2;
        }
    }
    else
    {
        
        if (my_rate)    
        {
            if ((other_rate) && (my_rate < other_rate))
            {
                
                my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
                other_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
            else if ((other_rate) && (my_rate >= other_rate))
            {
                
                my_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
                other_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
            }
            else
            {
                
                my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
                other_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
        }
        
    }

    
    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        mep_profile_data->lmm_count = my_cnt;
        mep_profile_data->dmm_count = other_cnt;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        mep_profile_data->dmm_count = my_cnt;
        mep_profile_data->lmm_count = other_cnt;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_scan_count_calc_opcode_x(
    int unit,
    uint8 ccm_transmit_phase_is_odd,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dbal_enum_value_field_ccm_interval_e tx_rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    if ((opcode != OAMP_MEP_TX_OPCODE_OP0) && (opcode != OAMP_MEP_TX_OPCODE_OP1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_transmit_rate_for_opcode
                    (unit, opcode, 0  , mep_profile_data, &tx_rate));

    
    if (tx_rate)        
    {
        if (opcode == OAMP_MEP_TX_OPCODE_OP0)
        {
            mep_profile_data->opcode_0_count = (ccm_transmit_phase_is_odd) ? 4 : 5;
        }
        else
        {
            mep_profile_data->opcode_1_count = (ccm_transmit_phase_is_odd) ? 4 : 5;
        }
    }
    

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_oamp_scan_count_calc(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_jumbo_tlv_dm,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    uint8 ccm_rx_without_tx,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    uint8 ccm_transmit_phase_is_odd = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    if (opcode != OAMP_MEP_TX_OPCODE_CCM_BFD)
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_verify
                        (unit, is_short_entry_type, ccm_rate, opcode, mep_profile_data));
    }

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            dnx_oam_oamp_scan_count_calc_ccm_bfd(unit, endpoint_id, is_short_entry_type, ccm_rx_without_tx,
                                                 mep_profile_data);
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
        case OAMP_MEP_TX_OPCODE_DMM:
            DNX_OAM_IS_CCM_PHASE_ODD(ccm_transmit_phase_is_odd, is_short_entry_type, endpoint_id);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_lm_dm
                            (unit, endpoint_id, is_jumbo_tlv_dm, ccm_transmit_phase_is_odd, ccm_rate, opcode,
                             mep_profile_data));
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
        case OAMP_MEP_TX_OPCODE_OP1:
            DNX_OAM_IS_CCM_PHASE_ODD(ccm_transmit_phase_is_odd, is_short_entry_type, endpoint_id);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_opcode_x
                            (unit, ccm_transmit_phase_is_odd, opcode, mep_profile_data));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
