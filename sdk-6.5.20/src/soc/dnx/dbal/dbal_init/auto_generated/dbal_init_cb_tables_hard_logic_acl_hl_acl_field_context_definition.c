
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
field_context_hash_configuration_compression_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_pmf_ctx_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_PMF_CTX_ID, key_value__field_pmf_ctx_id);
    *offset = key_value__field_pmf_ctx_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_ipmf1_context_general_first_context_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_pmf_ctx_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_PMF_CTX_ID, key_value__field_pmf_ctx_id);
    *offset = 5*key_value__field_pmf_ctx_id;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_context_general_acl_context_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_pmf_ctx_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_PMF_CTX_ID, key_value__field_pmf_ctx_id);
    *offset = key_value__field_pmf_ctx_id*6;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
field_pmf_context_general_fwd_context_profile_dataoffset_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    dbal_entry_handle_t *  eh = (dbal_entry_handle_t *) entry_handle;
    uint32 key_value__field_pmf_ctx_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_KEY_VALUE(unit, eh, DBAL_FIELD_FIELD_PMF_CTX_ID, key_value__field_pmf_ctx_id);
    *offset = key_value__field_pmf_ctx_id*5;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
