/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include <soc/mcm/memregs.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/JER/JER_PP/jer_pp_diag.h>
#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_kaps_xpt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_kaps_xpt.h>
#include <soc/dpp/JER/jer_sbusdma_desc.h>

#include <soc/sand/sand_signals.h>



#define JER_KAPS_RPB_CAM_BIST_CONTROL_REG_ADDR  0x2A
#define JER_KAPS_RPB_CAM_BIST_STATUS_REG_ADDR   0x2B

const char* JER_KAPS_TABLE_NAMES[] = {"CORE_0_PRIVATE_IPV4_UC",
                                      "CORE_1_PRIVATE_IPV4_UC",
                                      "CORE_0_PRIVATE_IPV6_UC",
                                      "CORE_1_PRIVATE_IPV6_UC",
                                      "CORE_0_PRIVATE_IPV4_MC",
                                      "CORE_0_PRIVATE_IPV6_MC",
                                      "CORE_0_PUBLIC_IPV4_UC ",
                                      "CORE_1_PUBLIC_IPV4_UC ",
                                      "CORE_0_PUBLIC_IPV6_UC ",
                                      "CORE_1_PUBLIC_IPV6_UC ",
                                      "CORE_0_PUBLIC_IPV4_MC ",
                                      "CORE_0_PUBLIC_IPV6_MC "};


soc_error_t soc_jer_pp_diag_kaps_lkup_info_get(
   SOC_SAND_IN int unit,
   SOC_SAND_IN int core_id
   )
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    int flp_prog_id, num_of_flp_progs;
    int flp_invoked = 0;
    uint32 flp_lpm_lookups[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint32 rv = SOC_SAND_OK;
    uint32 dbal_table_id = 0;
    SOC_DPP_DBAL_TABLE_INFO dbal_table;
    
    uint32 search_keys[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)];
    uint8 search_keys_8[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint32 ad_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint8 ad_array_8[(BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS))];
    uint32 status[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint32 prio_len[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    uint32 i,j;

    char* search_names[] = {"KAPS First Private (default: RPF vrf!=0)",
                            "KAPS First Public (default: RPF vrf==0)",
                            "KAPS Second Private (default: FWD vrf!=0)",
                            "KAPS Second Public (default: FWD vrf==0)"
                            };

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(flp_lpm_lookups, 0, sizeof(uint32) * JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES);

    for (i=0; i<JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
        for (j=0; j<BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES); j++) {
            search_keys[i][j] = 0;
        }
        ad_array[i] = 0;
        status[i] = 0;
    }

    
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FLP", "LPM", "LPM_1st_A_Lookup_Key", search_keys[0], BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FLP", "LPM", "LPM_1st_B_Lookup_Key", search_keys[1], BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FLP", "LPM", "LPM_2nd_A_Lookup_Key", search_keys[2], BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FLP", "LPM", "LPM_2nd_B_Lookup_Key", search_keys[3], BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)));

    
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_1st_A_Lookup_Result", &ad_array[0], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_1st_B_Lookup_Result", &ad_array[1], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_2nd_A_Lookup_Result", &ad_array[2], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_2nd_B_Lookup_Result", &ad_array[3], 1));

    
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_1st_A_Lookup_Status", &status[0], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_1st_B_Lookup_Status", &status[1], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_2nd_A_Lookup_Status", &status[2], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_2nd_B_Lookup_Status", &status[3], 1));

    
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_1st_A_Lookup_Length", &prio_len[0], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_1st_B_Lookup_Length", &prio_len[1], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_2nd_A_Lookup_Length", &prio_len[2], 1));
    SOCDNX_SAND_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_2nd_B_Lookup_Length", &prio_len[3], 1));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_flp_access_print_last_programs_data(unit, core_id, 1, &flp_prog_id, &num_of_flp_progs));

    
    if (num_of_flp_progs != 0 ) {
        rv = arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, flp_prog_id, &flp_lookups_tbl);
        SOCDNX_SAND_IF_ERR_EXIT(rv);

        flp_lpm_lookups[0] = flp_lookups_tbl.lpm_1st_lkp_valid;
        flp_lpm_lookups[1] = flp_lookups_tbl.lpm_public_1st_lkp_valid;
        flp_lpm_lookups[2] = flp_lookups_tbl.lpm_2nd_lkp_valid;
        flp_lpm_lookups[3] = flp_lookups_tbl.lpm_public_2nd_lkp_valid;
        flp_invoked = 1;
    }
    cli_out("\n");

    
    for (i=0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
        
        if (flp_invoked == 0) {
            cli_out("Last invoked FLP Program was not detected, printing all KAPS searches. \n");
        } else if ((flp_lpm_lookups[0]==0) && (flp_lpm_lookups[1]==0) && (flp_lpm_lookups[2]==0) && (flp_lpm_lookups[3]==0)) {
            cli_out("Last invoked FLP Program did not include KAPS (LPM) searches.\n");
            break;
        }

        
        if (flp_lpm_lookups[i] == 0 && flp_invoked == 1) {
            continue;
        }

        cli_out("%s: \n", search_names[i]);

        for (j=0; j<JER_KAPS_KEY_BUFFER_NOF_BYTES; j++) {
            search_keys_8[j] = (search_keys[i][BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)-1-j/WORDS2BYTES(1)] >> BYTES2BITS(WORDS2BYTES(1) - (j % WORDS2BYTES(1)) - 1)) & SOC_SAND_U8_MAX;
        }

        ad_array_8[0] = (ad_array[i] >> 12) & SOC_SAND_U8_MAX;
        ad_array_8[1] = (ad_array[i] >> 4) & SOC_SAND_U8_MAX;
        ad_array_8[2] = (ad_array[i] << 4) & SOC_SAND_U8_MAX;

        for (dbal_table_id = 0; dbal_table_id < SOC_DPP_DBAL_SW_NOF_TABLES; dbal_table_id++) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, dbal_table_id, &dbal_table));

            if ((dbal_table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) && ((search_keys_8[0] >> (SOC_SAND_NOF_BITS_IN_BYTE - dbal_table.db_prefix_len)) == dbal_table.db_prefix)) {
                break;
            }
        }

        SOCDNX_IF_ERR_EXIT(jer_kaps_parse_print_entry(unit, dbal_table_id, prio_len[i], search_keys_8, ad_array_8, 1 ));
    }

exit:
  SOCDNX_FUNC_RETURN
}


int jer_kaps_search_generic(int unit, uint32 search_id, uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES],
                            uint32 *return_is_matched, uint32 *return_prefix_len, uint8  return_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)],
                            struct kbp_search_result *return_cmp_rslt)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    struct kbp_search_result cmp_rslt, *cmp_rslt_p;
    int32 i, j=0, is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], prefix_len[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint8 payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)];
    JER_KAPS_SEARCH_CONFIG search_cfg;
    JER_KAPS_IP_TBL_ID *table_id;
    uint32 key_offset;
    uint8 master_key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES * JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint32 key_index = search_id;

    memset(&cmp_rslt, 0, sizeof(cmp_rslt));
    memset(&master_key, 0, sizeof(uint8) * JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES * JER_KAPS_KEY_BUFFER_NOF_BYTES);

    
    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        if (key_index == JER_KAPS_IPV4_MC_SEARCH_ID) {
            key_index = JER_KAPS_IPV6_UC_SEARCH_ID;
        } else if (key_index == JER_KAPS_IPV6_UC_SEARCH_ID) {
            key_index = JER_KAPS_IPV4_MC_SEARCH_ID;
        }
    }

    if ((key[key_index][0] >> (SOC_SAND_NOF_BITS_IN_BYTE - JER_KAPS_TABLE_PREFIX_LENGTH)) == JER_KAPS_IPV4_UC_AND_NON_IP_TABLE_PREFIX) {
        
        jer_kaps_search_config_get(unit, JER_KAPS_IPV4_UC_SEARCH_ID, &search_cfg);
    } else if ((key[key_index][0] >> (SOC_SAND_NOF_BITS_IN_BYTE - JER_KAPS_TABLE_PREFIX_LENGTH)) == JER_KAPS_IPV4_MC_TABLE_PREFIX){
        
        jer_kaps_search_config_get(unit, JER_KAPS_IPV4_MC_SEARCH_ID, &search_cfg);
    } else {
        jer_kaps_search_config_get(unit, search_id, &search_cfg);
    }
    table_id = search_cfg.tbl_id;

    for(i = 0; i<search_cfg.valid_tables_num; i++) {
        key_offset = (i * search_cfg.max_tables_num) / search_cfg.valid_tables_num;
        is_matched[i] = 0;
        prefix_len[i] = 0;

        
        for (j=0; j < JER_KAPS_KEY_BUFFER_NOF_BYTES; j++){
            master_key[key_offset*JER_KAPS_KEY_BUFFER_NOF_BYTES + j] = key[i][j];
        }
        
        for (j=0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS); j++){
            payload[i][j] = 0;
        }
    }

    cli_out("\n\rSW search\n\r---------\n\r");
    cli_out("Dynamic tables are searched in IPV4 UC/MC!\n");

    for(i = 0; i<search_cfg.valid_tables_num; i++) {
        soc_sand_rv = jer_kaps_sw_search_test(unit, table_id[i], key[i], payload[i], &prefix_len[i], &is_matched[i]);
        rv = handle_sand_result(soc_sand_rv);
        if (BCM_FAILURE(rv)) {
            cli_out("Error: jer_kaps_sw_search_test(%d)\n", unit);
            return rv;
        }

        if (return_is_matched != NULL) {
            return_is_matched[i] = is_matched[i];
        }
        if (return_prefix_len != NULL) {
            return_prefix_len[i] = prefix_len[i];
        }
        if (return_payload != NULL) {
            for (j = 0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS); j++) {
                return_payload[i][j] =  payload[i][j];
            }
        }

        if (is_matched[i]) {
            cli_out("SW search %d in %s: matched!, payload:0x", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
            for (j = 0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS-7); j++) {
                cli_out("%02x", payload[i][j]);
            }
            
            cli_out("%x", payload[i][j] >> (BYTES2BITS(BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS))-JER_KAPS_AD_WIDTH_IN_BITS));
            cli_out(", prefix_len:%d\n", prefix_len[i]);
        }
        else {
            cli_out("SW search %d in %s: no match\n", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
        }
    }

    cli_out("\n\rHW search\n\r---------\n\r");
    cli_out("Dynamic tables are search in IPV4 UC/MC!\n");

    
    if (return_cmp_rslt != NULL) {
        cmp_rslt_p = return_cmp_rslt;
    } else {
        cmp_rslt_p = &cmp_rslt;
    }

    soc_sand_rv = jer_kaps_hw_search_test(unit, search_id, master_key, cmp_rslt_p);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: jer_kaps_hw_search_test(%d)\n", unit);
        return rv;
    }

    for(i = 0; i<search_cfg.valid_tables_num; i++) {
        key_offset = (i * search_cfg.max_tables_num) / search_cfg.valid_tables_num;
        
        if ((key_offset != i) && (return_cmp_rslt != NULL)) {
            sal_memcpy(return_cmp_rslt->assoc_data[i], cmp_rslt_p->assoc_data[key_offset], sizeof(uint8) * KBP_INSTRUCTION_MAX_AD_BYTES);
            return_cmp_rslt->hit_or_miss[i] = cmp_rslt_p->hit_or_miss[key_offset];
        }
        if (cmp_rslt_p->hit_or_miss[key_offset]) {
            cli_out("HW search %d in %s: matched!, payload:0x", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
            for (j = 0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS-7); j++) {
                cli_out("%02x", cmp_rslt_p->assoc_data[key_offset][j]);
            }
            
            cli_out("%x", cmp_rslt_p->assoc_data[key_offset][j] >> (BYTES2BITS(BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS))-JER_KAPS_AD_WIDTH_IN_BITS));
            cli_out("\n");
        }
        else {
            cli_out("HW search %d in %s: no match\n", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
        }
    }

    return rv;
}

int jer_kaps_sw_search_test(int unit, uint32 tbl_id, uint8 *key, uint8 *payload, int32 *prefix_len, int32 *is_matched)
{
    struct kbp_ad_db *ad_db_p;
    struct kbp_entry *entry_p = NULL;
    struct kbp_ad *ad_entry_p = NULL;
    int32 index, clone_db, res;
    JER_KAPS_TABLE_CONFIG table_cfg_p;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    jer_kaps_table_config_get(unit, tbl_id, &table_cfg_p);

    clone_db = jer_kaps_clone_of_db_id_get(unit, table_cfg_p.db_id);

    if (clone_db == JER_KAPS_IP_NOF_DB) {
        
        jer_kaps_ad_db_by_db_id_get(unit, table_cfg_p.db_id, &ad_db_p);
    }
    else {
        
        jer_kaps_ad_db_by_db_id_get(unit, clone_db, &ad_db_p);
    }

    res = kbp_db_search(table_cfg_p.tbl_p, key, &entry_p, &index, prefix_len);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_db_search failed with : %s!\n"),
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));

        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    if (entry_p) {
        *is_matched = TRUE;
        res = kbp_entry_get_ad(table_cfg_p.tbl_p, entry_p, &ad_entry_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): kbp_entry_get_ad failed: %s\n"),
                        FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }
        res = kbp_ad_db_get(ad_db_p, ad_entry_p, payload);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): kbp_ad_db_get failed: %s\n"),
                        FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_search_test()", 0, 0);
}

int jer_kaps_hw_search_test(int unit, uint32 search_id, uint8 *master_key, struct kbp_search_result *cmp_rslt)
{
    int32 res;
    JER_KAPS_SEARCH_CONFIG search_cfg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    jer_kaps_search_config_get(unit, search_id, &search_cfg);
    res = kbp_instruction_search(search_cfg.inst_p, master_key, 0, cmp_rslt);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_instruction_search failed with : %s!\n"),
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));

        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_search_test()", 0, 0);
}

int jer_kaps_parse_print_entry(int unit, uint32 dbal_table_id, uint32 prio_len, uint8 *data_bytes, uint8 *ad_8, uint8 header_flag) {
    uint32 res;
    uint32 qual_print_len = 0;
    uint32 i;
    uint32 temp_prio_len;

    uint32 qual_prio_len;
    uint32 qual_max_size; 
    uint32 temp_zero_padding, zero_padding; 
    uint32 qual_tbl_index, qual_type_tbl_index = 0;
    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    char qual_name[20];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, dbal_table_id, &dbal_table);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_dbal_kbp_buffer_to_entry_key(unit, &dbal_table, prio_len, data_bytes, qual_vals);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    cli_out(header_flag? "|Raw: 0x" : "|     0x");
    for (i = 0; i < JER_KAPS_KEY_BUFFER_NOF_BYTES; i++) {
        cli_out("%02x", data_bytes[i]);
    }
    cli_out("/%03d", prio_len);
    cli_out(header_flag ? " |TBL:0x%*x/%01d/%01d" : " |    0x%*x/%01d/%01d", (dbal_table.db_prefix_len + 3)/4, dbal_table.db_prefix, dbal_table.db_prefix_len, dbal_table.db_prefix_len);

    prio_len -= dbal_table.db_prefix_len;
    
    temp_prio_len = prio_len;
    zero_padding = 0;
    temp_zero_padding = 0;
    qual_tbl_index = dbal_table.nof_qualifiers - 1; 
    while (qual_tbl_index != -1) {
        if (dbal_table.qual_info[qual_tbl_index].qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) {
             temp_zero_padding += dbal_table.qual_info[qual_tbl_index].qual_nof_bits;
        } else { 
            if (temp_prio_len > 0 && temp_prio_len <= prio_len) {
                zero_padding += temp_zero_padding;
                temp_zero_padding = 0;
            } else {
                break;
            }
        }
        temp_prio_len -= dbal_table.qual_info[qual_tbl_index].qual_nof_bits;

        qual_tbl_index--;
    }
    prio_len -= zero_padding;

    for (i=0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
        if (((qual_vals[i].type != SOC_PPC_NOF_FP_QUAL_TYPES) && (qual_vals[i].type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)
            && (qual_vals[i].type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES)) && (qual_vals[i].type != BCM_FIELD_ENTRY_INVALID)) { 

            if (qual_vals[i].type > SOC_PPC_NOF_FP_QUAL_TYPES) {
                cli_out("\nUnexpected qual type!\n");
                ARAD_DO_NOTHING_AND_EXIT;
            }

            switch (qual_vals[i].type) {
            case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP:
                sal_strcpy(qual_name, "DIPv4");
                qual_print_len = 8;
                break;
            case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP:
                sal_strcpy(qual_name, "SIPv4");
                qual_print_len = 8;
                break;
            case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW:
                sal_strcpy(qual_name, "DIPv6_L");
                qual_print_len = 8;
                break;
            case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH:
                sal_strcpy(qual_name, "DIPv6_H");
                qual_print_len = 8;
                break;
            case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW:
                sal_strcpy(qual_name, "SIPv6_L");
                qual_print_len = 8;
                break;
            case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH:
                sal_strcpy(qual_name, "SIPv6_H");
                qual_print_len = 8;
                break;
            case SOC_PPC_FP_QUAL_IRPP_VRF:
                sal_strcpy(qual_name, "VRF");
                qual_print_len = 4;
                break;
            case SOC_PPC_FP_QUAL_IRPP_IN_RIF:
                sal_strcpy(qual_name, "INRIF");
                qual_print_len = 4;
                break;
            default:
                
                if (sal_strcmp(" Unknown", SOC_PPC_FP_QUAL_TYPE_to_string(qual_vals[i].type))) {
                    if (sal_strlen(SOC_PPC_FP_QUAL_TYPE_to_string(qual_vals[i].type))>=20) {
                        cli_out("Qual name string too long!\n");
                        sal_strncpy(qual_name, SOC_PPC_FP_QUAL_TYPE_to_string(qual_vals[i].type), 19);
                        qual_name[19] = '\0';
                    } else {
                        sal_strcpy(qual_name, SOC_PPC_FP_QUAL_TYPE_to_string(qual_vals[i].type));
                    }
                } else {
                    sal_sprintf(qual_name, "Qual-%03d", qual_vals[i].type);
                }
                qual_print_len = 8;
            }
            header_flag ? cli_out(" |%s: 0x", qual_name) : cli_out(" |%*s  0x", (int)strlen(qual_name), "");

            qual_tbl_index = 0;
            qual_max_size = 0;
            while (qual_tbl_index != SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                if (dbal_table.qual_info[qual_tbl_index].qual_type == qual_vals[i].type) {
                    qual_max_size += dbal_table.qual_info[qual_tbl_index].qual_nof_bits;
                    qual_type_tbl_index = qual_tbl_index;
                }
                qual_tbl_index++;
            }

            
            if (dbal_table.qual_info[qual_type_tbl_index].qual_full_size > SOC_SAND_NOF_BITS_IN_UINT32) {
                cli_out("%0*x", 8, qual_vals[i].val.arr[1]);
            }

            
            qual_prio_len = prio_len > qual_max_size ? qual_max_size : prio_len;
            prio_len -= qual_prio_len;
            cli_out("%0*x/%02d/%02d", qual_print_len, qual_vals[i].val.arr[0], qual_prio_len, qual_max_size);
        }
    }

    cli_out(header_flag ? " |Result: 0x" : " |        0x");

    for (i=0; i < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS - 7); i++ ) {
        cli_out("%02x", ad_8[i]);
    }
    
    cli_out("%01x/%02d", ad_8[i] >> (BYTES2BITS(BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS))-JER_KAPS_AD_WIDTH_IN_BITS), JER_KAPS_AD_WIDTH_IN_BITS);

    cli_out("|\n\r");

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_parse_print_entry()", 0, 0);
}


int jer_kaps_show_table(int unit, uint32 dbal_table_id, uint32 print_entries) {
    struct kbp_entry_iter *iter;
    struct kbp_entry *kpb_e;
    struct kbp_entry_info kpb_e_info;
    struct kbp_ad_db *ad_db_p;

    JER_KAPS_TABLE_CONFIG table_cfg_p, table_cfg_p_iter;
    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    JER_KAPS_DB *tbl_p;
    uint32 j, i;
    uint32 res;

    uint8 db_id = 0;

    uint8 ad_8[BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)];
    uint32 tbl_idx = 0;

    uint32 num_of_entries = 0;

    uint8 header_flag = 1;

    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, dbal_table_id, &dbal_table);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    if (dbal_table.physical_db_type != SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
        cli_out("Provided DBAL_TBL_ID is not associated with KAPS.\n");
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    for (j=0; j<2; j++) {
        if((j == 0) && (SOC_DPP_CONFIG(unit)->arad->init.elk.tcam_dev_type != ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE)) {
            
            continue;
        }
        if (!JER_KAPS_ENABLE_PUBLIC_DB(unit) && j==0) {
            continue;
        }
        if (!JER_KAPS_ENABLE_PRIVATE_DB(unit) && j==1) {
            continue;
        }

        res = jer_pp_kaps_dbal_table_id_translate(unit, dbal_table_id, NULL, j, &tbl_idx);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (tbl_idx < JER_KAPS_IP_NOF_TABLES) {
            jer_kaps_table_config_get(unit, tbl_idx, &table_cfg_p);
            tbl_p = table_cfg_p.tbl_p;
            db_id = table_cfg_p.db_id;
        } else {
            tbl_p = NULL;
        }


        
        if ((table_cfg_p.clone_of_tbl_id == JER_KAPS_IP_NOF_TABLES)
            || ((table_cfg_p.clone_of_tbl_id == JER_KAPS_TABLE_USE_DB_HANDLE) &&
                (jer_kaps_clone_of_db_id_get(unit, table_cfg_p.db_id) == JER_KAPS_IP_NOF_DB))) {

            

            
            KBP_TRY(kbp_db_entry_iter_init(tbl_p, &iter));
            num_of_entries = 0;
            
            do {
                KBP_TRY(kbp_db_entry_iter_next(tbl_p, iter, &kpb_e));

                if (kpb_e == NULL)
                    break;

                KBP_TRY(kbp_entry_get_info(tbl_p, kpb_e, &kpb_e_info));
                
                if (kpb_e_info.data[0] >> (SOC_SAND_NOF_BITS_IN_BYTE - dbal_table.db_prefix_len) != dbal_table.db_prefix) {
                    continue;
                }

                num_of_entries += 1;

            } while (1);

            
            KBP_TRY(kbp_db_entry_iter_destroy(tbl_p, iter));


            if (tbl_idx < JER_KAPS_IP_PUBLIC_INDEX) {
                cli_out("\n\rDBAL Table ID %d: %s - Private DB, number of entries: %d."
                        "\n\r---------------------------------------------------------------"
                        "-------------------------------------------------------------------"
                        "--------------------------------------------------------------------\n\r",
                        dbal_table_id, dbal_table.table_name, num_of_entries);
            } else {
                cli_out("\n\rDBAL Table ID %d: %s - Public DB, number of entries: %d."
                        "\n\r---------------------------------------------------------------"
                        "-------------------------------------------------------------------"
                        "--------------------------------------------------------------------\n\r",
                        dbal_table_id, dbal_table.table_name, num_of_entries);
            }

            header_flag = 1;

            if (print_entries) {
                
                KBP_TRY(kbp_db_entry_iter_init(tbl_p, &iter));

                
                do {
                    KBP_TRY(kbp_db_entry_iter_next(tbl_p, iter, &kpb_e));

                    if (kpb_e == NULL)
                        break;

                    KBP_TRY(kbp_entry_get_info(tbl_p, kpb_e, &kpb_e_info));

                    
                    if (kpb_e_info.data[0] >> (SOC_SAND_NOF_BITS_IN_BYTE - dbal_table.db_prefix_len) != dbal_table.db_prefix) {
                        continue;
                    }

                    jer_kaps_ad_db_by_db_id_get(unit, db_id, &ad_db_p);

                    if ((ad_db_p != NULL) && (kpb_e_info.ad_handle != NULL)) {
                            KBP_TRY(kbp_ad_db_get(ad_db_p, kpb_e_info.ad_handle, ad_8));
                    } else {
                        sal_memset(ad_8, 0, BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS));
                    }

                    res = jer_kaps_parse_print_entry(unit, dbal_table_id, kpb_e_info.prio_len, kpb_e_info.data, ad_8, header_flag);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

                    header_flag = 0;
                } while (1);

                
                KBP_TRY(kbp_db_entry_iter_destroy(tbl_p, iter));
            }
        } else { 
            SOC_DPP_DBAL_SW_TABLE_IDS orig_dbal_table_id;
            SOC_DPP_DBAL_TABLE_INFO orig_dbal_table;

            if (table_cfg_p.clone_of_tbl_id == JER_KAPS_TABLE_USE_DB_HANDLE) {
                
                uint32 orig_dbal_id = jer_kaps_clone_of_db_id_get(unit, table_cfg_p.db_id);
                for (i=0; i<JER_KAPS_IP_NOF_TABLES ; i++) {
                    jer_kaps_table_config_get(unit, i, &table_cfg_p_iter);
                    if ((table_cfg_p_iter.db_id == orig_dbal_id) && (table_cfg_p_iter.clone_of_tbl_id == JER_KAPS_TABLE_USE_DB_HANDLE)) {
                        break;
                    }
                }

                res = jer_pp_kaps_table_id_to_dbal_translate(unit, i, &orig_dbal_table_id);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            } else {
                res = jer_pp_kaps_table_id_to_dbal_translate(unit, table_cfg_p.clone_of_tbl_id, &orig_dbal_table_id);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            }

            res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, orig_dbal_table_id, &orig_dbal_table);
            SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

            if (tbl_idx < JER_KAPS_IP_PUBLIC_INDEX) {
                cli_out("\n\rDBAL Table ID %d: %s is clone of DBAL Table ID %d: %s - Private DB"
                        "\n\r---------------------------------------------------------------"
                        "-------------------------------------------------------------------"
                        "--------------------------------------------------------------------\n\r",
                        dbal_table_id, dbal_table.table_name, orig_dbal_table_id, orig_dbal_table.table_name);
            } else {
                cli_out("\n\rDBAL Table ID %d: %s is clone of DBAL Table ID %d: %s - Public DB"
                        "\n\r---------------------------------------------------------------"
                        "-------------------------------------------------------------------"
                        "--------------------------------------------------------------------\n\r",
                        dbal_table_id, dbal_table.table_name, orig_dbal_table_id, orig_dbal_table.table_name);
            }
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_show_table()", 0, 0);
}



int jer_kaps_show_db_stats( int unit, uint32 dbal_table_id ) {
    struct kbp_db_stats stats;

    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    JER_KAPS_DB *tbl_p;
    JER_KAPS_TABLE_CONFIG table_cfg_p;
    uint32 i, j;
    uint32 res;

    uint32 tbl_idx;

    uint32 db_size;
    uint32 db_width;

    SOC_SAND_INIT_ERROR_DEFINITIONS( 0 );

    res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get( unit, dbal_table_id, &dbal_table );
    SOC_SAND_CHECK_FUNC_RESULT( res, 12, exit );

    if (dbal_table.physical_db_type != SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
        cli_out("Provided DBAL_TBL_ID is not associated with KAPS.\n");
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    for ( j = 0; j < 2 ; j++ ) {
        res = jer_pp_kaps_dbal_table_id_translate( unit, dbal_table_id, NULL, j, &tbl_idx);
        SOC_SAND_CHECK_FUNC_RESULT( res, 10, exit );

        if (tbl_idx < JER_KAPS_IP_NOF_TABLES) {
            jer_kaps_table_config_get(unit, tbl_idx, &table_cfg_p);
            tbl_p = table_cfg_p.tbl_p;

            if ((table_cfg_p.db_id == JER_KAPS_IP_CORE_0_PRIVATE_DB_ID) || (table_cfg_p.db_id == JER_KAPS_IP_CORE_1_PRIVATE_DB_ID))
            {
                db_size = soc_property_get(unit, spn_PRIVATE_IP_FRWRD_TABLE_SIZE, 0);
            }
            else
            {
                db_size = soc_property_get(unit, spn_PUBLIC_IP_FRWRD_TABLE_SIZE, 0);
            }

            db_width = dbal_table.db_prefix_len;
            
            for (i = 0; i < dbal_table.nof_qualifiers; i++) {
                if (dbal_table.qual_info[i].qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) {
                    db_width += dbal_table.qual_info[i].qual_nof_bits;
                }
            }
        } else {
            tbl_p = NULL;
        }

        
        if ( ( tbl_p != NULL ) && ( dbal_table.is_table_initiated ) ) {
            cli_out( " %-3d", dbal_table_id);
            cli_out( "%-10s", j ? "- Private" : "- Public" );
            cli_out( " %-23s", dbal_table.table_name );
            cli_out( " %-10d", db_size );
            cli_out( " %-13d", db_width );
            cli_out( " %-10d", JER_KAPS_AD_WIDTH_IN_BITS );
            res = kbp_db_stats( tbl_p, &stats );
            if ( ARAD_KBP_TO_SOC_RESULT( res ) != SOC_SAND_OK ) {
                LOG_ERROR( BSL_LS_SOC_TCAM, ( BSL_META_U( unit, "Error in %s(): DB: kbp_db_stats with failed: %s!\n" ), FUNCTION_NAME(), kbp_get_status_string( res ) ) );
            }
            cli_out( " %-13d", stats.num_entries );
            cli_out( " %-10d\n", stats.capacity_estimate );
        } else {
            cli_out( " %-10d", dbal_table_id );
            cli_out( " %-23s", dbal_table.table_name );
            cli_out( "not allocated\n" );
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_show_db_stats()", 0, 0 );
}


int jer_pp_kaps_tcam_bist(int unit)
{
    int32 res;
    uint32 blk_id, cam_index, cam_index_max, nbytes = sizeof(uint32);
    JER_KAPS_XPT xpt_p;
    uint8 data[4]={0}, res_data[4]={0}, expected_data[4]={0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    expected_data[0] = 0;
    expected_data[1] = 0;
    expected_data[2] = 0;
    expected_data[3] = 3;

    cam_index_max = SOC_IS_JERICHO_PLUS(unit) ? 1 : 2;

    
    sal_memset(&xpt_p, 0, sizeof(xpt_p));
    xpt_p.jer_data.unit = unit;

    
    if ((jer_kaps_app_data_get(unit) == NULL) && !SOC_IS_JERICHO_PLUS(unit)) {
        res = jer_pp_xpt_extended_command(&xpt_p, 0, KAPS_CMD_EXTENDED, KAPS_FUNC15, 0);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n jer_pp_xpt_extended_command Failed, Reason Code: %s\n"),
                      kbp_get_status_string(res)));

           SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }

        res = jer_pp_xpt_extended_command(&xpt_p, 0, KAPS_CMD_EXTENDED, KAPS_FUNC14, 0);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n jer_pp_xpt_extended_command Failed, Reason Code: %s\n"),
                      kbp_get_status_string(res)));

           SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
    }

    for (blk_id = JER_KAPS_RPB_BLOCK_INDEX_START; blk_id <= JER_KAPS_RPB_BLOCK_INDEX_END; blk_id++) {
        for(cam_index = 0; cam_index < cam_index_max; cam_index++) {
            sal_memset(data, 0, sizeof(uint32));
            sal_memset(res_data, 0, sizeof(uint32));
            data[3] = cam_index + 1;

            if (SOC_IS_JERICHO_PLUS(unit)) {
                res = qax_pp_kaps_write_command(&xpt_p,
                                                blk_id,
                                                KAPS_CMD_WRITE,
                                                KAPS_FUNC0,
                                                JER_KAPS_RPB_CAM_BIST_CONTROL_REG_ADDR,
                                                nbytes,
                                                data);
            } else {
                res = jer_pp_kaps_write_command(&xpt_p,
                                                blk_id,
                                                KAPS_CMD_WRITE,
                                                KAPS_FUNC0,
                                                JER_KAPS_RPB_CAM_BIST_CONTROL_REG_ADDR,
                                                nbytes,
                                                data);
            }

            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n jer_pp_kaps_write_command Failed, Reason Code: %s\n"),
                          kbp_get_status_string(res)));

               SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            if (SOC_IS_JERICHO_PLUS(unit)) {
                res = arad_polling(
                            unit,
                            ARAD_TIMEOUT,
                            ARAD_MIN_POLLS,
                            KAPS_RPB_CAM_BIST_STATUSr,
                            REG_PORT_ANY,
                            (blk_id - JER_KAPS_RPB_BLOCK_INDEX_START),
                            RPB_BIST_STATUS_Nf,
                            0x3
                           );
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
            }

            if (SOC_IS_JERICHO_PLUS(unit)) {
                res = qax_pp_kaps_read_command(&xpt_p,
                                               blk_id,
                                               KAPS_CMD_READ,
                                               KAPS_FUNC0,
                                               JER_KAPS_RPB_CAM_BIST_STATUS_REG_ADDR,
                                               nbytes,
                                               res_data);
            } else {
                res = jer_pp_kaps_read_command(&xpt_p,
                                               blk_id,
                                               KAPS_CMD_READ,
                                               KAPS_FUNC0,
                                               JER_KAPS_RPB_CAM_BIST_STATUS_REG_ADDR,
                                               nbytes,
                                               res_data);
            }

            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n jer_pp_kaps_read_command Failed, Reason Code: %s\n"),
                          kbp_get_status_string(res)));

               SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            
            data[3] = 0;

            if (SOC_IS_JERICHO_PLUS(unit)) {
                res = qax_pp_kaps_write_command(&xpt_p,
                                                blk_id,
                                                KAPS_CMD_WRITE,
                                                KAPS_FUNC0,
                                                JER_KAPS_RPB_CAM_BIST_CONTROL_REG_ADDR,
                                                nbytes,
                                                data);
            } else {
                res = jer_pp_kaps_write_command(&xpt_p,
                                                blk_id,
                                                KAPS_CMD_WRITE,
                                                KAPS_FUNC0,
                                                JER_KAPS_RPB_CAM_BIST_CONTROL_REG_ADDR,
                                                nbytes,
                                                data);
            }

            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n jer_pp_kaps_write_command revert bist Failed, Reason Code: %s\n"),
                          kbp_get_status_string(res)));

               SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            if (sal_memcmp(res_data, expected_data, 4) != 0) {
               uint32 res_data_print = res_data[0] << 24 | res_data[1] << 16 | res_data[2] << 8 | res_data[3];
               uint32 expected_data_print = expected_data[0] << 24 | expected_data[1] << 16 | expected_data[2] << 8 | expected_data[3];
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n Unexpected KAPS_RPB_CAM_BIST_STATUS_REG register value. actual: 0x%x expected: 0x%x\n"),
                          res_data_print, expected_data_print));

               SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_pp_kaps_tcam_bist()", 0, 0);
}

int jer_pp_kbp_sdk_ver_test(int unit)
{
    int32 res;
    uint32 regval, i, array_index = 0;
    
    uint32 number_of_elements;
    const char* kbp_sdk_ver_arr[] =       {"KBP SDK 1.5.13"};
    const uint32 sw_sdk_ver_arr_6_5[]  = {0x65e00000};
    const uint32 sw_sdk_ver_arr_6_4[]  = {0x64b00000};

    const uint32 *relevant_array;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_reg32_get(unit, ECI_SW_VERSIONr, REG_PORT_ANY,  0, &regval);
    if (res != SOC_SAND_OK) {
        cli_out("Error: jer_pp_kbp_sdk_ver_test(%d), soc_reg32_get\n", unit);
        return res;
    }

    
    if (regval < sw_sdk_ver_arr_6_5[0]) {
        relevant_array = sw_sdk_ver_arr_6_4;
    } else {
        relevant_array = sw_sdk_ver_arr_6_5;
    }

    
    number_of_elements = sizeof(sw_sdk_ver_arr_6_5)/sizeof(sw_sdk_ver_arr_6_5[0]);
    for (i=0; i<number_of_elements; i++) {
        if (regval >= relevant_array[i]) {
            array_index = i;
        }
    }

    if (sal_strncmp(kbp_sdk_ver_arr[array_index], kbp_device_get_sdk_version(), sal_strlen(kbp_sdk_ver_arr[array_index]))) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "\n kbp_sdk_ver and sw_sdk_ver do not match. Expected: %s. Current: %s.\n"),
                  kbp_sdk_ver_arr[array_index], kbp_device_get_sdk_version()));

       SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_pp_kbp_sdk_ver_test()", 0, 0);
}

STATIC
int jer_pp_kaps_verify_search_results(int unit, uint32 is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], uint32 expected_is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES],
                                      uint32 search_payload_32[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES],
                                      uint32 expected_search_payload_32[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES],
                                      uint32 prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], uint32 expected_prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES],
                                      uint32 num_of_valid_tables)
{
    uint32 i;

    SOCDNX_INIT_FUNC_DEFS;

    for (i=0; i < num_of_valid_tables; i++) {
        if (is_matched[i] == expected_is_matched[i]) {
            if (search_payload_32[i] == expected_search_payload_32[i]) {
                if ((prefix_len_array != NULL) && (prefix_len_array[i] != expected_prefix_len_array[i])) {
                    cli_out("search verification failed, wrong prefix length!\nDetails: Search id: %d Prefix_len: %d Expected_prefix_len: %d \n",
                            i, prefix_len_array[i], expected_prefix_len_array[i]);
                    SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                } else{
                    cli_out("Search %d verified!\n", i);
                }
            } else {
                cli_out("Search verification failed, wrong payloads! \nDetails: Search id: %d Payload: 0x%x Expected_payload: 0x%x \n",
                            i, search_payload_32[i], expected_search_payload_32[i]);
                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
            }
        } else {
            cli_out("Search verification failed, hit-match mismatch! \nDetails: Search id: %d Match: %d Expected_match: %d \n",
                        i, is_matched[i], expected_is_matched[i]);
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_pp_kaps_search_test(int unit, uint32 add_entries, uint32 search_entries, uint32 delete_entries, uint32 loop_test, uint32 cache_test)
{
    uint32 res;

    SOC_SAND_PP_IPV6_ADDRESS dip_v6_32,
           sip_v6_32,
        mc_v6_32;
    uint32 dip_v4 = 0, sip_v4 = 0, mc_v4 = 0, vrf = 0, inrif = 0;

    uint32 i,j,k;
    uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES];

    uint32 payload;

    JER_KAPS_SEARCH_CONFIG search_cfg;

    SOC_SAND_SUCCESS_FAILURE success;

    uint32  prefix_len;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S] = {SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX};
    uint32 partial_mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S] = {0, 0, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX};
    SOC_DPP_DBAL_SW_TABLE_IDS table_id;
    SOC_DPP_DBAL_TABLE_INFO table;

    struct kbp_search_result cmp_rslt;
    uint32 is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint32 expected_is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], expected_prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint8 search_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)];
    uint32 expected_search_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    uint32 search_payload_32[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    uint32 desc_dma_init_state = 0;

    SOCDNX_INIT_FUNC_DEFS;

    dip_v6_32.address[0] = 0x12345678;
    dip_v6_32.address[1] = 0x9abcdef0;
    dip_v6_32.address[2] = 0xfedcba98;
    dip_v6_32.address[3] = 0x76543210;

    sip_v6_32.address[0] = 0x01020304;
    sip_v6_32.address[1] = 0x05060708;
    sip_v6_32.address[2] = 0x090a0b0c;
    sip_v6_32.address[3] = 0x0d0e0f00;

    mc_v6_32.address[0] = 0x01020304;
    mc_v6_32.address[1] = 0x05060708;
    mc_v6_32.address[2] = 0x090a0b0c;
    mc_v6_32.address[3] = 0x0d0e0f00;

    dip_v4 = 0xa1b2c3d4;
    sip_v4 = 0xf9e8d7c6;
    mc_v4 =  0xf13579bd;

    vrf = 5;
    inrif = 3;

    #define IPV4_DIP_PAYLOAD 0x12345
    #define IPV4_SIP_PAYLOAD 0x54321
    #define IPV4_MC_PAYLOAD  0xabcde

    #define IPV6_DIP_PAYLOAD 0x67890
    #define IPV6_SIP_PAYLOAD 0x98765
    #define IPV6_MC_PAYLOAD  0xfedcb

    if (cache_test) {
        if (SOC_IS_JERICHO_PLUS(unit)) {
            desc_dma_init_state = jer_sbusdma_desc_is_enabled(unit);
            if (!desc_dma_init_state) {
                SOCDNX_IF_ERR_EXIT(jer_sbusdma_desc_init(unit, 0x100, 0x1000, 10000));
            }
        } else {
            cli_out("Error: KAPS caching is only available in QAX and above\n");
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_UNAVAIL);
        }
    }

    do {
        loop_test--;
        for (k=0; k < 2; k++) { 
            
            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[0], sip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
            DBAL_QUAL_VAL_ENCODE_IPV6_SIP_LOW(&qual_vals[1], sip_v6_32.address, mask);
            DBAL_QUAL_VAL_ENCODE_IPV6_SIP_HIGH(&qual_vals[2], sip_v6_32.address, mask);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], dip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], dip_v6_32.address, mask);
            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], dip_v6_32.address, mask);
            DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[6], inrif, SOC_SAND_U32_MAX);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);

            if (add_entries && (k==0)) {
                
                payload = IPV4_DIP_PAYLOAD;
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, qual_vals, 0,  &payload, &success));

                
                payload = IPV4_SIP_PAYLOAD;
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], sip_v4, SOC_SAND_NOF_BITS_IN_UINT32/2);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, qual_vals, 0,  &payload, &success));

                
                payload = IPV4_MC_PAYLOAD;
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], mc_v4, SOC_SAND_NOF_BITS_IN_UINT32);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, qual_vals, 0,  &payload, &success));

                
                payload = IPV6_DIP_PAYLOAD;
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, qual_vals, 0,  &payload, &success));

                
                payload = IPV6_SIP_PAYLOAD;
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], sip_v6_32.address, partial_mask);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], sip_v6_32.address, partial_mask);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, qual_vals, 0,  &payload, &success));

                
                payload = IPV6_MC_PAYLOAD;
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], mc_v6_32.address, mask);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], mc_v6_32.address, mask);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC, qual_vals, 0,  &payload, &success));
            }

            if (delete_entries && (k==1)) {
                cli_out("\n-----------------------------\nClearing Test Entries.\n-----------------------------\n");
                
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, qual_vals, &success));

                
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], sip_v4, SOC_SAND_NOF_BITS_IN_UINT32/2);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, qual_vals, &success));

                
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], mc_v4, SOC_SAND_NOF_BITS_IN_UINT32);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, qual_vals, &success));

                
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, qual_vals, &success));

                
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], sip_v6_32.address, partial_mask);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], sip_v6_32.address, partial_mask);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE, qual_vals, &success));

                
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], mc_v6_32.address, mask);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], mc_v6_32.address, mask);
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC, qual_vals, &success));
            }

            if (SOC_IS_JERICHO_PLUS(unit) && jer_sbusdma_desc_is_enabled(unit)) {
                SOCDNX_IF_ERR_EXIT(jer_sbusdma_desc_wait_done(unit));
            } else {
                SOCDNX_IF_ERR_EXIT(jer_pp_xpt_wait_dma_done(unit));
            }
            sal_memset(expected_is_matched, 0, sizeof(uint32)*JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES);
            sal_memset(expected_search_payload, 0, sizeof(uint32)*JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES);
            sal_memset(expected_prefix_len_array, 0, sizeof(uint32)*JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES);
            if (search_entries && ((k==0 && add_entries) || (k==1 && delete_entries))) {
                for (j=0; j < JER_KAPS_NOF_SEARCHES; j++) {
                    jer_kaps_search_config_get(unit, j, &search_cfg);
                    if (!search_cfg.valid_tables_num) {
                        LOG_ERROR(BSL_LS_SOC_TCAM,
                             (BSL_META_U(unit,
                                         "\n Search config %d disabled.\n"), j));
                        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                    }

                    cli_out("\n-----------------------------\nTesting KAPS search config %d - Tables %s.\n-----------------------------\n", j, k ? "cleared" : "filled");

                    for (i=0; i < search_cfg.valid_tables_num; i++) {
                        
                        if (search_cfg.tbl_id[i] < JER_KAPS_IP_PUBLIC_INDEX) {
                            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);
                        } else{
                            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                        }

                        res = jer_pp_kaps_table_id_to_dbal_translate(unit, search_cfg.tbl_id[i], &table_id);
                        if (res != SOC_SAND_OK) {
                            cli_out("Error: jer_pp_kaps_search_test(%d), table translation failed\n", unit);
                            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                        }

                        res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table);
                        if (res != SOC_SAND_OK) {
                            cli_out("Error: jer_pp_kaps_search_test(%d), get dbal table failed\n", unit);
                            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                        }

                        
                        if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS) {
                            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], dip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
                        } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS) {
                            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], mc_v4, SOC_SAND_NOF_BITS_IN_UINT32);
                        } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE) {
                            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], dip_v6_32.address, mask);
                            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], dip_v6_32.address, mask);
                        } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC) {
                            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], mc_v6_32.address, mask);
                            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], mc_v6_32.address, mask);
                        }

                        res = arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, key[i]);
                        if (res != SOC_SAND_OK) {
                            cli_out("Error: jer_pp_kaps_search_test(%d), key buffer construction failed\n", unit);
                            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                        }

                        
                        if (k == 0) { 
                            SOC_DPP_DBAL_TABLE_INFO dbal_table;
                            res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, &dbal_table);
                            if (res != SOC_SAND_OK) {
                                cli_out("Error: sw_state_access(%d), Get DBAL table info failed\n", unit);
                                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                            }
                            switch (search_cfg.tbl_id[i]) {
                                case JER_KAPS_IP_CORE_1_PUBLIC_IPV4_UC_TBL_ID:
                                    expected_is_matched[i] = 1; expected_search_payload[i] = IPV4_SIP_PAYLOAD;
                                    
                                    expected_prefix_len_array[i] = 30 + (dbal_table.db_prefix_len > JER_KAPS_TABLE_PREFIX_LENGTH ? 6 : 2);
                                    break;
                                case JER_KAPS_IP_CORE_0_PUBLIC_IPV6_UC_TBL_ID:
                                    expected_is_matched[i] = 1; expected_search_payload[i] = IPV6_DIP_PAYLOAD; expected_prefix_len_array[i] = 144;
                                    break;
                                case JER_KAPS_IP_CORE_0_PUBLIC_IPV6_MC_TBL_ID:
                                    expected_is_matched[i] = 1; expected_search_payload[i] = IPV6_MC_PAYLOAD; expected_prefix_len_array[i] = 159;
                                    break;
                                case JER_KAPS_IP_CORE_0_PRIVATE_IPV4_UC_TBL_ID:
                                    expected_is_matched[i] = 1; expected_search_payload[i] = IPV4_DIP_PAYLOAD;
                                    
                                    expected_prefix_len_array[i] = 46 + (dbal_table.db_prefix_len > JER_KAPS_TABLE_PREFIX_LENGTH ? 6 : 2);
                                    break;
                                case JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID:
                                    expected_is_matched[i] = 1; expected_search_payload[i] = IPV4_MC_PAYLOAD; expected_prefix_len_array[i] = 92;
                                    break;
                                case JER_KAPS_IP_CORE_1_PRIVATE_IPV6_UC_TBL_ID:
                                    expected_is_matched[i] = 1; expected_search_payload[i] = IPV6_SIP_PAYLOAD; expected_prefix_len_array[i] = 80;
                                    break;
                                case JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID:
                                case JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID:
                                case JER_KAPS_IP_CORE_1_PUBLIC_IPV6_UC_TBL_ID:
                                case JER_KAPS_IP_CORE_1_PRIVATE_IPV4_UC_TBL_ID:
                                case JER_KAPS_IP_CORE_0_PRIVATE_IPV6_UC_TBL_ID:
                                case JER_KAPS_IP_CORE_0_PRIVATE_IPV6_MC_TBL_ID:
                                    expected_is_matched[i] = 0; expected_search_payload[i] = 0; expected_prefix_len_array[i] = 0;
                                    break;
                                default:
                                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("jer_pp_kaps_search_test - invalid kaps table_id: %d \n"), search_cfg.tbl_id[i]));
                            }
                        }
                    }

                    res = jer_kaps_search_generic(unit, j, key, is_matched, prefix_len_array, search_payload, &cmp_rslt);
                    if (res != SOC_SAND_OK) {
                        cli_out("Error: jer_kaps_search_generic(%d)\n", unit);
                        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                    }

                    
                    cli_out("\nVerifying SW search config %d: \n", j);
                    for (i=0; i < search_cfg.valid_tables_num; i++) {
                        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, search_payload[i], &search_payload_32[i]));
                    }
                    SOCDNX_IF_ERR_EXIT(jer_pp_kaps_verify_search_results(unit, is_matched, expected_is_matched, search_payload_32, expected_search_payload, prefix_len_array, expected_prefix_len_array, search_cfg.valid_tables_num));

                    cli_out("\nVerifying HW search config %d: \n", j);
                    for (i=0; i < search_cfg.valid_tables_num; i++) {
                        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, cmp_rslt.assoc_data[i], &search_payload_32[i]));
                    }
                    SOCDNX_IF_ERR_EXIT(jer_pp_kaps_verify_search_results(unit, cmp_rslt.hit_or_miss, expected_is_matched, search_payload_32, expected_search_payload, NULL , NULL, search_cfg.valid_tables_num));

                }
            }

            if (delete_entries == 0) {
                break;
            }
        }
    } while (loop_test);

    if (SOC_IS_JERICHO_PLUS(unit) && cache_test && desc_dma_init_state == 0) {
        SOCDNX_IF_ERR_EXIT(jer_sbusdma_desc_deinit(unit));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


#else 
soc_error_t soc_jer_pp_diag_kaps_lkup_info_get(
   SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id
   )
{
    SOCDNX_INIT_FUNC_DEFS;
    cli_out("KAPS unsupported\n");
    SOCDNX_FUNC_RETURN
}

#endif 
