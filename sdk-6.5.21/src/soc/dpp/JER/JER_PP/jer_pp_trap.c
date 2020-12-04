/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_TRAP




#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_eg_mirror.h>

#include <soc/dpp/JER/JER_PP/jer_pp_trap.h>


#define LIF_MTU_PROFILE_ZERO  (0) 
#define LIF_MTU_OUTLIF_PROFILE_NOF_BITS (6)
#define LIF_MTU_OUTLIF_PROFILE_SIZE (64) 
#define LIF_MTU_NOF_BITS_MTU_PROFILE (3)




static soc_reg_t etpp_reg_names[ETPP_NOF_TRAPS] =
{
    
    EPNI_CFG_EES_ACTION_TRAPr,
    
    EPNI_CFG_STP_STATE_TRAPr,
    
    EPNI_CFG_PROTECTION_PATH_TRAPr,
    
    EPNI_CFG_GLEM_TRAPr,
    
    EPNI_CFG_MTU_TRAPr,
    
    EPNI_CFG_ACC_FRAME_TYPE_TRAPr,
    
    EPNI_CFG_SPLIT_HORIZON_TRAPr,
    
    EPNI_CFG_VSI_MEMBERSHIP_FILTERr,
    
    EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr
};


static soc_field_t etpp_mtu_field_names[ETPP_EPNI_NOF_MTU_PROFILE] = 
{
    LIF_MTU_PROFILE_ZERO, 
    LIF_MTU_1f,
    LIF_MTU_2f,
    LIF_MTU_3f,
    LIF_MTU_4f,
    LIF_MTU_5f,
    LIF_MTU_6f,
    LIF_MTU_7f       
    
};


static soc_field_t etpp_field_names[ETPP_NOF_TRAPS][ETPP_NUMBER_TRAP_FIELDS] =
{
    
    {CFG_EES_ACTION_TRAP_MIRROR_CMDf,       CFG_EES_ACTION_TRAP_FWD_STRENGTHf,      CFG_EES_ACTION_TRAP_MIRROR_STRENGTHf,       CFG_EES_ACTION_TRAP_FWD_ENf,        CFG_EES_ACTION_TRAP_MIRROR_ENf},
    
    {CFG_STP_STATE_TRAP_MIRROR_CMDf,        CFG_STP_STATE_TRAP_FWD_STRENGTHf,       CFG_STP_STATE_TRAP_MIRROR_STRENGTHf,        CFG_STP_STATE_TRAP_FWD_ENf,         CFG_STP_STATE_TRAP_MIRROR_ENf},
    
    {CFG_PROTECTION_PATH_TRAP_MIRROR_CMDf,  CFG_PROTECTION_PATH_TRAP_FWD_STRENGTHf, CFG_PROTECTION_PATH_TRAP_MIRROR_STRENGTHf,  CFG_PROTECTION_PATH_TRAP_FWD_ENf,   CFG_PROTECTION_PATH_TRAP_MIRROR_ENf},
    
    {CFG_GLEM_TRAP_MIRROR_CMDf,             CFG_GLEM_TRAP_FWD_STRENGTHf,            CFG_GLEM_TRAP_MIRROR_STRENGTHf,             CFG_GLEM_TRAP_FWD_ENf,              CFG_GLEM_TRAP_MIRROR_ENf},
    
    {CFG_MTU_TRAP_MIRROR_CMDf,              CFG_MTU_TRAP_FWD_STRENGTHf,             CFG_MTU_TRAP_MIRROR_STRENGTHf,              CFG_MTU_TRAP_FWD_ENf,               CFG_MTU_TRAP_MIRROR_ENf},
    
    {CFG_ACC_FRAME_TYPE_TRAP_MIRROR_CMDf,   CFG_ACC_FRAME_TYPE_TRAP_FWD_STRENGTHf,  CFG_ACC_FRAME_TYPE_TRAP_MIRROR_STRENGTHf,   CFG_ACC_FRAME_TYPE_TRAP_FWD_ENf,    CFG_ACC_FRAME_TYPE_TRAP_MIRROR_ENf},
    
    {CFG_SPLIT_HORIZON_TRAP_MIRROR_CMDf,    CFG_SPLIT_HORIZON_TRAP_FWD_STRENGTHf,   CFG_SPLIT_HORIZON_TRAP_MIRROR_STRENGTHf,    CFG_SPLIT_HORIZON_TRAP_FWD_ENf,     CFG_SPLIT_HORIZON_TRAP_MIRROR_ENf},
    
    {CFG_VSI_MEMBERSHIP_TRAP_MIRROR_CMDf,   CFG_VSI_MEMBERSHIP_TRAP_FWD_STRENGTHf,  CFG_VSI_MEMBERSHIP_TRAP_MIRROR_STRENGTHf,   CFG_VSI_MEMBERSHIP_TRAP_FWD_ENf,    CFG_VSI_MEMBERSHIP_TRAP_MIRROR_ENf},
    
    {CFG_EGRESS_MEMBERSHIP_TRAP_MIRROR_CMDf,   CFG_EGRESS_MEMBERSHIP_TRAP_FWD_STRENGTHf,  CFG_EGRESS_MEMBERSHIP_TRAP_MIRROR_STRENGTHf,   CFG_EGRESS_MEMBERSHIP_TRAP_FWD_ENf,    CFG_EGRESS_MEMBERSHIP_TRAP_MIRROR_ENf}      

};


soc_error_t
soc_jer_pp_eg_pmf_mirror_params_set(int unit, uint32 mirror_profile, dpp_outbound_mirror_config_t *config)
{

    uint32 tbl_data, mirror_index;
    
    int
      table_index_min,
      table_index_max ;

    SOCDNX_INIT_FUNC_DEFS;

    table_index_max = SOC_MEM_INFO(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm).index_max ;
    table_index_min = SOC_MEM_INFO(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm).index_min ;
    mirror_profile += table_index_min ;

    if (mirror_profile <= table_index_max  && mirror_profile >= table_index_min) 
    {
        mirror_index = mirror_profile - table_index_min ;

        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_COMMANDf, (uint32)(config->mirror_command));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_STRENGTHf, (uint32)(config->forward_strength));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_STRENGTHf, (uint32)(config->mirror_strength));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_ENABLEf, (uint32)(config->forward_en));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_ENABLEf, (uint32)(config->mirror_en));
 
        SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          EPNI_PMF_MIRROR_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          mirror_index,
          &tbl_data
        ));
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(
            SOC_E_EXISTS,
            (_BSL_SOCDNX_MSG(
                "Mirror profile (%d) is out of range. Should be between %d and %d"),
                mirror_profile,table_index_min,table_index_max
            )
        ) ;
    }
 
exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_eg_etpp_trap_set(int unit, SOC_PPC_TRAP_ETPP_TYPE trap, SOC_PPC_TRAP_ETPP_INFO *entry_info)
{

    uint32 tbl_data;
    int trap_index;

    SOCDNX_INIT_FUNC_DEFS;

    tbl_data = 0;

    SOCDNX_IF_ERR_EXIT(soc_jer_eg_etpp_verify_parmas(unit, entry_info));
   
    
    if (trap <= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_15  && trap >= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0) 
    {
        trap_index = trap - SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0;


        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_COMMANDf, entry_info->mirror_cmd);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_STRENGTHf, entry_info->fwd_strength);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_STRENGTHf, entry_info->mirror_strength);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_ENABLEf, entry_info->fwd_enable);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_ENABLEf, entry_info->mirror_enable);
 
        SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          EPNI_PMF_MIRROR_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          trap_index,
          &tbl_data
        ));
        
      
    }
    else
    {
        soc_jer_eg_etpp_trap_get_array_index(unit, trap, &trap_index); 
        if (trap_index == -1)
        {
             SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Unknown trap id")));
        }

          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_CMD_INDEX], entry_info->mirror_cmd);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_FWD_STRENGTH_INDEX], entry_info->fwd_strength);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_STRENGTH_INDEX], entry_info->mirror_strength);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_FWD_ENABLE_INDEX], entry_info->fwd_enable);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_ENABLE_INDEX], entry_info->mirror_enable);
          
          SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit,  etpp_reg_names[trap_index], REG_PORT_ANY, 0, tbl_data));
    }
 
exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_etpp_verify_parmas(int unit, SOC_PPC_TRAP_ETPP_INFO *entry_info)
{

    SOCDNX_INIT_FUNC_DEFS;
            
    
    if( entry_info->mirror_enable > 1 || entry_info->fwd_enable > 1 )
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("valid bit must be 0 or 1")));
    }

    
    if( entry_info->mirror_strength > MIRROR_STRENGTH_MAX_VALUE || (int)entry_info->mirror_strength < MIRROR_STRENGTH_MIN_VALUE) 
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Mirror strength range is %d to %d"),MIRROR_STRENGTH_MIN_VALUE, MIRROR_STRENGTH_MAX_VALUE ));
    }

    if( entry_info->fwd_strength > FWD_STRENGTH_MAX_VALUE || (int)entry_info->fwd_strength < FWD_STRENGTH_MIN_VALUE) 
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Forward strength range is %d to %d"),FWD_STRENGTH_MIN_VALUE, FWD_STRENGTH_MIN_VALUE ));
    }

    if( entry_info->mirror_cmd > MIRROR_COMMAND_MAX_VALUE || (int)entry_info->mirror_cmd < MIRROR_COMMAND_MIN_VALUE) 
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Mirror command range is %d to %d"),MIRROR_COMMAND_MIN_VALUE, MIRROR_COMMAND_MAX_VALUE ));
    }


exit:
  SOCDNX_FUNC_RETURN;




}

soc_error_t
soc_jer_eg_etpp_trap_get(int unit, SOC_PPC_TRAP_ETPP_TYPE trap, SOC_PPC_TRAP_ETPP_INFO *entry_info)
{
   
    uint32 tbl_data;
    int trap_index;
    SOCDNX_INIT_FUNC_DEFS;

   tbl_data = 0;

    
    if (trap <= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_15  && trap >= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0) 
    {
        trap_index = trap - SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0;
        
         SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          EPNI_PMF_MIRROR_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          trap_index,
          &tbl_data
        ));

                              
    entry_info->mirror_cmd = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_COMMANDf);
    entry_info->fwd_strength = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_STRENGTHf);
    entry_info->mirror_strength = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_STRENGTHf);
    entry_info->fwd_enable = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_ENABLEf);
    entry_info->mirror_enable = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_ENABLEf);

    }
    else
    {
        soc_jer_eg_etpp_trap_get_array_index(unit, trap, &trap_index);
        
        if (trap_index == -1)
        {
             SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Unknown trap id")));
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit,  etpp_reg_names[trap_index], REG_PORT_ANY, 0, &tbl_data));

        entry_info->mirror_cmd = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data,etpp_field_names[trap_index][ETPP_MIRROR_CMD_INDEX]);
        entry_info->fwd_strength = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_FWD_STRENGTH_INDEX]);
        entry_info->mirror_strength = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_STRENGTH_INDEX]);
        entry_info->fwd_enable = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_FWD_ENABLE_INDEX]);
        entry_info->mirror_enable = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_ENABLE_INDEX]);
    }



exit:
  SOCDNX_FUNC_RETURN;
} 


void
soc_jer_eg_etpp_trap_get_array_index(int unit, SOC_PPC_TRAP_CODE trap, int *trap_index)
{
    
      switch (trap)
      {
      case SOC_PPC_TRAP_CODE_ETPP_OUT_VPORT_DISCARD:
      {
                *trap_index = ETPP_OUT_VPORT_DISCARD_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_STP_STATE_FAIL:
      {
                *trap_index = ETPP_STP_STATE_FAIL_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_PROTECTION_PATH_UNEXPECTED:
      {
                *trap_index = ETPP_PROTECTION_PATH_UNEXPECTED_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_VPORT_LOOKUP_FAIL:
      {
                *trap_index = ETPP_VPORT_LOOKUP_FAIL_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_MTU_FILTER:
      {
          *trap_index = ETPP_MTU_FILTER;
          break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_ACC_FRAME_TYPE:
      {
          *trap_index = ETPP_ACC_FRAME_TYPE;
          break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_SPLIT_HORIZON:
      {
          *trap_index = ETPP_SPLIT_HORIZON;
          break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_PORT_NOT_VLAN_MEMBER:
      {
          if (SOC_IS_JERICHO_PLUS(unit))
          {
            *trap_index = ETPP_PORT_NOT_VLAN_MEMBER_JERICHO_PLUS;
          }
          else
          {
            *trap_index = ETPP_PORT_NOT_VLAN_MEMBER;
          }
          break;
      }      
      default:
                *trap_index = -1;
          
      } 
}


soc_error_t soc_jer_eg_etpp_out_lif_mtu_map_set(int unit,uint32 out_lif_profile_bit_mask, uint32 mtu_profile, uint32 mtu_val)
{
    soc_reg_above_64_val_t data_above_64;
    uint32 out_lif_profile_to_set = 0;
    uint32 out_lif_profile_bit_map_mtu_profile_ones = 0;
    uint32 out_lif_profile_bit_map_mtu_profile_zeroes = 0;
    int bit = 0;
    int bit_of_mtu = 0;
    SOCDNX_INIT_FUNC_DEFS;
    
    if(mtu_profile >= ETPP_EPNI_NOF_MTU_PROFILE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid mtu profile")));
    }

    
    
    if(mtu_profile  != LIF_MTU_PROFILE_ZERO) 
    {
        SOC_REG_ABOVE_64_CLEAR(data_above_64);

        SOCDNX_IF_ERR_EXIT(
        soc_reg_above_64_get(unit, EPNI_LIF_MTUr, REG_PORT_ANY, 0, data_above_64));
        soc_reg_above_64_field32_set(unit,EPNI_LIF_MTUr,data_above_64,etpp_mtu_field_names[mtu_profile],mtu_val);
        SOCDNX_IF_ERR_EXIT(
        soc_reg_above_64_set(unit,  EPNI_LIF_MTUr, REG_PORT_ANY, 0, data_above_64));
    }

    
    for(bit = 0 ; bit < LIF_MTU_OUTLIF_PROFILE_NOF_BITS; bit++)
    {
        
        if(out_lif_profile_bit_mask & (1<<bit))
        {
            if(mtu_profile & (1 << bit_of_mtu))
            {
                out_lif_profile_bit_map_mtu_profile_ones |= (1 <<bit);
            }
            else
            {
                out_lif_profile_bit_map_mtu_profile_zeroes |= (1 <<bit);
            }
            bit_of_mtu++;
            if(bit_of_mtu > LIF_MTU_NOF_BITS_MTU_PROFILE)
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid out_lif_profile_mask")));
            }
        }
    }
    
    for(out_lif_profile_to_set = 0 ; out_lif_profile_to_set < LIF_MTU_OUTLIF_PROFILE_SIZE; out_lif_profile_to_set++)
    {
        if(((out_lif_profile_to_set & out_lif_profile_bit_map_mtu_profile_ones) == out_lif_profile_bit_map_mtu_profile_ones) &&
            ((out_lif_profile_to_set & out_lif_profile_bit_map_mtu_profile_zeroes) == 0))
        {
            
            SOCDNX_IF_ERR_EXIT(
            soc_reg_above_64_get(unit, EPNI_LIF_MTU_PROFILE_MAPPINGr, REG_PORT_ANY, 0, data_above_64));

            
            SHR_BITCOPY_RANGE(data_above_64,                                    
                             out_lif_profile_to_set*ETPP_EPNI_NOF_MTU_PROFILES_BIT,    
                             &mtu_profile,                                      
                             0,                                                 
                             ETPP_EPNI_NOF_MTU_PROFILES_BIT                     
                             );
            
            
            SOCDNX_IF_ERR_EXIT(
            soc_reg_above_64_set(unit, EPNI_LIF_MTU_PROFILE_MAPPINGr, REG_PORT_ANY,0, data_above_64));
        }
    } 
    
exit:
      SOCDNX_FUNC_RETURN;

}


soc_error_t soc_jer_eg_etpp_out_lif_mtu_map_get(int unit,uint32 out_lif_profile, uint32 *mtu_val)
{
   soc_reg_above_64_val_t data_above_64;
   uint32 mtu_profile = 0;
   SOCDNX_INIT_FUNC_DEFS;

   
    SOCDNX_IF_ERR_EXIT(
    soc_reg_above_64_get(unit, EPNI_LIF_MTU_PROFILE_MAPPINGr, REG_PORT_ANY, 0, data_above_64));

   
   
    SHR_BITCOPY_RANGE(&mtu_profile,                                    
                     0,                                                
                     data_above_64,                                    
                     out_lif_profile*ETPP_EPNI_NOF_MTU_PROFILES_BIT,   
                     ETPP_EPNI_NOF_MTU_PROFILES_BIT                    
                     );

    
    if(mtu_profile != 0)
    {
        SOCDNX_IF_ERR_EXIT(
        soc_reg_above_64_get(unit, EPNI_LIF_MTUr, REG_PORT_ANY, 0, data_above_64));
        *mtu_val = soc_reg_above_64_field32_get(unit,EPNI_LIF_MTUr,data_above_64,etpp_mtu_field_names[mtu_profile]);
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("The Out-LIF was not mapped to MTU value")));
    }
   
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t soc_jer_eg_etpp_out_lif_mtu_check_set(int unit,uint32 enable)
{
    soc_reg_above_64_val_t data_above_64;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOCDNX_IF_ERR_EXIT(
    soc_reg_above_64_get(unit, EPNI_LIF_MTUr, REG_PORT_ANY, 0, data_above_64));
    soc_reg_above_64_field32_set(unit,EPNI_LIF_MTUr,data_above_64,LIF_MTU_ENABLEf,enable);
    SOCDNX_IF_ERR_EXIT(
    soc_reg_above_64_set(unit,  EPNI_LIF_MTUr, REG_PORT_ANY, 0, data_above_64));
exit:
    SOCDNX_FUNC_RETURN; 
}





#include <soc/dpp/SAND/Utils/sand_footer.h>

