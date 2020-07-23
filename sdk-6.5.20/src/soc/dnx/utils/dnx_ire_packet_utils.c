/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <soc/dnx/utils/dnx_ire_packet_utils.h>
#include <soc/drv.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ire_packet_generator_access.h>
#include <include/soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>


shr_error_e
dnx_ire_packet_init(
    int unit,
    dnx_ire_packet_control_info_t * packet_info)
{
    uint32 valid_bytes = 0;
    uint8 is_init = 0;
    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ire_packet_generator_info.is_init(unit, &is_init));
    if (!is_init)
    {
        SHR_IF_ERR_EXIT(ire_packet_generator_info.init(unit));
    }
    valid_bytes = UTILEX_MIN(packet_info->valid_bytes, DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_BYTES);
    SHR_IF_ERR_EXIT(soc_reg_field32_modify
                    (unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_SEND_DATAf, 0));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IRE_PACKET_REGISTER_INTERFACE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOP, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EOP, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID_BYTES, INST_SINGLE, valid_bytes);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEND_MODE, INST_SINGLE, packet_info->packet_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID_BYTES, INST_SINGLE, valid_bytes);
    if (packet_info->packet_mode == DNX_IRE_PACKET_MODE_CONST_RATE)
    {
        uint32 clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
        uint32 max_clocks_per_packet = 0x3ff;
        uint32 max_packet_rate_khz = clock_khz;
        uint32 min_packet_rate_khz = clock_khz / max_clocks_per_packet;
        uint32 packet_khz = packet_info->pakcet_rate_khz;
        uint32 clocks_per_packet = clock_khz / packet_khz;

        if ((packet_khz < min_packet_rate_khz) || (packet_khz > max_packet_rate_khz))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid packet rate in KHz. Supported packet rate is %d-%d KHz\n",
                         min_packet_rate_khz, max_packet_rate_khz);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMER_CYCLES, INST_SINGLE, clocks_per_packet);
    }
    else if (packet_info->packet_mode == DNX_IRE_PACKET_MODE_SINGLE_SHOT)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_PACKETS, INST_SINGLE,
                                     packet_info->nof_packets);
    }

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, INST_SINGLE, packet_info->ptc_0);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_DATA_512, INST_SINGLE,
                                     packet_info->packet_data_0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, INST_SINGLE, packet_info->ptc_1);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_DATA_512, INST_SINGLE,
                                         packet_info->packet_data_1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_ire_packet_start(
    int unit)
{
    soc_reg_above_64_val_t data;
    uint32 datapath_enable_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IRE_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, 0, data));
    datapath_enable_val = soc_reg_field_get(unit, IRE_DYNAMIC_CONFIGURATIONr, data[0], ENABLE_DATA_PATHf);
    SHR_IF_ERR_EXIT(ire_packet_generator_info.ire_datapath_enable_state.set(unit, datapath_enable_val));

    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, ENABLE_DATA_PATHf, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify
                    (unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_SEND_DATAf, 1));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_ire_packet_end(
    int unit)
{
    uint32 datapath_orig_value = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ire_packet_generator_info.ire_datapath_enable_state.get(unit, &datapath_orig_value));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify
                    (unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_SEND_DATAf, 0));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify
                    (unit, IRE_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, ENABLE_DATA_PATHf, datapath_orig_value));
    SHR_IF_ERR_EXIT(ire_packet_generator_info.ire_datapath_enable_state.set(unit, 0));

exit:
    SHR_FUNC_EXIT;
}
