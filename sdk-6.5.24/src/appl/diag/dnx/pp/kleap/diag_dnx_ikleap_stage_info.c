/** \file diag_dnx_ikleap_stage_info.c
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGKLEAPDNX

#ifdef BCM_DNX2_SUPPORT
#define INDEX_OF_VTT45 2
#define INDEX_OF_FWD12 3
#endif

/*************
 * INCLUDES  *
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_kleap_stage_info.h>
#include "diag_dnx_ikleap_layers_translation_to_pparse_definition.h"
#include "diag_dnx_ikleap.h"

kleap_layer_to_pparse_t *vt1_header[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt1_qualifier[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt2_header[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt2_qualifier[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt3_header[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt3_qualifier[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt4_header[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt4_qualifier[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt5_header[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *vt5_qualifier[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *fwd1_header[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *fwd1_qualifier[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *fwd2_header[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };
kleap_layer_to_pparse_t *fwd2_qualifier[KLEAP_LAYER_TO_PARSE_MAX_NOF_CTX * KLEAP_NOF_RELATIVE_LAYERS] = { 0 };

kleap_stages_header_qualifiert_t headers_and_qualifiers_per_stage[KLEAP_NOF_STAGES_MAX] = {
    {"VTT1", vt1_header, vt1_qualifier}
    ,
    {"VTT2", vt2_header, vt2_qualifier}
    ,
    {"VTT3", vt3_header, vt3_qualifier}
    ,
    {"VTT4", vt4_header, vt4_qualifier}
    ,
    {"VTT5", vt5_header, vt5_qualifier}
    ,
    {"FWD1", fwd1_header, fwd1_qualifier}
    ,
    {"FWD2", fwd2_header, fwd2_qualifier}
    ,
    {"", NULL, NULL}
    ,
    {"", NULL, NULL}
    ,
    {"", NULL, NULL}
};

stage_t ikleap_stages_info[KLEAP_NOF_STAGES_MAX];

static shr_error_e
dnx_ikleap_get_stage_info(
    int unit)
{
    int ii, jj;
    uint32 nof_stages;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_stages = dnx_data_kleap_stage_info.kleap_stage_info.nof_kleap_stages_get(unit);
    for (ii = 0; ii < nof_stages; ii++)
    {
        /*
         * Get info per stage
         */
        ikleap_stages_info[ii].stage_type =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->type;
        ikleap_stages_info[ii].kleap_stage_name =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->stage_name;
        ikleap_stages_info[ii].nof_kbrs =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->nof_kbrs;
        ikleap_stages_info[ii].nof_ffc =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->nof_ffc;
        ikleap_stages_info[ii].dbal_table_resource_mapping =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->dbal_table_resource_mapping;
        ikleap_stages_info[ii].dbal_table_kbr_info =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->dbal_table_kbr_info;
        ikleap_stages_info[ii].dbal_table_pd_info =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->dbal_table_pd_info;
        ikleap_stages_info[ii].dbal_kbr_idx_field_per_stage =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->dbal_kbr_idx_field_per_stage;
        ikleap_stages_info[ii].dbal_context_profile_field_per_stage =
            dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit,
                                                                          ii)->dbal_context_profile_field_per_stage;

        /*
         * Get info per stage per KBR
         */
        for (jj = 0; jj < ikleap_stages_info[ii].nof_kbrs; jj++)
        {
            ikleap_stages_info[ii].kbr2physical[jj] =
                dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_kbr_get(unit, ii)->kbr2physical[jj];
            ikleap_stages_info[ii].kbr_if_id[jj] =
                dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_kbr_get(unit, ii)->kbr2key_sig[jj];
        }

        /*
         * Get info per stage per Sub stage
         */
        for (jj = 0; jj < dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, ii)->nof_sub_stages; jj++)
        {
            ikleap_stages_info[ii].kleap_sub_stages_names[jj] =
                dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_sub_stage_get(unit, ii,
                                                                                            jj)->kleap_sub_stage_name;
            ikleap_stages_info[ii].stage_index_in_pipe[jj] =
                dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_sub_stage_get(unit, ii,
                                                                                            jj)->stage_index_in_pipe;
            ikleap_stages_info[ii].dbal_context_enum[jj] =
                dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_sub_stage_get(unit, ii,
                                                                                            jj)->dbal_context_enum;
            ikleap_stages_info[ii].dbal_stage_context_id[jj] =
                dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_sub_stage_get(unit, ii,
                                                                                            jj)->dbal_stage_context_id;
            ikleap_stages_info[ii].dbal_stage_context_properties[jj] =
                dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_sub_stage_get(unit, ii,
                                                                                            jj)->dbal_stage_context_properties;
        }
    }
    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX2_SUPPORT
shr_error_e
dnx2_ikleap_additinial_info_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    uint8 exem_is_lexem = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ikleap_get_stage_info(unit));

    if (sw_state_is_warm_boot(unit))
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_EXEM_MUX, &entry_handle_id));
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LOOKUP_IN_LEXEM, INST_SINGLE, &exem_is_lexem);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** KBR 7 can be configured to SEXEM3 or LEXEM.*/
    if (exem_is_lexem)
    {
        ikleap_stages_info[INDEX_OF_FWD12].kbr2physical[7] = DBAL_PHYSICAL_TABLE_LEXEM;
    }
    else
    {
        ikleap_stages_info[INDEX_OF_FWD12].kbr2physical[7] = DBAL_PHYSICAL_TABLE_SEXEM_3;
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.em.feature_get(unit, dnx_data_mdb_em_is_isem_dpc_in_vtt5))
    {
        ikleap_stages_info[INDEX_OF_VTT45].kbr2physical[2] = DBAL_PHYSICAL_TABLE_ISEM_3;
        ikleap_stages_info[INDEX_OF_VTT45].kbr2physical[3] = DBAL_PHYSICAL_TABLE_ISEM_3;
    }
#endif
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif
