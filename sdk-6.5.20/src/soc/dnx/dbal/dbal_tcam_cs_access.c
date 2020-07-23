/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include "dbal_internal.h"
#include "dbal_hl_access.h"
#include <soc/dnx/dbal/dbal.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>




#define TCAM_CS_KEY_SIZE_PREFIX_BITS_HALF 0

#define TCAM_CS_KEY_SIZE_PREFIX_BITS_FULL 1

#define TCAM_CS_KEY_SIZE_PREFIX_BITS_MASK 1

#define CS_TCAM_BANK_TBL_DATA_WIDTH_MAX \
    (BITS2WORDS(DNX_DATA_MAX_INGRESS_CS_PROPERTIES_CS_BUFFER_NOF_BITS_MAX + \
    (2 * DNX_DATA_MAX_INGRESS_CS_PROPERTIES_NOF_BITS_ENTRY_SIZE_PREFIX)))

#define TCAM_CS_VALID_BITS_FULL 0x3

#define TCAM_CS_VALID_BITS_HALF_FIRST 0x1

#define TCAM_CS_VALID_BITS_HALF_SECOND 0x2

#define TCAM_CS_VALID_BITS_NONE 0x0


#define CS_TCAM_BANK_STAGE_ECC_FIX_EN_DISABLE_VALUE (0)


#define DBAL_TCAM_CS_ECC_DISABLE(_in_tcam_cs_unit, _in_tcam_cs_ecc_fix_en_reg) \
{ \
   \
  int _tcam_cs_ecc_value_acquired = FALSE; \
  int _tcam_cs_ecc_disable_needed = FALSE; \
  int _tcam_cs_ecc_reenable_needed = FALSE; \
  uint32 _tcam_cs_ecc_value; \
  int _tcam_cs_unit = _in_tcam_cs_unit; \
  soc_reg_t _tcam_cs_ecc_fix_en_reg = _in_tcam_cs_ecc_fix_en_reg; \
  if (dnx_data_ingress_cs.features.feature_get(_tcam_cs_unit, dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write)) \
  { \
    _tcam_cs_ecc_disable_needed = TRUE; \
     \
    SHR_IF_ERR_EXIT(soc_reg32_get(_tcam_cs_unit, _tcam_cs_ecc_fix_en_reg, REG_PORT_ANY, 0, &_tcam_cs_ecc_value)); \
    _tcam_cs_ecc_value_acquired = TRUE; \
    if (_tcam_cs_ecc_value != CS_TCAM_BANK_STAGE_ECC_FIX_EN_DISABLE_VALUE) \
    { \
        _tcam_cs_ecc_reenable_needed = TRUE; \
        SHR_IF_ERR_EXIT(soc_reg32_set(_tcam_cs_unit, _tcam_cs_ecc_fix_en_reg, REG_PORT_ANY, 0, CS_TCAM_BANK_STAGE_ECC_FIX_EN_DISABLE_VALUE)); \
    } \
  }


#define DBAL_TCAM_CS_ECC_REENABLE \
    if (_tcam_cs_ecc_disable_needed && _tcam_cs_ecc_value_acquired && _tcam_cs_ecc_reenable_needed) \
    { \
        SHR_SET_CURRENT_ERR(soc_reg32_set(_tcam_cs_unit, _tcam_cs_ecc_fix_en_reg, REG_PORT_ANY, 0, _tcam_cs_ecc_value)); \
    } \
}



shr_error_e
dbal_tcam_cs_stage_info_get(
    int unit,
    dbal_stage_e dbal_stage,
    dbal_tcam_cs_stage_info_t * stage_info)
{
    SHR_FUNC_INIT_VARS(unit);

    stage_info->stage = dbal_stage;

    switch (dbal_stage)
    {
        case DBAL_STAGE_PRT:
        {
            stage_info->cmd_memory = IPPE_PRTCAM_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = PRTCAM_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = PRTCAM_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = PRTCAM_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = PRTCAM_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = PRTCAM_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = PRTCAM_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPE_PRTCAM_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = PRTCAM_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = PRTCAM_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPE_PRTCAM_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_LLR:
        {
            stage_info->cmd_memory = IPPE_LLRCS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = LLRCS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = LLRCS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = LLRCS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = LLRCS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = LLRCS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = LLRCS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPE_LLRCS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = LLRCS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = LLRCS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPE_LLRCS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_VT1:
        {
            stage_info->cmd_memory = IPPF_VTACS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = VTACS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = VTACS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = VTACS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = VTACS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = VTACS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = VTACS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPF_VTACS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = VTACS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = VTACS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPF_VTACS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_VT2:
        {
            stage_info->cmd_memory = IPPF_VTBCS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = VTBCS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = VTBCS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = VTBCS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = VTBCS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = VTBCS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = VTBCS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPF_VTBCS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = VTBCS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = VTBCS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPF_VTBCS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_VT3:
        {
            stage_info->cmd_memory = IPPF_VTCCS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = VTCCS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = VTCCS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = VTCCS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = VTCCS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = VTCCS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = VTCCS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPF_VTCCS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = VTCCS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = VTCCS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPF_VTCCS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_VT4:
        {
            stage_info->cmd_memory = IPPA_VTDCS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = VTDCS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = VTDCS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = VTDCS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = VTDCS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = VTDCS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = VTDCS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPA_VTDCS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = VTDCS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = VTDCS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPA_VTDCS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_VT5:
        {
            stage_info->cmd_memory = IPPA_VTECS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = VTECS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = VTECS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = VTECS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = VTECS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = VTECS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = VTECS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPA_VTECS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = VTECS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = VTECS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPA_VTECS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_FWD1:
        {
            stage_info->cmd_memory = IPPB_FLPACS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = FLPACS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = FLPACS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = FLPACS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = FLPACS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = FLPACS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = FLPACS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPB_FLPACS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = FLPACS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = FLPACS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPB_FLPACS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_FWD2:
        {
            stage_info->cmd_memory = IPPB_FLPBCS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = FLPBCS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = FLPBCS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = FLPBCS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = FLPBCS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = FLPBCS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = FLPBCS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPB_FLPBCS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = FLPBCS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = FLPBCS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPB_FLPBCS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_IPMF1:
        {
            stage_info->cmd_memory = IPPC_PMFACSA_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = PMFACSA_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = PMFACSA_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = PMFACSA_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = PMFACSA_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = PMFACSA_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = PMFACSA_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPC_PMFACSA_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = PMFACSA_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = PMFACSA_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPC_PMFACSA_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_IPMF2:
        {
            stage_info->cmd_memory = IPPC_PMFACSB_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = PMFACSB_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = PMFACSB_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = PMFACSB_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = PMFACSB_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = PMFACSB_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = PMFACSB_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPC_PMFACSB_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = PMFACSB_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = PMFACSB_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPC_PMFACSB_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_IPMF3:
        {
            stage_info->cmd_memory = IPPD_PMFBCS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = PMFBCS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = PMFBCS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = PMFBCS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = PMFBCS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = PMFBCS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = PMFBCS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = IPPD_PMFBCS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = PMFBCS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = PMFBCS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = IPPD_PMFBCS_TCAM_ECC_FIX_ENr;
            break;
        }
        case DBAL_STAGE_EPMF:
        {
            stage_info->cmd_memory = ERPP_EPMFCS_TCAM_BANK_COMMANDm;
            stage_info->cmd_write_field = EPMFCS_TCAM_CPU_CMD_WRf;
            stage_info->cmd_read_field = EPMFCS_TCAM_CPU_CMD_RDf;
            stage_info->cmd_compare_field = EPMFCS_TCAM_CPU_CMD_CMPf;
            stage_info->cmd_valid_field = EPMFCS_TCAM_CPU_CMD_VALIDSf;
            stage_info->cmd_key_field = EPMFCS_TCAM_CPU_CMD_KEYf;
            stage_info->cmd_mask_field = EPMFCS_TCAM_CPU_CMD_MASKf;
            stage_info->reply_memory = ERPP_EPMFCS_TCAM_BANK_REPLYm;
            stage_info->reply_valid_field = EPMFCS_TCAM_CPU_REP_VALIDf;
            stage_info->reply_data_out_field = EPMFCS_TCAM_CPU_REP_DOUTf;
            stage_info->ecc_fix_en_reg = ERPP_EPMFCS_TCAM_ECC_FIX_ENr;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection dbal_stage %d is unknown to function.\n", dbal_stage);
            break;
        }
    }

    
    stage_info->line_length_with_key_size_prefix =
        soc_mem_field_length(unit, stage_info->reply_memory, stage_info->reply_data_out_field);

    stage_info->supports_half_entries =
        dnx_data_ingress_cs.properties.per_stage_get(unit, dbal_stage)->supports_half_entries;

    
    if (stage_info->supports_half_entries)
    {
        stage_info->line_length_key_only =
            stage_info->line_length_with_key_size_prefix -
            (2 * dnx_data_ingress_cs.properties.nof_bits_entry_size_prefix_get(unit));
    }
    else
    {
        stage_info->line_length_key_only = stage_info->line_length_with_key_size_prefix;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_tcam_cs_physical_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 entry_num,
    uint8 print_only_key,
    char *action)
{
    int ii, bsl_severity;
    int key_size_in_words = BITS2WORDS(entry_handle->table->key_size);
    int payload_size_in_words = BITS2WORDS(entry_handle->table->max_payload_size);
    int is_tcam = DBAL_TABLE_IS_TCAM(entry_handle->table);
    int logger_action = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            if ((!sal_strcasecmp(action, "get from")) || (!sal_strcasecmp(action, "get next from"))
                || (!sal_strcasecmp(action, "Access ID get")))
            {
                logger_action = 1;
            }
            else if (!sal_strcasecmp(action, "delete from"))
            {
                logger_action = 2;
            }
            else
            {
                logger_action = 0;
            }
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_TCAM_CS, logger_action);

            LOG_CLI((BSL_META("Entry %s ,Stage %s\n"), action,
                     dbal_stage_to_string(unit, entry_handle->table->dbal_stage)));
        
            LOG_CLI((BSL_META("Phy. entry buffer:\n")));
            LOG_CLI((BSL_META("entry number %d\n"), entry_num));

            if (!print_only_key)
            {
            
                if (is_tcam)
                {
                    LOG_CLI((BSL_META("Key & Mask(%d bits)\n\t0x"), entry_handle->table->key_size));
                    for (ii = key_size_in_words - 1; ii >= 0; ii--)
                    {
                        LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.key[ii]));
                    }
                    LOG_CLI((BSL_META("\n\t0x")));
                    for (ii = key_size_in_words - 1; ii >= 0; ii--)
                    {
                        LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.k_mask[ii]));
                    }
                    LOG_CLI((BSL_META("\n")));
                }

            
                LOG_CLI((BSL_META("Payload(%d bits):\n\t0x"), entry_handle->table->max_payload_size));
                for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                {
                    LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.payload[ii]));
                }
                LOG_CLI((BSL_META("\n")));

            
                if (!is_tcam)
                {
                    LOG_CLI((BSL_META("P mask(%d bits):\n\t0x"), entry_handle->phy_entry.payload_size));
                    for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                    {
                        LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.p_mask[ii]));
                    }
                    LOG_CLI((BSL_META("\n")));
                    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_TCAM_CS,
                                                       logger_action);
                }
            }
        }
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tcam_cs_key_read_full_line(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    uint32 line_index,
    uint32 *valid_bits,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 k_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    dbal_entry_handle_t * entry_handle)
{
    soc_reg_above_64_val_t mem_val, data_out;
    uint32 hw_key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    int first_block = MEM_BLOCK_ANY, num_of_blocks;
    dbal_offset_encode_info_t block_index_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    
    SOC_REG_ABOVE_64_CLEAR(mem_val);
    
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_read_field, 0x1);
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_write_field, 0x0);

    if ((entry_handle->table->core_mode != DBAL_CORE_MODE_SBC) && (entry_handle->core_id != DBAL_CORE_ALL))
    {
        
        SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, entry_handle->core_id, stage_info->cmd_memory,
                                               &block_index_info, DBAL_FIELD_EMPTY, &first_block, &num_of_blocks));
    }

    
    SHR_IF_ERR_EXIT(soc_mem_write(unit, stage_info->cmd_memory, first_block, ((line_index * 2) + 1), mem_val));
    SHR_IF_ERR_EXIT(soc_mem_read(unit, stage_info->cmd_memory, first_block, ((line_index * 2) + 1), mem_val));

    
    *valid_bits = soc_mem_field32_get(unit, stage_info->reply_memory, mem_val, stage_info->reply_valid_field);

    
    if (*valid_bits == TCAM_CS_VALID_BITS_NONE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    
    if (stage_info->supports_half_entries == FALSE &&
        *valid_bits != TCAM_CS_VALID_BITS_FULL && *valid_bits != TCAM_CS_VALID_BITS_NONE)
    {
        
#ifdef PLISIM
        if (!SAL_BOOT_PLISIM)   
#endif
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Line data valid is not correct: line=%d valid_fld=%d. Only the half of the entry is valid, "
                         "while the CS TCAM table %s (%d) does not support half entries.\n",
                         line_index, *valid_bits, dbal_stage_to_string(unit, stage_info->stage), stage_info->stage);
        }
    }

    
    SOC_REG_ABOVE_64_CLEAR(data_out);
    soc_mem_field_get(unit, stage_info->reply_memory, mem_val, stage_info->reply_data_out_field, data_out);
    SHR_BITCOPY_RANGE(hw_key_mask, 0, data_out, 0, stage_info->line_length_with_key_size_prefix);

    
    SOC_REG_ABOVE_64_CLEAR(mem_val);
    
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_read_field, 0x1);
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_write_field, 0x0);

    
    SHR_IF_ERR_EXIT(soc_mem_write(unit, stage_info->cmd_memory, first_block, line_index * 2, mem_val));
    SHR_IF_ERR_EXIT(soc_mem_read(unit, stage_info->cmd_memory, first_block, line_index * 2, mem_val));

    
    SOC_REG_ABOVE_64_CLEAR(data_out);
    soc_mem_field_get(unit, stage_info->reply_memory, mem_val, stage_info->reply_data_out_field, data_out);
    SHR_BITCOPY_RANGE(key, 0, data_out, 0, stage_info->line_length_with_key_size_prefix);

    
    SHR_BITOR_RANGE(key, hw_key_mask, 0, stage_info->line_length_with_key_size_prefix, k_mask);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tcam_cs_key_write_full_line(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    uint32 line_index,
    uint32 valid_bits,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 k_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    dbal_entry_handle_t * entry_handle)
{
    soc_reg_above_64_val_t mem_val;
    int block = 0, first_block = MEM_BLOCK_ANY, num_of_blocks;
    dbal_offset_encode_info_t block_index_info = { 0 };
    
    uint32 fld_key[CS_TCAM_BANK_TBL_DATA_WIDTH_MAX] = { 0 };
    uint32 fld_not_key[CS_TCAM_BANK_TBL_DATA_WIDTH_MAX] = { 0 };
    uint32 fld_mask[CS_TCAM_BANK_TBL_DATA_WIDTH_MAX] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_BITAND_RANGE(key, k_mask, 0, stage_info->line_length_with_key_size_prefix, fld_key);
    SHR_BITNEGATE_RANGE(key, 0, stage_info->line_length_with_key_size_prefix, fld_not_key);
    SHR_BITAND_RANGE(fld_not_key, k_mask, 0, stage_info->line_length_with_key_size_prefix, fld_mask);

    SOC_REG_ABOVE_64_CLEAR(mem_val);
    
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_write_field, 0x1);
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_read_field, 0x0);
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_compare_field, 0x0);
    
    soc_mem_field32_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_valid_field, valid_bits);
    
    soc_mem_field_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_key_field, fld_key);
    soc_mem_field_set(unit, stage_info->cmd_memory, mem_val, stage_info->cmd_mask_field, fld_mask);

    
    if ((entry_handle->table->core_mode == DBAL_CORE_MODE_SBC) || (entry_handle->core_id == DBAL_CORE_ALL))
    {
        SHR_IF_ERR_EXIT(soc_mem_write(unit, stage_info->cmd_memory, MEM_BLOCK_ANY, line_index * 2, mem_val));
    }
    
    else
    {
        SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, entry_handle->core_id, stage_info->cmd_memory,
                                               &block_index_info, DBAL_FIELD_EMPTY, &first_block, &num_of_blocks));
        for (block = first_block; block < first_block + num_of_blocks; ++block)
        {
            SHR_IF_ERR_EXIT(soc_mem_write(unit, stage_info->cmd_memory, block, line_index * 2, mem_val));
        }
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tcam_cs_key_read_half_entry_supported(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    uint32 entry_num,
    dbal_entry_handle_t * entry_handle)
{
    uint32 valid_bits;
    int entry_is_valid;
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 k_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    int key_size_prefix_size;
    int offset_first_half_key;
    int offset_second_half_key;
    int half_entry_size;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);

    

    
    rv = dbal_tcam_cs_key_read_full_line(unit, stage_info, (entry_num / 2), &valid_bits, key, k_mask, entry_handle);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND) SHR_IF_ERR_EXIT_NO_MSG(rv);

    
    if (entry_num % 2 == 0)
    {
        entry_is_valid = valid_bits & TCAM_CS_VALID_BITS_HALF_FIRST;
    }
    else
    {
        entry_is_valid = valid_bits & TCAM_CS_VALID_BITS_HALF_SECOND;
    }

    if (entry_is_valid == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    
    key_size_prefix_size = dnx_data_ingress_cs.properties.nof_bits_entry_size_prefix_get(unit);
    half_entry_size = stage_info->line_length_key_only / 2;
    offset_first_half_key = key_size_prefix_size;
    offset_second_half_key = key_size_prefix_size + half_entry_size + key_size_prefix_size;
    
    if (entry_num % 2 == 0)
    {
        
        SHR_BITCOPY_RANGE(entry_handle->phy_entry.key, 0, key, offset_first_half_key, half_entry_size);
        SHR_BITCOPY_RANGE(entry_handle->phy_entry.k_mask, 0, k_mask, offset_first_half_key, half_entry_size);
    }
    else
    {
        
        SHR_BITCOPY_RANGE(entry_handle->phy_entry.key, 0, key, offset_second_half_key, half_entry_size);
        SHR_BITCOPY_RANGE(entry_handle->phy_entry.k_mask, 0, k_mask, offset_second_half_key, half_entry_size);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tcam_cs_key_read(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    uint32 entry_num,
    dbal_entry_handle_t * entry_handle)
{
    uint32 valid_bits;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_TCAM_CS_ECC_DISABLE(unit, stage_info->ecc_fix_en_reg);

    if (stage_info->supports_half_entries)
    {
        rv = dbal_tcam_cs_key_read_half_entry_supported(unit, stage_info, entry_num, entry_handle);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND) SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    else
    {
        rv = dbal_tcam_cs_key_read_full_line(unit, stage_info, entry_num, &valid_bits, entry_handle->phy_entry.key,
                                             entry_handle->phy_entry.k_mask, entry_handle);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND) SHR_IF_ERR_EXIT_NO_MSG(rv);
    }

exit:
    DBAL_TCAM_CS_ECC_REENABLE;
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tcam_cs_key_write_half_entry_supported(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    uint32 entry_num,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e dbal_action)
{
    uint32 valid_bits_read;
    uint32 valid_bits_to_write;
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 k_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 tcam_mode[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 tcam_mode_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    int key_size_prefix_size;
    int offset_first_half_prefix;
    int offset_first_half_key;
    int offset_second_half_prefix;
    int offset_second_half_key;
    int half_entry_size;

    SHR_FUNC_INIT_VARS(unit);

    

    
    if (entry_num % 2 == 0)
    {
        valid_bits_to_write = TCAM_CS_VALID_BITS_HALF_FIRST;
    }
    else
    {
        valid_bits_to_write = TCAM_CS_VALID_BITS_HALF_SECOND;
    }

    
    SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_tcam_cs_key_read_full_line
                              (unit, stage_info, (entry_num / 2), &valid_bits_read, key, k_mask, entry_handle),
                              _SHR_E_NOT_FOUND);

    
    if (dbal_action == DBAL_ACTION_ENTRY_COMMIT)
    {
        valid_bits_to_write = valid_bits_read | valid_bits_to_write;
    }
    else if (dbal_action == DBAL_ACTION_ENTRY_CLEAR)
    {
        valid_bits_to_write = valid_bits_read & (~valid_bits_to_write);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The dbal action %d is not supported for dbal_tcam_cs_key_write. Supported actions: DBAL_ACTION_ENTRY_COMMIT and DBAL_ACTION_ENTRY_CLEAR\n",
                     dbal_action);
    }

    
    key_size_prefix_size = dnx_data_ingress_cs.properties.nof_bits_entry_size_prefix_get(unit);
    half_entry_size = stage_info->line_length_key_only / 2;
    offset_first_half_prefix = 0;
    offset_first_half_key = offset_first_half_prefix + key_size_prefix_size;
    offset_second_half_prefix = offset_first_half_key + half_entry_size;
    offset_second_half_key = offset_second_half_prefix + key_size_prefix_size;
    tcam_mode_mask[0] = TCAM_CS_KEY_SIZE_PREFIX_BITS_MASK;
    tcam_mode[0] = TCAM_CS_KEY_SIZE_PREFIX_BITS_HALF;
    
    if (entry_num % 2 == 0)
    {
        
        SHR_BITCOPY_RANGE(key, offset_first_half_prefix, tcam_mode, 0, key_size_prefix_size);
        SHR_BITCOPY_RANGE(k_mask, offset_first_half_prefix, tcam_mode_mask, 0, key_size_prefix_size);
        SHR_BITCOPY_RANGE(key, offset_first_half_key, entry_handle->phy_entry.key, 0, half_entry_size);
        SHR_BITCOPY_RANGE(k_mask, offset_first_half_key, entry_handle->phy_entry.k_mask, 0, half_entry_size);

    }
    else
    {
        
        SHR_BITCOPY_RANGE(key, offset_second_half_prefix, tcam_mode, 0, key_size_prefix_size);
        SHR_BITCOPY_RANGE(k_mask, offset_second_half_prefix, tcam_mode_mask, 0, key_size_prefix_size);
        SHR_BITCOPY_RANGE(key, offset_second_half_key, entry_handle->phy_entry.key, 0, half_entry_size);
        SHR_BITCOPY_RANGE(k_mask, offset_second_half_key, entry_handle->phy_entry.k_mask, 0, half_entry_size);
    }

    
    SHR_IF_ERR_EXIT(dbal_tcam_cs_key_write_full_line
                    (unit, stage_info, (entry_num / 2), valid_bits_to_write, key, k_mask, entry_handle));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tcam_cs_key_write(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    uint32 entry_num,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e dbal_action)
{
    uint32 valid_bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_TCAM_CS_ECC_DISABLE(unit, stage_info->ecc_fix_en_reg);

    if (stage_info->supports_half_entries)
    {
        SHR_IF_ERR_EXIT(dbal_tcam_cs_key_write_half_entry_supported
                        (unit, stage_info, entry_num, entry_handle, dbal_action));
    }
    else
    {
        
        if (dbal_action == DBAL_ACTION_ENTRY_COMMIT)
        {
            valid_bits = TCAM_CS_VALID_BITS_FULL;
        }
        else if (dbal_action == DBAL_ACTION_ENTRY_CLEAR)
        {
            valid_bits = TCAM_CS_VALID_BITS_NONE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The dbal action %d is not supported for dbal_tcam_cs_key_write. Supported actions: DBAL_ACTION_ENTRY_COMMIT and DBAL_ACTION_ENTRY_CLEAR\n",
                         dbal_action);
        }

        
        SHR_IF_ERR_EXIT(dbal_tcam_cs_key_write_full_line
                        (unit, stage_info, entry_num, valid_bits, entry_handle->phy_entry.key,
                         entry_handle->phy_entry.k_mask, entry_handle));
    }

exit:
    DBAL_TCAM_CS_ECC_REENABLE;
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_result_get(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    dbal_entry_handle_t * entry_handle)
{
    int data_size, block = MEM_BLOCK_ANY, num_of_blocks;
    uint32 array_offset;
    soc_mem_t memory;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    uint32 offset = 0;
    dbal_offset_encode_info_t block_index_info = { 0 };
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    memory = table->tcam_cs_mapping->memory;

    if (memory == INVALIDm)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "memory ilegal for table %s \n", table->table_name);
    }

    array_offset = 0;

    block_index_info.formula = NULL;
    SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, entry_handle->core_id, memory,
                                           &(block_index_info), DBAL_FIELD_EMPTY, &block, &num_of_blocks));

    
    {
        int entry_index = entry_handle->phy_entry.entry_hw_id;
        
        DBAL_MEM_READ_LOG(memory, entry_index, block, array_offset);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, memory, array_offset, block, entry_index, data));
        DBAL_ACTION_PERFORMED_LOG;
        DBAL_DATA_LOG(data, 1);
    }

    data_size = table->max_payload_size;
    offset = 0;

    
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data, offset, data_size, entry_handle->phy_entry.payload));

    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_TCAM_CS, 1);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_result_set(
    int unit,
    dbal_tcam_cs_stage_info_t * stage_info,
    dbal_entry_handle_t * entry_handle)
{
    int iter, block = MEM_BLOCK_ANY, num_of_blocks;
    uint32 array_offset = 0;
    uint32 offset = 0;
    soc_mem_t memory;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_offset_encode_info_t block_index_info = { 0 };
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (table->multi_res_info[entry_handle->cur_res_type].nof_result_fields != entry_handle->nof_result_fields)
    {
        if (!entry_handle->entry_merged)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "partial access not supported for CS table=%s. nof result %d table results %d\n",
                         table->table_name, table->multi_res_info[entry_handle->cur_res_type].nof_result_fields,
                         entry_handle->nof_result_fields);
        }
    }

    memory = table->tcam_cs_mapping->memory;
    array_offset = 0;

    block_index_info.formula = NULL;
    SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, entry_handle->core_id, memory,
                                           &(block_index_info), DBAL_FIELD_EMPTY, &block, &num_of_blocks));
    offset = 0;

    
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (entry_handle->phy_entry.payload, offset, table->max_payload_size, data));

    
    for (iter = 0; iter < num_of_blocks; ++iter)
    {
        int entry_index = entry_handle->phy_entry.entry_hw_id;
        if (stage_info->supports_half_entries
            && entry_handle->phy_entry.key_size > (stage_info->line_length_key_only / 2))
        {
            
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Full entries in a CS TCAM that supports half entries not yet supported. "
                         "entry %d, CS TCAM %s (%d), key size %d.\n",
                         entry_index, dbal_stage_to_string(unit, stage_info->stage), stage_info->stage,
                         entry_handle->phy_entry.key_size);
            
        }
        DBAL_DATA_LOG(data, 1);
        DBAL_MEM_WRITE_LOG(memory, entry_index, block, array_offset);
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, memory, array_offset, block, entry_index, data));
        DBAL_ACTION_PERFORMED_LOG;
        block++;
    }

    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_TCAM_CS, 0);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tcam_cs_entry_add_internal(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e dbal_action)
{
    dbal_tcam_cs_stage_info_t stage_info;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM_BY_ID)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error: The function is UNAVAILABLE for table type %d!\n",
                     entry_handle->table->table_type);

    }

    SHR_IF_ERR_EXIT(dbal_tcam_cs_stage_info_get(unit, entry_handle->table->dbal_stage, &stage_info));

    
    SHR_IF_ERR_EXIT(dbal_tcam_cs_physical_entry_print
                    (unit, entry_handle, entry_handle->phy_entry.entry_hw_id, FALSE, "Commit"));

    SHR_IF_ERR_EXIT(dbal_tcam_cs_key_write
                    (unit, &stage_info, entry_handle->phy_entry.entry_hw_id, entry_handle, dbal_action));

    SHR_IF_ERR_EXIT(dbal_tcam_cs_result_set(unit, &stage_info, entry_handle));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_add_internal(unit, entry_handle, DBAL_ACTION_ENTRY_COMMIT));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_tcam_cs_stage_info_t stage_info;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM_BY_ID)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The function is UNAVAILABLE for table type %d!\n",
                     entry_handle->table->table_type);
    }

    if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "result type not initializad table = %s\n", entry_handle->table->table_name);
    }

    SHR_IF_ERR_EXIT(dbal_tcam_cs_stage_info_get(unit, entry_handle->table->dbal_stage, &stage_info));

    
    entry_handle->phy_entry.payload_size = entry_handle->table->max_payload_size;

    SHR_SET_CURRENT_ERR(dbal_tcam_cs_key_read(unit, &stage_info, entry_handle->phy_entry.entry_hw_id, entry_handle));
    if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
    {
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());
    }

    SHR_IF_ERR_EXIT(dbal_tcam_cs_result_get(unit, &stage_info, entry_handle));

    
    SHR_IF_ERR_EXIT(dbal_tcam_cs_physical_entry_print
                    (unit, entry_handle, entry_handle->phy_entry.entry_hw_id, FALSE, "get from"));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_entry_handle_t *default_handle = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(default_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error: The API is UNAVAILABLE for table type DIRECT!\n");
    }

    SHR_IF_ERR_EXIT(dbal_hl_entry_default_entry_build(unit, entry_handle, default_handle));
    SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_add_internal(unit, default_handle, DBAL_ACTION_ENTRY_CLEAR));

exit:
    SHR_FREE(default_handle);
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int iter;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error: The API is UNAVAILABLE for table type DIRECT!\n");
    }

    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        
        entry_handle->core_id = DBAL_CORE_ALL;
    }

    for (iter = 0; iter < entry_handle->table->max_capacity; iter++)
    {
        entry_handle->phy_entry.entry_hw_id = iter;
        SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_delete(unit, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        entry_handle->core_id = 0;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    uint8 entry_found = FALSE;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    entry_handle->access_id_set = 0;

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT ||
        entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error: The API is UNAVAILABLE for table type %s.\n",
                     dbal_table_type_to_string(unit, entry_handle->table->table_type));
    }

    while ((!entry_found) && (!iterator_info->is_end))
    {
        if (iterator_info->used_first_key)
        {
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
                
                entry_handle->core_id = (entry_handle->core_id + 1) % DBAL_MAX_NUM_OF_CORES;
            }

            
            if ((entry_handle->table->core_mode == DBAL_CORE_MODE_SBC) || (entry_handle->core_id == 0))
            {
                
                ++(entry_handle->phy_entry.entry_hw_id);

                
                if (entry_handle->phy_entry.entry_hw_id == entry_handle->table->max_capacity)
                {
                    break;
                }
            }
        }
        iterator_info->used_first_key = TRUE;
        SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
        if (is_valid_entry == DBAL_KEY_IS_VALID)
        {
            
            sal_memset(entry_handle->phy_entry.k_mask, 0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
            sal_memset(entry_handle->phy_entry.key, 0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
            sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
            rv = dbal_tcam_cs_entry_get(unit, entry_handle);
            if (rv == _SHR_E_NONE)
            {
                entry_found = TRUE;
            }
            else if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "Error from TCAM CS entry get\n");
            }
        }
    }

    if (entry_found == FALSE)
    {
        iterator_info->is_end = TRUE;
    }
    else
    {
        entry_handle->access_id_set = 1;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tcam_cs_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
