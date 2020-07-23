/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_mgmt.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/swstate/access/sw_state_access.h>
#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>



#define _SOC_JER_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT   (32)
#define _SOC_JER_ECI_PVT_MON_CONTROL_REG_RESET_BIT       (33)

#define _SOC_JER_ECI_PVT_MON_CONTROL_REG_RESET_BIT       (33)
#define DPP_SOC_PVT_MON_SIZE_MAX                         (4)
#define _SOC_JER_PVT_MON_NOF                             (SOC_IS_QAX(unit) ? (SOC_IS_QUX(unit) ? 1:2) : (SOC_IS_FLAIR(unit) ? 1:DPP_SOC_PVT_MON_SIZE_MAX))
#define _SOC_JER_PVT_FACTOR                              (49103)
#define _SOC_JER_PVT_BASE                                (41205000)
#define _SOC_FLAIR_PVT_FACTOR                              (49055)
#define _SOC_FLAIR_PVT_BASE                                (41335000)

#define JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP 16
#define JER_MGMT_NOF_CORES 2
#define JER_MGMT_NOF_PROCESSOR_IDS 18
#define JER_CR_VAL_BMP_NOF_BITS 16

jer_mgmt_dma_fifo_source_channels_t jer_mgmt_dma_fifo_source_channels_db[SOC_MAX_NUM_DEVICES];
int jer_mgmt_pvt_mon_peak[SOC_MAX_NUM_DEVICES][DPP_SOC_PVT_MON_SIZE_MAX];



int
  jer_mgmt_credit_worth_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              credit_worth
  )
{
  uint32
    fld_val = 0;
  soc_reg_t credit_value_reg;
  soc_field_t credit_value_lcl_field;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_credit_worth_verify(unit, credit_worth)) ;

  fld_val = credit_worth;
  
  credit_value_reg = SOC_IS_JERICHO_PLUS_A0(unit) ? IPST_CREDIT_CONFIG_1r : IPS_IPS_CREDIT_CONFIGr ;
  credit_value_lcl_field = CREDIT_VALUE_0f;
  SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, credit_value_reg, REG_PORT_ANY, credit_value_lcl_field, fld_val)) ;

exit:
  SOCDNX_FUNC_RETURN;
}

int
   jer_mgmt_credit_worth_get(
            SOC_SAND_IN  int                 unit,
            SOC_SAND_OUT uint32              *credit_worth
           )
{
  uint32
      reg_val,
      fld_val = 0;
  soc_reg_t credit_value_reg;
  soc_field_t credit_value_lcl_field;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(credit_worth);

  
  credit_value_reg = !SOC_IS_JERICHO_PLUS_A0(unit) ? IPS_IPS_CREDIT_CONFIGr : IPST_CREDIT_CONFIG_1r;
  credit_value_lcl_field = CREDIT_VALUE_0f ;
  
  SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, credit_value_reg, REG_PORT_ANY, 0, &reg_val)) ;
  fld_val = soc_reg_field_get(unit, credit_value_reg, reg_val, credit_value_lcl_field) ;
  *credit_worth = fld_val;
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  jer_mgmt_credit_worth_remote_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    credit_worth_remote
  )
{
    uint32 res, reg_val;
    uint16 nof_remote_faps_with_remote_credit_value;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, sw_state_access[unit].dpp.soc.jericho.tm.nof_remote_faps_with_remote_credit_value.get(unit, &nof_remote_faps_with_remote_credit_value));

    if (credit_worth_remote < SOC_TMC_CREDIT_SIZE_BYTES_MIN || credit_worth_remote > SOC_TMC_CREDIT_SIZE_BYTES_MAX) {
        LOG_ERROR(BSL_LS_SOC_MANAGEMENT, (BSL_META_U(unit, "Remote size %d is not between %u..%u") , credit_worth_remote, SOC_TMC_CREDIT_SIZE_BYTES_MIN, SOC_TMC_CREDIT_SIZE_BYTES_MAX));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
    } else {
        uint32 arg_local, arg_remote;
        jer_mgmt_credit_worth_remote_get(unit, &arg_remote); 
        jer_mgmt_credit_worth_get(unit, &arg_local); 
        if (credit_worth_remote != arg_remote) { 
            if (nof_remote_faps_with_remote_credit_value) { 
                if (credit_worth_remote != arg_local) {
                    LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                              (BSL_META_U(unit,"The Remote credit value is assigned to remote devices. To change the value you must first assign the local credit value to these devices.")));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 20, exit);
                } else { 
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, jer_mgmt_change_all_faps_credit_worth_unsafe(unit, SOC_TMC_FAP_CREDIT_VALUE_LOCAL));
                }
            } else {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IPS_IPS_CREDIT_CONFIGr(unit, &reg_val));
                soc_reg_field_set(unit, IPS_IPS_CREDIT_CONFIGr, &reg_val, CREDIT_VALUE_1f, credit_worth_remote);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, WRITE_IPS_IPS_CREDIT_CONFIGr(unit, reg_val));
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_credit_worth_remote_set()", unit, 0);
}


uint32
  jer_mgmt_credit_worth_remote_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint32    *credit_worth_remote
  )
{
    uint32 res, reg_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(credit_worth_remote);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IPS_IPS_CREDIT_CONFIGr(unit, &reg_val));
    *credit_worth_remote = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIGr, reg_val, CREDIT_VALUE_1f);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_credit_worth_remote_get()", unit, 0);
}



uint32
  jer_mgmt_module_to_credit_worth_map_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type 
  )
{
    uint32 data = 0, fap_bitmap[1], res, remote_bit;
    uint16 nof_remote_faps_with_remote_credit_value;
    soc_mem_t mem;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (fap_id > SOC_TMC_NOF_FAPS_IN_SYSTEM) {
        SOC_SAND_SET_ERROR_CODE(ARAD_FAP_FABRIC_ID_OUT_OF_RANGE_ERR, 10, exit);
    }
    
    if (SOC_IS_QUX(unit)) {
        SOC_EXIT;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, sw_state_access[unit].dpp.soc.jericho.tm.nof_remote_faps_with_remote_credit_value.get(unit, &nof_remote_faps_with_remote_credit_value));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IPS_CREDIT_CONFIGr(unit, SOC_CORE_ALL, &data)); 
    soc_reg_field_set(unit, IPS_CREDIT_CONFIGr, &data, CR_VAL_SEL_ENABLEf, TRUE);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IPS_CREDIT_CONFIGr(unit, SOC_CORE_ALL, data)); 

    data = 0;
    mem =  SOC_IS_JERICHO_PLUS_A0(unit) ? IPST_CRVSm : IPS_CRVSm;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_mem_read(unit, mem, MEM_BLOCK_ANY, fap_id / JER_CR_VAL_BMP_NOF_BITS, &data));
    *fap_bitmap = soc_mem_field32_get(unit, mem, &data, CR_VAL_BMPf);

    remote_bit = SHR_BITGET(fap_bitmap,(fap_id % JER_CR_VAL_BMP_NOF_BITS));
    if (credit_value_type == SOC_TMC_FAP_CREDIT_VALUE_LOCAL) {
        if (remote_bit != SOC_TMC_FAP_CREDIT_VALUE_LOCAL) {
            SHR_BITCLR(fap_bitmap,(fap_id % JER_CR_VAL_BMP_NOF_BITS));
            --nof_remote_faps_with_remote_credit_value;
        }
    } else if (credit_value_type == SOC_TMC_FAP_CREDIT_VALUE_REMOTE) {
        if (remote_bit != SOC_TMC_FAP_CREDIT_VALUE_REMOTE) {
            SHR_BITSET(fap_bitmap,(fap_id % JER_CR_VAL_BMP_NOF_BITS));
            ++nof_remote_faps_with_remote_credit_value;
        }
    } else {
        SOC_SAND_SET_ERROR_CODE(ARAD_CREDIT_TYPE_INVALID_ERR, 40, exit);
    }

    soc_mem_field32_set(unit, mem, &data, CR_VAL_BMPf, *fap_bitmap); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_mem_write(unit, mem, MEM_BLOCK_ANY, fap_id / JER_CR_VAL_BMP_NOF_BITS, &data));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, sw_state_access[unit].dpp.soc.jericho.tm.nof_remote_faps_with_remote_credit_value.set(unit, nof_remote_faps_with_remote_credit_value));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_module_to_credit_worth_map_set()", unit, 0);
}


uint32
  jer_mgmt_module_to_credit_worth_map_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type 
  )
{
    uint32 data = 0, fap_bitmap[1], res;
    soc_mem_t mem;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (fap_id > SOC_TMC_NOF_FAPS_IN_SYSTEM) {
        SOC_SAND_SET_ERROR_CODE(ARAD_FAP_FABRIC_ID_OUT_OF_RANGE_ERR, 30, exit);
    }
    
    if (SOC_IS_QUX(unit)) {
        *credit_value_type = 0;
        SOC_EXIT;
    }

    mem =  SOC_IS_JERICHO_PLUS_A0(unit) ? IPST_CRVSm : IPS_CRVSm;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_mem_read(unit, mem, MEM_BLOCK_ANY, fap_id / JER_CR_VAL_BMP_NOF_BITS, &data));
    *fap_bitmap = soc_mem_field32_get(unit, mem, &data, CR_VAL_BMPf);
    *credit_value_type = SHR_IS_BITSET(fap_bitmap, (fap_id % JER_CR_VAL_BMP_NOF_BITS));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_module_to_credit_worth_map_get()", unit, 0);

}



uint32
  jer_mgmt_change_all_faps_credit_worth_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint8     credit_value_to_use
  )
{
    uint32 res, reg_val;
    uint32 credit_worth_local = 0, credit_worth_remote = 0;
    uint32 data = 0;
    soc_mem_t mem;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    mem =  SOC_IS_JERICHO_PLUS_A0(unit) ? IPST_CRVSm : IPS_CRVSm;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IPS_IPS_CREDIT_CONFIGr(unit, &reg_val));
    credit_worth_local = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIGr, reg_val, CREDIT_VALUE_0f);
    credit_worth_remote = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIGr, reg_val, CREDIT_VALUE_1f);
    if (credit_worth_local != credit_worth_remote) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
    } else if (credit_value_to_use == SOC_TMC_FAP_CREDIT_VALUE_LOCAL) {
        soc_mem_field32_set(unit, mem, &data, CR_VAL_BMPf, 0); 
        res = arad_fill_table_with_entry(unit, mem, MEM_BLOCK_ANY, &data); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 115, exit);
    } else if (credit_value_to_use == SOC_TMC_FAP_CREDIT_VALUE_REMOTE) {
        soc_mem_field32_set(unit, mem, &data, CR_VAL_BMPf, 0xffff); 
        res = arad_fill_table_with_entry(unit, mem, MEM_BLOCK_ANY, &data); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);
    } else {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 125, exit);
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_change_all_faps_credit_worth_unsafe()", unit, credit_value_to_use);
}










uint32 jer_mgmt_system_fap_id_set(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  sys_fap_id
  )
{
    uint32 reg32, group_ctx, fap_id_plus_1 = sys_fap_id + 1;
    uint64 reg64;
    int is_mesh, offset, i,is_single_core;
    uint8 is_traffic_enabled=0;
    uint8 is_ctrl_cells_enabled=0;
    uint32  old_sys_fap_id=0;
    uint8 is_allocated;

    SHR_BITDCLNAME (dqcq_map, 32);
    SOCDNX_INIT_FUNC_DEFS;
    is_single_core = SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit);
    if (sys_fap_id >= ARAD_NOF_FAPS_IN_SYSTEM || (!is_single_core && (sys_fap_id % SOC_DPP_DEFS_GET(unit, nof_cores)))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("FAP ID %u is illegal, must be a multiple if %u and under %u."),
          sys_fap_id, SOC_DPP_DEFS_GET(unit, nof_cores), ARAD_NOF_FAPS_IN_SYSTEM));
    }
    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_enable_traffic_get,(unit, &is_traffic_enabled)));
    }
    SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_all_ctrl_cells_enable_get,(unit, &is_ctrl_cells_enabled)));
    if (is_traffic_enabled || is_ctrl_cells_enabled) {
        SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get,(unit,&old_sys_fap_id)));
        
        if (old_sys_fap_id != sys_fap_id) {
            LOG_WARN(BSL_LS_SOC_FABRIC, (BSL_META_U(unit,
                                        " Warning: fabric force should not be done when traffic is enabled.\nTo disable traffic, use bcm_stk_module_enable and bcm_fabric_control_set with bcmFabricControlCellsEnable parameter.\n")));
        }
    }
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, DEV_ID_0f, sys_fap_id);

    SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.lag.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.lag.alloc(unit));
    }
    SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.lag.my_modid.set(unit, sys_fap_id));

    if (!is_single_core) {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, DEV_ID_1f, fap_id_plus_1);
    } else {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, DEV_ID_1f, 0x7FF);
    }
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, FORCE_FABRICf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32));

    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass &&
        SOC_DPP_CONFIG(unit)->arad->init.fabric.is_128_in_system &&
        SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr,
          REG_PORT_ANY, 0, TDM_SOURCE_FAP_IDf,
          sys_fap_id + SOC_DPP_CONFIG(unit)->arad->tdm_source_fap_id_offset));
    }
    
    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH ||
               SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP);
    if (is_mesh) { 
        for (group_ctx = 0, i = sys_fap_id; i < sys_fap_id + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++group_ctx, ++i) {
            offset = (i % 16) * SOC_JER_FABRIC_GROUP_CTX_LENGTH; 
            if (offset < SOC_JER_FABRIC_STK_FAP_GROUP_SIZE) {
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, &reg64)); 
                *dqcq_map = soc_reg64_field32_get(unit,ECI_GLOBAL_DQCQ_MAP_CFG_1r, reg64, DEV_ID_TO_DQCQ_MAP_LSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
            } else {
                offset -= SOC_JER_FABRIC_STK_FAP_GROUP_SIZE;
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, &reg64)); 
                *dqcq_map = soc_reg64_field32_get(unit,ECI_GLOBAL_DQCQ_MAP_CFG_2r, reg64, DEV_ID_TO_DQCQ_MAP_MSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));
            }
        }
    } else { 
        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, sys_fap_id | (sys_fap_id << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MASKf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
        COMPILER_64_ZERO(reg64);
        if (!is_single_core) {
            soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, fap_id_plus_1 | (fap_id_plus_1 << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
            soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0);
        } else {
            soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, 0xFFFFFFFF | (0xFFFFFFFF << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
            soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));
    }
exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  jer_mgmt_system_fap_id_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_OUT uint32    *sys_fap_id
  )
{
  uint8 is_allocated;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(sys_fap_id);

  *sys_fap_id = 0;
  SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.lag.is_allocated(unit, &is_allocated));
  if(is_allocated) {
    
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.lag.my_modid.get(unit, sys_fap_id));
  }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_init_ctrl_cells_enable_set(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable,
    SOC_SAND_IN  uint32  flags

  )
{
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_FUNC_RETURN;
}


static int
   jer_mgmt_temp_pvt_raw_get(int unit,
                            soc_switch_temperature_monitor_t *temperature_array,
                            int *faulty_pvt_mon,
                            uint32 *faulty_pvt_mon_bitmap)
{
    int ii;
    uint32 reg32_val;
    uint64 rval64;
    int peak, curr, curr_rst;
    int tmp_peak;
    soc_reg_t temp_reg[] = {ECI_PVT_MON_A_THERMAL_DATAr, ECI_PVT_MON_B_THERMAL_DATAr, ECI_PVT_MON_C_THERMAL_DATAr, ECI_PVT_MON_D_THERMAL_DATAr};
    soc_field_t curr_field[] = {THERMAL_DATA_Af, THERMAL_DATA_Bf, THERMAL_DATA_Cf, THERMAL_DATA_Df};
    soc_field_t peak_field[] = {PEAK_THERMAL_DATA_Af, PEAK_THERMAL_DATA_Bf, PEAK_THERMAL_DATA_Cf, PEAK_THERMAL_DATA_Df};
    int faulty_mon_counter = 0;
    uint32 faulty_mon_bitmap = 0;
    int curr_total = 0, peak_total = 0;
    SOCDNX_INIT_FUNC_DEFS;

    for (ii = 0; ii < _SOC_JER_PVT_MON_NOF; ii++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, temp_reg[ii], REG_PORT_ANY, 0, &reg32_val));
        curr = soc_reg_field_get(unit, temp_reg[ii], reg32_val, curr_field[ii]);
        peak = soc_reg_field_get(unit, temp_reg[ii], reg32_val, peak_field[ii]);

        SOC_REG_RST_VAL_GET(unit, temp_reg[ii], rval64);
        curr_rst = soc_reg64_field32_get(unit, temp_reg[ii], rval64, curr_field[ii]);

        
        
        if ((curr == curr_rst) || (SOC_IS_JERICHO_PLUS_ONLY(unit) && ii == 2) ) {
            faulty_mon_counter++;
            faulty_mon_bitmap |= (1 << ii);
            continue;
        }

        temperature_array[ii].curr =  curr;
        temperature_array[ii].peak = peak;

        
        if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
            tmp_peak = jer_mgmt_pvt_mon_peak[unit][ii];
            if (curr < tmp_peak) {
                jer_mgmt_pvt_mon_peak[unit][ii] = curr; 
                temperature_array[ii].peak = curr;
            } else {
                temperature_array[ii].peak = tmp_peak;
            }
        }
        curr_total += temperature_array[ii].curr;
        peak_total += temperature_array[ii].peak;
    }

    if ((faulty_mon_counter != 0) && (faulty_mon_counter != _SOC_JER_PVT_MON_NOF)) {
        curr = curr_total/(_SOC_JER_PVT_MON_NOF - faulty_mon_counter);
        peak = peak_total/(_SOC_JER_PVT_MON_NOF - faulty_mon_counter);

        
        for (ii = 0; ii < _SOC_JER_PVT_MON_NOF; ii++) {
            if ((faulty_mon_bitmap & (1 << ii)) != 0) {
                temperature_array[ii].curr = curr;
                temperature_array[ii].peak = peak;

                
                if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
                    tmp_peak = jer_mgmt_pvt_mon_peak[unit][ii];
                    if (curr < tmp_peak) {
                        jer_mgmt_pvt_mon_peak[unit][ii] = curr; 
                        temperature_array[ii].peak = curr;
                    } else {
                        temperature_array[ii].peak = tmp_peak;
                    }
                }
            }
        }
    }

    *faulty_pvt_mon = faulty_mon_counter;
    *faulty_pvt_mon_bitmap = faulty_mon_bitmap;

exit:
    SOCDNX_FUNC_RETURN;
}


int
   jer_mgmt_temp_pvt_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int ii;
    int faulty_mon_counter = 0;
    uint32 faulty_mon_bitmap = 0;
    soc_switch_temperature_monitor_t tmp_temperature_array[DPP_SOC_PVT_MON_SIZE_MAX];
    int factor;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (temperature_max < _SOC_JER_PVT_MON_NOF)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Array size should be equal to or larger than %d.\n"), _SOC_JER_PVT_MON_NOF));
    }
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, ECI_PVT_MON_THERMAL_DATAr, REG_PORT_ANY, 0, &reg32_val));
        tmp_temperature_array[0].curr = soc_reg_field_get(unit, ECI_PVT_MON_THERMAL_DATAr, reg32_val, THERMAL_DATAf);
        tmp_temperature_array[0].peak = soc_reg_field_get(unit, ECI_PVT_MON_THERMAL_DATAr, reg32_val, PEAK_THERMAL_DATAf);
    } else {
        SOCDNX_IF_ERR_EXIT(jer_mgmt_temp_pvt_raw_get(unit, tmp_temperature_array, &faulty_mon_counter, &faulty_mon_bitmap));
    }
    for (ii = 0; ii < _SOC_JER_PVT_MON_NOF; ii++)
    {
        if (!SOC_IS_FLAIR(unit)) {
            
            temperature_array[ii].curr = (_SOC_JER_PVT_BASE - tmp_temperature_array[ii].curr* _SOC_JER_PVT_FACTOR) / 10000;
            
            temperature_array[ii].peak = (_SOC_JER_PVT_BASE - tmp_temperature_array[ii].peak* _SOC_JER_PVT_FACTOR) / 10000;
        } else {
            
            temperature_array[ii].curr = (_SOC_FLAIR_PVT_BASE - tmp_temperature_array[ii].curr* _SOC_FLAIR_PVT_FACTOR) / 10000;
            
            temperature_array[ii].peak = (_SOC_FLAIR_PVT_BASE - tmp_temperature_array[ii].peak* _SOC_FLAIR_PVT_FACTOR) / 10000;
        }

        if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
            if (soc->pvt_mon_correction_interval == 0) {
                
                temperature_array[ii].peak = -1;
            } else {
                jer_mgmt_pvt_mon_peak[unit][ii] = tmp_temperature_array[ii].curr; 
            }
        }
    }

    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        
        factor = soc_property_get(unit, spn_PVT_MON_CORRECTION_FACTOR, 0);
        temperature_array[2].curr += (factor*10);
        temperature_array[2].peak += (factor*10);
    }

    *temperature_count = _SOC_JER_PVT_MON_NOF;

exit:
    SOCDNX_FUNC_RETURN; 
}




int jer_mgmt_drv_pvt_monitor_enable(int unit)
{
    uint64 reg64_val;
    soc_reg_t pvt_monitors[] = {ECI_PVT_MON_A_CONTROL_REGr, ECI_PVT_MON_B_CONTROL_REGr, ECI_PVT_MON_C_CONTROL_REGr, ECI_PVT_MON_D_CONTROL_REGr, ECI_PVT_MON_CONTROL_REGr};
    int pvt_index;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit)) {
        
        COMPILER_64_ZERO(reg64_val);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, ECI_PVT_MON_CONTROL_REGr, REG_PORT_ANY, 0, reg64_val));
        
        COMPILER_64_BITSET(reg64_val, _SOC_JER_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, ECI_PVT_MON_CONTROL_REGr, REG_PORT_ANY, 0, reg64_val));
        
        COMPILER_64_ZERO(reg64_val);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, ECI_PVT_MON_CONTROL_REGr, REG_PORT_ANY, 0, reg64_val));
        
        COMPILER_64_BITSET(reg64_val, _SOC_JER_ECI_PVT_MON_CONTROL_REG_RESET_BIT);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, ECI_PVT_MON_CONTROL_REGr, REG_PORT_ANY, 0, reg64_val));
    } else {
        
        COMPILER_64_ZERO(reg64_val);
        for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
            if (!SOC_REG_IS_VALID(unit, pvt_monitors[pvt_index])) continue;
            SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
        }

        
        COMPILER_64_BITSET(reg64_val, _SOC_JER_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT);
        for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
            if (!SOC_REG_IS_VALID(unit, pvt_monitors[pvt_index])) continue;
            SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
       }

        
        COMPILER_64_ZERO(reg64_val);
        for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
            if (!SOC_REG_IS_VALID(unit, pvt_monitors[pvt_index])) continue;
            SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
        }

        
        COMPILER_64_BITSET(reg64_val, _SOC_JER_ECI_PVT_MON_CONTROL_REG_RESET_BIT);
        for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
            if (!SOC_REG_IS_VALID(unit, pvt_monitors[pvt_index])) continue;
            SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}


static void jer_mgmt_pvt_correction_thread(void *param)
{
    int curr;
    int port, rv;
    int unit = PTR_TO_INT(param);
    soc_control_t *soc = SOC_CONTROL(unit);
    int i;
    soc_switch_temperature_monitor_t tmp_temperature_array[DPP_SOC_PVT_MON_SIZE_MAX];
    int faulty_mon_counter;
    uint32 faulty_mon_bitmap;
    portmod_pbmp_t pvt_mon_phys;
    portmod_pbmp_t tmp_pbmp;
    portmod_pbmp_t pm_phys;
    uint8   pcount=0;
    uint32 pormod_tmp;
    int factor;
    int first_phy, sub_phy;

    
    
    PORTMOD_PBMP_CLEAR(pvt_mon_phys);
    for (i = 192; i <= 215; i++) {
        PORTMOD_PBMP_PORT_ADD(pvt_mon_phys, i);
    }

    LOG_VERBOSE(BSL_LS_SOC_MANAGEMENT,
                                (BSL_META_U(unit, "JER MGMT: PVT correction thread starting\n")));


    while (soc->pvt_mon_correction_interval > 0)
    {
        faulty_mon_bitmap = 0;
        faulty_mon_counter = 0;

        
        if (SOC_WARM_BOOT(unit)) {
            sal_sem_take(soc->pvt_mon_correction, soc->pvt_mon_correction_interval * 1000000);
            continue;
        }

        
        rv = jer_mgmt_temp_pvt_raw_get(unit, tmp_temperature_array, &faulty_mon_counter, &faulty_mon_bitmap);
        if (rv) {
            sal_sem_take(soc->pvt_mon_correction, soc->pvt_mon_correction_interval * 1000000);
            continue;
        }
        
        if ((faulty_mon_counter > 1) || ((faulty_mon_bitmap & (1 << 2)) == 0)) {
            LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                                (BSL_META_U(unit,"Faulty PVT monitor number is %d, faulty monitor shoud be C\n"), faulty_mon_counter));
            sal_sem_take(soc->pvt_mon_correction, soc->pvt_mon_correction_interval * 1000000);
            continue;
        }
        factor = soc_property_get(unit, spn_PVT_MON_CORRECTION_FACTOR, 0);
        
        BCM_PBMP_ITER(PBMP_PORT_ALL(unit), port){
            PORTMOD_PBMP_CLEAR(pm_phys);
            rv = portmod_port_first_phy_get(unit, port, &first_phy, &sub_phy);
            if (BCM_FAILURE(rv)){
                continue;
            }
            PORTMOD_PBMP_PORT_ADD(pm_phys, first_phy);

            
            curr = (_SOC_JER_PVT_BASE - tmp_temperature_array[2].curr * _SOC_JER_PVT_FACTOR)/100000;
            
            curr = curr + factor;
            PORTMOD_PBMP_CLEAR(tmp_pbmp);
            PORTMOD_PBMP_ASSIGN(tmp_pbmp, pvt_mon_phys);
            PORTMOD_PBMP_AND(tmp_pbmp, pm_phys);
            PORTMOD_PBMP_COUNT(tmp_pbmp, pcount);

            if (pcount != 0) {
                
                pormod_tmp = ((431045 - ((curr << 10) + (curr << 4) + (curr << 3) + (curr << 1) + curr)) >> 9) | 0x1000;
                
                rv = portmod_port_phy_reg_write(unit, port, -1, BCM_PORT_PHY_INTERNAL, 0x0001d21b | (7<<28), pormod_tmp);
                if (rv) {
                    LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                        (BSL_META_U(unit,"portmod_port_phy_reg_write failed on port %d\n"), port));
                }
            }
        }
        sal_sem_take(soc->pvt_mon_correction, soc->pvt_mon_correction_interval * 1000000);
    }

    soc->pvt_mon_correction_interval = -1;

    LOG_VERBOSE(BSL_LS_SOC_MANAGEMENT,
                                (BSL_META_U(unit, "JER MGMT: PVT correction thread exitting\n")));

    sal_thread_exit(0);
}

int jer_mgmt_pvt_monitor_correction_enable(int unit, int interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    char thread_name[16];
    soc_switch_temperature_monitor_t tmp_temperature_array[DPP_SOC_PVT_MON_SIZE_MAX];
    int faulty_mon_counter;
    uint32 faulty_mon_bitmap;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The PVT monitor correction is for Jericho plus only\n")));
    }

    
    if ((interval > (SAL_INT32_MAX/1000000)) || (interval <= 0)) {
        LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
            (BSL_META_U(unit,"The interval value is out of range(1 to %d)\n"), (SAL_INT32_MAX/(1000000))));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    
    SOCDNX_IF_ERR_EXIT(jer_mgmt_temp_pvt_raw_get(unit, tmp_temperature_array, &faulty_mon_counter, &faulty_mon_bitmap));
    
    if (faulty_mon_counter > 1) {
        LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                                (BSL_META_U(unit,"faulty monitors number %d\n"), faulty_mon_counter));
        SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    }
    
    if ((faulty_mon_bitmap & (1 << 2)) == 0) {
        LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                  (BSL_META_U(unit,"Faulty monitor shoud be C\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    }

    if (soc->pvt_mon_correction_interval > 0) {
        
        SOCDNX_IF_ERR_EXIT(jer_mgmt_pvt_monitor_correction_disable(unit));
    }

    soc->pvt_mon_correction_interval = interval;
    soc->pvt_mon_correction = sal_sem_create("pvt_mon", sal_sem_BINARY, 0);

    sal_snprintf(thread_name, sizeof(thread_name), "PVT_mon.%d", unit);
    sal_thread_create(thread_name, SAL_THREAD_STKSZ,
        512,
        jer_mgmt_pvt_correction_thread, INT_TO_PTR(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_mgmt_pvt_monitor_correction_disable(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The PVT monitor correction is for Jericho plus only\n")));
    }

    if (soc->pvt_mon_correction_interval == 0) {
        LOG_INFO(BSL_LS_SOC_MANAGEMENT,
            (BSL_META_U(unit,"PVT mon thread is not running\n")));
        SOC_EXIT;
    }

    if (soc->pvt_mon_correction != NULL) {
        soc->pvt_mon_correction_interval = 0;
        sal_sem_give(soc->pvt_mon_correction);
        
        while (soc->pvt_mon_correction_interval >= 0) {
            sal_usleep(10000);
        }

        sal_sem_destroy(soc->pvt_mon_correction);
        soc->pvt_mon_correction = NULL;
        soc->pvt_mon_correction_interval = 0;
        SOCDNX_IF_ERR_EXIT(jer_mgmt_pvt_monitor_db_init(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_mgmt_pvt_monitor_db_init (int unit)
{
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < _SOC_JER_PVT_MON_NOF; i++) {
        jer_mgmt_pvt_mon_peak[unit][i] = 0x3FF; 
    }

    SOCDNX_FUNC_RETURN;
}

int jer_mgmt_pvt_monitor_correction_status_get(int unit, uint8  *enable, int *interval, int *counter)
{
    int faulty_mon_counter = 0;
    uint32 faulty_mon_bitmap = 0;
    soc_switch_temperature_monitor_t tmp_temperature_array[DPP_SOC_PVT_MON_SIZE_MAX];
    soc_control_t *soc = SOC_CONTROL(unit);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_mgmt_temp_pvt_raw_get(unit, tmp_temperature_array, &faulty_mon_counter, &faulty_mon_bitmap));

    *counter = faulty_mon_counter;

    *interval = soc->pvt_mon_correction_interval;
    *enable = (soc->pvt_mon_correction_interval > 0)?1:0;

exit:
    SOCDNX_FUNC_RETURN;
}
int jer_mgmt_pvt_correction_once(int unit, pbmp_t pbmp)
{
    int curr;
    int i,port, phy, rv, pcount;
    soc_switch_temperature_monitor_t tmp_temperature_array[DPP_SOC_PVT_MON_SIZE_MAX];
    int faulty_mon_counter;
    uint32 faulty_mon_bitmap;
    soc_pbmp_t tmp_pbmp, port_pbmp;
    uint32 pormod_tmp;
    int factor;
    uint32 regVal;
    soc_pbmp_t quad_pbmp, q_pbmp;
    portmod_port_diag_info_t diag_info;
    SOCDNX_INIT_FUNC_DEFS;

    
    
    SOC_PBMP_CLEAR(quad_pbmp);
    for (i = 192; i <= 215; i++) {
        SOC_PBMP_PORT_ADD(quad_pbmp, i / NUM_OF_LANES_IN_PM);
    }
    
    SOC_PBMP_ASSIGN(port_pbmp,PBMP_PORT_ALL(unit));
    SOC_PBMP_REMOVE(port_pbmp, pbmp);

    
    SOC_PBMP_ITER(port_pbmp, port){
        rv = portmod_port_diag_info_get(unit, port, &diag_info);
        if (BCM_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_MANAGEMENT,(BSL_META_U(unit,"Get faulty PVT failed\n")));
            SOC_EXIT;
        }
        PORTMOD_PBMP_ITER(diag_info.phys, phy){
            SOC_PBMP_PORT_REMOVE(quad_pbmp, phy / NUM_OF_LANES_IN_PM);
        }
    }

    faulty_mon_bitmap = 0;
    faulty_mon_counter = 0;

    
    rv = jer_mgmt_temp_pvt_raw_get(unit, tmp_temperature_array, &faulty_mon_counter, &faulty_mon_bitmap);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_MANAGEMENT,(BSL_META_U(unit,"Get faulty PVT failed\n")));
        SOC_EXIT;
    }

    if ((faulty_mon_counter > 1) || ((faulty_mon_bitmap & (1 << 2)) == 0)) {
        LOG_ERROR(BSL_LS_SOC_MANAGEMENT,(BSL_META_U(unit,"Faulty PVT monitor number is %d, or faulty is not C\n"), faulty_mon_counter));
        SOC_EXIT;
    }
    factor = soc_property_get(unit, spn_PVT_MON_CORRECTION_FACTOR, 0);
    
    SOC_PBMP_ITER(pbmp, port){
        
        SOC_PBMP_CLEAR(q_pbmp);
        
        SOC_PBMP_ASSIGN(tmp_pbmp, quad_pbmp);
        rv = portmod_port_diag_info_get(unit, port, &diag_info);
        if (BCM_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_MANAGEMENT,(BSL_META_U(unit,"Get faulty PVT failed\n")));
            continue;
        }
        
        PORTMOD_PBMP_ITER(diag_info.phys, phy){
            SOC_PBMP_PORT_ADD(q_pbmp, phy / NUM_OF_LANES_IN_PM);
        }
        
        SOC_PBMP_AND(tmp_pbmp, q_pbmp);
        SOC_PBMP_COUNT(tmp_pbmp, pcount);
        if (pcount == 0) {
            continue;
        }
        SOC_PBMP_REMOVE(quad_pbmp, q_pbmp);

        
        curr = (_SOC_JER_PVT_BASE - tmp_temperature_array[2].curr * _SOC_JER_PVT_FACTOR)/100000;
        
        curr = curr + factor;
        
        pormod_tmp = ((431045 - ((curr << 10) + (curr << 4) + (curr << 3) + (curr << 1) + curr)) >> 9) | 0x1000;
        
        rv = portmod_port_phy_reg_write(unit, port, -1, BCM_PORT_PHY_INTERNAL, 0x0001d21b | (7<<28), pormod_tmp);
        if (rv) {
             LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                       (BSL_META_U(unit,"portmod_port_phy_reg_write failed on port %d\n"), port));
             continue;
        }
        
        rv = portmod_port_phy_reg_read(unit, port, -1, BCM_PORT_PHY_INTERNAL, 0x1d104 | (7<<28), &regVal);
        if (rv) {
            LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                       (BSL_META_U(unit,"portmod_port_phy_reg_read failed to get core_dp_s_rstb on port %d\n"), port));
            continue;
        }
        regVal &= (~(1 << 13));
        rv = portmod_port_phy_reg_write(unit, port, -1, BCM_PORT_PHY_INTERNAL, 0x1d104 | (7<<28), regVal);
        if (rv) {
            LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                       (BSL_META_U(unit,"portmod_port_phy_reg_read failed to low core_dp_s_rstb on port %d\n"), port));
            continue;
        }
        
        sal_usleep(10);
        
        regVal |= (1 << 13);
        rv = portmod_port_phy_reg_write(unit, port, -1, BCM_PORT_PHY_INTERNAL, 0x1d104 | (7<<28), regVal);
        if (rv) {
            LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
                       (BSL_META_U(unit,"portmod_port_phy_reg_read failed to high core_dp_s_rstb on port %d\n"), port));
            continue;
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int jer_mgmt_revision_fixes (int unit)
{
    uint64 reg64_val;
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    int array_index, core_index ;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, CFC_SCH_OOB_RX_CFGr, REG_PORT_ANY,  0, &reg64_val));
    
    soc_reg64_field32_set(unit, CFC_SCH_OOB_RX_CFGr, &reg64_val, SCH_OOB_CRC_CFGf, 0x7);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, CFC_SCH_OOB_RX_CFGr, REG_PORT_ANY,  0,  reg64_val));

    
    for (array_index = 0; array_index < JER_MGMT_NOF_PROCESSOR_IDS; ++array_index) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY, array_index, &reg32_val));
         
        soc_reg_field_set(unit, CRPS_CRPS_GENERAL_CFGr, &reg32_val, CRPS_N_ACT_CNT_VALIDATE_ENf, 0x1);
        
        soc_reg_field_set(unit, CRPS_CRPS_GENERAL_CFGr, &reg32_val, CRPS_N_OVTH_MEM_RFRSH_BPASS_ENf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY, array_index,  reg32_val));
    }    

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IPT_CNM_CONTORLr, REG_PORT_ANY,  0, &reg32_val));
    
    soc_reg_field_set(unit, IPT_CNM_CONTORLr, &reg32_val, CNM_CALC_FABRIC_CRC_DISf, 0x0);
    
    soc_reg_field_set(unit, IPT_CNM_CONTORLr, &reg32_val, CNM_REMOVE_DRAM_CRC_DISf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IPT_CNM_CONTORLr, REG_PORT_ANY,  0,  reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IPT_IPT_ENABLESr, REG_PORT_ANY,  0, &reg32_val));
    
    soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg32_val, CNM_FHEI_5_BYTES_FIX_ENf, 0x1);
    
    soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg32_val, OCB_READ_PHASE_PROTECT_ENf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IPT_IPT_ENABLESr, REG_PORT_ANY,  0,  reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY,  0, &reg64_val));
    
    soc_reg64_field32_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, &reg64_val, LARGE_EM_CFG_LIMIT_MODE_FIDf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY,  0,  reg64_val)); 

    
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        reg32_val = 0;
        soc_reg_field_set(unit, EPNI_CHICKEN_BIT_VECTORr, &reg32_val, CHICKEN_BIT_VECTORf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CHICKEN_BIT_VECTORr(unit, reg32_val));
    }

    for (core_index = 0; core_index < JER_MGMT_NOF_CORES; ++core_index) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0, &reg32_val));

        
        soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_BUG_FIX_18_DISABLEf, 0x0);
        
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "refilter_uc_pkt_en", 0)) {
            soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_BUG_FIX_98_DISABLEf, 0x1);
        } else {
            soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_BUG_FIX_98_DISABLEf, 0x0);
        }

        
        soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_JBUG_FIX_7_DISABLEf, 0x0);

        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0,  reg32_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, core_index,  0, &reg32_val));
        
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg32_val, QPAIR_SPR_RESOLUTIONf, 0x0);
        
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg32_val, TCG_SPR_RESOLUTIONf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, core_index,  0,  reg32_val));

        
        if ((TRUE == SOC_DPP_CONFIG(unit)->arad->init.pp_enable) && (TRUE == SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) &&
            soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "petrab_in_tm_mode", 0) )
        {
            
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EGQ_PP_CONFIGr, core_index,  0, &reg32_val));
            
            soc_reg_field_set(unit, EGQ_PP_CONFIGr, &reg32_val, FIELD_13_13f, 0x1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EGQ_PP_CONFIGr, core_index,  0,  reg32_val));
        }

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, EGQ_QPAIR_SPR_DISr, core_index,  0, reg_above_64_val));
        
        soc_reg_field_set(unit, EGQ_QPAIR_SPR_DISr, reg_above_64_val, QPAIR_SPR_DISf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_QPAIR_SPR_DISr, core_index,  0,  reg_above_64_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, EGQ_TCG_SPR_DISr, core_index,  0, reg_above_64_val));
        
        soc_reg_field_set(unit, EGQ_TCG_SPR_DISr, reg_above_64_val, TCG_SPR_DISf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_TCG_SPR_DISr, core_index,  0,  reg_above_64_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0, &reg32_val));
        
        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_BUG_FIX_18_DISABLEf, 0x0);

 
        
        if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
            
            soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_PLUS_F_5_DISABLEf, 0x0);
            
            soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_NEW_TRILL_FORMAT_DISABLEf, 0x0);
        }

        
        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_PLUS_F_24_DISABLEf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0,  reg32_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, core_index,  0, &reg32_val));
        if ((SOC_DPP_CONFIG(unit)->pp.next_hop_mac_extension_enable)) 
        {
            
            soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_USE_HOST_INDEX_FOR_DA_CALC_ENABLEf, SOC_IS_NEXT_HOP_MAC_EXT_ARAD_COMPATIBLE(unit) ? 0x0 : 0x1);
        }



        
        if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
            if (soc_property_get(unit, spn_SYSTEM_IS_ARAD_IN_SYSTEM, 0)) {
                
                soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_EEI_OUTLIF_SWAP_ENABLEf, 0);
            }
            else {
                soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_EEI_OUTLIF_SWAP_ENABLEf, 0x1);
            }
        }

         
        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_MPLS_PIPE_FIX_ENABLEf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, core_index,  0,  reg32_val));

        
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, core_index,  0, &reg32_val));
        soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_EEI_OUTLIF_SAME_IF_FILTER_ENABLEf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, core_index,  0,  reg32_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_DC_OVERLAYr, core_index,  0, &reg32_val));
        
        soc_reg_field_set(unit, EPNI_CFG_DC_OVERLAYr, &reg32_val, CFG_EN_VXLAN_ENCAPSULATIONf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_DC_OVERLAYr, core_index,  0,  reg32_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_ENABLE_FILTERING_PER_FWD_CODEr, core_index,  0, &reg32_val));
        
        soc_reg_field_set(unit, EPNI_CFG_ENABLE_FILTERING_PER_FWD_CODEr, &reg32_val, CFG_ENABLE_FILTERING_PER_FWD_CODEf, 0xf7ff);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_ENABLE_FILTERING_PER_FWD_CODEr, core_index,  0,  reg32_val));

        
        if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
            
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_PP_CONFIGr, core_index,  0, &reg32_val));

            
            soc_reg_field_set(unit, EPNI_PP_CONFIGr, &reg32_val, INVALID_OUTLIF_FIX_ENf, 0x1);
            
            if ((TRUE == SOC_DPP_CONFIG(unit)->arad->init.pp_enable) && (TRUE == SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) &&
                soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "petrab_in_tm_mode", 0) )
            {
                
                soc_reg_field_set(unit, EPNI_PP_CONFIGr, &reg32_val, PETRAB_WITH_ARAD_PP_MODEf, 0x1);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_PP_CONFIGr, core_index,  0,  reg32_val));
        }

        
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IHB_LBP_GENERAL_CONFIG_0r, core_index,  0, &reg64_val));
        
        if ((TRUE == SOC_DPP_CONFIG(unit)->arad->init.pp_enable) && (TRUE == SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) &&
            soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "petrab_in_tm_mode", 0) )
        {
            
            soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64_val, PETRA_FTMH_WITH_ARAD_PPH_MODEf, 0x1);
        }
        
        soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64_val, ENABLE_FHEI_WITH_IN_LIF_PROFILEf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IHB_LBP_GENERAL_CONFIG_0r, core_index,  0,  reg64_val));

        


        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IHP_VTT_GENERAL_CONFIGS_1r, core_index,  0, reg_above_64_val));
        
        soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, reg_above_64_val, ENABLE_COS_MARKING_UPGRADESf, 0x1);
        
        soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, reg_above_64_val, DISABLE_INNER_COMPATIBLE_MCf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, core_index,  0,  reg_above_64_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IQM_RATE_CLASS_RD_WEIGHTr, core_index,  0, &reg32_val));
        
        soc_reg_field_set(unit, IQM_RATE_CLASS_RD_WEIGHTr, &reg32_val, RATE_CLASS_RD_WEIGHTf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IQM_RATE_CLASS_RD_WEIGHTr, core_index,  0,  reg32_val));  

        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {



            
            if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
                
                if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "roo_remark_profile", 0)) {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_TX_TAG_USED_AS_RIF_REMARKr, core_index,  0, &reg32_val));
                    soc_reg_field_set(unit, EPNI_CFG_TX_TAG_USED_AS_RIF_REMARKr, &reg32_val, CFG_TX_TAG_USED_AS_RIF_REMARKf, 1);
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_TX_TAG_USED_AS_RIF_REMARKr, core_index,  0, reg32_val));
                }

                
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_COUNTER_RESOLUTION_FIX_ENABLEDr, core_index,  0,  1));

                
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_FALL_TO_BRIDGE_FWD_CODE_FIX_ENABLEDr, core_index,  0,  1));

                
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_MAC_IN_MAC_ROO_FIX_ENABLEDr, core_index,  0,  1));
                
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IHP_RESERVED_SPARE_2r, core_index,  0, &reg32_val));
                soc_reg_field_set(unit, IHP_RESERVED_SPARE_2r, &reg32_val, FIELD_0_0f, 0x1);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IHP_RESERVED_SPARE_2r, core_index,  0,  reg32_val));

            }


            if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && !SOC_IS_QAX_A0(unit)) {
                
                COMPILER_64_SET(reg64_val, 0, 0);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, EPNI_MPLS_OUTLIF_PROFILE_PUSH_TO_SWAP_ENABLEDr, REG_PORT_ANY,  0,  reg64_val));
            }

            
            if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
                
                if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "roo_new_link_outer_vid", 0)) {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_ROO_NEW_LINK_OUTER_VID_ENABLEDr, core_index, 0,  1));
                } else {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_ROO_NEW_LINK_OUTER_VID_ENABLEDr, core_index, 0,  0));
                }
            }


            
            if (SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_TC_DP_MAP_PORT_FIX_ENABLEDr, core_index,  0,  1));
            } else {
                
                if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_TC_DP_MAP_PORT_FIX_ENABLEDr, core_index,  0,  0));
                }
            }

            
            if (SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit)) {
               reg32_val = SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE; 
               SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_NEW_IP_TUNNEL_ENC_MODE_ENABLEDr(unit, REG_PORT_ANY, reg32_val));
            } 

            
            
            if (SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EDB_EEDB_CHICKEN_BITSr, core_index,  0,  1));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


uint32
  jer_mgmt_enable_traffic_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_mgmt_enable_traffic_advanced_set(unit, NULL, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_mgmt_enable_traffic_get(
    SOC_SAND_IN   int unit,
    SOC_SAND_OUT  uint8 *enable
  )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(jer_mgmt_enable_traffic_advanced_get(unit, NULL, enable));
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  jer_mgmt_enable_traffic_advanced_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  jer_mgmt_enable_traffic_config_t *enable_traffic_config,
    SOC_SAND_IN  uint8 enable
  )
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOCDNX_PCID_LITE_SKIP(unit);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = jer_mgmt_enable_traffic_set_unsafe(unit, enable_traffic_config, enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_enable_traffic_set()",unit,enable);
}

uint32
  jer_mgmt_enable_traffic_advanced_get(
    SOC_SAND_IN   int unit,
    SOC_SAND_OUT  jer_mgmt_enable_traffic_config_t *enable_traffic_config,
    SOC_SAND_OUT  uint8 *enable
  )
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = jer_mgmt_enable_traffic_get_unsafe(unit, enable_traffic_config, enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_enable_traffic_get()",unit,enable);
}

STATIC uint32
  jer_mgmt_enable_traffic_config_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN jer_mgmt_enable_traffic_config_t *enable_traffic_config)
{
    uint32 reg_val32;
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    for (core = 0; core < SOC_DPP_DEFS_GET(unit, nof_cores); ++core) {
        
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr(unit, core, &reg_val32));
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_0_PRIO_0_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_0_drop_th);
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_0_PRIO_1_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_1_drop_th);
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_0_PRIO_2_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_2_drop_th);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr(unit, core, reg_val32));

        
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr(unit, core, &reg_val32));
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_1_PRIO_0_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_0_drop_th);
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_1_PRIO_1_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_1_drop_th);
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_1_PRIO_2_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_2_drop_th);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr(unit, core, reg_val32));

        
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr(unit, core, &reg_val32));
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_2_PRIO_0_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_0_drop_th);
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_2_PRIO_1_DROP_THRf, enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_1_drop_th);
        soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr, &reg_val32, EGQ_N_FABFIF_2_PRIO_2_DROP_THRf,  enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_2_drop_th);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr(unit, core, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
  jer_mgmt_enable_traffic_config_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT jer_mgmt_enable_traffic_config_t *enable_traffic_config)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr(unit, 0, &reg_val32));
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_0_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_0_PRIO_0_DROP_THRf);
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_1_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_0_PRIO_1_DROP_THRf);
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_2_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_0_PRIO_2_DROP_THRf);

    
    SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr(unit, 0, &reg_val32));
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_0_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_1_PRIO_0_DROP_THRf);
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_1_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_1_PRIO_1_DROP_THRf);
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_2_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_1_PRIO_2_DROP_THRf);

    
    SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr(unit, 0, &reg_val32));
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_0_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_2_PRIO_0_DROP_THRf);
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_1_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_2_PRIO_1_DROP_THRf);
    enable_traffic_config->fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_2_drop_th = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_2_PRIO_DROP_THRr, reg_val32, EGQ_N_FABFIF_2_PRIO_2_DROP_THRf);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 jer_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  jer_mgmt_enable_traffic_config_t *enable_traffic_config,
    SOC_SAND_IN  uint8  enable)
{
    uint32 data, mem_val32;
    int core_for_write, core_for_read;
    soc_reg_above_64_val_t tdm_context_drop;
    bcm_port_t port;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_FLAIR(unit))
    {
        goto exit;
    }

    
    if (enable && !soc_feature(unit, soc_feature_no_fabric) &&
             (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP) &&
             (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "skip_enable_traffic_state_modify", 0) == 0))
    {
    	SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_mesh_topology_state_modify(unit));
    }


    
    core_for_read = 0 ;
    core_for_write = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores == 1 ? 0 : SOC_CORE_ALL;

    if (enable == FALSE) {

        
        if (!SOC_IS_QUX(unit))
        {
            SOCDNX_IF_ERR_EXIT(READ_IRE_TDM_CONTEXT_DROPr(unit, tdm_context_drop));
            if (bcm_petra_init_check(unit) == BCM_E_NONE)
            {
                BCM_PBMP_ITER(PBMP_TDM_ALL(unit), port)
                {
                    
                    if(SHR_BITGET(tdm_context_drop, port) == 0)
                    {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("all TDM contexts should be disabled prior disabling traffic. Use bcm_port_ingress_tdm_failover_set")));
                    }
                }
            }
        }

        
        SOCDNX_IF_ERR_EXIT(READ_IRE_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IRE_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_DYNAMIC_CONFIGURATIONr(unit, data));

        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_ENABLERSr(unit, SOC_CORE_ALL, 0x00));

            SOCDNX_IF_ERR_EXIT(READ_SPB_DYNAMIC_CONFIGURATIONr(unit, SOC_CORE_ALL, &data));
            soc_reg_field_set(unit, SPB_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_SPB_DYNAMIC_CONFIGURATIONr(unit, SOC_CORE_ALL, data));
        } else {
                
            SOCDNX_IF_ERR_EXIT(READ_IDR_DYNAMIC_CONFIGURATIONr(unit, &data));
            soc_reg_field_set(unit, IDR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_IDR_DYNAMIC_CONFIGURATIONr(unit, data));

                
            SOCDNX_IF_ERR_EXIT(READ_IRR_DYNAMIC_CONFIGURATIONr(unit, &data));
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_0f, 0x0);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_1f, 0x0);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_0f, 0x0);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_1f, 0x0);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_0f, 0x0);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_1f, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_IRR_DYNAMIC_CONFIGURATIONr(unit, data));
        }
        
        if (SOC_IS_QUX(unit)) {
            SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_CONFIGr(unit, core_for_read, &data));
            soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &data, DBG_FDA_STOPf, 0x1);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_CONFIGr(unit, core_for_write, data));
        } else {
            
            if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && enable_traffic_config) {
                SOCDNX_IF_ERR_EXIT(jer_mgmt_enable_traffic_config_set(unit, enable_traffic_config));
            }

            SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core_for_read, &data));
            soc_reg_field_set(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, &data, DBG_FDA_STOPf, 0x1);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core_for_write, data));
        }
                
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &data, DISABLE_FABRIC_MSGSf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_write, data));
        sal_usleep(100);
                 
        SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_CONFIGr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_DVS_CONFIGr, &data, FORCE_PAUSEf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_DVS_CONFIGr(unit, core_for_write, data));
        sal_usleep(100);
                
        SOCDNX_IF_ERR_EXIT(READ_SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr, &data, DLM_ENABLEf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr(unit, core_for_write, data));
        sal_usleep(100);
    } else {
    	
        if (SOC_IS_QUX(unit)) {
            
            SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_CONFIGr(unit, core_for_read, &data));
            soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &data, DBG_FDA_STOPf, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_CONFIGr(unit,  core_for_write, data));
        } else {
            
            SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core_for_read, &data));
            soc_reg_field_set(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, &data, DBG_FDA_STOPf, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit,  core_for_write, data));

            
            if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && enable_traffic_config) {
                SOCDNX_IF_ERR_EXIT(jer_mgmt_enable_traffic_config_set(unit, enable_traffic_config));
            }
        }
        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(READ_SPB_DYNAMIC_CONFIGURATIONr(unit, SOC_CORE_ALL, &data));
            soc_reg_field_set(unit, SPB_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x1);
            SOCDNX_IF_ERR_EXIT(WRITE_SPB_DYNAMIC_CONFIGURATIONr(unit, SOC_CORE_ALL, data));

            SOCDNX_IF_ERR_EXIT(WRITE_CGM_ENABLERSr(unit, SOC_CORE_ALL, 0xff));
        } else {
                    
            SOCDNX_IF_ERR_EXIT(READ_IRR_DYNAMIC_CONFIGURATIONr(unit, &data));
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_0f, 0x1);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_1f, 0x1);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_0f, 0x1);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_1f, 0x1);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_0f, 0x1);
            soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_1f, 0x1);
            SOCDNX_IF_ERR_EXIT(WRITE_IRR_DYNAMIC_CONFIGURATIONr(unit, data));

                        
            SOCDNX_IF_ERR_EXIT(READ_IDR_DYNAMIC_CONFIGURATIONr(unit, &data));
            soc_reg_field_set(unit, IDR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x1);
            SOCDNX_IF_ERR_EXIT(WRITE_IDR_DYNAMIC_CONFIGURATIONr(unit, data));
        }

                
        SOCDNX_IF_ERR_EXIT(READ_IRE_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IRE_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_DYNAMIC_CONFIGURATIONr(unit, data));

                
        SOCDNX_IF_ERR_EXIT(READ_SCH_MEM_01F00000m(unit, SCH_BLOCK(unit, core_for_read), 0, &mem_val32));
        soc_mem_field32_set(unit, SCH_MEM_01F00000m, &mem_val32, ITEM_0_0f, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_MEM_01F00000m(unit, SCH_BLOCK(unit, core_for_write), 0, &mem_val32));

                
        SOCDNX_IF_ERR_EXIT(READ_SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr, &data, DLM_ENABLEf, 0x7);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr(unit, core_for_write, data));
        sal_usleep(100);
                
        SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_CONFIGr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_DVS_CONFIGr, &data, FORCE_PAUSEf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_DVS_CONFIGr(unit, core_for_write, data));
        sal_usleep(100);
                
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &data, DISABLE_FABRIC_MSGSf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_write, data));
        sal_usleep(100);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 jer_mgmt_enable_traffic_get_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_OUT  jer_mgmt_enable_traffic_config_t *enable_traffic_config,
    SOC_SAND_OUT  uint8  *enable)
{
    uint32 reg_val32;
    uint8  enable_curr;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && enable_traffic_config) {
        SOCDNX_IF_ERR_EXIT(jer_mgmt_enable_traffic_config_get(unit, enable_traffic_config));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_IRE_DYNAMIC_CONFIGURATIONr(unit, &reg_val32));
    enable_curr = soc_reg_field_get(unit, IRE_DYNAMIC_CONFIGURATIONr, reg_val32, ENABLE_DATA_PATHf);
    enable_curr = SOC_SAND_NUM2BOOL(enable_curr);

    *enable = enable_curr;
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 jer_mgmt_set_mru_by_dbuff_size(
    SOC_SAND_IN  int     unit
  )
{
    uint32 mru = SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size * ARAD_MGMT_MAX_BUFFERS_PER_PACKET;
    soc_mem_t mem = SOC_IS_QAX(unit) ? SPB_CONTEXT_MRUm : IDR_CONTEXT_MRUm;
    uint32 entry[2] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    if (mru > ARAD_MGMT_PCKT_SIZE_BYTES_VSC_MAX) {
         mru = ARAD_MGMT_PCKT_SIZE_BYTES_VSC_MAX;
    }

    soc_mem_field32_set(unit, mem, entry, MAX_ORG_SIZEf, mru);
    soc_mem_field32_set(unit, mem, entry, MAX_SIZEf, mru);
    soc_mem_field32_set(unit, mem, entry, MIN_ORG_SIZEf, 0x20);
    soc_mem_field32_set(unit, mem, entry, MIN_SIZEf, 0x20);

    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, mem, MEM_BLOCK_ANY, entry));

exit:
    SOCDNX_FUNC_RETURN;
}



int jer_mgmt_dma_fifo_channel_free_find(SOC_SAND_IN int unit, SOC_SAND_IN uint8 skip_pci_cmc, SOC_SAND_OUT int * channel_number)
{
    int i, rv = SOC_E_NONE;    
    uint64 fifo_dma_sel_data;
    uint32 selected_applicaion;
    CONST soc_field_t field_name[NOF_FIFO_DMA_CHANNELS] = {FIFO_DMA_0_SELf, FIFO_DMA_1_SELf, FIFO_DMA_2_SELf, FIFO_DMA_3_SELf, 
                                                                        FIFO_DMA_4_SELf, FIFO_DMA_5_SELf, FIFO_DMA_6_SELf, FIFO_DMA_7_SELf, 
                                                                        FIFO_DMA_8_SELf, FIFO_DMA_9_SELf, FIFO_DMA_10_SELf, FIFO_DMA_11_SELf};
    SOCDNX_INIT_FUNC_DEFS;

    * channel_number = -1;
    
    
     rv = READ_ECI_FIFO_DMA_SELr(unit, &fifo_dma_sel_data);
     SOCDNX_IF_ERR_EXIT(rv);  

    
    if (!soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) && SOC_PCI_CMCS_NUM(unit) > 1)
    {
         LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "PCI_CMC_NUM=%d, not allowed when soc_feature_cmicm_multi_dma_cmc disabled \n"), SOC_PCI_CMCS_NUM(unit)));
         SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);  
    }
    
    
    
    for(i = SOC_PCI_CMC(unit)*NOF_DMA_FIFO_PER_CMC; i < (SOC_PCI_CMC(unit) + SOC_PCI_CMCS_NUM(unit))*NOF_DMA_FIFO_PER_CMC ;i++)
    {        
        if (i < NOF_FIFO_DMA_CHANNELS)
        {
            selected_applicaion = soc_reg64_field32_get(unit, ECI_FIFO_DMA_SELr, fifo_dma_sel_data, field_name[i]);
            if(selected_applicaion == dma_fifo_channel_src_reserved)
            {
                if(skip_pci_cmc == FALSE)
                {
                    *channel_number = i;
                    break;                
                }
                else 
                {
                    if(i >= (SOC_PCI_CMC(unit)*NOF_DMA_FIFO_PER_CMC + NOF_DMA_FIFO_PER_CMC))
                    {
                        *channel_number = i;
                        break;  
                    }
                }
            }
        }
        else
        {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "FIFO DMA index out of range. \n")));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);                            
        }
    }    

exit:
    SOCDNX_FUNC_RETURN;   
}



int jer_mgmt_dma_fifo_channel_set(SOC_SAND_IN int unit, SOC_SAND_IN int channel, SOC_SAND_IN dma_fifo_channel_src_t value) 
{
    int rv = SOC_E_NONE;
    uint64 reg64, field64, val64;
    CONST soc_field_t field_name[NOF_FIFO_DMA_CHANNELS] = {FIFO_DMA_0_SELf, FIFO_DMA_1_SELf, FIFO_DMA_2_SELf, FIFO_DMA_3_SELf, 
                                                                        FIFO_DMA_4_SELf, FIFO_DMA_5_SELf, FIFO_DMA_6_SELf, FIFO_DMA_7_SELf, 
                                                                        FIFO_DMA_8_SELf, FIFO_DMA_9_SELf, FIFO_DMA_10_SELf, FIFO_DMA_11_SELf};
    dma_fifo_channel_src_t prev_channel_src_val;
    SOCDNX_INIT_FUNC_DEFS;
   
    if(channel >= NOF_FIFO_DMA_CHANNELS)
    {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "FIFO DMA channel out of range. \n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);       
    }
    rv = READ_ECI_FIFO_DMA_SELr(unit, &reg64);
    SOCDNX_IF_ERR_EXIT(rv);
    
    val64 = soc_reg64_field_get(unit, ECI_FIFO_DMA_SELr, reg64, field_name[channel]);
    prev_channel_src_val = (dma_fifo_channel_src_t)COMPILER_64_LO(val64);
    COMPILER_64_SET(field64,0,value);
    soc_reg64_field_set(unit, ECI_FIFO_DMA_SELr, &reg64, field_name[channel], field64);

    rv = WRITE_ECI_FIFO_DMA_SELr(unit, reg64);
    SOCDNX_IF_ERR_EXIT(rv);  

    
    if(value == dma_fifo_channel_src_reserved)
    {
        
        if (prev_channel_src_val < dma_fifo_channel_src_max)
        {
            jer_mgmt_dma_fifo_source_channels_db[unit].dma_fifo_source_channels_array[prev_channel_src_val] = -1;
        }
    }
    else
    {
        if (value < dma_fifo_channel_src_max)
        {
            jer_mgmt_dma_fifo_source_channels_db[unit].dma_fifo_source_channels_array[value] = channel;
        }
        else
        {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "value=%d. out of range \n"), value));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);                 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_mgmt_dma_fifo_channel_get (SOC_SAND_IN int unit, SOC_SAND_IN dma_fifo_channel_src_t source, SOC_SAND_OUT int* channel)
{    
    SOCDNX_INIT_FUNC_DEFS;

    if (source < dma_fifo_channel_src_max)
    {
        *channel = jer_mgmt_dma_fifo_source_channels_db[unit].dma_fifo_source_channels_array[source];
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "source=%d. out of range \n"), source));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);     
    }

exit:
    SOCDNX_FUNC_RETURN;    
}



int jer_mgmt_dma_fifo_source_channels_db_init (int unit)
{    
    int rv = SOC_E_NONE, i;
    uint64 reg64, val64;
    CONST soc_field_t field_name[NOF_FIFO_DMA_CHANNELS] = {FIFO_DMA_0_SELf, FIFO_DMA_1_SELf, FIFO_DMA_2_SELf, FIFO_DMA_3_SELf, 
                                                                        FIFO_DMA_4_SELf, FIFO_DMA_5_SELf, FIFO_DMA_6_SELf, FIFO_DMA_7_SELf, 
                                                                        FIFO_DMA_8_SELf, FIFO_DMA_9_SELf, FIFO_DMA_10_SELf, FIFO_DMA_11_SELf};
    dma_fifo_channel_src_t channel;
    SOCDNX_INIT_FUNC_DEFS;


    rv = READ_ECI_FIFO_DMA_SELr(unit, &reg64);
    SOCDNX_IF_ERR_EXIT(rv);
    
    for(i=0; i < dma_fifo_channel_src_max; i++)
    {
        jer_mgmt_dma_fifo_source_channels_db[unit].dma_fifo_source_channels_array[i] = -1;
    }
    
    
    for(i=0; i < NOF_FIFO_DMA_CHANNELS; i++)
    {
        val64 = soc_reg64_field_get(unit, ECI_FIFO_DMA_SELr, reg64, field_name[i]);
        channel = (dma_fifo_channel_src_t)COMPILER_64_LO(val64);  
        if(channel != dma_fifo_channel_src_reserved)
        {
            jer_mgmt_dma_fifo_source_channels_db[unit].dma_fifo_source_channels_array[channel] = i;           
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;    
}




int
  jer_mgmt_voq_is_ocb_eligible_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  int         core_id,
    SOC_SAND_IN  uint32      qid,
    SOC_SAND_OUT uint32      *is_ocb_eligible
  )
{
    uint32 line = 0, bit_in_line = 0, eligibility[1] = {0};
    uint64 reg64_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(is_ocb_eligible); 
    if ((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    COMPILER_64_ZERO(reg64_val);

    if (!SOC_IS_QAX(unit)) {
        
        line = qid / 32;
        bit_in_line = qid % 32;

        
        SOCDNX_IF_ERR_EXIT(READ_IDR_QUEUE_IS_OCB_ELIGIBLEm(unit, core_id, IDR_BLOCK(unit), line, &reg64_val));
        eligibility[0] = soc_mem_field32_get(unit, IDR_QUEUE_IS_OCB_ELIGIBLEm, &reg64_val, ELIGIBILITYf);
        *is_ocb_eligible = SHR_BITGET(eligibility, bit_in_line);
    }
exit:
    SOCDNX_FUNC_RETURN;    
}


int jer_mgmt_avs_value_get(
            int       unit,
            uint32*      avs_val)
{
    uint32
        reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(avs_val);

    *avs_val = 0;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, ECI_ROV_STATUSr, REG_PORT_ANY, 0, &reg_val));
    *avs_val = soc_reg_field_get(unit, ECI_ROV_STATUSr, reg_val, AVS_STATUSf);

exit:
    SOCDNX_FUNC_RETURN;
}



int jer_mgmt_mirror_snoop_forward_original_when_dropped_set(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8       enabled
  )
{
    uint32 value;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IRR_STATIC_CONFIGURATIONr(unit, &value));
    soc_reg_field_set(unit, IRR_STATIC_CONFIGURATIONr, &value, FWD_VALID_EN_ON_SNOOP_MIRROR_DROPf, enabled ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_IRR_STATIC_CONFIGURATIONr(unit, value));
exit:
    SOCDNX_FUNC_RETURN;    
}



int jer_mgmt_mirror_snoop_forward_original_when_dropped_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_OUT uint8       *enabled
  )
{
    uint32 value;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(enabled);

    SOCDNX_IF_ERR_EXIT(READ_IRR_STATIC_CONFIGURATIONr(unit, &value));
    *enabled = soc_reg_field_get(unit, IRR_STATIC_CONFIGURATIONr, value, FWD_VALID_EN_ON_SNOOP_MIRROR_DROPf);

exit:
    SOCDNX_FUNC_RETURN;    
}


#undef _ERR_MSG_MODULE_NAME

