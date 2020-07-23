/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_ports.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ce_instruction.h>


#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/scache.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/ha.h>
#endif















uint8 hit_bit_result_for_do_search = 0;

extern genericTblMgrAradAppData *AradAppData[SOC_SAND_MAX_DEVICE];





STATIC void arad_kbp_diag_endians_fix(int unit, uint32 value[24], int nof_bits)
{
    uint32 i = 0;

    if(((nof_bits + 31) / 32) > 24) {
        LOG_CLI((BSL_META_U(unit,
                            "\n***** nof_bit %d out of range!\n\n"), nof_bits));
        return;
    }

    for(i = 0 ; i < (nof_bits + 31) / 32; i++) {
        value[i] = soc_ntohl(value[i]);
    }
}

void 
    arad_kbp_sw_config_print(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  int  print_mode,
       SOC_SAND_IN  int  print_ltr
    )
{
    uint32 
        idx, ltr_start, ltr_end;

    ARAD_KBP_TABLE_CONFIG           Arad_kbp_table_config_info = {0};
    arad_kbp_frwd_ltr_db_t          Arad_kbp_gtm_ltr_info = {0};
    ARAD_KBP_LTR_CONFIG             ltr_config_info = {0};
    arad_kbp_lut_data_t             lut_data = {0};
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO gtm_config_info = {0};

    ARAD_KBP_TABLE_CONFIG_clear(&Arad_kbp_table_config_info);
    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);
    arad_kbp_frwd_ltr_db_clear(&Arad_kbp_gtm_ltr_info);
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO_clear(&gtm_config_info);

    if(NULL == AradAppData[unit])
    {
        LOG_CLI((BSL_META_U(unit,
                            "\n***** KBP Device is not yet installed with KBP configuration!\n\n")));
        return;
    }

    if(print_ltr < 0) { 

        LOG_CLI((BSL_META_U(unit,
                            "\nTable Configuration\n\n")));
        LOG_CLI((BSL_META_U(unit,
                            "%-10s"),"Tbl-ID"));
        LOG_CLI((BSL_META_U(unit,
                            "%-21s"),"Tbl-Name"));
        LOG_CLI((BSL_META_U(unit,
                            "%-10s"),"Size"));
        LOG_CLI((BSL_META_U(unit,
                            "%-10s"),"Width"));
        LOG_CLI((BSL_META_U(unit,
                            "%-10s"),"AD Width"));
        LOG_CLI((BSL_META_U(unit,
                            "%-10s"),"Num ent."));
        LOG_CLI((BSL_META_U(unit,
                            "%-10s\n"),"~Capacity"));
        LOG_CLI((BSL_META_U(unit,
                            "--------------------------------------------------------------------------------\n")));

        for ( idx = 0; 
              idx < ARAD_KBP_MAX_NUM_OF_TABLES; 
              idx++) 
        {
            if(print_mode <= 0 && 
                    (idx >= ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC && idx <= ARAD_KBP_FRWRD_TBL_ID_DUMMY_7))
            {
                continue;
            }
            KBP_INFO.Arad_kbp_table_config_info.get(unit, idx, &Arad_kbp_table_config_info);         
            if (Arad_kbp_table_config_info.valid) 
            {
                ARAD_KBP_TABLE_CONFIG_print(unit, &Arad_kbp_table_config_info);
            }
        }

        ltr_start = 0;
        ltr_end = ARAD_KBP_MAX_NUM_OF_FRWRD_DBS;
        LOG_CLI((BSL_META_U(unit,
                            "\n\nApplication Configuration\n\n")));

        if(print_mode < 0) { 

            LOG_CLI((BSL_META_U(unit,
                                "%-10s"), "LTR Index"));
            LOG_CLI((BSL_META_U(unit,
                                "%s\n"), "Program"));
            LOG_CLI((BSL_META_U(unit,
                                "--------------------------------------------------------------------------------\n")));
            for ( idx = ltr_start; 
                  idx < ltr_end; 
                  idx++) 
            {
                if((print_mode <= 0) && (idx == ARAD_KBP_FRWRD_DB_TYPE_DUMMY_FRWRD)) {
                    continue;
                }
                KBP_INFO.Arad_kbp_ltr_config.get(unit, idx, &ltr_config_info);
                if (ltr_config_info.valid) 
                {
                    LOG_CLI((BSL_META_U(unit,
                                        " %-9d"), idx));
                    LOG_CLI((BSL_META_U(unit,
                                        "%s\n"), ARAD_KBP_FRWRD_IP_DB_TYPE_to_string((ARAD_KBP_FRWRD_IP_DB_TYPE)idx)));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));

            return; 
        }

    } else {
        if(print_ltr >= ARAD_KBP_MAX_NUM_OF_FRWRD_DBS) {
            LOG_CLI((BSL_META_U(unit,
                                "\n\nInvalid LTR Index\n")));
            return;
        }
        
        ltr_start = print_ltr;
        ltr_end = print_ltr + 1;
    }

    for ( idx = ltr_start; 
          idx < ltr_end; 
          idx++) 
    {
        if((print_mode <= 0) && (idx == ARAD_KBP_FRWRD_DB_TYPE_DUMMY_FRWRD)) {
            continue;
        }
        KBP_INFO.Arad_kbp_ltr_config.get(unit, idx, &ltr_config_info);
        if (ltr_config_info.valid) 
        {
            LOG_CLI((BSL_META_U(unit,
                                "--------------------------------------------------------------------------------\n")));
            LOG_CLI((BSL_META_U(unit,
                                "LTR Index: %d, program: %s\n"), idx, ARAD_KBP_FRWRD_IP_DB_TYPE_to_string((ARAD_KBP_FRWRD_IP_DB_TYPE)idx)));
            ARAD_KBP_LTR_CONFIG_print(unit, &ltr_config_info, print_mode);
            LOG_CLI((BSL_META_U(unit,
                                "\n")));

            KBP_INFO.Arad_kbp_gtm_ltr_info.get(unit, idx, &Arad_kbp_gtm_ltr_info);
            arad_kbp_frwd_ltr_db_print(unit, &Arad_kbp_gtm_ltr_info, print_mode);
            LOG_CLI((BSL_META_U(unit,
                                "\n")));

            if (print_mode == 2) {
                           LOG_CLI((BSL_META_U(unit,"  Debug\n")));
                           KBP_INFO.Arad_kbp_gtm_opcode_config_info.get(unit, idx, &gtm_config_info);
                           ARAD_KBP_GTM_OPCODE_CONFIG_INFO_print(&gtm_config_info);

                           KBP_INFO.Arad_kbp_gtm_lut_info.get(unit, idx, &lut_data); 
                           arad_kbp_frwd_lut_db_print(unit, &lut_data);

            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        } else if (print_ltr >= 0) { 
            LOG_CLI((BSL_META_U(unit,
                                "\n\nInvalid LTR Index\n")));
        }
    }
}

void
    arad_kbp_device_print(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  char*   file_name
    )
{
    FILE *kbp_file;
    uint core = 0;
    int res;

    if(NULL == AradAppData[unit])
    {
        LOG_CLI((BSL_META_U(unit,
                            "\n***** KBP Device is not yet installed with KBP configuration!\n\n")));
        return;
    }

    if (file_name == NULL)
    {
        return;
    }

    kbp_file = fopen(file_name,"w");

    if (kbp_file != NULL) {
        res = kbp_device_print_html(AradAppData[unit]->device_p[core], kbp_file);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit, "Error in %s(): kbp_device_print_html failed: %s\n"),
                    FUNCTION_NAME(), kbp_get_status_string(res)));
        }
        fclose(kbp_file);
    }
}


void
    arad_kbp_device_dump(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  char*   file_name
    )
{
    FILE *kbp_file;
    uint core = 0;
    int res;

    if(NULL == AradAppData[unit])
    {
        LOG_CLI((BSL_META_U(unit,
                            "\n***** KBP Device is not yet installed with KBP configuration!\n\n")));
        return;
    }

    if (file_name == NULL)
    {
        return;
    }

    kbp_file = fopen(file_name,"w");

    if (kbp_file != NULL) {
        res = kbp_device_dump(AradAppData[unit]->device_p[core], TRUE, kbp_file);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit, "Error in %s(): kbp_device_dump failed: %s\n"),
                    FUNCTION_NAME(), kbp_get_status_string(res)));
        }
        fclose(kbp_file);
    }
}

void
  arad_kbp_frwd_ltr_db_clear(
    SOC_SAND_OUT arad_kbp_frwd_ltr_db_t *info
  )
{
    uint32 
        res_idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(arad_kbp_frwd_ltr_db_t));
    for( res_idx = 0; 
         res_idx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; 
         res_idx++ )
    {
        info->res_format[res_idx] = NLM_ARAD_NO_INDEX_NO_AD;
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_GTM_OPCODE_CONFIG_INFO_clear(
    SOC_SAND_OUT ARAD_KBP_GTM_OPCODE_CONFIG_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_GTM_OPCODE_CONFIG_INFO));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_GTM_LUT_clear(
    SOC_SAND_OUT arad_kbp_lut_data_t *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(arad_kbp_lut_data_t));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_CONFIG_clear(
    SOC_SAND_OUT ARAD_KBP_LTR_CONFIG *info
  )
{
    uint32 
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_LTR_CONFIG));
    info->valid = FALSE;
    
    for (index = 0; index < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; index++) 
    {
        info->tbl_id[index] = ARAD_KBP_TABLE_INDX_TO_DUMMY_TABLE_ID(index);
        ARAD_KBP_LTR_SINGLE_SEARCH_clear(info->ltr_search);
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_SINGLE_SEARCH_clear(
    SOC_SAND_OUT ARAD_KBP_LTR_SINGLE_SEARCH *info
  )
{
    uint32 
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_LTR_SINGLE_SEARCH));

    for (index = 0; index < ARAD_KBP_MAX_NOF_KEY_SEGMENTS; index++) 
    {
        ARAD_KBP_KEY_SEGMENT_clear(&info->key_segment[index]);
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_TABLE_CONFIG_clear(
    SOC_SAND_OUT ARAD_KBP_TABLE_CONFIG *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_TABLE_CONFIG));
    info->valid = FALSE;
    info->tbl_width = NLM_TBL_WIDTH_END;
    info->tbl_asso_width = NLM_TBL_ADLEN_END;
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_KEY_SEGMENT_clear(
    SOC_SAND_OUT ARAD_KBP_KEY_SEGMENT *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_KEY_SEGMENT));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  NlmAradCompareResponseFormat_to_string(
    SOC_SAND_IN  NlmAradCompareResponseFormat enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case NLM_ARAD_ONLY_INDEX_NO_AD:
    str = "Index-No-AD";
  break;
  case NLM_ARAD_INDEX_AND_32B_AD:
    str = "Index,32bAD";
  break;
  case NLM_ARAD_INDEX_AND_64B_AD:
    str = "Index,64bAD";
  break;
  case NLM_ARAD_INDEX_AND_128B_AD:
    str = "Index,128bAD";
  break;
  case NLM_ARAD_NO_INDEX_NO_AD:
    str = "No-Index-No-AD";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  ARAD_KBP_FRWRD_IP_DB_TYPE_to_string(
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC:
    str = "IPv4 Unicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_RPF:
    str = "IPv4 Unicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_RPF:
    str = "IPv4 Multicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC:
    str = "IPv6 Unicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF:
    str = "IPv6 Unicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_MC_RPF:
    str = "IPv6 Multicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_LSR:
    str = "MPLS";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_TRILL_UC:
    str = "TRILL Unicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_TRILL_MC:
    str = "TRILL Multicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC:
    str = "IPV4 Double Capacity";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_IP:
    str = "IP LSR SHARED FOR IP";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_IP_WITH_RPF:
    str = "IP LSR SHARED FOR IP WITH RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_LSR:
    str = "IP LSR SHARED FOR LSR";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_EXTENDED_IPv6:
    str = "EXTENDED IPv6";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_EXTENDED_P2P:
    str = "EXTENDED P2P";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC:
    str = "IPv4 UC Public";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC_RPF:
    str = "IPv4 UC Public RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC:
    str = "IPv6 UC Public";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC_RPF:
    str = "IPv6 UC Public RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_DUMMY_FRWRD:
    str = "DUMMY FRWRD";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_BRIDGE:
    str = "IPV4 MC BRIDGE";
  break;
  default:
    str = "ACL";
  }
  return str;
}

const char*
  ARAD_KBP_FRWRD_IP_TBL_ID_to_string(
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0:
    str = "IPv4 UC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1:
    str = "IPv4 RPF";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_MC:
    str = "IPv4 MC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0:
    str = "IPv6 UC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1:
    str = "IPv6 RPF";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_MC:
    str = "IPv6 MC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR:
    str = "MPLS";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_TRILL_UC:
    str = "TRILL UC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_TRILL_MC:
    str = "TRILL MC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC:
    str = "IPV4 MC DUMMY";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC:
    str = "IPV6 MC DUMMY";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC:
    str = "IPV4 UC DUMMY";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC:
    str = "IPV6 UC DUMMY";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC:
    str = "IPV4 DC DUMMY";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_FRWRD:
    str = "DUMMY FRWRD";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_0:
    str = "DUMMY 0";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_1:
    str = "DUMMY 1";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_2:
    str = "DUMMY 2";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_3:
    str = "DUMMY 3";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_4:
    str = "DUMMY 4";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_5:
    str = "DUMMY 5";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_6:
    str = "DUMMY 6";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_7:
    str = "DUMMY 7";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED:
    str = "LSR IP SHARED";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP:
    str = "LSR IP SHARED FOR IP";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR:
    str = "LSR IP SHARED FOR LSR";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6:
    str = "EXTENDED IPV6";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_DC:
    str = "IPv4 DC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0:
    str = "IPv4 UC PUBLIC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1:
    str = "IPv4 RPF PUBLIC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0:
    str = "IPv6 UC PUBLIC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1:
    str = "IPv6 RPF PUBLIC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE:
    str = "IPV4 MC BRIDGE";
  break;
  default:
    str = "ACL";
  }
  return str;
}

const char*
  ARAD_KBP_TABLE_DB_TYPE_to_string(
    SOC_SAND_IN enum kbp_db_type enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
    case KBP_DB_ACL:
      str = "ACL";
    break;
    case KBP_DB_LPM:
      str = "Forwarding";
    break;
    default:
      str = "Invalid";
    break;
  }
  return str;
}

const char*
  ARAD_KBP_KEY_FIELD_TYPE_to_string(
    SOC_SAND_IN enum kbp_key_field_type enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
    case KBP_KEY_FIELD_TERNARY:
      str = "TERNARY";
    break;
    case KBP_KEY_FIELD_PREFIX:
      str = "PREFIX";
    break;
    case KBP_KEY_FIELD_EM:
      str = "EM";
    break;
    case KBP_KEY_FIELD_RANGE:
      str = "RANGE";
    break;
    case KBP_KEY_FIELD_TABLE_ID:
      str = "TABLE_ID";
    break;
    case KBP_KEY_FIELD_HOLE:
      str = "HOLE";
    break;
    case KBP_KEY_FIELD_DUMMY_FILL:
      str = "DUMMY";
    break;
    default:
      str = "INVALID";
    break;
  }
  return str;
}


void
  arad_kbp_frwd_lut_db_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  arad_kbp_lut_data_t *info
  )
{
    int i;
    int num_of_searches = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    num_of_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    LOG_CLI((BSL_META_U(unit,"  \tLUT DB:\n")));
    LOG_CLI((BSL_META_U(unit,
                        "  \t\trec_is_valid=%d, rec_size=%d, rec_type=%d\n"
                        "  \t\tmode=%d, key_config=%d, lut_key_data=%d\n"
                        "  \t\tinstr=%d, key_w_cpd_gt_80=%d, copy_data_cfg=%d\n"),
             info->rec_is_valid,info->rec_size,info->rec_type,
             info->mode,info->key_config,info->lut_key_data,
             info->instr,info->key_w_cpd_gt_80,info->copy_data_cfg));

    for (i = 0; i < num_of_searches; i++){
        LOG_CLI((BSL_META_U(unit, "  \t\tresult_idx_ad_cfg[%d]=%d, result_idx_or_ad[%d]=%d\n"),
                 i, info->result_idx_ad_cfg[i], i, info->result_idx_or_ad[i]));
    }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_kbp_frwd_ltr_db_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  arad_kbp_frwd_ltr_db_t *info,
    SOC_SAND_IN  int print_mode
  )
{
    uint32
        index,
        num_of_kbp_searches;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    LOG_CLI((BSL_META_U(unit,
                        "  Total Result Length=%d\n"), 
             info->res_total_data_len));
    if(print_mode > 0) { 
        for (index = 0; index < num_of_kbp_searches; index++) 
        {
            LOG_CLI((BSL_META_U(unit,
                            "  Result %d: length=%d  format=<%s>\n"), 
                    index, 
                    info->res_data_len[index],
                    NlmAradCompareResponseFormat_to_string(info->res_format[index])));
        }
    }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_GTM_OPCODE_CONFIG_INFO_print(
    SOC_SAND_IN  ARAD_KBP_GTM_OPCODE_CONFIG_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit," \tGTM Config Info:\n"))); 
    LOG_CLI((BSL_META_U(unit," \t\tTX Data Size: %d\n"), info->tx_data_size)); 
    LOG_CLI((BSL_META_U(unit," \t\tTX Data Type: %d\n"), info->tx_data_type)); 
    LOG_CLI((BSL_META_U(unit," \t\tRX Data Size: %d\n"), info->rx_data_size)); 
    LOG_CLI((BSL_META_U(unit," \t\tRX Data Type: %d\n"), info->rx_data_type)); 

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_CONFIG_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  ARAD_KBP_LTR_CONFIG *info,
    SOC_SAND_IN  int print_mode
  )
{
    uint32
        index, search_id,
        num_of_kbp_searches;

    ARAD_KBP_TABLE_CONFIG table_config_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    if (info->valid) 
    {
        LOG_CLI((BSL_META_U(unit,
                "  Opcode: %d, LTR ID: %d, Parallel Searches bitmap: %d, Cmpr3: %s\n"), 
                info->opcode, 
                info->ltr_id,
                info->parallel_srches_bmp,
                (info->is_cmp3_search ? "TRUE" : "FALSE")));

        LOG_CLI((BSL_META_U(unit,
                "                                            Master Key ")));
        for (index = 0; index < info->master_key_fields.nof_key_segments; index++) 
        {
            ARAD_KBP_KEY_SEGMENT_print(&info->master_key_fields.key_segment[index], print_mode);
        }
        LOG_CLI((BSL_META_U(unit, "\n")));

        for (index = 0, search_id = 0; search_id < num_of_kbp_searches; index++, search_id++)
        {
            int print_private_table = -1;
            if(print_mode <= 0 && 
                info->tbl_id[index] >= ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC &&
                info->tbl_id[index] <= ARAD_KBP_FRWRD_TBL_ID_DUMMY_7)
            {
                continue;
            }

            
            if(search_id == 0) {
                if(info->ltr_id == ARAD_KBP_FRWRD_LTR_IPV4_UC_PUBLIC || info->ltr_id == ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF_PUBLIC) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
                } else if(info->ltr_id == ARAD_KBP_FRWRD_LTR_IPV6_UC_PUBLIC || info->ltr_id == ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_PUBLIC) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0;
                }
            } else if(search_id == 3) { 
                if(info->ltr_id == ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF_PUBLIC) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1;
                } else if(info->ltr_id == ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_PUBLIC) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1;
                }
            }
            if(print_private_table != -1) {
                int i;
                KBP_INFO.Arad_kbp_table_config_info.get(unit, print_private_table, &table_config_info);

                LOG_CLI((BSL_META_U(unit,
                        "  Search %d Result %d: Tbl-ID %3d %-15s type %s "), search_id, index, print_private_table,
                        ARAD_KBP_FRWRD_IP_TBL_ID_to_string((ARAD_KBP_FRWRD_IP_DB_TYPE)print_private_table),
                        ARAD_KBP_TABLE_DB_TYPE_to_string(table_config_info.db_type)));

                for(i = 0; i < table_config_info.entry_key_parsing.nof_segments; i++) {
                    ARAD_KBP_KEY_SEGMENT_print(&(table_config_info.entry_key_parsing.key_segment[i]), print_mode);
                }
                LOG_CLI((BSL_META_U(unit, "\n")));

                
                search_id++;
            }

            KBP_INFO.Arad_kbp_table_config_info.get(unit, info->tbl_id[index], &table_config_info);

            LOG_CLI((BSL_META_U(unit,
                    "  Search %d Result %d: Tbl-ID %3d %-15s type %s "), search_id, index, info->tbl_id[index],
                    ARAD_KBP_FRWRD_IP_TBL_ID_to_string((ARAD_KBP_FRWRD_IP_DB_TYPE)info->tbl_id[index]),
                    ARAD_KBP_TABLE_DB_TYPE_to_string(table_config_info.db_type)));

            ARAD_KBP_LTR_SINGLE_SEARCH_print(&info->ltr_search[index], print_mode);
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_SINGLE_SEARCH_print(
    SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH *info,
    SOC_SAND_IN  int print_mode
    )
{
    uint32 
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    for (index = 0; index < info->nof_key_segments; index++) 
    {
        ARAD_KBP_KEY_SEGMENT_print(&info->key_segment[index], print_mode);
    }
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_TABLE_CONFIG_print(
      SOC_SAND_IN  int  unit,
      SOC_SAND_IN  ARAD_KBP_TABLE_CONFIG *info
    )
{
    struct kbp_db_stats stats;
    uint32 res;

    if (info->valid) 
    {
        LOG_CLI((BSL_META_U(unit,
                            " %-9d"), info->tbl_id));
        LOG_CLI((BSL_META_U(unit,
                            "%-21s"), ARAD_KBP_FRWRD_IP_TBL_ID_to_string(info->tbl_id)));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->tbl_size));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->tbl_width));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), ARAD_KBP_AD_WIDTH_TYPE_TO_BITS(info->tbl_asso_width)));
         
        res = kbp_db_stats(AradAppData[unit]->g_gtmInfo[info->tbl_id].tblInfo.db_p, &stats);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): DB: kbp_db_stats with failed: %s!\n"), 
                            FUNCTION_NAME(), kbp_get_status_string(res)));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), stats.num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d\n"), stats.capacity_estimate));
    }
}

void
  ARAD_KBP_KEY_SEGMENT_print(
    SOC_SAND_IN  ARAD_KBP_KEY_SEGMENT *info,
    SOC_SAND_IN  int print_mode
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    if(print_mode == 2) {
        LOG_CLI((BSL_META_U(unit,
                "<%s,%d,%s> "), info->name, info->nof_bytes, ARAD_KBP_KEY_FIELD_TYPE_to_string(info->type)));
    } else {
        LOG_CLI((BSL_META_U(unit,
                "<%s,%d> "), info->name, info->nof_bytes));
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


static void
  arad_kbp_parse_and_print_entry(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8                   acl_indication,
    SOC_SAND_IN uint8                   *data,
    SOC_SAND_IN uint8                   *mask,
    SOC_SAND_IN int                     data_len_bytes,
    SOC_SAND_IN uint32                  prefix_length,
    SOC_SAND_IN uint8                   *ad_data,
    SOC_SAND_IN int                     ad_len_bytes,
    SOC_SAND_IN int                     nof_segments,
    SOC_SAND_IN ARAD_KBP_KEY_SEGMENT    segment[ARAD_KBP_MAX_NOF_KEY_SEGMENTS],
    SOC_SAND_IN int                     entry_index)
{
    int i,j,k=0;
    uint32 prefix_length_counter;
    uint8 prefix_length_mask;

    if(entry_index == -1){
        LOG_CLI((BSL_META_U(unit,"| Row Data=0x")));
    }else{
        LOG_CLI((BSL_META_U(unit,"| E.%02d - Row Data=0x"),entry_index));
    }
    
    if(acl_indication == TRUE){
        
        for(i=0;i<data_len_bytes;i++) {
            LOG_CLI((BSL_META_U(unit,"%02x"),(data[i])));
        }
        LOG_CLI((BSL_META_U(unit,"\tMask=0x")));
        for(i=0;i<data_len_bytes;i++) {
            LOG_CLI((BSL_META_U(unit,"%02x"),(mask[i])));
        }
        LOG_CLI((BSL_META_U(unit,"\tPrio=%u"),(prefix_length)));
    }else{
        prefix_length_counter = 0;
        for(i=0;i<data_len_bytes;i++) {
            
            if(prefix_length >= prefix_length_counter + SOC_SAND_NOF_BITS_IN_BYTE) {
                prefix_length_mask = SOC_SAND_U8_MAX;
            } else if(prefix_length <= prefix_length_counter) {
                prefix_length_mask = 0x0;
            } else {
                prefix_length_mask = SOC_SAND_U8_MAX << (SOC_SAND_NOF_BITS_IN_BYTE - (prefix_length - prefix_length_counter));
            }
            LOG_CLI((BSL_META_U(unit,"%02x"),(data[i] & prefix_length_mask)));
            prefix_length_counter += SOC_SAND_NOF_BITS_IN_BYTE;
        }
        
        LOG_CLI((BSL_META_U(unit,"/%d\t"),prefix_length));

        if(entry_index == -1){
            LOG_CLI((BSL_META_U(unit,"\n| ")));
        }
        for (i=0;i<nof_segments;i++) {
            LOG_CLI((BSL_META_U(unit,"%s"), segment[i].name));
            if ((strcmp(segment[i].name,"SIP")) == 0 || (strcmp(segment[i].name,"DIP") == 0)) {
                if (segment[i].nof_bytes == 4) {
                    
                    LOG_CLI((BSL_META_U(unit,"(dec)=")));
                    for (j=0;j<4;j++) {
                        LOG_CLI((BSL_META_U(unit,"%d"),(data[k++])));
                        if(j != 3) {
                            LOG_CLI((BSL_META_U(unit,".")));
                        }
                    }
                }else if(segment[i].nof_bytes == 16) {
                    
                    LOG_CLI((BSL_META_U(unit,"(hex)=")));
                    prefix_length_counter = 0;
                    for (j=0;j<16;j++) {
                        
                        if(prefix_length >= prefix_length_counter + SOC_SAND_NOF_BITS_IN_BYTE) {
                            prefix_length_mask = SOC_SAND_U8_MAX;
                        } else if(prefix_length <= prefix_length_counter) {
                            prefix_length_mask = 0x0;
                        } else {
                            prefix_length_mask = SOC_SAND_U8_MAX << (SOC_SAND_NOF_BITS_IN_BYTE - (prefix_length - prefix_length_counter));
                        }
                        LOG_CLI((BSL_META_U(unit,"%02x"),(data[k] & prefix_length_mask)));
                        k++;
                        if(j % 2 && j != 15) {
                            LOG_CLI((BSL_META_U(unit,".")));
                        }
                        prefix_length_counter += SOC_SAND_NOF_BITS_IN_BYTE;
                    }
                }
            }else{
                LOG_CLI((BSL_META_U(unit,"=0x")));
                for (j=0;j<segment[i].nof_bytes;j++) {
                    LOG_CLI((BSL_META_U(unit,"%02x"),(data[k++])));
                }
            }
            LOG_CLI((BSL_META_U(unit,"\t")));
        }
    }
    LOG_CLI((BSL_META_U(unit,"\n")));

    
    if (ad_len_bytes>0) {
        LOG_CLI((BSL_META_U(unit, "| Associate Data (result)=0x")));
        for (i=0;i<ad_len_bytes;i++) {
            LOG_CLI((BSL_META_U(unit,"%02x"),(ad_data[i])));
        }
    }
    LOG_CLI((BSL_META_U(unit,"\n")));
}

static uint32
    arad_kbp_print_db_search_info(
        SOC_SAND_IN  int     unit,
        SOC_SAND_IN  uint32  table_id)
{
    int i, res = 0;
    ARAD_KBP_TABLE_CONFIG table_config_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = KBP_INFO.Arad_kbp_table_config_info.get(unit, table_id, &table_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

    LOG_CLI((BSL_META_U(unit,"Table ID=%d, %s\t"), table_id, ARAD_KBP_FRWRD_IP_TBL_ID_to_string(table_id)));
    LOG_CLI((BSL_META_U(unit,"{")));
    for(i = 0; i < table_config_info.entry_key_parsing.nof_segments; i++) {
        LOG_CLI((BSL_META_U(unit,"%s\\%d bytes"), table_config_info.entry_key_parsing.key_segment[i].name, table_config_info.entry_key_parsing.key_segment[i].nof_bytes));
        if (i != table_config_info.entry_key_parsing.nof_segments - 1) {
            LOG_CLI((BSL_META_U(unit,", ")));
        }
    }
    LOG_CLI((BSL_META_U(unit,"}\t")));

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_print_db_search_info()", 0, 0);
}

static uint32
  arad_kbp_print_searches(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 ltr_indx,
    SOC_SAND_IN  uint8                  *result)
{
    int i,j,search_id;
    uint32 result_length;
    uint32 res;
    int result_start_offset = 1; 

    ARAD_KBP_LTR_CONFIG             Arad_kbp_ltr_config = {0};

    uint32 max_nof_device_searches;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_LTR_CONFIG_clear(&Arad_kbp_ltr_config);

    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        max_nof_device_searches = 4; 
    } else if(ARAD_KBP_IS_OP_OR_OP2) {
        max_nof_device_searches = 8; 
    } else {
        max_nof_device_searches = 6; 
    }

    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_indx, &Arad_kbp_ltr_config);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    for (i = 0, search_id = 0; search_id < max_nof_device_searches; i++, search_id++) {
        LOG_CLI((BSL_META_U(unit,"| search index %d\t"),search_id));

        res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_data_len.get(unit, ltr_indx, i, &result_length);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if ((Arad_kbp_ltr_config.tbl_id[i] >= ARAD_KBP_FRWRD_TBL_ID_DUMMY_0) &&
            (Arad_kbp_ltr_config.tbl_id[i] < (ARAD_KBP_FRWRD_TBL_ID_DUMMY_0+ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS))) 
        {   
            LOG_CLI((BSL_META_U(unit,"Empty\n")));
        }else{
            
            int print_private_table = -1;
            if(search_id == 0) {
                if(ltr_indx == ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC || ltr_indx == ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC_RPF) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
                } else if(ltr_indx == ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC || ltr_indx == ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC_RPF) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0;
                }
            } else if(search_id == 3) { 
                if(ltr_indx == ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC_RPF) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1;
                } else if(ltr_indx == ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC_RPF) {
                    print_private_table = ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1;
                }
            }
            if(print_private_table != -1) {
                res = arad_kbp_print_db_search_info(unit, print_private_table);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

                
                if (result[0]& 1<<(7-i)) {
                    LOG_CLI((BSL_META_U(unit, "Found\tresult=0x")));
                    for(j=0;j<result_length;j++){
                        LOG_CLI((BSL_META_U(unit,"%02x"),result[result_start_offset+j]));
                    }
                    LOG_CLI((BSL_META_U(unit,"\n")));
                }else{
                    LOG_CLI((BSL_META_U(unit, "Not Found\n")));
                }

                
                LOG_CLI((BSL_META_U(unit,"| search index %d\t"), ++search_id));
            }

            LOG_CLI((BSL_META_U(unit,"Table ID=%d, %s\t"),Arad_kbp_ltr_config.tbl_id[i], ARAD_KBP_FRWRD_IP_TBL_ID_to_string(Arad_kbp_ltr_config.tbl_id[i])));
            LOG_CLI((BSL_META_U(unit,"{")));
            for (j=0;j<Arad_kbp_ltr_config.ltr_search[i].nof_key_segments;j++) {
                LOG_CLI((BSL_META_U(unit,"%s\\%d bytes"),Arad_kbp_ltr_config.ltr_search[i].key_segment[j].name,Arad_kbp_ltr_config.ltr_search[i].key_segment[j].nof_bytes));
                if (j!=Arad_kbp_ltr_config.ltr_search[i].nof_key_segments-1) {
                    LOG_CLI((BSL_META_U(unit,", ")));
                }
            }
            LOG_CLI((BSL_META_U(unit,"}\t")));

            
            if (result[0]& 1<<(7-i)) {
                LOG_CLI((BSL_META_U(unit, "Found\tresult=0x")));
                for(j=0;j<result_length;j++){
                    LOG_CLI((BSL_META_U(unit,"%02x"),result[result_start_offset+j]));
                }
                LOG_CLI((BSL_META_U(unit,"\n")));
            }else{
                LOG_CLI((BSL_META_U(unit, "Not Found\n")));
            }
        }
        result_start_offset += result_length;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_print_searches()", 0, 0);
}

static void
  arad_kbp_print_result(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                  *data,
    SOC_SAND_IN  int                    data_len_bytes,
    SOC_SAND_IN  int                    hit_bits_len_bytes)

{
    int indx=0;
    LOG_CLI((BSL_META_U(unit,"| Hit bits=0x")));
    for (indx=0;indx<hit_bits_len_bytes;indx++) {
        LOG_CLI((BSL_META_U(unit,"%02x"),data[indx]));
    }
    LOG_CLI((BSL_META_U(unit,"\t")));
    LOG_CLI((BSL_META_U(unit,"Data=0x")));
    for (;indx<data_len_bytes;indx++) {
        LOG_CLI((BSL_META_U(unit,"%02x"),data[indx]));
    }
    LOG_CLI((BSL_META_U(unit,"\n")));
}

int
  arad_kbp_print_diag_entry_added(
     SOC_SAND_IN  int               unit,
     SOC_SAND_IN  tableInfo         *tbl_info,
     SOC_SAND_IN  uint8             *data,
     SOC_SAND_IN  uint32            prefix_len,
     SOC_SAND_IN  uint8             *mask,
     SOC_SAND_IN  uint8             *ad_data)
{
    int table_id;
    int tbl_width_bytes;
    uint8 is_acl = FALSE;
    ARAD_KBP_TABLE_CONFIG kbp_table_config_info = {0};
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(tbl_info);
    SOC_SAND_CHECK_NULL_INPUT(data);

    ARAD_KBP_TABLE_CONFIG_clear(&kbp_table_config_info);

    table_id = tbl_info->tbl_id;
    tbl_width_bytes = tbl_info->tbl_width >> 3; 
    LOG_CLI((BSL_META_U(unit,"|------------------------------------------------------------------------------------------------------------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"| New entry was added to KBP, Table ID = %d, %s\n"),table_id, ARAD_KBP_FRWRD_IP_TBL_ID_to_string(table_id)));

    if (table_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES) {
        is_acl = TRUE;
    }

    res = KBP_INFO.Arad_kbp_table_config_info.get(unit, table_id, &kbp_table_config_info); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    arad_kbp_parse_and_print_entry(unit, is_acl,
                                   data, mask, tbl_width_bytes,prefix_len,
                                   ad_data, kbp_table_config_info.tbl_asso_width/8,
                                   kbp_table_config_info.entry_key_parsing.nof_segments,
                                   kbp_table_config_info.entry_key_parsing.key_segment,
                                   -1);

    LOG_CLI((BSL_META_U(unit,"|-------------------------------------------------------------------------------------------------------------------------------------------\n\n")));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_print_diag_entry_added()", 0, 0);
}

int
  arad_kbp_print_diag_all_entries(int unit, int print_table){

    struct kbp_db 
        *db_p = NULL;
    struct kbp_entry_iter
        *iter_p = NULL;
    struct kbp_entry
        *db_entry_p = NULL;
    struct kbp_entry_info
        info;
    struct kbp_ad_db 
        *ad_db_p = NULL;

    uint32 table_id;
    uint32 res;
    uint8 is_acl = FALSE;
    int i;
    uint8 assoData[256];
    uint8 valid_ad = 0;
    int half[3];
    int iterate_start;
    int iterate_end;

    ARAD_KBP_TABLE_CONFIG kbp_table_config_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_TABLE_CONFIG_clear(&kbp_table_config_info);


    if(NULL == AradAppData[unit])
    {
        LOG_CLI((BSL_META_U(unit, "\n***** KBP Device is not yet installed with KBP configuration!\n\n")));
        return 0;
    }

    
    
    if(print_table < 0) {
        iterate_start = 0;
        iterate_end = ARAD_KBP_MAX_NUM_OF_TABLES;
    } else if(print_table >= ARAD_KBP_MAX_NUM_OF_TABLES) {
        LOG_CLI((BSL_META_U(unit, "\nInvalid Table ID\n")));
        ARAD_DO_NOTHING_AND_EXIT;
    } else {
        iterate_start = print_table;
        iterate_end = print_table + 1;
    }

    LOG_CLI((BSL_META_U(unit,"|-------------------------------------------------------------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|---------------------------------------  KBP Entries  -------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|-------------------------------------------------------------------------------------------\n")));

    for (table_id = iterate_start; table_id < iterate_end; table_id++) {
        i=0;
        half[0] = 0;
        half[1] = 0;
        half[2] = 0;

        
        res =  arad_kbp_alg_kbp_db_get(
                unit,
                table_id, 
                &db_p
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(db_p == NULL){
            continue;
        }
        res =  arad_kbp_alg_kbp_ad_db_get(
                unit,
                table_id, 
                &ad_db_p
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        valid_ad = (ad_db_p == NULL) ? 0 : 1;

        res = KBP_INFO.Arad_kbp_table_config_info.get(unit, table_id, &kbp_table_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        res = kbp_db_entry_iter_init(db_p, &iter_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit, "Error in %s(): kbp_db_entry_iter_init failed: %s\n"),
                      FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
        SOC_SAND_CHECK_NULL_PTR(iter_p, 20, exit);

        res = kbp_db_entry_iter_next(db_p, iter_p, &db_entry_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit, "Error in %s(): kbp_db_entry_iter_next failed: %s\n"),
                      FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }

        while (db_entry_p != NULL) {
            if (i==0) {
                LOG_CLI((BSL_META_U(unit,"|\n")));
                LOG_CLI((BSL_META_U(unit,"| Table ID = %d, %s:\n"), table_id,ARAD_KBP_FRWRD_IP_TBL_ID_to_string(table_id)));
            }
            res = kbp_entry_get_info(db_p, db_entry_p, &info);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit, "Error in %s(): kbp_entry_get_info failed: %s\n"),
                          FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            sal_memset(assoData, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit));

            if (valid_ad) {
                res = kbp_ad_db_get(ad_db_p, info.ad_handle, assoData);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                              (BSL_META_U(unit, "Error in %s(): kbp_ad_db_get failed: %s\n"),
                              FUNCTION_NAME(), kbp_get_status_string(res)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
                }
            }

            
            if (table_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES) {
                is_acl = TRUE;
            }

            arad_kbp_parse_and_print_entry(unit, is_acl,
                                           info.data, info.mask, info.width_8,info.prio_len,
                                           assoData, (valid_ad == 1)?kbp_table_config_info.tbl_asso_width/8:0,
                                           kbp_table_config_info.entry_key_parsing.nof_segments,
                                           kbp_table_config_info.entry_key_parsing.key_segment,
                                           i++);

            half[info.which_half]++;
            res = kbp_db_entry_iter_next(db_p, iter_p, &db_entry_p);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit, "Error in %s(): kbp_db_entry_iter_next failed: %s\n"),
                          FUNCTION_NAME(), kbp_get_status_string(res)));
                ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);
            }
        }
        if (i != 0) {
            if (table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_DC) {
                LOG_CLI((BSL_META_U(unit, "|half[0] = %d, half[1]=%d, half[2]=%d, total=%d\n"),half[0],half[1],half[2],half[0] + half[1] + half[2]));
            }
            LOG_CLI((BSL_META_U(unit,"|___________________________________________________________________________________________\n")));
        }
        
        res = kbp_db_entry_iter_destroy(db_p, iter_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit, "Error in %s(): kbp_db_entry_iter_destroy failed: %s\n"),
                      FUNCTION_NAME(), kbp_get_status_string(res)));
        }
    }
    LOG_CLI((BSL_META_U(unit,"|                                                                                           \n")));
    LOG_CLI((BSL_META_U(unit,"|-----------------------------------  End Of KBP Entries  ----------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|___________________________________________________________________________________________\n\n")));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_print_diag_all_entries()", 0, 0);
}

int
  arad_kbp_print_diag_last_packet(int unit, int core, int flp_program){

    uint32 res;
    uint32 signal_value[24];
    uint8 prog_opcode, signal_opcode, sw_prog_id=0xff;
    int core_id = core;
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART5;
    ARAD_KBP_LTR_SINGLE_SEARCH master_key;
    int signal_nof_bits, signal_nof_bytes, signal_nof_32bits;
    int master_key_len_bytes=0,master_key_bytes_counter;
    int i,indx=0;
    uint8 *signal_byte_ptr;
    uint8 master_key_value[ARAD_KBP_MASTER_KEY_MAX_LENGTH_BYTES];
    uint8 result_value[ARAD_KBP_RESULT_MAX_LENGTH_BYTES]={0};
    uint32   db_type, ltr_id;
    SOC_SAND_SUCCESS_FAILURE    success;
    int prog_id, num_of_progs;
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    master_key.nof_key_segments=0;

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    if (flp_program == -1) {
        
        res = arad_pp_flp_access_print_last_programs_data(unit, core_id, 0, &prog_id, &num_of_progs);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);
    }else{
        prog_id = flp_program;
    }

    
    res = arad_pp_prog_index_to_flp_app_get(unit,prog_id,&sw_prog_id);
    ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);
    prog_opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(sw_prog_id);

    
    res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_OPCODE, signal_value, &signal_nof_bits);
    ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

    
    signal_opcode = signal_value[0]&0xFF;
    if (prog_opcode != signal_opcode) {
        uint8 last_prog_id;
        arad_pp_flp_app_to_prog_index_get(unit,ARAD_KBP_OPCODE_TO_FLP_PROG(signal_opcode),&last_prog_id);
        if (flp_program == -1) {
            LOG_CLI((BSL_META_U(unit, "Last FLP program (ID=%d) did not perform a KBP lookup on core=%d\nLast FLP program with KBP lookup is ID=%d\n"), prog_id, core_id, last_prog_id));
        }else{
            LOG_CLI((BSL_META_U(unit, "The required FLP program (ID=%d) did not perform a KBP lookup on core=%d\nLast FLP program with KBP lookup is ID=%d\n"), prog_id, core_id, last_prog_id));
        }
        return 0;
    }

    
    res = arad_kbp_opcode_to_db_type(unit, signal_opcode, &db_type, &ltr_id, &success);
    ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);
    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info);

    master_key = ltr_config_info.master_key_fields;

    
    for (i=0;i<master_key.nof_key_segments;i++) {
        master_key_len_bytes += master_key.key_segment[i].nof_bytes;
    }
    master_key_bytes_counter = master_key_len_bytes;

    
    if (master_key_len_bytes == 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Master key length is zero; opcode %d\n"),
                        FUNCTION_NAME(), signal_opcode));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

    

    
    res = arad_pp_signal_mngr_signal_get(unit, core_id, signal_id, signal_value, &signal_nof_bits);
    ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

    arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

    master_key_value[indx++] = signal_value[0]&0xFF;
    master_key_bytes_counter -= 1; 
    signal_id--; 

    while (master_key_bytes_counter > 0) {
        
        res = arad_pp_signal_mngr_signal_get(unit, core_id, signal_id, signal_value, &signal_nof_bits);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

        arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

        signal_nof_bytes  = (signal_nof_bits+7)/8;
        signal_nof_32bits = (signal_nof_bits+31)/32;

        while (signal_nof_32bits > 0) {
            
            signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
            master_key_value[indx++] = *signal_byte_ptr++;
            master_key_value[indx++] = *signal_byte_ptr++;
            master_key_value[indx++] = *signal_byte_ptr++;
            master_key_value[indx++] = *signal_byte_ptr++;
            signal_nof_32bits--;
        }
        master_key_bytes_counter -= signal_nof_bytes;
        signal_id--;
    }

    LOG_CLI((BSL_META_U(unit,"|-------------------------------------------------------------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|------------------------------------  KBP Last Packet  ------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|-------------------------------------------------------------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"| Core = %d, FLP program = %s (opcode=%d), LTR index = %d\n"), core_id, ARAD_KBP_FRWRD_IP_DB_TYPE_to_string(db_type), signal_opcode, db_type));

    
    LOG_CLI((BSL_META_U(unit,"|\n")));
    LOG_CLI((BSL_META_U(unit,"| -Master key \\ %d bytes:\n"),master_key_len_bytes));
    arad_kbp_parse_and_print_entry(unit, FALSE,
                                   master_key_value, NULL, master_key_len_bytes, -1,
                                   NULL, 0,
                                   master_key.nof_key_segments,
                                   master_key.key_segment,
                                   -1);

    
    res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES, signal_value, &signal_nof_bits);
    ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

    arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

    signal_nof_bytes  = (signal_nof_bits+7)/8;
    signal_nof_32bits = (signal_nof_bits+31)/32;
    indx = 0;
    while (signal_nof_32bits > 0) {
        
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        signal_nof_32bits--;
    }

    if (SOC_IS_QAX(unit)) {
        
        res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART2, signal_value, &signal_nof_bits);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

        arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

        
        signal_nof_32bits = (signal_nof_bits+31)/32;
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
        signal_byte_ptr += 3;
        result_value[indx++] = *signal_byte_ptr++;
        signal_nof_32bits--;

        while (signal_nof_32bits > 0) {
            
            signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
            result_value[indx++] = *signal_byte_ptr++;
            result_value[indx++] = *signal_byte_ptr++;
            result_value[indx++] = *signal_byte_ptr++;
            result_value[indx++] = *signal_byte_ptr++;
            signal_nof_32bits--;
        }

        
        res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART1, signal_value, &signal_nof_bits);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

        arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

        
        signal_nof_32bits = (signal_nof_bits+31)/32;
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
        signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        signal_nof_32bits--;
    }
    else if (SOC_IS_JERICHO_PLUS(unit)) {
        
        res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART2, signal_value, &signal_nof_bits);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

        arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

        
        signal_nof_32bits = (signal_nof_bits+31)/32;
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
        signal_byte_ptr++;
        signal_byte_ptr++;
        result_value[indx] = (*signal_byte_ptr&0x1F)<<3;
        signal_byte_ptr++;
        result_value[indx++] |= (*signal_byte_ptr&0xE0)>>5;
        result_value[indx] = (*signal_byte_ptr&0x1F)<<3;
        signal_byte_ptr++;
        signal_nof_32bits--;

        while (signal_nof_32bits > 0) {
            
            signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
            result_value[indx++] |= (*signal_byte_ptr&0xE0)>>5;
            result_value[indx] = (*signal_byte_ptr&0x1F)<<3;
            signal_byte_ptr++;
            result_value[indx++] |= (*signal_byte_ptr&0xE0)>>5;
            result_value[indx] = (*signal_byte_ptr&0x1F)<<3;
            signal_byte_ptr++;
            result_value[indx++] |= (*signal_byte_ptr&0xE0)>>5;
            result_value[indx] = (*signal_byte_ptr&0x1F)<<3;
            signal_byte_ptr++;
            result_value[indx++] |= (*signal_byte_ptr&0xE0)>>5;
            result_value[indx] = (*signal_byte_ptr&0x1F)<<3;
            signal_byte_ptr++;
            signal_nof_32bits--;
        }

        
        res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART1, signal_value, &signal_nof_bits);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

        arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

        
        signal_nof_32bits = (signal_nof_bits+31)/32;
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];

        signal_byte_ptr++;
        result_value[indx++] |= (*signal_byte_ptr&0x07);
        signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        signal_nof_32bits--;
    }
    else if (SOC_IS_JERICHO(unit)) {
        
        res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART2, signal_value, &signal_nof_bits);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

        arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

        
        signal_nof_32bits = (signal_nof_bits+31)/32;
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
        signal_byte_ptr++;
        signal_byte_ptr++;
        signal_byte_ptr++;
        result_value[indx] = ((*signal_byte_ptr&0x7F))<<1;
        signal_nof_32bits--;

        
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
        result_value[indx++] |= (*signal_byte_ptr&0x80)>>7;
        result_value[indx]    = (*signal_byte_ptr&0x7F)<<1;
        signal_byte_ptr++;
        result_value[indx++] |= (*signal_byte_ptr&0x80)>>7;
        result_value[indx]    = (*signal_byte_ptr&0x7F)<<1;
        signal_byte_ptr++;
        result_value[indx++] |= (*signal_byte_ptr&0x80)>>7;
        result_value[indx]    = (*signal_byte_ptr&0x7F)<<1;
        signal_byte_ptr++;
        result_value[indx++] |= (*signal_byte_ptr&0x80)>>7;
        result_value[indx]    = (*signal_byte_ptr&0x7F)<<1;

        
        res = arad_pp_signal_mngr_signal_get(unit, core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART1, signal_value, &signal_nof_bits);
        ARAD_KBP_CHECK_FUNC_RESULT(res, 10, exit);

        arad_kbp_diag_endians_fix(unit, signal_value, signal_nof_bits);

        
        signal_nof_32bits = (signal_nof_bits+31)/32;
        signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];

        result_value[indx++] |= (*signal_byte_ptr&0x1);
        signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        result_value[indx++] = *signal_byte_ptr++;
        signal_nof_32bits--;

        while (signal_nof_32bits > 0) {
            
            signal_byte_ptr = (uint8*)&signal_value[signal_nof_32bits-1];
            result_value[indx++] = *signal_byte_ptr++;
            result_value[indx++] = *signal_byte_ptr++;
            result_value[indx++] = *signal_byte_ptr++;
            result_value[indx++] = *signal_byte_ptr++;
            signal_nof_32bits--;
        }
    }
    
    LOG_CLI((BSL_META_U(unit,"|\n")));
    LOG_CLI((BSL_META_U(unit,"| -Look Ups associated with the LTR:\n")));
    arad_kbp_print_searches(unit, db_type, result_value);

    
    LOG_CLI((BSL_META_U(unit,"|\n")));
    LOG_CLI((BSL_META_U(unit,"| -Result (Row of Data):\n")));
    arad_kbp_print_result(unit, result_value, 32, 1);


    LOG_CLI((BSL_META_U(unit,"|                                                                                           \n")));
    LOG_CLI((BSL_META_U(unit,"|-----------------------------------  End Of KBP packet ------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|___________________________________________________________________________________________\n\n")));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_print_diag_last_key()", 0, 0);
}

int
  arad_kbp_print_search_result(int unit, uint32 ltr_idx, uint8 *master_key_buffer, struct kbp_search_result *search_rslt){

    int res, i, j;
    uint8 result[32] = {0};
    uint32 result_width_byte, result_length_byte, result_offset_byte = 1;
    ARAD_KBP_LTR_SINGLE_SEARCH master_key;
    ARAD_KBP_LTR_CONFIG ltr_config_info;
    int master_key_len_bytes = 0;
    NlmAradCompareResponseFormat configured_result_type;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(result, 0x0, sizeof(uint8) * 32);

    for (i=0;i<ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS;i++) {
        res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_data_len.get(unit, ltr_idx, i, &result_length_byte);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_format.get(unit, ltr_idx, i, &configured_result_type);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        if((hit_bit_result_for_do_search & (0x1 << (7 - i)))
            || ((search_rslt->hit_or_miss[i] == KBP_HIT) && (search_rslt->result_valid[i] == KBP_RESULT_IS_VALID))){
            
            if ((search_rslt->resp_type[i] > KBP_ONLY_AD_256B) || (search_rslt->resp_type[i] < KBP_INDEX_AND_32B_AD)) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): Result response type out of range\n"),
                                FUNCTION_NAME()));
                SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_ACTION_SIZE_OUT_OF_RANGE_ERR, 10, exit);
            }
            
            result[0] |= 0x1<<(7-i);

            
            result_width_byte = (0x1<<((int)configured_result_type + 1));

            for (j=0;j<result_length_byte;j++) {
                result[result_offset_byte + j] = search_rslt->assoc_data[i][result_width_byte-(result_length_byte-j)];
            }

        }
        result_offset_byte += result_length_byte;
    }

    LOG_CLI((BSL_META_U(unit,"|-------------------------------------------------------------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|-------------------------------------  KBP Do search  -------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|-------------------------------------------------------------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|LTR index = %d\n"),ltr_idx));
    LOG_CLI((BSL_META_U(unit,"|NOTE!! The result value of R2 in compare3 LTR is invalid, but hit indication is valid.\n")));

    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_idx, &ltr_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10+ltr_idx, exit);

    master_key = ltr_config_info.master_key_fields;

    
    for (i=0;i<master_key.nof_key_segments;i++) {
        master_key_len_bytes += master_key.key_segment[i].nof_bytes;
    }

    
    LOG_CLI((BSL_META_U(unit,"|\n")));
    LOG_CLI((BSL_META_U(unit,"| -Master key \\ %d bytes:\n"),master_key_len_bytes));
    arad_kbp_parse_and_print_entry(unit, FALSE,
                                   master_key_buffer, NULL, master_key_len_bytes,-1,
                                   NULL, 0,
                                   master_key.nof_key_segments,
                                   master_key.key_segment,
                                   -1);
    
    LOG_CLI((BSL_META_U(unit,"|\n")));
    LOG_CLI((BSL_META_U(unit,"| -Look Ups associated with the LTR:\n")));
    arad_kbp_print_searches(unit, ltr_idx, result);

    LOG_CLI((BSL_META_U(unit,"|                                                                                           \n")));
    LOG_CLI((BSL_META_U(unit,"|---------------------------------  End Of KBP Do search -----------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,"|___________________________________________________________________________________________\n\n")));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_print_search_result()", 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif 

