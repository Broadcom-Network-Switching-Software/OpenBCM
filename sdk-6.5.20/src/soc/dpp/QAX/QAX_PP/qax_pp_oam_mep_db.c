/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_pp_oam_mep_db.c
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM
#include <shared/bsl.h>



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/mbcm_pp.h>


#include <soc/dpp/QAX/QAX_PP/qax_pp_oam.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam_mep_db.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>

#include <bcm_int/common/debug.h>












#define SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, blk, index, data) \
            soc_mem_array_read(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index),\
                               blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data)

#define SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB(unit, blk, index, data) \
            soc_mem_array_write(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index),\
                               blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data)

#define SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_ONE_WAYm(unit, blk, index, data) \
            SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB(unit, blk, index, data)
#define SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_DM_STAT_ONE_WAYm(unit, blk, index, data) \
            SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, blk, index, data)
#define SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit, blk, index, data) \
            SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB(unit, blk, index, data)
#define SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit, blk, index, data) \
            SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, blk, index, data)



#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_LSB        (0) 
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_SIZE       (16)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_MASK       (SOC_SAND_BITS_MASK(QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_SIZE - 1, 0))
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_GET(key)   \
            (((key) >> QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_LSB) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_MASK)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_SET(key, opcode) \
    (key) |= (((opcode) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_MASK) << QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_LSB) 

#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_LSB       QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_SIZE
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_SIZE      (1)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_MASK      (SOC_SAND_BITS_MASK(QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_SIZE - 1, 0))
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_GET(key)   \
            (((key) >> QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_LSB) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_MASK)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_SET(key, oam_bfd) \
    (key) |= (((oam_bfd) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_MASK) << QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_LSB) 

#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_LSB   (QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_LSB + QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_SIZE)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_SIZE  (6)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_MASK  (SOC_SAND_BITS_MASK(QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_SIZE - 1, 0))
    #define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_GET(key)   \
            (((key) >> QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_LSB) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_MASK)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_SET(key, pe_profile) \
    (key) |= (((pe_profile) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_MASK) << QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_LSB) 



#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_LSB      (0) 
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_SIZE     (3)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_MASK     (SOC_SAND_BITS_MASK(0, QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_SIZE - 1))
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_GET(data)   \
            (((data) >> QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_LSB) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_MASK)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_SET(data, mask_sel) \
    (data) |= (((mask_sel) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_MASK) << QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_LSB) 

#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_LSB      (QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_SIZE) 
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_SIZE     (1)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_MASK     (SOC_SAND_BITS_MASK(0, QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_SIZE - 1))
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_GET(data)   \
            (((data) >> QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_LSB) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_MASK)
#define QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_SET(data, crc_sel) \
    (data) |= (((crc_sel) & QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_MASK) << QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_LSB) 


















STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__add_update(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__remove(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_mep_scan(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


STATIC
soc_error_t soc_qax_pp_oam_mep_db_ptr_set(
   int unit,
   uint32 mep_idx,
   soc_field_t field,
   uint32 first_entry
   );


STATIC
soc_error_t soc_qax_pp_oam_mep_db_ptr_get(
   int unit,
   uint32 mep_idx,
   soc_field_t field,
   int *first_entry
   );


STATIC
soc_error_t soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info,
   uint32 entry,
   uint8 val
   );


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_scan(
   int unit,
   int endpoint_id,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info,
   uint32 entry
   );

    
STATIC
int _arad_pp_oam_internal_no_cache_read_oamp_mep_DBm(
   int unit,
   uint32 flags,
   int blk,
   uint32 index,
   void *data
   );


STATIC
void _soc_qax_pp_oam_dm_two_way_entry_read(
   int unit,
   SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info,
   soc_reg_above_64_val_t reg_data
   );


STATIC
void _soc_qax_pp_oam_dm_one_way_entry_read(
   int unit,
   SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info,
   soc_reg_above_64_val_t reg_data
   );


STATIC
void _soc_qax_pp_oam_lm_entry_read(
   int unit,
   SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info,
   soc_reg_above_64_val_t reg_data
   );


STATIC
void _soc_qax_pp_oam_lm_stat_entry_read(
   int unit,
   SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info,
   soc_reg_above_64_val_t reg_data
   );

int get_ccm_entry(int unit, uint32 entry_index, soc_reg_above_64_val_t *reg_data, soc_mem_t *entry_type);








soc_error_t
soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find(int unit,
                                              ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(
       soc_qax_pp_oam_oamp_lm_dm_mep_scan(unit, lm_dm_info));

    
    if (lm_dm_info->action_type == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_ADD_UPDATE) {
        SOCDNX_IF_ERR_EXIT(
           soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__add_update(unit, lm_dm_info));
    } else {
        SOCDNX_IF_ERR_EXIT(
           soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__remove(unit, lm_dm_info));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_qax_pp_oam_oamp_lm_dm_set(int unit,
                                          ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {
    soc_error_t res;
    soc_reg_above_64_val_t reg_data, min_delay_field, entry_above_64_get;
    uint32 flex_lm_dm_ptr;
    soc_mem_t  mem = OAMP_MEP_DBm ;
    uint32     reg32_field, reg32_field_get;
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *lm_dm_mep_db_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    res = arad_pmf_db_fes_user_header_sizes_get(unit,
                                               &user_header_0_size,
                                               &user_header_1_size,
                                               &user_header_egress_pmf_offset_0_dummy,
                                               &user_header_egress_pmf_offset_1_dummy);


    user_header_size = (user_header_0_size + user_header_1_size) / 8;

    switch (lm_dm_info->action) {

    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_NONE:
        break;  


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_1WAY:
        
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field32_set(unit, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_DM_ONE_WAY);
        SOC_REG_ABOVE_64_CLEAR(min_delay_field);
        SHR_BITSET_RANGE(min_delay_field, 0, soc_mem_field_length(unit, OAMP_MEP_DB_DM_STAT_ONE_WAYm, MIN_DELAY_DMRf));
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_set(unit, reg_data, MIN_DELAY_DMRf, min_delay_field);
        
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field32_set(unit, reg_data, LAST_ENTRYf, 1);
        res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_ONE_WAYm(unit, MEM_BLOCK_ALL,
                                                                         lm_dm_info->entries_to_add[0], reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_2WAY:
        
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field32_set(unit, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_DM);
        SOC_REG_ABOVE_64_CLEAR(min_delay_field);
        SHR_BITSET_RANGE(min_delay_field, 0, soc_mem_field_length(unit, OAMP_MEP_DB_DM_STAT_TWO_WAYm, MIN_DELAYf));
        soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_set(unit, reg_data, MIN_DELAYf, min_delay_field);
        soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field32_set(unit, reg_data, LAST_ENTRYf, 1);
        res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit, MEM_BLOCK_ALL,
                                                                         lm_dm_info->entries_to_add[0], reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM:
    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_WITH_STAT:
        
        soc_OAMP_MEP_DB_LM_DBm_field32_set(unit, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_LM);
        soc_OAMP_MEP_DB_LM_DBm_field32_set(unit, reg_data, LAST_ENTRYf,
                                           (lm_dm_info->action == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM));
        soc_OAMP_MEP_DB_LM_DBm_field32_set(unit, reg_data, LM_CNTRS_VALIDf, 0);
        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_DBm(unit, MEM_BLOCK_ALL,
                                                             lm_dm_info->entries_to_add[0], reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (lm_dm_info->action == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM) {
            
            break;
        }
        
    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_STAT:
        
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_LM_STAT);
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, LAST_ENTRYf, 1);
        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL,
                                                               lm_dm_info->entries_to_add[lm_dm_info->num_entries_to_add - 1], reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT:
        
        res = soc_qax_pp_oam_oamp_lm_dm_delete(unit, lm_dm_info);
        SOCDNX_IF_ERR_EXIT(res);
        break;


    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Unsupported MEP-DB add/update action.")));
    }

    if (lm_dm_info->action != ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_NONE) {
        
        if (lm_dm_info->mep_entry.lm_dm_ptr == 0) {
            flex_lm_dm_ptr = _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(lm_dm_info->entries_to_add[0]);
            res = soc_qax_pp_oam_mep_db_ptr_set(unit, lm_dm_info->endpoint_id, FLEX_LM_DM_PTRf, flex_lm_dm_ptr);
            SOCDNX_IF_ERR_EXIT(res);

        }
        
        else if (lm_dm_info->action != ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT) {
            res = soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(unit, lm_dm_info, lm_dm_info->last_entry, 0);
            SOCDNX_IF_ERR_EXIT(res);
        }
    }

    lm_dm_mep_db_entry = lm_dm_info->lm_dm_entry;

    

    if((lm_dm_info->out_lif != 0) && (lm_dm_info->mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) && (lm_dm_mep_db_entry->is_piggyback_down)) {

        

        SOC_REG_ABOVE_64_CLEAR(reg_data);

        res = get_ccm_entry(unit, lm_dm_mep_db_entry->mep_id, &reg_data, &mem);
        SOC_SAND_IF_ERR_EXIT(res);

        
        if (lm_dm_info->is_maid_48){
            if(user_header_size) {
                arad_pp_oamp_pe_program_profile_get(unit,  ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH, &reg32_field);
            }else{
                arad_pp_oamp_pe_program_profile_get(unit,  ARAD_PP_OAMP_PE_PROGS_MAID_48, &reg32_field);
            }
        } else{
            if (SOC_DPP_CONFIG(unit)->pp.oam_statistics) {
              arad_pp_oamp_pe_program_profile_get(unit,  ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM, &reg32_field);
            } else{
              arad_pp_oamp_pe_program_profile_get(unit,  ARAD_PP_OAMP_PE_PROGS_DEFAULT, &reg32_field);
            }
        }

        soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg32_field);

        do
        {
            res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, lm_dm_mep_db_entry->mep_id, &reg_data);
            SOC_SAND_IF_ERR_EXIT(res);

            
            SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
            res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, lm_dm_mep_db_entry->mep_id, &entry_above_64_get);
            SOC_SAND_IF_ERR_EXIT(res);
            soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &reg32_field_get);

        } while (reg32_field != reg32_field_get);

   }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_qax_pp_oam_oamp_lm_dm_delete(int unit,
                                             ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {

    soc_error_t res;
    uint32 entry;
    uint32 new_last_entry;
    soc_mem_t  mem = OAMP_MEP_DBm ;
    uint32  reg_val;
    uint32 mep_pe_profile = 0;
    uint32 is_slm = 0;
    int entries_to_remove = 1;
    soc_reg_above_64_val_t reg_data, entry_above_64_get;
    uint32 mep_pe_profile_slm = 0;
    uint32 mep_pe_profile_slm_48 = 0;
    uint32 mep_pe_profile_slm_pointer = 0;
    SOC_PPC_OAM_MEP_TYPE entry_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    switch (lm_dm_info->action) {


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM:
        
        entries_to_remove += (lm_dm_info->lm_stat_entry > 0); 

        
        if (lm_dm_info->dm_entry) {
            if (lm_dm_info->dm_entry != lm_dm_info->last_entry) {
                
                res = soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(unit, lm_dm_info, lm_dm_info->dm_entry, 1);
                SOCDNX_IF_ERR_EXIT(res);
            }
            if (lm_dm_info->dm_entry != lm_dm_info->mep_entry.lm_dm_ptr) {
                
                res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                                                              lm_dm_info->dm_entry, reg_data);
                SOCDNX_IF_ERR_EXIT(res);
                do
                {
                    uint32 reg_val=0;
                    res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                            lm_dm_info->mep_entry.lm_dm_ptr, reg_data);
                    SOCDNX_IF_ERR_EXIT(res);
                    
                    SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                            lm_dm_info->mep_entry.lm_dm_ptr, entry_above_64_get);
                    SOCDNX_IF_ERR_EXIT(res);

                    soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
                    soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

                } while (sal_memcmp(reg_data, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));

            }
        } else {
            
            res = soc_qax_pp_oam_mep_db_ptr_set(unit, lm_dm_info->endpoint_id, FLEX_LM_DM_PTRf, 0);
            SOCDNX_IF_ERR_EXIT(res);
            SOC_REG_ABOVE_64_CLEAR(reg_data);
           do
            {
                uint32 reg_val=0;
                res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                        lm_dm_info->mep_entry.lm_dm_ptr, reg_data);
                SOCDNX_IF_ERR_EXIT(res);
                
                SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                        lm_dm_info->mep_entry.lm_dm_ptr, entry_above_64_get);
                SOCDNX_IF_ERR_EXIT(res);

                soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
                soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

            } while (sal_memcmp(reg_data, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));
        }
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_DM:
    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT:
        entry = (lm_dm_info->action == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_DM) ?
           lm_dm_info->dm_entry :
           lm_dm_info->lm_stat_entry;
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        
        res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        
        entry_type = soc_OAMP_MEP_DBm_field32_get(unit, reg_data, MEP_TYPEf);
        if (entry_type == SOC_PPC_OAM_MEP_TYPE_DM_ONE_WAY) {
            uint32 mep_pe_prof=0;
            SOC_REG_ABOVE_64_CLEAR(reg_data);

            res = get_ccm_entry(unit, lm_dm_info->endpoint_id, &reg_data, &mem);
            SOC_SAND_IF_ERR_EXIT(res);

            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &mep_pe_prof);
            res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit,mem,MEM_BLOCK_ALL,lm_dm_info->endpoint_id,reg_data);
            SOC_SAND_IF_ERR_EXIT(res);
        }
        
        if (entry == lm_dm_info->last_entry) { 
            if (lm_dm_info->mep_entry.lm_dm_ptr == entry) { 
                
                res = soc_qax_pp_oam_mep_db_ptr_set(unit, lm_dm_info->endpoint_id, FLEX_LM_DM_PTRf, 0);
                SOCDNX_IF_ERR_EXIT(res);
                SOC_REG_ABOVE_64_CLEAR(reg_data);
                do
                {
                    uint32 reg_val=0;
                    res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                            lm_dm_info->mep_entry.lm_dm_ptr, reg_data);
                    SOCDNX_IF_ERR_EXIT(res);
                    
                    SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                            lm_dm_info->mep_entry.lm_dm_ptr, entry_above_64_get);
                    SOCDNX_IF_ERR_EXIT(res);

                    soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
                    soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

                } while (sal_memcmp(reg_data, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));
            } else { 
                
                new_last_entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_PREV(unit, entry);
                res = soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(unit, lm_dm_info, new_last_entry, 1);
                SOCDNX_IF_ERR_EXIT(res);
            }
        } else { 
            
            while (entry < lm_dm_info->last_entry) {
                uint32 next_entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit, entry);
                SOC_REG_ABOVE_64_CLEAR(reg_data);
                res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, next_entry, reg_data);
                SOCDNX_IF_ERR_EXIT(res);

                do
                {
                    res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_data);
                    SOCDNX_IF_ERR_EXIT(res);
                    
                    SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, entry_above_64_get);
                    SOCDNX_IF_ERR_EXIT(res);

                } while (sal_memcmp(reg_data, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));
                entry = next_entry;
            }
        }
        break;


    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Unsupported MEP-DB remove action.")));
    }
    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = get_ccm_entry(unit, lm_dm_info->endpoint_id, &reg_data ,&mem);
    SOC_SAND_IF_ERR_EXIT(res);

    
    soc_mem_field_get(unit, mem, reg_data, MEP_PE_PROFILEf, &mep_pe_profile);

    if(SOC_IS_QAX(unit)){
        
        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_SLM_DOWN, &mep_pe_profile_slm);

         
        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_SLM_POINTER, &mep_pe_profile_slm_pointer);

        
        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_SLM_MAID_48, &mep_pe_profile_slm_48);

        
        if(mep_pe_profile == mep_pe_profile_slm || mep_pe_profile == mep_pe_profile_slm_48 || mep_pe_profile == mep_pe_profile_slm_pointer){
            
            reg_val = mep_pe_profile & (~(1 << ARAD_PP_OAMP_MEP_PE_PROFILE_SLM_OFFSET));
            
            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg_val);
        }
    }
     
    if (SOC_IS_QAX(unit) && lm_dm_info->out_lif != 0) {

        
        is_slm = mep_pe_profile & (1 << ARAD_PP_OAMP_MEP_PE_PROFILE_COUNT_SLM_OFFSET);

        
        if(is_slm) {

            
            reg_val = mep_pe_profile & (~(1 << ARAD_PP_OAMP_MEP_PE_PROFILE_COUNT_SLM_OFFSET)); 

            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg_val);
        }
    }
    
    if( lm_dm_info->out_lif != 0 && lm_dm_info->mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE && (!is_slm)){

        
        if(lm_dm_info->is_maid_48) {
            
            arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_48_MAID_QAX, &reg_val);
            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg_val);
        }else{
            arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_QAX, &reg_val);
            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg_val);
        }
    }
    
    do
    {
        uint32 reg_val=0;
        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, lm_dm_info->endpoint_id, &reg_data);
        SOC_SAND_IF_ERR_EXIT(res);

        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, lm_dm_info->endpoint_id, &entry_above_64_get);
        SOC_SAND_IF_ERR_EXIT(res);

        soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
        soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

    } while (sal_memcmp(reg_data, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));

    
    res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(unit, lm_dm_info, lm_dm_info->last_entry);
    SOCDNX_IF_ERR_EXIT(res);
    entry = lm_dm_info->last_entry;
    while (--entries_to_remove) {
        entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_PREV(unit, entry);
        res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(unit, lm_dm_info, entry);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_qax_pp_oam_oamp_lm_dm_search(int unit, uint32 endpoint_id, uint32 *found_bmp) {

    uint32 found_bitmap_lcl[1];

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO lm_dm_info = { 0 };

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_qax_pp_oam_oamp_lm_dm_shared_scan(unit,
                                                             endpoint_id,
                                                             &lm_dm_info));

    
    *found_bitmap_lcl = 0;
    if (lm_dm_info.lm_entry > 0) {
        SHR_BITSET(found_bitmap_lcl, SOC_PPC_OAM_MEP_TYPE_LM);
    }
    if (lm_dm_info.lm_stat_entry > 0) {
        SHR_BITSET(found_bitmap_lcl, SOC_PPC_OAM_MEP_TYPE_LM_STAT);
    }
    if (lm_dm_info.dm_entry > 0) {
        SHR_BITSET(found_bitmap_lcl, SOC_PPC_OAM_MEP_TYPE_DM);
    }

    *found_bmp = *found_bitmap_lcl;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_qax_pp_oam_oamp_dm_get(int unit, SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info, uint8 *is_1DM) {
    soc_error_t res;
    uint32 flags = SOC_MEM_NO_FLAGS;

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO lm_dm_info = { 0 };

    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_qax_pp_oam_oamp_lm_dm_shared_scan(unit,
                                                             dm_info->entry_id,
                                                             &lm_dm_info));

    
    if (lm_dm_info.dm_entry == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("No DM entry for endpoint %d "),dm_info->entry_id));
    }

    
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    res = _arad_pp_oam_internal_no_cache_read_oamp_mep_DBm(unit, flags, MEM_BLOCK_ANY, lm_dm_info.dm_entry, &reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    switch (soc_OAMP_MEP_DBm_field32_get(unit, reg_data, MEP_TYPEf)) {
    case SOC_PPC_OAM_MEP_TYPE_DM:
        *is_1DM = FALSE;
        _soc_qax_pp_oam_dm_two_way_entry_read(unit, dm_info, reg_data);
        res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit, MEM_BLOCK_ALL, lm_dm_info.dm_entry, reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        break;
    case SOC_PPC_OAM_MEP_TYPE_DM_ONE_WAY:
        *is_1DM = TRUE;
        _soc_qax_pp_oam_dm_one_way_entry_read(unit, dm_info, reg_data);
        res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_ONE_WAYm(unit, MEM_BLOCK_ALL, lm_dm_info.dm_entry, reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Something went wrong")));
    }

    
    if (lm_dm_info.mep_entry.lm_dm_ptr == lm_dm_info.dm_entry) {
        
        dm_info->entry_id = lm_dm_info.dm_entry;
    } else {
        
        dm_info->entry_id = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;

}


soc_error_t
soc_qax_pp_oam_oamp_lm_get(int unit, SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info) {
    soc_error_t res;
    uint32 flags = SOC_MEM_NO_FLAGS;

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO lm_dm_info = { 0 };

    soc_reg_above_64_val_t reg_data, entry_above_64_get;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_qax_pp_oam_oamp_lm_dm_shared_scan(unit,
                                                             lm_info->entry_id,
                                                             &lm_dm_info));

    
    if (lm_dm_info.lm_entry == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("No LM entry for endpoint %d "),lm_info->entry_id));
    }

    
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    res = _arad_pp_oam_internal_no_cache_read_oamp_mep_DBm(unit, flags, MEM_BLOCK_ANY, lm_dm_info.lm_entry, &reg_data);
    SOCDNX_IF_ERR_EXIT(res);
    _soc_qax_pp_oam_lm_entry_read(unit, lm_info, reg_data);

    
    lm_info->is_extended = (lm_dm_info.lm_stat_entry > 0);
    if (lm_info->is_extended) {
        
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        res = _arad_pp_oam_internal_no_cache_read_oamp_mep_DBm(unit, flags, MEM_BLOCK_ANY, lm_dm_info.lm_stat_entry, &reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        _soc_qax_pp_oam_lm_stat_entry_read(unit, lm_info, reg_data);

        do
        {
            uint32 reg_val=0;
            res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info.lm_stat_entry, reg_data);
            SOCDNX_IF_ERR_EXIT(res);
            
            SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
            res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info.lm_stat_entry, entry_above_64_get);
            SOCDNX_IF_ERR_EXIT(res);
            soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
            soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

        } while (sal_memcmp(reg_data, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));


    }

    
    if (lm_dm_info.mep_entry.lm_dm_ptr == lm_dm_info.lm_entry) {
        
        lm_info->entry_id = lm_dm_info.lm_entry;
    } else {
        
        lm_info->entry_id = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;

}







STATIC
int _arad_pp_oam_internal_no_cache_read_oamp_mep_DBm(int unit, uint32 flags, int blk, uint32 index, void *data) {
    int rv;

    flags |= SOC_MEM_DONT_USE_CACHE;

    if (SOC_IS_QAX(unit)) {

        MEM_LOCK(unit, OAMP_MEP_DBm);
        rv = soc_mem_array_read_flags(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data, flags);
        MEM_UNLOCK(unit, OAMP_MEP_DBm);

    } else {
        rv = soc_mem_read_no_cache(unit, flags, OAMP_MEP_DBm, 0, blk, index, data);
    }
    return rv;
}


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__add_update(int unit,
                   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {

    soc_error_t res;

    SOCDNX_INIT_FUNC_DEFS;

    if (lm_dm_info->lm_dm_entry->is_update) { 

        
        if (lm_dm_info->lm_dm_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
            if (lm_dm_info->lm_stat_entry == 0) {
                
                lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_STAT;
                res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(unit, lm_dm_info);
                SOCDNX_IF_ERR_EXIT(res);
            }
            
        } else {
            
            if (lm_dm_info->lm_stat_entry > 0) {
                lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT;
            }
            
        }

    } 

    else { 

        int entries_to_allocate = 1;
        lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM;

        switch (lm_dm_info->lm_dm_entry->entry_type) {

        case SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT:
            
            lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_WITH_STAT;
            entries_to_allocate = 2;
            
        case SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM:
            
            if (lm_dm_info->lm_entry) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                     (_BSL_SOCDNX_MSG("Loss measurement was already added to MEP")));
            }
            break;

        case SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM:
            
            if (lm_dm_info->dm_entry) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                     (_BSL_SOCDNX_MSG("Delay measurement was already added to MEP")));
            }
            if (lm_dm_info->lm_dm_entry->is_1DM) {
                lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_1WAY;
            } else {
                lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_2WAY;
            }
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                 (_BSL_SOCDNX_MSG("Undefined action.")));

        }

        
        for (; entries_to_allocate > 0; --entries_to_allocate) {
            res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(unit, lm_dm_info);
            SOCDNX_IF_ERR_EXIT(res);
        }
    } 

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__remove(int unit,
                ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {

    SOCDNX_INIT_FUNC_DEFS;

    switch (lm_dm_info->action_type) {


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_LM:
        
        if (lm_dm_info->lm_entry == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND,
                                 (_BSL_SOCDNX_MSG("Loss entry not found.")));
        }
        lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM;
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_DM:
        
        if (lm_dm_info->dm_entry == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND,
                                 (_BSL_SOCDNX_MSG("Delay entry not found.")));
        }
        lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_DM;
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Undefined action.")));

    }
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_mep_scan(int unit,
                                ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {

    soc_error_t res = SOC_E_NONE;
    uint32 entry;
    uint8 last_bit = 0;
    int chain_count;
    SOC_PPC_OAM_MEP_TYPE entry_type;
    soc_reg_above_64_val_t reg_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (lm_dm_info->mep_entry.lm_dm_ptr) {
        
        entry = lm_dm_info->mep_entry.lm_dm_ptr;
        
        for (chain_count = 0;
             (chain_count < ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_SHARED_MAX_CHAIN_LEN(unit)) &&
             (last_bit == 0) &&
             (entry < ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit));
             ++chain_count) {
            SOC_REG_ABOVE_64_CLEAR(reg_above_64);
            
            res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_above_64);
            SOCDNX_IF_ERR_EXIT(res);
            
            entry_type = soc_OAMP_MEP_DBm_field32_get(unit, reg_above_64, MEP_TYPEf);
            
            switch (entry_type) {
            case SOC_PPC_OAM_MEP_TYPE_DM:
                lm_dm_info->dm_entry = entry;
                last_bit = soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            case SOC_PPC_OAM_MEP_TYPE_DM_ONE_WAY:
                lm_dm_info->dm_entry = entry;
                last_bit = soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            case SOC_PPC_OAM_MEP_TYPE_LM:
                lm_dm_info->lm_entry = entry;
                last_bit = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            case SOC_PPC_OAM_MEP_TYPE_LM_STAT:
                lm_dm_info->lm_stat_entry = entry;
                last_bit = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            default:
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                     (_BSL_SOCDNX_MSG("LM/DM chain ended abraptly without LAST_ENTRY bit set.")));
            }
            lm_dm_info->last_entry = entry;
            entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit, entry);
        }
    } else {
        
        lm_dm_info->last_entry = 0;
    }

    
    lm_dm_info->mep_scanned = TRUE;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(int unit,
                      ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {
    uint32 entry;

    SOCDNX_INIT_FUNC_DEFS;

    if (lm_dm_info->num_entries_to_add) {
        
        entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit,
                                                         lm_dm_info->entries_to_add[lm_dm_info->num_entries_to_add - 1]);
    } else if (lm_dm_info->last_entry == 0) {
        
        entry = lm_dm_info->lm_dm_entry->lm_dm_id;
    } else {
        
        entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit, lm_dm_info->last_entry);
    }

    if (entry > ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("No room to add another LM/DM statistics entry.")));
    }
    lm_dm_info->entries_to_add[lm_dm_info->num_entries_to_add++] = entry;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(int unit,
                            ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info, uint32 entry) {
    SOCDNX_INIT_FUNC_DEFS;

    lm_dm_info->entries_to_remove[lm_dm_info->num_entries_to_remove++] = entry;

    SOC_EXIT;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_mep_db_ptr_set(int unit,
                    uint32 mep_idx,
                    soc_field_t field,
                    uint32 first_entry) {

    soc_error_t res;
    soc_reg_above_64_val_t reg_data, entry_above_64_get;
    uint32 mep_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_idx, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    mep_type = soc_mem_field32_get(unit, OAMP_MEP_DBm, reg_data, MEP_TYPEf); 
    
    switch (mep_type) {
    case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
        soc_OAMP_MEP_DBm_field32_set(unit, reg_data, field, first_entry);
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
        soc_OAMP_MEP_DB_Y_1731_ON_MPLSTPm_field32_set(unit, reg_data, field, first_entry);
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
        soc_OAMP_MEP_DB_Y_1731_ON_PWEm_field32_set(unit, reg_data, field, first_entry);
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
        if (field == EXTRA_DATA_PTRf) {
            switch (mep_type) {
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
                soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field32_set(unit, reg_data, field, first_entry);
                break;
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
                soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field32_set(unit, reg_data, field, first_entry);
                break;
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
                soc_OAMP_MEP_DB_BFD_ON_MPLSm_field32_set(unit, reg_data, field, first_entry);
                break;
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
                soc_OAMP_MEP_DB_BFD_ON_PWEm_field32_set(unit, reg_data, field, first_entry);
                break;
            }
            break;
        }
        
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOCDNX_MSG("Error - next entry pointer not supported for the MEP type used.")));
    }

    do
    {
        uint32 reg_val=0;
        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_idx, reg_data);
        SOC_SAND_IF_ERR_EXIT(res);

        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_idx, entry_above_64_get);
        SOC_SAND_IF_ERR_EXIT(res);
        
        soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
        soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

    } while (sal_memcmp(reg_data, entry_above_64_get,(soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_mep_db_ptr_get(int unit,
                    uint32 mep_idx,
                    soc_field_t field,
                    int *first_entry) {

    soc_error_t res;
    soc_reg_above_64_val_t reg_data;
    uint32 mep_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_idx, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    mep_type = soc_mem_field32_get(unit, OAMP_MEP_DBm, reg_data, MEP_TYPEf); 
    
    switch (mep_type) {
    case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
        *first_entry = soc_OAMP_MEP_DBm_field32_get(unit, reg_data, field);
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
        *first_entry = soc_OAMP_MEP_DB_Y_1731_ON_MPLSTPm_field32_get(unit, reg_data, field);
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
        *first_entry = soc_OAMP_MEP_DB_Y_1731_ON_PWEm_field32_get(unit, reg_data, field);
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
        if (field == EXTRA_DATA_PTRf) {
            switch (mep_type) {
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
                *first_entry = soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field32_get(unit, reg_data, field);
                break;
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
                *first_entry = soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field32_get(unit, reg_data, field);
                break;
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
                *first_entry = soc_OAMP_MEP_DB_BFD_ON_MPLSm_field32_get(unit, reg_data, field);
                break;
            case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
                *first_entry = soc_OAMP_MEP_DB_BFD_ON_PWEm_field32_get(unit, reg_data, field);
                break;
            }
            break;
        }
        
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOCDNX_MSG("Error - next entry pointer not supported for the MEP type used.")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(int unit,
                                                       ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info, 
                                                       uint32 entry, uint8 val) {

    soc_error_t res;
    soc_reg_above_64_val_t reg_data,entry_above_64_get;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
    if (lm_dm_info->lm_entry == entry) {
        soc_OAMP_MEP_DB_LM_DBm_field32_set(unit, reg_data, LAST_ENTRYf, val);
    } else if (lm_dm_info->lm_stat_entry == entry) {
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, LAST_ENTRYf, val);
    } else if (lm_dm_info->dm_entry == entry) {
        soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field32_set(unit, reg_data, LAST_ENTRYf, val);
    } else {
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Something went wrong.")));

    }
    
    do
    {
        uint32 reg_val=0;
        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, entry_above_64_get);
        SOCDNX_IF_ERR_EXIT(res);
        
        soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
        soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

    } while (sal_memcmp(reg_data, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_scan(int unit, int endpoint_id,
                                                  ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {
    soc_error_t res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    lm_dm_info->endpoint_id = endpoint_id;

    
    res = arad_pp_oam_oamp_mep_db_entry_get_unsafe(unit, endpoint_id, &lm_dm_info->mep_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    res = soc_qax_pp_oam_oamp_lm_dm_mep_scan(unit, lm_dm_info);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC
void _soc_qax_pp_oam_dm_two_way_entry_read(int unit, SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info,
                                           soc_reg_above_64_val_t reg_data) {

    soc_reg_above_64_val_t reg_field;

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_get(unit, reg_data, LAST_DELAYf, reg_field);
    SHR_BITCOPY_RANGE(&(dm_info->last_delay_sub_seconds), 0, reg_field, 0, 30);
    SHR_BITCOPY_RANGE(&(dm_info->last_delay_second), 0, reg_field, 30, 12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_get(unit, reg_data, MAX_DELAYf, reg_field);
    SHR_BITCOPY_RANGE(&(dm_info->max_delay_sub_seconds), 0, reg_field, 0, 30);
    SHR_BITCOPY_RANGE(&(dm_info->max_delay_second), 0, reg_field, 30, 12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_get(unit, reg_data, MIN_DELAYf, reg_field);
    SHR_BITCOPY_RANGE(&(dm_info->min_delay_sub_seconds), 0, reg_field, 0, 30);
    SHR_BITCOPY_RANGE(&(dm_info->min_delay_second), 0, reg_field, 30, 12);

    
    dm_info->last_delay_sub_seconds <<=2;
    dm_info->max_delay_sub_seconds <<=2;
    dm_info->min_delay_sub_seconds <<=2;


    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_set(unit, reg_data, MAX_DELAYf, reg_field);
    SHR_BITSET_RANGE(reg_field, 0, soc_mem_field_length(unit, OAMP_MEP_DB_DM_STAT_TWO_WAYm, MIN_DELAYf));
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_set(unit, reg_data, MIN_DELAYf, reg_field);

}


STATIC
void _soc_qax_pp_oam_dm_one_way_entry_read(int unit, SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info,
                                           soc_reg_above_64_val_t reg_data) {

    soc_reg_above_64_val_t reg_field;
    soc_reg_above_64_val_t profile_entry;
    uint32 dm_type;

    SOC_REG_ABOVE_64_CLEAR(reg_field);

    READ_OAMP_MEP_PROFILEm(unit, MEM_BLOCK_ANY,dm_info->Y1731_profile_index, profile_entry);

    dm_type  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, DMM_MEASURE_ONE_WAYf);

    if (dm_type == 1) {
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, LAST_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_sub_seconds), 0, reg_field, 0, 20);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_second), 0, reg_field, 20, 9);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MAX_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_sub_seconds), 0, reg_field, 0, 20);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_second), 0, reg_field, 20, 9);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MIN_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_sub_seconds), 0, reg_field, 0, 20);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_second), 0, reg_field, 20, 9);
    }
    if (dm_type == 2) {
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, LAST_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_sub_seconds), 0, reg_field, 0, 22);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_second), 0, reg_field, 22, 7);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MAX_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_sub_seconds), 0, reg_field, 0, 22);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_second), 0, reg_field, 22, 7);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MIN_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_sub_seconds), 0, reg_field, 0, 22);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_second), 0, reg_field, 22, 7);
    }
    if (dm_type == 3) {
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, LAST_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_sub_seconds), 0, reg_field, 0, 24);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_second), 0, reg_field, 24, 5);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MAX_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_sub_seconds), 0, reg_field, 0, 24);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_second), 0, reg_field, 24, 5);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MIN_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_sub_seconds), 0, reg_field, 0, 24);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_second), 0, reg_field, 24, 5);
    }
    if (dm_type == 4) {
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, LAST_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_sub_seconds), 0, reg_field, 0, 26);
        SHR_BITCOPY_RANGE(&(dm_info->last_delay_second), 0, reg_field, 26, 3);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MAX_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_sub_seconds), 0, reg_field, 0, 26);
        SHR_BITCOPY_RANGE(&(dm_info->max_delay_second), 0, reg_field, 26, 3);

        SOC_REG_ABOVE_64_CLEAR(reg_field);
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field_get(unit, reg_data, MIN_DELAY_DMRf, reg_field);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_sub_seconds), 0, reg_field, 0, 26);
        SHR_BITCOPY_RANGE(&(dm_info->min_delay_second), 0, reg_field, 26, 3);
    }

}


STATIC
void _soc_qax_pp_oam_lm_entry_read(int unit, SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info,
                                   soc_reg_above_64_val_t reg_data) {
    lm_info->my_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data, MY_TXf);
    lm_info->my_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data, MY_RXf);
    lm_info->peer_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data, PEER_TXf);
    lm_info->peer_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data, PEER_RXf);
}


STATIC
void _soc_qax_pp_oam_lm_stat_entry_read(int unit, SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info,
                                        soc_reg_above_64_val_t reg_data) {
    lm_info->last_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, LAST_LM_NEARf);
    lm_info->last_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, LAST_LM_FARf);
    lm_info->acc_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, ACC_LM_NEARf);
    lm_info->acc_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, ACC_LM_FARf);
    lm_info->max_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, MAX_LM_NEARf);
    lm_info->max_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, MAX_LM_FARf);

    
    soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, MAX_LM_NEARf, 0);
    soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, MAX_LM_FARf, 0);
    soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, ACC_LM_NEARf, 0);
    soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, ACC_LM_FARf, 0);
}


STATIC
void _reverse_buffer_copy(SHR_BITDCL *dst_ptr,
                          CONST int dst_first,
                          CONST SHR_BITDCL *src_ptr,
                          int src_first,
                          int range) {
    int i, bits_from_left, bits_from_right, bits_to_copy, src_offset, buf_elem_size, initial_offset, first_copy;
    buf_elem_size = SHR_BITWID;
    initial_offset = src_first % buf_elem_size;

    
    if (initial_offset != 0) {
        first_copy = buf_elem_size - initial_offset;
        SHR_BITCOPY_RANGE(dst_ptr, dst_first + range - first_copy, src_ptr, src_first - initial_offset, first_copy);
        src_first += first_copy;
        range -= first_copy;
    }

    for (i = 0; i < ((range + buf_elem_size - 1) / buf_elem_size) ; i++) {
        bits_from_left = (i + 1) * buf_elem_size;
        bits_from_right = range >= bits_from_left ? range - bits_from_left : 0;
        bits_to_copy = range >= bits_from_left ? buf_elem_size : range % buf_elem_size;
        src_offset = range >= bits_from_left ? i * buf_elem_size : buf_elem_size * (i + 1) - bits_to_copy;
        SHR_BITCOPY_RANGE(dst_ptr, dst_first + bits_from_right, src_ptr, src_first + src_offset, bits_to_copy);
    }
}

soc_error_t
qax_pp_oam_bfd_flexible_verification_init(int unit){
    soc_error_t res;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_WARM_BOOT(unit)) {
        res = sw_state_access[unit].dpp.soc.qax.pp.mep_db.flexible_verification_use_indicator.alloc_bitmap(unit, ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit));
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
qax_pp_oam_bfd_flexible_verification_set(int unit, SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info)
{
    soc_error_t res;

    soc_reg_above_64_val_t reg_data, entry_above_64_get;
    uint32 crc_msb_mask[SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE / 4], flex_crc_tcam_key, flex_crc_tcam_data, flex_crc_tcam_mask;
    int byte;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    if (info->mep_idx != -1) {
        res = SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, MEM_BLOCK_ALL, info->mep_idx, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_mem_field32_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR);
        soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_set(unit, reg_data, CHECK_CRC_VALUE_1f, info->crc_info.crc16_val1);
        soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_set(unit, reg_data, CHECK_CRC_VALUE_2f, info->crc_info.crc16_val2);

        do
        {
            uint32 reg_val=0;
            res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB(unit, MEM_BLOCK_ALL, info->mep_idx, reg_data);
            SOC_SAND_IF_ERR_EXIT(res);

            
            SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
            res = SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, MEM_BLOCK_ALL, info->mep_idx, entry_above_64_get);
            SOC_SAND_IF_ERR_EXIT(res);
            
            soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
            soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

        } while (sal_memcmp(reg_data, entry_above_64_get,(soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));

        
        res = sw_state_access[unit].dpp.soc.qax.pp.mep_db.flexible_verification_use_indicator.bit_set(unit, info->mep_idx);
        SOCDNX_IF_ERR_EXIT(res);
    }

    
    if (info->mask_tbl_index != -1) {
        sal_memset(reg_data, 0, sizeof(reg_data));
        sal_memset(crc_msb_mask, 0, sizeof(crc_msb_mask));

        
        for (byte = 0; byte < SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE; byte++) {
            crc_msb_mask[byte / 4] += info->crc_info.mask.msb_mask[byte] << ((byte % 4) * 8);
        }

        
        soc_mem_field_set(unit, OAMP_FLEX_VER_MASK_TEMPm, reg_data, PER_BIT_MASKf, crc_msb_mask);
        soc_mem_field_set(unit, OAMP_FLEX_VER_MASK_TEMPm, reg_data, PER_BYTE_MASKf, info->crc_info.mask.lsbyte_mask);

        if (SOC_IS_QUX(unit)) {
            
            res = soc_mem_write(unit, OAMP_FLEX_VER_MASK_TEMPm, MEM_BLOCK_ALL, info->mask_tbl_index, reg_data);
            SOCDNX_IF_ERR_EXIT(res);
        } else {
            soc_reg_above_64_val_t fld_above64_val, soft_init_reg_val;
            SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit,soft_init_reg_val));
            SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
            SOC_REG_ABOVE_64_CREATE_MASK(fld_above64_val, 1, 0);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_56f, fld_above64_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

            
            res = soc_mem_write(unit, OAMP_FLEX_VER_MASK_TEMPm, MEM_BLOCK_ALL, info->mask_tbl_index, reg_data);
            SOCDNX_IF_ERR_EXIT(res);

            
            SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit,soft_init_reg_val));
            SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_56f, fld_above64_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
        }
    }

    
    if (info->crc_tcam_index != -1) {
        sal_memset(reg_data, 0, sizeof(reg_data));

        
        flex_crc_tcam_key   = 0;

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_SET(flex_crc_tcam_key, info->crc_tcam_info.opcode_bmp);

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_SET(flex_crc_tcam_key, info->crc_tcam_info.oam_bfd);

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_SET(flex_crc_tcam_key, info->crc_tcam_info.mep_pe_profile);

        soc_mem_field32_set(unit, OAMP_CLS_FLEX_CRC_TCAMm, reg_data, KEYf, flex_crc_tcam_key);

        
        flex_crc_tcam_mask   = 0;

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_SET(flex_crc_tcam_mask, info->crc_tcam_info.opcode_bmp_mask);

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_SET(flex_crc_tcam_mask, info->crc_tcam_info.oam_bfd_mask);

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_SET(flex_crc_tcam_mask, info->crc_tcam_info.mep_pe_profile_mask);

        soc_mem_field32_set(unit, OAMP_CLS_FLEX_CRC_TCAMm, reg_data, MASKf, flex_crc_tcam_mask);

        
        flex_crc_tcam_data  = 0;

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_SET(flex_crc_tcam_data, info->crc_tcam_info.mask_tbl_index);

        QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_SET(flex_crc_tcam_data, info->crc_tcam_info.crc_select);

        soc_mem_field32_set(unit, OAMP_CLS_FLEX_CRC_TCAMm, reg_data, DATf, flex_crc_tcam_data);

        soc_mem_field32_set(unit, OAMP_CLS_FLEX_CRC_TCAMm, reg_data, VALIDf, 1);

        
        if (SOC_IS_QUX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1));
        }

        res = soc_mem_write(unit, OAMP_CLS_FLEX_CRC_TCAMm, MEM_BLOCK_ALL, info->crc_tcam_index, reg_data);

        if (SOC_IS_QUX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 0));
        }

        SOCDNX_IF_ERR_EXIT(res);
    }



exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
qax_pp_oam_bfd_flexible_verification_get(int unit, SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info) {
    soc_error_t res;

    soc_reg_above_64_val_t reg_data;
    uint32 crc_msb_mask[SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE / 4], flex_crc_tcam_key, flex_crc_tcam_data;
    int byte;
    uint8 crc_is_in_use;
    uint32 entry_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    

    res = SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, MEM_BLOCK_ALL, info->mep_idx, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    entry_type = soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_get(unit, reg_data, MEP_TYPEf);

    
    res = sw_state_access[unit].dpp.soc.qax.pp.mep_db.flexible_verification_use_indicator.bit_get(unit, info->mep_idx, &crc_is_in_use);
    SOCDNX_IF_ERR_EXIT(res);

    if ((entry_type != SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR) || !crc_is_in_use) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("CRC is not used in this entry.")));
    }

    info->crc_info.crc16_val1 = soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_get(unit, reg_data, CHECK_CRC_VALUE_1f);
    info->crc_info.crc16_val2 = soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_get(unit, reg_data, CHECK_CRC_VALUE_2f);
    
    

    res = soc_mem_read(unit, OAMP_CLS_FLEX_CRC_TCAMm, MEM_BLOCK_ALL, info->crc_tcam_index, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    if (soc_mem_field32_get(unit, OAMP_CLS_FLEX_CRC_TCAMm, reg_data, VALIDf)) {

        
        flex_crc_tcam_data = soc_mem_field32_get(unit, OAMP_CLS_FLEX_CRC_TCAMm, reg_data, DATf);

        info->crc_tcam_info.mask_tbl_index = QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_MASK_SEL_GET(flex_crc_tcam_data);

        info->crc_tcam_info.crc_select = QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_CRC_SEL_GET(flex_crc_tcam_data);

        if (info->mask_tbl_index == -1) {
            
            info->mask_tbl_index = info->crc_tcam_info.mask_tbl_index;
        }

        
        flex_crc_tcam_key = soc_mem_field32_get(unit, OAMP_CLS_FLEX_CRC_TCAMm, reg_data, KEYf);

        info->crc_tcam_info.opcode_bmp = QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OPCODE_GET(flex_crc_tcam_key);

        info->crc_tcam_info.oam_bfd = QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_OAM_BFD_GET(flex_crc_tcam_key);

        info->crc_tcam_info.mep_pe_profile = QAX_PP_OAMP_CLS_FLEX_CRC_TCAM_PE_PROFILE_GET(flex_crc_tcam_key);
    } else {
        info->crc_tcam_info.mask_tbl_index = -1;

        info->crc_tcam_info.crc_select = -1;

        info->crc_tcam_info.opcode_bmp = -1;

        info->crc_tcam_info.oam_bfd = -1;

        info->crc_tcam_info.mep_pe_profile = -1;
    }


    

    
    res = soc_mem_read(unit, OAMP_FLEX_VER_MASK_TEMPm, MEM_BLOCK_ALL, info->mask_tbl_index, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    
    soc_mem_field_get(unit, OAMP_FLEX_VER_MASK_TEMPm, reg_data, PER_BIT_MASKf, crc_msb_mask);
    soc_mem_field_get(unit, OAMP_FLEX_VER_MASK_TEMPm, reg_data, PER_BYTE_MASKf, info->crc_info.mask.lsbyte_mask);

    
    for (byte = 0; byte < SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE; byte++) {
        info->crc_info.mask.msb_mask[byte] = (crc_msb_mask[byte / 4] >> (byte % 4)) & 0xff;
    }


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
qax_pp_oam_bfd_flexible_verification_delete(int unit, SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info) {
    soc_error_t res;

    soc_reg_above_64_val_t reg_data, entry_above_64_get;
    uint32 entry_type;
    uint8 crc_is_in_use;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    if (info->mep_idx != -1) {
        res = SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, MEM_BLOCK_ALL, info->mep_idx, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        entry_type = soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_get(unit, reg_data, MEP_TYPEf);

        
        res = sw_state_access[unit].dpp.soc.qax.pp.mep_db.flexible_verification_use_indicator.bit_get(unit, info->mep_idx, &crc_is_in_use);
        SOCDNX_IF_ERR_EXIT(res);

        if ((entry_type != SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR) || (!crc_is_in_use)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("CRC is not used in this entry.")));
        }

        if (!soc_mem_field32_get(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, EXT_DATA_LENGTHf)){
            
            sal_memset(reg_data, 0, sizeof(reg_data));
        } else {
            
            soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_set(unit, reg_data, CHECK_CRC_VALUE_1f, 0);
            soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_set(unit, reg_data, CHECK_CRC_VALUE_2f, 0);
        }
        
        do
        {
            uint32 reg_val=0;
            res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB(unit, MEM_BLOCK_ALL, info->mep_idx, reg_data);
            SOC_SAND_IF_ERR_EXIT(res);

            
            SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
            res = SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB(unit, MEM_BLOCK_ALL, info->mep_idx, entry_above_64_get);
            SOC_SAND_IF_ERR_EXIT(res);
            
            soc_mem_field_set(unit, OAMP_MEP_DBm, reg_data, ECCf, &reg_val);
            soc_mem_field_set(unit, OAMP_MEP_DBm, entry_above_64_get, ECCf, &reg_val);

        } while (sal_memcmp(reg_data, entry_above_64_get,(soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));

        
        res = sw_state_access[unit].dpp.soc.qax.pp.mep_db.flexible_verification_use_indicator.bit_clear(unit, info->mep_idx);
        SOCDNX_IF_ERR_EXIT(res);
    }

    sal_memset(reg_data, 0, sizeof(reg_data));

    
    if (info->mask_tbl_index != -1) {
        res = soc_mem_write(unit, OAMP_FLEX_VER_MASK_TEMPm, MEM_BLOCK_ALL, info->mask_tbl_index, reg_data);
        SOCDNX_IF_ERR_EXIT(res);
    }

    
    if (info->crc_tcam_index != -1) {
        res = soc_mem_write(unit, OAMP_CLS_FLEX_CRC_TCAMm, MEM_BLOCK_ALL, info->crc_tcam_index, reg_data);
        SOCDNX_IF_ERR_EXIT(res);
    }



exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
qax_pp_oam_bfd_mep_db_ext_data_set(int unit, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO *info) {
    soc_error_t res;

    soc_reg_above_64_val_t reg_data;
    uint32      extra_data_ptr, buf[SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_SIZE_UINT32];
    int bank, entry_idx, remaining_bits, bits_to_write;
    int extra_bits, current_entry, nof_entries;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    sal_memset(reg_data, 0, sizeof(reg_data));
    sal_memset(buf, 0, sizeof(reg_data));

    
    if (info->data_size_in_bits > SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_SIZE_BITS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Data size is too big.")));
    }
    if ((info->mep_idx != -1) && (info->mep_idx > SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Mep idx is too high.")));
    }
    if (info->extension_idx > ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Extestion idx is too high.")));
    }
    if (info->opcode_bmp & ~SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_LEGAL_OPCODE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Illegal bit set in opcode bitmap.")));
    }

    bank = OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(info->extension_idx);
    entry_idx = OAMP_MEP_DB_ENTRY_ID_TO_INDEX(info->extension_idx);

    if(info->mep_idx != -1) {
        
        extra_data_ptr = _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(info->extension_idx);
        res = soc_qax_pp_oam_mep_db_ptr_set(unit, info->mep_idx, EXTRA_DATA_PTRf, extra_data_ptr);
        SOCDNX_IF_ERR_EXIT(res);
    }

    

    if (info->data_size_in_bits > 0) {
        
        nof_entries = 1; 
        extra_bits = info->data_size_in_bits - SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS;

        if (extra_bits > 0) {
            nof_entries += extra_bits / SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS;
            nof_entries += (extra_bits % SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS) ? 1 : 0; 
        }

        
        if ((bank + nof_entries) > OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Data is too large to fit in legal entries.")));
        }
        
        res = soc_mem_array_read(unit, OAMP_MEP_DB_EXT_DATA_HDRm, bank, MEM_BLOCK_ALL, entry_idx, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_mem_field32_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR);
        soc_mem_field32_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, OPCODES_TO_PREPENDf, info->opcode_bmp);
        soc_mem_field32_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, EXT_DATA_LENGTHf, nof_entries);
        _reverse_buffer_copy(buf, 0, info->data, 0, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS);
        soc_mem_field_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, VALUEf, buf);

        res = soc_mem_array_write(unit, OAMP_MEP_DB_EXT_DATA_HDRm, bank, MEM_BLOCK_ALL, entry_idx, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        
        for (remaining_bits = extra_bits, current_entry = 1 ;
                remaining_bits > 0 ;
                    remaining_bits -= SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS, current_entry++) {
            sal_memset(reg_data, 0, sizeof(reg_data));
            sal_memset(buf, 0, sizeof(reg_data));

            
            bits_to_write = SOC_SAND_MIN(remaining_bits, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS);
            
            _reverse_buffer_copy(buf, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS - bits_to_write, info->data,
                                 SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS + (current_entry - 1) * SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS,
                                 bits_to_write);
            soc_mem_field_set(unit, OAMP_MEP_DB_EXT_DATA_PLDm, reg_data, VALUEf, buf);
            soc_mem_field32_set(unit, OAMP_MEP_DB_EXT_DATA_PLDm, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_EXT_DATA_PLD);

            res = soc_mem_array_write(unit, OAMP_MEP_DB_EXT_DATA_PLDm, bank + current_entry, MEM_BLOCK_ALL, entry_idx, reg_data);
            SOCDNX_IF_ERR_EXIT(res);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
qax_pp_oam_bfd_mep_db_ext_data_get(int unit, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO *info){
    soc_error_t res;

    soc_reg_above_64_val_t reg_data;
    uint32      buf[SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_SIZE_UINT32];
    int bank, entry_idx;
    int current_entry, nof_extra_entries;
    uint32 entry_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    sal_memset(reg_data, 0, sizeof(reg_data));
    sal_memset(buf, 0, sizeof(reg_data));
    sal_memset(info->data, 0, sizeof(info->data));

    
    if (!info->extension_idx) {
         
        if (info->mep_idx > SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Mep idx is too high.")));
        }
        res = soc_qax_pp_oam_mep_db_ptr_get(unit, info->mep_idx, EXTRA_DATA_PTRf, &info->extension_idx);
        SOCDNX_IF_ERR_EXIT(res);

        if (!info->extension_idx) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Mep entry doesn't have an extesion.")));
        }
    } 


    if (info->extension_idx > ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Extension idx is too high.")));
    }

    bank = OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(info->extension_idx);
    entry_idx = OAMP_MEP_DB_ENTRY_ID_TO_INDEX(info->extension_idx);
    
    
    res = soc_mem_array_read(unit, OAMP_MEP_DB_EXT_DATA_HDRm, bank, MEM_BLOCK_ALL, entry_idx, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    entry_type = soc_mem_field32_get(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, MEP_TYPEf);

    if (entry_type != SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Extesion entry is not of extesion type.")));
    }

    info->opcode_bmp = soc_mem_field32_get(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, OPCODES_TO_PREPENDf);
    nof_extra_entries = soc_mem_field32_get(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, EXT_DATA_LENGTHf) - 1 ;

    if (info->data_size_in_bits == 0) {
        
        info->data_size_in_bits = SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS + nof_extra_entries * SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS;
    }

    soc_mem_field_get(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, VALUEf, buf);
    SHR_BITCOPY_RANGE(info->data, 0, buf, 0, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS);

    
    
    for (current_entry = 0 ; current_entry < nof_extra_entries ; current_entry++) {
        sal_memset(reg_data, 0, sizeof(reg_data));
        sal_memset(buf, 0, sizeof(reg_data));

        res = soc_mem_array_read(unit, OAMP_MEP_DB_EXT_DATA_PLDm, bank + current_entry, MEM_BLOCK_ALL, entry_idx, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        entry_type = soc_mem_field32_get(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, MEP_TYPEf);

        if (entry_type != SOC_PPC_OAM_MEP_TYPE_EXT_DATA_PLD) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("Extra extesion entry is not of extesion type.")));
        }

        
        soc_mem_field_get(unit, OAMP_MEP_DB_EXT_DATA_PLDm, reg_data, VALUEf, buf);

        SHR_BITCOPY_RANGE(info->data,
                          SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS + current_entry * SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS,
                          buf,
                          0,
                          SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
qax_pp_oam_bfd_mep_db_ext_data_delete(int unit, int extension_idx){
    soc_error_t res;

    soc_reg_above_64_val_t reg_data, buf;
    int bank, entry_idx;
    int current_entry, nof_extra_entries;
    uint8 crc_in_use;
    uint32 entry_type;
    SOCDNX_INIT_FUNC_DEFS;

    if (extension_idx > ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Extension idx is too high.")));
    }


    
    bank = OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(extension_idx);
    entry_idx = OAMP_MEP_DB_ENTRY_ID_TO_INDEX(extension_idx);

    
    res = soc_mem_array_read(unit, OAMP_MEP_DB_EXT_DATA_HDRm, bank, MEM_BLOCK_ALL, entry_idx, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    entry_type = soc_OAMP_MEP_DB_EXT_DATA_HDRm_field32_get(unit, reg_data, MEP_TYPEf);

    if (entry_type != SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Extesion entry is not of extesion type.")));
    }

    nof_extra_entries   = soc_mem_field32_get(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, EXT_DATA_LENGTHf) - 1 ;
    
    
    res = sw_state_access[unit].dpp.soc.qax.pp.mep_db.flexible_verification_use_indicator.bit_get(unit, extension_idx, &crc_in_use);
    SOCDNX_IF_ERR_EXIT(res);

    
    if (!crc_in_use) {
        
        sal_memset(reg_data, 0, sizeof(reg_data));
    } else {
        
        soc_mem_field32_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, OPCODES_TO_PREPENDf, 0);
        soc_mem_field32_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, EXT_DATA_LENGTHf, 0);
        sal_memset(buf, 0, sizeof(buf));
        soc_mem_field_set(unit, OAMP_MEP_DB_EXT_DATA_HDRm, reg_data, VALUEf, buf);
    }
    res = soc_mem_array_write(unit, OAMP_MEP_DB_EXT_DATA_HDRm, bank, MEM_BLOCK_ALL, entry_idx, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    
    sal_memset(reg_data, 0, sizeof(reg_data));
    for (current_entry = 1 ; current_entry <= nof_extra_entries ; current_entry++) {
        res = soc_mem_array_write(unit, OAMP_MEP_DB_EXT_DATA_PLDm, bank + current_entry, MEM_BLOCK_ALL, entry_idx, reg_data);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>

