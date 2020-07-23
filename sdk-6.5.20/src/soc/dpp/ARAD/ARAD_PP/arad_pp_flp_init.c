/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>




#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <bcm_int/common/debug.h>
#include <soc/dcmn/utils.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/arad_tcam.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_dbal.h>

#include <soc/register.h>

#include <soc/dpp/ARAD/arad_general.h>

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#endif






#define ARAD_PP_FLP_INIT_PRINT_ADVANCE                                                               soc_sand_os_printf

#define ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP         (0x060)
#define ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID    (0x018)
#define ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP     (0x004)
#define ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID    (0x003)
#define ARAD_PP_FLP_QLFR_ETH_MASK_NEXT_PROTOCOL (0x780)


#define ARAD_PP_FLP_QLFR_ETH_NEXT_PROTOCOL_IPV4 (0xD << 7)


#define ARAD_PP_FLP_LKP_KEY_SELECT_A_KEY_HW_VAL  (0x0)
#define ARAD_PP_FLP_LKP_KEY_SELECT_B_KEY_HW_VAL  (0x1)

#define ARAD_PP_FLP_INSTRUCTION_0_16B 0
#define ARAD_PP_FLP_INSTRUCTION_1_16B 1
#define ARAD_PP_FLP_INSTRUCTION_2_16B 2
#define ARAD_PP_FLP_INSTRUCTION_3_32B 3
#define ARAD_PP_FLP_INSTRUCTION_4_32B 4
#define ARAD_PP_FLP_INSTRUCTION_5_32B 5

#define ARAD_PP_FLP_INSTRUCTION_0_TO_5 0
#define ARAD_PP_FLP_INSTRUCTION_6_TO_11 1

#define ARAD_PP_FLP_SERVICE_TYPE_AC_P2P_AC2AC  0
#define ARAD_PP_FLP_SERVICE_TYPE_AC_P2P_AC2PWE 1
#define ARAD_PP_FLP_SERVICE_TYPE_AC_P2P_AC2PBB 2
#define ARAD_PP_FLP_SERVICE_TYPE_AC_MP 3
#define ARAD_PP_FLP_SERVICE_TYPE_ISID_P2P 4
#define ARAD_PP_FLP_SERVICE_TYPE_ISID_MP 5
#define ARAD_PP_FLP_SERVICE_TYPE_TRILL_IP_PWE_VRF_LSP_MP 6
#define ARAD_PP_FLP_SERVICE_TYPE_PWE_P2P 7

#define ARAD_PP_FLP_SERVICE_TYPE_MPLS_P2P 7

#define ARAD_PP_FLP_SERVICE_TYPE_MPLS_EXP_NULL_TTL_REJECT ARAD_PP_FLP_SERVICE_TYPE_ISID_P2P




#define ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(bitmap,index) \
  { \
    if(!COMPILER_64_BITTEST(bitmap,index) && (index<SOC_DPP_DEFS_GET(unit, nof_flp_programs))) { \
        COMPILER_64_BITSET(bitmap,index); \
    } \
    else { \
        SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 5500, exit); \
    } \
  }

#define ARAD_PP_FLP_SW_DB_FLP_PROGRAM_ID_SET(unit, prog_id, app_id) \
  { \
    if ((prog_id) >= (SOC_DPP_DEFS_GET(unit, nof_flp_programs))) \
    { \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 8100, exit); \
    } \
    res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.set((unit), (prog_id), (app_id)); \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit); \
  }

#define ARAD_PP_FLP_SW_DB_FLP_PROGRAM_ID_GET(unit, prog_id, app_id) \
  { \
    if ((prog_id) >= (SOC_DPP_DEFS_GET(unit, nof_flp_programs))) \
    { \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 8100, exit); \
    } \
    res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get((unit), (prog_id), (&app_id)); \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit); \
  }

#define SEPARATE_IPV4_IPV6_RPF_ENABLE ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "separate_ipv4_ipv6_rpf_enable", 0)))





typedef struct
{
    
    uint32 instruction;

    
    uint8 is_inst_6_11;

    
    uint8 id;

} ARAD_FLP_CUSTOM_LPM_INSTRUCTION_INFO;

typedef struct
{
    
    uint8 key;

    
    uint8 key_or_value;

    
    uint8 inst_num;

    
    ARAD_FLP_CUSTOM_LPM_INSTRUCTION_INFO inst[12];

} ARAD_FLP_CUSTOM_LPM_KEY_CONSTR;

typedef struct
{
    
    uint8 app_id;

    
    uint8 lookup_num;

    
    uint8 lookup_to_use;

    
    ARAD_FLP_CUSTOM_LPM_KEY_CONSTR key[2];

} ARAD_FLP_CUSTOM_LPM_LOOKUP_INFO;






STATIC uint32 is_pon_dpp_support = 0;

STATIC
    ARAD_FLP_CUSTOM_LPM_LOOKUP_INFO
        Arad_flp_custom_lpm_lookup_info[] = {
#if 0
        {
          PROG_FLP_ETHERNET_MAC_IN_MAC,
          
          2, 0,
              {
                  { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  }
              }
        },
        {
          PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM,
          
          2, 0,
              {
                  { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  }
              }
        },
#endif
        {
          PROG_FLP_ETHERNET_ING_LEARN,
          
          2, 0,
              {
                  { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  }
              }
        },
        {
          PROG_FLP_IPV4UC_RPF,
          
          2, 0,
              {
                  { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  }
              }
        },
        {
          PROG_FLP_IPV4UC,
          
          2, 0,
              {
                  { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  }
              }
        },
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        {
          PROG_FLP_IPV6UC_RPF,
          
          2, 0,
              {
                  { 1, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  } ,
                  { 1, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  }
              }
         },
#endif
        {
          PROG_FLP_IPV6UC,
          
          2, 0,
              {
                  { 1, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  } ,
                  { 1, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  }
              }
         },
         {
           PROG_FLP_LSR,
           
           2, 0,
              {
                  { 1, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  } ,
                  { 1, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  }
              }
          },
          {
            PROG_FLP_IPV4COMPMC_WITH_RPF,
            
            2, 0,
              {
                  { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  }
              }
          },
          {
            PROG_FLP_IPV6MC,
            
            2, 0,
              {
                  { 1, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  } ,
                  { 1, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  }
              }
          },
          {
            PROG_FLP_P2P,
            
            2, 0,
              {
                  { 1, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  } ,
                  { 1, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_2_16B},
                      }
                  }
              }
          },
          {
            PROG_FLP_IPV4UC_PUBLIC,
            
            2, 0,
              {
                  { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_0_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_0_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_0_TO_5, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  }
              }
          },
          {
            PROG_FLP_IPV4MC_BRIDGE,
            
            2, 0,
              {
                 { 2, 0, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B}, 
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  } ,
                  { 2, 2, 2, 
                      {
                          { ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_0_16B},
                          { ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D, ARAD_PP_FLP_INSTRUCTION_6_TO_11, ARAD_PP_FLP_INSTRUCTION_1_16B},
                      }
                  }
              }
          },
    };

uint8 sip_sharing_uc[BCM_MAX_NUM_UNITS] = { 0 };
uint8 sip_sharing_uc_public[BCM_MAX_NUM_UNITS] = { 0 };
uint8 sip_sharing_uc_rpf[BCM_MAX_NUM_UNITS] = { 0 };
uint8 sip_sharing_uc_public_rpf[BCM_MAX_NUM_UNITS] = { 0 };
uint8 sip_sharing_mc[BCM_MAX_NUM_UNITS] = { 0 };






#if defined(INCLUDE_KBP)
STATIC uint32 first_lem_lkup_sa_fid_search_set(int unit, ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA* flp_lookups_tbl){
	if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_learn_force_in_fwd", 0)) {
	   
	   flp_lookups_tbl->lem_1st_lkp_valid     = 1;
	   flp_lookups_tbl->lem_1st_lkp_key_select = 3; 
	   flp_lookups_tbl->lem_1st_lkp_key_type   = 0x1; 
	   flp_lookups_tbl->lem_1st_lkp_and_value  = 0x0;
	   flp_lookups_tbl->lem_1st_lkp_or_value   = 0x0;
	}
	return BCM_E_NONE;
}
#else
#define first_lem_lkup_sa_fid_search_set(unit,flp_lookups_tbl) BCM_E_NONE
#endif


#if defined(INCLUDE_KBP)
STATIC uint32 process_tbl_learn_enable_set(int unit, ARAD_PP_IHB_FLP_PROCESS_TBL_DATA *flp_process_tbl) {
	if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_learn_force_in_fwd", 0)) {
		flp_process_tbl->learn_enable = 0x1;
	}
	return BCM_E_NONE;
}
#else
#define process_tbl_learn_enable_set(unit,flp_process_tbl) BCM_E_NONE
#endif

STATIC uint32
   arad_pp_flp_pwe_gre_progs_init(
     int unit
   );

STATIC uint32
   arad_pp_flp_fcoe_progs_init(
     int unit
   );

STATIC uint32
   arad_pp_flp_ipmc_bidir_progs_init(
     int unit
   );

STATIC uint32
   arad_pp_flp_vmac_progs_init(
     int unit,
    uint8 sa_auth_enabled,
    uint8 slb_enabled
   );

#if defined(INCLUDE_KBP)
STATIC uint32
   arad_pp_flp_ipv6uc_with_rpf_prog_init(
     int unit
   );
#endif

STATIC uint32
  arad_pp_flp_pon_arp_prog_init(
    int unit,
    uint8 sa_auth_enabled,
    uint8 slb_enabled
  );

STATIC uint32
  arad_pp_flp_lpm_custom_lookup_enable(
     int unit,
     uint8 app_id
   );

STATIC uint32 arad_pp_flp_program_disable(
        int unit,
        uint32 prog_id
   )
{
  uint32
    res,
    indx;

  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA    prog_selection_cam_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (indx = 0; indx < SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines); ++indx) {
      res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, indx, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (prog_selection_cam_tbl.program == prog_id) {
          if (prog_selection_cam_tbl.valid != 0) {
              prog_selection_cam_tbl.valid = 0;
              res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, indx, &prog_selection_cam_tbl);
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
              break;
          }
      }
  }
  if (indx == SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_program_disable", 0, 0);
}


STATIC uint32 arad_pp_flp_set_app_id_and_get_cam_sel(
   int    unit,
   uint32 sw_prog_id,
   int8   is_static_prog,
   int8   update_hw_prog_id,
   int32 *cam_sel_id,
   int32 *hw_prog_id)
{
  int i;
  uint32
      res;
  int nof_flp_hw_programs = SOC_DPP_DEFS_GET(unit, nof_flp_programs);
  int nof_flp_selections  = SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines);

  uint64 hw_prog_bitmap;
  uint64 prog_sel_bitmap;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(cam_sel_id);

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_hw_prog_id_bitmap.get(unit,&hw_prog_bitmap);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_prog_select_id_bitmap.get(unit,&prog_sel_bitmap);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (update_hw_prog_id) {
      if (is_static_prog) {
          if (!COMPILER_64_BITTEST(hw_prog_bitmap, sw_prog_id)) {
              
              SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 5500, exit);
          }
          ARAD_PP_FLP_SW_DB_FLP_PROGRAM_ID_SET(unit, sw_prog_id, sw_prog_id);
      }
      else {
          SOC_SAND_CHECK_NULL_INPUT(hw_prog_id);
          for (i = 0; i < nof_flp_hw_programs; i++) {
              if (!COMPILER_64_BITTEST(hw_prog_bitmap,i)) {
                  COMPILER_64_BITSET(hw_prog_bitmap,i);
                  *hw_prog_id = i;
                  break;
              }
              if (i == nof_flp_hw_programs-1) {
                  
                  SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 5500, exit);
              }
          }
          ARAD_PP_FLP_SW_DB_FLP_PROGRAM_ID_SET(unit, *hw_prog_id, sw_prog_id);
      }
  }

  
  for (i = 0; i < nof_flp_selections; i++) {
      if (!COMPILER_64_BITTEST(prog_sel_bitmap,i)) {
          COMPILER_64_BITSET(prog_sel_bitmap,i);
          *cam_sel_id = i;
          break;
      }
      if (i == nof_flp_selections-1) {
          
          SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 5500, exit);
      }
  }

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_hw_prog_id_bitmap.set(unit,hw_prog_bitmap);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_prog_select_id_bitmap.set(unit,prog_sel_bitmap);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_set_app_id_and_get_cam_sel", sw_prog_id, 0);
}


STATIC uint32 arad_pp_flp_get_cam_sel_list_by_prog_id(
   int    unit,
   uint32 prog_id,
   uint64 *cam_sel_list,
   int    *nof_selections)
{
  int i, counter;
  uint32
      res;
  int nof_flp_selections  = SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines);
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(cam_sel_list);
  SOC_SAND_CHECK_NULL_INPUT(nof_selections);

  counter = 0;
  for (i = 0; i < nof_flp_selections; i++) {
      res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, i, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if ((prog_selection_cam_tbl.program == prog_id)&&(prog_selection_cam_tbl.valid == 1)) {
          COMPILER_64_BITSET(*cam_sel_list, i);
          counter++;
      }
  }

  *nof_selections = counter;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_get_cam_sel_list_by_prog_id", prog_id, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_key_program_tm(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_DPP_PP_ENABLE(unit)) {
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_TM,TRUE,TRUE,&cam_sel_id,NULL);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } else {
      cam_sel_id = PROG_FLP_TM;
  }

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_TM,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);


  prog_selection_cam_tbl.program = PROG_FLP_TM;
  prog_selection_cam_tbl.valid = 1;

  if (SOC_IS_ARADPLUS_A0(unit) && SOC_DPP_CONFIG(unit)->pp.oam_statistics){
      prog_selection_cam_tbl.vt_lookup_0_found =0;
      prog_selection_cam_tbl.vt_lookup_0_found_mask =0;
      prog_selection_cam_tbl.vt_lookup_1_found =0;
      prog_selection_cam_tbl.vt_lookup_1_found_mask =0;
  }


  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_key_program_tm", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_vpws_tagged(
     int unit
   )
{
    uint32 res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;
    int prog_id;
    uint8 is_single_tag;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (is_single_tag = 1; is_single_tag!=(uint8)(-1); is_single_tag--) {
        
        prog_id = is_single_tag ? PROG_FLP_VPWS_TAGGED_SINGLE_TAG:PROG_FLP_VPWS_TAGGED_DOUBLE_TAG;
        res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_id,TRUE,TRUE,&cam_sel_id,NULL);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                           &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
        } else {
            res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                        &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
        }
        prog_selection_cam_tbl.service_type = ARAD_PP_FLP_SERVICE_TYPE_PWE_P2P;
        prog_selection_cam_tbl.service_type_mask = 0;
        prog_selection_cam_tbl.port_profile = 0;
        prog_selection_cam_tbl.ptc_profile  = 0;
        prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
        prog_selection_cam_tbl.ptc_profile_mask  = 0x03;
        prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
        prog_selection_cam_tbl.forwarding_code_mask = 0x00;
        prog_selection_cam_tbl.cos_profile_mask = 0x3F;
        prog_selection_cam_tbl.valid = 1;

        if (is_single_tag) {
            
            
            
            prog_selection_cam_tbl.forwarding_header_qualifier = 0; 
            prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                                   ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_FLP_QLFR_ETH_MASK_NEXT_PROTOCOL);
        }

        prog_selection_cam_tbl.program = prog_id;

        res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit,cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_vpws_tagged", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_p2p(
     int unit
   )
{
    uint32 res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;
    uint32 i = 0;
    int service_type[4]      = {ARAD_PP_FLP_SERVICE_TYPE_AC_P2P_AC2AC, 
                                ARAD_PP_FLP_SERVICE_TYPE_AC_P2P_AC2PBB,
                                ARAD_PP_FLP_SERVICE_TYPE_ISID_P2P,
                                ARAD_PP_FLP_SERVICE_TYPE_PWE_P2P};
    int service_type_mask[4] = {1, 0, 0, 0};
    
    int num_of_selections = (ARAD_PP_FLP_PON_PROG_LOAD() ? 1 : 4);
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if defined(INCLUDE_KBP)

    if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
        num_of_selections = 1;
    }
#endif

    for (i = 0; i < num_of_selections; i++) {
        if (i == 0) {
            res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_P2P, TRUE, TRUE, &cam_sel_id, NULL);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        else {
            res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_P2P+i, FALSE, FALSE, &cam_sel_id, NULL);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }

        res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                           &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
        } else {
            res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                        &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
        }
        prog_selection_cam_tbl.service_type = service_type[i];
        prog_selection_cam_tbl.service_type_mask = service_type_mask[i];
        prog_selection_cam_tbl.port_profile = 0;
        prog_selection_cam_tbl.ptc_profile  = 0;
        prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
        prog_selection_cam_tbl.ptc_profile_mask  = 0x03;
        prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
        prog_selection_cam_tbl.forwarding_code_mask = 0x00;
        prog_selection_cam_tbl.cos_profile_mask = 0x3F;
        prog_selection_cam_tbl.program = PROG_FLP_P2P;
        prog_selection_cam_tbl.valid = 1;

        if ((service_type[i] == ARAD_PP_FLP_SERVICE_TYPE_PWE_P2P) && soc_property_get(unit, spn_VPWS_TAGGED_MODE, 0)) {
            
            prog_selection_cam_tbl.forwarding_header_qualifier = 0; 
            prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_FLP_QLFR_ETH_MASK_NEXT_PROTOCOL);
        }
        res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_p2p", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_mpls_p2p(
     int unit
   )
{
    uint32 res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if defined(INCLUDE_KBP)

    if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
        res = SOC_SAND_OK;
        goto exit;
    }
#endif
    if (ARAD_PP_FLP_PON_PROG_LOAD()) {
        res = SOC_SAND_OK;
        goto exit;
    }

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_P2P, FALSE, FALSE, &cam_sel_id, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
        res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                       &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
    } else {
        res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                        &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
    }
    prog_selection_cam_tbl.service_type = ARAD_PP_FLP_SERVICE_TYPE_MPLS_P2P;
    prog_selection_cam_tbl.service_type_mask = 0;
    prog_selection_cam_tbl.port_profile = 0;
    prog_selection_cam_tbl.ptc_profile  = 0;
    prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
    prog_selection_cam_tbl.ptc_profile_mask  = 0x03;
    prog_selection_cam_tbl.forwarding_code = 0;
    prog_selection_cam_tbl.forwarding_code_mask = 0xF;
    prog_selection_cam_tbl.cos_profile_mask = 0x3F;
    prog_selection_cam_tbl.program = PROG_FLP_P2P;
    prog_selection_cam_tbl.valid = 1;

    prog_selection_cam_tbl.forwarding_header_qualifier = 0;
    prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP |
                                                               ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                               ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP |
                                                               ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID |
                                                               ARAD_PP_FLP_QLFR_ETH_MASK_NEXT_PROTOCOL);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_mpls_p2p", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_mpls_drop_unmatch_ces_packet(
     int unit
   )
{
    uint32 res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;
    int32 cam_sel_id;
    bcm_port_t port;
    SOC_PPC_PORT soc_ppd_port = 0;
    pbmp_t ports_map;
    int core;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if defined(INCLUDE_KBP)

    if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
        res = SOC_SAND_OK;
        goto exit;
    }
#endif
    if (ARAD_PP_FLP_PON_PROG_LOAD() || !SOC_IS_JERICHO(unit)) {
        res = SOC_SAND_OK;
        goto exit;
    }
    BCM_PBMP_ASSIGN(ports_map, PBMP_E_ALL(unit));
    BCM_PBMP_ITER(ports_map, port) {
        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core));
        SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);
        if ((soc_property_port_suffix_num_get(unit, soc_ppd_port, -1, spn_CUSTOM_FEATURE, "drop_unmatched_ces_packet", -1) == 1)) {
            res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_TM, FALSE, FALSE, &cam_sel_id, NULL);
            SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);

            res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
            SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
            if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
                res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                               &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
                SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
            } else {
                res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                                &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
                SOC_SAND_CHECK_FUNC_RESULT(res, 340, exit);
            }
            prog_selection_cam_tbl.service_type = ARAD_PP_FLP_SERVICE_TYPE_AC_MP;
            prog_selection_cam_tbl.service_type_mask = 0;
            prog_selection_cam_tbl.port_profile = ARAD_PP_FLP_PORT_PROFILE_CES;
            prog_selection_cam_tbl.ptc_profile  = 0;
            prog_selection_cam_tbl.port_profile_mask = 0;
            prog_selection_cam_tbl.ptc_profile_mask  = 0x03;
            prog_selection_cam_tbl.forwarding_code = 5;
            prog_selection_cam_tbl.forwarding_code_mask = 0;
            prog_selection_cam_tbl.cos_profile_mask = 0x3F;
            prog_selection_cam_tbl.program = PROG_FLP_TM;
            prog_selection_cam_tbl.valid = 1;

            prog_selection_cam_tbl.forwarding_header_qualifier = 0;
            prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP |
                                                                       ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                                       ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP |
                                                                       ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID |
                                                                       ARAD_PP_FLP_QLFR_ETH_MASK_NEXT_PROTOCOL);

            res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
            SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);
            break;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_mpls_p2p", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_tk_epon_uni_v4(
     int unit
   )
{
  uint32
    res,
    program_id;
  int
    i;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (i = 0; i < 2; i++) { 
    
    if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable && (i==0)) {
      continue;
    }
    program_id = (i == 0) ? PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP:PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC;

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,program_id,TRUE,TRUE,&cam_sel_id,NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_IPV4,
                &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
    res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_IPV4, DPP_PLC_ANY_PP,
                &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

    prog_selection_cam_tbl.port_profile = 0x0;
    prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
    prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_PON;
    prog_selection_cam_tbl.ptc_profile_mask = 0x0;
    prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
    prog_selection_cam_tbl.forwarding_code_mask = 0x00;
    prog_selection_cam_tbl.cos_profile = SOC_PPC_FLP_COS_PROFILE_ANTI_SPOOFING;
    prog_selection_cam_tbl.cos_profile_mask = 0x1F; 
    prog_selection_cam_tbl.tt_lookup_0_found = (program_id == PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP) ? 0x1:0x0;
    prog_selection_cam_tbl.tt_lookup_0_found_mask = 0x0;
    prog_selection_cam_tbl.program = program_id;
    prog_selection_cam_tbl.valid = 1;
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_tk_epon_uni_v4", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ethernet_tk_epon_uni_v6(
     int unit
   )
{
  uint32
    res,
    program_id;
  int
    i;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (i = 0; i < 2; i++) { 
    
    if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable && (i==0)) {
      continue;
    }
    program_id = (i == 0) ? PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP:PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC;

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,program_id,TRUE,TRUE,&cam_sel_id, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_IPV6,
                &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
    res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_IPV6, DPP_PLC_ANY_PP,
                &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

    prog_selection_cam_tbl.port_profile = 0x0;
    prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
    prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_PON;
    prog_selection_cam_tbl.ptc_profile_mask = 0x0;
    prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
    prog_selection_cam_tbl.forwarding_code_mask = 0x00;
    prog_selection_cam_tbl.cos_profile = SOC_PPC_FLP_COS_PROFILE_ANTI_SPOOFING;
    prog_selection_cam_tbl.cos_profile_mask = 0x1F; 
    if (program_id == PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP){
      prog_selection_cam_tbl.tt_lookup_0_found = 0x1;
      prog_selection_cam_tbl.tt_lookup_0_found_mask = 0x0;
    } else {
      if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
        prog_selection_cam_tbl.tt_lookup_1_found = 0x1;
        prog_selection_cam_tbl.tt_lookup_1_found_mask = 0x0;
      } else {
        prog_selection_cam_tbl.tt_lookup_0_found = 0x0;
        prog_selection_cam_tbl.tt_lookup_0_found_mask = 0x0;
      }
    }

    prog_selection_cam_tbl.program = program_id;
    prog_selection_cam_tbl.valid = 1;
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_tk_epon_uni_v6", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_pon_arp_downstream(
    int unit,
    int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_PON_ARP_DOWNSTREAM,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_NONE;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.ll_is_arp        = 1;
  prog_selection_cam_tbl.cos_profile      = 0x0;
  prog_selection_cam_tbl.port_profile_mask = 0x00;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.ll_is_arp_mask        = 0;
  prog_selection_cam_tbl.cos_profile_mask = 0x3F; 
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.port_profile = 0x0;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_pon_arp_downstream", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_pon_arp_upstream(
    int unit,
    int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_PON_ARP_UPSTREAM,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_PON;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.ll_is_arp        = 1;
  prog_selection_cam_tbl.cos_profile = SOC_PPC_FLP_COS_PROFILE_ANTI_SPOOFING;
  prog_selection_cam_tbl.port_profile_mask = 0x00;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.ll_is_arp_mask        = 0;
  prog_selection_cam_tbl.cos_profile_mask = 0x1F; 
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.port_profile = 0x0;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;


  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_pon_arp_upstream", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_ing_learn(
     int unit,
     uint8 mac_in_mac_enabled,
     int32* mac_in_mac_prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_LEARN,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = 0;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  
  prog_selection_cam_tbl.cos_profile_mask = 0x00;
  prog_selection_cam_tbl.cos_profile_mask = 0x3F;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = PROG_FLP_ETHERNET_ING_LEARN;
  if (ARAD_PP_FLP_IBC_PORT_ENABLE(unit)) {
      prog_selection_cam_tbl.ptc_profile      = SOC_TMC_PORTS_FLP_PROFILE_IBC;
      prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  }
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (mac_in_mac_enabled) {

      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM,FALSE,TRUE,&cam_sel_id, mac_in_mac_prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
          res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                         &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
          SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
      } else {
          res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                      &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
          SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
      }
      prog_selection_cam_tbl.port_profile = ARAD_PP_FLP_PORT_PROFILE_PBP; 
      prog_selection_cam_tbl.ptc_profile  = 0;
      prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
      prog_selection_cam_tbl.forwarding_offset_index = 2;
      prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
      prog_selection_cam_tbl.ptc_profile_mask = 0x03;
      prog_selection_cam_tbl.forwarding_code_mask = 0x00;
      prog_selection_cam_tbl.forwarding_offset_index_mask = 0;
      prog_selection_cam_tbl.program = (*mac_in_mac_prog_id);
      prog_selection_cam_tbl.valid = 1;
      res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_ing_learn", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_mpls_control_word_trap(
     int unit,
     uint8 mac_in_mac_enabled,
     int32* mac_in_mac_prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_MPLS_CONTROL_WORD_TRAP,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = 0;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.cpu_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP;
  prog_selection_cam_tbl.cpu_trap_code_mask = 0x0;

  
  prog_selection_cam_tbl.cos_profile_mask = 0x00;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = PROG_FLP_MPLS_CONTROL_WORD_TRAP;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (mac_in_mac_enabled) {
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM,FALSE,TRUE,&cam_sel_id, mac_in_mac_prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
          res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                         &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
          SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
      } else {
          res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                      &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
          SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
      }
      prog_selection_cam_tbl.port_profile = ARAD_PP_FLP_PORT_PROFILE_PBP; 
      prog_selection_cam_tbl.ptc_profile  = 0;
      prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
      prog_selection_cam_tbl.forwarding_offset_index = 2;
      prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
      prog_selection_cam_tbl.ptc_profile_mask = 0x03;
      prog_selection_cam_tbl.forwarding_code_mask = 0x00;
      prog_selection_cam_tbl.forwarding_offset_index_mask = 0;
      prog_selection_cam_tbl.program = (*mac_in_mac_prog_id);
      prog_selection_cam_tbl.valid = 1;
      res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_ing_learn", 0, 0);
}


uint32
   arad_pp_flp_prog_mask_map_ivl_learn(
       int unit,
       uint32 *o_mask,
       uint32 *o_val
   )
{
    int res = SOC_E_NONE;
    SHR_BITDCL occ_mask = 0, occ_mask_flipped = 0;
    uint32 val = *o_val;
	uint32
            int_o_val[1] = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *o_val = 0;
    *o_mask = 0;
    res = arad_pp_occ_mgmt_tcam_args_get(unit,SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_APP_USER, val, int_o_val, &occ_mask, &occ_mask_flipped);
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

    *o_mask = occ_mask_flipped;
	*o_val = int_o_val[0];


    return res;

    exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_mask_map_ivl_learn", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_learn(
     int unit,
     int32 *prog_id
   )
{
    uint32
    res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
    uint32 in_lif_profile_mask, in_lif_profile_val;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_IVL_LEARN,FALSE,TRUE,&cam_sel_id, prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

    in_lif_profile_val = SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile;
    res = arad_pp_flp_prog_mask_map_ivl_learn(unit, &in_lif_profile_mask, &in_lif_profile_val);

    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "[%d ; %x] in_lif_profile_mask, [%d ; %x] in_lif_profile_val\n"),
                         in_lif_profile_mask, in_lif_profile_mask, in_lif_profile_val, in_lif_profile_val));
    if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
        res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                       &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
    } else {
        res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                    &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
    }
    prog_selection_cam_tbl.port_profile = 0;
    prog_selection_cam_tbl.ptc_profile  = 0;
    prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
    prog_selection_cam_tbl.in_lif_profile = in_lif_profile_val;  
    
    prog_selection_cam_tbl.cos_profile_mask = 0x00;
    prog_selection_cam_tbl.cos_profile_mask = 0x3F;
    prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
    prog_selection_cam_tbl.ptc_profile_mask = 0x03;
    prog_selection_cam_tbl.forwarding_code_mask = 0x00;
    prog_selection_cam_tbl.in_lif_profile_mask = in_lif_profile_mask;   
    prog_selection_cam_tbl.program = *prog_id ; 
    prog_selection_cam_tbl.valid = 1;
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_learn", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_learn(
     int unit,
     int32 prog_id   
   )
{
    uint32
      res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
      prog_selection_cam_tbl;
    uint32 in_lif_profile_mask, in_lif_profile_val;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    in_lif_profile_val = SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile;
    res = arad_pp_flp_prog_mask_map_ivl_learn(unit, &in_lif_profile_mask, &in_lif_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "[%d ; %x] in_lif_profile_mask, [%d ; %x] in_lif_profile_val\n"),
                         in_lif_profile_mask, in_lif_profile_mask, in_lif_profile_val, in_lif_profile_val));

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_id,FALSE,FALSE,&cam_sel_id, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
    if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
        res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                    &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
    } else {
        res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY,
                    &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
        res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                    &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
    }

    prog_selection_cam_tbl.port_profile                   = 0;
    prog_selection_cam_tbl.ptc_profile                    = 0;
    prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_ETHERNET;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable           = 0x0;
    prog_selection_cam_tbl.packet_is_compatible_mc        = 1;

    
    prog_selection_cam_tbl.forwarding_header_qualifier      = ARAD_PP_FLP_QLFR_ETH_NEXT_PROTOCOL_IPV4;
    prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                                   ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID);

    prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
    prog_selection_cam_tbl.ptc_profile_mask               = 0x03;
    prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1; 
    prog_selection_cam_tbl.packet_is_compatible_mc_mask   = 0x0; 

    
    prog_selection_cam_tbl.in_lif_profile = in_lif_profile_val;  
    prog_selection_cam_tbl.in_lif_profile_mask = in_lif_profile_mask;   

    prog_selection_cam_tbl.program                        = prog_id; 
    prog_selection_cam_tbl.valid                          = 1;

    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_learn", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_inner_learn(
     int unit,
     int32 *prog_id
   )
{
    uint32
      res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
      prog_selection_cam_tbl;
    int idx;
    uint32 in_lif_profile_mask, in_lif_profile_val;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    in_lif_profile_val = SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile;
    res = arad_pp_flp_prog_mask_map_ivl_learn(unit, &in_lif_profile_mask, &in_lif_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "[%d ; %x] in_lif_profile_mask, [%d ; %x] in_lif_profile_val\n"),
                         in_lif_profile_mask, in_lif_profile_mask, in_lif_profile_val, in_lif_profile_val));

    for (idx=0;idx<2;idx++) {
        

        if (idx == 0) {
            res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_IVL_INNER_LEARN,FALSE,TRUE,&cam_sel_id, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        else {
            res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_IVL_INNER_LEARN,FALSE,FALSE,&cam_sel_id, NULL);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                             &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
        } else {
            res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                        &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
        }
        prog_selection_cam_tbl.port_profile = 0;
        prog_selection_cam_tbl.ptc_profile  = 0;
        prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
        prog_selection_cam_tbl.in_lif_profile = in_lif_profile_val;  
        
        prog_selection_cam_tbl.cos_profile_mask = 0x00;
        prog_selection_cam_tbl.cos_profile_mask = 0x3F;
        prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
        prog_selection_cam_tbl.ptc_profile_mask = 0x03;
        prog_selection_cam_tbl.forwarding_code_mask = 0x00;
        prog_selection_cam_tbl.in_lif_profile_mask = in_lif_profile_mask;   

        prog_selection_cam_tbl.llvp_incoming_tag_structure = (idx==1 ? 6: prog_selection_cam_tbl.llvp_incoming_tag_structure);
        prog_selection_cam_tbl.llvp_incoming_tag_structure_mask = (idx==1 ? 0x0: prog_selection_cam_tbl.llvp_incoming_tag_structure_mask);

        prog_selection_cam_tbl.forwarding_offset_index = (idx==0 ? 3: prog_selection_cam_tbl.forwarding_offset_index);
        prog_selection_cam_tbl.forwarding_offset_index_mask = (idx==0 ? 0x0: prog_selection_cam_tbl.forwarding_offset_index_mask);

        prog_selection_cam_tbl.program = *prog_id ; 
        prog_selection_cam_tbl.valid = 1;

        res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_inner_learn", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_inner_learn(
     int unit,
     int32 prog_id   
   )
{
    uint32
      res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
      prog_selection_cam_tbl;
    int idx;
    uint32 in_lif_profile_mask, in_lif_profile_val;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    in_lif_profile_val = SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile;
    res = arad_pp_flp_prog_mask_map_ivl_learn(unit, &in_lif_profile_mask, &in_lif_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "[%d ; %x] in_lif_profile_mask, [%d ; %x] in_lif_profile_val\n"),
                         in_lif_profile_mask, in_lif_profile_mask, in_lif_profile_val, in_lif_profile_val));

    for (idx = 0; idx < 2; idx++) {
        
        res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_id,FALSE,FALSE,&cam_sel_id, NULL);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                        &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 201, exit);
        } else {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY,
                        &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
            res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                        &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        }

        prog_selection_cam_tbl.port_profile                   = 0;
        prog_selection_cam_tbl.ptc_profile                    = 0;
        prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_ETHERNET;
        prog_selection_cam_tbl.in_rif_uc_rpf_enable           = 0x0;
        prog_selection_cam_tbl.packet_is_compatible_mc        = 1;

        
        prog_selection_cam_tbl.forwarding_header_qualifier      = ARAD_PP_FLP_QLFR_ETH_NEXT_PROTOCOL_IPV4;
        prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                                       ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID);

        prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
        prog_selection_cam_tbl.ptc_profile_mask               = 0x03;
        prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
        prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1; 
        prog_selection_cam_tbl.packet_is_compatible_mc_mask   = 0x0; 

        
        prog_selection_cam_tbl.llvp_incoming_tag_structure = (idx==1 ? 6: prog_selection_cam_tbl.llvp_incoming_tag_structure);
        prog_selection_cam_tbl.llvp_incoming_tag_structure_mask = (idx==1 ? 0x0: prog_selection_cam_tbl.llvp_incoming_tag_structure_mask);

        prog_selection_cam_tbl.forwarding_offset_index = (idx==0 ? 3: prog_selection_cam_tbl.forwarding_offset_index);
        prog_selection_cam_tbl.forwarding_offset_index_mask = (idx==0 ? 0x0: prog_selection_cam_tbl.forwarding_offset_index_mask);

        prog_selection_cam_tbl.in_lif_profile = in_lif_profile_val;  
        prog_selection_cam_tbl.in_lif_profile_mask = in_lif_profile_mask;   

        prog_selection_cam_tbl.program                        = prog_id; 
        prog_selection_cam_tbl.valid                          = 1;

        res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_inner_learn", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_fwd_outer_learn(
     int unit,
     int32 *prog_id
   )
{
    uint32
      res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
      prog_selection_cam_tbl;
    int idx;
    uint32 in_lif_profile_mask, in_lif_profile_val;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    in_lif_profile_val = SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile;
    res = arad_pp_flp_prog_mask_map_ivl_learn(unit, &in_lif_profile_mask, &in_lif_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "[%d ; %x] in_lif_profile_mask, [%d ; %x] in_lif_profile_val\n"),
                         in_lif_profile_mask, in_lif_profile_mask, in_lif_profile_val, in_lif_profile_val));

    for (idx=0;idx<2;idx++) {
        

        if (idx == 0) {
            
            res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_IVL_FWD_OUTER_LEARN,FALSE,TRUE,&cam_sel_id, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        else {
            res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_IVL_FWD_OUTER_LEARN,FALSE,FALSE,&cam_sel_id, NULL);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                           &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
        } else {
            res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                        &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        }
        prog_selection_cam_tbl.port_profile = 0;
        prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
        prog_selection_cam_tbl.ptc_profile  = 0;
        prog_selection_cam_tbl.ptc_profile_mask = 0x03;

        prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
        prog_selection_cam_tbl.forwarding_code_mask = 0x00;

        prog_selection_cam_tbl.in_lif_profile = in_lif_profile_val;  
        prog_selection_cam_tbl.in_lif_profile_mask = in_lif_profile_mask;   
        prog_selection_cam_tbl.cos_profile = 0x20;   
        prog_selection_cam_tbl.cos_profile_mask = 0x1F; 

        

        prog_selection_cam_tbl.forwarding_header_qualifier = (idx==0 ? 0x001: 0x002);       
        prog_selection_cam_tbl.forwarding_header_qualifier_mask = (idx==0 ? 0x7FE: 0x7FC);  

        prog_selection_cam_tbl.program = *prog_id ; 
        prog_selection_cam_tbl.valid = 1;

        res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_fwd_outer_learn", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_fwd_outer_learn(
     int unit,
     int32 prog_id   
   )
{
    uint32
      res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
      prog_selection_cam_tbl;
    int idx;
    uint32 in_lif_profile_mask, in_lif_profile_val;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    in_lif_profile_val = SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile;
    res = arad_pp_flp_prog_mask_map_ivl_learn(unit, &in_lif_profile_mask, &in_lif_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "[%d ; %x] in_lif_profile_mask, [%d ; %x] in_lif_profile_val\n"),
                         in_lif_profile_mask, in_lif_profile_mask, in_lif_profile_val, in_lif_profile_val));

    for (idx = 0; idx < 2; idx++) {
        
        res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_id,FALSE,FALSE,&cam_sel_id, NULL);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                           &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
        } else {
            res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY,
                        &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
            res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                        &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
        }

        prog_selection_cam_tbl.port_profile                   = 0;
        prog_selection_cam_tbl.ptc_profile                    = 0;
        prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_ETHERNET;
        prog_selection_cam_tbl.in_rif_uc_rpf_enable           = 0x0;
        prog_selection_cam_tbl.packet_is_compatible_mc        = 1;

        
        prog_selection_cam_tbl.forwarding_header_qualifier      = ARAD_PP_FLP_QLFR_ETH_NEXT_PROTOCOL_IPV4;
        prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                                       ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID);

        prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
        prog_selection_cam_tbl.ptc_profile_mask               = 0x03;
        prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
        prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1; 
        prog_selection_cam_tbl.packet_is_compatible_mc_mask   = 0x0; 

        
        prog_selection_cam_tbl.llvp_incoming_tag_structure = (idx==1 ? 6: prog_selection_cam_tbl.llvp_incoming_tag_structure);
        prog_selection_cam_tbl.llvp_incoming_tag_structure_mask = (idx==1 ? 0x0: prog_selection_cam_tbl.llvp_incoming_tag_structure_mask);

        prog_selection_cam_tbl.forwarding_offset_index = (idx==0 ? 3: prog_selection_cam_tbl.forwarding_offset_index);
        prog_selection_cam_tbl.forwarding_offset_index_mask = (idx==0 ? 0x0: prog_selection_cam_tbl.forwarding_offset_index_mask);

        prog_selection_cam_tbl.in_lif_profile = in_lif_profile_val;  
        prog_selection_cam_tbl.in_lif_profile_mask = in_lif_profile_mask;   

        prog_selection_cam_tbl.program                        = prog_id; 
        prog_selection_cam_tbl.valid                          = 1;

        res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_fwd_outer_learn", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_oam_statistics(
     int unit,
     int32* prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_OAM_STATISTICS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.cos_profile_mask = 0x3F;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_oam_statistics", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_oam_down_untagged_statistics(
     int unit,
     int32* prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_OAM_DOWN_UNTAGGED_STATISTICS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_TM;
  prog_selection_cam_tbl.cos_profile_mask = 0x3F;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.tt_lookup_0_found =1;
  prog_selection_cam_tbl.tt_lookup_0_found_mask = 0;
  prog_selection_cam_tbl.vt_lookup_0_found = 1;
  prog_selection_cam_tbl.vt_lookup_0_found_mask = 0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_oam_statistics", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_oam_single_tag_statistics(
     int unit,
     int32* prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_OAM_SINGLE_TAG_STATISTICS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_TM;
  prog_selection_cam_tbl.cos_profile_mask = 0x3F;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.tt_lookup_1_found =1;
  prog_selection_cam_tbl.tt_lookup_1_found_mask = 0;
  prog_selection_cam_tbl.tt_lookup_0_found =0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask = 0;
  prog_selection_cam_tbl.vt_lookup_0_found = 1;
  prog_selection_cam_tbl.vt_lookup_0_found_mask = 0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_oam_statistics", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_oam_double_tag_statistics(
     int unit,
     int32* prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_OAM_DOUBLE_TAG_STATISTICS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_TM;
  prog_selection_cam_tbl.cos_profile_mask = 0x3F;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.tt_lookup_1_found =0;
  prog_selection_cam_tbl.tt_lookup_1_found_mask = 0;
  prog_selection_cam_tbl.tt_lookup_0_found =0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask = 0;
  prog_selection_cam_tbl.vt_lookup_0_found = 1;
  prog_selection_cam_tbl.vt_lookup_0_found_mask = 0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_oam_statistics", 0, 0);
}




uint32
   arad_pp_flp_prog_sel_cam_bfd_mpls_statistics(
     int unit,
     int32* prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_BFD_MPLS_STATISTICS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);



  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_TM;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.vt_lookup_1_found=1;
  prog_selection_cam_tbl.vt_lookup_1_found_mask=0;
  prog_selection_cam_tbl.tt_lookup_0_found=0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask=0;
  prog_selection_cam_tbl.tt_lookup_1_found=1;
  prog_selection_cam_tbl.tt_lookup_1_found_mask=0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_bfd_statistics", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_bfd_pwe_statistics(
     int unit,
     int32* prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_BFD_PWE_STATISTICS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_TM;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.vt_lookup_1_found=1;
  prog_selection_cam_tbl.vt_lookup_1_found_mask=0;
  prog_selection_cam_tbl.tt_lookup_0_found=0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask=0;
  prog_selection_cam_tbl.tt_lookup_1_found=0;
  prog_selection_cam_tbl.tt_lookup_1_found_mask=0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_bfd_statistics", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_bfd_statistics(
     int unit,
     int32* prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_BFD_STATISTICS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_TM;
  prog_selection_cam_tbl.ptc_profile_mask = 0x0;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  prog_selection_cam_tbl.vt_lookup_1_found=1;
  prog_selection_cam_tbl.vt_lookup_1_found_mask=0;
  prog_selection_cam_tbl.tt_lookup_0_found=1;
  prog_selection_cam_tbl.tt_lookup_0_found_mask=0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_bfd_statistics", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_pon_default_upstream(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_PON_DEFAULT_UPSTREAM,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                 &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_PON;
  prog_selection_cam_tbl.ptc_profile_mask = 0;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.cos_profile = 0;
  
  if (SOC_DPP_CONFIG(unit)->pp.vmac_enable ||
      SOC_DPP_CONFIG(unit)->pp.local_switching_enable) {
      prog_selection_cam_tbl.cos_profile_mask = 0x1F;
  } else {
      prog_selection_cam_tbl.cos_profile_mask = 0x3F;
  }
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_pon_default_upstream", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ethernet_pon_default_downstream(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_PON_DEFAULT_DOWNSTREAM,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile  = SOC_TMC_PORTS_FLP_PROFILE_NONE;
  prog_selection_cam_tbl.ptc_profile_mask = 0;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.cos_profile = 0;
  if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) ||
      SOC_DPP_CONFIG(unit)->pp.vmac_enable ||
      SOC_DPP_CONFIG(unit)->pp.local_switching_enable) {
      prog_selection_cam_tbl.cos_profile_mask = 0x1F;
  } else {
      prog_selection_cam_tbl.cos_profile_mask = 0x3F;
  }
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_pon_default_downstream", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_pon_local_route(
     int unit,
     uint8 mac_in_mac_enabled,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_PON_LOCAL_ROUTE,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile  = 0;
  prog_selection_cam_tbl.ptc_profile_mask = 0x03;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.cos_profile = SOC_PPC_LIF_AC_LOCAL_SWITCHING_COS_PROFILE;
  prog_selection_cam_tbl.cos_profile_mask = 0x1F;
  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (mac_in_mac_enabled) {
      int32 mim_prog_id;

      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM,FALSE,TRUE,&cam_sel_id, &mim_prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
          res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                         &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
          SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
      } else {
          res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                      &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
          SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
      }
      prog_selection_cam_tbl.port_profile = ARAD_PP_FLP_PORT_PROFILE_PBP; 
      prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
      prog_selection_cam_tbl.ptc_profile  = 0;
      prog_selection_cam_tbl.ptc_profile_mask = 0x03;
      prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
      prog_selection_cam_tbl.forwarding_code_mask = 0x00;
      prog_selection_cam_tbl.forwarding_offset_index = 2;
      prog_selection_cam_tbl.forwarding_offset_index_mask = 0;
      prog_selection_cam_tbl.program = mim_prog_id;
      prog_selection_cam_tbl.valid = 1;
      res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_pon_local_route", 0, 0);
}



uint32
   arad_pp_flp_prog_sel_cam_fcoe_ethernet_ing_learn(
     int    unit,
     int32  *prog_id,
     int    is_fcoe_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(is_fcoe_enabled){
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_FC_TRANSIT,FALSE,TRUE,&cam_sel_id, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }else{
      (*prog_id) = PROG_FLP_ETHERNET_ING_LEARN;
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_LEARN,FALSE,FALSE,&cam_sel_id, NULL);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_FCOE,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_FCOE, DPP_PLC_ANY_FCOE,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  
  prog_selection_cam_tbl.port_profile                   = 0;
  prog_selection_cam_tbl.ptc_profile                    = 0;
  prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_offset_index        = 1;

  prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask               = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index_mask   = 0;

  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_fcoe_ethernet_ing_learn", 0, 0);
}



uint32
   arad_pp_flp_prog_sel_cam_fcoe_fcf_with_vft(
     int unit,
     int32  *prog_id,
     uint32 is_npv
     )
{
  uint32
      prog_usage,
      res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (is_npv == 1) {
      prog_usage = PROG_FLP_FC_WITH_VFT_N_PORT;
  }else{
      prog_usage = PROG_FLP_FC_WITH_VFT;
  }

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_usage,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_FCOE_STD_ETH,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_FCOE, DPP_PLC_FCOE_VFT,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_CUSTOM1;
  prog_selection_cam_tbl.forwarding_offset_index = 2;

  prog_selection_cam_tbl.tt_lookup_0_found       = 0x1; 
  prog_selection_cam_tbl.tt_processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC; 

  prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index_mask   = 0;
  prog_selection_cam_tbl.tt_processing_profile_mask     = 0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask         = is_npv; 
  prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NPV;

  if (is_npv) {
      prog_selection_cam_tbl.port_profile               = ARAD_PP_FLP_PORT_PROFILE_FC_N_PORT;
  }else{
      prog_selection_cam_tbl.port_profile               = ARAD_PP_FLP_PORT_PROFILE_DEFAULT;
  }

  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_fcoe_fcf_with_vft", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_fcoe_fcf(
     int unit,
     int32  *prog_id,
     uint32 is_npv
   )
{
  uint32
      prog_usage,
      res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (is_npv == 1) {
      prog_usage = PROG_FLP_FC_N_PORT;
  }else{
      prog_usage = PROG_FLP_FC;
  }

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_usage,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_FCOE,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_FCOE, DPP_PLC_ANY_FCOE,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_CUSTOM1;
  prog_selection_cam_tbl.forwarding_offset_index        = 2;
  prog_selection_cam_tbl.tt_processing_profile          = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC; 
  prog_selection_cam_tbl.tt_lookup_0_found              = 0x1; 
  prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index_mask   = 0;
  prog_selection_cam_tbl.tt_processing_profile_mask     = 0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask         = is_npv; 

  if (is_npv) {
      prog_selection_cam_tbl.port_profile_mask          = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NPV;
      prog_selection_cam_tbl.port_profile               = ARAD_PP_FLP_PORT_PROFILE_FC_N_PORT;
  }else {
      prog_selection_cam_tbl.port_profile_mask          = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NPV;
      prog_selection_cam_tbl.port_profile               = ARAD_PP_FLP_PORT_PROFILE_DEFAULT;
  }

  prog_selection_cam_tbl.program                        = *prog_id;
  prog_selection_cam_tbl.valid                          = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_fcoe_fcf", 0, 0);
}



uint32
   arad_pp_flp_prog_sel_cam_fcoe_fcf_with_vft_remote(
     int unit,
     int32  *prog_id
   )
{
  uint32
     res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_FC_WITH_VFT_REMOTE,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_FCOE_STD_ETH,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_FCOE, DPP_PLC_FCOE_VFT,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_CUSTOM1;
  prog_selection_cam_tbl.forwarding_offset_index        = 2;
  prog_selection_cam_tbl.tt_processing_profile          = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC; 
  prog_selection_cam_tbl.tt_lookup_0_found              = 0x0; 
  prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index_mask   = 0;
  prog_selection_cam_tbl.tt_processing_profile_mask     = 0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask         = 0;

  prog_selection_cam_tbl.port_profile                   = ARAD_PP_FLP_PORT_PROFILE_DEFAULT;
  prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NPV;

  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_fcoe_fcf_with_vft_remote", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_fcoe_fcf_remote(
     int unit,
     int32  *prog_id
   )
{
  uint32
      res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_FC_REMOTE,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_FCOE,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_FCOE, DPP_PLC_ANY_FCOE,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_CUSTOM1;
  prog_selection_cam_tbl.forwarding_offset_index        = 2;
  prog_selection_cam_tbl.tt_processing_profile          = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC; 
  prog_selection_cam_tbl.tt_lookup_0_found              = 0x0;  

  prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index_mask   = 0;
  prog_selection_cam_tbl.tt_processing_profile_mask     = 0;
  prog_selection_cam_tbl.tt_lookup_0_found_mask         = 0;

  prog_selection_cam_tbl.port_profile                   = ARAD_PP_FLP_PORT_PROFILE_DEFAULT;
  prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NPV;

  prog_selection_cam_tbl.program = *prog_id;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_fcoe_fcf_remote", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ethernet_mac_in_mac(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_MAC_IN_MAC,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = ARAD_PP_FLP_PORT_PROFILE_PBP; 
  prog_selection_cam_tbl.ptc_profile  = 0;
  prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_offset_index = 1;
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index_mask = 0;
  prog_selection_cam_tbl.program = PROG_FLP_ETHERNET_MAC_IN_MAC;
  prog_selection_cam_tbl.valid = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ethernet_mac_in_mac", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn_rpf(
     int unit,
     int32 *prog_id,
     int spb_l2_rpf
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(prog_id);
  if (spb_l2_rpf){
     *prog_id = PROG_FLP_IPV4UC;
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,*prog_id,TRUE,TRUE,&cam_sel_id, NULL);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } else {
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV4UC_PUBLIC_RPF,FALSE,TRUE,&cam_sel_id, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = (spb_l2_rpf) ? ARAD_PP_FLP_PORT_PROFILE_PBP : 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV4_UC;
  prog_selection_cam_tbl.l_3_vpn_default_routing     = 0x1;
  prog_selection_cam_tbl.l_3_vpn_default_routing_mask = 0x00;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.program                   = *prog_id;
  prog_selection_cam_tbl.valid                     = 1;
  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
         
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_TM,
                  &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  }
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x1;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4uc_with_l3vpn", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(prog_id);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6UC_PUBLIC,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;
  prog_selection_cam_tbl.l_3_vpn_default_routing     = 0x1;
  prog_selection_cam_tbl.l_3_vpn_default_routing_mask = 0x00;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.program                   = *prog_id;
  prog_selection_cam_tbl.valid                     = 1;
  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
         
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_TM,
                  &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  }
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn_rpf(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(prog_id);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6UC_PUBLIC_RPF,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                     &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;
  prog_selection_cam_tbl.l_3_vpn_default_routing     = 0x1;
  prog_selection_cam_tbl.l_3_vpn_default_routing_mask = 0x00;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.program                   = *prog_id;
  prog_selection_cam_tbl.valid                     = 1;
  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
         
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_TM,
                  &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  }
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x1;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x0;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn_rpf", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn(
       int unit,
       int spb_l2_rpf
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;
  int prog_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  prog_id = (spb_l2_rpf) ? PROG_FLP_IPV4UC : PROG_FLP_IPV4UC_PUBLIC;

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV4UC_PUBLIC,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                     &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = (spb_l2_rpf) ? ARAD_PP_FLP_PORT_PROFILE_PBP : 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV4_UC;
  prog_selection_cam_tbl.l_3_vpn_default_routing     = 0x1;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.l_3_vpn_default_routing_mask= 0x00;
  prog_selection_cam_tbl.program                     = prog_id;
  prog_selection_cam_tbl.valid                       = 1;
  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
         
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_TM,
                  &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  }
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
  if (JER_KAPS_ENABLE(unit)) {
      
      prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
      prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x0;
  }
#endif
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4uc_with_l3vpn", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn_custom_prgrm(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IP4UC_CUSTOM_ROUTE,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_IPV4,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  prog_selection_cam_tbl.port_profile = ARAD_PP_FLP_PORT_PROFILE_PBP; 
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV4_UC;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index = 2;
  prog_selection_cam_tbl.forwarding_offset_index_mask = 0x0;
  prog_selection_cam_tbl.program                   = *prog_id;
  prog_selection_cam_tbl.valid                     = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn_custom_prgrm", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn_custom_prgrm(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IP6UC_CUSTOM_ROUTE,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_IPV6,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  prog_selection_cam_tbl.port_profile = ARAD_PP_FLP_PORT_PROFILE_PBP; 
  prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index = 2;
  prog_selection_cam_tbl.forwarding_offset_index_mask = 0x0;
  prog_selection_cam_tbl.program                   = *prog_id;
  prog_selection_cam_tbl.valid                     = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn_custom_prgrm", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf_2pass(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6UC_WITH_RPF_2PASS,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 211, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;   
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x1;                        
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;                        
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x00;                     
  prog_selection_cam_tbl.program                   = *prog_id;
  prog_selection_cam_tbl.valid                     = 1;
  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
	  prog_selection_cam_tbl.l_3_vpn_default_routing        = 0x1;
	  prog_selection_cam_tbl.l_3_vpn_default_routing_mask   = 0x0;
	  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1;
  }
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 212, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf_2pass", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv4uc_with_rpf(
     int unit,
     int spb_l2_rpf
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;
  int prog_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  prog_id = (spb_l2_rpf) ? PROG_FLP_IPV4UC : PROG_FLP_IPV4UC_RPF;

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_id,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = (spb_l2_rpf) ? ARAD_PP_FLP_PORT_PROFILE_PBP : 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV4_UC;
  prog_selection_cam_tbl.port_profile_mask           = (spb_l2_rpf) ? ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP : ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x00;
  prog_selection_cam_tbl.program                   = prog_id;
  prog_selection_cam_tbl.valid                     = 1;
  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
      prog_selection_cam_tbl.in_rif_uc_rpf_enable           = 0x1;
      prog_selection_cam_tbl.l_3_vpn_default_routing_mask   = 0x1;
      prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x0;
  }
  if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)){
      prog_selection_cam_tbl.cos_profile = SOC_PPC_FLP_COS_PROFILE_ANTI_SPOOFING;
      prog_selection_cam_tbl.cos_profile_mask = 0x1F; 
      prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
  } else {
      prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x1;
  }
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
  if ((!ARAD_KBP_IS_IN_USE && JER_KAPS_ENABLE(unit)) ||
      (ARAD_KBP_IS_IN_USE && ARAD_KBP_ENABLE_IPV4_RPF_PUBLIC && SOC_IS_JERICHO(unit)))
  {
      prog_selection_cam_tbl.l_3_vpn_default_routing     = 0x0;
      prog_selection_cam_tbl.l_3_vpn_default_routing_mask = 0x00;
  }
#endif
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4uc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv4uc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV4UC,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {

      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }

  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV4_UC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x00;
  prog_selection_cam_tbl.program                   = PROG_FLP_IPV4UC;
  prog_selection_cam_tbl.valid                     = 1;

#if defined(INCLUDE_KBP)
      if (SOC_IS_JERICHO(unit)) {
          if(ARAD_KBP_ENABLE_IPV4_DC){ 
              prog_selection_cam_tbl.custom_rif_profile_bit      = 0x0;
              prog_selection_cam_tbl.custom_rif_profile_bit_mask = 0x0;
          }
      }
#endif

  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
	  prog_selection_cam_tbl.in_rif_uc_rpf_enable           = 0x0;
	  prog_selection_cam_tbl.l_3_vpn_default_routing_mask   = 0x1;
	  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x0;
  }
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4uc", 0, 0);
}

uint32
arad_pp_flp_prog_sel_cam_mpls_exp_null_ttl_reject(
     int unit,
     int32  *prog_id
   )
{
    uint32
    res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_MPLS_EXP_NULL_TTL_REJECT,FALSE,TRUE,&cam_sel_id, prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {

      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                         &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

    } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                            &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
    }

    prog_selection_cam_tbl.tt_processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MPLS_EXP_NULL;
    prog_selection_cam_tbl.tt_processing_profile_mask  = 0x0;
    prog_selection_cam_tbl.service_type = ARAD_PP_FLP_SERVICE_TYPE_MPLS_EXP_NULL_TTL_REJECT;
    prog_selection_cam_tbl.service_type_mask = 0;
    prog_selection_cam_tbl.program                   = *prog_id;
    prog_selection_cam_tbl.valid                     = 1;

    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_mpls_exp_null_ttl_reject", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ipv4_dc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV4_DC,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV4_UC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x00;

  prog_selection_cam_tbl.custom_rif_profile_bit_mask = 0x0;
  prog_selection_cam_tbl.custom_rif_profile_bit      = 0x1;

  prog_selection_cam_tbl.program                     = PROG_FLP_IPV4_DC;
  prog_selection_cam_tbl.valid                       = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4_dc", 0, 0);
}




uint32
   arad_pp_flp_prog_sel_cam_bfd_single_hop(
     int unit,
     int is_ipv6,
	 int32  *prog_id
   )
{
  uint32
    res;
  uint32 prog_usage;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;
  DPP_PFC_E pfc_type;
  DPP_PLC_E parser_leaf_context = DPP_PLC_NOF ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  prog_usage = (is_ipv6 ? PROG_FLP_BFD_IPV6_SINGLE_HOP : PROG_FLP_BFD_IPV4_SINGLE_HOP);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,prog_usage,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) {
      
      prog_selection_cam_tbl.ptc_profile             = ARAD_PORTS_FLP_PROFILE_NON_BFD_FRWD;
      prog_selection_cam_tbl.ptc_profile_mask        = 0x00;
      parser_leaf_context = DPP_PLC_BFD_SINGLE_HOP;
  } else {
      prog_selection_cam_tbl.ptc_profile                 = 0;
      prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
      parser_leaf_context = DPP_PLC_BFD_SINGLE_HOP;
  }

  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      pfc_type = (is_ipv6 ? DPP_PFC_ANY_IPV6 : DPP_PFC_ANY_IPV4);
      res = dpp_parser_pfc_get_hw_by_sw(unit, pfc_type,
                  &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

      res = dpp_parser_plc_hw_by_sw(unit, pfc_type, parser_leaf_context,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_ETH, parser_leaf_context,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.forwarding_code             = (is_ipv6 ? ARAD_PP_FWD_CODE_IPV6_UC : ARAD_PP_FWD_CODE_IPV4_UC);
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.program                     = *prog_id;
  prog_selection_cam_tbl.valid                       = 1;
 
  prog_selection_cam_tbl.forwarding_header_qualifier = 0x480;
  prog_selection_cam_tbl.forwarding_header_qualifier_mask = 0x7F;



  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id,&prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_bfd_single_hop", 0, 0);
}


uint32 arad_pp_flp_prog_sel_cam_bfd_single_hop_bridge( int unit)
{
    uint32 res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_LEARN,FALSE,FALSE,&cam_sel_id, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
        res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_IP,
                    &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

        res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_IP, DPP_PLC_BFD_SINGLE_HOP,
                    &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
    } else {
        res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_ETH, DPP_PLC_BFD_SINGLE_HOP,
                    &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
    }
    prog_selection_cam_tbl.port_profile = 0;
    prog_selection_cam_tbl.ptc_profile  = 0;
    prog_selection_cam_tbl.forwarding_code = ARAD_PP_FWD_CODE_ETHERNET;
    prog_selection_cam_tbl.cos_profile_mask = 0x00;
    prog_selection_cam_tbl.cos_profile_mask = 0x3F;
    prog_selection_cam_tbl.port_profile_mask = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
    prog_selection_cam_tbl.ptc_profile_mask = 0x03;
    prog_selection_cam_tbl.forwarding_code_mask = 0x00;
    prog_selection_cam_tbl.program = PROG_FLP_ETHERNET_ING_LEARN;
    prog_selection_cam_tbl.valid = 1;
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
     SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_bfd_single_hop_bridge", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf_2pass_fwd(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6UC,FALSE,FALSE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = ARAD_PORTS_FLP_PROFILE_OVERLAY_RCY; 
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;   
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x1;                        
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x00;                        
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;                        
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x00;                     

  prog_selection_cam_tbl.program                   = PROG_FLP_IPV6UC;
  prog_selection_cam_tbl.valid                     = 1;

  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
	  prog_selection_cam_tbl.l_3_vpn_default_routing        = 0x1;
	  prog_selection_cam_tbl.l_3_vpn_default_routing_mask   = 0x0;
	  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1;
  }
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf_2pass_fwd", 0, 0);
}



#if defined(INCLUDE_KBP)

uint32
   arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(prog_id);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6UC_RPF,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY,  (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x00;
  if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV6_ENABLE(unit)){
      prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
      prog_selection_cam_tbl.cos_profile                 = SOC_PPC_FLP_COS_PROFILE_ANTI_SPOOFING;
      prog_selection_cam_tbl.cos_profile_mask            = 0x1F; 
  } else {
      prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x1;
  }
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
  if ((JER_KAPS_ENABLE(unit) && !ARAD_KBP_ENABLE_IPV6_RPF)
        || (ARAD_KBP_IS_IN_USE && ARAD_KBP_ENABLE_IPV6_RPF_PUBLIC && SOC_IS_JERICHO(unit))) {
      prog_selection_cam_tbl.l_3_vpn_default_routing     = 0x0;
      prog_selection_cam_tbl.l_3_vpn_default_routing_mask= 0x0;
  }
#endif

  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.program                   = *prog_id;
  prog_selection_cam_tbl.valid                     = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf", 0, 0);
}
#endif


uint32
   arad_pp_flp_prog_sel_cam_ipv6uc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6UC,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x01; 
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
  if ((JER_KAPS_ENABLE(unit) && !ARAD_KBP_ENABLE_IPV6_UC)
        || (ARAD_KBP_IS_IN_USE && ARAD_KBP_ENABLE_IPV6_UC_PUBLIC && SOC_IS_JERICHO(unit))) {
      prog_selection_cam_tbl.l_3_vpn_default_routing        = 0x0;
      prog_selection_cam_tbl.l_3_vpn_default_routing_mask   = 0x0;
      prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
      prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x0;
  }
#endif
#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_UC || ARAD_KBP_ENABLE_IPV6_EXTENDED){
      
      prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x00; 
  } else if (SOC_IS_JERICHO(unit)) {
#else
if (SOC_IS_JERICHO(unit)) {
#endif
    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_ipv6_uc_use_tcam", 0)) {
      
      prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x01;
    } else {
      
      prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x00; 
    }
}


  prog_selection_cam_tbl.program                   = PROG_FLP_IPV6UC;
  prog_selection_cam_tbl.valid                     = 1;

  if (SEPARATE_IPV4_IPV6_RPF_ENABLE) {
	  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1;
  }

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6uc", 0, 0);
}

STATIC
  uint32
    arad_pp_flp_instruction_rsrc_set(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  prog_id
    )
{
    uint32
      ce_rsrc = 0;
    soc_error_t
      rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    arad_pp_ihb_flp_ce_resources_per_program_get_unsafe(unit, prog_id, &ce_rsrc);


    rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
            prog_id,
            0, 
            ce_rsrc
        );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 41, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_elk_instruction_rsrc_set", 0, 0);
}

STATIC
  uint32
    arad_pp_flp_all_progs_instruction_set(
       SOC_SAND_IN  int  unit
    )
{
  uint32
    prog_idx,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (prog_idx = 0; prog_idx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); prog_idx++)
  {
      res = arad_pp_flp_instruction_rsrc_set(unit, prog_idx);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_all_progs_instruction_set", 0, 0);
}

#if defined(INCLUDE_KBP)
uint32
    arad_pp_flp_elk_prog_config(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  prog_id,
       SOC_SAND_IN  uint32  opcode,
       SOC_SAND_IN  int     nof_shared_quals,
       SOC_SAND_IN  uint32  key_size_msb,
       SOC_SAND_IN  uint32  key_size
    )
{
    uint32
      res;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
      flp_lookups_tbl;

#ifdef BCM_88675_A0
    ARAD_INIT_ELK* elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
#endif

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(
            unit,
            prog_id,
            &flp_lookups_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (SOC_IS_JERICHO(unit)) {
#ifdef BCM_88675_A0

        if(ARAD_KBP_PROG_IS_L2_SHARING(opcode)) {
            
            if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_da_sa_sharing_enable", 0)) {
                flp_lookups_tbl.elk_packet_data_select = 0x6; 
            }
        }
        else { 
            uint8 sip_sharing_configure = 0;

            switch(opcode)
            {
                case PROG_FLP_IPV6UC:
                    sip_sharing_configure = sip_sharing_uc[unit];
                    break;
                case PROG_FLP_IPV6UC_RPF:
                    sip_sharing_configure = sip_sharing_uc_rpf[unit];
                    break;
                case PROG_FLP_IPV6UC_PUBLIC:
                    sip_sharing_configure = sip_sharing_uc_public[unit];
                    break;
                case PROG_FLP_IPV6UC_PUBLIC_RPF:
                    sip_sharing_configure = sip_sharing_uc_public_rpf[unit];
                    break;
                case PROG_FLP_IPV6MC:
                    sip_sharing_configure = sip_sharing_mc[unit];
                    break;
                default:
                    break;
            }

            if(!sip_sharing_configure && (elk->kbp_no_fwd_ipv6_dip_sip_sharing_disable == 0) && (ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_VALID(opcode))) {
            
                int bytes_lsb = 10;
                int bytes_msb = 8;
                if(ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_UC_VALID(opcode)) {
                    if(elk->kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header != 0) {
                        flp_lookups_tbl.elk_packet_data_select = 0x6; 
                    } else {
                        res = arad_pp_flp_dbal_ipv6uc_kbp_sip_sharing_program_tables_init(unit, prog_id);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                        res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(
                                unit,
                                prog_id,
                                &flp_lookups_tbl
                              );
                        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
                        if(prog_id == PROG_FLP_IPV6UC) {
                            flp_lookups_tbl.elk_key_a_valid_bytes = bytes_lsb;
                            flp_lookups_tbl.elk_key_a_msb_valid_bytes = bytes_msb;
                        } else {
                            flp_lookups_tbl.elk_key_b_valid_bytes = bytes_lsb;
                            flp_lookups_tbl.elk_key_b_msb_valid_bytes = bytes_msb;
                        }
                    }
                } else {
                    flp_lookups_tbl.elk_key_a_valid_bytes = bytes_lsb;
                    flp_lookups_tbl.elk_key_b_valid_bytes = bytes_lsb;
                    flp_lookups_tbl.elk_key_a_msb_valid_bytes = bytes_msb;
                    flp_lookups_tbl.elk_key_b_msb_valid_bytes = bytes_msb;
                }
            }

            switch(opcode)
            {
                case PROG_FLP_IPV6UC:
                    sip_sharing_uc[unit] = 1;
                    break;
                case PROG_FLP_IPV6UC_RPF:
                    sip_sharing_uc_rpf[unit] = 1;
                    break;
                case PROG_FLP_IPV6UC_PUBLIC:
                    sip_sharing_uc_public[unit] = 1;
                    break;
                case PROG_FLP_IPV6UC_PUBLIC_RPF:
                    sip_sharing_uc_public_rpf[unit] = 1;
                    break;
                case PROG_FLP_IPV6MC:
                    sip_sharing_mc[unit] = 1;
                    break;
                default:
                    break;
            }
        }

#endif
        flp_lookups_tbl.elk_key_c_valid_bytes += key_size;
        flp_lookups_tbl.elk_key_c_msb_valid_bytes += key_size_msb;

    }else{
        flp_lookups_tbl.elk_key_c_valid_bytes += key_size;
    }

    flp_lookups_tbl.elk_lkp_valid = 0x1;
    flp_lookups_tbl.elk_wait_for_reply = 0x1;
    flp_lookups_tbl.elk_opcode = opcode;

    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(
            unit,
            prog_id,
            &flp_lookups_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_elk_prog_config", 0, 0);
}

uint32
    arad_pp_flp_elk_prog_config_max_key_size_get(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  prog_id,
       SOC_SAND_IN  uint32  zone_id,
       SOC_SAND_OUT uint32   *max_key_size_in_bits
    )
{
    uint32
      res;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
      flp_lookups_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
   res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(
            unit,
            prog_id,
            &flp_lookups_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

	if(SOC_IS_JERICHO(unit)){
		if (zone_id == 0) {
	        *max_key_size_in_bits = ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS - (flp_lookups_tbl.elk_key_c_valid_bytes * SOC_SAND_NOF_BITS_IN_CHAR);
		}else{
	        *max_key_size_in_bits = ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS - (flp_lookups_tbl.elk_key_c_msb_valid_bytes * SOC_SAND_NOF_BITS_IN_CHAR);
	    }
	}else{
    	*max_key_size_in_bits = ARAD_PP_FLP_KEY_C_ZONE_SIZE_ARAD_PLUS_BITS - (flp_lookups_tbl.elk_key_c_valid_bytes * SOC_SAND_NOF_BITS_IN_CHAR);
	}

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_elk_prog_config_max_key_size_get", 0, 0);
}

STATIC
  uint32
   arad_pp_flp_ipv6uc_with_rpf_prog_init(
     int unit
   )
{
    uint32 vrf_ce_inst = 0,
      res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
      prog_selection_cam_tbl;
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
      flp_key_construction_tbl;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
      flp_lookups_tbl;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
      flp_process_tbl;
    uint32
      tmp;
    soc_reg_above_64_val_t
      reg_val;
    int32
      prog_id;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);

    
    
    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6UC_RPF,FALSE,TRUE, &cam_sel_id, &prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
        res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                       &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
    } else {
        res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) ? DPP_PLC_ANY_PP_AND_BFD : DPP_PLC_ANY_PP,
                    &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
        SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
    }
    prog_selection_cam_tbl.port_profile              = 0;
    prog_selection_cam_tbl.ptc_profile               = 0;
    prog_selection_cam_tbl.forwarding_code           = ARAD_PP_FWD_CODE_IPV6_UC;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable      = 0x1;
    prog_selection_cam_tbl.port_profile_mask         = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
    prog_selection_cam_tbl.ptc_profile_mask          = 0x03;
    prog_selection_cam_tbl.forwarding_code_mask      = 0x00;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask = 0x0;
    prog_selection_cam_tbl.program                   = prog_id;
    prog_selection_cam_tbl.valid                     = 1;

    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    flp_key_construction_tbl.instruction_0_16b        = vrf_ce_inst;
    flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_63_48
    flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_47_32
    flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_127_96
    flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_95_64
    flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_31_0
    flp_key_construction_tbl.key_a_inst_0_to_5_valid  = 0x19 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid  = 0x26 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid  = 0x0  ;

    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    flp_key_construction_tbl.instruction_0_16b       = vrf_ce_inst;
    flp_key_construction_tbl.instruction_1_16b       = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_127_112
    flp_key_construction_tbl.instruction_2_16b       = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_111_96
    flp_key_construction_tbl.instruction_3_32b       = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_95_64
    flp_key_construction_tbl.instruction_4_32b       = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_63_32
    flp_key_construction_tbl.instruction_5_32b       = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_31_0
    flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid = SOC_IS_JERICHO(unit)? 0x3806 : 0x3e ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

    
    
    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    flp_lookups_tbl.elk_lkp_valid = 1;
    flp_lookups_tbl.elk_wait_for_reply = 1;
    flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF;

    flp_lookups_tbl.tcam_lkp_db_profile    = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    flp_lookups_tbl.tcam_lkp_key_select    = ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL; 
    flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = 0x3F;
    flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = 0x3F;
    flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

    flp_lookups_tbl.elk_key_a_valid_bytes = 10;
    flp_lookups_tbl.elk_key_b_valid_bytes = 8;
    flp_lookups_tbl.elk_key_c_valid_bytes = 16;

    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    
    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    sal_memset(&flp_process_tbl, 0x0, sizeof(ARAD_PP_IHB_FLP_PROCESS_TBL_DATA));
    
    flp_process_tbl.result_a_format             = 0;
    flp_process_tbl.result_b_format             = 0;
    flp_process_tbl.procedure_ethernet_default  = 0;
    flp_process_tbl.enable_hair_pin_filter      = 1;
    flp_process_tbl.enable_rpf_check            = 1;
    flp_process_tbl.sa_lkp_process_enable       = 0;
    flp_process_tbl.apply_fwd_result_a          = 1;
    
    flp_process_tbl.not_found_trap_strength     = 0;
    flp_process_tbl.not_found_trap_code         = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6;
    
    flp_process_tbl.select_default_result_a     = 0;
    flp_process_tbl.select_default_result_b     = 0;

    flp_process_tbl.elk_result_format = 1;
    flp_process_tbl.include_elk_fwd_in_result_a = 1;
    flp_process_tbl.include_elk_ext_in_result_a = 0;
    flp_process_tbl.include_elk_fwd_in_result_b = 0;
    flp_process_tbl.include_elk_ext_in_result_b = 1;

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);


    
    tmp = 2;
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV6UC,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

  exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_ipv6uc_with_rpf_prog_init", 0, 0);
}
#endif

uint32
   arad_pp_flp_prog_sel_cam_ipv6mc(
     int unit,
     int32 *ipv6mc_sel_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(ipv6mc_sel_id);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6MC,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *ipv6mc_sel_id = cam_sel_id;
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_MC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = (SOC_IS_JERICHO(unit)) ? 0x01 : 0x0;
  prog_selection_cam_tbl.program                     = PROG_FLP_IPV6MC;
  prog_selection_cam_tbl.valid                       = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6mc", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv6mc_ssm_compress_sip(
     int unit,
     int32 *ipv6mc_hw_prog_id
   )
{
  uint32 res = SOC_SAND_OK;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(ipv6mc_hw_prog_id);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP, FALSE, TRUE, &cam_sel_id, ipv6mc_hw_prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }

  
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile                 = SOC_TMC_PORTS_FLP_PROFILE_NONE;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_MC;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.lsb_rif_profile_bit         = 0x1;
  prog_selection_cam_tbl.lsb_rif_profile_bit_mask    = 0x0;
  prog_selection_cam_tbl.program                     = *ipv6mc_hw_prog_id;
  prog_selection_cam_tbl.valid                       = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6mc_ssm_compress_sip", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_ipv6mc_ssm(
     int unit,
     int32 *ipv6mc_hw_prog_id
   )
{
  uint32 res = SOC_SAND_OK;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(ipv6mc_hw_prog_id);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_IPV6_MC_SSM, FALSE, TRUE, &cam_sel_id, ipv6mc_hw_prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  }

  
  prog_selection_cam_tbl.ptc_profile                 = ARAD_PORTS_FLP_PROFILE_OVERLAY_RCY;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x00;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_MC;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.lsb_rif_profile_bit         = 0;
  prog_selection_cam_tbl.lsb_rif_profile_bit_mask    = 0x3f;

  prog_selection_cam_tbl.program                     = *ipv6mc_hw_prog_id;
  prog_selection_cam_tbl.valid                       = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv6mc_ssm", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_pwe_gre(
     int unit,
     int32  *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_VPLSOGRE,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_ETHERNET; 
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.tt_processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PWEoGRE;
  prog_selection_cam_tbl.tt_processing_profile_mask  = 0x0;
  prog_selection_cam_tbl.program                     = *prog_id;
  prog_selection_cam_tbl.valid                       = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_pwe_gre", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_lsr(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_LSR,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile                = 0;
  prog_selection_cam_tbl.ptc_profile                 = 0;
  prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_MPLS;
  prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
  prog_selection_cam_tbl.program                     = PROG_FLP_LSR;
  prog_selection_cam_tbl.valid                       = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_lsr", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_TRILL_uc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_TRILL_UC,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.port_profile            = 0;
  prog_selection_cam_tbl.ptc_profile             = 0;
  prog_selection_cam_tbl.forwarding_code         = ARAD_PP_FWD_CODE_TRILL;
  prog_selection_cam_tbl.trill_mc                = 0x0;
  prog_selection_cam_tbl.port_profile_mask       = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask        = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask    = 0x00;
  prog_selection_cam_tbl.trill_mc_mask           = 0x0;
  prog_selection_cam_tbl.program                 = PROG_FLP_TRILL_UC;
  prog_selection_cam_tbl.valid                   = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_TRILL", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_TRILL_mc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_TRILL_MC_ONE_TAG,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.port_profile             = 0;
  prog_selection_cam_tbl.ptc_profile              = 0;
  prog_selection_cam_tbl.forwarding_code          = ARAD_PP_FWD_CODE_TRILL;
  prog_selection_cam_tbl.trill_mc                 = 0x1;
  prog_selection_cam_tbl.vt_lookup_1_found        = 0x0; 
  prog_selection_cam_tbl.port_profile_mask        = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask         = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask     = 0x00;
  prog_selection_cam_tbl.trill_mc_mask            = 0x0;
  prog_selection_cam_tbl.vt_lookup_1_found_mask   = 0x0;
  prog_selection_cam_tbl.program                  = PROG_FLP_TRILL_MC_ONE_TAG;
  prog_selection_cam_tbl.valid                    = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_TRILL_MC_TWO_TAGS,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.port_profile             = 0;
  prog_selection_cam_tbl.ptc_profile              = 0;
  prog_selection_cam_tbl.forwarding_code          = ARAD_PP_FWD_CODE_TRILL;
  prog_selection_cam_tbl.trill_mc                 = 0x1;
  prog_selection_cam_tbl.vt_lookup_1_found        = 0x1; 
  prog_selection_cam_tbl.port_profile_mask        = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask         = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask     = 0x00;
  prog_selection_cam_tbl.trill_mc_mask            = 0x0;
  prog_selection_cam_tbl.vt_lookup_1_found_mask   = 0x0;
  prog_selection_cam_tbl.program                  = PROG_FLP_TRILL_MC_TWO_TAGS;
  prog_selection_cam_tbl.valid                    = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_TRILL_mc", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_TRILL_mc_after_recycle_overlay(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_TRILL_AFTER_TERMINATION,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.port_profile            = 0;
  prog_selection_cam_tbl.ptc_profile             = ARAD_PORTS_FLP_PROFILE_OVERLAY_RCY;
  prog_selection_cam_tbl.ptc_profile_mask        = 0x0;
  prog_selection_cam_tbl.forwarding_code         = ARAD_PP_FWD_CODE_TRILL;
  prog_selection_cam_tbl.trill_mc                = 0x1;
  prog_selection_cam_tbl.port_profile_mask       = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask        = 0x00;
  prog_selection_cam_tbl.forwarding_code_mask    = 0x00;
  prog_selection_cam_tbl.trill_mc_mask           = 0x0;
  prog_selection_cam_tbl.program                 = PROG_FLP_TRILL_AFTER_TERMINATION;
  prog_selection_cam_tbl.valid                   = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_TRILL_mc_after_recycle_overlay", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_pon_vmac_upstream(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_VMAC_UPSTREAM,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile               = 0;
  prog_selection_cam_tbl.port_profile_mask          = 0x00;
  prog_selection_cam_tbl.ptc_profile                = SOC_TMC_PORTS_FLP_PROFILE_PON;
  prog_selection_cam_tbl.ptc_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.forwarding_code            = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_code_mask       = 0x00;
  prog_selection_cam_tbl.cos_profile                = SOC_PPC_FLP_COS_PROFILE_VMAC;
  prog_selection_cam_tbl.cos_profile_mask           = 0x1F;
  prog_selection_cam_tbl.program                    = *prog_id;
  prog_selection_cam_tbl.valid                      = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_pon_vmac_upstream", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_pon_vmac_downstream(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_VMAC_DOWNSTREAM,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }
  prog_selection_cam_tbl.port_profile = 0;
  prog_selection_cam_tbl.port_profile_mask          = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile                = SOC_TMC_PORTS_FLP_PROFILE_NONE;
  prog_selection_cam_tbl.ptc_profile_mask           = 0x0;
  prog_selection_cam_tbl.forwarding_code            = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_code_mask       = 0x00;
  prog_selection_cam_tbl.cos_profile                = SOC_PPC_FLP_COS_PROFILE_VMAC;
  prog_selection_cam_tbl.cos_profile_mask           = 0x1F;
  prog_selection_cam_tbl.program                    = *prog_id;
  prog_selection_cam_tbl.valid                      = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_pon_vmac_downstream", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_oam_outlif_l2g(
      int unit,
      int *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32
    cam_sel_id;
  SOC_PPC_TRAP_CODE
    trap_code_ndx;
  SOC_PPC_TRAP_CODE_INTERNAL
    trap_code_internal;
  ARAD_SOC_REG_FIELD
    strength_fld_fwd,
    strength_fld_snp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  trap_code_ndx = SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_MIP_EGRESS_SNOOP;
  res = arad_pp_trap_mgmt_trap_code_to_internal(
          unit,
          trap_code_ndx, 
          &trap_code_internal,
          &strength_fld_fwd,
          &strength_fld_snp
        );

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_OAM_OUTLIF_L2G,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_TM,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  prog_selection_cam_tbl.cpu_trap_code      = trap_code_internal;
  prog_selection_cam_tbl.cpu_trap_code_mask = 0;
  prog_selection_cam_tbl.program            = *prog_id;
  prog_selection_cam_tbl.valid              = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_lsr", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_two_pass_mc_encap_to_dest(
      int unit,
      int *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32
    cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_TWO_PASS_MC_ENCAP_TO_DEST,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  prog_selection_cam_tbl.ptc_profile             = ARAD_PORTS_FLP_PROFILE_OVERLAY_RCY;
  prog_selection_cam_tbl.ptc_profile_mask        = 0x0;
  prog_selection_cam_tbl.program                 = *prog_id;
  prog_selection_cam_tbl.valid                   = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_lsr", 0, 0);
}

uint32
   arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_dhcp(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
    uint32
        res,
        flp_prog_sel;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
        flp_lookups_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    flp_prog_sel = PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP;
    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    flp_lookups_tbl.lem_1st_lkp_valid      = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
    flp_lookups_tbl.lem_1st_lkp_key_select = 1; 
    flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
    flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
    flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);

    flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
    flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
    flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x7;
    flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IP_SPOOF_DHCP_KEY_OR_MASK;
    flp_lookups_tbl.learn_key_select       = 1;

    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_dhcp", 0, 0);
}

uint32
   arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_static(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
    uint32
        res,
        flp_prog_sel;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
        flp_lookups_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    flp_prog_sel = PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC;
    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    flp_lookups_tbl.lem_1st_lkp_valid      = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
    flp_lookups_tbl.lem_1st_lkp_key_select = 1; 
    flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
    flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
    flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);

    flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
    flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
    flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
    flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK;

    if (SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_ENABLE(unit)) {
        flp_lookups_tbl.lpm_1st_lkp_valid      = 1;
        flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
        flp_lookups_tbl.lpm_1st_lkp_and_value  = 3;
        flp_lookups_tbl.lpm_1st_lkp_or_value   = 0;
    }
    flp_lookups_tbl.learn_key_select       = 1;
    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_static", 0, 0);
}

uint32
   arad_pp_flp_lookups_ethernet_tk_epon_uni_v6(
     int unit,
     uint32 tcam_access_profile_id,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res,
    flp_prog_sel;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!SOC_IS_JERICHO(unit)){
    flp_prog_sel = PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC;
    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    flp_lookups_tbl.lem_1st_lkp_valid      = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
    flp_lookups_tbl.lem_1st_lkp_key_select = 1; 
    flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
    flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
    flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);

    if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
      flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
      flp_lookups_tbl.lem_2nd_lkp_key_select = 0; 

      flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x7;
      flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IP_SPOOF_DHCP_KEY_OR_MASK;
    } else {
      flp_lookups_tbl.lem_2nd_lkp_valid      = 0;
      flp_lookups_tbl.tcam_lkp_key_select    = ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL;
      flp_lookups_tbl.tcam_lkp_db_profile    = tcam_access_profile_id;
    }
    flp_lookups_tbl.learn_key_select   = 1;

    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  }

  
  if (!SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
    flp_prog_sel = PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP;
    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

    flp_lookups_tbl.lem_1st_lkp_valid      = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
    flp_lookups_tbl.lem_1st_lkp_key_select = 1; 
    flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
    flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
    flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);

    flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
    flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
    flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x7;
    flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IP_SPOOF_DHCP_KEY_OR_MASK;

    flp_lookups_tbl.learn_key_select   = 1;

    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, flp_prog_sel, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_tk_epon_uni_v6", 0, 0);
}

uint32
   arad_pp_flp_lookups_pon_arp_downstream(
     int unit,
     int prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK;
  flp_lookups_tbl.lpm_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lpm_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lpm_2nd_lkp_and_value = 3;
  flp_lookups_tbl.lpm_2nd_lkp_or_value = 0;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_pon_arp_downstream", 0, 0);
}

uint32
   arad_pp_flp_lookups_pon_arp_upstream(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_lookups_tbl.lem_1st_lkp_valid      = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_lookups_tbl.lem_1st_lkp_key_select = 1; 
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK;
  flp_lookups_tbl.lpm_1st_lkp_valid     = 1;
  flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
  flp_lookups_tbl.lpm_1st_lkp_and_value = 3;
  flp_lookups_tbl.lpm_1st_lkp_or_value = 0;
  flp_lookups_tbl.learn_key_select   = 1;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_pon_arp_upstream", 0, 0);
}



uint32
   arad_pp_flp_tk_epon_uni_v6_ing_learn_get(
     int unit,
     uint8 *ingress_learn_enable,
     uint8 *ingress_learn_oppurtunistic
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  if (flp_process_tbl.lem_1st_lkp_valid)
  {
    *ingress_learn_enable = 1;
    *ingress_learn_oppurtunistic = 0;
  }
  else
  {
    *ingress_learn_enable = 0;
    *ingress_learn_oppurtunistic = 0;
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_tk_epon_uni_v6_ing_learn_get", 0, 0);
}


uint32
   arad_pp_flp_tk_epon_uni_v6_tcam_profile_get(
     int unit,
     uint32 *tcam_access_profile_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  *tcam_access_profile_id = flp_process_tbl.tcam_lkp_db_profile;


exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_tk_epon_uni_v6_tcam_profile_get", 0, 0);
}

uint32
   arad_pp_flp_lookups_ethernet_ing_learn(
     int unit,
     uint8 ingress_learn_enable,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = (ingress_learn_enable &&(!sa_auth_enabled && !slb_enabled))? 1 : 0;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.learn_key_select   = 0;

#if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
	  flp_lookups_tbl.elk_lkp_valid = 0x1;
	  flp_lookups_tbl.elk_wait_for_reply = 0x1;
	  flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_P2P; 

	  flp_lookups_tbl.elk_key_a_valid_bytes = 0;
	  flp_lookups_tbl.elk_key_b_valid_bytes = 2;
	  flp_lookups_tbl.elk_key_c_valid_bytes = 0;
  }
#endif

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_ing_learn", 0, 0);
}


uint32
   arad_pp_flp_lookups_mpls_control_word_trap(
     int unit,
     uint8 ingress_learn_enable,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  flp_lookups_tbl.lem_1st_lkp_valid     = (ingress_learn_enable &&(!sa_auth_enabled && !slb_enabled))? 1 : 0;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.learn_key_select   = 0;
#if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
	  flp_lookups_tbl.elk_lkp_valid = 0x1;
	  flp_lookups_tbl.elk_wait_for_reply = 0x1;
	  flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_P2P; 
	  flp_lookups_tbl.elk_key_a_valid_bytes = 0;
	  flp_lookups_tbl.elk_key_b_valid_bytes = 2;
	  flp_lookups_tbl.elk_key_c_valid_bytes = 0;
  }
#endif
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_ing_learn", 0, 0);
}
uint32
   arad_pp_flp_lookups_ethernet_ing_ivl_learn(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 204, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = (!sa_auth_enabled && !slb_enabled)? 1 : 0 ;
  flp_lookups_tbl.lem_1st_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_A_KEY_HW_VAL;   
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_B_KEY_HW_VAL; 
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.learn_key_select   = 0; 

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 204, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_ing_ivl_learn", 0, 0);
}

uint32
   arad_pp_flp_lookups_ethernet_pon_default_upstream(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = (!sa_auth_enabled && !slb_enabled)? 1 : 0 ;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
      flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
      flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  } else {
      flp_lookups_tbl.lem_2nd_lkp_valid     = 0;
      flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
  }
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = 0;
  flp_lookups_tbl.learn_key_select   = 0;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_pon_default_upstream", 0, 0);
}


uint32
   arad_pp_flp_lookups_ethernet_pon_default_downstream(
     int unit,
     uint32 tcam_access_profile_id,
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic, 
     int32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable) {
    flp_lookups_tbl.lem_1st_lkp_valid      =  1;
    flp_lookups_tbl.lem_1st_lkp_key_select = 0; 
    flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
    flp_lookups_tbl.lem_1st_lkp_and_value  = 0;
    res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_IP6_COMPRESSION_DIP_KEY_OR_MASK,&flp_lookups_tbl.lem_1st_lkp_or_value);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    flp_lookups_tbl.tcam_lkp_key_select    = ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL;
    flp_lookups_tbl.tcam_lkp_db_profile    = tcam_access_profile_id;
  } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
    flp_lookups_tbl.lem_1st_lkp_valid      =  1;
    flp_lookups_tbl.lem_1st_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
    flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
    flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
    flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  }
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
      
      flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  } else {
      flp_lookups_tbl.learn_key_select   = 0;
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_pon_default_downstream", 0, 0);
}


uint32
   arad_pp_flp_lookups_ethernet_pon_local_route(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = (!sa_auth_enabled && !slb_enabled)? 1 : 0 ;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.learn_key_select   = 0;


  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_pon_local_route", 0, 0);
}


uint32
   arad_pp_flp_lookups_TRILL_mc_after_recycle_overlay(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit,PROG_FLP_TRILL_AFTER_TERMINATION , &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = (!sa_auth_enabled && !slb_enabled)? 1 : 0 ;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.learn_key_select   = 0;


  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit,PROG_FLP_TRILL_AFTER_TERMINATION , &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_TRILL_mc_after_recycle_overlay", 0, 0);
}

uint32
   arad_pp_flp_key_const_ethernet_mac_in_mac(
     int unit
   )
{
  uint32
      res, vsi_ce_inst = 0, pp_port_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;
  int32 prog_id = PROG_FLP_ETHERNET_MAC_IN_MAC;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,0, &vsi_ce_inst);

  if (SOC_IS_JERICHO(unit)) {
      arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 0, &pp_port_ce_inst);
  }

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  if (SOC_DPP_CONFIG(unit)->pp.test2) {
      flp_key_construction_tbl.instruction_0_16b        = vsi_ce_inst;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x19 ;
  } else {
      flp_key_construction_tbl.instruction_0_16b        =  (SOC_IS_JERICHO(unit) ? pp_port_ce_inst : ARAD_PP_FLP_16B_INST_P6_IN_PORT_D);
      flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_ETH_HEADER_OUTER_TAG;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x1B ;
  }

  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_24MSB;
  flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SA_24LSB;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ehternet_mac_in_mac", 0, 0);
}

uint32
   arad_pp_flp_lookups_ethernet_mac_in_mac(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_MAC_IN_MAC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid      = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 2;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_MAC_IN_MAC_TUNNEL_KEY_OR_MASK; 
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL; 
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_B_ETH_KEY_OR_MASK(unit);

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_MAC_IN_MAC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ethernet_mac_in_mac", 0, 0);
}

uint32
   arad_pp_flp_key_const_mac_in_mac_after_termination(
     int unit,
     int32 prog_id
   )
{
  uint32
      res, vsi_ce_inst = 0, pp_port_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,0, &vsi_ce_inst);

  if (SOC_IS_JERICHO(unit)) {
      arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 0, &pp_port_ce_inst);
  }

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  if (SOC_DPP_CONFIG(unit)->pp.test2) {
      flp_key_construction_tbl.instruction_0_16b        = vsi_ce_inst;
      flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x19 ;
  } else {
      flp_key_construction_tbl.instruction_0_16b        =  (SOC_IS_JERICHO(unit) ? pp_port_ce_inst : ARAD_PP_FLP_16B_INST_P6_IN_PORT_D);
      flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_ETH_HEADER_OUTER_TAG;
      flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x1B ;
      if (SOC_IS_JERICHO(unit)) {
          flp_key_construction_tbl.instruction_2_16b        = vsi_ce_inst;
          flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB_CB32;
          flp_key_construction_tbl.instruction_7_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
          flp_key_construction_tbl.key_d_inst_0_to_7_valid    = 0xA4 ;
      }
  }

  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_24MSB;
  flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SA_24LSB;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_mac_in_mac_after_termination", 0, 0);
}

uint32
   arad_pp_flp_lookups_mac_in_mac_after_termination(
     int unit,
     int32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid      = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_MAC_IN_MAC_TUNNEL_KEY_OR_MASK; 
  flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit); 
  flp_lookups_tbl.learn_key_select       = 0;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_mac_in_mac_after_termination", 0, 0);
}

uint32
   arad_pp_flp_process_mac_in_mac_after_termination(
     int unit,
     uint8 learn_enable,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.sa_lkp_result_select           = 0;
  flp_process_tbl.sa_lkp_process_enable          = 0;
  flp_process_tbl.procedure_ethernet_default     = 3;
  flp_process_tbl.enable_hair_pin_filter         = 1;
  flp_process_tbl.learn_enable                   = learn_enable;
  flp_process_tbl.not_found_trap_strength        = 0;
  flp_process_tbl.unknown_address                = 3;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_mac_in_mac_after_termination", 0, 0);
}


uint32
   arad_pp_flp_process_key_program_tm(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;

  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  ARAD_CLEAR(&flp_process_tbl, ARAD_PP_IHB_FLP_PROCESS_TBL_DATA, 1);
  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_TM, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 0;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_TM,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);


exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_key_program_tm", 0, 0);
}


uint32
   arad_pp_flp_process_tm(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_TM, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 0;
  flp_process_tbl.include_lem_1st_in_result_b    = 0;
  flp_process_tbl.include_lpm_2nd_in_result_a    = 0;
  flp_process_tbl.include_lpm_1st_in_result_b    = 0;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.procedure_ethernet_default  = 0;
  flp_process_tbl.enable_hair_pin_filter       = 0;
  flp_process_tbl.enable_rpf_check            = 0;
  
  flp_process_tbl.select_default_result_b = 2;
  flp_process_tbl.select_default_result_a = 2;
  flp_process_tbl.not_found_trap_strength = 0;
  flp_process_tbl.not_found_snoop_strength = 0;
  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_TM, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 7; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_TM,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_tm", 0, 0);
}


uint32
   arad_pp_flp_lookups_p2p(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_P2P, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

#if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
      flp_lookups_tbl.elk_lkp_valid = 0x1;
      flp_lookups_tbl.elk_wait_for_reply = 0x1;
      flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_P2P;

      flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

      flp_lookups_tbl.elk_key_a_valid_bytes = 2;
      flp_lookups_tbl.elk_key_b_valid_bytes = 0;
      flp_lookups_tbl.elk_key_c_valid_bytes = 0;
  }
#endif

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_P2P, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 243, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_p2p", 0, 0);
}

uint32
   arad_pp_flp_process_p2p(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_P2P, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.procedure_ethernet_default   = 0;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.apply_fwd_result_a           = 0;
  flp_process_tbl.sa_lkp_process_enable        = 0;
  flp_process_tbl.learn_enable                 = 0;
  flp_process_tbl.not_found_trap_strength      = 0;

  flp_process_tbl.unknown_address = 0; 

#if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
    flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION;
    flp_process_tbl.elk_result_format = 1;
    
    flp_process_tbl.include_elk_fwd_in_result_a = 0;
    flp_process_tbl.include_elk_ext_in_result_a = 0;
    flp_process_tbl.include_elk_fwd_in_result_b = 0;
    flp_process_tbl.include_elk_ext_in_result_b = 0;
  }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_P2P, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 0;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_P2P,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_p2p", 0, 0);
}

uint32
   arad_pp_flp_key_const_p2p(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_P2P, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  #if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
      flp_key_construction_tbl.instruction_0_16b       = ARAD_PP_FLP_16B_INST_P6_IN_LIF_D;
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x1 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_P2P, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_P2P+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  }
  #endif

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_P2P+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_p2p", 0, 0);
}


uint32
   arad_pp_flp_process_ethernet_tk_epon_uni_v4(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  uint32
    tmp,
    flp_prog_sel;
  int
    i;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (i = 0; i < 2; i++) { 
    
    if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable && (i==0)) {
      continue;
    }

    flp_prog_sel = (i == 0) ? PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP:PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC;

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, flp_prog_sel, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    flp_process_tbl.include_lem_1st_in_result_b    = 1;
    flp_process_tbl.result_a_format                = 0;
    flp_process_tbl.result_b_format                = 0;
    flp_process_tbl.sa_lkp_result_select           = 1;
    flp_process_tbl.sa_lkp_process_enable          = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
    flp_process_tbl.procedure_ethernet_default     = 3;
    flp_process_tbl.enable_hair_pin_filter         = 1;
    flp_process_tbl.learn_enable                   = learn_enable;
    flp_process_tbl.select_default_result_b        = 3;
    flp_process_tbl.not_found_trap_code            = 0;
    flp_process_tbl.not_found_trap_strength        = 0;
    flp_process_tbl.not_found_snoop_strength       = 0;
    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.local_switching_enable &&
        flp_prog_sel == PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC) {
        flp_process_tbl.include_lem_2nd_in_result_a    = 1;
    }

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, flp_prog_sel, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    tmp = 0; 
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*flp_prog_sel,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_tk_epon_uni_v4", 0, 0);
}

uint32
   arad_pp_flp_process_ethernet_tk_epon_uni_v6(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  uint32
    tmp,
    flp_prog_sel;
  int
    i;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  flp_prog_sel = PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP;



  for (i = 0; i < 2; i++) { 
    
    if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable && (i==0)) {
      continue;
    }

    flp_prog_sel = (i == 0) ? PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP:PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC;

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, flp_prog_sel, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    flp_process_tbl.include_lem_1st_in_result_b    = 1;
    flp_process_tbl.result_a_format                = 0;
    flp_process_tbl.result_b_format                = 0;
    flp_process_tbl.sa_lkp_result_select           = 1;
    flp_process_tbl.sa_lkp_process_enable          = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
    flp_process_tbl.procedure_ethernet_default     = 3;
    flp_process_tbl.enable_hair_pin_filter         = 1;
    flp_process_tbl.learn_enable                   = learn_enable;
    flp_process_tbl.select_default_result_b        = 3;
    flp_process_tbl.not_found_trap_code            = 0;
    flp_process_tbl.not_found_trap_strength        = 0;
    flp_process_tbl.not_found_snoop_strength       = 0;
    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.local_switching_enable &&
        flp_prog_sel == PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC) {
        flp_process_tbl.include_lem_2nd_in_result_a    = 1;
    }

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, flp_prog_sel, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    tmp = 0; 
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*flp_prog_sel,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_tk_epon_uni_v6", 0, 0);
}

uint32
   arad_pp_flp_process_pon_arp_downstream(
     int unit,
     int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.procedure_ethernet_default     = 3;
  flp_process_tbl.enable_hair_pin_filter         = 1;
  flp_process_tbl.enable_rpf_check               = 0;
  flp_process_tbl.not_found_trap_code            = 0;
  flp_process_tbl.not_found_trap_strength        = 0;
  flp_process_tbl.not_found_snoop_strength       = 0;
  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pon_arp_downstream", 0, 0);
}

uint32
   arad_pp_flp_process_pon_arp_upstream(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;

  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.sa_lkp_result_select           = 1;
  flp_process_tbl.sa_lkp_process_enable          = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default     = 3;
  flp_process_tbl.enable_hair_pin_filter         = 1;
  flp_process_tbl.learn_enable                   = learn_enable;
  flp_process_tbl.select_default_result_b        = 3;
  flp_process_tbl.not_found_trap_code            = 0;
  flp_process_tbl.not_found_trap_strength        = 0;
  flp_process_tbl.not_found_snoop_strength       = 0;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pon_arp_upstream", 0, 0);
}



uint32
   arad_pp_flp_process_oam_statistics(
     int unit,
     uint32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = 1;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = 0;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_oam_statistics", 0, 0);
}

uint32
   arad_pp_flp_process_bfd_statistics(
     int unit,
     uint32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&flp_process_tbl, ARAD_PP_IHB_FLP_PROCESS_TBL_DATA, 1);

 

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 0;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_oam_statistics", 0, 0);
}

uint32
   arad_pp_flp_process_vpws_tagged(int unit)
{
  uint32 res;
  uint32 tmp;
  soc_reg_above_64_val_t reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;
  uint8 is_single_tag;
  uint32 flp_prog;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (is_single_tag=0; is_single_tag<=1; is_single_tag++) {
      flp_prog = is_single_tag ? PROG_FLP_VPWS_TAGGED_SINGLE_TAG:PROG_FLP_VPWS_TAGGED_DOUBLE_TAG;
      res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, flp_prog, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

      flp_process_tbl.include_lem_1st_in_result_a   = 1;
      flp_process_tbl.include_lem_2nd_in_result_a   = 1;
      flp_process_tbl.result_a_format               = 0;
      flp_process_tbl.result_b_format               = 0;
      flp_process_tbl.sa_lkp_result_select          = 0;
      flp_process_tbl.sa_lkp_process_enable         = 0;
      flp_process_tbl.procedure_ethernet_default    = 3;
      flp_process_tbl.enable_hair_pin_filter        = 1;
      flp_process_tbl.learn_enable                  = 0;
      flp_process_tbl.not_found_trap_strength       = 0;
      flp_process_tbl.unknown_address               = 0;

      res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, flp_prog, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

      tmp = 0; 
      SOC_REG_ABOVE_64_CLEAR(reg_val);
      res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      SHR_BITCOPY_RANGE(reg_val,3*flp_prog,&tmp,0,3);
      res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_vpws_tagged", 0, 0);
}


uint32
   arad_pp_flp_process_ethernet_ing_learn(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

#if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
	  flp_process_tbl.elk_result_format = 1;
	  
	  flp_process_tbl.include_elk_fwd_in_result_a = 0;
	  flp_process_tbl.include_elk_ext_in_result_a = 0;
	  flp_process_tbl.include_elk_fwd_in_result_b = 0;
	  flp_process_tbl.include_elk_ext_in_result_b = 0;
  }
#endif
  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_ETHERNET_ING_LEARN,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_ing_learn", 0, 0);
}

uint32
   arad_pp_flp_process_mpls_control_word_trap(
     int unit,
     uint8 learn_enable,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

#if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
	  flp_process_tbl.elk_result_format = 1;
	  flp_process_tbl.include_elk_fwd_in_result_a = 0;
	  flp_process_tbl.include_elk_ext_in_result_a = 0;
	  flp_process_tbl.include_elk_fwd_in_result_b = 0;
	  flp_process_tbl.include_elk_ext_in_result_b = 0;
  }
#endif
  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_MPLS_CONTROL_WORD_TRAP,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_ing_learn", 0, 0);
}

uint32
   arad_pp_flp_process_ethernet_ing_all_ivl_learn(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 401, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1; 
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

#if defined(INCLUDE_KBP)
  if (ARAD_KBP_ENABLE_P2P_EXTENDED) {
	  flp_process_tbl.elk_result_format = 1;
	  
	  flp_process_tbl.include_elk_fwd_in_result_a = 0;
	  flp_process_tbl.include_elk_ext_in_result_a = 0;
	  flp_process_tbl.include_elk_fwd_in_result_b = 0;
	  flp_process_tbl.include_elk_ext_in_result_b = 0;
  }
#endif
  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 401, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 401, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 401, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_ing_all_ivl_learn", 0, 0);
}



uint32
   arad_pp_flp_process_ethernet_pon_default_upstream(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
      flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  } else {
      flp_process_tbl.include_lem_2nd_in_result_a    = 0;
  }
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_pon_default_upstream", 0, 0);
}


uint32
   arad_pp_flp_process_ethernet_pon_default_downstream(
     int unit,
     uint8 learn_enable, 
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 0;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
      
      flp_process_tbl.learn_enable               = 1;
  } else {
      flp_process_tbl.learn_enable               = 0;
  }
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;
  if (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable) {
    flp_process_tbl.include_lem_1st_in_result_b    = 1;
    flp_process_tbl.include_tcam_in_result_a       = 0;
  }

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_pon_default_downstream", 0, 0);
}


uint32
   arad_pp_flp_process_ethernet_pon_local_route(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_pon_local_route", 0, 0);
}


uint32
   arad_pp_flp_process_TRILL_mc_after_recycle_overlay(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_TRILL_AFTER_TERMINATION, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_TRILL_AFTER_TERMINATION, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_TRILL_AFTER_TERMINATION,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_TRILL_mc_after_recycle_overlay", 0, 0);
}


uint32
   arad_pp_flp_process_pon_vmac_upstream(
     int unit,
     uint32 prog_id,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.include_lem_1st_in_result_a    = 0;
  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_2nd_in_result_b    = 0;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.sa_lkp_result_select           = 1;
  flp_process_tbl.sa_lkp_process_enable          = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default     = 2;
  flp_process_tbl.enable_hair_pin_filter         = 1;
  flp_process_tbl.learn_enable                   = learn_enable;
  flp_process_tbl.select_default_result_b        = 3;
  flp_process_tbl.select_default_result_a        = 3;
  flp_process_tbl.not_found_trap_code            = 0;
  flp_process_tbl.not_found_trap_strength        = 0;
  flp_process_tbl.not_found_snoop_strength       = 0;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pon_vmac_upstream", 0, 0);
}

uint32
   arad_pp_flp_process_pon_vmac_downstream(
     int unit,
     uint32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.sa_lkp_result_select           = 1;
  flp_process_tbl.sa_lkp_process_enable          = 0;
  flp_process_tbl.procedure_ethernet_default     = 0;
  flp_process_tbl.enable_hair_pin_filter         = 1;
  flp_process_tbl.learn_enable                   = 0;
  flp_process_tbl.select_default_result_b        = 3;
  flp_process_tbl.select_default_result_a        = 3;
  flp_process_tbl.not_found_trap_code            = 0;
  flp_process_tbl.not_found_trap_strength        = 0;
  flp_process_tbl.not_found_snoop_strength       = 0;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pon_vmac_upstream", 0, 0);
}


uint32
   arad_pp_flp_ethernet_prog_learn_get(
     int unit,
     uint8 *learn_enable
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  *learn_enable = flp_process_tbl.learn_enable;


exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_ethernet_prog_learn_get", 0, 0);
}


uint32
   arad_pp_flp_process_ethernet_mac_in_mac(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_MAC_IN_MAC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.result_a_format                = 1;
  flp_process_tbl.sa_lkp_result_select           = 0;
  flp_process_tbl.sa_lkp_process_enable          = 1;
  flp_process_tbl.procedure_ethernet_default     = 3;
  flp_process_tbl.enable_hair_pin_filter         = 1;
  flp_process_tbl.learn_enable                   = 0;
  flp_process_tbl.not_found_trap_strength        = 0;


  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_MAC_IN_MAC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  SHR_BITCOPY_RANGE(reg_val, 3*PROG_FLP_ETHERNET_MAC_IN_MAC, &tmp, 0, 3);

  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ethernet_ing_learn", 0, 0);
}

uint32
   arad_pp_flp_key_const_ethernet_tk_epon_uni_v4_dhcp(
     int unit
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  if (!SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
    
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

    flp_key_construction_tbl.instruction_0_16b        = ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D; 
    flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_CE_SA_FWD_HEADER_40_32_CB16; 
    flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB; 
    if (SOC_IS_JERICHO(unit)){
        flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_FLP_32B_INST_P6_GLOBAL_IN_LIF_D; 
        flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x35 ;
    }
    else{
       flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_FLP_16B_INST_P6_IN_LIF_D;
       flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x17 ;
    }
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
    flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
    flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
    flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0xB ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  }


exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_tk_epon_uni_v4_dhcp", 0, 0);
}


uint32
   arad_pp_flp_key_const_ethernet_tk_epon_uni_v4_static(
     int unit
   )
{
    uint32
        res, fid_ce_inst = 0;
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
        flp_key_construction_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

    
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 143, exit);

    if (SOC_IS_JERICHO(unit)){
        flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_IPV4_SIP_HDR2_HEADER;
    }
    else{
        flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_IPV4_SIP_HDR2_HEADER;
    }

    if (SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_OR_ARP_ENABLE(unit)) {
        flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
        flp_key_construction_tbl.key_a_inst_0_to_5_valid  = (SOC_IS_JERICHO(unit))?0x21:0x9 ; 
    } else {
        if (SOC_IS_JERICHO(unit)){
            flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_FLP_32B_INST_P6_GLOBAL_IN_LIF_D; 
            flp_key_construction_tbl.key_a_inst_0_to_5_valid  = 0x30 ;
        }
        else{
            flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_FLP_16B_INST_P6_IN_LIF_D;
            flp_key_construction_tbl.key_a_inst_0_to_5_valid  = 0xC ;
        }
    }

    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 144, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);
    flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
    flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
    flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0xB ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 146, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_tk_epon_uni_v4_static", 0, 0);
}

uint32
   arad_pp_flp_key_const_ethernet_tk_epon_uni_v6(
     int unit
   )
{
  uint32
    res, fid_ce_inst = 0, tt9b_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 9, SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0, &tt9b_ce_inst);

  if (!SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
    
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

    flp_key_construction_tbl.instruction_0_16b        = ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D; 
    flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_CE_SA_FWD_HEADER_40_32_CB16; 
    flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;  
    if (SOC_IS_JERICHO(unit)){
       flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_FLP_32B_INST_P6_GLOBAL_IN_LIF_D; 
       flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x35 ;
    }
    else{
       flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_FLP_16B_INST_P6_IN_LIF_D; 
       flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x17 ;
    }
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
    flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
    flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
    flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0xB ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
  }

  if (!SOC_IS_JERICHO(unit)){
      
      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 143, exit);
      if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
         flp_key_construction_tbl.instruction_0_16b        = tt9b_ce_inst; 
         flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_63_32; 
         flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_31_0; 
         flp_key_construction_tbl.key_a_inst_0_to_5_valid    = SOC_IS_JERICHO(unit)? 0x1801 : 0x19 ;
         flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
         flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
      } else {
         flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_FLP_16B_INST_P6_IN_LIF_D; 
         flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_127_96;
         flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_95_64;
         flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_63_32;
         flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
         flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
         flp_key_construction_tbl.key_c_inst_0_to_5_valid    = SOC_IS_JERICHO(unit)? 0x3804 : 0x3C ;
      }
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 144, exit);

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);
      flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_31_0;
      flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
      flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
      flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
      flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x13 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid    = (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable ? 0x0: 0x8) ;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 146, exit);
   }
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_tk_epon_uni_v6", 0, 0);
}

uint32
   arad_pp_flp_key_const_pon_arp_downstream(
     int unit,
     int prog_id
   )
{
  uint32
    res, fid_ce_inst;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_ARP_TPA_HEADER_2;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x9 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_pon_arp_downstream", 0, 0);
}


uint32
   arad_pp_flp_key_const_pon_arp_upstream(
     int unit,
     int prog_id
   )
{
  uint32
    res, fid_ce_inst;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_key_construction_tbl.instruction_0_16b          = fid_ce_inst;
  flp_key_construction_tbl.instruction_3_32b          = ARAD_PP_CE_ARP_SPA_HEADER_2;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x9 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0xB ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_pon_arp_upstream", 0, 0);
}


uint32
   arad_pp_flp_key_const_ethernet_ing_learn(
     int unit
   )
{
  uint32
    res, fid_ce_inst;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB_CB32;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x9 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;

#if defined(INCLUDE_KBP)
	if (ARAD_KBP_ENABLE_P2P_EXTENDED) { 
        flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_FLP_16B_INST_P6_IN_LIF_D;
		flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x2 ;
	}
#endif

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x8 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_ETHERNET_ING_LEARN+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_ing_learn", 0, 0);
}

uint32
   arad_pp_flp_key_const_mpls_control_word_trap(
     int unit
   )
{
  uint32
    res, fid_ce_inst;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB_CB32;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x9 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;

#if defined(INCLUDE_KBP)
	if (ARAD_KBP_ENABLE_P2P_EXTENDED) { 
        flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_FLP_16B_INST_P6_IN_LIF_D;
		flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x2 ;
	}
#endif

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x8 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_MPLS_CONTROL_WORD_TRAP+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_ing_learn", 0, 0);
}

uint32
   arad_pp_flp_key_const_ethernet_pon_default_upstream(
     int unit,
     int32 prog_id
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0xB ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_pon_default_upstream", 0, 0);
}


uint32
   arad_pp_flp_key_const_ethernet_pon_default_downstream(
     int unit,
     int32 prog_id
   )
{
  uint32
    res, tt12b_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 12, SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0, &tt12b_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable) {
    flp_key_construction_tbl.instruction_0_16b        = tt12b_ce_inst; 
    flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_55_32; 
    flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_31_0; 
    if (SOC_IS_JERICHO(unit)) {
        flp_key_construction_tbl.instruction_1_16b    = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_127_112;
        flp_key_construction_tbl.instruction_5_32b    = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_111_80;
    } else {
        flp_key_construction_tbl.instruction_5_32b    = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_127_96; 
    }
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x19 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = SOC_IS_JERICHO(unit) ? 0x2200 : 0x20;
  } else {
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable) {
    if (SOC_IS_JERICHO(unit)) {
        flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_79_64;
    } else {
        flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_95_64;
    }
    flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_63_32;
    flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_31_0;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid  = 0x38;
  } else {
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_pon_default_downstream", 0, 0);
}


uint32
   arad_pp_flp_key_const_ethernet_pon_local_route(
     int unit,
     int32 prog_id
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0xB ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ethernet_pon_local_route", 0, 0);
}


uint32
   arad_pp_flp_key_const_TRILL_mc_after_recycle_overlay(
     int unit
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_TRILL_AFTER_TERMINATION, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  
  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_TRILL_NATIVE_DA_32MSB;
  flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_TRILL_NATIVE_DA_16LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0  ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x31 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0  ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_TRILL_AFTER_TERMINATION, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_TRILL_AFTER_TERMINATION+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_TRILL_NATIVE_SA_32MSB;
  flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_TRILL_NATIVE_SA_16LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x19 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0  ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0  ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_TRILL_AFTER_TERMINATION+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_TRILL_mc_after_recycle_overlay", 0, 0);
}

uint32
   arad_pp_flp_key_const_ipv4uc_l3vpn(
     int unit
   )
{
  uint32
    res, vrf_ce_inst;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4UC_PUBLIC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_DIP_FWD_HEADER;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x8 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4UC_PUBLIC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4UC_PUBLIC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_CE_DIP_FWD_HEADER;
  flp_key_construction_tbl.instruction_0_16b     = vrf_ce_inst;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x9 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4UC_PUBLIC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv4uc_with_l3vpn", 0, 0);
}

uint32
   arad_pp_flp_key_const_ipv6uc_with_rpf_2pass(
     int unit,
     int32 prog_id
   )
{
  uint32
    res, vrf_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

    res = arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

    flp_key_construction_tbl.instruction_0_16b        = vrf_ce_inst;
    flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_63_48
    flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_47_32
    flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_127_96
    flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_95_64
    flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_31_0
    flp_key_construction_tbl.key_a_inst_0_to_5_valid  = 0x19 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid  = 0x26 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid  = 0x0  ;


    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 241, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv6uc_with_rpf_2pass", 0, 0);
}

uint32
   arad_pp_flp_key_const_ipv4uc_rpf(
     int unit
   )
{
  uint32
    res, vrf_ce_inst = 0, rif_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 1, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4UC_RPF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b        = vrf_ce_inst;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SIP_FWD_HEADER;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x9 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4UC_RPF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4UC_RPF+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_CE_DIP_FWD_HEADER;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x8; 
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0;
	  if(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
		flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_FLP_32B_INST_N_ZEROS(4);
		flp_key_construction_tbl.instruction_5_32b     = rif_ce_inst;
		flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x38; 
	}
  }else
#endif
  {
      flp_key_construction_tbl.instruction_0_16b     = vrf_ce_inst;
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x9 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4UC_RPF+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv4uc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_key_const_ipv4uc(
     int unit
   )
{
  uint32
    res, vrf_ce_inst, rif_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 1, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);


  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4UC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b        = vrf_ce_inst;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SIP_FWD_HEADER;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x9 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
#if defined(INCLUDE_KBP)
  if( ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED &
     (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1)){
      flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x9 ;
  }
  #endif
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4UC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4UC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_CE_DIP_FWD_HEADER;


#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x8; 
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0;
	  if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
		flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x38; 
		flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_FLP_32B_INST_N_ZEROS(4);
		flp_key_construction_tbl.instruction_5_32b     = rif_ce_inst;
		if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1) {
              flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0;
              flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0; 
              flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x38;
          }
	  }
  }
  else
#endif
  {
      flp_key_construction_tbl.instruction_0_16b     = vrf_ce_inst;
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x9 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4UC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv4uc", 0, 0);
}

uint32
   arad_pp_flp_key_const_bfd_single_hop(
     int unit,
	 int32  prog
   )
{
  uint32
    res, vrf_ce_inst = 0, header4_ce_inst = 0, ttl_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_HEADER_OFFSET4,0, &header4_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_TTL,0, &ttl_ce_inst);


  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b        = ttl_ce_inst;

  flp_key_construction_tbl.instruction_1_16b        = header4_ce_inst;

  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_YOUR_DESCRIMINATOR;

  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0)) {
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x8;  
   } else {
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x9    ; 
  }
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x2 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_bfd_single_hop", 0, 0);
}

uint32
   arad_pp_flp_key_const_ipv6uc(
     int unit
   )
{
  uint32
    res, vrf_ce_inst;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV6UC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b        = vrf_ce_inst;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_63_48
  flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_47_32
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_127_96
  flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_95_64
  flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_31_0

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_UC || ARAD_KBP_ENABLE_IPV6_EXTENDED){
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x19;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x26;
      if(ARAD_KBP_ENABLE_IPV6_EXTENDED){
          flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x1; 
          flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0;
      }
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0;
  }
  else
#endif
  {
      flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x3F ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV6UC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV6UC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b     = vrf_ce_inst;
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_127_112
  flp_key_construction_tbl.instruction_2_16b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_111_96
  flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_95_64
  flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_63_32
  flp_key_construction_tbl.instruction_5_32b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_31_0

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_UC || ARAD_KBP_ENABLE_IPV6_EXTENDED){
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3e; 
  }
  else
#endif
  {
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV6UC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv6uc", 0, 0);
}


uint32
   arad_pp_flp_key_const_ipv6mc(
     int unit
   )
{
  uint32
    res, vrf_ce_inst = 0, rif_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);


  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV6MC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_MC){
      
      flp_key_construction_tbl.instruction_0_16b = vrf_ce_inst | 0xf000;
      flp_key_construction_tbl.instruction_1_16b = rif_ce_inst | 0xf000;
      flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_127_96;

      
      flp_key_construction_tbl.instruction_2_16b = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_95_80;
      flp_key_construction_tbl.instruction_4_32b = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_79_48;

      
      flp_key_construction_tbl.instruction_5_32b = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_31_0;

      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0xb;  
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x14; 
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x20; 
  }
  else
#endif
  {
      flp_key_construction_tbl.instruction_0_16b        = rif_ce_inst;
      flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_119_112
      flp_key_construction_tbl.instruction_2_16b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_111_96
      flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_95_64
      flp_key_construction_tbl.instruction_4_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_63_32
      flp_key_construction_tbl.instruction_5_32b        = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_31_0
      flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x3F ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV6MC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV6MC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_MC){
      
      flp_key_construction_tbl.instruction_0_16b = ARAD_PP_CE_SIP_IPV6_FWD_HEADER_47_32;

      
      flp_key_construction_tbl.instruction_1_16b = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_127_112;
      flp_key_construction_tbl.instruction_2_16b = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_111_96;
      flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_95_64;
      flp_key_construction_tbl.instruction_4_32b = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_63_32;
      flp_key_construction_tbl.instruction_5_32b = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_31_0;

      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x1;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3e; 
  }
  else
#endif
  {
      flp_key_construction_tbl.instruction_0_16b     = rif_ce_inst;
      flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_119_112
      flp_key_construction_tbl.instruction_2_16b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_111_96
      flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_95_64
      flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_63_32
      flp_key_construction_tbl.instruction_5_32b     = ARAD_PP_CE_DIP_IPV6_FWD_HEADER_31_0
      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3F ;
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV6MC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv6mc", 0, 0);
}

uint32
   arad_pp_flp_key_const_pwe_gre(
     int    unit,
     uint8  in_rif,
     uint8  in_exp
   )
{
  uint32
    res, mpls_exp_ce_inst = 0, rif_ce_inst = 0;
  uint32
    prog;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_MPLS_EXP,0, &mpls_exp_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);

  res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_VPLSOGRE, &prog);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b     = (in_rif)  ? rif_ce_inst:ARAD_PP_FLP_16B_INST_12_ZEROS;
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_FLP_16B_INST_8_ZEROS;
  flp_key_construction_tbl.instruction_2_16b     =             mpls_exp_ce_inst;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x7 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_CE_MPLS_FWD_HEADER_15_0; 
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_CE_MPLS_FWD_HEADER_19_16;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x3 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_pwe_gre", 0, 0);
}

#if defined(INCLUDE_KBP)
uint32
   arad_pp_flp_key_const_lsr(
     int unit,
     uint8  in_port,
     uint8  in_rif,
     uint8  in_exp
   )
{
  uint32
      pp_port_ce_inst = 0;
  uint32
    res, mpls_exp_ce_inst = 0, rif_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_MPLS_EXP,0, &mpls_exp_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);

  if (SOC_IS_JERICHO(unit)) {
      arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 0, &pp_port_ce_inst);
  }

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_LSR, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b     =  ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D_3_BITS; 
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x1 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_LSR, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_LSR+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_CE_MPLS_FWD_HEADER_15_0; 
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_CE_MPLS_FWD_HEADER_19_16;
  flp_key_construction_tbl.instruction_2_16b     = ARAD_PP_CE_MPLS_FWD_HEADER_BOS_BIT_23;

  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x7 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_LSR+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  
  res = arad_pp_flp_lpm_custom_lookup_enable(unit, PROG_FLP_LSR);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_lsr", 0, 0);
}
#endif

uint32
   arad_pp_flp_lsr_stat_init(int unit)
{
    uint32 res=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    res = arad_pp_flp_instruction_rsrc_set(unit, PROG_FLP_LSR);
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    arad_pp_flp_dbal_mpls_lsr_stat_table_init(unit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_lsr", 0, 0);
}



uint32
   arad_pp_flp_key_const_fcoe_fcf_npv(
     int unit,
     int32  progs[2],
     int is_vsan_from_vsi
   )
{
    uint32
        res;
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
        flp_key_construction_tbl;
    uint32
        vrf_bits;
    uint32
        vsi_vft_instuction,
        key_gen_var;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (is_vsan_from_vsi == 0) {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D_13_BITS;
    } else {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_VSI(12);
    }

    vrf_bits = soc_sand_log2_round_up(((SOC_DPP_CONFIG(unit))->l3.max_nof_vrfs));

    
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[0], &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
    flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_0_PROGRAM_KEY_GEN_VAR(vrf_bits); 
    flp_key_construction_tbl.instruction_1_16b     = vsi_vft_instuction;
    flp_key_construction_tbl.instruction_2_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(1);
    flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_S_ID;
    flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0  ;
    if (is_vsan_from_vsi == 0) {
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x12 ;
        flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3  ;
    } else {
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x16 ;
        flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x7  ;
    }

    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[0], &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[0]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
    flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_ARAD_FC_S_ID_8_MSB;
    flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(32-ARAD_PP_FLP_VFT_NOF_BITS-8);
    flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[0]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

    
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[1], &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
    flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_0_PROGRAM_KEY_GEN_VAR(vrf_bits); 

    if (is_vsan_from_vsi == 0) {
      flp_key_construction_tbl.instruction_1_16b = ARAD_PP_FLP_16B_INST_ARAD_FC_WITH_VFT_VFT_ID;
    } else {
      flp_key_construction_tbl.instruction_1_16b = ARAD_PP_FLP_16B_INST_P6_VSI(12);}

    flp_key_construction_tbl.instruction_2_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(1);
    flp_key_construction_tbl.instruction_5_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_WITH_VFT_S_ID;
    flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0  ;
    if (is_vsan_from_vsi == 0) {
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x22 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3  ;
    } else {
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x26 ;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x7  ;
    }
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[1], &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[1]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
    flp_key_construction_tbl.instruction_0_16b     = ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_S_ID_8_MSB; 
    flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(32-ARAD_PP_FLP_VFT_NOF_BITS-8);
    flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[1]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

    
    if (!SOC_IS_JERICHO(unit)) {
        key_gen_var = (((SOC_DPP_CONFIG(unit))->pp.fcoe_reserved_vrf) + 1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_FLP_PROGRAM_KEY_GEN_VARm(unit,MEM_BLOCK_ANY,progs[0],&key_gen_var));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_IHP_FLP_PROGRAM_KEY_GEN_VARm(unit,MEM_BLOCK_ANY,progs[1],&key_gen_var));
    }

    if (SOC_IS_JERICHO(unit)) {
        

        
      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[0], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_0_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[0], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[0]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_0_16b = 0;
      flp_key_construction_tbl.instruction_1_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[0]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      
      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[1], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_0_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[1], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[1]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_0_16b = 0;
      flp_key_construction_tbl.instruction_1_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[1]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      
      res = arad_pp_flp_instruction_rsrc_set(unit, progs[0]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
      res = arad_pp_flp_instruction_rsrc_set(unit, progs[1]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_fcoe_fcf_npv", 0, 0);
}


uint32
   arad_pp_flp_key_const_fcoe_fcf(
     int unit,
     int32  progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS],
     int is_vsan_from_vsi
   )
{
  uint32
    res;
  uint32
    vrf_bits;
  uint32
      vsi_vft_instuction,
      key_gen_var;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (is_vsan_from_vsi == 0) {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D_13_BITS;
    } else {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_VSI(12);
    }

  
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[0], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b     = vsi_vft_instuction;
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(1);
  flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_S_ID;
  flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_D_ID;
  if (is_vsan_from_vsi == 0) {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x19 ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x11 ;
  } else {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x1B ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x13 ;
  }
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[0], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[0]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[0]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[1], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  if (is_vsan_from_vsi == 0) {
      flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_ARAD_FC_WITH_VFT_VFT_ID;
  } else {
      flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_P6_VSI(12);}
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(1);
  flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_WITH_VFT_S_ID;
  flp_key_construction_tbl.instruction_5_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_WITH_VFT_D_ID;
  if (is_vsan_from_vsi == 0) {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x31 ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x21 ;
  } else {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x33 ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x23 ;
  }
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[1], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[1]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[1]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

    
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[2], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  vrf_bits = soc_sand_log2_round_up(((SOC_DPP_CONFIG(unit))->l3.max_nof_vrfs));

  flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_0_PROGRAM_KEY_GEN_VAR(vrf_bits); 
  flp_key_construction_tbl.instruction_1_16b     = vsi_vft_instuction;
  flp_key_construction_tbl.instruction_2_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(1);
  flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_S_ID;
  flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_D_ID;
  if (is_vsan_from_vsi == 0) {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x1A ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x2 ;
        flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3 ; 
  } else {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x1E ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x6 ;
        flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x7 ; 
  }
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[2], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[2]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_ARAD_FC_D_ID_8_MSB;
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(32-ARAD_PP_FLP_VFT_NOF_BITS-8);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x1 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3 ; 
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[2]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[3], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b     = ARAD_PP_FLP_16B_INST_0_PROGRAM_KEY_GEN_VAR(vrf_bits); 

  if (is_vsan_from_vsi == 0) {
      flp_key_construction_tbl.instruction_1_16b = ARAD_PP_FLP_16B_INST_ARAD_FC_WITH_VFT_VFT_ID; 
  } else {
      flp_key_construction_tbl.instruction_1_16b = ARAD_PP_FLP_16B_INST_P6_VSI(12);}

  flp_key_construction_tbl.instruction_2_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(1);
  flp_key_construction_tbl.instruction_4_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_WITH_VFT_S_ID; 
  flp_key_construction_tbl.instruction_5_32b     = ARAD_PP_FLP_32B_INST_ARAD_FC_WITH_VFT_D_ID; 
  if (is_vsan_from_vsi == 0) {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x32 ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x2 ;
        flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3; 
  } else {
        flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x36 ;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x6 ;
        flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x7; 
  }

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[3], &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[3]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  flp_key_construction_tbl.instruction_0_16b     = ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_D_ID_8_MSB; 
  flp_key_construction_tbl.instruction_1_16b     = ARAD_PP_FLP_16B_INST_N_ZEROS(32-ARAD_PP_FLP_VFT_NOF_BITS-8);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x1 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x3 ; 
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[3]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  if (!SOC_IS_JERICHO(unit)) { 
      
      key_gen_var = (SOC_DPP_CONFIG(unit))->pp.fcoe_reserved_vrf;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_FLP_PROGRAM_KEY_GEN_VARm(unit,MEM_BLOCK_ANY,progs[0],&key_gen_var));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_IHP_FLP_PROGRAM_KEY_GEN_VARm(unit,MEM_BLOCK_ANY,progs[1],&key_gen_var));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_IHP_FLP_PROGRAM_KEY_GEN_VARm(unit,MEM_BLOCK_ANY,progs[2],&key_gen_var));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHP_FLP_PROGRAM_KEY_GEN_VARm(unit,MEM_BLOCK_ANY,progs[3],&key_gen_var));
  }

  if (SOC_IS_JERICHO(unit)) {
      

      
      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[2], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_0_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[2], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[2]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_1_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[2]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      
      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[3], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_0_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[3], &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, progs[3]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      flp_key_construction_tbl.instruction_1_16b = 0;
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, progs[3]+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      
      res = arad_pp_flp_instruction_rsrc_set(unit, progs[2]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      res = arad_pp_flp_instruction_rsrc_set(unit, progs[3]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      res = arad_pp_flp_dbal_fcoe_program_tables_init(unit, is_vsan_from_vsi, progs[2], progs[3]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
#endif
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_fcoe_fcf", 0, 0);
}


uint32
   arad_pp_flp_key_const_TRILL_uc(
     int unit
   )
{
  uint32
    res, src_port_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,0, &src_port_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_TRILL_UC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b = ARAD_PP_CE_TRILL_EGRESS_NICK;
  flp_key_construction_tbl.instruction_1_16b = src_port_ce_inst;
  flp_key_construction_tbl.instruction_2_16b = ARAD_PP_CE_TRILL_SA_16MSB;
  flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_TRILL_SA_32LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x1 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0xe ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_TRILL_UC, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_TRILL_UC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_TRILL_UC+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  res = arad_pp_flp_instruction_rsrc_set(unit, PROG_FLP_TRILL_UC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_TRILL", 0, 0);
}

uint32
   arad_pp_flp_key_const_TRILL_mc(
     int unit)
{
  uint32
    res, fid_ce_inst = 0, esadi_ce_inst, src_port_ce_inst = 0,
    prog ;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;
  uint8 is_two_tags;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI,0, &esadi_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,0, &src_port_ce_inst);

  
  for (is_two_tags = 0; is_two_tags <= 1; ++is_two_tags) {
      prog = is_two_tags ?
          PROG_FLP_TRILL_MC_TWO_TAGS :
          PROG_FLP_TRILL_MC_ONE_TAG;

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_TRILL_MC &&
         (prog == PROG_FLP_TRILL_MC_ONE_TAG))
      {
          flp_key_construction_tbl.instruction_0_16b = esadi_ce_inst;
          flp_key_construction_tbl.instruction_1_16b = SOC_DPP_CONFIG(unit)->trill.mc_prune_mode ? fid_ce_inst : ARAD_PP_FLP_16B_INST_15_ZEROS;
          flp_key_construction_tbl.instruction_2_16b = ARAD_PP_CE_TRILL_DIST_TREE_NICK_16;

          flp_key_construction_tbl.key_a_inst_0_to_5_valid  = 0x7; 
          flp_key_construction_tbl.key_b_inst_0_to_5_valid  = 0x0; 
          flp_key_construction_tbl.key_c_inst_0_to_5_valid  = 0x0; 
      }
      else
#endif
      {
          flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0;
          flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0xb;
          flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0;
          
          flp_key_construction_tbl.instruction_0_16b = src_port_ce_inst;
          flp_key_construction_tbl.instruction_1_16b = ARAD_PP_CE_TRILL_SA_16MSB;
          flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_TRILL_SA_32LSB;
      }
      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

      res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_TRILL_MC && (prog == PROG_FLP_TRILL_MC_ONE_TAG))
      {
          flp_key_construction_tbl.key_a_inst_0_to_5_valid  = 0x0; 
          flp_key_construction_tbl.key_b_inst_0_to_5_valid  = 0xb; 
          flp_key_construction_tbl.key_c_inst_0_to_5_valid  = 0x0; 
		  flp_key_construction_tbl.instruction_0_16b = src_port_ce_inst;
          flp_key_construction_tbl.instruction_1_16b = ARAD_PP_CE_TRILL_SA_16MSB;
          flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_TRILL_SA_32LSB;
      }
      else
#endif
      {
          if (SOC_IS_DPP_TRILL_FGL(unit) && SOC_DPP_CONFIG(unit)->trill.mc_prune_mode) {
              
              if (is_two_tags) {
                flp_key_construction_tbl.instruction_0_16b = ARAD_PP_CE_TRILL_NATIVE_INNER_TAG_CE16;
              } else {
                flp_key_construction_tbl.instruction_0_16b = ARAD_PP_FLP_16B_INST_N_ZEROS(12);
              }
              flp_key_construction_tbl.instruction_1_16b = ARAD_PP_CE_TRILL_NATIVE_OUTER_TAG_CE16;
              flp_key_construction_tbl.instruction_2_16b = esadi_ce_inst;
              flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_TRILL_DIST_TREE_NICK_32;
              flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0xf ;
              flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
              flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
          } else {
            
            
              flp_key_construction_tbl.instruction_0_16b = SOC_DPP_CONFIG(unit)->trill.mc_prune_mode ? fid_ce_inst :ARAD_PP_FLP_16B_INST_15_ZEROS;
              flp_key_construction_tbl.instruction_1_16b = esadi_ce_inst;
              flp_key_construction_tbl.instruction_2_16b = ARAD_PP_CE_TRILL_DIST_TREE_NICK_16;
              flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x7 ;
              flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
              flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
          }
      }

      res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);
      res = arad_pp_flp_instruction_rsrc_set(unit, prog);
      SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_TRILL_mc", 0, 0);
}


uint32
   arad_pp_flp_key_const_pon_vmac_upstream(
     int unit,
     uint32 prog_id
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0xB ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_pon_vmac_upstream", 0, 0);
}

uint32
   arad_pp_flp_key_const_pon_vmac_downstream(
     int unit,
     uint32 prog_id
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  
  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_DA_FWD_HEADER_16_MSB;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_DA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0xB ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_pon_vmac_downstream", 0, 0);
}

uint32
   arad_pp_flp_lookups_ipv4uc_l3vpn(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV4UC_PUBLIC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  flp_lookups_tbl.lem_1st_lkp_valid     = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
  }
  else
#endif
  {
      flp_lookups_tbl.lpm_1st_lkp_valid	  = 1;
      flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
      flp_lookups_tbl.lpm_1st_lkp_and_value = 3;
      flp_lookups_tbl.lpm_1st_lkp_or_value = 0;
      flp_lookups_tbl.lpm_2nd_lkp_valid	  = 1;
      flp_lookups_tbl.lpm_2nd_lkp_key_select = 1;
      flp_lookups_tbl.lpm_2nd_lkp_and_value = 3;
      flp_lookups_tbl.lpm_2nd_lkp_or_value = 0;
  }

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      
  }
#endif

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV4UC_PUBLIC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv4uc_with_l3vpn", 0, 0);
}

uint32
   arad_pp_flp_lookups_ipv6uc_with_rpf_2pass(
     int unit,
     int32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 242, exit);
#if defined(INCLUDE_KBP)
  flp_lookups_tbl.elk_lkp_valid = 0x1;
  flp_lookups_tbl.elk_wait_for_reply = 0x1;
  flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF_2PASS;

  flp_lookups_tbl.tcam_lkp_db_profile    = ARAD_TCAM_ACCESS_PROFILE_INVALID;
  flp_lookups_tbl.tcam_lkp_key_select    = ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL; 
  flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = 0x3F;
  flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = 0x3F;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

  
  flp_lookups_tbl.elk_key_a_valid_bytes = 10;
  
  flp_lookups_tbl.elk_key_b_valid_bytes = 8;
  flp_lookups_tbl.elk_key_c_valid_bytes = 0;

  res = first_lem_lkup_sa_fid_search_set(unit,&flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);
#endif


  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 243, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv6uc_with_rpf_2pass", 0, 0);
}

uint32
   arad_pp_flp_lookups_ipv4uc_rpf(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV4UC_RPF, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
  flp_lookups_tbl.lpm_1st_lkp_valid     = 1;
  flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
  flp_lookups_tbl.lpm_1st_lkp_and_value = 3; 
  flp_lookups_tbl.lpm_1st_lkp_or_value = 0;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      flp_lookups_tbl.elk_lkp_valid = 0x1;
      flp_lookups_tbl.elk_wait_for_reply = 0x1;
      flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_RPF;
      flp_lookups_tbl.elk_key_a_valid_bytes = 0x6;
      flp_lookups_tbl.elk_key_b_valid_bytes = 0x4;
      flp_lookups_tbl.elk_key_c_valid_bytes = 0x0;
      if(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
          flp_lookups_tbl.elk_key_b_valid_bytes = 0x6;
          flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP_WITH_RPF;

		  res = first_lem_lkup_sa_fid_search_set(unit,&flp_lookups_tbl);
		  SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);
      }
  }
  else
#endif
  {
      flp_lookups_tbl.lpm_2nd_lkp_valid     = 1;
      flp_lookups_tbl.lpm_2nd_lkp_key_select = 1;
      flp_lookups_tbl.lpm_2nd_lkp_and_value = 3; 
      flp_lookups_tbl.lpm_2nd_lkp_or_value = 0;
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV4UC_RPF, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv4uc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_lookups_ipv4uc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV4UC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  flp_lookups_tbl.lem_1st_lkp_valid     = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
  }
  else
#endif
  {
      flp_lookups_tbl.lpm_1st_lkp_valid     = 1;
      flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
      flp_lookups_tbl.lpm_1st_lkp_and_value = 3;
      flp_lookups_tbl.lpm_1st_lkp_or_value = 0;
      flp_lookups_tbl.lpm_2nd_lkp_valid     = 1;
      flp_lookups_tbl.lpm_2nd_lkp_key_select = 1;
      flp_lookups_tbl.lpm_2nd_lkp_and_value = 3;
      flp_lookups_tbl.lpm_2nd_lkp_or_value = 0;
  }

  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      flp_lookups_tbl.elk_lkp_valid = 0x1;
      flp_lookups_tbl.elk_wait_for_reply = 0x1;
      flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC;
      flp_lookups_tbl.elk_key_a_valid_bytes = 0x6;
      flp_lookups_tbl.elk_key_b_valid_bytes = 0x4;
      flp_lookups_tbl.elk_key_c_valid_bytes = 0x0;
	  if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
		  flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP;
          flp_lookups_tbl.elk_key_b_valid_bytes = 0x6;
		  res = first_lem_lkup_sa_fid_search_set(unit, &flp_lookups_tbl);
		  SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);
	  }

      if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1) {
          flp_lookups_tbl.elk_key_a_valid_bytes = 0x0;
          flp_lookups_tbl.elk_key_b_valid_bytes = 0x0;
          flp_lookups_tbl.elk_key_c_valid_bytes = 0xa;
          if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
              flp_lookups_tbl.elk_key_c_valid_bytes = 0xc;
		  }
          }
  }
#endif

  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) {
      flp_lookups_tbl.lem_1st_lkp_valid      = 1;
      flp_lookups_tbl.lem_1st_lkp_key_select = 0;
      flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
      flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
      flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_MAC_IN_MAC_TUNNEL_KEY_OR_MASK;
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV4UC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv4uc", 0, 0);
}


uint32
   arad_pp_flp_lookups_bfd_single_hop(
     int unit,
	 int prog
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  flp_lookups_tbl.lem_1st_lkp_valid     = 0;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = 0;
  flp_lookups_tbl.lpm_1st_lkp_valid     = 0;
  flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
  flp_lookups_tbl.lpm_1st_lkp_and_value = 0;
  flp_lookups_tbl.lpm_1st_lkp_or_value = 0;
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lpm_2nd_lkp_valid     = 0;
  flp_lookups_tbl.lpm_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value =0x0;
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_SINGLE_HOP,&flp_lookups_tbl.lem_2nd_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  flp_lookups_tbl.lpm_2nd_lkp_and_value = 0;
  flp_lookups_tbl.lpm_2nd_lkp_or_value = 0;
  flp_lookups_tbl.learn_key_select      = 1;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_bfd_single_hop", 0, 0);
}


uint32
   arad_pp_flp_lookups_tcam_profile_set(
     int unit,
     uint32 tcam_access_profile_ndx,
     uint32 tcam_access_profile_id,
     uint32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  if (tcam_access_profile_ndx == 0) {
      flp_lookups_tbl.tcam_lkp_db_profile    = tcam_access_profile_id;
      flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = 0x3F;
  } else {
      flp_lookups_tbl.tcam_lkp_db_profile_1    = tcam_access_profile_id;
      flp_lookups_tbl.tcam_lkp_key_select_1    = 1;
      flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = 0x3F;
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_tcam_profile_set", 0, 0);
}

uint32
   arad_pp_flp_process_ipv6uc_with_rpf_2pass(
     int unit,
     int32 prog_id
   )
{
  uint32
      res;
  uint32
      fld_val,
      mem_val=0;
  uint32
      tmp;
  soc_reg_above_64_val_t
      reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
      flp_process_tbl;
  SOC_SAND_OUT ARAD_SOC_REG_FIELD   strength_fld_fwd,
                                    strength_fld_snp;
  SOC_SAND_OUT SOC_PPC_TRAP_CODE_INTERNAL   trap_code_internal;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_trap_mgmt_trap_code_to_internal(unit, SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS, &trap_code_internal, &strength_fld_fwd, &strength_fld_snp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

    

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 241, exit);

    flp_process_tbl.include_tcam_in_result_a      = 0x0;
    flp_process_tbl.result_a_format               = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
    flp_process_tbl.result_b_format               = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
    flp_process_tbl.procedure_ethernet_default    = 0x0; 
    flp_process_tbl.enable_hair_pin_filter        = 0x0; 
    flp_process_tbl.enable_rpf_check              = 0x1;
    flp_process_tbl.sa_lkp_process_enable         = 0x0;
    flp_process_tbl.apply_fwd_result_a            = 0x0; 

    
    flp_process_tbl.not_found_trap_strength       = 0x6;
    flp_process_tbl.not_found_trap_code           = trap_code_internal;
    
    flp_process_tbl.select_default_result_a     = 0x2;  
    flp_process_tbl.select_default_result_b     = 0x0;

    flp_process_tbl.elk_result_format = 1;
    flp_process_tbl.include_elk_fwd_in_result_a = 0;     
    flp_process_tbl.include_elk_ext_in_result_a = 0;
    flp_process_tbl.include_elk_fwd_in_result_b = 0;
    flp_process_tbl.include_elk_ext_in_result_b = 1;

    flp_process_tbl.fwd_processing_profile        = ARAD_PP_FLP_PROGRAM_FWD_PROCESS_PROFILE_REPLACE_FWD_CODE;

	res = process_tbl_learn_enable_set(unit,&flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);


    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 244, exit);

  
    tmp = 2;
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 245, exit);

    SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 246, exit);

    fld_val = 0x6; 
    soc_mem_field_set(unit, IHB_ELK_PAYLOAD_FORMATm, &mem_val, EXT_FOUND_OFFSETf, &fld_val);
    fld_val = 0x5; 
    soc_mem_field_set(unit, IHB_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_OFFSETf, &fld_val);
    fld_val = 0x2; 
    soc_mem_field_set(unit, IHB_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_FORMATf, &fld_val);
    res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, prog_id, &mem_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101, exit);


exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6uc_with_rpf_2pass", 0, 0);
}

uint32
   arad_pp_flp_process_ipv4uc_rpf(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV4UC_RPF, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;

  flp_process_tbl.result_a_format                = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.result_b_format                = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.procedure_ethernet_default =
      SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)){
      flp_process_tbl.enable_rpf_check            = 0;
  } else {
      flp_process_tbl.enable_rpf_check            = 1;
  }
  
  flp_process_tbl.select_default_result_a = 0;
  flp_process_tbl.select_default_result_b = 0;
  flp_process_tbl.sa_lkp_process_enable = 0;
  flp_process_tbl.not_found_trap_strength = 3;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
      if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
          res = process_tbl_learn_enable_set(unit, &flp_process_tbl);
          SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);
      }else{
          if (SOC_IS_JERICHO(unit)) {
              flp_process_tbl.include_lem_2nd_in_result_a    = 2;
              flp_process_tbl.include_lem_1st_in_result_b    = 2;
          }
      }
      
      flp_process_tbl.include_lpm_1st_in_result_a = 1;
      flp_process_tbl.include_lpm_1st_in_result_b = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default  = 1;
  }
  else
#endif
  {
      flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
      flp_process_tbl.include_lpm_1st_in_result_b    = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      if(JER_KAPS_ENABLE(unit)) {
          flp_process_tbl.lpm_2nd_lkp_enable_default = 1;
      }
#endif
  }


  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV4UC_RPF, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 1; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV4UC_RPF,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4uc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_lookups_ipv6uc(
     int unit,
     uint32 tcam_access_profile_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV6UC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  {
      flp_lookups_tbl.tcam_lkp_db_profile    = tcam_access_profile_id;
      flp_lookups_tbl.tcam_lkp_key_select    = 0;
      flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = 0x3F;
      flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = 0x3F;
      flp_lookups_tbl.learn_key_select      = 0;

#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV6_UC || ARAD_KBP_ENABLE_IPV6_EXTENDED){
          flp_lookups_tbl.elk_lkp_valid = 0x1;
          flp_lookups_tbl.elk_wait_for_reply = 0x1;
          flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC;

          if(ARAD_KBP_ENABLE_IPV6_EXTENDED) {
              flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_IPV6;
		  res = first_lem_lkup_sa_fid_search_set(unit, &flp_lookups_tbl);
		  SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);
		  }

          flp_lookups_tbl.elk_key_a_valid_bytes = 10;
          flp_lookups_tbl.elk_key_b_valid_bytes = 8;
          if(ARAD_KBP_ENABLE_IPV6_EXTENDED) {
              flp_lookups_tbl.elk_key_a_valid_bytes = 2; 
              flp_lookups_tbl.elk_key_b_valid_bytes = 0;
          }
          flp_lookups_tbl.elk_key_c_valid_bytes = 16;
      }
#endif
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV6UC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv6uc", 0, 0);
}

uint32
   arad_pp_flp_lookups_ipv6mc(
     int unit,
     uint32 tcam_access_profile_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV6MC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  {
      flp_lookups_tbl.tcam_lkp_db_profile    = tcam_access_profile_id;
      flp_lookups_tbl.tcam_lkp_key_select    = ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL; 
      flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = 0x3F;
      flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = 0x3F;
      flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

    #if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV6_MC){
          flp_lookups_tbl.elk_lkp_valid = 0x1;
          flp_lookups_tbl.elk_wait_for_reply = 0x1;
          flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF;

          flp_lookups_tbl.elk_key_a_valid_bytes = 8;  
          flp_lookups_tbl.elk_key_b_valid_bytes = 8;  
          flp_lookups_tbl.elk_key_c_valid_bytes = 20; 
      }
    #endif
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV6MC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv6mc", 0, 0);
}

uint32
   arad_pp_flp_lookups_oam(
     int unit,
     uint32 tcam_access_profile_id_0,
     uint32 tcam_access_profile_id_1,
     uint32 flp_key_program
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, flp_key_program, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = tcam_access_profile_id_0;
  flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = tcam_access_profile_id_1;
  if(flp_key_program == PROG_FLP_MPLS_CONTROL_WORD_TRAP){
      flp_lookups_tbl.enable_tcam_identification_oam = 0;
  }
  else{
      flp_lookups_tbl.enable_tcam_identification_oam = 1;
  }
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, flp_key_program, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_oam", 0, 0);
}

uint32
   arad_pp_flp_lookups_fcf_npv(
     int    unit,
     int32  progs[2]
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   
  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, progs[0], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1; 
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_FC_N_PORT_KEY_OR_MASK,&flp_lookups_tbl.lem_2nd_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  flp_lookups_tbl.learn_key_select      = 0;

  
  flp_lookups_tbl.elk_lkp_valid      = 0;
  flp_lookups_tbl.lpm_1st_lkp_valid  = 0;
  if (!SOC_IS_JERICHO(unit)) {
      
      flp_lookups_tbl.lpm_2nd_lkp_valid  = 1;
      flp_lookups_tbl.lpm_2nd_lkp_key_select = 2; 
  }
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[0], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[1], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_fcf_npv", 0, 0);
}


uint32
   arad_pp_flp_lookups_fcf(
     int    unit,
     int32  progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS],
     int    is_zoning_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, progs[0], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  
  flp_lookups_tbl.lem_1st_lkp_valid      = is_zoning_enabled;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0; 
  flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;

  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_FC_ZONING_KEY_OR_MASK,&flp_lookups_tbl.lem_1st_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  
  flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1; 
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_FC_KEY_OR_MASK,&flp_lookups_tbl.lem_2nd_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  flp_lookups_tbl.learn_key_select      = 0;

  
  flp_lookups_tbl.elk_lkp_valid      = 0;
  flp_lookups_tbl.lpm_1st_lkp_valid  = 0;
  flp_lookups_tbl.lpm_2nd_lkp_valid  = 0;
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[0], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[1], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  

  
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_FC_REMOTE_KEY_OR_MASK,&flp_lookups_tbl.lem_2nd_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  if (!SOC_IS_JERICHO(unit)) { 
      flp_lookups_tbl.lpm_2nd_lkp_valid  = 1;
      flp_lookups_tbl.lpm_2nd_lkp_key_select = 2; 
  }
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[2], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
  
  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[3], &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_fcf", 0, 0);
}



#if defined(INCLUDE_KBP)
uint32
   arad_pp_flp_lookups_lsr(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_LSR, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_LSR_KEY_OR_MASK;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  flp_lookups_tbl.elk_lkp_valid = 0x1;
  flp_lookups_tbl.elk_wait_for_reply = 0x1;

  flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_LSR;
  flp_lookups_tbl.elk_key_a_valid_bytes = 3;

  res = first_lem_lkup_sa_fid_search_set(unit, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);

  flp_lookups_tbl.elk_key_b_valid_bytes = 0;
  flp_lookups_tbl.elk_key_c_valid_bytes = 0;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_LSR, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_lsr", 0, 0);
}
#endif


uint32
   arad_pp_flp_lookups_pwe_gre(
     int unit,
     int prog
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_LSR_KEY_OR_MASK;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_pwe_gre", 0, 0);
}

uint32
   arad_pp_flp_lookups_TRILL_uc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_TRILL_UC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_TRILL_ADJ_KEY_OR_MASK,&flp_lookups_tbl.lem_1st_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_TRILL_KEY_OR_MASK;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_TRILL_UC){
      flp_lookups_tbl.elk_lkp_valid = 0x1;
      flp_lookups_tbl.elk_wait_for_reply = 0x1;
      flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC;

      flp_lookups_tbl.elk_key_a_valid_bytes = 2;
      flp_lookups_tbl.elk_key_b_valid_bytes = 0;
      flp_lookups_tbl.elk_key_c_valid_bytes = 0;
  }
#endif

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_TRILL_UC, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_TRILL", 0, 0);
}

uint32
    arad_pp_flp_lookups_mac_in_mac_after_termination_update(
     int unit,
     uint32 is_ingress_learn,
     uint32 prog_id)
{
  uint32 res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  
  flp_lookups_tbl.learn_key_select = (is_ingress_learn ? 0x3 : 0x0);

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_mac_in_mac_after_termination_update", 0, 0);
}

uint32
    arad_pp_flp_lookups_TRILL_mc_update(int unit,
     uint32 is_ingress_learn)
{
    uint32 res, prog ;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
  uint8 is_two_tags;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (is_two_tags = 0; is_two_tags <= 1; ++is_two_tags) {
      prog = is_two_tags ?PROG_FLP_TRILL_MC_TWO_TAGS :PROG_FLP_TRILL_MC_ONE_TAG;
      res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog, &flp_lookups_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
      flp_lookups_tbl.learn_key_select = (is_ingress_learn ? 0x1 : 0x0); 

      res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog, &flp_lookups_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_TRILL_mc", 0, 0);

}
uint32
   arad_pp_flp_lookups_TRILL_mc(
     int unit,
     uint32 is_ingress_learn,
     uint32 tcam_access_profile_id
   )
{
   uint32
    res,
    prog ;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;
  uint8 is_two_tags;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  for (is_two_tags = 0; is_two_tags <= 1; ++is_two_tags) {
      prog = is_two_tags ?
          PROG_FLP_TRILL_MC_TWO_TAGS :
          PROG_FLP_TRILL_MC_ONE_TAG;
      res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog, &flp_lookups_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

      flp_lookups_tbl.lem_1st_lkp_valid     = 1;
      flp_lookups_tbl.lem_1st_lkp_key_select = 1;
      flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
      res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_TRILL_ADJ_KEY_OR_MASK,&flp_lookups_tbl.lem_1st_lkp_or_value);
      SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

      flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
      flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
      flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
      flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_TRILL_KEY_OR_MASK_MC;
      flp_lookups_tbl.learn_key_select = (is_ingress_learn ? 0x1 : 0x0); 

	  if (is_two_tags) {
          flp_lookups_tbl.tcam_lkp_key_select = ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL;
          flp_lookups_tbl.tcam_lkp_db_profile = tcam_access_profile_id;
          flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = 0x3F;
          flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = 0x3F;
	  }

#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_TRILL_MC &&
         (prog == PROG_FLP_TRILL_MC_ONE_TAG)){
          flp_lookups_tbl.elk_lkp_valid = 0x1;
          flp_lookups_tbl.elk_wait_for_reply = 0x1;
          flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC;

          flp_lookups_tbl.elk_key_a_valid_bytes = 4;
          flp_lookups_tbl.elk_key_b_valid_bytes = 0;
          flp_lookups_tbl.elk_key_c_valid_bytes = 0;
      }
#endif
      res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog, &flp_lookups_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_TRILL_mc", 0, 0);
}


uint32
   arad_pp_flp_lookups_pon_vmac_upstream(
     int unit,
     int32  prog_id,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  flp_lookups_tbl.lem_1st_lkp_valid      = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0; 
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);

  
  flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 0; 
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_OMAC_2_VMAC_KEY_OR_MASK,&flp_lookups_tbl.lem_2nd_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  flp_lookups_tbl.learn_key_select       = 0;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_pon_vmac_upstream", 0, 0);
}

uint32
   arad_pp_flp_lookups_pon_vmac_downstream(
     int unit,
     uint32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  flp_lookups_tbl.lem_1st_lkp_valid      = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0; 
  flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0;
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_VMAC_2_OMAC_KEY_OR_MASK,&flp_lookups_tbl.lem_1st_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL; 
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0;
  res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_VMAC_KEY_OR_MASK,&flp_lookups_tbl.lem_2nd_lkp_or_value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  flp_lookups_tbl.learn_key_select       = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_pon_vmac_downstream", 0, 0);
}


uint32
   arad_pp_flp_process_ipv4uc_l3vpn_rpf(
     int unit,
     int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  flp_process_tbl.include_lpm_1st_in_result_b    = 1;
  flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
  flp_process_tbl.result_a_format            = 2;
  flp_process_tbl.result_b_format            = 2;
  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.enable_rpf_check            = 1;
  
  flp_process_tbl.select_default_result_a = 0;
  flp_process_tbl.select_default_result_b = 0;
  flp_process_tbl.sa_lkp_process_enable = 0;
  flp_process_tbl.not_found_trap_strength = 3;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_RPF_PUBLIC){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
      
      flp_process_tbl.include_lpm_1st_in_result_a = 1;
      flp_process_tbl.include_lpm_1st_in_result_b = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default  = 1;
      flp_process_tbl.lpm_public_1st_lkp_enable_default = 1;
  } else {
	  flp_process_tbl.lpm_2nd_lkp_enable_default = 1;
	  flp_process_tbl.lpm_public_2nd_lkp_enable_default = 1;
  }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 1; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4uc_l3vpn_rpf", 0, 0);
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
uint32
   arad_pp_flp_process_ipv6uc_l3vpn(
     int unit,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  uint8 sw_prog_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  ARAD_PP_FLP_SW_DB_FLP_PROGRAM_ID_GET(unit, prog_id, sw_prog_id);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lpm_2nd_in_result_a     = 1;
  flp_process_tbl.result_a_format            = 2;
  flp_process_tbl.procedure_ethernet_default  = 0;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.enable_rpf_check            = 0;
  flp_process_tbl.sa_lkp_process_enable       = 0;
  
  flp_process_tbl.not_found_trap_strength       = 0;
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6;
  
  flp_process_tbl.select_default_result_a = 0;

  if ((SOC_DPP_CONFIG(unit)->pp.custom_ip_route) && (sw_prog_id == PROG_FLP_IP6UC_CUSTOM_ROUTE)) {
      flp_process_tbl.include_lem_1st_in_result_b    = 0;
      flp_process_tbl.include_lpm_1st_in_result_b    = 0;

      flp_process_tbl.include_lem_2nd_in_result_a    = 0; 
      flp_process_tbl.include_lem_1st_in_result_a    = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  }

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_UC_PUBLIC){
        flp_process_tbl.elk_result_format = 1;
        flp_process_tbl.include_elk_fwd_in_result_a = 1;
        flp_process_tbl.include_elk_ext_in_result_a = 0;
        flp_process_tbl.include_elk_fwd_in_result_b = 0;
        flp_process_tbl.include_elk_ext_in_result_b = 1;
        
        flp_process_tbl.include_lpm_2nd_in_result_a = 1;
        flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;
        flp_process_tbl.lpm_public_2nd_lkp_enable_default = 1;
    } else if(JER_KAPS_ENABLE(unit)) {
        flp_process_tbl.lpm_2nd_lkp_enable_default = 1;
        flp_process_tbl.lpm_public_2nd_lkp_enable_default = 1;
    }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  tmp = 2;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6uc", 0, 0);
}
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
uint32
   arad_pp_flp_process_ipv6uc_l3vpn_rpf(
     int unit,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lpm_1st_in_result_b     = 1;
  flp_process_tbl.include_lpm_2nd_in_result_a     = 1;

  flp_process_tbl.result_a_format            = 2;
  flp_process_tbl.result_b_format            = 2;
  flp_process_tbl.procedure_ethernet_default  = 0;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.enable_rpf_check            = 1;
  flp_process_tbl.sa_lkp_process_enable       = 0;
  
  flp_process_tbl.not_found_trap_strength       = 0;
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6;
  
  flp_process_tbl.select_default_result_a = 0;
  flp_process_tbl.select_default_result_b = 0;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_RPF_PUBLIC){
        flp_process_tbl.elk_result_format = 1;
        flp_process_tbl.include_elk_fwd_in_result_a = 1;
        flp_process_tbl.include_elk_ext_in_result_a = 0;
        flp_process_tbl.include_elk_fwd_in_result_b = 0;
        flp_process_tbl.include_elk_ext_in_result_b = 1;
        
        flp_process_tbl.include_lpm_1st_in_result_a = 1;
        flp_process_tbl.include_lpm_1st_in_result_b = 1;
        flp_process_tbl.lpm_1st_lkp_enable_default  = 1;
        flp_process_tbl.lpm_public_1st_lkp_enable_default = 1;
    } else if(JER_KAPS_ENABLE(unit)) {
        flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
        flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
        flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
        flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
    }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  tmp = 2;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6uc", 0, 0);
}
#endif



uint32
   arad_pp_flp_process_ipv4uc_l3vpn(
     int unit,
     int custom_prgrm
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  int prog_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (custom_prgrm) {
      prog_id = custom_prgrm;
  } else {
      prog_id = PROG_FLP_IPV4UC_PUBLIC;
  }

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  if (SOC_IS_ARADPLUS(unit)) {
      flp_process_tbl.include_lem_2nd_in_result_a    = 4;
      flp_process_tbl.include_lem_1st_in_result_a    = 2; 
  } else {
      
      flp_process_tbl.include_lem_2nd_in_result_a    = 1;
      flp_process_tbl.include_lem_1st_in_result_b    = 1;
  }
  flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  if(SOC_IS_JERICHO(unit)){
      flp_process_tbl.include_lpm_1st_in_result_a    = 3;
  }
  flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
  flp_process_tbl.result_a_format            = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.result_b_format            = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.enable_rpf_check            = 0;
  
  flp_process_tbl.select_default_result_a = 0;
  flp_process_tbl.select_default_result_b = 0;
  flp_process_tbl.sa_lkp_process_enable = 0;
  flp_process_tbl.not_found_trap_strength = 3;


  if (custom_prgrm) {
      flp_process_tbl.include_lem_1st_in_result_b    = 0;
      flp_process_tbl.include_lpm_1st_in_result_b    = 0;

      flp_process_tbl.include_lem_2nd_in_result_a    = 0; 
      flp_process_tbl.include_lem_1st_in_result_a    = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  }

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC_PUBLIC){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
      
      flp_process_tbl.include_lpm_2nd_in_result_a = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default = 1;
  } else if(JER_KAPS_ENABLE(unit)) {
      flp_process_tbl.lpm_2nd_lkp_enable_default = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default = 1;
  }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 1; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_l3vpn", 0, 0);
}



uint32
   arad_pp_flp_process_ipv4uc(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV4UC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;

  if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
      flp_process_tbl.include_lem_2nd_in_result_a    = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
      flp_process_tbl.include_lem_1st_in_result_a    = 2;
      flp_process_tbl.include_lpm_1st_in_result_a    = 2;
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
  }

  flp_process_tbl.result_a_format            = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.result_b_format            = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.enable_rpf_check            = 0;
  
  flp_process_tbl.select_default_result_a = 0;
  flp_process_tbl.select_default_result_b = 0;
  flp_process_tbl.sa_lkp_process_enable = 0;
  flp_process_tbl.not_found_trap_strength = 3;


  if (SOC_IS_JERICHO(unit)) {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      if(JER_KAPS_ENABLE(unit)) {
          flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
          flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
          flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
          flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
      }
      else
#endif
      {
          flp_process_tbl.include_tcam_in_result_a    = 1;
          flp_process_tbl.include_tcam_1_in_result_b  = 1;
      }
  }


#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
	  if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
		  res = process_tbl_learn_enable_set(unit, &flp_process_tbl);
		  SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);
	  }
      if(SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1){
          flp_process_tbl.include_lem_2nd_in_result_a    = 7;
      }
      if (SOC_IS_JERICHO(unit)) {
          flp_process_tbl.include_lem_2nd_in_result_a    = 2;
          flp_process_tbl.include_lem_1st_in_result_b    = 2;
      }
      
      flp_process_tbl.include_lpm_2nd_in_result_a = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;
  }
  else
#endif
  {
      flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
      flp_process_tbl.include_lpm_1st_in_result_b    = 1;
  }

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV4UC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 1; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV4UC,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4uc", 0, 0);
}

uint32
arad_pp_flp_process_mpls_exp_null_ttl_reject(
     int unit,
     int prog_id
   )
{
    uint32
    res;
    uint32
    tmp;
    soc_reg_above_64_val_t
    reg_val;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    flp_process_tbl.not_found_trap_strength = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "explicit_null_ttl_trap_strength", 6);
    flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0;

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    tmp = 1; 
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV4UC,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_mpls_exp_null_ttl_reject", 0, 0);
}

uint32
   arad_pp_flp_process_ipv4_dc(
     int unit
   )
{
  uint32 res;
  uint32 tmp;
  soc_reg_above_64_val_t reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV4_DC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.result_a_format               = 2;
  flp_process_tbl.result_b_format               = 2;
  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.enable_hair_pin_filter        = 1;
  flp_process_tbl.enable_rpf_check              = 0;
  
  flp_process_tbl.select_default_result_a       = 0;
  flp_process_tbl.select_default_result_b       = 0;
  flp_process_tbl.sa_lkp_process_enable         = 0;
  flp_process_tbl.not_found_trap_strength       = 3;

  flp_process_tbl.elk_result_format = 1;
  flp_process_tbl.include_elk_fwd_in_result_a = 1;
  flp_process_tbl.include_elk_ext_in_result_a = 1;
  flp_process_tbl.include_elk_fwd_in_result_b = 0;
  flp_process_tbl.include_elk_ext_in_result_b = 0;
  
  flp_process_tbl.include_lpm_2nd_in_result_a = 1;
  flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;

  
  flp_process_tbl.include_lem_2nd_in_result_a    = 3;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV4_DC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 1; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV4_DC,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

  exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4_dc", 0, 0);
}

uint32
   arad_pp_flp_process_bfd_single_hop(
     int unit,
     int is_ipv6,
	 int prog
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  SOC_PPC_TRAP_CODE
    trap_code;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_PPC_TRAP_CODE_INTERNAL   trap_code_internal;
  ARAD_SOC_REG_FIELD   strength_fld_fwd,
                       strength_fld_snp;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "common_bfd_discr_not_found_trap", 0)) {
        
        trap_code =SOC_PPC_TRAP_CODE_BFD_IPV4_IPV6_YOUR_DISCR_NOT_FOUND;
    } else {
        trap_code = (is_ipv6 ? SOC_PPC_TRAP_CODE_BFD_IPV6_YOUR_DISCR_NOT_FOUND : SOC_PPC_TRAP_CODE_BFD_IPV4_YOUR_DISCR_NOT_FOUND);
    }

    
    res = arad_pp_trap_mgmt_trap_code_to_internal(unit, trap_code, &trap_code_internal, &strength_fld_fwd, &strength_fld_snp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    flp_process_tbl.include_lem_2nd_in_result_a    = 1;
    flp_process_tbl.include_lem_1st_in_result_a    = 0;
    flp_process_tbl.include_lpm_2nd_in_result_a    = 0;
    flp_process_tbl.include_lem_1st_in_result_b    = 0;
    flp_process_tbl.include_lem_2nd_in_result_b    = 1;
    flp_process_tbl.result_a_format            = 2;
    flp_process_tbl.result_b_format            = 2;
    flp_process_tbl.procedure_ethernet_default
        = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
    flp_process_tbl.enable_hair_pin_filter       = 1;
    flp_process_tbl.enable_rpf_check            = 0;
    flp_process_tbl.not_found_trap_code         = trap_code_internal;
    flp_process_tbl.not_found_trap_strength     = SOC_DPP_OAM_TRAP_STRENGTH(unit);

    flp_process_tbl.select_default_result_a = 3;
    flp_process_tbl.select_default_result_b = 3;

    if (SOC_IS_JERICHO(unit)) {
      flp_process_tbl.include_tcam_in_result_a    = 1;
      flp_process_tbl.include_tcam_1_in_result_b  = 1;
    }

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    tmp = (is_ipv6 ? 2 : 1); 
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*prog,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4uc", 0, 0);
}


uint32
   arad_pp_flp_process_TRILL_uc(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_TRILL_UC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lpm_2nd_in_result_a    = 0;
  flp_process_tbl.apply_fwd_result_a           = 1; 
  flp_process_tbl.not_found_trap_code          = SOC_PPC_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.procedure_ethernet_default  = 2; 
  flp_process_tbl.sa_lkp_process_enable       = 0;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.enable_rpf_check            = 0;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_TRILL_UC){
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
  }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_TRILL_UC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 5; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_TRILL_UC,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_TRILL", 0, 0);
}

uint32
   arad_pp_flp_process_TRILL_mc(
     int unit,
     uint8 ingress_learn_enable
   )
{
  uint32
    res;
  uint32
    tmp,
    prog ;
  uint8 is_two_tags;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  for (is_two_tags = 0; is_two_tags <= 1; ++is_two_tags) {
      prog = is_two_tags ?
          PROG_FLP_TRILL_MC_TWO_TAGS :
          PROG_FLP_TRILL_MC_ONE_TAG;

      res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

      flp_process_tbl.include_lem_1st_in_result_b    = 1;
      flp_process_tbl.include_lem_2nd_in_result_a    = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a    = 0;
      flp_process_tbl.include_tcam_in_result_a       = 1;
      flp_process_tbl.apply_fwd_result_a           = 1; 
      flp_process_tbl.not_found_trap_code          = SOC_PPC_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC;
      flp_process_tbl.result_a_format            = 0;
      flp_process_tbl.result_b_format            = 0;
      flp_process_tbl.procedure_ethernet_default  = 2; 
      flp_process_tbl.enable_hair_pin_filter       = 1;
      flp_process_tbl.enable_rpf_check            = 0;
      
      flp_process_tbl.learn_enable                 = (ingress_learn_enable) ? 0 : 1;

#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_TRILL_MC &&
         (prog == PROG_FLP_TRILL_MC_ONE_TAG)){
          flp_process_tbl.include_elk_fwd_in_result_a = 1;
      }
#endif

      res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

      tmp = 5; 
      SOC_REG_ABOVE_64_CLEAR(reg_val);
      res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      SHR_BITCOPY_RANGE(reg_val,3*prog,&tmp,0,3);
      res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_TRILL_mc", 0, 0);
}

#if defined(INCLUDE_KBP)
uint32
   arad_pp_flp_process_ipv6uc_with_rpf(
     int unit,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lpm_1st_in_result_b     = 1;
  flp_process_tbl.include_lpm_2nd_in_result_a     = 1;

  flp_process_tbl.result_a_format            = 2;
  flp_process_tbl.result_b_format            = 2;
  flp_process_tbl.procedure_ethernet_default  = 0;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV6_ENABLE(unit)){
      flp_process_tbl.enable_rpf_check            = 0;
  } else {
      flp_process_tbl.enable_rpf_check            = 1;
  }
  flp_process_tbl.sa_lkp_process_enable       = 0;
  
  flp_process_tbl.not_found_trap_strength       = 0;
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6;
  
  flp_process_tbl.select_default_result_a = 0;
  flp_process_tbl.select_default_result_b = 0;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_UC || ARAD_KBP_ENABLE_IPV6_EXTENDED){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
      
      flp_process_tbl.include_lpm_1st_in_result_a = 1;
      flp_process_tbl.include_lpm_1st_in_result_b = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default  = 1;
  }
#if defined(BCM_88675_A0)
  else if(JER_KAPS_ENABLE(unit)) {
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
  }
#endif
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  tmp = 2;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6uc", 0, 0);
}
#endif

uint32
   arad_pp_flp_process_ipv6uc(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV6UC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  if (SOC_IS_JERICHO(unit) && !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_ipv6_uc_use_tcam", 0)) &&
                              !((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long))) {
      flp_process_tbl.include_lpm_2nd_in_result_a         = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default          = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default   = 1;
  } else if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) {
      
      flp_process_tbl.include_lem_2nd_in_result_a    = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
      flp_process_tbl.include_lem_1st_in_result_a    = 2;
      flp_process_tbl.include_lpm_1st_in_result_a    = 3;
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
  } else {
      flp_process_tbl.include_tcam_in_result_a      = 1;
  }
  flp_process_tbl.result_a_format             = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.result_b_format             = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0 : 2;
  flp_process_tbl.procedure_ethernet_default  = 0;
  flp_process_tbl.enable_hair_pin_filter      = 1;
  flp_process_tbl.enable_rpf_check            = 0;
  flp_process_tbl.sa_lkp_process_enable       = 0;
  
  flp_process_tbl.not_found_trap_strength     = 0;
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6;
  
  flp_process_tbl.select_default_result_a = 0;
  flp_process_tbl.select_default_result_b = 0;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_UC || ARAD_KBP_ENABLE_IPV6_EXTENDED){
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      
      flp_process_tbl.include_lpm_2nd_in_result_a = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;
  }
  if (ARAD_KBP_ENABLE_IPV6_EXTENDED) {
	  res = process_tbl_learn_enable_set(unit, &flp_process_tbl);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);
  }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV6UC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  tmp = 2;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV6UC,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6uc", 0, 0);
}

uint32
   arad_pp_flp_process_ipv6mc(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV6MC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);


  if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam == ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE)) {
      flp_process_tbl.include_lpm_1st_in_result_b        = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default         = 1;
      flp_process_tbl.lpm_public_1st_lkp_enable_default  = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a        = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default         = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default  = 1;
  } else {
      flp_process_tbl.include_tcam_in_result_a           = 1;
  }
  flp_process_tbl.result_a_format               = 0;
  flp_process_tbl.result_b_format               = 0;
  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.enable_hair_pin_filter        = 1;
  flp_process_tbl.enable_rpf_check              = (SOC_IS_JERICHO(unit)) ? 1 : 0;
  flp_process_tbl.compatible_mc_bridge_fallback = 1;
  flp_process_tbl.sa_lkp_process_enable         = 0;
  
  flp_process_tbl.not_found_trap_strength       = 0;
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6;
  
  flp_process_tbl.select_default_result_a = 1;
  flp_process_tbl.select_default_result_b = 1;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV6_MC){
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
      
      flp_process_tbl.include_lpm_2nd_in_result_a = 1;
      flp_process_tbl.include_lpm_1st_in_result_b = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default  = 1;
  }
#endif

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV6MC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  tmp = 2;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV6MC,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6mc", 0, 0);
}

uint32
   arad_pp_flp_process_ipv6mc_ssm_compress_sip(
      int unit,
      int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.include_lpm_2nd_in_result_a        = 1;
  flp_process_tbl.include_lpm_2nd_in_result_b        = 1;
  flp_process_tbl.lpm_public_2nd_lkp_enable_default  = 1;
  flp_process_tbl.result_a_format                    = 0;
  flp_process_tbl.result_b_format                    = 0;
  flp_process_tbl.enable_rpf_check                   = 0;
  flp_process_tbl.sa_lkp_process_enable              = 0;
  
  flp_process_tbl.not_found_trap_strength            = 0;
  flp_process_tbl.not_found_trap_code                = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6;
  
  flp_process_tbl.select_default_result_a            = 1;
  flp_process_tbl.select_default_result_b            = 1;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  tmp = 2;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val, 3*prog_id, &tmp, 0, 3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6mc_ssm_compress_sip", 0, 0);
}

uint32
   arad_pp_flp_process_ipv6mc_ssm(
      int unit,
      int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);


  flp_process_tbl.include_lpm_1st_in_result_b        = 1;
  flp_process_tbl.lpm_1st_lkp_enable_default         = 1;
  flp_process_tbl.lpm_public_1st_lkp_enable_default  = 1;
  flp_process_tbl.include_lpm_2nd_in_result_a        = 1;
  flp_process_tbl.lpm_2nd_lkp_enable_default         = 1;
  flp_process_tbl.lpm_public_2nd_lkp_enable_default  = 1;
  flp_process_tbl.result_a_format                    = 0;
  flp_process_tbl.result_b_format                    = 0;
  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.enable_hair_pin_filter             = 1;
  flp_process_tbl.enable_rpf_check                   = 0;
  flp_process_tbl.compatible_mc_bridge_fallback      = 1;
  flp_process_tbl.sa_lkp_process_enable              = 0;
  
  flp_process_tbl.not_found_trap_strength            = 0;
  flp_process_tbl.not_found_trap_code                = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6;
  
  flp_process_tbl.select_default_result_a            = 1;
  flp_process_tbl.select_default_result_b            = 1;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  
  tmp = 2;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val, 3*prog_id, &tmp, 0, 3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv6mc_ssm", 0, 0);
}

uint32
   arad_pp_flp_process_lsr(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_LSR, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_MPLS || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
  }
  else
#endif
  {
      flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
      flp_process_tbl.include_lpm_1st_in_result_b    = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;

      flp_process_tbl.include_lem_2nd_in_result_a    = 1;
      flp_process_tbl.include_lem_1st_in_result_b    = 1;
  }

  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.procedure_ethernet_default
      = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
  flp_process_tbl.enable_hair_pin_filter         = 0;
  flp_process_tbl.enable_rpf_check               = 0;
  flp_process_tbl.enable_lsr_p2p_service         = 0;
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL;


  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_LSR, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 3; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_LSR,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_lsr", 0, 0);
}

uint32
   arad_pp_flp_process_pwe_gre(
     int unit,
     int prog
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
  flp_process_tbl.include_lpm_1st_in_result_b    = 1;
  flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.procedure_ethernet_default     = 3;
  flp_process_tbl.enable_hair_pin_filter         = 0;
  flp_process_tbl.enable_rpf_check               = 0;
  flp_process_tbl.enable_lsr_p2p_service         = 0;
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  
  tmp = 5; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pwe_gre", 0, 0);
}

uint32
   arad_pp_flp_process_fcoe_fcf_npv(
     int unit,
     int32  progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS]
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, progs[0], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_1st_in_result_a    = 0; 
  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.include_lem_1st_in_result_b    = 0;
  flp_process_tbl.include_lpm_2nd_in_result_b    = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.procedure_ethernet_default     = 0;
  flp_process_tbl.enable_hair_pin_filter         = 0;
  flp_process_tbl.learn_enable                   = 0;

 
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_FCF;
  flp_process_tbl.not_found_trap_strength = 7;
  flp_process_tbl.not_found_snoop_strength = 0;

  
  flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, progs[0], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, progs[1], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);


  tmp = 4; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*progs[0],&tmp,0,3);
  SHR_BITCOPY_RANGE(reg_val,3*progs[1],&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_fcoe_fcf_npv", 0, 0);

}


uint32
   arad_pp_flp_process_fcoe_fcf(
     int unit,
     int32  progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS]
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, progs[0], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_1st_in_result_a    = 0; 
  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.result_a_format                = 0;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.include_lpm_2nd_in_result_b    = 0;
  flp_process_tbl.result_b_format                = 0;
  flp_process_tbl.procedure_ethernet_default     = 0;
  flp_process_tbl.enable_hair_pin_filter         = 0;
  flp_process_tbl.learn_enable                   = 0;

  
  flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_FCF;
  flp_process_tbl.not_found_trap_strength = 7;
  flp_process_tbl.not_found_snoop_strength = 0;

  
  flp_process_tbl.select_default_result_b = 2;
  flp_process_tbl.apply_fwd_result_a = 1;


  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, progs[0], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, progs[1], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  
  flp_process_tbl.include_lpm_2nd_in_result_a    = 1;
  flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, progs[2], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, progs[3], &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);


  tmp = 4; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit);
  SHR_BITCOPY_RANGE(reg_val,3*progs[0],&tmp,0,3);
  SHR_BITCOPY_RANGE(reg_val,3*progs[1],&tmp,0,3);
  SHR_BITCOPY_RANGE(reg_val,3*progs[2],&tmp,0,3);
  SHR_BITCOPY_RANGE(reg_val,3*progs[3],&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 47, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_fcoe_fcf", 0, 0);
}

uint32
   arad_pp_flp_process_oam_outlif_l2g(
    int unit,
    int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_1st_in_result_a    = 1;
  flp_process_tbl.result_a_format                = 2;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 3; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_lsr", 0, 0);
}

uint32
   arad_pp_flp_process_two_pass_mc_encap_to_dest(
    int unit,
    int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  flp_process_tbl.include_lem_1st_in_result_a    = 1;
  flp_process_tbl.result_a_format                = 2;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit);

  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);

  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 43, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_two_pass_mc_encap_to_dest", 0, 0);
}

uint32
    arad_pp_flp_prog_sel_cam_ipv4mc_bridge_v4mc(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_ETHERNET_ING_LEARN,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_ipv4mc_bridge_v4mc_cam_sel_id.set(unit,cam_sel_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY,
                  &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }

  prog_selection_cam_tbl.port_profile                   = 0;
  prog_selection_cam_tbl.ptc_profile                    = 0;
  prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable           = 0x0;
  prog_selection_cam_tbl.custom_rif_profile_bit        = 0x0;
  prog_selection_cam_tbl.packet_is_compatible_mc        = 1;

  
  prog_selection_cam_tbl.forwarding_header_qualifier      = ARAD_PP_FLP_QLFR_ETH_NEXT_PROTOCOL_IPV4;
  prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                            ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID);

  prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask               = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1;
  
  prog_selection_cam_tbl.custom_rif_profile_bit_mask   = (SOC_DPP_CONFIG(unit)->pp.ipmc_l3mcastl2_mode == 1) ?0x0 : 0x1;
  prog_selection_cam_tbl.packet_is_compatible_mc_mask   = 0x0; 
  prog_selection_cam_tbl.program                        = PROG_FLP_ETHERNET_ING_LEARN;
  prog_selection_cam_tbl.valid                          = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4mc_bridge", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_ipv4mc_bridge(
     int unit,
     int mode,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  if (mode == 1 && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) { 
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_IPV4MC_BRIDGE_USER_MODE, FALSE, TRUE, &cam_sel_id, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } else if (mode == 2 && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_IPV4MC_BRIDGE_USER_MODE_UNTAGGED, FALSE, TRUE, &cam_sel_id, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } else {
      res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_IPV4MC_BRIDGE, TRUE, TRUE, &cam_sel_id, NULL);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY,
                  &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }

  prog_selection_cam_tbl.port_profile                   = 0;
  prog_selection_cam_tbl.ptc_profile                    = 0;
  prog_selection_cam_tbl.forwarding_code                = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable           = 0x0;
  prog_selection_cam_tbl.custom_rif_profile_bit        = 0x1;
  prog_selection_cam_tbl.packet_is_compatible_mc        = 1;

  
  prog_selection_cam_tbl.forwarding_header_qualifier      = ARAD_PP_FLP_QLFR_ETH_NEXT_PROTOCOL_IPV4;
  prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_TPID |
                                                            ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_FLP_QLFR_ETH_MASK_INNER_TPID);

  prog_selection_cam_tbl.port_profile_mask              = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask               = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask           = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask      = 0x1;
  prog_selection_cam_tbl.custom_rif_profile_bit_mask   = 0x0;
  prog_selection_cam_tbl.packet_is_compatible_mc_mask   = 0x0; 

  if (mode == 0 && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
      prog_selection_cam_tbl.lsb_rif_profile_bit = 0x0;
      prog_selection_cam_tbl.lsb_rif_profile_bit_mask = 0x0;
  }

  if (mode == 1 && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) { 
      prog_selection_cam_tbl.lsb_rif_profile_bit = 0x1; 
      prog_selection_cam_tbl.lsb_rif_profile_bit_mask = 0x0;
  }

  if (mode == 2 && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) { 
      prog_selection_cam_tbl.lsb_rif_profile_bit = 0x1; 
      prog_selection_cam_tbl.lsb_rif_profile_bit_mask = 0x0;
      
      prog_selection_cam_tbl.forwarding_header_qualifier_mask = (ARAD_PP_FLP_QLFR_ETH_MASK_ENCAP | ARAD_PP_FLP_QLFR_ETH_MASK_OUTER_PCP);
  }

  if (mode != 0 && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
      prog_selection_cam_tbl.program                        = *prog_id;
  } else {
      prog_selection_cam_tbl.program                        = PROG_FLP_IPV4MC_BRIDGE;
  }
  prog_selection_cam_tbl.valid                          = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4mc_bridge_v4mc", 0, 0);
}



uint32
   arad_pp_flp_key_const_ipv4mc_bridge(
     int unit
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b = fid_ce_inst;
  flp_key_construction_tbl.instruction_1_16b = 0;
  flp_key_construction_tbl.instruction_2_16b = 0;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.instruction_5_32b = arad_pp_ce_instruction_composer(28,ARAD_PP_CE_HEADER_AFTER_FWD,128,ARAD_PP_CE_IS_CE32);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0xB ; 
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x21 ; 

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.instruction_0_16b = 0;
  flp_key_construction_tbl.instruction_1_16b = 0;
  flp_key_construction_tbl.instruction_2_16b = 0;
  flp_key_construction_tbl.instruction_3_32b = 0;
  flp_key_construction_tbl.instruction_4_32b = 0;
  flp_key_construction_tbl.instruction_5_32b = 0;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv4mc_bridge", 0, 0);
}


uint32
   arad_pp_flp_lookups_ipv4mc_bridge_update(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  flp_lookups_tbl.lem_1st_lkp_valid     = (!sa_auth_enabled && !slb_enabled)? 1 : 0 ;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv4mc_bridge", 0, 0);
}

uint32
   arad_pp_flp_lookups_ipv4mc_bridge(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  flp_lookups_tbl.lem_1st_lkp_valid     = (!sa_auth_enabled && !slb_enabled)? 1 : 0 ;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);


  
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1; 
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_COMP_KEY_OR_MASK;
  flp_lookups_tbl.learn_key_select      = 0;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv4mc_bridge", 0, 0);
}


uint32
   arad_pp_flp_process_ipv4mc_bridge(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;

  if((SOC_IS_JERICHO(unit)) && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_ELK_LPM)) {
      flp_process_tbl.include_elk_fwd_in_result_a = 2; 
      flp_process_tbl.elk_ext_lkp_enable_default     = 1;
      flp_process_tbl.elk_fwd_lkp_enable_default     = 1;
  } else if ((SOC_IS_JERICHO(unit)) && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_KAPS_LPM)) {
      flp_process_tbl.include_lpm_2nd_in_result_a = 2;
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
  } else if ((SOC_IS_ARADPLUS(unit)) && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_TCAM_LPM)) {
      flp_process_tbl.include_tcam_in_result_a    = 2;
  }

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV4MC_BRIDGE, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV4MC_BRIDGE,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4mc_bridge", 0, 0);
}


uint32
   arad_pp_flp_process_ipv4mc_bridge_ivl(
     int unit,
     int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = 0;
  flp_process_tbl.procedure_ethernet_default  = 3; 
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = 0x0;
  flp_process_tbl.not_found_trap_strength      = 0;
  flp_process_tbl.unknown_address              = 3;


  flp_process_tbl.include_lpm_2nd_in_result_a = 2;
  flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;


  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);

  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4mc_bridge", 0, 0);
}






uint32
   arad_pp_flp_prog_sel_cam_ipv4compmc_with_rpf(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV4COMPMC_WITH_RPF,TRUE,TRUE,&cam_sel_id, NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if(SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) {
      res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_ETH,
                     &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  } else {
      res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY_PP,
                  &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
      SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
  }

  prog_selection_cam_tbl.port_profile                 = 0;
  prog_selection_cam_tbl.ptc_profile                  = 0;
  prog_selection_cam_tbl.forwarding_code              = ARAD_PP_FWD_CODE_IPV4_MC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable         = 0x0;
  prog_selection_cam_tbl.packet_is_compatible_mc      = 1;
  prog_selection_cam_tbl.port_profile_mask            = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask             = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask         = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask    = 0x1;
  prog_selection_cam_tbl.packet_is_compatible_mc_mask = 0x1; 

  prog_selection_cam_tbl.program                   = PROG_FLP_IPV4COMPMC_WITH_RPF;
  prog_selection_cam_tbl.valid                     = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4compmc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_key_const_ipv4compmc_with_rpf(
     int unit
   )
{
  uint32
    res, vrf_ce_inst = 0, rif_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_2_16b = 0;
  flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_SIP_FWD_HEADER;
  flp_key_construction_tbl.instruction_4_32b = 0;
  flp_key_construction_tbl.instruction_5_32b = 0;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_MC){
      flp_key_construction_tbl.instruction_5_32b = ARAD_PP_CE_DIP_FWD_HEADER;
      flp_key_construction_tbl.instruction_0_16b = vrf_ce_inst;
	  if (SOC_IS_JERICHO(unit)) {
          flp_key_construction_tbl.instruction_1_16b = ARAD_PP_FLP_16B_INST_N_ZEROS(1);
      } else {
          flp_key_construction_tbl.instruction_1_16b = ARAD_PP_FLP_16B_INST_N_ZEROS(4);
      }
      flp_key_construction_tbl.instruction_2_16b = rif_ce_inst;

      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0xf;  
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x20; 
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0;
  }
  else
#endif
  {
      flp_key_construction_tbl.instruction_0_16b = rif_ce_inst;
      flp_key_construction_tbl.instruction_1_16b = vrf_ce_inst;
      flp_key_construction_tbl.instruction_4_32b = ARAD_PP_CE_DIP_FWD_HEADER;

      flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0xA ; 
      flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x12 ; 
      flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0;
  }


  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b = 0;
  flp_key_construction_tbl.instruction_1_16b = 0;
  flp_key_construction_tbl.instruction_2_16b = 0;
  flp_key_construction_tbl.instruction_3_32b = 0;
  flp_key_construction_tbl.instruction_4_32b = 0;
  flp_key_construction_tbl.instruction_5_32b = 0;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv4compmc_with_rpf", 0, 0);
}


uint32
   arad_pp_flp_lookups_ipv4compmc_with_rpf(
     int unit,
     uint32 tcam_access_profile_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  {
      
      flp_lookups_tbl.lem_1st_lkp_valid     = 1;
      flp_lookups_tbl.lem_1st_lkp_key_select = 1;
      flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
      flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;


      



      
      flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
      flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
      flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
      flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;

      flp_lookups_tbl.lpm_2nd_lkp_valid     = 1;
      flp_lookups_tbl.lpm_2nd_lkp_key_select = 0;
      flp_lookups_tbl.lpm_2nd_lkp_and_value = 3;
      flp_lookups_tbl.lpm_2nd_lkp_or_value = 0;

      flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

    #if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV4_MC){
          flp_lookups_tbl.elk_lkp_valid = 0x1;
          flp_lookups_tbl.elk_wait_for_reply = 0x1;
          flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP;

          flp_lookups_tbl.elk_key_a_valid_bytes = 8;
          flp_lookups_tbl.elk_key_b_valid_bytes = 4;
          flp_lookups_tbl.elk_key_c_valid_bytes = 0;
      }
    #endif
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv4compmc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_process_ipv4compmc_with_rpf(
     int unit
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  if (SOC_IS_JERICHO(unit)) {
      flp_process_tbl.include_lpm_1st_in_result_b     = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a     = 2;
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
	  flp_process_tbl.include_lem_1st_in_result_b     = 1;
	  flp_process_tbl.include_lem_2nd_in_result_a     = 1;
  }
  else {
      flp_process_tbl.include_tcam_in_result_a       = 1;
      flp_process_tbl.include_lem_1st_in_result_a     = 1;

      flp_process_tbl.include_lem_2nd_in_result_b     = 1;
#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV4_MC) {
      }
	  else
#endif
      {
          flp_process_tbl.include_lpm_2nd_in_result_b     = 1;
      }
  }

  if (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) {
      flp_process_tbl.include_tcam_in_result_a       = 1;
  }
  flp_process_tbl.result_a_format             = 0;
  flp_process_tbl.result_b_format             = 0;
  flp_process_tbl.compatible_mc_bridge_fallback = 1;
  flp_process_tbl.procedure_ethernet_default   = 3;
  flp_process_tbl.enable_hair_pin_filter        = 1;
  flp_process_tbl.enable_rpf_check             = 1;
  flp_process_tbl.not_found_trap_strength       = 0; 
    
  flp_process_tbl.select_default_result_a = 1;
  flp_process_tbl.select_default_result_b = 1;
  flp_process_tbl.sa_lkp_process_enable = 0;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_MC){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
      
      flp_process_tbl.include_lpm_2nd_in_result_a = 1;
      flp_process_tbl.include_lpm_1st_in_result_b = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default  = 1;
  }
#endif


  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 1;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV4COMPMC_WITH_RPF,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4compmc_with_rpf", 0, 0);
}




uint32
   arad_pp_flp_prog_sel_cam_ipv4compmc_with_mim_learning(
     int unit,
     int *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV4COMPMC_WITH_MIM_LEARNING,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  prog_selection_cam_tbl.parser_leaf_context          = ARAD_PARSER_PLC_PP;
  prog_selection_cam_tbl.port_profile                 = ARAD_PP_FLP_PORT_PROFILE_PBP;
  prog_selection_cam_tbl.ptc_profile                  = 0;
  prog_selection_cam_tbl.forwarding_code              = ARAD_PP_FWD_CODE_IPV4_MC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable         = 0x0;
  prog_selection_cam_tbl.packet_is_compatible_mc      = 1;
  prog_selection_cam_tbl.forwarding_offset_index      = 3;
  prog_selection_cam_tbl.parser_leaf_context_mask     = 0x03; 
  prog_selection_cam_tbl.port_profile_mask            = 3     ;
  prog_selection_cam_tbl.ptc_profile_mask             = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask         = 0x00;
  prog_selection_cam_tbl.forwarding_offset_index_mask = 0;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask    = 0x1;
  prog_selection_cam_tbl.packet_is_compatible_mc_mask = 0x1;  

  prog_selection_cam_tbl.program                   = *prog_id ;
  prog_selection_cam_tbl.valid                     = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_ipv4compmc_with_mim_learning", 0, 0);
}

uint32
   arad_pp_flp_key_const_ipv4compmc_with_mim_learning(
     int unit,
     int prog_id
   )
{
  uint32
    res, pp_port_ce_inst = 0, vrf_ce_inst = 0, rif_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_JERICHO(unit)) {
      arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 0, &pp_port_ce_inst);
  }

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_1_16b = (SOC_IS_JERICHO(unit) ? pp_port_ce_inst : ARAD_PP_FLP_16B_INST_P6_IN_PORT_D);
  flp_key_construction_tbl.instruction_2_16b = ARAD_PP_CE_ETH_HEADER_OUTER_TAG;
  flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_SA_24MSB;
  flp_key_construction_tbl.instruction_4_32b = ARAD_PP_CE_SA_24LSB;
  flp_key_construction_tbl.instruction_5_32b = ARAD_PP_CE_DIP_FWD_HEADER;

  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x1e;   
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x20;   
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0;

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b = 0;
  flp_key_construction_tbl.instruction_1_16b = 0;
  flp_key_construction_tbl.instruction_2_16b = 0;
  flp_key_construction_tbl.instruction_3_32b = 0;
  flp_key_construction_tbl.instruction_4_32b = 0;
  flp_key_construction_tbl.instruction_5_32b = 0;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_ipv4compmc_with_mim_learning", 0, 0);
}


uint32
   arad_pp_flp_lookups_ipv4compmc_with_mim_learning(
     int unit,
     uint32 tcam_access_profile_id,
     int prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  {
      
      flp_lookups_tbl.lem_1st_lkp_valid     = 1;
      flp_lookups_tbl.lem_1st_lkp_key_select = 1;
      flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
      flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;


      



      
      flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
      flp_lookups_tbl.lem_2nd_lkp_key_select = 0;
      flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
      flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_MAC_IN_MAC_TUNNEL_KEY_OR_MASK;

      flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

    #if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV4_MC){
          flp_lookups_tbl.elk_lkp_valid = 0x1;
          flp_lookups_tbl.elk_wait_for_reply = 0x1;
          flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP;

          flp_lookups_tbl.elk_key_a_valid_bytes = 8;
          flp_lookups_tbl.elk_key_b_valid_bytes = 4;
          flp_lookups_tbl.elk_key_c_valid_bytes = 0;
      }
    #endif
  }

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_ipv4compmc_with_mim_learning", 0, 0);
}

uint32
   arad_pp_flp_process_ipv4compmc_with_mim_learning(
     int unit,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  if (SOC_IS_JERICHO(unit)) {
      flp_process_tbl.include_lpm_1st_in_result_b     = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;
      flp_process_tbl.include_lpm_2nd_in_result_a     = 2;
      flp_process_tbl.lpm_2nd_lkp_enable_default     = 1;
      flp_process_tbl.lpm_public_2nd_lkp_enable_default     = 1;
      flp_process_tbl.include_lem_1st_in_result_b     = 1;
      flp_process_tbl.include_lem_2nd_in_result_a     = 1;
  }
  else {
      flp_process_tbl.include_tcam_in_result_a       = 1;
      flp_process_tbl.include_lem_1st_in_result_a     = 1;

      flp_process_tbl.include_lem_2nd_in_result_b     = 1;
#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV4_MC) {
      }
	  else
#endif
      {
          flp_process_tbl.include_lpm_2nd_in_result_b     = 1;
      }
  }

  if (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) {
      flp_process_tbl.include_tcam_in_result_a       = 1;
  }
  flp_process_tbl.result_a_format             = 0;
  flp_process_tbl.result_b_format             = 0;
  flp_process_tbl.compatible_mc_bridge_fallback = 1;
  flp_process_tbl.procedure_ethernet_default   = 3;
  flp_process_tbl.enable_hair_pin_filter        = 1;
  flp_process_tbl.enable_rpf_check             = 1;
  flp_process_tbl.not_found_trap_strength       = 0; 
    
  flp_process_tbl.select_default_result_a = 1;
  flp_process_tbl.select_default_result_b = 1;
  flp_process_tbl.sa_lkp_process_enable = 0;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_MC){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 1;
      
      flp_process_tbl.include_lpm_2nd_in_result_a = 1;
      flp_process_tbl.include_lpm_1st_in_result_b = 1;
      flp_process_tbl.lpm_2nd_lkp_enable_default  = 1;
      flp_process_tbl.lpm_1st_lkp_enable_default  = 1;
  }
#endif


  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  tmp = 1;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_ipv4compmc_with_mim_learning", 0, 0);
}




uint32
   arad_pp_flp_prog_sel_cam_global_ipv4compmc_with_rpf(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;

  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF,FALSE,TRUE,&cam_sel_id, prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_PP_AND_PP_L4,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.port_profile                 = 0;
  prog_selection_cam_tbl.ptc_profile                  = 0;
  prog_selection_cam_tbl.forwarding_code              = ARAD_PP_FWD_CODE_IPV4_MC;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable         = 0x0;
  prog_selection_cam_tbl.packet_is_compatible_mc      = 1;
  prog_selection_cam_tbl.port_profile_mask            = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP;
  prog_selection_cam_tbl.ptc_profile_mask             = 0x03;
  prog_selection_cam_tbl.forwarding_code_mask         = 0x00;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask    = 0x1;
  prog_selection_cam_tbl.packet_is_compatible_mc_mask = 0x1; 

  prog_selection_cam_tbl.program                      = *prog_id;
  prog_selection_cam_tbl.valid                        = 1;
  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_global_ipv4compmc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_key_const_global_ipv4compmc_with_rpf(
     int    unit,
     int32 prog_id
   )
{
  uint32
    res, vrf_ce_inst = 0, rif_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_key_construction_tbl.instruction_2_16b = 0;
  flp_key_construction_tbl.instruction_3_32b = ARAD_PP_CE_SIP_FWD_HEADER;
  flp_key_construction_tbl.instruction_4_32b = 0;
  flp_key_construction_tbl.instruction_5_32b = ARAD_PP_CE_DIP_FWD_HEADER_27_0;

  flp_key_construction_tbl.instruction_0_16b = rif_ce_inst;
  flp_key_construction_tbl.instruction_1_16b = vrf_ce_inst;
  flp_key_construction_tbl.instruction_4_32b = ARAD_PP_CE_DIP_FWD_HEADER;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0xA  ; 
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x29 ; 

  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  flp_key_construction_tbl.instruction_0_16b = 0;
  flp_key_construction_tbl.instruction_1_16b = 0;
  flp_key_construction_tbl.instruction_2_16b = 0;
  flp_key_construction_tbl.instruction_3_32b = 0;
  flp_key_construction_tbl.instruction_4_32b = 0;
  flp_key_construction_tbl.instruction_5_32b = 0;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_global_ipv4compmc_with_rpf", 0, 0);
}


uint32
   arad_pp_flp_lookups_global_ipv4compmc_with_rpf(
     int    unit,
     int prog_id,
     uint32 tcam_access_profile_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;
  uint32 prefix = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res =  arad_pp_lem_access_app_to_prefix_get(unit, ARAD_PP_FLP_GLOBAL_IPV4_KEY_OR_MASK,&prefix);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = 1;
  
  flp_lookups_tbl.lem_2nd_lkp_and_value  = ((~prefix) & 0xF);
  flp_lookups_tbl.lem_2nd_lkp_or_value   = prefix;

  

  
  flp_lookups_tbl.lem_1st_lkp_valid     = 1;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK;
  flp_lookups_tbl.lpm_1st_lkp_valid     = 1;
  flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
  flp_lookups_tbl.lpm_1st_lkp_and_value = 3;
  flp_lookups_tbl.lpm_1st_lkp_or_value = 0;
  flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_global_ipv4compmc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_process_global_ipv4compmc_with_rpf(
     int    unit,
     int prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flp_process_tbl.include_tcam_in_result_a       = 1;
  flp_process_tbl.include_lem_2nd_in_result_a     = 1;

  flp_process_tbl.include_lem_1st_in_result_b     = 1;
  flp_process_tbl.include_lpm_1st_in_result_b     = 1;
  flp_process_tbl.lpm_1st_lkp_enable_default     = 1;
  flp_process_tbl.lpm_public_1st_lkp_enable_default     = 1;

  flp_process_tbl.result_a_format             = 0;
  flp_process_tbl.result_b_format             = 0;
  flp_process_tbl.compatible_mc_bridge_fallback = 1;
  flp_process_tbl.procedure_ethernet_default   = 3;
  flp_process_tbl.enable_hair_pin_filter        = 1;
  flp_process_tbl.enable_rpf_check             = 1;
  flp_process_tbl.not_found_trap_strength       = 0; 
    
  flp_process_tbl.select_default_result_a = 1;
  flp_process_tbl.select_default_result_b = 1;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  tmp = 1;
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_global_ipv4compmc_with_rpf", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_pppoe_ipv4(
     int unit
   )
{
    uint32
        res,
        program_id;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    program_id = PROG_FLP_PPPOE_IPV4UC;
    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, program_id, TRUE, TRUE, &cam_sel_id, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_IPV4,
                &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_IPV4, DPP_PLC_ANY_PP,
                &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    prog_selection_cam_tbl.port_profile                = 0;
    prog_selection_cam_tbl.ptc_profile                 = 0;
    prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV4_UC;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
    prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
    prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
    prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x00;
    prog_selection_cam_tbl.tt_processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PPPoE;
    prog_selection_cam_tbl.tt_processing_profile_mask  = 0;
    prog_selection_cam_tbl.program                     = program_id;
    prog_selection_cam_tbl.valid                       = 1;
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_pppoe_ipv4", 0, 0);
}

uint32
   arad_pp_flp_process_pppoe_ipv4(
     int unit
   )
{
    uint32
        res;
    uint32
        tmp;
    soc_reg_above_64_val_t
        reg_val;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
        flp_process_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_PPPOE_IPV4UC, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    flp_process_tbl.include_lem_2nd_in_result_a        = 1;
    flp_process_tbl.include_lem_1st_in_result_b        = 1;

    flp_process_tbl.result_a_format                    = 2;
    flp_process_tbl.result_b_format                    = 2;
    flp_process_tbl.procedure_ethernet_default
        = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
    flp_process_tbl.enable_hair_pin_filter             = 1;
    flp_process_tbl.enable_rpf_check                   = 0;
    
    flp_process_tbl.select_default_result_a            = 0;
    flp_process_tbl.select_default_result_b            = 0;
    flp_process_tbl.sa_lkp_process_enable              = 0;
    flp_process_tbl.not_found_trap_strength            = 3;

    flp_process_tbl.lpm_2nd_lkp_enable_default         = 1;
    flp_process_tbl.lpm_public_2nd_lkp_enable_default  = 1;
    flp_process_tbl.lpm_1st_lkp_enable_default         = 1;
    flp_process_tbl.lpm_public_1st_lkp_enable_default  = 1;
    flp_process_tbl.include_lpm_2nd_in_result_a        = 1;
    flp_process_tbl.include_lpm_1st_in_result_b        = 1;

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_PPPOE_IPV4UC, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    tmp = 1; 
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV4UC,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pppoe_ipv4", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_pppoe_ipv6(
     int unit
   )
{
    uint32
        res,
        program_id;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    program_id = PROG_FLP_PPPOE_IPV6UC;
    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, program_id, TRUE, TRUE, &cam_sel_id, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY_IPV6,
                &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY_IPV6, DPP_PLC_ANY_PP,
                &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    prog_selection_cam_tbl.port_profile                = 0;
    prog_selection_cam_tbl.ptc_profile                 = 0;
    prog_selection_cam_tbl.forwarding_code             = ARAD_PP_FWD_CODE_IPV6_UC;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable        = 0x0;
    prog_selection_cam_tbl.port_profile_mask           = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
    prog_selection_cam_tbl.ptc_profile_mask            = 0x03;
    prog_selection_cam_tbl.forwarding_code_mask        = 0x00;
    prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask   = 0x00;
    prog_selection_cam_tbl.tt_processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PPPoE;
    prog_selection_cam_tbl.tt_processing_profile_mask  = 0;
    prog_selection_cam_tbl.program                     = program_id;
    prog_selection_cam_tbl.valid                       = 1;
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_pppoe_ipv6", 0, 0);
}

uint32
   arad_pp_flp_process_pppoe_ipv6(
     int unit
   )
{
    uint32
        res;
    uint32
        tmp;
    soc_reg_above_64_val_t
        reg_val;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
        flp_process_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_PPPOE_IPV6UC, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    flp_process_tbl.include_lem_2nd_in_result_a        = 0;
    flp_process_tbl.include_lem_1st_in_result_b        = 1;

    flp_process_tbl.result_a_format                    = 2;
    flp_process_tbl.result_b_format                    = 2;
    flp_process_tbl.procedure_ethernet_default
        = SOC_DPP_CONFIG(unit)->pp.eth_default_procedure_disable ? 0 : 3;
    flp_process_tbl.enable_hair_pin_filter             = 1;
    flp_process_tbl.enable_rpf_check                   = 0;
    
    flp_process_tbl.select_default_result_a            = 0;
    flp_process_tbl.select_default_result_b            = 0;
    flp_process_tbl.sa_lkp_process_enable              = 0;
    flp_process_tbl.not_found_trap_strength            = 3;

    flp_process_tbl.lpm_2nd_lkp_enable_default         = 1;
    flp_process_tbl.lpm_public_2nd_lkp_enable_default  = 1;
    flp_process_tbl.lpm_1st_lkp_enable_default         = 1;
    flp_process_tbl.lpm_public_1st_lkp_enable_default  = 1;
    flp_process_tbl.include_lpm_2nd_in_result_a        = 1;
    flp_process_tbl.include_lpm_1st_in_result_b        = 1;

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_PPPOE_IPV6UC, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    tmp = 2; 
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*PROG_FLP_IPV6UC,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pppoe_ipv6", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_mymac_ip_disabled_trap(
     int unit,
     int *prog_id
   )
{
    uint32
        res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *prog_id = 0;
    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit, PROG_FLP_MYMAC_IP_DISABLED_TRAP, FALSE, TRUE, &cam_sel_id, prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    prog_selection_cam_tbl.tt_processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MYMAC_IP_DISABLED_TRAP;
    prog_selection_cam_tbl.tt_processing_profile_mask  = 0;
    prog_selection_cam_tbl.program                     = *prog_id;
    prog_selection_cam_tbl.valid                       = 1;
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_pppoe_ipv4", 0, 0);
}

uint32
   arad_pp_flp_process_mymac_ip_disabled_trap(
     int unit,
     int program_id
   )
{
    uint32
        res;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
        flp_process_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, program_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    flp_process_tbl.not_found_trap_code                = SOC_PPC_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE;

    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, program_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_pppoe_ipv4", 0, 0);
}

 uint32
   arad_pp_flp_process_gpon_l2_ip4mc(int unit, int prog_id)
{
  uint32 res;
  uint32 tmp;
  soc_reg_above_64_val_t reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  sal_memset(&flp_process_tbl, 0x0, sizeof(ARAD_PP_IHB_FLP_PROCESS_TBL_DATA));

  flp_process_tbl.include_lem_1st_in_result_a = 1;     
  flp_process_tbl.include_lem_2nd_in_result_a = 2;     
  flp_process_tbl.select_default_result_a     = 2;     
  flp_process_tbl.result_a_format             = 0;     
  flp_process_tbl.apply_fwd_result_a           = 1;    
  flp_process_tbl.procedure_ethernet_default   = 1;
  flp_process_tbl.unknown_address              = 3;
  flp_process_tbl.fwd_processing_profile       = 0;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0;
  
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_vpws_tagged", 0, 0);
}

uint32
   arad_pp_flp_process_gpon_l2_ip6mc(
     int unit,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  sal_memset(&flp_process_tbl, 0x0, sizeof(ARAD_PP_IHB_FLP_PROCESS_TBL_DATA));

  flp_process_tbl.include_lem_1st_in_result_a = 1;     
  flp_process_tbl.select_default_result_a     = 2;     
  flp_process_tbl.result_a_format             = 0;     
  flp_process_tbl.apply_fwd_result_a           = 1;    
  flp_process_tbl.procedure_ethernet_default   = 1;
  flp_process_tbl.unknown_address              = 3;
  flp_process_tbl.fwd_processing_profile       = 0;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 0; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_gpon_l2_ip6mc", 0, 0);
}


uint32
   arad_pp_flp_static_programs_init(
     int unit)
{
  uint32 res;
  uint8  mac_in_mac_enabled=0;
#if defined(INCLUDE_L3)
  uint8  is_routing_enabled=0;
#endif
  uint64 hw_prog_bitmap;
  uint64 prog_sel_bitmap;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  COMPILER_64_ZERO(hw_prog_bitmap);
  COMPILER_64_ZERO(prog_sel_bitmap);

  
  

  
  if (SOC_DPP_PP_ENABLE(unit)) {
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_TM);
  }

  
  if (SOC_DPP_L3_SRC_BIND_IPV4_ENABLE(unit)) {
      if (!SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP);
      }
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC);
  }

  
  if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit)) {
      if (!SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP);
      }
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC);
  }

  
  res = arad_pp_is_mac_in_mac_enabled(unit,&mac_in_mac_enabled);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (mac_in_mac_enabled) {
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_ETHERNET_MAC_IN_MAC);
  }

  
  ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_P2P);

  
  if (!ARAD_PP_FLP_PON_PROG_LOAD()) {
      
      if (soc_property_get(unit, spn_VPWS_TAGGED_MODE, 0)) {
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_VPWS_TAGGED_SINGLE_TAG);
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_VPWS_TAGGED_DOUBLE_TAG);
      }

      
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_ETHERNET_ING_LEARN);
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_MPLS_CONTROL_WORD_TRAP);
  }

  
  if (ARAD_PP_FLP_PON_PROG_LOAD() && ARAD_PP_FLP_IBC_PORT_ENABLE(unit)) {
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_ETHERNET_ING_LEARN);
  }

#if defined(INCLUDE_L3)

  res = arad_pp_sw_db_ipv4_is_routing_enabled(unit, &is_routing_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
  if (is_routing_enabled) {

      
	  ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV4UC_PUBLIC);

      
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV4UC);

      
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV4UC_RPF);

      
#if defined(INCLUDE_KBP)
      if (SOC_IS_JERICHO(unit) && ARAD_KBP_ENABLE_IPV4_DC) {
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV4_DC);
      }
#endif

      
      if (!ARAD_PP_FLP_PON_PROG_LOAD()){
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV4MC_BRIDGE);
      }

      
      if (SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) {
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV4COMPMC_WITH_RPF);
      }

      
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV6UC);

      
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_IPV6MC);

      
      ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_LSR);

      
      if (SOC_DPP_CONFIG(unit)->trill.mode)
      {
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_TRILL_UC);
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_TRILL_MC_ONE_TAG);
          ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_TRILL_MC_TWO_TAGS);
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              ARAD_PP_FLP_COMPILER_64_BITSET_WITH_VALIDATION(hw_prog_bitmap, PROG_FLP_TRILL_AFTER_TERMINATION);
          }
      }
  }
#endif 

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_hw_prog_id_bitmap.set(unit,hw_prog_bitmap);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_prog_select_id_bitmap.set(unit,prog_sel_bitmap);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_init", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_init(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  uint32
    addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&prog_selection_cam_tbl, ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ANY,
              &prog_selection_cam_tbl.packet_format_code, &prog_selection_cam_tbl.packet_format_code_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  res = dpp_parser_plc_hw_by_sw(unit, DPP_PFC_ANY, DPP_PLC_ANY,
              &prog_selection_cam_tbl.parser_leaf_context, &prog_selection_cam_tbl.parser_leaf_context_mask);
  SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);

  prog_selection_cam_tbl.valid = 0;
  prog_selection_cam_tbl.port_profile_mask                = ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE;
  prog_selection_cam_tbl.ptc_profile_mask                 = 0x3;
  prog_selection_cam_tbl.forwarding_header_qualifier_mask = 0x7FF;
  prog_selection_cam_tbl.forwarding_code_mask            = 0xF;
  prog_selection_cam_tbl.forwarding_offset_index_mask     = SOC_IS_ARAD_B1_AND_BELOW(unit)? 0x3 : 0x7;
  prog_selection_cam_tbl.l_3_vpn_default_routing_mask       = 0x1;
  prog_selection_cam_tbl.trill_mc_mask                   = 0x1;
  prog_selection_cam_tbl.packet_is_compatible_mc_mask      = 0x1;
  prog_selection_cam_tbl.in_rif_uc_rpf_enable_mask          = 0x1;
  prog_selection_cam_tbl.ll_is_arp_mask                   = 0x1;
  prog_selection_cam_tbl.elk_status_mask                 = 0x1;
  prog_selection_cam_tbl.cos_profile_mask                = 0x3F;
  prog_selection_cam_tbl.service_type_mask               = 0x7;
  prog_selection_cam_tbl.vt_processing_profile_mask       = 0x7;
  prog_selection_cam_tbl.vt_lookup_0_found_mask            = 0x1;
  prog_selection_cam_tbl.vt_lookup_1_found_mask            = 0x1;
  prog_selection_cam_tbl.tt_processing_profile_mask       = 0x7;
  prog_selection_cam_tbl.tt_lookup_0_found_mask            = 0x1;
  prog_selection_cam_tbl.tt_lookup_1_found_mask           = 0x1;

  if (SOC_IS_JERICHO(unit)) {
      prog_selection_cam_tbl.forwarding_offset_index_ext_mask           = 0x3;
      prog_selection_cam_tbl.cpu_trap_code_mask                         = 0xFF;
      prog_selection_cam_tbl.in_lif_profile_mask                        = 0xF;
      prog_selection_cam_tbl.llvp_incoming_tag_structure_mask           = 0xF;
      prog_selection_cam_tbl.forwarding_plus_1_header_qualifier_mask    = 0x7FF;
      prog_selection_cam_tbl.tunnel_termination_code_mask               = 0xF;
      prog_selection_cam_tbl.qualifier_0_mask                           = 0x7;
      prog_selection_cam_tbl.in_lif_data_index_mask                     = 0x3;
      prog_selection_cam_tbl.in_lif_data_msb_mask                       = 0x1F;
      prog_selection_cam_tbl.custom_rif_profile_bit_mask                = 0x1;
  }

  for(addr = 0; addr < SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines); ++addr)
  {
    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, addr, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_init", 0, 0);
}


uint32
   arad_pp_flp_prog_sel_cam_gpon_l2_ip4mc(
     int unit,
     int32 *prog_id
   )
{
    uint32 res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV4_MC_SSM,FALSE,TRUE,&cam_sel_id,prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    prog_selection_cam_tbl.packet_format_code            = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection_cam_tbl.packet_format_code_mask       = 0x18 ;
    prog_selection_cam_tbl.forwarding_code               = ARAD_PP_FWD_CODE_ETHERNET;
    prog_selection_cam_tbl.forwarding_code_mask          = 0x00;
    prog_selection_cam_tbl.packet_is_compatible_mc        = 1;
    prog_selection_cam_tbl.packet_is_compatible_mc_mask   = 0x0; 
    prog_selection_cam_tbl.ptc_profile                           = SOC_TMC_PORTS_FLP_PROFILE_NONE; 
    prog_selection_cam_tbl.ptc_profile_mask              = 0;
    prog_selection_cam_tbl.cos_profile                   = SOC_PPC_LIF_AC_L2_SSM_IPMC_COS_PROFILE;
    prog_selection_cam_tbl.cos_profile_mask              = 0x1F;

    prog_selection_cam_tbl.parser_leaf_context = ARAD_PARSER_PLC_PP;
    prog_selection_cam_tbl.parser_leaf_context_mask = 0x03; 
    prog_selection_cam_tbl.valid = 1;
    prog_selection_cam_tbl.program = *prog_id;

    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit,cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_vpws_tagged", 0, 0);
}

uint32
   arad_pp_flp_prog_sel_cam_gpon_l2_ip6mc(
     int unit,
     int32 *prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_IPV6_MC_SSM_EUI,FALSE,TRUE,&cam_sel_id,prog_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  
  prog_selection_cam_tbl.packet_format_code            = ARAD_PARSER_PFC_IPV6_ETH;
  prog_selection_cam_tbl.packet_format_code_mask       = 0x18 ;
  prog_selection_cam_tbl.forwarding_code               = ARAD_PP_FWD_CODE_ETHERNET;
  prog_selection_cam_tbl.forwarding_code_mask          = 0x00;
  prog_selection_cam_tbl.packet_is_compatible_mc        = 1;
  prog_selection_cam_tbl.packet_is_compatible_mc_mask   = 0x0; 
  prog_selection_cam_tbl.ptc_profile                           = SOC_TMC_PORTS_FLP_PROFILE_NONE; 
  prog_selection_cam_tbl.ptc_profile_mask              = 0;
  prog_selection_cam_tbl.cos_profile                   = 0x20;
  prog_selection_cam_tbl.cos_profile_mask              = 0x1F;

  prog_selection_cam_tbl.program                       = *prog_id;
  prog_selection_cam_tbl.valid                                 = 1;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_sel_cam_gpon_l2_ip6mc", 0, 0);
}

uint32
   arad_pp_flp_lookups_init(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;
  uint32
    addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_CLEAR(&flp_lookups_tbl, ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA, 1);

  flp_lookups_tbl.elk_lkp_valid      = 0;
  flp_lookups_tbl.lem_1st_lkp_valid  = 0;
  flp_lookups_tbl.lem_2nd_lkp_valid  = 0;
  flp_lookups_tbl.lpm_1st_lkp_valid  = 0;
  flp_lookups_tbl.lpm_2nd_lkp_valid  = 0;
  flp_lookups_tbl.enable_tcam_identification_ieee_1588 = 0;
  flp_lookups_tbl.enable_tcam_identification_oam      = 0;
  flp_lookups_tbl.tcam_lkp_db_profile = 0x3F;
  flp_lookups_tbl.tcam_traps_lkp_db_profile_0 = 0x3F;
  flp_lookups_tbl.tcam_traps_lkp_db_profile_1 = 0x3F;

  if (SOC_IS_JERICHO(unit)) {
      flp_lookups_tbl.tcam_lkp_db_profile_1 = 0x3F;
  }

  for(addr = 0; addr < 0x18; ++addr)
  {
    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, addr, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_init", 0, 0);
}





#if defined(INCLUDE_KBP)
uint32
   arad_pp_flp_elk_result_configure(int unit, int prog_id, int fwd_res_size, int fwd_res_found_bit_offset,
                                                           int fec_res_size, int fec_res_data_start, int fec_res_found_bit_offset )
{
  uint32 res, mem_val=0;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;

  int fwd_res_data_start = ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS - fwd_res_size;

  uint32 fld_val_fwd_res_found_offset = 0, fld_val_fwd_res_data_offset = 0, fld_val_fwd_data_type = 0;
  uint32 fld_val_fec_res_found_offset = 0, fld_val_fec_res_data_offset = 0, fld_val_fec_data_type = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&flp_process_tbl, ARAD_PP_IHB_FLP_PROCESS_TBL_DATA, 1);

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  if ((fwd_res_size != ARAD_PP_FLP_DEFAULT_FWD_RES_SIZE) && (fwd_res_size != ARAD_KBP_24BIT_FWD_RES_SIZE) && (fwd_res_size != 0)) {
      LOG_ERROR(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"Error in %s(): fwd_res_size not valid size %d\n"),
                       FUNCTION_NAME(), fwd_res_size));
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
  }

  fld_val_fwd_data_type = (fwd_res_size == ARAD_KBP_24BIT_FWD_RES_SIZE) ? 0x2 : 0x7;
  

  if (prog_id == PROG_FLP_IPV4_DC && ARAD_KBP_IPV4DC_24BIT_FWD) {
      
      fld_val_fwd_data_type = 0x2;
      
      fwd_res_size = ARAD_KBP_24BIT_FWD_RES_SIZE;
      fwd_res_data_start = ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS - fwd_res_size;
  }

  if (fwd_res_found_bit_offset > ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS+7 || fwd_res_found_bit_offset < ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS) {
      
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
  }

  fld_val_fwd_res_found_offset = fwd_res_found_bit_offset - ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS;

  if (fwd_res_data_start%8 > 0) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
  }else{
      fld_val_fwd_res_data_offset = fwd_res_data_start/8;
      if((prog_id == PROG_FLP_IPV4_DC && ARAD_KBP_IPV4DC_24BIT_FWD) || (fwd_res_size == ARAD_KBP_24BIT_FWD_RES_SIZE)) {
          
          if (fld_val_fwd_res_data_offset != 12) {
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
          }
      } else {
          
          if (fld_val_fwd_res_data_offset != 9) {
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
          }
      }
  }

  soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_FOUND_OFFSETf, &fld_val_fwd_res_found_offset);
  soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_DATA_OFFSETf, &fld_val_fwd_res_data_offset);
  soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_DATA_FORMATf, &fld_val_fwd_data_type);



  if (fec_res_size == 0) {
      res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, prog_id, &mem_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
      goto exit;
  }
  

  if (fec_res_found_bit_offset > ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS+7 || fec_res_found_bit_offset < ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS) {
      
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
  }

  fld_val_fec_data_type = 0x2;
  

  if (prog_id == PROG_FLP_IPV4_DC && !ARAD_KBP_IPV4DC_24BIT_FWD) {
      
      fld_val_fec_data_type = 0x7;
  }

  fld_val_fec_res_found_offset = fec_res_found_bit_offset - ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS;

  if (fec_res_data_start%8 > 0) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
  }else{
      fld_val_fec_res_data_offset = fec_res_data_start/8;
      
      if (fld_val_fec_res_data_offset > 7) {
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
      }
  }

  soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_FOUND_OFFSETf, &fld_val_fec_res_found_offset);
  soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_OFFSETf, &fld_val_fec_res_data_offset);
  soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_FORMATf, &fld_val_fec_data_type);

  res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, prog_id, &mem_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_elk_result_configure", prog_id, 0);
}
#endif 


uint32
   arad_pp_flp_init_bfd_program(int unit)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop || (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable==SOC_DPP_ARAD_BFD_IPV6_SUPPORT_WITH_LEM)) {

        res = arad_pp_flp_prog_sel_cam_bfd_single_hop_bridge(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 875, exit);

        if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop) {
            int bfd_ipv4_single_hop_prog_id;
            res = arad_pp_flp_prog_sel_cam_bfd_single_hop(unit, 0, &bfd_ipv4_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 880, exit);
            res = arad_pp_flp_key_const_bfd_single_hop(unit, bfd_ipv4_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 885, exit);
            res = arad_pp_flp_lookups_bfd_single_hop(unit, bfd_ipv4_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 890, exit);
            res = arad_pp_flp_process_bfd_single_hop(unit, 0, bfd_ipv4_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 896, exit);
        }

        if ((SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable==SOC_DPP_ARAD_BFD_IPV6_SUPPORT_WITH_LEM)) {
            int bfd_ipv6_single_hop_prog_id;
            res = arad_pp_flp_prog_sel_cam_bfd_single_hop(unit, 1, &bfd_ipv6_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 900, exit);
            res = arad_pp_flp_key_const_bfd_single_hop(unit, bfd_ipv6_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 905, exit);
            res = arad_pp_flp_lookups_bfd_single_hop(unit, bfd_ipv6_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 910, exit);
            res = arad_pp_flp_process_bfd_single_hop(unit, 1, bfd_ipv6_single_hop_prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 915, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_init_bfd", 0, 0);
}


STATIC uint32 arad_pp_flp_program_get_namespace(int unit, uint8 app_id, int *name_space_idx)
{
    uint32 res=0;
    uint8 mac_in_mac_enabled;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    res = arad_pp_is_mac_in_mac_enabled(unit, &mac_in_mac_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    
    if (SOC_DPP_PPPOE_IS_ENABLE(unit)) {
        if (app_id == PROG_FLP_PPPOE_IPV4UC) {
            *name_space_idx = ARAD_PP_FLP_PROG_IPV4;
            SOC_SAND_EXIT_NO_ERROR;
        } else if (app_id == PROG_FLP_PPPOE_IPV6UC) {
            *name_space_idx = ARAD_PP_FLP_PROG_IPV6;
            SOC_SAND_EXIT_NO_ERROR;
        }
    }

    
    if (ARAD_PP_FLP_PON_PROG_LOAD()) {
        if (app_id == PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC
            || (app_id == PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP)
            || (app_id == PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC)
            || (app_id == PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP)) {
            *name_space_idx = ARAD_PP_FLP_PROG_ETH;
            SOC_SAND_EXIT_NO_ERROR;
        }
    }

    
    if (mac_in_mac_enabled) {
        if (app_id == PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM) {
            *name_space_idx = ARAD_PP_FLP_PROG_ETH;
            SOC_SAND_EXIT_NO_ERROR;
        }
    }
    
    
    switch (app_id) {
        case PROG_FLP_ETHERNET_ING_LEARN:
        case PROG_FLP_P2P:
        case PROG_FLP_TRILL_UC:
        case PROG_FLP_TRILL_MC_ONE_TAG:
        case PROG_FLP_TRILL_MC_TWO_TAGS:
        case PROG_FLP_TRILL_AFTER_TERMINATION:
        case PROG_FLP_ETHERNET_MAC_IN_MAC:
        case PROG_FLP_IPV4MC_BRIDGE:
        case PROG_FLP_VMAC_UPSTREAM:
        case PROG_FLP_VMAC_DOWNSTREAM:
        case PROG_FLP_ETHERNET_PON_DEFAULT_UPSTREAM:
        case PROG_FLP_ETHERNET_PON_DEFAULT_DOWNSTREAM:
        case PROG_FLP_ETHERNET_PON_LOCAL_ROUTE:
        case PROG_FLP_ETHERNET_ING_IVL_LEARN:
        case PROG_FLP_ETHERNET_ING_IVL_INNER_LEARN:
        case PROG_FLP_ETHERNET_ING_IVL_FWD_OUTER_LEARN:
            *name_space_idx = ARAD_PP_FLP_PROG_ETH;
            break;
            
        case PROG_FLP_IPV4UC_PUBLIC:
        case PROG_FLP_IPV4UC_RPF:
        case PROG_FLP_IPV4UC:  
        case PROG_FLP_IPV4COMPMC_WITH_RPF:
        case PROG_FLP_IPV4_DC:
        case PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF:
        case PROG_FLP_IPV4_MC_SSM:
        case PROG_FLP_BIDIR:    
        case PROG_FLP_PON_ARP_DOWNSTREAM:
        case PROG_FLP_PON_ARP_UPSTREAM:
        case PROG_FLP_BFD_IPV4_SINGLE_HOP:
        case PROG_FLP_IPV4UC_PUBLIC_RPF:
            *name_space_idx = ARAD_PP_FLP_PROG_IPV4;
            break;
            
        case PROG_FLP_IPV6MC:
        case PROG_FLP_IPV6UC:
        case PROG_FLP_IPV6UC_RPF:
        case PROG_FLP_IPV6UC_PUBLIC:
        case PROG_FLP_IPV6UC_PUBLIC_RPF:
        case PROG_FLP_IPV6_MC_SSM_EUI:
        case PROG_FLP_IPV6UC_WITH_RPF_2PASS:
        case PROG_FLP_BFD_IPV6_SINGLE_HOP:
        case PROG_FLP_IPV6_MC_SSM:
        case PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP:
            *name_space_idx = ARAD_PP_FLP_PROG_IPV6;
            break;
            
        case PROG_FLP_LSR:
        case PROG_FLP_VPWS_TAGGED_SINGLE_TAG:
        case PROG_FLP_VPWS_TAGGED_DOUBLE_TAG:
        case PROG_FLP_VPLSOGRE:
        case PROG_FLP_MPLS_CONTROL_WORD_TRAP:
        case PROG_FLP_BFD_MPLS_STATISTICS:
        case PROG_FLP_BFD_PWE_STATISTICS:
            *name_space_idx = ARAD_PP_FLP_PROG_MPLS;
            break;
            
        default:
            *name_space_idx = ARAD_PP_FLP_PROG_NOP;
            break;
    }
    SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_init_bfd", 0, 0);
}



STATIC uint32 arad_pp_flp_program_assigne_namespace(int unit, uint8 app_id, int hw_prog)
{
    uint32 res;
    int name_space_idx = ARAD_PP_FLP_PROG_NOP;
    uint16 count=0;
    uint8 hw_prog_tmp = INVALID_FLP_HW_PROG;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_flp_program_get_namespace(unit, app_id, &name_space_idx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = sw_state_access[unit].dpp.soc.arad.pp.fec.name_space.count.get(unit, name_space_idx, &count);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = sw_state_access[unit].dpp.soc.arad.pp.fec.name_space.hw_prog.get(unit, name_space_idx, count, &hw_prog_tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);    
    if (hw_prog_tmp == INVALID_FLP_HW_PROG) {
        res = sw_state_access[unit].dpp.soc.arad.pp.fec.name_space.hw_prog.set(unit, name_space_idx, count, hw_prog);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        count++;
        res = sw_state_access[unit].dpp.soc.arad.pp.fec.name_space.count.set(unit, name_space_idx, count);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);   
   }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_assigne_program_name_space", 0, 0);

}



uint32 arad_pp_flp_update_programs_pem_contex(int unit)
{
    int i;
    int prog_id;
    uint32 res;
    int nof_flp_selections;
    int nof_flp_hw_programs;
    uint64 hw_prog_bitmap;   
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    nof_flp_hw_programs = SOC_DPP_DEFS_GET(unit, nof_flp_programs);
    nof_flp_selections = SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines);

    res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_hw_prog_id_bitmap.get(unit,&hw_prog_bitmap);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    
    if (SOC_IS_QUX(unit)) {
        for (prog_id = 0; prog_id < nof_flp_hw_programs; prog_id++) {
            if (!COMPILER_64_BITTEST(hw_prog_bitmap, prog_id)) {
                continue;
            }

            for (i = 0; i <nof_flp_selections; i++) {
                ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA prog_selection_cam_tbl;
                res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, i, &prog_selection_cam_tbl);
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

                if ((prog_selection_cam_tbl.program & 0x1F) == prog_id) {
                    prog_selection_cam_tbl.program = (prog_selection_cam_tbl.program & 0x1F) | (prog_id<<5) | (1<<10);
                    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, i, &prog_selection_cam_tbl);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                }
            }
        }
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_update_programs_pem_contex", 0, 0);
}


STATIC uint32 arad_pp_flp_set_programs_name_space(int unit)
{
    int prog_id;
    uint32 res;
    int nof_flp_hw_programs = SOC_DPP_DEFS_GET(unit, nof_flp_programs);
    uint64 hw_prog_bitmap;
    uint8 app_id;
    int ns;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_hw_prog_id_bitmap.get(unit,&hw_prog_bitmap);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    for (ns = ARAD_PP_FLP_PROG_ETH; ns < ARAD_PP_FLP_PROG_NOF; ns++) {
        res = sw_state_access[unit].dpp.soc.arad.pp.fec.name_space.count.set(unit, ns, 0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);  
        for (prog_id = 0; prog_id < MAX_PROG_IN_NS; prog_id++) {
            res = sw_state_access[unit].dpp.soc.arad.pp.fec.name_space.hw_prog.set(unit, ns, prog_id, INVALID_FLP_HW_PROG);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
    }

    for (prog_id = 0; prog_id < nof_flp_hw_programs; prog_id++) {
        if (!COMPILER_64_BITTEST(hw_prog_bitmap, prog_id)) {
            continue;
        }
        
        res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

        res = arad_pp_flp_program_assigne_namespace(unit, app_id, prog_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_set_program_name_space", 0, 0);
}



uint32 
    arad_pp_flp_program_get_namespace_info(
        int unit,
        int ns_id,
        ARAD_PP_FLP_PROG_NS_INFO *ns_info)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(ns_info);

    res = sw_state_access[unit].dpp.soc.arad.pp.fec.name_space.get(unit, ns_id, ns_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_program_get_namespace_info", 0, 0);
}


uint32
   arad_pp_flp_process_init(
     int unit
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  uint32
    addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&flp_process_tbl, ARAD_PP_IHB_FLP_PROCESS_TBL_DATA, 1);

  flp_process_tbl.include_lem_2nd_in_result_a     = 0;
  flp_process_tbl.include_tcam_in_result_a       = 0;
  flp_process_tbl.include_lpm_2nd_in_result_a     = 0;
  flp_process_tbl.include_lem_1st_in_result_a     = 0;
  flp_process_tbl.include_lpm_1st_in_result_a     = 0;
  flp_process_tbl.select_default_result_a       = 3;
  flp_process_tbl.include_elk_ext_in_result_b     = 0;
  flp_process_tbl.include_tcam_in_result_b       = 0;
  flp_process_tbl.include_lpm_1st_in_result_b     = 0;
  flp_process_tbl.include_lem_2nd_in_result_b     = 0;
  flp_process_tbl.include_lpm_2nd_in_result_b     = 0;
  flp_process_tbl.select_default_result_b       = 3;
  flp_process_tbl.apply_fwd_result_a            = 1;
  flp_process_tbl.not_found_snoop_strength      = 0;
  flp_process_tbl.not_found_trap_strength       = 7;
  flp_process_tbl.not_found_trap_code           = 0;
  flp_process_tbl.result_b_format             = 0;
  flp_process_tbl.result_a_format             = 0;
  flp_process_tbl.include_lem_1st_in_result_b     = 0;
  flp_process_tbl.include_elk_fwd_in_result_b     = 0;
  flp_process_tbl.include_elk_ext_in_result_a     = 0;
  flp_process_tbl.include_elk_fwd_in_result_a     = 0;
  flp_process_tbl.elk_result_format            = 0;
  flp_process_tbl.sa_lkp_result_select          = 0;
  flp_process_tbl.sa_lkp_process_enable         = 1;
  flp_process_tbl.procedure_ethernet_default   = 0;
  flp_process_tbl.unknown_address             = 0;
  flp_process_tbl.enable_hair_pin_filter        = 0;
  flp_process_tbl.enable_rpf_check             = 0;
  flp_process_tbl.compatible_mc_bridge_fallback = 0;
  flp_process_tbl.enable_lsr_p2p_service        = 0;
  flp_process_tbl.learn_enable                = 0;
  flp_process_tbl.fwd_processing_profile       = 0;
  for(addr = 0; addr < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++addr)
  {
    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, addr, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
  }

#ifdef BCM_88660_A0
#if defined(INCLUDE_KBP)
    if (SOC_IS_ARADPLUS(unit)) {
        uint32
            fld_val,
            mem_val=0;

        
        fld_val = 0x7; 
        soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_FOUND_OFFSETf, &fld_val);
        fld_val = 0x9; 
        soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_DATA_OFFSETf, &fld_val);
        fld_val = 0x7; 
        soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_DATA_FORMATf, &fld_val);
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0)) {
            
			fld_val = 0x6; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_FOUND_OFFSETf, &fld_val);
            fld_val = 0x7; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_OFFSETf, &fld_val);
        }
        else {
            
            fld_val = 0x5; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_FOUND_OFFSETf, &fld_val);
            fld_val = 0x3; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_OFFSETf, &fld_val);
        }
        fld_val = 0x2; 
        soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_FORMATf, &fld_val);

        for(addr = 0; addr < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++addr)
        {
            res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, addr, &mem_val);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101 + addr, exit);
        }

		if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
			
			fld_val = 0x6; 
			soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_FOUND_OFFSETf, &fld_val);
			fld_val = 0x7; 
			soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_OFFSETf, &fld_val);
			res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, PROG_FLP_IPV4COMPMC_WITH_RPF, &mem_val);
			SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101 + addr, exit);
		}

        if(ARAD_KBP_ENABLE_IPV6_EXTENDED) {
            
            fld_val = 0x7; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_FOUND_OFFSETf, &fld_val);
            fld_val = 0x9; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_DATA_OFFSETf, &fld_val);
            fld_val = 0x7; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, FWD_DATA_FORMATf, &fld_val);

            fld_val = 0x6; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_FOUND_OFFSETf, &fld_val);
            fld_val = 0x0; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_OFFSETf, &fld_val);

            fld_val = 0x2; 
            soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_FORMATf, &fld_val);

            res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, PROG_FLP_IPV6UC, &mem_val);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101 + addr, exit);
		}

		if(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED)
		{
			
			fld_val = 0x2; 
			soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_DATA_OFFSETf, &fld_val);
			fld_val = 0x6; 
			soc_mem_field_set(unit, IHP_ELK_PAYLOAD_FORMATm, &mem_val, EXT_FOUND_OFFSETf, &fld_val);

			res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, PROG_FLP_LSR, &mem_val);
			SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101 + addr, exit);

			res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, PROG_FLP_IPV4UC_RPF, &mem_val);
			SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101 + addr, exit);

			res = WRITE_IHP_ELK_PAYLOAD_FORMATm(unit, MEM_BLOCK_ANY, PROG_FLP_IPV4UC, &mem_val);
			SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101 + addr, exit);
		}
    }

#endif 
#endif 

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_init", 0, 0);
}
uint32
   arad_pp_flp_lookups_initial_programs(
     int unit,
     uint8 ingress_learn_enable,
     uint8 ingress_learn_oppurtunistic
   )
{
    uint32 res=0;
    uint8
        mac_in_mac_enabled,
        slb_enabled = 0,
        sa_auth_enabled = 0,
        early_bfd_init = 0;

#if defined(INCLUDE_L3)
    uint8 is_routing_enabled;
    int32 ipv6mc_cam_sel_id;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
    slb_enabled = 1;
  }

  if (soc_property_get(unit, spn_SA_AUTH_ENABLED, 0)) {
    sa_auth_enabled = 1;
  }

  
  if((SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) ||
     (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0))) {
      early_bfd_init = 1;
  }
  
  if (SOC_DPP_MYMAC_IP_DISABLED_TRAP_IS_ENABLED(unit)) {
      if (early_bfd_init == 1) {
          int32 prog_id;
          res = arad_pp_flp_prog_sel_mymac_ip_disabled_trap(unit, &prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          res = arad_pp_flp_process_mymac_ip_disabled_trap(unit, prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
      } else {
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("mymac and IP disabled trap cannot working without early bfd init mode.\n")));
      }
  }

  
  
  
  if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.explicit_null_support) {
      int prog_id = 0;
      res = arad_pp_flp_prog_sel_cam_mpls_exp_null_ttl_reject(unit, &prog_id); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 132, exit);
      res = arad_pp_flp_process_mpls_exp_null_ttl_reject(unit,prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);
  }

  
  res = arad_pp_is_mac_in_mac_enabled(
      unit,
      &mac_in_mac_enabled
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0) &&
      (
       (mac_in_mac_enabled) ||
       (SOC_DPP_CONFIG(unit)->pp.fcoe_enable) ||
       (SOC_DPP_CONFIG(unit)->l3.nof_rps != 0) ||
       ARAD_PP_FLP_PON_PROG_LOAD() ||
       (SOC_DPP_CONFIG(unit)->trill.mode)
      )) {
            const char *program = NULL;
            if (mac_in_mac_enabled) {
              program = "mac-in-mac";
            } else if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable) {
              program = "FCoE";
            } else if (SOC_DPP_CONFIG(unit)->l3.nof_rps != 0) {
              program = "bidirectional";
            } else if (ARAD_PP_FLP_PON_PROG_LOAD()) {
              program = "PON";
            } else if (SOC_DPP_CONFIG(unit)->trill.mode) {
              program = "TRILL";
            } else {
              SOC_SAND_VERIFY(FALSE);
            }

            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("%s cannot be used with SLB.\n"), program));

  }
#endif 

  if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
      int prog_id;
      arad_pp_flp_prog_sel_cam_oam_outlif_l2g(unit, &prog_id);
      arad_pp_flp_dbal_oam_outlif_l2g_program_tables_init(unit, prog_id);
      arad_pp_flp_process_oam_outlif_l2g(unit, prog_id);
  }

  if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "two_pass_mc_dest_flow", 0)) {
      int program_id = 0;
      res = arad_pp_flp_prog_sel_cam_two_pass_mc_encap_to_dest(unit, &program_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
      res = arad_pp_flp_dbal_two_pass_mc_encap_to_dest_program_tables_init(unit, program_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      res = arad_pp_flp_process_two_pass_mc_encap_to_dest(unit, program_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  }

  res = arad_pp_flp_prog_sel_cam_key_program_tm(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  res = arad_pp_flp_process_key_program_tm(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  res = arad_pp_flp_process_tm(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  
    if (ARAD_PP_FLP_PON_PROG_LOAD()) {
        if (SOC_DPP_L3_SRC_BIND_IPV4_ENABLE(unit)) {
            
            res = arad_pp_flp_prog_sel_cam_ethernet_tk_epon_uni_v4(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            res = arad_pp_flp_key_const_ethernet_tk_epon_uni_v4_dhcp(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
            res = arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_dhcp(unit, sa_auth_enabled, slb_enabled);
            SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

            if (SOC_IS_JERICHO(unit) && SOC_DPP_L3_SRC_BIND_IPV4_OR_ARP_ENABLE(unit)) {
                res = arad_pp_flp_dbal_pon_ipv4_sav_static_program_tables_init(unit, sa_auth_enabled, slb_enabled);
                SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
            } else {
                res = arad_pp_flp_key_const_ethernet_tk_epon_uni_v4_static(unit);
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

                res = arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_static(unit, sa_auth_enabled, slb_enabled);
                SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
            }
            res = arad_pp_flp_process_ethernet_tk_epon_uni_v4(unit, TRUE,sa_auth_enabled, slb_enabled);
            SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
        }

        if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit)) {
            
            res = arad_pp_flp_prog_sel_cam_ethernet_tk_epon_uni_v6(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            if (SOC_IS_JERICHO(unit)){
                res = arad_pp_flp_dbal_pon_ipv6_sav_static_program_tables_init(unit,sa_auth_enabled, slb_enabled,ARAD_TCAM_ACCESS_PROFILE_INVALID);
                SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
            }
            res = arad_pp_flp_key_const_ethernet_tk_epon_uni_v6(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
            res = arad_pp_flp_lookups_ethernet_tk_epon_uni_v6(unit, ARAD_TCAM_ACCESS_PROFILE_INVALID, sa_auth_enabled, slb_enabled);
            SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);
            res = arad_pp_flp_process_ethernet_tk_epon_uni_v6(unit, TRUE,sa_auth_enabled, slb_enabled);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        }

        if (SOC_DPP_L3_SRC_BIND_ARP_RELAY_ENABLE(unit)) {
            res = arad_pp_flp_pon_arp_prog_init(unit, sa_auth_enabled, slb_enabled);
            SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
        }
    }

  if (SOC_DPP_PPPOE_IS_ENABLE(unit)) {
      if (SOC_DPP_L3_SRC_BIND_IPV4_ENABLE(unit)) {
          
          res = arad_pp_flp_prog_sel_pppoe_ipv4(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          res = arad_pp_flp_dbal_pppoe_ipv4uc_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
          res = arad_pp_flp_process_pppoe_ipv4(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
      }

      if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit)) {
          
          res = arad_pp_flp_prog_sel_pppoe_ipv6(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          res = arad_pp_flp_dbal_pppoe_ipv6uc_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
          res = arad_pp_flp_process_pppoe_ipv6(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
  }


  if (mac_in_mac_enabled) {

      res = arad_pp_flp_prog_sel_cam_ethernet_mac_in_mac(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      res = arad_pp_flp_key_const_ethernet_mac_in_mac(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 83, exit);
      res = arad_pp_flp_lookups_ethernet_mac_in_mac(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);
      res = arad_pp_flp_process_ethernet_mac_in_mac(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

  res = arad_pp_flp_prog_sel_cam_p2p(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
  res = arad_pp_flp_key_const_p2p(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  res = arad_pp_flp_lookups_p2p(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
  res = arad_pp_flp_process_p2p(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  if (!ARAD_PP_FLP_PON_PROG_LOAD()) {
      
      if (soc_property_get(unit, spn_VPWS_TAGGED_MODE, 0)) {
          res = arad_pp_flp_prog_sel_cam_vpws_tagged(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);
          res = arad_pp_flp_dbal_vpws_tagged_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
          res = arad_pp_flp_process_vpws_tagged(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);
      }
  }

  
  res = arad_pp_flp_prog_sel_cam_mpls_p2p(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);

  
  res = arad_pp_flp_prog_sel_cam_mpls_drop_unmatch_ces_packet(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 128, exit);

  if(early_bfd_init == 1) {
      arad_pp_flp_init_bfd_program(unit);
  }

#if defined(INCLUDE_L3)

  res = arad_pp_sw_db_ipv4_is_routing_enabled(unit, &is_routing_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);
  if(is_routing_enabled)
  {
      
      
      
#if defined(INCLUDE_KBP)
      if (!ARAD_KBP_ENABLE_IPV4_UC) {
#else
      {
#endif

    #if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
          if(JER_KAPS_ENABLE(unit) && SOC_DPP_CONFIG(unit)->pp.custom_ip_route){
              int prog_id = 0;
              
              res = arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn_custom_prgrm(unit,&prog_id); 
              SOC_SAND_CHECK_FUNC_RESULT(res, 135, exit);
              res = arad_pp_flp_dbal_ipv4uc_l3vpn_custom_program_tables_init(unit,prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
              res = arad_pp_flp_process_ipv4uc_l3vpn(unit,prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);

              
              res = arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn_custom_prgrm(unit,&prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
              res = arad_pp_flp_dbal_ipv6uc_l3vpn_program_tables_init(unit,prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 151, exit);
              res = arad_pp_flp_process_ipv6uc_l3vpn(unit,prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);
          }
    #endif
      }

      
      
      
#if defined(INCLUDE_KBP)
      if(!ARAD_KBP_IS_IN_USE || (ARAD_KBP_IS_IN_USE && ARAD_KBP_ENABLE_IPV4_UC_PUBLIC && SOC_IS_JERICHO(unit)))
#endif
      {
          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)){
              res = arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn(unit, 1);
              SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);
          }
          res = arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn(unit, 0);
          SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);
      }
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      if(JER_KAPS_ENABLE(unit)) {
          res = arad_pp_flp_dbal_ipv4uc_l3vpn_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
      }
      else
#endif
     {
          res = arad_pp_flp_key_const_ipv4uc_l3vpn(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 175, exit);
          res = arad_pp_flp_lookups_ipv4uc_l3vpn(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
      }
      res = arad_pp_flp_process_ipv4uc_l3vpn(unit,0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 185, exit);

      
      
      
      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)){
           
           res = arad_pp_flp_prog_sel_cam_ipv4uc_with_rpf(unit, 1);
           SOC_SAND_CHECK_FUNC_RESULT(res, 203, exit);
      }
      res = arad_pp_flp_prog_sel_cam_ipv4uc_with_rpf(unit, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 205, exit);
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      if((JER_KAPS_ENABLE(unit)) || (ARAD_KBP_ENABLE_IPV4_RPF && SOC_IS_JERICHO(unit))) {
          res = arad_pp_flp_dbal_ipv4uc_rpf_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
      }
      else
#endif
      {
          res = arad_pp_flp_key_const_ipv4uc_rpf(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 215, exit);
          res = arad_pp_flp_lookups_ipv4uc_rpf(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
      }
      res = arad_pp_flp_process_ipv4uc_rpf(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);

      
      
      
      res = arad_pp_flp_prog_sel_cam_ipv4uc(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 245, exit);
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      if(JER_KAPS_ENABLE(unit) || (ARAD_KBP_ENABLE_IPV4_UC && SOC_IS_JERICHO(unit))){
          res = arad_pp_flp_dbal_ipv4uc_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
      }
      else
#endif
      {
          res = arad_pp_flp_key_const_ipv4uc(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 255, exit);
          res = arad_pp_flp_lookups_ipv4uc(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
      }
      res = arad_pp_flp_process_ipv4uc(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 265, exit);

      
      
      
#if defined(INCLUDE_KBP)
      if (SOC_IS_JERICHO(unit) && ARAD_KBP_ENABLE_IPV4_DC) {
          res = arad_pp_flp_prog_sel_cam_ipv4_dc(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
          res = arad_pp_flp_dbal_ipv4_dc_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 275, exit);
          res = arad_pp_flp_process_ipv4_dc(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
      }
#endif

      
      if ((SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile == 1)) {
          
          int32 ivl_prog_id;
          
          res = arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_inner_learn(unit, &ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 285, exit);
          
          res = arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_inner_learn(unit, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
          
          res = arad_pp_dbal_flp_ethernet_ing_ivl_inner_learn_tables_create(unit, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 295, exit);
          
          res = arad_pp_flp_process_ethernet_ing_all_ivl_learn(unit, TRUE, sa_auth_enabled, slb_enabled, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

          
          ivl_prog_id = 0;
          
          res = arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_fwd_outer_learn(unit, &ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 305, exit);
          
          
          res = arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_fwd_outer_learn(unit, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);
          
          res = arad_pp_dbal_flp_ethernet_ing_ivl_fwd_outer_learn_tables_create(unit, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 315, exit);
          
          res = arad_pp_flp_process_ethernet_ing_all_ivl_learn(unit, TRUE, sa_auth_enabled, slb_enabled, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);

          
          ivl_prog_id = 0;
          
          res = arad_pp_flp_prog_sel_cam_ethernet_ing_ivl_learn(unit, &ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 325, exit);
          
          res = arad_pp_flp_prog_sel_cam_ethernet_mc_ing_ivl_learn(unit, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
          
          res = arad_pp_dbal_flp_ethernet_ing_ivl_learn_tables_create(unit, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res,335, exit);

          
          res = arad_pp_flp_process_ethernet_ing_all_ivl_learn(unit, TRUE, sa_auth_enabled, slb_enabled, ivl_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 340, exit);
      }


    if (SOC_IS_ARADPLUS_A0(unit) && SOC_DPP_CONFIG(unit)->pp.oam_statistics) {
          int oam_statistics_prog_id;
          int bfd_statistics_prog_id;
          int oam_down_statistics_prog_id;
          int oam_single_tag_statistics_prog_id;
          int oam_double_tag_statistics_prog_id;
          int bfd_mpls_statistics_prog_id;
          int bfd_pwe_statistics_prog_id;

          
          res = arad_pp_flp_prog_sel_cam_oam_statistics(unit, &oam_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 345, exit);
          res = arad_pp_flp_dbal_oam_statistics_program_tables_init(unit, oam_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);
          res = arad_pp_flp_process_oam_statistics(unit, oam_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 355, exit);

          
          res = arad_pp_flp_prog_sel_cam_bfd_statistics(unit, &bfd_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
          res = arad_pp_flp_dbal_bfd_statistics_program_tables_init(unit, bfd_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 365, exit);
          res = arad_pp_flp_process_bfd_statistics(unit, bfd_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

          
          res = arad_pp_flp_prog_sel_cam_oam_down_untagged_statistics(unit, &oam_down_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 375, exit);
          res = arad_pp_flp_dbal_oam_down_untagged_statistics_program_tables_init(unit, oam_down_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 380, exit);
          res = arad_pp_flp_process_oam_statistics(unit, oam_down_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 385, exit);

          
          res = arad_pp_flp_prog_sel_cam_oam_single_tag_statistics(unit, &oam_single_tag_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);
          res = arad_pp_flp_dbal_oam_single_tag_statistics_program_tables_init(unit, oam_single_tag_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 395, exit);
          res = arad_pp_flp_process_oam_statistics(unit, oam_single_tag_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);

          
          res = arad_pp_flp_prog_sel_cam_oam_double_tag_statistics(unit, &oam_double_tag_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 405, exit);
          res = arad_pp_flp_dbal_oam_double_tag_statistics_program_tables_init(unit, oam_double_tag_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);
          res = arad_pp_flp_process_oam_statistics(unit, oam_double_tag_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 415, exit);

          
          res = arad_pp_flp_prog_sel_cam_bfd_mpls_statistics(unit, &bfd_mpls_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);
          res = arad_pp_flp_dbal_bfd_mpls_statistics_program_tables_init(unit, bfd_mpls_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 425, exit);
          res = arad_pp_flp_process_oam_statistics(unit, bfd_mpls_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);

          
          res = arad_pp_flp_prog_sel_cam_bfd_pwe_statistics(unit, &bfd_pwe_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 435, exit);
          res = arad_pp_flp_dbal_bfd_pwe_statistics_program_tables_init(unit, bfd_pwe_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 440, exit);
          res = arad_pp_flp_process_oam_statistics(unit, bfd_pwe_statistics_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 445, exit);
      }


      
      if (!ARAD_PP_FLP_PON_PROG_LOAD() && !soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {

          res = arad_pp_flp_prog_sel_cam_ipv4mc_bridge_v4mc(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

          arad_pp_flp_prog_sel_cam_ipv4mc_bridge(unit,0x0,NULL);
          SOC_SAND_CHECK_FUNC_RESULT(res, 455, exit);

          if(SOC_IS_ARADPLUS(unit)) {
              res = arad_pp_flp_dbal_ipv4mc_bridge_program_tables_init(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);
          } else {
              res = arad_pp_flp_key_const_ipv4mc_bridge(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 465, exit);
              res = arad_pp_flp_lookups_ipv4mc_bridge(unit, sa_auth_enabled, slb_enabled);
              SOC_SAND_CHECK_FUNC_RESULT(res, 470, exit);
          }
          res = arad_pp_flp_process_ipv4mc_bridge(unit,TRUE,sa_auth_enabled, slb_enabled);
          SOC_SAND_CHECK_FUNC_RESULT(res, 475, exit);
      }


      
      
      
      if(SOC_DPP_CONFIG(unit)->l3.nof_rps != 0){
          res = arad_pp_flp_ipmc_bidir_progs_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 480, exit);
      }

      
      
      

      if (SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)){
          int prog_id;
          
          res = arad_pp_flp_prog_sel_cam_ipv4compmc_with_mim_learning(unit, &prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 485, exit);

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
          if((JER_KAPS_ENABLE(unit)) || (ARAD_KBP_ENABLE_IPV4_MC && SOC_IS_JERICHO(unit))){
              res = arad_pp_flp_dbal_ipv4compmc_with_mim_learning_program_tables_init(unit, prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);
          }
          else
#endif
          {
              res = arad_pp_flp_key_const_ipv4compmc_with_mim_learning(unit, prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 495, exit);
              res = arad_pp_flp_lookups_ipv4compmc_with_mim_learning(unit, ARAD_TCAM_ACCESS_PROFILE_INVALID, prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);
              
              res = arad_pp_flp_instruction_rsrc_set(unit, prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 505, exit);
              res = arad_pp_flp_dbal_ipv4mc_tcam_tables_init(unit, prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 510, exit);
          }
          res = arad_pp_flp_process_ipv4compmc_with_mim_learning(unit, prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 520, exit);
      }

      
      
      
      if (SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) {
          
          res = arad_pp_flp_prog_sel_cam_ipv4compmc_with_rpf(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 485, exit);

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
          if((JER_KAPS_ENABLE(unit)) || (ARAD_KBP_ENABLE_IPV4_MC && SOC_IS_JERICHO(unit))){
              res = arad_pp_flp_dbal_ipv4compmc_with_rpf_program_tables_init(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);
          }
          else
#endif
          {
              res = arad_pp_flp_key_const_ipv4compmc_with_rpf(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 495, exit);
              res = arad_pp_flp_lookups_ipv4compmc_with_rpf(unit, ARAD_TCAM_ACCESS_PROFILE_INVALID);
              SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);
              
              res = arad_pp_flp_instruction_rsrc_set(unit, PROG_FLP_IPV4COMPMC_WITH_RPF);
              SOC_SAND_CHECK_FUNC_RESULT(res, 505, exit);
              res = arad_pp_flp_dbal_ipv4mc_tcam_tables_init(unit, -1);
              SOC_SAND_CHECK_FUNC_RESULT(res, 510, exit);
          }
          res = arad_pp_flp_process_ipv4compmc_with_rpf(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 520, exit);
      }

      
      
      
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit))
      {
          int program_id_ipv6mc_ssm_compress_sip = 0;
          int program_id_ipv6mc_ssm = 0;
          res = arad_pp_flp_prog_sel_cam_ipv6mc_ssm(unit, &program_id_ipv6mc_ssm);
          SOC_SAND_CHECK_FUNC_RESULT(res, 600, exit);
          res = arad_pp_flp_dbal_ipv6mc_ssm_program_tables_init(unit, program_id_ipv6mc_ssm);
          SOC_SAND_CHECK_FUNC_RESULT(res, 610, exit);
          res = arad_pp_flp_process_ipv6mc_ssm(unit, program_id_ipv6mc_ssm);
          SOC_SAND_CHECK_FUNC_RESULT(res, 620, exit);
          res = arad_pp_flp_prog_sel_cam_ipv6mc_ssm_compress_sip(unit, &program_id_ipv6mc_ssm_compress_sip);
          SOC_SAND_CHECK_FUNC_RESULT(res, 630, exit);
          res = arad_pp_flp_dbal_ipv6mc_ssm_compress_sip_program_tables_init(unit, program_id_ipv6mc_ssm_compress_sip);
          SOC_SAND_CHECK_FUNC_RESULT(res, 640, exit);
          res = arad_pp_flp_process_ipv6mc_ssm_compress_sip(unit, program_id_ipv6mc_ssm_compress_sip);
          SOC_SAND_CHECK_FUNC_RESULT(res, 650, exit);
      }
#endif
      res = arad_pp_flp_prog_sel_cam_ipv6mc(unit, &ipv6mc_cam_sel_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 570, exit);
      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipv6_mc_forwarding_disable", 0))
      {
          ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;
          res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, ipv6mc_cam_sel_id, &flp_process_tbl);
          SOC_SAND_CHECK_FUNC_RESULT(res, 575, exit);
          flp_process_tbl.not_found_trap_strength = 0;
          flp_process_tbl.not_found_trap_code = SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6;
          res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, ipv6mc_cam_sel_id, &flp_process_tbl);
          SOC_SAND_CHECK_FUNC_RESULT(res, 580, exit);
      }
      else {
#if defined(INCLUDE_KBP)
          if(ARAD_KBP_ENABLE_IPV6_MC){
              if (SOC_IS_JERICHO(unit)) {
                  res = arad_pp_flp_dbal_ipv6mc_program_tables_init(unit);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 585, exit);
              }
              else{
                  res = arad_pp_flp_key_const_ipv6mc(unit);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 590, exit);
                  res = arad_pp_flp_lookups_ipv6mc(unit,ARAD_TCAM_ACCESS_PROFILE_INVALID);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 595, exit);
              }
          }
          else
#endif
          {
              res = arad_pp_flp_dbal_ipv6mc_program_tables_init(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 560, exit);
          }
          res = arad_pp_flp_process_ipv6mc(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 565, exit);
      }

      
      
      
      res = arad_pp_flp_prog_sel_cam_lsr(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 570, exit);
#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
          res = arad_pp_flp_key_const_lsr(unit, FALSE, FALSE, FALSE);
          SOC_SAND_CHECK_FUNC_RESULT(res, 575, exit);
          res = arad_pp_flp_lookups_lsr(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 580, exit);
      }
#endif
      res = arad_pp_flp_dbal_lsr_program_tables_init(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 585, exit);
      res = arad_pp_flp_process_lsr(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 590, exit);

      
      if (SOC_DPP_CONFIG(unit)->trill.mode)
      {
            res = arad_pp_flp_prog_sel_cam_TRILL_uc(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 575, exit);
            res = arad_pp_flp_key_const_TRILL_uc(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 580, exit);
            res = arad_pp_flp_lookups_TRILL_uc(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 585, exit);
            res = arad_pp_flp_process_TRILL_uc(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 590, exit);

          if (SOC_IS_ARADPLUS_AND_BELOW(unit))
          {
                res = arad_pp_flp_prog_sel_cam_TRILL_mc_after_recycle_overlay(unit);
                SOC_SAND_CHECK_FUNC_RESULT(res, 595, exit);
                res = arad_pp_flp_key_const_TRILL_mc_after_recycle_overlay(unit);
                SOC_SAND_CHECK_FUNC_RESULT(res, 600, exit);
                res = arad_pp_flp_lookups_TRILL_mc_after_recycle_overlay(unit, sa_auth_enabled, slb_enabled);
                SOC_SAND_CHECK_FUNC_RESULT(res, 605, exit);
                res = arad_pp_flp_process_TRILL_mc_after_recycle_overlay(unit,TRUE,sa_auth_enabled, slb_enabled);
                SOC_SAND_CHECK_FUNC_RESULT(res, 610, exit);
          }

            res = arad_pp_flp_prog_sel_cam_TRILL_mc(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 615, exit);
            res = arad_pp_flp_key_const_TRILL_mc(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 620, exit);
            res = arad_pp_flp_lookups_TRILL_mc(unit,ingress_learn_enable,ARAD_TCAM_ACCESS_PROFILE_INVALID);
            SOC_SAND_CHECK_FUNC_RESULT(res, 625, exit);
          if (SOC_DPP_CONFIG(unit)->trill.transparent_service){
                res = arad_pp_flp_dbal_trill_program_tcam_tables_init(unit);
                SOC_SAND_CHECK_FUNC_RESULT(res, 630, exit);
          }
            res = arad_pp_flp_process_TRILL_mc(unit,ingress_learn_enable);
            SOC_SAND_CHECK_FUNC_RESULT(res, 635, exit);
      }

      

      if (SOC_IS_ARADPLUS_AND_BELOW(unit) && (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE)) {
          res = arad_pp_flp_pwe_gre_progs_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 1573, exit);
      }

      if (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
          int prog_id_untagged = 0x0;
          int prog_id = 0x0;
          int user_mode_untagged = 2;
          int user_mode = 1;

          
          arad_pp_flp_prog_sel_cam_ipv4mc_bridge(unit,user_mode_untagged, &prog_id_untagged);  

          arad_pp_flp_prog_sel_cam_ipv4mc_bridge(unit,user_mode, &prog_id);  

          arad_pp_flp_dbal_ipv4mc_bridge_program_tables_init_ivl(unit,prog_id_untagged,prog_id); 

          arad_pp_flp_process_ipv4mc_bridge_ivl(unit,prog_id_untagged); 
          arad_pp_flp_process_ipv4mc_bridge_ivl(unit,prog_id);
      }

      
      
      
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      if (SOC_IS_JERICHO(unit)) {
          
          if(!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_ipv6_uc_use_tcam", 0)) {
              int prog_id;
              res = arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf(unit,&prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 660, exit);

              if (JER_KAPS_ENABLE(unit) || ARAD_KBP_ENABLE_IPV6_RPF) {
                  res = arad_pp_flp_dbal_ipv6uc_with_rpf_program_tables_init(unit,prog_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 665, exit);
              }
              res = arad_pp_flp_process_ipv6uc_with_rpf(unit, prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 670, exit);
          }
      }
      else
#endif
#if defined(INCLUDE_KBP)
      {
          if(ARAD_KBP_ENABLE_IPV6_RPF){
              
              if (!SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists)
              {
                  
                  res = arad_pp_flp_ipv6uc_with_rpf_prog_init(unit);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
              }
          }
      }
#endif

      
      
      
      res = arad_pp_flp_prog_sel_cam_ipv6uc(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 540, exit);

#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_IPV6_UC){
          if (SOC_IS_JERICHO(unit)) {
              res = arad_pp_flp_dbal_ipv6uc_program_tables_init(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 545, exit);
          }
          else{
              res = arad_pp_flp_key_const_ipv6uc(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 550, exit);
              res = arad_pp_flp_lookups_ipv6uc(unit,ARAD_TCAM_ACCESS_PROFILE_INVALID);
              SOC_SAND_CHECK_FUNC_RESULT(res, 555, exit);
          }
      }
      else
#endif
      {
          res = arad_pp_flp_dbal_ipv6uc_program_tables_init(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 560, exit);
      }
      res = arad_pp_flp_process_ipv6uc(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 565, exit);

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      
      
      
      if ((!ARAD_KBP_IS_IN_USE && JER_KAPS_ENABLE(unit)) ||
          (ARAD_KBP_IS_IN_USE && ARAD_KBP_ENABLE_IPV4_RPF_PUBLIC && SOC_IS_JERICHO(unit)))
      {
          int prog_id;
          
          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)){
              arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn_rpf(unit, &prog_id, 1);
              SOC_SAND_CHECK_FUNC_RESULT(res, 695, exit);
          }
          res = arad_pp_flp_prog_sel_cam_ipv4uc_l3vpn_rpf(unit, &prog_id, 0);
          SOC_SAND_CHECK_FUNC_RESULT(res, 697, exit);
          res = arad_pp_flp_dbal_ipv4uc_l3vpn_rpf_program_tables_init(unit, prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 700, exit);
          res = arad_pp_flp_process_ipv4uc_l3vpn_rpf(unit, prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 705, exit);
      }
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      
      
      
      if ((JER_KAPS_ENABLE(unit) && !ARAD_KBP_ENABLE_IPV6_UC)
           || (ARAD_KBP_IS_IN_USE && ARAD_KBP_ENABLE_IPV6_UC_PUBLIC && SOC_IS_JERICHO(unit))) {
          int prog_id;
          arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn(unit,&prog_id);
          arad_pp_flp_dbal_ipv6uc_l3vpn_program_tables_init(unit,prog_id);
          arad_pp_flp_process_ipv6uc_l3vpn(unit,prog_id);
      }
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
      
      
      
      if ((JER_KAPS_ENABLE(unit) && !ARAD_KBP_ENABLE_IPV6_RPF)
            || (ARAD_KBP_IS_IN_USE && ARAD_KBP_ENABLE_IPV6_RPF_PUBLIC && SOC_IS_JERICHO(unit))) {
          int prog_id;
          arad_pp_flp_prog_sel_cam_ipv6uc_l3vpn_rpf(unit, &prog_id);
          arad_pp_flp_dbal_ipv6uc_l3vpn_rpf_program_tables_init(unit, prog_id);
          arad_pp_flp_process_ipv6uc_l3vpn_rpf(unit, prog_id);
      }
#endif

#if defined(INCLUDE_KBP)
      
      if(ARAD_KBP_ENABLE_IPV6_EXTENDED){
          
          if (SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists)
          {
              int32 prog_id;
              res = arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf_2pass(unit,&prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 710, exit);
              res = arad_pp_flp_key_const_ipv6uc_with_rpf_2pass(unit,prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 715, exit);
              res = arad_pp_flp_lookups_ipv6uc_with_rpf_2pass(unit,prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 720, exit);
              res = arad_pp_flp_process_ipv6uc_with_rpf_2pass(unit,prog_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 725, exit);
              res = arad_pp_flp_prog_sel_cam_ipv6uc_with_rpf_2pass_fwd(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 730, exit);
          }
      }
#endif 

  }
#endif 

  if (!(SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable)) {
      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
          int gpon_l2_ip4mc_prog_id;
          int gpon_l2_ip6mc_prog_id;

         
          res = arad_pp_flp_prog_sel_cam_gpon_l2_ip4mc(unit, &gpon_l2_ip4mc_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 920, exit);

          res = arad_pp_flp_dbal_gpon_l2_ip4mc_tables_init(unit, gpon_l2_ip4mc_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 925, exit);

          res = arad_pp_flp_process_gpon_l2_ip4mc(unit, gpon_l2_ip4mc_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 930, exit);

         
          res = arad_pp_flp_prog_sel_cam_gpon_l2_ip6mc(unit, &gpon_l2_ip6mc_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 920, exit);

          res = arad_pp_flp_dbal_gpon_l2_ip6mc_tables_init(unit, gpon_l2_ip6mc_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 925, exit);

          res = arad_pp_flp_process_gpon_l2_ip6mc(unit, gpon_l2_ip6mc_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 930, exit);
      } else {
          int program_id = 0;
          
          res = arad_pp_flp_prog_sel_cam_global_ipv4compmc_with_rpf(unit, &program_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 735, exit);
          res = arad_pp_flp_key_const_global_ipv4compmc_with_rpf(unit, program_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 740, exit);
          res = arad_pp_flp_lookups_global_ipv4compmc_with_rpf(unit, program_id, ARAD_TCAM_ACCESS_PROFILE_INVALID);
          SOC_SAND_CHECK_FUNC_RESULT(res, 745, exit);
          
          res = arad_pp_flp_instruction_rsrc_set(unit, program_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 750, exit);
          res = arad_pp_flp_dbal_ipv4mc_tcam_tables_init(unit, -1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 755, exit);
          res = arad_pp_flp_process_global_ipv4compmc_with_rpf(unit, program_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 760, exit);
      }
  }

  if (ARAD_PP_FLP_PON_PROG_LOAD()) {
      int32 prog_id;

      if (SOC_DPP_CONFIG(unit)->pp.local_switching_enable) {
          
          res = arad_pp_flp_prog_sel_cam_ethernet_pon_local_route(unit, mac_in_mac_enabled, &prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 765, exit);
          res = arad_pp_flp_key_const_ethernet_pon_local_route(unit, prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 770, exit);
          res = arad_pp_flp_lookups_ethernet_pon_local_route(unit, sa_auth_enabled, slb_enabled, prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 775, exit);
          res = arad_pp_flp_process_ethernet_pon_local_route(unit, TRUE,sa_auth_enabled, slb_enabled, prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 780, exit);
      }

      
      if (ARAD_PP_FLP_IBC_PORT_ENABLE(unit)) {
          res = arad_pp_flp_prog_sel_cam_ethernet_ing_learn(unit, 0, &prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 825, exit);
          res = arad_pp_flp_key_const_ethernet_ing_learn(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 830, exit);
          res = arad_pp_flp_lookups_ethernet_ing_learn(unit, ingress_learn_enable, sa_auth_enabled, slb_enabled);
          SOC_SAND_CHECK_FUNC_RESULT(res, 835, exit);
          res = arad_pp_flp_process_ethernet_ing_learn(unit, TRUE,sa_auth_enabled, slb_enabled);
          SOC_SAND_CHECK_FUNC_RESULT(res, 840, exit);
      }

      
      res = arad_pp_flp_prog_sel_cam_ethernet_pon_default_upstream(unit, &prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 785, exit);
      res = arad_pp_flp_key_const_ethernet_pon_default_upstream(unit, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 790, exit);
      res = arad_pp_flp_lookups_ethernet_pon_default_upstream(unit, sa_auth_enabled, slb_enabled, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 795, exit);
      res = arad_pp_flp_process_ethernet_pon_default_upstream(unit, TRUE,sa_auth_enabled, slb_enabled, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 800, exit);
      
      res = arad_pp_flp_prog_sel_cam_ethernet_pon_default_downstream(unit, &prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 805, exit);
      res = arad_pp_flp_key_const_ethernet_pon_default_downstream(unit, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 810, exit);
      res = arad_pp_flp_lookups_ethernet_pon_default_downstream(unit, ARAD_TCAM_ACCESS_PROFILE_INVALID, sa_auth_enabled, slb_enabled, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 815, exit);
      res = arad_pp_flp_process_ethernet_pon_default_downstream(unit, TRUE,sa_auth_enabled, slb_enabled, prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 820, exit);
  } else {
      int32 mac_in_mac_prog_id;

      res = arad_pp_flp_prog_sel_cam_ethernet_ing_learn(unit, mac_in_mac_enabled, &mac_in_mac_prog_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 825, exit);
      res = arad_pp_flp_key_const_ethernet_ing_learn(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 830, exit);
      res = arad_pp_flp_lookups_ethernet_ing_learn(unit, ingress_learn_enable, sa_auth_enabled, slb_enabled);
      SOC_SAND_CHECK_FUNC_RESULT(res, 835, exit);
      res = arad_pp_flp_process_ethernet_ing_learn(unit, TRUE,sa_auth_enabled, slb_enabled);
      SOC_SAND_CHECK_FUNC_RESULT(res, 840, exit);

      if (mac_in_mac_enabled) {
          
          res = arad_pp_flp_key_const_mac_in_mac_after_termination(unit, mac_in_mac_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 845, exit);
          res = arad_pp_flp_lookups_mac_in_mac_after_termination(unit, mac_in_mac_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 850, exit);
          res = arad_pp_flp_process_mac_in_mac_after_termination(unit,TRUE, mac_in_mac_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 855, exit);
      }

      if(SOC_IS_JERICHO(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 1) == 0)){
          res = arad_pp_flp_prog_sel_cam_mpls_control_word_trap(unit, mac_in_mac_enabled, &mac_in_mac_prog_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 860, exit);
          res = arad_pp_flp_key_const_mpls_control_word_trap(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 830, exit);
          res = arad_pp_flp_lookups_mpls_control_word_trap(unit, ingress_learn_enable, sa_auth_enabled, slb_enabled);
          SOC_SAND_CHECK_FUNC_RESULT(res, 835, exit);
          res = arad_pp_flp_process_mpls_control_word_trap(unit, TRUE,sa_auth_enabled, slb_enabled);
          SOC_SAND_CHECK_FUNC_RESULT(res, 840, exit);
      }
  }

  
  if(SOC_DPP_CONFIG(unit)->pp.fcoe_enable){
      res = arad_pp_flp_fcoe_progs_init(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  }else
  {
      int32 prog_id;
      res = arad_pp_flp_prog_sel_cam_fcoe_ethernet_ing_learn(unit, &prog_id, FALSE);
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  }

  if (SOC_DPP_CONFIG(unit)->pp.vmac_enable) {
    res = arad_pp_flp_vmac_progs_init(unit, sa_auth_enabled, slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  }

  
  if(early_bfd_init == 0) {
      arad_pp_flp_init_bfd_program(unit);
  }

  
  res = arad_pp_flp_set_programs_name_space(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_initial_programs", 0, 0);
}


STATIC uint32
  arad_pp_flp_key_const_lpm_custom_lookup(
     int unit,
     int32 prog_id,
     uint32 db_indx
   )
{
    uint32
      res,
      indx,
      inst,
      inst_0_to_5_valid,
      inst_6_to_11_valid,
      *inst_valid;

    ARAD_FLP_CUSTOM_LPM_LOOKUP_INFO *lpm_lookup;
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
      flp_key_construction_tbl_0_5,
      flp_key_construction_tbl_6_11,
      *flp_key_construction;

    ARAD_FLP_CUSTOM_LPM_KEY_CONSTR *key;
    ARAD_FLP_CUSTOM_LPM_INSTRUCTION_INFO *inst_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    lpm_lookup = &Arad_flp_custom_lpm_lookup_info[db_indx];

    
    if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1){
        if (lpm_lookup->app_id == PROG_FLP_IPV4UC) {
            lpm_lookup->key[0].key =0;
            lpm_lookup->key[1].key = 0;
        }
    }


    for (indx=0; indx<lpm_lookup->lookup_num; ++indx) {
        key = &lpm_lookup->key[indx];

        if ( (indx == 0) || (key->key != lpm_lookup->key[0].key) ) { 
            res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl_0_5);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl_6_11);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            inst_0_to_5_valid = 0;
            inst_6_to_11_valid = 0;

            
            for (inst=0; inst<key->inst_num; ++inst) {
                inst_info = &key->inst[inst];

                if (inst_info->is_inst_6_11) {
                    flp_key_construction = &flp_key_construction_tbl_6_11;
                    inst_valid = &inst_6_to_11_valid;
                }
                else {
                    flp_key_construction = &flp_key_construction_tbl_0_5;
                    inst_valid = &inst_0_to_5_valid;
                }

                switch (inst_info->id) {
                    case 0:
                        flp_key_construction->instruction_0_16b = inst_info->instruction;
                        break;
                    case 1:
                        flp_key_construction->instruction_1_16b = inst_info->instruction;
                        break;
                    case 2:
                        flp_key_construction->instruction_2_16b = inst_info->instruction;
                        break;
                    case 3:
                        flp_key_construction->instruction_3_32b = inst_info->instruction;
                        break;
                    case 4:
                        flp_key_construction->instruction_4_32b = inst_info->instruction;
                        break;
                    case 5:
                        flp_key_construction->instruction_5_32b = inst_info->instruction;
                        break;
                    default:
                        break;
                }

                *inst_valid |= (0x1 << inst_info->id);
            }

            if (inst_0_to_5_valid) {
                switch (key->key) {
                    case 0:
                        flp_key_construction_tbl_0_5.key_a_inst_0_to_5_valid = inst_0_to_5_valid;

                        if ((flp_key_construction_tbl_0_5.key_b_inst_0_to_5_valid & inst_0_to_5_valid) ||
                            (flp_key_construction_tbl_0_5.key_c_inst_0_to_5_valid & inst_0_to_5_valid)) {
                            uint8 app_id;
                            res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

                            LOG_ERROR(BSL_LS_SOC_INIT,
                                     (BSL_META_U(unit,
                                                 "[%s] trying to override instructions used by key b/key c\n"),
                                      arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                            SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 41, exit);
                            }
                        break;
                    case 1:
                        flp_key_construction_tbl_0_5.key_b_inst_0_to_5_valid = inst_0_to_5_valid;

                        if ((flp_key_construction_tbl_0_5.key_a_inst_0_to_5_valid & inst_0_to_5_valid) ||
                            (flp_key_construction_tbl_0_5.key_c_inst_0_to_5_valid & inst_0_to_5_valid)) {
                            uint8 app_id;
                            res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit);

                            LOG_ERROR(BSL_LS_SOC_INIT,
                                     (BSL_META_U(unit,
                                                 "[%s] trying to override instructions used by key a/key c\n"),
                                      arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                            SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 43, exit);
                            }
                        break;
                    case 2:
                        flp_key_construction_tbl_0_5.key_c_inst_0_to_5_valid = inst_0_to_5_valid;

                        if ((flp_key_construction_tbl_0_5.key_a_inst_0_to_5_valid & inst_0_to_5_valid) ||
                            (flp_key_construction_tbl_0_5.key_b_inst_0_to_5_valid & inst_0_to_5_valid)) {
                            uint8 app_id;
                            res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit);

                            LOG_ERROR(BSL_LS_SOC_INIT,
                                     (BSL_META_U(unit,
                                                 "[%s] trying to override instructions used by key a/key b\n"),
                                      arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                            SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 45, exit);
                            }
                        break;
                    default:
                         break;
                }

                res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl_0_5);
                SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            }

            if (inst_6_to_11_valid) {
                switch (key->key) {
                    case 0:
                        flp_key_construction_tbl_6_11.key_a_inst_0_to_5_valid = inst_6_to_11_valid;

                        if ((flp_key_construction_tbl_6_11.key_b_inst_0_to_5_valid & inst_0_to_5_valid) ||
                            (flp_key_construction_tbl_6_11.key_c_inst_0_to_5_valid & inst_0_to_5_valid)) {
                            uint8 app_id;
                            res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit);

                            LOG_ERROR(BSL_LS_SOC_INIT,
                                     (BSL_META_U(unit,
                                                 "[%s] trying to override instructions used by key b/key c\n"),
                                      arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                            SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 47, exit);
                            }
                        break;
                    case 1:
                        flp_key_construction_tbl_6_11.key_b_inst_0_to_5_valid = inst_6_to_11_valid;

                        if ((flp_key_construction_tbl_6_11.key_a_inst_0_to_5_valid & inst_0_to_5_valid) ||
                            (flp_key_construction_tbl_6_11.key_c_inst_0_to_5_valid & inst_0_to_5_valid)) {
                            uint8 app_id;
                            res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 48, exit);

                            LOG_ERROR(BSL_LS_SOC_INIT,
                                     (BSL_META_U(unit,
                                                 "[%s] trying to override instructions used by key b/key c\n"),
                                      arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                            SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 49, exit);
                            }
                        break;
                    case 2:
                        flp_key_construction_tbl_6_11.key_c_inst_0_to_5_valid = inst_6_to_11_valid;

                        if ((flp_key_construction_tbl_6_11.key_a_inst_0_to_5_valid & inst_0_to_5_valid) ||
                            (flp_key_construction_tbl_6_11.key_b_inst_0_to_5_valid & inst_0_to_5_valid)) {
                            uint8 app_id;
                            res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

                            LOG_ERROR(BSL_LS_SOC_INIT,
                                     (BSL_META_U(unit,
                                                 "[%s] trying to override instructions used by key b/key c\n"),
                                      arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                            SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 51, exit);
                            }
                        break;
                    default:
                         break;
                }

                res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl_6_11);
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
            }
        }
    }

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_key_const_lpm_custom_lookup", 0, 0);
}


STATIC uint32
  arad_pp_flp_lookup_lpm_custom_lookup(
     int unit,
     int32 prog_id,
     uint32 db_indx
   )
{
    uint32
      res,
      indx,
      lookup;

    ARAD_FLP_CUSTOM_LPM_LOOKUP_INFO *lpm_lookup;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    ARAD_FLP_CUSTOM_LPM_KEY_CONSTR *key;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	lpm_lookup = &Arad_flp_custom_lpm_lookup_info[db_indx];

    
    if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1){
        if (lpm_lookup->app_id == PROG_FLP_IPV4UC) {
            lpm_lookup->key[0].key =0;
            lpm_lookup->key[1].key =0;
        }
    }

    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (indx=0; indx<lpm_lookup->lookup_num; ++indx) {
        key = &lpm_lookup->key[indx];
        lookup = (lpm_lookup->lookup_num>1)?indx:lpm_lookup->lookup_to_use;

        
        if (lookup == 0) {
            flp_lookups_tbl.lpm_1st_lkp_valid = 1;
            flp_lookups_tbl.lpm_1st_lkp_key_select = key->key;
            flp_lookups_tbl.lpm_1st_lkp_and_value = 0;
            flp_lookups_tbl.lpm_1st_lkp_or_value = key->key_or_value;
        }
        else {
            flp_lookups_tbl.lpm_2nd_lkp_valid = 1;
            flp_lookups_tbl.lpm_2nd_lkp_key_select = key->key;
            flp_lookups_tbl.lpm_2nd_lkp_and_value = 0;
            flp_lookups_tbl.lpm_2nd_lkp_or_value = key->key_or_value;
        }

    }

	res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
	SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_lookup_lpm_custom_lookup", 0, 0);
}


STATIC uint32
  arad_pp_flp_process_lpm_custom_lookup(
     int unit,
     int32 prog_id,
     uint32 db_indx
   )
{
    uint32
      res,
      indx,
      lookup,
      update = 0;

    ARAD_FLP_CUSTOM_LPM_LOOKUP_INFO *lpm_lookup;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
        flp_process_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	lpm_lookup = &Arad_flp_custom_lpm_lookup_info[db_indx];

    
    if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1){
        if (lpm_lookup->app_id == PROG_FLP_IPV4UC) {
            lpm_lookup->key[0].key =0;
            lpm_lookup->key[1].key =0;
        }
    }

    
    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (indx=0; indx<lpm_lookup->lookup_num; ++indx) {
        lookup = (lpm_lookup->lookup_num>1)?indx:lpm_lookup->lookup_to_use;

        
        if (lookup == 0) {

            if (flp_process_tbl.include_lpm_1st_in_result_a || flp_process_tbl.include_lpm_1st_in_result_b) {
                uint8 app_id;
                res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);

                
                #if 0
                LOG_INFO(BSL_LS_SOC_INIT,
                         (BSL_META_U(unit,
                                     "[%s] LPM 1st lookup is used for customized lookup,"
                                     " but the LPM 1st lookup result is included in FLP result.\n"),
                          arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                #endif
                flp_process_tbl.include_lpm_1st_in_result_a = 0;
                flp_process_tbl.include_lpm_1st_in_result_b = 0;
                update = 1;
            }
        }
        else {
            if (flp_process_tbl.include_lpm_2nd_in_result_a || flp_process_tbl.include_lpm_2nd_in_result_b) {
                uint8 app_id;
                res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &app_id);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

                
                #if 0
                LOG_INFO(BSL_LS_SOC_INIT,
                         (BSL_META_U(unit,
                                     "[%s] LPM 2nd lookup is used for customized lookup,"
                                     " but the LPM 2nd lookup result is included in FLP result.\n"),
                          arad_pp_flp_prog_id_to_prog_name(unit, app_id)));
                #endif
                flp_process_tbl.include_lpm_2nd_in_result_a = 0;
                flp_process_tbl.include_lpm_2nd_in_result_b = 0;
                update = 1;
            }
        }

    }

    if (update) {
        res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_process_lpm_custom_lookup", 0, 0);
}

STATIC uint32
  arad_pp_flp_lpm_custom_lookup_enable(
     int unit,
     uint8 app_id
   )
{
    uint32
      res,
      prog_id,
      indx;
    int found = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!ARAD_LPM_CUSTOM_LOOKUP_ENABLED(unit)) {
        goto exit; 
    }

    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, app_id, &prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (prog_id == -1) {
        goto exit; 
    }

    for (indx=0; indx<sizeof(Arad_flp_custom_lpm_lookup_info)/sizeof(Arad_flp_custom_lpm_lookup_info[0]); ++indx) {
        if (Arad_flp_custom_lpm_lookup_info[indx].app_id == app_id) {
            found = 1;
            break;
            }
        }

    if (!found) {
        goto exit;
        }

    res = arad_pp_flp_key_const_lpm_custom_lookup(unit, prog_id, indx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pp_flp_lookup_lpm_custom_lookup(unit, prog_id, indx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = arad_pp_flp_process_lpm_custom_lookup(unit, prog_id, indx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_lpm_custom_lookup_enable", 0, 0);
}

STATIC uint32
  arad_pp_flp_lpm_custom_lookup_init(
     int unit
   )
{
    uint32
      res,
      indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (indx=0; indx<sizeof(Arad_flp_custom_lpm_lookup_info)/sizeof(Arad_flp_custom_lpm_lookup_info[0]); ++indx) {
        res = arad_pp_flp_lpm_custom_lookup_enable(unit, Arad_flp_custom_lpm_lookup_info[indx].app_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_lpm_custom_lookup_init", 0, 0);
}


uint32
  arad_pp_flp_init(
     int unit,
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic, 
     uint32  sa_lookup_type 
   )
{
  uint32 res;
  char *propval;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  propval = soc_property_get_str(unit, spn_PON_APPLICATION_SUPPORT_ENABLED);
  if (propval) {
      if (sal_strcmp(propval, "TRUE") == 0) {
          is_pon_dpp_support = 1;
      } else {
          is_pon_dpp_support = 0;
      }
  }


  res = arad_pp_flp_static_programs_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);
  res = arad_pp_flp_prog_sel_cam_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);
  res = arad_pp_flp_lookups_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
  res = arad_pp_flp_process_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);
  res = arad_pp_flp_lookups_initial_programs(unit, ingress_learn_enable,ingress_learn_oppurtunistic);
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lpm_l2_mc", 0)) {
      res = arad_pp_ipv4mc_bridge_lookup_update(unit,1);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }

  res = arad_pp_flp_lpm_custom_lookup_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = arad_pp_flp_all_progs_instruction_set(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_init", 0, 0);
}


uint32
  arad_pp_flp_ethernet_prog_update(
     int unit,
     uint8 learn_enable,
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic, 
     uint32  sa_lookup_type 
   )
{
  uint32
    res;
  uint8
    mac_in_mac_enabled = 0;
  uint32
    tcam_access_profile_id;
  uint8
    prog_id = 0,
    slb_enabled = 0,
    sa_auth_enabled = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  if (!ARAD_PP_FLP_PON_PROG_LOAD()) {
    res = arad_pp_flp_lookups_ethernet_ing_learn(unit,ingress_learn_enable,sa_auth_enabled,slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_flp_process_ethernet_ing_learn(unit,learn_enable,sa_auth_enabled,slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  
  res = arad_pp_is_mac_in_mac_enabled(
      unit,
      &mac_in_mac_enabled
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  if (mac_in_mac_enabled)
  {
    uint32 prog_id;
    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM, &prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    res = arad_pp_flp_lookups_mac_in_mac_after_termination_update(unit, ingress_learn_enable, prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    res = arad_pp_flp_process_mac_in_mac_after_termination(unit, learn_enable, prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  }

  
  if (!ARAD_PP_FLP_PON_PROG_LOAD()) {
      res = arad_pp_flp_lookups_ipv4mc_bridge_update(unit, sa_auth_enabled,slb_enabled);
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

      res = arad_pp_flp_process_ipv4mc_bridge(unit, learn_enable, sa_auth_enabled,slb_enabled);
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

  if (SOC_DPP_CONFIG(unit)->trill.mode)
  {
    res = arad_pp_flp_lookups_TRILL_mc_update(unit, ingress_learn_enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = arad_pp_flp_process_TRILL_mc(unit,ingress_learn_enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      res = arad_pp_flp_lookups_TRILL_mc_after_recycle_overlay(unit,sa_auth_enabled,slb_enabled);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_pp_flp_process_TRILL_mc_after_recycle_overlay(unit,learn_enable,sa_auth_enabled,slb_enabled);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  if (SOC_DPP_L3_SRC_BIND_IPV4_ENABLE(unit) && SOC_IS_ARADPLUS_AND_BELOW(unit))
  {
    
    res = arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_dhcp(unit, sa_auth_enabled, slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    res = arad_pp_flp_lookups_ethernet_tk_epon_uni_v4_static(unit, sa_auth_enabled, slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    res = arad_pp_flp_process_ethernet_tk_epon_uni_v4(unit,learn_enable,sa_auth_enabled,slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit))
  {
    
    res = arad_pp_flp_tk_epon_uni_v6_tcam_profile_get(unit,&tcam_access_profile_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

    if(!SOC_IS_JERICHO(unit)){
       res = arad_pp_flp_lookups_ethernet_tk_epon_uni_v6(unit,tcam_access_profile_id,sa_auth_enabled,slb_enabled);
       SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
    res = arad_pp_flp_process_ethernet_tk_epon_uni_v6(unit,learn_enable,sa_auth_enabled,slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);
  }
  if ((SOC_DPP_CONFIG(unit)->pp.l3_src_bind_arp_relay & SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_UP) && !SOC_IS_JERICHO(unit)) {
    res = arad_pp_flp_app_to_prog_index_get(unit,
                                            PROG_FLP_PON_ARP_UPSTREAM,
                                            &prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    
    res = arad_pp_flp_lookups_pon_arp_upstream(unit,sa_auth_enabled,slb_enabled, prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    res = arad_pp_flp_process_pon_arp_upstream(unit,learn_enable,sa_auth_enabled,slb_enabled, prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_ethernet_prog_update", 0, 0);
}


uint32 arad_pp_flp_app_to_prog_index_get(
   int unit,
   uint32 app_id,
   uint8  *prog_index
){

    uint8
        cur_prog_id;
    uint32
      res;
    uint32
        indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    for (indx = 0; indx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++indx) {
        res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit,indx,&cur_prog_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        if (app_id == cur_prog_id) {
            *prog_index = indx;
            goto exit;
        }
    }
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_NOT_EXIST, 20, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_app_to_prog_index_get", app_id, 0);
}


uint32 arad_pp_prog_index_to_flp_app_get(
   int unit,
   uint32 prog_index,
   uint8 *app_id)
{

    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit,prog_index,app_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_prog_index_to_flp_app_get", prog_index, 0);
}


uint32
  arad_pp_flp_trap_config_update(
     int unit,
     SOC_PPC_TRAP_CODE_INTERNAL trap_code_internal,
     int trap_strength,
     int  snoop_strength
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  int line = -1;
  int indx;
  uint8 act_line=0;
  uint8 nof_lines = 1;
  uint8 map_id=0;
  uint8 use_lines_array = 0;
  int lines[4]; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch(trap_code_internal) {
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL:
      line = PROG_FLP_LSR;
      if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE)
      {
        map_id = 1;
        nof_lines = 2;
        use_lines_array = 1;
        lines[0] = PROG_FLP_LSR;
        lines[1] = PROG_FLP_VPLSOGRE;
      }
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION:
      line = PROG_FLP_P2P;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC:
      line = PROG_FLP_TRILL_UC;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC:
      line = PROG_FLP_TRILL_MC_ONE_TAG;
      nof_lines = 2; 

      map_id = 1;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6:
      map_id = 1;
      nof_lines = 4;
      use_lines_array = 1;
      lines[0] = PROG_FLP_IPV6UC;
      lines[1] = PROG_FLP_IPV6UC_RPF;
      lines[2] = PROG_FLP_IPV6UC_PUBLIC;
      lines[3] = PROG_FLP_IPV6UC_PUBLIC_RPF;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6:
      line = PROG_FLP_IPV6MC;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_FCF:
      line = PROG_FLP_FC_REMOTE;
      nof_lines = 4; 
      map_id = 1;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE:
      line = PROG_FLP_MYMAC_IP_DISABLED_TRAP;
      nof_lines = 1;
      map_id = 1;
      break;
  default:
      break;
  }

  
  if((line == - 1) && (use_lines_array == 0)) {
      goto exit;
  }

  for (indx = 0;indx < nof_lines; line++,indx++) {
      if (use_lines_array) {
          line = lines[indx];
      }

      if (map_id != 0) {
          uint8 indx;
          uint8 cur_prog_id;
          
          for (indx = 0; indx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++indx) {
              res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit,indx,&cur_prog_id);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
              if ((uint32)line == cur_prog_id) {
                  act_line = indx;
                  break;
              }
          }

          
          if (indx == SOC_DPP_DEFS_GET(unit, nof_flp_programs)) {
              continue;
          }
      }
      else {
          act_line = (uint8)line;
      }
      res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, act_line, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 141, exit);

      if (trap_strength != -1) {
          flp_process_tbl.not_found_trap_strength      = trap_strength;
      }
      if (snoop_strength != -1) {
          flp_process_tbl.not_found_snoop_strength      = snoop_strength;
      }
      if ((trap_code_internal == SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6) || (act_line == PROG_FLP_IPV6MC)) {
          

          if (flp_process_tbl.not_found_trap_strength) {
              
              flp_process_tbl.select_default_result_a = 2;
              flp_process_tbl.select_default_result_b = 2;
          }
          else {
              
              if (trap_code_internal == SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6) {
                  flp_process_tbl.select_default_result_a = 0;
                  flp_process_tbl.select_default_result_b = 0;
              }
              else { 
                  flp_process_tbl.select_default_result_a = 1;
                  flp_process_tbl.select_default_result_b = 1;
              }
          }
      }

      res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, act_line, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 142, exit);
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_trap_config_update", trap_code_internal, trap_strength);
}


uint32
  arad_pp_flp_trap_config_get(
     int unit,
     SOC_PPC_TRAP_CODE_INTERNAL trap_code_internal,
     uint32 *trap_strength,
     uint32  *snoop_strength
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  int line = -1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch(trap_code_internal) {
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL:
      line = PROG_FLP_LSR;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION:
      line = PROG_FLP_P2P;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC:
      line = PROG_FLP_TRILL_UC;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC:
      line = PROG_FLP_TRILL_MC_ONE_TAG;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6:
      line = PROG_FLP_IPV6UC;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6:
      line = PROG_FLP_IPV6MC;
      break;
  case SOC_PPC_TRAP_CODE_INTERNAL_FLP_FCF:
      line = PROG_FLP_FC_REMOTE;
      break;
  default:
      break;
  }
  
  if(line == - 1) {
      goto exit;
  }
  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, line, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 141, exit);

  if (trap_strength) {
      *trap_strength = flp_process_tbl.not_found_trap_strength;
  }
  if (snoop_strength) {
      *snoop_strength = flp_process_tbl.not_found_snoop_strength;
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_trap_config_update", trap_code_internal, 0);
}


uint32
  arad_pp_flp_prog_learn_set(
     int unit,
     int32  fwd_code,
     uint8  learn_enable
  )
{
  uint32 res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    flp_cam_tbl_data;
  uint32 indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (indx = 0; indx < SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines); indx++) {
      res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, indx, &flp_cam_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      if (flp_cam_tbl_data.parser_leaf_context != ARAD_PARSER_PLC_BFD_SINGLE_HOP) {

          if ((flp_cam_tbl_data.valid == 1) && (flp_cam_tbl_data.forwarding_code == fwd_code)) {
              res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, flp_cam_tbl_data.program, &flp_process_tbl);
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
              if(learn_enable)
              {
                  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, flp_cam_tbl_data.program, &flp_lookups_tbl);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                  flp_lookups_tbl.learn_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;

                  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, flp_cam_tbl_data.program, &flp_lookups_tbl);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
              }

              flp_process_tbl.learn_enable = learn_enable;
              res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, flp_cam_tbl_data.program, &flp_process_tbl);
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          }
      }
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_learn_set", 0, 0);
}


uint32
  arad_pp_flp_prog_learn_get(
     int unit,
     int32  fwd_code,
     uint8  *learn_enable
  )
{
  uint32 res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    flp_cam_tbl_data;
  uint32 indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (indx = 0; indx < SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines); indx++) {
      res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, indx, &flp_cam_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      if (flp_cam_tbl_data.parser_leaf_context != ARAD_PARSER_PLC_BFD_SINGLE_HOP) {

          if ((flp_cam_tbl_data.valid == 1) && (flp_cam_tbl_data.forwarding_code == fwd_code)) {
              res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, flp_cam_tbl_data.program, &flp_process_tbl);
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

              *learn_enable = flp_process_tbl.learn_enable;
              break;
          }
      }
  }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_prog_learn_get", 0, 0);
}

uint32
   arad_pp_ipmc_not_found_proc_update(
     int unit,
     uint8  flood
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;
  uint8 program_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV6MC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  flp_process_tbl.compatible_mc_bridge_fallback = flood;
  flp_process_tbl.select_default_result_a = (flood)?2:1; 
  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, PROG_FLP_IPV6MC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0))) {
      
      if (SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) {
        
        program_id = PROG_FLP_IPV4COMPMC_WITH_RPF;
      } else {
        
        res = arad_pp_flp_app_to_prog_index_get(unit,
                                                PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF,
                                                &program_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
      res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, program_id, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);
      flp_process_tbl.compatible_mc_bridge_fallback = flood;
      flp_process_tbl.select_default_result_a = (flood)?2:1; 
      res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, program_id, &flp_process_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);
   }

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ipmc_not_found_proc_update", 0, 0);
}


uint32
   arad_pp_ipmc_not_found_proc_get(
     int unit,
     uint8  *flood
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_IPV6MC, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  *flood = flp_process_tbl.compatible_mc_bridge_fallback;

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ipmc_not_found_proc_update", 0, 0);
}



uint32
   arad_pp_ipv4mc_bridge_lookup_update(
     int unit,
     uint8  mode 
   )
{
  uint32
    res,
    program_id;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  program_id = (mode == 0)?PROG_FLP_ETHERNET_ING_LEARN:PROG_FLP_IPV4MC_BRIDGE;

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_ipv4mc_bridge_v4mc_cam_sel_id.get(unit,&cam_sel_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  prog_selection_cam_tbl.program = program_id;

  res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ipv4mc_bridge_lookup_update", mode, 0);
}

uint32
   arad_pp_ipv4mc_bridge_lookup_get(
     int unit,
     uint8  *mode
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
    prog_selection_cam_tbl;
  int32 cam_sel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_ipv4mc_bridge_v4mc_cam_sel_id.get(unit,&cam_sel_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *mode  = (prog_selection_cam_tbl.program == PROG_FLP_ETHERNET_ING_LEARN)?0:1;

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ipv4mc_bridge_lookup_get", 0, 0);
}



uint32
   arad_pp_flp_key_const_fcf_transit(
     int unit,
     int32  prog_id
   )
{
  uint32
    res, fid_ce_inst = 0;
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
    flp_key_construction_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_FID,0, &fid_ce_inst);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  flp_key_construction_tbl.instruction_0_16b        = fid_ce_inst;
  flp_key_construction_tbl.instruction_1_16b        = ARAD_PP_CE_SA_FWD_HEADER_16_MSB;
  flp_key_construction_tbl.instruction_3_32b        = ARAD_PP_CE_SA_FWD_HEADER_32_LSB;
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0xB ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
  flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
  flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
  res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_key_const_fcf_transit_learn", 0, 0);
}



uint32
   arad_pp_flp_lookups_fcf_transit(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
    flp_lookups_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  flp_lookups_tbl.lem_1st_lkp_valid     = (!sa_auth_enabled && !slb_enabled)? 1 : 0 ;
  flp_lookups_tbl.lem_1st_lkp_key_select = 0;
  flp_lookups_tbl.lem_1st_lkp_key_type   = 1;
  flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
  flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
  flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
  flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
  flp_lookups_tbl.learn_key_select   = 0;


  res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_lookups_fcf_transit", 0, 0);
}


uint32
   arad_pp_flp_process_fcf_transit(
     int unit,
     uint8 learn_enable, 
     uint8 sa_auth_enabled,
     uint8 slb_enabled,
     int32 prog_id
   )
{
  uint32
    res;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;
  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
    flp_process_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  flp_process_tbl.include_lem_2nd_in_result_a    = 1;
  flp_process_tbl.include_lem_1st_in_result_b    = 1;
  flp_process_tbl.result_a_format            = 0;
  flp_process_tbl.result_b_format            = 0;
  flp_process_tbl.sa_lkp_result_select         = 0;
  flp_process_tbl.sa_lkp_process_enable        = (!sa_auth_enabled && !slb_enabled)? 1 : 0;
  flp_process_tbl.procedure_ethernet_default  = 3;
  flp_process_tbl.enable_hair_pin_filter       = 1;
  flp_process_tbl.learn_enable               = learn_enable;
  flp_process_tbl.not_found_trap_strength      = 7;
  flp_process_tbl.unknown_address              = 3;
  flp_process_tbl.not_found_trap_code          = SOC_PPC_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0;

  res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tmp = 4; 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
  res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_process_fcf_transit", 0, 0);
}




STATIC uint32
   arad_pp_flp_pwe_gre_progs_init(
     int unit
   )
{
  uint32
    res;
  int prog;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = arad_pp_flp_prog_sel_cam_pwe_gre(unit,&prog);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = arad_pp_flp_key_const_pwe_gre(unit, FALSE, FALSE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  res = arad_pp_flp_lookups_pwe_gre(unit,prog);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  res = arad_pp_flp_process_pwe_gre(unit,prog);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_pwe_gre_progs_init", 0, 0);
}


STATIC uint32
   arad_pp_flp_fcoe_progs_init(
     int unit
   )
{
    uint32
      res;
    int progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    
    res = arad_pp_flp_prog_sel_cam_fcoe_fcf_with_vft(unit,&progs[1], FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);
    res = arad_pp_flp_prog_sel_cam_fcoe_fcf(unit,&progs[0],FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
    res = arad_pp_flp_prog_sel_cam_fcoe_fcf_with_vft_remote(unit,&progs[3]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    res = arad_pp_flp_prog_sel_cam_fcoe_fcf_remote(unit,&progs[2]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

    
    res = arad_pp_flp_lookups_fcf(unit,progs, TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

    
    res = arad_pp_flp_key_const_fcoe_fcf(unit,progs, FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

    res = arad_pp_flp_process_fcoe_fcf(unit,progs);
    SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

    
    
    res = arad_pp_flp_prog_sel_cam_fcoe_ethernet_ing_learn(unit,&progs[0], TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    res = arad_pp_flp_key_const_fcf_transit(unit,progs[0]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
    
    res = arad_pp_flp_lookups_fcf_transit(unit, 0,0,progs[0]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    res = arad_pp_flp_process_fcf_transit(unit, 0,0,0,progs[0]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_fcoe_progs_init", 0, 0);
}


uint32
   arad_pp_flp_npv_programs_init(
     int unit
   )
{
    uint32
      res;
    int progs[2];
    int is_vsan_from_vsi;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_flp_fcoe_is_vsan_from_vsi_mode(unit, &is_vsan_from_vsi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

    
    res = arad_pp_flp_prog_sel_cam_fcoe_fcf_with_vft(unit, &progs[1], TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

    res = arad_pp_flp_prog_sel_cam_fcoe_fcf(unit,&progs[0], TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    
    res = arad_pp_flp_key_const_fcoe_fcf_npv(unit,progs, is_vsan_from_vsi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

    res = arad_pp_flp_lookups_fcf_npv(unit,progs); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

    if (SOC_IS_JERICHO(unit)) {
        int is_table_initiated = 0;
        res = arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, &is_table_initiated);
        SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

        if (!is_table_initiated) {
            res = arad_pp_flp_dbal_fcoe_npv_program_tables_init(unit, is_vsan_from_vsi, progs[0], progs[1]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 9, exit);
        }
    }

    res = arad_pp_flp_process_fcoe_fcf_npv(unit,progs);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_n_port_programs_init", 0, 0);
}

uint32
   arad_pp_flp_n_port_programs_disable(
     int unit
   )
{
    uint32
        prog_id,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_WITH_VFT_N_PORT, &prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    res = arad_pp_flp_program_disable(unit,prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_N_PORT, &prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    res = arad_pp_flp_program_disable(unit,prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_n_port_programs_init", 0, 0);
}

uint32
arad_pp_flp_fcoe_zoning_set(
     int unit,
     int is_npv
   )
{
    uint32
      res;
    int progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS];
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA   flp_lookups_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC, (uint32*)&(progs[0]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_WITH_VFT, (uint32*)&(progs[1]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_REMOTE, (uint32*)&(progs[2]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_WITH_VFT_REMOTE, (uint32*)&(progs[3]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, progs[0], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    flp_lookups_tbl.lem_1st_lkp_valid      = (!is_npv);
    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[0], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  	res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, progs[1], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    flp_lookups_tbl.lem_1st_lkp_valid      = (!is_npv);
    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[1], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, progs[2], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    flp_lookups_tbl.lem_1st_lkp_valid      = (!is_npv);
    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[2], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, progs[3], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    flp_lookups_tbl.lem_1st_lkp_valid      = (!is_npv);
    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, progs[3], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_fcoe_zoning_set", 0, 0);
}


uint32
arad_pp_flp_fcoe_vsan_mode_set(
     int unit,
     int is_vsan_from_vsi
   )
{
    uint32
      res;
    int progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS];
    int is_zoning_enabled;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC, (uint32*)&(progs[0]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_WITH_VFT, (uint32*)&(progs[1]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_REMOTE, (uint32*)&(progs[2]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_WITH_VFT_REMOTE, (uint32*)&(progs[3]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_flp_key_const_fcoe_fcf(unit,progs, is_vsan_from_vsi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    
    res = arad_pp_flp_fcoe_is_zoning_enabled(unit, &is_zoning_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (!is_zoning_enabled) {
        res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_N_PORT, (uint32*)&(progs[0]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
        res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC_WITH_VFT_N_PORT, (uint32*)&(progs[1]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = arad_pp_flp_key_const_fcoe_fcf_npv(unit,progs, is_vsan_from_vsi);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    if (SOC_IS_JERICHO(unit)) {
        int is_table_initiated = 0;
        res = arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, &is_table_initiated);
        SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

        
        if (is_table_initiated) {
            res = arad_pp_flp_dbal_fcoe_npv_program_tables_init(unit, is_vsan_from_vsi, progs[0], progs[1]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 9, exit);
        }
    }

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_fcoe_vsan_mode_set", 0, 0);
}

uint32
arad_pp_flp_fcoe_is_zoning_enabled(
     int unit,
     int* is_enabled
   )
{
    uint32
      res;
    int progs[ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS];
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA    flp_lookups_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC, (uint32*)&(progs[0]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, progs[0], &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    (*is_enabled) = flp_lookups_tbl.lem_1st_lkp_valid;

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_fcoe_is_zoning_enabled", 0, 0);

}


uint32
arad_pp_flp_fcoe_is_vsan_from_vsi_mode(
     int unit,
     int* is_vsan_from_vsi
   )
{
    uint32
        prog,
        res;
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA flp_key_construction_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_FC, &prog);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    if(flp_key_construction_tbl.instruction_0_16b == ARAD_PP_FLP_16B_INST_P6_VSI(12)){
        (*is_vsan_from_vsi) = TRUE;
    }else{
        (*is_vsan_from_vsi) = FALSE;
    }

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_fcoe_is_vsan_from_vsi_mode", 0, 0);
}





STATIC uint32
   arad_pp_flp_ipmc_bidir_progs_init(
     int unit
   )
{
    uint32 vrf_ce_inst = 0, rif_ce_inst = 0,
      res;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA
      prog_selection_cam_tbl;
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA
      flp_key_construction_tbl;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
      flp_lookups_tbl;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
      flp_process_tbl;
    uint32
      tmp;
    soc_reg_above_64_val_t
      reg_val;
    int32  prog_id;
    int32 cam_sel_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF,0, &vrf_ce_inst);
    arad_pp_dbal_qualifier_to_instruction(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF,0, &rif_ce_inst);

    
    
    res = arad_pp_flp_set_app_id_and_get_cam_sel(unit,PROG_FLP_BIDIR,FALSE,TRUE,&cam_sel_id, &prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    prog_selection_cam_tbl.tt_lookup_0_found = 0x1;
    prog_selection_cam_tbl.tt_lookup_0_found_mask = 0x0;
    prog_selection_cam_tbl.vt_lookup_1_found = 0x0;
    prog_selection_cam_tbl.tt_lookup_0_found_mask = 0x0;
    prog_selection_cam_tbl.forwarding_code  = ARAD_PP_FWD_CODE_IPV4_MC;
    prog_selection_cam_tbl.forwarding_code_mask  = 0x0;
    prog_selection_cam_tbl.program = prog_id;
    prog_selection_cam_tbl.valid = 1;

    res = arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(unit, cam_sel_id, &prog_selection_cam_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    flp_key_construction_tbl.instruction_0_16b        = ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D_BITS(8); 
    flp_key_construction_tbl.instruction_1_16b        = rif_ce_inst; 
    flp_key_construction_tbl.key_a_inst_0_to_5_valid    = 0x3 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid    = 0x0 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid    = 0x0 ;
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    res = arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    flp_key_construction_tbl.instruction_0_16b     = vrf_ce_inst;
    flp_key_construction_tbl.instruction_3_32b     = ARAD_PP_CE_DIP_FWD_HEADER;
    flp_key_construction_tbl.key_a_inst_0_to_5_valid = 0x0 ;
    flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x9 ;
    flp_key_construction_tbl.key_c_inst_0_to_5_valid = 0x0 ;
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
        
        flp_key_construction_tbl.instruction_3_32b  = ARAD_PP_CE_SIP_FWD_HEADER;
        flp_key_construction_tbl.instruction_4_32b  = ARAD_PP_CE_DIP_FWD_HEADER;
        flp_key_construction_tbl.key_b_inst_0_to_5_valid = 0x19 ;
    }
#endif
    res = arad_pp_ihb_flp_key_construction_tbl_set_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_construction_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

    
    
    res = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    flp_lookups_tbl.lem_1st_lkp_valid     = 1;
    flp_lookups_tbl.lem_1st_lkp_key_select = 0;
    flp_lookups_tbl.lem_1st_lkp_key_type   = 0;
    flp_lookups_tbl.lpm_1st_lkp_valid     = 0;
    flp_lookups_tbl.lpm_1st_lkp_key_select = 0;
    flp_lookups_tbl.lpm_1st_lkp_and_value = 0;
    flp_lookups_tbl.lpm_1st_lkp_or_value = 0;
    flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
    
    res =  arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_FLP_IPMC_BIDIR_KEY_OR_MASK,&flp_lookups_tbl.lem_1st_lkp_or_value);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    flp_lookups_tbl.lem_2nd_lkp_valid     = 1;
    flp_lookups_tbl.lem_2nd_lkp_key_select = 1;
    flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
    flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_IPV4_KEY_OR_MASK; 
    flp_lookups_tbl.lpm_2nd_lkp_valid     = 0;
    flp_lookups_tbl.lpm_2nd_lkp_key_select = 0;
    flp_lookups_tbl.lpm_2nd_lkp_and_value = 0;
    flp_lookups_tbl.lpm_2nd_lkp_or_value = 0;
    flp_lookups_tbl.learn_key_select      = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
        flp_lookups_tbl.elk_lkp_valid = 0x1;
        flp_lookups_tbl.elk_wait_for_reply = 0x1;
        flp_lookups_tbl.elk_opcode = ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC;

        flp_lookups_tbl.elk_key_a_valid_bytes = 0x0;
        flp_lookups_tbl.elk_key_b_valid_bytes = 10; 
        flp_lookups_tbl.elk_key_c_valid_bytes = 0x0;
    }
#endif
    res = arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    


    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    flp_process_tbl.include_lem_2nd_in_result_a    = 1;
    flp_process_tbl.include_lem_1st_in_result_b    = 1;
    flp_process_tbl.include_lpm_2nd_in_result_a    = 0;
    flp_process_tbl.include_lpm_1st_in_result_b    = 0;
    flp_process_tbl.result_a_format            = 0;
    flp_process_tbl.result_b_format            = 0;
    flp_process_tbl.procedure_ethernet_default  = 3;
    flp_process_tbl.enable_hair_pin_filter       = 1;
    flp_process_tbl.enable_rpf_check            = 0;
    flp_process_tbl.learn_enable = 1;
    
    flp_process_tbl.select_default_result_a = 1;
    flp_process_tbl.compatible_mc_bridge_fallback = 1;
    flp_process_tbl.select_default_result_b = 0;
    flp_process_tbl.apply_fwd_result_a           = 1; 
#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
      flp_process_tbl.elk_result_format = 1;
      flp_process_tbl.include_elk_fwd_in_result_a = 1;
      flp_process_tbl.include_elk_ext_in_result_a = 0;
      flp_process_tbl.include_elk_fwd_in_result_b = 0;
      flp_process_tbl.include_elk_ext_in_result_b = 0;
  }
#endif
    res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, prog_id, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    tmp = 1; 
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    res = READ_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, 0, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    SHR_BITCOPY_RANGE(reg_val,3*prog_id,&tmp,0,3);
    res = WRITE_IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr(unit, SOC_CORE_ALL, reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

  exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_ipmc_bidir_progs_init", 0, 0);
}


STATIC uint32
   arad_pp_flp_vmac_progs_init(
     int unit,
     uint8  sa_auth_enabled,
     uint8  slb_enabled
   )
{
    uint32
      res;
    int progs[2];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_flp_prog_sel_cam_pon_vmac_upstream(unit, &progs[0]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = arad_pp_flp_key_const_pon_vmac_upstream(unit, progs[0]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = arad_pp_flp_lookups_pon_vmac_upstream(unit,
                                                progs[0],
                                                sa_auth_enabled,
                                                slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    res = arad_pp_flp_process_pon_vmac_upstream(unit,
                                                progs[0],
                                                TRUE,
                                                sa_auth_enabled,
                                                slb_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = arad_pp_flp_prog_sel_cam_pon_vmac_downstream(unit, &progs[1]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = arad_pp_flp_key_const_pon_vmac_downstream(unit, progs[1]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = arad_pp_flp_lookups_pon_vmac_downstream(unit, progs[1]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = arad_pp_flp_process_pon_vmac_downstream(unit, progs[1]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_vmac_progs_init", 0, 0);
}

STATIC uint32
   arad_pp_flp_pon_arp_prog_init(
     int unit,
     uint8 sa_auth_enabled,
     uint8 slb_enabled
   )
{
    uint32
        res;
        int prog_id;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_DPP_CONFIG(unit)->pp.l3_src_bind_arp_relay & SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_DOWN) {
        res = arad_pp_flp_prog_sel_cam_pon_arp_downstream(unit, &prog_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(SOC_IS_JERICHO(unit)){
            res = arad_pp_flp_dbal_pon_sav_arp_program_tables_init(unit, FALSE, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
        } else {
            res = arad_pp_flp_key_const_pon_arp_downstream(unit, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            res = arad_pp_flp_lookups_pon_arp_downstream(unit, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
        res = arad_pp_flp_process_pon_arp_downstream(unit, prog_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    if (SOC_DPP_CONFIG(unit)->pp.l3_src_bind_arp_relay & SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_UP) {
        prog_id = 0;
        res = arad_pp_flp_prog_sel_cam_pon_arp_upstream(unit, &prog_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        if(SOC_IS_JERICHO(unit)){
            res = arad_pp_flp_dbal_pon_sav_arp_program_tables_init(unit, TRUE, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);
        } else {
            res = arad_pp_flp_key_const_pon_arp_upstream(unit, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
            res = arad_pp_flp_lookups_pon_arp_upstream(unit, sa_auth_enabled,slb_enabled, prog_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        }

        res = arad_pp_flp_process_pon_arp_upstream(unit, TRUE, sa_auth_enabled,slb_enabled, prog_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_pon_arp_prog_init", 0, 0);
}



uint32
  arad_pp_flp_not_found_trap_config_set(
     int unit,
     int hw_trap_id
   )
{
    uint32
        res, index = 0;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
        flp_process_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (index = 0; index < SOC_DPP_DEFS_GET(unit, nof_flp_programs); index++) {
        res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, index, &flp_process_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        if (flp_process_tbl.not_found_trap_code != SOC_PPC_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0) {
            continue;
        }

        flp_process_tbl.not_found_trap_code = hw_trap_id;
        res = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, index, &flp_process_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_not_found_trap_config_set", hw_trap_id, 0);
}

uint32
  arad_pp_flp_not_found_trap_config_get(
     int unit,
     int *hw_trap_id
   )
{
    uint32
        res = 0;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA
        flp_process_tbl;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, PROG_FLP_TM, &flp_process_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    *hw_trap_id = flp_process_tbl.not_found_trap_code;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_not_found_trap_config_get", 0, 0);
}


char*
  arad_pp_flp_prog_id_to_prog_name(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 prog_id
)
{
    switch(prog_id)
    {
    case PROG_FLP_TM:
      return "PROG_FLP_TM";
    case PROG_FLP_ETHERNET_ING_LEARN:
      return "PROG_FLP_ETHERNET_ING_LEARN";
    case PROG_FLP_MPLS_CONTROL_WORD_TRAP:
      return "PROG_FLP_MPLS_CONTROL_WORD_TRAP";
    case PROG_FLP_IPV4UC_PUBLIC_RPF:
      return "PROG_FLP_IPV4UC_PUBLIC_RPF";
    case PROG_FLP_IPV6UC_PUBLIC_RPF:
      return "PROG_FLP_IPV6UC_PUBLIC_RPF";
    case PROG_FLP_IPV6UC_PUBLIC:
      return "PROG_FLP_IPV6UC_PUBLIC";
    case PROG_FLP_IPV4UC_PUBLIC:
      return "PROG_FLP_IPV4UC_PUBLIC";
    case PROG_FLP_IPV4UC_RPF:
      return "PROG_FLP_IPV4UC_RPF";
    case PROG_FLP_IPV4UC:
      return "PROG_FLP_IPV4UC";
    case PROG_FLP_IPV6UC:
      return "PROG_FLP_IPV6UC";
    case PROG_FLP_P2P:
      return "PROG_FLP_P2P";
    case PROG_FLP_IPV6MC:
      return "PROG_FLP_IPV6MC";
    case PROG_FLP_LSR:
      return "PROG_FLP_LSR";
    case PROG_FLP_TRILL_UC:
      if (SOC_DPP_CONFIG(unit)->trill.mode) {
        return "PROG_FLP_TRILL_UC";
      }
      else if(SOC_DPP_PPPOE_IS_ENABLE(unit)){
        return "PROG_FLP_PPPOE_IPV4UC";
      }
      else {
        return "PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC";
      }
    case PROG_FLP_TRILL_MC_ONE_TAG:
      if (SOC_DPP_CONFIG(unit)->trill.mode) {
        return "PROG_FLP_TRILL_MC_ONE_TAG";
      }
      else if(SOC_DPP_PPPOE_IS_ENABLE(unit)){
        return "PROG_FLP_PPPOE_IPV6UC";
      }else {
        return "PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC";
      }
    case PROG_FLP_TRILL_MC_TWO_TAGS:
      if (SOC_DPP_CONFIG(unit)->trill.mode) {
        return "PROG_FLP_TRILL_MC_TWO_TAGS";
      } else {
        return "PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM";
      }
    case PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP :
        return (ARAD_PP_FLP_PON_PROG_LOAD() ? "PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP" : "PROG_FLP_VPWS_TAGGED_SINGLE_TAG");
    case PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP :
        return (ARAD_PP_FLP_PON_PROG_LOAD() ? "PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP" : "PROG_FLP_VPWS_TAGGED_DOUBLE_TAG");
    case PROG_FLP_ETHERNET_MAC_IN_MAC:
      return "PROG_FLP_ETHERNET_MAC_IN_MAC";
    case PROG_FLP_TRILL_AFTER_TERMINATION:
        if (SOC_DPP_CONFIG(unit)->pp.custom_ip_route) {
            return "PROG_FLP_IP4UC_CUSTOM_ROUTE";
        } else {
            return "PROG_FLP_TRILL_AFTER_TERMINATION";
        }
    case PROG_FLP_IPV4MC_BRIDGE:
      return "PROG_FLP_IPV4MC_BRIDGE";
    case PROG_FLP_IPV4COMPMC_WITH_RPF:
      return "PROG_FLP_IPV4COMPMC_WITH_RPF";
    case PROG_FLP_FC_WITH_VFT_N_PORT:
      return "PROG_FLP_FC_WITH_VFT_N_PORT";
    case PROG_FLP_FC_N_PORT:
      return "PROG_FLP_FC_N_PORT";
    case PROG_FLP_FC_REMOTE:
      return "PROG_FLP_FC_REMOTE";
    case PROG_FLP_FC_WITH_VFT_REMOTE:
      return "PROG_FLP_FC_WITH_VFT_REMOTE";
    case PROG_FLP_FC:
      return "PROG_FLP_FC";
    case PROG_FLP_FC_WITH_VFT:
      return "PROG_FLP_FC_WITH_VFT";
    case PROG_FLP_BIDIR:
      return "PROG_FLP_BIDIR";
    case PROG_FLP_IPV6UC_RPF:
      return "PROG_FLP_IPV6UC_RPF";
    case PROG_FLP_VMAC_UPSTREAM:
      return "PROG_FLP_VMAC_UPSTREAM";
    case PROG_FLP_VMAC_DOWNSTREAM:
      return "PROG_FLP_VMAC_DOWNSTREAM";
    case PROG_FLP_VPLSOGRE:
        if (SOC_DPP_CONFIG(unit)->pp.custom_ip_route) {
            return "PROG_FLP_IP6UC_CUSTOM_ROUTE";
        } else {
            return "PROG_FLP_VPLSOGRE";
        }
    case PROG_FLP_ETHERNET_PON_LOCAL_ROUTE:
      return "PROG_FLP_ETHERNET_PON_LOCAL_ROUTE";
    case PROG_FLP_ETHERNET_PON_DEFAULT_UPSTREAM:
      return "PROG_FLP_ETHERNET_PON_DEFAULT_UPSTREAM";
    case PROG_FLP_ETHERNET_PON_DEFAULT_DOWNSTREAM:
      return "PROG_FLP_ETHERNET_PON_DEFAULT_DOWNSTREAM";
    case PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF:
      return "PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF";
    case PROG_FLP_PON_ARP_DOWNSTREAM:
      return "PROG_FLP_PON_ARP_DOWNSTREAM";
    case PROG_FLP_PON_ARP_UPSTREAM:
      return "PROG_FLP_PON_ARP_UPSTREAM";
    case PROG_FLP_FC_TRANSIT:
        return "PROG_FLP_FC_TRANSIT";
    case PROG_FLP_IPV6UC_WITH_RPF_2PASS:
        return "PROG_FLP_IPV6UC_WITH_RPF_2PASS";
    case PROG_FLP_ETHERNET_ING_IVL_LEARN:
        return "PROG_FLP_ETHERNET_ING_IVL_LEARN";
    case PROG_FLP_ETHERNET_ING_IVL_INNER_LEARN:
        return "PROG_FLP_ETHERNET_ING_IVL_INNER_LEARN";
    case PROG_FLP_ETHERNET_ING_IVL_FWD_OUTER_LEARN:
        return "PROG_FLP_ETHERNET_ING_IVL_FWD_OUTER_LEARN";
    case PROG_FLP_BFD_IPV4_SINGLE_HOP:
        return "PROG_FLP_BFD_IPV4_SINGLE_HOP";
	case PROG_FLP_BFD_IPV6_SINGLE_HOP:
        return "PROG_FLP_BFD_IPV6_SINGLE_HOP";
	case PROG_FLP_OAM_STATISTICS:
        return "PROG_FLP_OAM_STATISTICS";
    case PROG_FLP_OAM_DOWN_UNTAGGED_STATISTICS:
        return "PROG_FLP_OAM_DOWN_UNTAGGED_STATISTICS";
    case PROG_FLP_OAM_SINGLE_TAG_STATISTICS:
        return "PROG_FLP_OAM_SINGLE_TAG_STATISTICS";
	case PROG_FLP_OAM_DOUBLE_TAG_STATISTICS:
        return "PROG_FLP_OAM_DOUBLE_TAG_STATISTICS";
    case PROG_FLP_BFD_STATISTICS:
        return "PROG_FLP_BFD_STATISTICS";
    case PROG_FLP_BFD_MPLS_STATISTICS:
        return "PROG_FLP_BFD_MPLS_STATISTICS";
    case PROG_FLP_BFD_PWE_STATISTICS:
        return "PROG_FLP_BFD_PWE_STATISTICS";
    case PROG_FLP_IPV4_DC:
        return "PROG_FLP_IPV4_DOUBLE_CAPACITY";
    case PROG_FLP_MYMAC_IP_DISABLED_TRAP:
        return "PROG_FLP_MYMAC_IP_DISABLED_TRAP";
    case PROG_FLP_IPV4_MC_SSM:
        return "PROG_FLP_IPV4_MC_SSM";
    case PROG_FLP_IPV6_MC_SSM_EUI:
       return "PROG_FLP_IPV6_MC_SSM_EUI";
    case PROG_FLP_MPLS_EXP_NULL_TTL_REJECT:
       return "PROG_FLP_MPLS_EXP_NULL_TTL_REJECT";
    case PROG_FLP_OAM_OUTLIF_L2G:
       return "PROG_FLP_OAM_OUTLIF_L2G";
    case PROG_FLP_IPV4COMPMC_WITH_MIM_LEARNING:
       return "PROG_FLP_IPV4COMPMC_WITH_MIM_LEARNING";
    case PROG_FLP_TWO_PASS_MC_ENCAP_TO_DEST:
       return "PROG_FLP_TWO_PASS_MC_ENCAP_TO_DEST";
    case PROG_FLP_IPV6_MC_SSM:
       return "PROG_FLP_IPV6_MC_SSM";
    case PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP:
       return "PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP";
    default:
      return "Unknown program";
    }
}

STATIC uint32
  arad_pp_flp_program_info_cam_selection_dump(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int32 prog_id
  )
{
    int i;
    uint32 res;
    uint64 cam_sel_list;
    int nof_cam_sel=0;
    int nof_flp_selections  = SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines);
    uint8 first_printed = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    COMPILER_64_ZERO(cam_sel_list);

    res = arad_pp_flp_get_cam_sel_list_by_prog_id(unit,prog_id,&cam_sel_list,&nof_cam_sel);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_CLI((BSL_META_U(unit,"Prog. Selections IDs [%d selection(s)]: "),nof_cam_sel));

    if (nof_cam_sel > 0) {
        for (i=0 ; i < nof_flp_selections ; i++) {
            if (COMPILER_64_BITTEST(cam_sel_list,i)) {
                if (first_printed) {
                    LOG_CLI((BSL_META_U(unit, ", ")));
                }
                LOG_CLI((BSL_META_U(unit, "%d"),i));
                first_printed=TRUE;
            }
        }
    }
    else{
        LOG_CLI((BSL_META_U(unit, "None")));
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_program_info_cam_selection_dump", prog_id, 0);
}

uint32
  arad_pp_flp_access_print_all_programs_data(
    SOC_SAND_IN  int                                 unit
  )
{
    uint8  cur_prog_id;
    uint32 prog_indx;
    char*  prog_name;
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_CLI((BSL_META_U(unit,
                        "\n\tVALID FLP PROGRAMS:\n")));
    LOG_CLI((BSL_META("\n----------------------------------------------------------------------------------------------------------------------------------------------------------\n\n\n")));
    for (prog_indx = 0; prog_indx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); prog_indx++) {
        res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit,prog_indx,&cur_prog_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        prog_name = arad_pp_flp_prog_id_to_prog_name(unit, cur_prog_id);

        if (cur_prog_id != ARAD_PP_FLP_MAP_PROG_NOT_SET) {
          LOG_CLI((BSL_META_U(unit,"Program %s ID (%d)\n"), prog_name, prog_indx));
          arad_pp_flp_program_info_cam_selection_dump(unit, prog_indx);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          arad_pp_flp_dbal_program_info_dump(unit, prog_indx);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_flp_access_print_all_programs_data", 0, 0);
}


uint32
  arad_pp_flp_access_print_last_programs_data(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 core_id,
    SOC_SAND_IN   int                 to_print,
    SOC_SAND_OUT  int                 *prog_id,
    SOC_SAND_OUT  int                 *num_of_progs

  )
{
    uint32 fld_val, progs_bmp, pgm_ndx;
    uint32 res = SOC_SAND_OK;
    uint8  cur_prog_id;
    char*  prog_name;
    uint8  found = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    *prog_id = -1;
    (*num_of_progs) = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_DBG_FLP_SELECTED_PROGRAMr, core_id, 0, DBG_FLP_SELECTED_PROGRAMf, &progs_bmp));
    if (to_print) {
        LOG_CLI((BSL_META_U(unit,
                            "Last invoked FLP program:\n")));
    }

    for (pgm_ndx = 0; pgm_ndx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); pgm_ndx++)
    {
        if (SOC_SAND_GET_BIT(progs_bmp, pgm_ndx))
        {
            
            res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit,pgm_ndx,&cur_prog_id);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

            *prog_id = pgm_ndx;
            (*num_of_progs) ++;

            prog_name = arad_pp_flp_prog_id_to_prog_name(unit, cur_prog_id);

            if (cur_prog_id != ARAD_PP_FLP_MAP_PROG_NOT_SET)
            {
                found = TRUE;
                if(to_print){
                    LOG_CLI((BSL_META_U(unit,"Program %s ID: %d\n"), prog_name, pgm_ndx));
                }
            }
        }
    }

    if (!found)
    {
        if (to_print) {
            LOG_CLI((BSL_META_U(unit,"No program invoked\n")));
        }
    }

    
    if(SOC_DPP_DEFS_GET(unit, nof_flp_programs) > 32) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 15, exit);
    }

    
    if(SOC_DPP_DEFS_GET(unit, nof_flp_programs) < 32) {
        fld_val = (1 << SOC_DPP_DEFS_GET(unit, nof_flp_programs)) - 1;
    } else {
        fld_val = 0xffffffff;
    }
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_DBG_FLP_SELECTED_PROGRAMr, core_id, 0, DBG_FLP_SELECTED_PROGRAMf,  fld_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_flp_access_print_last_programs_data()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

