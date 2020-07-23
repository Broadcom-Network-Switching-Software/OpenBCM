/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <sal/appl/sal.h>

#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/drv.h> 
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

char *
dnx_pp_stage_name(
    int unit,
    dnx_pp_stage_e pp_stage)
{
    if ((pp_stage < 0) || (pp_stage >= dnx_data_pp.stages.params_info_get(unit)->key_size[0]))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp stage:%d\n"), pp_stage));
        return NULL;
    }

    return dnx_data_pp.stages.params_get(unit, pp_stage)->name;
}

shr_error_e
dnx_pp_stage_string_to_id(
    int unit,
    char *pp_stage_name,
    dnx_pp_stage_e * pp_stage_id)
{
    dnx_pp_stage_e pp_stage_index;

    SHR_FUNC_INIT_VARS(unit);

    for (pp_stage_index = DNX_PP_STAGE_FIRST; pp_stage_index < DNX_PP_STAGE_NOF; pp_stage_index++)
    {
        if (sal_strcasecmp(dnx_data_pp.stages.params_get(unit, pp_stage_index)->name, pp_stage_name) == 0)
        {
            *pp_stage_id = pp_stage_index;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Stage:%s is not supported by dnx data\n", pp_stage_name);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pp_stage_pema_id(
    int unit,
    char *pp_stage_name,
    int *pema_id_p)
{
    dnx_pp_stage_e pp_stage_id;
    dbal_enum_value_field_pema_id_e pema_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_stage_string_to_id(unit, pp_stage_name, &pp_stage_id));
    pema_id = dnx_data_pp.stages.params_get(unit, pp_stage_id)->pema_id;

    if (pema_id == -1)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }
    else
    {
        *pema_id_p = pema_id;
    }

exit:
    SHR_FUNC_EXIT;
}

char *
dnx_pp_stage_block_name(
    int unit,
    dnx_pp_stage_e pp_stage)
{
    if ((pp_stage < 0) || (pp_stage >= dnx_data_pp.stages.params_info_get(unit)->key_size[0]))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp stage:%d\n"), pp_stage));
        return NULL;
    }

    return dnx_data_pp.stages.params_get(unit, pp_stage)->block;
}

const static dnx_pp_block_info_t pp_block_info[DNX_PP_BLOCK_NOF] = {
    {"Illegal", "Illegal"},
    {"IRPP", "Ingress"},
    {"ITPP", "Ingress"},
    {"ERPP", "Egress"},
    {"ETPP", "Egress"}
};

char *
dnx_pp_block_name(
    int unit,
    dnx_pp_block_e pp_block)
{
    if ((pp_block < DNX_PP_BLOCK_INVALID) || (pp_block >= DNX_PP_BLOCK_NOF))
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp block:%d\n"), pp_block));
        pp_block = DNX_PP_BLOCK_INVALID;
    }

    return (char *) pp_block_info[pp_block].name;
}

shr_error_e
dnx_pp_block_string_to_id(
    int unit,
    char *pp_block_name,
    dnx_pp_block_e * pp_block_id_p)
{
    dnx_pp_block_e pp_block_id;

    SHR_FUNC_INIT_VARS(unit);

    for (pp_block_id = DNX_PP_BLOCK_FIRST; pp_block_id < DNX_PP_BLOCK_NOF; pp_block_id++)
    {
        if (sal_strcasecmp(pp_block_info[pp_block_id].name, pp_block_name) == 0)
        {
            *pp_block_id_p = pp_block_id;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Block:%s is not supported by dnx data\n", pp_block_name);

exit:
    SHR_FUNC_EXIT;
}

char *
dnx_pp_block_direction(
    int unit,
    dnx_pp_block_e pp_block)
{
    if ((pp_block < DNX_PP_BLOCK_INVALID) || (pp_block >= DNX_PP_BLOCK_NOF))
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp block:%d\n"), pp_block));
        pp_block = DNX_PP_BLOCK_INVALID;
    }

    return (char *) pp_block_info[pp_block].direction;
}

shr_error_e
dnx_pp_get_block_id(
    int unit,
    int stage,
    int *block)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((stage >= DNX_PP_STAGE_IRPP_FIRST) && (stage < DNX_PP_STAGE_ERPP_FIRST))
    {
        *block = DNX_PP_BLOCK_IRPP;
    }
    else if ((stage >= DNX_PP_STAGE_ERPP_FIRST) && (stage < DNX_PP_STAGE_ETPP_FIRST))
    {
        *block = DNX_PP_BLOCK_ERPP;
    }
    else if ((stage >= DNX_PP_STAGE_ETPP_FIRST) && (stage < DNX_PP_STAGE_NOF))
    {
        *block = DNX_PP_BLOCK_ETPP;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "stage(%d) not mapped to block\n", stage);
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_pp_stage_is_kleap(
    int unit,
    dnx_pp_stage_e pp_stage)
{
    if ((pp_stage < 0) || (pp_stage >= dnx_data_pp.stages.params_info_get(unit)->key_size[0]))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp stage:%d\n"), pp_stage));
        return 0;
    }

    return dnx_data_pp.stages.params_get(unit, pp_stage)->is_kleap;
}

typedef struct
{
    char *kbr_connect_n;
    dbal_tables_e table_id;
    char *stage_n[2];
} pp_klear_kbr_info_t;


#define KBR_MAX_NUM     3


const static pp_klear_kbr_info_t pp_klear_kbr_info[KBR_MAX_NUM] = {
    {"VTT23", DBAL_TABLE_KLEAP_VT23_RESOURCES_MAPPING, {"VTT2", "VTT3"}},
    {"VTT45", DBAL_TABLE_KLEAP_VT45_RESOURCES_MAPPING, {"VTT4", "VTT5"}},
    {"FWD12", DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING, {"FWD1", "FWD2"}},
};


shr_error_e
dnx_pp_stage_kbr_resolve(
    int unit,
    char *stage_n_in,
    char *stage_n_out)
{
    uint32 entry_handle_id;
    uint32 select;
    char **tokens = NULL;
    uint32 realtokens = 0;
    int i_kbr;
    uint32 kbr_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    tokens = utilex_str_split(stage_n_in, "_", 2, &realtokens);
    if (realtokens != 2 || (tokens == NULL))
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META("Stage:%s has no KBR format\n"), stage_n_in));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    for (i_kbr = 0; i_kbr < KBR_MAX_NUM; i_kbr++)
    {
        if (!sal_strcasecmp(tokens[0], pp_klear_kbr_info[i_kbr].kbr_connect_n))
            break;
    }

    if (i_kbr == KBR_MAX_NUM)
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META("Stage:%s is not KBR supported\n"), stage_n_in));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    
    SHR_SET_CURRENT_ERR(utilex_str_stoul(tokens[1], &kbr_id));
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META("Stage:%s is not KBR encoded\n"), stage_n_in));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, pp_klear_kbr_info[i_kbr].table_id, &entry_handle_id));
    
    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR,
                                                            kbr_id, &select));

        if (select > 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong select value:%d for kbr:%s",
                         select, pp_klear_kbr_info[i_kbr].kbr_connect_n);
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("KBR Encoding:%s was resolved as:%s\n"),
                                     stage_n_in, pp_klear_kbr_info[i_kbr].stage_n[select]));

        sal_strncpy(stage_n_out, pp_klear_kbr_info[i_kbr].stage_n[select], RHNAME_MAX_SIZE - 1);
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
