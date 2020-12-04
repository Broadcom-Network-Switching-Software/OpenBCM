/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_ingress_packet_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT


#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/QAX/qax_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>


int
  qax_ipq_init(
    SOC_SAND_IN  int unit
  ) 
{
    uint64 reg64;
    uint32 reg32;
    uint32 system_red = ARAD_IS_VOQ_MAPPING_INDIRECT(unit) ? 1 : 0;
    int core = 0;
    soc_dpp_config_arad_t *dpp_arad;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    SOCDNX_IF_ERR_EXIT(READ_IPS_IPS_GENERAL_CONFIGURATIONSr(unit, core, &reg64));
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISCARD_ALL_CGM_MSGf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISCARD_ALL_CRDTf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISABLE_STATUS_MSG_GENf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DIS_DEQ_CMDSf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, ENABLE_SYSTEM_REDf, system_red);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, UPDATE_MAX_QSZ_FROM_LOCALf, 1);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, SEND_FSM_FOR_BIGGER_MAX_QUEUE_SIZEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_IPS_GENERAL_CONFIGURATIONSr(unit, core, reg64));

    
    reg32 = 0;
    if (dpp_arad->init.drc_info.dram_num == 0) { 
        SOCDNX_IF_ERR_EXIT(READ_IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr(unit, &reg32));
        soc_reg_field_set(unit, IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr, &reg32, S_DEQ_COMMAND_CREDIT_THf, 0x30);
        soc_reg_field_set(unit, IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr, &reg32, S_LFSR_BIT_NUMf, 0x3);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr(unit, reg32));
    } else { 
        
        SOCDNX_IF_ERR_EXIT(READ_IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIGr(unit, &reg32));
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIGr, &reg32, MAX_SDQ_CMD_CRDT_0f, 0x80);
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIGr, &reg32, MAX_SDQ_CMD_CRDT_1f, 0x80);
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIGr, &reg32, MAX_SDQ_CMD_CRDT_2f, 0x20);
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIGr, &reg32, MAX_SDQ_CMD_CRDT_3f, 0x10);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIGr(unit, reg32));

        
        reg32 = 0;
        SOCDNX_IF_ERR_EXIT(READ_IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIG_2r(unit, &reg32));
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIG_2r, &reg32, MAX_SDQ_CMD_CRDT_4f, 0x80);
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIG_2r, &reg32, MAX_SDQ_CMD_CRDT_5f, 0x80);
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIG_2r, &reg32, MAX_SDQ_CMD_CRDT_6f, 0x80);
        soc_reg_field_set(unit, IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIG_2r, &reg32, MAX_SDQ_CMD_CRDT_7f, 0x80);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIG_2r(unit, reg32));

        reg32 = 0;
        SOCDNX_IF_ERR_EXIT(READ_IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr(unit, &reg32));
        soc_reg_field_set(unit, IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr, &reg32, S_DEQ_COMMAND_CREDIT_THf, 0x70);
        soc_reg_field_set(unit, IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr, &reg32, S_LFSR_BIT_NUMf, 0x3);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_S_DEQ_COMMAND_CREDIT_ADJUSTr(unit, reg32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_iqm_dynamic_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          entry_offset,
    SOC_SAND_OUT  ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  )
{
    soc_reg_above_64_val_t data_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SIZEm(unit, CGM_BLOCK(unit, core), entry_offset, &data_above_64));

    IQM_dynamic_tbl_data->pq_inst_que_size = soc_CGM_VOQ_SIZEm_field32_get(unit, &data_above_64, WORDS_SIZEf);
    IQM_dynamic_tbl_data->pq_avrg_szie = soc_CGM_VOQ_SIZEm_field32_get(unit, &data_above_64, AVRG_SIZEf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_explicit_mapping_mode_info_get(
   SOC_SAND_IN  int                            unit,
   SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   ) 
{

    uint32 fld_val;

    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(info);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, TAR_FLOW_BASE_QUEUEr, REG_PORT_ANY, 0, FLOW_BASE_QUEUE_0f, &fld_val));

    info->base_queue_id = fld_val;
    info->queue_id_add_not_decrement = TRUE;

exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_explicit_mapping_mode_info_set(
   SOC_SAND_IN  int                            unit,
   SOC_SAND_IN ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   ) 
{

    uint32 fld_val, res;
    int profile_id;

    soc_field_t field_id[] = {FLOW_BASE_QUEUE_0f, FLOW_BASE_QUEUE_1f, FLOW_BASE_QUEUE_2f, FLOW_BASE_QUEUE_3f};

    SOCDNX_INIT_FUNC_DEFS;
    
    res = arad_ipq_explicit_mapping_mode_info_verify(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    
    
   
    fld_val = info->base_queue_id;
    for (profile_id = 0; profile_id < ARAD_NOF_INGRESS_FLOW_TC_MAPPING_PROFILES; ++profile_id) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, TAR_FLOW_BASE_QUEUEr, REG_PORT_ANY,  0,
                                                           field_id[profile_id],  fld_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_default_invalid_queue_set(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_IN  uint32         queue_id,
   SOC_SAND_IN  int            enable) 
{
    SOCDNX_INIT_FUNC_DEFS;

    if (enable) {
        SOCDNX_IF_ERR_EXIT(WRITE_TAR_INVALID_DESTINATION_QUEUEr(unit, queue_id));
    } else { 
        SOCDNX_IF_ERR_EXIT(WRITE_TAR_INVALID_DESTINATION_QUEUEr(unit, ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit)));
    }
            
exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_default_invalid_queue_get(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_OUT uint32         *queue_id,
   SOC_SAND_OUT int            *enable) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_TAR_INVALID_DESTINATION_QUEUEr(unit, queue_id));
    
    if (*queue_id != ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit)) {
        *enable = 1;
    } else {
        *enable = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_ipq_traffic_class_multicast_priority_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              traffic_class,
    SOC_SAND_IN  uint8               enable
  )
{
    uint32 data = 0;
    uint32 field_val[1] = {0}; 
    SOCDNX_INIT_FUNC_DEFS;

    if (traffic_class > SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("TC %u is out of range\n"), traffic_class));
    }

    SOCDNX_IF_ERR_EXIT(READ_CGM_MC_TC_PRIORITY_MAPr(unit, SOC_CORE_ALL, &data));

    field_val[0] = soc_reg_field_get(unit, CGM_MC_TC_PRIORITY_MAPr, data, MC_TC_PRIORITY_MAPf);
    if (enable) {
        SHR_BITSET(field_val, traffic_class);
    } else {
        SHR_BITCLR(field_val, traffic_class);
    }
    soc_reg_field_set(unit, CGM_MC_TC_PRIORITY_MAPr, &data, MC_TC_PRIORITY_MAPf, field_val[0]);

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_MC_TC_PRIORITY_MAPr(unit, SOC_CORE_ALL, data));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_ipq_traffic_class_multicast_priority_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              traffic_class,
    SOC_SAND_OUT uint8               *enable
  )
{
    uint32 data = 0;
    uint32 field_val[1] = {0}; 
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(enable);

    if (traffic_class > SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("TC %u is out of range\n"), traffic_class));
    }

    SOCDNX_IF_ERR_EXIT(READ_CGM_MC_TC_PRIORITY_MAPr(unit, SOC_CORE_ALL, &data));

    field_val[0] = soc_reg_field_get(unit, CGM_MC_TC_PRIORITY_MAPr, data, MC_TC_PRIORITY_MAPf);
    *enable = SHR_BITGET(field_val, traffic_class) ? TRUE : FALSE;

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

