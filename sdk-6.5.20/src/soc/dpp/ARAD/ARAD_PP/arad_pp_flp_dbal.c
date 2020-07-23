/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_dbal.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#if defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#endif






#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

STATIC uint32
arad_pp_flp_dbal_kaps_table_prefix_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 *table_prefix, uint32 *table_prefix_len)
{
    uint32 num_of_dynamic_tables = 0;
    uint32 num_of_dynamic_tables_mc = 0;
    uint32 dynamic_tables_additional_bits = 0;  
    uint32 dynamic_table_prefix = 0; 
    uint32 dynamic_tables_additional_bits_max = JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_BIT + 1; 
    uint32 ipv4uc_dynamic_tables_additional_bits = 0;
    uint32 ipv4mc_dynamic_tables_additional_bits = 0;

    SOCDNX_INIT_FUNC_DEFS

    
    if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable) {
        num_of_dynamic_tables++;
        num_of_dynamic_tables++; 
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_KAPS_LPM) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }


    if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
        num_of_dynamic_tables++;
        num_of_dynamic_tables++; 
    } else if ((table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS) || (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) {
        num_of_dynamic_tables++;
        num_of_dynamic_tables++; 
    } else if ((table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS) || (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (SOC_DPP_L3_SRC_BIND_IPV4_OR_ARP_ENABLE(unit)) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit)) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (ARAD_KBP_ENABLE_IPV4_UC) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (ARAD_KBP_ENABLE_IPV4_MC) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (ARAD_KBP_ENABLE_IPV4_DC) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (ARAD_KBP_ENABLE_IPV6_UC) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (ARAD_KBP_ENABLE_IPV6_MC) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4_ANTI_SPOOFING_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV6_ENABLE(unit)) {
        num_of_dynamic_tables++;
    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6_ANTI_SPOOFING_KAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)) {
        num_of_dynamic_tables++;
        num_of_dynamic_tables++; 
    } else if ((table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS) || (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - kaps table_id is disabled")));
    }

    
    if (num_of_dynamic_tables > (JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_NUM - 1)){
        num_of_dynamic_tables_mc = num_of_dynamic_tables - (JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_NUM - 1);
    }

    
    while (num_of_dynamic_tables > 0) {
        dynamic_tables_additional_bits++;
        num_of_dynamic_tables = num_of_dynamic_tables/2;
    }

    if (dynamic_tables_additional_bits > dynamic_tables_additional_bits_max) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - number of dynamic table prefixes exceeds the max")));
    }

    if (dynamic_tables_additional_bits <= JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_BIT) {
        ipv4uc_dynamic_tables_additional_bits = dynamic_tables_additional_bits;
        ipv4mc_dynamic_tables_additional_bits = 0;
    } else {
        ipv4uc_dynamic_tables_additional_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_BIT;
        
        while (num_of_dynamic_tables_mc > 0) {
            ipv4mc_dynamic_tables_additional_bits++;
            num_of_dynamic_tables_mc = num_of_dynamic_tables_mc/2;
        }
    }

    switch (table_id) {
    
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS:     
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS: 
        *table_prefix = (JER_KAPS_IPV4_UC_AND_NON_IP_TABLE_PREFIX << ipv4uc_dynamic_tables_additional_bits);
        *table_prefix_len =  JER_KAPS_TABLE_PREFIX_LENGTH + ipv4uc_dynamic_tables_additional_bits;
        break;
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS:     
        *table_prefix = (JER_KAPS_IPV4_MC_TABLE_PREFIX << ipv4mc_dynamic_tables_additional_bits);
        *table_prefix_len = JER_KAPS_TABLE_PREFIX_LENGTH + ipv4mc_dynamic_tables_additional_bits;
        break;
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE:
        *table_prefix = JER_KAPS_IPV6_UC_TABLE_PREFIX;
        *table_prefix_len = JER_KAPS_TABLE_PREFIX_LENGTH;
        break;
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS:
        *table_prefix = JER_KAPS_IPV6_UC_TABLE_PREFIX;
        *table_prefix_len = JER_KAPS_TABLE_PREFIX_LENGTH;
        break;
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC:
        *table_prefix = JER_KAPS_IPV6_MC_TABLE_PREFIX;
        *table_prefix_len = JER_KAPS_TABLE_PREFIX_LENGTH;
        break;
    
    
    case SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS:
        if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS:
        if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID:
        if (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_KAPS_LPM) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID:
        if (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS:
        if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS:
        if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS:
        if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS:
        if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS:
        if (SOC_DPP_L3_SRC_BIND_IPV4_OR_ARP_ENABLE(unit)) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS:
        if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit)) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS:
        if (ARAD_KBP_ENABLE_IPV4_UC) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS:
        if (ARAD_KBP_ENABLE_IPV4_MC) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS:
        if (ARAD_KBP_ENABLE_IPV4_DC) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS:
        if (ARAD_KBP_ENABLE_IPV6_UC) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS:
        if (ARAD_KBP_ENABLE_IPV6_MC) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV4_ANTI_SPOOFING_KAPS:
        if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6_ANTI_SPOOFING_KAPS:
        if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV6_ENABLE(unit)) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS:
        if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)) {
            dynamic_table_prefix++;
        }
    case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS:
        if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)) {
            dynamic_table_prefix++;
        }
    
    if (dynamic_table_prefix != 0) {
        if (dynamic_table_prefix <= (JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_NUM - 1)) { 
           *table_prefix = (JER_KAPS_IPV4_UC_AND_NON_IP_TABLE_PREFIX << ipv4uc_dynamic_tables_additional_bits) + dynamic_table_prefix;
            *table_prefix_len =  JER_KAPS_TABLE_PREFIX_LENGTH + ipv4uc_dynamic_tables_additional_bits;
        } else { 
            dynamic_table_prefix -= (JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_NUM - 1);
            if (dynamic_table_prefix > (JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_NUM - 1)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - number of dynamic table prefixes exceeds the max")));
            }
            *table_prefix = (JER_KAPS_IPV4_MC_TABLE_PREFIX << ipv4mc_dynamic_tables_additional_bits) + dynamic_table_prefix;
            *table_prefix_len = JER_KAPS_TABLE_PREFIX_LENGTH + ipv4mc_dynamic_tables_additional_bits;
        }
        break;
    }
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kaps_table_prefix_get - invalid kaps table_id")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 *is_dynamic)
{
    uint32 table_prefix;
    uint32 table_prefix_len;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, table_id, &table_prefix, &table_prefix_len));

    if (table_prefix_len == JER_KAPS_TABLE_PREFIX_LENGTH) {
        *is_dynamic = 0;
    } else {
        *is_dynamic = 1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
arad_pp_flp_dbal_l3_mac_learning_lem_custom_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_L3_MAC_LEARNING_CUSTOM_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_SA;
        qual_info[0].qual_offset = 32;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_SA;
        qual_info[1].qual_offset = 16;
        qual_info[1].qual_nof_bits = 16;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_SA;
        qual_info[2].qual_offset = 0;
        qual_info[2].qual_nof_bits = 16;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_FID;
        qual_info[3].qual_offset = 0;
        qual_info[3].qual_nof_bits = 15;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_L3_MAC_LEARNING_CUSTOM_LEM,
        ARAD_PP_FLP_ETH_KEY_OR_MASK(unit), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, 0, qual_info, "FLP: FLP IPv4 UC MACSA CUSTOM_LEM"));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_source_lookup_with_aget_access_enable(int unit, int prog_id)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA
      flp_lookups_tbl;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl));

    flp_lookups_tbl.lem_1st_lkp_key_type   = 1;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl));
exit:
     SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_oam_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_OPCODE;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL;


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS, DBAL_PREFIX_NOT_DEFINED, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP oam statistics LEM"));

    
    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_oam_down_untagged_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;


    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_UNTAGGED;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS, DBAL_PREFIX_NOT_DEFINED, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP OAM down untagged statistics LEM"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,&keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
arad_pp_flp_dbal_bfd_statistics_program_tables_init(
    int unit,
    int prog_id
    ) {
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = { 0 };
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 user_header_0_size;
    uint32 user_header_1_size;
    uint32 user_header_egress_pmf_offset_0;
    uint32 user_header_egress_pmf_offset_1;


    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_IPV4;

    SOCDNX_IF_ERR_EXIT(arad_pmf_db_fes_user_header_sizes_get(
                unit,
                &user_header_0_size,
                &user_header_1_size,
                &user_header_egress_pmf_offset_0,
                &user_header_egress_pmf_offset_1
              ));


    qual_info[0].ignore_qual_offset_for_entry_mngmnt = 1;
    qual_info[0].qual_offset = user_header_0_size + user_header_1_size;


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "FLP BFD statistics LEM"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_pp_flp_dbal_oam_single_tag_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_SINGLE_TAG;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS, DBAL_PREFIX_NOT_DEFINED, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP OAM sinngle tag statistics LEM"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_oam_double_tag_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_DOUBLE_TAG;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_TM_INNER_TAG;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS, DBAL_PREFIX_NOT_DEFINED, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP OAM double tag statistics LEM"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,&keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
   arad_pp_flp_dbal_bfd_mpls_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    uint32 user_header_0_size;
    uint32 user_header_1_size;
    uint32 user_header_egress_pmf_offset_0;
    uint32 user_header_egress_pmf_offset_1;

    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

    SOCDNX_IF_ERR_EXIT(arad_pmf_db_fes_user_header_sizes_get(
                unit,
                &user_header_0_size,
                &user_header_1_size,
                &user_header_egress_pmf_offset_0,
                &user_header_egress_pmf_offset_1
              ));

    qual_info[0].ignore_qual_offset_for_entry_mngmnt = 1;
    qual_info[0].qual_offset = user_header_0_size + user_header_1_size;
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_MPLS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "FLP BFD MPLS statistics LEM"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,&keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_bfd_pwe_statistics_program_tables_init(
     int unit,
	 int prog_id
   )
{
    uint32 user_header_0_size;
    uint32 user_header_1_size;
    uint32 user_header_egress_pmf_offset_0;
    uint32 user_header_egress_pmf_offset_1;

    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

    SOCDNX_IF_ERR_EXIT(arad_pmf_db_fes_user_header_sizes_get(
            unit,
            &user_header_0_size,
            &user_header_1_size,
            &user_header_egress_pmf_offset_0,
            &user_header_egress_pmf_offset_1
          ));

    qual_info[0].ignore_qual_offset_for_entry_mngmnt = 1;
    qual_info[0].qual_offset = user_header_0_size + user_header_1_size;
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_PWE;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "FLP BFD PWE statistics LEM"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
   arad_pp_flp_dbal_bfd_echo_program_tables_init(
     int unit
   )
{

    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);


  


    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_TTL;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL;



    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_ECHO_LEM, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_ECHO, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_ECHO, qual_info, "FLP BFD ECHO LEM"));

    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id.lookup_number = 2;
    keys_to_table_id.sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_BFD_ECHO_LEM;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV4UC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_flp_ethernet_ing_ivl_learn_tables_create(
        int unit,
        int prog_id)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
    qual_info[0].qual_offset = 16;
    qual_info[0].qual_nof_bits = 32;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
    qual_info[1].qual_offset = 0;
    qual_info[1].qual_nof_bits = 16;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
    qual_info[2].qual_offset = 0;
    qual_info[2].qual_nof_bits = 15;

    qual_info[3].qual_type = SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1;
    qual_info[3].qual_offset = 0;
    qual_info[3].qual_nof_bits = 12;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IVL_LEARN_LEM, (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IVL_LEARN_LEM(unit)), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_IVL_LEARN, qual_info, "FLP L2 Learn DB, IVL Learn Mode LEM"));

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
    qual_info[0].qual_offset = 16;
    qual_info[0].qual_nof_bits = 32;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
    qual_info[1].qual_offset = 0;
    qual_info[1].qual_nof_bits = 16;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
    qual_info[2].qual_offset = 0;
    qual_info[2].qual_nof_bits = 15;

    qual_info[3].qual_type = SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1;
    qual_info[3].qual_offset = 0;
    qual_info[3].qual_nof_bits = 12;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_LEM, (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IVL_LEARN_LEM(unit)), 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_IVL_LEARN, qual_info, "FLP L2 Fwd DB,IVL Learn Mode"));

   
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IVL_LEARN_LEM;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_LEM;
    keys_to_table_id[1].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                   keys_to_table_id, NULL, 2));

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_source_lookup_with_aget_access_enable(unit, prog_id));
exit:
     SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_flp_ethernet_ing_ivl_inner_learn_tables_create(
        int unit,
        int prog_id)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
    qual_info[0].qual_offset = 16;
    qual_info[0].qual_nof_bits = 32;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
    qual_info[1].qual_offset = 0;
    qual_info[1].qual_nof_bits = 16;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
    qual_info[2].qual_offset = 0;
    qual_info[2].qual_nof_bits = 15;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_INNERMOST_VLAN_TAG_ID;
    qual_info[3].qual_offset = 0;
    qual_info[3].qual_nof_bits = 12;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_LEARN_LEM, (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IVL_LEARN_LEM(unit)), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_IVL_LEARN, qual_info, "FLP L2 Learn DB, IVL INNER VLAN-TAG LEM Learn Mode"));

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
    qual_info[0].qual_offset = 16;
    qual_info[0].qual_nof_bits = 32;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
    qual_info[1].qual_offset = 0;
    qual_info[1].qual_nof_bits = 16;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
    qual_info[2].qual_offset = 0;
    qual_info[2].qual_nof_bits = 15;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_INNERMOST_VLAN_TAG_ID;
    qual_info[3].qual_offset = 0;
    qual_info[3].qual_nof_bits = 12;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_FWD_LEM, (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IVL_LEARN_LEM(unit)), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_IVL_LEARN, qual_info, "FLP L2 Fwd DB, IVL INNER VLAN-TAG LEM Learn Mode"));

   
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_LEARN_LEM;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_FWD_LEM;
    keys_to_table_id[1].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                   keys_to_table_id, NULL, 2));

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_source_lookup_with_aget_access_enable(unit, prog_id));


exit:
     SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_flp_ethernet_ing_ivl_fwd_outer_learn_tables_create(
        int unit,
        int prog_id)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
    qual_info[0].qual_offset = 16;
    qual_info[0].qual_nof_bits = 32;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
    qual_info[1].qual_offset = 0;
    qual_info[1].qual_nof_bits = 16;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
    qual_info[2].qual_offset = 0;
    qual_info[2].qual_nof_bits = 15;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG_ID;
    qual_info[3].qual_offset = 0;
    qual_info[3].qual_nof_bits = 12;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_OUTER_LEARN_LEM, (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IVL_LEARN_LEM(unit)), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_IVL_LEARN, qual_info, "FLP L2 Fwd DB, IVL FWD OUTER VLAN-TAG LEM Learn Mode"));

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
    qual_info[0].qual_offset = 16;
    qual_info[0].qual_nof_bits = 32;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
    qual_info[1].qual_offset = 0;
    qual_info[1].qual_nof_bits = 16;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
    qual_info[2].qual_offset = 0;
    qual_info[2].qual_nof_bits = 15;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG_ID;
    qual_info[3].qual_offset = 0;
    qual_info[3].qual_nof_bits = 12;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_OUTER_FWD_LEM, (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IVL_LEARN_LEM(unit)), 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_IVL_LEARN, qual_info, "FLP L2 Fwd DB, IVL FWD OUTER VLAN-TAG LEM Fwd Mode"));

   
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_OUTER_LEARN_LEM;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_OUTER_FWD_LEM;
    keys_to_table_id[1].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                   keys_to_table_id, NULL, 2));

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_source_lookup_with_aget_access_enable(unit, prog_id));
exit:
     SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4uc_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM, ARAD_PP_FLP_IPV4_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST, qual_info, "FLP IPv4 UC LEM"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_pon_ipv4_sav_lem_table_create(int unit, uint8 is_not_arp, uint8 is_arp_up, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, table_id, &is_table_initiated));
    if (!is_table_initiated) {
        
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = (is_not_arp ? SOC_PPC_FP_QUAL_HDR_IPV4_SIP : (is_arp_up ? SOC_PPC_FP_QUAL_ARP_SENDER_IP4 : SOC_PPC_FP_QUAL_ARP_TARGET_IP4));
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_FID;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, table_id, ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK, qual_info, "FLP IPv4 SAV STATIC LEM"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_ipv4uc_lem_custom_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_CUSTOM_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_SA;
        qual_info[0].qual_offset = 16;
        qual_info[0].qual_nof_bits = 32;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_SA;
        qual_info[1].qual_offset = 0;
        qual_info[1].qual_nof_bits = 16;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG;
        qual_info[2].qual_offset = 4;
        qual_info[2].qual_nof_bits = 12;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT;
        qual_info[3].qual_offset = 0;
        qual_info[3].qual_nof_bits = 12;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_CUSTOM_LEM, ARAD_PP_FLP_MAC_IN_MAC_TUNNEL_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC_IN_MAC_TUNNEL, qual_info, "FLP IPv4 UC CUSTOM_LEM"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4uc_lem_route_scale_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) > 16    &&
            (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) % 8 > 0) {
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[qualifiers_counter].qual_offset = 16;
            qual_info[qualifiers_counter++].qual_nof_bits = (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) - 16;
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[qualifiers_counter++].qual_nof_bits = 16;
        } else {
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[qualifiers_counter++].qual_nof_bits = (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length);
        }
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM, DBAL_PREFIX_NOT_DEFINED, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, qualifiers_counter, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv4 UC SCALE ROUTE LEM"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6uc_lem_route_scale_long_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0, qualifiers_counter = 0;
    uint32 db_prefix = 0;
    uint32 db_prefix_len = 0;
    uint32 additional_info = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        if (((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) > 32) {
            
            if (((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) - 32 > 16    &&
                ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) % 8 > 0) {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 16 + 32;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) - 32 - 16;
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 32;
                qual_info[qualifiers_counter++].qual_nof_bits = 16;
            } else {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 32;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) - 32;
            }
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[qualifiers_counter++].qual_nof_bits = 32;
        } else {
            
            if (((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) > 16    &&
                ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) % 8 > 0) {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 16;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) - 16;
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter++].qual_nof_bits = 16;
            } else {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long));
            }
        }
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;

        db_prefix = (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long == 64) ? ARAD_PP_LEM_ACCESS_KEY_TYPE_IP6_64_IN_LEM : DBAL_PREFIX_NOT_DEFINED;
        db_prefix_len = (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long == 64) ? ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV6_64_IN_LEM : ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
        additional_info = (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long == 64) ? ARAD_PP_LEM_ACCESS_KEY_TYPE_IP6_64_IN_LEM : SOC_DPP_DBAL_ATI_NONE;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM, db_prefix, db_prefix_len,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, qualifiers_counter, additional_info, qual_info, "FLP IPv6 UC SCALE ROUTE LEM LONG"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6uc_lem_route_scale_short_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        if (((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) > 32) {
            
            if (((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) - 32 > 16    &&
                ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) % 8 > 0) {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 16 + 32;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) - 32 - 16;
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 32;
                qual_info[qualifiers_counter++].qual_nof_bits = 16;
            } else {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 32;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) - 32;
            }
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[qualifiers_counter++].qual_nof_bits = 32;
        } else {
            
            if (((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) > 16    &&
                ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) % 8 > 0) {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter].qual_offset = 16;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short)) - 16;
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter++].qual_nof_bits = 16;
            } else {
                qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
                qual_info[qualifiers_counter++].qual_nof_bits = ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short));
            }
        }
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM, DBAL_PREFIX_NOT_DEFINED, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, qualifiers_counter, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 UC SCALE ROUTE LEM SHORT"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4mc_bridge_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
        qual_info[0].qual_offset = 16;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
        qual_info[1].qual_offset = 4;
        qual_info[1].qual_nof_bits = 12;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM, ARAD_PP_FLP_IPV4_COMP_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC, qual_info, "FLP IPv4 MC LEM"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4mc_Learning_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEARN_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
        qual_info[0].qual_offset = 32;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
        qual_info[1].qual_offset = 0;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
        qual_info[2].qual_offset = 0;
        qual_info[2].qual_nof_bits = 15;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEARN_LEM,
        ARAD_PP_FLP_ETH_KEY_OR_MASK(unit), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, 0, qual_info, "FLP: IPV4 MC Learning"));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_ipv4uc_rpf_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM, ARAD_PP_FLP_IPV4_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, 0, qual_info, "FLP IPv4 UC RPF LEM"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4uc_default_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT, ARAD_PP_FLP_IPV4_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, 0, qual_info, "FLP IPv4 UC default LEM "));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4mc_bridge_tcam_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix_len = 0;

    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID, &is_table_initiated));
    if (!is_table_initiated) {


        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID, DBAL_PREFIX_NOT_DEFINED, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 3, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv4 MC FID TCAM"));
    }



exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6uc_tcam_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;


    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, &is_table_initiated));
    if (!is_table_initiated) {

        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;  
        qual_info[0].qual_offset = 32;
        qual_info[0].qual_nof_bits = 32;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;  
        qual_info[1].qual_offset = 0;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;  
        qual_info[2].qual_offset = 48;
        qual_info[2].qual_nof_bits = 16;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;  
        qual_info[3].qual_offset = 32;
        qual_info[3].qual_nof_bits = 16;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;  
        qual_info[4].qual_offset = 0;
        qual_info[4].qual_nof_bits = 32;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[5].qual_offset = 0;
        qual_info[5].qual_nof_bits = 8;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE,
                DBAL_PREFIX_NOT_DEFINED, 0, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM,6, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 UC TCAM"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

uint32
    arad_pp_flp_dbal_fcoe_kaps_table_create(int unit, int is_vsan_from_vsi)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, &table_prefix, &table_prefix_len));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, &is_table_initiated));
    if (is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_destroy(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS));
    }

    if (is_vsan_from_vsi) {

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 27 - table_prefix_len; 
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[2].qual_nof_bits = 8;
        qual_info[2].qual_offset = 24;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_nof_bits = 1;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI;
        qual_info[4].qual_nof_bits = 12;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 5, 0, qual_info, "FLP FCoE VSI KAPS"));
    } else{
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 27 - table_prefix_len; 
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[2].qual_nof_bits = 8;
        qual_info[2].qual_offset = 24;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI;
        qual_info[3].qual_nof_bits = 13;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 4, 0, qual_info, "FLP FCoE VFT KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_fcoe_npv_kaps_table_create(int unit, int is_vsan_from_vsi)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, &table_prefix, &table_prefix_len));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, &is_table_initiated));
    if (is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_destroy(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS));
    }

    if (is_vsan_from_vsi) {

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 27 - table_prefix_len; 
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[2].qual_nof_bits = 8;
        qual_info[2].qual_offset = 24;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_nof_bits = 1;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI;
        qual_info[4].qual_nof_bits = 12;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 5, 0, qual_info, "FLP FCoE NPV VSI KAPS"));
    } else{
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 27 - table_prefix_len; 
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[2].qual_nof_bits = 8;
        qual_info[2].qual_offset = 24;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI;
        qual_info[3].qual_nof_bits = 13;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 4, 0, qual_info, "FLP FCoE NPV VFT KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_ipv4uc_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qualifiers_counter++].qual_nof_bits = table_prefix_len == JER_KAPS_TABLE_PREFIX_LENGTH ? 32 : 28; 
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        if (table_prefix_len != JER_KAPS_TABLE_PREFIX_LENGTH) { 
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv4 UC KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_pon_ipv4_sav_kaps_table_create(int unit, uint8 is_not_arp, uint8 is_arp_up, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, table_id, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        if (SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_OR_ARP_ENABLE(unit))
        {
            
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[0].qual_nof_bits = (33 - table_prefix_len);
            qual_info[1].qual_type = (is_not_arp ? SOC_PPC_FP_QUAL_HDR_IPV4_SIP : (is_arp_up ? SOC_PPC_FP_QUAL_ARP_SENDER_IP4 : SOC_PPC_FP_QUAL_ARP_TARGET_IP4));;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, table_id, table_prefix, table_prefix_len,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 3, 0, qual_info, "FLP IPV4 SAV STATIC KAPS"));
        }
        else
        {
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[0].qual_nof_bits = (30 - table_prefix_len);
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, table_id, table_prefix, table_prefix_len,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 3, 0, qual_info, "FLP IPV4 SAV STATIC KAPS"));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_pon_uni_v6_static_kaps_table_create(int unit)
{
     SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
     int is_table_initiated = 0;
     uint32 table_prefix, table_prefix_len;

     SOCDNX_INIT_FUNC_DEFS;
     SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS, &is_table_initiated));
     if (!is_table_initiated) {

          SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS, &table_prefix, &table_prefix_len));
          DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
          
          qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;  
          qual_info[0].qual_offset = 32;
          qual_info[0].qual_nof_bits = 32;
          qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;  
          qual_info[1].qual_offset = 0;
          qual_info[1].qual_nof_bits = 32;
          qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;  
          qual_info[2].qual_offset = 48;
          qual_info[2].qual_nof_bits = 16;
          
          qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;  
          qual_info[3].qual_offset = 32;
          qual_info[3].qual_nof_bits = 16;
          qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;  
          qual_info[4].qual_offset = 0;
          qual_info[4].qual_nof_bits = 32;
          qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
          qual_info[5].qual_nof_bits = 18;
          qual_info[6].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
          qual_info[6].qual_nof_bits = 10;

          SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS,
          table_prefix, table_prefix_len,SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS,7, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP:TK_EPON_UNI_V6_STATIC_KAPS"));
     }
exit:
     SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4uc_kaps_route_scale_long_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qualifiers_counter++].qual_nof_bits = table_prefix_len == JER_KAPS_TABLE_PREFIX_LENGTH ? 32 : 28; 
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        if (table_prefix_len != JER_KAPS_TABLE_PREFIX_LENGTH) { 
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv4 UC SCALE LONG KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4uc_kaps_route_scale_short_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len, qualifiers_counter = 0;
    uint32 dip_qual_size = (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        if (dip_qual_size < 28) {
            
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = 80 - 20  - 32 - dip_qual_size;
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = 32;
        } else {
            
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = 80 - 20  - dip_qual_size;
        }
        
        if ((dip_qual_size > 16) && (dip_qual_size % 8 > 0)) {
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[qualifiers_counter].qual_offset = 16;
            qual_info[qualifiers_counter++].qual_nof_bits = dip_qual_size - 16;
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[qualifiers_counter++].qual_nof_bits = 16;
        } else {
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[qualifiers_counter++].qual_nof_bits = dip_qual_size;
        }
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv4 UC SCALE SHORT KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6uc_kaps_route_scale_long_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[qualifiers_counter].qual_nof_bits = 32;
        qual_info[qualifiers_counter++].qual_offset = 32;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[qualifiers_counter++].qual_nof_bits = 32;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[qualifiers_counter].qual_nof_bits = 16;
        qual_info[qualifiers_counter++].qual_offset = 48;

        
        
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qualifiers_counter++].qual_nof_bits = 12;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[qualifiers_counter].qual_offset = 32;
        qual_info[qualifiers_counter++].qual_nof_bits = 16;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[qualifiers_counter++].qual_nof_bits = 32;
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        if (table_prefix_len != JER_KAPS_TABLE_PREFIX_LENGTH) { 
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv6 UC SCALE LONG KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6uc_kaps_route_scale_short_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[qualifiers_counter].qual_offset = 48;
        qual_info[qualifiers_counter++].qual_nof_bits = 12;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[qualifiers_counter].qual_offset = 32;
        qual_info[qualifiers_counter++].qual_nof_bits = 16;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[qualifiers_counter++].qual_nof_bits = 32;
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv6 UC SCALE SHORT KAPS"));
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_stage_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, 0, 0 ));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4mc_bridge_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len;
    uint32 qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID, &is_table_initiated));
    if (!is_table_initiated) {

        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        if (table_prefix_len < 5) {
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = 5 - table_prefix_len; 
        }
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
        qual_info[qualifiers_counter].qual_offset = 16;
        qual_info[qualifiers_counter++].qual_nof_bits = 16;
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
        qual_info[qualifiers_counter].qual_offset = 4;
        qual_info[qualifiers_counter++].qual_nof_bits = 12;
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_FID;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv4 MC FID KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_ipv4mc_bridge_kaps_table_create_ivl(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len;
    SOCDNX_INIT_FUNC_DEFS;


    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID, &table_prefix, &table_prefix_len));

    
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[1].qual_offset = 0;
    qual_info[1].qual_nof_bits = 16;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP;


    
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[3].qual_nof_bits = 25 - table_prefix_len; 
    qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
    qual_info[4].qual_offset = 16;
    qual_info[4].qual_nof_bits = 16;
    qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
    qual_info[5].qual_offset = 4;
    qual_info[5].qual_nof_bits = 12;
    qual_info[6].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_INNERMOST_VLAN_TAG_ID;
    qual_info[6].qual_offset = 0;
    qual_info[6].qual_nof_bits = 12;
    qual_info[7].qual_type = SOC_PPC_FP_QUAL_FID;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID, &is_table_initiated));
    if (!is_table_initiated) {

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 8, 0, qual_info, "FLP IPv4 MC IVL KAPS"));
    }


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID_UNTAGGED, &is_table_initiated));
    if (!is_table_initiated) {

        qual_info[6].qual_type = SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1;
        qual_info[6].qual_offset = 0;
        qual_info[6].qual_nof_bits = 12;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID_UNTAGGED, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 8, 0, qual_info, "FLP IPv4 MC UNTAGGED IVL KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4uc_anti_spoofing_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_ANTI_SPOOFING_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_ANTI_SPOOFING_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qualifiers_counter++].qual_nof_bits = table_prefix_len == JER_KAPS_TABLE_PREFIX_LENGTH ? 28 : 24; 
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
        if (table_prefix_len != JER_KAPS_TABLE_PREFIX_LENGTH) { 
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_ANTI_SPOOFING_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv4 UC ANTI-SPOOFING KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4uc_rpf_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len, qualifiers_counter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qualifiers_counter++].qual_nof_bits = table_prefix_len == JER_KAPS_TABLE_PREFIX_LENGTH ? 32 : 28; 
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        if (table_prefix_len != JER_KAPS_TABLE_PREFIX_LENGTH) { 
            qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter, 0, qual_info, "FLP IPv4 UC RPF KAPS"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#endif
#if defined(INCLUDE_KBP)
uint32
    arad_pp_flp_dbal_ipv4uc_kbp_table_create(int unit, int is_rpf, int is_public)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MASTER_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_nof_bits = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MASTER_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 4, SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION, qual_info, "FLP IPv4 UC MASTER KBP"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[2].qual_nof_bits = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 3, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv4 UC FWD KBP"));
    }

    if(is_public) {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP, &is_table_initiated));
        if (!is_table_initiated) {
            DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0, 0,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 1, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv4 UC PUBLIC FWD KBP"));
        }
    }

    if (is_rpf) {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KBP, &is_table_initiated));
        if (!is_table_initiated) {
            DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[2].qual_nof_bits = 2;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, 0,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 3, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv4 UC RPF KBP"));
        }

        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_DUMMY_KBP, &is_table_initiated));
        if (!is_table_initiated) {
            DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[1].qual_nof_bits = 2;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_DUMMY_KBP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC, 0,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 2, SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE, qual_info, "FLP IPv4 UC DUMMY KBP"));
        }

        if(is_public) {
            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_KBP, &is_table_initiated));
            if (!is_table_initiated) {
                DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1, 0,
                                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 1, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv4 UC PUBLIC RPF KBP"));
            }

            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_DUMMY_KBP, &is_table_initiated));
            if (!is_table_initiated) {
                DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
                qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                qual_info[1].qual_nof_bits = 2;
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_DUMMY_KBP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC_PUBLIC, 0,
                                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 2, SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE, qual_info, "FLP IPv4 UC DUMMY KBP"));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6uc_kbp_table_create(int unit, int is_rpf, int is_public)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_MASTER_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[0].qual_nof_bits = 32;
        qual_info[0].qual_offset = 32;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[2].qual_nof_bits = 16;
        qual_info[2].qual_offset = 48;
        
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[3].qual_nof_bits = 16;
        qual_info[3].qual_offset = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[4].qual_nof_bits = 32;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[5].qual_nof_bits = 32;
        qual_info[5].qual_offset = 32;
        
        qual_info[6].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[6].qual_nof_bits = 32;
        qual_info[7].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[7].qual_nof_bits = 32;
        qual_info[7].qual_offset = 32;
        qual_info[8].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[8].qual_nof_bits = 16;
        qual_info[8].qual_offset = 16;
        
        qual_info[9].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[9].qual_nof_bits = 16;
        qual_info[10].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[11].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[11].qual_nof_bits = 2;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_MASTER_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 12, SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION, qual_info, "FLP IPv6 UC MASTER KBP"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[0].qual_nof_bits = 32;
        qual_info[0].qual_offset = 32;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[2].qual_nof_bits = 32;
        qual_info[2].qual_offset = 32;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[3].qual_nof_bits = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[5].qual_nof_bits = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 6, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv6 UC FWD KBP"));
    }

    if(is_public) {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_FWD_KBP, &is_table_initiated));
        if (!is_table_initiated) {
            DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
            qual_info[0].qual_nof_bits = 32;
            qual_info[0].qual_offset = 32;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
            qual_info[1].qual_nof_bits = 32;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[2].qual_nof_bits = 32;
            qual_info[2].qual_offset = 32;
            qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[3].qual_nof_bits = 32;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_FWD_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0, 0,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 4, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv6 UC PUBLIC FWD KBP"));
        }
    }

    if (is_rpf) {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KBP, &is_table_initiated));
        if (!is_table_initiated) {
            DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
            qual_info[0].qual_nof_bits = 32;
            qual_info[0].qual_offset = 32;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
            qual_info[1].qual_nof_bits = 32;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
            qual_info[2].qual_nof_bits = 32;
            qual_info[2].qual_offset = 32;
            qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
            qual_info[3].qual_nof_bits = 32;
            qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[5].qual_nof_bits = 2;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, 0,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 6, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv6 UC RPF KBP"));
        }

        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_DUMMY_KBP, &is_table_initiated));
        if (!is_table_initiated) {
            DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[1].qual_nof_bits = 2;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_DUMMY_KBP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC, 0,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 2, SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE, qual_info, "FLP IPv4 UC DUMMY KBP"));
        }

        if(is_public) {
            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_KBP, &is_table_initiated));
            if (!is_table_initiated) {
                 DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                 qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
                 qual_info[0].qual_nof_bits = 32;
                 qual_info[0].qual_offset = 32;
                 qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
                 qual_info[1].qual_nof_bits = 32;
                 qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
                 qual_info[2].qual_nof_bits = 32;
                 qual_info[2].qual_offset = 32;
                 qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
                 qual_info[3].qual_nof_bits = 32;
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1, 0,
                                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 4, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv6 UC PUBLIC RPF KBP"));
            }

            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_DUMMY_KBP, &is_table_initiated));
            if (!is_table_initiated) {
                DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
                qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                qual_info[1].qual_nof_bits = 2;
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_DUMMY_KBP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC_PUBLIC, 0,
                                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 2, SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE, qual_info, "FLP IPv6 UC PUBLIC DUMMY KBP"));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_ipv6uc_kbp_sip_sharing_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP_SIP_SHARING, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[0].qual_nof_bits = 32;
        qual_info[0].qual_offset = 32;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[2].qual_nof_bits = 16;
        qual_info[2].qual_offset = 48;
        
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[3].qual_nof_bits = 16;
        qual_info[3].qual_offset = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[4].qual_nof_bits = 32;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP_SIP_SHARING, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 5, SOC_DPP_DBAL_ATI_KBP_FLP_CE_ALLOC_ONLY, qual_info, "FLP IPv6 UC FWD KBP SIP Sharing"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4mc_kbp_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        if (ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[3].qual_nof_bits = 2;
            qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
            qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[5].qual_nof_bits = 1;
        } else {
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[1].qual_nof_bits = 1;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
            qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[5].qual_nof_bits = 2;
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 6, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv4 MC FWD KBP"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_DUMMY_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[1].qual_nof_bits = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_DUMMY_KBP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 2, SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE, qual_info, "FLP IPv4 MC DUMMY KBP"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_RPF_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[2].qual_nof_bits = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_RPF_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 3, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv4 MC RPF KBP"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6mc_kbp_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    int nof_qualifiers = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_FWD_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        SOC_PPC_FP_QUAL_TYPE
            first_qual_high,
            first_qual_low,
            second_qual_high,
            second_qual_low;
        if(ARAD_KBP_IPV6MC_INVERT_GROUP_SOURCE) {
            first_qual_high = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
            first_qual_low = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
            second_qual_high = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            second_qual_low = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        } else {
            first_qual_high = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            first_qual_low = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
            second_qual_high = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
            second_qual_low = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        }
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        if (ARAD_KBP_IPV6_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
            
            qual_info[0].qual_type = second_qual_low;
            qual_info[0].qual_nof_bits = 32;
            qual_info[0].qual_offset = 32;
            qual_info[1].qual_type = second_qual_low;
            qual_info[1].qual_nof_bits = 32;
            qual_info[2].qual_type = second_qual_high;
            qual_info[2].qual_nof_bits = 16;
            qual_info[2].qual_offset = 48;
            
            qual_info[3].qual_type = second_qual_high;
            qual_info[3].qual_nof_bits = 16;
            qual_info[3].qual_offset = 32;
            qual_info[4].qual_type = second_qual_high;
            qual_info[4].qual_nof_bits = 32;
            qual_info[5].qual_type = first_qual_low;
            qual_info[5].qual_nof_bits = 32;
            qual_info[5].qual_offset = 32;
            
            qual_info[6].qual_type = first_qual_low;
            qual_info[6].qual_nof_bits = 32;
            qual_info[7].qual_type = first_qual_high;
            qual_info[7].qual_nof_bits = 16;
            qual_info[7].qual_offset = 48;
            qual_info[8].qual_type = first_qual_high;
            qual_info[8].qual_nof_bits = 32;
            qual_info[8].qual_offset = 16;
            
            qual_info[9].qual_type = first_qual_high;
            qual_info[9].qual_nof_bits = 16;
            qual_info[10].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[11].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[11].qual_nof_bits = 2;
            qual_info[12].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
            qual_info[13].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[13].qual_nof_bits = 1;

            nof_qualifiers = 14;
        } else {
            
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[1].qual_nof_bits = 1;
            qual_info[2].qual_type = second_qual_low;
            qual_info[2].qual_nof_bits = 32;
            qual_info[2].qual_offset = 32;
            qual_info[3].qual_type = second_qual_low;
            qual_info[3].qual_nof_bits = 32;
            
            qual_info[4].qual_type = second_qual_high;
            qual_info[4].qual_nof_bits = 32;
            qual_info[4].qual_offset = 32;
            qual_info[5].qual_type = second_qual_high;
            qual_info[5].qual_nof_bits = 32;
            qual_info[6].qual_type = first_qual_low;
            qual_info[6].qual_nof_bits = 16;
            qual_info[6].qual_offset = 48;
            
            qual_info[7].qual_type = first_qual_low;
            qual_info[7].qual_nof_bits = 16;
            qual_info[7].qual_offset = 32;
            qual_info[8].qual_type = first_qual_low;
            qual_info[8].qual_nof_bits = 32;
            qual_info[9].qual_type = first_qual_high;
            qual_info[9].qual_nof_bits = 32;
            qual_info[9].qual_offset = 32;
            
            qual_info[10].qual_type = first_qual_high;
            qual_info[10].qual_nof_bits = 32;
            qual_info[11].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
            qual_info[12].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[12].qual_nof_bits = 2;

            nof_qualifiers = 13;
        }

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_FWD_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, nof_qualifiers, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 MC FWD KBP"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_DUMMY_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[1].qual_nof_bits = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_DUMMY_KBP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 2, SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE, qual_info, "FLP IPv6 MC DUMMY KBP"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_RPF_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_nof_bits = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_RPF_KBP, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 4, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv6 MC RPF KBP"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_lsr_kbp_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_KBP, &is_table_initiated));
    if(!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
        qual_info[0].qual_offset = 4;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
        qual_info[1].qual_offset = 0;
        qual_info[1].qual_nof_bits = 4;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[2].qual_nof_bits = 28;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_KBP, ARAD_KBP_FRWRD_TBL_ID_LSR, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 3, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP LSR KBP"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_kbp_default_route_kaps_table_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS route_table)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix = DBAL_PREFIX_NOT_DEFINED, table_prefix_len = 0, qual_count = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if ((route_table != SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS) &&
        (route_table != SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS) &&
        (route_table != SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS) &&
        (route_table != SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS) &&
        (route_table != SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_kbp_default_route_kaps_table_create - invalid table id")));
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, route_table, &is_table_initiated));
    if(!is_table_initiated) {
        char table_name[40];

        
        switch(route_table) {
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS:
            sal_strcpy(table_name, "FLP IPv4 UC KBP DEFAULT ROUTE KAPS");
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS:
            sal_strcpy(table_name, "FLP IPv4 MC KBP DEFAULT ROUTE KAPS");
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS:
            sal_strcpy(table_name, "FLP IPv4 DC KBP DEFAULT ROUTE KAPS");
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS:
            sal_strcpy(table_name, "FLP IPv6 UC KBP DEFAULT ROUTE KAPS");
            break;
        default: 
            sal_strcpy(table_name, "FLP IPv6 MC KBP DEFAULT ROUTE KAPS");
            break;
        }

#ifdef BCM_88675_A0
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, route_table, &table_prefix, &table_prefix_len));
#endif 

        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[qual_count].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qual_count++].qual_nof_bits = 32;
        qual_info[qual_count].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qual_count++].qual_nof_bits = 28;
        qual_info[qual_count].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[qual_count++].qual_nof_bits = 14;
        qual_info[qual_count].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[qual_count++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, route_table, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qual_count, SOC_DPP_DBAL_ATI_NONE, qual_info,
                                                     table_name));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv4mc_bridge_kbp_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    int num_tables = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID, &is_table_initiated));
    if (!is_table_initiated) {

        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        qual_info[num_tables].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP;
        qual_info[num_tables++].qual_nof_bits = 32;
        qual_info[num_tables].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
        qual_info[num_tables].qual_offset = 16;
        qual_info[num_tables++].qual_nof_bits = 16;
        qual_info[num_tables].qual_type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;
        qual_info[num_tables].qual_offset = 4;
        qual_info[num_tables++].qual_nof_bits = 12;
        qual_info[num_tables].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[num_tables++].qual_nof_bits = 4; 
        qual_info[num_tables].qual_type = SOC_PPC_FP_QUAL_FID;
        qual_info[num_tables++].qual_nof_bits = 15;
        qual_info[num_tables].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[num_tables++].qual_nof_bits = 1; 
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID, ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, num_tables, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv4 MC FID KBP"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#endif


uint32
    arad_pp_flp_dbal_ipv4mc_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0, nof_qual;
    uint32 table_prefix = DBAL_PREFIX_NOT_DEFINED, table_prefix_len = 0;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    int qualifiers_counter = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, &is_table_initiated));
    if (!is_table_initiated) {

        if (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) {

            nof_qual = (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam == ARAD_PP_FLP_L3_MC_USE_TCAM_NO_IPV4_VRF) ? 4 : 5;

            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[0].qual_nof_bits = 1;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
            qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;

            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, table_prefix, table_prefix_len,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, nof_qual, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv4 MC TCAM"));
        }else{
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
            if(JER_KAPS_ENABLE(unit)){

                SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, &table_prefix, &table_prefix_len));
                if (SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1) {
                    
                    qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                    qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                    qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                    qual_info[qualifiers_counter].qual_nof_bits = 16;
                    qual_info[qualifiers_counter++].qual_offset = 16;
                    
                    qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                    qual_info[qualifiers_counter++].qual_nof_bits = 16;
                    qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
                    qual_info[qualifiers_counter].qual_offset = 16;
                    qual_info[qualifiers_counter++].qual_nof_bits = 16;
                    qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
                    qual_info[qualifiers_counter].qual_offset = 4;
                    qual_info[qualifiers_counter++].qual_nof_bits = 12;
                    qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
                    qual_info[qualifiers_counter++].qual_nof_bits = 14;
                    qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                    if (table_prefix_len != JER_KAPS_TABLE_PREFIX_LENGTH) { 
                        qual_info[qualifiers_counter++].qual_nof_bits = 5 - (table_prefix_len - JER_KAPS_TABLE_PREFIX_LENGTH);
                    } else {
                        qual_info[qualifiers_counter++].qual_nof_bits = 5;
                    }
                    qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
                    qual_info[qualifiers_counter++].qual_nof_bits = 15;
                } else {
                    if (table_prefix_len == JER_KAPS_TABLE_PREFIX_LENGTH) { 
                        
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter++].qual_nof_bits = 4;
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
                        qual_info[qualifiers_counter++].qual_nof_bits = 12;

                        
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
                        qual_info[qualifiers_counter].qual_offset = 12;
                        qual_info[qualifiers_counter++].qual_nof_bits = 3;
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter++].qual_nof_bits = 1;
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
                        qual_info[qualifiers_counter].qual_offset = 16;
                        qual_info[qualifiers_counter++].qual_nof_bits = 16;
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
                        qual_info[qualifiers_counter].qual_offset = 4;
                        qual_info[qualifiers_counter++].qual_nof_bits = 12;
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
                    } else { 
                        
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter++].qual_nof_bits = 1;
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
                        qual_info[qualifiers_counter++].qual_nof_bits = 15;

                        
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
                        qual_info[qualifiers_counter].qual_offset = 16;
                        qual_info[qualifiers_counter++].qual_nof_bits = 16;
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
                        qual_info[qualifiers_counter].qual_offset = 4;
                        qual_info[qualifiers_counter++].qual_nof_bits = 12;
                        qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
                        
                        qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
                        qual_info[qualifiers_counter++].qual_nof_bits = JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH - table_prefix_len;
                    }
                }
                nof_qual = qualifiers_counter;
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, table_prefix, table_prefix_len,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, nof_qual, 0, qual_info, "FLP IPv4 MC KAPS"));
            }
#endif
        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}


#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
uint32
    arad_pp_flp_dbal_ipv6uc_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, &table_prefix, &table_prefix_len));
         DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[1].qual_offset = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
        qual_info[2].qual_nof_bits = 32;

        
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[3].qual_nof_bits = 32;
        qual_info[3].qual_offset = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
        qual_info[4].qual_nof_bits = 32;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "FLP IPv6 UC KAPS"));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint32 table_prefix, table_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS, &table_prefix, &table_prefix_len));
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[1].qual_offset = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
        qual_info[2].qual_nof_bits = 32;

        
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[3].qual_nof_bits = 32;
        qual_info[3].qual_offset = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
        qual_info[4].qual_nof_bits = 32;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS, table_prefix, table_prefix_len,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "FLP IPv6 UC RPF KAPS"));


    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_ipv6uc_anti_spoofing_kaps_table_create(int unit)
{
     SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
     int is_table_initiated = 0;
     uint32 table_prefix, table_prefix_len;

     SOCDNX_INIT_FUNC_DEFS;
     SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6_ANTI_SPOOFING_KAPS, &is_table_initiated));
     if (!is_table_initiated) {

          SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6_ANTI_SPOOFING_KAPS, &table_prefix, &table_prefix_len));
          DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
          
          qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;  
          qual_info[0].qual_offset = 32;
          qual_info[0].qual_nof_bits = 32;
          qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;  
          qual_info[1].qual_offset = 0;
          qual_info[1].qual_nof_bits = 32;
          qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;  
          qual_info[2].qual_offset = 48;
          qual_info[2].qual_nof_bits = 16;
          
          qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;  
          qual_info[3].qual_offset = 32;
          qual_info[3].qual_nof_bits = 16;
          qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;  
          qual_info[4].qual_offset = 0;
          qual_info[4].qual_nof_bits = 32;
          qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
          qual_info[5].qual_nof_bits = 18;
          qual_info[6].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
          qual_info[6].qual_nof_bits = 14 - table_prefix_len;

          SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6_ANTI_SPOOFING_KAPS,
          table_prefix, table_prefix_len,SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS,7, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 UC ANTI-SPOOFING KAPS"));
     }
exit:
     SOCDNX_FUNC_RETURN;
}

#endif 

uint32
    arad_pp_flp_dbal_ipv6mc_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC, &is_table_initiated));
    if (!is_table_initiated) {

        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {

            qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
            qual_info[0].qual_nof_bits = 32;
            qual_info[0].qual_offset = 32;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
            qual_info[1].qual_nof_bits = 32;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[2].qual_nof_bits = 32;
            qual_info[2].qual_offset = 32;
            qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[3].qual_nof_bits = 32;
            qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;

            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC, DBAL_PREFIX_NOT_DEFINED, 0,
                                                                    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 5, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 MC TCAM"));

        } else {
            
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            qual_info[0].qual_nof_bits = 1;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
            qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
            qual_info[2].qual_nof_bits = 32;
            qual_info[2].qual_offset = 32;
            qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
            qual_info[3].qual_nof_bits = 32;

            
            qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[4].qual_nof_bits = 32;
            qual_info[4].qual_offset = 32;
            qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
            qual_info[5].qual_nof_bits = 32;
            qual_info[6].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;


             if (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) {

                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC, DBAL_PREFIX_NOT_DEFINED, 0,
                                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 7, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 MC TCAM"));
            } else {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

                uint32 table_prefix = 0, table_prefix_len = 0;
                SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC, &table_prefix, &table_prefix_len));

                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC, table_prefix, table_prefix_len,
                                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 7, 0, qual_info, "FLP IPv6 MC KAPS"));
#endif
            }
        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
uint32
    arad_pp_flp_dbal_ipv6mc_ssm_compressed_sip_table_create(int unit)
{
     SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
     int is_table_initiated = 0;
     uint32 table_prefix = 0;
     uint32 table_prefix_len = 0;
     uint32 qualifiers_counter = 0;

     SOCDNX_INIT_FUNC_DEFS;

     SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS,
                                                        &is_table_initiated));
     if (!is_table_initiated) {

          SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS,
                                                                    &table_prefix, &table_prefix_len));
          DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
          
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
          qual_info[qualifiers_counter++].qual_nof_bits = 1;
          qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;  
          qual_info[qualifiers_counter].qual_offset = 32;
          qual_info[qualifiers_counter++].qual_nof_bits = 32;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;  
          qual_info[qualifiers_counter].qual_offset = 0;
          qual_info[qualifiers_counter++].qual_nof_bits = 32;
          
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;  
          qual_info[qualifiers_counter].qual_offset = 32;
          qual_info[qualifiers_counter++].qual_nof_bits = 32;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;  
          qual_info[qualifiers_counter].qual_offset = 0;
          qual_info[qualifiers_counter++].qual_nof_bits = 32;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
          qual_info[qualifiers_counter++].qual_nof_bits = 16 - table_prefix_len;

          SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS,
                             table_prefix, table_prefix_len, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter,
                             SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 MC SSM compressed sip KAPS"));
     }
exit:
     SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6mc_ssm_table_create(int unit)
{
     SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
     int is_table_initiated = 0;
     uint32 table_prefix = 0;
     uint32 table_prefix_len = 0;
     uint32 qualifiers_counter = 0;

     SOCDNX_INIT_FUNC_DEFS;

     SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS,
                                                        &is_table_initiated));
     if (!is_table_initiated) {

          SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS,
                                                                    &table_prefix, &table_prefix_len));
          DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
          
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IPV6_COMPRESSED_SIP; 
          qual_info[qualifiers_counter].qual_nof_bits = 16;
          qual_info[qualifiers_counter].qual_offset = 0;
          qual_info[qualifiers_counter++].qual_is_in_hdr = 1;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
          qual_info[qualifiers_counter].qual_nof_bits = 32;
          qual_info[qualifiers_counter++].qual_offset = 32;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
          qual_info[qualifiers_counter].qual_nof_bits = 32;
          qual_info[qualifiers_counter++].qual_offset = 0;

          
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
          qual_info[qualifiers_counter].qual_nof_bits = 32;
          qual_info[qualifiers_counter++].qual_offset = 24;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
          qual_info[qualifiers_counter++].qual_nof_bits = 24;
          qual_info[qualifiers_counter++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
          qual_info[qualifiers_counter].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
          qual_info[qualifiers_counter++].qual_nof_bits = 10 - table_prefix_len;

          SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS,
                             table_prefix, table_prefix_len, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, qualifiers_counter,
                             SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv6 MC SSM KAPS"));
     }
exit:
     SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6mc_ssm_compress_sip_program_tables_init(
        int unit,
        int prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6mc_ssm_compressed_sip_table_create(unit));

    keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS;
    keys_to_table_id[nof_tables++].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id,
                          SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, nof_tables));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6mc_ssm_program_tables_init(
        int unit,
        int prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables = 0;
    uint8 is_dynamic = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6mc_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6mc_ssm_table_create(unit));

    keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC;
    keys_to_table_id[nof_tables].lookup_number = 2;
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
    keys_to_table_id[nof_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

    keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS;
    keys_to_table_id[nof_tables].lookup_number = 1;
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
    keys_to_table_id[nof_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id,
                          SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, nof_tables));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
   arad_pp_flp_dbal_fcoe_program_tables_init(int unit, int is_vsan_from_vsi, int fcoe_no_vft_prog_id, int fcoe_vft_prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    uint32 vsi_vft_instuction = 0, did_instuction = 0;
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, vft_pos, no_vft_pos, vsi_vft_inst_id, dip_inst_id;
    soc_mem_t flp_key_construction_mem;
    uint32 data[ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
    uint32 data2[ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;

    SOCDNX_INIT_FUNC_DEFS;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_fcoe_kaps_table_create(unit, is_vsan_from_vsi));
#endif

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B_MSB;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS;
    keys_to_table_id[0].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, fcoe_vft_prog_id , SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, 1));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, fcoe_no_vft_prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, 1));

    

    soc_sand_os_memset(data, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));
    soc_sand_os_memset(data2, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, &table));

    if (is_vsan_from_vsi == 0) {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D_13_BITS;
    } else {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_VSI(12);
    }

    if(table.table_programs[0].program_id == fcoe_no_vft_prog_id){
        no_vft_pos = 0;
        vft_pos = 1;
    }else{
        no_vft_pos = 1;
        vft_pos = 0;
    }

    

    vsi_vft_inst_id = -1;
    dip_inst_id = -1;
    for (i = 0; i < table.nof_qualifiers; i++) {
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
            dip_inst_id = table.table_programs[no_vft_pos].ce_assigned[i];
        }
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI) {
            vsi_vft_inst_id = table.table_programs[no_vft_pos].ce_assigned[i];
        }
    }

    if (vsi_vft_inst_id == -1 || dip_inst_id == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_fcoe_program_tables_init - instruction not found 1")));
    }

    flp_key_construction_mem = IHP_FLP_KEY_CONSTRUCTION_MSBm;

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_no_vft_prog_id,data));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_no_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF,data2));

    
    did_instuction = ARAD_PP_FLP_16B_INST_ARAD_FC_D_ID_8_MSB;
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);

    
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_no_vft_prog_id, data ));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_no_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, data2 ));

    

     if (is_vsan_from_vsi == 0) {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_ARAD_FC_WITH_VFT_VFT_ID;
    }
    ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_D_ID_8_MSB

    soc_sand_os_memset(data, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));
    soc_sand_os_memset(data2, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));

    vsi_vft_inst_id = -1;
    dip_inst_id = -1;
    for (i = 0; i < table.nof_qualifiers; i++) {
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
            dip_inst_id = table.table_programs[vft_pos].ce_assigned[i];
        }
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI) {
            vsi_vft_inst_id = table.table_programs[vft_pos].ce_assigned[i];
        }
    }

    if (vsi_vft_inst_id == -1 || dip_inst_id == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_fcoe_program_tables_init - instruction not found 1")));
    }

    flp_key_construction_mem = IHP_FLP_KEY_CONSTRUCTION_MSBm;

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_vft_prog_id,data));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF,data2));

    
    did_instuction = ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_D_ID_8_MSB;
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);

    
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_vft_prog_id, data ));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, data2 ));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_fcoe_npv_program_tables_init(int unit, int is_vsan_from_vsi, int fcoe_no_vft_prog_id, int fcoe_vft_prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    uint32 vsi_vft_instuction = 0, did_instuction = 0;
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, vft_pos, no_vft_pos, vsi_vft_inst_id, dip_inst_id;
    soc_mem_t flp_key_construction_mem;
    uint32 data[ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
    uint32 data2[ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;

    SOCDNX_INIT_FUNC_DEFS;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_fcoe_npv_kaps_table_create(unit, is_vsan_from_vsi));
#endif

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B_MSB;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS;
    keys_to_table_id[0].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, fcoe_vft_prog_id , SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, 1));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, fcoe_no_vft_prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, 1));

    

    soc_sand_os_memset(data, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));
    soc_sand_os_memset(data2, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, &table));

    if (is_vsan_from_vsi == 0) {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D_13_BITS;
    } else {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_P6_VSI(12);
    }

    if(table.table_programs[0].program_id == fcoe_no_vft_prog_id){
        no_vft_pos = 0;
        vft_pos = 1;
    }else{
        no_vft_pos = 1;
        vft_pos = 0;
    }

    

    vsi_vft_inst_id = -1;
    dip_inst_id = -1;
    for (i = 0; i < table.nof_qualifiers; i++) {
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP) {
            dip_inst_id = table.table_programs[no_vft_pos].ce_assigned[i];
        }
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI) {
            vsi_vft_inst_id = table.table_programs[no_vft_pos].ce_assigned[i];
        }
    }

    if (vsi_vft_inst_id == -1 || dip_inst_id == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_fcoe_program_tables_init - instruction not found 1")));
    }

    flp_key_construction_mem = IHP_FLP_KEY_CONSTRUCTION_MSBm;

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_no_vft_prog_id,data));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_no_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF,data2));

    
    did_instuction = ARAD_PP_FLP_16B_INST_ARAD_FC_S_ID_8_MSB;
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);

    
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_no_vft_prog_id, data ));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_no_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, data2 ));

    

     if (is_vsan_from_vsi == 0) {
        vsi_vft_instuction = ARAD_PP_FLP_16B_INST_ARAD_FC_WITH_VFT_VFT_ID;
    }
    ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_D_ID_8_MSB

    soc_sand_os_memset(data, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));
    soc_sand_os_memset(data2, 0x0, (ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE) * sizeof(uint32));

    vsi_vft_inst_id = -1;
    dip_inst_id = -1;
    for (i = 0; i < table.nof_qualifiers; i++) {
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP) {
            dip_inst_id = table.table_programs[vft_pos].ce_assigned[i];
        }
        if (table.qual_info[i].qual_type == SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI) {
            vsi_vft_inst_id = table.table_programs[vft_pos].ce_assigned[i];
        }
    }

    if (vsi_vft_inst_id == -1 || dip_inst_id == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_flp_dbal_fcoe_program_tables_init - instruction not found 1")));
    }

    flp_key_construction_mem = IHP_FLP_KEY_CONSTRUCTION_MSBm;

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_vft_prog_id,data));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit,flp_key_construction_mem,MEM_BLOCK_ANY,fcoe_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF,data2));

    
    did_instuction = ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_S_ID_8_MSB;
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, dip_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), did_instuction);

    
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);
    soc_mem_field32_set(unit, flp_key_construction_mem, data2, arad_pmf_ce_instruction_fld_get(unit,stage, vsi_vft_inst_id - (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1)), vsi_vft_instuction);

    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_vft_prog_id, data ));
    SOCDNX_SAND_IF_ERR_EXIT(soc_mem_write(unit, flp_key_construction_mem, MEM_BLOCK_ANY, fcoe_vft_prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, data2 ));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_mpls_lsr_stat_table_create(int unit)
{
     SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
     int is_table_initiated = 0;
     char *propval;
     int nof_qual=2;

     SOCDNX_INIT_FUNC_DEFS;
     SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_CNT_LEM, &is_table_initiated));

     if (!is_table_initiated) {
          DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

          qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
          qual_info[0].qual_offset = 16;
          qual_info[0].qual_nof_bits = 4;
          qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
          qual_info[1].qual_offset = 0;
          qual_info[1].qual_nof_bits = 16;

          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lsr_stat_with_context", 0)) {
              propval = soc_property_get_str(unit, spn_MPLS_CONTEXT);
              nof_qual=3;
              if (propval && sal_strcmp(propval, "port") == 0) {
                  
                  qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT;
              } else if(propval && sal_strcmp(propval, "interface") == 0){
                  
                  qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
              } else if(propval && sal_strcmp(propval, "vrf") == 0){
                  
                  qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
              }
          }
          SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_CNT_LEM,
          ARAD_PP_FLP_LSR_CNT_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
          SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, nof_qual, ARAD_PP_LEM_ACCESS_KEY_LSR_CUNT, qual_info, "FLP LSR STAT LEM"));
     }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_mpls_lsr_stat_table_init(int unit)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id_static[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_mpls_lsr_stat_table_create(unit));
    keys_to_table_id_static[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B; 
    keys_to_table_id_static[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_LSR_CNT_LEM;
    keys_to_table_id_static[0].lookup_number =  1;  

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_LSR, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
    keys_to_table_id_static, NULL, 1 ));

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
   arad_pp_flp_dbal_ipv4uc_rpf_program_tables_init(
     int unit
   )
{
    int nof_tables, table_index = 0;
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_lem_table_create(unit));
    }
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_RPF){    
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kbp_table_create(unit,1,0));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS));
    }else {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)) {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_anti_spoofing_kaps_table_create(unit));
        } else {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_kaps_table_create(unit));
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif
    }
#endif 

    if (!SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)) {
        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
        keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM;
        keys_to_table_id[table_index++].lookup_number = 1;
    }
    keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[table_index++].lookup_number = 2;

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_RPF){    
        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MASTER_KBP;
        keys_to_table_id[table_index].lookup_number = 1;
        keys_to_table_id[table_index++].public_lpm_lookup_size = 0;

        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP;
        keys_to_table_id[table_index].lookup_number = 0;
        keys_to_table_id[table_index++].public_lpm_lookup_size = 0;

        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KBP;
        keys_to_table_id[table_index].lookup_number = 2;
        keys_to_table_id[table_index++].public_lpm_lookup_size = 0;

        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_DUMMY_KBP;
        keys_to_table_id[table_index].lookup_number = 1;
        keys_to_table_id[table_index++].public_lpm_lookup_size = 0;

        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
        keys_to_table_id[table_index].lookup_number = 1;
        keys_to_table_id[table_index++].public_lpm_lookup_size = 0;
    }else
#endif
    {
        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV4_ENABLE(unit)) {
            keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_ANTI_SPOOFING_KAPS;
        } else {
            keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS;
        }
        keys_to_table_id[table_index].lookup_number = 1;
        keys_to_table_id[table_index++].public_lpm_lookup_size = 0;

        keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B_MSB;
        keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
        keys_to_table_id[table_index].lookup_number = 2;
        keys_to_table_id[table_index++].public_lpm_lookup_size = 0;
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mac_learning_enable", 0)) {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_l3_mac_learning_lem_custom_table_create(unit));
            keys_to_table_id[table_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
            keys_to_table_id[table_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_L3_MAC_LEARNING_CUSTOM_LEM;
            keys_to_table_id[table_index++].lookup_number = 1;
        }

    }
    nof_tables = table_index;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV4UC_RPF, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, nof_tables));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, PROG_FLP_IPV4UC_RPF, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
    arad_pp_flp_dbal_trill_program_tcam_tables_init(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table[1] = {{ 0 }};
    SOC_PPC_FP_DATABASE_STAGE stage;
    uint8 qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    int prog[2] = {PROG_FLP_TRILL_MC_TWO_TAGS, PROG_FLP_TRILL_MC_ONE_TAG};
    int iter, rv;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(qualifier_to_ce_id, 0, sizeof(qualifier_to_ce_id));
    stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;

    SOCDNX_IF_ERR_EXIT(
        arad_pp_dbal_table_is_initiated(unit,
                                        SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM,
                                        &is_table_initiated));

    
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_TRILL_EGRESS_NICK;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI;

        SOCDNX_IF_ERR_EXIT(
            arad_pp_dbal_table_create(unit,
                                      SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM,
                                      DBAL_PREFIX_NOT_DEFINED, 0,
                                      SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 3,
                                      SOC_DPP_DBAL_ATI_NONE, qual_info,
                                      "Trill Tcam"));
    }

    keys_to_table[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM;
    keys_to_table[0].lookup_number = 0;

    qualifier_to_ce_id[0][0] = 3;
    qualifier_to_ce_id[0][1] = 2;
    qualifier_to_ce_id[0][2] = 1;
    for (iter = 0; iter < 3; iter++) {
        qualifier_to_ce_id[0][iter] += ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP;
    }

    for (iter = 0; iter < 2; iter++) {
        rv = arad_pp_dbal_program_to_tables_associate_implicit(unit,
                prog[iter], stage, keys_to_table,
                qualifier_to_ce_id, 1);
        if (rv != SOC_E_NONE) {
            SOCDNX_EXIT_WITH_ERR(rv,
                (_BSL_SOCDNX_MSG("Error! Associating program %d to table %d"
                                 "failed while creating DBAL table for"
                                 "trill mc in stage %d"),
                                 prog[iter],
                                 SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM,
                                 SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP));
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_vpws_tagged_program_tables_init(int unit)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint8 prog_id;
    int is_single_tagged, tid_key_index;
    SOC_DPP_DBAL_SW_TABLE_IDS table_id;
    uint32 nof_qual;

    SOCDNX_INIT_FUNC_DEFS;

    for (is_single_tagged=1; is_single_tagged>=0; is_single_tagged--) {
        tid_key_index = 0;
        prog_id = is_single_tagged ? PROG_FLP_VPWS_TAGGED_SINGLE_TAG : PROG_FLP_VPWS_TAGGED_DOUBLE_TAG;
        table_id = is_single_tagged ? SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_SINGLE_TAG : SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_DOUBLE_TAG;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, table_id, &is_table_initiated));
        if (!is_table_initiated) {
            DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
            qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
            qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG_ID;
            if (is_single_tagged) {
                nof_qual = 2;
            }
            else {
                
                qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_VLAN_2ND_TAG_ID;
                nof_qual = 3;
            }

            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, table_id,
                                                         DBAL_PREFIX_NOT_DEFINED, 0,
                                                         SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, nof_qual, SOC_DPP_DBAL_ATI_NONE, qual_info,
                                                         is_single_tagged ? "FLP VPWS Tagged single tag" : "FLP VPWS Tagged double tag"));
        }

        keys_to_table_id[tid_key_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
        keys_to_table_id[tid_key_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_SINGLE_TAG;
        keys_to_table_id[tid_key_index].lookup_number = 1;
        tid_key_index++;

        if (!is_single_tagged) {
            keys_to_table_id[tid_key_index].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
            keys_to_table_id[tid_key_index].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_DOUBLE_TAG;
            keys_to_table_id[tid_key_index].lookup_number = 2;
            tid_key_index++;
        }

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, tid_key_index));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_lsr_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 nof_qual = 0;
    int is_table_initiated = 0;
    int i = 0;
    char *propval;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_LEM, &is_table_initiated));
    if(!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[i].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
        qual_info[i].qual_offset = 4;
        qual_info[i++].qual_nof_bits = 16;
        qual_info[i].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
        qual_info[i].qual_offset = 0;
        qual_info[i++].qual_nof_bits = 4;
        qual_info[i++].qual_type = SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_MPLS_EXP;

        propval = soc_property_get_str(unit, spn_MPLS_CONTEXT);
        if (propval && sal_strcmp(propval, "port") == 0) {
            
            qual_info[i++].qual_type = SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT;
        } else if(propval && sal_strcmp(propval, "interface") == 0){
            
            qual_info[i++].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        } else if(propval && sal_strcmp(propval, "vrf") == 0){
            
            qual_info[i++].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        }

        nof_qual = i;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_LEM, ARAD_PP_FLP_LSR_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, nof_qual, ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM, qual_info, "FLP LSR"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_lsr_program_tables_init(int unit)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables;
    uint8 prog_id;

    SOCDNX_INIT_FUNC_DEFS;

    prog_id = PROG_FLP_LSR;

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_MPLS) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_lsr_kbp_table_create(unit));

        keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
        keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_LSR_KBP;
        keys_to_table_id[0].lookup_number = 0;

        nof_tables = 1;
    }
    else
#endif 
    {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_lsr_lem_table_create(unit));

        keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
        keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_LSR_LEM;
        keys_to_table_id[0].lookup_number = 2;

        nof_tables = 1;
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, nof_tables));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_oam_outlif_l2g_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 nof_qual = 0;
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_OUTLIF_L2G_LEM, &is_table_initiated));
    if(!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_DSP_EXTENSION_AFTER_FTMH;    
        qual_info[0].qual_offset = 0;
        qual_info[0].qual_nof_bits = 16;
        qual_info[0].qual_is_in_hdr = 1;

        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FTMH;                        
        qual_info[1].qual_offset = 69;
        qual_info[1].qual_nof_bits = 3;
        qual_info[1].qual_is_in_hdr = 1;

        nof_qual = 2;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OUTLIF_L2G_LEM, ARAD_PP_FLP_OAM_OUTLIF_MAP_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
            SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, nof_qual, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_OULIF_MAP, qual_info, "FLP Outlif L2G LEM"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_two_pass_mc_encap_to_dest_lem_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 nof_qual = 0;
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM, &is_table_initiated));
    if(!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FTMH;   
        qual_info[1].qual_offset = 49;
        qual_info[1].qual_nof_bits = 3;
        qual_info[1].qual_is_in_hdr = 1;

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FTMH;   
        qual_info[0].qual_offset = 52;
        qual_info[0].qual_nof_bits = 16;
        qual_info[0].qual_is_in_hdr = 1;

        nof_qual = 2;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM, ARAD_PP_FLP_MC_ENCAP_MAP_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
            SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, nof_qual, ARAD_PP_LEM_ACCESS_KEY_TYPE_MC_ENCAP_MAP, qual_info, "FLP LIF TO DEST LEM"));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_oam_outlif_l2g_program_tables_init(
        int unit,
        int prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_oam_outlif_l2g_lem_table_create(unit));

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_OUTLIF_L2G_LEM;
    keys_to_table_id[0].lookup_number = 1;

    nof_tables = 1;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, nof_tables));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_two_pass_mc_encap_to_dest_program_tables_init(
        int unit,
        int prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_two_pass_mc_encap_to_dest_lem_table_create(unit));

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM;
    keys_to_table_id[0].lookup_number = 1;

    nof_tables = 1;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, nof_tables));

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
uint32
    arad_pp_flp_dbal_ipv4_dc_program_tables_init(int unit)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
		qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[1].qual_nof_bits = 8;
		SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC, ARAD_KBP_FRWRD_TBL_ID_IPV4_DC, 0,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 2, SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY, qual_info, "FLP IPv4 DOUBLE CAPACITY"));
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_MASTER, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
		qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[2].qual_nof_bits = 8;
		SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_MASTER, ARAD_KBP_FRWRD_TBL_ID_IPV4_DC, 0,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 3, SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION, qual_info, "FLP IPv4 DOUBLE CAPACITY MASTER"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_DUMMY_KBP, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
		qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[0].qual_nof_bits = 8;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_DUMMY_KBP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 1, SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE, qual_info, "FLP IPv4 DC DUMMY KBP"));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));
    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS));

	keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_MASTER;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC;
    keys_to_table_id[1].lookup_number = 0;

    keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC;
    keys_to_table_id[2].lookup_number = 2;

    keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_DUMMY_KBP;
    keys_to_table_id[3].lookup_number = 1;

    keys_to_table_id[4].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[4].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[4].lookup_number = 2;

    keys_to_table_id[5].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
    keys_to_table_id[5].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS;
    keys_to_table_id[5].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV4_DC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, 6));

exit:
    SOCDNX_FUNC_RETURN;
}

#endif 


uint32
   arad_pp_flp_dbal_ipv4uc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int table_id = 0, num_of_tables = 2;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_UC)
    {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kbp_table_create(unit, 0, 0));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS));
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MASTER_KBP;
        num_of_tables = 4;

        keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP;
        keys_to_table_id[2].lookup_number = 0;
        keys_to_table_id[2].public_lpm_lookup_size = 0;

        keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
        keys_to_table_id[3].lookup_number = 2;
        keys_to_table_id[3].public_lpm_lookup_size = 0;

    } else {
#if defined(BCM_88675_A0)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
        if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_route_scale_table_create(unit));
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_route_scale_short_table_create(unit));
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_route_scale_long_table_create(unit));

        }
        else
        {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
        }
#endif 
}
#endif 
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[0].lookup_number = 2;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
    keys_to_table_id[1].sw_table_id = table_id;
    keys_to_table_id[1].lookup_number = 2;
    keys_to_table_id[1].public_lpm_lookup_size = 0;

    if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
        
        keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
        keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
        keys_to_table_id[0].lookup_number = 1;

        keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B_MSB;
        keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS;
        keys_to_table_id[1].lookup_number = 1;
        keys_to_table_id[1].public_lpm_lookup_size = 0;

        keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
        keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM;
        keys_to_table_id[2].lookup_number = 2;

        keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS;
        keys_to_table_id[3].lookup_number = 2;
        keys_to_table_id[3].public_lpm_lookup_size = 0;

        num_of_tables = 4;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_custom_table_create(unit));
        keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
        keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_CUSTOM_LEM;
        keys_to_table_id[num_of_tables].lookup_number = 1;
        ++num_of_tables;
    }else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mac_learning_enable", 0)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_l3_mac_learning_lem_custom_table_create(unit));
        keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
        keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_L3_MAC_LEARNING_CUSTOM_LEM;
        keys_to_table_id[num_of_tables].lookup_number = 1;
        ++num_of_tables;
    }
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV4UC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                keys_to_table_id, NULL, num_of_tables));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, PROG_FLP_IPV4UC, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_pppoe_ipv4uc_anti_spoofing_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_IPV4_ANTI_SPOOFING, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_PPPOE_SESSION_ID;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_IPV4_ANTI_SPOOFING,
        ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK, ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_SPOOF_STATIC, qual_info, "FLP: PPPoE ANTI-SPOOFING LEM"));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_pppoe_ipv4uc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int num_of_tables = 4;

    SOCDNX_INIT_FUNC_DEFS;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_pon_ipv4_sav_kaps_table_create(unit, TRUE, FALSE, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_pppoe_ipv4uc_anti_spoofing_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B_MSB;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS;
    keys_to_table_id[0].lookup_number = 1;
    keys_to_table_id[0].public_lpm_lookup_size = 0;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_IPV4_ANTI_SPOOFING;
    keys_to_table_id[1].lookup_number = 1;

    keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[2].lookup_number = 2;

    keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
    keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    keys_to_table_id[3].lookup_number = 2;
    keys_to_table_id[3].public_lpm_lookup_size = 0;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_PPPOE_IPV4UC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                keys_to_table_id, NULL, num_of_tables));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_epon_uni_lem_default_learn_table_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_LEARN, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
        qual_info[0].qual_offset = 32;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_SA;
        qual_info[1].qual_offset = 0;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
        qual_info[2].qual_offset = 0;
        qual_info[2].qual_nof_bits = 15;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_LEARN,
        ARAD_PP_FLP_ETH_KEY_OR_MASK(unit), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, 0, qual_info, "FLP: TK_EPON_UNI_LEM_DEFAULT_LEARN"));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_epon_uni_lem_default_learn_table_forward(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_FORWARD, &is_table_initiated));
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
        qual_info[0].qual_offset = 32;
        qual_info[0].qual_nof_bits = 16;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_DA;
        qual_info[1].qual_offset = 0;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_FID;
        qual_info[2].qual_offset = 0;
        qual_info[2].qual_nof_bits = 15;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_FORWARD,
        ARAD_PP_FLP_ETH_KEY_OR_MASK(unit), ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, 0, qual_info, "FLP: TK_EPON_UNI_LEM_DEFAULT_FORWARD"));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_pon_ipv4_sav_static_program_tables_init(
     int unit, uint8 sa_auth_enabled,uint8 slb_enabled
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int num_of_tables = 2;

    SOCDNX_INIT_FUNC_DEFS;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_pon_ipv4_sav_kaps_table_create(unit, TRUE, FALSE, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS));
#endif 

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS;
    keys_to_table_id[0].lookup_number = 1;
    keys_to_table_id[0].public_lpm_lookup_size = 0;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_epon_uni_lem_default_learn_table_create(unit));
    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_LEARN;
    keys_to_table_id[1].lookup_number = ((!sa_auth_enabled && !slb_enabled)? 1 : 0);
    if (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)
    {
        if (sa_auth_enabled || slb_enabled)
        {
            SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
        }
        num_of_tables++;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_epon_uni_lem_default_learn_table_forward(unit));
        keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
        keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_FORWARD;
        keys_to_table_id[2].lookup_number = 1;
    }
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
    keys_to_table_id, NULL, num_of_tables));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC,
       SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_source_lookup_with_aget_access_enable(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_pon_sav_arp_program_tables_init(
     int unit, int is_up, int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int num_of_tables = 2;
    SOC_DPP_DBAL_SW_TABLE_IDS table_id = 0;

    SOCDNX_INIT_FUNC_DEFS;

    table_id = is_up ? SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS_ARP_REQ:SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS_ARP_RPY;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_pon_ipv4_sav_kaps_table_create(unit, FALSE, is_up, table_id));
#endif 
    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
    keys_to_table_id[0].sw_table_id = table_id;
    keys_to_table_id[0].lookup_number = (is_up ? 1 : 2);
    keys_to_table_id[0].public_lpm_lookup_size = 0;
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_epon_uni_lem_default_learn_table_create(unit));
    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_LEARN;
    keys_to_table_id[1].lookup_number = 1;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                keys_to_table_id, NULL, num_of_tables));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_source_lookup_with_aget_access_enable(unit, prog_id));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv4mc_bridge_program_tables_init(
     int unit
   )
{
    int ssm_lpm_key_id = 0;
    int nof_valid_keys = 2;
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_bridge_lem_table_create(unit));
    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_Learning_lem_table_create(unit));
    
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if((ARAD_KBP_ENABLE_IPV4_MC_BRIDGE) && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_ELK_LPM)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_bridge_kbp_table_create(unit));
        ssm_lpm_key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
    }

    if((JER_KAPS_ENABLE(unit)) && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_KAPS_LPM)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_bridge_kaps_table_create(unit));
        ssm_lpm_key_id = SOC_DPP_DBAL_PROGRAM_KEY_C_MSB;
    }
#endif

    if(SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_TCAM_LPM)
    {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_bridge_tcam_table_create(unit));
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if(SOC_IS_JERICHO(unit) && ARAD_KBP_IS_IN_USE) {
            
            ssm_lpm_key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
        }
        else {
#endif
            ssm_lpm_key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        }
#endif
    }

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEARN_LEM;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM;
    keys_to_table_id[1].lookup_number = 2;

    if (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode != BCM_IPMC_SSM_DISABLE) {
        keys_to_table_id[2].key_id = ssm_lpm_key_id;
        keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID;
        if (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_ELK_LPM) {
            keys_to_table_id[2].lookup_number = 0; 
        } else {
            keys_to_table_id[2].lookup_number = 2;
        }
        nof_valid_keys = 3;
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV4MC_BRIDGE, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                keys_to_table_id, NULL, nof_valid_keys));

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_source_lookup_with_aget_access_enable(unit, PROG_FLP_IPV4MC_BRIDGE));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv4mc_bridge_program_tables_init_ivl(
     int unit,
     int prog_id_untagged,
     int prog_id
   )
{
    int nof_valid_keys = 2;
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_bridge_lem_table_create(unit));
    
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if((JER_KAPS_ENABLE(unit)) ) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_bridge_kaps_table_create_ivl(unit));
    }
#endif

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID;
    keys_to_table_id[0].lookup_number = 2;
    nof_valid_keys = 1;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                keys_to_table_id, NULL, nof_valid_keys));

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID_UNTAGGED;
    keys_to_table_id[0].lookup_number = 2;
    nof_valid_keys = 1;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id_untagged, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                keys_to_table_id, NULL, nof_valid_keys));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_ipv4mc_tcam_tables_init(
     int unit,
     int mim_learning_prog_id
   )
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_table_initiated = 0;
    uint8 program_id = 0;
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = { { 0 } };

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) {
        program_id = (mim_learning_prog_id != -1) ? mim_learning_prog_id : PROG_FLP_IPV4COMPMC_WITH_RPF;
    } else {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_flp_app_to_prog_index_get(unit, PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF, &program_id));
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, &is_table_initiated));
    if (!is_table_initiated) {
      DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

      qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
      qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
      qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;

      SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, DBAL_PREFIX_NOT_DEFINED, 0,
                                      SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 3, SOC_DPP_DBAL_ATI_NONE, qual_info, "FLP IPv4 MC TCAM"));
    }

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
    keys_to_table_id[0].lookup_number = 2;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, program_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, 1));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_ipv4compmc_with_rpf_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables=0;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOC_DPP_DBAL_TABLE_INFO table;
    int nof_qualifiers=0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_lem_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM;
    keys_to_table_id[0].lookup_number = 1;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[1].lookup_number = 2;

    nof_tables = 2;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_MC){

        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_kbp_table_create(unit));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS));

        keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
        keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP;
        keys_to_table_id[2].lookup_number = 0;
        keys_to_table_id[2].public_lpm_lookup_size = 0;

        keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_DUMMY_KBP;
        keys_to_table_id[3].lookup_number = 1;
        keys_to_table_id[3].public_lpm_lookup_size = 0;

        keys_to_table_id[4].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[4].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_RPF_KBP;
        keys_to_table_id[4].lookup_number = 2;
        keys_to_table_id[4].public_lpm_lookup_size = 0;

        keys_to_table_id[5].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[5].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
        keys_to_table_id[5].lookup_number = 1;
        keys_to_table_id[5].public_lpm_lookup_size = 0;

        nof_tables = 6;

        
        SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS, &table));
        nof_qualifiers = table.nof_qualifiers;
        table.nof_qualifiers = 0;
        SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS, &table));
    }
    else{
        uint8 is_dynamic;

        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_kaps_table_create(unit));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_kaps_table_create(unit));

        keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS;
        keys_to_table_id[2].lookup_number = 1;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[2].sw_table_id, &is_dynamic));
        keys_to_table_id[2].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

        keys_to_table_id[3].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
        keys_to_table_id[3].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
        keys_to_table_id[3].lookup_number = 2;
        if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tcam_l3_mc", 0)) {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[3].sw_table_id, &is_dynamic));
            if (SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1) {
                keys_to_table_id[3].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_INRIF_IN_KEY;
            } else {
                keys_to_table_id[3].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);
            }
        }
        nof_tables = 4;
    }
#endif 

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, nof_tables));

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if (ARAD_KBP_ENABLE_IPV4_MC) {
        
        SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS, &table));
        table.nof_qualifiers = nof_qualifiers;
        SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS, &table));
    }
#endif

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, PROG_FLP_IPV4COMPMC_WITH_RPF, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv4compmc_with_mim_learning_program_tables_init(
     int unit,
     int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables=0;
    int lookup = 0;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    uint8 is_dynamic;
    SOC_DPP_DBAL_TABLE_INFO table;
    int nof_qualifiers=0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_custom_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_kaps_table_create(unit));

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    keys_to_table_id[lookup].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[lookup].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
    keys_to_table_id[lookup].lookup_number = 2;
    if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tcam_l3_mc", 0)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[lookup].sw_table_id, &is_dynamic));
        if (SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1) {
            keys_to_table_id[lookup].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_INRIF_IN_KEY;
        } else {
            keys_to_table_id[lookup].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);
        }
    }
    lookup++;
#endif

    keys_to_table_id[lookup].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
    keys_to_table_id[lookup].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[lookup].lookup_number = 2;
    lookup++;

    keys_to_table_id[lookup].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[lookup].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_CUSTOM_LEM;
    keys_to_table_id[lookup].lookup_number = 1;
    lookup++;


    nof_tables = lookup;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,keys_to_table_id, NULL, nof_tables));

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if (ARAD_KBP_ENABLE_IPV4_MC) {
        
        SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS, &table));
        table.nof_qualifiers = nof_qualifiers;
        SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS, &table));
    }
#endif

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv4uc_l3vpn_rpf_program_tables_init(
     int unit,
     int prog_id
   )
{
    uint8 is_dynamic = 0;
    int num_tables = 0;
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_lem_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit));

    keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM;
    keys_to_table_id[num_tables++].lookup_number = 1;

    keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[num_tables++].lookup_number = 2;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_RPF_PUBLIC && SOC_IS_JERICHO(unit)){
        
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kbp_table_create(unit,1,1));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS));

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MASTER_KBP;
        keys_to_table_id[num_tables].lookup_number = 1;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP;
        keys_to_table_id[num_tables].lookup_number = 0;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_DUMMY_KBP;
        keys_to_table_id[num_tables].lookup_number = 1;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_KBP;
        keys_to_table_id[num_tables].lookup_number = 2;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
        keys_to_table_id[num_tables].lookup_number = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
        keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

    } else
#endif 
    {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_rpf_kaps_table_create(unit));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif 

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS;
        keys_to_table_id[num_tables].lookup_number = 1;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
#endif 
        keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
        keys_to_table_id[num_tables].lookup_number = 2;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
#endif 
        keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);
    }
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, NULL, num_tables));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv4uc_l3vpn_program_tables_init(
     int unit)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int prog_id;
    uint8 is_dynamic = 0;
    int num_tables = 0;

    SOCDNX_INIT_FUNC_DEFS;


    prog_id = PROG_FLP_IPV4UC_PUBLIC;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_table_create(unit)); 


    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_default_lem_table_create(unit)); 
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif 
    keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT;
    keys_to_table_id[num_tables++].lookup_number = 1;


    keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
    keys_to_table_id[num_tables++].lookup_number = 2;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_UC_PUBLIC && SOC_IS_JERICHO(unit))
    {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kbp_table_create(unit, 0, 1));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS));

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MASTER_KBP;
        keys_to_table_id[num_tables].lookup_number = 1;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP;
        keys_to_table_id[num_tables].lookup_number = 0;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
        keys_to_table_id[num_tables].lookup_number = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
        keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

    } else
#endif 
    {
        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB; 
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
        keys_to_table_id[num_tables].lookup_number = DBAL_KAPS_2ND_LKP_DIASBLE;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
#endif 
        keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B_MSB; 
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
        keys_to_table_id[num_tables].lookup_number = 1;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, NULL, num_tables));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv6uc_l3vpn_program_tables_init(
     int unit,
     int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    uint8 is_dynamic = 0;
    int num_tables = 0;
    uint8 sw_prog_id = 0;

    SOCDNX_INIT_FUNC_DEFS;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_UC_PUBLIC && SOC_IS_JERICHO(unit))
    {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kbp_table_create(unit, 0, 1));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS));

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS;
        keys_to_table_id[num_tables].lookup_number = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
        keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_MASTER_KBP;
        keys_to_table_id[num_tables].lookup_number = 1;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_FWD_KBP;
        keys_to_table_id[num_tables].lookup_number = 0;
        keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;
    }
    else
#endif 
    {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_table_create(unit));
#endif 

        keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B; 
        keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
        keys_to_table_id[num_tables].lookup_number = 2;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
#endif 
        keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);
    }

    
    SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, prog_id, &sw_prog_id)); 
    if ((SOC_DPP_CONFIG(unit)->pp.custom_ip_route) && (sw_prog_id == PROG_FLP_IP6UC_CUSTOM_ROUTE)) {
       SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_custom_table_create(unit));
    
       keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
       keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_CUSTOM_LEM;
       keys_to_table_id[num_tables].lookup_number = 2;
       keys_to_table_id[num_tables++].public_lpm_lookup_size = 0;
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, NULL, num_tables));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv6uc_l3vpn_rpf_program_tables_init(
     int unit,
     int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables = 0;
    uint8 is_dynamic = 0;

    SOCDNX_INIT_FUNC_DEFS;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_UC_PUBLIC && SOC_IS_JERICHO(unit))
    {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kbp_table_create(unit, 1, 1));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS));

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS;
        keys_to_table_id[nof_tables].lookup_number = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_MASTER_KBP;
        keys_to_table_id[nof_tables].lookup_number = 1;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_FWD_KBP;
        keys_to_table_id[nof_tables].lookup_number = 0;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_KBP;
        keys_to_table_id[nof_tables].lookup_number = 2;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_DUMMY_KBP;
        keys_to_table_id[nof_tables].lookup_number = 1;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;
    } else
#endif 
    {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_table_create(unit));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(unit));
#endif

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS;
        keys_to_table_id[nof_tables].lookup_number = 1;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[0].sw_table_id, &is_dynamic));
#endif 
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
        keys_to_table_id[nof_tables].lookup_number = 2;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[1].sw_table_id, &is_dynamic));
#endif 
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, NULL, nof_tables));
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_ipv4uc_l3vpn_custom_program_tables_init(
     int unit,
     int custom_prgrm
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int prog_id;
    uint8 is_dynamic = 0;
    int num_tables = 0;

    SOCDNX_INIT_FUNC_DEFS;

    prog_id = custom_prgrm;

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_lem_custom_table_create(unit));

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_default_lem_table_create(unit)); 
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4uc_kaps_table_create(unit));
#endif 

    keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT;
    keys_to_table_id[num_tables++].lookup_number = 1;

    keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_CUSTOM_LEM;
    keys_to_table_id[num_tables++].lookup_number = 2;

    keys_to_table_id[num_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB; 
    keys_to_table_id[num_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    keys_to_table_id[num_tables].lookup_number = DBAL_KAPS_2ND_LKP_DIASBLE;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_tables].sw_table_id, &is_dynamic));
#endif 
    keys_to_table_id[num_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, NULL, num_tables));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, prog_id, SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));

exit:
    SOCDNX_FUNC_RETURN;
}


#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
uint32
   arad_pp_flp_dbal_ipv6uc_with_rpf_program_tables_init(
     int unit,
     int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint8 (*use_32_bit_ce_ptr)[32] = NULL;
    int nof_tables=0;
    uint8 is_dynamic;

    SOCDNX_INIT_FUNC_DEFS;

    if (ARAD_KBP_ENABLE_IPV6_RPF) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kbp_table_create(unit, 1, 0));
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS));

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
        keys_to_table_id[nof_tables].lookup_number = 1;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_MASTER_KBP;
        keys_to_table_id[nof_tables].lookup_number = 0;
        
        sal_memset(use_32_bit_ce, 0, sizeof(use_32_bit_ce));
        use_32_bit_ce[nof_tables][3] = 1; 
        use_32_bit_ce[nof_tables][9] = 1; 
        use_32_bit_ce_ptr = use_32_bit_ce;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP;
        keys_to_table_id[nof_tables].lookup_number = 0;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KBP;
        keys_to_table_id[nof_tables].lookup_number = 2;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_DUMMY_KBP;
        keys_to_table_id[nof_tables].lookup_number = 1;
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

    }
    else{
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_table_create(unit));

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
        if (SOC_DPP_L3_SRC_BIND_ANTI_SPOOFING_IPV6_ENABLE(unit)){
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_anti_spoofing_kaps_table_create(unit));
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6_ANTI_SPOOFING_KAPS;
        } else {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(unit));
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS;
        }
        keys_to_table_id[nof_tables].lookup_number = 1;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
        keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
        keys_to_table_id[nof_tables].lookup_number = 2;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
        keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mac_learning_enable", 0)) {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_l3_mac_learning_lem_custom_table_create(unit));
            keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_L3_MAC_LEARNING_CUSTOM_LEM;
            keys_to_table_id[nof_tables++].lookup_number = 1;
        }

    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                                                 keys_to_table_id, use_32_bit_ce_ptr, nof_tables));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv6uc_kbp_sip_sharing_program_tables_init(
     int unit,
     int prog_id
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int nof_tables=0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kbp_sip_sharing_table_create(unit));
    if(prog_id == PROG_FLP_IPV6UC){
        
        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    } else {
        
        keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    }
    keys_to_table_id[nof_tables++].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP_SIP_SHARING;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                                    keys_to_table_id, NULL, nof_tables));
exit:
    SOCDNX_FUNC_RETURN;
}
#endif 

uint32
   arad_pp_flp_dbal_pppoe_ipv6uc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int num_of_tables = 3;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    uint8 is_dynamic;
#endif

    SOCDNX_INIT_FUNC_DEFS;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_pon_uni_v6_static_kaps_table_create(unit));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_table_create(unit));
#endif
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_pppoe_ipv4uc_anti_spoofing_table_create(unit));

    keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS;
    keys_to_table_id[0].lookup_number = 1;
    keys_to_table_id[0].public_lpm_lookup_size = 0;

    keys_to_table_id[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_IPV4_ANTI_SPOOFING;
    keys_to_table_id[1].lookup_number = 1;

    keys_to_table_id[2].lookup_number = 2;
    keys_to_table_id[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[2].sw_table_id, &is_dynamic));
#endif
    keys_to_table_id[2].public_lpm_lookup_size = 0;
    keys_to_table_id[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_PPPOE_IPV6UC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, num_of_tables));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_flp_dbal_ipv6uc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
#endif
    uint8 (*use_32_bit_ce_ptr)[32] = NULL;
    int num_of_tables = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_ipv6_uc_use_tcam", 0))) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_tcam_table_create(unit));
        keys_to_table_id[num_of_tables].lookup_number = 1;
        keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
        keys_to_table_id[num_of_tables++].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
    } else {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if (ARAD_KBP_ENABLE_IPV6_UC) {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kbp_table_create(unit, 0, 0));
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS));

            keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
            keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS;
            keys_to_table_id[num_of_tables].lookup_number = 2;
            keys_to_table_id[num_of_tables++].public_lpm_lookup_size = 0;

            keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY;
            keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_MASTER_KBP;
            keys_to_table_id[num_of_tables].lookup_number = 0;
            
            sal_memset(use_32_bit_ce, 0, sizeof(use_32_bit_ce));
            use_32_bit_ce[num_of_tables][3] = 1; 
            use_32_bit_ce[num_of_tables][9] = 1; 
            use_32_bit_ce_ptr = use_32_bit_ce;
            keys_to_table_id[num_of_tables++].public_lpm_lookup_size = 0;

            keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
            keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP;
            keys_to_table_id[num_of_tables].lookup_number = 0;
            keys_to_table_id[num_of_tables++].public_lpm_lookup_size = 0;

        } else {
            uint8 is_dynamic;

            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_table_create(unit));
            keys_to_table_id[num_of_tables].lookup_number = 2;
            keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[num_of_tables].sw_table_id, &is_dynamic));
            keys_to_table_id[num_of_tables].public_lpm_lookup_size = 0;
            keys_to_table_id[num_of_tables++].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;

            if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) {
                SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_lem_route_scale_short_table_create(unit));
                SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_lem_route_scale_long_table_create(unit));
                
                SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_route_scale_short_table_create(unit));
                SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_kaps_route_scale_long_table_create(unit));

                num_of_tables = 0;

                
                keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
                keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM;
                keys_to_table_id[num_of_tables].lookup_number = 2;
                keys_to_table_id[num_of_tables++].public_lpm_lookup_size = 0;

                keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
                keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM;
                keys_to_table_id[num_of_tables].lookup_number = 1;
                keys_to_table_id[num_of_tables++].public_lpm_lookup_size = 0;

                keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D;
                keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS;
                keys_to_table_id[num_of_tables].lookup_number = 1;
                keys_to_table_id[num_of_tables++].public_lpm_lookup_size = 0;

                keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A_MSB;
                keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS;
                keys_to_table_id[num_of_tables].lookup_number = 2;
                keys_to_table_id[num_of_tables++].public_lpm_lookup_size = 0;
            }
        }
#endif 
    }
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mac_learning_enable", 0)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_l3_mac_learning_lem_custom_table_create(unit));
        keys_to_table_id[num_of_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
        keys_to_table_id[num_of_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_L3_MAC_LEARNING_CUSTOM_LEM;
        keys_to_table_id[num_of_tables].lookup_number = 1;
        ++num_of_tables;
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV6UC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, use_32_bit_ce_ptr, num_of_tables));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_flp_dbal_ipv6mc_program_tables_init(
     int unit
   )
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint8 (*use_32_bit_ce_ptr)[32] = NULL;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) || (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE)) {

        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6mc_table_create(unit));
        keys_to_table_id[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
        keys_to_table_id[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC;
        keys_to_table_id[0].lookup_number = 1;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV6MC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, 1));
    }else
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    {
        SOC_DPP_DBAL_TABLE_INFO table;
        int nof_qualifiers=0;
        int nof_tables=0;

        if (ARAD_KBP_ENABLE_IPV6_MC) {
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6mc_kbp_table_create(unit));
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kbp_default_route_kaps_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS));

            keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_D_MSB;
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS;
            keys_to_table_id[nof_tables].lookup_number = 1;
            keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

            keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY;
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_FWD_KBP;
            keys_to_table_id[nof_tables].lookup_number = 0;
            
            sal_memset(use_32_bit_ce, 0, sizeof(use_32_bit_ce));
            use_32_bit_ce[nof_tables][6] = 1; 
            use_32_bit_ce_ptr = use_32_bit_ce;
            keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

            keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_DUMMY_KBP;
            keys_to_table_id[nof_tables].lookup_number = 1;
            keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

            keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_RPF_KBP;
            keys_to_table_id[nof_tables].lookup_number = 2;
            keys_to_table_id[nof_tables++].public_lpm_lookup_size = 0;

            
            SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS, &table));
            nof_qualifiers = table.nof_qualifiers;
            table.nof_qualifiers = 0;
            SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS, &table));

        }
        else{
            uint8 is_dynamic;

            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6mc_table_create(unit));
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(unit));

            keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS;
            keys_to_table_id[nof_tables].lookup_number = 1;
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
            keys_to_table_id[nof_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);

            keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
            keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC;
            keys_to_table_id[nof_tables].lookup_number = 2;
            SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_kaps_table_prefix_is_dynamic(unit, keys_to_table_id[nof_tables].sw_table_id, &is_dynamic));
            keys_to_table_id[nof_tables++].public_lpm_lookup_size = SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic);
        }

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_IPV6MC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, use_32_bit_ce_ptr, nof_tables));
        if (ARAD_KBP_ENABLE_IPV6_MC) {
            
            SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS, &table));
            table.nof_qualifiers = nof_qualifiers;
            SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS, &table));
        }
    }
#else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ipv6mc Error no DB\n")));
    }
#endif 


exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_ipv6uc_kbp_table_init(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info = {0};
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;
    int msb_ce_offset = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX + 1;

    SOCDNX_INIT_FUNC_DEFS;

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 4));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 5));

    
    qual_info.qual_offset = 0;
    qual_info.qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
    arad_pp_dbal_qualifier_full_size_get(unit, stage, qual_info.qual_type, &(qual_info.qual_full_size), &(qual_info.qual_is_in_hdr));
    qual_info.qual_nof_bits = qual_info.qual_full_size;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+0));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+4));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+5));

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 0, 6));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 0, 7));

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 1, msb_ce_offset+6));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6UC, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 1, msb_ce_offset+7));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV6UC, &flp_lookups_tbl));
    flp_lookups_tbl.elk_lkp_valid = 0x1;
    flp_lookups_tbl.elk_wait_for_reply = 0x1;
    flp_lookups_tbl.elk_opcode = PROG_FLP_IPV6UC; 
    flp_lookups_tbl.elk_key_a_valid_bytes = 8;
    flp_lookups_tbl.elk_key_a_msb_valid_bytes = 10;
    flp_lookups_tbl.elk_key_b_valid_bytes = 8;
    flp_lookups_tbl.elk_key_b_msb_valid_bytes = 8;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV6UC, &flp_lookups_tbl));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_ipv6mc_kbp_table_init(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info = {0};
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;
    int msb_ce_offset = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX + 1;

    SOCDNX_INIT_FUNC_DEFS;

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 32+16;
    qual_info.qual_nof_bits = 16;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 0));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 4));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage, SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 5));


    
    qual_info.qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_full_size_get(unit, stage,qual_info.qual_type, &(qual_info.qual_full_size), &(qual_info.qual_is_in_hdr)));
    qual_info.qual_nof_bits = 16;
    qual_info.qual_offset = 0;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+1));

    qual_info.qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_full_size_get(unit, stage,qual_info.qual_type, &(qual_info.qual_full_size), &(qual_info.qual_is_in_hdr)));
    qual_info.qual_nof_bits = 16;
    qual_info.qual_offset = 0;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+0));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+4));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 16;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+5));

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 0, 6));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage,SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 0, 7));

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 1, msb_ce_offset+6));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, PROG_FLP_IPV6MC, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 1, msb_ce_offset+7));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, PROG_FLP_IPV6MC, &flp_lookups_tbl));
    flp_lookups_tbl.elk_lkp_valid = 0x1;
    flp_lookups_tbl.elk_wait_for_reply = 0x1;
    flp_lookups_tbl.elk_opcode = PROG_FLP_IPV6MC; 
    flp_lookups_tbl.elk_key_a_valid_bytes = 10;
    flp_lookups_tbl.elk_key_a_msb_valid_bytes = 10;
    flp_lookups_tbl.elk_key_b_valid_bytes = 8;
    flp_lookups_tbl.elk_key_b_msb_valid_bytes = 8;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, PROG_FLP_IPV6MC, &flp_lookups_tbl));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_flp_dbal_ipv6uc_rpf_kbp_table_init(int unit, int prog_id)
{
    SOC_DPP_DBAL_QUAL_INFO qual_info = {0};
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;
    int msb_ce_offset = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX + 1;

    SOCDNX_INIT_FUNC_DEFS;

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 32+16;
    qual_info.qual_nof_bits = 16;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 0));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 4));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage, SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 0, 5));


    

    qual_info.qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_full_size_get(unit, stage,qual_info.qual_type, &(qual_info.qual_full_size), &(qual_info.qual_is_in_hdr)));
    qual_info.qual_nof_bits = 16;
    qual_info.qual_offset = 0;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+0));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+4));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 16;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage,SOC_DPP_DBAL_PROGRAM_KEY_A, qual_info, 1, msb_ce_offset+5));

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage,SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 0, 6));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage,SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 0, 7));

    
    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info.qual_offset = 0;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 1, msb_ce_offset+6));

    qual_info.qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info.qual_offset = 32;
    qual_info.qual_nof_bits = 32;
    qual_info.qual_full_size = 32;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, prog_id, stage, SOC_DPP_DBAL_PROGRAM_KEY_B, qual_info, 1, msb_ce_offset+7));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl));
    flp_lookups_tbl.elk_lkp_valid = 0x1;
    flp_lookups_tbl.elk_wait_for_reply = 0x1;
    flp_lookups_tbl.elk_opcode = PROG_FLP_IPV6UC_RPF; 
    flp_lookups_tbl.elk_key_a_valid_bytes = 10;
    flp_lookups_tbl.elk_key_a_msb_valid_bytes = 8;
    flp_lookups_tbl.elk_key_b_valid_bytes = 8;
    flp_lookups_tbl.elk_key_b_msb_valid_bytes = 8;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_flp_dbal_epon_uni_v6_static_lem_table_create(int unit)
{
     SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
     int is_table_initiated = 0;
     SOCDNX_INIT_FUNC_DEFS;
     SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_LEM, &is_table_initiated));
     if (!is_table_initiated) {
          DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
          
          qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW; 
          qual_info[0].qual_offset = 32;
          qual_info[0].qual_nof_bits = 32;
          qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW; 
          qual_info[1].qual_nof_bits = 32;
          qual_info[2].qual_type = SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD; 
          qual_info[2].qual_nof_bits = 9;
          SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_LEM,
          (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable ? ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IP6_SPOOF_STATIC : ARAD_PP_FLP_IP_SPOOF_DHCP_KEY_OR_MASK),
          ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS,
          SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, ARAD_PP_LEM_ACCESS_KEY_TYPE_IP6_SPOOF_STATIC, qual_info, "FLP:TK_EPON_UNI_V6_STATIC_LEM"));
     }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_pon_ipv6_sav_static_program_tables_init(int unit,uint8 sa_auth_enabled,uint8 slb_enabled,uint32 tcam_access_profile_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id_static[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    int num_of_tables = 2;

    SOCDNX_INIT_FUNC_DEFS;
    
    if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable) {
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_epon_uni_v6_static_lem_table_create(unit));
        keys_to_table_id_static[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A; 
        keys_to_table_id_static[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_LEM;
        keys_to_table_id_static[0].lookup_number = 2;
    }
    else {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_pon_uni_v6_static_kaps_table_create(unit));
#endif
        keys_to_table_id_static[0].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
        keys_to_table_id_static[0].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS;
        keys_to_table_id_static[0].lookup_number = 1;
        keys_to_table_id_static[0].public_lpm_lookup_size = 0;
    }
    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_epon_uni_lem_default_learn_table_create(unit));
    keys_to_table_id_static[1].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id_static[1].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_LEARN;
    keys_to_table_id_static[1].lookup_number = ((!sa_auth_enabled && !slb_enabled)? 1 : 0);

    if (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)
    {
        if (sa_auth_enabled || slb_enabled)
        {
            SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
        }
        num_of_tables++;
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_epon_uni_lem_default_learn_table_forward(unit));
        keys_to_table_id_static[2].key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
        keys_to_table_id_static[2].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_FORWARD;
        keys_to_table_id_static[2].lookup_number = 1;
    }
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
    keys_to_table_id_static, NULL, num_of_tables));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_hw_based_key_enable(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC,
       SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY));
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_source_lookup_with_aget_access_enable(unit, PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_flp_dbal_gpon_l2_ip4mc_tables_init(int unit, int prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 nof_qual;
    uint32 nof_tables=0;
    int is_table_initiated;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_ipv4mc_bridge_lem_table_create(unit));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP4MC_SSM, &is_table_initiated));
    if (!is_table_initiated) {
        
        nof_qual=0;
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[nof_qual].qual_offset = 8;
        qual_info[nof_qual].qual_nof_bits = 24;
        qual_info[nof_qual++].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
        qual_info[nof_qual++].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
        qual_info[nof_qual++].qual_type = SOC_PPC_FP_QUAL_FID;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit,
                                                    SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP4MC_SSM,
						    (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IPV4_MC_SSM_VAL>>2),
						    ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC_SSM,
                                                    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM,
                                                    nof_qual,
                                                    ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC_SSM,
                                                    qual_info,
                                                    "FLP GPON L2 IP4MC SSM"));
    }

    keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM;
    keys_to_table_id[nof_tables++].lookup_number = 1;

    keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_B;
    keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP4MC_SSM;
    keys_to_table_id[nof_tables++].lookup_number = 2;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, nof_tables));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_flp_dbal_gpon_l2_ip6mc_tables_init(int unit, int prog_id)
{
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 nof_qual=0;
    uint32 nof_tables=0;

    SOCDNX_INIT_FUNC_DEFS;

    
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[nof_qual].qual_offset = 16;
    qual_info[nof_qual].qual_nof_bits = 32;
    qual_info[nof_qual++].qual_type = SOC_PPC_FP_QUAL_HDR_SA;
    qual_info[nof_qual].qual_offset = 0;
    qual_info[nof_qual].qual_nof_bits = 16;
    qual_info[nof_qual++].qual_type = SOC_PPC_FP_QUAL_HDR_SA;
    qual_info[nof_qual++].qual_type = SOC_PPC_FP_QUAL_FID;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit,
                                                 SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP6MC_SSM_EUI,
                                                 ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IPV6_MC_SSM_EUI,
                                                 ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV6_MC_SSM_EUI,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM,
                                                 nof_qual,
                                                 ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV6_MC_SSM_EUI,
                                                 qual_info,
                                                 "FLP IP6MC SSM EUI"));

    keys_to_table_id[nof_tables].key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id[nof_tables].sw_table_id = SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP6MC_SSM_EUI;
    keys_to_table_id[nof_tables++].lookup_number = 1;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, keys_to_table_id, NULL, nof_tables));
exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_flp_dbal_program_info_dump(int unit, uint32 prog_id)
{
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA flp_key_cons_lsb, flp_key_cons_msb;
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA lookups_tbl;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id, &flp_key_cons_lsb));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_key_construction_tbl_get_unsafe(unit, prog_id+ARAD_PP_FLP_INSTRUCTIONS_NOF, &flp_key_cons_msb));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &lookups_tbl));
    

    if (!SOC_IS_JERICHO(unit)) { 
        LOG_CLI((BSL_META(" KEYS:")));

        if ((flp_key_cons_lsb.key_a_inst_0_to_5_valid) || (flp_key_cons_msb.key_a_inst_0_to_5_valid)) {
            LOG_CLI((BSL_META("\t Key A: LSB = 0x%x, MSB = 0x%x,"), flp_key_cons_lsb.key_a_inst_0_to_5_valid, flp_key_cons_msb.key_a_inst_0_to_5_valid));
        } else {
            LOG_CLI((BSL_META("\t Key A: no CE assigned,")));
        }

        if (flp_key_cons_lsb.key_b_inst_0_to_5_valid || (flp_key_cons_msb.key_b_inst_0_to_5_valid)) {
            LOG_CLI((BSL_META("\tKey B: LSB = 0x%x, MSB = 0x%x,"), flp_key_cons_lsb.key_b_inst_0_to_5_valid, flp_key_cons_msb.key_b_inst_0_to_5_valid));
        } else {
            LOG_CLI((BSL_META("\tKey B: no CE assigned,")));
        }

        if (flp_key_cons_lsb.key_c_inst_0_to_5_valid || (flp_key_cons_msb.key_c_inst_0_to_5_valid)) {
            LOG_CLI((BSL_META("\tKey C: LSB = 0x%02x, MSB = 0x%x"), flp_key_cons_lsb.key_c_inst_0_to_5_valid, flp_key_cons_msb.key_c_inst_0_to_5_valid));
        } else {
            LOG_CLI((BSL_META("\tKey C: no CE assigned ")));
        }

        if (SOC_IS_JERICHO(unit)) {
            if (flp_key_cons_lsb.key_d_inst_0_to_7_valid || (flp_key_cons_msb.key_d_inst_0_to_7_valid)) {
                LOG_CLI((BSL_META(",\tKey D: LSB = 0x%x, MSB = 0x%x"), flp_key_cons_lsb.key_d_inst_0_to_7_valid, flp_key_cons_msb.key_d_inst_0_to_7_valid));
            } else {
                LOG_CLI((BSL_META(",\tKey D: no CE assigned ")));
            }
        }
        LOG_CLI((BSL_META("\n")));
    }

    LOG_CLI((BSL_META("LOOKUPS: ")));

    if(lookups_tbl.lem_1st_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLEM 1st: %s"), arad_pp_dbal_key_id_to_string(unit, lookups_tbl.lem_1st_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLEM 1st: Non")));
    }
    if(lookups_tbl.lem_2nd_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLEM 2nd: %s"), arad_pp_dbal_key_id_to_string(unit, lookups_tbl.lem_2nd_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLEM 2nd: Non")));
    }

    if(lookups_tbl.lpm_1st_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLPM 1st: %s"), arad_pp_dbal_key_id_to_string(unit, lookups_tbl.lpm_1st_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLPM 1st: Non")));
    }

    if(lookups_tbl.lpm_2nd_lkp_valid == 1) {
        LOG_CLI((BSL_META("\tLPM 2nd: %s"),arad_pp_dbal_key_id_to_string(unit, lookups_tbl.lpm_2nd_lkp_key_select) ));
    }else{
        LOG_CLI((BSL_META("\tLPM 2nd: Non")));
    }

    if (lookups_tbl.tcam_lkp_db_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID ) {
        uint32 key_select = lookups_tbl.tcam_lkp_key_select;
        if (!SOC_IS_JERICHO(unit)) {
            if (lookups_tbl.tcam_lkp_key_select == ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL) {
                key_select = SOC_DPP_DBAL_PROGRAM_KEY_C;
            }else{
                key_select = SOC_DPP_DBAL_PROGRAM_KEY_A;
            }
        }
        LOG_CLI((BSL_META("\tTCAM 1st: %s DB %d "), arad_pp_dbal_key_id_to_string(unit, key_select), lookups_tbl.tcam_lkp_db_profile ));
    }else{
        LOG_CLI((BSL_META("\tTCAM 1st: not valid")));
    }

    if (SOC_IS_JERICHO(unit)) {
        if (lookups_tbl.tcam_lkp_db_profile_1 != ARAD_TCAM_ACCESS_PROFILE_INVALID ) {
            LOG_CLI((BSL_META("\tTCAM 2nd: %s DB %d "), arad_pp_dbal_key_id_to_string(unit, lookups_tbl.tcam_lkp_key_select_1), lookups_tbl.tcam_lkp_db_profile_1 ));
        }else{
            LOG_CLI((BSL_META("\tTCAM 2nd: not valid")));
        }
    }

    if(lookups_tbl.elk_lkp_valid == 1){
        LOG_CLI((BSL_META("\tKBP: opcode %d "), lookups_tbl.elk_opcode));
        if (lookups_tbl.elk_key_a_valid_bytes != 0) {
            LOG_CLI((BSL_META("A LSB (%d) "), lookups_tbl.elk_key_a_valid_bytes));
        }
        if (SOC_IS_JERICHO(unit)) {
            if (lookups_tbl.elk_key_a_msb_valid_bytes != 0) {
                LOG_CLI((BSL_META("A MSB (%d) "), lookups_tbl.elk_key_a_msb_valid_bytes));
            }
        }
        if (lookups_tbl.elk_key_b_valid_bytes != 0) {
            LOG_CLI((BSL_META("B LSB (%d) "), lookups_tbl.elk_key_b_valid_bytes));
        }
        if (SOC_IS_JERICHO(unit)) {
            if (lookups_tbl.elk_key_b_msb_valid_bytes != 0) {
                LOG_CLI((BSL_META("B MSB (%d) "), lookups_tbl.elk_key_b_msb_valid_bytes));
            }
        }

        if (lookups_tbl.elk_key_c_valid_bytes != 0) {
            LOG_CLI((BSL_META("C LSB (%d) "), lookups_tbl.elk_key_c_valid_bytes));
        }
        if (SOC_IS_JERICHO(unit)) {
            if (lookups_tbl.elk_key_c_msb_valid_bytes != 0) {
                LOG_CLI((BSL_META("C MSB (%d) "), lookups_tbl.elk_key_c_msb_valid_bytes));
            }
        }

        if (SOC_IS_JERICHO(unit)) {
            if (lookups_tbl.elk_key_d_lsb_valid_bytes != 0) {
                LOG_CLI((BSL_META("D LSB (%d) "), lookups_tbl.elk_key_d_lsb_valid_bytes));
            }
            if (lookups_tbl.elk_key_d_msb_valid_bytes != 0) {
                LOG_CLI((BSL_META("D MSB (%d) "), lookups_tbl.elk_key_d_msb_valid_bytes));
            }
        }
    }

    LOG_CLI((BSL_META("\n----------------------------------------------------------------------------------------------------------------------------------------------------------\n")));
    LOG_CLI((BSL_META("\n\n")));
exit:
     SOCDNX_FUNC_RETURN;
}
