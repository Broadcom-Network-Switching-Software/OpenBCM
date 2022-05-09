
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_field.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <shared/utilex/utilex_integer_arithmetic.h>








static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_dir_ext_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_dir_ext;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_mdb_dt_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_masks_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_masks_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fes_id_per_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fes_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fes_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fes_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fes_instruction_per_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_fes_id_per_array_get(unit)*dnx_data_field.base_ipmf1.nof_fes_array_get(unit));

    
    define->data = (dnx_data_field.base_ipmf1.nof_fes_id_per_array_get(unit)*dnx_data_field.base_ipmf1.nof_fes_array_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fes_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fes_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_prog_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_prog_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_cs_container_5_selected_bits_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_link_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_link_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_cs_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_cs_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_actions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_actions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_qualifiers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_qualifiers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fes_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_fes_key_select_resolution_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fes_key_select_resolution_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_log_nof_bits_in_fem_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_field.base_ipmf1.log_nof_bits_in_fem_key_select_get(unit);

    
    define->data = 1 << dnx_data_field.base_ipmf1.log_nof_bits_in_fem_key_select_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_fem_key_select_resolution_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_log_nof_bits_in_fem_map_data_field_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_map_data_field_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_field.base_ipmf1.log_nof_bits_in_fem_map_data_field_get(unit);

    
    define->data = 1 << dnx_data_field.base_ipmf1.log_nof_bits_in_fem_map_data_field_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_2_15_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_0_1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_2_15_get(unit), dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_0_1_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_2_15_get(unit), dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_0_1_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_condition_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_condition;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_map_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_map_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_num_fems_with_short_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_num_fems_with_short_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_num_bits_in_fem_field_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_id_per_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.compare.nof_compare_pairs_get(unit);

    
    define->data = dnx_data_field.compare.nof_compare_pairs_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_compare_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_compare_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_l4_ops_ranges_legacy_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_ext_l4_ops_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_pkt_hdr_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_out_lif_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_out_lif_ranges;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_cmp_selection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_cmp_selection;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xF;

    
    define->data = 0xF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_dir_ext_single_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_dir_ext_single_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_dir_ext_first_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_dir_ext_first_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_exem_max_result_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_exem_max_result_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_fes_instruction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fes_instruction_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_fes_pgm_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fes_pgm_id_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0;

    
    define->data = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_exem_pdb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_exem_pdb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_PHYSICAL_TABLE_LEXEM;

    
    define->data = DBAL_PHYSICAL_TABLE_LEXEM;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_tcam_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_tcam;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_exem_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_exem;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_dir_ext_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_dir_ext;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_mdb_dt_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_mdb_dt;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_masks_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_masks_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_fes_id_per_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_fes_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_fes_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_fes_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_fes_instruction_per_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_fes_instruction_per_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf2.nof_fes_id_per_array_get(unit)*dnx_data_field.base_ipmf2.nof_fes_array_get(unit));

    
    define->data = (dnx_data_field.base_ipmf2.nof_fes_id_per_array_get(unit)*dnx_data_field.base_ipmf2.nof_fes_array_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_cs_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_cs_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_nof_qualifiers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_qualifiers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_dir_ext_single_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_dir_ext_single_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_dir_ext_first_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_dir_ext_first_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_exem_max_result_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_exem_max_result_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0;

    
    define->data = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf2_exem_pdb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_exem_pdb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_PHYSICAL_TABLE_SEXEM_3;

    
    define->data = DBAL_PHYSICAL_TABLE_SEXEM_3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_tcam_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_tcam;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_exem_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_exem;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_dir_ext_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_dir_ext;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_mdb_dt_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_mdb_dt;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_masks_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_masks_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_fes_id_per_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_fes_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_fes_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_fes_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_fes_instruction_per_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_fes_instruction_per_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf3.nof_fes_id_per_array_get(unit)*dnx_data_field.base_ipmf3.nof_fes_array_get(unit));

    
    define->data = (dnx_data_field.base_ipmf3.nof_fes_id_per_array_get(unit)*dnx_data_field.base_ipmf3.nof_fes_array_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_fes_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_fes_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_prog_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_prog_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_cs_scratch_pad_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_cs_scratch_pad_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 33;

    
    define->data = 33;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_cs_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_cs_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_actions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_actions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_qualifiers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_qualifiers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_bits_in_fes_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_fes_key_select_resolution_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_fes_key_select_resolution_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_out_lif_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_out_lif_ranges;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_fes_instruction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_fes_instruction_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_fes_pgm_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_fes_pgm_id_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_dir_ext_single_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_dir_ext_single_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_dir_ext_first_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_dir_ext_first_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_exem_max_result_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_exem_max_result_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 1;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 0;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_59_0_4_B0;

    
    define->data = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_59_0_4_B0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf3_exem_pdb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_exem_pdb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_PHYSICAL_TABLE_SEXEM_3;

    
    define->data = DBAL_PHYSICAL_TABLE_SEXEM_3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_keys_alloc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_for_tcam_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_keys_alloc_for_tcam;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_for_exem_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_keys_alloc_for_exem;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_for_dir_ext_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_keys_alloc_for_dir_ext;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_masks_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_masks_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_fes_id_per_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_fes_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_fes_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_fes_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_fes_instruction_per_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_fes_instruction_per_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_epmf.nof_fes_id_per_array_get(unit)*dnx_data_field.base_epmf.nof_fes_array_get(unit));

    
    define->data = (dnx_data_field.base_epmf.nof_fes_id_per_array_get(unit)*dnx_data_field.base_epmf.nof_fes_array_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_fes_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_fes_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_prog_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_prog_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_cs_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_cs_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_actions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_actions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_qualifiers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_qualifiers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_bits_in_fes_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_bits_in_fes_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_bits_in_fes_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_fes_key_select_resolution_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_fes_key_select_resolution_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_fes_instruction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_fes_instruction_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_fes_pgm_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_fes_pgm_id_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_dir_ext_single_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_dir_ext_single_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_dir_ext_first_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_dir_ext_first_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_exem_max_result_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_exem_max_result_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0;

    
    define->data = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_fes_shift_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_fes_shift_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_epmf_exem_pdb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_exem_pdb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_PHYSICAL_TABLE_LEXEM;

    
    define->data = DBAL_PHYSICAL_TABLE_LEXEM;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_base_ifwd2_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ifwd2;
    int define_index = dnx_data_field_base_ifwd2_define_nof_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 38;

    
    define->data = 38;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ifwd2_nof_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ifwd2;
    int define_index = dnx_data_field_base_ifwd2_define_nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ifwd2_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ifwd2;
    int define_index = dnx_data_field_base_ifwd2_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ifwd2_nof_cs_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ifwd2;
    int define_index = dnx_data_field_base_ifwd2_define_nof_cs_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_field_stage_stage_info_set(
    int unit)
{
    int stage_index;
    dnx_data_field_stage_stage_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_stage;
    int table_index = dnx_data_field_stage_table_stage_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_STAGE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_STAGE_NOF;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "0";
    table->values[10].default_val = "0";
    table->values[11].default_val = "0";
    table->values[12].default_val = "0";
    table->values[13].default_val = "0";
    table->values[14].default_val = "0";
    table->values[15].default_val = "0";
    table->values[16].default_val = "0";
    table->values[17].default_val = "0";
    table->values[18].default_val = "0";
    table->values[19].default_val = "0";
    table->values[20].default_val = "0";
    table->values[21].default_val = "0";
    table->values[22].default_val = "0";
    table->values[23].default_val = "0";
    table->values[24].default_val = "0";
    table->values[25].default_val = "0";
    table->values[26].default_val = "0";
    table->values[27].default_val = "0";
    table->values[28].default_val = "0";
    table->values[29].default_val = "0";
    table->values[30].default_val = "0";
    table->values[31].default_val = "0";
    table->values[32].default_val = "0";
    table->values[33].default_val = "0";
    table->values[34].default_val = "0";
    table->values[35].default_val = "0";
    table->values[36].default_val = "0";
    table->values[37].default_val = "0";
    table->values[38].default_val = "0";
    table->values[39].default_val = "0";
    table->values[40].default_val = "0";
    table->values[41].default_val = "0";
    table->values[42].default_val = "0";
    table->values[43].default_val = "0";
    table->values[44].default_val = "0";
    table->values[45].default_val = "0";
    table->values[46].default_val = "0";
    table->values[47].default_val = "0";
    table->values[48].default_val = "0";
    table->values[49].default_val = "0";
    table->values[50].default_val = "0";
    table->values[51].default_val = "-1";
    table->values[52].default_val = "0";
    table->values[53].default_val = "0";
    table->values[54].default_val = "0";
    table->values[55].default_val = "0";
    table->values[56].default_val = "-1";
    table->values[57].default_val = "0";
    table->values[58].default_val = "DBAL_PHYSICAL_TABLE_NONE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_stage_stage_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_stage_table_stage_info");

    
    default_data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = FALSE;
    default_data->nof_ffc = 0;
    default_data->nof_keys = 0;
    default_data->nof_keys_alloc = 0;
    default_data->nof_keys_alloc_for_tcam = 0;
    default_data->nof_keys_alloc_for_exem = 0;
    default_data->nof_keys_alloc_for_dir_ext = 0;
    default_data->nof_keys_alloc_for_mdb_dt = 0;
    default_data->nof_masks_per_fes = 0;
    default_data->nof_fes_id_per_array = 0;
    default_data->nof_fes_array = 0;
    default_data->nof_fes_instruction_per_context = 0;
    default_data->nof_fes_programs = 0;
    default_data->nof_prog_per_fes = 0;
    default_data->cs_scratch_pad_size = 0;
    default_data->cs_container_5_selected_bits_size = 0;
    default_data->nof_contexts = 0;
    default_data->nof_link_profiles = 0;
    default_data->nof_cs_lines = 0;
    default_data->nof_actions = 0;
    default_data->nof_qualifiers = 0;
    default_data->nof_bits_in_fes_action = 0;
    default_data->nof_bits_in_fes_key_select = 0;
    default_data->fes_key_select_resolution_in_bits = 0;
    default_data->fem_condition_ms_bit_min_value = 0;
    default_data->nof_fem_programs = 0;
    default_data->log_nof_bits_in_fem_key_select = 0;
    default_data->nof_bits_in_fem_key_select = 0;
    default_data->fem_key_select_resolution_in_bits = 0;
    default_data->log_nof_bits_in_fem_map_data_field = 0;
    default_data->nof_bits_in_fem_map_data_field = 0;
    default_data->nof_bits_in_fem_action_fems_2_15 = 0;
    default_data->nof_bits_in_fem_action_fems_0_1 = 0;
    default_data->nof_bits_in_fem_action = 0;
    default_data->nof_fem_condition = 0;
    default_data->nof_fem_map_index = 0;
    default_data->nof_fem_id = 0;
    default_data->num_fems_with_short_action = 0;
    default_data->num_bits_in_fem_field_select = 0;
    default_data->nof_fem_id_per_array = 0;
    default_data->nof_fem_array = 0;
    default_data->pbus_header_length = 0;
    default_data->nof_layer_records = 0;
    default_data->layer_record_size = 0;
    default_data->nof_compare_pairs_in_compare_mode = 0;
    default_data->compare_key_size = 0;
    default_data->nof_l4_ops_ranges_legacy = 0;
    default_data->nof_ext_l4_ops_ranges = 0;
    default_data->nof_pkt_hdr_ranges = 0;
    default_data->nof_out_lif_ranges = 0;
    default_data->fes_key_select_for_zero_bit = 0;
    default_data->fes_shift_for_zero_bit = -1;
    default_data->cmp_selection = 0;
    default_data->nof_bits_main_pbus = 0;
    default_data->nof_bits_native_pbus = 0;
    default_data->dir_ext_single_key_size = 0;
    default_data->dir_ext_first_key = -1;
    default_data->exem_max_result_size = 0;
    default_data->exem_pdb = DBAL_PHYSICAL_TABLE_NONE;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_ipmf1.nof_ffc_get(unit);
        data->nof_keys = dnx_data_field.base_ipmf1.nof_keys_get(unit);
        data->nof_keys_alloc = dnx_data_field.base_ipmf1.nof_keys_alloc_get(unit);
        data->nof_keys_alloc_for_tcam = dnx_data_field.base_ipmf1.nof_keys_alloc_for_tcam_get(unit);
        data->nof_keys_alloc_for_exem = dnx_data_field.base_ipmf1.nof_keys_alloc_for_exem_get(unit);
        data->nof_keys_alloc_for_dir_ext = dnx_data_field.base_ipmf1.nof_keys_alloc_for_dir_ext_get(unit);
        data->nof_keys_alloc_for_mdb_dt = dnx_data_field.base_ipmf1.nof_keys_alloc_for_mdb_dt_get(unit);
        data->nof_masks_per_fes = dnx_data_field.base_ipmf1.nof_masks_per_fes_get(unit);
        data->nof_fes_id_per_array = dnx_data_field.base_ipmf1.nof_fes_id_per_array_get(unit);
        data->nof_fes_array = dnx_data_field.base_ipmf1.nof_fes_array_get(unit);
        data->nof_fes_instruction_per_context = dnx_data_field.base_ipmf1.nof_fes_instruction_per_context_get(unit);
        data->nof_fes_programs = dnx_data_field.base_ipmf1.nof_fes_programs_get(unit);
        data->nof_prog_per_fes = dnx_data_field.base_ipmf1.nof_prog_per_fes_get(unit);
        data->cs_container_5_selected_bits_size = dnx_data_field.base_ipmf1.cs_container_5_selected_bits_size_get(unit);
        data->nof_contexts = dnx_data_field.base_ipmf1.nof_contexts_get(unit);
        data->nof_link_profiles = dnx_data_field.base_ipmf1.nof_link_profiles_get(unit);
        data->nof_cs_lines = dnx_data_field.base_ipmf1.nof_cs_lines_get(unit);
        data->nof_actions = dnx_data_field.base_ipmf1.nof_actions_get(unit);
        data->nof_qualifiers = dnx_data_field.base_ipmf1.nof_qualifiers_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_ipmf1.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fes_key_select_get(unit);
        data->fes_key_select_resolution_in_bits = dnx_data_field.base_ipmf1.fes_key_select_resolution_in_bits_get(unit);
        data->fem_condition_ms_bit_min_value = dnx_data_field.base_ipmf1.fem_condition_ms_bit_min_value_get(unit);
        data->nof_fem_programs = dnx_data_field.base_ipmf1.nof_fem_programs_get(unit);
        data->log_nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_key_select_get(unit);
        data->nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fem_key_select_get(unit);
        data->fem_key_select_resolution_in_bits = dnx_data_field.base_ipmf1.fem_key_select_resolution_in_bits_get(unit);
        data->log_nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_action_fems_2_15 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_2_15_get(unit);
        data->nof_bits_in_fem_action_fems_0_1 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_0_1_get(unit);
        data->nof_bits_in_fem_action = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_get(unit);
        data->nof_fem_condition = dnx_data_field.base_ipmf1.nof_fem_condition_get(unit);
        data->nof_fem_map_index = dnx_data_field.base_ipmf1.nof_fem_map_index_get(unit);
        data->nof_fem_id = dnx_data_field.base_ipmf1.nof_fem_id_get(unit);
        data->num_fems_with_short_action = dnx_data_field.base_ipmf1.num_fems_with_short_action_get(unit);
        data->num_bits_in_fem_field_select = dnx_data_field.base_ipmf1.num_bits_in_fem_field_select_get(unit);
        data->nof_fem_id_per_array = dnx_data_field.base_ipmf1.nof_fem_id_per_array_get(unit);
        data->nof_fem_array = dnx_data_field.base_ipmf1.nof_fem_array_get(unit);
        data->pbus_header_length = dnx_data_field.qual.ingress_pbus_header_length_get(unit);
        data->nof_layer_records = dnx_data_field.qual.ingress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.ingress_layer_record_size_get(unit);
        data->nof_compare_pairs_in_compare_mode = dnx_data_field.base_ipmf1.nof_compare_pairs_in_compare_mode_get(unit);
        data->compare_key_size = dnx_data_field.base_ipmf1.compare_key_size_get(unit);
        data->nof_l4_ops_ranges_legacy = dnx_data_field.base_ipmf1.nof_l4_ops_ranges_legacy_get(unit);
        data->nof_ext_l4_ops_ranges = dnx_data_field.base_ipmf1.nof_ext_l4_ops_ranges_get(unit);
        data->nof_pkt_hdr_ranges = dnx_data_field.base_ipmf1.nof_pkt_hdr_ranges_get(unit);
        data->nof_out_lif_ranges = dnx_data_field.base_ipmf1.nof_out_lif_ranges_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_ipmf1.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_ipmf1.fes_shift_for_zero_bit_get(unit);
        data->cmp_selection = dnx_data_field.base_ipmf1.cmp_selection_get(unit);
        data->nof_bits_main_pbus = dnx_data_field.qual.ipmf1_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_ipmf1.dir_ext_single_key_size_get(unit);
        data->dir_ext_first_key = dnx_data_field.base_ipmf1.dir_ext_first_key_get(unit);
        data->exem_max_result_size = dnx_data_field.base_ipmf1.exem_max_result_size_get(unit);
        data->exem_pdb = dnx_data_field.base_ipmf1.exem_pdb_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_ipmf2.nof_ffc_get(unit);
        data->nof_keys = dnx_data_field.base_ipmf2.nof_keys_get(unit);
        data->nof_keys_alloc = dnx_data_field.base_ipmf2.nof_keys_alloc_get(unit);
        data->nof_keys_alloc_for_tcam = dnx_data_field.base_ipmf2.nof_keys_alloc_for_tcam_get(unit);
        data->nof_keys_alloc_for_exem = dnx_data_field.base_ipmf2.nof_keys_alloc_for_exem_get(unit);
        data->nof_keys_alloc_for_dir_ext = dnx_data_field.base_ipmf2.nof_keys_alloc_for_dir_ext_get(unit);
        data->nof_keys_alloc_for_mdb_dt = dnx_data_field.base_ipmf2.nof_keys_alloc_for_mdb_dt_get(unit);
        data->nof_masks_per_fes = dnx_data_field.base_ipmf2.nof_masks_per_fes_get(unit);
        data->nof_fes_id_per_array = dnx_data_field.base_ipmf2.nof_fes_id_per_array_get(unit);
        data->nof_fes_array = dnx_data_field.base_ipmf2.nof_fes_array_get(unit);
        data->nof_fes_instruction_per_context = dnx_data_field.base_ipmf2.nof_fes_instruction_per_context_get(unit);
        data->nof_fes_programs = dnx_data_field.base_ipmf1.nof_fes_programs_get(unit);
        data->nof_prog_per_fes = dnx_data_field.base_ipmf1.nof_prog_per_fes_get(unit);
        data->nof_contexts = dnx_data_field.base_ipmf2.nof_contexts_get(unit);
        data->nof_cs_lines = dnx_data_field.base_ipmf2.nof_cs_lines_get(unit);
        data->nof_qualifiers = dnx_data_field.base_ipmf2.nof_qualifiers_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_ipmf1.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fes_key_select_get(unit);
        data->fes_key_select_resolution_in_bits = dnx_data_field.base_ipmf1.fes_key_select_resolution_in_bits_get(unit);
        data->fem_condition_ms_bit_min_value = dnx_data_field.base_ipmf1.fem_condition_ms_bit_min_value_get(unit);
        data->nof_fem_programs = dnx_data_field.base_ipmf1.nof_fem_programs_get(unit);
        data->log_nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_key_select_get(unit);
        data->nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fem_key_select_get(unit);
        data->fem_key_select_resolution_in_bits = dnx_data_field.base_ipmf1.fem_key_select_resolution_in_bits_get(unit);
        data->log_nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_action_fems_2_15 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_2_15_get(unit);
        data->nof_bits_in_fem_action_fems_0_1 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_0_1_get(unit);
        data->nof_bits_in_fem_action = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_get(unit);
        data->nof_fem_condition = dnx_data_field.base_ipmf1.nof_fem_condition_get(unit);
        data->nof_fem_map_index = dnx_data_field.base_ipmf1.nof_fem_map_index_get(unit);
        data->nof_fem_id = dnx_data_field.base_ipmf1.nof_fem_id_get(unit);
        data->num_fems_with_short_action = dnx_data_field.base_ipmf1.num_fems_with_short_action_get(unit);
        data->num_bits_in_fem_field_select = dnx_data_field.base_ipmf1.num_bits_in_fem_field_select_get(unit);
        data->nof_fem_id_per_array = dnx_data_field.base_ipmf1.nof_fem_id_per_array_get(unit);
        data->nof_fem_array = dnx_data_field.base_ipmf1.nof_fem_array_get(unit);
        data->pbus_header_length = dnx_data_field.qual.ingress_pbus_header_length_get(unit);
        data->nof_layer_records = dnx_data_field.qual.ingress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.ingress_layer_record_size_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_ipmf2.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_ipmf2.fes_shift_for_zero_bit_get(unit);
        data->nof_bits_main_pbus = dnx_data_field.qual.ipmf1_pbus_size_get(unit);
        data->nof_bits_native_pbus = dnx_data_field.qual.ipmf2_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_ipmf2.dir_ext_single_key_size_get(unit);
        data->dir_ext_first_key = dnx_data_field.base_ipmf2.dir_ext_first_key_get(unit);
        data->exem_max_result_size = dnx_data_field.base_ipmf2.exem_max_result_size_get(unit);
        data->exem_pdb = dnx_data_field.base_ipmf2.exem_pdb_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_ipmf3.nof_ffc_get(unit);
        data->nof_keys = dnx_data_field.base_ipmf3.nof_keys_get(unit);
        data->nof_keys_alloc = dnx_data_field.base_ipmf3.nof_keys_alloc_get(unit);
        data->nof_keys_alloc_for_tcam = dnx_data_field.base_ipmf3.nof_keys_alloc_for_tcam_get(unit);
        data->nof_keys_alloc_for_exem = dnx_data_field.base_ipmf3.nof_keys_alloc_for_exem_get(unit);
        data->nof_keys_alloc_for_dir_ext = dnx_data_field.base_ipmf3.nof_keys_alloc_for_dir_ext_get(unit);
        data->nof_keys_alloc_for_mdb_dt = dnx_data_field.base_ipmf2.nof_keys_alloc_for_mdb_dt_get(unit);
        data->nof_masks_per_fes = dnx_data_field.base_ipmf3.nof_masks_per_fes_get(unit);
        data->nof_fes_id_per_array = dnx_data_field.base_ipmf3.nof_fes_id_per_array_get(unit);
        data->nof_fes_array = dnx_data_field.base_ipmf3.nof_fes_array_get(unit);
        data->nof_fes_instruction_per_context = (dnx_data_field.base_ipmf3.nof_fes_instruction_per_context_get(unit)-dnx_data_field.base_ipmf3.nof_fes_used_by_sdk_get(unit));
        data->nof_fes_programs = dnx_data_field.base_ipmf3.nof_fes_programs_get(unit);
        data->nof_prog_per_fes = dnx_data_field.base_ipmf3.nof_prog_per_fes_get(unit);
        data->cs_scratch_pad_size = dnx_data_field.base_ipmf3.cs_scratch_pad_size_get(unit);
        data->nof_contexts = dnx_data_field.base_ipmf3.nof_contexts_get(unit);
        data->nof_cs_lines = dnx_data_field.base_ipmf3.nof_cs_lines_get(unit);
        data->nof_actions = dnx_data_field.base_ipmf3.nof_actions_get(unit);
        data->nof_qualifiers = dnx_data_field.base_ipmf3.nof_qualifiers_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_ipmf3.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_ipmf3.nof_bits_in_fes_key_select_get(unit);
        data->fes_key_select_resolution_in_bits = dnx_data_field.base_ipmf3.fes_key_select_resolution_in_bits_get(unit);
        data->nof_layer_records = dnx_data_field.qual.ingress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.ingress_layer_record_size_get(unit);
        data->nof_out_lif_ranges = dnx_data_field.base_ipmf3.nof_out_lif_ranges_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_ipmf3.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_ipmf3.fes_shift_for_zero_bit_get(unit);
        data->nof_bits_main_pbus = dnx_data_field.qual.ipmf3_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_ipmf3.dir_ext_single_key_size_get(unit);
        data->dir_ext_first_key = dnx_data_field.base_ipmf3.dir_ext_first_key_get(unit);
        data->exem_max_result_size = dnx_data_field.base_ipmf3.exem_max_result_size_get(unit);
        data->exem_pdb = dnx_data_field.base_ipmf3.exem_pdb_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_epmf.nof_ffc_get(unit);
        data->nof_keys = dnx_data_field.base_epmf.nof_keys_get(unit);
        data->nof_keys_alloc = dnx_data_field.base_epmf.nof_keys_alloc_get(unit);
        data->nof_keys_alloc_for_tcam = dnx_data_field.base_epmf.nof_keys_alloc_for_tcam_get(unit);
        data->nof_keys_alloc_for_exem = dnx_data_field.base_epmf.nof_keys_alloc_for_exem_get(unit);
        data->nof_keys_alloc_for_dir_ext = dnx_data_field.base_epmf.nof_keys_alloc_for_dir_ext_get(unit);
        data->nof_masks_per_fes = dnx_data_field.base_epmf.nof_masks_per_fes_get(unit);
        data->nof_fes_id_per_array = dnx_data_field.base_epmf.nof_fes_id_per_array_get(unit);
        data->nof_fes_array = dnx_data_field.base_epmf.nof_fes_array_get(unit);
        data->nof_fes_instruction_per_context = dnx_data_field.base_epmf.nof_fes_instruction_per_context_get(unit);
        data->nof_fes_programs = dnx_data_field.base_epmf.nof_fes_programs_get(unit);
        data->nof_prog_per_fes = dnx_data_field.base_epmf.nof_prog_per_fes_get(unit);
        data->nof_contexts = dnx_data_field.base_epmf.nof_contexts_get(unit);
        data->nof_cs_lines = dnx_data_field.base_epmf.nof_cs_lines_get(unit);
        data->nof_actions = dnx_data_field.base_epmf.nof_actions_get(unit);
        data->nof_qualifiers = dnx_data_field.base_epmf.nof_qualifiers_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_epmf.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_epmf.nof_bits_in_fes_key_select_get(unit);
        data->fes_key_select_resolution_in_bits = dnx_data_field.base_epmf.fes_key_select_resolution_in_bits_get(unit);
        data->pbus_header_length = dnx_data_field.qual.egress_pbus_header_length_get(unit);
        data->nof_layer_records = dnx_data_field.qual.egress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.egress_layer_record_size_get(unit);
        data->nof_l4_ops_ranges_legacy = dnx_data_field.base_epmf.nof_l4_ops_ranges_legacy_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_epmf.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_epmf.fes_shift_for_zero_bit_get(unit);
        data->nof_bits_main_pbus = dnx_data_field.qual.epmf_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_epmf.dir_ext_single_key_size_get(unit);
        data->dir_ext_first_key = dnx_data_field.base_epmf.dir_ext_first_key_get(unit);
        data->exem_max_result_size = dnx_data_field.base_epmf.exem_max_result_size_get(unit);
        data->exem_pdb = dnx_data_field.base_epmf.exem_pdb_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_ifwd2.nof_ffc_get(unit);
        data->nof_keys = dnx_data_field.base_ifwd2.nof_keys_get(unit);
        data->nof_contexts = dnx_data_field.base_ifwd2.nof_contexts_get(unit);
        data->nof_cs_lines = dnx_data_field.base_ifwd2.nof_cs_lines_get(unit);
        data->pbus_header_length = dnx_data_field.qual.ingress_pbus_header_length_get(unit);
        data->nof_layer_records = dnx_data_field.qual.ingress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.ingress_layer_record_size_get(unit);
        data->nof_bits_main_pbus = dnx_data_field.qual.ifwd2_pbus_size_get(unit);
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, 0);
        data->valid = TRUE;
        data->nof_masks_per_fes = dnx_data_field.ace.nof_masks_per_fes_get(unit);
        data->nof_fes_id_per_array = dnx_data_field.ace.nof_fes_id_per_array_get(unit);
        data->nof_fes_array = dnx_data_field.ace.nof_fes_array_get(unit);
        data->nof_fes_instruction_per_context = dnx_data_field.ace.nof_fes_instruction_per_context_get(unit);
        data->nof_fes_programs = dnx_data_field.ace.nof_fes_programs_get(unit);
        data->nof_prog_per_fes = dnx_data_field.ace.nof_prog_per_fes_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.ace.nof_bits_in_fes_action_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.ace.fes_shift_for_zero_bit_get(unit);
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, 0);
        data->valid = dnx_data_field.features.extended_l4_ops_get(unit);
        data->nof_bits_main_pbus = dnx_data_field.qual.l4_ops_pbus_size_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_field_external_tcam_has_acl_context_profile_mapping_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int feature_index = dnx_data_field_external_tcam_has_acl_context_profile_mapping;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_external_tcam_max_single_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_max_single_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_nof_acl_keys_master_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_nof_acl_keys_master_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_nof_acl_keys_fg_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_nof_acl_keys_fg_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_nof_keys_total_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_nof_keys_total;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_min_acl_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_min_acl_nof_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_max_fwd_context_num_for_one_apptype_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_max_fwd_context_num_for_one_apptype;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_max_acl_context_num_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_max_acl_context_num;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ifwd2.nof_contexts_get(unit);

    
    define->data = dnx_data_field.base_ifwd2.nof_contexts_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_size_apptype_profile_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_size_apptype_profile_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_key_bmp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_key_bmp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x30030;

    
    define->data = 0x30030;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_apptype_user_1st_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_apptype_user_1st;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_apptype_user_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_apptype_user_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_max_payload_size_per_opcode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_max_payload_size_per_opcode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 248;

    
    define->data = 248;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_nof_total_lookups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_nof_total_lookups;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_external_tcam_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int define_index = dnx_data_field_external_tcam_define_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP;

    
    define->data = DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_external_tcam_ffc_to_quad_and_group_map_set(
    int unit)
{
    int ffc_id_index;
    dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_external_tcam;
    int table_index = dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_field.base_ifwd2.nof_ffc_get(unit);
    table->info_get.key_size[0] = dnx_data_field.base_ifwd2.nof_ffc_get(unit);

    
    table->values[0].default_val = "SAL_UINT32_MAX";
    table->values[1].default_val = "SAL_UINT32_MAX";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map");

    
    default_data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->quad_id = SAL_UINT32_MAX;
    default_data->group_id = SAL_UINT32_MAX;
    
    for (ffc_id_index = 0; ffc_id_index < table->keys[0].size; ffc_id_index++)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, ffc_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->quad_id = 0;
        data->group_id = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->quad_id = 0;
        data->group_id = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_field_tcam_tcam_80b_traffic_safe_write_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int feature_index = dnx_data_field_tcam_tcam_80b_traffic_safe_write;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_mixed_ratio_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int feature_index = dnx_data_field_tcam_mixed_ratio_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_tcam_access_hw_mix_ratio_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_access_hw_mix_ratio_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_direct_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_direct_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_key_size_half_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_key_size_half;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_key_size_single_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_key_size_single;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_key_size_double_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_key_size_double;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_dt_max_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_dt_max_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_dt_max_key_size_small_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_dt_max_key_size_small_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_action_size_half_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_action_size_half;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_action_size_single_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_action_size_single;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_action_size_double_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_action_size_double;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_key_mode_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_key_mode_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_entry_size_single_key_hw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_entry_size_single_key_hw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (2*((dnx_data_field.tcam.key_size_half_get(unit))+(dnx_data_field.tcam.key_mode_size_get(unit))));

    
    define->data = (2*((dnx_data_field.tcam.key_size_half_get(unit))+(dnx_data_field.tcam.key_mode_size_get(unit))));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_entry_size_single_key_shadow_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_entry_size_single_key_shadow;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_field.tcam.entry_size_single_key_hw_get(unit))+dnx_data_field.tcam.entry_size_single_valid_bits_hw_get(unit));

    
    define->data = ((dnx_data_field.tcam.entry_size_single_key_hw_get(unit))+dnx_data_field.tcam.entry_size_single_valid_bits_hw_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_entry_size_single_valid_bits_hw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_entry_size_single_valid_bits_hw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_hw_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_hw_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_big_bank_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_big_bank_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_small_bank_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_small_bank_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_big_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_big_banks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_small_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_small_banks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_banks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_payload_tables_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_payload_tables;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_access_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_access_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_action_width_selector_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_action_width_selector_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_entries_160_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_entries_160_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25600;

    
    define->data = 25600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_entries_80_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_entries_80_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 51200;

    
    define->data = 51200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_tcam_banks_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_tcam_banks_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 65536;

    
    define->data = 65536;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_tcam_banks_last_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_tcam_banks_last_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 61952;

    
    define->data = 61952;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_tcam_handlers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_tcam_handlers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.tcam.nof_access_profiles_get(unit);

    
    define->data = dnx_data_field.tcam.nof_access_profiles_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_max_prefix_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_max_prefix_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_max_prefix_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_max_prefix_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_keys_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_keys_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_access_profile_half_key_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_access_profile_half_key_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_access_profile_single_key_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_access_profile_single_key_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_access_profile_double_key_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_access_profile_double_key_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_tcam_entries_per_hit_indication_entry_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_max_tcam_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_max_tcam_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x7fffffff;

    
    define->data = 0x7fffffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_big_banks_srams_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_big_banks_srams;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_field.tcam.nof_big_banks_get(unit))*(dnx_data_field.tcam.action_width_selector_size_get(unit)));

    
    define->data = ((dnx_data_field.tcam.nof_big_banks_get(unit))*(dnx_data_field.tcam.action_width_selector_size_get(unit)));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_small_banks_srams_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_small_banks_srams;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_field.tcam.nof_small_banks_get(unit))*(dnx_data_field.tcam.action_width_selector_size_get(unit)));

    
    define->data = ((dnx_data_field.tcam.nof_small_banks_get(unit))*(dnx_data_field.tcam.action_width_selector_size_get(unit)));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_big_bank_lines_per_sram_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_big_bank_lines_per_sram;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_field.tcam.nof_big_bank_lines_get(unit))/((dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit))*2));

    
    define->data = ((dnx_data_field.tcam.nof_big_bank_lines_get(unit))/((dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit))*2));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_nof_small_bank_lines_per_sram_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_small_bank_lines_per_sram;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_field.tcam.nof_small_bank_lines_get(unit))/((dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit))*2));

    
    define->data = ((dnx_data_field.tcam.nof_small_bank_lines_get(unit))/((dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit))*2));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_app_db_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_app_db_id_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tcam_tcam_indirect_command_version_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_tcam_indirect_command_version;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_group_nof_fgs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_group;
    int define_index = dnx_data_field_group_define_nof_fgs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_group_nof_action_per_fg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_group;
    int define_index = dnx_data_field_group_define_nof_action_per_fg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_group_nof_quals_per_fg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_group;
    int define_index = dnx_data_field_group_define_nof_quals_per_fg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_group_nof_keys_per_fg_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_group;
    int define_index = dnx_data_field_group_define_nof_keys_per_fg_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.tcam.nof_keys_max_get(unit), dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_field.tcam.nof_keys_max_get(unit), dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_group_id_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_group;
    int define_index = dnx_data_field_group_define_id_fec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.group.nof_fgs_get(unit)-1;

    
    define->data = dnx_data_field.group.nof_fgs_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_group_payload_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_group;
    int define_index = dnx_data_field_group_define_payload_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.tcam.action_size_double_get(unit), dnx_data_field.tcam.key_size_double_get(unit), dnx_data_field.exem.max_result_size_all_stages_get(unit), dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.tcam.action_size_double_get(unit), dnx_data_field.tcam.key_size_double_get(unit), dnx_data_field.exem.max_result_size_all_stages_get(unit), dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_efes_max_nof_key_selects_per_field_io_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_efes;
    int define_index = dnx_data_field_efes_define_max_nof_key_selects_per_field_io;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_efes_max_nof_field_ios_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_efes;
    int define_index = dnx_data_field_efes_define_max_nof_field_ios;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_efes_epmf_exem_zero_padding_added_to_field_io_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_efes;
    int define_index = dnx_data_field_efes_define_epmf_exem_zero_padding_added_to_field_io;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_efes_key_select_properties_set(
    int unit)
{
    int stage_index;
    int field_io_index;
    dnx_data_field_efes_key_select_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_efes;
    int table_index = dnx_data_field_efes_table_key_select_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_STAGE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_STAGE_NOF;
    table->keys[1].size = DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS;
    table->info_get.key_size[1] = DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS;

    
    table->values[0].default_val = "{-1}";
    table->values[1].default_val = "{0}";
    table->values[2].default_val = "{0}";
    table->values[3].default_val = "{0}";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_efes_key_select_properties_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_field_efes_table_key_select_properties");

    
    default_data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    for (int i = 0; i < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->key_select[i] = -1;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->num_bits[i] = 0;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->lsb[i] = 0;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->num_bits_not_on_key[i] = 0;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        for (field_io_index = 0; field_io_index < table->keys[1].size; field_io_index++)
        {
            data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, field_io_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0);
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 32;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 64;
        data->lsb[3] = 96;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 64;
        data->lsb[3] = 96;
        data->lsb[4] = 0;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 64;
        data->lsb[3] = 96;
        data->lsb[4] = 0;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4_5 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4_5);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 64;
        data->lsb[3] = 96;
        data->lsb[4] = 0;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5_6 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5_6);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 64;
        data->lsb[3] = 96;
        data->lsb[4] = 0;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6_7 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6_7);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 64;
        data->lsb[3] = 96;
        data->lsb[4] = 0;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_LEXEM < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_LEXEM);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 28;
        data->num_bits[1] = 60;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 28;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 36;
        data->num_bits_not_on_key[1] = 4;
        data->num_bits_not_on_key[2] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_SEXEM < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_SEXEM);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_59_0_4_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 28;
        data->num_bits[1] = 60;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 28;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 36;
        data->num_bits_not_on_key[1] = 4;
        data->num_bits_not_on_key[2] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_55_0_8_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_87_24;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_119_56;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 24;
        data->num_bits[1] = 56;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 24;
        data->lsb[3] = 56;
        data->lsb[4] = 88;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 40;
        data->num_bits_not_on_key[1] = 8;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIRECT_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_DIRECT_1);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_55_0_8_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_87_24;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_56;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_88;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 24;
        data->num_bits[1] = 56;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 32;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 24;
        data->lsb[3] = 56;
        data->lsb[4] = 88;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 40;
        data->num_bits_not_on_key[1] = 8;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0);
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 64;
        data->num_bits[5] = 32;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 32;
        data->lsb[3] = 64;
        data->lsb[4] = 96;
        data->lsb[5] = 128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 32;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_1);
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 64;
        data->num_bits[5] = 32;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 32;
        data->lsb[3] = 64;
        data->lsb[4] = 96;
        data->lsb[5] = 128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 32;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0_1);
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64;
        data->key_select[9] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96;
        data->key_select[10] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 64;
        data->num_bits[5] = 64;
        data->num_bits[6] = 64;
        data->num_bits[7] = 64;
        data->num_bits[8] = 64;
        data->num_bits[9] = 64;
        data->num_bits[10] = 32;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 32;
        data->lsb[3] = 64;
        data->lsb[4] = 96;
        data->lsb[5] = 128;
        data->lsb[6] = 160;
        data->lsb[7] = 192;
        data->lsb[8] = 224;
        data->lsb[9] = 256;
        data->lsb[10] = 288;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 32;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
        data->num_bits_not_on_key[9] = 0;
        data->num_bits_not_on_key[10] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0);
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_63_0;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 32;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 64;
        data->lsb[3] = 96;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_SEXEM < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_SEXEM);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_59_0_4_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 28;
        data->num_bits[1] = 60;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 28;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 36;
        data->num_bits_not_on_key[1] = 4;
        data->num_bits_not_on_key[2] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_31_0_KEY_B_159_128;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_55_0_8_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_87_24;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_119_56;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 24;
        data->num_bits[1] = 56;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 24;
        data->lsb[3] = 56;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 40;
        data->num_bits_not_on_key[1] = 8;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0);
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_63_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_95_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_127_64;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_159_96;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_31_0_KEY_A_159_128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 64;
        data->num_bits[5] = 32;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 32;
        data->lsb[3] = 64;
        data->lsb[4] = 96;
        data->lsb[5] = 128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 32;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_1);
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_31_0_KEY_A_159_128;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_63_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_95_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_127_64;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_159_96;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_31_0_KEY_B_159_128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 64;
        data->num_bits[5] = 32;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 32;
        data->lsb[3] = 64;
        data->lsb[4] = 96;
        data->lsb[5] = 128;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 32;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0_1);
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_63_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_95_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_127_64;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_159_96;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_31_0_KEY_A_159_128;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_63_0;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_95_32;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_127_64;
        data->key_select[9] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_159_96;
        data->key_select[10] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_31_0_KEY_B_159_128;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        data->num_bits[4] = 64;
        data->num_bits[5] = 64;
        data->num_bits[6] = 64;
        data->num_bits[7] = 64;
        data->num_bits[8] = 64;
        data->num_bits[9] = 64;
        data->num_bits[10] = 32;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 32;
        data->lsb[3] = 64;
        data->lsb[4] = 96;
        data->lsb[5] = 128;
        data->lsb[6] = 160;
        data->lsb[7] = 192;
        data->lsb[8] = 224;
        data->lsb[9] = 256;
        data->lsb[10] = 288;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 32;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
        data->num_bits_not_on_key[9] = 0;
        data->num_bits_not_on_key[10] = 0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_LEXEM < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DBAL_ENUM_FVAL_FIELD_IO_LEXEM);
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 60;
        data->num_bits[1] = 28;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 32;
        data->lsb[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1);
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 64;
        data->num_bits[1] = 32;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1 < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 64;
        data->num_bits[2] = 64;
        data->num_bits[3] = 64;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 32;
        data->lsb[3] = 64;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 32;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DBAL_ENUM_FVAL_FIELD_IO_ACE < table->keys[1].size)
    {
        data = (dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DBAL_ENUM_FVAL_FIELD_IO_ACE);
        data->key_select[0] = 0;
        data->num_bits[0] = 104;
        data->lsb[0] = 0;
        data->num_bits_not_on_key[0] = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_fem_max_nof_key_selects_per_field_io_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_fem;
    int define_index = dnx_data_field_fem_define_max_nof_key_selects_per_field_io;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_fem_nof_condition_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_fem;
    int define_index = dnx_data_field_fem_define_nof_condition_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_fem_key_select_properties_set(
    int unit)
{
    int field_io_index;
    dnx_data_field_fem_key_select_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_fem;
    int table_index = dnx_data_field_fem_table_key_select_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS;
    table->info_get.key_size[0] = DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS;

    
    table->values[0].default_val = "{-1}";
    table->values[1].default_val = "{0}";
    table->values[2].default_val = "{0}";
    table->values[3].default_val = "{0}";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_fem_key_select_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_fem_table_key_select_properties");

    
    default_data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    for (int i = 0; i < DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->key_select[i] = -1;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->num_bits[i] = 0;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->lsb[i] = 0;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; i++)  
        default_data->num_bits_not_on_key[i] = 0;
    
    for (field_io_index = 0; field_io_index < table->keys[0].size; field_io_index++)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, field_io_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0, 0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 16;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 16;
        data->lsb[2] = 32;
        data->lsb[3] = 48;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1, 0);
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 32;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 16;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 16;
        data->lsb[2] = 32;
        data->lsb[3] = 48;
        data->lsb[4] = 64;
        data->lsb[5] = 80;
        data->lsb[6] = 96;
        data->lsb[7] = 112;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 0;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 16;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 16;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4_5 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4_5, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 16;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5_6 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5_6, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 16;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6_7 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6_7, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 16;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_LEXEM, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_27_0_4_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_43_12;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_59_28;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 12;
        data->num_bits[1] = 28;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 12;
        data->lsb[3] = 28;
        data->lsb[4] = 44;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 20;
        data->num_bits_not_on_key[1] = 4;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_SEXEM < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_SEXEM, 0);
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_43_12;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_59_28;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 12;
        data->num_bits[1] = 28;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 16;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 12;
        data->lsb[3] = 28;
        data->lsb[4] = 44;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 20;
        data->num_bits_not_on_key[1] = 4;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_23_0_8_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_39_8;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_55_24;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_71_40;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_87_56;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_103_72;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_119_88;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 8;
        data->num_bits[1] = 24;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 16;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 8;
        data->lsb[3] = 24;
        data->lsb[4] = 40;
        data->lsb[5] = 56;
        data->lsb[6] = 72;
        data->lsb[7] = 88;
        data->lsb[8] = 104;
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 24;
        data->num_bits_not_on_key[1] = 8;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_DIRECT_1 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_DIRECT_1, 0);
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_23_0_8_B0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_39_8;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_55_24;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_71_40;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_87_56;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_103_72;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_119_88;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 8;
        data->num_bits[1] = 24;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 8;
        data->lsb[3] = 24;
        data->lsb[4] = 40;
        data->lsb[5] = 56;
        data->lsb[6] = 72;
        data->lsb[7] = 88;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 24;
        data->num_bits_not_on_key[1] = 8;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0, 0);
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112;
        data->key_select[9] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128;
        data->key_select[10] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 32;
        data->num_bits[9] = 32;
        data->num_bits[10] = 16;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        data->lsb[9] = 128;
        data->lsb[10] = 144;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
        data->num_bits_not_on_key[9] = 0;
        data->num_bits_not_on_key[10] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_1 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_1, 0);
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112;
        data->key_select[9] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128;
        data->key_select[10] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 32;
        data->num_bits[9] = 32;
        data->num_bits[10] = 16;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        data->lsb[9] = 128;
        data->lsb[10] = 144;
        SHR_RANGE_VERIFY(11, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
        data->num_bits_not_on_key[9] = 0;
        data->num_bits_not_on_key[10] = 0;
    }
    if (DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0_1 < table->keys[0].size)
    {
        data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44;
        data->key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0;
        data->key_select[2] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16;
        data->key_select[3] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32;
        data->key_select[4] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48;
        data->key_select[5] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64;
        data->key_select[6] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80;
        data->key_select[7] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96;
        data->key_select[8] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112;
        data->key_select[9] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128;
        data->key_select[10] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144;
        data->key_select[11] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0;
        data->key_select[12] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16;
        data->key_select[13] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32;
        data->key_select[14] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48;
        data->key_select[15] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64;
        data->key_select[16] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80;
        data->key_select[17] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96;
        data->key_select[18] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112;
        data->key_select[19] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128;
        data->key_select[20] = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits[0] = 16;
        data->num_bits[1] = 32;
        data->num_bits[2] = 32;
        data->num_bits[3] = 32;
        data->num_bits[4] = 32;
        data->num_bits[5] = 32;
        data->num_bits[6] = 32;
        data->num_bits[7] = 32;
        data->num_bits[8] = 32;
        data->num_bits[9] = 32;
        data->num_bits[10] = 32;
        data->num_bits[11] = 32;
        data->num_bits[12] = 32;
        data->num_bits[13] = 32;
        data->num_bits[14] = 32;
        data->num_bits[15] = 32;
        data->num_bits[16] = 32;
        data->num_bits[17] = 32;
        data->num_bits[18] = 32;
        data->num_bits[19] = 32;
        data->num_bits[20] = 16;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->lsb[0] = 0;
        data->lsb[1] = 0;
        data->lsb[2] = 16;
        data->lsb[3] = 32;
        data->lsb[4] = 48;
        data->lsb[5] = 64;
        data->lsb[6] = 80;
        data->lsb[7] = 96;
        data->lsb[8] = 112;
        data->lsb[9] = 128;
        data->lsb[10] = 144;
        data->lsb[11] = 160;
        data->lsb[12] = 176;
        data->lsb[13] = 192;
        data->lsb[14] = 208;
        data->lsb[15] = 224;
        data->lsb[16] = 240;
        data->lsb[17] = 256;
        data->lsb[18] = 272;
        data->lsb[19] = 288;
        data->lsb[20] = 304;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, _SHR_E_INTERNAL, "out of bound access to array")
        data->num_bits_not_on_key[0] = 16;
        data->num_bits_not_on_key[1] = 0;
        data->num_bits_not_on_key[2] = 0;
        data->num_bits_not_on_key[3] = 0;
        data->num_bits_not_on_key[4] = 0;
        data->num_bits_not_on_key[5] = 0;
        data->num_bits_not_on_key[6] = 0;
        data->num_bits_not_on_key[7] = 0;
        data->num_bits_not_on_key[8] = 0;
        data->num_bits_not_on_key[9] = 0;
        data->num_bits_not_on_key[10] = 0;
        data->num_bits_not_on_key[11] = 0;
        data->num_bits_not_on_key[12] = 0;
        data->num_bits_not_on_key[13] = 0;
        data->num_bits_not_on_key[14] = 0;
        data->num_bits_not_on_key[15] = 0;
        data->num_bits_not_on_key[16] = 0;
        data->num_bits_not_on_key[17] = 0;
        data->num_bits_not_on_key[18] = 0;
        data->num_bits_not_on_key[19] = 0;
        data->num_bits_not_on_key[20] = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_context_default_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_ftmh_mc_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_ftmh_mc_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-14);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-14);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-1);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-1);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_pph_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_pph_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-2);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-2);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_pph_fhei_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_pph_fhei_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-15);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-15);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_1588_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_1588_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-13);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-13);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_1588_pph_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_1588_pph_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-16);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-16);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_1588_pph_fhei_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_1588_pph_fhei_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-17);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-17);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_j1_itmh_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_j1_itmh_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-6);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-6);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_j1_itmh_pph_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_j1_itmh_pph_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-7);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-7);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_oam_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_oam_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-4);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-4);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_oam_reflector_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_oam_reflector_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-5);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-5);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_oam_upmep_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_oam_upmep_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-10);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-10);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_j1_learning_2ndpass_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_j1_learning_2ndpass_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-8);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-8);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_rch_remove_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_rch_remove_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-9);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-9);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_rch_extended_encap_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_rch_extended_encap_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-12);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-12);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_nat_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_nat_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-11);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-11);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_nof_hash_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_nof_hash_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_pph_unsupported_ext_ipmf2_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_pph_unsupported_ext_ipmf2_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-18);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-18);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_pph_fhei_unsupported_ext_ipmf2_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_pph_fhei_unsupported_ext_ipmf2_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-19);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-19);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_pph_fhei_vlan_ipmf2_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_pph_fhei_vlan_ipmf2_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-3);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-3);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-22);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-22);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_1588_pph_unsupported_ext_ipmf2_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_1588_pph_unsupported_ext_ipmf2_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-21);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-21);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_context_default_itmh_1588_pph_fhei_vlan_ipmf2_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_itmh_1588_pph_fhei_vlan_ipmf2_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-20);

    
    define->data = (dnx_data_field.base_ipmf1.nof_contexts_get(unit)-20);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 124;

    
    define->data = 124;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 123;

    
    define->data = 123;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 121;

    
    define->data = 121;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_oamp_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 122;

    
    define->data = 122;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_nat_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_nat_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 119;

    
    define->data = 119;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 115;

    
    define->data = 115;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 126;

    
    define->data = 126;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 125;

    
    define->data = 125;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 118;

    
    define->data = 118;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 117;

    
    define->data = 117;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 116;

    
    define->data = 116;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usd_eth_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usd_eth_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usd_ipv4_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usd_ipv4_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usd_ipv6_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usd_ipv6_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usd_mpls_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usd_mpls_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 23;

    
    define->data = 23;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usp_trap_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usp_trap_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 26;

    
    define->data = 26;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usp_fwd_trap_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usp_fwd_trap_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 27;

    
    define->data = 27;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usp_icmp_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usp_icmp_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 28;

    
    define->data = 28;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_mpls_bier_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_rcy_mirror_sys_hdr_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_rcy_mirror_sys_hdr_presel_id_ipmf1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 29;

    
    define->data = 29;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_unsupported_ext_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_unsupported_ext_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 126;

    
    define->data = 126;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 125;

    
    define->data = 125;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 124;

    
    define->data = 124;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_trap_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_trap_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 123;

    
    define->data = 123;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_vlan_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_vlan_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 122;

    
    define->data = 122;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 121;

    
    define->data = 121;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 119;

    
    define->data = 119;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 118;

    
    define->data = 118;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 117;

    
    define->data = 117;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 112;

    
    define->data = 112;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_trap_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_trap_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 111;

    
    define->data = 111;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_pph_vlan_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_vlan_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 110;

    
    define->data = 110;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 109;

    
    define->data = 109;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_trap_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_trap_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 108;

    
    define->data = 108;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_vlan_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_1588_pph_vlan_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 107;

    
    define->data = 107;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 117;

    
    define->data = 117;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 106;

    
    define->data = 106;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_oam_roo_ipv4_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 125;

    
    define->data = 125;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_oam_roo_ipv6_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 124;

    
    define->data = 124;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 123;

    
    define->data = 123;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 122;

    
    define->data = 122;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 121;

    
    define->data = 121;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_ipv4_mc_in_lif_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 119;

    
    define->data = 119;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_ipv6_mc_in_lif_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 118;

    
    define->data = 118;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_rch_remove_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 114;

    
    define->data = 114;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_rch_extended_encap_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_rch_extended_encap_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 113;

    
    define->data = 113;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 115;

    
    define->data = 115;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 116;

    
    define->data = 116;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_partial_fwd_layer_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_partial_fwd_layer_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_psp_ext_to_endpoint_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_psp_ext_to_endpoint_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_egress_usd_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_egress_usd_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_mpls_bier_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_ebtr_const_fwd_layer_eth_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_ebtr_const_fwd_layer_eth_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 126;

    
    define->data = 126;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_ext_termination_btr_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_ext_termination_btr_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 125;

    
    define->data = 125;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_learn_limit_presel_id_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_hbh_bta_presel_id_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_hbh_bta_presel_id_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 23;

    
    define->data = 23;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_transit_hbh_bta_presel_id_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_transit_hbh_bta_presel_id_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 26;

    
    define->data = 26;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_bta_presel_id_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_srv6_endpoint_psp_bta_presel_id_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_default_mpls_8b_fhei_presel_id_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_mpls_8b_fhei_presel_id_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_preselector_num_cs_inlif_profile_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_num_cs_inlif_profile_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_qual_user_1st_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_user_1st;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_user_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_user_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_predefined_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_predefined_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500;

    
    define->data = 500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_layer_record_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_layer_record_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_vw_1st_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_vw_1st;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8292;

    
    define->data = 8292;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_vw_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_vw_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_max_bits_in_qual_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_max_bits_in_qual;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_ingress_pbus_header_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ingress_pbus_header_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1152;

    
    define->data = 1152;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_egress_pbus_header_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_egress_pbus_header_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_ifwd2_pbus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ifwd2_pbus_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3161;

    
    define->data = 3161;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_ipmf1_pbus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ipmf1_pbus_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3355;

    
    define->data = 3355;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_ipmf2_pbus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ipmf2_pbus_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1384;

    
    define->data = 1384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_ipmf3_pbus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ipmf3_pbus_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1714;

    
    define->data = 1714;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_epmf_pbus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_epmf_pbus_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2555;

    
    define->data = 2555;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_l4_ops_pbus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_l4_ops_pbus_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_ingress_nof_layer_records_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ingress_nof_layer_records;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_ingress_layer_record_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ingress_layer_record_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_egress_nof_layer_records_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_egress_nof_layer_records;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_egress_layer_record_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_egress_layer_record_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 28;

    
    define->data = 28;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_kbp_extra_offset_after_layer_record_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_kbp_extra_offset_after_layer_record_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_special_metadata_qual_max_parts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_special_metadata_qual_max_parts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_action_user_1st_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_action;
    int define_index = dnx_data_field_action_define_user_1st;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_action_user_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_action;
    int define_index = dnx_data_field_action_define_user_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_action_predefined_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_action;
    int define_index = dnx_data_field_action_define_predefined_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_action_vw_1st_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_action;
    int define_index = dnx_data_field_action_define_vw_1st;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8292;

    
    define->data = 8292;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_action_vw_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_action;
    int define_index = dnx_data_field_action_define_vw_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_virtual_wire;
    int feature_index = dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_virtual_wire_signals_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_virtual_wire;
    int define_index = dnx_data_field_virtual_wire_define_signals_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_virtual_wire_actions_per_signal_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_virtual_wire;
    int define_index = dnx_data_field_virtual_wire_define_actions_per_signal_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_virtual_wire_general_data_user_general_containers_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_virtual_wire;
    int define_index = dnx_data_field_virtual_wire_define_general_data_user_general_containers_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 384;

    
    define->data = 384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_virtual_wire_ipmf1_general_data_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_virtual_wire;
    int define_index = dnx_data_field_virtual_wire_define_ipmf1_general_data_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_virtual_wire_signal_mapping_set(
    int unit)
{
    int stage_index;
    int signal_id_index;
    dnx_data_field_virtual_wire_signal_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_virtual_wire;
    int table_index = dnx_data_field_virtual_wire_table_signal_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_STAGE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_STAGE_NOF;
    table->keys[1].size = dnx_data_field.virtual_wire.signals_nof_get(unit);
    table->info_get.key_size[1] = dnx_data_field.virtual_wire.signals_nof_get(unit);

    
    table->values[0].default_val = "NULL";
    table->values[1].default_val = "DNX_FIELD_QUAL_ID_INVALID";
    table->values[2].default_val = "0";
    table->values[3].default_val = "{0}";
    table->values[4].default_val = "{0}";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_virtual_wire_signal_mapping_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_field_virtual_wire_table_signal_mapping");

    
    default_data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->signal_name = NULL;
    default_data->mapped_qual = DNX_FIELD_QUAL_ID_INVALID;
    default_data->offset_from_qual = 0;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF; i++)  
        default_data->mapped_action[i] = 0;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF; i++)  
        default_data->action_offset[i] = 0;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        for (signal_id_index = 0; signal_id_index < table->keys[1].size; signal_id_index++)
        {
            data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, signal_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 0);
        data->signal_name = "context";
        data->mapped_qual = DNX_FIELD_QUAL_CONTEXT;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 1);
        data->signal_name = "nasid";
        data->mapped_qual = DNX_FIELD_QUAL_NASID;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 2);
        data->signal_name = "saved_context_profile";
        data->mapped_qual = DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 3);
        data->signal_name = "rpf_route_valid";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_ROUTE_VALID;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 4);
        data->signal_name = "in_lif_unknown_da_profile";
        data->mapped_qual = DNX_FIELD_QUAL_IN_LIF_UNKNOWN_DA_PROFILE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 5);
        data->signal_name = "rpf_default_route_found";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 6);
        data->signal_name = "rpf_out_lif";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_OUT_LIF;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 7);
        data->signal_name = "enable_pp_inject";
        data->mapped_qual = DNX_FIELD_QUAL_ENABLE_PP_INJECT;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 8);
        data->signal_name = "oam_lif";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 9);
        data->signal_name = "lif_oam_trap_profile";
        data->mapped_qual = DNX_FIELD_QUAL_LIF_OAM_TRAP_PROFILE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 10);
        data->signal_name = "eei";
        data->mapped_qual = DNX_FIELD_QUAL_EEI;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 11);
        data->signal_name = "egress_learn_enable";
        data->mapped_qual = DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 12);
        data->signal_name = "fwd_action_cpu_trap_code";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 13);
        data->signal_name = "fwd_action_cpu_trap_qual";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 14);
        data->signal_name = "fwd_action_destination";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_DESTINATION;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 15);
        data->signal_name = "fwd_action_strength";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 16);
        data->signal_name = "fwd_domain_id";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_DOMAIN;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 17);
        data->signal_name = "fwd_domain_profile";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 18);
        data->signal_name = "fwd_layer_additional_info";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 19);
        data->signal_name = "layer_offsets";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 20 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 20);
        data->signal_name = "layer_qualifiers";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 21 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 21);
        data->signal_name = "layer_protocols";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 22 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 22);
        data->signal_name = "general_data";
        data->mapped_qual = DNX_FIELD_QUAL_GENERAL_DATA_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 23 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 23);
        data->signal_name = "ieee1588_command";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_COMMAND;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 24 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 24);
        data->signal_name = "ieee1588_compensate_time_stamp";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 25 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 25);
        data->signal_name = "ieee1588_encapsulation";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 26 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 26);
        data->signal_name = "ieee1588_header_offset";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 27 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 27);
        data->signal_name = "in_lif_profiles";
        data->mapped_qual = DNX_FIELD_QUAL_IN_LIF_PROFILE_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 28 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 28);
        data->signal_name = "in_lifs";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_IN_LIF_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 29 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 29);
        data->signal_name = "in_port";
        data->mapped_qual = DNX_FIELD_QUAL_PP_PORT;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 30 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 30);
        data->signal_name = "incoming_tag_structure";
        data->mapped_qual = DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 31 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 31);
        data->signal_name = "ingress_learn_enable";
        data->mapped_qual = DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 32 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 32);
        data->signal_name = "learn_info";
        data->mapped_qual = DNX_FIELD_QUAL_LEARN_INFO;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 33 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 33);
        data->signal_name = "mem_soft_err";
        data->mapped_qual = DNX_FIELD_QUAL_MEM_SOFT_ERR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 34 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 34);
        data->signal_name = "nwk_qos";
        data->mapped_qual = DNX_FIELD_QUAL_NWK_QOS;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 35 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 35);
        data->signal_name = "oam_offset";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_OFFSET;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 36 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 36);
        data->signal_name = "oam_stamp_offset";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_STAMP_OFFSET;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 37 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 37);
        data->signal_name = "oam_opcode";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_OPCODE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 38 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 38);
        data->signal_name = "oam_your_discr";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_YOUR_DISCR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 39 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 39);
        data->signal_name = "my_cfm_mac";
        data->mapped_qual = DNX_FIELD_QUAL_MY_CFM_MAC;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 40 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 40);
        data->signal_name = "packet_is_oam";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_OAM;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 41 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 41);
        data->signal_name = "packet_is_bfd";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_BFD;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 42 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 42);
        data->signal_name = "mdl";
        data->mapped_qual = DNX_FIELD_QUAL_MDL;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 43 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 43);
        data->signal_name = "vtt_oam_lif_0";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_LIF_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 44 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 44);
        data->signal_name = "vtt_oam_lif_0_valid";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 45 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 45);
        data->signal_name = "vtt_oam_lif_1";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_LIF_1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 46 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 46);
        data->signal_name = "vtt_oam_lif_1_valid";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_LIF_1_VALID;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 47 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 47);
        data->signal_name = "vtt_oam_lif_2";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_LIF_2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 48 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 48);
        data->signal_name = "vtt_oam_lif_2_valid";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_LIF_2_VALID;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 49 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 49);
        data->signal_name = "vtt_oam_pcp_0";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_PCP_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 50 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 50);
        data->signal_name = "vtt_oam_pcp_1";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_PCP_1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 51 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 51);
        data->signal_name = "vtt_oam_pcp_2";
        data->mapped_qual = DNX_FIELD_QUAL_VTT_OAM_PCP_2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 52 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 52);
        data->signal_name = "out_lifs";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_OUT_LIF_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 53 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 53);
        data->signal_name = "prt_qualifier";
        data->mapped_qual = DNX_FIELD_QUAL_PRT_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 54 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 54);
        data->signal_name = "packet_header";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 55 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 55);
        data->signal_name = "packet_header_size";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_HEADER_SIZE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 56 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 56);
        data->signal_name = "packet_is_applet";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_APPLET;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 57 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 57);
        data->signal_name = "packet_is_compatible_mc";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 58 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 58);
        data->signal_name = "packet_is_ieee1588";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_IEEE1588;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 59 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 59);
        data->signal_name = "dp";
        data->mapped_qual = DNX_FIELD_QUAL_DP;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 60 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 60);
        data->signal_name = "ecn";
        data->mapped_qual = DNX_FIELD_QUAL_ECN;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 61 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 61);
        data->signal_name = "tc";
        data->mapped_qual = DNX_FIELD_QUAL_TC;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 62 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 62);
        data->signal_name = "ptc";
        data->mapped_qual = DNX_FIELD_QUAL_PTC;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 63 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 63);
        data->signal_name = "rpf_destination";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DST;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 64 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 64);
        data->signal_name = "rpf_destination_valid";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DST_VALID;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 65 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 65);
        data->signal_name = "snoop_code";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_CODE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 66 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 66);
        data->signal_name = "snoop_strength";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 67 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 67);
        data->signal_name = "mirror_code";
        data->mapped_qual = DNX_FIELD_QUAL_MIRROR_CODE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 68 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 68);
        data->signal_name = "statistical_sampling_code";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 69 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 69);
        data->signal_name = "src_system_port_id";
        data->mapped_qual = DNX_FIELD_QUAL_SRC_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 70 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 70);
        data->signal_name = "flp_statistics_objects";
        data->mapped_qual = DNX_FIELD_QUAL_FLP_STATISTICS_ID_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 71 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 71);
        data->signal_name = "flp_statistics_objects_cmd";
        data->mapped_qual = DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 72 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 72);
        data->signal_name = "vlan_edit_cmd";
        data->mapped_qual = DNX_FIELD_QUAL_VLAN_EDIT_CMD;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 73 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 73);
        data->signal_name = "trace_packet";
        data->mapped_qual = DNX_FIELD_QUAL_TRACE_PACKET;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 74 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 74);
        data->signal_name = "fwd_layer_index";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_LAYER_INDEX;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 75 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 75);
        data->signal_name = "in_ttl";
        data->mapped_qual = DNX_FIELD_QUAL_IN_TTL;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 76 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 76);
        data->signal_name = "flpa_priority_decoder_fwd_strength";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 77 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 77);
        data->signal_name = "flpa_priority_decoder_rpf_strength";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 78 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 78);
        data->signal_name = "service_type";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 79 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 79);
        data->signal_name = "oam_context_selection_profile";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 80 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 80);
        data->signal_name = "port_profile";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 81 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 81);
        data->signal_name = "ptc_profile";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 82 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 82);
        data->signal_name = "pem_elk_acl_cs_var";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 83 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 83);
        data->signal_name = "statistics_meta_data";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICS_META_DATA;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 84 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 84);
        data->signal_name = "vsi_unknown_da_destination";
        data->mapped_qual = DNX_FIELD_QUAL_VSI_UNKNOWN_DA_DESTINATION;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 85 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 85);
        data->signal_name = "ecmp_lb_keys";
        data->mapped_qual = DNX_FIELD_QUAL_ECMP_LB_KEY_0;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 86 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 86);
        data->signal_name = "lag_lb_key";
        data->mapped_qual = DNX_FIELD_QUAL_LAG_LB_KEY;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 87 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 87);
        data->signal_name = "nwk_lb_key";
        data->mapped_qual = DNX_FIELD_QUAL_NWK_LB_KEY;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 88 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 88);
        data->signal_name = "flpa_priority_decoder_fwd_found";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 89 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 89);
        data->signal_name = "flpa_priority_decoder_rpf_found";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 90 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 90);
        data->signal_name = "flpa_priority_decoder_fwd_profile";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 91 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 91);
        data->signal_name = "flpa_priority_decoder_fwd_result";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 92 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 92);
        data->signal_name = "flpa_priority_decoder_rpf_profile";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 93 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 93);
        data->signal_name = "flpa_priority_decoder_rpf_result";
        data->mapped_qual = DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 94 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 94);
        data->signal_name = "visibility_clear";
        data->mapped_qual = DNX_FIELD_QUAL_VISIBILITY_CLEAR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 95 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 95);
        data->signal_name = "visibility";
        data->mapped_qual = DNX_FIELD_QUAL_VISIBILITY;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 96 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 96);
        data->signal_name = "action_profile_sa_drop_index";
        data->mapped_qual = DNX_FIELD_QUAL_ACTION_PROFILE_SA_DROP_INDEX;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 97 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 97);
        data->signal_name = "action_profile_sa_not_found_index";
        data->mapped_qual = DNX_FIELD_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 98 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 98);
        data->signal_name = "action_profile_da_not_found_index";
        data->mapped_qual = DNX_FIELD_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 99 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 99);
        data->signal_name = "ipv4_version_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_VERSION_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 100 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 100);
        data->signal_name = "ipv4_checksum_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_CHECKSUM_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 101 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 101);
        data->signal_name = "ipv4_total_length_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_TOTAL_LENGTH_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 102 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 102);
        data->signal_name = "ipv4_options_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_OPTIONS_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 103 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 103);
        data->signal_name = "ipv4_sip_equal_dip_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_SIP_EQUAL_DIP_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 104 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 104);
        data->signal_name = "ipv4_dip_zero_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_DIP_ZERO_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 105 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 105);
        data->signal_name = "ipv4_sip_is_mc_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_SIP_IS_MC_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 106 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 106);
        data->signal_name = "ipv4_header_length_error";
        data->mapped_qual = DNX_FIELD_QUAL_IPV4_HEADER_LENGTH_ERROR;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 107 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 107);
        data->signal_name = "cs_inlif_profile0";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 108 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 108);
        data->signal_name = "cs_inlif_profile1";
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 109 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 109);
        data->signal_name = "fwd_stat_attribute_obj_cmd";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 110 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 110);
        data->signal_name = "fwd_stat_attribute_obj";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 111 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 111);
        data->signal_name = "mp_profile_sel";
        data->mapped_qual = DNX_FIELD_QUAL_MP_PROFILE_SEL;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 112 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 112);
        data->signal_name = "acl_context";
        data->mapped_qual = DNX_FIELD_QUAL_ACL_CONTEXT;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 113 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 113);
        data->signal_name = "nof_valid_lm_lifs";
        data->mapped_qual = DNX_FIELD_QUAL_NOF_VALID_LM_LIFS;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && 114 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 114);
        data->signal_name = "is_in_lif_valid_for_lm";
        data->mapped_qual = DNX_FIELD_QUAL_IS_IN_LIF_VALID_FOR_LM;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 0);
        data->signal_name = "mem_soft_err";
        data->mapped_qual = DNX_FIELD_QUAL_MEM_SOFT_ERR;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 1);
        data->signal_name = "context";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_CONTEXT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 2);
        data->signal_name = "acl_context";
        data->mapped_qual = DNX_FIELD_QUAL_ACL_CONTEXT;
        data->mapped_action[0] = DNX_FIELD_ACTION_ACL_CONTEXT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 3);
        data->signal_name = "packet_is_applet";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_APPLET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 4);
        data->signal_name = "general_data";
        data->mapped_qual = DNX_FIELD_QUAL_GENERAL_DATA_0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_GENERAL_DATA0;
        data->mapped_action[1] = DNX_FIELD_ACTION_GENERAL_DATA1;
        data->mapped_action[2] = DNX_FIELD_ACTION_GENERAL_DATA2;
        data->mapped_action[3] = DNX_FIELD_ACTION_GENERAL_DATA3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 160;
        data->action_offset[1] = 192;
        data->action_offset[2] = 256;
        data->action_offset[3] = 296;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 5);
        data->signal_name = "fwd_domain_id";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_DOMAIN;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 6);
        data->signal_name = "fwd_domain_profile";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 7);
        data->signal_name = "vlan_edit_cmd";
        data->mapped_qual = DNX_FIELD_QUAL_VLAN_EDIT_CMD;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_VLAN_EDIT_CMD_INDEX;
        data->mapped_action[1] = DNX_FIELD_ACTION_VLAN_EDIT_VID_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_1;
        data->mapped_action[3] = DNX_FIELD_ACTION_VLAN_EDIT_VID_2;
        data->mapped_action[4] = DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_2;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 7;
        data->action_offset[2] = 19;
        data->action_offset[3] = 23;
        data->action_offset[4] = 35;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 8);
        data->signal_name = "src_system_port_id";
        data->mapped_qual = DNX_FIELD_QUAL_SRC_SYS_PORT;
        data->mapped_action[0] = DNX_FIELD_ACTION_SRC_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 9);
        data->signal_name = "snoop_code";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 10);
        data->signal_name = "snoop_strength";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 11);
        data->signal_name = "fwd_layer_additional_info";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO;
        data->mapped_action[0] = DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 12);
        data->signal_name = "rpf_destination";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DST;
        data->mapped_action[0] = DNX_FIELD_ACTION_RPF_DST;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 13);
        data->signal_name = "rpf_destination_valid";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DST_VALID;
        data->mapped_action[0] = DNX_FIELD_ACTION_RPF_DST_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 14);
        data->signal_name = "packet_is_ieee1588";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_IEEE1588;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 15);
        data->signal_name = "ieee1588_compensate_time_stamp";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 16);
        data->signal_name = "ieee1588_command";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_COMMAND;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 17);
        data->signal_name = "ieee1588_encapsulation";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 18);
        data->signal_name = "ieee1588_header_offset";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 19);
        data->signal_name = "packet_is_compatible_mc";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC;
        data->mapped_action[0] = DNX_FIELD_ACTION_PACKET_IS_COMPATIBLE_MC;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 20 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 20);
        data->signal_name = "prt_qualifier";
        data->mapped_qual = DNX_FIELD_QUAL_PRT_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 21 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 21);
        data->signal_name = "packet_header";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 22 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 22);
        data->signal_name = "packet_header_size";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_HEADER_SIZE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 23 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 23);
        data->signal_name = "oam_id";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_ID;
        data->mapped_action[0] = DNX_FIELD_ACTION_PMF_OAM_ID;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 24 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 24);
        data->signal_name = "oam_up_mep";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_UP_MEP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 25 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 25);
        data->signal_name = "oam_sub_type";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_SUB_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 26 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 26);
        data->signal_name = "oam_offset";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 27 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 27);
        data->signal_name = "oam_stamp_offset";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_STAMP_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 28 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 28);
        data->signal_name = "mirror_code";
        data->mapped_qual = DNX_FIELD_QUAL_MIRROR_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 29 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 29);
        data->signal_name = "learn_info";
        data->mapped_qual = DNX_FIELD_QUAL_LEARN_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 30 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 30);
        data->signal_name = "ingress_learn_enable";
        data->mapped_qual = DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DNX_FIELD_ACTION_INGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 31 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 31);
        data->signal_name = "egress_learn_enable";
        data->mapped_qual = DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DNX_FIELD_ACTION_EGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 32 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 32);
        data->signal_name = "in_lifs";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_IN_LIF_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_GLOB_IN_LIF_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_GLOB_IN_LIF_1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 33 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 33);
        data->signal_name = "in_lif_profiles";
        data->mapped_qual = DNX_FIELD_QUAL_IN_LIF_PROFILE_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_IN_LIF_PROFILE_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_IN_LIF_PROFILE_1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 34 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 34);
        data->signal_name = "layer_offsets";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 35 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 35);
        data->signal_name = "layer_qualifiers";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 36 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 36);
        data->signal_name = "layer_protocols";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 37 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 37);
        data->signal_name = "fwd_action_strength";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 38 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 38);
        data->signal_name = "fwd_action_destination";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_DESTINATION;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 39 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 39);
        data->signal_name = "fwd_action_cpu_trap_code";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 40 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 40);
        data->signal_name = "fwd_action_cpu_trap_qual";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 41 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 41);
        data->signal_name = "eei";
        data->mapped_qual = DNX_FIELD_QUAL_EEI;
        data->mapped_action[0] = DNX_FIELD_ACTION_EEI;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 42 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 42);
        data->signal_name = "elk_lkp_payload";
        data->mapped_qual = DNX_FIELD_QUAL_ELK_LKP_HIT_0;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 43 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 43);
        data->signal_name = "in_port";
        data->mapped_qual = DNX_FIELD_QUAL_PP_PORT;
        data->mapped_action[0] = DNX_FIELD_ACTION_PP_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 44 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 44);
        data->signal_name = "ptc";
        data->mapped_qual = DNX_FIELD_QUAL_PTC;
        data->mapped_action[0] = DNX_FIELD_ACTION_PTC;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 45 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 45);
        data->signal_name = "statistics_objects";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 46 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 46);
        data->signal_name = "statistics_objects_cmd";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 47 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 47);
        data->signal_name = "out_lifs";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_OUT_LIF_0;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 48 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 48);
        data->signal_name = "in_ttl";
        data->mapped_qual = DNX_FIELD_QUAL_IN_TTL;
        data->mapped_action[0] = DNX_FIELD_ACTION_IN_TTL;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 49 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 49);
        data->signal_name = "fwd_layer_index";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_LAYER_INDEX;
        data->mapped_action[0] = DNX_FIELD_ACTION_FWD_LAYER_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 50 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 50);
        data->signal_name = "saved_context_profile";
        data->mapped_qual = DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 51 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 51);
        data->signal_name = "visibility";
        data->mapped_qual = DNX_FIELD_QUAL_VISIBILITY;
        data->mapped_action[0] = DNX_FIELD_ACTION_VISIBILITY;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 52 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 52);
        data->signal_name = "visibility_clear";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 53 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 53);
        data->signal_name = "statistical_sampling_code";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 54 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 54);
        data->signal_name = "stat_obj_lm_read_index";
        data->mapped_action[0] = DNX_FIELD_ACTION_STAT_OBJ_LM_READ_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 55 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 55);
        data->signal_name = "rpf_out_lif";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_OUT_LIF;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 56 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 56);
        data->signal_name = "rpf_default_route_found";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 57 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 57);
        data->signal_name = "rpf_route_valid";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_ROUTE_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 58 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 58);
        data->signal_name = "ecmp_lb_keys";
        data->mapped_qual = DNX_FIELD_QUAL_ECMP_LB_KEY_0;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 59 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 59);
        data->signal_name = "lag_lb_key";
        data->mapped_qual = DNX_FIELD_QUAL_LAG_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 60 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 60);
        data->signal_name = "nwk_lb_key";
        data->mapped_qual = DNX_FIELD_QUAL_NWK_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 61 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 61);
        data->signal_name = "nasid";
        data->mapped_qual = DNX_FIELD_QUAL_NASID;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 62 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 62);
        data->signal_name = "oam_statistics_obj";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 63 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 63);
        data->signal_name = "oam_statistics_obj_cmd";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 64 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 64);
        data->signal_name = "statistics_meta_data";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICS_META_DATA;
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_META_DATA;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 65 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 65);
        data->signal_name = "oam_meter_disable";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_METER_DISABLE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 66 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 66);
        data->signal_name = "incoming_tag_structure";
        data->mapped_qual = DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 67 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 67);
        data->signal_name = "tc";
        data->mapped_qual = DNX_FIELD_QUAL_TC;
        data->mapped_action[0] = DNX_FIELD_ACTION_TC;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 68 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 68);
        data->signal_name = "dp";
        data->mapped_qual = DNX_FIELD_QUAL_DP;
        data->mapped_action[0] = DNX_FIELD_ACTION_DP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 69 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 69);
        data->signal_name = "ecn";
        data->mapped_qual = DNX_FIELD_QUAL_ECN;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 70 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 70);
        data->signal_name = "nwk_qos";
        data->mapped_qual = DNX_FIELD_QUAL_NWK_QOS;
        data->mapped_action[0] = DNX_FIELD_ACTION_NWK_QOS;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 71 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 71);
        data->signal_name = "trace_packet";
        data->mapped_qual = DNX_FIELD_QUAL_TRACE_PACKET;
        data->mapped_action[0] = DNX_FIELD_ACTION_TRACE_PACKET_ACT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 72 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 72);
        data->signal_name = "dp_meter_command";
        data->mapped_action[0] = DNX_FIELD_ACTION_DP_METER_COMMAND;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 73 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 73);
        data->signal_name = "mirror_qualifier";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 74 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 74);
        data->signal_name = "system_header_profile_index";
        data->mapped_action[0] = DNX_FIELD_ACTION_SYSTEM_HEADER_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 75 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 75);
        data->signal_name = "all_out_lifs";
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_GLOB_OUT_LIF_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_GLOB_OUT_LIF_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_GLOB_OUT_LIF_2;
        data->mapped_action[3] = DNX_FIELD_ACTION_GLOB_OUT_LIF_3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 22;
        data->action_offset[2] = 44;
        data->action_offset[3] = 66;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 76 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 76);
        data->signal_name = "st_vsq_pointer";
        data->mapped_action[0] = DNX_FIELD_ACTION_ST_VSQ_PTR;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 77 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 77);
        data->signal_name = "stacking_route_history_bitmap";
        data->mapped_action[0] = DNX_FIELD_ACTION_STACKING_ROUTE_HISTORY_BITMAP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 78 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 78);
        data->signal_name = "pph_type";
        data->mapped_action[0] = DNX_FIELD_ACTION_PPH_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 79 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 79);
        data->signal_name = "user_header1";
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 80 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 80);
        data->signal_name = "user_header2";
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 81 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 81);
        data->signal_name = "user_header3";
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 82 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 82);
        data->signal_name = "user_header4";
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 83 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 83);
        data->signal_name = "user_headers_type";
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_1_TYPE;
        data->mapped_action[1] = DNX_FIELD_ACTION_USER_HEADER_2_TYPE;
        data->mapped_action[2] = DNX_FIELD_ACTION_USER_HEADER_3_TYPE;
        data->mapped_action[3] = DNX_FIELD_ACTION_USER_HEADER_4_TYPE;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 2;
        data->action_offset[2] = 4;
        data->action_offset[3] = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 84 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 84);
        data->signal_name = "itpp_delta";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 85 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 85);
        data->signal_name = "latency_flow_id";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 86 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 86);
        data->signal_name = "latency_flow_profile";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 87 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 87);
        data->signal_name = "admt_profile";
        data->mapped_action[0] = DNX_FIELD_ACTION_ADMT_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 88 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 88);
        data->signal_name = "pph_reserved";
        data->mapped_action[0] = DNX_FIELD_ACTION_PPH_RESERVED;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 89 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 89);
        data->signal_name = "bier_string_size";
        data->mapped_qual = DNX_FIELD_QUAL_BIER_STR_SIZE;
        data->mapped_action[0] = DNX_FIELD_ACTION_BIER_STR_SIZE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 90 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 90);
        data->signal_name = "bier_string_offset";
        data->mapped_qual = DNX_FIELD_QUAL_BIER_STR_OFFSET;
        data->mapped_action[0] = DNX_FIELD_ACTION_BIER_STR_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 91 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 91);
        data->signal_name = "egress_parsing_index";
        data->mapped_qual = DNX_FIELD_QUAL_EGRESS_PARSING_INDEX;
        data->mapped_action[0] = DNX_FIELD_ACTION_EGRESS_PARSING_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 92 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 92);
        data->signal_name = "congestion_info";
        data->mapped_action[0] = DNX_FIELD_ACTION_CONGESTION_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 93 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 93);
        data->signal_name = "statistics_objects_resolved";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICS_ID_0;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_ID_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_STATISTICS_ID_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_STATISTICS_ID_2;
        data->mapped_action[3] = DNX_FIELD_ACTION_STATISTICS_ID_3;
        data->mapped_action[4] = DNX_FIELD_ACTION_STATISTICS_ID_4;
        data->mapped_action[5] = DNX_FIELD_ACTION_STATISTICS_ID_5;
        data->mapped_action[6] = DNX_FIELD_ACTION_STATISTICS_ID_6;
        data->mapped_action[7] = DNX_FIELD_ACTION_STATISTICS_ID_7;
        data->mapped_action[8] = DNX_FIELD_ACTION_STATISTICS_ID_8;
        data->mapped_action[9] = DNX_FIELD_ACTION_STATISTICS_ID_9;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 20;
        data->action_offset[2] = 40;
        data->action_offset[3] = 60;
        data->action_offset[4] = 80;
        data->action_offset[5] = 100;
        data->action_offset[6] = 120;
        data->action_offset[7] = 140;
        data->action_offset[8] = 160;
        data->action_offset[9] = 180;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 94 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 94);
        data->signal_name = "statistics_object10";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICS_OBJECT10;
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_OBJECT_10;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 95 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 95);
        data->signal_name = "statistics_objects_atr";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICS_ATR_0;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_ATR_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_STATISTICS_ATR_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_STATISTICS_ATR_2;
        data->mapped_action[3] = DNX_FIELD_ACTION_STATISTICS_ATR_3;
        data->mapped_action[4] = DNX_FIELD_ACTION_STATISTICS_ATR_4;
        data->mapped_action[5] = DNX_FIELD_ACTION_STATISTICS_ATR_5;
        data->mapped_action[6] = DNX_FIELD_ACTION_STATISTICS_ATR_6;
        data->mapped_action[7] = DNX_FIELD_ACTION_STATISTICS_ATR_7;
        data->mapped_action[8] = DNX_FIELD_ACTION_STATISTICS_ATR_8;
        data->mapped_action[9] = DNX_FIELD_ACTION_STATISTICS_ATR_9;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 5;
        data->action_offset[2] = 10;
        data->action_offset[3] = 15;
        data->action_offset[4] = 20;
        data->action_offset[5] = 25;
        data->action_offset[6] = 30;
        data->action_offset[7] = 35;
        data->action_offset[8] = 40;
        data->action_offset[9] = 45;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 96 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 96);
        data->signal_name = "tm_profile";
        data->mapped_action[0] = DNX_FIELD_ACTION_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 97 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 97);
        data->signal_name = "fwd_context";
        data->mapped_action[0] = DNX_FIELD_ACTION_FWD_CONTEXT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 98 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 98);
        data->signal_name = "slb_payload";
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_SLB_PAYLOAD0;
        data->mapped_action[1] = DNX_FIELD_ACTION_SLB_PAYLOAD1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 99 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 99);
        data->signal_name = "slb_found";
        data->mapped_action[0] = DNX_FIELD_ACTION_SLB_FOUND;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 100 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 100);
        data->signal_name = "end_of_packet_editing";
        data->mapped_action[0] = DNX_FIELD_ACTION_END_OF_PACKET_EDITING;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 101 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 101);
        data->signal_name = "ext_statistics";
        data->mapped_qual = DNX_FIELD_QUAL_EXT_STATISTICS_ID_0;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 102 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 102);
        data->signal_name = "ext_statistics_opcode";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 103 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 103);
        data->signal_name = "slb_key";
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_SLB_KEY0;
        data->mapped_action[1] = DNX_FIELD_ACTION_SLB_KEY1;
        data->mapped_action[2] = DNX_FIELD_ACTION_SLB_KEY2;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 32;
        data->action_offset[2] = 64;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 104 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 104);
        data->signal_name = "snoop_qualifier";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 105 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 105);
        data->signal_name = "statistical_sampling_qualifier";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 106 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 106);
        data->signal_name = "latency_flow_id_valid";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 107 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 107);
        data->signal_name = "elephant_trap_info";
        data->mapped_action[0] = DNX_FIELD_ACTION_ELEPHANT_PAYLOAD;
        data->action_offset[0] = 64;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 108 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 108);
        data->signal_name = "elephant_learn_needed";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 109 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 109);
        data->signal_name = "packet_is_bier";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_BIER;
        data->mapped_action[0] = DNX_FIELD_ACTION_PACKET_IS_BIER;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 110 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 110);
        data->signal_name = "ingress_time_stamp";
        data->mapped_action[0] = DNX_FIELD_ACTION_INGRESS_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 111 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 111);
        data->signal_name = "ingress_time_stamp_override";
        data->mapped_action[0] = DNX_FIELD_ACTION_INGRESS_TIME_STAMP_OVERRIDE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 112 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 112);
        data->signal_name = "ext_statistics_valid";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 113 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 113);
        data->signal_name = "next_nasid";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 114 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 114);
        data->signal_name = "next_context";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 115 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 115);
        data->signal_name = "csb_key";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 116 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 116);
        data->signal_name = "oam_time_of_day";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 117 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 117);
        data->signal_name = "ntp_time_of_day";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 118 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 118);
        data->signal_name = "csa_key";
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 119 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 119);
        data->signal_name = "packet_size";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 0);
        data->signal_name = "acl_context";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 1);
        data->signal_name = "admt_profile";
        data->mapped_qual = DNX_FIELD_QUAL_ADMT_PROFILE;
        data->mapped_action[0] = DNX_FIELD_ACTION_ADMT_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 2);
        data->signal_name = "bier_string_offset";
        data->mapped_qual = DNX_FIELD_QUAL_BIER_STR_OFFSET;
        data->mapped_action[0] = DNX_FIELD_ACTION_BIER_STR_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 3);
        data->signal_name = "bier_string_size";
        data->mapped_qual = DNX_FIELD_QUAL_BIER_STR_SIZE;
        data->mapped_action[0] = DNX_FIELD_ACTION_BIER_STR_SIZE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 4);
        data->signal_name = "congestion_info";
        data->mapped_qual = DNX_FIELD_QUAL_CONGESTION_INFO;
        data->mapped_action[0] = DNX_FIELD_ACTION_CONGESTION_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 5);
        data->signal_name = "eei";
        data->mapped_qual = DNX_FIELD_QUAL_EEI;
        data->mapped_action[0] = DNX_FIELD_ACTION_EEI;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 6);
        data->signal_name = "egress_learn_enable";
        data->mapped_qual = DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DNX_FIELD_ACTION_EGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 7);
        data->signal_name = "end_of_packet_editing";
        data->mapped_qual = DNX_FIELD_QUAL_END_OF_PACKET_EDITING;
        data->mapped_action[0] = DNX_FIELD_ACTION_END_OF_PACKET_EDITING;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 8);
        data->signal_name = "ext_statistics_valid";
        data->mapped_qual = DNX_FIELD_QUAL_EXT_STATISTICS_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 9);
        data->signal_name = "ext_statistics";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 10);
        data->signal_name = "ext_statistics_opcode";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 11);
        data->signal_name = "fwd_action_destination";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_DESTINATION;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 12);
        data->signal_name = "dp";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_DP;
        data->mapped_action[0] = DNX_FIELD_ACTION_DP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 13);
        data->signal_name = "tc";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_TC;
        data->mapped_action[0] = DNX_FIELD_ACTION_TC;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 14);
        data->signal_name = "dp_meter_command";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_DP_METER_COMMAND;
        data->mapped_action[0] = DNX_FIELD_ACTION_DP_METER_COMMAND;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 15);
        data->signal_name = "fwd_action_strength";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 16);
        data->signal_name = "fwd_action_cpu_trap_code";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 17);
        data->signal_name = "fwd_domain_id";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_DOMAIN;
        data->mapped_action[0] = DNX_FIELD_ACTION_FWD_DOMAIN;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 18);
        data->signal_name = "fwd_domain_profile";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 19);
        data->signal_name = "fwd_layer_index";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_LAYER_INDEX;
        data->mapped_action[0] = DNX_FIELD_ACTION_FWD_LAYER_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 20 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 20);
        data->signal_name = "general_data";
        data->mapped_qual = DNX_FIELD_QUAL_GENERAL_DATA;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_GENERAL_DATA0;
        data->mapped_action[1] = DNX_FIELD_ACTION_GENERAL_DATA1;
        data->mapped_action[2] = DNX_FIELD_ACTION_GENERAL_DATA2;
        data->mapped_action[3] = DNX_FIELD_ACTION_GENERAL_DATA3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 160;
        data->action_offset[1] = 192;
        data->action_offset[2] = 256;
        data->action_offset[3] = 296;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 21 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 21);
        data->signal_name = "ieee1588_command";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_COMMAND;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 22 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 22);
        data->signal_name = "ieee1588_compensate_time_stamp";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 23 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 23);
        data->signal_name = "ieee1588_encapsulation";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 24 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 24);
        data->signal_name = "ieee1588_header_offset";
        data->mapped_qual = DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 25 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 25);
        data->signal_name = "in_lif_profiles";
        data->mapped_qual = DNX_FIELD_QUAL_IN_LIF_PROFILE_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_IN_LIF_PROFILE_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_IN_LIF_PROFILE_1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 26 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 26);
        data->signal_name = "in_lifs";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_IN_LIF_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_GLOB_IN_LIF_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_GLOB_IN_LIF_1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 27 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 27);
        data->signal_name = "in_port";
        data->mapped_qual = DNX_FIELD_QUAL_PP_PORT;
        data->mapped_action[0] = DNX_FIELD_ACTION_PP_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 28 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 28);
        data->signal_name = "in_ttl";
        data->mapped_qual = DNX_FIELD_QUAL_IN_TTL;
        data->mapped_action[0] = DNX_FIELD_ACTION_IN_TTL;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 29 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 29);
        data->signal_name = "ingress_learn_enable";
        data->mapped_qual = DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DNX_FIELD_ACTION_INGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 30 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 30);
        data->signal_name = "itpp_delta";
        data->mapped_qual = DNX_FIELD_QUAL_ITPP_DELTA;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 31 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 31);
        data->signal_name = "lag_lb_key";
        data->mapped_qual = DNX_FIELD_QUAL_LAG_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 32 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 32);
        data->signal_name = "latency_flow_id";
        data->mapped_qual = DNX_FIELD_QUAL_LATENCY_FLOW_ID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 33 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 33);
        data->signal_name = "latency_flow_profile";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 34 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 34);
        data->signal_name = "layer_offsets";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 35 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 35);
        data->signal_name = "layer_protocols";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 36 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 36);
        data->signal_name = "layer_qualifiers";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 37 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 37);
        data->signal_name = "mem_soft_err";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 38 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 38);
        data->signal_name = "mirror_code";
        data->mapped_qual = DNX_FIELD_QUAL_MIRROR_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 39 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 39);
        data->signal_name = "mirror_qualifier";
        data->mapped_qual = DNX_FIELD_QUAL_MIRROR_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 40 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 40);
        data->signal_name = "nwk_lb_key";
        data->mapped_qual = DNX_FIELD_QUAL_NWK_LB_KEY;
        data->mapped_action[0] = DNX_FIELD_ACTION_NWK_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 41 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 41);
        data->signal_name = "nwk_qos";
        data->mapped_qual = DNX_FIELD_QUAL_NWK_QOS;
        data->mapped_action[0] = DNX_FIELD_ACTION_NWK_QOS;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 42 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 42);
        data->signal_name = "oam_id";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_ID;
        data->mapped_action[0] = DNX_FIELD_ACTION_PMF_OAM_ID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 43 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 43);
        data->signal_name = "oam_offset";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 44 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 44);
        data->signal_name = "oam_stamp_offset";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_STAMP_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 45 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 45);
        data->signal_name = "oam_sub_type";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_SUB_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 46 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 46);
        data->signal_name = "oam_up_mep";
        data->mapped_qual = DNX_FIELD_QUAL_OAM_UP_MEP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 47 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 47);
        data->signal_name = "all_out_lifs";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_OUT_LIF_0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_GLOB_OUT_LIF_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_GLOB_OUT_LIF_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_GLOB_OUT_LIF_2;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 22;
        data->action_offset[2] = 44;
        data->action_offset[3] = 66;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 48 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 48);
        data->signal_name = "packet_is_applet";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_APPLET;
        data->mapped_action[0] = DNX_FIELD_ACTION_IS_APPLET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 49 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 49);
        data->signal_name = "packet_is_bier";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_BIER;
        data->mapped_action[0] = DNX_FIELD_ACTION_PACKET_IS_BIER;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 50 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 50);
        data->signal_name = "packet_is_compatible_mc";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 51 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 51);
        data->signal_name = "packet_is_ieee1588";
        data->mapped_qual = DNX_FIELD_QUAL_PACKET_IS_IEEE1588;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 52 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 52);
        data->signal_name = "egress_parsing_index";
        data->mapped_qual = DNX_FIELD_QUAL_EGRESS_PARSING_INDEX;
        data->mapped_action[0] = DNX_FIELD_ACTION_EGRESS_PARSING_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 53 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 53);
        data->signal_name = "pph_reserved";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_RESERVED;
        data->mapped_action[0] = DNX_FIELD_ACTION_PPH_RESERVED;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 54 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 54);
        data->signal_name = "pph_type";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_TYPE;
        data->mapped_action[0] = DNX_FIELD_ACTION_PPH_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 55 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 55);
        data->signal_name = "rpf_destination";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DST;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 56 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 56);
        data->signal_name = "rpf_destination_valid";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_DST_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 57 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 57);
        data->signal_name = "snoop_code";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 58 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 58);
        data->signal_name = "snoop_strength";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 59 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 59);
        data->signal_name = "snoop_qualifier";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 60 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 60);
        data->signal_name = "src_system_port_id";
        data->mapped_qual = DNX_FIELD_QUAL_SRC_SYS_PORT;
        data->mapped_action[0] = DNX_FIELD_ACTION_SRC_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 61 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 61);
        data->signal_name = "st_vsq_pointer";
        data->mapped_qual = DNX_FIELD_QUAL_ST_VSQ_PTR;
        data->mapped_action[0] = DNX_FIELD_ACTION_ST_VSQ_PTR;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 62 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 62);
        data->signal_name = "statistical_sampling_code";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 63 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 63);
        data->signal_name = "statistical_sampling_qualifier";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 64 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 64);
        data->signal_name = "stacking_route_history_bitmap";
        data->mapped_qual = DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP;
        data->mapped_action[0] = DNX_FIELD_ACTION_STACKING_ROUTE_HISTORY_BITMAP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 65 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 65);
        data->signal_name = "statistics_meta_data";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICS_META_DATA;
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_META_DATA;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 66 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 66);
        data->signal_name = "statistics_objects_resolved";
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_ID_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_STATISTICS_ID_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_STATISTICS_ID_2;
        data->mapped_action[3] = DNX_FIELD_ACTION_STATISTICS_ID_3;
        data->mapped_action[4] = DNX_FIELD_ACTION_STATISTICS_ID_4;
        data->mapped_action[5] = DNX_FIELD_ACTION_STATISTICS_ID_5;
        data->mapped_action[6] = DNX_FIELD_ACTION_STATISTICS_ID_6;
        data->mapped_action[7] = DNX_FIELD_ACTION_STATISTICS_ID_7;
        data->mapped_action[8] = DNX_FIELD_ACTION_STATISTICS_ID_8;
        data->mapped_action[9] = DNX_FIELD_ACTION_STATISTICS_ID_9;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 20;
        data->action_offset[2] = 40;
        data->action_offset[3] = 60;
        data->action_offset[4] = 80;
        data->action_offset[5] = 100;
        data->action_offset[6] = 120;
        data->action_offset[7] = 140;
        data->action_offset[8] = 160;
        data->action_offset[9] = 180;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 67 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 67);
        data->signal_name = "statistics_object10";
        data->mapped_qual = DNX_FIELD_QUAL_STATISTICS_OBJECT10;
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_OBJECT_10;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 68 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 68);
        data->signal_name = "statistics_objects_atr";
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_STATISTICS_ATR_0;
        data->mapped_action[1] = DNX_FIELD_ACTION_STATISTICS_ATR_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_STATISTICS_ATR_2;
        data->mapped_action[3] = DNX_FIELD_ACTION_STATISTICS_ATR_3;
        data->mapped_action[4] = DNX_FIELD_ACTION_STATISTICS_ATR_4;
        data->mapped_action[5] = DNX_FIELD_ACTION_STATISTICS_ATR_5;
        data->mapped_action[6] = DNX_FIELD_ACTION_STATISTICS_ATR_6;
        data->mapped_action[7] = DNX_FIELD_ACTION_STATISTICS_ATR_7;
        data->mapped_action[8] = DNX_FIELD_ACTION_STATISTICS_ATR_8;
        data->mapped_action[9] = DNX_FIELD_ACTION_STATISTICS_ATR_9;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 5;
        data->action_offset[2] = 10;
        data->action_offset[3] = 15;
        data->action_offset[4] = 20;
        data->action_offset[5] = 25;
        data->action_offset[6] = 30;
        data->action_offset[7] = 35;
        data->action_offset[8] = 40;
        data->action_offset[9] = 45;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 69 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 69);
        data->signal_name = "system_header_profile_index";
        data->mapped_qual = DNX_FIELD_QUAL_SYSTEM_HEADER_PROFILE_INDEX;
        data->mapped_action[0] = DNX_FIELD_ACTION_SYSTEM_HEADER_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 70 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 70);
        data->signal_name = "tm_profile";
        data->mapped_qual = DNX_FIELD_QUAL_TM_PROFILE;
        data->mapped_action[0] = DNX_FIELD_ACTION_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 71 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 71);
        data->signal_name = "user_header1";
        data->mapped_qual = DNX_FIELD_QUAL_USER_HEADER_1;
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 72 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 72);
        data->signal_name = "user_header2";
        data->mapped_qual = DNX_FIELD_QUAL_USER_HEADER_2;
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 73 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 73);
        data->signal_name = "user_header3";
        data->mapped_qual = DNX_FIELD_QUAL_USER_HEADER_3;
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 74 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 74);
        data->signal_name = "user_header4";
        data->mapped_qual = DNX_FIELD_QUAL_USER_HEADER_4;
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADER_4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 75 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 75);
        data->signal_name = "user_headers_type";
        data->mapped_qual = DNX_FIELD_QUAL_USER_HEADERS_TYPE;
        data->mapped_action[0] = DNX_FIELD_ACTION_USER_HEADERS_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 76 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 76);
        data->signal_name = "visibility";
        data->mapped_qual = DNX_FIELD_QUAL_VISIBILITY;
        data->mapped_action[0] = DNX_FIELD_ACTION_VISIBILITY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 77 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 77);
        data->signal_name = "vlan_edit_cmd";
        data->mapped_qual = DNX_FIELD_QUAL_VLAN_EDIT_CMD;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DNX_FIELD_ACTION_VLAN_EDIT_CMD_INDEX;
        data->mapped_action[1] = DNX_FIELD_ACTION_VLAN_EDIT_VID_1;
        data->mapped_action[2] = DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_1;
        data->mapped_action[3] = DNX_FIELD_ACTION_VLAN_EDIT_VID_2;
        data->mapped_action[4] = DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_2;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 7;
        data->action_offset[2] = 19;
        data->action_offset[3] = 23;
        data->action_offset[4] = 35;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 78 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 78);
        data->signal_name = "fwd_layer_additional_info";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO;
        data->mapped_action[0] = DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 79 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 79);
        data->signal_name = "slb_learn_needed";
        data->mapped_qual = DNX_FIELD_QUAL_SLB_LEARN_NEEDED;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 80 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 80);
        data->signal_name = "slb_key";
        data->mapped_qual = DNX_FIELD_QUAL_SLB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 81 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 81);
        data->signal_name = "slb_payload";
        data->mapped_qual = DNX_FIELD_QUAL_SLB_PAYLOAD;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 82 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 82);
        data->signal_name = "learn_info";
        data->mapped_qual = DNX_FIELD_QUAL_LEARN_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 83 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 83);
        data->signal_name = "latency_flow_id_valid";
        data->mapped_qual = DNX_FIELD_QUAL_LATENCY_FLOW_ID_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 84 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 84);
        data->signal_name = "weak_tm_profile";
        data->mapped_qual = DNX_FIELD_QUAL_WEAK_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 85 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 85);
        data->signal_name = "stat_obj_lm_read_index";
        data->mapped_qual = DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX;
        data->mapped_action[0] = DNX_FIELD_ACTION_STAT_OBJ_LM_READ_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 86 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 86);
        data->signal_name = "mc_rpf_mode";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 87 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 87);
        data->signal_name = "ingress_time_stamp";
        data->mapped_action[0] = DNX_FIELD_ACTION_INGRESS_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 88 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 88);
        data->signal_name = "ingress_time_stamp_override";
        data->mapped_action[0] = DNX_FIELD_ACTION_INGRESS_TIME_STAMP_OVERRIDE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 89 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 89);
        data->signal_name = "elephant_trap_info";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 90 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 90);
        data->signal_name = "elephant_learn_needed";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 91 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 91);
        data->signal_name = "rpf_ecmp_group";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_ECMP_GROUP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 92 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 92);
        data->signal_name = "weak_tm_valid";
        data->mapped_qual = DNX_FIELD_QUAL_WEAK_TM_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 93 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 93);
        data->signal_name = "fer_statistics_obj";
        data->mapped_qual = DNX_FIELD_QUAL_FER_STATISTICS_OBJ;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 94 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 94);
        data->signal_name = "fer_statistics_obj_atr";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 95 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 95);
        data->signal_name = "rpf_ecmp_mode";
        data->mapped_qual = DNX_FIELD_QUAL_RPF_ECMP_MODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 96 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 96);
        data->signal_name = "ptc";
        data->mapped_qual = DNX_FIELD_QUAL_PTC;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 97 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 97);
        data->signal_name = "nasid";
        data->mapped_qual = DNX_FIELD_QUAL_NASID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 98 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 98);
        data->signal_name = "context";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 99 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 99);
        data->signal_name = "rpf_destination_is_ecmp";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 100 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 100);
        data->signal_name = "bytes_to_remove_header";
        data->mapped_qual = DNX_FIELD_QUAL_BYTES_TO_REMOVE;
        data->offset_from_qual = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 101 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 101);
        data->signal_name = "bytes_to_remove_fix";
        data->mapped_qual = DNX_FIELD_QUAL_BYTES_TO_REMOVE;
        data->offset_from_qual = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 102 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 102);
        data->signal_name = "cpu_trap_code";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 103 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 103);
        data->signal_name = "fwd_strength";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 104 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 104);
        data->signal_name = "int_profile";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 105 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 105);
        data->signal_name = "itpp_delta_valid";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 106 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 106);
        data->signal_name = "nwk_header_append_size_ptr";
        data->mapped_action[0] = DNX_FIELD_ACTION_NWK_HEADER_APPEND_SIZE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 107 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 107);
        data->signal_name = "int_command";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 108 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 108);
        data->signal_name = "system_header_size_adjust";
        data->mapped_action[0] = DNX_FIELD_ACTION_SYSTEM_HEADER_SIZE_ADJUST;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 109 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 109);
        data->signal_name = "parsing_start_offset";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 110 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 110);
        data->signal_name = "parsing_start_offset_valid";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 111 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 111);
        data->signal_name = "parsing_start_type";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 112 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 112);
        data->signal_name = "parsing_start_type_valid";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 113 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 113);
        data->signal_name = "ntp_time_of_day";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 114 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 114);
        data->signal_name = "next_nasid";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 115 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 115);
        data->signal_name = "packet_header";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 116 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 116);
        data->signal_name = "cs_key";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 117 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 117);
        data->signal_name = "next_context";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 118 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 118);
        data->signal_name = "fwd_action_cpu_trap_qual";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 119 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 119);
        data->signal_name = "packet_size";
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 120 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 120);
        data->signal_name = "oam_time_of_day";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 0);
        data->signal_name = "bit_position_idx";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 1);
        data->signal_name = "etm_record";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 2);
        data->signal_name = "discard";
        data->mapped_qual = DNX_FIELD_QUAL_DISCARD;
        data->mapped_action[0] = DNX_FIELD_ACTION_DISCARD;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 3);
        data->signal_name = "is_tdm";
        data->mapped_qual = DNX_FIELD_QUAL_IS_TDM;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 4);
        data->signal_name = "pp_dsp";
        data->mapped_qual = DNX_FIELD_QUAL_PP_DSP;
        data->mapped_action[0] = DNX_FIELD_ACTION_PP_DSP;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 5);
        data->signal_name = "cud_outlif_or_mcdb_ptr";
        data->mapped_qual = DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR;
        data->mapped_action[0] = DNX_FIELD_ACTION_CUD_OUTLIF_OR_MCDB_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 6);
        data->signal_name = "ftmh_packet_size";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_PACKET_SIZE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 7);
        data->signal_name = "ftmh_tc";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_TC;
        data->mapped_action[0] = DNX_FIELD_ACTION_TC;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 8);
        data->signal_name = "ftmh_source_system_port";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 9);
        data->signal_name = "ftmh_dp";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_DP;
        data->mapped_action[0] = DNX_FIELD_ACTION_DP;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 10);
        data->signal_name = "ftmh_tm_action_type";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 11);
        data->signal_name = "ftmh_tsh_ext_present";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_TSH_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 12);
        data->signal_name = "ftmh_pph_present";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_PPH_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 13);
        data->signal_name = "ftmh_tm_action_is_mc";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 14);
        data->signal_name = "ftmh_mcid_or_outlif_0_or_mcdb_ptr";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 15);
        data->signal_name = "ftmh_tm_destination_ext_present";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 16);
        data->signal_name = "ftmh_application_specific_ext_present";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 17);
        data->signal_name = "ftmh_flow_id_ext_present";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_FLOW_ID_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 18);
        data->signal_name = "ftmh_bier_bfr_ext_present";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_BIER_BFR_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 19);
        data->signal_name = "bier_bfr_ext";
        data->mapped_qual = DNX_FIELD_QUAL_BIER_BFR_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 20 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 20);
        data->signal_name = "pph_end_of_packet_editing";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 21 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 21);
        data->signal_name = "lif_ext";
        data->mapped_qual = DNX_FIELD_QUAL_LIF_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 22 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 22);
        data->signal_name = "ftmh_cni";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_CNI;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 23 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 23);
        data->signal_name = "ftmh_ecn_enable";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_ECN_ENABLE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 24 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 24);
        data->signal_name = "ftmh_tm_profile";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 25 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 25);
        data->signal_name = "ftmh_visibility";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_VISIBILITY;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 26 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 26);
        data->signal_name = "ftmh_reserved";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_RESERVED;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 27 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 27);
        data->signal_name = "lb_key_ext";
        data->mapped_qual = DNX_FIELD_QUAL_LB_KEY;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 28 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 28);
        data->signal_name = "stacking_ext";
        data->mapped_qual = DNX_FIELD_QUAL_STACKING_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 29 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 29);
        data->signal_name = "tm_destination_ext";
        data->mapped_qual = DNX_FIELD_QUAL_TM_DESTINATION_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 30 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 30);
        data->signal_name = "application_specific_ext";
        data->mapped_qual = DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 31 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 31);
        data->signal_name = "tsh_ext";
        data->mapped_qual = DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 32 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 32);
        data->signal_name = "pph_fhei_size";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_FHEI_SIZE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 33 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 33);
        data->signal_name = "pph_learn_ext_present";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 34 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 34);
        data->signal_name = "pph_lif_ext_type";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_LIF_EXT_TYPE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 35 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 35);
        data->signal_name = "pph_parsing_start_offset";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_PARSING_START_OFFSET;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 36 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 36);
        data->signal_name = "pph_parsing_start_type";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_PARSING_START_TYPE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 37 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 37);
        data->signal_name = "pph_fwd_layer_index";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 38 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 38);
        data->signal_name = "pph_forwarding_layer_additional_info";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 39 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 39);
        data->signal_name = "forwarding_strength";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 40 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 40);
        data->signal_name = "pph_in_lif_profile";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 41 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 41);
        data->signal_name = "pph_forward_domain";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_FWD_DOMAIN;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 42 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 42);
        data->signal_name = "pph_in_lif";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_IN_LIF;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 43 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 43);
        data->signal_name = "pph_nwk_qos";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_NWK_QOS;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 44 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 44);
        data->signal_name = "pph_ttl";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_TTL;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 45 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 45);
        data->signal_name = "pph_value1";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_VALUE1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 46 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 46);
        data->signal_name = "pph_value2";
        data->mapped_qual = DNX_FIELD_QUAL_PPH_VALUE2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 47 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 47);
        data->signal_name = "fhei_ext";
        data->mapped_qual = DNX_FIELD_QUAL_FHEI_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 48 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 48);
        data->signal_name = "learn_ext";
        data->mapped_qual = DNX_FIELD_QUAL_LEARN_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 49 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 49);
        data->signal_name = "learn_valid";
        data->mapped_qual = DNX_FIELD_QUAL_LEARN_VALID;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 50 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 50);
        data->signal_name = "udh";
        data->mapped_qual = DNX_FIELD_QUAL_USER_HEADER_4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 51 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 51);
        data->signal_name = "ip_mc_eligible";
        data->mapped_qual = DNX_FIELD_QUAL_IP_MC_ELIGIBLE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 52 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 52);
        data->signal_name = "system_headers_size";
        data->mapped_qual = DNX_FIELD_QUAL_SYSTEM_HEADERS_SIZE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 53 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 53);
        data->signal_name = "packet_header";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 54 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 54);
        data->signal_name = "epp_layer_offsets";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 55 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 55);
        data->signal_name = "epp_layer_protocols";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 56 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 56);
        data->signal_name = "epp_layer_qualifiers";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 57 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 57);
        data->signal_name = "packet_is_applet";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 58 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 58);
        data->signal_name = "mem_soft_err";
        data->mapped_qual = DNX_FIELD_QUAL_MEM_SOFT_ERR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 59 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 59);
        data->signal_name = "estimated_btr";
        data->mapped_qual = DNX_FIELD_QUAL_ESTIMATED_BTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 60 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 60);
        data->signal_name = "tc_map_profile";
        data->mapped_qual = DNX_FIELD_QUAL_TC_MAP_PROFILE;
        data->mapped_action[0] = DNX_FIELD_ACTION_TC_MAP_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 61 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 61);
        data->signal_name = "dst_sys_port";
        data->mapped_qual = DNX_FIELD_QUAL_DST_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 62 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 62);
        data->signal_name = "out_tm_port";
        data->mapped_qual = DNX_FIELD_QUAL_OUT_TM_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 63 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 63);
        data->signal_name = "out_pp_port";
        data->mapped_qual = DNX_FIELD_QUAL_OUT_PP_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 64 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 64);
        data->signal_name = "outlif_profile";
        data->mapped_qual = DNX_FIELD_QUAL_OUT_LIF_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 65 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 65);
        data->signal_name = "fabric_or_egress_mc";
        data->mapped_qual = DNX_FIELD_QUAL_FABRIC_OR_EGRESS_MC;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 66 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 66);
        data->signal_name = "fwd_context";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_CONTEXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 67 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 67);
        data->signal_name = "eth_tag_format";
        data->mapped_qual = DNX_FIELD_QUAL_ETH_TAG_FORMAT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 68 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 68);
        data->signal_name = "updated_tpids_packet_data";
        data->mapped_qual = DNX_FIELD_QUAL_UPDATED_TPIDS_PACKET_DATA;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 69 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 69);
        data->signal_name = "ive_bytes_to_add";
        data->mapped_qual = DNX_FIELD_QUAL_IVE_BYTES_TO_ADD;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 70 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 70);
        data->signal_name = "ive_bytes_to_remove";
        data->mapped_qual = DNX_FIELD_QUAL_IVE_BYTES_TO_REMOVE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 71 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 71);
        data->signal_name = "eth_type_code";
        data->mapped_qual = DNX_FIELD_QUAL_ETHER_TYPE_CODE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 72 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 72);
        data->signal_name = "inlif0";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_IN_LIF_0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 73 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 73);
        data->signal_name = "inlif_profile0";
        data->mapped_qual = DNX_FIELD_QUAL_IN_LIF_PROFILE_0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 74 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 74);
        data->signal_name = "local_outlif";
        data->mapped_qual = DNX_FIELD_QUAL_LOCAL_OUT_LIF;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 75 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 75);
        data->signal_name = "global_outlif0";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_OUT_LIF_0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 76 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 76);
        data->signal_name = "global_outlif1";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_OUT_LIF_1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 77 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 77);
        data->signal_name = "global_outlif2";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_OUT_LIF_2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 78 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 78);
        data->signal_name = "global_outlif3";
        data->mapped_qual = DNX_FIELD_QUAL_GLOB_OUT_LIF_3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 79 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 79);
        data->signal_name = "ip_mc_should_be_bridged";
        data->mapped_qual = DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 80 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 80);
        data->signal_name = "glem_pp_trap_is_required";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 81 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 81);
        data->signal_name = "glem_non_pp_trap_is_required";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 82 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 82);
        data->signal_name = "per_port_table_data";
        data->mapped_qual = DNX_FIELD_QUAL_PER_PORT_TABLE_DATA;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 83 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 83);
        data->signal_name = "enable_filter_per_port_table_data";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 84 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 84);
        data->signal_name = "forwarding_action_profile_index";
        data->mapped_qual = DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 85 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 85);
        data->signal_name = "snoop_action_profile_index";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 86 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 86);
        data->signal_name = "snoop_strength";
        data->mapped_qual = DNX_FIELD_QUAL_SNOOP_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 87 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 87);
        data->signal_name = "mirror_profile";
        data->mapped_action[0] = DNX_FIELD_ACTION_MIRROR_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 88 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 88);
        data->signal_name = "snoop_profile";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 89 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 89);
        data->signal_name = "cos_profile";
        data->mapped_action[0] = DNX_FIELD_ACTION_COS_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 90 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 90);
        data->signal_name = "pmf_counter_0_profile";
        data->mapped_action[0] = DNX_FIELD_ACTION_PMF_COUNTER_0_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 91 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 91);
        data->signal_name = "pmf_counter_0_ptr";
        data->mapped_action[0] = DNX_FIELD_ACTION_PMF_COUNTER_0_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 92 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 92);
        data->signal_name = "pmf_counter_1_profile";
        data->mapped_action[0] = DNX_FIELD_ACTION_PMF_COUNTER_1_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 93 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 93);
        data->signal_name = "pmf_counter_1_ptr";
        data->mapped_action[0] = DNX_FIELD_ACTION_PMF_COUNTER_1_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 94 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 94);
        data->signal_name = "packet_size";
        data->mapped_qual = DNX_FIELD_QUAL_FTMH_PACKET_SIZE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 95 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 95);
        data->signal_name = "otm";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 96 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 96);
        data->signal_name = "egress_multicast";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 97 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 97);
        data->signal_name = "sys_tc";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 98 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 98);
        data->signal_name = "sys_dp";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 99 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 99);
        data->signal_name = "tm_counter_ptr_valid";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 100 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 100);
        data->signal_name = "counter_0_valid";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 101 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 101);
        data->signal_name = "counter_0_type";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 102 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 102);
        data->signal_name = "counter_0_ptr";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 103 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 103);
        data->signal_name = "counter_1_valid";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 104 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 104);
        data->signal_name = "counter_1_type";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 105 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 105);
        data->signal_name = "counter_1_ptr";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 106 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 106);
        data->signal_name = "counter_metadata";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 107 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 107);
        data->signal_name = "counter_packet_size";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 108 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 108);
        data->signal_name = "bier_bit_mask";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 109 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 109);
        data->signal_name = "erpp_cud";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 110 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 110);
        data->signal_name = "mact_learn_command_data";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 111 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 111);
        data->signal_name = "mact_learn_command_valid";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 112 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 112);
        data->signal_name = "ingress_packet_header";
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 113 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 113);
        data->signal_name = "ftmh";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_profile_bits_max_port_profile_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_max_port_profile_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.profile_bits.ingress_pp_port_key_gen_var_size_get(unit), 64);

    
    define->data = UTILEX_MAX(dnx_data_field.profile_bits.ingress_pp_port_key_gen_var_size_get(unit), 64);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_profile_bits_nof_bits_in_port_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_nof_bits_in_port_profile;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_field.profile_bits.ingress_pp_port_key_gen_var_size_get(unit))-(dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit)));

    
    define->data = ((dnx_data_field.profile_bits.ingress_pp_port_key_gen_var_size_get(unit))-(dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit)));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_profile_bits_nof_ing_in_lif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_nof_ing_in_lif;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_IN_LIF_PROFILE_NOF_BITS;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 4;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_profile_bits_nof_ing_eth_rif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_nof_ing_eth_rif;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_IN_RIF_PROFILE_NOF_BITS;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 4;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_dir_ext_half_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_dir_ext;
    int define_index = dnx_data_field_dir_ext_define_half_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_dir_ext_single_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_dir_ext;
    int define_index = dnx_data_field_dir_ext_define_single_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_state_table_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_address_size_entry_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_address_size_entry_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_data_size_entry_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_data_size_entry_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_address_max_entry_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_address_max_entry_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1023;

    
    define->data = 1023;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_wr_bit_size_rw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_wr_bit_size_rw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_ipmf2_key_select_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_ipmf2_key_select_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_data_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_data_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.state_table.data_size_entry_max_get(unit));

    
    define->data = (dnx_data_field.state_table.data_size_entry_max_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_STATE_TABLE_PAYLOAD_SIZE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "8";
    define->property.mapping[0].val = 8;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_state_table_stage_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_state_table_stage_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_J;

    
    define->data = DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_J;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_STATE_TABLE_RMW_SOURCE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "ipmf1_key_j_msb";
    define->property.mapping[0].val = DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_J;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_address_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_address_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.state_table.address_size_entry_max_get(unit)+(utilex_log2_round_up(dnx_data_field.state_table.data_size_entry_max_get(unit)/dnx_data_field.state_table.data_size_get(unit))));

    
    define->data = (dnx_data_field.state_table.address_size_entry_max_get(unit)+(utilex_log2_round_up(dnx_data_field.state_table.data_size_entry_max_get(unit)/dnx_data_field.state_table.data_size_get(unit))));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_address_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_address_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((((dnx_data_field.state_table.address_max_entry_max_get(unit))+1)*((dnx_data_field.state_table.data_size_entry_max_get(unit))/(dnx_data_field.state_table.data_size_get(unit))))-1);

    
    define->data = ((((dnx_data_field.state_table.address_max_entry_max_get(unit))+1)*((dnx_data_field.state_table.data_size_entry_max_get(unit))/(dnx_data_field.state_table.data_size_get(unit))))-1);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_state_table_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_state_table;
    int define_index = dnx_data_field_state_table_define_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.state_table.address_size_get(unit)+dnx_data_field.state_table.data_size_get(unit)+dnx_data_field.state_table.wr_bit_size_rw_get(unit));

    
    define->data = (dnx_data_field.state_table.address_size_get(unit)+dnx_data_field.state_table.data_size_get(unit)+dnx_data_field.state_table.wr_bit_size_rw_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_action_size_large_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_action_size_large;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_ipmf1_key_select_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_ipmf1_key_select_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_ipmf1_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_ipmf1_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_ipmf2_key_select_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_ipmf2_key_select_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_ipmf2_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_ipmf2_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_ipmf3_key_select_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_ipmf3_key_select_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_ipmf3_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_ipmf3_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_mdb_dt_pmf_map_stage_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_mdb_dt;
    int define_index = dnx_data_field_mdb_dt_define_pmf_map_stage;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FIELD_STAGE_IPMF1;

    
    define->data = DNX_FIELD_STAGE_IPMF1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_MAP_STAGE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "IPMF1";
    define->property.mapping[0].val = DNX_FIELD_STAGE_IPMF1;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "IPMF3";
    define->property.mapping[1].val = DNX_FIELD_STAGE_IPMF3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_hash_max_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_hash;
    int define_index = dnx_data_field_hash_define_max_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_udh_ext_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_ext_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_udh_type_count_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_type_count;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_udh_type_0_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_type_0_length;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "type_0_length", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_field_udh_type_1_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_type_1_length;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "type_1_length", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_field_udh_type_2_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_type_2_length;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "type_2_length", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_field_udh_type_3_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_type_3_length;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "type_3_length", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




static shr_error_e
jr2_a0_dnx_data_field_udh_field_class_id_size_0_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_field_class_id_size_0;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FIELD_CLASS_ID_SIZE_0;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 5;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "8";
    define->property.mapping[1].val = 8;
    define->property.mapping[2].name = "16";
    define->property.mapping[2].val = 16;
    define->property.mapping[3].name = "24";
    define->property.mapping[3].val = 24;
    define->property.mapping[4].name = "32";
    define->property.mapping[4].val = 32;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_udh_field_class_id_size_1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_field_class_id_size_1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FIELD_CLASS_ID_SIZE_1;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 5;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "8";
    define->property.mapping[1].val = 8;
    define->property.mapping[2].name = "16";
    define->property.mapping[2].val = 16;
    define->property.mapping[3].name = "24";
    define->property.mapping[3].val = 24;
    define->property.mapping[4].name = "32";
    define->property.mapping[4].val = 32;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_udh_field_class_id_size_2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_field_class_id_size_2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FIELD_CLASS_ID_SIZE_2;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 5;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "8";
    define->property.mapping[1].val = 8;
    define->property.mapping[2].name = "16";
    define->property.mapping[2].val = 16;
    define->property.mapping[3].name = "24";
    define->property.mapping[3].val = 24;
    define->property.mapping[4].name = "32";
    define->property.mapping[4].val = 32;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_udh_field_class_id_size_3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_field_class_id_size_3;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FIELD_CLASS_ID_SIZE_3;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 5;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "8";
    define->property.mapping[1].val = 8;
    define->property.mapping[2].name = "16";
    define->property.mapping[2].val = 16;
    define->property.mapping[3].name = "24";
    define->property.mapping[3].val = 24;
    define->property.mapping[4].name = "32";
    define->property.mapping[4].val = 32;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_udh_field_class_id_total_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_udh;
    int define_index = dnx_data_field_udh_define_field_class_id_total_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.udh.field_class_id_size_3_get(unit))+(dnx_data_field.udh.field_class_id_size_2_get(unit))+(dnx_data_field.udh.field_class_id_size_1_get(unit))+(dnx_data_field.udh.field_class_id_size_0_get(unit));

    
    define->data = (dnx_data_field.udh.field_class_id_size_3_get(unit))+(dnx_data_field.udh.field_class_id_size_2_get(unit))+(dnx_data_field.udh.field_class_id_size_1_get(unit))+(dnx_data_field.udh.field_class_id_size_0_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_system_headers_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_system_headers;
    int define_index = dnx_data_field_system_headers_define_nof_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_system_headers_pph_learn_ext_disable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_system_headers;
    int define_index = dnx_data_field_system_headers_define_pph_learn_ext_disable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BCM886XX_PPH_LEARN_EXTENSION_DISABLE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_system_headers_system_header_profiles_set(
    int unit)
{
    int system_header_profile_index;
    dnx_data_field_system_headers_system_header_profiles_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_system_headers;
    int table_index = dnx_data_field_system_headers_table_system_header_profiles;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_NOF;

    
    table->values[0].default_val = "unsupported";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_system_headers_system_header_profiles_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_system_headers_table_system_header_profiles");

    
    default_data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->name = "unsupported";
    default_data->build_ftmh = 0;
    default_data->build_tsh = 0;
    default_data->build_pph = 0;
    default_data->build_udh = 0;
    default_data->never_add_pph_learn_ext = 0;
    
    for (system_header_profile_index = 0; system_header_profile_index < table->keys[0].size; system_header_profile_index++)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, system_header_profile_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_EMPTY < table->keys[0].size)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_EMPTY, 0);
        data->name = "NONE";
    }
    if (DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH < table->keys[0].size)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH, 0);
        data->name = "FTMH";
        data->build_ftmh = 1;
    }
    if (DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_PPH < table->keys[0].size)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_PPH, 0);
        data->name = "FTMH_PPH";
        data->build_ftmh = 1;
        data->build_pph = 1;
        data->never_add_pph_learn_ext = 1;
    }
    if (DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH < table->keys[0].size)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH, 0);
        data->name = "FTMH_TSH";
        data->build_ftmh = 1;
        data->build_tsh = 1;
    }
    if (DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH < table->keys[0].size)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH, 0);
        data->name = "FTMH_TSH_PPH";
        data->build_ftmh = 1;
        data->build_tsh = 1;
        data->build_pph = 1;
        data->never_add_pph_learn_ext = 1;
    }
    if (DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH < table->keys[0].size)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH, 0);
        data->name = "FTMH_TSH_PPH_UDH";
        data->build_ftmh = 1;
        data->build_tsh = 1;
        data->build_pph = 1;
        data->build_udh = 1;
        data->never_add_pph_learn_ext = dnx_data_headers.system_headers.system_headers_mode_get(unit) == dnx_data_headers.system_headers.system_headers_mode_jericho2_get(unit) ? 1 : dnx_data_field.system_headers.pph_learn_ext_disable_get(unit);
    }
    if (DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_UDH < table->keys[0].size)
    {
        data = (dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_UDH, 0);
        data->name = "FTMH_UDH";
        data->build_ftmh = 1;
        data->build_udh = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_field_exem_exem_vmv_padded_for_2msb_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int feature_index = dnx_data_field_exem_exem_vmv_padded_for_2msb;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_exem_small_app_db_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_app_db_id_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_large_app_db_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_app_db_id_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_max_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_max_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_large_max_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_max_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_max_result_size_all_stages_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_max_result_size_all_stages;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.exem_max_result_size_get(unit), dnx_data_field.base_ipmf2.exem_max_result_size_get(unit), dnx_data_field.base_ipmf3.exem_max_result_size_get(unit), dnx_data_field.base_epmf.exem_max_result_size_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.exem_max_result_size_get(unit), dnx_data_field.base_ipmf2.exem_max_result_size_get(unit), dnx_data_field.base_ipmf3.exem_max_result_size_get(unit), dnx_data_field.base_epmf.exem_max_result_size_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_max_container_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_max_container_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_large_max_container_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_max_container_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 240;

    
    define->data = 240;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_vmv_hw_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_vmv_hw_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_min_app_db_id_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_min_app_db_id_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_large_min_app_db_id_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_min_app_db_id_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_max_app_db_id_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_max_app_db_id_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 63;

    
    define->data = 63;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_large_max_app_db_id_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_max_app_db_id_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 63;

    
    define->data = 63;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_ipmf2_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_ipmf2_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_ipmf2_key_part_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_ipmf2_key_part;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_ipmf2_key_hw_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_ipmf2_key_hw_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.exem.small_ipmf2_key_get(unit);

    
    define->data = dnx_data_field.exem.small_ipmf2_key_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_small_ipmf2_key_hw_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_ipmf2_key_hw_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_ipmf1_key_configurable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_ipmf1_key_configurable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_ipmf2_key_configurable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_ipmf2_key_configurable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_ipmf1_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_ipmf1_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_exem_pmf_sexem3_stage_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_pmf_sexem3_stage;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "pmf_sexem3_stage", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FIELD_STAGE_IPMF3;

    
    define->data = DNX_FIELD_STAGE_IPMF3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_SEXEM3_STAGE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "IPMF2";
    define->property.mapping[0].val = DNX_FIELD_STAGE_IPMF2;
    define->property.mapping[1].name = "IPMF3";
    define->property.mapping[1].val = DNX_FIELD_STAGE_IPMF3;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_has_elephant_signals_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int feature_index = dnx_data_field_exem_learn_flush_machine_has_elephant_signals;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_flush_machine_nof_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_flush_machine_nof_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_entry_extra_bits_for_hash_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_entry_extra_bits_for_hash;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_sexem_entry_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_sexem_entry_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.exem.small_max_container_size_get(unit)+dnx_data_field.exem_learn_flush_machine.entry_extra_bits_for_hash_get(unit);

    
    define->data = dnx_data_field.exem.small_max_container_size_get(unit)+dnx_data_field.exem_learn_flush_machine.entry_extra_bits_for_hash_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_lexem_entry_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_lexem_entry_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.exem.large_max_container_size_get(unit)+dnx_data_field.exem_learn_flush_machine.entry_extra_bits_for_hash_get(unit);

    
    define->data = dnx_data_field.exem.large_max_container_size_get(unit)+dnx_data_field.exem_learn_flush_machine.entry_extra_bits_for_hash_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_command_bit_transplant_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_command_bit_transplant;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_command_bit_delete_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_command_bit_delete;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_command_bit_clear_hit_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_command_bit_clear_hit_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_nof_bits_source_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_nof_bits_source;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_source_bit_for_scan_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_source_bit_for_scan;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_source_bit_for_pipe_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_source_bit_for_pipe;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_source_bit_for_mrq_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_source_bit_for_mrq;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_nof_bits_accessed_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_nof_bits_accessed;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_nof_flush_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int define_index = dnx_data_field_exem_learn_flush_machine_define_nof_flush_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_set(
    int unit)
{
    dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int table_index = dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_92000m";
    table->values[1].default_val = "ITEM_0_0f";
    table->values[2].default_val = "ITEM_155_291f";
    table->values[3].default_val = "ITEM_8_144f";
    table->values[4].default_val = "ITEM_292_294f";
    table->values[5].default_val = "ITEM_145_147f";
    table->values[6].default_val = "ITEM_149_154f";
    table->values[7].default_val = "ITEM_2_7f";
    table->values[8].default_val = "ITEM_148_148f";
    table->values[9].default_val = "ITEM_1_1f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, (1 + 1  ), "data of dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories");

    
    default_data = (dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_92000m;
    default_data->valid = ITEM_0_0f;
    default_data->entry_data = ITEM_155_291f;
    default_data->entry_mask = ITEM_8_144f;
    default_data->src_data = ITEM_292_294f;
    default_data->src_mask = ITEM_145_147f;
    default_data->appdb_id_data = ITEM_149_154f;
    default_data->appdb_id_mask = ITEM_2_7f;
    default_data->accessed_data = ITEM_148_148f;
    default_data->accessed_mask = ITEM_1_1f;
    
    data = (dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_set(
    int unit)
{
    dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int table_index = dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_101000m";
    table->values[1].default_val = "ITEM_0_0f";
    table->values[2].default_val = "ITEM_275_531f";
    table->values[3].default_val = "ITEM_8_264f";
    table->values[4].default_val = "ITEM_532_534f";
    table->values[5].default_val = "ITEM_265_267f";
    table->values[6].default_val = "ITEM_269_274f";
    table->values[7].default_val = "ITEM_2_7f";
    table->values[8].default_val = "ITEM_268_268f";
    table->values[9].default_val = "ITEM_1_1f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, (1 + 1  ), "data of dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories");

    
    default_data = (dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_101000m;
    default_data->valid = ITEM_0_0f;
    default_data->entry_data = ITEM_275_531f;
    default_data->entry_mask = ITEM_8_264f;
    default_data->src_data = ITEM_532_534f;
    default_data->src_mask = ITEM_265_267f;
    default_data->appdb_id_data = ITEM_269_274f;
    default_data->appdb_id_mask = ITEM_2_7f;
    default_data->accessed_data = ITEM_268_268f;
    default_data->accessed_mask = ITEM_1_1f;
    
    data = (dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_set(
    int unit)
{
    dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int table_index = dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_93000m";
    table->values[1].default_val = "ITEM_0_3f";
    table->values[2].default_val = "ITEM_64_123f";
    table->values[3].default_val = "ITEM_4_63f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t, (1 + 1  ), "data of dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules");

    
    default_data = (dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_93000m;
    default_data->command = ITEM_0_3f;
    default_data->payload = ITEM_64_123f;
    default_data->payload_mask = ITEM_4_63f;
    
    data = (dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_set(
    int unit)
{
    dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    int table_index = dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_102000m";
    table->values[1].default_val = "ITEM_0_3f";
    table->values[2].default_val = "ITEM_64_123f";
    table->values[3].default_val = "ITEM_4_63f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t, (1 + 1  ), "data of dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules");

    
    default_data = (dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_102000m;
    default_data->command = ITEM_0_3f;
    default_data->payload = ITEM_64_123f;
    default_data->payload_mask = ITEM_4_63f;
    
    data = (dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_ace_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_ace_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_ace_id_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_app_db_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_app_db_id_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_payload_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_payload_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 110;

    
    define->data = 110;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_min_key_range_pmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_min_key_range_pmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3c0000;

    
    define->data = 0x3c0000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_max_key_range_pmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_max_key_range_pmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3fffff;

    
    define->data = 0x3fffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_min_ace_id_dynamic_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_min_ace_id_dynamic_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_ace_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_ace_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_action_per_ace_format_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_action_per_ace_format;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_row_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_row_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_min_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_min_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_ace_id_pmf_alloc_first_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_ace_id_pmf_alloc_first;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_ace_id_pmf_alloc_last_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_ace_id_pmf_alloc_last;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 63;

    
    define->data = 63;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_masks_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_masks_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_fes_id_per_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_fes_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_fes_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_fes_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_fes_instruction_per_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_fes_instruction_per_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_field.ace.nof_fes_id_per_array_get(unit)*dnx_data_field.ace.nof_fes_array_get(unit));

    
    define->data = (dnx_data_field.ace.nof_fes_id_per_array_get(unit)*dnx_data_field.ace.nof_fes_array_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_fes_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_fes_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_prog_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_prog_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_nof_bits_in_fes_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_nof_bits_in_fes_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_fes_shift_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_fes_shift_for_zero_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 110;

    
    define->data = 110;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_fes_instruction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_fes_instruction_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_fes_shift_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_fes_shift_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_fes_invalid_bits_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_fes_invalid_bits_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_fes_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_fes_type_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_fes_ace_action_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_fes_ace_action_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_ace_fes_chosen_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_fes_chosen_mask_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_entry_dir_ext_nof_fields_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_entry;
    int define_index = dnx_data_field_entry_define_dir_ext_nof_fields;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_entry_nof_action_params_per_entry_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_entry;
    int define_index = dnx_data_field_entry_define_nof_action_params_per_entry;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_entry_nof_qual_params_per_entry_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_entry;
    int define_index = dnx_data_field_entry_define_nof_qual_params_per_entry;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_L4_Ops_udp_position_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_L4_Ops;
    int define_index = dnx_data_field_L4_Ops_define_udp_position;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_L4_Ops_tcp_position_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_L4_Ops;
    int define_index = dnx_data_field_L4_Ops_define_tcp_position;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_L4_Ops_nof_range_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_L4_Ops;
    int define_index = dnx_data_field_L4_Ops_define_nof_range_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_L4_Ops_nof_ext_encoders_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_L4_Ops;
    int define_index = dnx_data_field_L4_Ops_define_nof_ext_encoders;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_L4_Ops_nof_ext_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_L4_Ops;
    int define_index = dnx_data_field_L4_Ops_define_nof_ext_types;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_L4_Ops_nof_configurable_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_L4_Ops;
    int define_index = dnx_data_field_L4_Ops_define_nof_configurable_ranges;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_encoded_qual_actions_offset_trap_strength_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_encoded_qual_actions_offset;
    int define_index = dnx_data_field_encoded_qual_actions_offset_define_trap_strength_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_encoded_qual_actions_offset_trap_qualifier_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_encoded_qual_actions_offset;
    int define_index = dnx_data_field_encoded_qual_actions_offset_define_trap_qualifier_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_encoded_qual_actions_offset_sniff_qualifier_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_encoded_qual_actions_offset;
    int define_index = dnx_data_field_encoded_qual_actions_offset_define_sniff_qualifier_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_encoded_qual_actions_offset_mirror_qualifier_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_encoded_qual_actions_offset;
    int define_index = dnx_data_field_encoded_qual_actions_offset_define_mirror_qualifier_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_encoded_qual_actions_offset_forward_trap_qualifier_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_encoded_qual_actions_offset;
    int define_index = dnx_data_field_encoded_qual_actions_offset_define_forward_trap_qualifier_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_compare_equal_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_equal_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_compare_not_equal_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_not_equal_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_compare_smaller_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_smaller_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_compare_not_smaller_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_not_smaller_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_compare_bigger_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_bigger_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_compare_not_bigger_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_not_bigger_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_compare_nof_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_compare_nof_compare_pairs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_compare;
    int define_index = dnx_data_field_compare_define_nof_compare_pairs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_diag_tcam_cs_hit_bit_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int feature_index = dnx_data_field_diag_tcam_cs_hit_bit_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_diag_bytes_to_remove_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_bytes_to_remove_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x0F;

    
    define->data = 0x0F;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_layers_to_remove_size_in_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_layers_to_remove_size_in_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_layers_to_remove_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_layers_to_remove_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x07;

    
    define->data = 0x07;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_nof_signals_per_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_nof_signals_per_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_nof_signals_per_qualifier_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_nof_signals_per_qualifier;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_key_signal_size_in_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_key_signal_size_in_bytes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_result_signal_size_in_words_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_result_signal_size_in_words;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_dt_result_signal_size_in_words_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_dt_result_signal_size_in_words;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_diag_hit_signal_size_in_words_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_hit_signal_size_in_words;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_vw_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_vw_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 28;

    
    define->data = 28;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_pwe_in_lif_wide_data_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_pwe_in_lif_wide_data_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_ip_tunnel_in_lif_wide_data_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_ip_tunnel_in_lif_wide_data_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_native_ac_in_lif_wide_data_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_native_ac_in_lif_wide_data_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_extended_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_extended_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 59;

    
    define->data = 59;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_mact_entry_grouping_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_mact_entry_grouping_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_vw_vip_valid_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_vw_vip_valid_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_vw_vip_id_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_vw_vip_id_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_vw_member_reference_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_vw_member_reference_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_vw_pcc_hit_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_vw_pcc_hit_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_l3srcbind_hit_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_l3srcbind_hit_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_srv6_next_si_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_srv6_next_si;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_general_data_qualifiers_srv6_next_gsid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    int define_index = dnx_data_field_general_data_qualifiers_define_srv6_next_gsid;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_ffc_get(unit), dnx_data_field.base_ipmf2.nof_ffc_get(unit), dnx_data_field.base_ipmf3.nof_ffc_get(unit), dnx_data_field.base_epmf.nof_ffc_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_ffc_get(unit), dnx_data_field.base_ipmf2.nof_ffc_get(unit), dnx_data_field.base_ipmf3.nof_ffc_get(unit), dnx_data_field.base_epmf.nof_ffc_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_masks_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_masks_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_masks_per_fes_get(unit), dnx_data_field.base_ipmf3.nof_masks_per_fes_get(unit), dnx_data_field.base_epmf.nof_masks_per_fes_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_masks_per_fes_get(unit), dnx_data_field.base_ipmf3.nof_masks_per_fes_get(unit), dnx_data_field.base_epmf.nof_masks_per_fes_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fes_id_per_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fes_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_fes_id_per_array_get(unit), dnx_data_field.base_ipmf2.nof_fes_id_per_array_get(unit), dnx_data_field.base_ipmf3.nof_fes_id_per_array_get(unit), dnx_data_field.base_epmf.nof_fes_id_per_array_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_fes_id_per_array_get(unit), dnx_data_field.base_ipmf2.nof_fes_id_per_array_get(unit), dnx_data_field.base_ipmf3.nof_fes_id_per_array_get(unit), dnx_data_field.base_epmf.nof_fes_id_per_array_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fes_array_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fes_array;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_fes_array_get(unit), dnx_data_field.base_ipmf2.nof_fes_array_get(unit), dnx_data_field.base_ipmf3.nof_fes_array_get(unit), dnx_data_field.base_epmf.nof_fes_array_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_fes_array_get(unit), dnx_data_field.base_ipmf2.nof_fes_array_get(unit), dnx_data_field.base_ipmf3.nof_fes_array_get(unit), dnx_data_field.base_epmf.nof_fes_array_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fes_instruction_per_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fes_instruction_per_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_fes_instruction_per_context_get(unit), dnx_data_field.base_ipmf2.nof_fes_instruction_per_context_get(unit), dnx_data_field.base_ipmf3.nof_fes_instruction_per_context_get(unit), dnx_data_field.base_epmf.nof_fes_instruction_per_context_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_fes_instruction_per_context_get(unit), dnx_data_field.base_ipmf2.nof_fes_instruction_per_context_get(unit), dnx_data_field.base_ipmf3.nof_fes_instruction_per_context_get(unit), dnx_data_field.base_epmf.nof_fes_instruction_per_context_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fes_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fes_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_fes_programs_get(unit), dnx_data_field.base_ipmf3.nof_fes_programs_get(unit), dnx_data_field.base_epmf.nof_fes_programs_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_fes_programs_get(unit), dnx_data_field.base_ipmf3.nof_fes_programs_get(unit), dnx_data_field.base_epmf.nof_fes_programs_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_prog_per_fes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_prog_per_fes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_prog_per_fes_get(unit), dnx_data_field.base_ipmf3.nof_prog_per_fes_get(unit), dnx_data_field.base_epmf.nof_prog_per_fes_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_prog_per_fes_get(unit), dnx_data_field.base_ipmf3.nof_prog_per_fes_get(unit), dnx_data_field.base_epmf.nof_prog_per_fes_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_cs_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_cs_lines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_cs_lines_get(unit), dnx_data_field.base_ipmf2.nof_cs_lines_get(unit), dnx_data_field.base_ipmf3.nof_cs_lines_get(unit), dnx_data_field.base_epmf.nof_cs_lines_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_cs_lines_get(unit), dnx_data_field.base_ipmf2.nof_cs_lines_get(unit), dnx_data_field.base_ipmf3.nof_cs_lines_get(unit), dnx_data_field.base_epmf.nof_cs_lines_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_contexts_get(unit), dnx_data_field.base_ipmf2.nof_contexts_get(unit), dnx_data_field.base_ipmf3.nof_contexts_get(unit), dnx_data_field.base_epmf.nof_contexts_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_contexts_get(unit), dnx_data_field.base_ipmf2.nof_contexts_get(unit), dnx_data_field.base_ipmf3.nof_contexts_get(unit), dnx_data_field.base_epmf.nof_contexts_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_actions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_actions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_actions_get(unit),  dnx_data_field.base_ipmf3.nof_actions_get(unit), dnx_data_field.base_epmf.nof_actions_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_actions_get(unit),  dnx_data_field.base_ipmf3.nof_actions_get(unit), dnx_data_field.base_epmf.nof_actions_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_qualifiers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_qualifiers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_qualifiers_get(unit), dnx_data_field.base_ipmf2.nof_qualifiers_get(unit), dnx_data_field.base_ipmf3.nof_qualifiers_get(unit), dnx_data_field.base_epmf.nof_qualifiers_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_qualifiers_get(unit), dnx_data_field.base_ipmf2.nof_qualifiers_get(unit), dnx_data_field.base_ipmf3.nof_qualifiers_get(unit), dnx_data_field.base_epmf.nof_qualifiers_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fes_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_bits_in_fes_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_bits_in_fes_action_get(unit), dnx_data_field.base_ipmf3.nof_bits_in_fes_action_get(unit), dnx_data_field.base_epmf.nof_bits_in_fes_action_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_bits_in_fes_action_get(unit), dnx_data_field.base_ipmf3.nof_bits_in_fes_action_get(unit), dnx_data_field.base_epmf.nof_bits_in_fes_action_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fes_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_bits_in_fes_key_select_get(unit), dnx_data_field.base_ipmf3.nof_bits_in_fes_key_select_get(unit), dnx_data_field.base_epmf.nof_bits_in_fes_key_select_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_bits_in_fes_key_select_get(unit), dnx_data_field.base_ipmf3.nof_bits_in_fes_key_select_get(unit), dnx_data_field.base_epmf.nof_bits_in_fes_key_select_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_bits_in_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_bits_in_ffc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_ffc_in_uint32_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_ffc_in_uint32;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_field.common_max_val.nof_ffc_get(unit)+32-1)/32);

    
    define->data = ((dnx_data_field.common_max_val.nof_ffc_get(unit)+32-1)/32);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_action_per_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_action_per_group;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.group.nof_action_per_fg_get(unit), dnx_data_field.ace.nof_action_per_ace_format_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_field.group.nof_action_per_fg_get(unit), dnx_data_field.ace.nof_action_per_ace_format_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_layer_records_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_layer_records;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.qual.ingress_nof_layer_records_get(unit), dnx_data_field.qual.egress_nof_layer_records_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_field.qual.ingress_nof_layer_records_get(unit), dnx_data_field.qual.egress_nof_layer_records_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_layer_record_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_layer_record_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.qual.ingress_layer_record_size_get(unit), dnx_data_field.qual.egress_layer_record_size_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_field.qual.ingress_layer_record_size_get(unit), dnx_data_field.qual.egress_layer_record_size_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_l4_ops_ranges_legacy_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.base_ipmf1.nof_l4_ops_ranges_legacy_get(unit), dnx_data_field.base_epmf.nof_l4_ops_ranges_legacy_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_field.base_ipmf1.nof_l4_ops_ranges_legacy_get(unit), dnx_data_field.base_epmf.nof_l4_ops_ranges_legacy_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_pkt_hdr_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_pkt_hdr_ranges_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_pkt_hdr_ranges_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_out_lif_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_out_lif_ranges;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_field.base_ipmf1.nof_out_lif_ranges_get(unit), dnx_data_field.base_ipmf3.nof_out_lif_ranges_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_field.base_ipmf1.nof_out_lif_ranges_get(unit), dnx_data_field.base_ipmf3.nof_out_lif_ranges_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_kbr_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_kbr_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.compare.nof_compare_pairs_get(unit);

    
    define->data = dnx_data_field.compare.nof_compare_pairs_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fem_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_bits_in_fem_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fem_condition_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fem_condition;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_fem_condition_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_fem_condition_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fem_map_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fem_map_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_fem_map_index_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_fem_map_index_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fem_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fem_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_fem_programs_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_fem_programs_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_fem_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fem_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_fem_id_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_fem_id_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_array_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_array_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_dbal_pairs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_dbal_pairs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_common_max_val_array_id_type_set(
    int unit)
{
    int array_id_index;
    dnx_data_field_common_max_val_array_id_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int table_index = dnx_data_field_common_max_val_table_array_id_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_field.common_max_val.nof_array_ids_get(unit);
    table->info_get.key_size[0] = dnx_data_field.common_max_val.nof_array_ids_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_common_max_val_array_id_type_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_common_max_val_table_array_id_type");

    
    default_data = (dnx_data_field_common_max_val_array_id_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_fes = 0;
    default_data->is_fem = 0;
    default_data->fes_array_index = 0;
    default_data->fem_array_index = 0;
    
    for (array_id_index = 0; array_id_index < table->keys[0].size; array_id_index++)
    {
        data = (dnx_data_field_common_max_val_array_id_type_t *) dnxc_data_mgmt_table_data_get(unit, table, array_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_field_common_max_val_array_id_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->is_fes = 1;
        data->is_fem = 0;
        data->fes_array_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_field_common_max_val_array_id_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_fes = 0;
        data->is_fem = 1;
        data->fem_array_index = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_field_common_max_val_array_id_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->is_fes = 1;
        data->is_fem = 0;
        data->fes_array_index = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_field_common_max_val_array_id_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->is_fes = 0;
        data->is_fem = 1;
        data->fem_array_index = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_init_fec_dest_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_fec_dest;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_l4_trap_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_l4_trap;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_oam_layer_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_oam_layer_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_oam_stat_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_oam_stat;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_roo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_roo;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_disable_erpp_counters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_disable_erpp_counters;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_jr1_ipmc_inlif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_jr1_ipmc_inlif;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_j1_same_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_j1_same_port;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_j1_learning_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_j1_learning;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_learn_limit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_learn_limit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_j1_php_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_j1_php;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_ftmh_mc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_ftmh_mc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_init_flow_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_init;
    int define_index = dnx_data_field_init_define_flow_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_features_tcam_cs_inlif_profile_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_tcam_cs_inlif_profile_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_tcam_cs_is_tcam_direct_access_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_tcam_cs_is_tcam_direct_access;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_ecc_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_ecc_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_switch_to_acl_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_switch_to_acl_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_per_pp_port_pmf_profile_cs_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_per_pp_port_pmf_profile_cs_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_tcam_result_flip_eco_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_tcam_result_flip_eco;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_tcam_result_half_payload_on_msb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_tcam_result_half_payload_on_msb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_parsing_start_offset_msb_meaningless_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_parsing_start_offset_msb_meaningless;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_kbp_opcode_in_ipmf1_cs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_kbp_opcode_in_ipmf1_cs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_kbp_hitbits_correct_in_ipmf1_cs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_kbp_hitbits_correct_in_ipmf1_cs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_exem_vmv_removable_from_payload_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_exem_vmv_removable_from_payload;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_multiple_dynamic_mem_enablers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_multiple_dynamic_mem_enablers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_aacl_super_group_handler_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_aacl_super_group_handler_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_aacl_tcam_swap_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_aacl_tcam_swap_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_extended_l4_ops_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_extended_l4_ops;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_state_table_ipmf1_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_state_table_ipmf1_key_select;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_state_table_acr_bus_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_state_table_acr_bus;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_state_table_atomic_rmw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_state_table_atomic_rmw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_hitbit_volatile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_hitbit_volatile;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_fem_replace_msb_instead_lsb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_fem_replace_msb_instead_lsb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_dir_ext_epmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_dir_ext_epmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_hashing_process_lsb_to_msb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_hashing_process_lsb_to_msb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_exem_age_flush_scan_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_exem_age_flush_scan;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_epmf_has_lookup_enabler_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_epmf_has_lookup_enabler;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_epmf_outlif_profile_map_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_epmf_outlif_profile_map;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_epmf_inlif_profile_map_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_epmf_inlif_profile_map;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_tcam_full_key_half_payload_both_lsb_msb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_tcam_full_key_half_payload_both_lsb_msb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_ecmp_tunnel_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_ecmp_tunnel_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_result_signal_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_result_signal_exists;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_apptype_based_on_fwd2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_apptype_based_on_fwd2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_udh_base_cs_is_mapped_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_udh_base_cs_is_mapped;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_state_collection_engine_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_state_collection_engine;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_features_empf_trap_action_is_strength_based_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_empf_trap_action_is_strength_based;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_signal_sizes_dual_queue_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_signal_sizes;
    int define_index = dnx_data_field_signal_sizes_define_dual_queue_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_signal_sizes_packet_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_signal_sizes;
    int define_index = dnx_data_field_signal_sizes_define_packet_header_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_signal_sizes_ecn_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_signal_sizes;
    int define_index = dnx_data_field_signal_sizes_define_ecn;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_signal_sizes_congestion_info_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_signal_sizes;
    int define_index = dnx_data_field_signal_sizes_define_congestion_info;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.signal_sizes.ecn_get(unit);

    
    define->data = dnx_data_field.signal_sizes.ecn_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_signal_sizes_parsing_start_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_signal_sizes;
    int define_index = dnx_data_field_signal_sizes_define_parsing_start_offset_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_dnx_data_internal_lr_eth_is_da_mac_valid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_dnx_data_internal;
    int define_index = dnx_data_field_dnx_data_internal_define_lr_eth_is_da_mac_valid;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_field_tests_learn_info_actions_structure_change_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_learn_info_actions_structure_change;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tests_ingress_time_stamp_increased_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_ingress_time_stamp_increased;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tests_ace_debug_signals_exist_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_ace_debug_signals_exist;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tests_epmf_debug_rely_on_tcam0_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_epmf_debug_rely_on_tcam0;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tests_epmf_tm_port_cs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_epmf_tm_port_cs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tests_small_exem_addr_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_small_exem_addr_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tests_stub_adapter_supports_semantic_flush_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_stub_adapter_supports_semantic_flush;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_tests_parsing_start_offset_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tests;
    int define_index = dnx_data_field_tests_define_parsing_start_offset_exists;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_field_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_field;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_field_submodule_base_ipmf1;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_ipmf1_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_ffc_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_keys_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_dir_ext;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_dir_ext_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_keys_alloc_for_mdb_dt_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_masks_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_masks_per_fes_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fes_id_per_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fes_id_per_array_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fes_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fes_array_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fes_instruction_per_context_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fes_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fes_programs_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_prog_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_prog_per_fes_set;
    data_index = dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_cs_container_5_selected_bits_size_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_contexts_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_link_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_link_profiles_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_cs_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_cs_lines_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_actions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_actions_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_qualifiers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_qualifiers_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_key_select_resolution_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_key_select_resolution_in_bits_set;
    data_index = dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_programs_set;
    data_index = dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_log_nof_bits_in_fem_key_select_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_key_select_set;
    data_index = dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fem_key_select_resolution_in_bits_set;
    data_index = dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_log_nof_bits_in_fem_map_data_field_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_map_data_field_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_2_15_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_0_1_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_action_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_condition;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_condition_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_map_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_map_index_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_id_set;
    data_index = dnx_data_field_base_ipmf1_define_num_fems_with_short_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_num_fems_with_short_action_set;
    data_index = dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_num_bits_in_fem_field_select_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_id_per_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_id_per_array_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_array_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_set;
    data_index = dnx_data_field_base_ipmf1_define_compare_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_compare_key_size_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_l4_ops_ranges_legacy_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_ext_l4_ops_ranges_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_pkt_hdr_ranges_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_out_lif_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_out_lif_ranges_set;
    data_index = dnx_data_field_base_ipmf1_define_cmp_selection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_cmp_selection_set;
    data_index = dnx_data_field_base_ipmf1_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_dir_ext_single_key_size_set;
    data_index = dnx_data_field_base_ipmf1_define_dir_ext_first_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_dir_ext_first_key_set;
    data_index = dnx_data_field_base_ipmf1_define_exem_max_result_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_exem_max_result_size_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_instruction_size_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_pgm_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_pgm_id_offset_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf1_define_exem_pdb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_exem_pdb_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_ipmf2;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_ipmf2_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_ffc_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_keys_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_tcam;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_tcam_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_exem;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_exem_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_dir_ext;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_dir_ext_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_mdb_dt;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_keys_alloc_for_mdb_dt_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_masks_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_masks_per_fes_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_fes_id_per_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_fes_id_per_array_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_fes_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_fes_array_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_fes_instruction_per_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_fes_instruction_per_context_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_cs_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_cs_lines_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_contexts_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_qualifiers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_qualifiers_set;
    data_index = dnx_data_field_base_ipmf2_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_dir_ext_single_key_size_set;
    data_index = dnx_data_field_base_ipmf2_define_dir_ext_first_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_dir_ext_first_key_set;
    data_index = dnx_data_field_base_ipmf2_define_exem_max_result_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_exem_max_result_size_set;
    data_index = dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf2_define_exem_pdb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_exem_pdb_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_ipmf3;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_ipmf3_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_ffc_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_keys_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_tcam;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_tcam_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_exem;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_exem_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_dir_ext;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_dir_ext_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_mdb_dt;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_keys_alloc_for_mdb_dt_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_masks_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_masks_per_fes_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_fes_id_per_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_fes_id_per_array_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_fes_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_fes_array_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_fes_instruction_per_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_fes_instruction_per_context_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_fes_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_fes_programs_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_prog_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_prog_per_fes_set;
    data_index = dnx_data_field_base_ipmf3_define_cs_scratch_pad_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_cs_scratch_pad_size_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_contexts_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_cs_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_cs_lines_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_actions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_actions_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_qualifiers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_qualifiers_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_key_select_resolution_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_key_select_resolution_in_bits_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_out_lif_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_out_lif_ranges_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_instruction_size_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_pgm_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_pgm_id_offset_set;
    data_index = dnx_data_field_base_ipmf3_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_dir_ext_single_key_size_set;
    data_index = dnx_data_field_base_ipmf3_define_dir_ext_first_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_dir_ext_first_key_set;
    data_index = dnx_data_field_base_ipmf3_define_exem_max_result_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_exem_max_result_size_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf3_define_exem_pdb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_exem_pdb_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_epmf;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_epmf_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_ffc_set;
    data_index = dnx_data_field_base_epmf_define_nof_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_keys_set;
    data_index = dnx_data_field_base_epmf_define_nof_keys_alloc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_set;
    data_index = dnx_data_field_base_epmf_define_nof_keys_alloc_for_tcam;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_for_tcam_set;
    data_index = dnx_data_field_base_epmf_define_nof_keys_alloc_for_exem;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_for_exem_set;
    data_index = dnx_data_field_base_epmf_define_nof_keys_alloc_for_dir_ext;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_keys_alloc_for_dir_ext_set;
    data_index = dnx_data_field_base_epmf_define_nof_masks_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_masks_per_fes_set;
    data_index = dnx_data_field_base_epmf_define_nof_fes_id_per_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_fes_id_per_array_set;
    data_index = dnx_data_field_base_epmf_define_nof_fes_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_fes_array_set;
    data_index = dnx_data_field_base_epmf_define_nof_fes_instruction_per_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_fes_instruction_per_context_set;
    data_index = dnx_data_field_base_epmf_define_nof_fes_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_fes_programs_set;
    data_index = dnx_data_field_base_epmf_define_nof_prog_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_prog_per_fes_set;
    data_index = dnx_data_field_base_epmf_define_nof_cs_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_cs_lines_set;
    data_index = dnx_data_field_base_epmf_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_contexts_set;
    data_index = dnx_data_field_base_epmf_define_nof_actions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_actions_set;
    data_index = dnx_data_field_base_epmf_define_nof_qualifiers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_qualifiers_set;
    data_index = dnx_data_field_base_epmf_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_base_epmf_define_fes_key_select_resolution_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_key_select_resolution_in_bits_set;
    data_index = dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_set;
    data_index = dnx_data_field_base_epmf_define_fes_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_instruction_size_set;
    data_index = dnx_data_field_base_epmf_define_fes_pgm_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_pgm_id_offset_set;
    data_index = dnx_data_field_base_epmf_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_dir_ext_single_key_size_set;
    data_index = dnx_data_field_base_epmf_define_dir_ext_first_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_dir_ext_first_key_set;
    data_index = dnx_data_field_base_epmf_define_exem_max_result_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_exem_max_result_size_set;
    data_index = dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_epmf_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_epmf_define_exem_pdb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_exem_pdb_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_ifwd2;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_ifwd2_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_nof_ffc_set;
    data_index = dnx_data_field_base_ifwd2_define_nof_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_nof_keys_set;
    data_index = dnx_data_field_base_ifwd2_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_nof_contexts_set;
    data_index = dnx_data_field_base_ifwd2_define_nof_cs_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_nof_cs_lines_set;

    

    
    
    submodule_index = dnx_data_field_submodule_stage;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_field_stage_table_stage_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_stage_stage_info_set;
    
    submodule_index = dnx_data_field_submodule_external_tcam;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_external_tcam_define_max_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_max_single_key_size_set;
    data_index = dnx_data_field_external_tcam_define_nof_acl_keys_master_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_nof_acl_keys_master_max_set;
    data_index = dnx_data_field_external_tcam_define_nof_acl_keys_fg_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_nof_acl_keys_fg_max_set;
    data_index = dnx_data_field_external_tcam_define_nof_keys_total;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_nof_keys_total_set;
    data_index = dnx_data_field_external_tcam_define_min_acl_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_min_acl_nof_ffc_set;
    data_index = dnx_data_field_external_tcam_define_max_fwd_context_num_for_one_apptype;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_max_fwd_context_num_for_one_apptype_set;
    data_index = dnx_data_field_external_tcam_define_max_acl_context_num;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_max_acl_context_num_set;
    data_index = dnx_data_field_external_tcam_define_size_apptype_profile_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_size_apptype_profile_id_set;
    data_index = dnx_data_field_external_tcam_define_key_bmp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_key_bmp_set;
    data_index = dnx_data_field_external_tcam_define_apptype_user_1st;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_apptype_user_1st_set;
    data_index = dnx_data_field_external_tcam_define_apptype_user_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_apptype_user_nof_set;
    data_index = dnx_data_field_external_tcam_define_max_payload_size_per_opcode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_max_payload_size_per_opcode_set;
    data_index = dnx_data_field_external_tcam_define_nof_total_lookups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_nof_total_lookups_set;
    data_index = dnx_data_field_external_tcam_define_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_external_tcam_type_set;

    
    data_index = dnx_data_field_external_tcam_has_acl_context_profile_mapping;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_external_tcam_has_acl_context_profile_mapping_set;

    
    data_index = dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_external_tcam_ffc_to_quad_and_group_map_set;
    
    submodule_index = dnx_data_field_submodule_tcam;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_tcam_define_access_hw_mix_ratio_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_access_hw_mix_ratio_supported_set;
    data_index = dnx_data_field_tcam_define_direct_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_direct_supported_set;
    data_index = dnx_data_field_tcam_define_key_size_half;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_key_size_half_set;
    data_index = dnx_data_field_tcam_define_key_size_single;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_key_size_single_set;
    data_index = dnx_data_field_tcam_define_key_size_double;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_key_size_double_set;
    data_index = dnx_data_field_tcam_define_dt_max_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_dt_max_key_size_set;
    data_index = dnx_data_field_tcam_define_dt_max_key_size_small_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_dt_max_key_size_small_bank_set;
    data_index = dnx_data_field_tcam_define_action_size_half;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_action_size_half_set;
    data_index = dnx_data_field_tcam_define_action_size_single;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_action_size_single_set;
    data_index = dnx_data_field_tcam_define_action_size_double;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_action_size_double_set;
    data_index = dnx_data_field_tcam_define_key_mode_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_key_mode_size_set;
    data_index = dnx_data_field_tcam_define_entry_size_single_key_hw;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_entry_size_single_key_hw_set;
    data_index = dnx_data_field_tcam_define_entry_size_single_key_shadow;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_entry_size_single_key_shadow_set;
    data_index = dnx_data_field_tcam_define_entry_size_single_valid_bits_hw;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_entry_size_single_valid_bits_hw_set;
    data_index = dnx_data_field_tcam_define_hw_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_hw_bank_size_set;
    data_index = dnx_data_field_tcam_define_nof_big_bank_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_big_bank_lines_set;
    data_index = dnx_data_field_tcam_define_nof_small_bank_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_small_bank_lines_set;
    data_index = dnx_data_field_tcam_define_nof_big_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_big_banks_set;
    data_index = dnx_data_field_tcam_define_nof_small_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_small_banks_set;
    data_index = dnx_data_field_tcam_define_nof_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_banks_set;
    data_index = dnx_data_field_tcam_define_nof_payload_tables;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_payload_tables_set;
    data_index = dnx_data_field_tcam_define_nof_access_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_access_profiles_set;
    data_index = dnx_data_field_tcam_define_action_width_selector_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_action_width_selector_size_set;
    data_index = dnx_data_field_tcam_define_nof_entries_160_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_entries_160_bits_set;
    data_index = dnx_data_field_tcam_define_nof_entries_80_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_entries_80_bits_set;
    data_index = dnx_data_field_tcam_define_tcam_banks_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_tcam_banks_size_set;
    data_index = dnx_data_field_tcam_define_tcam_banks_last_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_tcam_banks_last_index_set;
    data_index = dnx_data_field_tcam_define_nof_tcam_handlers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_tcam_handlers_set;
    data_index = dnx_data_field_tcam_define_max_prefix_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_max_prefix_size_set;
    data_index = dnx_data_field_tcam_define_max_prefix_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_max_prefix_value_set;
    data_index = dnx_data_field_tcam_define_nof_keys_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_keys_max_set;
    data_index = dnx_data_field_tcam_define_access_profile_half_key_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_access_profile_half_key_mode_set;
    data_index = dnx_data_field_tcam_define_access_profile_single_key_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_access_profile_single_key_mode_set;
    data_index = dnx_data_field_tcam_define_access_profile_double_key_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_access_profile_double_key_mode_set;
    data_index = dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_tcam_entries_per_hit_indication_entry_set;
    data_index = dnx_data_field_tcam_define_max_tcam_priority;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_max_tcam_priority_set;
    data_index = dnx_data_field_tcam_define_nof_big_banks_srams;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_big_banks_srams_set;
    data_index = dnx_data_field_tcam_define_nof_small_banks_srams;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_small_banks_srams_set;
    data_index = dnx_data_field_tcam_define_nof_big_bank_lines_per_sram;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_big_bank_lines_per_sram_set;
    data_index = dnx_data_field_tcam_define_nof_small_bank_lines_per_sram;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_small_bank_lines_per_sram_set;
    data_index = dnx_data_field_tcam_define_app_db_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_app_db_id_size_set;
    data_index = dnx_data_field_tcam_define_tcam_indirect_command_version;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_tcam_indirect_command_version_set;

    
    data_index = dnx_data_field_tcam_tcam_80b_traffic_safe_write;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_tcam_tcam_80b_traffic_safe_write_set;
    data_index = dnx_data_field_tcam_mixed_ratio_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_tcam_mixed_ratio_supported_set;

    
    
    submodule_index = dnx_data_field_submodule_group;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_group_define_nof_fgs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_group_nof_fgs_set;
    data_index = dnx_data_field_group_define_nof_action_per_fg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_group_nof_action_per_fg_set;
    data_index = dnx_data_field_group_define_nof_quals_per_fg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_group_nof_quals_per_fg_set;
    data_index = dnx_data_field_group_define_nof_keys_per_fg_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_group_nof_keys_per_fg_max_set;
    data_index = dnx_data_field_group_define_id_fec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_group_id_fec_set;
    data_index = dnx_data_field_group_define_payload_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_group_payload_max_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_efes;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_efes_define_max_nof_key_selects_per_field_io;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_efes_max_nof_key_selects_per_field_io_set;
    data_index = dnx_data_field_efes_define_max_nof_field_ios;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_efes_max_nof_field_ios_set;
    data_index = dnx_data_field_efes_define_epmf_exem_zero_padding_added_to_field_io;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_efes_epmf_exem_zero_padding_added_to_field_io_set;

    

    
    data_index = dnx_data_field_efes_table_key_select_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_efes_key_select_properties_set;
    
    submodule_index = dnx_data_field_submodule_fem;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_fem_define_max_nof_key_selects_per_field_io;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_fem_max_nof_key_selects_per_field_io_set;
    data_index = dnx_data_field_fem_define_nof_condition_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_fem_nof_condition_bits_set;

    

    
    data_index = dnx_data_field_fem_table_key_select_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_fem_key_select_properties_set;
    
    submodule_index = dnx_data_field_submodule_context;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_context_define_default_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_context_set;
    data_index = dnx_data_field_context_define_default_ftmh_mc_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_ftmh_mc_context_set;
    data_index = dnx_data_field_context_define_default_itmh_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_context_set;
    data_index = dnx_data_field_context_define_default_itmh_pph_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_pph_context_set;
    data_index = dnx_data_field_context_define_default_itmh_pph_fhei_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_pph_fhei_context_set;
    data_index = dnx_data_field_context_define_default_itmh_1588_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_1588_context_set;
    data_index = dnx_data_field_context_define_default_itmh_1588_pph_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_1588_pph_context_set;
    data_index = dnx_data_field_context_define_default_itmh_1588_pph_fhei_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_1588_pph_fhei_context_set;
    data_index = dnx_data_field_context_define_default_j1_itmh_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_j1_itmh_context_set;
    data_index = dnx_data_field_context_define_default_j1_itmh_pph_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_j1_itmh_pph_context_set;
    data_index = dnx_data_field_context_define_default_oam_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_oam_context_set;
    data_index = dnx_data_field_context_define_default_oam_reflector_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_oam_reflector_context_set;
    data_index = dnx_data_field_context_define_default_oam_upmep_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_oam_upmep_context_set;
    data_index = dnx_data_field_context_define_default_j1_learning_2ndpass_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_j1_learning_2ndpass_context_set;
    data_index = dnx_data_field_context_define_default_rch_remove_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_rch_remove_context_set;
    data_index = dnx_data_field_context_define_default_rch_extended_encap_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_rch_extended_encap_context_set;
    data_index = dnx_data_field_context_define_default_nat_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_nat_context_set;
    data_index = dnx_data_field_context_define_nof_hash_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_nof_hash_keys_set;
    data_index = dnx_data_field_context_define_default_itmh_pph_unsupported_ext_ipmf2_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_pph_unsupported_ext_ipmf2_context_set;
    data_index = dnx_data_field_context_define_default_itmh_pph_fhei_unsupported_ext_ipmf2_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_pph_fhei_unsupported_ext_ipmf2_context_set;
    data_index = dnx_data_field_context_define_default_itmh_pph_fhei_vlan_ipmf2_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_pph_fhei_vlan_ipmf2_context_set;
    data_index = dnx_data_field_context_define_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_set;
    data_index = dnx_data_field_context_define_default_itmh_1588_pph_unsupported_ext_ipmf2_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_1588_pph_unsupported_ext_ipmf2_context_set;
    data_index = dnx_data_field_context_define_default_itmh_1588_pph_fhei_vlan_ipmf2_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_1588_pph_fhei_vlan_ipmf2_context_set;

    

    
    
    submodule_index = dnx_data_field_submodule_preselector;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_oamp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_nat_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_nat_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usd_eth_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usd_eth_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usd_ipv4_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usd_ipv4_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usd_ipv6_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usd_ipv6_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usd_mpls_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usd_mpls_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usp_trap_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usp_trap_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usp_fwd_trap_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usp_fwd_trap_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usp_icmp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usp_icmp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_mpls_bier_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_rcy_mirror_sys_hdr_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_rcy_mirror_sys_hdr_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_unsupported_ext_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_unsupported_ext_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_trap_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_trap_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_fhei_vlan_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_fhei_vlan_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_trap_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_trap_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_vlan_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_vlan_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_trap_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_trap_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_itmh_1588_pph_vlan_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_1588_pph_vlan_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_oam_roo_ipv4_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_oam_roo_ipv6_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_ipv4_mc_in_lif_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_ipv6_mc_in_lif_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_rch_remove_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_rch_extended_encap_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_rch_extended_encap_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_partial_fwd_layer_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_partial_fwd_layer_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_psp_ext_to_endpoint_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_psp_ext_to_endpoint_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_srv6_egress_usd_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_egress_usd_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_mpls_bier_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_ebtr_const_fwd_layer_eth_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_ebtr_const_fwd_layer_eth_epmf_set;
    data_index = dnx_data_field_preselector_define_srv6_ext_termination_btr_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_ext_termination_btr_epmf_set;
    data_index = dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_set;
    data_index = dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_learn_limit_presel_id_epmf_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_hbh_bta_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_hbh_bta_presel_id_epmf_set;
    data_index = dnx_data_field_preselector_define_srv6_transit_hbh_bta_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_transit_hbh_bta_presel_id_epmf_set;
    data_index = dnx_data_field_preselector_define_srv6_endpoint_psp_bta_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_srv6_endpoint_psp_bta_presel_id_epmf_set;
    data_index = dnx_data_field_preselector_define_default_mpls_8b_fhei_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_mpls_8b_fhei_presel_id_epmf_set;
    data_index = dnx_data_field_preselector_define_num_cs_inlif_profile_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_num_cs_inlif_profile_entries_set;

    

    
    
    submodule_index = dnx_data_field_submodule_qual;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_qual_define_user_1st;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_user_1st_set;
    data_index = dnx_data_field_qual_define_user_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_user_nof_set;
    data_index = dnx_data_field_qual_define_predefined_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_predefined_nof_set;
    data_index = dnx_data_field_qual_define_layer_record_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_layer_record_nof_set;
    data_index = dnx_data_field_qual_define_vw_1st;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_vw_1st_set;
    data_index = dnx_data_field_qual_define_vw_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_vw_nof_set;
    data_index = dnx_data_field_qual_define_max_bits_in_qual;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_max_bits_in_qual_set;
    data_index = dnx_data_field_qual_define_ingress_pbus_header_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ingress_pbus_header_length_set;
    data_index = dnx_data_field_qual_define_egress_pbus_header_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_egress_pbus_header_length_set;
    data_index = dnx_data_field_qual_define_ifwd2_pbus_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ifwd2_pbus_size_set;
    data_index = dnx_data_field_qual_define_ipmf1_pbus_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ipmf1_pbus_size_set;
    data_index = dnx_data_field_qual_define_ipmf2_pbus_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ipmf2_pbus_size_set;
    data_index = dnx_data_field_qual_define_ipmf3_pbus_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ipmf3_pbus_size_set;
    data_index = dnx_data_field_qual_define_epmf_pbus_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_epmf_pbus_size_set;
    data_index = dnx_data_field_qual_define_l4_ops_pbus_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_l4_ops_pbus_size_set;
    data_index = dnx_data_field_qual_define_ingress_nof_layer_records;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ingress_nof_layer_records_set;
    data_index = dnx_data_field_qual_define_ingress_layer_record_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ingress_layer_record_size_set;
    data_index = dnx_data_field_qual_define_egress_nof_layer_records;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_egress_nof_layer_records_set;
    data_index = dnx_data_field_qual_define_egress_layer_record_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_egress_layer_record_size_set;
    data_index = dnx_data_field_qual_define_kbp_extra_offset_after_layer_record_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_kbp_extra_offset_after_layer_record_offset_set;
    data_index = dnx_data_field_qual_define_special_metadata_qual_max_parts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_special_metadata_qual_max_parts_set;

    

    
    
    submodule_index = dnx_data_field_submodule_action;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_action_define_user_1st;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_action_user_1st_set;
    data_index = dnx_data_field_action_define_user_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_action_user_nof_set;
    data_index = dnx_data_field_action_define_predefined_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_action_predefined_nof_set;
    data_index = dnx_data_field_action_define_vw_1st;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_action_vw_1st_set;
    data_index = dnx_data_field_action_define_vw_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_action_vw_nof_set;

    

    
    
    submodule_index = dnx_data_field_submodule_virtual_wire;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_virtual_wire_define_signals_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_virtual_wire_signals_nof_set;
    data_index = dnx_data_field_virtual_wire_define_actions_per_signal_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_virtual_wire_actions_per_signal_nof_set;
    data_index = dnx_data_field_virtual_wire_define_general_data_user_general_containers_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_virtual_wire_general_data_user_general_containers_size_set;
    data_index = dnx_data_field_virtual_wire_define_ipmf1_general_data_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_virtual_wire_ipmf1_general_data_index_set;

    
    data_index = dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw_set;

    
    data_index = dnx_data_field_virtual_wire_table_signal_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_virtual_wire_signal_mapping_set;
    
    submodule_index = dnx_data_field_submodule_profile_bits;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_set;
    data_index = dnx_data_field_profile_bits_define_max_port_profile_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_max_port_profile_size_set;
    data_index = dnx_data_field_profile_bits_define_nof_bits_in_port_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_bits_in_port_profile_set;
    data_index = dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_set;
    data_index = dnx_data_field_profile_bits_define_nof_ing_in_lif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_ing_in_lif_set;
    data_index = dnx_data_field_profile_bits_define_nof_ing_eth_rif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_ing_eth_rif_set;

    

    
    
    submodule_index = dnx_data_field_submodule_dir_ext;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_dir_ext_define_half_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_dir_ext_half_key_size_set;
    data_index = dnx_data_field_dir_ext_define_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_dir_ext_single_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_state_table;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_state_table_define_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_supported_set;
    data_index = dnx_data_field_state_table_define_address_size_entry_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_address_size_entry_max_set;
    data_index = dnx_data_field_state_table_define_data_size_entry_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_data_size_entry_max_set;
    data_index = dnx_data_field_state_table_define_address_max_entry_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_address_max_entry_max_set;
    data_index = dnx_data_field_state_table_define_wr_bit_size_rw;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_wr_bit_size_rw_set;
    data_index = dnx_data_field_state_table_define_ipmf2_key_select_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_ipmf2_key_select_supported_set;
    data_index = dnx_data_field_state_table_define_data_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_data_size_set;
    data_index = dnx_data_field_state_table_define_state_table_stage_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_state_table_stage_key_set;
    data_index = dnx_data_field_state_table_define_address_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_address_size_set;
    data_index = dnx_data_field_state_table_define_address_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_address_max_set;
    data_index = dnx_data_field_state_table_define_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_state_table_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_mdb_dt;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_mdb_dt_define_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_supported_set;
    data_index = dnx_data_field_mdb_dt_define_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_key_size_set;
    data_index = dnx_data_field_mdb_dt_define_action_size_large;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_action_size_large_set;
    data_index = dnx_data_field_mdb_dt_define_ipmf1_key_select_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_ipmf1_key_select_supported_set;
    data_index = dnx_data_field_mdb_dt_define_ipmf1_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_ipmf1_key_select_set;
    data_index = dnx_data_field_mdb_dt_define_ipmf2_key_select_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_ipmf2_key_select_supported_set;
    data_index = dnx_data_field_mdb_dt_define_ipmf2_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_ipmf2_key_select_set;
    data_index = dnx_data_field_mdb_dt_define_ipmf3_key_select_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_ipmf3_key_select_supported_set;
    data_index = dnx_data_field_mdb_dt_define_ipmf3_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_ipmf3_key_select_set;
    data_index = dnx_data_field_mdb_dt_define_pmf_map_stage;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_mdb_dt_pmf_map_stage_set;

    

    
    
    submodule_index = dnx_data_field_submodule_hash;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_hash_define_max_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_hash_max_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_udh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_udh_define_ext_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_ext_supported_set;
    data_index = dnx_data_field_udh_define_type_count;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_type_count_set;

    if (soc_is(unit, DNX2_DEVICE))
    {
        data_index = dnx_data_field_udh_define_type_0_length;
        define = &submodule->defines[data_index];
        define->set = jr2_a0_dnx_data_field_udh_type_0_length_set;
    }


    if (soc_is(unit, DNX2_DEVICE))
    {
        data_index = dnx_data_field_udh_define_type_1_length;
        define = &submodule->defines[data_index];
        define->set = jr2_a0_dnx_data_field_udh_type_1_length_set;
    }


    if (soc_is(unit, DNX2_DEVICE))
    {
        data_index = dnx_data_field_udh_define_type_2_length;
        define = &submodule->defines[data_index];
        define->set = jr2_a0_dnx_data_field_udh_type_2_length_set;
    }


    if (soc_is(unit, DNX2_DEVICE))
    {
        data_index = dnx_data_field_udh_define_type_3_length;
        define = &submodule->defines[data_index];
        define->set = jr2_a0_dnx_data_field_udh_type_3_length_set;
    }

    data_index = dnx_data_field_udh_define_field_class_id_size_0;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_field_class_id_size_0_set;
    data_index = dnx_data_field_udh_define_field_class_id_size_1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_field_class_id_size_1_set;
    data_index = dnx_data_field_udh_define_field_class_id_size_2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_field_class_id_size_2_set;
    data_index = dnx_data_field_udh_define_field_class_id_size_3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_field_class_id_size_3_set;
    data_index = dnx_data_field_udh_define_field_class_id_total_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_field_class_id_total_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_system_headers;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_system_headers_define_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_system_headers_nof_profiles_set;
    data_index = dnx_data_field_system_headers_define_pph_learn_ext_disable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_system_headers_pph_learn_ext_disable_set;

    

    
    data_index = dnx_data_field_system_headers_table_system_header_profiles;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_system_headers_system_header_profiles_set;
    
    submodule_index = dnx_data_field_submodule_exem;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_exem_define_small_app_db_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_app_db_id_size_set;
    data_index = dnx_data_field_exem_define_large_app_db_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_app_db_id_size_set;
    data_index = dnx_data_field_exem_define_small_max_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_max_key_size_set;
    data_index = dnx_data_field_exem_define_large_max_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_max_key_size_set;
    data_index = dnx_data_field_exem_define_max_result_size_all_stages;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_max_result_size_all_stages_set;
    data_index = dnx_data_field_exem_define_small_max_container_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_max_container_size_set;
    data_index = dnx_data_field_exem_define_large_max_container_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_max_container_size_set;
    data_index = dnx_data_field_exem_define_vmv_hw_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_vmv_hw_max_size_set;
    data_index = dnx_data_field_exem_define_small_min_app_db_id_range;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_min_app_db_id_range_set;
    data_index = dnx_data_field_exem_define_large_min_app_db_id_range;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_min_app_db_id_range_set;
    data_index = dnx_data_field_exem_define_small_max_app_db_id_range;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_max_app_db_id_range_set;
    data_index = dnx_data_field_exem_define_large_max_app_db_id_range;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_max_app_db_id_range_set;
    data_index = dnx_data_field_exem_define_small_ipmf2_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_ipmf2_key_set;
    data_index = dnx_data_field_exem_define_small_ipmf2_key_part;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_ipmf2_key_part_set;
    data_index = dnx_data_field_exem_define_small_ipmf2_key_hw_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_ipmf2_key_hw_value_set;
    data_index = dnx_data_field_exem_define_small_ipmf2_key_hw_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_ipmf2_key_hw_bits_set;
    data_index = dnx_data_field_exem_define_ipmf1_key_configurable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_ipmf1_key_configurable_set;
    data_index = dnx_data_field_exem_define_ipmf2_key_configurable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_ipmf2_key_configurable_set;
    data_index = dnx_data_field_exem_define_ipmf1_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_ipmf1_key_set;

    if (soc_is(unit, DNX2_DEVICE))
    {
        data_index = dnx_data_field_exem_define_pmf_sexem3_stage;
        define = &submodule->defines[data_index];
        define->set = jr2_a0_dnx_data_field_exem_pmf_sexem3_stage_set;
    }


    
    data_index = dnx_data_field_exem_exem_vmv_padded_for_2msb;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_exem_exem_vmv_padded_for_2msb_set;

    
    
    submodule_index = dnx_data_field_submodule_exem_learn_flush_machine;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_exem_learn_flush_machine_define_flush_machine_nof_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_flush_machine_nof_entries_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_entry_extra_bits_for_hash;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_entry_extra_bits_for_hash_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_sexem_entry_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_sexem_entry_max_size_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_lexem_entry_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_lexem_entry_max_size_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_command_bit_transplant;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_command_bit_transplant_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_command_bit_delete;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_command_bit_delete_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_command_bit_clear_hit_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_command_bit_clear_hit_bit_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_nof_bits_source;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_nof_bits_source_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_source_bit_for_scan;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_source_bit_for_scan_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_source_bit_for_pipe;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_source_bit_for_pipe_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_source_bit_for_mrq;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_source_bit_for_mrq_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_nof_bits_accessed;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_nof_bits_accessed_set;
    data_index = dnx_data_field_exem_learn_flush_machine_define_nof_flush_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_nof_flush_profiles_set;

    
    data_index = dnx_data_field_exem_learn_flush_machine_has_elephant_signals;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_has_elephant_signals_set;

    
    data_index = dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_set;
    data_index = dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_set;
    data_index = dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_set;
    data_index = dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_set;
    
    submodule_index = dnx_data_field_submodule_ace;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_ace_define_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_supported_set;
    data_index = dnx_data_field_ace_define_ace_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_ace_id_size_set;
    data_index = dnx_data_field_ace_define_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_key_size_set;
    data_index = dnx_data_field_ace_define_app_db_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_app_db_id_size_set;
    data_index = dnx_data_field_ace_define_payload_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_payload_size_set;
    data_index = dnx_data_field_ace_define_min_key_range_pmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_min_key_range_pmf_set;
    data_index = dnx_data_field_ace_define_max_key_range_pmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_max_key_range_pmf_set;
    data_index = dnx_data_field_ace_define_min_ace_id_dynamic_range;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_min_ace_id_dynamic_range_set;
    data_index = dnx_data_field_ace_define_nof_ace_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_ace_id_set;
    data_index = dnx_data_field_ace_define_nof_action_per_ace_format;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_action_per_ace_format_set;
    data_index = dnx_data_field_ace_define_row_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_row_size_set;
    data_index = dnx_data_field_ace_define_min_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_min_entry_size_set;
    data_index = dnx_data_field_ace_define_ace_id_pmf_alloc_first;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_ace_id_pmf_alloc_first_set;
    data_index = dnx_data_field_ace_define_ace_id_pmf_alloc_last;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_ace_id_pmf_alloc_last_set;
    data_index = dnx_data_field_ace_define_nof_masks_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_masks_per_fes_set;
    data_index = dnx_data_field_ace_define_nof_fes_id_per_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_fes_id_per_array_set;
    data_index = dnx_data_field_ace_define_nof_fes_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_fes_array_set;
    data_index = dnx_data_field_ace_define_nof_fes_instruction_per_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_fes_instruction_per_context_set;
    data_index = dnx_data_field_ace_define_nof_fes_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_fes_programs_set;
    data_index = dnx_data_field_ace_define_nof_prog_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_prog_per_fes_set;
    data_index = dnx_data_field_ace_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_ace_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_ace_define_fes_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_fes_instruction_size_set;
    data_index = dnx_data_field_ace_define_fes_shift_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_fes_shift_offset_set;
    data_index = dnx_data_field_ace_define_fes_invalid_bits_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_fes_invalid_bits_offset_set;
    data_index = dnx_data_field_ace_define_fes_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_fes_type_offset_set;
    data_index = dnx_data_field_ace_define_fes_ace_action_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_fes_ace_action_offset_set;
    data_index = dnx_data_field_ace_define_fes_chosen_mask_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_fes_chosen_mask_offset_set;

    

    
    
    submodule_index = dnx_data_field_submodule_entry;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_entry_define_dir_ext_nof_fields;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_entry_dir_ext_nof_fields_set;
    data_index = dnx_data_field_entry_define_nof_action_params_per_entry;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_entry_nof_action_params_per_entry_set;
    data_index = dnx_data_field_entry_define_nof_qual_params_per_entry;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_entry_nof_qual_params_per_entry_set;

    

    
    
    submodule_index = dnx_data_field_submodule_L4_Ops;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_L4_Ops_define_udp_position;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_L4_Ops_udp_position_set;
    data_index = dnx_data_field_L4_Ops_define_tcp_position;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_L4_Ops_tcp_position_set;
    data_index = dnx_data_field_L4_Ops_define_nof_range_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_L4_Ops_nof_range_entries_set;
    data_index = dnx_data_field_L4_Ops_define_nof_ext_encoders;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_L4_Ops_nof_ext_encoders_set;
    data_index = dnx_data_field_L4_Ops_define_nof_ext_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_L4_Ops_nof_ext_types_set;
    data_index = dnx_data_field_L4_Ops_define_nof_configurable_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_L4_Ops_nof_configurable_ranges_set;

    

    
    
    submodule_index = dnx_data_field_submodule_encoded_qual_actions_offset;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_encoded_qual_actions_offset_define_trap_strength_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_encoded_qual_actions_offset_trap_strength_offset_set;
    data_index = dnx_data_field_encoded_qual_actions_offset_define_trap_qualifier_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_encoded_qual_actions_offset_trap_qualifier_offset_set;
    data_index = dnx_data_field_encoded_qual_actions_offset_define_sniff_qualifier_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_encoded_qual_actions_offset_sniff_qualifier_offset_set;
    data_index = dnx_data_field_encoded_qual_actions_offset_define_mirror_qualifier_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_encoded_qual_actions_offset_mirror_qualifier_offset_set;
    data_index = dnx_data_field_encoded_qual_actions_offset_define_forward_trap_qualifier_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_encoded_qual_actions_offset_forward_trap_qualifier_offset_set;

    

    
    
    submodule_index = dnx_data_field_submodule_compare;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_compare_define_equal_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_equal_offset_set;
    data_index = dnx_data_field_compare_define_not_equal_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_not_equal_offset_set;
    data_index = dnx_data_field_compare_define_smaller_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_smaller_offset_set;
    data_index = dnx_data_field_compare_define_not_smaller_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_not_smaller_offset_set;
    data_index = dnx_data_field_compare_define_bigger_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_bigger_offset_set;
    data_index = dnx_data_field_compare_define_not_bigger_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_not_bigger_offset_set;
    data_index = dnx_data_field_compare_define_nof_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_nof_keys_set;
    data_index = dnx_data_field_compare_define_nof_compare_pairs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_compare_nof_compare_pairs_set;

    

    
    
    submodule_index = dnx_data_field_submodule_diag;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_diag_define_bytes_to_remove_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_bytes_to_remove_mask_set;
    data_index = dnx_data_field_diag_define_layers_to_remove_size_in_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_layers_to_remove_size_in_bit_set;
    data_index = dnx_data_field_diag_define_layers_to_remove_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_layers_to_remove_mask_set;
    data_index = dnx_data_field_diag_define_nof_signals_per_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_nof_signals_per_action_set;
    data_index = dnx_data_field_diag_define_nof_signals_per_qualifier;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_nof_signals_per_qualifier_set;
    data_index = dnx_data_field_diag_define_key_signal_size_in_bytes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_key_signal_size_in_bytes_set;
    data_index = dnx_data_field_diag_define_result_signal_size_in_words;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_result_signal_size_in_words_set;
    data_index = dnx_data_field_diag_define_dt_result_signal_size_in_words;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_dt_result_signal_size_in_words_set;
    data_index = dnx_data_field_diag_define_hit_signal_size_in_words;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_hit_signal_size_in_words_set;

    
    data_index = dnx_data_field_diag_tcam_cs_hit_bit_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_diag_tcam_cs_hit_bit_support_set;

    
    
    submodule_index = dnx_data_field_submodule_general_data_qualifiers;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_general_data_qualifiers_define_vw_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_vw_supported_set;
    data_index = dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_pwe_in_lif_wide_data_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_pwe_in_lif_wide_data_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_ip_tunnel_in_lif_wide_data_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_ip_tunnel_in_lif_wide_data_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_native_ac_in_lif_wide_data_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_native_ac_in_lif_wide_data_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_extended_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_extended_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_mact_entry_grouping_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_mact_entry_grouping_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_vw_vip_valid_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_vw_vip_valid_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_vw_vip_id_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_vw_vip_id_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_vw_member_reference_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_vw_member_reference_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_vw_pcc_hit_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_vw_pcc_hit_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_l3srcbind_hit_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_l3srcbind_hit_nof_bits_set;
    data_index = dnx_data_field_general_data_qualifiers_define_srv6_next_si;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_srv6_next_si_set;
    data_index = dnx_data_field_general_data_qualifiers_define_srv6_next_gsid;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_general_data_qualifiers_srv6_next_gsid_set;

    

    
    
    submodule_index = dnx_data_field_submodule_common_max_val;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_common_max_val_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_ffc_set;
    data_index = dnx_data_field_common_max_val_define_nof_masks_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_masks_per_fes_set;
    data_index = dnx_data_field_common_max_val_define_nof_fes_id_per_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fes_id_per_array_set;
    data_index = dnx_data_field_common_max_val_define_nof_fes_array;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fes_array_set;
    data_index = dnx_data_field_common_max_val_define_nof_fes_instruction_per_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fes_instruction_per_context_set;
    data_index = dnx_data_field_common_max_val_define_nof_fes_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fes_programs_set;
    data_index = dnx_data_field_common_max_val_define_nof_prog_per_fes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_prog_per_fes_set;
    data_index = dnx_data_field_common_max_val_define_nof_cs_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_cs_lines_set;
    data_index = dnx_data_field_common_max_val_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_contexts_set;
    data_index = dnx_data_field_common_max_val_define_nof_actions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_actions_set;
    data_index = dnx_data_field_common_max_val_define_nof_qualifiers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_qualifiers_set;
    data_index = dnx_data_field_common_max_val_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_common_max_val_define_nof_bits_in_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_bits_in_ffc_set;
    data_index = dnx_data_field_common_max_val_define_nof_ffc_in_uint32;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_ffc_in_uint32_set;
    data_index = dnx_data_field_common_max_val_define_nof_action_per_group;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_action_per_group_set;
    data_index = dnx_data_field_common_max_val_define_nof_layer_records;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_layer_records_set;
    data_index = dnx_data_field_common_max_val_define_layer_record_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_layer_record_size_set;
    data_index = dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_l4_ops_ranges_legacy_set;
    data_index = dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_pkt_hdr_ranges_set;
    data_index = dnx_data_field_common_max_val_define_nof_out_lif_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_out_lif_ranges_set;
    data_index = dnx_data_field_common_max_val_define_kbr_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_kbr_size_set;
    data_index = dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_set;
    data_index = dnx_data_field_common_max_val_define_nof_bits_in_fem_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fem_action_set;
    data_index = dnx_data_field_common_max_val_define_nof_fem_condition;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fem_condition_set;
    data_index = dnx_data_field_common_max_val_define_nof_fem_map_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fem_map_index_set;
    data_index = dnx_data_field_common_max_val_define_nof_fem_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fem_programs_set;
    data_index = dnx_data_field_common_max_val_define_nof_fem_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fem_id_set;
    data_index = dnx_data_field_common_max_val_define_nof_array_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_array_ids_set;
    data_index = dnx_data_field_common_max_val_define_dbal_pairs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_dbal_pairs_set;

    

    
    data_index = dnx_data_field_common_max_val_table_array_id_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_common_max_val_array_id_type_set;
    
    submodule_index = dnx_data_field_submodule_init;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_init_define_fec_dest;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_fec_dest_set;
    data_index = dnx_data_field_init_define_l4_trap;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_l4_trap_set;
    data_index = dnx_data_field_init_define_oam_layer_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_oam_layer_index_set;
    data_index = dnx_data_field_init_define_oam_stat;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_oam_stat_set;
    data_index = dnx_data_field_init_define_roo;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_roo_set;
    data_index = dnx_data_field_init_define_disable_erpp_counters;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_disable_erpp_counters_set;
    data_index = dnx_data_field_init_define_jr1_ipmc_inlif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_jr1_ipmc_inlif_set;
    data_index = dnx_data_field_init_define_j1_same_port;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_j1_same_port_set;
    data_index = dnx_data_field_init_define_j1_learning;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_j1_learning_set;
    data_index = dnx_data_field_init_define_learn_limit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_learn_limit_set;
    data_index = dnx_data_field_init_define_j1_php;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_j1_php_set;
    data_index = dnx_data_field_init_define_ftmh_mc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_ftmh_mc_set;
    data_index = dnx_data_field_init_define_flow_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_flow_id_set;

    

    
    
    submodule_index = dnx_data_field_submodule_features;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_features_define_tcam_cs_inlif_profile_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_tcam_cs_inlif_profile_supported_set;
    data_index = dnx_data_field_features_define_tcam_cs_is_tcam_direct_access;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_tcam_cs_is_tcam_direct_access_set;
    data_index = dnx_data_field_features_define_ecc_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_ecc_enable_set;
    data_index = dnx_data_field_features_define_switch_to_acl_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_switch_to_acl_context_set;
    data_index = dnx_data_field_features_define_per_pp_port_pmf_profile_cs_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_per_pp_port_pmf_profile_cs_offset_set;
    data_index = dnx_data_field_features_define_tcam_result_flip_eco;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_tcam_result_flip_eco_set;
    data_index = dnx_data_field_features_define_tcam_result_half_payload_on_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_tcam_result_half_payload_on_msb_set;
    data_index = dnx_data_field_features_define_parsing_start_offset_msb_meaningless;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_parsing_start_offset_msb_meaningless_set;
    data_index = dnx_data_field_features_define_kbp_opcode_in_ipmf1_cs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_kbp_opcode_in_ipmf1_cs_set;
    data_index = dnx_data_field_features_define_kbp_hitbits_correct_in_ipmf1_cs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_kbp_hitbits_correct_in_ipmf1_cs_set;
    data_index = dnx_data_field_features_define_exem_vmv_removable_from_payload;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_exem_vmv_removable_from_payload_set;
    data_index = dnx_data_field_features_define_multiple_dynamic_mem_enablers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_multiple_dynamic_mem_enablers_set;
    data_index = dnx_data_field_features_define_aacl_super_group_handler_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_aacl_super_group_handler_enable_set;
    data_index = dnx_data_field_features_define_aacl_tcam_swap_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_aacl_tcam_swap_enable_set;
    data_index = dnx_data_field_features_define_extended_l4_ops;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_extended_l4_ops_set;
    data_index = dnx_data_field_features_define_state_table_ipmf1_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_state_table_ipmf1_key_select_set;
    data_index = dnx_data_field_features_define_state_table_acr_bus;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_state_table_acr_bus_set;
    data_index = dnx_data_field_features_define_state_table_atomic_rmw;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_state_table_atomic_rmw_set;
    data_index = dnx_data_field_features_define_hitbit_volatile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_hitbit_volatile_set;
    data_index = dnx_data_field_features_define_fem_replace_msb_instead_lsb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_fem_replace_msb_instead_lsb_set;
    data_index = dnx_data_field_features_define_dir_ext_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_dir_ext_epmf_set;
    data_index = dnx_data_field_features_define_hashing_process_lsb_to_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_hashing_process_lsb_to_msb_set;
    data_index = dnx_data_field_features_define_exem_age_flush_scan;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_exem_age_flush_scan_set;
    data_index = dnx_data_field_features_define_epmf_has_lookup_enabler;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_epmf_has_lookup_enabler_set;
    data_index = dnx_data_field_features_define_epmf_outlif_profile_map;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_epmf_outlif_profile_map_set;
    data_index = dnx_data_field_features_define_epmf_inlif_profile_map;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_epmf_inlif_profile_map_set;
    data_index = dnx_data_field_features_define_tcam_full_key_half_payload_both_lsb_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_tcam_full_key_half_payload_both_lsb_msb_set;
    data_index = dnx_data_field_features_define_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_set;
    data_index = dnx_data_field_features_define_ecmp_tunnel_priority;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_ecmp_tunnel_priority_set;
    data_index = dnx_data_field_features_define_result_signal_exists;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_result_signal_exists_set;
    data_index = dnx_data_field_features_define_apptype_based_on_fwd2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_apptype_based_on_fwd2_set;
    data_index = dnx_data_field_features_define_udh_base_cs_is_mapped;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_udh_base_cs_is_mapped_set;
    data_index = dnx_data_field_features_define_state_collection_engine;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_state_collection_engine_set;
    data_index = dnx_data_field_features_define_empf_trap_action_is_strength_based;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_empf_trap_action_is_strength_based_set;

    

    
    
    submodule_index = dnx_data_field_submodule_signal_sizes;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_signal_sizes_define_dual_queue_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_signal_sizes_dual_queue_size_set;
    data_index = dnx_data_field_signal_sizes_define_packet_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_signal_sizes_packet_header_size_set;
    data_index = dnx_data_field_signal_sizes_define_ecn;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_signal_sizes_ecn_set;
    data_index = dnx_data_field_signal_sizes_define_congestion_info;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_signal_sizes_congestion_info_set;
    data_index = dnx_data_field_signal_sizes_define_parsing_start_offset_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_signal_sizes_parsing_start_offset_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_dnx_data_internal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_dnx_data_internal_define_lr_eth_is_da_mac_valid;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_dnx_data_internal_lr_eth_is_da_mac_valid_set;

    

    
    
    submodule_index = dnx_data_field_submodule_tests;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_tests_define_learn_info_actions_structure_change;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_learn_info_actions_structure_change_set;
    data_index = dnx_data_field_tests_define_ingress_time_stamp_increased;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_ingress_time_stamp_increased_set;
    data_index = dnx_data_field_tests_define_ace_debug_signals_exist;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_ace_debug_signals_exist_set;
    data_index = dnx_data_field_tests_define_epmf_debug_rely_on_tcam0;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_epmf_debug_rely_on_tcam0_set;
    data_index = dnx_data_field_tests_define_epmf_tm_port_cs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_epmf_tm_port_cs_set;
    data_index = dnx_data_field_tests_define_small_exem_addr_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_small_exem_addr_bits_set;
    data_index = dnx_data_field_tests_define_stub_adapter_supports_semantic_flush;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_stub_adapter_supports_semantic_flush_set;
    data_index = dnx_data_field_tests_define_parsing_start_offset_exists;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tests_parsing_start_offset_exists_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

