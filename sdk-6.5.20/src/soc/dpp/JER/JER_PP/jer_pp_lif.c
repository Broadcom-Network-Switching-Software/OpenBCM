/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/dpp/JER/JER_PP/jer_pp_lif.h>


#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif 

#ifdef SOC_DPP_IS_EM_HW_ENABLE
  #include <soc/dpp/ARAD/arad_sim_em.h>
#else
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#endif







#define SOC_JER_LIF_GLEM_BUFFER_SIZE (2)


#define SOC_JER_LIF_GLEM_ACTION_WRITE (1)
#define SOC_JER_LIF_GLEM_ACTION_REMOVE (0)


#define SOC_JER_LIF_GLEM_ACCESS_DEBUG (0)

#define JER_PP_EG_ENCAP_NOF_ENTRIES_PER_HALF_BANK (4096)


















#ifdef SOC_DPP_IS_EM_HW_ENABLE
STATIC soc_error_t
soc_jer_lif_glem_entry_buffer_set(int unit, uint32 action_type, uint32 global_lif_id, uint32 egress_lif_id, uint32 *buffer);

soc_error_t
soc_jer_lif_glem_access_assert_management_request_complete(int unit);

STATIC soc_error_t
soc_jer_lif_glem_access_write_to_machine(int unit, uint32 *buffer);

STATIC soc_error_t
soc_jer_lif_glem_access_entry_by_key_get_from_machine(int unit, int global_lif_id, int *egress_lif_id, uint8 *accessed, uint8 *is_found);
#endif 



soc_error_t
soc_jer_lif_init(int unit){
    int rv;
    uint64 buffer;
    int nof_direct_banks; 
    SOCDNX_INIT_FUNC_DEFS;

    
     
    
    rv = soc_reg_above_64_field32_modify(unit, EDB_GLEM_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, GLEM_MNGMNT_UNIT_ENABLEf,  0x1);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_reg_field32_modify(unit, EPNI_CFG_GLEM_LKUP_CONFIGr, REG_PORT_ANY, CFG_GLEM_LKUP_RESULT_PHASEf, 0x1);
    SOCDNX_IF_ERR_EXIT(rv);

    

    
 
    if (SOC_IS_JERICHO_PLUS(unit)){
        COMPILER_64_ALLONES(buffer);
    } else if (SOC_IS_JERICHO_AND_BELOW(unit)) {
        nof_direct_banks = SOC_DPP_CONFIG(unit)->l3.nof_rifs / JER_PP_EG_ENCAP_NOF_ENTRIES_PER_HALF_BANK;
        COMPILER_64_MASK_CREATE(buffer, 64  - nof_direct_banks, nof_direct_banks);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Glem does not exist on this chip.")));
    }
    
    
    rv = soc_reg_above_64_field64_modify(unit, EPNI_CFG_OUTLIF_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_OUTLIF_MAPPING_IS_REQUIREDf, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_reg_above_64_field64_modify(unit, EPNI_CFG_EEI_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_EEI_MAPPING_IS_REQUIREDf, buffer);
    SOCDNX_IF_ERR_EXIT(rv);


    
    if (SOC_IS_JERICHO_A0(unit) || SOC_IS_QMX_A0(unit)) {
        rv = soc_jer_lif_glem_access_entry_add(unit, SOC_PPC_INVALID_GLOBAL_LIF, SOC_PPC_INVALID_GLOBAL_LIF);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_lif_glem_access_entry_add(int unit, int global_lif_id, int egress_lif_id){
    int rv;

    uint32 uint_global_lif_id[1], uint_egress_lif_id[1];
#ifdef SOC_DPP_IS_EM_HW_ENABLE
    uint32 buffer[SOC_JER_LIF_GLEM_BUFFER_SIZE] = {0};
    uint32 reason;
    uint32
      is_failed;
    uint32 failure;
#endif
    uint8 is_success = TRUE;
    SOCDNX_INIT_FUNC_DEFS;
    
 
#ifdef SOC_DPP_IS_EM_HW_ENABLE
 
    

    rv = soc_jer_lif_glem_entry_buffer_set(unit, SOC_JER_LIF_GLEM_ACTION_WRITE, global_lif_id, egress_lif_id, buffer);
    SOCDNX_IF_ERR_EXIT(rv);



    
    rv = soc_jer_lif_glem_access_write_to_machine(unit, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

     
    rv = READ_EDB_GLEM_MANAGEMENT_UNIT_FAILUREr(unit, &failure);
    SOCDNX_IF_ERR_EXIT(rv);
    
    is_success = TRUE;
    
    is_failed = soc_reg_field_get(unit, EDB_GLEM_MANAGEMENT_UNIT_FAILUREr, failure, GLEM_MNGMNT_UNIT_FAILURE_VALIDf);
    if (is_failed)
    {

        reason = soc_reg_field_get(unit, EDB_GLEM_MANAGEMENT_UNIT_FAILUREr, failure, GLEM_MNGMNT_UNIT_FAILURE_REASONf);

        switch(reason)
        {
        case 0x001:
        case 0x002:
        case 0x008:
        case 0x080:
        case 0x100:
        case 0x200:
        case 0x400:
            is_success = FALSE;
        break;
        default:
        break;
        }
    } else {
        reason = 0;
    }

    
#if SOC_JER_LIF_GLEM_ACCESS_DEBUG
    if (reason)
    {
      switch(reason)
      {
      case 0x001:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Cam table full              ")));
        break;
      case 0x002:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Table coherency             ")));
        break;
      case 0x004:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Delete unknown key          ")));
        break;
      case 0x008:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Reached max entry limit     ")));
        break;
      case 0x010:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Inserted existing           ")));
        break;
      case 0x020:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Learn request over static   ")));
        break;
      case 0x040:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Learn over existing         ")));
        break;
      case 0x080:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Change-fail non exist       ")));
        break;
      case 0x100:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Change request over static  ")));
        break;
      case 0x200:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Change non-exist from other ")));
        break;
      case 0x400:
        LOG_INFO(BSL_LS_SOC_LIF,
                 (BSL_META_U(unit,
                             "Notice: Change non-exist from self  ")));
        break;
      }
    }
#endif 

#endif   

    if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
        
        *uint_egress_lif_id = egress_lif_id;
        *uint_global_lif_id = global_lif_id;
        rv = chip_sim_exact_match_entry_add_unsafe(
                unit,
                ARAD_CHIP_SIM_GLEM_BASE,
                uint_global_lif_id,
                ARAD_CHIP_SIM_GLEM_KEY,
                uint_egress_lif_id,
                ARAD_CHIP_SIM_GLEM_PAYLOAD,
                &is_success
              );
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (!is_success){
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Glem table is full")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_lif_glem_access_entry_remove(int unit, int global_lif_id){
    int rv;
    uint32 uint_global_lif_id[1];
#ifdef SOC_DPP_IS_EM_HW_ENABLE
    uint32
      buffer[SOC_JER_LIF_GLEM_BUFFER_SIZE] = {0};
#endif
    SOCDNX_INIT_FUNC_DEFS;
    
    
        
#ifdef SOC_DPP_IS_EM_HW_ENABLE
    
    
    rv = soc_jer_lif_glem_entry_buffer_set(unit, SOC_JER_LIF_GLEM_ACTION_REMOVE, global_lif_id, 0, buffer);
    SOCDNX_IF_ERR_EXIT(rv);


    
    rv = soc_jer_lif_glem_access_write_to_machine(unit, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

#endif 

    if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
        *uint_global_lif_id = global_lif_id;
        rv = chip_sim_exact_match_entry_remove_unsafe(
                unit,
                ARAD_CHIP_SIM_GLEM_BASE,
                uint_global_lif_id,
                ARAD_CHIP_SIM_GLEM_KEY
              );
        SOCDNX_IF_ERR_EXIT(rv);
    }


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_lif_glem_access_entry_by_key_get(int unit, int global_lif_id, int *egress_lif_id, uint8 *accessed, uint8 *is_found){
    int rv;
    uint32 fld_val[1];
    uint32 reg_val1[1];
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(egress_lif_id);
    SOCDNX_NULL_CHECK(is_found);
    
#ifdef SOC_DPP_IS_EM_HW_ENABLE
    
    if (!SOC_DPP_IS_EM_SIM_ENABLE(unit) || accessed) {
        rv = soc_jer_lif_glem_access_entry_by_key_get_from_machine(unit, global_lif_id, egress_lif_id, accessed, is_found);
        SOCDNX_IF_ERR_EXIT(rv);
    } else 

#endif 
    {
        
        *reg_val1 = global_lif_id;
        rv = chip_sim_exact_match_entry_get_unsafe(
                unit,
                ARAD_CHIP_SIM_GLEM_BASE,
                reg_val1,
                ARAD_CHIP_SIM_GLEM_KEY,
                fld_val,
                ARAD_CHIP_SIM_GLEM_PAYLOAD,
                is_found
              );
        SOCDNX_IF_ERR_EXIT(rv);
        
        if (*is_found) {
            *egress_lif_id = *fld_val;
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}
#ifdef SOC_DPP_IS_EM_HW_ENABLE

soc_error_t
soc_jer_lif_glem_access_assert_management_request_complete(int unit){
    int rv;
    uint32 temp;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_reg_above_64_field32_read(unit, EDB_GLEM_INTERRUPT_REGISTER_ONEr, REG_PORT_ANY, 0, GLEM_MANAGEMENT_COMPLETEDf, &temp);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (temp){
        SOCDNX_EXIT_WITH_ERR(SOC_E_BUSY, (_BSL_SOCDNX_MSG("Glem table manager is busy")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
soc_jer_lif_glem_access_write_to_machine(int unit, uint32 *buffer){
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);

    
    rv = soc_mem_write(unit, EDB_GLEM_MANAGEMENT_REQUESTm, SOC_BLOCK_ALL, 0, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    

    rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EDB_GLEM_INTERRUPT_REGISTER_ONEr, REG_PORT_ANY, 0, GLEM_MANAGEMENT_COMPLETEDf, 1);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    
    
    rv = soc_reg_field32_modify(unit, EDB_GLEM_INTERRUPT_REGISTER_ONEr, REG_PORT_ANY, GLEM_MANAGEMENT_COMPLETEDf, 1);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
soc_jer_lif_glem_entry_buffer_set(int unit, uint32 action_type, uint32 global_lif_id, uint32 egress_lif_id, uint32 *buffer){
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);

    
    sal_memset(buffer, 0, sizeof(uint32) * SOC_JER_LIF_GLEM_BUFFER_SIZE);

    
    soc_EDB_GLEM_MANAGEMENT_REQUESTm_field32_set(unit, buffer, GLEM_TYPEf, action_type);

    
    soc_EDB_GLEM_MANAGEMENT_REQUESTm_field32_set(unit, buffer, GLEM_KEYf, global_lif_id);

    
    soc_EDB_GLEM_MANAGEMENT_REQUESTm_field32_set(unit, buffer, GLEM_PAYLOADf, egress_lif_id);

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
soc_jer_lif_glem_access_entry_by_key_get_from_machine(int unit, int global_lif_id, int *egress_lif_id, uint8 *accessed, uint8 *is_found){
    int rv;
    uint32 fld_val[1];
    uint32 reg_val1[1];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    *fld_val = global_lif_id;
    rv = WRITE_EDB_GLEM_DIAGNOSTICS_KEYr(unit, *fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

    
    *fld_val = 0x1;
    rv = soc_reg_field32_modify(unit, EDB_GLEM_DIAGNOSTICSr, REG_PORT_ANY, GLEM_DIAGNOSTICS_LOOKUPf, *fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

    
    rv = (arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EDB_GLEM_DIAGNOSTICSr, 0, 0, GLEM_DIAGNOSTICS_LOOKUPf, 0));
    SOC_SAND_CHECK_FUNC_RESULT(rv, 30, exit);

    
    rv = READ_EDB_GLEM_DIAGNOSTICS_LOOKUP_RESULTr(unit, reg_val1);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 40, exit);

    *is_found = soc_reg_field_get(unit, EDB_GLEM_DIAGNOSTICS_LOOKUP_RESULTr, *reg_val1, GLEM_ENTRY_FOUNDf);

    if (!*is_found){
        SOC_EXIT;
    }

    *egress_lif_id = soc_reg_field_get(unit, EDB_GLEM_DIAGNOSTICS_LOOKUP_RESULTr, *reg_val1, GLEM_ENTRY_PAYLOADf);

    if (accessed) {
        *accessed = soc_reg_field_get(unit, EDB_GLEM_DIAGNOSTICS_LOOKUP_RESULTr, *reg_val1, GLEM_ENTRY_ACCESSEDf);
    }

exit:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_lif_glem_access_entry_by_key_get_from_machine()", unit, 0);
}

#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>


