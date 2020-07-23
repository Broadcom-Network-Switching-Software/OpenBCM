/** \file pemladrv_pem_init_flow.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BCM_DNX_SUPPORT

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT_PP

#include <soc/dnx/drv.h>
#include <shared/utilex/utilex_str.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/utils/dnx_ire_packet_utils.h>

#ifndef ADAPTER_SERVER_MODE
typedef struct
{
    
    soc_reg_above_64_val_t ippf_reg_03a0;
    soc_reg_above_64_val_t ippf_reg_0310;
    soc_reg_above_64_val_t ippa_reg_0420;
    soc_reg_above_64_val_t ippb_reg_0300;
    soc_reg_above_64_val_t ippb_reg_0500;
    soc_reg_above_64_val_t ippc_reg_0330;
    soc_reg_above_64_val_t ippd_reg_0400;
    soc_reg_above_64_val_t etppa_reg_0340;
    soc_reg_above_64_val_t etppc_reg_0330;
    soc_reg_above_64_val_t etppc_reg_0320;
    soc_reg_above_64_val_t etppc_reg_0300;
    soc_reg_above_64_val_t etppb_reg_0520;
    soc_reg_above_64_val_t etppb_reg_0300;
    soc_reg_above_64_val_t etppb_reg_0320;
    soc_reg_above_64_val_t etppb_reg_0340;
    soc_reg_above_64_val_t etppb_reg_0360;
    soc_reg_above_64_val_t pem_map_config;
    soc_reg_above_64_val_t pem_alu_config;
    soc_reg_above_64_val_t pem_cam_config;
    soc_reg_above_64_val_t pem_prio_config;
    soc_reg_above_64_val_t pem_reg_0100;
    soc_reg_above_64_val_t ippe_pinfo_llr[256];
    soc_reg_above_64_val_t ippe_llrcs_action[128];
    soc_reg_above_64_val_t ippf_vtacs_action[128];
    soc_reg_above_64_val_t ippf_vtbcs_action[128];
    soc_reg_above_64_val_t ippf_vtccs_action[128];
    
    soc_reg_above_64_val_t ire_dynamic_configuration;
    soc_reg_above_64_val_t ire_register_interface_packet_data_bits_511_0;
    soc_reg_above_64_val_t ire_register_interface_packet_control;
} dnx_pemla_init_ire_packet_info_t;


static shr_error_e
dnx_pemla_init_vt_pem_packet_config_read(
    int unit,
    dnx_pemla_init_ire_packet_info_t * data)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(data, 0, sizeof(dnx_pemla_init_ire_packet_info_t));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPF_REG_03A0r, REG_PORT_ANY, 0, data->ippf_reg_03a0));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPF_REG_0310r, REG_PORT_ANY, 0, data->ippf_reg_0310));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPA_REG_0420r, REG_PORT_ANY, 0, data->ippa_reg_0420));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPB_REG_0300r, REG_PORT_ANY, 0, data->ippb_reg_0300));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPB_REG_0500r, REG_PORT_ANY, 0, data->ippb_reg_0500));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPC_REG_0330r, REG_PORT_ANY, 0, data->ippc_reg_0330));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPD_REG_0400r, REG_PORT_ANY, 0, data->ippd_reg_0400));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPA_REG_0340r, REG_PORT_ANY, 0, data->etppa_reg_0340));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPC_REG_0330r, REG_PORT_ANY, 0, data->etppc_reg_0330));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPC_REG_0320r, REG_PORT_ANY, 0, data->etppc_reg_0320));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPC_REG_0300r, REG_PORT_ANY, 0, data->etppc_reg_0300));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPB_REG_0520r, REG_PORT_ANY, 0, data->etppb_reg_0520));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPB_REG_0300r, REG_PORT_ANY, 0, data->etppb_reg_0300));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPB_REG_0320r, REG_PORT_ANY, 0, data->etppb_reg_0320));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPB_REG_0340r, REG_PORT_ANY, 0, data->etppb_reg_0340));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ETPPB_REG_0360r, REG_PORT_ANY, 0, data->etppb_reg_0360));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, PEM_MAP_CONFIGr, REG_PORT_ANY, 0, data->pem_map_config));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, PEM_ALU_CONFIGr, REG_PORT_ANY, 0, data->pem_alu_config));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, PEM_CAM_CONFIGr, REG_PORT_ANY, 0, data->pem_cam_config));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, PEM_PRIO_CONFIGr, REG_PORT_ANY, 0, data->pem_prio_config));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, PEM_REG_0100r, REG_PORT_ANY, 0, data->pem_reg_0100));

    for (ii = 0; ii < 256; ii++)
    {
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPE_PINFO_LLRm, 0, MEM_BLOCK_ANY, ii, data->ippe_pinfo_llr[ii]));
    }

    for (ii = 0; ii < 128; ii++)
    {
        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, IPPE_LLRCS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippe_llrcs_action[ii]));
        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, IPPF_VTACS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippf_vtacs_action[ii]));
        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, IPPF_VTBCS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippf_vtbcs_action[ii]));
        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, IPPF_VTCCS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippf_vtccs_action[ii]));
    }

    SHR_IF_ERR_EXIT(soc_reg_above_64_get
                    (unit, IRE_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, 0, data->ire_dynamic_configuration));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get
                    (unit, IRE_REGISTER_INTERFACE_PACKET_DATA_BITS_511_0r, REG_PORT_ANY, 0,
                     data->ire_register_interface_packet_data_bits_511_0));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get
                    (unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, 0,
                     data->ire_register_interface_packet_control));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_pemla_init_vt_pem_packet_config_write(
    int unit,
    dnx_pemla_init_ire_packet_info_t * data)
{
    int ii, jj;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPPF_REG_03A0r, REG_PORT_ANY, 0, data->ippf_reg_03a0));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPPF_REG_0310r, REG_PORT_ANY, 0, data->ippf_reg_0310));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPPA_REG_0420r, REG_PORT_ANY, 0, data->ippa_reg_0420));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPPB_REG_0300r, REG_PORT_ANY, 0, data->ippb_reg_0300));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPPB_REG_0500r, REG_PORT_ANY, 0, data->ippb_reg_0500));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPPC_REG_0330r, REG_PORT_ANY, 0, data->ippc_reg_0330));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPPD_REG_0400r, REG_PORT_ANY, 0, data->ippd_reg_0400));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPA_REG_0340r, REG_PORT_ANY, 0, data->etppa_reg_0340));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPC_REG_0330r, REG_PORT_ANY, 0, data->etppc_reg_0330));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPC_REG_0320r, REG_PORT_ANY, 0, data->etppc_reg_0320));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPC_REG_0300r, REG_PORT_ANY, 0, data->etppc_reg_0300));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPB_REG_0520r, REG_PORT_ANY, 0, data->etppb_reg_0520));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPB_REG_0300r, REG_PORT_ANY, 0, data->etppb_reg_0300));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPB_REG_0320r, REG_PORT_ANY, 0, data->etppb_reg_0320));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPB_REG_0340r, REG_PORT_ANY, 0, data->etppb_reg_0340));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ETPPB_REG_0360r, REG_PORT_ANY, 0, data->etppb_reg_0360));

    for (ii = 0; ii < 8; ii++)
    {
        for (jj = 0; jj < 4; jj++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, PEM_MAP_CONFIGr, ii, jj, data->pem_map_config));
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, PEM_ALU_CONFIGr, ii, jj, data->pem_alu_config));
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, PEM_CAM_CONFIGr, ii, jj, data->pem_cam_config));
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, PEM_PRIO_CONFIGr, ii, jj, data->pem_prio_config));
        }
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, PEM_REG_0100r, ii, 0, data->pem_reg_0100));
    }

    for (ii = 0; ii < 256; ii++)
    {
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPE_PINFO_LLRm, 0, MEM_BLOCK_ANY, ii, data->ippe_pinfo_llr[ii]));
    }

    for (ii = 0; ii < 128; ii++)
    {
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, IPPE_LLRCS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippe_llrcs_action[ii]));
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, IPPF_VTACS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippf_vtacs_action[ii]));
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, IPPF_VTBCS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippf_vtbcs_action[ii]));
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, IPPF_VTCCS_ACTIONm, 0, MEM_BLOCK_ANY, ii, data->ippf_vtccs_action[ii]));
    }

    SHR_IF_ERR_EXIT(soc_reg_above_64_set
                    (unit, IRE_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, 0, data->ire_dynamic_configuration));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set
                    (unit, IRE_REGISTER_INTERFACE_PACKET_DATA_BITS_511_0r, REG_PORT_ANY, 0,
                     data->ire_register_interface_packet_data_bits_511_0));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set
                    (unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, 0,
                     data->ire_register_interface_packet_control));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_pemla_init_vt_pem_packet_send(
    int unit)
{
    int ii, jj;
    dnx_ire_packet_control_info_t packet_info;

    
    uint32 packet_data[16] = { 0x12341234, 0x12341234, 0x12341234, 0x12341234,
        0x12341234, 0x12341234, 0x12341234, 0x12341234,
        0x12341234, 0x12341234, 0x12341234, 0x12341234,
        0x12341234, 0x12341234, 0x12341234, 0x12341234
    };

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&packet_info, 0, sizeof(dnx_ire_packet_control_info_t));

    
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPF_REG_03A0r, REG_PORT_ANY, FIELD_17_17f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPF_REG_03A0r, REG_PORT_ANY, FIELD_16_16f, 0));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPF_REG_0310r, REG_PORT_ANY, FIELD_17_17f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPF_REG_0310r, REG_PORT_ANY, FIELD_16_16f, 0));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPA_REG_0420r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPB_REG_0300r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPB_REG_0500r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPC_REG_0330r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IPPD_REG_0400r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPA_REG_0340r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPC_REG_0330r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPC_REG_0320r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPC_REG_0300r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPB_REG_0520r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPB_REG_0300r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPB_REG_0320r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPB_REG_0340r, REG_PORT_ANY, FIELD_16_16f, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, ETPPB_REG_0360r, REG_PORT_ANY, FIELD_16_16f, 1));

    
    for (ii = 0; ii < 8; ii++)
    {
        
        for (jj = 0; jj < 4; jj++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_MAP_CONFIGr, ii, jj, MAP_N_PE_BYPASSf, 1));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_MAP_CONFIGr, ii, jj, MAP_N_INPUT_SELECTf, 2));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_ALU_CONFIGr, ii, jj, ALU_N_PE_BYPASSf, 1));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_ALU_CONFIGr, ii, jj, ALU_N_INPUT_SELECTf, 2));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_CAM_CONFIGr, ii, jj, CAM_N_PE_BYPASSf, 1));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_CAM_CONFIGr, ii, jj, CAM_N_INPUT_SELECTf, 2));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_PRIO_CONFIGr, ii, jj, PRIO_N_PE_BYPASSf, 1));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PEM_PRIO_CONFIGr, ii, jj, PRIO_N_INPUT_SELECTf, 2));
        }
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, PEM_REG_0100r, ii, ROUTER_IN_0_INPUT_SELECT_Cf, 0));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, PEM_REG_0100r, ii, ROUTER_IN_3_INPUT_SELECT_Cf, 0));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, PEM_REG_0100r, ii, ROUTER_IN_2_INPUT_SELECT_Cf, 0));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, PEM_REG_0100r, ii, ROUTER_IN_1_INPUT_SELECT_Cf, 0));
    }

    for (ii = 0; ii < 256; ii++)
    {
        SHR_IF_ERR_EXIT(soc_mem_field32_modify(unit, IPPE_PINFO_LLRm, ii, CONTEXT_ENABLEf, 1));
        SHR_IF_ERR_EXIT(soc_mem_field32_modify(unit, IPPE_PINFO_LLRm, ii, CONTEXT_SELECTIONf, 0));
    }

    for (ii = 0; ii < 128; ii++)
    {
        SHR_IF_ERR_EXIT(soc_mem_field32_modify(unit, IPPE_LLRCS_ACTIONm, ii, LLRCS_ACTIONf, 0));
        SHR_IF_ERR_EXIT(soc_mem_field32_modify(unit, IPPF_VTACS_ACTIONm, ii, VTACS_ACTIONf, 0));
        SHR_IF_ERR_EXIT(soc_mem_field32_modify(unit, IPPF_VTBCS_ACTIONm, ii, VTBCS_ACTIONf, 0));
        SHR_IF_ERR_EXIT(soc_mem_field32_modify(unit, IPPF_VTCCS_ACTIONm, ii, VTCCS_ACTIONf, 0));
    }

    
    packet_info.packet_mode = DNX_IRE_PACKET_MODE_SINGLE_SHOT;
    packet_info.nof_packets = 1;
    packet_info.valid_bytes = 0x3f;
    sal_memcpy(packet_info.packet_data_0, packet_data,
               UTILEX_MIN(sizeof(packet_data), sizeof(packet_info.packet_data_0)));
    sal_memcpy(packet_info.packet_data_1, packet_data,
               UTILEX_MIN(sizeof(packet_data), sizeof(packet_info.packet_data_1)));
    SHR_IF_ERR_EXIT(dnx_ire_packet_init(unit, &packet_info));
    SHR_IF_ERR_EXIT(dnx_ire_packet_start(unit));
    SHR_IF_ERR_EXIT(dnx_ire_packet_end(unit));

exit:
    SHR_FUNC_EXIT;
}

#endif 

shr_error_e
dnx_init_pem_sequence_flow(
    int unit)
{
#ifndef ADAPTER_SERVER_MODE
    dnx_pemla_init_ire_packet_info_t *original_data_before_packet = NULL;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_debug.feature.feature_get(unit, dnx_data_debug_feature_pem_vt_init_fix))
    {
        SHR_ALLOC_SET_ZERO(original_data_before_packet, sizeof(dnx_pemla_init_ire_packet_info_t),
                           "original_data_before_packet", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_pemla_init_vt_pem_packet_config_read(unit, original_data_before_packet));
        SHR_IF_ERR_EXIT(dnx_pemla_init_vt_pem_packet_send(unit));
        SHR_IF_ERR_EXIT(dnx_pemla_init_vt_pem_packet_config_write(unit, original_data_before_packet));
    }
exit:
   SHR_FREE(original_data_before_packet);
   SHR_FUNC_EXIT;
#else
   return _SHR_E_NONE;
#endif
}

#endif 
