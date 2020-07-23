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

#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/field.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/ha.h>
#endif

















static
    ARAD_KBP_FRWRD_IP_LTR
        Arad_kbp_db_type_to_ltr[ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD_PLUS] = {
            ARAD_KBP_FRWRD_LTR_IPV4_UC,
            ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF,
            ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF, 
            ARAD_KBP_FRWRD_LTR_IPV6_UC,
            ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_2PASS,
            ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF,
            ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF,
            ARAD_KBP_FRWRD_LTR_LSR,
            ARAD_KBP_FRWRD_LTR_TRILL_UC,
            ARAD_KBP_FRWRD_LTR_TRILL_MC,
            ARAD_KBP_FRWRD_LTR_IPV4_DC,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP_WITH_RPF,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_LSR,
            ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_IPV6,
            ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_P2P,
            ARAD_KBP_FRWRD_LTR_IPV4_UC_PUBLIC,
            ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF_PUBLIC,
            ARAD_KBP_FRWRD_LTR_IPV6_UC_PUBLIC,
            ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_PUBLIC,
            ARAD_KBP_FRWRD_LTR_IPV4_MC_BRIDGE,
            ARAD_KBP_FRWRD_LTR_DUMMY_FRWRD,
        };

static ARAD_KBP_LTR_SINGLE_SEARCH ipv6_dip_sip_sharing_forwarding_header_search =
       {4, {{"DUMMY_KEY_SEARCH", 1, KBP_KEY_FIELD_PREFIX}, {"UNUSED_HEADER_PART", 7, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}};

static ARAD_KBP_LTR_SINGLE_SEARCH ipv6_dip_sip_sharing_forwarding_header_search_op =
       {4, {{"DUMMY_KEY_SEARCH", 1, KBP_KEY_FIELD_EM}, {"UNUSED_HEADER_PART", 7, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_EM}}};

static ARAD_KBP_LTR_SINGLE_SEARCH ipv6_dip_sip_sharing_dynamic_segments_ipv6_uc =
       {4, {{"DIP", 16, KBP_KEY_FIELD_EM}, {"DUMMY_KEY_SEARCH", 1, KBP_KEY_FIELD_EM}, {"PADDING_1", 1, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}};

static ARAD_KBP_LTR_SINGLE_SEARCH ipv6_dip_sip_sharing_dynamic_segments_ipv6_uc_public =
       {4, {{"SIP", 16, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX},  {"DUMMY_KEY_SEARCH", 1, KBP_KEY_FIELD_EM}, {"PADDING_1", 1, KBP_KEY_FIELD_EM}}};

static ARAD_KBP_LTR_SINGLE_SEARCH ipv6_dip_sip_sharing_dynamic_segments_ipv6_uc_rpf =
       {5, {{"SIP", 16, KBP_KEY_FIELD_EM}, {"DUMMY_KEY_SEARCH", 1, KBP_KEY_FIELD_EM}, {"PADDING_1", 1, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}, {"PADDING_2", 2, KBP_KEY_FIELD_EM}}};

static ARAD_KBP_LTR_SINGLE_SEARCH l2_da_sa_sharing_forwarding_header_search =
       {2, {{"DA", 6, KBP_KEY_FIELD_EM}, {"SA", 6, KBP_KEY_FIELD_EM}}};

ARAD_KBP_LTR_SINGLE_SEARCH dummy_key_search =
       {1, {{"DUMMY_KEY_SEARCH", 1, KBP_KEY_FIELD_PREFIX}}};





STATIC uint32 arad_kbp_db_create_verify(
       SOC_SAND_IN  ARAD_KBP_TABLE_CONFIG   *sw_table)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(sw_table);

    
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(sw_table->bankNum, NLMDEV_BANK_0, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(sw_table->group_id_start, 0, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(sw_table->group_id_end, 0, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 30, exit);

    
    if(sw_table->tbl_width != NLM_TBL_WIDTH_80 &&
       sw_table->tbl_width != NLM_TBL_WIDTH_160 &&
       sw_table->tbl_width != NLM_TBL_WIDTH_320 &&
       sw_table->tbl_width != NLM_TBL_WIDTH_480 &&
       sw_table->tbl_width != NLM_TBL_WIDTH_640)
    {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table %d has invalid width %d\n"),
                        FUNCTION_NAME(), sw_table->tbl_id, sw_table->tbl_width));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
    }

    
    if(sw_table->tbl_asso_width != NLM_TBL_ADLEN_ZERO &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_24B &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_32B &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_48B &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_64B &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_128B )
    {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table %d has invalid asso_width %d\n"),
                        FUNCTION_NAME(), sw_table->tbl_id, sw_table->tbl_asso_width));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_db_create_verify()", 0, 0);
}

STATIC uint32 arad_kbp_db_create(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  ARAD_KBP_TABLE_CONFIG       *sw_table,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success)
{
    uint32
        res = SOC_SAND_OK;

    nlm_u8 valid = FALSE;

    uint8
        check_only;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(sw_table);

    check_only = (flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY) ? TRUE : FALSE;
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;

    
    res = arad_kbp_db_create_verify(sw_table);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = KBP_INFO.Arad_kbp_table_config_info.valid.get(unit, table_id, &valid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if(valid == FALSE)
    {
        if (!check_only)
        {
            res = KBP_INFO.Arad_kbp_table_config_info.set(unit, table_id, sw_table);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit, table_id, TRUE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            res = KBP_INFO.Arad_kbp_table_config_info.tbl_id.set(unit, table_id, table_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }
        *success = SOC_SAND_SUCCESS;
    }
    else
    {
        
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table %d is already configured\n"),
                        FUNCTION_NAME(), table_id));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_db_create()", 0, 0);
}


STATIC void ARAD_KBP_QUAL_TYPE_to_string(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_PPC_FP_QUAL_TYPE     qual_type,
    SOC_SAND_OUT char*                   qual_name)
{
    switch(qual_type){
    case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP:
    case SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP:
    case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH:
    case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW:
        sal_strcpy(qual_name, "DIP");
        break;
    case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP:
    case SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP:
    case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH:
    case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW:
        sal_strcpy(qual_name, "SIP");
        break;
    case SOC_PPC_FP_QUAL_IRPP_VRF:
        sal_strcpy(qual_name, "VRF");
        break;
    case SOC_PPC_FP_QUAL_IRPP_IN_RIF:
        sal_strcpy(qual_name, "IN-RIF");
        break;
    case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD:
        sal_strcpy(qual_name, "MPLS-KEY");
        break;
    default:
        sal_strcpy(qual_name, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type));
        break;
    }
}


STATIC uint32 arad_kbp_convert_quals_to_segments(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN SOC_DPP_DBAL_QUAL_INFO      qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_IN int                         nof_qualifiers,
    SOC_SAND_IN int                         prog_id,
    SOC_SAND_OUT ARAD_KBP_LTR_SINGLE_SEARCH *master_key)
{
    int qual_ndx;
    int segment_ndx = -1;
    int qual_length_in_bits=0;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    char next_segment_name[20];
    char segment_name[20];
    int segment_length_in_bits=0;
    int zeroes_ones_padding_length = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(master_key, 0x0, sizeof(ARAD_KBP_LTR_SINGLE_SEARCH));
    sal_strcpy(segment_name,"");
    sal_strcpy(next_segment_name,"");

    for (qual_ndx = nof_qualifiers-1; qual_ndx >= 0 ; qual_ndx--) {
        qual_length_in_bits = qual_info[qual_ndx].qual_nof_bits == 0 ? qual_info[qual_ndx].qual_full_size : qual_info[qual_ndx].qual_nof_bits;
        qual_type = qual_info[qual_ndx].qual_type;
        if ((qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES)||(qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {
            zeroes_ones_padding_length = qual_length_in_bits;
            continue;
        }
        ARAD_KBP_QUAL_TYPE_to_string(unit, qual_type, next_segment_name);
        if (sal_strcmp(segment_name,next_segment_name) != 0) {
            
            if (segment_ndx >=0) {
                
                master_key->nof_key_segments++;
                sal_strcpy(master_key->key_segment[segment_ndx].name,segment_name);
                if ((segment_length_in_bits & 0x7) != 0) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): Qualifier is not byte aligned. prog_id %d segment %d qual_type %d bits %d\n"),
                                    FUNCTION_NAME(), prog_id, segment_ndx, qual_type, segment_length_in_bits));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
                }
                master_key->key_segment[segment_ndx].nof_bytes = (segment_length_in_bits)/8;
                if(ARAD_KBP_IS_OP_OR_OP2) {
                    master_key->key_segment[segment_ndx].type = KBP_KEY_FIELD_EM;
                } else {
                    master_key->key_segment[segment_ndx].type = KBP_KEY_FIELD_PREFIX;
                }
            }
            segment_ndx++;
            segment_length_in_bits = qual_length_in_bits+zeroes_ones_padding_length;
            zeroes_ones_padding_length=0;
            sal_strcpy(segment_name,next_segment_name);
        } else {
            segment_length_in_bits += qual_length_in_bits;
        }
    }
    master_key->nof_key_segments++;
    if ((segment_length_in_bits & 0x7) != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Qualifier is not byte alligned\n"),
                        FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
    }

    if (segment_ndx >= 0) {
        sal_strcpy(master_key->key_segment[segment_ndx].name,segment_name);
        master_key->key_segment[segment_ndx].nof_bytes = (segment_length_in_bits)/8;
        if(ARAD_KBP_IS_OP_OR_OP2) {
            master_key->key_segment[segment_ndx].type = KBP_KEY_FIELD_EM;
        } else {
            master_key->key_segment[segment_ndx].type = KBP_KEY_FIELD_PREFIX;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_convert_quals_to_segments", segment_length_in_bits, 0);
}


uint32 arad_kbp_update_master_key(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     prog_id,
    SOC_SAND_IN SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_IN int                     nof_qualifiers)
{
    int i,res;
    uint8 opcode;
    uint8 sw_prog_id=0xFF;
    uint32 db_ndx,ltr_id;
    ARAD_KBP_LTR_SINGLE_SEARCH master_key;
    ARAD_KBP_LTR_CONFIG kbp_ltr_config;
    SOC_SAND_SUCCESS_FAILURE success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_kbp_convert_quals_to_segments(unit,qual_info,nof_qualifiers,prog_id,&master_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

    
    arad_pp_prog_index_to_flp_app_get(unit,prog_id,&sw_prog_id);
    opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(sw_prog_id);
    res = arad_kbp_opcode_to_db_type(
              unit,
              opcode,
              &db_ndx,
              &ltr_id,
              &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

    if (success != SOC_SAND_SUCCESS) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): LTR IDs exhausted\n"),
                        FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 2, exit);
    }

    
    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_ndx, &kbp_ltr_config);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10+db_ndx, exit);

    if (kbp_ltr_config.valid == FALSE) {
        kbp_ltr_config.valid = TRUE;
        kbp_ltr_config.ltr_id = ltr_id;
        kbp_ltr_config.opcode = opcode;
    }

    kbp_ltr_config.master_key_fields.nof_key_segments = master_key.nof_key_segments;
    for (i=0;i<master_key.nof_key_segments;i++) {
        kbp_ltr_config.master_key_fields.key_segment[i].nof_bytes = master_key.key_segment[i].nof_bytes;
        kbp_ltr_config.master_key_fields.key_segment[i].type = master_key.key_segment[i].type;
        sal_strcpy(kbp_ltr_config.master_key_fields.key_segment[i].name,master_key.key_segment[i].name);
    }

    res = KBP_INFO.Arad_kbp_ltr_config.set(unit, db_ndx, &kbp_ltr_config);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10+db_ndx, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_update_master_key", 0,0);
}


uint32 arad_kbp_update_lookup(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     table_id,
    SOC_SAND_IN int                     prog_id,
    SOC_SAND_IN int                     search_id,
    SOC_SAND_IN SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_IN int                     nof_qualifiers)
{
    int i,res;
    int key_length=0;
    uint8 opcode;
    uint8 sw_prog_id=0xFF;
    uint32 db_ndx,ltr_id;
    ARAD_KBP_LTR_SINGLE_SEARCH lookup_key;
    ARAD_KBP_LTR_CONFIG kbp_ltr_config;
    ARAD_KBP_TABLE_CONFIG kbp_table_config,kbp_table;
    SOC_SAND_SUCCESS_FAILURE success;
    int tbl_id;
    
    uint32 tmp_parallel_srches_bmp[1] = { 0 };

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_kbp_convert_quals_to_segments(unit,qual_info,nof_qualifiers,prog_id,&lookup_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

    
    arad_pp_prog_index_to_flp_app_get(unit,prog_id,&sw_prog_id);
    opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(sw_prog_id);
    res = arad_kbp_opcode_to_db_type(
              unit,
              opcode,
              &db_ndx,
              &ltr_id,
              &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

    if (success != SOC_SAND_SUCCESS) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): LTR IDs exhausted\n"),
                        FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 2, exit);
    }

    
    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_ndx, &kbp_ltr_config);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10+db_ndx, exit);

    
    if (kbp_ltr_config.valid == FALSE) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): LTR ID %d is invalid and cannot be updated\n"),
                        FUNCTION_NAME(), db_ndx));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
    }

    kbp_ltr_config.ltr_search[search_id].nof_key_segments = lookup_key.nof_key_segments;
    for (i=0;i<lookup_key.nof_key_segments;i++) {
        key_length += lookup_key.key_segment[i].nof_bytes;
        kbp_ltr_config.ltr_search[search_id].key_segment[i].nof_bytes = lookup_key.key_segment[i].nof_bytes;
        kbp_ltr_config.ltr_search[search_id].key_segment[i].type = lookup_key.key_segment[i].type;
        sal_strcpy(kbp_ltr_config.ltr_search[search_id].key_segment[i].name,lookup_key.key_segment[i].name);
    }

    if(table_id == -1){
        
        SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  success;
        ARAD_KBP_TABLE_CONFIG_clear(&kbp_table);
        kbp_table.tbl_id = ARAD_KBP_MAX_NUM_OF_TABLES;

        
        if (key_length <= 10) {
            kbp_table.tbl_width = NLM_TBL_WIDTH_80;
        }
        else if (key_length <= 20) {
            kbp_table.tbl_width = NLM_TBL_WIDTH_160;
        }
        else if (key_length <= 40) {
            kbp_table.tbl_width = NLM_TBL_WIDTH_320;
        }
        else if (key_length <= 60) {
            kbp_table.tbl_width = NLM_TBL_WIDTH_480;

        } else {
            
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Key length (%d bits) exceeds the maximum table width (480 bits)\n"),
                            FUNCTION_NAME(), (key_length * 8)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
        }

        kbp_table.tbl_asso_width = NLM_TBL_ADLEN_64B;
        kbp_table.bankNum = NLMDEV_BANK_0;
        kbp_table.group_id_start = 0;
        kbp_table.group_id_end = 20;
        kbp_table.min_priority = 32;
        kbp_table.clone_of_tbl_id = ARAD_KBP_FRWRD_IP_NOF_TABLES;
        kbp_table.tbl_size = 0;
        kbp_table.db_type = KBP_DB_LPM;
        res = arad_kbp_db_create(unit,
                                 table_id,
                                 &kbp_table,
                                 0,
                                 &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10000, exit);
    }

    kbp_table_config.tbl_id = 0;
    tbl_id = (table_id != -1) ? table_id : kbp_table_config.tbl_id;
    
    res = KBP_INFO.Arad_kbp_table_config_info.get(unit, tbl_id, &kbp_table_config);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10+db_ndx, exit);
    if (kbp_table_config.valid == FALSE) {
        kbp_table_config.valid = TRUE;
    }

    kbp_table_config.entry_key_parsing.nof_segments = lookup_key.nof_key_segments;
    for (i=0;i<lookup_key.nof_key_segments;i++) {
        kbp_table_config.entry_key_parsing.key_segment[i].nof_bytes = lookup_key.key_segment[i].nof_bytes;
        kbp_table_config.entry_key_parsing.key_segment[i].type = lookup_key.key_segment[i].type;
        sal_strcpy(kbp_table_config.entry_key_parsing.key_segment[i].name,lookup_key.key_segment[i].name);
    }

    kbp_ltr_config.tbl_id[search_id] = (ARAD_KBP_FRWRD_IP_TBL_ID)tbl_id;
    SHR_BITSET(tmp_parallel_srches_bmp, search_id);
    kbp_ltr_config.parallel_srches_bmp |= tmp_parallel_srches_bmp[0];
    if ((kbp_ltr_config.opcode == ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_RPF) ||
        (kbp_ltr_config.opcode == ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF) ||
        (kbp_ltr_config.opcode == ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_DC) ){
        kbp_ltr_config.is_cmp3_search = NlmTRUE;
    }else{
        kbp_ltr_config.is_cmp3_search = NlmFALSE;
    }

    res = KBP_INFO.Arad_kbp_ltr_config.set(unit, db_ndx, &kbp_ltr_config);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20+db_ndx, exit);

    res = KBP_INFO.Arad_kbp_table_config_info.set(unit, table_id, &kbp_table_config);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10+db_ndx, exit);

    
    res = arad_kbp_set_all_db_by_ltr(unit, db_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10+db_ndx, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_update_lookup", 0, 0);
}


STATIC uint32 arad_kbp_get_master_key_info_by_ltr_id(
       SOC_SAND_IN  int                      	unit,
	   SOC_SAND_IN  uint32                   	ltr_id,
	   SOC_SAND_OUT uint32                   	*num_of_frwd_seg,
       SOC_SAND_OUT ARAD_KBP_LTR_SINGLE_SEARCH   *master_key)
{
    int res;
    int i;
    uint32 nof_frwrd_segments = 0;
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};
    uint32 ltr_idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *num_of_frwd_seg = 0;
    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    
    if (ltr_id < ARAD_KBP_ACL_LTR_ID_OFFSET) {
        for(ltr_idx = ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC; ltr_idx < ARAD_KBP_FRWRD_DB_NOF_TYPES; ltr_idx++) {
            if (Arad_kbp_db_type_to_ltr[ltr_idx] == ltr_id) {
                res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_idx, &ltr_config_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
                for (i=0;i<ltr_config_info.master_key_fields.nof_key_segments;i++) {
                    if(sal_strstr(ltr_config_info.master_key_fields.key_segment[i].name,"ACL") == NULL){
                        nof_frwrd_segments++;
                    }
                }
                break;
            }
        }
    }
    else {
        ltr_idx = (ltr_id - ARAD_KBP_ACL_LTR_ID_OFFSET)/2 + ARAD_KBP_FRWRD_DB_NOF_TYPES;
        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_idx, &ltr_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        nof_frwrd_segments = 0;
    }
    *num_of_frwd_seg = nof_frwrd_segments;
    master_key->nof_key_segments = ltr_config_info.master_key_fields.nof_key_segments;
    for (i=0;i<master_key->nof_key_segments;i++) {
        master_key->key_segment[i].nof_bytes = ltr_config_info.master_key_fields.key_segment[i].nof_bytes;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_get_masker_key_by_ltr_id()", 0, ltr_id);

}


STATIC int arad_kbp_acl_get_lsb_boundry_index(
       SOC_SAND_IN  int                      	unit,
       SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH   *master_key)
{
    int i;
    uint32 lsb_size = 0;

    
    if (master_key->nof_key_segments == 0) {
        return 0;
    }

    
    for(i = master_key->nof_key_segments-1; i >= 0; i--) {
        lsb_size += master_key->key_segment[i].nof_bytes;

        
        if (strstr(master_key->key_segment[i].name, "ACL") == NULL ) {
            return (i+1);
        }
        if (lsb_size == 10) {
            return i;
        }
        if (lsb_size > 10) {
            return i+1;
        }
    }
    return 0;
}


STATIC int
    arad_kbp_search_key_segments_remove_from_ltr_master_key(
        int unit,
        uint32 db_type,
        ARAD_KBP_LTR_SINGLE_SEARCH *search_to_remove
    )
{
    uint32
        res;
    int
        master_key_field_index,
        search_key_field_index,
        is_header_present_in_master_key,
        search_size_in_bytes = 0;
    ARAD_KBP_LTR_CONFIG
        ltr_config_info;
    arad_kbp_lut_data_t
        lut_info;
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO
        opcode_config_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = KBP_INFO.Arad_kbp_gtm_lut_info.get(unit, db_type, &lut_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.get(unit, db_type, &opcode_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if(ltr_config_info.master_key_fields.nof_key_segments >= search_to_remove->nof_key_segments) {
        
        master_key_field_index = ltr_config_info.master_key_fields.nof_key_segments - 1;
        search_key_field_index = search_to_remove->nof_key_segments - 1;
        is_header_present_in_master_key = 1;

        while(master_key_field_index >= 0 && search_key_field_index >= 0) {
            
            if(sal_strcmp(ltr_config_info.master_key_fields.key_segment[master_key_field_index].name, search_to_remove->key_segment[search_key_field_index].name) != 0) {
                is_header_present_in_master_key = 0;
                break;
            }

            master_key_field_index--;
            search_key_field_index--;
        }

        if(is_header_present_in_master_key != 0) {
            
            for(search_key_field_index = 0; search_key_field_index < search_to_remove->nof_key_segments; search_key_field_index++) {
                search_size_in_bytes += search_to_remove->key_segment[search_key_field_index].nof_bytes;
            }
            
            ltr_config_info.master_key_fields.nof_key_segments -= search_to_remove->nof_key_segments;
            
            lut_info.rec_size -= search_size_in_bytes;
            
            if(search_size_in_bytes > opcode_config_info.tx_data_size) {
                opcode_config_info.tx_data_size = 0;
            } else {
                opcode_config_info.tx_data_size -= search_size_in_bytes;
            }

            
            res = KBP_INFO.Arad_kbp_ltr_config.set(unit, db_type, &ltr_config_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

            res = KBP_INFO.Arad_kbp_gtm_lut_info.set(unit, db_type, &lut_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.set(unit, db_type, &opcode_config_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_search_key_segments_remove_from_ltr_master_key()", 0, 0);
}


STATIC int
    arad_kbp_search_key_segments_append_to_ltr_master_key(
        int unit,
        uint32 db_type,
        ARAD_KBP_LTR_SINGLE_SEARCH *search_to_append
    )
{
    uint32
        res;
    int
        master_key_field_index,
        search_key_field_index,
        search_size_in_bytes = 0;
    ARAD_KBP_LTR_CONFIG
        ltr_config_info;
    arad_kbp_lut_data_t
        lut_info;
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO
        opcode_config_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(search_to_append == NULL) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Invalid search segments info for LTR %d\n"),
                        FUNCTION_NAME(), db_type));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
    }

    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = KBP_INFO.Arad_kbp_gtm_lut_info.get(unit, db_type, &lut_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.get(unit, db_type, &opcode_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if(ltr_config_info.master_key_fields.nof_key_segments + search_to_append->nof_key_segments > ARAD_KBP_MAX_NOF_KEY_SEGMENTS) {
        
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Maximum allowed number of key segments exceeded (%d)\n"),
                        FUNCTION_NAME(), ARAD_KBP_MAX_NOF_KEY_SEGMENTS));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
    }

    
    for(search_key_field_index = 0; search_key_field_index < search_to_append->nof_key_segments; search_key_field_index++) {
        search_size_in_bytes += search_to_append->key_segment[search_key_field_index].nof_bytes;
    }

    master_key_field_index = ltr_config_info.master_key_fields.nof_key_segments;
    search_key_field_index = 0;

    while(search_key_field_index < search_to_append->nof_key_segments) {
        
        sal_strcpy(ltr_config_info.master_key_fields.key_segment[master_key_field_index].name, search_to_append->key_segment[search_key_field_index].name);
        ltr_config_info.master_key_fields.key_segment[master_key_field_index].nof_bytes = search_to_append->key_segment[search_key_field_index].nof_bytes;
        ltr_config_info.master_key_fields.key_segment[master_key_field_index].type = search_to_append->key_segment[search_key_field_index].type;

        master_key_field_index++;
        search_key_field_index++;
    }

    ltr_config_info.master_key_fields.nof_key_segments += search_to_append->nof_key_segments;
    
    lut_info.rec_size += search_size_in_bytes;
    
    opcode_config_info.tx_data_size += search_size_in_bytes;

    
    res = KBP_INFO.Arad_kbp_ltr_config.set(unit, db_type, &ltr_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = KBP_INFO.Arad_kbp_gtm_lut_info.set(unit, db_type, &lut_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.set(unit, db_type, &opcode_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_search_key_segments_append_to_ltr_master_key()", 0, 0);
}

static
ARAD_KBP_LTR_SINGLE_SEARCH *
    arad_kbp_ipv6_kaps_keys_for_sharing(int opcode)
{
    switch(opcode) {
        case PROG_FLP_IPV6UC:
            return &ipv6_dip_sip_sharing_dynamic_segments_ipv6_uc;
            break;
        case PROG_FLP_IPV6UC_PUBLIC:
            return &ipv6_dip_sip_sharing_dynamic_segments_ipv6_uc_public;
            break;
        case PROG_FLP_IPV6UC_RPF:
        case PROG_FLP_IPV6UC_PUBLIC_RPF:
        case PROG_FLP_IPV6MC:
            return &ipv6_dip_sip_sharing_dynamic_segments_ipv6_uc_rpf;
            break;
        default:
            return NULL;
    }
}

STATIC uint32 arad_kbp_ltr_lookup_add_verify(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      ltr_idx,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH  *search_format
   )
{
    uint32
        field_idx, res;

    uint8 is_valid = TRUE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_ABOVE_NOF(ltr_id, ARAD_KBP_MAX_ACL_LTR_ID, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(ltr_idx, ARAD_KBP_MAX_NUM_OF_FRWRD_DBS, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(table_id, ARAD_KBP_ACL_TABLE_ID_OFFSET, ARAD_KBP_MAX_NUM_OF_TABLES - 1, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);

    res = KBP_INFO.Arad_kbp_table_config_info.valid.get(unit, table_id, &is_valid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
    if((flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY) && (is_valid)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table %d is already configured\n"),
                        FUNCTION_NAME(), table_id));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }
    if(!(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY) && (!is_valid)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table %d is not yet configured\n"),
                        FUNCTION_NAME(), table_id));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }

    
    {
        uint32 nof_key_segments = 0;
        res = KBP_INFO.Arad_kbp_ltr_config.master_key_fields.nof_key_segments.get(unit, ltr_idx, &nof_key_segments);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if((search_format->nof_key_segments + nof_key_segments) > ARAD_KBP_MAX_NOF_KEY_SEGMENTS) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Maximum allowed number of key segments exceeded (%d)\n"),
                            FUNCTION_NAME(), ARAD_KBP_MAX_NOF_KEY_SEGMENTS));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }
    }

    
    for (field_idx = 0; field_idx < search_format->nof_key_segments; field_idx++)
    {
        if(search_format->key_segment[field_idx].nof_bytes > ARAD_KBP_MAX_SEGMENT_LENGTH_BYTES) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Maximum allowed search key segment size exceeded (%d)\n"),
                            FUNCTION_NAME(), ARAD_KBP_MAX_SEGMENT_LENGTH_BYTES));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_ltr_lookup_add_verify()", 0, 0);
}

STATIC uint32 arad_kbp_ltr_lookup_add(
       SOC_SAND_IN  int                             unit,
       SOC_SAND_IN  uint32                          search_id,
       SOC_SAND_IN  uint8                           opcode,
       SOC_SAND_IN  uint32                          ltr_id,
       SOC_SAND_IN  uint32                          flags,
       SOC_SAND_IN  uint32                          table_id,
       SOC_SAND_IN  ARAD_KBP_ACL_IN_MASTER_KEY_TYPE acl_type,
       SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH      *search_format,
       uint32                                       nof_shared_quals,
       uint32                                       nof_range_quals,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success)
{
    uint32
        ltr_idx,
        nof_segments,
        current_search_segment,
        res = SOC_SAND_OK;
    uint8
        check_only;

    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(success);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    check_only = (flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY) ? TRUE : FALSE;
    *success = SOC_SAND_FAILURE_INTERNAL_ERR;

    
    if (ltr_id < ARAD_KBP_ACL_LTR_ID_OFFSET) {
        for(ltr_idx = ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC; ltr_idx < ARAD_KBP_FRWRD_DB_NOF_TYPES; ltr_idx++) {
            if (Arad_kbp_db_type_to_ltr[ltr_idx] == ltr_id) {
                break;
            }
        }
    }
    else {
        ltr_idx = (ltr_id - ARAD_KBP_ACL_LTR_ID_OFFSET)/2 + ARAD_KBP_FRWRD_DB_NOF_TYPES;
    }

    res = arad_kbp_ltr_lookup_add_verify(
            unit,
            ltr_id,
            ltr_idx,
            flags,
            table_id,
            search_format
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_idx, &ltr_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    if (ltr_config_info.tbl_id[search_id] == ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC ||
        ltr_config_info.tbl_id[search_id] == ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC ||
        ltr_config_info.tbl_id[search_id] == ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC ||
        ltr_config_info.tbl_id[search_id] == ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC ||
        ltr_config_info.tbl_id[search_id] == ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC ||
        ltr_config_info.tbl_id[search_id] == ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC_PUBLIC ||
        ltr_config_info.tbl_id[search_id] == ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC_PUBLIC) {
        if (search_id != 1) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Trying to set a dummy table in LTR ID %d not on search 1 (%d)\n"),
                            FUNCTION_NAME(), ltr_idx, search_id));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }
    }
    else {
        if (SHR_BITGET(&ltr_config_info.parallel_srches_bmp, search_id)) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): LTR ID %d has already search %d configured\n"),
                            FUNCTION_NAME(), ltr_idx, search_id));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }
    }

    
    if ((search_id >= ARAD_KBP_CMPR3_FIRST_ACL) && !(SOC_IS_JERICHO(unit))) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Trying to configure unsupported search %d in LTR ID %d\n"),
                        FUNCTION_NAME(), search_id, ltr_idx));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

    if (!check_only)
    {
        uint32 index_of_first_lsb_segment;
        ARAD_KBP_LTR_SINGLE_SEARCH copy_of_lsb_master_key;
        int rv;
        

        SHR_BITSET(&ltr_config_info.parallel_srches_bmp, search_id);
        ltr_config_info.tbl_id[search_id] = table_id;

        
        sal_memcpy(&ltr_config_info.ltr_search[search_id],
                   search_format,
                   sizeof(ARAD_KBP_LTR_SINGLE_SEARCH));

        
        nof_segments = ltr_config_info.master_key_fields.nof_key_segments;
        
        current_search_segment = nof_shared_quals;

        

        if (SOC_IS_JERICHO(unit)) {
            int i;
            if(acl_type == ARAD_KBP_ACL_IN_MASTER_KEY_LSB_ONLY) {
                
                sal_memcpy(&ltr_config_info.master_key_fields.key_segment[nof_segments],
                           &search_format->key_segment[current_search_segment],
                           sizeof(ARAD_KBP_KEY_SEGMENT));
                nof_segments++;
                current_search_segment++;

                
                for(i = 0; i < nof_range_quals; i++) {
                    sal_memcpy(&ltr_config_info.master_key_fields.key_segment[nof_segments],
                               &search_format->key_segment[current_search_segment],
                               sizeof(ARAD_KBP_KEY_SEGMENT));
                    nof_segments++;
                    current_search_segment++;
                }
            } else if(acl_type == ARAD_KBP_ACL_IN_MASTER_KEY_MSB_ONLY) {
                rv = arad_kbp_acl_get_lsb_boundry_index(unit, &ltr_config_info.master_key_fields);
                index_of_first_lsb_segment = rv;

                
                
                for(i = 0; i < nof_range_quals; i++) {
                    sal_memcpy(&ltr_config_info.master_key_fields.key_segment[nof_segments],
                               &search_format->key_segment[current_search_segment],
                               sizeof(ARAD_KBP_KEY_SEGMENT));
                    nof_segments++;
                    current_search_segment++;
                }

                sal_memcpy(&copy_of_lsb_master_key.key_segment[0],
                           &search_format->key_segment[0 + nof_shared_quals],
                           sizeof(ARAD_KBP_KEY_SEGMENT));

                sal_memcpy(&copy_of_lsb_master_key.key_segment[1],
                           &ltr_config_info.master_key_fields.key_segment[index_of_first_lsb_segment],
                           sizeof(ARAD_KBP_KEY_SEGMENT) * (nof_segments - index_of_first_lsb_segment));

                sal_memcpy(&ltr_config_info.master_key_fields.key_segment[index_of_first_lsb_segment],
                           &copy_of_lsb_master_key.key_segment[0],
                           sizeof(ARAD_KBP_KEY_SEGMENT) * (nof_segments - index_of_first_lsb_segment + 1));

            } else if(acl_type == ARAD_KBP_ACL_IN_MASTER_KEY_LSB_MSB) {
                rv = arad_kbp_acl_get_lsb_boundry_index(unit, &ltr_config_info.master_key_fields);
                index_of_first_lsb_segment = rv;

                sal_memcpy(&ltr_config_info.master_key_fields.key_segment[nof_segments],
                           &search_format->key_segment[current_search_segment],
                           sizeof(ARAD_KBP_KEY_SEGMENT));
                nof_segments++;
                current_search_segment++;

                
                for(i = 0; i < nof_range_quals; i++) {
                    sal_memcpy(&ltr_config_info.master_key_fields.key_segment[nof_segments],
                               &search_format->key_segment[current_search_segment],
                               sizeof(ARAD_KBP_KEY_SEGMENT));
                    nof_segments++;
                    current_search_segment++;
                }

                sal_memcpy(&copy_of_lsb_master_key.key_segment[0],
                           &search_format->key_segment[current_search_segment],
                           sizeof(ARAD_KBP_KEY_SEGMENT));

                sal_memcpy(&copy_of_lsb_master_key.key_segment[1],
                           &ltr_config_info.master_key_fields.key_segment[index_of_first_lsb_segment],
                           sizeof(ARAD_KBP_KEY_SEGMENT) * (nof_segments - index_of_first_lsb_segment));

                sal_memcpy(&ltr_config_info.master_key_fields.key_segment[index_of_first_lsb_segment],
                           &copy_of_lsb_master_key.key_segment[0],
                           sizeof(ARAD_KBP_KEY_SEGMENT) * (nof_segments - index_of_first_lsb_segment + 1));
            } else { 
                
                for(i = 0; i < nof_range_quals; i++) {
                    sal_memcpy(&ltr_config_info.master_key_fields.key_segment[nof_segments],
                               &search_format->key_segment[current_search_segment],
                               sizeof(ARAD_KBP_KEY_SEGMENT));
                    nof_segments++;
                    current_search_segment++;
                }
            }
        }else{
            sal_memcpy(&ltr_config_info.master_key_fields.key_segment[nof_segments],
                       &search_format->key_segment[current_search_segment],
                       sizeof(ARAD_KBP_KEY_SEGMENT) * (search_format->nof_key_segments - nof_shared_quals));
        }

        ltr_config_info.master_key_fields.nof_key_segments +=
            (search_format->nof_key_segments - nof_shared_quals);

        
        ltr_config_info.valid = TRUE;
        ltr_config_info.opcode = opcode;
        ltr_config_info.ltr_id = ltr_id;

        if (search_id >= ARAD_KBP_CMPR3_FIRST_ACL) {
            ltr_config_info.is_cmp3_search = NlmTRUE;
        }

        res = KBP_INFO.Arad_kbp_ltr_config.set(unit, ltr_idx, &ltr_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }

    *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_ltr_lookup_add()", 0, 0);
}

uint32 arad_kbp_opcode_to_db_type(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_OUT uint32                      *db_type,
       SOC_SAND_OUT uint32                      *ltr_id,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success)
{
    uint32
        db_type_idx, res,
        found_idx = 0;

    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(ltr_id);
    SOC_SAND_CHECK_NULL_INPUT(success);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;

    
    for (db_type_idx = 0; db_type_idx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type_idx++)
    {
        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type_idx, &ltr_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10+ db_type_idx, exit);

        if ((ltr_config_info.valid) && (ltr_config_info.opcode == opcode))
        {
            found_idx = db_type_idx;
            *success = SOC_SAND_SUCCESS;
            break;
        }
    }

    
    if (*success != SOC_SAND_SUCCESS) {
        for (db_type_idx = ARAD_KBP_FRWRD_DB_NOF_TYPES; db_type_idx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type_idx++) {
            res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type_idx, &ltr_config_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 100+ db_type_idx, exit);
            if(!ltr_config_info.valid) {
                found_idx = db_type_idx;
                *success = SOC_SAND_SUCCESS;
                break;
            }
        }
    }

    
    if (*success == SOC_SAND_SUCCESS)
    {
        if (found_idx < ARAD_KBP_FRWRD_DB_NOF_TYPES) {
            *ltr_id = Arad_kbp_db_type_to_ltr[found_idx];
        } else {
           *ltr_id = ARAD_KBP_ACL_LTR_ID_OFFSET + 2*(found_idx - ARAD_KBP_FRWRD_DB_NOF_TYPES);
            if (*ltr_id >= ARAD_KBP_MAX_ACL_LTR_ID) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): LTR IDs exhausted\n"),
                                FUNCTION_NAME()));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
            }
        }
        *db_type = found_idx;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_opcode_to_db_type()", 0, 0);
}

STATIC uint32 arad_kbp_add_ltr_to_opcode_verify(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE   db_type,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      key_size_in_bits,
       SOC_SAND_IN  uint32                      search_id)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_ABOVE_NOF(db_type, ARAD_KBP_MAX_NUM_OF_FRWRD_DBS, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(ltr_id, ARAD_KBP_MAX_ACL_LTR_ID, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 30, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(key_size_in_bits, ARAD_KBP_MASTER_KEY_MAX_LENGTH, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 40, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(search_id, ARAD_KBP_NUM_OF_KBP_SEARCHES(unit), ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_ltr_to_opcode_verify()", 0, 0);
}


STATIC uint32 arad_kbp_add_ltr_search_to_opcode(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE   db_type,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      key_size_in_bytes,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success)
{
    uint32
        result_idx, res;

    arad_kbp_lut_data_t lut_info = {0};
    arad_kbp_frwd_ltr_db_t ltr_db_info;
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO opcode_info = {0};

    ARAD_PMF_CE_IRPP_QUALIFIER_INFO irpp_qual_info;
    uint8 found;
    NlmBool is_cmp3_search = NlmTRUE;
    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(success);

    arad_kbp_frwd_ltr_db_clear(&ltr_db_info);

    
    res = KBP_INFO.Arad_kbp_gtm_lut_info.get(unit, db_type, &lut_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    lut_info.rec_size += key_size_in_bytes;
    lut_info.rec_type = ARAD_KBP_LUT_REC_TYPE_REQUEST;
    lut_info.rec_is_valid = 1;
    lut_info.mode = ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE;
    lut_info.key_config = ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA;
    res = KBP_INFO.Arad_kbp_ltr_config.is_cmp3_search.get(unit, db_type, &is_cmp3_search);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10 + db_type, exit);
    if (is_cmp3_search) {
        lut_info.instr = ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(ltr_id);
    }
    else {
        lut_info.instr = ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ltr_id);
    }

    res = KBP_INFO.Arad_kbp_gtm_ltr_info.get(unit, db_type, &ltr_db_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    ltr_db_info.opcode = opcode;

    arad_pmf_ce_internal_field_info_find(unit, SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0 + search_id,
                                                                     SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, 0, &found, &irpp_qual_info);

    if (!found) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Qualifier info not found; LTR ID %d; search %d\n"),
                        FUNCTION_NAME(), db_type, search_id));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

    lut_info.result_idx_ad_cfg[search_id] = irpp_qual_info.info.qual_nof_bits/8;
    lut_info.result_idx_or_ad[search_id] = ARAD_KBP_LUT_TRANSFER_AD_ONLY;

    if ((search_id == 1) && (db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC) && (ARAD_KBP_ENABLE_IPV4_DC)) {
         lut_info.result_idx_ad_cfg[search_id] += ARAD_KBP_IPV4DC_RES1_PAD_BYTES;
    }

    if ((search_id == 3) && (db_type != ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC) && (ARAD_KBP_ENABLE_IPV4_DC)) {
         lut_info.result_idx_ad_cfg[search_id] += ARAD_KBP_IPV4DC_RES3_PAD_BYTES;
    }

    res = KBP_INFO.Arad_kbp_gtm_lut_info.set(unit, db_type, &lut_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ltr_db_info.res_data_len[search_id] = irpp_qual_info.info.qual_nof_bits/8;

    if ((db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC) && (search_id == 1) && (ARAD_KBP_ENABLE_IPV4_DC)) {
        ltr_db_info.res_data_len[search_id] += ARAD_KBP_IPV4DC_RES1_PAD_BYTES;
    }

    if ((db_type != ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC) && (search_id == 3) && (ARAD_KBP_ENABLE_IPV4_DC)) {
        ltr_db_info.res_data_len[search_id] += ARAD_KBP_IPV4DC_RES3_PAD_BYTES;
    }

    if (ltr_db_info.res_data_len[search_id] <= 4) {
        ltr_db_info.res_format[search_id] = NLM_ARAD_INDEX_AND_32B_AD;
    }else if (ltr_db_info.res_data_len[search_id] <= 8) {
        ltr_db_info.res_format[search_id] = NLM_ARAD_INDEX_AND_64B_AD;
    }else{
        ltr_db_info.res_format[search_id] = NLM_ARAD_INDEX_AND_128B_AD;
    }

    
    ltr_db_info.res_total_data_len = 1;
    for (result_idx = 0; result_idx < num_of_kbp_searches; result_idx++ ){
        if ((ltr_db_info.res_format[result_idx] != NLM_ARAD_NO_INDEX_NO_AD) &&
            (ltr_db_info.res_format[result_idx] != NLM_ARAD_ONLY_INDEX_NO_AD)){
            ltr_db_info.res_total_data_len += ltr_db_info.res_data_len[result_idx];
        }
    }

    res = KBP_INFO.Arad_kbp_gtm_ltr_info.set(unit, db_type, &ltr_db_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


    

    res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.get(unit, db_type, &opcode_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    opcode_info.tx_data_size = lut_info.rec_size - 1;
    opcode_info.tx_data_type = 1; 
    opcode_info.rx_data_size = ltr_db_info.res_total_data_len - 1;
    opcode_info.rx_data_type = 3; 

    res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.set(unit, db_type, &opcode_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_ltr_search_to_opcode()", 0, 0);
}

STATIC uint32 arad_kbp_add_ltr_to_opcode(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE   db_type,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      key_size_in_bytes,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH  *search_format,
       uint32                                   nof_shared_segments,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success)
{
    uint32
        srch_idx,
        ltr_key_size_in_bytes,
        res;

    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    
    res = arad_kbp_add_ltr_to_opcode_verify(
            unit,
            db_type,
            opcode,
            ltr_id,
            (key_size_in_bytes * 8),
            search_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(!(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY))
    {
        for(srch_idx = 0; srch_idx <= search_id; srch_idx++)
        {
            if ((srch_idx != search_id) &&
                SHR_BITGET(&(ltr_config_info.parallel_srches_bmp), srch_idx)) {
                continue;
            }

            ltr_key_size_in_bytes = (srch_idx == search_id) ? key_size_in_bytes : 0;

            
            res = arad_kbp_add_ltr_search_to_opcode(
                    unit,
                    db_type,
                    opcode,
                    ltr_id,
                    ltr_key_size_in_bytes,
                    srch_idx,
                    success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            if (srch_idx != search_id)
            {
                
                
                res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit, ARAD_KBP_TABLE_INDX_TO_DUMMY_TABLE_ID(srch_idx), TRUE);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                SHR_BITSET(&ltr_config_info.parallel_srches_bmp, srch_idx);

                if(SOC_IS_JERICHO(unit)) {
                    
                    ltr_config_info.ltr_search[srch_idx] = dummy_key_search;
                } else {
                    
                    int i;
                    ltr_config_info.ltr_search[srch_idx].nof_key_segments = 1 + nof_shared_segments;
                    for (i = 0; i < 1 + nof_shared_segments; i++) {
                        ltr_config_info.ltr_search[srch_idx].key_segment[i] = search_format->key_segment[i];
                    }
                }

                res = KBP_INFO.Arad_kbp_ltr_config.set(unit, db_type, &ltr_config_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100+srch_idx, exit);
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_ltr_to_opcode()", 0, 0);
}


uint32 arad_kbp_shrink_shared_qualifiers(SOC_SAND_IN  int                      unit,
                                      ARAD_PP_FP_SHARED_QUAL_INFO*          shared_quals_info,
                                      uint32                                nof_shared_quals,
                                      int*                                  nof_shared_segments,
                                      int*                                  total_shared_size)
{
    int i;
    int ip6_sip = 0, ip6_dip = 0;
    ARAD_PP_FP_SHARED_QUAL_INFO          shared_quals_info_lcl[MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM];

    (*total_shared_size) = 0;
    (*nof_shared_segments) = 0;

    for(i = 0; i < nof_shared_quals; i++){
        (*total_shared_size) += shared_quals_info[i].size;
        if((shared_quals_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW) || (shared_quals_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH)){
            ip6_sip++;
            if (ip6_sip == 2) {
                memcpy(&shared_quals_info_lcl[(*nof_shared_segments)], &shared_quals_info[i], sizeof(ARAD_PP_FP_SHARED_QUAL_INFO));
                shared_quals_info_lcl[(*nof_shared_segments)].size = shared_quals_info[i].size*2;
                (*nof_shared_segments)++;
            }
        }

        if((shared_quals_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH) || (shared_quals_info[i].qual_type == SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW)){
            ip6_dip++;
            if (ip6_dip == 2) {
                memcpy(&shared_quals_info_lcl[(*nof_shared_segments)], &shared_quals_info[i], sizeof(ARAD_PP_FP_SHARED_QUAL_INFO));
                shared_quals_info_lcl[(*nof_shared_segments)].size = shared_quals_info[i].size*2;
                (*nof_shared_segments)++;
            }
        }
    }

    if (ip6_dip == 2 || ip6_sip == 2) {
        memcpy(shared_quals_info, &shared_quals_info_lcl, sizeof(ARAD_PP_FP_SHARED_QUAL_INFO)* MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM);
    }

    (*nof_shared_segments) = nof_shared_quals - (*nof_shared_segments);


return 0;
}


int update_kbp_tx_data_size(int unit, bcm_field_AppType_t apptype, uint32 key_size_in_bytes)
{
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO kbp_gtm_opcode_config_info = {0};
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};
    int ltr_index = 0;
    int res = 0;
    bcm_field_AppType_t apptype_check;

    
    for(ltr_index = 0; ltr_index < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; ltr_index++)
    {
        
        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_index, &ltr_config_info);
        if(res)
        {
            printf("Error, could not get device ltr info\n");
            return  res;
        }

        
        if (ltr_config_info.valid)
        {
            
            res = _bcm_dpp_field_app_type_ppd_to_bcm(unit, ltr_config_info.opcode, &apptype_check);
            if(res)
            {
                printf("Error, _bcm_dpp_field_app_type_ppd_to_bcm failed\n");
                return  res;
            }

            
            if (apptype_check == apptype) 
            {
                
                res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.get(unit, ltr_index, &kbp_gtm_opcode_config_info);
                if(res)
                {
                    printf("Error, fail to get kbp rx/tx data\n");
                    return  res;
                }

                
                kbp_gtm_opcode_config_info.tx_data_size = kbp_gtm_opcode_config_info.tx_data_size + key_size_in_bytes;

                
                res = arad_kbp_frwrd_opcode_set(unit, ltr_config_info.opcode,
                                                kbp_gtm_opcode_config_info.tx_data_size,
                                                kbp_gtm_opcode_config_info.tx_data_type,
                                                kbp_gtm_opcode_config_info.rx_data_size,
                                                kbp_gtm_opcode_config_info.rx_data_type);
                if(res)
                {
                    printf("Error, arad_kbp_frwrd_opcode_set() failed\n");
                    return  res;
                }

                
                KBP_INFO.Arad_kbp_gtm_opcode_config_info.set(unit, ltr_index, &kbp_gtm_opcode_config_info);
            }
        }
    }

    return res;
}


int check_new_ltr_required(int unit, uint32 pgm_ndx, uint32 pgm_bmp_used, SOC_SAND_SUCCESS_FAILURE *success, uint32 *end_func)
{
    uint8 prog_id;
    uint32 ltr_id;
    uint32 db_type;
    ARAD_KBP_FRWRD_IP_OPCODE opcode;
    int res = 0;

    if (SOC_SAND_GET_BIT(pgm_bmp_used, pgm_ndx) != 0x1)
    {
        
        return res;
    }

    
    res = arad_pp_prog_index_to_flp_app_get(unit,pgm_ndx,&prog_id);
    if(res)
    {
        printf("Error, arad_pp_prog_index_to_flp_app_get() failed\n");
        return  res;
    }

    opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(prog_id);

    res = arad_kbp_opcode_to_db_type(unit, opcode, &db_type, &ltr_id, success);
    if(res)
    {
        printf("Error, arad_kbp_opcode_to_db_type() failed\n");
        return  res;
    }

    
    if (db_type < ARAD_KBP_FRWRD_DB_NOF_TYPES) 
    {
        *end_func = 1;
    }

    return res;
}


uint32 arad_kbp_add_acl(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_IN  uint32                      pgm_bmp_used,
       SOC_SAND_IN  uint32                      pgm_ndx_min,
       SOC_SAND_IN  uint32                      pgm_ndx_max,
       SOC_SAND_IN  uint32                      key_size_in_bytes_lsb,
       SOC_SAND_IN  uint32                      key_size_in_bytes_msb,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_IN  uint32                      min_priority,
       uint32                                   nof_shared_quals,
       ARAD_PP_FP_SHARED_QUAL_INFO              shared_quals_info[MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM],
       uint32                                   nof_range_quals,
       ARAD_PP_FP_RANGE_QUAL_INFO               range_quals_info[ARAD_KBP_MAX_NOF_RANGES_IN_TABLE],
       SOC_SAND_OUT uint32                      *pgm_bmp_new,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success)
{
    uint32
        res,
        db_type,
        pgm_ndx,
        ltr_id,
        segment_idx,
        key_size_in_bytes = key_size_in_bytes_lsb + key_size_in_bytes_msb,
        end_func = 0;
    uint8 prog_id;
    ARAD_KBP_TABLE_CONFIG
        kbp_table_config;
    ARAD_KBP_LTR_SINGLE_SEARCH
        search_format,
        *ipv6_dip_sip_sharing_header;
    ARAD_KBP_FRWRD_IP_OPCODE
        opcode;

    int i, total_shared_size = 0, nof_shared_segments = nof_shared_quals;

    ARAD_PMF_CE_IRPP_QUALIFIER_INFO irpp_qual_info;
    uint8 found;

#if defined(INCLUDE_KBP)
    ARAD_INIT_ELK* elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
#endif

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0))
    {
        
        for (pgm_ndx = 0; pgm_ndx < pgm_ndx_max; ++pgm_ndx)
        {
            
            if (SHR_BITGET(&pgm_bmp_used, pgm_ndx) > 0)
            {
                
                bcm_field_AppType_t apptype;
                res = _bcm_dpp_field_app_type_ppd_to_bcm(unit, pgm_ndx, &apptype);

                
                res = update_kbp_tx_data_size(unit, apptype, key_size_in_bytes);
                if(res)
                {
                    printf("Error, update_kbp_tx_data_size() failed\n");
                    return  res;
                }
            }
        }

        for (pgm_ndx = pgm_ndx_min; pgm_ndx < pgm_ndx_max; ++pgm_ndx)
        {
            
            res = check_new_ltr_required(unit, pgm_ndx, pgm_bmp_used, success, &end_func);
            if(res)
            {
                printf("Error, check_new_ltr_required() failed\n");
                return  res;
            }

            
            if (end_func) 
            {
                return 0;
            }
        }
    }

     ARAD_KBP_TABLE_CONFIG_clear(&kbp_table_config);
     ARAD_KBP_LTR_SINGLE_SEARCH_clear(&search_format);

    if(ARAD_KBP_IS_OP_OR_OP2) {
        ipv6_dip_sip_sharing_header = &ipv6_dip_sip_sharing_forwarding_header_search_op;
    } else {
        ipv6_dip_sip_sharing_header = &ipv6_dip_sip_sharing_forwarding_header_search;
    }

     if(nof_shared_quals > 0){
         arad_kbp_shrink_shared_qualifiers(unit, shared_quals_info, nof_shared_quals, &nof_shared_segments, &total_shared_size);
     }
    
    kbp_table_config.tbl_id = ARAD_KBP_MAX_NUM_OF_TABLES;

    
    if (key_size_in_bytes + total_shared_size <= 10) {
        kbp_table_config.tbl_width = NLM_TBL_WIDTH_80;
    }
    else if (key_size_in_bytes + total_shared_size <= 20) {
        kbp_table_config.tbl_width = NLM_TBL_WIDTH_160;
    }
    else if (key_size_in_bytes + total_shared_size <= 40) {
        kbp_table_config.tbl_width = NLM_TBL_WIDTH_320;
    }
    else if (key_size_in_bytes + total_shared_size <= 60) {
        kbp_table_config.tbl_width = NLM_TBL_WIDTH_480;
    }
    else {
        
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Key length (%d bits) exceeds the maximum table width (480 bits)\n"),
                        FUNCTION_NAME(), ((key_size_in_bytes + total_shared_size) * 8)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
    }


    arad_pmf_ce_internal_field_info_find(unit, SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0 + search_id,
                                                                     SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, 0, &found, &irpp_qual_info);

    if (!found) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

    if (irpp_qual_info.info.qual_nof_bits <= 24) {
        kbp_table_config.tbl_asso_width = NLM_TBL_ADLEN_24B;
    }else if (irpp_qual_info.info.qual_nof_bits <= 32) {
        kbp_table_config.tbl_asso_width = NLM_TBL_ADLEN_32B;
    }else if (irpp_qual_info.info.qual_nof_bits <= 48) {
        kbp_table_config.tbl_asso_width = NLM_TBL_ADLEN_48B;
    }else if (irpp_qual_info.info.qual_nof_bits <= 64) {
        kbp_table_config.tbl_asso_width = NLM_TBL_ADLEN_64B;
    }else{
        kbp_table_config.tbl_asso_width = NLM_TBL_ADLEN_128B;
    }

    kbp_table_config.bankNum = NLMDEV_BANK_0;
    kbp_table_config.group_id_start = 0;
    kbp_table_config.group_id_end = 0;
    kbp_table_config.min_priority = min_priority;
    kbp_table_config.clone_of_tbl_id = ARAD_KBP_FRWRD_IP_NOF_TABLES;
    kbp_table_config.db_type = KBP_DB_ACL;

    
    switch (kbp_table_config.tbl_width) {
    case NLM_TBL_WIDTH_80:
        kbp_table_config.tbl_size = soc_property_get(unit, spn_EXT_ACL80_TABLE_SIZE, (64*1024));
        break;
    case NLM_TBL_WIDTH_160:
        kbp_table_config.tbl_size = soc_property_get(unit, spn_EXT_ACL160_TABLE_SIZE, (64*1024));
        break;
    case NLM_TBL_WIDTH_320:
        kbp_table_config.tbl_size = soc_property_get(unit, spn_EXT_ACL320_TABLE_SIZE, (64*1024));
        break;
    case NLM_TBL_WIDTH_480:
        kbp_table_config.tbl_size = soc_property_get(unit, spn_EXT_ACL480_TABLE_SIZE, (64*1024));
        break;
    default:
        
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table width %d is not defined\n"),
                        FUNCTION_NAME(), kbp_table_config.tbl_width));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 2, exit);
    }

    if(!SOC_IS_JERICHO(unit)){
        

        

        memset(&search_format, 0, sizeof(search_format));
        search_format.nof_key_segments = (key_size_in_bytes + 9) / 10;
        search_format.nof_key_segments = search_format.nof_key_segments+nof_shared_segments;

        for (i = 0; i < nof_shared_segments; i++) {
            search_format.key_segment[i].nof_bytes = shared_quals_info[i].size;
            sal_memcpy(search_format.key_segment[i].name, shared_quals_info[i].name, 20);
            search_format.key_segment[i].type = KBP_KEY_FIELD_TERNARY;
        }

        for (segment_idx = 0; segment_idx < search_format.nof_key_segments - nof_shared_segments; segment_idx++)
        {
          
          search_format.key_segment[segment_idx+nof_shared_segments].nof_bytes =
            (segment_idx+nof_shared_segments == (search_format.nof_key_segments-1)) ? (((key_size_in_bytes - 1) % 10) + 1) : 10;

          sal_sprintf(search_format.key_segment[segment_idx+nof_shared_segments].name, "ACL%d-s%d", table_id, segment_idx);
          search_format.key_segment[segment_idx+nof_shared_segments].type = KBP_KEY_FIELD_TERNARY;
        }
    }

    

    res = arad_kbp_db_create(
              unit,
              table_id,
              &kbp_table_config,
              flags,
              success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

    if(SOC_SAND_SUCCESS != *success) {
        
        ARAD_DO_NOTHING_AND_EXIT;
    }

    for (pgm_ndx = pgm_ndx_min; pgm_ndx < pgm_ndx_max; ++pgm_ndx)
    {
        ARAD_KBP_LTR_SINGLE_SEARCH master_key_fields;
        ARAD_KBP_ACL_IN_MASTER_KEY_TYPE acl_type = ARAD_KBP_ACL_IN_MASTER_KEY_LSB_ONLY;
        uint32 master_key_len = 0;
        uint32 num_of_frwd_segments = 0;
        uint8 dummy_search_added = 0;

        if (SOC_SAND_GET_BIT(pgm_bmp_used, pgm_ndx) != 0x1){
            
            continue;
        }

        
        res = arad_pp_prog_index_to_flp_app_get(unit,pgm_ndx,&prog_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED)) {
            
            *success = SOC_SAND_SUCCESS;
            
            SHR_BITSET(pgm_bmp_new, pgm_ndx);
            continue;
        }

        opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(prog_id);

        if (!ARAD_KBP_IS_OP_OR_OP2 && ((search_id == 6) || (search_id == 7))) {
            
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Trying to configure unsupported search %d\n"),
                            FUNCTION_NAME(), search_id));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }

        if (ARAD_KBP_ENABLE_IPV4_DC && ((search_id == 5) || (search_id == 6) || (search_id == 7)) && !ARAD_KBP_IPV4DC_24BIT_FWD) {
            
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Trying to configure search %d which is not supported with Double Capacity\n"),
                            FUNCTION_NAME(), search_id));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }

        res = arad_kbp_opcode_to_db_type(
                  unit,
                  opcode,
                  &db_type,
                  &ltr_id,
                  success
              );

        SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

        if(SOC_SAND_SUCCESS != *success) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

        sal_memset(&master_key_fields, 0x0, sizeof(master_key_fields));
        if(SOC_IS_JERICHO(unit)){
            
            ARAD_KBP_LTR_SINGLE_SEARCH * ipv6_dip_sip_segments = arad_kbp_ipv6_kaps_keys_for_sharing(opcode);
            uint8 is_sip_dip_sharing_acl_via_fwd = 0;
            int range_quals_size_in_bytes = 0;
            int shared_quals_size_in_bytes = 0;
            int acl_s0_size = 0;
            int acl_s1_size = 0;
            res = arad_kbp_search_key_segments_remove_from_ltr_master_key(unit, db_type, &dummy_key_search);
            SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

            
            if(elk->kbp_no_fwd_ipv6_dip_sip_sharing_disable == 0)
            {
                if((elk->kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header != 0) &&
                   (ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_UC_VALID(opcode)))
                {
                    res = arad_kbp_search_key_segments_remove_from_ltr_master_key(unit, db_type, ipv6_dip_sip_sharing_header);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);
                }
                else if(ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_VALID(opcode))
                {
                    is_sip_dip_sharing_acl_via_fwd = TRUE;
                    dummy_search_added = TRUE;
                    res = arad_kbp_get_master_key_info_by_ltr_id(unit, ltr_id, &num_of_frwd_segments, &master_key_fields);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
                    if(master_key_fields.nof_key_segments == 0)
                    {
                        res = arad_kbp_search_key_segments_append_to_ltr_master_key(unit, db_type, ipv6_dip_sip_segments);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
                    }
                }
            }

            
            if(ARAD_KBP_PROG_IS_L2_SHARING(opcode) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_da_sa_sharing_enable", 0))
            {
                res = arad_kbp_search_key_segments_remove_from_ltr_master_key(unit, db_type, &l2_da_sa_sharing_forwarding_header_search);
                SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
            }

            
            memset(&search_format, 0, sizeof(search_format));

            
            for(i = 0; i < nof_shared_segments; i++) {
                search_format.key_segment[i].nof_bytes = shared_quals_info[i].size;
                sal_memcpy(search_format.key_segment[i].name, shared_quals_info[i].name, 20);
                search_format.key_segment[i].type = ARAD_KBP_IS_OP_OR_OP2 ? KBP_KEY_FIELD_EM : KBP_KEY_FIELD_TERNARY;
                shared_quals_size_in_bytes += shared_quals_info[i].size;
            }
            search_format.nof_key_segments += nof_shared_segments;

            
            for(i = 0; i < nof_range_quals; i++) {
                range_quals_size_in_bytes += range_quals_info[i].size_in_bytes;
            }
            search_format.nof_key_segments += nof_range_quals;

            
            res = arad_kbp_get_master_key_info_by_ltr_id(unit, ltr_id, &num_of_frwd_segments, &master_key_fields);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            if(is_sip_dip_sharing_acl_via_fwd) {
                num_of_frwd_segments = ipv6_dip_sip_segments->nof_key_segments;
            }

            
            master_key_len = 0;
            for(i = num_of_frwd_segments; i < master_key_fields.nof_key_segments; i++) {
                master_key_len += master_key_fields.key_segment[i].nof_bytes;
            }

            
            acl_s0_size = key_size_in_bytes_lsb - range_quals_size_in_bytes;
            acl_s1_size = key_size_in_bytes_msb;

            

            if(acl_s0_size != 0 && acl_s1_size != 0) {
                acl_type = ARAD_KBP_ACL_IN_MASTER_KEY_LSB_MSB;
            }
            else if(acl_s0_size != 0 && acl_s1_size == 0) {
                acl_type = ARAD_KBP_ACL_IN_MASTER_KEY_LSB_ONLY;
            }
            else if(acl_s0_size == 0 && acl_s1_size != 0) {
                acl_type = ARAD_KBP_ACL_IN_MASTER_KEY_MSB_ONLY;
            }
            else {
                acl_type = ARAD_KBP_ACL_IN_MASTER_KEY_NOT_REQUIRED;
            }

            
            if(acl_type != ARAD_KBP_ACL_IN_MASTER_KEY_NOT_REQUIRED) {
                search_format.nof_key_segments += (acl_type == ARAD_KBP_ACL_IN_MASTER_KEY_LSB_MSB) ? 2 : 1;
            }

            
            if(search_format.nof_key_segments >= ARAD_KBP_MAX_NOF_KEY_SEGMENTS) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): Maximum allowed number of key segments exceeded (%d). New number of segments %d.\n"),
                                FUNCTION_NAME(), ARAD_KBP_MAX_NOF_KEY_SEGMENTS, search_format.nof_key_segments));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            
            for(i = 0; i < nof_range_quals; i++) {
                if ((range_quals_info[i].qual_type != SOC_PPC_FP_QUAL_HDR_ELK_RANGE_IPV4_SRC_PORT) &&
                    (range_quals_info[i].qual_type != SOC_PPC_FP_QUAL_HDR_ELK_RANGE_IPV4_DEST_PORT)
                ) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): Given qualifier %s (%d) is not supported as range qualifier for KBP.\n"),
                                    FUNCTION_NAME(), _bcm_dpp_field_qual_name[range_quals_info[i].qual_type], range_quals_info[i].qual_type));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                }
            }

            
            segment_idx = nof_shared_segments;

            
            if(acl_s0_size) {
                sal_sprintf(search_format.key_segment[segment_idx].name, "ACL%d-s0", table_id);
                search_format.key_segment[segment_idx].type = KBP_KEY_FIELD_TERNARY;
                search_format.key_segment[segment_idx].nof_bytes = acl_s0_size;
                segment_idx++;
            }


            
            for(i = 0; i < nof_range_quals; i++) {
                
                sal_sprintf(search_format.key_segment[segment_idx].name, "ACL%d-r%d", table_id, i);
                search_format.key_segment[segment_idx].type = KBP_KEY_FIELD_RANGE;
                search_format.key_segment[segment_idx].nof_bytes = range_quals_info[i].size_in_bytes;
                
                range_quals_info[i].offset = shared_quals_size_in_bytes + acl_s0_size + range_quals_info[i].offset;
                segment_idx++;

                res = KBP_INFO.db_info.range_qualifier.set(unit, table_id, i, &range_quals_info[i]);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
            }

            res = KBP_INFO.db_info.nof_range_qualifiers.set(unit, table_id, nof_range_quals);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);


            
            if(acl_s1_size) {
                sal_sprintf(search_format.key_segment[segment_idx].name, "ACL%d-s1", table_id);
                search_format.key_segment[segment_idx].type = KBP_KEY_FIELD_TERNARY;
                search_format.key_segment[segment_idx].nof_bytes = acl_s1_size;
                segment_idx++;
            }
        }

        
        res = arad_kbp_ltr_lookup_add(
                   unit,
                   search_id,
                   opcode,
                   ltr_id,
                   flags,
                   table_id,
                   acl_type,
                   &search_format,
                   nof_shared_segments,
                   nof_range_quals,
                   success
               );

        SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

        
        if(SOC_IS_JERICHO(unit))
        {
            if((elk->kbp_no_fwd_ipv6_dip_sip_sharing_disable == 0) &&
               (elk->kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header != 0) &&
               (ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_UC_VALID(opcode)))
            {
                dummy_search_added = TRUE;
                res = arad_kbp_search_key_segments_append_to_ltr_master_key(unit, db_type, ipv6_dip_sip_sharing_header);
                SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
            }
            if(ARAD_KBP_PROG_IS_L2_SHARING(opcode) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_da_sa_sharing_enable", 0))
            {
                res = arad_kbp_search_key_segments_append_to_ltr_master_key(unit, db_type, &l2_da_sa_sharing_forwarding_header_search);
                SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
            }

            if (!dummy_search_added)
            {
                
                res = arad_kbp_search_key_segments_append_to_ltr_master_key(unit, db_type, &dummy_key_search);
                SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);
            }
        }

        if(SOC_SAND_SUCCESS != *success) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

        
        res = arad_kbp_add_ltr_to_opcode(
                unit,
                db_type,
                flags,
                opcode,
                ltr_id,
                key_size_in_bytes,
                search_id,
                &search_format,
                nof_shared_segments,
                success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

        if(SOC_SAND_SUCCESS != *success) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

        
        if ((prog_id == PROG_FLP_IPV6MC) && (search_id == 1) && !(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) {
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC, FALSE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
        }
        if ((prog_id == PROG_FLP_IPV4COMPMC_WITH_RPF) && (search_id == 1) && !(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) {
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC, FALSE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
        }
        if ((prog_id == PROG_FLP_IPV6UC_RPF) && (search_id == 1) && !(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) {
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC, FALSE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
        }
        if ((prog_id == PROG_FLP_IPV4UC_RPF) && (search_id == 1) && !(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) {
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC, FALSE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
        }
        if ((prog_id == PROG_FLP_IPV4_DC) && (search_id == 1) && !(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) {
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC, FALSE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
        }
        if ((prog_id == PROG_FLP_IPV4UC_PUBLIC_RPF) && (search_id == 1) && !(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) {
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC_PUBLIC, FALSE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
        }
        if ((prog_id == PROG_FLP_IPV6UC_PUBLIC_RPF) && (search_id == 1) && !(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) {
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC_PUBLIC, FALSE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
        }

        
        SHR_BITSET(pgm_bmp_new, pgm_ndx);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_acl()", 0, 0);
}

static
int arad_kbp_acl_range_validate_index(int unit, uint32 range_id)
{
    int res;
    uint32 max_num_ranges;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = KBP_INFO.Arad_kbp_max_num_ranges.get(unit, &max_num_ranges);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if(range_id >= max_num_ranges) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Given range ID %d is invalid. Must be between 0 - %d\n"),
                        FUNCTION_NAME(), range_id, max_num_ranges));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_acl_range_validate_index()", 0, 0);
}

static
int arad_kbp_acl_range_validate_range(int unit, uint32 emin, uint32 emax, uint16 *min, uint16 *max)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(min);
    SOC_SAND_CHECK_NULL_INPUT(max);

    if(emin > 0xffff || emax > 0xffff) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Given minimum or maximum for the range is out of range\n Min=%d Max=%d Allowed range is 0 - 65535\n"),
                        FUNCTION_NAME(), emin, emax));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    } else if(emin > emax) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Given minimum (%d) is bigger than the given maximum (%d)\n"),
                        FUNCTION_NAME(), emin, emax));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    } else {
        *min = (uint16)emin;
        *max = (uint16)emax;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_acl_range_validate_range()", 0, 0);
}

static
int arad_kbp_acl_range_validate_flags(int unit, uint32 flags)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (!(flags & BCM_FIELD_RANGE_SRCPORT) && !(flags & BCM_FIELD_RANGE_DSTPORT)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): No relevant flags are given. Supported are BCM_FIELD_RANGE_SRCPORT and BCM_FIELD_RANGE_DSTPORT\n"),
                        FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_acl_range_validate_flags()", 0, 0);
}

uint32 arad_kbp_acl_range_get_by_range_and_flags(int unit, uint32 flags, uint32 *range_id, uint32 emin, uint32 emax, int *result)
{
    uint32 res = BCM_E_NONE;
    uint32 index;
    kbp_acl_range_t range;
    uint16 min;
    uint16 max;
    uint32 max_num_ranges;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(result);

    res = arad_kbp_acl_range_validate_range(unit, emin, emax, &min, &max);
    if(res != SOC_SAND_NO_ERR) {
        *result = BCM_E_PARAM;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = KBP_INFO.Arad_kbp_max_num_ranges.get(unit, &max_num_ranges);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    for(index = 0; index < max_num_ranges; index++) {
        res = KBP_INFO.Arad_kbp_acl_range.get(unit, index, &range);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(range.in_use == TRUE && range.low == min && range.high == max && range.flags == flags) {
            
            break;
        }
    }

    if(index == max_num_ranges) {
        *result = BCM_E_NOT_FOUND;
    } else {
        if(range_id) {
            *range_id = index;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_acl_range_get_by_range_and_flags()", 0, 0);
}

uint32 arad_kbp_acl_range_get_by_id(int unit, uint32 *flags, uint32 range_id, uint32 *emin, uint32 *emax, int *result)
{
    uint32 res = BCM_E_NONE;
    kbp_acl_range_t range;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(result);

    *result = BCM_E_NONE;

    res = arad_kbp_acl_range_validate_index(unit, range_id);
    if(res != SOC_SAND_NO_ERR) {
        *result = BCM_E_PARAM;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = KBP_INFO.Arad_kbp_acl_range.get(unit, range_id, &range);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(range.in_use == FALSE) {
        *result = BCM_E_NOT_FOUND;
    } else {
        if(flags) {
            *flags = range.flags;
        }
        if(emin) {
            *emin = range.low;
        }
        if(emax) {
            *emax = range.high;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_acl_range_get_by_id()", 0, 0);
}

uint32 arad_kbp_acl_range_create(int unit, uint32 flags, uint32 *range_id, uint32 emin, uint32 emax, uint8 is_id_api, int *result)
{
    uint32 res = BCM_E_NONE;
    uint32 index;
    kbp_acl_range_t range;
    int not_found;
    uint16 min;
    uint16 max;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(result);
    SOC_SAND_CHECK_NULL_INPUT(range_id);

    res = arad_kbp_acl_range_validate_flags(unit, flags);
    if(res != SOC_SAND_NO_ERR) {
        *result = BCM_E_INTERNAL;
        ARAD_DO_NOTHING_AND_EXIT;
    }
    res = arad_kbp_acl_range_validate_range(unit, emin, emax, &min, &max);
    if(res != SOC_SAND_NO_ERR) {
        *result = BCM_E_PARAM;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    if(flags & BCM_FIELD_RANGE_REPLACE) {
        res = arad_kbp_acl_range_get_by_id(unit, NULL, *range_id, NULL, NULL, &not_found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(not_found) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Trying to replace a range that is not in use.\n"),
                            FUNCTION_NAME()));
            *result = BCM_E_PARAM;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
        
        index = *range_id;
    } else {
        if(is_id_api) {
            res = arad_kbp_acl_range_validate_index(unit, *range_id);
            if(res != SOC_SAND_NO_ERR) {
                *result = BCM_E_PARAM;
                ARAD_DO_NOTHING_AND_EXIT;
            }
            res = arad_kbp_acl_range_get_by_id(unit, NULL, *range_id, NULL, NULL, &not_found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            if(not_found) {
                index = *range_id;
            } else {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): Range ID %d is in use and BCM_FIELD_RANGE_REPLACE is not set.\n"),
                                FUNCTION_NAME(), *range_id));
                *result = BCM_E_PARAM;
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        } else {
            uint32 max_num_ranges;
            res = KBP_INFO.Arad_kbp_max_num_ranges.get(unit, &max_num_ranges);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

            
            for(index = 0; index < max_num_ranges; index++) {
                res = KBP_INFO.Arad_kbp_acl_range.get(unit, index, &range);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

                if(range.in_use == FALSE) {
                    
                    break;
                }
            }
            if(index == max_num_ranges) {
                
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): KBP acl ranges exhausted\n"),
                                FUNCTION_NAME()));
                *result = BCM_E_FULL;
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }
    }

    range.in_use = TRUE;
    range.flags = flags;
    range.low = min;
    range.high = max;

    res = KBP_INFO.Arad_kbp_acl_range.set(unit, index, &range);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *range_id = index;
    *result = BCM_E_NONE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_acl_range_create()", 0, 0);
}

uint32
    arad_kbp_acl_range_qualify_add(
        void *unitDataIn,
        bcm_field_qualify_t qual,
        bcm_field_entry_t entry,
        bcm_field_range_t range_id,
        int *result
    )
{
    bcm_dpp_field_info_OLD_t *unitData = (bcm_dpp_field_info_OLD_t*)unitDataIn;
    int unit = unitData->unit;
    uint32 res;
    uint32 flags;
    int not_found;
    uint64 edata;
    uint64 emask;
    int i;
    bcm_field_qualify_t range_qual[ARAD_KBP_MAX_NOF_RANGES_IN_TABLE] = { bcmFieldQualifyElkRangeL4SrcPort, bcmFieldQualifyElkRangeL4DstPort };
    uint8 qual_add[ARAD_KBP_MAX_NOF_RANGES_IN_TABLE] = { FALSE, FALSE };

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(result);

    *result = BCM_E_NONE;

    
    if(_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): KBP range qualifiers are not supported for preselectors\n"),
                    FUNCTION_NAME()));
        *result = BCM_E_PARAM;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    } else {
        *result = _bcm_dpp_field_entry_exists(unitData, entry);
        if(*result != BCM_E_NONE) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }
    }

    
    if(qual != bcmFieldQualifyL4PortRangeCheck) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Provided qualifier %s (%d) is not supported as ranged in external TCAM\n"),
                        FUNCTION_NAME(), _bcm_dpp_field_qual_name[qual], qual));
        *result = BCM_E_PARAM;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

    
    res = arad_kbp_acl_range_get_by_id(unit, &flags, range_id, NULL, NULL, &not_found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(not_found) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Range ID %d is not in use\n"),
                        FUNCTION_NAME(), range_id));
        *result = BCM_E_NOT_FOUND;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

    res = arad_kbp_acl_range_validate_flags(unit, flags);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    if(flags & BCM_FIELD_RANGE_SRCPORT) {
        qual_add[0] = TRUE;
    }
    if(flags & BCM_FIELD_RANGE_DSTPORT) {
        qual_add[1] = TRUE;
    }

    for(i = 0; i < ARAD_KBP_MAX_NOF_RANGES_IN_TABLE; i++) {
        if(qual_add[i]) {
            *result = _bcm_dpp_field_entry_qualifier_general_get_int(unitData, entry, range_qual[i], 1 , &edata, &emask);
            if(*result == BCM_E_NOT_FOUND) {
                
                COMPILER_64_ZERO(edata);
                COMPILER_64_ZERO(emask);
                *result = BCM_E_NONE;
            }
            if(*result != BCM_E_NONE) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }

            COMPILER_64_SET(edata, 0, range_id);
            COMPILER_64_SET(emask, 0, 0x0000FFFF);

            *result = _bcm_dpp_field_entry_qualifier_general_set_int(unitData, entry, range_qual[i], 1 , &edata, &emask);
            if(*result != BCM_E_NONE) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_acl_range_qualify_add()", 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif 

