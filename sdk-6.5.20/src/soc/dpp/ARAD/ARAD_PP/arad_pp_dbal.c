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
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#include <shared/l3.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#if defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#endif
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>
#endif



#define DBAL_MAX_QUALIFIER_LENGTH           32
#define DBAL_KBP_INVALID_TABLE_ID 			SAL_UINT32_MAX


#define DBAL_KEY_SIZE_IN_BITS               80
#define DBAL_KEY_C_ARAD_SIZE_IN_BITS        160
#define DBAL_KEY_TWO_BUFFERS_SIZE_IN_BITS   (2*DBAL_KEY_SIZE_IN_BITS)
#define DBAL_KEY_THREE_BUFFERS_SIZE_IN_BITS (3*DBAL_KEY_SIZE_IN_BITS)
#define DBAL_KEY_FOUR_BUFFERS_SIZE_IN_BITS  (4*DBAL_KEY_SIZE_IN_BITS)

#define DBAL_TABLE_ID_VALID(__table_id)            (__table_id < SOC_DPP_DBAL_SW_NOF_TABLES)
#define DBAL_CHECK_TABLE_ID_EXIT_WITH_ERR(__table_id) \
    do { \
        if (!DBAL_TABLE_ID_VALID(__table_id)) { \
		    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid table id\n"))); \
        } \
	} while (0)

#define DBAL_DIAG_ENTRY_MANAGE_MODE_VALID(__mode)        (__mode >= 0 && __mode < SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_NUM_OF_MODE)

typedef enum
{
    SOC_DPP_DBAL_CE_SEARCH_MODE_USE_FIRST_16_BIT_FIRST,
    SOC_DPP_DBAL_CE_SEARCH_MODE_FIRST_FREE
}SOC_DPP_DBAL_CE_SEARCH_MODE;


#define DBAL_TABLE_INFO_UPDATE(table_info, db_prefix,db_prefix_len, physical_db, nof_qualifiers, additional_info, qual_info, table_name)\
                                table_info.db_prefix        = db_prefix;\
                                table_info.db_prefix_len    = db_prefix_len;\
                                table_info.physical_db_type = physical_db;\
                                table_info.nof_qualifiers   = nof_qualifiers;\
                                table_info.additional_table_info = additional_info;\
                                sal_memcpy(&(table_info.qual_info[0]), &qual_info[0], sizeof(SOC_DPP_DBAL_QUAL_INFO)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);\
                                sal_strncpy(&(table_info.table_name[0]), table_name, sal_strlen(table_name));



#if defined(INCLUDE_KBP)
typedef struct
{
    JER_KAPS_DB           *private_table_p;
    uint8                  private_db_id;
    struct kbp_entry_iter *private_table_iterator;
    JER_KAPS_DB           *public_table_p;
    uint8                  public_db_id;
    struct kbp_entry_iter *public_table_iterator;
} DBAL_KAPS_ITERATOR;

typedef struct
{
    uint32 table_id;
    struct kbp_db 			*db_p;
    struct kbp_entry_iter 	*iter_p;
} DBAL_KBP_ITERATOR;
#endif 

#define DBAL_LEM_ITERATOR_BLOCK_SIZE 130
typedef struct
{
    uint32                       current_index;
    uint32                       block_index;
    uint32                       last_nof_entries;
    
    ARAD_PP_LEM_ACCESS_KEY      *read_keys;
    ARAD_PP_LEM_ACCESS_PAYLOAD  *read_vals;
    
    ARAD_PP_LEM_ACCESS_KEY       key;
    ARAD_PP_LEM_ACCESS_KEY       key_mask;
    SOC_SAND_TABLE_BLOCK_RANGE   block_range;
} DBAL_LEM_ITERATOR;



STATIC uint32 arad_pp_dbal_next_available_ce_get(int unit, int qual_nof_bits, uint8 use_32_ce, uint32 last_unchecked_ce, SOC_PPC_FP_DATABASE_STAGE stage, uint32 ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE mode, int is_msb, ARAD_PP_KEY_CE_ID* free_ce);
STATIC uint32 arad_pp_dbal_entry_key_to_lem_buffer(int unit, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, ARAD_PP_LEM_ACCESS_KEY *key);
STATIC uint32 arad_pp_dbal_lem_buffer_to_entry_key(int unit, ARAD_PP_LEM_ACCESS_KEY *key, uint32 table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]);
STATIC uint32 arad_pp_dbal_entry_delete_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL  qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success);
uint32 arad_pp_dbal_table_lem_delete_from_shadow(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_table_lem_delete_by_iterator(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
STATIC uint32 arad_pp_dbal_iterator_deinit_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
STATIC uint32 arad_pp_dbal_iterator_init_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
STATIC uint32 arad_pp_dbal_iterator_get_next_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 flags, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], ARAD_PP_LEM_ACCESS_PAYLOAD* payload, uint32* priority, uint8* hit_bit, uint8* found);
STATIC uint32 arad_pp_dbal_next_available_tcam_db_id_get(int unit, uint32* tcam_db_id);
STATIC uint32 arad_pp_dbal_next_available_lem_app_id_get(int unit, uint32* app_id);
STATIC uint32 arad_pp_dbal_next_available_key_get(int unit, SOC_PPC_FP_DATABASE_STAGE stage, uint32 prog_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES db_type, SOC_DPP_DBAL_PROGRAM_KEYS* key_id);
STATIC uint32 arad_pp_dbal_next_available_table_id_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS* table_id);
STATIC uint32 arad_pp_dbal_sem_entry_key_build(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE]);
STATIC uint32 arad_pp_dbal_sem_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES  physical_db_type, uint8 prefix, uint8 prefix_len, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
STATIC uint32 arad_pp_dbal_sem_next_available_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8* prefix);
STATIC uint32 arad_pp_dbal_sem_prefix_usage_get(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, uint8 db_prefix, int* count);
STATIC uint32 arad_pp_dbal_tcam_table_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_DATABASE_STAGE stage);
#if defined(INCLUDE_KBP)
STATIC uint32 arad_pp_dbal_kbp_table_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_DATABASE_STAGE stage, int lookup_number, int program_id);
#endif
STATIC uint32 arad_pp_dbal_diag_input_parse(int unit, const char *input, uint32 payload_nof_words, uint32 *payload_value, uint32 *input_nof_words);
STATIC uint32 arad_pp_dbal_entry_get_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
STATIC uint32 arad_pp_dbal_tcam_prefix_to_table_id(int unit, int tcam_db_ndx, SOC_DPP_DBAL_SW_TABLE_IDS* table_id);
uint32 arad_pp_dbal_entry_lem_buffer_to_key(int unit, uint32 table_id, uint32 buffer[ARAD_PP_FP_TCAM_ENTRY_SIZE], SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]);

#define DBAL_GENERAL_FUNCTIONS

uint32
    arad_pp_dbal_init(int unit)
{
    uint8 is_allocated;
    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_WARM_BOOT(unit)){
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.is_allocated(unit, &is_allocated));
        if(!is_allocated) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.alloc(unit));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_deinit(int unit)
{
    return 0;
}
#undef DBAL_GENERAL_FUNCTIONS





#define DBAL_SERVICE_FUNCTIONS


uint32
    arad_pp_dbal_sem_entry_key_build(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE])
{
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  db_prefix, db_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.db_prefix.get(unit, table_id, &db_prefix));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.db_prefix_len.get(unit, table_id, &db_prefix_len));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, 0, qual_vals, entry_key, entry_mask ));
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_set_any_field(&db_prefix, ARAD_PP_ISEM_ACCESS_LSB_PREFIX, db_prefix_len, entry_key));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_db_predfix_get(int unit, uint32 table_id, uint32* db_prefix)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(table.is_table_initiated == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_db_predfix_get - table not initiated table_id %d"), table_id));
    }

    (*db_prefix) = table.db_prefix;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_ce_info_get(int unit, uint32 table_id, SOC_PPC_FP_DATABASE_STAGE stage, ARAD_PMF_CE *ce_array, uint8 *nof_ces, uint8 *is_key_320b, uint8 *ces_ids)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*nof_ces) = table.nof_qualifiers;
        (*is_key_320b) = 0;

        for (i = 0; i < table.nof_qualifiers; i++) {
            ces_ids[i] = i;
            ce_array[table.nof_qualifiers - 1 - i].is_used = 1;
            ce_array[table.nof_qualifiers - 1 - i].qual_type = table.qual_info[i].qual_type;
            ce_array[table.nof_qualifiers - 1 - i].msb       = table.qual_info[i].qual_nof_bits-1;
            ce_array[table.nof_qualifiers - 1 - i].msb_padding = 0;
            ce_array[table.nof_qualifiers - 1 - i].qual_lsb  = table.qual_info[i].qual_offset;
            if ((table.qual_info[i].qual_is_in_hdr) && (table.qual_info[i].qual_full_size > 32)) {
                ce_array[table.nof_qualifiers - 1 - i].qual_lsb = (table.qual_info[i].qual_full_size-1 ) - table.qual_info[i].qual_offset - (table.qual_info[i].qual_nof_bits-1);
            }
            
            
        }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_table_physical_db_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type )
{

    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - table not initiated")));
    }

    (*physical_db_type) = table.physical_db_type;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_table_is_initiated(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, int* is_table_initiated)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    *is_table_initiated = table.is_table_initiated;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_next_available_ce_get(int unit, int qual_nof_bits, uint8 use_ce_32, uint32 last_unchecked_ce, SOC_PPC_FP_DATABASE_STAGE stage, uint32 ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE mode, int is_msb, ARAD_PP_KEY_CE_ID* free_ce)
{
    ARAD_PP_KEY_CE_ID ce_ndx;
    uint32 ce_rsrc_lcl[1];
    int low_limit = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (is_msb) {
        low_limit = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
    }

    *ce_rsrc_lcl = ce_rsrc;
    for (ce_ndx = last_unchecked_ce; (int)ce_ndx >= low_limit; ce_ndx--) {
        

        if ((!SHR_BITGET(ce_rsrc_lcl ,ce_ndx))) {
            
            if (qual_nof_bits > 16 || use_ce_32) {
                if ((arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx))){
                    (*free_ce) = ce_ndx;
                    break; 
                }
            }else if ((!arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx)) || (mode == SOC_DPP_DBAL_CE_SEARCH_MODE_FIRST_FREE)) {
                    (*free_ce) = ce_ndx;
                    break; 
                }
        }
    }

    if((int)ce_ndx < low_limit) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Error - no available CE! last_unchecked_ce = %d"
                                                              " ce_rsrc = %u, is_msb = %d, qual_nof_bits = %d "), last_unchecked_ce, ce_rsrc, is_msb, qual_nof_bits));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_qualifier_full_size_get(int unit, SOC_PPC_FP_DATABASE_STAGE stage, SOC_PPC_FP_QUAL_TYPE qual_type, uint8* qual_full_size, uint8* qual_is_in_hdr)
{
    uint8                           found = FALSE;
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO irpp_qual_info;
    ARAD_PMF_CE_HEADER_QUAL_INFO    header_qual_info;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_internal_field_info_find(unit, qual_type, stage, 0, &found, &irpp_qual_info));

    if (!found) {
        
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_header_info_find(unit, qual_type, stage, &found, &header_qual_info));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid Qualifier")));
        }else {
            (*qual_full_size) = (header_qual_info.lsb - header_qual_info.msb + 1);
            (*qual_is_in_hdr) = 1;
        }
    }else{
        (*qual_full_size) = irpp_qual_info.info.qual_nof_bits;
        (*qual_is_in_hdr) = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_next_available_tcam_db_id_get(int unit, uint32* tcam_db_id)
{
    uint8 is_in_use;
    uint32 curr_tcam_db_id;

    SOCDNX_INIT_FUNC_DEFS;

    (*tcam_db_id) = 0xFFFFFFFF;

    for(curr_tcam_db_id = ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_BASE; curr_tcam_db_id < ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_END; curr_tcam_db_id++){

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, curr_tcam_db_id, &is_in_use ));
        if (!is_in_use) {
            (*tcam_db_id) = curr_tcam_db_id;
            break;
        }
    }

    if ((*tcam_db_id) == 0xFFFFFFFF) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_next_available_tcam_db_id_get - tcam_db_id available ")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_next_available_lem_app_id_get(int unit, uint32* app_id)
{
    uint32 curr_app_id;
    uint32 prefix = ARAD_PP_FLP_MAP_PROG_NOT_SET;

    SOCDNX_INIT_FUNC_DEFS;

    (*app_id) = 0xFFFFFFFF;

    for(curr_app_id = ARAD_PP_LEM_ACCESS_KEY_TYPE_DBAL_BASE; curr_app_id <= ARAD_PP_LEM_ACCESS_KEY_TYPE_DBAL_END; curr_app_id++){

        arad_pp_lem_access_app_to_prefix_get(unit, curr_app_id, &prefix);
        if (prefix == ARAD_PP_FLP_MAP_PROG_NOT_SET) {
            (*app_id) = curr_app_id;
            break;
        }
    }

    if ((*app_id) == 0xFFFFFFFF) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_next_available_lem_app_id_get - no app_id available ")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_next_available_key_get(int unit, SOC_PPC_FP_DATABASE_STAGE stage, uint32 prog_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES db_type, SOC_DPP_DBAL_PROGRAM_KEYS* key_id)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    SOC_DPP_DBAL_PROGRAM_KEYS curr_key;

    SOCDNX_INIT_FUNC_DEFS;

    (*key_id) = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;

    if (stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR cannot allocate key dynamiclly to VTT stage")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl));


    for (curr_key = SOC_DPP_DBAL_PROGRAM_KEY_A; curr_key < ARAD_PMF_LOW_LEVEL_NOF_KEYS; curr_key++) {

        if (!SOC_IS_JERICHO(unit) && db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) {
            curr_key = SOC_DPP_DBAL_PROGRAM_KEY_C; 
        }
        if (flp_lookups_tbl.elk_lkp_valid) {
            if (flp_lookups_tbl.elk_key_a_valid_bytes) {
                break;
            }
        }
        if (flp_lookups_tbl.lem_1st_lkp_valid) {
            if (flp_lookups_tbl.lem_1st_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.lem_2nd_lkp_valid) {
            if (flp_lookups_tbl.lem_2nd_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.lpm_1st_lkp_valid) {
            if (flp_lookups_tbl.lpm_1st_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.lpm_2nd_lkp_valid) {
            if (flp_lookups_tbl.lpm_2nd_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.tcam_lkp_db_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID) {
            if ((curr_key == SOC_DPP_DBAL_PROGRAM_KEY_A) || (curr_key == SOC_DPP_DBAL_PROGRAM_KEY_B)) {
                if(flp_lookups_tbl.tcam_lkp_key_select == 0){
                    continue;
                }
            }else{
                if(flp_lookups_tbl.tcam_lkp_key_select == ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL){
                    continue;
                }
            }
        }

        (*key_id) = curr_key;
        break;
    }

    if ((*key_id) == SOC_DPP_DBAL_PROGRAM_NOF_KEYS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR no available key found for lookup ")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_next_available_table_id_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS* table_id)
{
    uint32 curr_table_id;
    int is_table_initiated;

    SOCDNX_INIT_FUNC_DEFS;

    (*table_id) = SOC_DPP_DBAL_SW_TABLE_ID_INVALID;

    for(curr_table_id = SOC_DPP_DBAL_SW_TABLE_DYNAMIC_BASE_ID; curr_table_id <= SOC_DPP_DBAL_SW_TABLE_DYNAMIC_END_ID; curr_table_id++){

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, curr_table_id, &is_table_initiated));
        if (is_table_initiated == 0) {
            (*table_id) = curr_table_id;
            break;
        }
    }

    if ((*table_id) == SOC_DPP_DBAL_SW_TABLE_ID_INVALID) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_next_available_table_id_get - no available tables ")));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_sem_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES  physical_db_type, uint8 prefix, uint8 prefix_len, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_SW_TABLE_IDS curr_table_id;
    int i;
    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; 
    int max_prefix_value = ((1 << max_prefix_len) - 1);

    SOCDNX_INIT_FUNC_DEFS;

    if (prefix > max_prefix_value) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_sem_prefix_allocate - prefix out of range %d"), prefix));
    }

    prefix = prefix << (max_prefix_len - prefix_len);

    for(i=0; i < (2^(max_prefix_len - prefix_len)); i++, prefix++)
    {
        switch(physical_db_type){
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.get(unit, (int)prefix, &curr_table_id ));
            if((curr_table_id != 0) && (curr_table_id != table_id)){
                
                
            }
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.set(unit, (int)prefix, table_id ));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.get(unit, (int)prefix, &curr_table_id ));
            if((curr_table_id != 0) && (curr_table_id != table_id)){
                
                
            }
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.set(unit, (int)prefix, table_id ));
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_sem_prefix_allocate - physcal DB error %d"), physical_db_type));
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_sem_next_available_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8* prefix)
{
    SOC_DPP_DBAL_SW_TABLE_IDS curr_table_id = 0;
    int curr_prefix = 0;
    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; 
    int max_prefix_value = ((1 << max_prefix_len) - 1);

    SOCDNX_INIT_FUNC_DEFS;

    (*prefix) = 0xFF;

    for (curr_prefix = 0; curr_prefix <=  max_prefix_value; curr_prefix++) {
        if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.get(unit, curr_prefix, &curr_table_id ));
        } else{
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.get(unit, curr_prefix, &curr_table_id ));
        }

        if (curr_table_id == 0) {
            (*prefix) = curr_prefix;
            break;
        }
    }

    if((*prefix) == 0xFF){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_sem_next_available_prefix_allocate - no available prefixes")));
    }

    if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.set(unit, (int)(*prefix), table_id ));
    }else{
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.set(unit, (int)(*prefix), table_id ));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_sem_prefix_usage_get(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, uint8 db_prefix, int* count)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    (*count) = 0;

    for (i = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST; i <= SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST; i++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));
        if (table.is_table_initiated) {
            if (table.physical_db_type == physical_db_type) {
                if (db_prefix == table.db_prefix) {
                    (*count)++;
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_tcam_table_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_DATABASE_STAGE stage)
{
    ARAD_TCAM_ACCESS_INFO tcam_access_info;
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, key_size = 0;
    SOC_SAND_SUCCESS_FAILURE success;
    uint32 tcam_db_id = 0;
    uint8 is_valid = 0;
    int entry_key_size = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);


    switch (table.additional_table_info) {

    case SOC_DPP_DBAL_ATI_NONE: 
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_tcam_db_id_get(unit, &tcam_db_id));
        table.db_prefix = tcam_db_id;
        table.additional_table_info = SOC_DPP_DBAL_ATI_TCAM_STATIC;
        break;

    case SOC_DPP_DBAL_ATI_TCAM_DUMMY: 
        break;

    case SOC_DPP_DBAL_ATI_TCAM_STATIC: 

        if ((table.db_prefix == ARAD_TCAM_MAX_NOF_LISTS) || (table.db_prefix == ARAD_TCAM_DB_ID_INVALID) || (table.db_prefix > ARAD_PP_ISEM_ACCESS_TCAM_END)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid tcam_db_id for table_id = %s (%d) ATI = %d table.db_prefix = %d\n"), table.table_name, table_id, table.additional_table_info, table.db_prefix));
        }

        tcam_db_id = table.db_prefix;
        SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_id, &is_valid));
        

    break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid ATI for table_id = %d ATI = %d\n"), table_id, table.additional_table_info));
        break;
    }

    
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

    if ((is_valid) || (table.additional_table_info == SOC_DPP_DBAL_ATI_TCAM_DUMMY)) {
        
        goto exit;
    }

    
    tcam_access_info.is_direct          = FALSE;
    tcam_access_info.action_bitmap_ndx  = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
    tcam_access_info.min_banks          = ARAD_PP_ISEM_ACCESS_TCAM_DEF_MIN_BANKS; 
    tcam_access_info.user_data          = tcam_db_id;
    tcam_access_info.callback           = arad_pp_fp_tcam_callback; 

    
    for(i = 0; i < table.nof_qualifiers; i++) {
        key_size += table.qual_info[i].qual_nof_bits;
    }

    if(key_size <= 80) {
        tcam_access_info.entry_size         = ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS;
        entry_key_size = ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_80;
    } else {
        tcam_access_info.entry_size         = ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS;
        entry_key_size = ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_160;
    }

    
    tcam_access_info.prefix_size =
        ((entry_key_size - key_size) < ARAD_PP_ISEM_ACCESS_TCAM_DEF_PREFIX_SIZE)
        ? (entry_key_size - key_size) : ARAD_PP_ISEM_ACCESS_TCAM_DEF_PREFIX_SIZE;

    switch (stage) {

    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
        tcam_access_info.bank_owner         = ARAD_TCAM_BANK_OWNER_VT;
        break;

    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
        tcam_access_info.bank_owner         = ARAD_TCAM_BANK_OWNER_TT;
        break;

    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
        tcam_access_info.action_bitmap_ndx  = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
        tcam_access_info.bank_owner         = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_tcam_table_init - ilegal stage")));
        break;
    }

    SOCDNX_IF_ERR_EXIT(arad_tcam_access_create_unsafe( unit, tcam_db_id, &tcam_access_info, &success));

    if(success != SOC_SAND_SUCCESS){

        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_tcam_table_init - arad_tcam_access_create_unsafe failed")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_kbp_table_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_DATABASE_STAGE stage, int lookup_number, int program_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    switch (table.additional_table_info) {
    case SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION:
        SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_update_master_key(unit,program_id,table.qual_info,table.nof_qualifiers));
        break;
    case SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY:
    case SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE:
        SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_update_lookup(unit,table.db_prefix,program_id,lookup_number,table.qual_info,table.nof_qualifiers));
        break;
    case SOC_DPP_DBAL_ATI_KBP_NO_DB_UPDATE:
    case SOC_DPP_DBAL_ATI_KBP_FLP_CE_ALLOC_ONLY:
        break;
    default:
        SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_update_master_key(unit,program_id,table.qual_info,table.nof_qualifiers));
        SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_update_lookup(unit,table.db_prefix,program_id,lookup_number,table.qual_info,table.nof_qualifiers));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#undef DBAL_SERVICE_FUNCTIONS


#define DBAL_TABLE_FUNCTIONS




uint32
    arad_pp_dbal_table_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 db_prefix, uint32 db_prefix_len, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db,
                              int nof_qualifiers, uint32 additional_info, SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 app_id = 0; 
    uint32 prefix; 
    uint8 sem_prefix;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    sal_memset(&table, 0x0, sizeof(SOC_DPP_DBAL_TABLE_INFO));

    if(table.is_table_initiated != 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_create - trying to init table more than once table_id %d"), table_id));
    }

    
    if (nof_qualifiers > SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("nof_qualifiers out of range")));
    }

    if (strlen(table_name) >= DBAL_MAX_NAME_LENGTH) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("DABL table name length is out of range")));
    }

    switch (physical_db) {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            if ((additional_info == SOC_DPP_DBAL_ATI_NONE) || (additional_info == SOC_DPP_DBAL_ATI_TCAM_DUMMY)){
                db_prefix = DBAL_PREFIX_NOT_DEFINED;
            }

            if ((additional_info == SOC_DPP_DBAL_ATI_TCAM_STATIC) && (db_prefix == ARAD_TCAM_DB_ID_INVALID)){
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR tcam create table %d %s"), table_id, table_name));}

            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            if (db_prefix == DBAL_PREFIX_NOT_DEFINED) {
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_next_available_prefix_allocate(unit, physical_db, table_id, &sem_prefix));
                additional_info = SOC_DPP_DBAL_ATI_NONE;
                db_prefix = (sem_prefix);
            }else{
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_allocate(unit, physical_db, db_prefix, db_prefix_len, table_id));
            }
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            if ((additional_info == SOC_DPP_DBAL_ATI_NONE) && (db_prefix == DBAL_PREFIX_NOT_DEFINED)) {
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_lem_app_id_get(unit, &app_id));
                SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_prefix_alloc(unit,0, app_id, ARAD_PP_LEM_ACCESS_PREFIX_NUM_1, &prefix));

                db_prefix = app_id;
                additional_info = app_id;
                db_prefix_len = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
            }

            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
            if (db_prefix == DBAL_PREFIX_NOT_DEFINED) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("must give valid db_prefix")));
            }
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
            if (db_prefix == DBAL_PREFIX_NOT_DEFINED) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("must give valid db_prefix")));
            }
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_stage_create - illegal physical type")));
    }

    if(table.is_table_initiated == 0){
        DBAL_TABLE_INFO_UPDATE(table, db_prefix, db_prefix_len, physical_db, nof_qualifiers, additional_info, qual_info, table_name);
        table.is_table_initiated = 1;
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));
    }else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_create - trying to init table more than once table_id %d"), table_id));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_dynamic_table_create(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db, int nof_qualifiers,
                                  SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name, SOC_DPP_DBAL_SW_TABLE_IDS* table_id)
{
    uint32 additional_info, db_prefix, db_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_table_id_get(unit, table_id));

    db_prefix = DBAL_PREFIX_NOT_DEFINED;

    switch (physical_db) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        additional_info = SOC_DPP_DBAL_ATI_NONE; 
        db_prefix_len = 0;
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        additional_info = SOC_DPP_DBAL_ATI_NONE; 
        db_prefix_len = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        additional_info = SOC_DPP_DBAL_ATI_NONE; 
        db_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX;
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_dynamic_table_create, physical db type not supported %d"), physical_db));
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, (*table_id), db_prefix, db_prefix_len, physical_db, nof_qualifiers, additional_info,
                                                  qual_info, table_name));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_table_stage_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_DATABASE_STAGE stage, int lookup_number, int program_id )
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int curr_qual_id;
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_stage_create - table not initiated %d"), table_id));
    }

    for (curr_qual_id = 0; curr_qual_id < table.nof_qualifiers; curr_qual_id++){

        qualifier_info = &(table.qual_info[curr_qual_id]);

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_full_size_get(unit, stage, qualifier_info->qual_type, &(qualifier_info->qual_full_size), &(qualifier_info->qual_is_in_hdr)));
        if (qualifier_info->qual_nof_bits == 0) {
            qualifier_info->qual_nof_bits = qualifier_info->qual_full_size;
        }
    }

    
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

    if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tcam_table_init(unit, table_id, stage));
    }
#if defined(INCLUDE_KBP)
    if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP)) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_kbp_table_init(unit, table_id, stage, lookup_number, program_id));
    }
#endif
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_table_actions_set(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_ACTION_TYPE action[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX])
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_actions_set - table not initiated %d"), table_id));
    }

    for (i = 0; i < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; i++) {
        if (action[i] == SOC_PPC_FP_ACTION_TYPE_INVALID) {
            break;
        }
        table.action[i] = action[i];
        table.nof_actions++;
    }

    
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

exit:
    SOCDNX_FUNC_RETURN;
}



#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_table_clear_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_table_clear(unit, table_id));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
arad_pp_dbal_table_clear_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
  ARAD_PP_LEM_ACCESS_KEY              key;
  ARAD_PP_LEM_ACCESS_KEY              key_mask;
  SOC_SAND_TABLE_BLOCK_RANGE          block_range;
  SOC_DPP_DBAL_TABLE_INFO             table;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION  action;
  uint32                              nof_entries;
  int i;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);


  if(SOC_DPP_IS_LEM_SIM_ENABLE(unit)){
     SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_lem_delete_by_iterator(unit, table_id));
  }else{
      key.nof_params = table.nof_qualifiers;
      for (i = 0; i < table.nof_qualifiers; i++) {
          key.param[i].nof_bits = table.qual_info[i].qual_nof_bits;
      }

      key.type = table.additional_table_info; 

      key_mask.type = table.additional_table_info;
      key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
      key_mask.prefix.nof_bits = table.db_prefix_len;

      
      action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

      SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&key,&key_mask,NULL,&block_range,&action, TRUE,&nof_entries));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_dbal_table_clear_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
  uint32 data_indx;
  SOC_SAND_HASH_TABLE_PTR hash_tbl;
  SOC_SAND_HASH_TABLE_ITER iter;
  uint8 stream[ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES] = {0};
  SOC_DPP_DBAL_TABLE_INFO table;
  ARAD_TCAM_ENTRY  entry;
  uint32 db_prefix;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

  SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.vtt.isem_key_to_entry_id.get(unit, &hash_tbl));

  SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);
  SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_get_next(unit, hash_tbl, &iter, stream, &data_indx));

  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
      sal_memcpy(&(entry.value[0]), &(stream[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
      sal_memcpy(&(entry.mask[0]), &(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]),  ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
      sal_memcpy(&(db_prefix), &(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]),  ARAD_PP_SW_DB_HASH_KEY_TABLE_PART_LEN_BYTES);

    if (table.db_prefix == db_prefix)
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_remove_by_index(unit, hash_tbl, data_indx));
        SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_remove_unsafe(unit, table.db_prefix, data_indx));
    }

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_get_next(unit,hash_tbl,&iter,stream,&data_indx));
  }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_dbal_table_clear_isem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    uint32                          nof_entries = 0;
    uint32                          entry_key_array[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    uint32                          data_out[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    uint32                          mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_PPC_FP_QUAL_VAL             qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_TABLE_BLOCK_RANGE      block_range;
    SOC_SAND_SUCCESS_FAILURE        success;
    SOC_DPP_DBAL_TABLE_INFO         table;
    int                             i,
                                    j,
                                    base = ARAD_CHIP_SIM_ISEM_A_BASE,
                                    key = ARAD_CHIP_SIM_ISEM_A_KEY,
                                    payload = ARAD_CHIP_SIM_ISEM_A_PAYLOAD;

    int                             is_table_mode = 0;
    uint32                          prefix, logical_prefix = 0xffff;

    SOCDNX_INIT_FUNC_DEFS;

    if(!soc_property_get(unit, spn_EXACT_MATCH_TABLES_SHADOW_ENABLE, 0)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("exact match shadowing must be enabled for this operation")));
    }

    if ( (table_id != SOC_DPP_DBAL_SW_NOF_TABLES ) && (table_id != SOC_DPP_DBAL_SW_TABLE_ID_INVALID)) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
        is_table_mode = 1;

         if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B)) {
            base = ARAD_CHIP_SIM_ISEM_B_BASE;
            key = ARAD_CHIP_SIM_ISEM_B_KEY;
            payload = ARAD_CHIP_SIM_ISEM_B_PAYLOAD;
         }
    }

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    if (is_table_mode) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_app_to_prefix_get(unit,table.db_prefix, &logical_prefix));
    }

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))){

        SOCDNX_SAND_IF_ERR_EXIT(chip_sim_em_get_block(unit, base, key, payload,
                                NULL, entry_key_array, data_out, &nof_entries, &block_range));

        for (i = 0; i < nof_entries; i++) {
            for(j = 0; j < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; j++) {
                SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[j]);
            }

            soc_sand_bitstream_get_any_field(&(entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]),
                                                SOC_DPP_DEFS_GET(unit, lem_width) - SOC_DPP_DEFS_GET(unit, nof_lem_prefixes),
                                                SOC_DPP_DEFS_GET(unit, nof_lem_prefixes), &prefix);

            if ((!is_table_mode) || (prefix == logical_prefix)) {
                SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit,
                                                                            SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT,
                                                                            table_id,
                                                                            ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY,
                                                                            0, 
                                                                            1, 
                                                                            0,
                                                                            qual_vals,
                                                                            &(entry_key_array[ i * SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]),
                                                                            mask));

                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
                SOC_SAND_IF_FAIL_RETURN(success);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_dbal_table_clear(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear_lem(unit, table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear_tcam(unit, table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - KBP not supported")));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear_isem(unit, table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear_isem(unit, table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear_kaps(unit, table_id));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - illegal physical DB type")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_table_lem_delete_by_iterator(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;
    uint8 hit_bit;
    uint8 found;
    SOC_SAND_SUCCESS_FAILURE success;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++i) {
        SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[i]);
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_init_lem(unit, table_id));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_get_next_lem(unit, table_id, 0, qual_vals, &payload, NULL, &hit_bit, &found));

    while (found) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit,table_id,qual_vals, &success));

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_get_next_lem(unit, table_id, 0, qual_vals, &payload, NULL, &hit_bit, &found));
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit(unit, table_id));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_table_print_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO *table = NULL;
    uint32 nof_entries = 0, prefix, logical_prefix = 0xffff;
    uint32 entry_key_array[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    uint32 data_out[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    int i,j, actual_nof_entries = 0;
    int is_table_mode = 0;
    uint32 start_bit = SOC_DPP_DEFS_GET(unit, lem_width) - SOC_DPP_DEFS_GET(unit, nof_lem_prefixes);
    uint32 nof_bits = SOC_DPP_DEFS_GET(unit, nof_lem_prefixes);
    uint32 lem_prefix = 0xffff;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_DPP_IS_LEM_SIM_ENABLE(unit)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("exact match shadowing must be enabled for this diagnostics")));
    }

    if (table_id != SOC_DPP_DBAL_SW_TABLE_ID_INVALID) {
        SOCDNX_ALLOC(table, SOC_DPP_DBAL_TABLE_INFO, 1, "arad_pp_dbal_table_print_lem.table");
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, table));
        is_table_mode = 1;
    }

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    if (is_table_mode) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_app_to_prefix_get(unit,table->db_prefix, &logical_prefix));
        LOG_CLI((BSL_META("\nPrinting all entries for table %s\n"), table->table_name));
        LOG_CLI((BSL_META("\tTable basic information:\n")));
        LOG_CLI((BSL_META("\tPrefix = %d, Total entries in table = %d\n"), logical_prefix, table->nof_entries_added_to_table ));
    }else{
        LOG_CLI((BSL_META("\nPrinting all entries for LEM:\n\n")));
    }

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))){

        SOCDNX_SAND_IF_ERR_EXIT(chip_sim_em_get_block(unit, ARAD_CHIP_SIM_LEM_BASE,  ARAD_CHIP_SIM_LEM_KEY, ARAD_CHIP_SIM_LEM_PAYLOAD,
                                NULL, entry_key_array, data_out, &nof_entries, &block_range));

        for (i = 0; i < nof_entries; i++) {

            prefix = 0;
            if((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long == 64)
                && (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM)) {
                nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV6_64_IN_LEM;
                start_bit = SOC_DPP_DEFS_GET(unit, lem_width) - nof_bits;
                lem_prefix = logical_prefix >> (SOC_DPP_DEFS_GET(unit, nof_lem_prefixes) - nof_bits);
            }
            soc_sand_bitstream_get_any_field(&(entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]),
              start_bit, nof_bits, &prefix);

            if ((!is_table_mode) || (prefix == logical_prefix)
                || ((prefix == lem_prefix) && (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM)
                     && (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long == 64))) {
                actual_nof_entries++;
                LOG_CLI((BSL_META("\n  Entry %2d: "), actual_nof_entries));

                if (!is_table_mode) {
                    LOG_CLI((BSL_META("Prefix=%2d,"), prefix));
                    LOG_CLI((BSL_META("  Key (include prefix)=0x")));
                    for (j = SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S - 1; j >= 0; j--) {
                        LOG_CLI((BSL_META("%08x "), entry_key_array[ j + i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]));
                    }
                }else{
                    arad_pp_dbal_entry_dump(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, table_id, NULL, &(entry_key_array[ i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]) );
                }

                LOG_CLI((BSL_META("  Payload=0x")));
                for (j = ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S-1; j >= 0; j--) {
                    LOG_CLI((BSL_META("%08x "), data_out[j + i*ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]));
                }
            }
        }
    }

    LOG_CLI((BSL_META("\n")));

exit:
    SOCDNX_FREE(table);
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_em_dump(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES db_type)
{
    uint32 nof_entries = 0, vsi_db_id = 0;
    uint32 entry_key_array[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    uint32 data_out[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    int i,j, actual_nof_entries = 0;
    int base, key, payload, key_size;
    _bcm_dpp_sw_resources_outlif_t outlif_sw_res = { 0 };
    ARAD_PP_ESEM_KEY        esem_key = {0};
    ARAD_PP_ESEM_ENTRY      esem_entry = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload= {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY     oem2_key= {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload= {0};

    SOCDNX_INIT_FUNC_DEFS;

    if(!soc_property_get(unit, spn_EXACT_MATCH_TABLES_SHADOW_ENABLE, 0)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("exact match shadowing must be enabled for this diagnostics")));
    }

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    LOG_CLI((BSL_META("\nPrinting all entries for %s:\n\n"), arad_pp_dbal_physical_db_to_string(db_type)));

    switch (db_type) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_ESEM:
        base = ARAD_CHIP_SIM_ESEM_BASE;
        key = ARAD_CHIP_SIM_ESEM_KEY;
        payload = ARAD_CHIP_SIM_ESEM_PAYLOAD;
        key_size = ARAD_PP_ESEM_KEY_SIZE;
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM1:
        base = ARAD_CHIP_SIM_OEMA_BASE;
        key = ARAD_CHIP_SIM_OEMA_KEY;
        payload = ARAD_CHIP_SIM_OEMA_PAYLOAD;
        key_size = ARAD_CHIP_SIM_OEMA_KEY;
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM2:
        base = ARAD_CHIP_SIM_OEMB_BASE;
        key = ARAD_CHIP_SIM_OEMB_KEY;
        payload = ARAD_CHIP_SIM_OEMB_PAYLOAD;
        key_size = ARAD_CHIP_SIM_OEMB_KEY;
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_RMAP:
        base = ARAD_CHIP_SIM_RMAPEM_BASE;
        key = ARAD_CHIP_SIM_RMAPEM_KEY;
        payload = ARAD_CHIP_SIM_RMAPEM_PAYLOAD;
        key_size = ARAD_CHIP_SIM_RMAPEM_KEY;
        break;

     case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_GLEM:
        base = ARAD_CHIP_SIM_GLEM_BASE;
        key = ARAD_CHIP_SIM_GLEM_KEY;
        payload = ARAD_CHIP_SIM_GLEM_PAYLOAD;
        key_size = ARAD_CHIP_SIM_GLEM_KEY;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_em_dump ilegal db_type %d"), db_type));
        break;
    }

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))){

        SOCDNX_SAND_IF_ERR_EXIT(chip_sim_em_get_block(unit, base, key, payload,
                                NULL, entry_key_array, data_out, &nof_entries, &block_range));

        for (i = 0; i < nof_entries; i++) {
            uint32* data_out_for_oam;

            actual_nof_entries++;
            LOG_CLI((BSL_META("\n  Entry %3d: "), actual_nof_entries));
            for (j = 0; j < key_size; j++) {
                LOG_CLI((BSL_META("%08x "), entry_key_array[ (SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S - j - 2) + i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]));
            }

            LOG_CLI((BSL_META("  Payload=0x")));
                LOG_CLI((BSL_META("%08x "), data_out[ i*ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]));

            LOG_CLI((BSL_META("\n")));

            switch (db_type) {
            case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_ESEM:
                
                SOCDNX_SAND_IF_ERR_EXIT(arad_pp_esem_entry_from_buffer(unit,&(data_out[ i*ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]), &esem_entry));

                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.gport_mgmt.outlif_info.get(unit, ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, esem_entry.out_ac), &outlif_sw_res));
                SOCDNX_SAND_IF_ERR_EXIT(arad_pp_esem_buffer_to_key(unit, (outlif_sw_res.lif_type == _bcmDppLifTypeNativeVlan), &(entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]), &esem_key, &vsi_db_id));

                if (vsi_db_id == ARAD_PP_ESEM_KEY_VSI_DB_ID_VAL) {
                    LOG_CLI((BSL_META_U(unit,
                                    "\tESEM key: VD: %d, VSI: %d \n"), esem_key.key_info.vd_vsi.vd, esem_key.key_info.vd_vsi.vsi));
                } else if (outlif_sw_res.lif_type != _bcmDppLifTypeNativeVlan) {
                    LOG_CLI((BSL_META_U(unit,
                                    "\tESEM key: VD: %d, VSI: %d, CV_ID: %d\n"), esem_key.key_info.vd_cvid.vd, esem_key.key_info.vd_cvid.vsi, esem_key.key_info.vd_cvid.cvid));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                    "\tESEM key: VSI: %d, OUTLIF: %d\n"), esem_key.key_info.lif_vsi.vsi, esem_key.key_info.lif_vsi.lif));
                }

                LOG_CLI((BSL_META_U(unit,"\tESEM payload: OUT_AC: %d\n"), esem_entry.out_ac));
                break;

            case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM1:
                _ARAD_PP_OAM_OEM1_KEY_TO_KEY_STRUCT(oem1_key, entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]);
                _ARAD_PP_OAM_PRINT_OEM1_KEY(oem1_key);

                data_out_for_oam = &(data_out[ i*ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]);
                _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(data_out_for_oam, oem1_payload);
                _ARAD_PP_OAM_PRINT_OEM1_PAYLOAD(oem1_payload);
                break;

            case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM2:
                _ARAD_PP_OAM_OEM2_KEY_TO_KEY_STRUCT(oem2_key, entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]);
                LOG_CLI((BSL_META_U(unit,
                                    "\tOEMB key: ingress: %d, MDL: %d, OAM LIF: 0x%x, your disc: %d\n"),  oem2_key.ingress, oem2_key.mdl,  oem2_key.oam_lif, oem2_key.your_disc));

                _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(data_out[ i*ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S], oem2_payload);
                LOG_CLI((BSL_META_U(unit,"\t\tOEMB payload: MP profile: 0x%x,OAM ID: 0x%x\n"), oem2_payload.mp_profile, oem2_payload.oam_id));
                break;

            default:

                break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_dbal_table_print_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    uint32 data_indx;
    SOC_SAND_HASH_TABLE_PTR hash_tbl;
    SOC_SAND_HASH_TABLE_ITER iter;
    uint8 stream[ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES] = {0};
    SOC_DPP_DBAL_TABLE_INFO table;
    ARAD_TCAM_ENTRY  entry;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    ARAD_TCAM_ACTION payload;
    uint32 priority;
    uint8 hit_bit;
    uint8 found;
    uint32 db_prefix;
    SOC_DPP_DBAL_SW_TABLE_IDS entry_tbl_id=0;
    int i, is_table_mode = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if ( (table_id != SOC_DPP_DBAL_SW_NOF_TABLES ) && (table_id != SOC_DPP_DBAL_SW_TABLE_ID_INVALID)) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
        is_table_mode = 1;
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.vtt.isem_key_to_entry_id.get(unit, &hash_tbl));

    if (is_table_mode) {
        LOG_CLI((BSL_META("\nPrinting all entries for table %s\n\n"), table.table_name));
    }else{
        LOG_CLI((BSL_META("\nPrinting all entries for TCAM:\n\n")));
    }


    SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_get_next(unit, hash_tbl, &iter, stream, &data_indx));

    i = 0;
    while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
    {
        sal_memcpy(&(entry.value[0]), &(stream[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
        sal_memcpy(&(entry.mask[0]), &(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]),  ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
        sal_memcpy(&(db_prefix), &(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]),  ARAD_PP_SW_DB_HASH_KEY_TABLE_PART_LEN_BYTES);

        if(!is_table_mode){
            arad_pp_dbal_tcam_prefix_to_table_id(unit, db_prefix, &entry_tbl_id);
            if (entry_tbl_id != 0) {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, entry_tbl_id, &table));
                if ((table.is_table_initiated) && (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM)) {
                    table_id = entry_tbl_id;
                }else{
                    LOG_CLI((BSL_META("unknown prefix received by hash %d\n"), entry_tbl_id));
                }
            }else{
               LOG_CLI((BSL_META("entry found, DBAL not recognize type!!!!\n")));
               for (i = 0; i < 44; i++) {
                   LOG_CLI((BSL_META("%x "), stream[i]));
               }
               LOG_CLI((BSL_META("\n")));
               i = 0;
            }
        }else{
            if (table.db_prefix == db_prefix) {
                entry_tbl_id = table_id;
            }
        }

        if (table_id == entry_tbl_id)
        {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_id_tcam(unit, table_id, data_indx, qual_vals, &payload,  &priority, &hit_bit, &found));
            if (found) {
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_dump(unit, table.table_programs[0].stage, table_id, qual_vals, NULL) );
                LOG_CLI((BSL_META(" hit_bit=%d, priority=%u"), hit_bit, priority));

                LOG_CLI((BSL_META("  Payload=0x")));
                for (i = 0; i < ARAD_TCAM_ACTION_MAX_LEN-1; i++) {
                    LOG_CLI((BSL_META("%08x "), payload.value[i]));
                }
                LOG_CLI((BSL_META(" DBAL table=%d"), table_id));
                LOG_CLI((BSL_META("\n")));
            }else{
                LOG_CLI((BSL_META("entry NOT found\n")));
            }
        }else{
            if (!is_table_mode) {
                LOG_CLI((BSL_META("ERROR - entry found unrecognized to DBAL")));
            }
        }
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_get_next(unit,hash_tbl,&iter,stream,&data_indx));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_table_print_sem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO *table = NULL;
    uint32 nof_entries = 0, prefix;
    uint32 entry_key_array[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    uint32 data_out[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    int i,j;
    int base = ARAD_CHIP_SIM_ISEM_A_BASE, key = ARAD_CHIP_SIM_ISEM_A_KEY, payload = ARAD_CHIP_SIM_ISEM_A_PAYLOAD;
    int is_table_mode = 0, actual_nof_entries = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if(!soc_property_get(unit, spn_EXACT_MATCH_TABLES_SHADOW_ENABLE, 0)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("exact match shadowing must be enabled for this diagnostics")));
    }

    if ( (table_id != SOC_DPP_DBAL_SW_NOF_TABLES ) && (table_id != SOC_DPP_DBAL_SW_TABLE_ID_INVALID)) {
        SOCDNX_ALLOC(table, SOC_DPP_DBAL_TABLE_INFO, 1, "arad_pp_dbal_table_print_sem.table");
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, table));
        is_table_mode = 1;

         if ((table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B)) {
            base = ARAD_CHIP_SIM_ISEM_B_BASE;
            key = ARAD_CHIP_SIM_ISEM_B_KEY;
            payload = ARAD_CHIP_SIM_ISEM_B_PAYLOAD;
         }
    }

    if ((table_id == SOC_DPP_DBAL_SW_NOF_TABLES)) {
            base = ARAD_CHIP_SIM_ISEM_B_BASE;
            key = ARAD_CHIP_SIM_ISEM_B_KEY;
            payload = ARAD_CHIP_SIM_ISEM_B_PAYLOAD;
    }

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    if (is_table_mode) {
        LOG_CLI((BSL_META("\nPrinting all entries for table %s\n"), table->table_name));
        LOG_CLI((BSL_META("\tTable basic information:\n")));
        LOG_CLI((BSL_META("\tPrefix = %d, Total entries in table = %d\n"), table->db_prefix, table->nof_entries_added_to_table ));
    }else{
        LOG_CLI((BSL_META("\nPrinting all entries in SEM %c\n"), base==ARAD_CHIP_SIM_ISEM_A_BASE ? 'A':'B' ));
    }

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))){

        SOCDNX_SAND_IF_ERR_EXIT(chip_sim_em_get_block(unit, base, key, payload,
                                NULL, entry_key_array, data_out, &nof_entries, &block_range));

        for (i = 0; i < nof_entries; i++) {

            prefix = 0;
            soc_sand_bitstream_get_any_field(&entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S], ARAD_PP_ISEM_ACCESS_LSB_PREFIX, ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX, &prefix);

            if ((!is_table_mode) || (prefix == table->db_prefix)) {
                actual_nof_entries++;
                LOG_CLI((BSL_META("\n  Entry %2d: "), actual_nof_entries));

                if (!is_table_mode) {
                    LOG_CLI((BSL_META("Prefix=%2d,"), prefix));
                    LOG_CLI((BSL_META("  Key (include prefix)=0x")));
                    for (j = 0; j < ARAD_PP_ISEM_ACCESS_KEY_SIZE; j++) {
                        LOG_CLI((BSL_META("%08x "), entry_key_array[ (SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S - j - 2) + i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]));
                    }
                }else{
                    arad_pp_dbal_entry_dump(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT, table_id, NULL, &(entry_key_array[ i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]) );
                }

                LOG_CLI((BSL_META("  Payload=")));
                LOG_CLI((BSL_META("0x%08x "), data_out[ i*ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]));
            }
        }
    }
    LOG_CLI((BSL_META("\n")));
exit:
    SOCDNX_FREE(table);
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_table_print(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_CHECK_TABLE_ID_EXIT_WITH_ERR(table_id);

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_print - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_print_lem(unit,table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_print_tcam(unit,table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_print - KBP not supported")));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_print - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_print_sem(unit,table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_print_sem(unit,table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(jer_kaps_show_table(unit, table_id, 1));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_print - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_print - illegal physical DB type")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_block_get_tcam(int unit, SOC_PPC_IP_ROUTING_TABLE_RANGE *block_range_key, SOC_DPP_DBAL_SW_TABLE_IDS table_id,
                           SOC_PPC_FP_QUAL_VAL *qual_vals_array, void* payload,  uint32 *nof_entries)
{
    uint8  is_vrf_mode = 0;
    SOC_DPP_DBAL_SW_TABLE_IDS entry_tbl_id;
    uint32 data_indx, priority, db_prefix;
    int entry_index = -1;
    int vrf_qual_index = 0;
    SOC_SAND_HASH_TABLE_PTR  hash_tbl;
    SOC_SAND_HASH_TABLE_ITER iter;
    uint8 stream[ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES] = {0};
    uint8 hit_bit, found;
    ARAD_TCAM_ENTRY  entry;
    uint32 vrf_ndx = 0;
    ARAD_TCAM_ACTION tcam_payload = {0};
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    if(qual_vals_array[0].type == SOC_PPC_FP_QUAL_IRPP_VRF){
        vrf_ndx = qual_vals_array[0].val.arr[0];
        is_vrf_mode = 1;
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.vtt.isem_key_to_entry_id.get(unit, &hash_tbl));

    SOC_PPC_FP_QUAL_VAL_clear(qual_vals_array);
    SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);

    
    do {
        ++entry_index;
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_get_next(unit,hash_tbl,&iter,stream,&data_indx));
    }
    while ( !SOC_SAND_HASH_TABLE_ITER_IS_END(&iter) && (entry_index < block_range_key->start.payload.arr[0])); 

    do {
        if (SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)){
            break;
        }

        sal_memcpy(&(entry.value[0]), &(stream[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
        sal_memcpy(&(entry.mask[0]), &(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]),  ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
        sal_memcpy(&(db_prefix), &(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]),  ARAD_PP_SW_DB_HASH_KEY_TABLE_PART_LEN_BYTES);

        if(table.db_prefix == db_prefix){
            entry_tbl_id = table_id;
        }else{
            entry_tbl_id = -1;
        }

        if (table_id == entry_tbl_id)
        {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_id_tcam(unit, table_id, data_indx, qual_vals_array + (*nof_entries)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX, &tcam_payload,  &priority, &hit_bit, &found));
            if (found) {
                if (is_vrf_mode) {
                    
                    vrf_qual_index = 0;
                    while (qual_vals_array[(*nof_entries)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + vrf_qual_index].type != SOC_PPC_FP_QUAL_IRPP_VRF) {
                        vrf_qual_index++;
                        if (vrf_qual_index == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                            break;
                        }
                    }

                    
                    if ((vrf_qual_index == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX)){
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("no VRF in Table!!! %d"), table_id));
                    }
                }

                
                if ((is_vrf_mode == 0) || (qual_vals_array[(*nof_entries)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + vrf_qual_index].val.arr[0] == vrf_ndx)){
                    (*((uint32 *)payload + (*nof_entries))) = tcam_payload.value[0];
                    (*nof_entries)++;
                }
            }
        }

        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_get_next(unit, hash_tbl, &iter, stream, &data_indx));
        entry_index++;

    } while (((*nof_entries) < block_range_key->entries_to_act) && (entry_index < block_range_key->start.payload.arr[0] + block_range_key->entries_to_scan));


    if((SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)) || ((*nof_entries) < block_range_key->entries_to_act)){
        block_range_key->start.payload.arr[0] = SOC_SAND_UINT_MAX;
        block_range_key->start.payload.arr[1] = SOC_SAND_UINT_MAX;
    }
    else {
        block_range_key->start.payload.arr[0] = entry_index;
        block_range_key->start.payload.arr[1] = vrf_ndx;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

uint32
    arad_pp_dbal_block_get_kaps(int unit, SOC_PPC_IP_ROUTING_TABLE_RANGE *block_range_key, SOC_DPP_DBAL_SW_TABLE_IDS table_id,
                           SOC_PPC_FP_QUAL_VAL *qual_vals_array, void* payload,  uint32 *nof_entries)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    uint32 vrf_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    
    if (qual_vals_array[0].type != SOC_PPC_FP_QUAL_IRPP_VRF) {
        vrf_ndx = -1;
    }else{
        vrf_ndx = qual_vals_array[0].val.arr[0];
    }

    SOC_PPC_FP_QUAL_VAL_clear(qual_vals_array);

    SOCDNX_IF_ERR_EXIT(jer_pp_kaps_entry_get_block_hw(unit, table_id, vrf_ndx, &table, block_range_key, qual_vals_array, payload, nof_entries));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif 

uint32
    arad_pp_dbal_block_get(int unit, SOC_PPC_IP_ROUTING_TABLE_RANGE *block_range_key, SOC_DPP_DBAL_SW_TABLE_IDS table_id,
                           SOC_PPC_FP_QUAL_VAL *qual_vals_array, void* payload,  uint32 *nof_entries)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - LEM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_block_get_tcam(unit, block_range_key, table_id, qual_vals_array, payload,  nof_entries));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - KBP not supported")));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - SEM_A not in use")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - SEM_B not in use")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_block_get_kaps(unit, block_range_key, table_id, qual_vals_array, payload,  nof_entries));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - illegal physical DB type")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_table_destroy(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{

    SOC_DPP_DBAL_TABLE_INFO table;
    int count = 0;
    uint32 lem_prefix = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_disassociate_all(unit, table_id));

    switch (table.physical_db_type) {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_access_destroy_unsafe(unit, table.db_prefix));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, table.physical_db_type, table.db_prefix, &count));
            if (count > 1) {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.set(unit, table.db_prefix, 0));
            }
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, table.physical_db_type, table.db_prefix, &count));
            if (count > 1) {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.set(unit, table.db_prefix, 0));
            }
            break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_app_to_prefix_get( unit, table.db_prefix, &lem_prefix));
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_prefix_dealloc(unit, lem_prefix));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_destroy - illegal physical type")));
    }

    sal_memset(&table, 0x0, sizeof(SOC_DPP_DBAL_TABLE_INFO));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_table_disassociate_all(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{

    SOC_DPP_DBAL_TABLE_INFO table;
    int i, prog_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    for (i = 0; i < table.nof_table_programs; i++) {
        
        prog_index = (table.nof_table_programs - i) - 1;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_table_disassociate(unit, table.table_programs[prog_index].program_id,
                                                                        table.table_programs[prog_index].stage, table_id));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_dbal_table_kaps_hitbit_set(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 enable) {
    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &dbal_table));

    if (dbal_table.additional_table_info == enable) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error: Table %s hitbits already set to enable=%d.\n"), dbal_table.table_name, enable));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear(unit, table_id));

    dbal_table.additional_table_info = enable;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &dbal_table));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_dbal_table_kaps_hitbit_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 *enable) {
    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &dbal_table));

    *enable = dbal_table.additional_table_info;

exit:
    SOCDNX_FUNC_RETURN;
}
#undef DBAL_TABLE_FUNCTIONS
#define DBAL_PROGRAM_FUNCTIONS



uint32
    arad_pp_dbal_qualifier_to_instruction(int unit, SOC_PPC_FP_DATABASE_STAGE stage, uint8 is_32b_ce, uint32 nof_bits, SOC_PPC_FP_QUAL_TYPE qual_type, int is_msb, uint32* ce_instr_encoding)
{
    uint8                           found = FALSE;
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO irpp_qual_info;
    uint8 is_ce_in_msb = 0;
    uint32 qual_lsb = 0; 
    uint32 lost_bits = 0; 
    uint32 ce_offset = 0, fld_val = 0, nof_bits_fields = 0, hw_buffer_jericho = 0, nof_bits_in_instr_encoded_lsb,  offset_in_instr_encoded_msb;

    SOCDNX_INIT_FUNC_DEFS;

    (*ce_instr_encoding) = 0;

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_internal_field_info_find(unit, qual_type, stage, is_msb, &found, &irpp_qual_info));

    if (found) {

        if (nof_bits == 0) {
            nof_bits = irpp_qual_info.info.qual_nof_bits;
        }

        if (nof_bits > ((is_32b_ce)? ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_32B_MAX : ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_16B_MAX)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG(" dbal_qualifier_to_instruction qual_type %s, qual nof_bits = %d, ce_size = %d"),
                                             SOC_PPC_FP_QUAL_TYPE_to_string(qual_type), nof_bits,
                                              (is_32b_ce)? ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_32B_MAX : ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_16B_MAX));
        }

        
        (*ce_instr_encoding) |= SOC_SAND_SET_BITS_RANGE(ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_INTERNAL_FIELD,
                                             ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);

        arad_pmf_ce_internal_field_offset_compute(unit, qual_type, stage, is_ce_in_msb, nof_bits, qual_lsb, lost_bits,
                is_32b_ce, &found, &ce_offset, &nof_bits_fields, &hw_buffer_jericho);

        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("dbal_qualifier_to_instruction Stage %s, qual_type %s, in msb %s, Number of bits %d, Qual lsb %d,"
                                                                  " Lost bits %d, is 32-bits Ce %s.\n\r"),SOC_PPC_FP_DATABASE_STAGE_to_string(stage),
                                                  SOC_PPC_FP_QUAL_TYPE_to_string(qual_type), is_ce_in_msb ? "TRUE" : "FALSE",
                                                  nof_bits, qual_lsb, lost_bits, is_32b_ce ? "TRUE" : "FALSE" ));
        }

        
        fld_val = SOC_IS_JERICHO(unit)? (hw_buffer_jericho << ARAD_PMF_CE_HW_BUFFER_SUB_HEADER_BIT): 0;
        (*ce_instr_encoding) |= SOC_SAND_SET_BITS_RANGE(fld_val, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB);
        fld_val = nof_bits_fields - 1;
        nof_bits_in_instr_encoded_lsb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32 : ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16;
        (*ce_instr_encoding) |= SOC_SAND_SET_BITS_RANGE(fld_val, ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB, nof_bits_in_instr_encoded_lsb);
        offset_in_instr_encoded_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
        (*ce_instr_encoding) |= SOC_SAND_SET_BITS_RANGE(ce_offset, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);

    }else {
        
        
    }

    if (!found) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid Qualifier")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_key_inst_set(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage, int key_id, SOC_DPP_DBAL_QUAL_INFO qual_info, int is_msb, int ce_id)
{
    uint8                           found = FALSE;
    ARAD_PMF_CE_PACKET_HEADER_INFO  ce_packet_header_info;
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO irpp_qual_info;
    ARAD_PMF_CE_HEADER_QUAL_INFO    header_qual_info;

    SOCDNX_INIT_FUNC_DEFS;

    if (is_msb) { 
        ce_id -= (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1);
    }

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_internal_field_info_find(unit, qual_info.qual_type, stage, is_msb, &found, &irpp_qual_info));

    if (found) {
    
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_internal_info_entry_set_unsafe(
                unit,
                stage,
                program_id,
                key_id,
                ce_id, 
                is_msb,
                0, 
                0, 
                qual_info.qual_offset, 
                0, 
                qual_info.qual_nof_bits,
                qual_info.qual_type
            ));

    }else {

        
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_header_info_find(unit, qual_info.qual_type, stage, &found, &header_qual_info));

        if (found) {
          ARAD_PMF_CE_PACKET_HEADER_INFO_clear(&ce_packet_header_info);
          ce_packet_header_info.sub_header = header_qual_info.header_ndx_0;
		  ce_packet_header_info.offset = header_qual_info.msb + qual_info.qual_offset;

		  
          ce_packet_header_info.nof_bits = qual_info.qual_nof_bits;
          SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_packet_header_entry_set_unsafe(unit, stage, program_id, key_id, ce_id, is_msb, 0, &ce_packet_header_info));
        }
    }

    if (!found) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid Qualifier")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_flp_hw_based_key_enable(int unit, int program_id, SOC_DPP_HW_KEY_LOOKUP hw_key_based_lookup_enable)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, program_id, &flp_lookups_tbl));

    switch(hw_key_based_lookup_enable ) {

    case SOC_DPP_HW_KEY_LOOKUP_IN_LEM_1ST:
        if ((flp_lookups_tbl.lem_1st_lkp_valid == 1) && (flp_lookups_tbl.lem_1st_lkp_key_select != ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("trying to reconfigure value lem1")));
            
        }
        flp_lookups_tbl.lem_1st_lkp_valid      = 1;
        flp_lookups_tbl.lem_1st_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
        flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
        flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
        break;

    case SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND:
        if ((flp_lookups_tbl.lem_2nd_lkp_valid == 1) && (flp_lookups_tbl.lem_2nd_lkp_key_select != ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("trying to reconfigure value lem2")));
            
        }
        flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
        flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
        flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
        flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
        break;

    case SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY:
        flp_lookups_tbl.learn_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
        break;

    case SOC_DPP_HW_KEY_LOOKUP_DISABLED:
    default:
        break;
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, program_id, &flp_lookups_tbl));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_flp_lookup_update(int unit, DBAL_PROGRAM_INFO table_program, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 is_to_remove, uint8* lookup_used, int is_msb)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    SOC_DPP_DBAL_TABLE_INFO table;
    int max_prefix_len = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS; 
    int max_prefix_value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
    uint32 lem_prefix = 0;
    uint32 access_profile_id;
    int disable_lpm_2nd_private_lookup = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, table_program.program_id, &flp_lookups_tbl));

    if (table_program.lookup_number == 0xFF ) {
        
        table_program.lookup_number = 1;
        if (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM) {
            
            if(flp_lookups_tbl.lem_1st_lkp_valid == 0){
                table_program.lookup_number = 1;
            } else if(flp_lookups_tbl.lem_2nd_lkp_valid == 0 ){
                table_program.lookup_number = 2;
            } else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("no available lookup number")));
            }
        } else if(table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM){
            if(flp_lookups_tbl.tcam_lkp_db_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID){
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("no available lookups for TCAM")));
            }
        }
    }

    if (table_program.lookup_number == DBAL_KAPS_2ND_LKP_DIASBLE) {
        disable_lpm_2nd_private_lookup = 1;
        table_program.lookup_number = 2;
    }

    if(table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP ){
        if(table_program.lookup_number > 5){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number %d"), table_program.lookup_number));
        }
    }else if (table_program.lookup_number > 2 ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number %d"), table_program.lookup_number));
    }

    if (table_program.lookup_number == 0) {

        if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM) || (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number for LEM/KAPS")));
        }
    }

    (*lookup_used) = table_program.lookup_number;

    switch(table.physical_db_type){
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        arad_pp_lem_access_app_to_prefix_get(unit,table.db_prefix, &lem_prefix);
        if((table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM)
            && SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long == 64) {
                
                lem_prefix = lem_prefix >> (max_prefix_len - table.db_prefix_len);
            }
        if (table_program.lookup_number == 1) {
            flp_lookups_tbl.lem_1st_lkp_valid      = 1;
            if(is_to_remove){
                flp_lookups_tbl.lem_1st_lkp_valid      = 0;
            }
            flp_lookups_tbl.lem_1st_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lem_1st_lkp_and_value  = max_prefix_value >> table.db_prefix_len;
            flp_lookups_tbl.lem_1st_lkp_or_value   = lem_prefix << (max_prefix_len - table.db_prefix_len);
        } else {
            flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
            if(is_to_remove){
                flp_lookups_tbl.lem_2nd_lkp_valid      = 0;
            }
            flp_lookups_tbl.lem_2nd_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lem_2nd_lkp_and_value  = max_prefix_value >> table.db_prefix_len;
            flp_lookups_tbl.lem_2nd_lkp_or_value   = lem_prefix << (max_prefix_len - table.db_prefix_len);
        }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
        if (table_program.lookup_number == 1) {
            flp_lookups_tbl.lpm_1st_lkp_valid      = 1;
            flp_lookups_tbl.Lpm_1stLkpType = 1;
            if(is_to_remove){
                flp_lookups_tbl.lpm_1st_lkp_valid      = 0;
                flp_lookups_tbl.Lpm_1stLkpType = 0;
            }
            flp_lookups_tbl.lpm_1st_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lpm_1st_lkp_and_value  = max_prefix_value >> table.db_prefix_len;
            flp_lookups_tbl.lpm_1st_lkp_or_value   = table.db_prefix << (max_prefix_len - table.db_prefix_len);
#if defined(INCLUDE_KBP)
            if((table_program.public_lpm_lookup_size != 0) && JER_KAPS_ENABLE_PUBLIC_DB(unit)){
                flp_lookups_tbl.lpm_public_1st_lkp_valid = 1;
                flp_lookups_tbl.LpmPublic_1stLkpType = 1;
                if(is_to_remove){
                    flp_lookups_tbl.lpm_public_1st_lkp_valid = 0;
                    flp_lookups_tbl.LpmPublic_1stLkpType = 0;
                }
                if (table_program.public_lpm_lookup_size != SOC_DPP_DBAL_USE_COMPLETE_KEY) {
                    flp_lookups_tbl.lpm_public_1st_lkp_key_select = 4; 
                    flp_lookups_tbl.lpm_public_1st_lkp_key_size   = table_program.public_lpm_lookup_size;
                } else {
                    flp_lookups_tbl.lpm_public_1st_lkp_key_select = table_program.key_id;
                    flp_lookups_tbl.lpm_public_1st_lkp_key_size   = 0;
                }

                flp_lookups_tbl.lpm_public_1st_lkp_and_value = max_prefix_value >> table.db_prefix_len;
                flp_lookups_tbl.lpm_public_1st_lkp_or_value  = table.db_prefix << (max_prefix_len - table.db_prefix_len);
            }
#endif
        } else {
            flp_lookups_tbl.lpm_2nd_lkp_valid      = 1;
            flp_lookups_tbl.Lpm_2ndLkpType = 1;
            if(is_to_remove){
                flp_lookups_tbl.lpm_2nd_lkp_valid      = 0;
                flp_lookups_tbl.Lpm_2ndLkpType = 0;
            }

            if (disable_lpm_2nd_private_lookup) {
                flp_lookups_tbl.lpm_2nd_lkp_valid      = 0;
            }

            flp_lookups_tbl.lpm_2nd_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lpm_2nd_lkp_and_value  = max_prefix_value >> table.db_prefix_len;
            flp_lookups_tbl.lpm_2nd_lkp_or_value   = table.db_prefix << (max_prefix_len - table.db_prefix_len);

#if defined(INCLUDE_KBP)
            if((table_program.public_lpm_lookup_size != 0)  && JER_KAPS_ENABLE_PUBLIC_DB(unit)){
                flp_lookups_tbl.lpm_public_2nd_lkp_valid = 1;
                flp_lookups_tbl.LpmPublic_2ndLkpType = 1;
                if(is_to_remove){
                    flp_lookups_tbl.lpm_public_2nd_lkp_valid = 0;
                    flp_lookups_tbl.LpmPublic_2ndLkpType = 0;
                }
                if (table_program.public_lpm_lookup_size != SOC_DPP_DBAL_USE_COMPLETE_KEY) {
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_select = 4; 
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_size   = table_program.public_lpm_lookup_size;
                } else {
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_select = table_program.key_id;
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_size   = 0;
                }

                flp_lookups_tbl.lpm_public_2nd_lkp_and_value = max_prefix_value >> table.db_prefix_len;
                flp_lookups_tbl.lpm_public_2nd_lkp_or_value  = table.db_prefix << (max_prefix_len - table.db_prefix_len);
            }
#endif
        }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
	        
            if((is_to_remove) || (table.db_prefix == DBAL_PREFIX_NOT_DEFINED)){
	                flp_lookups_tbl.tcam_lkp_db_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID;
	        }else{
                if (SOC_IS_JERICHO(unit)) {
                    flp_lookups_tbl.tcam_lkp_key_select = table_program.key_id;
                }else{
                    if (table_program.key_id == SOC_DPP_DBAL_PROGRAM_KEY_C) {
                        flp_lookups_tbl.tcam_lkp_key_select = ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL;
                    }else {
                        flp_lookups_tbl.tcam_lkp_key_select = 0;
                    }
                }
                
                SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, table.db_prefix, 0, &access_profile_id ));
                flp_lookups_tbl.tcam_lkp_db_profile = access_profile_id;
            }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
        {
            SOC_DPP_DBAL_PROGRAM_KEYS key_used_in_kbp = table_program.key_id;
            int nof_bytes = ((table_program.nof_bits_used_in_key+7)/8);

            if(is_to_remove){
                flp_lookups_tbl.elk_lkp_valid = 0;
                flp_lookups_tbl.elk_wait_for_reply = 0;
                nof_bytes = 0;
            }else{
                flp_lookups_tbl.elk_lkp_valid = 1;
                flp_lookups_tbl.elk_wait_for_reply = 1;
            }

#if defined(INCLUDE_KBP)
            {
                uint8 sw_prog_id=0xFF;
                arad_pp_prog_index_to_flp_app_get(unit,table_program.program_id,&sw_prog_id);
                flp_lookups_tbl.elk_opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(sw_prog_id);
            }
#endif
            if (is_msb && (key_used_in_kbp != SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY)) {
                key_used_in_kbp += SOC_DPP_DBAL_PROGRAM_NOF_COMPLETE_KEYS;
            }
            switch(key_used_in_kbp){
                case SOC_DPP_DBAL_PROGRAM_KEY_A:
                    flp_lookups_tbl.elk_key_a_valid_bytes = nof_bytes;
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_B:
                    flp_lookups_tbl.elk_key_b_valid_bytes = nof_bytes;
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_C:
                    flp_lookups_tbl.elk_key_c_valid_bytes = nof_bytes;
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_D:
                    flp_lookups_tbl.elk_key_d_lsb_valid_bytes = nof_bytes;
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_A_MSB:
                    if (nof_bytes > 10) {
                        flp_lookups_tbl.elk_key_a_valid_bytes = 10;
                        flp_lookups_tbl.elk_key_a_msb_valid_bytes = nof_bytes-10;
                    }
                    else{
                        flp_lookups_tbl.elk_key_a_msb_valid_bytes = nof_bytes;
                    }
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_B_MSB:
                    if (nof_bytes > 10) {
                        flp_lookups_tbl.elk_key_b_valid_bytes = 10;
                        flp_lookups_tbl.elk_key_b_msb_valid_bytes = nof_bytes-10;
                    }
                    else{
                        flp_lookups_tbl.elk_key_b_msb_valid_bytes = nof_bytes;
                    }
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_C_MSB:
                    flp_lookups_tbl.elk_key_c_msb_valid_bytes = nof_bytes;
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_D_MSB:
                    flp_lookups_tbl.elk_key_d_msb_valid_bytes = nof_bytes;
                    break;
                case SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY:
                    
                    flp_lookups_tbl.elk_key_b_valid_bytes = 10;
                    flp_lookups_tbl.elk_key_b_msb_valid_bytes = 10;
                    if (is_msb) {
                        flp_lookups_tbl.elk_key_a_valid_bytes = 10;
                        flp_lookups_tbl.elk_key_a_msb_valid_bytes = nof_bytes-30;
                    }
                    else{
                        flp_lookups_tbl.elk_key_b_valid_bytes = nof_bytes-20;
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid key id")));
            }
        }
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid physical DB TYPE")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, table_program.program_id, &flp_lookups_tbl));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_dbal_vt_lookup_update(int unit, DBAL_PROGRAM_INFO table_program, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 is_to_remove)
{
    ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA ihp_vtt1st_key_construction_tbl_data;
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 access_profile_id;

    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; 
    int max_prefix_value = ((1 << max_prefix_len) - 1);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    soc_sand_os_memset(&ihp_vtt1st_key_construction_tbl_data, 0x0, sizeof(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA));

    if (table_program.lookup_number == 0xFF ) {
        table_program.lookup_number = 0;
    }

    if (table_program.lookup_number > 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number")));
    }

    ihp_vtt1st_key_construction_tbl_data.dbal = 1;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(unit, table_program.program_id, &ihp_vtt1st_key_construction_tbl_data ));

    switch(table.physical_db_type){
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        ihp_vtt1st_key_construction_tbl_data.isa_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt1st_key_construction_tbl_data.isa_lookup_enable = 0;
        }
        ihp_vtt1st_key_construction_tbl_data.isa_and_mask      = max_prefix_value >> table.db_prefix_len; 
        ihp_vtt1st_key_construction_tbl_data.isa_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); 
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        ihp_vtt1st_key_construction_tbl_data.isb_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt1st_key_construction_tbl_data.isb_lookup_enable = 0;
        }
        ihp_vtt1st_key_construction_tbl_data.isb_and_mask      = max_prefix_value >> table.db_prefix_len; 
        ihp_vtt1st_key_construction_tbl_data.isb_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); 
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            if ((is_to_remove) || (table.db_prefix == DBAL_PREFIX_NOT_DEFINED)) {
                ihp_vtt1st_key_construction_tbl_data.tcam_db_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID;
            }else{
                SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, table.db_prefix, 0, &access_profile_id ));
                ihp_vtt1st_key_construction_tbl_data.tcam_db_profile = access_profile_id;
            }
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid physical DB TYPE")));
    }

    ihp_vtt1st_key_construction_tbl_data.dbal = 1;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(unit, table_program.program_id, &ihp_vtt1st_key_construction_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_tt_lookup_update(int unit, DBAL_PROGRAM_INFO table_program, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 is_to_remove)
{
    ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  ihp_vtt2nd_key_construction_tbl_data;
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 access_profile_id;

    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; 
    int max_prefix_value = ((1 << max_prefix_len) - 1);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    soc_sand_os_memset(&ihp_vtt2nd_key_construction_tbl_data, 0x0, sizeof(ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA));

    if (table_program.lookup_number == 0xFF ) {
        table_program.lookup_number = 0;
    }
    if (table_program.lookup_number > 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number")));
    }

    ihp_vtt2nd_key_construction_tbl_data.dbal = 1;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(unit, table_program.program_id, &ihp_vtt2nd_key_construction_tbl_data ));

    switch(table.physical_db_type){
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        ihp_vtt2nd_key_construction_tbl_data.isa_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt2nd_key_construction_tbl_data.isa_lookup_enable = 0;
        }
        ihp_vtt2nd_key_construction_tbl_data.isa_and_mask      = max_prefix_value >> table.db_prefix_len; 
        ihp_vtt2nd_key_construction_tbl_data.isa_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); 
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        ihp_vtt2nd_key_construction_tbl_data.isb_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt2nd_key_construction_tbl_data.isb_lookup_enable = 0;
        }
        ihp_vtt2nd_key_construction_tbl_data.isb_and_mask      = max_prefix_value >> table.db_prefix_len; 
        ihp_vtt2nd_key_construction_tbl_data.isb_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); 
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            if (is_to_remove || (table.db_prefix == DBAL_PREFIX_NOT_DEFINED)) {
            	ihp_vtt2nd_key_construction_tbl_data.tcam_db_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID;
        	}else{
                SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, table.db_prefix, 0, &access_profile_id ));
                ihp_vtt2nd_key_construction_tbl_data.tcam_db_profile = access_profile_id;
            }
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid physical DB TYPE")));
    }

    ihp_vtt2nd_key_construction_tbl_data.dbal = 1;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(unit, table_program.program_id, &ihp_vtt2nd_key_construction_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_dbal_program_to_tables_associate(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage,
                                             SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], int nof_valid_keys)
{
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;
    int                         curr_key_indx, curr_qual_id, curr_table_program, table_id;
    uint32                      ce_rsrc[1];
    ARAD_PP_KEY_CE_ID           ce_ndx = 0;
    uint32                      last_unchecked_ce;
    SOC_DPP_DBAL_TABLE_INFO     table;
    int                         nof_bits_used_in_key;
    int                         is_msb; 
    SOC_DPP_DBAL_PROGRAM_KEYS   curr_key;
    int                         curr_use_32_bit_ce;
    uint8                       kbp_multi_key = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (stage < 0 || stage >= SOC_PPC_NOF_FP_DATABASE_STAGES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid stage %d. \n\rThe range is: 0 - SOC_PPC_NOF_FP_DATABASE_STAGES-1.\n\r"), stage));
    }
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, ce_rsrc));

    for (curr_key_indx = 0; curr_key_indx < nof_valid_keys; curr_key_indx++){

        table_id = keys_to_table_id[curr_key_indx].sw_table_id;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_stage_create(unit, table_id, stage, keys_to_table_id[curr_key_indx].lookup_number, program_id));

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

        if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) &&
            ((table.additional_table_info == SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY)||(table.additional_table_info == SOC_DPP_DBAL_ATI_KBP_NO_DB_UPDATE)||(table.additional_table_info == SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE))) {
			
			table.table_programs[table.nof_table_programs].stage = stage;
			table.table_programs[table.nof_table_programs].program_id = program_id;
			table.table_programs[table.nof_table_programs].key_id = keys_to_table_id[curr_key_indx].key_id;
			table.table_programs[table.nof_table_programs].lookup_number = keys_to_table_id[curr_key_indx].lookup_number;
			table.table_programs[table.nof_table_programs].nof_bits_used_in_key = 0;
			table.table_programs[table.nof_table_programs].public_lpm_lookup_size = 0;

			table.nof_table_programs++;

			SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

            continue; 
        }

        nof_bits_used_in_key = 0;
        last_unchecked_ce = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
        is_msb = FALSE;

        curr_table_program = table.nof_table_programs;
        curr_key = keys_to_table_id[curr_key_indx].key_id;
        if (keys_to_table_id[curr_key_indx].key_id == SOC_DPP_DBAL_PROGRAM_NOF_KEYS) { 
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_key_get(unit,stage, program_id, table.physical_db_type, &curr_key));
            keys_to_table_id[curr_key_indx].key_id = curr_key;
        }else if(keys_to_table_id[curr_key_indx].key_id > SOC_DPP_DBAL_PROGRAM_KEY_D) {
            if ((keys_to_table_id[curr_key_indx].key_id == SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY)
                && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)) {
                if (table.physical_db_type != SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR key SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY can be set only with KBP tables")));
                }
                kbp_multi_key = 1;
                curr_key = SOC_DPP_DBAL_PROGRAM_KEY_B;
            }
            else{
                if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) || (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP)) {
                    curr_key = curr_key - SOC_DPP_DBAL_PROGRAM_NOF_COMPLETE_KEYS;
                    is_msb = TRUE;
                    last_unchecked_ce = (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX*ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB) + 1;
                }else{
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("illegal use of MSB key for DB type")));
                }
            }
        }

        for (curr_qual_id = 0; curr_qual_id < table.nof_qualifiers; curr_qual_id++){

            qualifier_info = &(table.qual_info[curr_qual_id]);
            if (!kbp_multi_key) {
                
                if (nof_bits_used_in_key + qualifier_info->qual_nof_bits > DBAL_KEY_C_ARAD_SIZE_IN_BITS){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Tables associat nof bits is bigger than 160, current nof bits is %d + %d"), nof_bits_used_in_key, qualifier_info->qual_nof_bits));
                }
                if( ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) {
                    if (((nof_bits_used_in_key + qualifier_info->qual_nof_bits) > DBAL_KEY_SIZE_IN_BITS) && (!is_msb)) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Tables associate nof bits for LSB key is bigger than 80 current bits is %d + %d"), nof_bits_used_in_key, qualifier_info->qual_nof_bits));
                    }
                } else if( (curr_key != SOC_DPP_DBAL_PROGRAM_KEY_C) && ((nof_bits_used_in_key + qualifier_info->qual_nof_bits) > DBAL_KEY_SIZE_IN_BITS)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Tables associat nof bits is bigger than 80 for key current bits is %d"), nof_bits_used_in_key + qualifier_info->qual_nof_bits));
                }

                if ((curr_key >= SOC_DPP_DBAL_PROGRAM_KEY_C) && (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Key for Egress stage: key:%d"), curr_key));
                }
            }
            else{
                if (nof_bits_used_in_key + qualifier_info->qual_nof_bits > DBAL_KEY_FOUR_BUFFERS_SIZE_IN_BITS){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Tables associat (KBP multi key) nof bits is bigger than 320b, current nof bits is %d + %d"), nof_bits_used_in_key, qualifier_info->qual_nof_bits));
                }
            }
            if (use_32_bit_ce) {
                curr_use_32_bit_ce = use_32_bit_ce[curr_key_indx][curr_qual_id];
            }else {
                curr_use_32_bit_ce = 0;
            }

            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_ce_get(unit, qualifier_info->qual_nof_bits, curr_use_32_bit_ce, last_unchecked_ce, stage, *ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE_USE_FIRST_16_BIT_FIRST, is_msb, &ce_ndx));

            last_unchecked_ce = ce_ndx - 1;

            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, program_id, stage, curr_key, (*qualifier_info), is_msb, ce_ndx));
            
            
            SHR_BITSET(ce_rsrc, ce_ndx);
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, program_id, 0, *ce_rsrc));
            table.table_programs[curr_table_program].ce_assigned[curr_qual_id] = ce_ndx;
            nof_bits_used_in_key += qualifier_info->qual_nof_bits;
            if ((nof_bits_used_in_key == DBAL_KEY_SIZE_IN_BITS) && (!is_msb)) { 
                if (ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) { 
                    is_msb = TRUE;
                    last_unchecked_ce = (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX*ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB) + 1;
				}
            }
            else if ((kbp_multi_key)&&(nof_bits_used_in_key == DBAL_KEY_TWO_BUFFERS_SIZE_IN_BITS)) {
                
                last_unchecked_ce = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
                is_msb = FALSE;
                curr_key = SOC_DPP_DBAL_PROGRAM_KEY_A;
            }
            else if ((kbp_multi_key)&&(nof_bits_used_in_key == DBAL_KEY_THREE_BUFFERS_SIZE_IN_BITS)) {
                
                is_msb = TRUE;
                last_unchecked_ce = (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX*ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB) + 1;
            }
        }

        
        table.table_programs[curr_table_program].stage = stage;
        table.table_programs[curr_table_program].program_id = program_id;
        table.table_programs[curr_table_program].key_id = curr_key;
        table.table_programs[curr_table_program].lookup_number = keys_to_table_id[curr_key_indx].lookup_number;
        table.table_programs[curr_table_program].nof_bits_used_in_key = nof_bits_used_in_key;
        table.table_programs[curr_table_program].public_lpm_lookup_size = keys_to_table_id[curr_key_indx].public_lpm_lookup_size;
        if (kbp_multi_key) {
            table.table_programs[curr_table_program].key_id = SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY;
        }

        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_lookup_update(unit, table.table_programs[curr_table_program], table_id, 0, &(table.table_programs[curr_table_program].lookup_number), is_msb));
        } else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vt_lookup_update(unit, table.table_programs[curr_table_program], table_id, 0));
        } else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tt_lookup_update(unit, table.table_programs[curr_table_program], table_id, 0));
        }
        table.nof_table_programs++;

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_dbal_program_to_tables_associate_implicit(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage,
                                                      SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],
                                                      uint8 qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                      int nof_valid_keys)
{
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;
    int                         curr_key_indx, curr_qual_id;
    int                         nof_table_programs;
    SOC_DPP_DBAL_TABLE_INFO     table;
    uint32                      ce_rsrc[1];
    ARAD_PP_KEY_CE_ID           ce_ndx;
    int                         is_msb = 0;
    int                         nof_bits_used_in_key = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, ce_rsrc));

    for (curr_key_indx = 0; curr_key_indx < nof_valid_keys; curr_key_indx++){
		nof_bits_used_in_key = 0;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_stage_create(unit, keys_to_table_id[curr_key_indx].sw_table_id, stage, keys_to_table_id[curr_key_indx].lookup_number,program_id));

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, keys_to_table_id[curr_key_indx].sw_table_id, &table));
        nof_table_programs = table.nof_table_programs;

        if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) && (table.additional_table_info == SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY)) {
            continue; 
        }

        for (curr_qual_id = 0; curr_qual_id < table.nof_qualifiers; curr_qual_id++){

            
            qualifier_info = &(table.qual_info[curr_qual_id]);
            ce_ndx = qualifier_to_ce_id[curr_key_indx][curr_qual_id];

            nof_bits_used_in_key += qualifier_info->qual_nof_bits;
            if (ce_ndx > ARAD_PMF_LOW_LEVEL_CE_NDX_MAX) {
                is_msb = 1;
            }

            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_inst_set(unit, program_id, stage, keys_to_table_id[curr_key_indx].key_id, (*qualifier_info), is_msb, ce_ndx));
            SHR_BITSET(ce_rsrc, ce_ndx);
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, program_id, 0, *ce_rsrc));
            table.table_programs[nof_table_programs].ce_assigned[curr_qual_id] = ce_ndx;
        }

        
        table.table_programs[nof_table_programs].stage = stage;
        table.table_programs[nof_table_programs].program_id = program_id;
        table.table_programs[nof_table_programs].key_id = keys_to_table_id[curr_key_indx].key_id;
        table.table_programs[nof_table_programs].lookup_number = keys_to_table_id[curr_key_indx].lookup_number;
        table.table_programs[nof_table_programs].nof_bits_used_in_key = nof_bits_used_in_key;
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_lookup_update(unit, table.table_programs[nof_table_programs], keys_to_table_id[curr_key_indx].sw_table_id, 0, &(table.table_programs[nof_table_programs].lookup_number), is_msb));
        } else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vt_lookup_update(unit, table.table_programs[nof_table_programs], keys_to_table_id[curr_key_indx].sw_table_id, 0));
        } else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tt_lookup_update(unit, table.table_programs[nof_table_programs], keys_to_table_id[curr_key_indx].sw_table_id, 0));
        }
        table.nof_table_programs ++;

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, keys_to_table_id[curr_key_indx].sw_table_id, &table));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_program_table_disassociate(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    int i, is_msb;
    int program_index = -1;
    uint32 ce_rsrc[1];
    SOC_DPP_DBAL_TABLE_INFO table;
    ARAD_PP_KEY_CE_ID       ce_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, ce_rsrc));

    for (i = 0; i < table.nof_table_programs; i++) {
        if(table.table_programs[i].program_id == program_id){
            if (stage == table.table_programs[i].stage) {
                program_index = i;
                break;
            }
        }
    }

    if (program_index == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_program_table_disassociate table %d not associated to program %d"), table_id, program_id));
    }

    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
            uint8 lookup_used;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_lookup_update(unit, table.table_programs[program_index], table_id, 1, &(lookup_used), 0));
        } else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vt_lookup_update(unit, table.table_programs[program_index], table_id, 1));
        } else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tt_lookup_update(unit, table.table_programs[program_index], table_id, 1));
        }

    
    for (i = 0; i < table.nof_qualifiers; i++) {
        is_msb = 0;
        ce_ndx = table.table_programs[program_index].ce_assigned[i];
        SHR_BITCLR(ce_rsrc, ce_ndx);
        if (ce_ndx > ARAD_PMF_LOW_LEVEL_CE_NDX_MAX) {
            is_msb = 1;
            ce_ndx -= (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1);
        }
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_nop_entry_set_unsafe(unit, stage, program_id, table.table_programs[program_index].key_id, ce_ndx, is_msb, 0, TRUE));
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, program_id, 0, *ce_rsrc));

    sal_memset(&(table.table_programs[program_index]), 0x0, sizeof(DBAL_PROGRAM_INFO));

    if (program_index != table.nof_table_programs-1) {
        sal_memcpy(&(table.table_programs[program_index]), &(table.table_programs[table.nof_table_programs-1]), sizeof(DBAL_PROGRAM_INFO));
    }
    table.nof_table_programs--;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

exit:
    SOCDNX_FUNC_RETURN;
}
#undef DBAL_PROGRAM_FUNCTIONS


#define DBAL_ENTRY_MANAGMENT_FUNCTIONS

STATIC uint32
    arad_pp_dbal_entry_payload_to_lem_payload(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, void* payload, ARAD_PP_LEM_ACCESS_PAYLOAD* lem_payload)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO         table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    for (i = 0; i < table.nof_actions; i++) {
        if (table.action[i] == SOC_PPC_FP_ACTION_TYPE_FLP_LEM_1ST_RESULT_0) {

            lem_payload->dest = *(((uint32*)payload)+i);
            lem_payload->flags = ARAD_PP_FWD_DECISION_PARSE_RAW_DATA;
        }

        if (table.action[i] == SOC_PPC_FP_ACTION_TYPE_FLP_LEM_1ST_RESULT_1) {

            lem_payload->asd = *(((uint32*)payload)+i);
            lem_payload->flags = ARAD_PP_FWD_DECISION_PARSE_RAW_DATA;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_add_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL  qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
    ARAD_PP_LEM_ACCESS_ACK_STATUS   ack;
    ARAD_PP_LEM_ACCESS_REQUEST      request;
    ARAD_PP_LEM_ACCESS_PAYLOAD      lem_payload;
    SOC_DPP_DBAL_TABLE_INFO         table;


    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload);

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lem_buffer(unit, qual_vals, table_id, &(request.key)));
    request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;

    
    if (table.nof_actions != 0 ) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_payload_to_lem_payload(unit, table_id, payload, &lem_payload));
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_add_unsafe(unit, &request, &lem_payload, &ack));
    }else{
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_add_unsafe(unit, &request, payload, &ack));
    }

    if(ack.is_success == TRUE){
      *success = SOC_SAND_SUCCESS;
    } else{
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_add_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_payload[SOC_DPP_DEFS_MAX(ISEM_PAYLOAD_NOF_UINT32)];

    SOCDNX_INIT_FUNC_DEFS;

    entry_payload[0] = *((uint32*)payload); 

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, qual_vals, entry_key));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_add_unsafe(unit, entry_key, entry_payload, sem_id, success));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_add_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority,  SOC_SAND_SUCCESS_FAILURE *success)
{
    
    uint32 data_indx, tcam_db_id;
    uint8  stream[ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES] = {0}; 
    SOC_SAND_HASH_TABLE_PTR hash_tbl;
    uint8 found, entry_added;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;

    SOC_DPP_DBAL_TABLE_INFO table;
    uint8 is_in_vtt_stage = 0;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    is_in_vtt_stage = ((table.table_programs[0].stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT) || (table.table_programs[0].stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT));

    
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);

    
    rv = sw_state_access[unit].dpp.soc.arad.pp.vtt.isem_key_to_entry_id.get(unit, &hash_tbl);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, 0, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table.db_prefix), ARAD_PP_SW_DB_HASH_KEY_TABLE_PART_LEN_BYTES);

    if (is_in_vtt_stage) {
        action.value[0] = (*(ARAD_PP_ISEM_ACCESS_ENTRY*)payload).sem_result_ndx;
    } else { 
        action.value[0] = (*(uint32*)payload);
        action.value[1] = (*(((uint32*)payload)+1));
    }

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(unit, hash_tbl, stream, &data_indx, &found));

    if (!found) {
        
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_add(unit, hash_tbl, stream, &data_indx, &entry_added ));
    } else {
        entry_added = TRUE;
    }

    if (entry_added) { 
        tcam_db_id = table.db_prefix;

        if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("TCAM profile not assigned, cannot access TCAM ")));
        }

        
        entry.is_for_update = found;
        entry.valid = TRUE;

        SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_add_unsafe(unit, tcam_db_id, data_indx, FALSE, priority, &entry, &action, success ));

        if ((*success != SOC_SAND_SUCCESS)) {
            if ((!found)) {
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_remove_by_index(unit, hash_tbl, data_indx));
            }
            if ((*success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("ERROR Tcam full %s %d"), table.table_name, table_id));
            }else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR internal tcam entry already found table %s %d"), table.table_name, table_id));
            }
        }

    } else {
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_add_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority,  SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  table_size_in_bytes, table_payload_in_bytes;

    uint8   elk_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];
    uint8   elk_ad_value[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
	if (table.additional_table_info == SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION) {
		SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("illegal additional table info: master key indication in entry add kbp")));
	}

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table.db_prefix, &table_size_in_bytes, &table_payload_in_bytes));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_kbp_route_payload_encode(unit, table.db_prefix, (ARAD_PP_LEM_ACCESS_PAYLOAD*)payload, elk_ad_value));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_tcam_kbp_fwd_route_add(unit, table_id, prefix_len, elk_data, elk_ad_value, success));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_add_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority,  SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  prefix_len;
    uint8   data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint8   ad_bytes[JER_KAPS_AD_BUFFER_NOF_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1, &kaps_table_id));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_encode(unit, payload, ad_bytes));

    SOCDNX_IF_ERR_EXIT(jer_pp_kaps_entry_add_hw(unit, kaps_table_id, prefix_len, table.additional_table_info, data_bytes, ad_bytes, success));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_add(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    uint8 dbal_entry_exist = FALSE;
    ARAD_PP_LEM_ACCESS_PAYLOAD dbal_get_payload;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "refine_dbal_entry_total_num", 0)){
            ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&dbal_get_payload);
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, &dbal_get_payload, &priority, NULL, &dbal_entry_exist));
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_lem(unit, table_id, qual_vals, payload, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_tcam(unit, table_id, qual_vals, payload, priority, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_kbp(unit, table_id, qual_vals, payload, priority, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_A, table_id, qual_vals, payload, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_B, table_id, qual_vals, payload, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_kaps(unit, table_id, qual_vals, payload, priority, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - illegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry added to table %s \n"), table.table_name));
    arad_pp_dbal_entry_dump(unit, table.table_programs[0].stage, table_id, qual_vals, NULL);
    LOG_CLI((BSL_META("\nPayload: %d \n"), *(uint32*)payload));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_input_validate(unit, table_id, qual_vals));
#endif

    if (dbal_entry_exist == FALSE){
        table.nof_entries_added_to_table++;
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, table.nof_entries_added_to_table));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_delete_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, qual_vals, entry_key));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_remove_unsafe(unit, entry_key, sem_id));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_delete_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL  qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    ARAD_PP_LEM_ACCESS_ACK_STATUS      ack;
    ARAD_PP_LEM_ACCESS_REQUEST         request;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lem_buffer(unit, qual_vals, table_id, &(request.key)));

    request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_remove_unsafe(unit, &request, &ack));

    if(ack.is_success == TRUE){
      *success = SOC_SAND_SUCCESS;
    } else{
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_delete_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{

    uint32 data_indx;
    uint8  stream[ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES] = {0}; 
    SOC_SAND_HASH_TABLE_PTR hash_tbl;
    uint8 found;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;

    SOC_DPP_DBAL_TABLE_INFO table;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);

    
    rv = sw_state_access[unit].dpp.soc.arad.pp.vtt.isem_key_to_entry_id.get(unit, &hash_tbl);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, 0, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table.db_prefix), ARAD_PP_SW_DB_HASH_KEY_TABLE_PART_LEN_BYTES);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(unit, hash_tbl, stream, &data_indx, &found));

    if (!found){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry doesn't exists")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_remove_by_index(unit, hash_tbl,data_indx));

    if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("TCAM profile not assigned, cannot access TCAM ")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_remove_unsafe(unit, table.db_prefix, data_indx));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_delete_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    
    uint32  table_size_in_bytes, table_payload_in_bytes;
    uint8   elk_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];
    
    
    
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*success) = SOC_SAND_FAILURE_INTERNAL_ERR;

    

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table.db_prefix, &table_size_in_bytes, &table_payload_in_bytes));

    
    

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_tcam_kbp_fwd_route_remove(unit, table_id, prefix_len, elk_data));
        
    (*success) = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_delete_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  prefix_len;
    uint8   data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*success) = SOC_SAND_FAILURE_INTERNAL_ERR;

    
    if (table.table_iterator != NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("ERROR it is not possible to delete entries in the LPM(KAPS) while traversing them.")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1, &kaps_table_id));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));
    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_remove_hw(unit, kaps_table_id, prefix_len, table.additional_table_info, data_bytes));

    (*success) = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_delete(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_lem(unit, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_tcam(unit, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_kbp(unit, table_id, qual_vals, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_A, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_B, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_kaps(unit, table_id, qual_vals, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete - illegal physical DB type")));
        break;
    }

    table.nof_entries_added_to_table--;
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, table.nof_entries_added_to_table));

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry deleted from table %s \n"),table.table_name));
    arad_pp_dbal_entry_dump(unit, table.table_programs[0].stage, table_id, qual_vals, NULL);
    LOG_CLI((BSL_META("\n")));
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_get_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL  qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint8 *found)
{
    ARAD_PP_LEM_ACCESS_REQUEST         request;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lem_buffer(unit, qual_vals, table_id, &(request.key)));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_by_key_get_unsafe(unit, &request.key, (ARAD_PP_LEM_ACCESS_PAYLOAD*)(payload), found));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_get_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint8 *found)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_payload[SOC_DPP_DEFS_MAX(ISEM_PAYLOAD_NOF_UINT32)];

    SOCDNX_INIT_FUNC_DEFS;

    entry_payload[0] = 0;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, qual_vals, entry_key));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_get_unsafe(unit, entry_key, entry_payload, sem_id, found));

    *((uint32*)payload) = entry_payload[0]; 

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_get_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    uint32 data_indx;
    uint8  stream[ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES] = {0}; 
    SOC_SAND_HASH_TABLE_PTR hash_tbl ;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;

    SOC_DPP_DBAL_TABLE_INFO table;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);

    
    rv = sw_state_access[unit].dpp.soc.arad.pp.vtt.isem_key_to_entry_id.get(unit, &hash_tbl);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, 0, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table.db_prefix), ARAD_PP_SW_DB_HASH_KEY_TABLE_PART_LEN_BYTES);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(unit, hash_tbl, stream, &data_indx, found));

    if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("TCAM profile not assigned, cannot access TCAM ")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_get_unsafe(unit, table.db_prefix, data_indx, TRUE ,  priority, &entry, &action, found, hit_bit ));

    sal_memcpy((uint32*)payload, action.value, ARAD_TCAM_ACTION_MAX_LEN*sizeof(uint32));

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_get_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* found)
{
    uint32  table_size_in_bytes, table_payload_in_bytes;
    uint8   elk_data[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    uint8   elk_ad_value[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*priority) = 0;
    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table.db_prefix, &table_size_in_bytes, &table_payload_in_bytes));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_tcam_kbp_fwd_route_get(unit, table_id, prefix_len, elk_data, elk_ad_value, found));
    if (*found) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_kbp_route_payload_decode(unit,table.db_prefix,0 ,elk_ad_value,(ARAD_PP_LEM_ACCESS_PAYLOAD*)payload));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_get_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8* found)
{
    uint32  prefix_len;
    uint8   data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint8   ad_bytes[JER_KAPS_AD_BUFFER_NOF_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1, &kaps_table_id));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));
    if ((hit_bit != NULL) && (*hit_bit & SOC_DPP_DBAL_HITBIT_FLAG_CLEAR) && (table.additional_table_info == 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR hitbit not enabled for KAPS table, but hitbit clear flag set.")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_get_hw(unit, kaps_table_id, prefix_len, table.additional_table_info, data_bytes, ad_bytes, hit_bit, found));

    if (*found) {
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, ad_bytes, payload));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif


uint32
    arad_pp_dbal_entry_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get - table not initiated %d"), table_id));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_lem(unit, table_id, qual_vals, payload, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_tcam(unit, table_id, qual_vals, payload, priority, hit_bit, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_kbp(unit, table_id, qual_vals, payload, priority, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_A, table_id, qual_vals, payload, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_B, table_id, qual_vals, payload, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_kaps(unit, table_id, qual_vals, payload, priority, hit_bit, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get - illegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("Entry searched in table %s, found=%d\n"), table.table_name, (*found)));
    arad_pp_dbal_entry_dump(unit, table.table_programs[0].stage, table_id, qual_vals, NULL);
    LOG_CLI((BSL_META("\n")));
#endif

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_iterator_get_next_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 flags, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], ARAD_PP_LEM_ACCESS_PAYLOAD* payload, uint32* priority, uint8* hit_bit, uint8* found)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_LEM_ITERATOR *table_iterator;
    uint32 entry_index;
    uint32 iter_is_end = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (table.table_iterator == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next_lem - lem iterator not initialized.")));
    }

    table_iterator = (DBAL_LEM_ITERATOR *)table.table_iterator;

    if ((table_iterator->read_keys == NULL) || (table_iterator->read_vals == NULL)) {
        *found = FALSE;
    } else {
        
        if (table_iterator->current_index == table_iterator->block_index + table_iterator->last_nof_entries) {
            if (SOC_SAND_TBL_ITER_IS_END(&table_iterator->block_range.iter)) {
                *found = FALSE;
                arad_pp_dbal_iterator_deinit(unit, table_id);
                iter_is_end = 1;
            } else {
                table_iterator->block_index = table_iterator->block_index + table_iterator->last_nof_entries;
                SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_lem_get_block_unsafe(unit, &table_iterator->key, &table_iterator->key_mask,
                                                                        NULL,0, &table_iterator->block_range, table_iterator->read_keys,
                                                                        table_iterator->read_vals, &table_iterator->last_nof_entries));
            }
        }
    }

    if ((iter_is_end == 0) && (table_iterator->read_keys != NULL) && (table_iterator->read_vals != NULL) && (table_iterator->last_nof_entries != 0)) {
        entry_index = table_iterator->current_index - table_iterator->block_index;
        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_lem_buffer_to_entry_key(unit, &table_iterator->read_keys[entry_index], table_id, qual_vals));
        if(hit_bit) {
            if (table_iterator->read_vals[entry_index].flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
                *hit_bit = 1;
            } else {
                *hit_bit = 0;
            }
        }

        soc_sand_os_memcpy(payload, &table_iterator->read_vals[entry_index], sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD));

        *found = TRUE;
        table_iterator->current_index++;
    } else {
        *found = FALSE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_iterator_get_next_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 flags, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8* found)
{
    uint8   ad_bytes[JER_KAPS_AD_BUFFER_NOF_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;

    struct kbp_entry *kpb_e;
    struct kbp_entry_info kpb_e_info;
    struct kbp_ad_db *ad_db_p;

    DBAL_KAPS_ITERATOR *table_iterator;

    JER_KAPS_DB    *table_p;
    struct kbp_entry_iter *kaps_iterator;
    int i;
    uint8 db_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (table.table_iterator == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next_kaps - kaps iterator not initialized.")));
    }

    table_iterator = (DBAL_KAPS_ITERATOR *)table.table_iterator;

    *found = FALSE;

    
    for (i=0; i<2; i++) {
        if (i==0) {
            if(SOC_DPP_CONFIG(unit)->arad->init.elk.tcam_dev_type != ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE) {
                
                continue;
            }
            table_p = table_iterator->public_table_p;
            kaps_iterator = table_iterator->public_table_iterator;
            db_id = table_iterator->public_db_id;
        } else {
            table_p = table_iterator->private_table_p;
            kaps_iterator = table_iterator->private_table_iterator;
            db_id = table_iterator->private_db_id;
        }

        if ((kaps_iterator != NULL) && (*found == FALSE)) {
            SOC_PPC_FP_QUAL_VAL_clear(qual_vals);
            while (TRUE) { 
                KBP_TRY(kbp_db_entry_iter_next(table_p, kaps_iterator, &kpb_e));

                if (kpb_e == NULL) {
                    KBP_TRY(kbp_db_entry_iter_destroy(table_p, kaps_iterator));
                    if (i==0) {
                        table_iterator->public_table_iterator = NULL;
                    } else {
                        table_iterator->private_table_iterator = NULL;
                    }

                    break;
                } else {
                    KBP_TRY(kbp_entry_get_info(table_p, kpb_e, &kpb_e_info));

                    
                    if (kpb_e_info.data[0] >> (SOC_SAND_NOF_BITS_IN_BYTE - table.db_prefix_len) != table.db_prefix) {
                        continue;
                    }

                    jer_kaps_ad_db_by_db_id_get(unit, db_id, &ad_db_p);

                    if ((ad_db_p != NULL) && (kpb_e_info.ad_handle != NULL)) {
                            KBP_TRY(kbp_ad_db_get(ad_db_p, kpb_e_info.ad_handle, ad_bytes));
                            SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, ad_bytes, payload));
                            if (JER_PP_KAPS_PAYLOAD_IS_FEC((*(uint32*)payload))) {
                                *(uint32*)payload = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_FEC_DECODE(*(uint32*)payload));
                                _SHR_L3_ITF_SET(*(uint32*)payload, BCM_L3_ITF_TYPE_FEC, *(uint32*)payload);
                            } else {
                                *(uint32*)payload = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_MC_DECODE(*(uint32*)payload));
                            }

                            if (SOC_IS_JERICHO_PLUS(unit)) {
                                
                                if (table.additional_table_info != 0) {
                                    uint8 clear_on_read = *hit_bit & SOC_DPP_DBAL_HITBIT_FLAG_CLEAR ? 1 : 0;
                                    JER_KAPS_IP_TBL_ID kaps_table_id;
                                    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, db_id, NULL, i == 0 ? 0 : 1, &kaps_table_id));

                                    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_get_hit_bit(unit, kaps_table_id, kpb_e, clear_on_read, hit_bit));
                                }
                            }
                    }

                    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_kbp_buffer_to_entry_key(unit, &table, kpb_e_info.prio_len, kpb_e_info.data, qual_vals));
                    *found = TRUE;
                    break;
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_iterator_get_next_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 flags, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8* found)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_KBP_ITERATOR *kbp_table_iterator;

    struct kbp_ad_db 		*ad_db_p	= NULL;
	struct kbp_entry 		*db_entry_p = NULL;
	struct kbp_entry_info 	db_entry_info;

	uint16 is_host, is_host_prefix, is_ipmc;
	uint16 host_prefix_len;

	ARAD_TCAM_ACTION 					kbp_action;
	ARAD_PP_LEM_ACCESS_PAYLOAD 			kbp_lem_payload;
	SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO	*kbp_route_info;

	SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (table.table_iterator == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next_kbp - kbp iterator not initialized.")));
    }

    kbp_table_iterator = (DBAL_KBP_ITERATOR *)table.table_iterator;
	kbp_route_info = (SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO*)payload;

    *found = FALSE;
	is_host = (flags & ARAD_PP_FRWRD_LEM_GET_BLOCK_KBP_HOST_INDICATION) ? TRUE : FALSE;

    is_ipmc = ((kbp_table_iterator->table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_MC) ||
               (kbp_table_iterator->table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_MC)) ? TRUE : FALSE;

	host_prefix_len = ((kbp_table_iterator->table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0) ||
                       (kbp_table_iterator->table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1) ||
                       (kbp_table_iterator->table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_MC)) ? 144 : 48;

	if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC) {
        host_prefix_len = 40;
	}

	sal_memset(&db_entry_info, 0, sizeof(struct kbp_entry_info));
	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);

	
	SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_alg_kbp_ad_db_get(unit,kbp_table_iterator->table_id,&ad_db_p));
    SOCDNX_NULL_CHECK(ad_db_p);

	while(TRUE) {
		
		KBP_TRY(kbp_db_entry_iter_next(kbp_table_iterator->db_p, kbp_table_iterator->iter_p, &db_entry_p));

		if (db_entry_p == NULL) {
			KBP_TRY(kbp_db_entry_iter_destroy(kbp_table_iterator->db_p, kbp_table_iterator->iter_p));
			kbp_table_iterator->iter_p = NULL;
			break;
		} else {
			KBP_TRY(kbp_entry_get_info(kbp_table_iterator->db_p, db_entry_p, &db_entry_info));
            if(is_ipmc) {
                *found = TRUE;
                break;
            }
            is_host_prefix = (db_entry_info.prio_len == host_prefix_len) ? TRUE : FALSE;
			if (is_host_prefix == is_host) {
				*found = TRUE;
				break;
			}
		}
	}

	if (*found) {

        KBP_TRY(kbp_entry_get_info(kbp_table_iterator->db_p, db_entry_p, &db_entry_info));
        SOCDNX_NULL_CHECK(db_entry_info.ad_handle);

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_kbp_buffer_to_entry_key(unit, &table, db_entry_info.prio_len, db_entry_info.data, qual_vals));

		
		sal_memset(kbp_action.elk_ad_value, 0x0, sizeof(uint8) * SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit));
		sal_memset(&kbp_lem_payload, 0x0, sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD));

		KBP_TRY(kbp_ad_db_get(ad_db_p, db_entry_info.ad_handle, kbp_action.elk_ad_value));
        SOCDNX_NULL_CHECK(kbp_action.elk_ad_value);

		
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_kbp_route_payload_decode(unit,
																 kbp_table_iterator->table_id,
																 0 ,
																 kbp_action.elk_ad_value,
																 &kbp_lem_payload));

		SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_em_dest_to_fec(unit,
															 &kbp_lem_payload,
															 kbp_route_info));

		if (is_host) {

			
			ARAD_PP_LEM_ACCESS_HI_FROM_EEI(kbp_lem_payload.asd, kbp_route_info->mac_lsb);

			
			kbp_route_info->native_vsi = kbp_lem_payload.native_vsi;

			if (kbp_lem_payload.flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
				*hit_bit = TRUE;
			}
		}else{
			kbp_route_info->fec_id = kbp_route_info->frwrd_decision.dest_id;
		}
	}

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_iterator_get_next(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 flags, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next - table not initiated %d"), table_id));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_get_next_lem(unit, table_id, flags, qual_vals, payload, priority, hit_bit, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next - TCAM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_get_next_kbp(unit, table_id,  flags, qual_vals, payload, priority, hit_bit, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next - SEM A not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next - SEM B not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_get_next_kaps(unit, table_id,  flags, qual_vals, payload, priority, hit_bit, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_get_next - illegal physical DB type")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_iterator_init_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_LEM_ITERATOR *table_iterator;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (table.table_iterator == NULL) {
        SOCDNX_ALLOC(table.table_iterator, DBAL_LEM_ITERATOR, 1,"LEM iterator");
    }

    table_iterator = (DBAL_LEM_ITERATOR *)table.table_iterator;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit(unit, table_id));

    table_iterator->block_range.entries_to_act = DBAL_LEM_ITERATOR_BLOCK_SIZE;
    table_iterator->block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    SOC_SAND_TBL_ITER_SET_BEGIN(&(table_iterator->block_range.iter));

    ARAD_PP_LEM_ACCESS_KEY_clear(&table_iterator->key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&table_iterator->key_mask);

    table_iterator->key.type = table.additional_table_info;
    table_iterator->key.prefix.value = table.db_prefix;
    table_iterator->key.prefix.nof_bits = table.db_prefix_len;
    table_iterator->key_mask.prefix.value = table.db_prefix;
    table_iterator->key_mask.prefix.nof_bits = table.db_prefix_len;

    if (table_iterator->read_keys == NULL) {
        SOCDNX_ALLOC(table_iterator->read_keys, ARAD_PP_LEM_ACCESS_KEY, DBAL_LEM_ITERATOR_BLOCK_SIZE, "LEM iterator keys block");
    }
    if (table_iterator->read_vals == NULL) {
        SOCDNX_ALLOC(table_iterator->read_vals, ARAD_PP_LEM_ACCESS_PAYLOAD, DBAL_LEM_ITERATOR_BLOCK_SIZE, "LEM iterator vals block");
    }

    table_iterator->current_index = 0;
    table_iterator->block_index = 0;
    table_iterator->last_nof_entries = 0;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_iterator_init_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_KAPS_ITERATOR *kaps_table_iterator;
    uint32 kaps_table_id;
    JER_KAPS_TABLE_CONFIG kaps_table_cfg_p;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit(unit, table_id));

    if (table.table_iterator == NULL) {
        SOCDNX_ALLOC(table.table_iterator, DBAL_KAPS_ITERATOR, 1, "KAPS iterator");
    }

    
    kaps_table_iterator = (DBAL_KAPS_ITERATOR *)table.table_iterator;

    SOCDNX_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, NULL, 1, &kaps_table_id));
    jer_kaps_table_config_get(unit, kaps_table_id, &kaps_table_cfg_p);
    kaps_table_iterator->private_table_p = kaps_table_cfg_p.tbl_p;
    kaps_table_iterator->private_db_id = kaps_table_cfg_p.db_id;
    kaps_table_iterator->private_table_iterator = NULL;

    SOCDNX_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, NULL, 0, &kaps_table_id));
    jer_kaps_table_config_get(unit, kaps_table_id, &kaps_table_cfg_p);
    kaps_table_iterator->public_table_p = kaps_table_cfg_p.tbl_p;
    kaps_table_iterator->public_db_id = kaps_table_cfg_p.db_id;
    kaps_table_iterator->public_table_iterator = NULL;

    if (JER_KAPS_ENABLE_PUBLIC_DB(unit)) {
        KBP_TRY(kbp_db_entry_iter_init(kaps_table_iterator->public_table_p, &kaps_table_iterator->public_table_iterator));
    }

    if (JER_KAPS_ENABLE_PRIVATE_DB(unit)) {
        KBP_TRY(kbp_db_entry_iter_init(kaps_table_iterator->private_table_p, &kaps_table_iterator->private_table_iterator));
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_iterator_init_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_KBP_ITERATOR *kbp_table_iterator;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit(unit, table_id));

    if (table.table_iterator == NULL) {
        SOCDNX_ALLOC(table.table_iterator, DBAL_KBP_ITERATOR, 1, "KBP iterator");
    }

    kbp_table_iterator = (DBAL_KBP_ITERATOR *)table.table_iterator;
	kbp_table_iterator->table_id = table.db_prefix;

	kbp_table_iterator->db_p 		= NULL;
    kbp_table_iterator->iter_p 		= NULL;

	
    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_alg_kbp_db_get(unit,kbp_table_iterator->table_id,&kbp_table_iterator->db_p));
    SOCDNX_NULL_CHECK(kbp_table_iterator->db_p);

	
    KBP_TRY(kbp_db_entry_iter_init(kbp_table_iterator->db_p, &kbp_table_iterator->iter_p));
    SOCDNX_NULL_CHECK(kbp_table_iterator->iter_p);

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_iterator_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_init - table not initiated %d"), table_id));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_init_lem(unit, table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_init - TCAM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_init_kbp(unit, table_id));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_init - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_init - SEM A not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_init - SEM B not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_init_kaps(unit, table_id));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_init - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_init - illegal physical DB type")));
        break;
    }

    if (physical_db_type != NULL) {
        *physical_db_type = table.physical_db_type;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_iterator_deinit_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_LEM_ITERATOR *table_iterator;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    table_iterator = (DBAL_LEM_ITERATOR *)table.table_iterator;

    if (table_iterator != NULL) {
        if (table_iterator->read_keys != NULL) {
            SOCDNX_FREE(table_iterator->read_keys);
            table_iterator->read_keys = NULL;
        }

        if (table_iterator->read_vals != NULL) {
            SOCDNX_FREE(table_iterator->read_vals);
            table_iterator->read_vals = NULL;
        }

        SOC_SAND_TBL_ITER_SET_END(&(table_iterator->block_range.iter));

        table_iterator->current_index = 0;
        table_iterator->block_index = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_iterator_deinit_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_KAPS_ITERATOR *kaps_table_iterator;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    kaps_table_iterator = (DBAL_KAPS_ITERATOR *)table.table_iterator;

    if (kaps_table_iterator != NULL) {
        if (kaps_table_iterator->public_table_iterator != NULL) {
            KBP_TRY(kbp_db_entry_iter_destroy(kaps_table_iterator->public_table_p, kaps_table_iterator->public_table_iterator));
            kaps_table_iterator->public_table_iterator = NULL;
        }

        if (kaps_table_iterator->private_table_iterator != NULL) {
            KBP_TRY(kbp_db_entry_iter_destroy(kaps_table_iterator->private_table_p, kaps_table_iterator->private_table_iterator));
            kaps_table_iterator->private_table_iterator = NULL;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_iterator_deinit_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    DBAL_KBP_ITERATOR *kbp_table_iterator;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    kbp_table_iterator = (DBAL_KBP_ITERATOR *)table.table_iterator;

    if (kbp_table_iterator != NULL) {
		if (kbp_table_iterator->iter_p != NULL) {
			KBP_TRY(kbp_db_entry_iter_destroy(kbp_table_iterator->db_p, kbp_table_iterator->iter_p));
			kbp_table_iterator->iter_p = NULL;
		}
		kbp_table_iterator->db_p = NULL;
		kbp_table_iterator->table_id = DBAL_KBP_INVALID_TABLE_ID;
	}

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_iterator_deinit(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_deinit - table not initiated %d"), table_id));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit_lem(unit, table_id));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_deinit - TCAM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit_kbp(unit, table_id));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_deinit - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_deinit - SEM A not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_deinit - SEM B not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit_kaps(unit, table_id));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_deinit - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_iterator_deinit - illegal physical DB type")));
        break;
    }

exit:
    SOCDNX_FREE(table.table_iterator);
    table.table_iterator = NULL;
    SOCDNX_IF_ERR_REPORT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_add_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority, SOC_SAND_SUCCESS_FAILURE* success)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    ARAD_TCAM_ENTRY  entry;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(entry.value,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memset(entry.mask,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    ARAD_TCAM_ENTRY_clear(&entry);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, 0, qual_vals, entry.value, entry.mask ));

    if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("TCAM profile not assigned, cannot access TCAM ")));
    }

    entry.valid = TRUE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_add_unsafe(unit, table.db_prefix, entry_id, FALSE, priority, &entry, (ARAD_TCAM_ACTION*)payload, success ));

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_add_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 payload[ARAD_TCAM_ACTION_MAX_LEN], uint32 priority, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE* success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, 0, qual_vals, entry_key, entry_mask ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_tcam_kbp_tcam_entry_add(unit, table.db_prefix, entry_id, is_for_update,
                                                               priority, entry_key, entry_mask, payload, NULL, success));
exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_add_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, ARAD_TCAM_ACTION* payload, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_id_tcam(unit, table_id, entry_id, qual_vals, payload, priority, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_id_kbp(unit, table_id, entry_id, qual_vals, payload->value, priority, is_for_update,  success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
        break;

    default:
     
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - illegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry added to table %s \n"), table.table_name));
    arad_pp_dbal_entry_dump(unit, table.table_programs[0].stage, table_id, qual_vals, NULL);
    LOG_CLI((BSL_META("\nPayload: %d \n"), *(uint32*)payload));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_input_validate(unit, table_id, qual_vals));
#endif

    table.nof_entries_added_to_table++;
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, table.nof_entries_added_to_table));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_delete_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{

    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("TCAM profile not assigned, cannot access TCAM ")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_remove_unsafe(unit, table.db_prefix, entry_id));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_delete_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_tcam_kbp_route_remove(unit, table.db_prefix, entry_id));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_delete_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_id_tcam(unit, table_id, entry_id, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_id_kbp(unit, table_id, entry_id, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - KBP not in use")));
#endif
        break;

    default:
     
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - illegal physical DB type")));
        break;
    }

    table.nof_entries_added_to_table--;
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, table.nof_entries_added_to_table));

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry deleted from table %s entry_id = %d\n"),table.table_name, entry_id));
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_get_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    ARAD_TCAM_ENTRY  entry;
    int i;
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    
    ARAD_TCAM_ENTRY_clear(&entry);

    for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++i) {
        SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[i]);
    }

    if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("TCAM profile not assigned, cannot access TCAM ")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_get_unsafe(unit, table.db_prefix, entry_id, TRUE ,  priority, &entry, (ARAD_TCAM_ACTION*)payload, found, hit_bit ));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 0, 1, 0, qual_vals, entry.value, entry.mask ));

exit:
    SOCDNX_FUNC_RETURN;
}


#if defined(INCLUDE_KBP)
STATIC uint32
    arad_pp_dbal_entry_get_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8* found)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++i) {
        SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[i]);
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_tcam_kbp_tcam_entry_get(
           unit,
           table.db_prefix,
           entry_id,
           FALSE,
           entry_key,
           entry_mask,
           payload,
           priority,
           found,
           hit_bit
         ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 1, 1, 0, qual_vals, entry_key, entry_mask ));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif 



uint32
    arad_pp_dbal_entry_get_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get_id - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_id_tcam(unit, table_id, entry_id, qual_vals, payload, priority, hit_bit, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_id_kbp(unit, table_id, entry_id, qual_vals, payload, priority, hit_bit, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KBP not in use")));
#endif
        break;

    default:
     
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - illegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("Entry to get (%d), found %d\n"), entry_id, (*found)));
    arad_pp_dbal_entry_dump(unit, table.table_programs[0].stage, table_id, qual_vals, NULL);
    LOG_CLI((BSL_META("\n")));
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_key_to_lem_buffer(int unit, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, ARAD_PP_LEM_ACCESS_KEY *key)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, param_counter = 0;
    uint8 qual_total_bits, qual_offset, table_qual_nof_bits;
    uint32 msb_value, lsb_value;
    uint32 user_qual_idx, qual_type;
    uint32 mask;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    ARAD_PP_LEM_ACCESS_KEY_clear(key);

    key->type = table.additional_table_info;

    key->nof_params = table.nof_qualifiers;

    for (i = 0; i < table.nof_qualifiers; i++) {
        qual_total_bits = table.qual_info[i].qual_full_size;
        table_qual_nof_bits = table.qual_info[i].qual_nof_bits;
        
        if (!(table.qual_info[i].qual_is_in_hdr)) {
            qual_offset = table.qual_info[i].qual_offset;
        } else{
            if (table.qual_info[i].ignore_qual_offset_for_entry_mngmnt){
                table.qual_info[i].qual_offset = 0;
            }
            qual_offset = qual_total_bits - table.qual_info[i].qual_offset - table_qual_nof_bits;
        }

        qual_type = table.qual_info[i].qual_type;
        key->param[param_counter].nof_bits = table_qual_nof_bits;

        if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {
            
            user_qual_idx = 0;
            while ((user_qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (qual_type != qual_vals[user_qual_idx].type)) {
                user_qual_idx++;
            }

            if (user_qual_idx == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Missing qualifier :%s, in table:%s.\n"), SOC_PPC_FP_QUAL_TYPE_to_string(qual_type), table.table_name));
            } else {
                if ((qual_offset + table.qual_info[i].qual_nof_bits > 32) && (qual_offset < 32)) {
                    
                    mask = (1 << (table_qual_nof_bits - (32 - qual_offset))) - 1;
                    msb_value = (qual_vals[user_qual_idx].val.arr[1] & mask);
                    mask = (1 << ((32 - qual_offset))) - 1;
                    lsb_value = ((qual_vals[user_qual_idx].val.arr[0] >> qual_offset) & mask);
                    key->param[param_counter].value[0] =  (msb_value << (32 - qual_offset)) | lsb_value;
                } else if (qual_offset >= 32) {
                    
                    mask = table_qual_nof_bits == 32 ? SOC_SAND_U32_MAX : (1 << (table_qual_nof_bits)) - 1;
                    msb_value = (qual_vals[user_qual_idx].val.arr[1] >> (qual_offset - 32)) & mask;
                    key->param[param_counter].value[0] =  msb_value;
                } else {
                    
                    mask = table_qual_nof_bits == 32 ? SOC_SAND_U32_MAX : (1 << (table_qual_nof_bits)) - 1;
                    lsb_value = (qual_vals[user_qual_idx].val.arr[0] >> qual_offset) & mask;
                    key->param[param_counter].value[0] =  lsb_value;
                }
            }
        } else if (qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ONES) {
            key->param[param_counter].value[0] =  (1 << (table_qual_nof_bits)) - 1;
        }

        param_counter++;
    }

    key->prefix.nof_bits = table.db_prefix_len;
    key->prefix.value = table.db_prefix;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
  arad_pp_dbal_lem_buffer_to_entry_key(int unit, ARAD_PP_LEM_ACCESS_KEY *key, uint32 table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX])
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int i;
    uint8 qual_total_bits, qual_offset, table_qual_nof_bits;
    uint32 user_qual_idx, qual_type;
    uint32 mask;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    for (i = 0; i < key->nof_params; i++) {
        qual_total_bits = table.qual_info[i].qual_full_size;
        table_qual_nof_bits = table.qual_info[i].qual_nof_bits;
        
        if (!(table.qual_info[i].qual_is_in_hdr)) {
            qual_offset = table.qual_info[i].qual_offset;
        } else{
            qual_offset = qual_total_bits - table.qual_info[i].qual_offset - table_qual_nof_bits;
        }
        qual_type = table.qual_info[i].qual_type;

        if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {
            
            user_qual_idx = 0;
            
            while ((user_qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (qual_type != qual_vals[user_qual_idx].type) && (qual_vals[user_qual_idx].type != BCM_FIELD_ENTRY_INVALID)) {
                user_qual_idx++;
            }

            if (user_qual_idx == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unable to find empty qual for:%s \n"), SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
            } else {
                if ((qual_offset + table.qual_info[i].qual_nof_bits > 32) && (qual_offset < 32)) {
                    
                    mask = (1 << (table_qual_nof_bits - (32 - qual_offset))) - 1;
                    qual_vals[user_qual_idx].val.arr[1] |= (key->param[i].value[0] >> (32 - qual_offset)) & mask;
                    mask = (1 << ((32 - qual_offset))) - 1;
                    qual_vals[user_qual_idx].val.arr[0] |= key->param[i].value[0] & mask;
                } else if (qual_offset >= 32) {
                    
                    mask = table_qual_nof_bits == 32 ? SOC_SAND_U32_MAX : (1 << (table_qual_nof_bits)) - 1;
                    qual_vals[user_qual_idx].val.arr[1] |= (key->param[i].value[0] & mask) << (qual_offset - 32);
                } else {
                    
                    mask = table_qual_nof_bits == 32 ? SOC_SAND_U32_MAX : (1 << (table_qual_nof_bits)) - 1;
                    qual_vals[user_qual_idx].val.arr[0] |= (key->param[i].value[0] & mask) << qual_offset;
                }
                qual_vals[user_qual_idx].type = qual_type;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_dbal_entry_lem_buffer_to_key(int unit, uint32 table_id, uint32 buffer[ARAD_PP_FP_TCAM_ENTRY_SIZE], SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX])
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, offset_fix = 0;
    ARAD_PP_LEM_ACCESS_KEY key;
    uint32 curr_bit_lsb, param_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    ARAD_PP_LEM_ACCESS_KEY_clear(&key);


    
    key.type = table.additional_table_info;

    key.nof_params = table.nof_qualifiers;

    for (i = 0; i < table.nof_qualifiers; i++) {
        if(i > 0){
            if(table.qual_info[i].qual_type == table.qual_info[i-1].qual_type) {
                key.param[i-1-offset_fix].nof_bits += table.qual_info[i].qual_nof_bits;
                key.nof_params -= 1;
                offset_fix++;
            }else {
                key.param[i-offset_fix].nof_bits = table.qual_info[i].qual_nof_bits;
            }
        }else {
                key.param[i].nof_bits = table.qual_info[i].qual_nof_bits;
        }
    }

    key.prefix.nof_bits = table.db_prefix_len;
    key.prefix.value = table.db_prefix;

    
    curr_bit_lsb = 0;
    for (param_ndx = 0; param_ndx < key.nof_params; ++param_ndx){

        if (((key.param[param_ndx].nof_bits == 0)) ||
             (key.param[param_ndx].nof_bits > SOC_DPP_DEFS_GET(unit, lem_width)) ||
             (curr_bit_lsb > SOC_DPP_DEFS_GET(unit, lem_width))){
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("lem key param size out of range err %d"), table_id));
        }

        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_get_any_field(buffer, curr_bit_lsb, key.param[param_ndx].nof_bits, key.param[param_ndx].value));
        curr_bit_lsb += key.param[param_ndx].nof_bits;
    }

    
    offset_fix = 0;
    for (i = 0; i < table.nof_qualifiers; i++) {
        if(i > 0){
            if(table.qual_info[i].qual_type == table.qual_info[i-1].qual_type) {
                qual_vals[i-1-offset_fix].val.arr[1] = key.param[i-1-offset_fix].value[1];
                offset_fix++;
            }else {
                qual_vals[i-offset_fix].val.arr[0] = key.param[i-offset_fix].value[0];
            }
        }else {
            qual_vals[i].val.arr[0] = key.param[i].value[0];
        }
        qual_vals[i].type = table.qual_info[i].qual_type;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
uint32
  arad_pp_dbal_entry_key_to_kbp_buffer(int unit, SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 table_size_in_bytes, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                       uint32 *prefix_len, uint8 *data_bytes)
{
    uint8
        nof_quals = dbal_table->nof_qualifiers,
        iter_bit_len,
        temp_prefix_len = 0,
        qual_total_bits,
        qual_prefix_len,
        byte_prefix_len,
        continue_flag = 0;
    uint32
        bit_offset = 0,
        qual_remaining_bits,
        qual_offset,
        data_idx = 0,
        user_qual_idx,
        temp_byte_mask,
        qual_idx=0, i, j;
    uint64
        val = {0},
        temp_val = {0},
        mask = {0},
        temp_mask = {0};
    SOC_PPC_FP_QUAL_TYPE    qual_type;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(dbal_table);
    SOC_SAND_CHECK_NULL_INPUT(qual_vals);
    SOC_SAND_CHECK_NULL_INPUT(data_bytes);

    sal_memset(data_bytes, 0x0, sizeof(uint8) * table_size_in_bytes);

    
    if (dbal_table->db_prefix != DBAL_PREFIX_NOT_DEFINED) {
        data_bytes[0] = dbal_table->db_prefix;
        data_bytes[0] <<= (SOC_SAND_NOF_BITS_IN_CHAR - dbal_table->db_prefix_len);
        bit_offset = SOC_SAND_NOF_BITS_IN_CHAR - dbal_table->db_prefix_len;
        *prefix_len = dbal_table->db_prefix_len;
    } else {
        *prefix_len = 0;
    }

    
    for (i = 0; i < nof_quals; i++)
    {
        qual_idx = nof_quals - i - 1;
        qual_prefix_len = 0; 
        qual_type = dbal_table->qual_info[qual_idx].qual_type;
        qual_total_bits = dbal_table->qual_info[qual_idx].qual_full_size;
        qual_remaining_bits = dbal_table->qual_info[qual_idx].qual_nof_bits;
        
        if (!(dbal_table->qual_info[qual_idx].qual_is_in_hdr)) {
            qual_offset = dbal_table->qual_info[qual_idx].qual_offset;
        } else{
            qual_offset = qual_total_bits - qual_remaining_bits - dbal_table->qual_info[qual_idx].qual_offset;
        }

        if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {
            
            user_qual_idx = 0;
            while ((user_qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (qual_type != qual_vals[user_qual_idx].type)) {
                user_qual_idx++;
            }

            if (user_qual_idx == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                
                COMPILER_64_SET(val, 0, 0);
                COMPILER_64_SET(mask, 0, 0);
            } else{
                COMPILER_64_SET(val, qual_vals[user_qual_idx].val.arr[1], qual_vals[user_qual_idx].val.arr[0]);
                COMPILER_64_SET(mask, qual_vals[user_qual_idx].is_valid.arr[1], qual_vals[user_qual_idx].is_valid.arr[0]);
            }
        } else if (qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ONES) {
            COMPILER_64_SET(val, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX);
            COMPILER_64_SET(mask, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX);
        }

        
        while (qual_remaining_bits > 0)
        {
            
            iter_bit_len = bit_offset > qual_remaining_bits ? qual_remaining_bits : bit_offset;

            if (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) {
                temp_val = val;
                temp_mask = mask;

                
                COMPILER_64_SHR(temp_mask, qual_remaining_bits + qual_offset - iter_bit_len);
                temp_byte_mask = (((SOC_SAND_U8_MAX >> (SOC_SAND_NOF_BITS_IN_CHAR - iter_bit_len)) &
                                  temp_mask)
                                  << (bit_offset - iter_bit_len)); 
                byte_prefix_len = soc_sand_nof_on_bits_in_char(temp_byte_mask);

                
                if (qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ONES) {
                    temp_prefix_len += byte_prefix_len;
                }else {
                    if (byte_prefix_len) {
                        *prefix_len += byte_prefix_len + temp_prefix_len;
                        temp_prefix_len = 0;
                    }
                }
                qual_prefix_len += byte_prefix_len;
                COMPILER_64_SHR(temp_val, qual_remaining_bits + qual_offset - iter_bit_len);
                data_bytes[data_idx] |= ((temp_val
                                         << (bit_offset - iter_bit_len)) & temp_byte_mask); 
            } else {
                
                temp_prefix_len += iter_bit_len;
                qual_prefix_len += iter_bit_len;
            }

            qual_remaining_bits -= iter_bit_len;
            bit_offset -= iter_bit_len;

            if (bit_offset == 0) {
                data_idx++;
                bit_offset = SOC_SAND_NOF_BITS_IN_CHAR;
            }
        }

        
        if (qual_prefix_len != dbal_table->qual_info[qual_idx].qual_nof_bits) {
            break;
        }
    }

    
    for (i = 0; i < qual_idx; i++) {
        qual_type = dbal_table->qual_info[i].qual_type;
        if (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES && qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES){
            
            user_qual_idx = 0;
            while ((user_qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (qual_type != qual_vals[user_qual_idx].type)) {
                user_qual_idx++;
            }
            if (user_qual_idx != SOC_PPC_FP_NOF_QUALS_PER_DB_MAX){  
                if  (qual_vals[user_qual_idx].is_valid.arr[0] == 0 && qual_vals[user_qual_idx].is_valid.arr[1] == 0){
                    continue;
                }
                else{
                    
                    continue_flag = 0;
                    for (j = qual_idx; j < nof_quals; j++) {
                        if (qual_type == dbal_table->qual_info[j].qual_type) {
                            continue_flag = 1;
                            break;
                        }
                    }
                    if (continue_flag) {
                        continue;
                    }
                    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Error: valid qualifier %s after don't care qualifier\n"),SOC_PPC_FP_QUAL_TYPE_to_string(qual_vals[user_qual_idx].type)));
                }
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_dbal_key_encode()",0,0);
}
#endif 

#if defined(INCLUDE_KBP)
uint32
  arad_pp_dbal_kbp_buffer_to_entry_key(int unit, const SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 prefix_len, uint8 *data_bytes,
                                       SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX])
{
    uint8
        nof_quals = dbal_table->nof_qualifiers,
        iter_bit_len,
        qual_total_bits,
        byte_prefix_len;
    uint32
        bit_offset = 0,
        qual_remaining_bits,
        qual_offset,
        data_idx = 0,
        user_qual_idx = 0,
        temp_byte_mask,
        qual_idx, i,
        prefix_remain;
    uint64
        part_qual_val,
        part_qual_mask;

    SOC_PPC_FP_QUAL_TYPE    qual_type;
    ARAD_KBP_TABLE_CONFIG   table_config_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(dbal_table);
    SOC_SAND_CHECK_NULL_INPUT(qual_vals);
    SOC_SAND_CHECK_NULL_INPUT(data_bytes);

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    SOC_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.kbp_info.Arad_kbp_table_config_info.get(unit, dbal_table->db_prefix, &table_config_info));

    prefix_remain = prefix_len;

    
    if (dbal_table->db_prefix != DBAL_PREFIX_NOT_DEFINED) {
        bit_offset = SOC_SAND_NOF_BITS_IN_CHAR - dbal_table->db_prefix_len;
        prefix_remain -= dbal_table->db_prefix_len;
    }

    
    for (i = 0; i < nof_quals; i++)
    {
        qual_idx = nof_quals - i - 1;
        qual_type = dbal_table->qual_info[qual_idx].qual_type;
        qual_total_bits = dbal_table->qual_info[qual_idx].qual_full_size;
        qual_remaining_bits = dbal_table->qual_info[qual_idx].qual_nof_bits;
        
        if (!(dbal_table->qual_info[qual_idx].qual_is_in_hdr)) {
            qual_offset = dbal_table->qual_info[qual_idx].qual_offset;
        } else{
            qual_offset = qual_total_bits - qual_remaining_bits - dbal_table->qual_info[qual_idx].qual_offset;
        }

        if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {
            
            user_qual_idx = 0;
            while ((user_qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (qual_type != qual_vals[user_qual_idx].type)) {
                
                if ((qual_vals[user_qual_idx].type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_vals[user_qual_idx].type == BCM_FIELD_ENTRY_INVALID)) {
                    SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[user_qual_idx]);
                    qual_vals[user_qual_idx].type = qual_type;
                    break;
                }
                user_qual_idx++;
            }
        }

        
        while (qual_remaining_bits > 0 && prefix_remain > 0)
        {
            
            iter_bit_len = bit_offset > qual_remaining_bits ? qual_remaining_bits : bit_offset;
            
            
            iter_bit_len = iter_bit_len > prefix_remain ? prefix_remain : iter_bit_len;

            if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {

                temp_byte_mask = (SOC_SAND_U8_MAX >> (SOC_SAND_NOF_BITS_IN_BYTE - iter_bit_len)); 
                part_qual_mask = COMPILER_64_INIT(0, temp_byte_mask);

                COMPILER_64_SHL(part_qual_mask, qual_remaining_bits + qual_offset - iter_bit_len);

                byte_prefix_len = soc_sand_nof_on_bits_in_char(temp_byte_mask);

                
                part_qual_val = COMPILER_64_INIT(0, (data_bytes[data_idx] >> (bit_offset - iter_bit_len)) & temp_byte_mask);
                COMPILER_64_SHL(part_qual_val, qual_remaining_bits + qual_offset - iter_bit_len);


                qual_vals[user_qual_idx].val.arr[0] +=  COMPILER_64_LO(part_qual_val);
                qual_vals[user_qual_idx].val.arr[1] +=  COMPILER_64_HI(part_qual_val);
                qual_vals[user_qual_idx].is_valid.arr[0] +=  COMPILER_64_LO(part_qual_mask);
                qual_vals[user_qual_idx].is_valid.arr[1] +=  COMPILER_64_HI(part_qual_mask);

                prefix_remain -= byte_prefix_len;
            } else {
                
                
                
                    prefix_remain -= iter_bit_len;
                
            }

            qual_remaining_bits -= iter_bit_len;
            bit_offset -= iter_bit_len;

            if (bit_offset == 0) {
                data_idx++;
                bit_offset = SOC_SAND_NOF_BITS_IN_CHAR;
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_dbal_kbp_buffer_to_entry_key()",0,0);
}
#endif 


#undef DBAL_ENTRY_MANAGMENT_FUNCTIONS
#define DBAL_DIAGNOSTICS_FUNCTIONS


uint32
    arad_pp_dbal_phisycal_db_dump(int unit, int mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (mode) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_table_print_sem(unit, SOC_DPP_DBAL_SW_TABLE_ID_INVALID));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_table_print_sem(unit, SOC_DPP_DBAL_SW_NOF_TABLES));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_print_tcam(unit,SOC_DPP_DBAL_SW_TABLE_ID_INVALID));
        break;

	case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP)
        SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_print_diag_all_entries(unit, -1));
#endif 

        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(INCLUDE_KBP)
        {
            
            LOG_CLI((BSL_META("Not supported yet.\n")));
        }
#endif 
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_table_print_lem(unit, SOC_DPP_DBAL_SW_TABLE_ID_INVALID));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_ESEM:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM1:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM2:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_RMAP:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_GLEM:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_em_dump(unit, mode));
        break;

    default:
        LOG_CLI((BSL_META("Ilegal parameter value.\n")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_entry_dump(int unit, SOC_PPC_FP_DATABASE_STAGE stage, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL in_qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 in_data[ARAD_PP_FP_TCAM_ENTRY_SIZE])
{
    SOC_DPP_DBAL_TABLE_INFO     *table = NULL;
    uint32                      data[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] = {0};
    uint32                      mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_PPC_FP_QUAL_VAL         qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int i, no_buffer_print = 0, buffer_to_qualifier_mode = 0, is_prefix_print_needed = 0;
    uint32 prefix;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(table, SOC_DPP_DBAL_TABLE_INFO, 1, "arad_pp_dbal_entry_dump.table");
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, table));

    if (in_data != NULL) {
        buffer_to_qualifier_mode = 1;
        for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
            SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[i]);
        }
    }else{
        if (in_qual_vals == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR - iilegal input ")));
        }
        buffer_to_qualifier_mode = 0;
    }

    switch (table->physical_db_type) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:

        if (buffer_to_qualifier_mode) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 0, 1, 0, qual_vals, in_data, mask));
            soc_sand_bitstream_get_any_field(&in_data[0], ARAD_PP_ISEM_ACCESS_LSB_PREFIX, ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX, &prefix);
            is_prefix_print_needed = 1;
        }else{
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, in_qual_vals, &data[0]));
        }

        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:

        if (buffer_to_qualifier_mode) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 0, 1, 0, qual_vals, in_data, mask));
        }else{
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, 0, in_qual_vals, data, mask));
        }

        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(INCLUDE_KBP)
        {
            
        }
#endif 
        SOCDNX_FUNC_RETURN;
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        if (buffer_to_qualifier_mode) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_lem_buffer_to_key(unit, table_id, in_data, qual_vals));
            soc_sand_bitstream_get_any_field(&(in_data[0]), SOC_DPP_DEFS_GET(unit, lem_width) - SOC_DPP_DEFS_GET(unit, nof_lem_prefixes),
                                         SOC_DPP_DEFS_GET(unit, nof_lem_prefixes),&prefix);
            is_prefix_print_needed = 1;
        }else{
            no_buffer_print = 1;
            
        }

        break;

    default:
        SOCDNX_FUNC_RETURN;
        break;
    }

    if (in_data == NULL) {
        in_data = data;
    }

    if (in_qual_vals == NULL) {
        in_qual_vals = qual_vals;
    }

    if (is_prefix_print_needed) {
        LOG_CLI((BSL_META("Prefix=%2d "), prefix));
    }

    for (i = 0; i < table->nof_qualifiers; i++) {
        if (in_qual_vals[i].type == BCM_FIELD_ENTRY_INVALID) {
            break;
        }
        LOG_CLI((BSL_META("%s=0x%x  "), (SOC_PPC_FP_QUAL_TYPE_to_string(in_qual_vals[i].type)), in_qual_vals[i].val.arr[0]));
        if (in_qual_vals[i].val.arr[1] != 0) {
            LOG_CLI((BSL_META(" 0x%x  "), in_qual_vals[i].val.arr[1]));
        }
        if (buffer_to_qualifier_mode == 0){
            if ((table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM)
                && (in_qual_vals[i].type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (in_qual_vals[i].type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)){
                LOG_CLI((BSL_META("mask=0x%x  "), in_qual_vals[i].is_valid.arr[0]));
            }
            if (in_qual_vals[i].val.arr[1] != 0) {
                LOG_CLI((BSL_META(" 0x%x  "), in_qual_vals[i].is_valid.arr[1]));
            }
        }
    }

    if (!no_buffer_print) {
        int entry_size = SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S - 1;
        if (table->table_programs[0].nof_bits_used_in_key > 80) {
            entry_size = (entry_size*2) - 1;
        }

        LOG_CLI((BSL_META("Full buffer=0x")));
        for (i = entry_size; i >= 0; i--) {
            if (i == entry_size) {
                LOG_CLI((BSL_META("%02x "), in_data[i]));
            }else{
                LOG_CLI((BSL_META("%08x "), in_data[i]));
            }
        }
    }

exit:
    SOCDNX_FREE(table);
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_entry_input_validate(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL in_qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX])
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, j, found_error = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    for (i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
        for (j = 0; j < table.nof_qualifiers; j++) {
            if (in_qual_vals[i].type == table.qual_info[j].qual_type) {
                break;
            }
        }
        if ((j == table.nof_qualifiers) && (in_qual_vals[i].type != SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG)) {
            LOG_CLI((BSL_META("WARNING:: qualifier %s (%d) not exists value = %08x\n"), SOC_PPC_FP_QUAL_TYPE_to_string(in_qual_vals[i].type), in_qual_vals[i].type, in_qual_vals[i].val.arr[0]));
            found_error = 1;
        }
    }

    if (found_error) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("WARNING:: invalid qualifier input!! in table %s (%d)"),table.table_name, table_id));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
	arad_pp_dbal_diag_entry_manage(int unit, SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_MODE mode, int table_id, args_t* arg)
{
	int i;
	SOC_DPP_DBAL_TABLE_INFO table;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;

	uint32 tcam_payload[ARAD_TCAM_ACTION_MAX_LEN];
	ARAD_PP_LEM_ACCESS_PAYLOAD lem_payload;
	ARAD_PP_ISEM_ACCESS_ENTRY sem_payload;
	uint32 kaps_payload, *payload_array, payload_nof_words, input_nof_words;

	void *payload;

	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_NULL_CHECK(arg);
	if (!DBAL_DIAG_ENTRY_MANAGE_MODE_VALID(mode)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid entry manage mode\n")));
	}

	for (i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
		SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[i]);
	}

    DBAL_CHECK_TABLE_ID_EXIT_WITH_ERR(table_id);

	SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

	if (!table.is_table_initiated) {
		SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Specified table not initiated\n")));
	}

	if (ARG_CNT(arg) < table.nof_qualifiers){
		LOG_CLI((BSL_META("Missing qualifiers, list qualifier values in this order:\n")));
		for (i = 0; i < table.nof_qualifiers; i++) {
			LOG_CLI((BSL_META("<%s (%d bits)> "), SOC_PPC_FP_QUAL_TYPE_to_string(table.qual_info[i].qual_type), table.qual_info[i].qual_nof_bits));
		}
		LOG_CLI((BSL_META("\n\n")));
		SOCDNX_FUNC_RETURN;
	}

	if (mode == SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_ADD) {
        if (ARG_CNT(arg) < table.nof_qualifiers + 1) {
            LOG_CLI((BSL_META("Missing entry payload\n")));
            SOCDNX_FUNC_RETURN;
        }
		if (ARG_CNT(arg) > table.nof_qualifiers + 1) {
            LOG_CLI((BSL_META("Excess arguments provided\n")));
            SOCDNX_FUNC_RETURN;
        }
	} else {
        if (ARG_CNT(arg) > table.nof_qualifiers) {
            LOG_CLI((BSL_META("Excess arguments provided\n")));
            SOCDNX_FUNC_RETURN;
        }
    }

	
	for (i =  0; i < table.nof_qualifiers; i++) {
		qual_vals[i].type = table.qual_info[i].qual_type;
		SOCDNX_IF_ERR_EXIT(arad_pp_dbal_diag_input_parse(unit, ARG_GET(arg), SOC_SAND_U64_NOF_UINT32S, qual_vals[i].val.arr, &input_nof_words));
	}
    LOG_CLI((BSL_META("Table: %s \nQualifiers:\n"), table.table_name));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_dump(unit, table.table_programs[0].stage, table_id, qual_vals, NULL));

    
    if (mode == SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_ADD) {

        switch(table.physical_db_type)
        {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload);
            lem_payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
            payload_array = &lem_payload.dest;
            payload_nof_words = 1;
            payload = (void *)&lem_payload;
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
            soc_sand_os_memset(tcam_payload, 0x0, ARAD_TCAM_ACTION_MAX_LEN * sizeof(uint32));
            payload_array = tcam_payload;
            payload_nof_words = ARAD_TCAM_ACTION_MAX_LEN;
            payload = (void *)&tcam_payload;
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
            soc_sand_os_memset(&sem_payload, 0x0, sizeof(ARAD_PP_ISEM_ACCESS_ENTRY));
            payload_array = &sem_payload.sem_result_ndx;
            payload_nof_words = 1;
            payload = (void *)&sem_payload;
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
            soc_sand_os_memset(&kaps_payload, 0x0, sizeof(kaps_payload));
            payload_array = &kaps_payload;
            payload_nof_words = 1;
            payload = (void *)&kaps_payload;
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_diag_entry_manage - illegal physical DB type")));
            break;
        }

        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_diag_input_parse(unit, ARG_GET(arg), payload_nof_words, payload_array, &input_nof_words));

        LOG_CLI((BSL_META("\nPayload:\n")));
		
		if (payload_nof_words == 1) {
			LOG_CLI((BSL_META("%d "), payload_array[0]));
		}
		else {
			for (i =  input_nof_words - 1; i >= 0; i--) {
				LOG_CLI((BSL_META("%d "), payload_array[i]));
			}
		}
        LOG_CLI((BSL_META("\n")));

    }

	
	switch (mode) {
    case SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_ADD:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0, &payload, &success));
        break;
    default:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
        break;
	}

	if (SOC_SAND_SUCCESS2BOOL(success)) {
		LOG_CLI((BSL_META("Entry updated successfuly\n")));
	}
	else{
		SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Failed updating entry\n")));
	}

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
	arad_pp_dbal_diag_input_parse(int unit, const char *input, uint32 payload_nof_words, uint32 *payload_value, uint32 *input_nof_words)
{
	char *input_copy, *tmp_token, *saveptr = NULL;
	int i = 0;
	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_NULL_CHECK(input);
	SOCDNX_NULL_CHECK(payload_value);
	SOCDNX_NULL_CHECK(input_nof_words);

	
	tmp_token = sal_strchr(input, ':');
	if (!tmp_token) {
        payload_value[0] = _shr_ctoi(input);
		*input_nof_words = 1;
		SOCDNX_FUNC_RETURN;
	}

	i = 1;

	
	while (tmp_token && i < payload_nof_words) {
		i++;
		tmp_token++; 
		tmp_token = sal_strchr(tmp_token, ':');
	}

	
	if (tmp_token) {
		SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_diag_entry_manage - provided input too long")));
	}

	*input_nof_words = i;

	input_copy = sal_strdup(input); 

	tmp_token = sal_strtok_r(input_copy, ":", &saveptr);
	while (tmp_token != NULL && i > 0) {
        payload_value[i - 1] = _shr_ctoi(tmp_token);
		tmp_token = sal_strtok_r(NULL, ":", &saveptr);
		i--;
	}

	sal_free_safe(input_copy);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_table_info_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    const char* str;
    int i,j;
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 lem_prefix = 0;
    uint8 prog_id = 0;

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_CHECK_TABLE_ID_EXIT_WITH_ERR(table_id);

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (!table.is_table_initiated) {
        LOG_CLI((BSL_META("\nThis Table is not initialized already, Table ID: %d\n\n"), table_id));
        goto exit;
    }

    
    LOG_CLI((BSL_META("\nTable information for: %s, ID - %d\n\n"), (table.table_name), table_id));
    LOG_CLI((BSL_META("  Physical DB type: %s\n"), (arad_pp_dbal_physical_db_to_string(table.physical_db_type) )));
    if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        LOG_CLI((BSL_META("  DB prefix not defined\n")));
    }else {
        switch(table.physical_db_type){
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            arad_pp_lem_access_app_to_prefix_get(unit, table.db_prefix, &lem_prefix);
            LOG_CLI((BSL_META("  App type: %s, logical prefix value: 0x%x, prefix length: %d\n"), ARAD_PP_LEM_ACCESS_KEY_TYPE_to_string(unit, table.additional_table_info), lem_prefix, (table.db_prefix_len)));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            if (table.additional_table_info == SOC_DPP_DBAL_ATI_TCAM_DUMMY) {
                LOG_CLI((BSL_META("  Dummy tcam table - using the key but not allocating tcam resources \n")));
            } else{
			    uint32 access_profile_id;
                LOG_CLI((BSL_META("  tcam DB ID: %d\n"), (table.db_prefix)));
				SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, table.db_prefix, 0, &access_profile_id ));
                LOG_CLI((BSL_META("  tcam access profile (HW): %d\n"), (access_profile_id )));
            }
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
            LOG_CLI((BSL_META("  Table prefix: %d, prefix length: %d\n"), table.db_prefix, (table.db_prefix_len)));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
            LOG_CLI((BSL_META("  Table prefix: %d, prefix length: %d\n"), table.db_prefix, (table.db_prefix_len)));
            if (table.additional_table_info == SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION) {
                LOG_CLI((BSL_META("  This tabale defines the master key structure for the KBP\n")));
            }

            if (table.additional_table_info == SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY) {
                LOG_CLI((BSL_META("  this table is only for lookup configuration in the KBP (no key construction)\n")));
            }
            break;

        default:
            break;
        }
    }
    if (table.nof_actions > 0) {
        LOG_CLI((BSL_META("  Table actions:\n")));
        for (i = 0; i < table.nof_actions; i++) {
            LOG_CLI((BSL_META("  \tAction %s\n"), (SOC_PPC_FP_ACTION_TYPE_to_string(table.action[i]) )));
        }
    }
    LOG_CLI((BSL_META("  Entries in table: %d\n"), (table.nof_entries_added_to_table)));
    LOG_CLI((BSL_META("  Table qualifiers: \n")));
    for (i = 0; i < table.nof_qualifiers; i++) {
        LOG_CLI((BSL_META("  \tQualifier - %s, offset (%d), size (%d)\n"), (SOC_PPC_FP_QUAL_TYPE_to_string(table.qual_info[i].qual_type)), (table.qual_info[i].qual_offset), (table.qual_info[i].qual_nof_bits)));
    }
    LOG_CLI((BSL_META("  Number of programs associated to this table: %d\n\n"), (table.nof_table_programs)));
    for (i = 0; i < table.nof_table_programs; i++) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_string(unit, table.table_programs[i].stage, table.table_programs[i].program_id, &str, &prog_id));
        LOG_CLI((BSL_META("  \tProgram: %s, ID %d, stage: %s \n"), str, (table.table_programs[i].program_id), SOC_PPC_FP_DATABASE_STAGE_to_string(table.table_programs[i].stage)));
        if (!SOC_IS_JERICHO(unit) && table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) {
            uint8 key_id = table.table_programs[i].key_id;
            if (key_id == ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL) {
                key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
            }
            LOG_CLI((BSL_META("  \t\tLookup Key: %s, lookup number %d, allocated bits for key = %d\n"), arad_pp_dbal_key_id_to_string(unit, key_id), (table.table_programs[i].lookup_number), table.table_programs[i].nof_bits_used_in_key));
        }else{
            LOG_CLI((BSL_META("  \t\tLookup Key: %s, lookup number %d, allocated bits for key = %d\n"), arad_pp_dbal_key_id_to_string(unit, table.table_programs[i].key_id), (table.table_programs[i].lookup_number), table.table_programs[i].nof_bits_used_in_key));
        }
        for (j = 0; j < table.nof_qualifiers; j++) {
            LOG_CLI((BSL_META("  \t\tCE %d assigned for qualifier %s, number of bits = %d\n"), (table.table_programs[i].ce_assigned[j]), (SOC_PPC_FP_QUAL_TYPE_to_string(table.qual_info[j].qual_type)), table.qual_info[j].qual_nof_bits));
        }
        LOG_CLI((BSL_META("\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_tables_dump(int unit, int is_full_info)
{
    int table_id;
    SOC_DPP_DBAL_TABLE_INFO  table;
    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\nExisting dbal tables\n--------------------\n")));

    for (table_id = 0; table_id < SOC_DPP_DBAL_SW_NOF_TABLES; table_id++) {
        if (is_full_info) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_table_info_dump(unit, table_id));
        }else {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
            if (table.is_table_initiated) {
                
                LOG_CLI((BSL_META("\nTable: %s, ID (%d)"), (table.table_name), table_id));
            }
        }
     }
    LOG_CLI((BSL_META("\n")));
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_ce_per_program_dump(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage)
{

    uint32              ce_rsrc[1], last_ce_ndx = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
    ARAD_PP_KEY_CE_ID   ce_ndx;
    uint8               ce_size;
    const char*         str;
    uint8               prog_id = 0;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    if (stage < 0 || stage >= SOC_PPC_NOF_FP_DATABASE_STAGES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid stage %d. \n\rThe range is: 0 - SOC_PPC_NOF_FP_DATABASE_STAGES-1.\n\r"), stage));
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_string(unit, stage, program_id, &str, &prog_id));
    LOG_CLI((BSL_META("\n Genral program information for %s, Stage = %s \n\n"), str, SOC_PPC_FP_DATABASE_STAGE_to_string(stage)));

    switch (stage) {
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_dbal_program_info_dump(unit, program_id));
        break;

    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_print_vt_program_look_info(unit, program_id));
        break;

    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_print_tt_program_look_info(unit, program_id));
        break;

    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
    case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
        break;

    
    
    default:
        goto exit;
        break;
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, &(ce_rsrc[0])));

    if(ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) {
        last_ce_ndx = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX*ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB + 1;
    }

    LOG_CLI((BSL_META("\nCopy engines that are in use:\n")));
    LOG_CLI((BSL_META("-----------------------------\n")));
    for (ce_ndx = 0; ce_ndx <= last_ce_ndx; ce_ndx++) {
        ce_size = 16;

        
        
        if ((SHR_BITGET(ce_rsrc,ce_ndx))) {

            if ((arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx))){
                ce_size = 32;
            }
            LOG_CLI((BSL_META("\tCE %d: size %d bits - in use\n"), ce_ndx, ce_size));
        }
    }

    for (i = 0; i < ARAD_PMF_LOW_LEVEL_NOF_KEYS; i++) {
        LOG_CLI((BSL_META("\n %s Copy Engine usage: \n"), arad_pp_dbal_key_id_to_string(unit, i)));
        for (ce_ndx = 0; ce_ndx < ARAD_PMF_LOW_LEVEL_CE_NDX_MAX+1; ce_ndx++) {
            arad_pmf_ce_for_key_dump(unit, stage, program_id, i, ce_ndx, 0, 0);
        }
        if(ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1){
            for (ce_ndx = 0; ce_ndx < ARAD_PMF_LOW_LEVEL_CE_NDX_MAX+1; ce_ndx++) {
                arad_pmf_ce_for_key_dump(unit, stage, program_id, i, ce_ndx, 1, 0);
            }
        }
    }

    LOG_CLI((BSL_META("\n")));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_qualifier_to_instruction_dump(int unit,SOC_PPC_FP_DATABASE_STAGE stage, int is_32b_ce, int nof_bits, SOC_PPC_FP_QUAL_TYPE qual_type)
{
    uint32 ce_inst;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_to_instruction(unit,stage, is_32b_ce, nof_bits, qual_type,0, &ce_inst));

    LOG_CLI((BSL_META("qualifier = %s, calculated value = %d \n"), SOC_PPC_FP_QUAL_TYPE_to_string(qual_type), ce_inst));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_dbal_last_packet_signal_dump(int unit, int core_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, int lookup_number, SOC_PPC_FP_DATABASE_STAGE  stage)
{
    uint32 ind;
    uint32 val[ARAD_PP_DIAG_DBG_VAL_LEN];
    SOC_DPP_DBAL_TABLE_INFO *table = NULL;
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE key_signal_id[5];
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE hit_signal_id = 0, res_signal_id = 0, opcode_signal_id = 0;
    int nof_key_signals = 1;
    int size_of_signal_in_bits;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(table, SOC_DPP_DBAL_TABLE_INFO, 1, "arad_pp_dbal_last_packet_signal_dump.table");
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, table));

    LOG_CLI((BSL_META("\n   A lookup was made in table: %s (%d), lookup information:\n\n"), table->table_name, table_id ));

    SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_ids_get(unit, table->physical_db_type, lookup_number, stage, key_signal_id, &res_signal_id, &hit_signal_id, &opcode_signal_id, &nof_key_signals));

    if ((table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS || (table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP))) {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if (table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS){
            if (lookup_number == 2) {
                LOG_CLI((BSL_META("\t calling KAPS diagnostics (both lookup are printed, you are interested in Second lookup) \n\n")));
            }else{
                LOG_CLI((BSL_META("\t calling KAPS diagnostics (both lookup are printed, you are interested in First lookup) \n\n")));
            }

            SOCDNX_IF_ERR_EXIT(soc_jer_pp_diag_kaps_lkup_info_get(unit, core_id));
        }else{
            LOG_CLI((BSL_META("\t KBP last packet diagnostics are not supported, use diag pp kbp last \n\n")));
        }
#endif
    }else{

        LOG_CLI((BSL_META("    KEY: ")));

        SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, key_signal_id[0], val, &size_of_signal_in_bits));
        if (nof_key_signals > 1) {
            uint32 segment_val[ARAD_PP_DIAG_DBG_VAL_LEN];
            int last_pos = size_of_signal_in_bits;
            for (ind = 1; ind < nof_key_signals; ind++) {
                SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, key_signal_id[ind], segment_val, &size_of_signal_in_bits));
                SHR_BITCOPY_RANGE(&val[0], last_pos, segment_val, 0, size_of_signal_in_bits);
                last_pos += size_of_signal_in_bits;
            }
        }

        LOG_CLI((BSL_META("\t ")));
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_dump(unit, stage, table_id, NULL, val));

        LOG_CLI((BSL_META("\n\n")));

        if ((table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP)) {
            LOG_CLI((BSL_META("    OPCODE: ")));
            SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, opcode_signal_id, val, &size_of_signal_in_bits));
            LOG_CLI((BSL_META("0x%08x "), val[0]));
        }

        SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, hit_signal_id, val, &size_of_signal_in_bits));
        if ((val[0] != 0)) {
            int num_of_int;
            SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, res_signal_id, val, &size_of_signal_in_bits));
            num_of_int = size_of_signal_in_bits/32 + ((size_of_signal_in_bits%32)?1:0);
            LOG_CLI((BSL_META("    RESULT: ")));
            LOG_CLI((BSL_META("\t Lookup found, Lookup result: ")));
            for (ind = 0; ind < num_of_int; ind++) {
                LOG_CLI((BSL_META("0x%08x "), val[ind])); 
            }
            LOG_CLI((BSL_META("\n")));
        } else {
            LOG_CLI((BSL_META("\t Lookup not found any result\n")));
        }
    }

exit:
    SOCDNX_FREE(table);
    SOCDNX_FUNC_RETURN;
}



STATIC uint32
    arad_pp_dbal_tcam_prefix_to_table_id(int unit, int tcam_db_ndx, SOC_DPP_DBAL_SW_TABLE_IDS* table_id)
{
    uint8 found = 0;
    int i;
    SOC_DPP_DBAL_TABLE_INFO  table;

    SOCDNX_INIT_FUNC_DEFS;

    (*table_id) = 0;

    for (i = 0; i < SOC_DPP_DBAL_SW_NOF_TABLES; i++){
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));
        if ((table.is_table_initiated) && (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) && (table.db_prefix == tcam_db_ndx)) {
            if (found) {
                if(table.nof_entries_added_to_table > 0){
                    (*table_id) = i;
                }
            }else{
                (*table_id) = i;
            }
            found = 1;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}

uint32
    arad_pp_dbal_tcam_prefix_table_dump(int unit)
{
    uint32 access_profile_id;
    uint8 valid, found = 0;
    int tcam_db_ndx, i;
    SOC_DPP_DBAL_TABLE_INFO  table;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\nTCAM Logical database mapping\n")));
    LOG_CLI((BSL_META("------------------------------\n")));

    for (tcam_db_ndx = 0; tcam_db_ndx < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_ndx++){

        SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_ndx, &valid));
        if (valid) {
            SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_ndx, 0, &access_profile_id ));
            LOG_CLI((BSL_META("tcam_db_id (db_prefix) %02d is mapped to HW profile (%d) "), tcam_db_ndx, access_profile_id));
            found = 0;
            for (i = 0; i < SOC_DPP_DBAL_SW_NOF_TABLES; i++){
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));
                if ((table.is_table_initiated) && (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) && (table.db_prefix == tcam_db_ndx)) {
                    if (!found) {
                        LOG_CLI((BSL_META("DBAL tables related: %s (%d)"), table.table_name, i));
                    }else{
                        LOG_CLI((BSL_META(", %s (%d)"), table.table_name, i));
                    }
                    found = 1;
                }
            }
            if (!found) {
                LOG_CLI((BSL_META("no DBAL tables related to this profile")));
            }
            LOG_CLI((BSL_META("\n")));
        }
    }
    LOG_CLI((BSL_META("\n")));

exit:
    SOCDNX_FUNC_RETURN;

}
uint32
    arad_pp_dbal_isem_prefix_table_dump(int unit)
{
    SOC_DPP_DBAL_SW_TABLE_IDS table_id = 0;
    int prefix = 0, available_prefixes = 0, count;
    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; 
    int max_prefix_value = ((1 << max_prefix_len) - 1), i;
    SOC_DPP_DBAL_TABLE_INFO  table;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\nSEM A Logical database mapping\n")));
    LOG_CLI((BSL_META("------------------------------\n")));
    LOG_CLI((BSL_META("(for static tables it is possible that prefix is allocated for more than one table)\n\n")));
    for (prefix = 0; prefix <=  max_prefix_value; prefix++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.get(unit, prefix, &table_id ));
        if (table_id != 0) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A, prefix, &count));
            if (count > 1) {
                LOG_CLI((BSL_META("Prefix %02d is mapped to multiple tables (%d): "), prefix, count));
                for (i = 0; i < SOC_DPP_DBAL_SW_NOF_TABLES; i++){
                    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));
                    if ((table.is_table_initiated) && (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A) && (table.db_prefix == prefix)) {
                        LOG_CLI((BSL_META(" %s (%d)"), table.table_name, i));
                    }
                }
                LOG_CLI((BSL_META("\n")));
            }else{
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
                LOG_CLI((BSL_META("Prefix %02d is mapped to table %s (%d)\n"), prefix, table.table_name, table_id));
            }
        } else {
            available_prefixes++;
        }
    }
    LOG_CLI((BSL_META("Toatal prefixes %d, available prefixes = %d\n\n"), max_prefix_value+1, available_prefixes));

    available_prefixes = 0;
    LOG_CLI((BSL_META("\nSEM B Logical database mapping\n")));
    LOG_CLI((BSL_META("------------------------------\n")));
    LOG_CLI((BSL_META("(for static tables it is possible that prefix is allocated for more than one table)\n\n")));
    for (prefix = 0; prefix <=  max_prefix_value; prefix++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.get(unit, prefix, &table_id ));
        if (table_id != 0) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B, prefix, &count));
            if (count > 1) {
                LOG_CLI((BSL_META("Prefix %02d is mapped to multiple tables (%d): "), prefix, count));
                for (i = 0; i < SOC_DPP_DBAL_SW_NOF_TABLES; i++){
                    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));
                    if ((table.is_table_initiated) && (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B) && (table.db_prefix == prefix)) {
                        LOG_CLI((BSL_META(" %s (%d)"), table.table_name, i));
                    }
                }
                LOG_CLI((BSL_META("\n")));
            }else{
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
                LOG_CLI((BSL_META("Prefix %02d is mapped to table %s (%d)\n"), prefix, table.table_name, table_id));
            }
        } else {
            available_prefixes++;
        }
    }
    LOG_CLI((BSL_META("Toatal prefixes %d, available prefixes = %d\n\n"), max_prefix_value+1, available_prefixes));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_lem_prefix_table_dump(int unit)
{
    uint8 app_id = 0, found;
    int prefix = 0, i;
    SOC_DPP_DBAL_TABLE_INFO  table;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\nLEM Logical database mapping \n")));
    LOG_CLI((BSL_META("---------------------------- \n")));
    for (prefix = 0; prefix < ARAD_PP_LEM_ACCESS_NOF_PREFIXES; ++prefix) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.fec.lem_prefix_mapping.get(unit, prefix, &app_id));
        if (app_id == ARAD_PP_FLP_MAP_PROG_NOT_SET) {
            
        } else{
            found = 0;
            LOG_CLI((BSL_META("Prefix %02d is mapped to %s (%d)"), prefix, arad_pp_lem_access_app_id_to_app_name(unit, app_id), app_id));
            for (i = 0; i < SOC_DPP_DBAL_SW_NOF_TABLES; i++){
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));
                if ((table.is_table_initiated) && (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM) && (table.additional_table_info == app_id)) {
                    if (!found) {
                        LOG_CLI((BSL_META("DBAL tables related:")));
                    }
                    LOG_CLI((BSL_META(", %s (%d)"), table.table_name, i));
                    found = 1;
                }
            }
            if (!found) {
                LOG_CLI((BSL_META(" no DBAL tables related to this profile")));
            }
            LOG_CLI((BSL_META("\n")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_last_packet_dump(int unit, int core_id)
{
#define NUM_OF_STAGES  3
    int prog_id[NUM_OF_STAGES], i, stage, j, num_of_progs[NUM_OF_STAGES] = {0};
    int first_table_id[NUM_OF_STAGES];
    int last_table_id[NUM_OF_STAGES];
    SOC_DPP_DBAL_TABLE_INFO *table = NULL;
    char* stage_names[] = {"VT", "TT", "FLP"};
    const char *str;
    uint8 found;
    uint8 prog_id_stage = 0;

    SOC_PPC_FP_DATABASE_STAGE  actual_stages[3];
    int stage_vt = 0, stage_tt = 1, stage_flp = 2;
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO routing_info;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&routing_info);

    

    actual_stages[stage_vt] = SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT;
    actual_stages[stage_tt] = SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT;
    actual_stages[stage_flp] = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;

    last_table_id[stage_tt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST + 1;
    last_table_id[stage_vt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST + 1;
    last_table_id[stage_flp] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST + 1;

    first_table_id[stage_vt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST;
    first_table_id[stage_tt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST;
    first_table_id[stage_flp] = SOC_DPP_DBAL_SW_TABLE_ID_INVALID +1;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_print_last_vtt_program_data(unit,core_id, 0, &prog_id[stage_vt], &prog_id[stage_tt], &(num_of_progs[stage_vt]), &(num_of_progs[stage_tt])));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_flp_access_print_last_programs_data(unit, core_id, 0, &prog_id[stage_flp], &num_of_progs[stage_flp]));

    LOG_CLI((BSL_META("\n\n PACKET INFORMATION BY STAGE FOR CORE = %d\n"), core_id));
    LOG_CLI((BSL_META("\n -----------------------------------------\n")));
    SOCDNX_ALLOC(table, SOC_DPP_DBAL_TABLE_INFO, 1, "arad_pp_dbal_last_packet_dump.table");
    for (stage = 0; stage < NUM_OF_STAGES; stage++) {

        if (prog_id[stage] == -1) { 
            LOG_CLI((BSL_META("  No program was invoked for stage %s.\n"), stage_names[stage]));
            continue;
        } else {
            LOG_CLI((BSL_META("\n ****************************** Stage - %s *********************************\n\n"), stage_names[stage]));
            if (num_of_progs[stage] != 1) {
                LOG_CLI((BSL_META("  WARNING!!! more than one program was invoked at this stage. "
                                  "Info may be incorrect, to receive verified info please resend last packet and run diag command again\n\n")));
            }
            if (stage == stage_flp) {
                SOCDNX_IF_ERR_EXIT(arad_pp_prog_index_to_flp_app_get(unit, prog_id[stage], &prog_id_stage));
                
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_string(unit, actual_stages[stage], prog_id_stage, &str, &prog_id_stage));
            } else {
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_string(unit, actual_stages[stage], prog_id[stage], &str, &prog_id_stage));
            }
            LOG_CLI((BSL_META("  Program %s was invoked for this stage\n"),str));
        }
        found = 0;

        
        for (i = first_table_id[stage]; i < last_table_id[stage]; i++) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, table));

            if (!(table->is_table_initiated)) {
                continue;
            }

            
            if ((table->additional_table_info == SOC_DPP_DBAL_ATI_TCAM_DUMMY) && (table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM)) {
                continue;
            }

            
            if ((table->additional_table_info == SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION) && (table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP)) {
                continue;
            }

            for (j = 0; j < table->nof_table_programs; j++) {
                if ((table->table_programs[j].program_id == prog_id[stage]) && (table->table_programs[j].stage == actual_stages[stage])) {
                    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_last_packet_signal_dump(unit, core_id, i, table->table_programs[j].lookup_number, actual_stages[stage]));
                    found = 1;
                }
            }
        }
        
        for (i = SOC_DPP_DBAL_SW_TABLE_DYNAMIC_BASE_ID; i < SOC_DPP_DBAL_SW_NOF_TABLES; i++) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, table));

            if (!(table->is_table_initiated)) {
                continue;
            }

            if (table->table_programs[0].stage != stage) {
                continue;
            }

            for (j = 0; j < table->nof_table_programs; j++) {
                if ((table->table_programs[j].program_id == prog_id[stage]) && (table->table_programs[j].stage == actual_stages[stage])) {
                    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_last_packet_signal_dump(unit, core_id, i, table->table_programs[j].lookup_number, actual_stages[stage]));
                    found = 1;
                }
            }
        }

        if (found == 0) {
            LOG_CLI((BSL_META("  No information provided for this program \n\n")));
        }else{
            if (actual_stages[stage] == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
                soc_ppd_diag_frwrd_decision_get(unit, core_id, &payload.dest);
                arad_pp_frwrd_em_dest_to_fec(unit, &payload, &routing_info);
                LOG_CLI((BSL_META("\n\n-------------------------------------------------------------------------------------------------------\n")));
                LOG_CLI((BSL_META("|  Forwarding result from the FLP to next block: encoded value = 0x%x, Decoded: {Type %s, ID %d}\n"), payload.dest, SOC_PPC_FRWRD_DECISION_TYPE_to_string(routing_info.frwrd_decision.type), routing_info.frwrd_decision.dest_id));
                LOG_CLI((BSL_META("-------------------------------------------------------------------------------------------------------\n")));
            }
        }
    } 
    LOG_CLI((BSL_META("\n")));
exit:
#undef NUM_OF_STAGES
    SOCDNX_FREE(table);
    SOCDNX_FUNC_RETURN;
}






const char*
    arad_pp_dbal_physical_db_to_string(SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type)
{
    const char* str = NULL;

    switch(physical_db_type)
    {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            str = "LEM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            str = "TCAM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
            str = "KBP";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            str = "ISEM_A";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
            str = "ISEM_B";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
            str = "KAPS";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_ESEM:
            str = "ESEM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM1:
            str = "OEM_A";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM2:
            str = "OEM_B";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_RMAP:
            str = "RMEP_EM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_GLEM:
            str = "GLEM";
            break;
        default:
            str = " Unknown physical DB";
            break;
    }
    return str;
}


int
    arad_pp_dbal_program_to_string(int unit, SOC_PPC_FP_DATABASE_STAGE stage, int cam_line, const char**str, uint8* prog_id)
{


    switch (stage) {
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
        *str = arad_pp_flp_prog_id_to_prog_name(unit, cam_line);
        break;
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
        arad_pp_isem_access_program_sel_line_to_program_id(unit, cam_line, 1, prog_id);
        *str = arad_pp_isem_access_print_vt_program_data(unit, (*prog_id), 0);
        break;
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
        arad_pp_isem_access_program_sel_line_to_program_id(unit, cam_line, 0, prog_id);
        *str =  arad_pp_isem_access_print_tt_program_data(unit, (*prog_id), 0);
        break;
    default:
        *str = "Unknown";
        break;
    }
    
    return 0;
}

const char*
    arad_pp_dbal_key_id_to_string(int unit, uint8 key_id)
{
    if (key_id == ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL) {
        key_id = 0xFF;
    }
    switch (key_id) {
    case SOC_DPP_DBAL_PROGRAM_KEY_A:
        return "key A";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_B:
        return "key B";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_C:
        return "key C";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_D:
        return "key D";
        break;

    case 0xFF:
        return "HW key";
        break;

    default:
        return "";
        LOG_CLI((BSL_META("Key %d"), key_id));
        break;
    }
}

#undef DBAL_DIAGNOSTICS_FUNCTIONS

#define SIGNAL_MNGR_FUNCTIONS



STATIC SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO arad_pp_signal_mngr_arad_signals_table[SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES] = {
    
    { TRUE,  ARAD_IHB_ID,  22,       0,     234,  161,      74,        0},
    { TRUE,  ARAD_IHB_ID,  22,       0,     116,   43,      74,        0},
    { TRUE,  ARAD_IHB_ID,  21,       0,      88,   46,      43,        0},
    { TRUE,  ARAD_IHB_ID,  21,       0,      42,    0,      43,        0},
    { TRUE,  ARAD_IHB_ID,  21,       0,      91,   91,       1,        0},
    { TRUE,  ARAD_IHB_ID,  21,       0,      45,   45,       1,        0},
    { TRUE,  ARAD_IHB_ID,  13,       0,      43,    0,      43,        0},
    { FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    { FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    { FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    { TRUE,  ARAD_IHB_ID,  14,       0,      15,    1,      15,        0},
    { FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    { FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    { FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    { TRUE,  ARAD_IHB_ID,  15,       1,     235,   76,     160,        0},
    { TRUE,  ARAD_IHB_ID,  16,       0,     124,   85,      40,        0},
    { TRUE,  ARAD_IHB_ID,  16,       0,     125,  125,       1,        0},
    { TRUE,  ARAD_IHB_ID,   3,       0,       7,    0,       8,        0},
    { TRUE,  ARAD_IHB_ID,   3,       0,     255,    8,     248,        0},
    { TRUE,  ARAD_IHB_ID,   3,       1,     255,    0,     256,        0},
    { TRUE,  ARAD_IHB_ID,   3,       2,     255,    0,     256,        0},
    { TRUE,  ARAD_IHB_ID,   3,       3,     255,    0,     256,        0},
    { TRUE,  ARAD_IHB_ID,   3,       4,       7,    0,       8,        0},
	{ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
	{ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    { TRUE,  ARAD_IHB_ID,   4,       0,     128,    1,     128,        0},
    { TRUE,  ARAD_IHB_ID,   4,       0,       0,    0,       1,        0},
    { TRUE,  ARAD_IHP_ID,  13,       0,      81,   41,      41,        0},
    { TRUE,  ARAD_IHP_ID,  14,       0,      33,   18,      16,        0},
    { TRUE,  ARAD_IHP_ID,  14,       0,      35,   35,       1,        0},
    { TRUE,  ARAD_IHP_ID,  15,       0,      81,   41,      41,        0},
    { TRUE,  ARAD_IHP_ID,  16,       0,      33,   18,      16,        0},
    { TRUE,  ARAD_IHP_ID,  16,       0,      35,   35,       1,        0},
    { TRUE,  ARAD_IHP_ID,  11,       0,     255,  166,      90,        0},
    { TRUE,  ARAD_IHP_ID,  11,       1,      69,    0,      70,        0},
    { TRUE,  ARAD_IHP_ID,  12,       0,      82,   43,      40,        0},
    { TRUE,  ARAD_IHP_ID,  12,       0,      83,   83,       1,        0},
    { TRUE,  ARAD_IHP_ID,  13,       0,      40,    0,      41,        0},
    { TRUE,  ARAD_IHP_ID,  14,       0,      15,    0,      16,        0},
    { TRUE,  ARAD_IHP_ID,  14,       0,      17,   17,       1,        0},
    { TRUE,  ARAD_IHP_ID,  15,       0,      40,    0,      41,        0},
    { TRUE,  ARAD_IHP_ID,  16,       0,      15,    0,      16,        0},
    { TRUE,  ARAD_IHP_ID,  16,       0,      17,   17,       1,        0},
    { TRUE,  ARAD_IHP_ID,  11,       0,     165,    6,     160,        0},
    { TRUE,  ARAD_IHP_ID,  12,       0,      40,    1,      40,        0},
    { TRUE,  ARAD_IHP_ID,  12,       0,      41,   41,       1,        0},
    { TRUE,  ARAD_IHP_ID,   4,       1,     225,  214,      11,        0},
    { FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
};


STATIC SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO arad_pp_signal_mngr_jericho_signals_table[SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES] = {
    
    { TRUE,  ARAD_IHP_ID,  26,       0,     205,  126,      80,        0},
    { TRUE,  ARAD_IHP_ID,  26,       0,      79,    0,      80,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      92,   48,      45,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      44,    0,      44,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      95,   95,       1,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      47,   47,       1,        0},
    { TRUE,  ARAD_IHP_ID,  30,       1,     255,  252,       4,        0},
    { TRUE,  ARAD_IHP_ID,  30,       2,     155,    0,     156,        0},
    { TRUE,  ARAD_IHP_ID,  30,       0,      77,    0,     160,       82},
    { TRUE,  ARAD_IHP_ID,  30,       1,     255,  174,     160,       78},
    { TRUE,  ARAD_IHP_ID,  31,       0,     119,  100,      20,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,      59,   40,      20,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,       9,    8,       2,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,      99,  100,       2,        0},
    { TRUE,  ARAD_IHP_ID,  28,       0,     255,  166,     160,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      98,   51,      48,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      99,   99,       1,        0},
    { TRUE,  ARAD_IHP_ID,  32,       0,       7,    0,       8,        0},
    { TRUE,  ARAD_IHP_ID,  32,       0,     255,    8,     248,        0},
    { TRUE,  ARAD_IHP_ID,  32,       1,     255,    0,     256,        0},
    { TRUE,  ARAD_IHP_ID,  32,       2,     255,    0,     256,        0},
    { TRUE,  ARAD_IHP_ID,  32,       3,     255,    0,     256,        0},
    { TRUE,  ARAD_IHP_ID,  32,       4,       7,    0,       8,        0},
	{ TRUE,  ARAD_IHP_ID,   4,       8,     255,  167,      89,        0},   
	{ TRUE,  ARAD_IHP_ID,   4,       9,      38,    0,      39,        0},   
    { TRUE,  ARAD_IHP_ID,   4,       9,     166,   39,     128,        0},   
    { TRUE,  ARAD_IHP_ID,   4,       9,     167,  167,       1,        0},   
    { TRUE,  ARAD_IHP_ID,  20,       0,      99,   50,      50,        0},
    { TRUE,  ARAD_IHP_ID,  21,       0,      35,   19,      17,        0},
    { TRUE,  ARAD_IHP_ID,  21,       0,      37,   37,       1,        0},
    { TRUE,  ARAD_IHP_ID,  22,       0,      99,   50,      50,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      35,   19,      17,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      37,   37,       1,        0},
    { TRUE,  ARAD_IHP_ID,  24,       0,     255,  172,      84,        0},
    { TRUE,  ARAD_IHP_ID,  24,       1,      75,    0,      76,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      98,   51,      48,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      99,   99,       1,        0},
    { TRUE,  ARAD_IHP_ID,  20,       0,      49,    0,      50,        0},
    { TRUE,  ARAD_IHP_ID,  21,       0,      16,    0,      17,        0},
    { TRUE,  ARAD_IHP_ID,  21,       0,      18,   18,       1,        0},
    { TRUE,  ARAD_IHP_ID,  22,       0,      49,    0,      50,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      16,    0,      17,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      18,   18,       1,        0},
    { TRUE,  ARAD_IHP_ID,  24,       0,     165,    6,     160,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      48,    1,      48,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      49,   49,       1,        0},
    { TRUE,  ARAD_IHP_ID,   3,       1,     215,  201,      15,        0},
    { TRUE,  ARAD_IHP_ID,   3,       1,     200,  195,       6,        0},
};

STATIC SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO arad_pp_signal_mngr_qax_signals_table[SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES] = {
    
    { TRUE,  ARAD_IHP_ID,  28,       0,     205,  126,      80,        0},
    { TRUE,  ARAD_IHP_ID,  28,       0,      79,    0,      80,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      44,    0,      44,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      92,   48,      45,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      95,   95,       1,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      47,   47,       1,        0},
    { TRUE,  ARAD_IHP_ID,  32,       1,     255,  252,       4,        0},
    { TRUE,  ARAD_IHP_ID,  32,       2,     155,    0,     156,        0},
    { TRUE,  ARAD_IHP_ID,  32,       0,     166,    7,     160,        0},
    { TRUE,  ARAD_IHP_ID,  32,       1,     255,  174,     160,       78},
    { TRUE,  ARAD_IHP_ID,  33,       0,     119,  100,      20,        0},
    { TRUE,  ARAD_IHP_ID,  33,       0,      59,   40,      20,        0},
    { TRUE,  ARAD_IHP_ID,  33,       0,       9,    8,       2,        0},
    { TRUE,  ARAD_IHP_ID,  33,       0,      99,  100,       2,        0},
    { TRUE,  ARAD_IHP_ID,  30,       0,     255,  166,     160,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,      98,   51,      48,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,      99,   99,       1,        0},
    { TRUE,  ARAD_IHP_ID,  34,       0,       7,    0,       8,        0},
    { TRUE,  ARAD_IHP_ID,  34,       0,     255,    8,     248,        0},
    { TRUE,  ARAD_IHP_ID,  34,       1,     255,    0,     255,        0},
    { TRUE,  ARAD_IHP_ID,  34,       2,     255,    0,     255,        0},
    { TRUE,  ARAD_IHP_ID,  34,       3,     255,    0,     255,        0},
    { TRUE,  ARAD_IHP_ID,  34,       4,       7,    0,       8,        0},
	{ TRUE,  ARAD_IHP_ID,   4,       8,     255,  232,      24,        0},   
	{ TRUE,  ARAD_IHP_ID,   4,       9,     103,    0,     104,        0},   
    { TRUE,  ARAD_IHP_ID,   4,       9,     231,  104,     128,        0},   
    { TRUE,  ARAD_IHP_ID,   4,       9,     232,  232,       1,        0},   
    { TRUE,  ARAD_IHP_ID,  24,       0,      99,   50,      50,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      33,   18,      16,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      35,   35,       1,        0},
    { TRUE,  ARAD_IHP_ID,  26,       0,      99,   50,      50,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      33,   18,      16,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      35,   35,       1,        0},
    { TRUE,  ARAD_IHP_ID,  14,       0,     255,  166,      90,        0},
    { TRUE,  ARAD_IHP_ID,  14,       1,      69,    0,      70,        0},
    { TRUE,  ARAD_IHP_ID,  15,       0,      98,   51,      48,        0},
    { TRUE,  ARAD_IHP_ID,  15,       0,      99,   99,       1,        0},
    { TRUE,  ARAD_IHP_ID,  24,       0,      49,    0,      50,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      15,    0,      17,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      17,   17,       1,        0},
    { TRUE,  ARAD_IHP_ID,  26,       0,      49,    0,      50,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      15,    0,      16,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      17,   17,       1,        0},
    { TRUE,  ARAD_IHP_ID,  14,       0,     159,    0,     160,        0},
    { TRUE,  ARAD_IHP_ID,  15,       0,      48,    1,      48,        0},
    { TRUE,  ARAD_IHP_ID,  15,       0,      49,   49,       1,        0},
    { TRUE,  ARAD_IHP_ID,   3,       2,      46,   32,      15,        0},
    { TRUE,  ARAD_IHP_ID,   3,       2,      31,   26,       6,        0},
};

STATIC SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO arad_pp_signal_mngr_jericho_plus_signals_table[SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES] = {
    
    { TRUE,  ARAD_IHP_ID,  26,       0,     205,  126,      80,        0},
    { TRUE,  ARAD_IHP_ID,  26,       0,      79,    0,      80,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      44,    0,      45,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      92,   48,      45,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      95,   95,       1,        0},
    { TRUE,  ARAD_IHP_ID,  27,       0,      47,   47,       1,        0},
    { TRUE,  ARAD_IHP_ID,  30,       1,     255,  252,       4,        0},
    { TRUE,  ARAD_IHP_ID,  30,       2,     155,    0,     156,        4},
    { TRUE,  ARAD_IHP_ID,  30,       0,     166,    7,     160,        0},
    { TRUE,  ARAD_IHP_ID,  30,       1,     255,  252,       4,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,     119,  100,      20,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,      59,   40,      20,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,       9,    8,       2,        0},
    { TRUE,  ARAD_IHP_ID,  31,       0,      99,   98,       2,        0},
    { TRUE,  ARAD_IHP_ID,  28,       0,     255,  166,      90,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      98,   51,      48,        0},
    { TRUE,  ARAD_IHP_ID,  29,       0,      99,   99,       1,        0},
    { TRUE,  ARAD_IHP_ID,  32,       0,       7,    0,       8,        0},
    { TRUE,  ARAD_IHP_ID,  32,       0,     255,    8,     248,        0},
    { TRUE,  ARAD_IHP_ID,  32,       1,     255,    0,     256,        0},
    { TRUE,  ARAD_IHP_ID,  32,       2,     255,    0,     256,        0},
    { TRUE,  ARAD_IHP_ID,  32,       3,     255,    0,     256,        0},
    { TRUE,  ARAD_IHP_ID,  32,       4,       7,    0,       8,        0},
    { TRUE,  ARAD_IHP_ID,   4,       8,     255,  237,      19,        0},   
    { TRUE,  ARAD_IHP_ID,   4,       9,     108,    0,     109,        0},   
    { TRUE,  ARAD_IHP_ID,   4,       9,     236,  109,     128,        0},   
    { TRUE,  ARAD_IHP_ID,   4,       9,     237,  237,       1,        0},   
    { TRUE,  ARAD_IHP_ID,  22,       0,      99,   50,      50,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      35,   19,      17,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      37,   37,       1,        0},
    { TRUE,  ARAD_IHP_ID,  24,       0,      99,   50,      50,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      35,   19,      17,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      37,   37,       1,        0},
    { TRUE,  ARAD_IHP_ID,  12,       0,     255,  166,      90,        0},
    { TRUE,  ARAD_IHP_ID,  12,       1,      69,    0,      70,       90},
    { TRUE,  ARAD_IHP_ID,  13,       0,      98,   51,      48,        0},
    { TRUE,  ARAD_IHP_ID,  13,       0,      99,   99,       1,        0},
    { TRUE,  ARAD_IHP_ID,  22,       0,      49,    0,      50,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      16,    0,      17,        0},
    { TRUE,  ARAD_IHP_ID,  23,       0,      17,   17,       1,        0},
    { TRUE,  ARAD_IHP_ID,  24,       0,      49,    0,      50,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      16,    0,      17,        0},
    { TRUE,  ARAD_IHP_ID,  25,       0,      18,   18,       1,        0},
    { TRUE,  ARAD_IHP_ID,  12,       0,     159,    0,     160,        0},
    { TRUE,  ARAD_IHP_ID,  13,       0,      48,    1,      48,        0},
    { TRUE,  ARAD_IHP_ID,  13,       0,      49,   49,       1,        0},
    { TRUE,  ARAD_IHP_ID,   3,       2,      35,   30,       6,        0},
    { TRUE,  ARAD_IHP_ID,   3,       2,      35,   30,       6,        0},
};

void
    arad_pp_signal_mngr_signal_info_get(int unit, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO* sig_info)
{
    if(SOC_IS_QAX(unit)) {
        sal_memcpy(sig_info, &arad_pp_signal_mngr_qax_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));
    } else if(SOC_IS_JERICHO_PLUS(unit)) {
        sal_memcpy(sig_info, &arad_pp_signal_mngr_jericho_plus_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));
    } else if(SOC_IS_JERICHO(unit)) {
        sal_memcpy(sig_info, &arad_pp_signal_mngr_jericho_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));
    } else if(SOC_IS_ARADPLUS(unit)){
        
        sal_memcpy(sig_info, &arad_pp_signal_mngr_arad_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));
    } else{
        sal_memcpy(sig_info, &arad_pp_signal_mngr_arad_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));
    }
}

uint32
    arad_pp_signal_mngr_signal_get(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, uint32 val[8], int* size_of_signal_in_bits)
{
    uint32 val_aligned[ARAD_PP_DIAG_DBG_VAL_LEN] = {0}; 
    SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO sig_info;
    ARAD_PP_DIAG_REG_FIELD prm_fld;
    int num_of_bytes = 0, ind;
    int offset_in_bytes = 0;

    SOCDNX_INIT_FUNC_DEFS;

    arad_pp_signal_mngr_signal_info_get(unit, signal_id, &sig_info);
    if(sig_info.is_valid) {
        (*size_of_signal_in_bits) = sig_info.signal_length_in_bits;
        prm_fld.base = (sig_info.addr_high << 16) + sig_info.addr_low;
        prm_fld.lsb = sig_info.lsb;
        prm_fld.msb = sig_info.msb;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_diag_dbg_val_get_unsafe(unit, core_id, (sig_info.prm_blk), &prm_fld, val));

        num_of_bytes = sig_info.signal_length_in_bits/32 + 1;

        if(sig_info.signal_offset > 0) {
            offset_in_bytes = (sig_info.signal_offset + 31)/32;
            for(ind = 0; ind < num_of_bytes - offset_in_bytes; ++ind) {
                val_aligned[ind] = val[ind + offset_in_bytes];
			}

            sal_memcpy(val, val_aligned, sizeof(uint32) * ARAD_PP_DIAG_DBG_VAL_LEN);
        }
    } else {
        return -1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_signal_mngr_signal_print(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id)
{
    int num_of_int = 0;
    int ind, size_of_signal_in_bits;
    uint32 val[ARAD_PP_DIAG_DBG_VAL_LEN];
    int res = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_signal_mngr_signal_get(unit, core_id, signal_id, val, &size_of_signal_in_bits);

    num_of_int = (size_of_signal_in_bits/32) + 1;

    

    if (res == 0) {
        for(ind = 0; ind < num_of_int; ++ind) {
            LOG_CLI((BSL_META("\t buff[%u]: 0x%08x\n\r"), ind, val[ind]));
        }
    } else {
        LOG_CLI((BSL_META("\t SIGNAL NOT VALID \n\r")));
    }

    SOCDNX_FUNC_RETURN;
}



uint32
    arad_pp_signal_mngr_signal_ids_get(
       int unit,
       SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type,
       int lookup_number,
       SOC_PPC_FP_DATABASE_STAGE  stage,
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE key_signal_id[5],
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* res_signal_id,
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* hit_signal_id,
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* opcode_signal_id,
       int* nof_key_signals)
{

    SOCDNX_INIT_FUNC_DEFS;

    (*nof_key_signals) = 1;

    switch (physical_db_type) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        if(lookup_number == 2) {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM2_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_RES;

        } else {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM1_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_RES;
        }
        break;
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
        if(lookup_number == 2) {
            if(SOC_IS_JERICHO(unit)) {
                (*nof_key_signals) = 2;
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY;
                key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY_2;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_RES;
            } else {
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_RES;
            }

        } else {
            if(SOC_IS_JERICHO(unit)) {
                (*nof_key_signals) = 2;
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY;
                key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY_2;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_RES;
            }
        }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_TCAM_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_RES;
        } else {

            if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) {
                (*nof_key_signals) = 2;
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_LSB;
                key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_MSB;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_RES;
            } else {
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_KEY;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_RES;
            }
        }
            break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
        key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART1;
        key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART2;
        key_signal_id[2] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART3;
        key_signal_id[3] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART4;
        key_signal_id[4] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART5;
        (*opcode_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_OPCODE;
        (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_ERR;
        (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES;

        (*nof_key_signals) = 5;
            break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_RES;
        } else { 
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_RES;
        }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_RES;

        } else { 
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_RES;
        }
            break;

    default:
        goto exit;
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#undef SIGNAL_MNGR_FUNCTIONS

